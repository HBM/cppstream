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

#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#ifdef _WIN32
#ifndef ssize_t
#define ssize_t int
#endif
#else
#include <sys/types.h>
#endif


/// \warning not reentrant
class BufferedReader
{
public:
	BufferedReader();

	/// behaves like ::recv
	ssize_t recv(int sockfd, void *buf, size_t len, int flags);

private:
	unsigned char m_buffer[65536*4];
	size_t m_fillLevel;
	size_t m_alreadyRead;
};

#endif // BUFFEREDREADER_H
