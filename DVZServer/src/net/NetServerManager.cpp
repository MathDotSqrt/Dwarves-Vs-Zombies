#include "server/net/NetServerManager.hpp"

using namespace DVZ::Net;

NetServerManager::NetServerManager() : socket(std::make_unique<Net::ServerSocket>(50150)) {
	
} 

ServerSocket& NetServerManager::getSocket() {
	return *socket;
}