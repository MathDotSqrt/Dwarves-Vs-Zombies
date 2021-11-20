#include "server/net/ServerSocket.hpp"

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <spdlog/spdlog.h>

using namespace DVZ::Net;

ServerSocket* ServerSocket::callbackInstance = nullptr;

void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
	SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp();
	spdlog::debug(pszMsg);
	if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug)
	{
		//spdlog::error("Steam networking bug: {}", pszMsg);
	}
}

ServerSocket::ServerSocket(u16 port) {
	
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
		spdlog::critical("GameNetworkingSockets_Init failed: {}", errMsg);

	SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Warning, DebugOutput);

	socketsInterface = SteamNetworkingSockets();
	SteamNetworkingIPAddr serverLocalAddr;
	serverLocalAddr.Clear();
	serverLocalAddr.m_port = port;
	SteamNetworkingConfigValue_t opt;
	opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)connectionStatusCallback);

	listenSock = socketsInterface->CreateListenSocketIP(serverLocalAddr, 1, &opt);
	if (listenSock == k_HSteamListenSocket_Invalid) {
		spdlog::critical("Failed to listen on port {}", port);
	}

	pollGroup = socketsInterface->CreatePollGroup();
	if (pollGroup == k_HSteamNetPollGroup_Invalid) {
		spdlog::critical("Failed to listen on port {}", port);
	}

	spdlog::info("Listening on port: {}", port);

}

ServerSocket::~ServerSocket() {
	socketsInterface->CloseListenSocket(listenSock);
}

void ServerSocket::pollIncomingMessages(const std::function<void(std::string_view, HSteamNetConnection)>& func) {
	pollConnectionStateChanges();

	while (true) {
		ISteamNetworkingMessage* message = nullptr;
		int num_messages = socketsInterface->ReceiveMessagesOnPollGroup(pollGroup, &message, 1);
		if (num_messages == 0) {
			break;
		}

		if (num_messages < 0) {
			spdlog::error("Error polling incoming message");
		}

		if (message) {
			std::string_view data{ (char*)message->m_pData, (size_t)message->m_cbSize };
			func(data, message->m_conn);
			message->Release();
		}
	}
}

void ServerSocket::sendMessage(std::string_view sv, HSteamNetConnection conn) {
	const auto iter = connections.find(conn);
	if (iter != connections.end()) {
		const ConnectionState& state = iter->second;
		if (state == ConnectionState::CONNECTED) {
			socketsInterface->SendMessageToConnection(conn, sv.data(), (uint32)sv.size(), k_nSteamNetworkingSend_Reliable, nullptr);
		}
	}
}

void ServerSocket::sendToAllMessage(std::string_view data, HSteamNetConnection except) {
	for (const auto& connection : connections) {
		const auto& [connection_id, state] = connection;
		if (connection_id != except && state == ConnectionState::CONNECTED) {
			socketsInterface->SendMessageToConnection(connection_id, data.data(), (uint32)data.size(), k_nSteamNetworkingSend_Reliable, nullptr);
		}
	}
}

void ServerSocket::onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info) {
	HSteamNetConnection connection = info->m_hConn;
	ConnectionState state = ConnectionState::INVALID;

	switch (info->m_info.m_eState) {
	case k_ESteamNetworkingConnectionState_None:
		//We get callback when we destroy connections
		break;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		spdlog::info("Connection closed");

		if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
			//Something bad probably happened here
			//spdlog::error("Connection failed");
			state = ConnectionState::CONNECTION_FAILED;
		}
		else if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
			// Note: we could distinguish between a timeout, a rejected connection,
			// or some other transport problem.
			spdlog::error("Connection failed");
			state = ConnectionState::CONNECTION_FAILED;
		}
		else if (info->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
			spdlog::error("Connection lost");
			state = ConnectionState::CONNECTION_FAILED;
		}
		else {
			// NOTE: We could check the reason code for a normal disconnection
			spdlog::info("Server disconnected us");
			state = ConnectionState::DISCONNECTED;
		}
		socketsInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
		break;
	case k_ESteamNetworkingConnectionState_Connecting:
		spdlog::info("New client connecting...");
		if (socketsInterface->AcceptConnection(info->m_hConn) == k_EResultOK) {
			if (!socketsInterface->SetConnectionPollGroup(info->m_hConn, pollGroup)) {
				socketsInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
				spdlog::error("Failed to set poll group");
				state = ConnectionState::CONNECTION_FAILED;
			}
			state = ConnectionState::CONNECTING;
		}
		else {
			spdlog::info("Failed to accept connection");
			socketsInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
			state = ConnectionState::CONNECTION_FAILED;
		}
		
		break;
	case k_ESteamNetworkingConnectionState_Connected:
		spdlog::info("Connection accepted");
		state = ConnectionState::CONNECTED;
		// We will get a callback immediately after accepting the connection.
		// Since we are the server, we can ignore this, it's not news to us.
		break;
	default:
		// Silences -Wswitch
		break;
	}

	connections[connection] = state;
}

void ServerSocket::connectionStatusCallback(SteamNetConnectionStatusChangedCallback_t* info) {
	callbackInstance->onConnectionStatusChanged(info);
}

void ServerSocket::pollConnectionStateChanges() {
	callbackInstance = this;
	socketsInterface->RunCallbacks();
}