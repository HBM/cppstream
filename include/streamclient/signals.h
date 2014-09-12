#ifndef _HBM__STREAMING__SIGNALS
#define _HBM__STREAMING__SIGNALS

#include <unordered_map>
#include <vector>

#include <boost/function.hpp>

#include <json/value.h>

#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {

		typedef std::function<void(SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& params)> SignalMetaCb_t;

		/// contains all subscribed signals.
		class Signals {
		public:
			Signals();

			/// \warning set callback function before calling start(), otherwise you will miss meta information received.
			void setSignalMetaCb(SignalMetaCb_t cb);

			/// \warning set callback function before subscribing signals, otherwise you will miss measured values received.
			void setDataCb(DataCb_t cb);

			/// new subscribed signals are added with arival of initial meta information
			void processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len);
			void processMetaInformation(unsigned int signalNumber, const std::string &method, const Json::Value& params);
			void clear();

		private:

			/// signal number is the key
			typedef std::unordered_map < unsigned int, SubscribedSignal > subscribedSignals_t;

			/// processes measured data and keeps meta information about all subscribed signals
			subscribedSignals_t m_subscribedSignals;

			SignalMetaCb_t m_signalMetaCb;
			DataCb_t m_dataCb;
		};
	}
}
#endif
