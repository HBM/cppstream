#ifndef METAINFORMATION_H
#define METAINFORMATION_H

#include <vector>

#include <stdint.h>

#ifdef _WIN32
#include "jsoncpp/include/json/value.h"
#else
#include "jsoncpp/json/value.h"
#endif

#include "SocketNonblocking.h"
namespace hbm {
	namespace streaming {
		static const int METAINFORMATION_JSON = 1;

		/// several types of meta information are defined. Type 1 means any meta data encoded using JSON.
		/// meta information is received from the socket. Its content can be retrieved as JSON or in binary form depending on the type
		class MetaInformation
		{
		public:
			typedef std::vector < unsigned char > binaryContent_t;

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
			int m_type;
			Json::Value m_jsonContent;
			binaryContent_t m_binaryContent;
		};
	}
}

#endif // METAINFORMATION_H
