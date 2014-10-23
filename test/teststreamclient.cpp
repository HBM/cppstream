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

#include <json/value.h>

#include "streamclient/streamclient.h"
#include "streamclient/timeinfo.h"
#include "teststreamclient.h"
#include "../lib/signalextract.h"
#include "../lib/endianess.h"

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

BOOST_AUTO_TEST_CASE(test_time)
{
	Json::Value timeObject;
	uint32_t eraReq = 1;
	uint32_t secondsReq = 2;
	uint32_t fractionReq = 3;
	uint32_t subFractionReq = 4;
	timeObject["type"] = "ntp";
	timeObject["era"] = eraReq;
	timeObject["seconds"] = secondsReq;
	timeObject["fraction"] = fractionReq;
	timeObject["subFraction"] = subFractionReq;

	hbm::streaming::timeInfo_t timeInfo;
	timeInfo.set(timeObject);
	BOOST_CHECK(eraReq == timeInfo.era());
	BOOST_CHECK(secondsReq == timeInfo.seconds());
	BOOST_CHECK(fractionReq == timeInfo.fraction());
	BOOST_CHECK(subFractionReq == timeInfo.subFraction());

	hbm::streaming::timeInfo_t timeDiff;

	timeDiff.setTimestamp(8);

	for(unsigned int i=0;i<10;++i) {
		BOOST_CHECK(eraReq == timeInfo.era());
		BOOST_CHECK(secondsReq == timeInfo.seconds());
		BOOST_CHECK(fractionReq == timeInfo.fraction());
		BOOST_CHECK((subFractionReq+(i*8)) == timeInfo.subFraction());
		timeInfo.increment(timeDiff);
	}
}

BOOST_AUTO_TEST_CASE(test_signalextract)
{
	{
		u_int32_t source = 0x01020304;

		unsigned char buffer[sizeof(source)];
		memcpy(buffer, &source, sizeof(buffer));
		unsigned char* pBuffer = buffer;
		u_int32_t result = hbm::streaming::extract<uint32_t, hbm::streaming::big>()(&pBuffer);
		BOOST_CHECK(result==0x04030201);
	}


	{
		u_int64_t source = 0x0102030405060708;
		unsigned char buffer[8];
		memcpy(buffer, &source, sizeof(buffer));
		unsigned char* pBuffer = buffer;
		u_int64_t result = hbm::streaming::extract<uint64_t, hbm::streaming::big>()(&pBuffer);
		BOOST_CHECK(result==0x0807060504030201);
	}

	{
		uint64_t sourceUpperHalf = 0x0102030405060708;
		uint64_t sourceLowerHalf = 0x090A0B0C0D0E0F10;
		boost::multiprecision::uint128_t source = sourceUpperHalf;
		source <<= 64;
		source |= sourceLowerHalf;
		unsigned char buffer[sizeof(source)];
		memcpy(buffer, &source, sizeof(source));
		unsigned char* pBuffer = buffer;
		boost::multiprecision::uint128_t result = hbm::streaming::extract<boost::multiprecision::uint128_t, hbm::streaming::big>()(&pBuffer);

		uint64_t resultLowerHalf = static_cast < uint64_t > (result & 0xffffffffffffffff);
		uint64_t resultUpperHalf = static_cast < uint64_t > (result >> 64);


		BOOST_CHECK(resultLowerHalf==be64toh(sourceUpperHalf));
		BOOST_CHECK(resultUpperHalf==be64toh(sourceLowerHalf));
	}
}

BOOST_AUTO_TEST_SUITE_END()
