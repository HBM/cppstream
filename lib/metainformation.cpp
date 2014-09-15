#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#include "json/reader.h"
#else
#include <arpa/inet.h>
#include <jsoncpp/json/reader.h>
#endif

#include "metainformation.h"

namespace hbm {
	namespace streaming {

		MetaInformation::MetaInformation(SocketNonblocking &socket, size_t size)
			: m_type(0)
			, m_jsonContent()
			, m_binaryContent()
		{
			int32_t typeBig;

			if (size<=sizeof(typeBig)) {
				std::cerr << __FUNCTION__ << ": invalid size" <<std::endl;
				return;
			}

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
				socket.receiveComplete(&m_binaryContent[0], m_binaryContent.size());
			}
		}
	}
}
