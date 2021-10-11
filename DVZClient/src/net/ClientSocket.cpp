#include "client/net/ClientSocket.hpp"

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <spdlog/spdlog.h>
#include <iostream>

using namespace DVZ::Net;

ClientSocket* ClientSocket::callbackInstance = nullptr;

void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
	SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp();
	spdlog::info(pszMsg);
	if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug)
	{
		//spdlog::error("Steam networking bug: {}", pszMsg);
	}
}

ClientSocket::ClientSocket(){
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
		spdlog::critical("GameNetworkingSockets_Init failed: {}", errMsg);
	}

	SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, DebugOutput);

	socketInterface = SteamNetworkingSockets();
}

ClientSocket::ClientSocket(const std::string& ip) : ClientSocket(){
	connectToServer(ip);
}

ClientSocket::~ClientSocket() {
	if (isValid() && connection != 0) {
		socketInterface->CloseConnection(connection, 0, nullptr, true);
	}
}

bool ClientSocket::connectToServer(const std::string& ip) {
	SteamNetworkingIPAddr serverAddr;
	serverAddr.Clear();
	bool is_valid_ip = serverAddr.ParseString(std::string{ ip }.c_str());

	if (is_valid_ip) {
		SteamNetworkingConfigValue_t opt;
		opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusChangedCallback);
		connection = socketInterface->ConnectByIPAddress(serverAddr, 1, &opt);
		if (connection == k_HSteamNetConnection_Invalid) {
			spdlog::error("Failed to connect");
		}
		else {
			connectionState = ConnectionState::CONNECTING;
		}
	}
	else {
		spdlog::error("Invalid IP format: {}", ip);
	}

	return isValid();
}

void ClientSocket::pollIncommingMessages(){
	pollConnectionStateChanges();

	if (connectionState == ConnectionState::CONNECTED) {
		while (true) {
			ISteamNetworkingMessage* message = nullptr;
			int numMsgs = socketInterface->ReceiveMessagesOnConnection(connection, &message, 1);
			if (numMsgs == 0) {
				break;
			}
			if (numMsgs < 0) {
				spdlog::error("Cannot read message");
			}

			//std::string_view data{(char*)message->m_pData, (size_t)message->m_cbSize};
			//spdlog::info("Message received: {}", data);
			message->Release();
		}
	}
}

bool ClientSocket::isValid() const {
	return socketInterface != nullptr;
}

ConnectionState ClientSocket::getConnectionState() const {
	if (isValid()) {
		return connectionState;
	}
	return ConnectionState::INVALID;
}

void ClientSocket::onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info) {
	switch (info->m_info.m_eState) {
	case k_ESteamNetworkingConnectionState_None:
		break;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
			// Note: we could distinguish between a timeout, a rejected connection,
			// or some other transport problem.
			spdlog::error("Connection failed");
			connectionState = ConnectionState::CONNECTION_FAILED;
		}
		else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
			spdlog::error("Connection lost");
			connectionState = ConnectionState::CONNECTION_FAILED;
		}
		else {
			// NOTE: We could check the reason code for a normal disconnection
			spdlog::info("Server disconnected us");
			connectionState = ConnectionState::DISCONNECTED;
		}

		socketInterface->CloseConnection(connection, 0, nullptr, true);

		break;
	case k_ESteamNetworkingConnectionState_Connecting:
		// We will get this callback when we start connecting.
		spdlog::info("Starting to connect");
		connectionState = ConnectionState::CONNECTING;
		break;
	case k_ESteamNetworkingConnectionState_Connected:
		spdlog::info("Connected to server");
		connectionState = ConnectionState::CONNECTED;
		break;
	default:
		// Silences -Wswitch
		break;
	}
}

void ClientSocket::connectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* info) {
	callbackInstance->onConnectionStatusChanged(info);
}

void ClientSocket::pollConnectionStateChanges() {
	callbackInstance = this;
	socketInterface->RunCallbacks();
}