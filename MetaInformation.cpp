#ifdef _WIN32
#include <WinSock2.h>
#include "jsoncpp/include/json/reader.h"
#else
#include <arpa/inet.h>
#include <jsoncpp/json/reader.h>
#endif

#include "MetaInformation.h"

namespace hbm {
	namespace streaming {

		static const int METAINFORMATION_JSON = 1;

		MetaInformation::MetaInformation(SocketNonblocking &socket, size_t size)
		{
			uint32_t typeBig;
			socket.receiveComplete(&typeBig, sizeof(typeBig));

			m_type = ntohl(typeBig);
			if(m_type==METAINFORMATION_JSON) {
				size_t dataSize = size-sizeof(m_type);
				std::vector < char > data(dataSize);
				socket.receiveComplete(&data[0], data.size());
				data.push_back('\0');

				Json::Reader().parse(&data[0], &data[dataSize], m_jsonContent);
			} else {
				m_binaryContent.resize(size-sizeof(m_type));
				socket.receive(&m_binaryContent[0], m_binaryContent.size());			}

		}
	}
}
