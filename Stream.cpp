#include <cstdlib>
#include <vector>

#include "SocketNonblocking.h"
#include "TransportHeader.h"

static const std::string STREAM_DATA_PORT = "7411";


int main(int argc, char* argv[])
{
	std::string address = "hbm-00087b";
	hbm::SocketNonblocking socket;
	socket.connect(address.c_str(), STREAM_DATA_PORT);

	hbm::streaming::TransportHeader transportHeader(socket);
	int type = transportHeader.type();
	size_t size = transportHeader.size();

	if(type==hbm::streaming::TYPE_DATA) {

	} else if(type==hbm::streaming::TYPE_META){
		std::vector < char > data(size);
		socket.receiveComplete(&data[0], size);
		std::cout << std::string(&data[0], size) << std::endl;
	}

	return EXIT_SUCCESS;
}
