#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#include "json/reader.h"
#include "json/writer.h"
#define strncasecmp _strnicmp
#else
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#endif

#include "TransportHeader.h"
#include "MetaInformation.h"
#include "SubscribedSignal.h"
#include "Types.h"
#include "StreamClient.h"
#include "Controller.h"

namespace hbm {
	namespace streaming {

		StreamClient::StreamClient()
			: m_streamSocket()
			, m_address()
			, m_apiVersion()
			, m_streamId()
			, m_controlPort()
			, m_initialTime()
			, m_availableSignals()
			, m_subscribedSignals()
			, m_customDataCb()
			, m_customStreamMetaCb()
		{
		}

		StreamClient::StreamClient(const std::string& fileName)
			: m_streamSocket(fileName)
			, m_address()
			, m_apiVersion()
			, m_streamId()
			, m_controlPort()
			, m_initialTime()
			, m_availableSignals()
			, m_subscribedSignals()
			, m_customDataCb()
			, m_customStreamMetaCb()
		{
		}

		void StreamClient::setCustomDataCb(DataCb_t cb)
		{
			m_customDataCb = cb;
		}


		void StreamClient::setCustomStreamMetaCb(StreamMetaCb_t cb)
		{
			m_customStreamMetaCb = cb;
		}

		void StreamClient::setCustomSignalMetaCb(SignalMetaCb_t cb)
		{
			m_customSignalMetaCb = cb;
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


		int StreamClient::start(const std::string& address, const std::string &streamPort, const std::string &controlPort)
		{
			int result = m_streamSocket.connect(address.c_str(), streamPort);
			if(result<0) {
				return -1;
			}

			m_address = address;
			m_controlPort = controlPort;


			unsigned char dataRecvBuffer[8192];

			TransportHeader transportHeader(m_streamSocket);
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
					result = m_streamSocket.receiveComplete(dataRecvBuffer, size);
					if(static_cast < size_t >(result)!=size) {
						break;
					}

					m_subscribedSignals[signalNumber].dataCb(dataRecvBuffer, result);
					if(m_customDataCb) {
						m_customDataCb(*this, signalNumber, dataRecvBuffer, result);
					}
				} else if(type==TYPE_META){
					MetaInformation metaInformation(m_streamSocket, size);
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
								/// this is the first signal related meta information to arrive!
								if(params.empty()==false) {
									std::string signalReference = params[0].asString();
									m_subscribedSignals[signalNumber].setSignalReference(signalReference);
								}
							} else if(method=="unsubscribe") {
								m_subscribedSignals.erase(signalNumber);
							} else {
								m_subscribedSignals[signalNumber].metaCb(method, params);

								if(m_customSignalMetaCb) {
									m_customSignalMetaCb(*this, signalNumber, method, params);
								}
							}
						}
					}
				}
			} while(true);

			return 0;
		}

		void StreamClient::stop()
		{
			m_streamSocket.stop();
			m_address.clear();
			m_apiVersion.clear();
			m_streamId.clear();
			m_controlPort.clear();
			m_initialTime.clear();
			m_availableSignals.clear();
			m_subscribedSignals.clear();
		}

		int StreamClient::metaCb(const std::string& method, const Json::Value& params)
		{
			try {
				// stream related meta information
				if(method=="apiVersion") {
					if(params.empty()==false) {
						m_apiVersion = params[0].asString();
					}
					std::cout << m_address << ": daq stream version: " << m_apiVersion << std::endl;
				} else if(method=="init") {
					m_streamId = params["streamId"].asString();
					std::cout << m_address << ": this is " << m_streamId << std::endl;
					std::cout << m_address << ": supported features: " << params["supported"] << std::endl;
					const Json::Value& commandInterfaces = params["commandInterfaces"];
					for (Json::ValueConstIterator iter = commandInterfaces.begin(); iter!= commandInterfaces.end(); ++iter) {
						const Json::Value& element = *iter;
						std::cout << m_address << ": command interfaces: " << element << std::endl;
						static const char POST[] = "post";
						if(strncasecmp(element["httpMethod"].asString().c_str(), POST, sizeof(POST))==0) {
							m_httpPath = element["httpPath"].asString();
						}
					}
				} else if(method=="time") {
					m_initialTime.set(params["stamp"]);
					m_initialTimeEpoch = params["epoch"].asString();
					m_initialTimeScale = params["scale"].asString();
				} else if(method=="alive") {
					// We do ignore this. We are using TCP keep alive in order to detect communication problems.
				} else if(method=="fill") {
					if(params.empty()==false) {
						unsigned int fill = params[0].asUInt();
						if(fill>25) {
							std::cout << m_address << ": ring buffer fill level is " << params[0].asUInt() << "%" << std::endl;
						}
					}
				} else if(method=="available") {
					for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
						const Json::Value& element = *iter;
						m_availableSignals.insert(element.asString());
					}
				} else if(method=="unavailable") {
					for (Json::ValueConstIterator iter = params.begin(); iter!= params.end(); ++iter) {
						const Json::Value& element = *iter;
						m_availableSignals.erase(element.asString());
					}
				} else {
					std::cout << m_address << ": unhandled stream related meta information '" << method << "' with parameters: " << Json::StyledWriter().write(params) << std::endl;
				}
				return 0;
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
				return -1;
			}
		}
	}
}
