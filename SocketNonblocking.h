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


#ifndef __HBM__JET__SOCKETASYNC_H
#define __HBM__JET__SOCKETASYNC_H

#include <string>


#ifdef _WIN32
#include <WinSock2.h>
#undef max
#undef min
#endif



#include "BufferedReader.h"

namespace hbm
{
	class SocketNonblocking
	{
	public:
		SocketNonblocking();
		virtual ~SocketNonblocking();

		/// \return 0: success; -1: error
		int connect(const std::string& address, const std::string& port);

		int sendBlock(const void* pBlock, size_t len, bool more);

		/// might return with less bytes the requested
		ssize_t receive(void* pBlock, size_t len);

		ssize_t receiveComplete(void* pBlock, size_t len);

		/// poll this fd to see whether there is something to be received or out of order;
		int getFd() const
		{
			return m_fd;
		}

#ifdef _WIN32
		WSAEVENT getEvent() const
		{
			return m_event;
		}
#endif
		void stop();

	private:
		int init();


		int m_fd;
#ifdef _WIN32
		WSAEVENT m_event;
#endif
		BufferedReader m_bufferedReader;
	};
}
#endif
