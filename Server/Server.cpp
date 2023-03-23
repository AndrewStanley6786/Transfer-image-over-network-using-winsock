#include <windows.networking.sockets.h>
#include <iostream>
#include <string>
#include <fstream>
#include "../Client/PktDef.h"
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	//create server socket
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//binds socket to address
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	//listen on a socket
	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	cout << "Waiting for client connection\n" << endl;

	//accepts a connection from a client
	SOCKET ConnectionSocket;
	ConnectionSocket = SOCKET_ERROR;
	if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	cout << "Connection Established" << endl;

	char* TxBuffer;
	char RxBuffer[1024];
	bool terminateConnection = false;		// Will check for if client has sent finish or error flag

	ofstream ofs;
	ofs.open("image.jpeg", ios::binary);
	while (!terminateConnection)
	{
		recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0);
		PktDef newPacket(RxBuffer);

		if (!newPacket.getFinishFlag() && !newPacket.getErrorFlag())		// If standard packet, write data to file
		{
			ofs.write(newPacket.getAddressOfData(), newPacket.getLength());
			int size;
			newPacket.setAckFlag(true);
			newPacket.swapDestination();
			newPacket.setData(NULL, 0);
			TxBuffer = newPacket.SerializeData(size);

			send(ConnectionSocket, TxBuffer, size, 0);		// Respond with acknowledgement packet
		}
		else
		{
			int size;
			newPacket.setAckFlag(true);
			newPacket.swapDestination();
			newPacket.setData(NULL, 0);
			TxBuffer = newPacket.SerializeData(size);

			send(ConnectionSocket, TxBuffer, size, 0);

			terminateConnection = true;		// Terminate upon error or finish flag
		}

	}

	ofs.close();
	
	closesocket(ConnectionSocket);	//closes incoming socket
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources

	//This code has been added to simply keep the console window open until you
	//type a character.
	int garbage;
	cin >> garbage;

	return 1;
}