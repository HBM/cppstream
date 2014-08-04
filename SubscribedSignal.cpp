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
		{
		}

		void SubscribedSignal::interpreteValues(unsigned char *pData, size_t size)
		{
			if(m_dataIsBigEndian) {
				if(m_dataValueType==DATATYPE_REAL32) {
					uint32_t* pPos = reinterpret_cast < uint32_t* > (pData);
					uint32_t targetUint32;
					float* pTarget;
					size_t count = size >> 2;
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
					size_t count = size >> 4;
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
					size_t count = size >> 2;
					for(size_t i=0; i<count; ++i) {
						// this is it!
						target = ntohl(*pPos);
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_S32) {
					int32_t* pPos = reinterpret_cast < int32_t* > (pData);
					int32_t target;
					size_t count = size >> 2;
					for(size_t i=0; i<count; ++i) {
						// this is it!
						target = ntohl(*pPos);
						sum += target;
						++pPos;
					}
				} else if(m_dataValueType==DATATYPE_U64) {
					uint64_t* pPos = reinterpret_cast < uint64_t* > (pData);
					uint64_t target;
					size_t count = size >> 4;
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
					size_t count = size >> 4;
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
					size_t count = size >> 2;
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
					size_t count = size >> 4;
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
					size_t count = size >> 2;
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
					size_t count = size >> 2;
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
					size_t count = size >> 4;
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
					size_t count = size >> 4;
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


		void SubscribedSignal::dataCb(unsigned char* pData, size_t size)
		{
			switch(m_dataFormatPattern) {
				case PATTERN_V:
					interpreteValues(pData, size);
					break;
				case PATTERN_TV:
					// 1 time stamp, 1 value
					// there might be an optimized version here that expects only one value.
					if(size>=m_dataTimeSize) {
						interpreteValues(pData+m_dataTimeSize, size-m_dataTimeSize);
					}
					break;
				case PATTERN_TB:
					// 1 time stamp n values
					if(size>=m_dataTimeSize) {
						interpreteValues(pData+m_dataTimeSize, size-m_dataTimeSize);
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
				m_signalRate = params;
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
