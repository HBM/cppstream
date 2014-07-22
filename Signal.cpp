#include "Signal.h"

namespace hbm {
	namespace streaming {
		Signal::Signal()
			: m_dataByteCount(0)
		{
		}

		void Signal::dataCb(unsigned char* pData, size_t size)
		{
			m_dataByteCount += size;
		}
	}
}
