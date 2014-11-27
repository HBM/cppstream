// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#ifndef _HBM__ENDIANESS_H
#define _HBM__ENDIANESS_H

#ifdef _WIN32
#include <cstdlib>
#include <WinSock2.h>

#define be64toh(x) _byteswap_uint64(x)
#define be32toh(x) _byteswap_ulong(x)
#define le64toh(x) (x)
#define le32toh(x) (x)

#else
#include <arpa/inet.h>
#include <endian.h>
#endif

#endif
