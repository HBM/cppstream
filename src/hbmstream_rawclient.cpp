// Copyright 2018 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <iomanip>
#include <iostream>
#include <string>
#include <signal.h>

#include <json/writer.h>
#include <json/value.h>

#include "streamclient/streamclient.h"
#include "streamclient/signalcontainer.h"
#include "streamclient/types.h"


/// receives data from DAQ Stream Server. Subscribes/Unsubscribes signals
static hbm::streaming::StreamClient streamClient;

/// handles signal related meta information and measured data.
static hbm::streaming::SignalContainer signalContainer;

static void sigHandler(int)
{
	streamClient.stop();
}

static void streamMetaInformationCb(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)
{
	if (method == hbm::streaming::META_METHOD_AVAILABLE) {
		std::cout << "The following signal(s) are available : ";
		// simply subscibe all signals that become available.
		hbm::streaming::signalReferences_t signalReferences;
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			std::cout << element.asString() << ", ";
			signalReferences.push_back(element.asString());
		}
		std::cout << std::endl << std::endl;

		try {
			stream.subscribe(signalReferences);
			std::cout << "The following " << signalReferences.size() << " signal(s) were subscribed: ";
		} catch(const std::runtime_error& e) {
			std::cerr << "Error '" << e.what() << "' subscribing the following signal(s): ";
		}

		for(hbm::streaming::signalReferences_t::const_iterator iter=signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
			std::cout << "'" << *iter << "' ";
		}
		std::cout << std::endl << std::endl;

	} else if(method==hbm::streaming::META_METHOD_UNAVAILABLE) {
		std::cout << "The following signal(s) are not available anyore: ";

		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			std::cout << element.asString() << ", ";
		}
		std::cout << std::endl << std::endl;

	} else if(method==hbm::streaming::META_METHOD_ALIVE) {
		// We do ignore this. We are using TCP keep alive in order to detect communication problems.
	} else if(method==hbm::streaming::META_METHOD_FILL) {
		if(params.empty()==false) {
			unsigned int fill = params[0u].asUInt();
			if(fill>25) {
				std::cout << stream.address() << ": ring buffer fill level is " << params[0u].asUInt() << "%" << std::endl;
			}
		}
	} else {
		std::cout << "Meta information: " << method << " " << Json::FastWriter().write(params) << std::endl;
	}
}

static void signalMetaInformationCb(hbm::streaming::SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& )
{
	std::cout << subscribedSignal.signalReference() << ": " << method << std::endl;
}

static void rawCbVerbose(hbm::streaming::SubscribedSignal& subscribedSignal, uint64_t ntpTimestamp, const uint8_t* pValues, size_t count)
{
	std::cout << subscribedSignal.signalReference() << ": " << std::hex << ntpTimestamp << " ";
	//std::cout << std::setw(2);
	std::cout << std::setfill('0');
	for (size_t i=0; i<count; ++i) {
		std::cout << std::setw(2) << (int)*pValues << " ";
		++pValues;
	}
	std::cout << std::dec << std::endl;
}


static void dataCbQuiet(hbm::streaming::SubscribedSignal& , uint64_t , const uint8_t* , size_t )
{
}

int main(int argc, char* argv[])
{
	// Some signals should lead to a normal shutdown of the daq stream client. Afterwards the program exists.
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	if ((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <stream server address> [ -q]" << std::endl;
		std::cout << "use option -q to print meta data only" << std::endl;
		return EXIT_SUCCESS;
	}
	
	if ((argc>=3) && (std::string(argv[2]) == "-q")) {
		signalContainer.setDataAsRawCb(dataCbQuiet);
	} else {
		signalContainer.setDataAsRawCb(rawCbVerbose);
	}

	signalContainer.setSignalMetaCb(signalMetaInformationCb);

	streamClient.setStreamMetaCb(streamMetaInformationCb);
	streamClient.setSignalContainer(&signalContainer);

	// connect to the daq stream service and give control to the receiving function.
	// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
	streamClient.start(argv[1], hbm::streaming::DAQSTREAM_PORT);
	return EXIT_SUCCESS;
}
