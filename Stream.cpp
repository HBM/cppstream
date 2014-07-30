#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

#ifdef _WIN32
#include "jsoncpp/include/json/reader.h"
#include "jsoncpp/include/json/writer.h"
#else
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#endif

#include "SocketNonblocking.h"
#include "TransportHeader.h"
#include "MetaInformation.h"
#include "SubscribedSignal.h"
#include "Types.h"
#include "Stream.h"
#include "Controller.h"

namespace hbm {
	namespace streaming {

		Stream::Stream(const std::string& address)
			: m_address(address)
		{
		}

		void Stream::setCustomStreamMetaCb(StreamMetaCb_t cb)
		{
			m_customStreamMetaCb = cb;
		}

		void Stream::setCustomSignalMetaCb(SignalMetaCb_t cb)
		{
			m_customSignalMetaCb = cb;
		}

		int Stream::subscribe(const signalReferences_t& signalReferences)
		{
			Controller controller(m_streamId, m_address.c_str(), m_controlPort);
			controller.subscribe(signalReferences);
		}

		int Stream::unsubscribe(const signalReferences_t& signalReferences)
		{
			Controller controller(m_streamId, m_address.c_str(), m_controlPort);
			controller.unsubscribe(signalReferences);

		}


		int Stream::receive(const std::string &controlPort)
		{
			m_controlPort = controlPort;
			hbm::SocketNonblocking streamSocket;
			int result = streamSocket.connect(m_address.c_str(), STREAM_DATA_PORT);
			if(result<0) {
				return -1;
			}

			unsigned char dataRecvBuffer[1024];

			TransportHeader transportHeader(streamSocket);
			do {
				result = transportHeader.receive();
				if(result<0) {
					break;
				}
				int type = transportHeader.type();
				size_t size = transportHeader.size();
				unsigned int signalNumber = transportHeader.signalNumber();

				if(type==TYPE_DATA) {
					// read measured data. This happens really often! Be sure to be as efficient as possible here.
					result = streamSocket.receiveComplete(dataRecvBuffer, size);
					if(result!=size) {
						break;
					}
					m_signalProperties[signalNumber].dataCb(dataRecvBuffer, result);
				} else if(type==TYPE_META){
					MetaInformation metaInformation(streamSocket, size);
					if(metaInformation.type()!=METAINFORMATION_JSON) {
						std::cout << "unhandled meta information of type " << metaInformation.type() << std::endl;
					} else {
						const Json::Value& content = metaInformation.jsonContent();
						std::string method = content[METHOD].asString();
						const Json::Value& params = content[PARAMS];

						if(signalNumber==0) {
							// stream related meta information
							metaCb(method, params);
							if(m_customStreamMetaCb) {
								m_customStreamMetaCb(*this, method, params);
							}
						} else {
							// signal related meta information
							if(method=="subscribe") {
								std::string signalReference = params[0].asString();
								m_signalProperties[signalNumber].setSignalReference(signalReference);
								std::cout << "subscribed signal number= " << signalNumber << " with signal reference '" << signalReference << "'" << std::endl;
							} else if(method=="unsubscribe") {
								std::string signalReference = params[0].asString();
								m_signalProperties.erase(signalNumber);
								std::cout << "unsubscribed signal number= " << signalNumber << " with signal reference '" << signalReference << "'" << std::endl;
							} else {
								m_signalProperties[signalNumber].metaCb(method, params);
								if(m_customStreamMetaCb) {
									m_customSignalMetaCb(*this, signalNumber, method, params);
								}
							}
						}
					}
				}
			} while(true);

			return 0;
		}

		int Stream::metaCb(const std::string& method, const Json::Value& params)
		{
			try {
				// stream related meta information
				if(method=="apiVersion") {
					m_apiVersion = params[0].asString();
				} else if(method=="init") {
					m_streamId = params["streamId"].asString();
					std::cout << "this is: " << m_streamId << std::endl;
					const Json::Value& supported = params["supported"];
					for (Json::ValueConstIterator iter = supported.begin(); iter!= supported.end(); ++iter) {
						const Json::Value& element = *iter;
						std::cout << Json::StyledWriter().write(element) << std::endl;
					}

					const Json::Value& commandInterfaces = params["commandInterfaces"];
					for (Json::ValueConstIterator iter = commandInterfaces.begin(); iter!= commandInterfaces.end(); ++iter) {
						const Json::Value& element = *iter;
						std::cout << Json::StyledWriter().write(element) << std::endl;
					}
				} else if(method=="time") {
					m_initialTime.set(params);
				} else if(method=="alive") {
					// We do ignore this. We are using TCP keep alive in order to detect communication problems.
				} else if(method=="fill") {
					unsigned int fill = params[0].asUInt();
					if(fill>25) {
						std::cout << "ring buffer fill level is " << params[0].asUInt() << "%" << std::endl;
					}
				} else if(method=="available") {
					std::string signalReference;
					std::cout << "the following signal(s) became available: ";
					for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
						const Json::Value& element = *iter;
						signalReference = element.asString();
						m_availableSignals.insert(signalReference);
						std::cout << "'" << signalReference << "' ";
					}
					std::cout << std::endl;
				} else if(method=="unavailable") {
					std::string signalReference;
					std::cout << "the following signal(s) became unavailable: ";
					for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
						const Json::Value& element = *iter;
						signalReference = element.asString();
						m_availableSignals.erase(signalReference);
						std::cout << "'" << signalReference << "' ";
					}
					std::cout << std::endl;
				} else {
					std::cout << "unhandled stream related meta information '" << method << "' with parameters: " << Json::StyledWriter().write(params) << std::endl;
				}
				return 0;
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
				return -1;
			}
		}
	}
}
