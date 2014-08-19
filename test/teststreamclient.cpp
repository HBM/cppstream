#ifndef _WIN32
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN


#define BOOST_TEST_MODULE Scan Client Test

#include <iostream>
#include <stdexcept>

#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "streamclient/streamclient.h"
#include "teststreamclient.h"

DeviceFixture::DeviceFixture()
	: m_address("hbm-00087b")
	, m_controlPort("http")
{
	m_streamer = boost::thread(boost::bind(&hbm::streaming::StreamClient::start, &m_streamClient, m_address, hbm::streaming::DAQSTREAM_PORT, m_controlPort));
}

DeviceFixture::~DeviceFixture()
{
	m_streamClient.stop();
	m_streamer.join();
}

BOOST_FIXTURE_TEST_SUITE(test, DeviceFixture)

BOOST_AUTO_TEST_CASE (test_subscribe)
{
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

	hbm::streaming::signalReferences_t signalReferences;
	signalReferences.push_back("aninvalidsignalreference");
	BOOST_CHECK_THROW(m_streamClient.subscribe(signalReferences), std::runtime_error);
	BOOST_CHECK_THROW(m_streamClient.unsubscribe(signalReferences), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
