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

		typedef std::function<void(hbm::streaming::Stream& stream, unsigned int signalId, const unsigned char* pData, size_t size)> DataCb_t;
		typedef std::function<void(hbm::streaming::Stream& stream, const std::string& method, const Json::Value& params)> StreamMetaCb_t;
		typedef std::function<void(hbm::streaming::Stream& stream, int signalNumber, const std::string& method, const Json::Value& params)> SignalMetaCb_t;

		class Stream {
		public:
			Stream();

			void setCustomDataCb(DataCb_t cb);

			void setCustomStreamMetaCb(StreamMetaCb_t cb);

			void setCustomSignalMetaCb(SignalMetaCb_t cb);

			int subscribe(const signalReferences_t& signalReferences);

			int unsubscribe(const signalReferences_t& signalReferences);

			std::string address() const
			{
				return m_address;
			}

			/// connects to a streaming server and processes all received data.
			/// Returns when stream is stopped by calling stop() or if loss of connection is recognized.
			/// @param address address of the HBM daq stream server
			/// @param streamPort name or number of the HBM daq stream port. Might differ from default when communication runs via a router (CX27)
			/// @param controlPort name or number of the HBM daq stream control port. Might differ from default when communication runs via a router (CX27)
			int start(const std::string& address, const std::string &streamPort = "7411", const std::string& controlPort = "http");

			/// closes the stream socket.
			void stop();

		private:
			/// signal number is the key
			typedef std::unordered_map < unsigned int, SubscribedSignal > subscribedSignals_t;

			typedef std::set < std::string > availableSignals_t;

			Stream(const Stream&);
			Stream& operator= (const Stream&);

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
			subscribedSignals_t m_subscribedSignals;

			DataCb_t m_customDataCb;
			StreamMetaCb_t m_customStreamMetaCb;
			SignalMetaCb_t m_customSignalMetaCb;
		};
	}
}
#endif // _HBM__STREAMING__STREAM_H
