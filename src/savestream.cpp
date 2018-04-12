// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <memory>
#include <unordered_map>
#include <iostream>
#include <string>
#include <signal.h>
#include <fstream>

#include <json/writer.h>

#include "streamclient/streamclient.h"
#include "streamclient/signalcontainer.h"
#include "streamclient/types.h"

class signalInfo_t {
public:
	signalInfo_t(const std::string& sinalReference)
		: dataFile("signalData_"+sinalReference+".dump")
		, dataFileNextLine(1)
		, metaInfoFile("signalMetaInfo_"+sinalReference+".dump")
	{
	}

	/// meta inforamtion is written into file. It is preceded by the next line in the corresponding data file.
	void addMetaInfo(const std::string& method, const Json::Value& params)
	{
		metaInfoFile << dataFileNextLine << ": " << method << " " << Json::FastWriter().write(params) << std::endl;
	}

	void addData(uint64_t ntpTimestamp, const double* pValues, size_t count)
	{
		dataFile << std::hex << ntpTimestamp << std::dec << " ";
		for (size_t i=0; i<count; ++i) {
			dataFile << *pValues << " ";
			++pValues;
		}
		dataFile << std::endl;
		++dataFileNextLine;
	}

private:
	std::ofstream dataFile;
	off_t dataFileNextLine;
	std::ofstream metaInfoFile;
};

/// signal number is the key
typedef std::unordered_map < unsigned int, std::unique_ptr < signalInfo_t > > signalInfos_t;

/// receives data from DAQ Stream Server. Subscribes/Unsubscribes signals
static hbm::streaming::StreamClient streamClient;

/// handles signal related meta information and measured data.
static hbm::streaming::SignalContainer signalContainer;


static std::ofstream streamMetaFile;
static signalInfos_t signalFiles;

static void sigHandler(int)
{
	streamClient.stop();
}

static void streamMetaInformationCb(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)
{
	streamMetaFile << method << ": " << Json::FastWriter().write(params) << std::endl;
	if (method == hbm::streaming::META_METHOD_AVAILABLE) {
		// simply subscibe all signals that become available.
		hbm::streaming::signalReferences_t signalReferences;
		for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
			const Json::Value& element = *iter;
			signalReferences.push_back(element.asString());
		}

		try {
			stream.subscribe(signalReferences);
			std::cout << "The following " << signalReferences.size() << " signal(s) were subscribed: ";
			for(hbm::streaming::signalReferences_t::const_iterator iter=signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
				std::cout << "'" << *iter << "' ";
			}
			std::cout << std::endl;
		} catch(const std::runtime_error& e) {
			std::cerr << __FUNCTION__ << " error '" << e.what() << "' subscribing the following signal(s): ";
		}
	} else if(method==hbm::streaming::META_METHOD_UNAVAILABLE) {
		std::cout << __FUNCTION__ << " the following signal(s) is(are) not available anymore: ";

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
	}
}

static void signalMetaInformationCb(hbm::streaming::SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& params)
{
	unsigned int signalNumber = subscribedSignal.signalNumber();
	if(method=="subscribe") {
		std::unique_ptr < signalInfo_t > pSignalInfo(new signalInfo_t(subscribedSignal.signalReference()));
		pSignalInfo->addMetaInfo(method, params);
		signalFiles.insert(std::pair < unsigned int, std::unique_ptr < signalInfo_t > > (signalNumber, std::move(pSignalInfo)));
	} else if(method=="unsubscribe") {
		signalFiles.erase(signalNumber);
	} else {
		signalInfos_t::iterator iter = signalFiles.find(signalNumber);
		if(iter==signalFiles.end()) {
			std::cerr << "received meta information for signal that is not known to be subscribed" << std::endl;
			return;
		}
		iter->second->addMetaInfo(method, params);
	}
}

static void dataCb(hbm::streaming::SubscribedSignal& subscribedSignal, uint64_t ntpTimestamp, const double* pValues, size_t count)
{
	signalInfos_t::iterator iter = signalFiles.find(subscribedSignal.signalNumber());
	if(iter==signalFiles.end()) {
		return;
	}

	iter->second->addData(ntpTimestamp, pValues,  count);
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

	static const std::string streamMetaFilename = "streamMeta.dump";
	streamMetaFile.open(streamMetaFilename);

	signalContainer.setDataAsDoubleCb(dataCb);
	signalContainer.setSignalMetaCb(signalMetaInformationCb);

	streamClient.setStreamMetaCb(streamMetaInformationCb);
	streamClient.setSignalContainer(&signalContainer);

	// connect to the daq stream service and give control to the receiving function.
	// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
	streamClient.start(argv[1], hbm::streaming::DAQSTREAM_PORT);
	return EXIT_SUCCESS;
}
