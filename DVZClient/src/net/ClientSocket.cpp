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
	spdlog::error(pszMsg);
	if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug)
	{
		//spdlog::error("Steam networking bug: {}", pszMsg);
	}
}

ClientSocket::ClientSocket(std::string_view ip) {
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
		spdlog::critical("GameNetworkingSockets_Init failed: {}", errMsg);

	SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, DebugOutput);

	socketInterface = SteamNetworkingSockets();

	SteamNetworkingIPAddr serverAddr;
	serverAddr.Clear();
	bool is_valid_ip = serverAddr.ParseString(std::string{ip}.c_str());

	if (is_valid_ip) {
		spdlog::info("Connecting to server at [{}]", ip);
		SteamNetworkingConfigValue_t opt;
		opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusChangedCallback);
		connection = socketInterface->ConnectByIPAddress(serverAddr, 1, &opt);
		if (connection == k_HSteamNetConnection_Invalid) {
			spdlog::error("Failed to connect");
		}
	}
	else {
		spdlog::error("Invalid IP format: {}", ip);
	}
}

ClientSocket::~ClientSocket() {
	socketInterface->CloseConnection(connection, 0, nullptr, true); 
}

void ClientSocket::pollIncommingMessages(){
	pollConnectionStateChanges();

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
		}
		else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
			spdlog::error("Connection lost");
		}
		else {
			// NOTE: We could check the reason code for a normal disconnection
			spdlog::info("Server disconnected us");
		}

		socketInterface->CloseConnection(connection, 0, nullptr, true);
		connection = k_HSteamNetConnection_Invalid;
		break;
	case k_ESteamNetworkingConnectionState_Connecting:
		// We will get this callback when we start connecting.
		spdlog::info("Starting to connect");

		break;
	case k_ESteamNetworkingConnectionState_Connected:
		spdlog::info("Connected to server");
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