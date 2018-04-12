// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _HBM__STREAMING__TRANSPORTHEADER_H
#define _HBM__STREAMING__TRANSPORTHEADER_H

#include <stdint.h>

#include "socketnonblocking.h"

namespace hbm {
	namespace streaming {
		enum type_t {
			TYPE_UNKNOWN = 0,
			TYPE_DATA = 1,
			TYPE_META = 2
		};

		/// reads and interprets the header of the transport layer
		class TransportHeader {
		public:
			/// þaram socket receiving socket
			TransportHeader(SocketNonblocking& socket);

			/// at least 32 Bit are being read. Maybe 32 additional bit extended length information are being read.
			/// \return the number of bytes read or error
			ssize_t receive();

			/// of payload
			inline size_t size() const
			{
				return m_dataByteCount;
			}

			/// only data and meta information is defined and supported in the moment.
			inline type_t type() const
			{
				return m_type;
			}

			/// \return 0 tells, that this is stream related.
			inline unsigned int signalNumber() const
			{
				return m_signalNumber;
			}
		private:
			TransportHeader(const TransportHeader&);
			TransportHeader& operator= (const TransportHeader&);
			SocketNonblocking& m_socket;
			size_t m_dataByteCount;
			unsigned int m_signalNumber;
			type_t m_type;
		};
	}
}

#endif
