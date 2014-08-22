#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#include <WinSock2.h>
#undef max
#undef min
#else
#include <sys/socket.h>
#endif



#include <cstring>

#include <iostream>

#include "bufferedreader.h"

namespace hbm {
	BufferedReader::BufferedReader()
		: m_fillLevel(0)
		, m_alreadyRead(0)
		, m_dumpFile()
	{
	}

	BufferedReader::BufferedReader(const std::string& fileName)
		: m_fillLevel(0)
		, m_alreadyRead(0)
		, m_dumpFile(fileName, std::ios_base::binary)
	{
		if(!m_dumpFile) {
			throw std::runtime_error("could not open dump file");
		}
	}


	ssize_t BufferedReader::recv(int sockfd, void *buf, size_t desiredLen, int flags)
	{
		// check whether there is something left
		size_t bytesLeft = m_fillLevel - m_alreadyRead;

		if(bytesLeft>=desiredLen) {
			// there is more than desired
			memcpy(buf, m_buffer+m_alreadyRead, desiredLen);
			m_alreadyRead += desiredLen;
			return static_cast < ssize_t > (desiredLen);
		} else if(bytesLeft>0) {
			// return the rest which is less than desired (a short read)
			memcpy(buf, m_buffer+m_alreadyRead, bytesLeft);
			m_alreadyRead = m_fillLevel;
			return static_cast < ssize_t > (bytesLeft);
		}

		// try to read as much as possible into the empty buffer space
	#ifdef _WIN32
		ssize_t retVal = ::recv(sockfd, (char*)m_buffer, sizeof(m_buffer), flags);
	#else
		ssize_t retVal = ::recv(sockfd, m_buffer, sizeof(m_buffer), flags);
	#endif

		if(retVal<=0) {
			return retVal;
		}

		if(m_dumpFile) {
			m_dumpFile.write(reinterpret_cast < char* > (m_buffer), retVal);
		}
		m_fillLevel = retVal;

		// length to return is up to the desired length.
		// remember the number of bytes already read.
		if(m_fillLevel>=desiredLen) {
			m_alreadyRead = desiredLen;
		} else {
			m_alreadyRead = m_fillLevel;
		}
		memcpy(buf, m_buffer, m_alreadyRead);
		return static_cast < ssize_t > (m_alreadyRead);
	}
}
