#include <sstream>
#include <cstdio>

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include "SocketNonblocking.h"

#include "Controller.h"

namespace hbm {
	namespace streaming {

		unsigned int Controller::s_id = 0;

		Controller::Controller(const std::string& streamId, const std::string& address, const std::string& port, const std::string& httpPath, const std::string& httpVersion)
			: m_streamId(streamId)
			, m_address(address)
			, m_port(port)
			, m_httpPath(httpPath)
			, m_httpVersion(httpVersion)
		{
		}

		int Controller::subscribe(const std::string& signalReference)
		{
			Json::Value content;
			content["jsonrpc"] = "2.0";
			content["method"] = m_streamId +".subscribe";
			content["params"].append(signalReference);
			content["id"] = ++ s_id;

			std::string contentString = Json::FastWriter().write(content);


			SocketNonblocking socket;
			socket.connect(m_address, m_port);



			std::stringstream message;
			message << "POST /" << m_httpPath << " HTTP/" << m_httpVersion << "\r\n";
			message << "Host: " << m_address << "\r\n";
			message << "Accept: */*\r\n";
			message << "Content-Type: application/json; charset=utf-8" << "\r\n";
			message << "Content-Length: " << contentString.length() << "\r\n";
			message << "\r\n";
			message << contentString;

			std::cout << message.str() << std::endl;
			if(socket.sendBlock(message.str().c_str(), message.str().length(), 0)<=0) {
				return -1;
			}


			char recvBuffer[1024];
			ssize_t retVal = socket.receiveComplete(recvBuffer, sizeof(recvBuffer));
			if(retVal>0) {
				std::cout << recvBuffer << std::endl;

				Json::Value result;
				if(Json::Reader().parse(recvBuffer, recvBuffer+retVal, result)==false) {
					return -1;
				}
				if(result.isMember("error")) {
					return -1;
				}
			}



			return 0;
		}

		int Controller::unsubscribe(const std::string& signalReference)
		{
			return 0;
		}

	}
}

