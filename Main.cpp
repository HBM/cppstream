#include <iostream>
#include "Stream.h"

int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address>" << std::endl;
		return EXIT_SUCCESS;
	}

	hbm::streaming::Stream stream(argv[1]);

	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if(argc>2) {
		controlPort = argv[2];
	}

	return stream.execute(controlPort);
}
