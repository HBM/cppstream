#include <iostream>
#include <string>
#include <cstdlib> //atoi

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#ifdef _WIN32
#include "json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif
#include "StreamClient.h"



/// periodically connects to a daq stream, subscribes all signals and disconnects after a specified time
int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address> < control port (default is http) > <cycle time in ms (default is 3000)>" << std::endl;
		return EXIT_SUCCESS;
	}

	hbm::streaming::StreamClient stream;



	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}

	boost::chrono::milliseconds cycleTime(3000);

	if(argc>3) {
		cycleTime = boost::chrono::milliseconds(atoi(argv[3]));
	}


	do {
		boost::thread streamer = boost::thread(boost::bind(&hbm::streaming::StreamClient::start, &stream, argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort));
		std::cout << "Started" << std::endl;
		boost::this_thread::sleep_for(cycleTime);
		stream.stop();
		streamer.join();
	} while(true);
}
