#ifndef _HBM_STREAMING_TRANSPORTHEADER_H
#define _HBM_STREAMING_TRANSPORTHEADER_H

#include <stdint.h>

#include "SocketNonblocking.h"

namespace hbm {
	namespace streaming {
		enum type_t {
			TYPE_DATA = 1,
			TYPE_META = 2
		};
		/// at least 32 Bit. May have 32 additional Bit extended length information.
		class TransportHeader {
		public:
			TransportHeader(SocketNonblocking& socket);

			int receive();

			size_t size() const
			{
				return m_dataByteCount;
			}

			int type() const
			{
				return m_type;
			}

			unsigned int signalNumber() const
			{
				return m_signalNumber;
			}
		private:
			SocketNonblocking& m_socket;
			size_t m_dataByteCount;
			unsigned int m_signalNumber;
			int m_type;
		};
	}
}

#endif
