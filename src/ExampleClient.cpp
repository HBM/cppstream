#include <iostream>
#include <string>
#include <signal.h>

#include "StreamClient/StreamClient.h"
#include "StreamClient/Types.h"

/// this object represents
static hbm::streaming::StreamClient stream;

static void sigHandler(int)
{
	stream.stop();
}

int main(int argc, char* argv[])
{
	// Some signals should lead to a normal shutdown of the daq stream client. Afterwards the program exists.
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address>" << std::endl;
		return EXIT_SUCCESS;
	}

	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}

	// give control to the receiving function.
	// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
	stream.start(argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort);
	return EXIT_SUCCESS;
}
