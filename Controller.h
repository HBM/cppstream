#ifndef _HBM__STREMAING__CONTROLLER_H
#define _HBM__STREMAING__CONTROLLER_H

#include <string>
#include <vector>

namespace hbm {
	namespace streaming {
		typedef std::vector < std::string > signalReferences_t;
		/// used to send commands to the streaming control port.
		class Controller
		{
		public:

			Controller(const std::string& streamId, const std::string& address, const std::string& port);

			int subscribe(const signalReferences_t& signalReferences);

			int unsubscribe(const signalReferences_t& signalReferences);
		private:
			std::string m_streamId;
			std::string m_address;
			std::string m_port;

			static unsigned int s_id;
		};
	}
}
#endif // _HBM__STREMAING__CONTROLLER_H
