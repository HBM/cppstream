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
static streams_t streams;

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

		boost::thread* pStreamer = new boost::thread(boost::bind(&hbm::streaming::StreamClient::start, streamPtr, address, hbm::streaming::DAQSTREAM_PORT, "http"));
		threads.add_thread(pStreamer);
		streams.push_back(streamPtr);
	}

	threads.join_all();
	std::cout << "finished!" << std::endl;
}

