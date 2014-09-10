#include <iostream>
#include <string>
#include <signal.h>

#include "streamclient/streamclient.h"
#include "streamclient/types.h"

/// this object represents
static hbm::streaming::StreamClient stream;

static void sigHandler(int)
{
	stream.stop();
}

static void streamMetaInformationCb(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)
{
	if (method=="available") {
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
	} else if(method=="unavailable") {

		std::cout << __FUNCTION__ << "the following signal(s) are not available anyore: ";

		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			std::cout << element.asString() << ", ";
		}
		std::cout << std::endl;
	} else if(method=="alive") {
		// We do ignore this. We are using TCP keep alive in order to detect communication problems.
	} else if(method=="fill") {
		if(params.empty()==false) {
			unsigned int fill = params[0u].asUInt();
			if(fill>25) {
				std::cout << stream.address() << ": ring buffer fill level is " << params[0u].asUInt() << "%" << std::endl;
			}
		}
	}
}

static void signalMetaInformationCb(hbm::streaming::SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& params)
{
	std::cout << subscribedSignal.signalReference() << ": " << method << std::endl;
}

int main(int argc, char* argv[])
{
	// Some signals should lead to a normal shutdown of the daq stream client. Afterwards the program exists.
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	if ((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address>" << std::endl;
		return EXIT_SUCCESS;
	}

	// the control port might differ when communication runs via a router (CX27)
	std::string controlPort = "http";
	if (argc>2) {
		controlPort = argv[2];
	}

	stream.setStreamMetaCb(streamMetaInformationCb);
	stream.setSignalMetaCb(signalMetaInformationCb);

	// connect to the daq stream service and give control to the receiving function.
	// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
	stream.start(argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort);
	return EXIT_SUCCESS;
}
