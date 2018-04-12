// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _HBM__STREMAING__CONTROLLER_H
#define _HBM__STREMAING__CONTROLLER_H

#include <string>
#include <stdexcept>

#include "types.h"
#include "httppost.h"

namespace Json {
	class Value;
}

namespace hbm {
	namespace streaming {
		/// used to send commands to the streaming control http port.
		class Controller
		{
		public:
			/// \throws std::runtime_error
			Controller(const std::string& streamId, const std::string& address, const std::string& port, const std::string &path);

			/// \param signalReferences several signals might be subscribed with one request to the control port.
			/// \throws std::runtime_error
			void subscribe(const signalReferences_t& signalReferences);

			/// \param signalReferences several signals might be unsubscribed with one request to the control port.
			/// \throws std::runtime_error
			void unsubscribe(const signalReferences_t& signalReferences);
		private:
			Controller(const Controller&);
			Controller& operator= (const Controller&);

			/// \throws std::runtime_error
			void execute(const Json::Value& request);

			std::string m_streamId;
			HttpPost m_httpPost;

			static unsigned int s_id;
		};
	}
}
#endif // _HBM__STREMAING__CONTROLLER_H
