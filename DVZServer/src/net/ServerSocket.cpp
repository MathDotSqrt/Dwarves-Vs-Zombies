#include "server/net/ServerSocket.hpp"

#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <spdlog/spdlog.h>

using namespace DVZ::Net;

ServerSocket* ServerSocket::callbackInstance = nullptr;

ServerSocket::ServerSocket(u16 port) {
	
	SteamDatagramErrMsg errMsg;
	if (!GameNetworkingSockets_Init(nullptr, errMsg))
		spdlog::critical("GameNetworkingSockets_Init failed: {}", errMsg);

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

void ServerSocket::pollIncomingMessages() {
	while (true) {
		ISteamNetworkingMessage* message = nullptr;
		int num_messages = socketsInterface->ReceiveMessagesOnPollGroup(pollGroup, &message, 1);
		if (num_messages == 0) {
			break;
		}

		if (num_messages < 0) {
			spdlog::error("Error polling incoming message");
		}
		std::string_view data{(char*)message->m_pData, (size_t)message->m_cbSize};
		spdlog::info("Message recieved: {}", data);
	}
}

void ServerSocket::onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* info) {
	switch (info->m_info.m_eState) {
	case k_ESteamNetworkingConnectionState_None:
		//We get callback when we destroy connections
		break;
	case k_ESteamNetworkingConnectionState_ClosedByPeer:
	case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
		if (info->m_eOldState == k_ESteamNetworkingConnectionState_Connected){
			//Something bad probably happened here
		}
		socketsInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
		break;
	case k_ESteamNetworkingConnectionState_Connecting:
		spdlog::info("New client connecting...");
		if (socketsInterface->AcceptConnection(info->m_hConn) != k_EVoiceResultOK) {
			socketsInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
			spdlog::info("Cannot accept connection!");
		}

		if (!socketsInterface->SetConnectionPollGroup(info->m_hConn, pollGroup))
		{
			socketsInterface->CloseConnection(info->m_hConn, 0, nullptr, false);
			spdlog::error("Failed to set poll group");
			break;
		}
		
		break;
	case k_ESteamNetworkingConnectionState_Connected:
		// We will get a callback immediately after accepting the connection.
		// Since we are the server, we can ignore this, it's not news to us.
		break;
	default:
		// Silences -Wswitch
		break;
	}
}

void ServerSocket::connectionStatusCallback(SteamNetConnectionStatusChangedCallback_t* info) {
	callbackInstance->onConnectionStatusChanged(info);
}

void ServerSocket::pollConnectionStateChanges() {
	callbackInstance = this;
	socketsInterface->RunCallbacks();
}