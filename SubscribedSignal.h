#ifndef _HBM__STREAMING__SUBSCRIBEDSIGNAL
#define _HBM__STREAMING__SUBSCRIBEDSIGNAL

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include <jsoncpp/json/value.h>
#endif

#include "Types.h"

namespace hbm {
	namespace streaming {
		/// interpretes and stores meta information of a subscribed signal.
		/// Mesured data of a subscribed signal is processed here
		/// \warning for windows, this class works on little endian machines only. this is because of endian issues.
		class SubscribedSignal {
		public:
			SubscribedSignal();

			void setSignalReference(const std::string& signalReference) {
				m_signalReference = signalReference;
			}

			int setDataFormat(const Json::Value& params);

			/// process measured data
			void dataCb(unsigned char* pData, size_t size);

			void metaCb(const std::string& method, const Json::Value& params);

		private:

			void interpreteValues(unsigned char* pData, size_t size);

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



			std::string m_signalReference;
			timeInfo_t m_startTime;
			Json::Value m_signalRate;


			pattern_t m_dataFormatPattern;
			bool m_dataIsBigEndian;
			dataType_t m_dataValueType;
			size_t m_dataValueSize;
			std::string m_dataTimeType;
			size_t m_dataTimeSize;
		};
	}
}
#endif
