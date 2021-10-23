#pragma once
#ifndef DVZ_CONNECTION_STATE_HPP
#define DVZ_CONNECTION_STATE_HPP

namespace DVZ::Net {
	enum class ConnectionState {
		CONNECTED,
		CONNECTING,
		CONNECTION_FAILED,
		DISCONNECTED,
		INVALID
	};
}

#endif