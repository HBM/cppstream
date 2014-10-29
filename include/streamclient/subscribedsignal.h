#ifndef _HBM__STREAMING__SUBSCRIBEDSIGNAL
#define _HBM__STREAMING__SUBSCRIBEDSIGNAL

#include <stdint.h>
#include <functional>

#include <json/value.h>

#include "timeinfo.h"

namespace hbm {
	namespace streaming {

		class SubscribedSignal;

		typedef std::function<void(SubscribedSignal& subscribedSignal, uint64_t ntpTimeStamp, double* values, size_t count)> DataCb_t;

		/// interpretes and stores meta information of a subscribed signal.
		/// Mesured data of a subscribed signal is processed here
		/// \warning for windows, this class works on little endian machines only. this is because of endian issues.
		class SubscribedSignal {
		public:
			SubscribedSignal();

			/// process measured data
			/// \return number of bytes processed
			size_t processMeasuredData(unsigned char* pData, size_t size, DataCb_t cb);

			/// process signal related meta information.
			void processSignalMetaInformation(const std::string& method, const Json::Value& params);

			std::string signalReference()
			{
				return m_signalReference;
			}

		private:

			/// @param count number of values not the number of bytes!
			void interpretValues(unsigned char* pData, size_t count);

			uint64_t interpreteNtpTimestamp(unsigned char* pData);

			/// for Pattern V: If timestamp is not provided with the value(s), we calulate the time
			void incrementSyncSignalTime(unsigned int valueCount);

			int setDataFormat(const Json::Value& params);

			enum pattern_t {
				/// "V"; No timestamps, values only. Signal rate is recieved first.
				/// Time stamp of first value is received as meat information before value.
				/// Increment with delta time from signal rate for each value.
				PATTERN_V,
				/// "TV"; One timestamp per value, first comes the timestamp, then the value.
				/// This pattern is used for asynchonous values.
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

			enum timeType_t {
				TIMETYPE_NTP
			};

			std::string m_signalReference;

			/// For synchronuous signals (Pattern V): will be set to the time of the first measured value and will be incremented with each value received.
			timeInfo m_syncSignalTime;

			unsigned int m_signalRateSamples;
			/// For synchronuous signals (Pattern V): The time between m_signalRateSamples samples. Use this to omit rounding errors.
			timeInfo m_signalRateSamplesDelta;

			/// using this may cause rounding errors.
			uint64_t m_signalRateDelta;

			pattern_t m_dataFormatPattern;
			bool m_dataIsBigEndian;
			dataType_t m_dataValueType;
			size_t m_dataValueSize;
			timeType_t m_dataTimeType;
			size_t m_dataTimeSize;

			static const size_t m_valueBufferMaxValues = 128;

			double m_valueBuffer[m_valueBufferMaxValues];

			uint32_t m_subFraction;
			uint64_t m_subFractionCollected;
//			unsigned int m_syncSignalCorrectionCycle;
//			unsigned int m_syncSignalCorrectionCount;
		};
	}
}
#endif
