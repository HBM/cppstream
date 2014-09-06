#include <cstring>
#include <stdint.h>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#include <cstdlib>
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <endian.h>
#endif

#ifdef _WIN32
#include "json/writer.h"
#else
#include <jsoncpp/json/writer.h>
#endif

#include "endianess.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {

		// we use this as target for all values. Otherwise the compiler might optimize away a lot of functionality!
		static double sum = 0;

		SubscribedSignal::SubscribedSignal()
			: m_signalReference()
			, m_startTime()

			, m_signalRateSamples(0)
			, m_signalRateSamplesDelta()
			, m_signalRateDelta()

			, m_dataFormatPattern()
			, m_dataIsBigEndian(false)
			, m_dataValueType()
			, m_dataValueSize(0)
			, m_dataTimeType()
			, m_dataTimeSize(0)

			, m_valueCount(0)
		{
		}

		void SubscribedSignal::interpretValues(unsigned char *pData, size_t count)
		{
			if(m_dataIsBigEndian) {
				if(m_dataValueType==DATATYPE_REAL32) {
					for(size_t i=0; i<count; ++i) {
						uint32_t targetUint32;
						std::memcpy(&targetUint32, &pData, sizeof(targetUint32));
						targetUint32 = be32toh(targetUint32);
						float target;
						std::memcpy(&target, &targetUint32, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_REAL64) {
					for(size_t i=0; i<count; ++i) {
						uint64_t targetUint64;
						std::memcpy(&targetUint64, &pData, sizeof(targetUint64));
						targetUint64 = be64toh(targetUint64);
						double target;
						std::memcpy(&target, &targetUint64, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_U32) {
					for(size_t i=0; i<count; ++i) {
						uint32_t target;
						std::memcpy(&target, &pData, sizeof(target));
						target = be32toh(target);
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_S32) {
					for(size_t i=0; i<count; ++i) {
						uint32_t targetUint32;
						std::memcpy(&targetUint32, &pData, sizeof(targetUint32));
						targetUint32 = be32toh(targetUint32);
						int32_t target;
						std::memcpy(&target, &targetUint32, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_U64) {
					for(size_t i=0; i<count; ++i) {
						uint64_t target;
						std::memcpy(&target, &pData, sizeof(target));
						target = be64toh(target);
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_S64) {
					for(size_t i=0; i<count; ++i) {
						uint64_t targetUint64;
						std::memcpy(&targetUint64, &pData, sizeof(targetUint64));
						targetUint64 = be64toh(targetUint64);
						int64_t target;
						std::memcpy(&target, &targetUint64, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				}
			} else {
				// handle little endian to host here...
				if(m_dataValueType==DATATYPE_REAL32) {
					for(size_t i=0; i<count; ++i) {
						uint32_t targetUint32;
						std::memcpy(&targetUint32, &pData, sizeof(targetUint32));
						targetUint32 = le32toh(targetUint32);
						float target;
						std::memcpy(&target, &targetUint32, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_REAL64) {
					for(size_t i=0; i<count; ++i) {
						uint64_t targetUint64;
						std::memcpy(&targetUint64, &pData, sizeof(targetUint64));
						targetUint64 = le64toh(targetUint64);
						double target;
						std::memcpy(&target, &targetUint64, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_U32) {
					for(size_t i=0; i<count; ++i) {
						uint32_t target;
						std::memcpy(&target, &pData, sizeof(target));
						target = le32toh(target);
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_S32) {
					for(size_t i=0; i<count; ++i) {
						uint32_t targetUint32;
						std::memcpy(&targetUint32, &pData, sizeof(targetUint32));
						targetUint32 = le32toh(targetUint32);
						int32_t target;
						std::memcpy(&target, &targetUint32, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_U64) {
					for(size_t i=0; i<count; ++i) {
						uint64_t target;
						std::memcpy(&target, &pData, sizeof(target));
						target = le64toh(target);
						sum += target;
						pData += sizeof(target);
					}
				} else if(m_dataValueType==DATATYPE_S64) {
					for(size_t i=0; i<count; ++i) {
						uint64_t targetUint64;
						std::memcpy(&targetUint64, &pData, sizeof(targetUint64));
						targetUint64 = le64toh(targetUint64);
						int64_t target;
						std::memcpy(&target, &targetUint64, sizeof(target));
						sum += target;
						pData += sizeof(target);
					}
				}
			}
		}

		void SubscribedSignal::interpreteTimestamp(unsigned char* pData)
		{
			if(m_dataTimeType == TIMETYPE_NTP) {
				uint64_t ntpTimestamp;
				if(m_dataIsBigEndian) {
					ntpTimestamp = be64toh(*reinterpret_cast < uint64_t* > (pData));
				} else {
					ntpTimestamp = le64toh(*reinterpret_cast < uint64_t* > (pData));
				}
			}
		}

		void SubscribedSignal::calculateTimestamp()
		{
			timeInfo_t delta(m_signalRateDelta.ntpTimeStamp()*m_valueCount);
			m_startTime.increment(delta);
			timeInfo_t time = m_startTime;
		}




		void SubscribedSignal::dataCb(unsigned char* pData, size_t size)
		{
			switch(m_dataFormatPattern) {
				case PATTERN_V:
					{
						size_t valueCount = size / m_dataValueSize;
						calculateTimestamp();
						interpretValues(pData, valueCount);
						m_valueCount += valueCount;
					}
					break;
				case PATTERN_TV:
					{
						// 1 time stamp, 1 value
						size_t tupleSize = m_dataTimeSize+m_dataValueSize;
						while(size>=tupleSize) {
							interpreteTimestamp(pData);
							pData += m_dataTimeSize;
							interpretValues(pData, 1);
							pData += m_dataValueSize;
							size -= tupleSize;
							++m_valueCount;
						}
					}
					break;
				case PATTERN_TB:
					// 1 time stamp n values
					if(size>=m_dataTimeSize+m_dataValueSize) {
						size_t valueCount = (size-m_dataTimeSize) / m_dataValueSize;
						interpreteTimestamp(pData);
						interpretValues(pData+m_dataTimeSize, valueCount);
						m_valueCount += valueCount;
					}
					break;
			}
		}

		void SubscribedSignal::metaCb(const std::string& method, const Json::Value& params)
		{
			if(method=="time") {
				m_startTime.set(params);
			} else if(method=="data") {
				setDataFormat(params);
			} else if(method=="signalRate") {
				try {
					std::cout << Json::StyledWriter().write(params) << std::endl;
					m_signalRateSamples = params["samples"].asUInt();
					m_signalRateSamplesDelta.set(params["delta"]);
					m_signalRateDelta.setNtpTimestamp(m_signalRateSamplesDelta.ntpTimeStamp()/m_signalRateSamples);
				} catch(const std::runtime_error& e) {
					std::cerr << e.what();
				}
			} else {
				std::cout << "unhandled signal related meta information '" << method << "' for signal " << m_signalReference << " with parameters: " << Json::StyledWriter().write(params) << std::endl;
			}
		}


		int SubscribedSignal::setDataFormat(const Json::Value& params)
		{
			try {
				std::string dataFormatPattern = params["pattern"].asString();
				if(dataFormatPattern=="V") {
					m_dataFormatPattern = PATTERN_V;
				} else if(dataFormatPattern=="TV"){
					m_dataFormatPattern = PATTERN_TV;
				} else if(dataFormatPattern=="TB"){
					m_dataFormatPattern = PATTERN_TB;
				} else {
					return -1;
				}

				if(params["endian"].asString()=="big") {
					m_dataIsBigEndian = true;
				} else {
					m_dataIsBigEndian = false;
				}
				std::string dataValueType = params["valueType"].asString();
				if(dataValueType=="real32") {
					m_dataValueType = DATATYPE_REAL32;
					m_dataValueSize = 4;
				} else if(dataValueType=="u32") {
					m_dataValueType = DATATYPE_U32;
					m_dataValueSize = 4;
				} else if(dataValueType=="s32") {
					m_dataValueType = DATATYPE_S32;
					m_dataValueSize = 4;
				} else if(dataValueType=="real64") {
					m_dataValueType = DATATYPE_REAL64;
					m_dataValueSize = 8;
				} else if(dataValueType=="u64") {
					m_dataValueType = DATATYPE_U64;
					m_dataValueSize = 8;
				} else if(dataValueType=="s64") {
					m_dataValueType = DATATYPE_S64;
					m_dataValueSize = 8;
				}


				std::string dataTimeType = params["time"]["type"].asString();
				if(dataTimeType=="ntp") {
					m_dataTimeType = TIMETYPE_NTP;
				}
				m_dataTimeSize = params["time"]["size"].asUInt();

				return 0;
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
				return -1;
			}
		}
	}
}
