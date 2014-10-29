#ifndef _HBM__STREAMING__DELTATIMEINFO
#define _HBM__STREAMING__DELTATIMEINFO

#include <stdint.h>
#include <json/value.h>
#include "timeinfo.h"
#include "stdexcept"

namespace hbm {
	namespace streaming {
		class deltaTimeInfo: public timeInfo {
		public:
			deltaTimeInfo();

			/// \throws std::runtime_error if provided JSON is not valid
			void setSignalRate(const Json::Value& params);
			void increment(unsigned int count);
		private:
			uint64_t m_deltaNtpTimestamp;
			uint64_t m_deltaSubFraction;
			uint64_t m_deltaSubFractionCollected;
		};
	}
}
#endif
