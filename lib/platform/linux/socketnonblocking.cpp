#include <cstring>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h> //writev
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include <unistd.h>

#include <errno.h>
#include <syslog.h>
#include <poll.h>

#include "socketnonblocking.h"


/// Maximum time to wait for connecting
const time_t TIMEOUT_CONNECT_S = 5;


hbm::SocketNonblocking::SocketNonblocking()
	: m_fd(-1)
	, m_bufferedReader()
{
}

hbm::SocketNonblocking::SocketNonblocking(const std::string& fileName)
	: m_fd(-1)
	, m_bufferedReader(fileName)
{
}

hbm::SocketNonblocking::~SocketNonblocking()
{
	stop();
}

int hbm::SocketNonblocking::init()
{
	int retVal = 0;
	int opt = 1;

	m_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if(m_fd==-1) {
		retVal=-1;
	} else {
		// turn off Nagle algorithm
		setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&opt), sizeof(opt));

		opt = 12;
		// the interval between the last data packet sent (simple ACKs are not considered data) and the first keepalive probe;
		// after the connection is marked to need keepalive, this counter is not used any further
		setsockopt(m_fd, SOL_TCP, TCP_KEEPIDLE, reinterpret_cast<char*>(&opt), sizeof(opt));

		opt = 3;
		// the interval between subsequential keepalive probes, regardless of what the connection has exchanged in the meantime
		setsockopt(m_fd, SOL_TCP, TCP_KEEPINTVL, reinterpret_cast<char*>(&opt), sizeof(opt));

		opt = 2;
		// the number of unacknowledged probes to send before considering the connection dead and notifying the application layer
		setsockopt(m_fd, SOL_TCP, TCP_KEEPCNT, reinterpret_cast<char*>(&opt), sizeof(opt));

		opt = 1;
		setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&opt), sizeof(opt));
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
	if(err==-1) {
		if(errno == EINPROGRESS) {
			// success if errno equals EINPROGRESS
			struct pollfd pfd;
			pfd.fd = m_fd;
			pfd.events = POLLOUT;
			do {
				err = poll(&pfd, 1, TIMEOUT_CONNECT_S*1000);
			} while((err==-1) && (errno==EINTR) );
			if(err==1) {
				int value;
				socklen_t len = sizeof(value);
				getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &value, &len);
				if(value!=0) {
					retVal = -1;
				}
			} else {
				retVal = -1;
			}
		} else {
			retVal = -1;
		}
	}
	return retVal;
}

ssize_t hbm::SocketNonblocking::receive(void* pBlock, size_t size)
{
	return m_bufferedReader.recv(m_fd, pBlock, size, 0);
}

ssize_t hbm::SocketNonblocking::receiveComplete(void* pBlock, size_t size)
{
	ssize_t retVal;
	unsigned char* pPos = reinterpret_cast < unsigned char* > (pBlock);
	size_t sizeLeft = size;
	while(sizeLeft) {
		retVal = m_bufferedReader.recv(m_fd, pPos, sizeLeft, 0);
		if(retVal>0) {
			sizeLeft -= retVal;
			pPos += retVal;
		} else if(retVal==0) {
			return size-sizeLeft;
		} else {
			if(errno==EWOULDBLOCK || errno==EAGAIN) {
				// wait for socket to become readable.
				struct pollfd pfd;
				pfd.fd = m_fd;
				pfd.events = POLLIN;
				int nfds;
				do {
					nfds = poll(&pfd, 1, -1);
				} while((nfds==-1) && (errno==EINTR));
				if(nfds!=1) {
					return -1;
				}
			} else {
				return -1;
			}
		}
	}
	return size;
}


ssize_t hbm::SocketNonblocking::sendBlock(const void* pBlock, size_t size, bool more)
{
	const uint8_t* pDat = reinterpret_cast<const uint8_t*>(pBlock);
	size_t BytesLeft = size;
	ssize_t numBytes;
	ssize_t retVal = size;

	struct pollfd pfd;
	pfd.fd = m_fd;
	pfd.events = POLLOUT;

	int flags = 0;
	if(more) {
		flags |= MSG_MORE;
	}
	int err;

	while (BytesLeft > 0) {
		numBytes = send(m_fd, pDat, BytesLeft, flags);
		if(numBytes>0) {
			pDat += numBytes;
			BytesLeft -= numBytes;
		} else if(numBytes==0) {
			// connection lost...
			BytesLeft = 0;
			retVal = -1;
		} else {
			// <0
			if(errno==EWOULDBLOCK || errno==EAGAIN) {
				// wait for socket to become writable.
				do {
					err = poll(&pfd, 1, -1);
				} while((err==-1) && (errno==EINTR));
				if(err!=1) {
					BytesLeft = 0;
					retVal = -1;
				}
			} else {
				// a real error happened!
				BytesLeft = 0;
				retVal = -1;
			}
		}
	}
	return retVal;
}


void hbm::SocketNonblocking::stop()
{
	::shutdown(m_fd, SHUT_RDWR);
	::close(m_fd);
	m_fd = -1;
}
