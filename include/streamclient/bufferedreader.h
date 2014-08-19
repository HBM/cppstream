#ifndef _HBM__BUFFEREDREADER_H
#define _HBM__BUFFEREDREADER_H

#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#ifndef ssize_t
#define ssize_t int
#endif
#else
#include <sys/types.h>
#endif

namespace hbm {
	/// try to receive a big chunk even if only a small amount of data is requested.
	/// return the requested data and keep the remaining data.
	/// \warning not reentrant
	class BufferedReader
	{
	public:
		BufferedReader();

		/// use this variant to dump everything to a file
		/// \throw std::runtime_error
		BufferedReader(const std::string& fileName);

		/// behaves like ::recv
		ssize_t recv(int sockfd, void *buf, size_t len, int flags);

	private:
		BufferedReader(const BufferedReader& op);
		BufferedReader& operator=(const BufferedReader& op);

		unsigned char m_buffer[65536*4];
		size_t m_fillLevel;
		size_t m_alreadyRead;
		std::ofstream m_dumpFile;
	};
}
#endif // BUFFEREDREADER_H
