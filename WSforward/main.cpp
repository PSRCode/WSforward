#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

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


int cleanUp(){

	WSACleanup();
	
}


int main() {
	//Initialize Winsock.
		if(initWinSock()){
		cleanUp();
		return 1 ;
	}
	//Create a socket.

	//Bind the socket.

	//Listen on the socket for a client.

	//Accept a connection from a client.

	//Receive and send data.

	//Disconnect.
	cleanUp();

	system("pause");

	
  return 0;
}


