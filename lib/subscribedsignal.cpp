#include <cstring>
#include <stdint.h>
#include <iostream>
#include <stdexcept>

#include <json/writer.h>

#include "signalextract.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		SubscribedSignal::SubscribedSignal()
			: m_signalReference()
			, m_syncSignalTime()

			, m_dataFormatPattern()
			, m_dataIsBigEndian(false)
			, m_dataValueType()
			, m_dataValueSize(0)
			, m_dataTimeType()
			, m_dataTimeSize(0)
		{
		}

		void SubscribedSignal::interpretValues(unsigned char *pData, size_t count)
		{
			if(m_dataIsBigEndian) {
				switch (m_dataValueType) {
					case DATATYPE_REAL32:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<float, hbm::streaming::big>()(&pData);
						}
						break;

					case DATATYPE_REAL64:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<double, hbm::streaming::big>()(&pData);
						}
						break;

					case DATATYPE_U32:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<uint32_t, hbm::streaming::big>()(&pData);
						}
						break;

					case DATATYPE_S32:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<int32_t, hbm::streaming::big>()(&pData);
						}
						break;

					case DATATYPE_U64:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = static_cast < double > (hbm::streaming::extract<uint64_t, hbm::streaming::big>()(&pData));
						}
						break;

					case DATATYPE_S64:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = static_cast < double > (hbm::streaming::extract<int64_t, hbm::streaming::big>()(&pData));
						}
						break;

					default:
						throw std::runtime_error("datatype not supported!");
						break;
				}
			} else {
				switch (m_dataValueType) {
					case DATATYPE_REAL32:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<float, hbm::streaming::little>()(&pData);
						}
						break;

					case DATATYPE_REAL64:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<double, hbm::streaming::little>()(&pData);
						}
						break;

					case DATATYPE_U32:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<uint32_t, hbm::streaming::little>()(&pData);
						}
						break;

					case DATATYPE_S32:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = hbm::streaming::extract<int32_t, hbm::streaming::little>()(&pData);
						}
						break;

					case DATATYPE_U64:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = static_cast < double > (hbm::streaming::extract<uint64_t, hbm::streaming::little>()(&pData));
						}
						break;

					case DATATYPE_S64:
						for(size_t i=0; i<count; ++i) {
							m_valueBuffer[i] = static_cast < double > (hbm::streaming::extract<int64_t, hbm::streaming::little>()(&pData));
						}
						break;

					default:
						throw std::runtime_error("datatype not supported!");
						break;
				}
			}
		}

		uint64_t SubscribedSignal::interpreteNtpTimestamp(unsigned char* pData)
		{
			if((m_dataTimeType == TIMETYPE_NTP) && (m_dataTimeSize==8) ) {
				if(m_dataIsBigEndian) {
					return be64toh(*reinterpret_cast < uint64_t* > (pData));
				} else {
					return le64toh(*reinterpret_cast < uint64_t* > (pData));
				}
			} else {
				std::cerr << __FUNCTION__ << ": Time format is not supported" << std::endl;
				return 0;
			}
		}

		size_t SubscribedSignal::processMeasuredData(unsigned char* pData, size_t size, DataCb_t cb)
		{
			size_t bytesProcessed = 0;

			switch(m_dataFormatPattern) {
			case PATTERN_V:
				{
					size_t valueCount = size / m_dataValueSize;
					size_t valueChunk;

					while(valueCount>0) {
						if (valueCount > m_valueBufferMaxValues) {
							valueChunk = m_valueBufferMaxValues;
						} else {
							valueChunk = valueCount;
						}
						interpretValues(pData, valueChunk);
						uint64_t timeStamp = m_syncSignalTime.increment(valueChunk);
						if (cb) {
							cb(*this, timeStamp, m_valueBuffer, valueChunk);
						}
						valueCount -= valueChunk;
					}

					bytesProcessed = valueCount * m_dataValueSize;
				}
				break;
			case PATTERN_TV:
				{
					// 1 time stamp, 1 value
					size_t tupleSize = m_dataTimeSize+m_dataValueSize;
					while (size>=tupleSize) {
						uint64_t ntpTimeStamp = interpreteNtpTimestamp(pData);
						pData += m_dataTimeSize;
						interpretValues(pData, 1);
						pData += m_dataValueSize;
						size -= tupleSize;
						if (cb) {
							cb(*this, ntpTimeStamp, m_valueBuffer, 1);
						}
						bytesProcessed += tupleSize;
					}
				}
				break;
			case PATTERN_TB:
				{
					// 1 time stamp n values
					size_t tupleSize = m_dataTimeSize+m_dataValueSize;
					if(size>=tupleSize) {
						size_t valueCount = (size-m_dataTimeSize) / m_dataValueSize;
						if(valueCount>m_valueBufferMaxValues) {
							valueCount=m_valueBufferMaxValues;
						}
						uint64_t ntpTimeStamp = interpreteNtpTimestamp(pData);
						interpretValues(pData+m_dataTimeSize, valueCount);
						if (cb) {
							cb(*this, ntpTimeStamp, m_valueBuffer, valueCount);
						}
						bytesProcessed = m_dataTimeSize + (m_dataValueSize*valueCount);
					}
				}
				break;
			default:
				std::cerr << m_signalReference + " value pattern is not set!" << std::endl;
				bytesProcessed = size;
			}
			return bytesProcessed;
		}

		void SubscribedSignal::processSignalMetaInformation(const std::string& method, const Json::Value& params)
		{
			if(method=="subscribe") {
				/// this is the first signal related meta information to arrive!
				if(params.empty()==false) {
					m_signalReference = params[0u].asString();
				} else {
					throw std::runtime_error("Incomplete meta information 'subscribe'");
				}
			} else if(method=="time") {
				m_syncSignalTime.setTime(params);
			} else if(method=="data") {
				setDataFormat(params);
			} else if(method=="signalRate") {
				std::cout << Json::StyledWriter().write(params) << std::endl;
				m_syncSignalTime.setDelta(params);
			} else {
				std::cout << "unhandled signal related meta information '" << method << "' for signal " << m_signalReference << " with parameters: " << Json::StyledWriter().write(params) << std::endl;
			}
		}


		void SubscribedSignal::setDataFormat(const Json::Value& params)
		{
			std::string dataFormatPattern = params["pattern"].asString();
			if(dataFormatPattern=="V") {
				m_dataFormatPattern = PATTERN_V;
			} else if(dataFormatPattern=="TV"){
				m_dataFormatPattern = PATTERN_TV;
			} else if(dataFormatPattern=="TB"){
				m_dataFormatPattern = PATTERN_TB;
			} else {
				throw std::runtime_error(m_signalReference + ": Value pattern is not supported");
			}

			if(params["endian"].asString()=="big") {
				m_dataIsBigEndian = true;
			} else if(params["endian"].asString()=="little") {
				m_dataIsBigEndian = false;
			} else {
				throw std::runtime_error(m_signalReference + ": illegal endianness");
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
			} else {
				throw std::runtime_error(m_signalReference + ": Value type is not supported");
			}


			std::string dataTimeType = params["time"]["type"].asString();
			if(dataTimeType=="ntp") {
				m_dataTimeType = TIMETYPE_NTP;
			} else {
				throw std::runtime_error(m_signalReference + ": Time type is not supported");
			}
			m_dataTimeSize = params["time"]["size"].asUInt();
		}
	}
}
