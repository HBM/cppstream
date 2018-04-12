// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _HBM__STREAMING__METAINFORMATION_H
#define _HBM__STREAMING__METAINFORMATION_H

#include <vector>

#include <stdint.h>

#include <json/value.h>

#include "socketnonblocking.h"
namespace hbm {
	namespace streaming {
		static const int METAINFORMATION_JSON = 1;

		/// several types of meta information are defined. Type 1 means any meta data encoded using JSON.
		/// meta information is received from the socket. Its content can be retrieved as JSON or in binary form depending on the type
		class MetaInformation
		{
		public:
			typedef std::vector < unsigned char > binaryContent_t;

			/// \param size Size of the meta information
			MetaInformation(hbm::SocketNonblocking& socket, size_t size);


			/// \return meta data encoded in JSON; empty if this is meta data in binary form or contains invalid JSON
			/// \code
			/// {
			///   ”method”: < the type of meta data >,
			///   ”params”: < value >
			/// }
			/// \endcode
			Json::Value jsonContent() const
			{
				return m_jsonContent;
			}

			/// \return meta data in binray form; empty if this is meta data encoded in JSON
			binaryContent_t binaryContent() const
			{
				return m_binaryContent;
			}

			int type() const
			{
				return m_type;
			}

		private:
			MetaInformation(const MetaInformation&);
			MetaInformation& operator= (const MetaInformation&);

			int m_type;
			Json::Value m_jsonContent;
			binaryContent_t m_binaryContent;
		};
	}
}

#endif // METAINFORMATION_H
