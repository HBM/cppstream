#ifndef _HBM__STREMAING__HTTPPOST_H
#define _HBM__STREMAING__HTTPPOST_H

#include <string>

namespace hbm {
	namespace streaming {
		/// creates and send a HTTP post. Returns the response without HTTP post response header
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
#endif // _HBM__STREMAING__HTTPPOST_H
