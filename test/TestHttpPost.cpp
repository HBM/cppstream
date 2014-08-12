#include <iostream>

#include "HttpPost.h"

int main(int argc, char* argv[])
{
	if((argc<2) || (std::string(argv[1])=="-h") ) {
		std::cout << "syntax: " << argv[0] << " <http server address>" << std::endl;
		return EXIT_SUCCESS;
	}

	std::string address = argv[1];
	std::string port = "http";
	std::string path = "rpc";
	{
		hbm::HttpPost httpPost(address, port, path);
		std::cout << httpPost.execute("") << std::endl;
	}

	{
		// invalid port
		hbm::HttpPost httpPost(address, "4", path);
		std::cout << httpPost.execute("") << std::endl;
	}

	{
		// invalid path
		hbm::HttpPost httpPost(address, port, "bad");
		std::cout << httpPost.execute("") << std::endl;
	}

}
