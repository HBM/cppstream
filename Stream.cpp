#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>

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


struct ntptime_t {
	unsigned int seconds;
	unsigned int fraction;
	std::string scale;
	std::string epoch;
};


int main(int argc, char* argv[])
{
	std::string address = "hbm-00087b";
	hbm::SocketNonblocking streamSocket;
	int result = streamSocket.connect(address.c_str(), STREAM_DATA_PORT);
	if(result<0) {
		return EXIT_FAILURE;
	}

	std::string apiVersion;
	std::string streamId;
	Json::Value supported;
	ntptime_t startTime;

	std::set < std::string > availables;

	do {
		hbm::streaming::TransportHeader transportHeader(streamSocket);
		int type = transportHeader.type();
		size_t size = transportHeader.size();

		if(type==hbm::streaming::TYPE_DATA) {

		} else if(type==hbm::streaming::TYPE_META){
			hbm::streaming::MetaInformation metaInformation(streamSocket, size);
			const Json::Value& content = metaInformation.jsonContent();
			std::string method = content["method"].asString();

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
				const Json::Value& stampNode = content[PARAMS]["stamp"];
				if(stampNode["type"]=="ntp")
				std::cout << Json::StyledWriter().write(content) << std::endl;
				startTime.fraction = stampNode["fraction"].asUInt();
				startTime.seconds = stampNode["seconds"].asUInt();
				startTime.epoch = content[PARAMS]["epoch"].asString();
				startTime.scale = content[PARAMS]["scale"].asString();
			} else if(method=="available") {
				hbm::streaming::signalReferences_t signalReferences;
				for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
					const Json::Value& element = *iter;
					availables.insert(element.asString());
					signalReferences.push_back(element.asString());
				}

				hbm::streaming::Controller controller(streamId, address.c_str(), "http");
				controller.subscribe(signalReferences);
			} else if(method=="unavailable") {
				hbm::streaming::signalReferences_t signalReferences;
				for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
					const Json::Value& element = *iter;
					availables.erase(element.asString());
				}
				hbm::streaming::Controller controller(streamId, address.c_str(), "http");
				controller.unsubscribe(signalReferences);
			} else if(method=="subscribe") {
				std::cout << Json::StyledWriter().write(content) << std::endl;
			} else if(method=="alive") {
				std::cout << "alive!" << std::endl;
			} else if(method=="fill") {
				std::cout << "fill=" << content["params"][0].asUInt() << std::endl;

			// signal related meta information
			} else {
				std::cout << Json::StyledWriter().write(content) << std::endl;
			}

		}
	} while(true);

	return EXIT_SUCCESS;
}
