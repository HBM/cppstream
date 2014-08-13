#ifndef _HBM__STREAMING__TESTSTREAMCLIENT_H
#define _HBM__STREAMING__TESTSTREAMCLIENT_H

#include <boost/thread/thread.hpp>

#include "StreamClient.h"

class DeviceFixture {
public:
	DeviceFixture();

	virtual ~DeviceFixture();
protected:
	std::string m_address;
	std::string m_controlPort;
	hbm::streaming::StreamClient m_streamClient;
	boost::thread m_streamer;
};
#endif
