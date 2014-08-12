#ifndef _HBM__HTTPPOST_H
#define _HBM__HTTPPOST_H

#include <string>

namespace hbm {
	/// Creates and send a HTTP post. Returns the response without HTTP post response header.
	/// TCP Connection is opened with each execute() and is being closed afterwards.
	class HttpPost
	{
	public:
		HttpPost(const std::string& address, const std::string& port, const std::string& httpPath);

		/// \return the response or an empty string on error
		std::string execute(const std::string& request);

	private:
		std::string m_address;
		std::string m_port;
		std::string m_httpPath;
	};
}
#endif // _HBM__HTTPPOST_H
