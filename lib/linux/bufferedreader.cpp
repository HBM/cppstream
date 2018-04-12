// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided


#include <sys/socket.h>
#include <sys/uio.h>

#include <cstring>

#include "bufferedreader.h"

namespace hbm {
	BufferedReader::BufferedReader()
		: m_fillLevel(0)
		, m_alreadyRead(0)
	{
	}

	ssize_t BufferedReader::recv(int sockfd, void *buf, size_t desiredLen)
	{
		// check whether there is something left
		size_t bytesLeft = m_fillLevel - m_alreadyRead;

		if(bytesLeft>=desiredLen) {
			// there is more than or as much as desired
			memcpy(buf, m_buffer+m_alreadyRead, desiredLen);
			m_alreadyRead += desiredLen;
			return static_cast < ssize_t > (desiredLen);
		} else if(bytesLeft>0) {
			// return the rest which is less than desired (a short read)
			memcpy(buf, m_buffer+m_alreadyRead, bytesLeft);
			m_alreadyRead = m_fillLevel;
			return static_cast < ssize_t > (bytesLeft);
		}

		// try to read as much as possible into the provided buffer.In addition we fill our internal buffer if there is already more to read.
		// readv saves us from reading into the internal buffer first and copying into the provided buffer afterwards.
		struct iovec iov[2];
		iov[0].iov_base = buf;
		iov[0].iov_len = desiredLen;
		iov[1].iov_base = m_buffer;
		iov[1].iov_len = sizeof(m_buffer);

		ssize_t retVal = ::readv(sockfd, iov, 2);
		m_alreadyRead = 0;

		if (retVal>static_cast < ssize_t > (desiredLen)) {
			// readv returns the total number of bytes read
			m_fillLevel = retVal-desiredLen;
			return static_cast < ssize_t > (desiredLen);
		}
		m_fillLevel = 0;
		return retVal;
	}
}
