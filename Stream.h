#ifndef _HBM__STREAMING__STREAM_H
#define _HBM__STREAMING__STREAM_H

#include <string>
#include <set>
#include <unordered_map>

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif

#include "Types.h"
#include "SubscribedSignal.h"

namespace hbm {
	namespace streaming {
		typedef std::set < std::string > availableSignals_t;

		/// signal number is the key
		typedef std::unordered_map < unsigned int, SubscribedSignal > subscribedSignals_t;

		class Stream {
		public:
			Stream(const std::string& address);

			/// connects to a streaming server and processes all received data
			int execute(const std::string& controlPort);

		private:

			/// handle stream related meta information
			void metaCb(const std::string& method, const Json::Value& params);

			std::string m_address;

			std::string m_apiVersion;
			std::string m_streamId;

			/// initial time received when opening the stream
			timeInfo_t m_initialTime;

			/// signal references of all available signals
			availableSignals_t m_availableSignals;

			/// information about all subscribed signals
			subscribedSignals_t m_signalProperties;
		};
	}
}
#endif // _HBM__STREAMING__STREAM_H
