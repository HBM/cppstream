#ifndef _HBM__STREAMING__TIMEINFO
#define _HBM__STREAMING__TIMEINFO

#include <stdint.h>
#include <vector>
#include <json/value.h>

namespace hbm {
	namespace streaming {
		class timeInfo {
		public:
			timeInfo();
			timeInfo(uint64_t ntpTimeStamp);
			uint64_t ntpTimeStamp() const;
			uint32_t seconds() const;
			uint32_t fractions() const;
			void set(const Json::Value& StampNode);
			void setNtpTimestamp(uint64_t ntpTimeStamp);
			void clear();
			void increment(const timeInfo& op);

		private:
			/// the upper 32 byte are seconds
			/// the lower 32 bytes are fractions of seconds (0.5, 0.25, 0.125...)
			uint64_t m_ntpTimestamp;
		};
	}
}
#endif
