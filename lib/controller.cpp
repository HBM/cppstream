#include <sstream>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include "controller.h"
#include "httppost.h"
#include "types.h"

namespace hbm {
	namespace streaming {

		static const char ERROR[] = "error";
		static const char JSONRPC[] = "jsonrpc";

		unsigned int Controller::s_id = 0;

		Controller::Controller(const std::string& streamId, const std::string& address, const std::string& port, const std::string& path)
			: m_streamId(streamId)
			, m_httpPost(address, port, path)
		{
			if(m_streamId.empty()) {
				throw std::runtime_error("no stream id provided");
			}
		}

		void Controller::subscribe(const signalReferences_t &signalReferences)
		{
			if(signalReferences.empty()) {
				return;
			}
			Json::Value content;
			content[JSONRPC] = "2.0";
			content[METHOD] = m_streamId +".subscribe";
			for(signalReferences_t::const_iterator iter = signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
				content[PARAMS].append(*iter);
			}
			content["id"] = ++s_id;

			std::string request = Json::FastWriter().write(content);

			std::string response = m_httpPost.execute(request);

			Json::Value result;
			if(Json::Reader().parse(response, result)==false) {
				throw std::runtime_error("http response is not valid JSON");
			}
			if(result.isMember(ERROR)) {
				std::ostringstream msg;

				msg << "http response contains error: code=" << result[ERROR]["code"].asInt() << " '" << result[ERROR]["message"].asString() << "'";

				throw std::runtime_error(msg.str());
			}
		}

		void Controller::unsubscribe(const signalReferences_t &signalReferences)
		{
			if(signalReferences.empty()) {
				return;
			}
			Json::Value content;
			content[JSONRPC] = "2.0";
			content[METHOD] = m_streamId +".unsubscribe";
			for(signalReferences_t::const_iterator iter = signalReferences.begin(); iter!=signalReferences.end(); ++iter) {
				content[PARAMS].append(*iter);
			}
			content["id"] = ++s_id;

			std::string request = Json::FastWriter().write(content);

			std::string response = m_httpPost.execute(request);

			Json::Value result;
			if(Json::Reader().parse(response, result)==false) {
				throw std::runtime_error("http response is not valid JSON");
			}
			if(result.isMember(ERROR)) {
				std::ostringstream msg;

				msg << "http response contains error: code=" << result[ERROR]["code"].asInt() << " '" << result[ERROR]["message"].asString() << "'";

				throw std::runtime_error(msg.str());
			}
		}

	}
}

