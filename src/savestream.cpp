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

/// signal number is the key
typedef std::unordered_map < unsigned int, std::unique_ptr < std::ofstream > > files_t;

/// receives data from DAQ Stream Server. Subscribes/Unsubscribes signals
static hbm::streaming::StreamClient streamClient;

/// handles signal related meta information and measured data.
static hbm::streaming::SignalContainer signalContainer;


static std::ofstream streamMetaFile;
static files_t signalMetaFiles;
static files_t signalDataFiles;

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
			std::cout << __FUNCTION__ << "the following signal(s) were subscribed: ";
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
	}
}

static void signalMetaInformationCb(hbm::streaming::SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& params)
{
	unsigned int signalNumber = subscribedSignal.signalNumber();
	if(method=="subscribe") {
		std::string signalMetaFilename = "signalMeta_"+subscribedSignal.signalReference()+".dump";
		std::unique_ptr < std::ofstream > pSignalMetaFile(new std::ofstream);
		pSignalMetaFile->open(signalMetaFilename);
		*pSignalMetaFile << method << ": " << Json::FastWriter().write(params) << std::endl;
		signalMetaFiles.emplace(signalNumber, std::move(pSignalMetaFile));

		std::string signalDataFilename = "signalData_"+subscribedSignal.signalReference()+".dump";
		std::unique_ptr < std::ofstream > pSignalDataFile(new std::ofstream);
		pSignalDataFile->open(signalDataFilename);


		signalDataFiles.emplace(signalNumber, std::move(pSignalDataFile));
	} else if(method=="unsubscribe") {
		signalMetaFiles.erase(signalNumber);
		signalDataFiles.erase(signalNumber);
		return;
	} else {
		files_t::iterator iter = signalMetaFiles.find(signalNumber);
		if(iter==signalMetaFiles.end()) {
			return;
		}
		*iter->second << method << ": " << Json::FastWriter().write(params) << std::endl;
	}
}


static void dataCb(hbm::streaming::SubscribedSignal& subscribedSignal, uint64_t ntpTimestamp, const double* pValues, size_t count)
{
	files_t::iterator iter = signalDataFiles.find(subscribedSignal.signalNumber());
	if(iter==signalDataFiles.end()) {
		return;
	}

	*iter->second << std::hex << ntpTimestamp << std::dec << " ";
	for (size_t i=0; i<count; ++i) {
		*iter->second << *pValues << " ";
		++pValues;
	}
	*iter->second << std::endl;
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

	static const std::string streamMetaFilename = "streamMeta.dump";
	streamMetaFile.open(streamMetaFilename);

	signalContainer.setDataCb(dataCb);
	signalContainer.setSignalMetaCb(signalMetaInformationCb);

	streamClient.setStreamMetaCb(streamMetaInformationCb);
	streamClient.setSignalContainer(&signalContainer);

	// connect to the daq stream service and give control to the receiving function.
	// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
	streamClient.start(argv[1], hbm::streaming::DAQSTREAM_PORT, controlPort);
	return EXIT_SUCCESS;
}
