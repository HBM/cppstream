// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <vector>
#include <set>
#include <iostream>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif
#include <json/reader.h>
#include <json/writer.h>


#include "transportheader.h"
#include "metainformation.h"
#include "subscribedsignal.h"
#include "types.h"
#include "streamclient.h"
#include "controller.h"

namespace hbm {
	namespace streaming {

		StreamClient::StreamClient()
			: m_streamSocket()
			, m_address()
			, m_httpPath()
			, m_streamId()
			, m_streamPort()
			, m_controlPort()
			, m_initialTime()
			, m_initialTimeScale()
			, m_initialTimeEpoch()
			, m_pSignalContainer(NULL)
			, m_streamMetaCb()
		{
		}

		void StreamClient::setStreamMetaCb(StreamMetaCb_t cb)
		{
			m_streamMetaCb = cb;
		}

		void StreamClient::subscribe(const signalReferences_t& signalReferences)
		{
			Controller controller(m_streamId, m_address.c_str(), m_controlPort, m_httpPath);
			controller.subscribe(signalReferences);
		}

		void StreamClient::unsubscribe(const signalReferences_t& signalReferences)
		{
			Controller controller(m_streamId, m_address.c_str(), m_controlPort, m_httpPath);
			controller.unsubscribe(signalReferences);
		}


		int StreamClient::start(const std::string& address, const std::string &streamPort, const std::string& controlPort)
		{
			int result = m_streamSocket.connect(address.c_str(), streamPort);
			if (result < 0) {
				std::cerr << "could not connect to streming port " << address << ":" << streamPort << std::endl;
				return -1;
			}

			m_address = address;
			m_streamPort = streamPort;
			m_controlPort = controlPort;

			unsigned char dataRecvBuffer[8192];

			TransportHeader transportHeader(m_streamSocket);
			do {
				result = transportHeader.receive();
				if (result < 0) {
					break;
				}
				int type = transportHeader.type();
				size_t bytesToProcess = transportHeader.size();
				unsigned int signalNumber = transportHeader.signalNumber();

				if (type == TYPE_DATA) {
					// read and process measured data. This happens really often! Be sure to be as efficient as possible here.

					size_t bytesInBuffer = 0;
					size_t bytesToReadToBuffer;
					while(bytesToProcess>0) {
						if(bytesToProcess>sizeof(dataRecvBuffer)) {
							bytesToReadToBuffer = sizeof(dataRecvBuffer)-bytesInBuffer;
						} else {
							bytesToReadToBuffer = bytesToProcess-bytesInBuffer;
						}
						ssize_t bytesReadToBuffer = m_streamSocket.receiveComplete(dataRecvBuffer+bytesInBuffer, bytesToReadToBuffer);
						if(bytesReadToBuffer<=0) {
							break;
						}
						bytesInBuffer += bytesReadToBuffer;

						if (m_pSignalContainer) {
							size_t bytesProcessedFromBuffer = m_pSignalContainer->processMeasuredData(signalNumber, dataRecvBuffer, bytesInBuffer);
							bytesToProcess -= bytesProcessedFromBuffer;
							bytesInBuffer -= bytesProcessedFromBuffer;
							memmove(dataRecvBuffer, dataRecvBuffer+bytesProcessedFromBuffer, bytesInBuffer);
						} else {
							bytesToProcess -= bytesInBuffer;
							bytesInBuffer = 0;
						}
					}
				} else if (type == TYPE_META){
					MetaInformation metaInformation(m_streamSocket, bytesToProcess);
					if(metaInformation.type()!=METAINFORMATION_JSON) {
						std::cout << "Signal number " << signalNumber << ": unhandled meta information of type " << metaInformation.type() << std::endl;
					} else {
						const Json::Value& content = metaInformation.jsonContent();
						std::string method = content[METHOD].asString();
						const Json::Value& params = content[PARAMS];

						if (signalNumber == 0) {
							// stream related meta information
							processStreamMetaInformation(method, params);
						} else {
							// signal related meta information
							if (m_pSignalContainer) {
								m_pSignalContainer->processMetaInformation(signalNumber, method, params);
							}
						}
					}
				}
			} while(true);
			
			std::cout << "stream client " << m_address << ":" << m_streamPort << " stopped" << std::endl;

			return result;
		}

		void StreamClient::stop()
		{
			m_streamSocket.stop();
			m_address.clear();
			m_streamId.clear();
			m_controlPort.clear();
			m_initialTime.clear();
		}

		int StreamClient::processStreamMetaInformation(const std::string& method, const Json::Value& params)
		{
			try {
				// stream related meta information
				if (method == "init") {
					// this gives important information needed to control the daq stream.
					m_streamId = params["streamId"].asString();
					std::cout << m_address << ": this is " << m_streamId << std::endl;
					std::cout << m_address << ": supported features: " << params["supported"] << std::endl;
					const Json::Value& commandInterfaces = params["commandInterfaces"];
					for (Json::ValueConstIterator iter = commandInterfaces.begin(); iter!= commandInterfaces.end(); ++iter) {
						const Json::Value& element = *iter;
						std::cout << m_address << ": command interfaces: " << element << std::endl;
						static const char POST[] = "post";
						if (strncasecmp(element["httpMethod"].asString().c_str(), POST, sizeof(POST)) == 0) {
							m_httpPath = element["httpPath"].asString();
						}
						
						// do not overwrite if control port is already set.
						// important for devices behind NAT router
						if (m_controlPort.empty()) {
							m_controlPort = element["port"].asString();
						}
					}
				} else if (method == "time") {
					m_initialTime.set(params["stamp"]);
					m_initialTimeEpoch = params["epoch"].asString();
					m_initialTimeScale = params["scale"].asString();
				}
				if(m_streamMetaCb) {
					m_streamMetaCb(*this, method, params);
				}

				return 0;
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
				return -1;
			}
		}
	}
}
