#include <iostream>
#include <string>
#include <signal.h>

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif
#include "Stream.h"
#include "Types.h"

static std::string streamId;
static std::string controlPort;
static hbm::streaming::Stream stream;

void customStreamMetaCb(hbm::streaming::Stream& stream, const std::string& method, const Json::Value params)
{
	// additional handling of meta information goes in here

	// all signals that become available at any time are being subscribed
	if(method=="available") {
		hbm::streaming::signalReferences_t signalReferences;
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			signalReferences.push_back(element.asString());
		}
		stream.subscribe(signalReferences);

		std::cout << __FUNCTION__ << "the following signal(s) were subscribed: ";
		for(hbm::streaming::signalReferences_t::const_iterator iter=signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
			std::cout << "'" << *iter << "' ";
		}
		std::cout << std::endl;
	}
}


void customSignalMetaCb(hbm::streaming::Stream& stream, int signalNumber, const std::string& method, const Json::Value params)
{
	std::cout << __FUNCTION__ << ": " << signalNumber << " " << method << std::endl;
}

static void sigHandler(int)
{
	stream.stop();
}

int main(int argc, char* argv[])
{
	// some signals should lead to a normal shutdown
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

	// we want to be informed about stream related meta information
	stream.setCustomStreamMetaCb(customStreamMetaCb);
	// we want to be informed about signal related meta information
	stream.setCustomSignalMetaCb(customSignalMetaCb);

	// give control to the receiving function.
	return stream.start(argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort);
}
