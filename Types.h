#ifndef _HBM__STREAMING__TYPES
#define _HBM__STREAMING__TYPES

//#include <cstdlib>
//#include <vector>
//#include <set>
//#include <iostream>
//#include <unordered_map>

#ifdef _WIN32
#include "jsoncpp/include/json/reader.h"
//#include "jsoncpp/include/json/writer.h"
#else
#include <jsoncpp/json/reader.h>
//#include <jsoncpp/json/writer.h>
#endif

//#include "SocketNonblocking.h"
//#include "TransportHeader.h"
//#include "MetaInformation.h"
//#include "Controller.h"
//#include "Signal.h"

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
#endif
