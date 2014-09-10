#ifndef _HBM__STREAMING__SUBSCRIBEDSIGNALS
#define _HBM__STREAMING__SUBSCRIBEDSIGNALS

#include <unordered_map>

#include <jsoncpp/json/value.h>

#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		/// contains all subscribed signals.
		class SubscribedSignals {
		public:
			SubscribedSignals();

			/// new subscribed signals are added with arival of initial meta information
			void processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len);
			void processMetaInformation(unsigned int signalNumber, const std::string &method, const Json::Value& params);
			size_t erase(unsigned int signalNumber);
			void clear();

		private:

			/// signal number is the key
			typedef std::unordered_map < unsigned int, SubscribedSignal > subscribedSignals_t;

			/// processes measured data and keeps meta information about all subscribed signals
			subscribedSignals_t m_subscribedSignals;
		};
	}
}
#endif
