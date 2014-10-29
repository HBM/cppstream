#ifndef _HBM__STREAMING__DELTATIMEINFO
#define _HBM__STREAMING__DELTATIMEINFO

#include <stdint.h>
#include <json/value.h>
#include "timeinfo.h"
#include "stdexcept"

namespace hbm {
	namespace streaming {
		/// used for calculating the accumulated timestamp for measured values of synchronuous signals.
		class deltaTimeInfo {
		public:
			deltaTimeInfo();
			/// set accumulated timestamp
			void setTime(const Json::Value& params);

			/// set delta timestamp
			/// \throws std::runtime_error if provided JSON is not valid
			void setDelta(const Json::Value& params);

			/// increase era, seconds, fraction and sub fraction by delta timestamp * count
			/// \return the second and fraction part as NTP timestamp
			/// the upper 32 byte are seconds
			/// the lower 32 bytes are fraction of seconds (0.5, 0.25, 0.125...)
			uint64_t increment(unsigned int count);

			/// clear delta timestamp and accumulated timestamp
			void clear();

			/// \return the second and fraction part as NTP timestamp
			/// the upper 32 byte are seconds
			/// the lower 32 bytes are fraction of seconds (0.5, 0.25, 0.125...)
			uint64_t ntpTimeStamp() const;
		private:
			/// may not be copied
			deltaTimeInfo(const deltaTimeInfo&);
			/// may not be assigned
			deltaTimeInfo& operator=(const deltaTimeInfo&);

			uint64_t m_deltaNtpTimestamp;
			uint64_t m_deltaSubFraction;
			uint64_t m_deltaSubFractionCollected;

			timeInfo m_time;
		};
	}
}
#endif
