#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#ifdef _WIN32
#ifndef ssize_t
#define ssize_t int
#endif
#else
#include <sys/types.h>
#endif

namespace hbm {
	/// \warning not reentrant
	class BufferedReader
	{
	public:
		BufferedReader();

		/// behaves like ::recv
		ssize_t recv(int sockfd, void *buf, size_t len, int flags);

	private:
		BufferedReader(const BufferedReader& op);
		BufferedReader& operator=(const BufferedReader& op);

		unsigned char m_buffer[65536*4];
		size_t m_fillLevel;
		size_t m_alreadyRead;
	};
}


#endif // BUFFEREDREADER_H
