#include <iostream>
//#include <string>
//#include <signal.h>

#include <stdexcept>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>


//#ifdef _WIN32
//#include "json/value.h"
//#else
//#include <jsoncpp/json/value.h>
//#endif
#include "StreamClient.h"
//#include "Types.h"

/// this object represents
static hbm::streaming::StreamClient stream;

int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address>" << std::endl;
		return EXIT_SUCCESS;
	}

	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}


	boost::thread streamer = boost::thread(boost::bind(&hbm::streaming::StreamClient::start, &stream, argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort));

	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));

	hbm::streaming::signalReferences_t signalReferences;
	signalReferences.push_back("aninvalidsignalreference");
	try {
		stream.subscribe(signalReferences);
	} catch(const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}


	try {
		stream.unsubscribe(signalReferences);
	} catch(const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}

	stream.stop();
	streamer.join();

}
