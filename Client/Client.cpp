#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <string>
#include <fstream>
#include "PktDef.h"
#define MAX_NUM_OF_BYTES 50
using namespace std;

int main()
{
	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address
	if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		return 0;
	}

	ifstream ifs;
	ifs.open("image.jpeg", ios::binary);

	char* buffer = new char[MAX_NUM_OF_BYTES];
	char RxBuffer[1024];
	int seqNum = 0;		
	const char* TxBuffer;

	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			ifs.read(buffer, MAX_NUM_OF_BYTES);

			PktDef newPacket;
			newPacket.setDestinationValue(12);
			newPacket.setSourceValue(2);
			newPacket.setSequenceNumber(seqNum);
			newPacket.setData(buffer, MAX_NUM_OF_BYTES);

			int size;
			TxBuffer = newPacket.SerializeData(size);

			send(ClientSocket, TxBuffer, size, 0);

			recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);

			PktDef responsePacket(RxBuffer);

			responsePacket.Print();
			seqNum++;
		}

		PktDef lastPacket;

		lastPacket.setDestinationValue(12);
		lastPacket.setSourceValue(2);
		lastPacket.setSequenceNumber(seqNum);
		lastPacket.setFinishFlag(true);
		lastPacket.setData(NULL, 0);

		int size;
		char* TxBuffer = lastPacket.SerializeData(size);
		send(ClientSocket, TxBuffer, size, 0);

		recv(ClientSocket, RxBuffer, sizeof(RxBuffer), 0);
		PktDef responsePacket(RxBuffer);

		responsePacket.Print();

		if (buffer)
			delete[] buffer;
	}
	else
	{
		PktDef packet;
		packet.setErrorFlag(true);
		packet.setData(NULL, 0);
		int size;
		char* TxBuffer = packet.SerializeData(size);
		send(ClientSocket, TxBuffer, size, 0);
	}
	
	ifs.close();

	//closes connection and socket
	closesocket(ClientSocket);

	//frees Winsock DLL resources
	WSACleanup();

	//This code has been added to simply keep the console window open until you
	//type a character.
	int garbage;
	cin >> garbage;

	return 1;
}
