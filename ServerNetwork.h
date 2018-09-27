#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

class ServerNetwork {
	public:
		ServerNetwork(std::string ipAddress, int portNumber);
		~ServerNetwork();

		void CleanUp();
		void RunServer();

		bool EstablishWinSock();

	private:
		std::string netIP;
		int netPort;
		fd_set master;

		SOCKET CreateSocket();
};