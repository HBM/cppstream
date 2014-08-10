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
#include "jsoncpp/include/json/writer.h"
#else
#include <jsoncpp/json/writer.h>
#endif

#include "SubscribedSignal.h"

namespace hbm {
	namespace streaming {

		// we use this as target for all values. Otherwise the compiler might optimize away a lot of functionality!
		static double sum = 0;

		SubscribedSignal::SubscribedSignal()
			: m_signalReference()
			, m_startTime()

			, m_signalRateSamples(0)
			, m_signalRateDelta()


			, m_dataFormatPattern()
			, m_dataIsBigEndian(false)
			, m_dataValueType()
			, m_dataValueSize(0)
			, m_dataTimeType()
			, m_dataTimeSize(0)
	{
		}

		void SubscribedSignal::interpreteValues(unsigned char *pData, size_t count)
		{
			if(m_dataIsBigEndian) {
				if(m_dataValueType==DATATYPE_REAL32) {
					uint32_t* pPos = reinterpret_cast < uint32_t* > (pData);
					uint32_t targetUint32;
					float* pTarget;
					for(size_t i=0; i<count; ++i) {
						targetUint32 = ntohl(*pPos);
						// this is it!
						pTarget = reinterpret_cast < float* >(&targetUint32);
						sum += *pTarget;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_REAL64) {
					uint64_t* pPos = reinterpret_cast < uint64_t* > (pData);
					uint64_t targetUint64;
					double* pTarget;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						// this will create a mess on big endian machines
						targetUint64 = _byteswap_uint64(*pPos);
#else
						targetUint64 = be64toh(*pPos);
#endif
						// this is it!
						pTarget = reinterpret_cast < double* >(&targetUint64);
						sum += *pTarget;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_U32) {
					uint32_t* pPos = reinterpret_cast < uint32_t* > (pData);
					uint32_t target;
					for(size_t i=0; i<count; ++i) {
						// this is it!
						target = ntohl(*pPos);
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_S32) {
					int32_t* pPos = reinterpret_cast < int32_t* > (pData);
					int32_t target;
					for(size_t i=0; i<count; ++i) {
						// this is it!
						target = ntohl(*pPos);
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_U64) {
					uint64_t* pPos = reinterpret_cast < uint64_t* > (pData);
					uint64_t target;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						// this will create a mess on big endian machines
						target = _byteswap_uint64(*pPos);
#else
						target = be64toh(*pPos);
#endif
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_S64) {
					int64_t* pPos = reinterpret_cast < int64_t* > (pData);
					int64_t target;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						// this will create a mess on big endian machines
						target = _byteswap_uint64(*pPos);
#else
						target = be64toh(*pPos);
#endif
						sum += target;
						++pPos;
					}
				}
			} else {
				// handle little endian to host here...
				if(m_dataValueType==DATATYPE_REAL32) {
					uint32_t* pPos = reinterpret_cast < uint32_t* > (pData);
					uint32_t targetUint32;
					float* pTarget;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						targetUint32 =*pPos;
#else
						targetUint32 = le32toh(*pPos);
#endif
						// this is it!
						pTarget = reinterpret_cast < float* >(&targetUint32);
						sum += *pTarget;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_REAL64) {
					uint64_t* pPos = reinterpret_cast < uint64_t* > (pData);
					uint64_t targetUint64;
					double* pTarget;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						targetUint64 =*pPos;
#else
						targetUint64 = le64toh(*pPos);
#endif
						// this is it!
						pTarget = reinterpret_cast < double* >(&targetUint64);
						sum += *pTarget;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_U32) {
					uint32_t* pPos = reinterpret_cast < uint32_t* > (pData);
					uint32_t target;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						target =*pPos;
#else
						target = le32toh(*pPos);
#endif
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_S32) {
					int32_t* pPos = reinterpret_cast < int32_t* > (pData);
					int32_t target;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						target =*pPos;
#else
						target = le32toh(*pPos);
#endif
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_U64) {
					uint64_t* pPos = reinterpret_cast < uint64_t* > (pData);
					uint64_t target;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						target =*pPos;
#else
						target = le64toh(*pPos);
#endif
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_S64) {
					int64_t* pPos = reinterpret_cast < int64_t* > (pData);
					int64_t target;
					for(size_t i=0; i<count; ++i) {
#ifdef _WIN32
						target =*pPos;
#else
						target = le64toh(*pPos);
#endif
						sum += target;
						++pPos;
					}
				}
			}
		}

		void SubscribedSignal::interpreteTimestamp(unsigned char* pData)
		{
			if(m_dataTimeSize==4) {
				uint32_t target;
#ifdef _WIN32
				if(m_dataIsBigEndian) {
					target = ntohl(*pPos);
				} else {
					target = *pData;
				}
#else
				if(m_dataIsBigEndian) {
					target = be32toh(*pData);
				} else {
					target = le32toh(*pData);
				}
#endif
			} else if(m_dataTimeSize==8) {
				uint64_t target;
#ifdef _WIN32
				if(m_dataIsBigEndian) {
					// this will create a mess on big endian machines
					target = _byteswap_uint64(*pPos);
				} else {
					target = *pData;
				}
#else
				if(m_dataIsBigEndian) {
					target = be64toh(*pData);
				} else {
					target = le64toh(*pData);
				}
#endif
			}
		}



		void SubscribedSignal::dataCb(unsigned char* pData, size_t size)
		{
			switch(m_dataFormatPattern) {
				case PATTERN_V:
					{
						size_t valueCount;
						if(m_dataValueSize==4) {
							valueCount = size >> 2;
						} else if(m_dataValueSize==8) {
							valueCount = size >> 4;
						} else {
							valueCount / m_dataValueSize;
						}
						interpreteValues(pData, valueCount);
					}
					break;
				case PATTERN_TV:
					{
						// 1 time stamp, 1 value
						size_t tupleSize = m_dataTimeSize+m_dataValueSize;
						while(size>=tupleSize) {
							interpreteTimestamp(pData);
							pData += m_dataTimeSize;
							interpreteValues(pData, 1);
							pData += m_dataValueSize;
							size -= tupleSize;
						}
					}
					break;
				case PATTERN_TB:
					// 1 time stamp n values
					if(size>=m_dataTimeSize+m_dataValueSize) {
						size_t valueCount;
						if(m_dataValueSize==4) {
							valueCount = (size-m_dataTimeSize) >> 2;
						} else if(m_dataValueSize==8) {
							valueCount = (size-m_dataTimeSize) >> 4;
						} else {
							valueCount / m_dataValueSize;
						}
						interpreteTimestamp(pData);
						interpreteValues(pData+m_dataTimeSize, valueCount);
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
					m_signalRateSamples = params["samples"].asUInt();
					m_signalRateDelta.set(params["delta"]);
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


				m_dataTimeType = params["time"]["type"].asString();
				m_dataTimeSize = params["time"]["size"].asUInt();
				return 0;
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
				return -1;
			}
		}
	}
}
