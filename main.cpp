#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "ServerNetwork.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	
	ServerNetwork server("127.0.0.1", 54000);

	if (server.EstablishWinSock()) {
		server.RunServer();
	}
	else {
		std::cout << "Error: Winsock Faliure." << std::endl;
	}

	std::cin.get();

	return 0;
}