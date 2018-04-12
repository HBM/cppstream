// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _WIN32
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN


#define BOOST_TEST_MODULE Scan Client Test

#include <iostream>
#include <stdexcept>

#include <boost/test/unit_test.hpp>

#include "streamclient/streamclient.h"
#include "streamclient/timeinfo.h"
#include "streamclient/deltatimeinfo.h"
#include "teststreamclient.h"


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

		params["stamp"]["type"] = "ntp";
		params["stamp"]["seconds"] = 8;

		timeInfo.set(params);
	}
	timestamp = timeInfo.ntpTimeStamp();
	BOOST_CHECK(timestamp==0x0800000000);

	timeInfo.clear();
	timestamp = timeInfo.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);

	{
		Json::Value params;

		params["stamp"]["type"] = "ntp";
		params["stamp"]["era"] = 3;

		timeInfo.set(params);
	}
	BOOST_CHECK(timeInfo.era()==3);
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

	hbm::streaming::deltaTimeInfo deltaTime;
	uint64_t timestamp;

	{
		Json::Value params;

		params["samples"] = 1;
		params["delta"]["type"] = "ntp";
		params["delta"]["seconds"] = 1;

		deltaTime.setDelta(params);
	}


	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	deltaTime.increment(1);
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0100000000));
	deltaTime.increment(2);
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0300000000));


	deltaTime.clear();
	{
		Json::Value params;

		params["samples"] = 1;
		params["delta"]["type"] = "ntp";
		params["delta"]["subFraction"] = 0x10000000;

		deltaTime.setDelta(params);
	}
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	timestamp = deltaTime.increment(16);
	BOOST_CHECK(timestamp==(0x01));

	deltaTime.clear();
	{
		Json::Value params;

		params["samples"] = 2;
		params["delta"]["type"] = "ntp";
		params["delta"]["subFraction"] = 0x20000000;

		deltaTime.setDelta(params);
	}
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	deltaTime.increment(16);
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x01));


	deltaTime.clear();
	{
		Json::Value params;

		params["samples"] = 2;
		params["delta"]["type"] = "ntp";
		params["delta"]["seconds"] = 1;

		deltaTime.setDelta(params);
	}
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==0);
	deltaTime.increment(4);
	timestamp = deltaTime.ntpTimeStamp();
	BOOST_CHECK(timestamp==(0x0200000000));
}
