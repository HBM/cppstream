#ifndef HTTPPOST_H
#define HTTPPOST_H

#include <string>

namespace hbm {
	namespace streaming {
		class HttpPost
		{
		public:
			HttpPost(const std::string& address, const std::string& port, const std::string& httpPath);

			std::string execute(const std::string& request);

		private:
			std::string m_address;
			std::string m_port;
			std::string m_httpPath;
			std::string m_httpVersion;
		};
	}
}

#endif // HTTPPOST_H
