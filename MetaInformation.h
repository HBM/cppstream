#ifndef METAINFORMATION_H
#define METAINFORMATION_H

#include <vector>

#include <stdint.h>

#include "jsoncpp/json/value.h"

#include "SocketNonblocking.h"
namespace hbm {
	namespace streaming {
		class MetaInformation
		{
		public:
			typedef std::vector < unsigned char > binaryContent_t;

			MetaInformation(hbm::SocketNonblocking& socket, size_t size);

			Json::Value jsonContent()
			{
				return m_jsonContent;
			}

			binaryContent_t binaryContent()
			{
				return m_binaryContent;
			}

		private:
			uint32_t m_type;
			Json::Value m_jsonContent;
			binaryContent_t m_binaryContent;
			//std::string m_apiVersion;
			//std::string m_streamId;
		};
	}
}

#endif // METAINFORMATION_H
