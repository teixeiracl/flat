#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lib/cofee.h"

#define MAX_PAYLOAD_LEN 30
#define HASH_SIZE_BYTES 20

#define SERVER_PORT 5000

int main (int argc, char *argv[]){
	/* int socket(domain, type, protocol)
	   domain = AF_INET to IPv4, AF_INET6 to IPv6 
	   type = SOCK_DGRAM to UDP, SOCK_STREAM to TCP
	   protocol = value 0 for IP
	   */
	int socketServer = socket(AF_INET,SOCK_DGRAM,0);

	//Tests if the socket was created
	if (socketServer < 0){
		perror("Socket creation failed.");
		exit(1);
	}

	//Set server address properties
	struct sockaddr_in serverAddress;
	int addressLength = sizeof(serverAddress);

	//Address properties for receiving data
	struct sockaddr_in receiveAddress;

	//IPv4 addresses
	serverAddress.sin_family = AF_INET;

	//To listen in all interfaces
	serverAddress.sin_addr.s_addr =  htonl(INADDR_ANY);

	//Set the port to listen
	serverAddress.sin_port = htons(SERVER_PORT);

	//Define send and receive buffer
	char sendReceiveBuffer[MAX_MESSAGE_SIZE];

	//Bind the socket to the address and tests if it was successful
	int bindReturn = bind(socketServer, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if(bindReturn < 0){
		perror("Binding failed.");
		exit(1);
	}

	//Clear the buffer
	memset(sendReceiveBuffer,'\0',MAX_MESSAGE_SIZE);

	//Receive data 
	int receivedBytes = recvfrom(socketServer, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &receiveAddress, &addressLength);

	if(receivedBytes < 0){
		perror("Receiving failed.");
		exit(1);
	}

	//Translate the address and port to be exhibited
	char *translatedAddress = inet_ntoa(receiveAddress.sin_addr);
	int translatedPort = ntohs(receiveAddress.sin_port);

	//Print the sender information and contents of the message
	printf("********** Received message type %c of %d bytes from address %s, port %d\n", sendReceiveBuffer[0],receivedBytes, translatedAddress, translatedPort);
	printf("Content:%s\n",sendReceiveBuffer);

	struct cofeePKG hc1;
	char seq[SEQUENCE_SIZE];
	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	cert_send(spID, nodeID,&hc1,seq);

	char msg[MAX_MESSAGE_SIZE];
	buildMessage(&hc1,msg);

	//Reply the received message to the client
	int sentBytes = sendto(socketServer, msg, strlen(msg), 0, (struct sockaddr*) &receiveAddress, addressLength);

	if(sentBytes < 0){
		perror("Sending failed.");
		exit(1);
	}

	memset(sendReceiveBuffer,'\0',MAX_MESSAGE_SIZE);

	//Receive data 
	receivedBytes = -1;

	receivedBytes = recvfrom(socketServer, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &receiveAddress, &addressLength);

	if(receivedBytes< 0){
		perror("Receiving failed.");
		exit(1);
	}

	//Translate the address and port to be exhibited
	translatedAddress = inet_ntoa(receiveAddress.sin_addr);
	translatedPort = ntohs(receiveAddress.sin_port);

	//Print the sender information and contents of the message
	printf("********** Received message type %c of %d bytes from address %s, port %d\n", sendReceiveBuffer[0],receivedBytes, translatedAddress, translatedPort);
	printf("Content:%.755s\n",sendReceiveBuffer);

	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	memset(msg,'\0',MAX_MESSAGE_SIZE);

	struct cofeePKG hc2;

	key_exchange(spID, nodeID,&hc2,seq);

	buildMessage(&hc2,msg);

	sentBytes = -1;
	sentBytes = sendto(socketServer, msg, strlen(msg), 0, (struct sockaddr*) &receiveAddress, addressLength);

	if(sentBytes < 0){
		perror("Sending failed.");
		exit(1);
	}

	memset(sendReceiveBuffer,'\0',MAX_MESSAGE_SIZE);

	//Receive data 
	receivedBytes = -1;

	receivedBytes = recvfrom(socketServer, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0, (struct sockaddr*) &receiveAddress, &addressLength);

	if(receivedBytes< 0){
		perror("Receiving failed.");
		exit(1);
	}

	//Translate the address and port to be exhibited
	translatedAddress = inet_ntoa(receiveAddress.sin_addr);
	translatedPort = ntohs(receiveAddress.sin_port);

	//Print the sender information and contents of the message
	printf("********** Received message type %c of %d bytes from address %s, port %d\n", sendReceiveBuffer[0],receivedBytes, translatedAddress, translatedPort);
	printf("Content:%.755s\n",sendReceiveBuffer);

	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	memset(msg,'\0',MAX_MESSAGE_SIZE);

	struct cofeePKG hc3;

	service_accept(spID, nodeID,&hc3,seq);

	buildMessage(&hc3,msg);

	sentBytes = -1;
	sentBytes = sendto(socketServer, msg, strlen(msg), 0, (struct sockaddr*) &receiveAddress, addressLength);

	if(sentBytes < 0){
		perror("Sending failed.");
		exit(1);
	}


	close(socketServer);
	return 0;
}
