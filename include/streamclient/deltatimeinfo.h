#ifndef _HBM__STREAMING__DELTATIMEINFO
#define _HBM__STREAMING__DELTATIMEINFO

#include <stdint.h>
#include <json/value.h>
#include "timeinfo.h"
#include "stdexcept"

namespace hbm {
	namespace streaming {
		class deltaTimeInfo {
		public:
			deltaTimeInfo();
			void setTime(const Json::Value& params);

			/// \throws std::runtime_error if provided JSON is not valid
			void setDelta(const Json::Value& params);
			uint64_t increment(unsigned int count);
			void clear();

			uint64_t ntpTimeStamp() const;
		private:
			uint64_t m_deltaNtpTimestamp;
			uint64_t m_deltaSubFraction;
			uint64_t m_deltaSubFractionCollected;

			timeInfo m_time;
		};
	}
}
#endif
