#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>

#ifdef _WIN32
#include "jsoncpp/include/json/reader.h"
#include "jsoncpp/include/json/writer.h"
#else
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#endif

#include "SocketNonblocking.h"
#include "TransportHeader.h"
#include "MetaInformation.h"
#include "Controller.h"

static const std::string STREAM_DATA_PORT = "7411";
static const char* PARAMS = "params";



struct ntpTimeStamp_t {
	unsigned int seconds;
	unsigned int fraction;
};

struct timeInfo_t {
	void set(const Json::Value& params)
	{
		const Json::Value& stampNode = params["stamp"];
		if(stampNode["type"]=="ntp") {
			stamp.fraction = stampNode["fraction"].asUInt();
			stamp.seconds = stampNode["seconds"].asUInt();
		}
		epoch = params["epoch"].asString();
		scale = params["scale"].asString();
	}

	ntpTimeStamp_t stamp;
	std::string scale;
	std::string epoch;
};

struct signalProperties_t {
	std::string signalReference;
	timeInfo_t startTime;
	Json::Value signalRate;
	Json::Value data;
};


typedef std::unordered_map < unsigned int, signalProperties_t > signals_t;


int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
			std::cout << "syntax: " << argv[0] << " <stream source address>" << std::endl;
			return EXIT_SUCCESS;
	}

	std::string port = "http";
	std::string address = argv[1];
	if(argc>2) {
		port = argv[2];
	}

	hbm::SocketNonblocking streamSocket;
	int result = streamSocket.connect(address.c_str(), STREAM_DATA_PORT);
	if(result<0) {
		return EXIT_FAILURE;
	}

	std::string apiVersion;
	std::string streamId;
	Json::Value supported;
	timeInfo_t startTime;
	signals_t signalProperties;

	std::set < std::string > availables;

	do {
		hbm::streaming::TransportHeader transportHeader(streamSocket);
		int type = transportHeader.type();
		size_t size = transportHeader.size();
		unsigned int signalNumber = transportHeader.signalNumber();

		if(type==hbm::streaming::TYPE_DATA) {

		} else if(type==hbm::streaming::TYPE_META){
			hbm::streaming::MetaInformation metaInformation(streamSocket, size);
			const Json::Value& content = metaInformation.jsonContent();
			std::string method = content["method"].asString();

			if(signalNumber==0) {
				// stream related meta information
				if(method=="apiVersion") {
					apiVersion = content[PARAMS][0].asString();
				} else if(method=="init") {
					streamId = content[PARAMS]["streamId"].asString();
					supported = content[PARAMS]["supported"];
					const Json::Value& commandInterfaces = content[PARAMS]["commandInterfaces"];
					for (Json::ValueConstIterator iter = commandInterfaces.begin(); iter!= commandInterfaces.end(); ++iter) {
						const Json::Value& element = *iter;
						std::cout << Json::StyledWriter().write(element) << std::endl;
					}
				} else if(method=="time") {
					startTime.set(content[PARAMS]);
				} else if(method=="available") {
					hbm::streaming::signalReferences_t signalReferences;
					for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
						const Json::Value& element = *iter;
						availables.insert(element.asString());
						signalReferences.push_back(element.asString());
					}
					hbm::streaming::Controller controller(streamId, address.c_str(), port);
					controller.subscribe(signalReferences);
				} else if(method=="unavailable") {
					hbm::streaming::signalReferences_t signalReferences;
					for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
						const Json::Value& element = *iter;
						availables.erase(element.asString());
					}
					hbm::streaming::Controller controller(streamId, address.c_str(), port);
					controller.unsubscribe(signalReferences);
				} else if(method=="alive") {
					std::cout << "alive!" << std::endl;
				} else if(method=="fill") {
					std::cout << "fill=" << content["params"][0].asUInt() << std::endl;
				} else {
					std::cout << Json::StyledWriter().write(content) << std::endl;
				}
			} else {
				// signal related meta information
				if(method=="subscribe") {
					signalProperties[signalNumber].signalReference = content[PARAMS][0].asString();
				} else if(method=="unsubscribe") {
					signalProperties.erase(signalNumber);
				} else if(method=="time") {
					signalProperties[signalNumber].startTime.set(content[PARAMS]);
				} else if(method=="data") {
					signalProperties[signalNumber].data = content[PARAMS];
				} else if(method=="signalRate") {
					signalProperties[signalNumber].signalRate = content[PARAMS];
				} else {
					std::cout << "signal number= " << signalNumber << " ";
					std::cout << Json::StyledWriter().write(content) << std::endl;
				}
			}
		}
	} while(true);

	return EXIT_SUCCESS;
}
