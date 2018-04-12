// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _HBM__STREAMING__SIGNALS
#define _HBM__STREAMING__SIGNALS

#include <string>
#include <unordered_map>
#include <functional>

#include <json/value.h>

#include "subscribedsignal.h"


#ifdef _WIN32
#define ssize_t int
#endif

namespace hbm {
	namespace streaming {

		typedef std::function<void(SubscribedSignal& subscribedSignal, const std::string& method, const Json::Value& params)> SignalMetaCb_t;

		/// Contains all subscribed signals.
		/// Callback functions may be registered in order to get informed about signal related meta information and measured data.
		class SignalContainer {
		public:
			SignalContainer();

			/// \warning set callback function before calling start(), otherwise you will miss meta information received.
			void setSignalMetaCb(SignalMetaCb_t cb);

			/// \warning set callback function before subscribing signals, otherwise you will miss measured values received.
			void setDataAsDoubleCb(DataAsDoubleCb_t cb);

			/// \warning set callback function before subscribing signals, otherwise you will miss measured values received.
			void setDataAsRawCb(DataAsRawCb_t cb);

			/// new subscribed signals are added with arrival of initial meta information
			/// \throw std::runtime_error
			void processMetaInformation(unsigned int signalNumber, const std::string &method, const Json::Value& params);

			/// \return number of bytes processed or -1 if signal is unknown.
			ssize_t processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len);

			void clear();

		private:
			SignalContainer(const SignalContainer& op);
			SignalContainer& operator=(const SignalContainer& op);

			/// signal number is the key
			typedef std::unordered_map < unsigned int, SubscribedSignal > signals_t;

			/// processes measured data and keeps meta information about all subscribed signals
			signals_t m_subscribedsignals;

			SignalMetaCb_t m_signalMetaCb;
			DataAsDoubleCb_t m_dataAsDoubleCb;
			DataAsRawCb_t m_dataAsRawCb;
		};
	}
}
#endif
