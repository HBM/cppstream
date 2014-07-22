#ifndef _HBM__STREAMING__SIGNAL
#define _HBM__STREAMING__SIGNAL

#include "Types.h"

namespace hbm {
	namespace streaming {
		/// \warning for windows, this class works on little endian machines only. this is because of endian issues with data types wider than 4 byte.
		class Signal {
		public:
			Signal();

			void dataCb(unsigned char* pData, size_t size);

			std::string signalReference;
			timeInfo_t startTime;
			Json::Value signalRate;
			int dataFormat(const Json::Value& params);
		private:

			enum pattern_t {
				/// "V"; No timestamps, values only.
				PATTERN_V,
				/// "TV"; One timestamp per value, first comes the timestamp, then the value. This pattern is highly recommended for asynchonous values.
				PATTERN_TV,
				/// "TB"; One timestamp per signal block. The timestamp corresponds to the first sample in the signal block.
				PATTERN_TB,
			};

			enum dataType_t {
				DATATYPE_U32,
				DATATYPE_S32,
				DATATYPE_REAL32,
				DATATYPE_U64,
				DATATYPE_S64,
				DATATYPE_REAL64,
			};

			pattern_t m_dataFormatPattern;
			bool m_dataIsBigEndian;
			dataType_t m_dataValueType;
			size_t m_dataValueSize;
			std::string m_dataTimeType;
			size_t m_dataTimeSize;

			size_t m_dataByteCount;
		};
	}
}
#endif
