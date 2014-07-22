#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>

namespace hbm {
	namespace streaming {
		class Controller
		{
		public:
			Controller(const std::string& streamId, const std::string& address, const std::string& port);

			int subscribe(const std::string& signalReference);

			int unsubscribe(const std::string& signalReference);
		private:
			std::string m_streamId;
			std::string m_address;
			std::string m_port;

			static unsigned int s_id;
		};
	}
}


#endif // CONTROLLER_H
