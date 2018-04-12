// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "httppost.h"

#include "socketnonblocking.h"

namespace hbm {
	HttpPost::HttpPost(const std::string &address, const std::string &port, const std::string &httpPath)
		: m_address(address)
		, m_port(port)
		, m_httpPath(httpPath)
	{
		if(m_address.empty()) {
			throw std::runtime_error("no stream server address provided");
		}
		if(m_port.empty()) {
			throw std::runtime_error("no stream server control port provided");
		}
		if(m_httpPath.empty()) {
			throw std::runtime_error("no stream server path id provided");
		}

	}

	std::string HttpPost::execute(const std::string& request)
	{
		std::stringstream message;
		message << "POST " << m_httpPath << " HTTP/1.0\r\n";
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
		// important HTTP 1.0 will return the response and close the socket afterwards. This does look like a short read.
		ssize_t retVal = socket.receiveComplete(recvBuffer, sizeof(recvBuffer));
		// ensure termination!
		recvBuffer[sizeof(recvBuffer)-1] = '\0';

		std::cout << recvBuffer << std::endl;
		if(retVal>0) {
			static const std::string NEEDLE("\r\n\r\n");

			std::string response(recvBuffer, retVal);
			// body with response is to be found after the first empty line
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
