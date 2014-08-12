#ifndef _HBM__STREMAING__CONTROLLER_H
#define _HBM__STREMAING__CONTROLLER_H

#include <string>
#include <vector>

#include <Types.h>

namespace hbm {
	namespace streaming {
		/// used to send commands to the streaming control port.
		class Controller
		{
		public:
			Controller(const std::string& streamId, const std::string& address, const std::string& port, const std::string &path);

			/// \param signalReferences several signals might be subscribed with one request to the control port.
			int subscribe(const signalReferences_t& signalReferences);

			/// \param signalReferences several signals might be unsubscribed with one request to the control port.
			int unsubscribe(const signalReferences_t& signalReferences);
		private:
			std::string m_streamId;
			std::string m_address;
			std::string m_port;
			std::string m_path;

			static unsigned int s_id;
		};
	}
}
#endif // _HBM__STREMAING__CONTROLLER_H
