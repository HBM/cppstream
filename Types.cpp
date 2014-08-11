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
		void timeInfo_t::set(const Json::Value& timeObject)
		{
			try {
				if(timeObject["type"]=="ntp") {
					ntpStamp.fraction = timeObject["fraction"].asUInt();
					ntpStamp.seconds = timeObject["seconds"].asUInt();
				}
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
			}
		}

		void timeInfo_t::clear()
		{
			ntpStamp.fraction = 0;
			ntpStamp.seconds = 0;
		}
	}
}
