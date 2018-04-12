// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <signal.h>
#include <thread>
#include <vector>

#include <json/writer.h>
#include <json/value.h>

#include "streamclient/streamclient.h"
#include "streamclient/signalcontainer.h"
#include "streamclient/types.h"

static std::vector < std::string > split(std::string text, char separator)
{
	std::vector < std::string > result;

	size_t pos_start=0;

	while(1)
	{
		size_t pos_end = text.find(separator, pos_start);
		std::string token = text.substr(pos_start, pos_end-pos_start);
		result.push_back(token);
		if(pos_end == std::string::npos) break;
		pos_start = pos_end+1;
	}
	return result;
}


struct Client {
	Client()
		: streamClient()
		, signalContainer()
		, worker()
	{
	}

	/// receives data from DAQ Stream Server. Subscribes/Unsubscribes signals
	hbm::streaming::StreamClient streamClient;
	/// handles signal related meta information and measured data.
	hbm::streaming::SignalContainer signalContainer;
	
	std::thread worker;
};

typedef std::vector <Client* > Clients;

static bool stop = false;
static Clients clients;

static void sigHandler(int)
{
	for (auto iter=clients.begin(); iter!=clients.end(); ++iter) {
		(*iter)->streamClient.stop();
	}
	stop = true;
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
			std::cout << stream.address() << ":"<< stream.streamPort() << ": The following " << signalReferences.size() << " signal(s) were subscribed: ";
		} catch(const std::runtime_error& e) {
			std::cerr << stream.address() << ":"<< stream.streamPort() << ": error '" << e.what() << "' subscribing the following signal(s): ";
		}

		for(hbm::streaming::signalReferences_t::const_iterator iter=signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
			std::cout << "'" << *iter << "' ";
		}
		std::cout << std::endl;
	} else if(method==hbm::streaming::META_METHOD_UNAVAILABLE) {

		std::cout << stream.address() << ":"<< stream.streamPort() << ": the following signal(s) are not available anyore: ";

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
				std::cout << stream.address() << ":"<< stream.streamPort() << ": ring buffer fill level is " << params[0u].asUInt() << "%" << std::endl;
			}
		}
	} else {
		std::cout << stream.address() << ":"<< stream.streamPort() << ": " << method << " " << Json::FastWriter().write(params) << std::endl;
	}
}

static void signalMetaInformationCb(hbm::streaming::SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& )
{
	std::cout << subscribedSignal.signalReference() << ": " << method << std::endl;
}

// does nothing because we want to be as fast as possible
static void dataCbQuiet(hbm::streaming::SubscribedSignal&, uint64_t, const double*, size_t)
{
}


/// Reads a file with the addresses of devices to connect to.
/// For devices behind CX27 streaming and http port can be provided.
/// <addresses>:[<streaming port>:<http port>]
int main(int argc, char* argv[])
{
	// Some signals should lead to a normal shutdown of the daq stream client. Afterwards the program exists.
	signal( SIGTERM, &sigHandler);
	signal( SIGINT, &sigHandler);

	if ((argc!=2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <address file>" << std::endl;
		return EXIT_SUCCESS;
	}
	std::ifstream addressFile;
	addressFile.open(argv[1]);
	if (!addressFile) {
		std::cerr << "could not open configuration file '" << argv[1] << "'" << std::endl;
		return EXIT_FAILURE;
	}
	std::string line;
	
	std::string address;
	std::string controlPort;
	std::string streamPort;

	while(std::getline(addressFile, line)) {
		// filter carriage return. Important when running under linux and using configuration file from windows.
		std::vector < std::string > tokensCarriageReturn = split(line, '\r');
		std::vector < std::string > tokens = split(tokensCarriageReturn[0], ':');
		if (tokens.size()==3) {
			address = tokens[0];
			if (address.find('#') == std::string::npos ) {
				controlPort = tokens[1];
				streamPort = tokens[2];
			
				Client* client(new(Client));
				client->signalContainer.setDataAsDoubleCb(dataCbQuiet);
				client->signalContainer.setSignalMetaCb(signalMetaInformationCb);
			
				client->streamClient.setStreamMetaCb(streamMetaInformationCb);
				client->streamClient.setSignalContainer(&client->signalContainer);
			
				// connect to the daq stream service and give control to the receiving function.
				// returns on signal (terminate, interrupt) buffer overrun on the server side or loss of connection.
				client->worker = std::thread(std::bind(&hbm::streaming::StreamClient::start, &client->streamClient, address, streamPort, controlPort));
				//int start(const std::string& address, const std::string &streamPort = DAQSTREAM_PORT, const std::string& controlPort = "");
				
				clients.push_back(client);
			}
		}
	}
	
	while(stop==false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return EXIT_SUCCESS;
}
