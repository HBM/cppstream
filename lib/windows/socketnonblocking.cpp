#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSTcpIP.h>
#undef max
#undef min

#define __time_t long
#define ssize_t int
#define snprintf sprintf_s



#include "SocketNonblocking.h"


/// Maximum time to wait for connecting
const time_t TIMEOUT_CONNECT_S = 5;


hbm::SocketNonblocking::SocketNonblocking()
	: m_fd(-1)
	, m_bufferedReader()
{
	WSADATA wsaData;
	WSAStartup(2, &wsaData);
}

hbm::SocketNonblocking::SocketNonblocking(const std::string& fileName)
	: m_fd(-1)
	, m_bufferedReader(fileName)
{
	WSADATA wsaData;
	WSAStartup(2, &wsaData);
}

hbm::SocketNonblocking::~SocketNonblocking()
{
	stop();
}

int hbm::SocketNonblocking::init()
{
	int retVal = 0;
	bool opt = true;

	m_fd = static_cast < int > (::socket(AF_INET, SOCK_STREAM, 0));
	if(m_fd==-1) {
		retVal=-1;
	} else {
		// switch to non blocking
		u_long value = 1;
		::ioctlsocket(m_fd, FIONBIO, &value);

		// turn off nagle algorithm
		setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&opt), sizeof(opt));


		// configure keep alive
		DWORD len;
		tcp_keepalive ka;
		ka.keepaliveinterval = 1000; // probe interval in ms
		ka.keepalivetime = 1000; // time of inactivity until first keep alive probe is being send in ms
		// from MSDN: on windows vista and later, the number of probes is set to 10 and can not be changed
		// time until recognition: keepaliveinterval + (keepalivetime*number of probes)
		ka.onoff = 1;
		WSAIoctl(m_fd, SIO_KEEPALIVE_VALS, &ka, sizeof(ka), NULL, 0, &len, NULL, NULL);
	}

	return retVal;
}

int hbm::SocketNonblocking::connect(const std::string &address, const std::string& port)
{
	int retVal = init();

	if(retVal<0) {
		return retVal;
	}


	struct addrinfo hints;
	struct addrinfo* pResult = NULL;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	if( getaddrinfo(address.c_str(), port.c_str(), &hints, &pResult)!=0 ) {
		return -1;
	}
	int err = ::connect(m_fd, pResult->ai_addr, sizeof(sockaddr_in));
	freeaddrinfo( pResult );
	// success if WSAGetLastError returns WSAEWOULDBLOCK
	if((err == SOCKET_ERROR) && (WSAGetLastError() == WSAEWOULDBLOCK))
	{

		fd_set fdWrite;

		struct timeval timeout;

		timeout.tv_sec = TIMEOUT_CONNECT_S;
		timeout.tv_usec = 0;


		FD_ZERO(&fdWrite);
		FD_SET(m_fd,&fdWrite);

		err = select(m_fd + 1, NULL, &fdWrite, NULL, &timeout);
		if(err==1) {
			int value;
			socklen_t len = sizeof(value);
			getsockopt(m_fd, SOL_SOCKET, SO_ERROR, reinterpret_cast < char* > (&value), &len);
			if(value!=0) {
				retVal = -1;
			}
		} else {
			retVal = -1;
		}
	} else {
		retVal = -1;
	}

	return retVal;
}


ssize_t hbm::SocketNonblocking::receive(void* pBlock, size_t size)
{
  return m_bufferedReader.recv(m_fd, pBlock, size, 0);
}

ssize_t hbm::SocketNonblocking::receiveComplete(void* pBlock, size_t len)
{
  size_t DataToGet = len;
  unsigned char* pDat = static_cast<unsigned char*>(pBlock);
  ssize_t numBytes = 0;

  fd_set recvFds;

  FD_ZERO(&recvFds);
  FD_SET(m_fd,&recvFds);
  int err;


  while (DataToGet > 0) {
    numBytes = m_bufferedReader.recv(m_fd, reinterpret_cast<char*>(pDat), static_cast < int >(DataToGet), 0);
    if(numBytes>0) {
      pDat += numBytes;
      DataToGet -= numBytes;
    } else if(numBytes==0) {
      // the peer has performed an orderly shutdown!
      DataToGet = 0;
      return -1;
    } else {
      if(WSAGetLastError()==WSAEWOULDBLOCK) {
	// wait until there is something to read or somthing bad happened
	do {
	  err = select(static_cast < int >(m_fd) + 1, &recvFds, NULL, NULL, NULL);
	} while((err==-1) && (WSAGetLastError()!=WSAEINTR));
	if(err!=1) {
	  return -1;
	}
      } else {
	return -1;
      }
    }
  }
  return static_cast < ssize_t > (len);
}


int hbm::SocketNonblocking::sendBlock(const void* pBlock, size_t size, bool more)
{
	const uint8_t* pDat = reinterpret_cast<const uint8_t*>(pBlock);
	size_t BytesLeft = size;
	int numBytes;
	int retVal = 0;

	fd_set recvFds;

	FD_ZERO(&recvFds);
	FD_SET(m_fd,&recvFds);
	int err;

	while (BytesLeft > 0) {
		// wir warten ohne timeout, bis wir schreiben koennen
		err = select(static_cast < int >(m_fd) + 1, NULL, &recvFds, NULL, NULL);

		if(err==1) {
			if(FD_ISSET(m_fd, &recvFds)) {
				numBytes = send(m_fd, reinterpret_cast < const char* >(pDat), static_cast < int >(BytesLeft), 0);
				if (numBytes > 0) {
					pDat += numBytes;
					BytesLeft -= numBytes;
				} else if(numBytes==0){
					BytesLeft = 0;
					retVal = -1;
				} else {
					// -1: error
					// ignore "would block"
					if(WSAGetLastError()!=WSAEWOULDBLOCK) {
						BytesLeft = 0;
						retVal = -1;
					}
				}
			}
		} else if(err==-1) {
			// ignore EINTR
			if( WSAGetLastError()!=WSAEINTR) {
				retVal = -1;
				break;
			}
		} else {
			// 0: timeout, should not happen here...
			retVal = -1;
			break;
		}
	}
	return retVal;
}


void hbm::SocketNonblocking::stop()
{
	::shutdown(m_fd, SD_BOTH);
	::closesocket(m_fd);
	m_fd = -1;
}
