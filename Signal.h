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
			Json::Value data;
		private:
			size_t m_dataByteCount;
		};
	}
}
#endif
