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
			/// this is best to be understood if represented as binary or hexadecimal number.
			/// 0x8 does mean half a second.
			/// 0x4 does mean a quarter second
			unsigned int fraction;
		};

		class timeInfo_t {
		public:
			void set(const Json::Value& StampNode);
			void clear();

		private:
			ntpTimeStamp_t ntpStamp;
		};
	}
}
#endif
