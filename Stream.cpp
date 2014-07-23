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
#include "Signal.h"
#include "Types.h"

using namespace hbm::streaming;

/// alle information goes in here.
typedef std::unordered_map < unsigned int, Signal > signals_t;


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

	timeInfo_t startTime;
	signals_t signalProperties;


	/// signal references of all available signals
	std::set < std::string > availables;


	unsigned char dataRecvBuffer[1024];

	TransportHeader transportHeader(streamSocket);
	do {
		result = transportHeader.receive();
		if(result<0) {
			break;
		}
		int type = transportHeader.type();
		size_t size = transportHeader.size();
		unsigned int signalNumber = transportHeader.signalNumber();

		if(type==TYPE_DATA) {
			// read measured data
			result = streamSocket.receiveComplete(dataRecvBuffer, size);
			if(result!=size) {
				break;
			}
			signalProperties[signalNumber].dataCb(dataRecvBuffer, result);
		} else if(type==TYPE_META){
			MetaInformation metaInformation(streamSocket, size);
			if(metaInformation.type()!=METAINFORMATION_JSON) {
				std::cout << "unhandled meta information of type " << metaInformation.type() << std::endl;
			} else {
				const Json::Value& content = metaInformation.jsonContent();
				std::string method = content["method"].asString();

				if(signalNumber==0) {
					// stream related meta information
					if(method=="apiVersion") {
						apiVersion = content[PARAMS][0].asString();
					} else if(method=="init") {
						streamId = content[PARAMS]["streamId"].asString();
						std::cout << "this is: " << streamId << std::endl;
						const Json::Value& supported = content[PARAMS]["supported"];
						for (Json::ValueConstIterator iter = supported.begin(); iter!= supported.end(); ++iter) {
							const Json::Value& element = *iter;
							std::cout << Json::StyledWriter().write(element) << std::endl;
						}

						const Json::Value& commandInterfaces = content[PARAMS]["commandInterfaces"];
						for (Json::ValueConstIterator iter = commandInterfaces.begin(); iter!= commandInterfaces.end(); ++iter) {
							const Json::Value& element = *iter;
							std::cout << Json::StyledWriter().write(element) << std::endl;
						}
					} else if(method=="time") {
						startTime.set(content[PARAMS]);
					} else if(method=="available") {
						// all signals that become available at any time are being subscribed
						signalReferences_t signalReferences;
						for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
							const Json::Value& element = *iter;
							availables.insert(element.asString());
							signalReferences.push_back(element.asString());
						}
						Controller controller(streamId, address.c_str(), port);
						controller.subscribe(signalReferences);
					} else if(method=="unavailable") {
						signalReferences_t signalReferences;
						for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
							const Json::Value& element = *iter;
							availables.erase(element.asString());
						}
						Controller controller(streamId, address.c_str(), port);
						controller.unsubscribe(signalReferences);
					} else if(method=="alive") {
						// ignore!
					} else if(method=="fill") {
						unsigned int fill = content[PARAMS][0].asUInt();
						if(fill>25) {
							std::cout << "fill=" << content[PARAMS][0].asUInt() << "%" << std::endl;
						}
					} else {
						std::cout << "unhandled stream related meta information: " << Json::StyledWriter().write(content) << std::endl;
					}
				} else {
					// signal related meta information
					if(method=="subscribe") {
						std::string signalReference = content[PARAMS][0].asString();
						signalProperties[signalNumber].signalReference = signalReference;
						std::cout << "subscribed signal number= " << signalNumber << " with signal reference '" << signalReference << "'" << std::endl;
					} else if(method=="unsubscribe") {
						std::string signalReference = content[PARAMS][0].asString();
						signalProperties.erase(signalNumber);
						std::cout << "unsubscribed signal number= " << signalNumber << " with signal reference '" << signalReference << "'" << std::endl;
					} else if(method=="time") {
						signalProperties[signalNumber].startTime.set(content[PARAMS]);
					} else if(method=="data") {
						signalProperties[signalNumber].dataFormat(content[PARAMS]);
					} else if(method=="signalRate") {
						signalProperties[signalNumber].signalRate = content[PARAMS];
					} else {
						std::cout << "unhandled signal related meta information for signal " << signalNumber << ": " << Json::StyledWriter().write(content) << std::endl;
					}
				}
			}
		}
	} while(true);

	return EXIT_SUCCESS;
}


