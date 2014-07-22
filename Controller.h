#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <vector>

namespace hbm {
	namespace streaming {
		typedef std::vector < std::string > signalReferences_t;
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


#endif // CONTROLLER_H
