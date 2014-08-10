#include <vector>
#include <stdexcept>
#include <iostream>
#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif

#include "Types.h"

namespace hbm {
	namespace streaming {
		void timeInfo_t::set(const Json::Value& params)
		{
			try {
				const Json::Value& ntpStampNode = params["stamp"];
				if(ntpStampNode["type"]=="ntp") {
					ntpStamp.fraction = ntpStampNode["fraction"].asUInt();
					ntpStamp.seconds = ntpStampNode["seconds"].asUInt();
				}
				epoch = params["epoch"].asString();
				scale = params["scale"].asString();
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
			}
		}

		void timeInfo_t::clear()
		{
			ntpStamp.fraction = 0;
			ntpStamp.seconds = 0;
			scale.clear();
			epoch.clear();
		}
	}
}
