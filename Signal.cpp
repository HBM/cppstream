#include <iostream>

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
			m_dataValueType = params["valueType"].asString();
			if(
				 (m_dataValueType=="real32") ||
				 (m_dataValueType=="u32") ||
				 (m_dataValueType=="s32") ||
				 (m_dataValueType=="u32")
				 ) {
				m_dataValueSize = 4;
			} else if(
								(m_dataValueType=="real64") ||
								(m_dataValueType=="u64") ||
								(m_dataValueType=="s64") ||
								(m_dataValueType=="u64")
								) {
				m_dataValueSize = 8;
			}

			m_dataTimeType = params["time"]["type"].asString();
			m_dataTimeSize = params["time"]["size"].asUInt();
			return 0;
		}
	}
}
