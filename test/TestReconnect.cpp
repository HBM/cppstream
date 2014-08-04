#include <iostream>
#include <string>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif
#include "Stream.h"

static std::string streamId;
static std::string controlPort;


void customStreamMetaCb(hbm::streaming::Stream& stream, const std::string& method, const Json::Value params)
{
	// additional handling of meta information goes in here

	// all signals that become available at any time are being subscribed
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


int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address>" << std::endl;
		return EXIT_SUCCESS;
	}

	hbm::streaming::Stream stream(argv[1]);

	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}

	stream.setCustomStreamMetaCb(customStreamMetaCb);
	stream.setCustomSignalMetaCb(customSignalMetaCb);

	static const boost::chrono::milliseconds CYCLETIME(3000);
	do {
		boost::thread streamer = boost::thread(boost::bind(&hbm::streaming::Stream::start, boost::ref(stream), hbm::streaming::STREAM_DATA_PORT, controlPort));
		std::cout << "started" << std::endl;
		std::cout << "waiting some time (" << CYCLETIME.count() << "ms)" << std::endl;
		boost::this_thread::sleep_for(CYCLETIME);
		stream.stop();
		streamer.join();
		std::cout << "stopped" << std::endl;
	} while(true);
}
