#include <fstream>
#include <vector>
#include <iostream>
#include <memory>
#include <signal.h>
#include <unordered_map>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "streamclient/streamclient.h"


typedef boost::ptr_vector < hbm::streaming::StreamClient > streams_t;
static streams_t streams;

static void sigHandler(int)
{
	for(streams_t::iterator iter = streams.begin(); iter!=streams.end(); ++iter) {
		(*iter).stop();
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

		boost::thread* pStreamer = new boost::thread(boost::bind(&hbm::streaming::StreamClient::start, streamPtr, address, hbm::streaming::DAQSTREAM_PORT, "http"));
		threads.add_thread(pStreamer);
		streams.push_back(streamPtr);
	}

	threads.join_all();
	std::cout << "finished!" << std::endl;
}

