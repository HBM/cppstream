#include <stdint.h>
#include <iostream>

#include <arpa/inet.h>

#include "Signal.h"

namespace hbm {
	namespace streaming {
		Signal::Signal()
			: m_dataByteCount(0)
		{
		}

		void Signal::dataCb(unsigned char* pData, size_t size)
		{
			switch(m_dataFormatPattern) {
				case PATTERN_V:
					{
						if(m_dataIsBigEndian) {
							if(m_dataValueType==DATATYPE_REAL32) {
								uint32_t* pPos = reinterpret_cast < uint32_t* > (pData);
								float target;
								size_t count = size >> 2;
								for(size_t i=0; i<count; ++i) {
									target = ntohl(*pPos);
									++pPos;
								}
							}
						}
					}
					break;
				case PATTERN_TV:
					break;
				case PATTERN_TB:
					break;
			}

			m_dataByteCount += size;
		}

		int Signal::dataFormat(const Json::Value& params)
		{
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
			} else if(dataValueType=="u32") {
				m_dataValueType = DATATYPE_U32;
			} else if(dataValueType=="s32") {
				m_dataValueType = DATATYPE_S32;
			} else if(dataValueType=="real64") {
				m_dataValueType = DATATYPE_REAL64;
			} else if(dataValueType=="u64") {
				m_dataValueType = DATATYPE_U64;
			} else if(dataValueType=="s64") {
				m_dataValueType = DATATYPE_S64;
			}

			if(
				 (m_dataValueType==DATATYPE_REAL32) ||
				 (m_dataValueType==DATATYPE_U32) ||
				 (m_dataValueType==DATATYPE_S32)
				 ) {
				m_dataValueSize = 4;
			} else if(
								(m_dataValueType==DATATYPE_REAL64) ||
								(m_dataValueType==DATATYPE_U64) ||
								(m_dataValueType==DATATYPE_S64)
								) {
				m_dataValueSize = 8;
			}








			m_dataTimeType = params["time"]["type"].asString();
			m_dataTimeSize = params["time"]["size"].asUInt();
			return 0;
		}
	}
}
