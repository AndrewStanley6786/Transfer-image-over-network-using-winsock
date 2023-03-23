#pragma once
#include <iostream>
using namespace std;	

class PktDef
{
	// ***** YOUR CODE GOES HERE ****** //
	struct Header		
	{
		unsigned char Destination;
		unsigned char Source;
		unsigned int sequenceNumber;
		bool ackFlag : 1;
		bool errorFlag : 1;
		bool finishFlag : 1;
		unsigned int bodyLength;
	};

	struct Packet
	{
		Header Head;
		char* Data;
		int CRC;
	} packet;

	char* pSerialBuff;

public:
	PktDef()
	{
		memset(&packet.Head, 0, sizeof(packet.Head));
		packet.Data = nullptr;
		int CRC = 0;
		pSerialBuff = nullptr;
	}

	PktDef(char* buffer)
	{
		int charSkip = 0;		// To skip characters in the buffer

		memcpy(&packet.Head.Destination, buffer, sizeof(packet.Head.Destination));
		charSkip += sizeof(packet.Head.Destination);

		memcpy(&packet.Head.Source, buffer + charSkip, sizeof(packet.Head.Source));
		charSkip += sizeof(packet.Head.Source);

		memcpy(&packet.Head.sequenceNumber, buffer + charSkip, sizeof(packet.Head.sequenceNumber));
		charSkip += sizeof(packet.Head.sequenceNumber);

		memcpy(&packet.Head.sequenceNumber + sizeof(char), buffer + charSkip, sizeof(char));
		charSkip += sizeof(char);

		memcpy(&packet.Head.bodyLength, buffer + charSkip, sizeof(packet.Head.bodyLength)); 
		charSkip += sizeof(packet.Head.bodyLength);

		packet.Data = new char[packet.Head.bodyLength];
		memcpy(packet.Data, buffer + charSkip, packet.Head.bodyLength);
		charSkip += packet.Head.bodyLength;

		memcpy(&packet.CRC, buffer + charSkip, sizeof(packet.CRC));
	}

	void setSourceValue(unsigned int source)
	{
		packet.Head.Source = (char)source;
	}

	void setDestinationValue(unsigned int dest)
	{
		packet.Head.Destination = char(dest);
	}

	void setAckFlag(bool ack)
	{
		packet.Head.ackFlag = ack;
	}

	void setFinishFlag(bool fin)
	{
		packet.Head.finishFlag = fin;
	}

	bool getFinishFlag()
	{
		return packet.Head.finishFlag;
	}

	void setErrorFlag(bool err)
	{
		packet.Head.errorFlag = err;
	}

	bool getErrorFlag()
	{
		return packet.Head.finishFlag;
	}

	void setSequenceNumber(int seqNum)		
	{
		packet.Head.sequenceNumber = seqNum;
	}

	int getLength()
	{
		return packet.Head.bodyLength;
	}

	void swapDestination()
	{
		char temp;
		temp = packet.Head.Source;
		packet.Head.Source = packet.Head.Destination;
		packet.Head.Destination = temp;
	}

	void setData(char* buffer, int size)
	{
		if (packet.Data)
			delete[] packet.Data;

		packet.Data = new char[size];
		memcpy(packet.Data, buffer, size);
		packet.Head.bodyLength = size;
		
	}

	char* getAddressOfData()
	{
		return packet.Data;
	}

	char* SerializeData(int &size)		// Size will be size of total packet
	{
		if (pSerialBuff)
			delete[] pSerialBuff;

		size = sizeof(packet.Head.Destination) + sizeof(packet.Head.Source) + sizeof(packet.Head.sequenceNumber) + sizeof(char) + sizeof(packet.Head.bodyLength) + packet.Head.bodyLength + sizeof(packet.CRC);
		pSerialBuff = new char[size];		// Character array should be able to hold data of entire packet
		int charSkip = 0;

		memcpy(pSerialBuff, &packet.Head.Destination, sizeof(packet.Head.Destination));		// Adding in one by one because trying all of header at once was giving trouble
		charSkip = charSkip + sizeof(packet.Head.Destination);
		memcpy(pSerialBuff + charSkip, &packet.Head.Source, sizeof(packet.Head.Source));
		charSkip = charSkip + sizeof(packet.Head.Source);
		memcpy(pSerialBuff + charSkip, &packet.Head.sequenceNumber, sizeof(packet.Head.sequenceNumber));
		charSkip = charSkip + sizeof(packet.Head.sequenceNumber);
		memcpy(pSerialBuff + charSkip, &packet.Head.sequenceNumber + sizeof(char), sizeof(char));		// Copying the 3 bits for flags plus 5 bits of padding
		charSkip = charSkip + sizeof(char);
		memcpy(pSerialBuff + charSkip, &packet.Head.bodyLength, sizeof(packet.Head.bodyLength));
		charSkip = charSkip + sizeof(packet.Head.bodyLength);
		memcpy(pSerialBuff + charSkip, packet.Data, packet.Head.bodyLength);
		charSkip = charSkip + packet.Head.bodyLength;

		packet.CRC = getCRC();
		memcpy(pSerialBuff + charSkip, &packet.CRC, sizeof(packet.CRC));

		return pSerialBuff;
	}

	int getCRC()
	{
		return 0xF500FF75;
	}

	void Print()
	{
		std::cout << "Src: " << (int)packet.Head.Source << " Dest: " << (int)packet.Head.Destination <<
			" Seq Num: " << packet.Head.sequenceNumber << " Flags (FIN/ACK/ERR): " << packet.Head.finishFlag
			<< "/" << packet.Head.ackFlag << "/" << packet.Head.errorFlag << endl;
	}

	~PktDef()		// Destructor for packets to delete allocated memory
	{
		if (packet.Data)
		{
			delete[] packet.Data;
		}
		if (pSerialBuff)
		{
			delete[] pSerialBuff;
		}
	}
};
