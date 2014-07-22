#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
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
				std::vector < char > data(size-sizeof(m_type));
				socket.receive(&data[0], data.size());

				Json::Reader().parse(&data[0], &data[data.size()], m_jsonContent);
			} else {
				m_binaryContent.resize(size-sizeof(m_type));
				socket.receive(&m_binaryContent[0], m_binaryContent.size());			}

		}
	}
}
