#ifndef _HBM__STREAMING__TIMEINFO
#define _HBM__STREAMING__TIMEINFO

#include <stdint.h>
#include <json/value.h>

namespace hbm {
	namespace streaming {
		/// the complete time stamp consists of 32 era information, an 64 bit NTP timestamp (32 bits seconds, 32 bits fraction) and 32 bits sub fraction.
		class timeInfo {
		public:
			timeInfo();

			/// the upper 32 byte are seconds
			/// the lower 32 bytes are fraction of seconds (0.5, 0.25, 0.125...)
			uint64_t ntpTimeStamp() const;
			uint32_t era() const;
			uint32_t seconds() const;
			uint32_t fraction() const;
			uint32_t subFraction() const;

			/// \throws std::runtime_error if provided JSON is not valid
			void set(const Json::Value& StampNode);
			uint64_t add(uint64_t inc);
			void clear();

		private:
			/// may not be copied
			timeInfo(const timeInfo&);
			/// may not be assigned
			timeInfo& operator=(const timeInfo&);

			uint32_t m_era;
			uint64_t m_ntpTimestamp;
			uint32_t m_subFraction;
		};
	}
}
#endif
