#include <cstdlib>
#include <vector>
#include <set>

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include "SocketNonblocking.h"
#include "TransportHeader.h"
#include "MetaInformation.h"

static const std::string STREAM_DATA_PORT = "7411";
static const char* PARAMS = "params";


struct timeMetaInformation_t {
	Json::Value stamp;
	std::string scale;
	std::string epoch;
};


int main(int argc, char* argv[])
{
	std::string address = "hbm-00087b";
	hbm::SocketNonblocking socket;
	int result = socket.connect(address.c_str(), STREAM_DATA_PORT);
	if(result<0) {
		return EXIT_FAILURE;
	}

	std::string apiVersion;
	std::string streamId;
	Json::Value supported;
	Json::Value commandInterfaces;
	timeMetaInformation_t time;

	std::set < std::string > availables;

	do {
		hbm::streaming::TransportHeader transportHeader(socket);
		int type = transportHeader.type();
		size_t size = transportHeader.size();

		if(type==hbm::streaming::TYPE_DATA) {

		} else if(type==hbm::streaming::TYPE_META){
			hbm::streaming::MetaInformation metaInformation(socket, size);
			const Json::Value& content = metaInformation.jsonContent();
			std::string method = content["method"].asString();
			if(method=="apiVersion") {
				apiVersion = content[PARAMS][0].asString();
			} else if(method=="init") {
				streamId = content[PARAMS]["streamId"].asString();
				supported = content[PARAMS]["supported"];
				commandInterfaces = content[PARAMS]["commandInterfaces"];
			} else if(method=="time") {
				std::cout << Json::StyledWriter().write(content) << std::endl;
				time.stamp = content[PARAMS]["stamp"];
				time.epoch = content[PARAMS]["epoch"].asString();
				time.scale = content[PARAMS]["scale"].asString();
//				{
//				  "method": "time",
//				  "params": {
//				    "stamp": <time_object>,
//				    "scale": <string>, // optional, e.g. UTC, TAI, GPS
//				    "epoch": <string> // optional
//				  }
//				}

			} else if(method=="available") {
				for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
					const Json::Value& element = *iter;
					availables.insert(element.asString());
				}
			} else if(method=="unavailable") {
				for (Json::ValueConstIterator iter = content[PARAMS].begin(); iter!= content[PARAMS].end(); ++iter) {
					const Json::Value& element = *iter;
					availables.erase(element.asString());
				}
			} else if(method=="alive") {
				std::cout << "alive!" << std::endl;
			} else if(method=="fill") {
				std::cout << "fill=" << content["params"][0].asUInt() << std::endl;
			} else {
				std::cout << Json::StyledWriter().write(content) << std::endl;
			}

		}
	} while(true);

	return EXIT_SUCCESS;
}
