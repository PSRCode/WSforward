#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <strstream>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define MYPORT "11000"


int main(void){

	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	struct addrinfo	hints,*res;
	int sockfd = INVALID_SOCKET ,new_fd = INVALID_SOCKET,error;
	WSADATA wsaData;

	//Winsock initialization
	error = WSAStartup(MAKEWORD(2,2), &wsaData);
	
	//Error check init
	if (error != 0 ) {
		printf("Erreor at initilization: %d", error);
		return 1; 
	}


	ZeroMemory(&hints,sizeof(hints));
	hints.ai_flags = AI_PASSIVE;   // this would be the ip we want to check 
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET; //  We sue ipv4 here   AF_UNSPEC if we don't care

	error = getaddrinfo(NULL,MYPORT,&hints,&res);
	//error check getaddrinfo
	if (error != 0 ) {
		printf("Error at getaddrinfo: %d", error);
		WSACleanup();
		return 1; 
	}
	 
	// make, bind and listen on socket
	sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if(sockfd == INVALID_SOCKET){
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(res);
		WSACleanup();
		return 1;
	}


	error = bind(sockfd,res->ai_addr,res->ai_addrlen);
	if (error == SOCKET_ERROR ) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(res);
		closesocket(sockfd);
		WSACleanup();
		return 1;
	}
	
	
	if(listen(sockfd,SOMAXCONN) == SOCKET_ERROR){
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(sockfd);
		WSACleanup();
		return 1;	
	};


	// accept incoming connection    may need thread here !
	addr_size = sizeof(client_addr);
	new_fd = accept(sockfd,(struct sockaddr * )&client_addr, &addr_size);
	if(new_fd == INVALID_SOCKET){
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(res);
		WSACleanup();
		return 1;
	}
	system("pause");
	//connect to next peer 
	struct addrinfo cHints, *cRes;
	int sockConnect;

	ZeroMemory(&cHints,sizeof(cHints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL,"12000",&hints,&cRes);

	sockConnect = socket(cRes->ai_family,cRes->ai_socktype,cRes->ai_protocol);

	connect(sockConnect,cRes->ai_addr,cRes->ai_addrlen);


	//char *msg = "Dam seems to work one way!";
	char readBuffer[100], outBuffer[100];
	int readBytes;

	readBytes = recv(sockfd, readBuffer, 100, 0);
	if (readBytes > 0) {
        std::cout << "Received " << readBytes << " bytes from client." <<  std::endl;
		 std::cout << "Received " << readBuffer << " from client." <<  std::endl;
		 send(sockConnect, outBuffer, 100, 0);
	}

	readBytes = recv(sockConnect, readBuffer, 100, 0);
	if (readBytes > 0) {
        std::cout << "Received " << readBytes << " bytes from client." <<  std::endl;
		 std::cout << "Received " << readBuffer << " from client." <<  std::endl;
		 send(sockfd, outBuffer, 100, 0);
	}

	system("pause");







	
}


// for later us on max conn SOMAXCONN 