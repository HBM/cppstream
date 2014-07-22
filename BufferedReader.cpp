/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2007 Hottinger Baldwin Messtechnik GmbH
 * Im Tiefen See 45
 * 64293 Darmstadt
 * Germany
 * http://www.hbm.com
 * All rights reserved
 *
 * The copyright to the computer program(s) herein is the property of
 * Hottinger Baldwin Messtechnik GmbH (HBM), Germany. The program(s)
 * may be used and/or copied only with the written permission of HBM
 * or in accordance with the terms and conditions stipulated in the
 * agreement/contract under which the program(s) have been supplied.
 * This copyright notice must not be removed.
 *
 * This Software is licenced by the
 * "General supply and license conditions for software"
 * which is part of the standard terms and conditions of sale from HBM.
*/

#ifdef _WIN32
#include <WinSock2.h>
#undef max
#undef min
#else
#include <sys/socket.h>
#endif



#include <cstring>

#include <iostream>

#include "BufferedReader.h"

namespace hbm {
	BufferedReader::BufferedReader()
		: m_fillLevel(0)
		, m_alreadyRead(0)
	{
	}

	ssize_t BufferedReader::recv(int sockfd, void *buf, size_t desiredLen, int flags)
	{
		// check whether there is something left
		size_t bytesLeft = m_fillLevel - m_alreadyRead;

		if(bytesLeft>=desiredLen) {
			// there is more than desired
			memcpy(buf, m_buffer+m_alreadyRead, desiredLen);
			m_alreadyRead += desiredLen;
			return desiredLen;
		} else if(bytesLeft>0) {
			// return the rest which is less than desired (a short read)
			memcpy(buf, m_buffer+m_alreadyRead, bytesLeft);
			m_alreadyRead = m_fillLevel;
			return bytesLeft;
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
		m_fillLevel = retVal;

		// length to return is up to the desired length.
		// remember the number of bytes already read.
		if(m_fillLevel>=desiredLen) {
			m_alreadyRead = desiredLen;
		} else {
			m_alreadyRead = m_fillLevel;
		}
		memcpy(buf, m_buffer, m_alreadyRead);
		return m_alreadyRead;
	}
}
