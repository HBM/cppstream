// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _WIN32
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN


#define BOOST_TEST_MODULE time Test

//#include <iostream>
//#include <stdexcept>

#include <json/value.h>

#include <boost/test/unit_test.hpp>

//#include "streamclient/streamclient.h"
//#include "streamclient/timeinfo.h"
#include "streamclient/deltatimeinfo.h"
//#include "teststreamclient.h"

BOOST_AUTO_TEST_CASE (test_timeIncrement)
{
	hbm::streaming::deltaTimeInfo dtInfo;
	Json::Value time;
	Json::Value deltaTime;
	uint64_t ntpTime;
	uint64_t expectedNtpTime;

	dtInfo.setTime(time);
	static const uint32_t seconds = 1;
	static const uint32_t fraction = 0x80000000;


	deltaTime["delta"]["type"] = "ntp";
	deltaTime["delta"]["seconds"] = seconds;
	deltaTime["delta"]["fraction"] = fraction;

	dtInfo.setDelta(deltaTime);

	ntpTime = dtInfo.getNtpTimeStamp();
	BOOST_CHECK_EQUAL(ntpTime, 0);
	dtInfo.increment(1);
	ntpTime = dtInfo.getNtpTimeStamp();
	expectedNtpTime = (uint64_t)seconds<<32;
	expectedNtpTime += fraction;
	ntpTime = dtInfo.getNtpTimeStamp();
	BOOST_CHECK_EQUAL(ntpTime, expectedNtpTime);

	dtInfo.increment(1);
	ntpTime = dtInfo.getNtpTimeStamp();
	BOOST_CHECK_EQUAL(ntpTime, expectedNtpTime*2);

	dtInfo.increment(8);
	ntpTime = dtInfo.getNtpTimeStamp();
	BOOST_CHECK_EQUAL(ntpTime, expectedNtpTime*10);
}



BOOST_AUTO_TEST_CASE (test_subFraction)
{
	hbm::streaming::deltaTimeInfo dtInfo;
	Json::Value time;
	Json::Value deltaTime;
	uint64_t ntpTime;
	uint64_t expectedNtpTime;

	dtInfo.setTime(time);
	static const uint32_t subFraction = 0x20000000;


	deltaTime["delta"]["type"] = "ntp";
	deltaTime["delta"]["subFraction"] = subFraction;

	// 8 increments result in overflow to fractions
	dtInfo.setDelta(deltaTime);
	ntpTime = dtInfo.getNtpTimeStamp();
	BOOST_CHECK_EQUAL(ntpTime, 0);
	dtInfo.increment(8);
	ntpTime = dtInfo.getNtpTimeStamp();
	expectedNtpTime = 1;
	BOOST_CHECK_EQUAL(ntpTime, expectedNtpTime);


	// 8 more increments result in another overflow to fractions
	dtInfo.increment(8);
	ntpTime = dtInfo.getNtpTimeStamp();
	expectedNtpTime = 2;
	BOOST_CHECK_EQUAL(ntpTime, expectedNtpTime);
}


