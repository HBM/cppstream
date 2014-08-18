#include <iostream>
#include <string>
#include <cstdlib> //atoi

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include "StreamClient/StreamClient.h"


int main(int argc, char* argv[])
{
	boost::chrono::milliseconds cycleTime(3000);

	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "periodically connects to a daq stream, subscribes all signals and disconnects after a specified time" << std::endl;
		std::cout << "syntax: " << argv[0] << " <stream server address> <control port (default is \"http\")> <cycle time in ms (default is " << cycleTime.count() << ")>" << std::endl;
		return EXIT_SUCCESS;
	}


	static const std::string address = argv[1];

	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}



	if(argc>3) {
		cycleTime = boost::chrono::milliseconds(atoi(argv[3]));
	}

	hbm::streaming::StreamClient stream;
	do {
		boost::thread streamer = boost::thread(boost::bind(&hbm::streaming::StreamClient::start, &stream, address, hbm::streaming::DAQSTREAM_PORT, controlPort));
		std::cout << "Started" << std::endl;
		boost::this_thread::sleep_for(cycleTime);
		stream.stop();
		streamer.join();
	} while(true);
}
