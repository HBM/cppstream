// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <cstdlib> //atoi


#include "streamclient/streamclient.h"

static void streamMetaInformationCb(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)
{
	if (method==hbm::streaming::META_METHOD_AVAILABLE) {
		// simply subscibe all signals that become available.
		hbm::streaming::signalReferences_t signalReferences;
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			signalReferences.push_back(element.asString());
		}

		try {
			stream.subscribe(signalReferences);
			std::cout << __FUNCTION__ << "the following signal(s) were subscribed: ";
		} catch(const std::runtime_error& e) {
			std::cerr << __FUNCTION__ << "error '" << e.what() << "' subscribing the following signal(s): ";
		}

		for(hbm::streaming::signalReferences_t::const_iterator iter=signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
			std::cout << "'" << *iter << "' ";
		}
		std::cout << std::endl;
	}
}


int main(int argc, char* argv[])
{
	std::chrono::milliseconds cycleTime(3000);

	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "periodically connects to a daq stream, subscribes all signals and disconnects after a specified time" << std::endl;
		std::cout << "syntax: " << argv[0] << " <stream server address> <cycle time in ms (default is " << cycleTime.count() << ")>" << std::endl;
		return EXIT_SUCCESS;
	}


	static const std::string address = argv[1];



	if(argc>2) {
		cycleTime = std::chrono::milliseconds(atoi(argv[2]));
	}

	hbm::streaming::StreamClient stream;
	stream.setStreamMetaCb(streamMetaInformationCb);
	do {
		std::thread streamer = std::thread(std::bind(&hbm::streaming::StreamClient::start, &stream, address, hbm::streaming::DAQSTREAM_PORT, ""));
		std::cout << "Started" << std::endl;
		std::this_thread::sleep_for(cycleTime);
		stream.stop();
		streamer.join();
	} while(true);
}
