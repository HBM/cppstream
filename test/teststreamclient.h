#ifndef _TESTSTREAMCLIENT_H
#define _TESTSTREAMCLIENT_H

#include <thread>

#include "streamclient/streamclient.h"

class DeviceFixture {
public:
	DeviceFixture();

	virtual ~DeviceFixture();
protected:
	std::string m_address;
	std::string m_controlPort;
	hbm::streaming::StreamClient m_streamClient;
	std::thread m_streamer;
};
#endif
