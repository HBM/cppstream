#include <fstream>
#include <vector>
#include <iostream>
#include <memory>
#include <signal.h>
#include <unordered_map>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#ifdef _WIN32
#include "json/value.h"
#include "json/writer.h"
#else
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#endif

#include "StreamClient.h"


typedef boost::ptr_vector < hbm::streaming::StreamClient > streams_t;
typedef std::unordered_map < std::string , size_t > receivedDataByteCounts_t;

static receivedDataByteCounts_t receivedDataByteCounts;
static streams_t streams;

/// additional handling of stream related meta information goes in here
/// all signals that become available at any time are being subscribed
void customStreamMetaCb(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)
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


void customSignalMetaCb(hbm::streaming::StreamClient& stream, int signalNumber, const std::string& method, const Json::Value& params)
{
	std::cout << stream.address() << "." << signalNumber << " " << method << " " << Json::FastWriter().write(params) << std::endl;
}


/// we simply accumulate the amount of bytes received in measured data packages.
void customDataCb(hbm::streaming::StreamClient& stream, unsigned int signalId, const unsigned char* pData, size_t size)
{
	receivedDataByteCounts[stream.address()] += size;
}

static void sigHandler(int)
{
	for(streams_t::iterator iter = streams.begin(); iter!=streams.end(); ++iter) {
		(*iter).stop();
	}
}

int main(int argc, char* argv[])
{
	// some signals should lead to a normal shutdown
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	boost::thread_group threads;

	static const std::string& fileName("Addresses.txt");
	if(argc>1) {
		std::cout << "opens daq stream of addresses defined in file " << fileName << std::endl;
		return EXIT_SUCCESS;
	}

	std::ifstream file(fileName);
	if(!file) {
		std::cerr << "file '" << fileName << "' not found!" << std::endl;
		return -1;
	}

	std::string address;
	while (std::getline(file, address))
	{
		// we dump into a file!
		std::string dumpFileName;
		dumpFileName = address + ".dump";
		hbm::streaming::StreamClient* streamPtr = new hbm::streaming::StreamClient(dumpFileName);
		//hbm::streaming::StreamClient* streamPtr = new hbm::streaming::StreamClient;

		streamPtr->setStreamMetaCb(customStreamMetaCb);
		streamPtr->setSignalMetaCb(customSignalMetaCb);
		streamPtr->setDataCb(customDataCb);

		boost::thread* pStreamer = new boost::thread(boost::bind(&hbm::streaming::StreamClient::start, streamPtr, address, hbm::streaming::DAQSTREAM_PORT, "http"));
		threads.add_thread(pStreamer);
		streams.push_back(streamPtr);
	}

	threads.join_all();

	for(receivedDataByteCounts_t::iterator iter = receivedDataByteCounts.begin(); iter!=receivedDataByteCounts.end(); ++iter) {
		std::cout << "received " << iter->second << " bytes of measured data from " << iter->first << std::endl;
	}

	std::cout << "finished!" << std::endl;
}

