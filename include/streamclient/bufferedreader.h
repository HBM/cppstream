// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _HBM__BUFFEREDREADER_H
#define _HBM__BUFFEREDREADER_H

#include <fstream>
#include <stdexcept>
#include <string>

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

		/// behaves like ::recv
		ssize_t recv(int sockfd, void *buf, size_t len);

	private:
		BufferedReader(const BufferedReader& op);
		BufferedReader& operator=(const BufferedReader& op);

		unsigned char m_buffer[65536*4];
		size_t m_fillLevel;
		size_t m_alreadyRead;
	};
}
#endif // BUFFEREDREADER_H
