#include <sstream>

#include "HttpPost.h"

#include "SocketNonblocking.h"

namespace hbm {
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
		message << "Connection: close\r\n";
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
		// ensure termination!
		recvBuffer[sizeof(recvBuffer)-1] = '\0';
		if(retVal>0) {
			static const std::string NEEDLE("\r\n\r\n");

			std::string response(recvBuffer, retVal);
			/// body with response is to be found after the first empty line
			size_t position = response.find(NEEDLE);
			if(position == std::string::npos) {
				return "";
			} else {
				return response.substr(position+NEEDLE.length());
			}
		}
		return "";
	}
}
