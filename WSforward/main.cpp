#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 102400

int initWinSock() {
	WSADATA wsaData;

	int result;
	// init winsock

	result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(result != 0){
		printf( "Failed to intilaze Winsock whit WSASTARTTUP %d \n" , result);
		return 1;
	}
	return 0 ;
};


void cleanUp(){

	WSACleanup();

}


int main() {


	int iResult;
	char ipstr[INET6_ADDRSTRLEN];

	//Initialize Winsock.
	if(initWinSock()){
		cleanUp();
		return 1 ;
	}

	struct addrinfo *result = NULL,
		*cResult = NULL, 
		*ptr = NULL,
		*cPtr = NULL, 
		hints
		;

	// this could be a define
	PCSTR listen_port = "1234"; // pcstr is a pointer to a constant null-terminated string of 8-bit Windows (ANSI) defined in winnt.h
	PCSTR send_port = "80";
	// hints for incoming connection 
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET ;  // since we work with ipv4   if ipv6 assign AF_INET6
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, listen_port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// hints for send

	iResult = getaddrinfo("www.jrjsys.com",send_port,&hints,&cResult);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// lsitening socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
	//Error cecking
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		freeaddrinfo(cResult);
		WSACleanup();
		return 1;
	}

	//Output socket 
	SOCKET OutputSocket = INVALID_SOCKET;
	OutputSocket = socket(cResult->ai_family,cResult->ai_socktype,cResult->ai_protocol);
	if (OutputSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		freeaddrinfo(cResult);
		WSACleanup();
		return 1;
	}

	//Bind the listenning socket.
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		freeaddrinfo(cResult);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);   // we dont need stuff from result anymore so how bout we clean it .

	//Listen on the socket for a client.
	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	//Accept a connection from a client.
	SOCKET ClientSocket;

	ClientSocket = INVALID_SOCKET;
	printf("waiting connection");
	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("get connection");


	// if we got connection we must connect to outgoing socket
	iResult = connect( OutputSocket, cResult->ai_addr, (int)cResult->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(OutputSocket);
		OutputSocket = INVALID_SOCKET;
	}

	// @TODO: Check if next adresse in cResult is good for a connection 
	freeaddrinfo(cResult);
	if (OutputSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//Receive and send data.


	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int iInResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(OutputSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		} else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

		iInResult = recv(OutputSocket,recvbuf,recvbuflen,0);
		if (iInResult > 0) {
			printf("Bytes received: %d\n", iInResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iInResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(OutputSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		} else if (iInResult == 0)
			printf("Connection closing on output...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0 && iInResult > 0);
	//Disconnect.
	cleanUp();

	system("pause");


	return 0;
}


