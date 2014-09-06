#ifndef __HBM__SOCKETNONBLOCKING_H
#define __HBM__SOCKETNONBLOCKING_H

#include <string>


#ifdef _WIN32
#include <WinSock2.h>
#undef max
#undef min
#endif



#include "bufferedreader.h"

namespace hbm
{
	/// the socke uses keep-alive in order to detect broken connection.
	class SocketNonblocking
	{
	public:
		SocketNonblocking();

		/// use this variant to dump everything to a file
		/// \throw std::runtime_error if file could not be opened
		SocketNonblocking(const std::string& fileName);
		virtual ~SocketNonblocking();

		/// \return 0: success; -1: error
		int connect(const std::string& address, const std::string& port);

		int sendBlock(const void* pBlock, size_t len, bool more);

		/// might return with less bytes the requested
		ssize_t receive(void* pBlock, size_t len);

		/// might return with less bytes the requested if connection is being closed before completion
		ssize_t receiveComplete(void* pBlock, size_t len);

		/// poll this fd to see whether there is something to be received or out of order;
		int getFd() const
		{
			return m_fd;
		}

		void stop();

	private:
		/// should not be copied
		SocketNonblocking(const SocketNonblocking& op);

		/// should not be assigned
		SocketNonblocking& operator= (const SocketNonblocking& op);

		int init();

		int m_fd;
#ifdef _WIN32
		WSAEVENT m_event;
#endif
		BufferedReader m_bufferedReader;
	};
}
#endif
