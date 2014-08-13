#ifndef _HBM__STREAMING__STREAMCLIENT_H
#define _HBM__STREAMING__STREAMCLIENT_H

#include <string>
#include <set>
#include <unordered_map>
#include <functional>

#ifdef _WIN32
#include "json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif

#include "Types.h"
#include "SubscribedSignal.h"
#include "SocketNonblocking.h"


namespace hbm {
	namespace streaming {

		class StreamClient;

		typedef std::function<void(hbm::streaming::StreamClient& stream, unsigned int signalId, const unsigned char* pData, size_t size)> DataCb_t;
		typedef std::function<void(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)> StreamMetaCb_t;
		typedef std::function<void(hbm::streaming::StreamClient& stream, int signalNumber, const std::string& method, const Json::Value& params)> SignalMetaCb_t;

		/// Connects to on daq stream server. Receives and interpretes meta data. Subcribes signals. Receives measured data
		/// Callback functions may be registered in order to get informed about meta information and measured data.
		class StreamClient {
		public:
			StreamClient();

			/// use this variant to dump everything received to a file
			/// \throw std::runtime_error if file could not be opened
			StreamClient(const std::string& fileName);

			/// \throws std::runtime_error
			void subscribe(const signalReferences_t& signalReferences);

			/// \throws std::runtime_error
			void unsubscribe(const signalReferences_t& signalReferences);

			std::string address() const
			{
				return m_address;
			}

			/// connects to a streaming server, receives and processes all meta information and data.
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

			StreamClient(const StreamClient&);
			StreamClient& operator= (const StreamClient&);

			/// handle stream related meta information
			int metaCb(const std::string& method, const Json::Value& params);

			hbm::SocketNonblocking m_streamSocket;

			std::string m_address;
			std::string m_httpPath;

			std::string m_apiVersion;
			std::string m_streamId;
			std::string m_controlPort;

			/// initial time received when opening the stream
			timeInfo_t m_initialTime;
			std::string m_initialTimeScale;
			std::string m_initialTimeEpoch;

			/// information about all subscribed signals
			subscribedSignals_t m_subscribedSignals;
			size_t m_receivedDataByteCount;
		};
	}
}
#endif // _HBM__STREAMING__STREAM_H
