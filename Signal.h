#ifndef _HBM__STREAMING__SIGNAL
#define _HBM__STREAMING__SIGNAL

#include "Types.h"

namespace hbm {
	namespace streaming {
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

			pattern_t m_dataFormatPattern;
			bool m_dataIsBigEndian;
			std::string m_dataValueType;
			size_t m_dataValueSize;
			std::string m_dataTimeType;
			size_t m_dataTimeSize;

			size_t m_dataByteCount;
		};
	}
}
#endif
