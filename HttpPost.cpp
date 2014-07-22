#include <sstream>
#include <cstring>

#include "HttpPost.h"

#include "SocketNonblocking.h"

namespace hbm {
	namespace streaming {
		HttpPost::HttpPost(const std::string &address, const std::string &port, const std::string &httpPath)
			: m_address(address)
			, m_port(port)
			, m_httpPath(httpPath)
		{
		}

		std::string HttpPost::execute(const std::string& request)
		{
			std::stringstream message;
			message << "POST /" << m_httpPath << " HTTP/1.1\r\n";
			message << "Host: " << m_address << "\r\n";
			message << "Accept: */*\r\n";
			message << "Content-Type: application/json; charset=utf-8" << "\r\n";
			message << "Content-Length: " << request.length() << "\r\n";
			message << "\r\n";
			message << request;

			SocketNonblocking socket;
			socket.connect(m_address, m_port);

			if(socket.sendBlock(message.str().c_str(), message.str().length(), 0)<=0) {
				return "";
			}

			char recvBuffer[1024];
			ssize_t retVal = socket.receiveComplete(recvBuffer, sizeof(recvBuffer));
			if(retVal>0) {
				const char NEEDLE[] = "\r\n\r\n";

				char *pPos = strstr(recvBuffer, NEEDLE);
				if(pPos==NULL) {
					return "";
				}
				pPos += sizeof(NEEDLE)-1;
				return pPos;
			}
			return "";
		}
	}
}
