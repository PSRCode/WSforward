#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512

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
	//Create a socket.

	//create multiple addrinfo struct  addrinfo look like 
	//{
	//	int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
	//	int                 ai_family;      // PF_xxx
	//	int                 ai_socktype;    // SOCK_xxx
	//	int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
	//	size_t              ai_addrlen;     // Length of ai_addr
	//	char *              ai_canonname;   // Canonical name for nodename
	//	_Field_size_bytes_(ai_addrlen) struct sockaddr *   ai_addr;        // Binary address
	//	struct addrinfo *   ai_next;        // Next structure in linked list
	//}
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	// this could be a define
	PCSTR default_port = "27015"; // pcstr is a pointer to a constant null-terminated string of 8-bit Windows (ANSI) defined in winnt.h

	//equivalent to memset(&hints,0,sizeof(hints)) wich allocate and set to 0  memory. Seems obvious but not for me :P
	ZeroMemory(&hints, sizeof(hints));

	//Setting familly 
	hints.ai_family = AF_INET ;  // since we work with ipv4   if ipv6 assign AF_INET6

	//Socket type
	// 1 => SOCK_STREAM    tcp style
	// 2 => SOCK_DGRAM		UDP style
	// for other types http://msdn.microsoft.com/en-ca/library/windows/desktop/ms740506(v=vs.85).aspx
	// we use stream here 

	hints.ai_socktype = SOCK_STREAM;

	// lot of protocol , will stick to TCP for now
	// @TODO: explore other protocol and difference between ipv4 and tcp ...  
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// not sure why we need to get info ... anyway probably just to make sure the server can support what we ask for
	iResult = getaddrinfo(NULL, default_port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
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
		WSACleanup();
		return 1;
	}
	//Output socket 


	//Bind the socket.
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
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

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//Receive and send data.


	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
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

	} while (iResult > 0);
	//Disconnect.
	cleanUp();

	system("pause");


	return 0;
}


