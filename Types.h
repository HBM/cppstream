#ifndef _HBM__STREAMING__TYPES
#define _HBM__STREAMING__TYPES

#include <vector>
#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif

namespace hbm {
	static const char PARAMS[] = "params";
	static const char METHOD[] = "method";


	namespace streaming {
		typedef std::vector < std::string > signalReferences_t;

		static const std::string DAQSTREAM_PORT = "7411";
		static const char SERVERPATH[] = "rpc";


		struct ntpTimeStamp_t {
			unsigned int seconds;
			unsigned int fraction;
		};

		class timeInfo_t {
		public:
			void set(const Json::Value& params);
			void clear();

		private:
			ntpTimeStamp_t ntpStamp;
			std::string scale;
			std::string epoch;
		};
	}
}
#endif
