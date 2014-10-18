#ifndef _HBM__STREAMING__STREAMCLIENT_H
#define _HBM__STREAMING__STREAMCLIENT_H

#include <string>
#include <functional>

#include <json/value.h>

#include "signalcontainer.h"
#include "socketnonblocking.h"


namespace hbm {
	namespace streaming {

		class StreamClient;

		typedef std::function<void(hbm::streaming::StreamClient& stream, const std::string& method, const Json::Value& params)> StreamMetaCb_t;

		/// Connects to on daq stream server. Receives and interpretes meta data and measured data. Subcribes and unsubscribes signals.
		/// Callback functions may be registered in order to get informed about stream related meta information.
		class StreamClient {
		public:
			StreamClient();

			/// use this variant to dump everything received to a file
			/// \throw std::runtime_error if file could not be opened
			StreamClient(const std::string& fileName);

			/// \warning set callback function before calling start() otherwise you will miss meta information received.
			void setStreamMetaCb(StreamMetaCb_t cb);

			void setSignalContainer(SignalContainer *pSignalContainer) {
				m_pSignalContainer = pSignalContainer;
			}

			/// \throws std::runtime_error
			void subscribe(const signalReferences_t& signalReferences);

			/// \throws std::runtime_error
			void unsubscribe(const signalReferences_t& signalReferences);

			/// connects to a streaming server, receives and processes all meta information and data.
			/// Returns when stream is stopped by calling stop() or if loss of connection is recognized.
			/// @param address address of the HBM daq stream server
			/// @param streamPort name or number of the HBM daq stream port. Might differ from default when communication runs via a router (CX27)
			/// @param controlPort name or number of the HBM daq stream control port. Might differ from default when communication runs via a router (CX27). In this case, use port information from the device announcement.
			int start(const std::string& address, const std::string &streamPort = "7411", const std::string& controlPort = "http");

			/// closes the stream socket.
			void stop();

			std::string address() const
			{
				return m_address;
			}

		private:

			StreamClient(const StreamClient&);
			StreamClient& operator= (const StreamClient&);

			/// handle stream related meta information that is relevant for this class.
			int processStreamMetaInformation(const std::string& method, const Json::Value& params);

			/// receives all data from the stream socket
			hbm::SocketNonblocking m_streamSocket;

			std::string m_address;
			std::string m_httpPath;

			std::string m_streamId;
			std::string m_controlPort;

			/// initial time received when opening the stream
			timeInfo_t m_initialTime;
			std::string m_initialTimeScale;
			std::string m_initialTimeEpoch;

			/// processes measured data and keeps meta information about all subscribed signals
			SignalContainer* m_pSignalContainer;

			StreamMetaCb_t m_streamMetaCb;
		};
	}
}
#endif // _HBM__STREAMING__STREAM_H
