#include "ServerNetwork.h"

ServerNetwork::ServerNetwork(std::string ipAdresses, int portNumber) {
	netIP = ipAdresses;
	netPort = portNumber;

	FD_ZERO(&master);
}

ServerNetwork::~ServerNetwork() {
	CleanUp();
}

void ServerNetwork::CleanUp() {
	for (int i = 0; i < master.fd_count; i++) {
		SOCKET sockClose = master.fd_array[i];

		FD_CLR(sockClose, &master);
		closesocket(sockClose);
	}

	WSACleanup();
}

bool ServerNetwork::EstablishWinSock() {
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);

	int wsOK = WSAStartup(version, &wsData);

	return wsOK == 0;
}

void ServerNetwork::RunServer() {
	char buffer[4096];

	while (true) {
		SOCKET listening = CreateSocket();

		if (listening == INVALID_SOCKET) {
			std::cout << "Error: Listening Socket Invalid." << std::endl;
			break;
		}

		FD_SET(listening, &master);
		
		std::cout << "Server Now Online, Awaiting Connections..." << std::endl;

		while (true) {
			fd_set copy = master;

			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

			for (int i = 0; i < socketCount; i++) {
				SOCKET sock = copy.fd_array[i];

				if (sock == listening) {
					SOCKET client = accept(sock, nullptr, nullptr);

					FD_SET(client, &master);

					std::cout << "New Client #" << client << " Has Connected " << master.fd_count-1 << " Online." << std::endl;

					std::string welcomeMessage = "Connection Successful\r\n";
					send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);
				}
				else {
					ZeroMemory(buffer, 4096);

					int bytesRecived = recv(sock, buffer, 4096, 0);

					if (bytesRecived <= 0) {
						std::cout << "Client #" << sock << " Has Disconnected " << master.fd_count-2 << " Online." << std::endl;

						FD_CLR(sock, &master);
						closesocket(sock);
					}
					else {
						for (int i = 0; i < master.fd_count; i++) {
							SOCKET outSock = master.fd_array[i];

							if (outSock != listening && outSock != sock) {
								std::ostringstream ss;
								ss << "Socket: " << outSock << ":" << buffer;
								std::string strOut = ss.str();

								send(outSock, strOut.c_str(), strOut.size() + 1, 0);
							}
						}
					}
				}
			}
		}
	}
}

SOCKET ServerNetwork::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);

	if (listening != INVALID_SOCKET) {
		sockaddr_in hint;

		hint.sin_family = AF_INET;
		hint.sin_port = htons(netPort);
		inet_pton(AF_INET, netIP.c_str(), &hint.sin_addr);

		if (bind(listening, (sockaddr*)&hint, sizeof(hint)) != SOCKET_ERROR) {
			if (listen(listening, SOMAXCONN) != SOCKET_ERROR) {
				return listening;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}

	return -1;
}