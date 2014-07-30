#ifndef _HBM__STREAMING__STREAM_H
#define _HBM__STREAMING__STREAM_H

#include <string>
#include <set>
#include <unordered_map>
#include <functional>

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif

#include "Types.h"
#include "SubscribedSignal.h"
#include "SocketNonblocking.h"


namespace hbm {
	namespace streaming {

		class Stream;

		/// signal number is the key
		typedef std::unordered_map < unsigned int, SubscribedSignal > subscribedSignals_t;

		typedef std::function<void(hbm::streaming::Stream& stream, const std::string& method, const Json::Value& params)> StreamMetaCb_t;
		typedef std::function<void(hbm::streaming::Stream& stream, int signalNumber, const std::string& method, const Json::Value& params)> SignalMetaCb_t;

		class Stream {
		public:
			Stream(const std::string& address);

			void setCustomStreamMetaCb(StreamMetaCb_t cb);

			void setCustomSignalMetaCb(SignalMetaCb_t cb);

			int subscribe(const signalReferences_t& signalReferences);

			int unsubscribe(const signalReferences_t& signalReferences);

			/// connects to a streaming server and processes all received data
			int start(const std::string& controlPort);

			/// closes the stream socket.
			void stop();

		private:
			typedef std::set < std::string > availableSignals_t;

			/// handle stream related meta information
			int metaCb(const std::string& method, const Json::Value& params);

			hbm::SocketNonblocking m_streamSocket;

			std::string m_address;

			std::string m_apiVersion;
			std::string m_streamId;
			std::string m_controlPort;

			/// initial time received when opening the stream
			timeInfo_t m_initialTime;

			/// signal references of all available signals
			availableSignals_t m_availableSignals;

			/// information about all subscribed signals
			subscribedSignals_t m_signalProperties;

			StreamMetaCb_t m_customStreamMetaCb;
			SignalMetaCb_t m_customSignalMetaCb;
		};
	}
}
#endif // _HBM__STREAMING__STREAM_H
