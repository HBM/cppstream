// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <set>
#include <iostream>
#include <string>
#include <functional>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdlib> //atoi


#include "streamclient/streamclient.h"

typedef std::set < std::string > signalreferences_t;

static signalreferences_t signalReferences;
static std::mutex signalReferenceMtx;


static void streamMetaInformationCb(hbm::streaming::StreamClient&, const std::string& method, const Json::Value& params)
{	
	if (method==hbm::streaming::META_METHOD_AVAILABLE) {
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			std::lock_guard < std::mutex > lock(signalReferenceMtx);
			const Json::Value& element = *iter;
			signalReferences.insert(element.asString());
		}
	} else if (method==hbm::streaming::META_METHOD_UNAVAILABLE) {
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			std::lock_guard < std::mutex > lock(signalReferenceMtx);
			const Json::Value& element = *iter;
			std::string signalreference = element.asString();
			signalReferences.erase(signalreference);
		}
	}
}


int main(int argc, char* argv[])
{
	std::chrono::milliseconds cycleTime(3000);

	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "connects to a daq stream. periodically subscribes and unsubscribes all signals" << std::endl;
		std::cout << "syntax: " << argv[0] << " <stream server address> <cycle time in ms (default is " << cycleTime.count() << ")>" << std::endl;
		return EXIT_SUCCESS;
	}

	static const std::string address = argv[1];

	if(argc>2) {
		cycleTime = std::chrono::milliseconds(atoi(argv[2]));
	}

	hbm::streaming::StreamClient stream;
	stream.setStreamMetaCb(streamMetaInformationCb);
	std::thread streamer = std::thread(std::bind(&hbm::streaming::StreamClient::start, &stream, address, hbm::streaming::DAQSTREAM_PORT, ""));
	std::this_thread::sleep_for(cycleTime);
	
	do {
		hbm::streaming::signalReferences_t subscribed;
		
		std::cout << "subscribing..." << std::endl;
		{
			std::lock_guard < std::mutex > lock(signalReferenceMtx);
			for (signalreferences_t::const_iterator iter = signalReferences.begin(); iter!= signalReferences.end(); ++iter) {
				subscribed.push_back(*iter);
			}
		}
		
		stream.subscribe(subscribed);
		//std::this_thread::sleep_for(cycleTime);
		stream.unsubscribe(subscribed);
		std::cout << "unsubscribing..." << std::endl;
		
	} while(true);
	stream.stop();
	streamer.join();
}
