#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cofee.h"

/* Creates an IPv4 UDP socket
*/
int createSocketUDPIPv4 (){
	/* int socket(domain, type, protocol)
	   domain = AF_INET to IPv4, AF_INET6 to IPv6 
	   type = SOCK_DGRAM to UDP, SOCK_STREAM to TCP
	   protocol = value 0 for IP
	   */
	int socketServer = socket(AF_INET,SOCK_DGRAM,0);

	//Tests if the socket was created
	if (socketServer < 0){
		perror("Socket creation failed.");
	}

	return socketServer;
}

/* Creates an IPv6 UDP socket
*/
int createSocketUDPIPv6 (){
	/* int socket(domain, type, protocol)
	   domain = AF_INET to IPv4, AF_INET6 to IPv6 
	   type = SOCK_DGRAM to UDP, SOCK_STREAM to TCP
	   protocol = value 0 for IP
	   */
	int socketServer = socket(AF_INET6,SOCK_DGRAM,0);

	//Tests if the socket was created
	if (socketServer < 0){
		perror("Socket creation failed.");
	}

	return socketServer;
}

/* Defines an IPv4 address for server
*/
struct sockaddr_in getIPv4AddrServer(int port){
	//Set server address properties
	struct sockaddr_in address;

	//IPv4 addresses
	address.sin_family = AF_INET;

	//To listen in all interfaces
	address.sin_addr.s_addr =  htonl(INADDR_ANY);

	//Set the port to listen
	address.sin_port = htons(port);

	return address;
}

/* Defines an IPv4 address for client
*/
struct sockaddr_in getIPv4AddrClient(int port, char *serverAddress){
	//Set server address properties
	struct sockaddr_in address;

	//IPv4 addresses
	address.sin_family = AF_INET;

	//Set the port
	address.sin_port = htons(port);

	//To listen in all interfaces
	int inetFlag = inet_aton(serverAddress, &address.sin_addr);

	if(inetFlag==0){
		perror("Conversion failed.");
	}

	return address;
}

/* Bind the socket to the address
*/
int bindAddr(int socket, struct sockaddr_in address){

	int bindReturn = bind(socket, (struct sockaddr*) &address, sizeof(address));

	if(bindReturn < 0){
		perror("Binding failed.");
	}

	return bindReturn;
}

/* Send data using UDP
*/
int sendDataUDP(int socket, char *buffer, struct sockaddr_in addrReceiver, int length, int addrLength){

	//Get the address length
	//int addrLength = sizeof(addrReceiver);

	int sentBytes = sendto(socket, buffer, length, 0, (struct sockaddr*) &addrReceiver, addrLength);

	if(sentBytes < 0){
		perror("Sending failed.");
		exit(1);
	}

	return sentBytes;
}

/* Receive data using UDP
*/
int receiveDataUDP(int socket, char *buffer, struct sockaddr_in addrSender,int addrLength){

	//Get the address length
	//int addrLength = sizeof(addrSender);

	//Receive data 
	int receivedBytes = recvfrom(socket, buffer, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &addrSender, &addrLength);

	if(receivedBytes < 0){
		perror("Receiving failed.");
	}

	return receivedBytes;
}

/* Print contents of the message
*/
void printBufferContent(char *buffer){
	printf("Message content:%s\n",buffer);
}

/* Print sender information
*/
void printMessageSender(int bytes, struct sockaddr_in address){

	//Translate the address and port to be exhibited
	char *translatedAddress = inet_ntoa(address.sin_addr);
	int translatedPort = ntohs(address.sin_port);

	printf("Received message of %d bytes from address %s, port %d\n", bytes, translatedAddress, translatedPort);
}

/* Print sender information and contents of the message
*/
void printReceivedMessage(int bytes, struct sockaddr_in address, char *buffer){

	//Translate the address and port to be exhibited
	char *translatedAddress = inet_ntoa(address.sin_addr);
	int translatedPort = ntohs(address.sin_port);

	printf("Received message of %d bytes from address %s, port %d\n", bytes, translatedAddress, translatedPort);
	printf("Content:%s\n", buffer);

}

/* Close the socket
*/
int closeSocket(int socket){
	int closeReturn = close(socket);

	if(closeReturn < 0){
		perror("Closing socket failed.");
	}

	return closeReturn;
}
