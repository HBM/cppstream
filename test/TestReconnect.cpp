#include <iostream>
#include <string>
#include <cstdlib> //atoi

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif
#include "Stream.h"

static size_t receivedDataByteCount;


/// additional handling of stream related meta information goes in here
/// all signals that become available at any time are being subscribed
void customStreamMetaCb(hbm::streaming::Stream& stream, const std::string& method, const Json::Value params)
{
	if(method=="available") {
		hbm::streaming::signalReferences_t signalReferences;
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			signalReferences.push_back(element.asString());
		}
		stream.subscribe(signalReferences);
	}
}


void customSignalMetaCb(hbm::streaming::Stream& stream, int signalNumber, const std::string& method, const Json::Value params)
{
	//	std::cout << signalNumber << " " << method << std::endl;
}


/// we simply accumulate the amount of bytes received in measured data packages.
void customDataCb(hbm::streaming::Stream& stream, unsigned int signalId, const unsigned char* pData, size_t size)
{
	receivedDataByteCount += size;
}


/// periodically connects to a daq stream, subscribes all signals and disconnects after a specified time
int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address> < control port (default is http) > <cycle time in ms (default is 3000)>" << std::endl;
		return EXIT_SUCCESS;
	}

	hbm::streaming::Stream stream;



	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}

	boost::chrono::milliseconds cycleTime(3000);

	if(argc>3) {
		cycleTime = boost::chrono::milliseconds(atoi(argv[3]));
	}


	stream.setCustomStreamMetaCb(customStreamMetaCb);
	stream.setCustomSignalMetaCb(customSignalMetaCb);
	stream.setCustomDataCb(customDataCb);

	do {
		receivedDataByteCount = 0;
		boost::thread streamer = boost::thread(boost::bind(&hbm::streaming::Stream::start, boost::ref(stream), argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort));
		std::cout << "Started" << std::endl;
		boost::this_thread::sleep_for(cycleTime);
		stream.stop();
		streamer.join();
		std::cout << "Stopped! Received " << receivedDataByteCount << " byte of measured data" << std::endl;
	} while(true);
}
