#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>

#include "Controller.h"
#include "HttpPost.h"

namespace hbm {
	namespace streaming {

		unsigned int Controller::s_id = 0;

		Controller::Controller(const std::string& streamId, const std::string& address, const std::string& port)
			: m_streamId(streamId)
			, m_address(address)
			, m_port(port)
		{
		}

		int Controller::subscribe(const signalReferences_t &signalReferences)
		{
			Json::Value content;
			content["jsonrpc"] = "2.0";
			content["method"] = m_streamId +".subscribe";
			for(signalReferences_t::const_iterator iter = signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
				content["params"].append(*iter);
			}
			content["id"] = ++s_id;

			std::string request = Json::FastWriter().write(content);

			HttpPost httpPost(m_address, m_port, "rpc");
			std::string response = httpPost.execute(request);

			Json::Value result;
			if(Json::Reader().parse(response, result)==false) {
				return -1;
			}
			if(result.isMember("error")) {
				return -1;
			}

			return 0;
		}

		int Controller::unsubscribe(const signalReferences_t &signalReferences)
		{
			Json::Value content;
			content["jsonrpc"] = "2.0";
			content["method"] = m_streamId +".unsubscribe";
			for(signalReferences_t::const_iterator iter = signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
				content["params"].append(*iter);
			}
			content["id"] = ++s_id;

			std::string request = Json::FastWriter().write(content);

			HttpPost httpPost(m_address, m_port, "rpc");
			std::string response = httpPost.execute(request);

			Json::Value result;
			if(Json::Reader().parse(response, result)==false) {
				return -1;
			}
			if(result.isMember("error")) {
				return -1;
			}

			return 0;
		}

	}
}

