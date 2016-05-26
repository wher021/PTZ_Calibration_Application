#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>

// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

/*
*/

char zoom_x1[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x30, 0x30, 0x30, 0x30 };
char zoom_x2[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x32, 0x33, 0x37, 0x32 };
char zoom_x3[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x33, 0x32, 0x39, 0x31 };

char zoom_x4[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x33, 0x42, 0x38, 0x33 };
char zoom_x5[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x34, 0x31, 0x42, 0x30 };
char zoom_x6[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x34, 0x36, 0x36, 0x38 };

char zoom_x7[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x34, 0x39, 0x46, 0x42 };
char zoom_x8[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x34, 0x44, 0x33, 0x43 };
char zoom_x9[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x30, 0x30, 0x30 };

char zoom_x10[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x32, 0x37, 0x30 };
char zoom_x11[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x34, 0x38, 0x44 };
char zoom_x12[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x36, 0x41, 0x41 };

char zoom_x13[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x38, 0x39, 0x45 };
char zoom_x14[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x41, 0x36, 0x38 };
char zoom_x15[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x42, 0x44, 0x45 };
char zoom_x16[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x44, 0x32, 0x42 };
char zoom_x17[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x45, 0x34, 0x46 };
char zoom_x18[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x35, 0x46, 0x34, 0x38 };

char zoom_x19[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x30, 0x31, 0x38 };
char zoom_x20[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x30, 0x42, 0x46 };
char zoom_x21[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x31, 0x36, 0x35 };

char zoom_x22[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x31, 0x45, 0x32 };
char zoom_x23[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x32, 0x35, 0x46 };
char zoom_x24[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x32, 0x42, 0x32 };

char zoom_x25[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x33, 0x30, 0x36 };
char zoom_x26[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x33, 0x35, 0x39 };
char zoom_x27[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x33, 0x38, 0x33 };

char zoom_x28[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x33, 0x41, 0x43 };
char zoom_x29[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x33, 0x44, 0x36 };
char zoom_x30[19] = { 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0A, 0x01, 0x48, 0x3A, 0x57, 0x46, 0x38, 0x35, 0x38, 0x36, 0x34, 0x30, 0x30 };

#define DEFAULT_PORT "1237"//"27015" 
#define DEFAULT_BUFFER_LENGTH	50000

class Client {
public:
	Client()
	{
		ConnectSocket = INVALID_SOCKET;
	}
	Client(char* servername)
	{
		szServerName = servername;
		ConnectSocket = INVALID_SOCKET;
	}
	void Client::getZoom(char *myarray, std::string input);

	void Connect(char* servername)
	{
		szServerName = servername;
		ConnectSocket = INVALID_SOCKET;
	}

	bool Start() {
		WSADATA wsaData;

		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			return false;
		}

		struct addrinfo	*result = NULL,
			*ptr = NULL,
			hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(szServerName, DEFAULT_PORT, &hints, &result);
		if (iResult != 0)
		{
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return false;
		}

		ptr = result;

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Error at socket(): %d\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return false;
		}

		// Connect to server
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Unable to connect to server!\n");
			WSACleanup();
			return false;
		}

		return true;
	};

	// Free the resouces
	void Stop() {
		int iResult = shutdown(ConnectSocket, SD_SEND);

		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
		}

		closesocket(ConnectSocket);
		WSACleanup();
	};

	// Send message to server
	bool Send(char* szMsg)
	{
		//strlen(reinterpret_cast<const char*>(szMsg))

		int iResult = send(ConnectSocket, (char*)(szMsg), 17, 0);
		iResult = send(ConnectSocket, (char*)(szMsg), 17, 0);//send twice

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
			Stop();
			return false;
		}

		return true;
	};
	bool Send16(char* szMsg)
	{
		//strlen(reinterpret_cast<const char*>(szMsg))

		int iResult = send(ConnectSocket, (char*)(szMsg), 30, 0);
		iResult = send(ConnectSocket, (char*)(szMsg), 30, 0);//send twice

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
			Stop();
			return false;
		}

		return true;
	};

	// Receive message from server
	bool Recv()
	{
		char recvbuf[DEFAULT_BUFFER_LENGTH];
		int iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFFER_LENGTH, 0);

		if (iResult > 0)
		{
			char msg[DEFAULT_BUFFER_LENGTH];
			memset(&msg, 0, sizeof(msg));
			strncpy(msg, recvbuf, iResult);

			printf("Received: %s\n", msg);

			return true;
		}


		return false;
	}

private:
	char* szServerName;
	SOCKET ConnectSocket;
};

void Client::getZoom(char *myarray, std::string input)
{

	
	if (input == "Zoom1")
	{
		for (int i = 0; i < 18; i++)
		{
			//std::cout << (int)zoom_x12[i];
			myarray[i] = zoom_x1[i];
		}

	}
	else if (input == "Zoom2")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x2[i];
		}

	}
	else if (input == "Zoom3")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x3[i];
		}

	}
	else if (input == "Zoom4")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x4[i];
		}

	}
	else if (input == "Zoom5")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x5[i];
		}

	}
	else if (input == "Zoom6")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x6[i];
		}

	}
	else if (input == "Zoom7")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x7[i];
		}

	}
	else if (input == "Zoom8")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x8[i];
		}

	}
	else if (input == "Zoom9")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x9[i];
		}

	}
	else if (input == "Zoom10")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x10[i];
		}

	}
	else if (input == "Zoom11")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x11[i];
		}

	}
	else if (input == "Zoom12")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x12[i];
		}

	}
	else if (input == "Zoom13")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x13[i];
		}

	}
	else if (input == "Zoom14")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x14[i];
		}

	}
	else if (input == "Zoom15")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x15[i];
		}

	}
	else if (input == "Zoom16")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x16[i];
		}

	}
	else if (input == "Zoom17")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x17[i];
		}

	}
	else if (input == "Zoom18")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x18[i];
		}

	}
	else if (input == "Zoom19")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x19[i];
		}

	}
	else if (input == "Zoom20")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x20[i];
		}

	}
	else if (input == "Zoom21")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x21[i];
		}

	}
	else if (input == "Zoom22")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x22[i];
		}

	}
	else if (input == "Zoom23")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x23[i];
		}

	}
	else if (input == "Zoom24")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x24[i];
		}

	}
	else if (input == "Zoom25")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x25[i];
		}

	}
	else if (input == "Zoom26")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x26[i];
		}

	}
	else if (input == "Zoom27")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x27[i];
		}

	}
	else if (input == "Zoom28")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x28[i];
		}

	}
	else if (input == "Zoom29")
	{
		for (int i = 0; i < 18; i++)
		{
			myarray[i] = zoom_x29[i];
		}

	}
	else if (input == "Zoom30")
	{
		for (int i = 0; i < 18; i++)
		{
			//std::cout << (int)zoom_x12[i];
			myarray[i] = zoom_x30[i];
		}

	}

	int i = 0;
	while (i<18)
	{
		std::cout << myarray[i];
		i++;
	}
	std :: cout << std :: endl;
	
}