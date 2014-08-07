#include <fstream>
#include <vector>
#include <iostream>
#include <memory>
#include <signal.h>
#include <unordered_map>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Stream.h"


typedef boost::ptr_vector < hbm::streaming::Stream > streams_t;
typedef std::unordered_map < std::string , size_t > receivedDataByteCounts_t;

static receivedDataByteCounts_t receivedDataByteCounts;
static streams_t streams;

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
		hbm::streaming::Stream* streamPtr = new hbm::streaming::Stream;

		streamPtr->setCustomStreamMetaCb(customStreamMetaCb);
		streamPtr->setCustomSignalMetaCb(customSignalMetaCb);
		streamPtr->setCustomDataCb(customDataCb);

		boost::thread* pStreamer = new boost::thread(boost::bind(&hbm::streaming::Stream::start, streamPtr, address, hbm::streaming::DAQSTREAM_PORT, "http"));
		threads.add_thread(pStreamer);
		streams.push_back(streamPtr);
	}

	boost::chrono::milliseconds cycleTime(30000);

	boost::this_thread::sleep_for(cycleTime);

	for(streams_t::iterator iter = streams.begin(); iter!=streams.end(); ++iter) {
		(*iter).stop();
	}

	threads.join_all();

	for(receivedDataByteCounts_t::iterator iter = receivedDataByteCounts.begin(); iter!=receivedDataByteCounts.end(); ++iter) {
		std::cout << "received " << iter->second << " bytes of measured data from " << iter->first << std::endl;
	}

}

