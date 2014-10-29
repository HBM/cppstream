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
#include "streamclient/timeinfo.h"
#include "streamclient/deltatimeinfo.h"
#include "teststreamclient.h"

DeviceFixture::DeviceFixture()
	: m_address("hbm-00087b")
	, m_controlPort("http")
	, m_streamClient()
	, m_streamer(boost::thread(boost::bind(&hbm::streaming::StreamClient::start, &m_streamClient, m_address, hbm::streaming::DAQSTREAM_PORT, m_controlPort)))
{
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


BOOST_AUTO_TEST_CASE (test_timeInfo)
{
//	}
//		”params”: {
//			"type": "ntp",
//			"era": <number>,
//			"seconds": <number>,
//			"fraction": <number>,
//			"subFraction": <number>
//		}
//	}


	hbm::streaming::timeInfo timeInfo;
	uint64_t timestamp;

	{
		Json::Value params;

		params["type"] = "ntp";
		params["seconds"] = 8;

		timeInfo.set(params);
	}
	timestamp = timeInfo.ntpTimeStamp();
	BOOST_CHECK(timestamp==0x0800000000);
}


BOOST_AUTO_TEST_CASE (test_deltaTimeInfo)
{
//	}
//		”params”: {
//			”samples”: <number>,
//			”delta”: {
//				"type": "ntp",
//				"era": <number>,
//				"seconds": <number>,
//				"fraction": <number>,
//				"subFraction": <number>
//			}
//		}
//	}

	hbm::streaming::deltaTimeInfo signalTime;
	uint64_t timestamp;

	{
		Json::Value params;

		params["samples"] = 1;
		params["delta"]["type"] = "ntp";
		params["delta"]["seconds"] = 1;

		signalTime.setSignalRate(params);
	}


	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	signalTime.increment(1);
	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0100000000));
	signalTime.increment(2);
	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0300000000));


	{
		Json::Value params;

		params["samples"] = 1;
		params["delta"]["type"] = "ntp";
		params["delta"]["fraction"] = 0x10000000;

		signalTime.setSignalRate(params);
	}
	signalTime.clear();
	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	signalTime.increment(16);
	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0100000000));

	{
		Json::Value params;

		params["samples"] = 2;
		params["delta"]["type"] = "ntp";
		params["delta"]["seconds"] = 1;

		signalTime.setSignalRate(params);
	}
	signalTime.clear();
	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	signalTime.increment(4);
	timestamp = signalTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0200000000));
}


BOOST_AUTO_TEST_SUITE_END()
