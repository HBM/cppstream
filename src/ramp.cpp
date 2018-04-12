// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <iostream>
#include <string>
#include <signal.h>
#include <unordered_map>
#include <cmath>

#include <json/writer.h>
#include <json/value.h>

#include "streamclient/streamclient.h"
#include "streamclient/signalcontainer.h"
#include "streamclient/types.h"

struct lastRampValues {
	uint64_t timeStamp;
	double amplitude;
};

typedef std::unordered_map < unsigned int, lastRampValues > lastRampValues_t;


/// receives data from DAQ Stream Server. Subscribes/Unsubscribes signals
static hbm::streaming::StreamClient streamClient;

/// handles signal related meta information and measured data.
static hbm::streaming::SignalContainer signalContainer;

static double rampValueDiff = 0.1;
static const double epsilon = 0.0000000001;

static lastRampValues_t m_lastRampValues;

static void sigHandler(int)
{
	streamClient.stop();
}

static void streamMetaInformationCb(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)
{
	if (method == hbm::streaming::META_METHOD_AVAILABLE) {
		// simply subscibe all signals that become available.
		hbm::streaming::signalReferences_t signalReferences;
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			signalReferences.push_back(element.asString());
		}

		try {
			stream.subscribe(signalReferences);
			std::cout << __FUNCTION__ << "the following " << signalReferences.size() << " signal(s) were subscribed: ";
		} catch(const std::runtime_error& e) {
			std::cerr << __FUNCTION__ << "error '" << e.what() << "' subscribing the following signal(s): ";
		}

		for(hbm::streaming::signalReferences_t::const_iterator iter=signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
			std::cout << "'" << *iter << "' ";
		}
		std::cout << std::endl;
	} else if(method==hbm::streaming::META_METHOD_UNAVAILABLE) {

		std::cout << __FUNCTION__ << "the following signal(s) are not available anyore: ";

		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			std::cout << element.asString() << ", ";
		}
		std::cout << std::endl;
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
		std::cout << __FUNCTION__ << " " << method << " " << Json::FastWriter().write(params) << std::endl;
	}
}

static void signalMetaInformationCb(hbm::streaming::SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& )
{
	std::cout << subscribedSignal.signalReference() << ": " << method << std::endl;
}


static void dataCb(hbm::streaming::SubscribedSignal& subscribedSignal, uint64_t timeStamp, const double* pValues, size_t count)
{
	unsigned int signalNumber = subscribedSignal.signalNumber();
	lastRampValues_t::iterator iter = m_lastRampValues.find(signalNumber);

	if(iter==m_lastRampValues.end()) {
		lastRampValues lastValues;
		lastValues.timeStamp = timeStamp;
		lastValues.amplitude = pValues[count-1];
		m_lastRampValues.insert(std::make_pair(signalNumber, lastValues));
	} else {
		double valueDiff = pValues[0] - iter->second.amplitude;
		if(fabs(valueDiff - rampValueDiff) > epsilon) {
			throw std::runtime_error (subscribedSignal.signalReference() + ": unexpected value in ramp!");
		} else if (iter->second.timeStamp>=timeStamp){
			throw std::runtime_error (subscribedSignal.signalReference() + ": unexpected time stamp in ramp!");
		} else {
			iter->second.amplitude = pValues[count-1];
		}
	}
}

int main(int argc, char* argv[])
{
	// Some signals should lead to a normal shutdown of the daq stream client. Afterwards the program exists.
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	if ((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "Subscribes all signals that become available." << std::endl;
		std::cout << "Each signal is expected to deliver a ramp with a defined slope." << std::endl;
		std::cout << std::endl;
		std::cout << "syntax: " << argv[0] << " <stream server address> <slope (default is " << rampValueDiff << ")>" << std::endl;
		return EXIT_SUCCESS;
	}

	if (argc==3) {
		char* pEnd;
		const char* pStart = argv[2];
		rampValueDiff = strtod(pStart, &pEnd);
		if (pEnd ==pStart) {
			std::cerr << "invalid slope value. Must be a number" << std::endl;
			return EXIT_FAILURE;
		}
	}

	signalContainer.setDataAsDoubleCb(dataCb);
	signalContainer.setSignalMetaCb(signalMetaInformationCb);

	streamClient.setStreamMetaCb(streamMetaInformationCb);
	streamClient.setSignalContainer(&signalContainer);

	// connect to the daq stream service and give control to the receiving function.
	// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
	streamClient.start(argv[1], hbm::streaming::DAQSTREAM_PORT);
	return EXIT_SUCCESS;
}
