#include <fstream>
#include <vector>
#include <iostream>
#include <memory>
#include <signal.h>
#include <functional>

#include <boost/thread/thread.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "streamclient/streamclient.h"


typedef boost::ptr_vector < hbm::streaming::StreamClient > streams_t;
typedef std::vector < hbm::streaming::SignalContainer > signalcontainers_t;
static streams_t streams;
static signalcontainers_t signalcontainers;

static void sigHandler(int)
{
	for(streams_t::iterator iter = streams.begin(); iter!=streams.end(); ++iter) {
		(*iter).stop();
	}
}

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


int main(int argc, char** )
{
	// Some signals should lead to a normal shutdown of the daq stream client. Afterwards the program exists.
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	boost::thread_group threads;

	static const std::string& fileName("Addresses.txt");
	if(argc>1) {
		std::cout << "Opens daq stream of addresses defined in file (one address per line) " << fileName << std::endl;
		std::cout << "Data received from stream is dumped into one for each daq stream server <address>.dump" << std::endl;
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
		streamPtr->setStreamMetaCb(streamMetaInformationCb);

		boost::thread* pStreamer = new boost::thread(std::bind(&hbm::streaming::StreamClient::start, streamPtr, address, hbm::streaming::DAQSTREAM_PORT));
		threads.add_thread(pStreamer);
		streams.push_back(streamPtr);
	}

	threads.join_all();
	std::cout << "finished!" << std::endl;
}

