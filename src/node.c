#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "lib/cofee.h"

#define MAX_PAYLOAD_LEN 30
#define HASH_SIZE_BYTES 20
#define BUFFER_SIZE 1024

#define CLIENT_PORT_1 5000
#define CLIENT_PORT_2 6000
#define BUFFER_SIZE 1024
#define SERVER_ADDRESS "127.0.0.1"

int main(int argc, char *argv[]) {
	// Buffer and message to be sent and its sizesx
	char buffer[MAX_MESSAGE_SIZE];

	struct cofeePKG hc;

	hello(nodeID, spID, &hc);

	char msg[MAX_MESSAGE_SIZE];

	// Buffer and message to be sent and its sizesx
	char sendReceiveBuffer[MAX_MESSAGE_SIZE];

	int i;
	for (i = 0; i < MAX_MESSAGE_SIZE; i++) {
		msg[i] = '#';
	}

	buildMessage(&hc, msg);

	/* int socket(domain, type, protocol)
	   domain = AF_INET to IPv4, AF_INET6 to IPv6
	   type = SOCK_DGRAM to UDP, SOCK_STREAM to TCP
	   protocol = value 0 for IP
	   */
	int socketClient = socket(AF_INET, SOCK_DGRAM, 0);

	// Tests if the socket was created
	if (socketClient < 0) {
		perror("Socket creation failed.");
		exit(1);
	}

	// Set server address properties
	struct sockaddr_in clientAddress;
	int clientAddressLength = sizeof(clientAddress);

	// IPv4 addresses
	clientAddress.sin_family = AF_INET;

	// Set the port
	clientAddress.sin_port = htons(CLIENT_PORT_1);

	// To listen in all interfaces
	int inetFlag = inet_aton(SERVER_ADDRESS, &clientAddress.sin_addr);

	if (inetFlag == 0) {
		perror("Conversion failed.");
		exit(1);
	}

	// Set another server address properties
	struct sockaddr_in clientAddress2;
	int clientAddressLength2 = sizeof(clientAddress);

	// IPv4 addresses
	clientAddress2.sin_family = AF_INET;

	// Set the port
	clientAddress2.sin_port = htons(CLIENT_PORT_2);

	// To listen in all interfaces
	int inetFlag2 = inet_aton(SERVER_ADDRESS, &clientAddress2.sin_addr);

	if (inetFlag2 == 0) {
		perror("Conversion failed.");
		exit(1);
	}

	// Send the message to the server
	int sendLength =
	    sendto(socketClient, msg, strlen(msg), 0,
		   (struct sockaddr *)&clientAddress, clientAddressLength);

	if (sendLength < 0) {
		perror("Sending failed.");
		exit(1);
	}

	// Clear the buffer
	memset(sendReceiveBuffer, '\0', MAX_MESSAGE_SIZE);

	// Receive the reply
	int receivedBytes =
	    recvfrom(socketClient, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0,
		     (struct sockaddr *)&clientAddress, &clientAddressLength);

	if (receivedBytes < 0) {
		perror("Receiving failed.");
		exit(1);
	}

	// Translate the address and port to be exhibited
	char *translatedAddress = inet_ntoa(clientAddress.sin_addr);
	int translatedPort = ntohs(clientAddress.sin_port);

	// Print the sender information and contents of the message
	printf(
	    "********** Received message type %c of %d bytes from address %s, "
	    "port %d\n",
	    sendReceiveBuffer[0], receivedBytes, translatedAddress,
	    translatedPort);
	printf("Content:%s\n", sendReceiveBuffer);

	/*int k=0;
	  for(k=0;k<MAX_MESSAGE_SIZE;k++){
	  printf("sendReceiveBuffer[%d]: %c\n",k,sendReceiveBuffer[k]);
	  }*/

	char seq[SEQUENCE_SIZE];
	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	memset(msg, '\0', MAX_MESSAGE_SIZE);

	struct cofeePKG hc2;

	key_exchange(nodeID, spID, &hc2, seq);

	buildMessage(&hc2, msg);

	sendLength = -1;
	sendLength =
	    sendto(socketClient, msg, strlen(msg), 0,
		   (struct sockaddr *)&clientAddress, clientAddressLength);

	if (sendLength < 0) {
		perror("Sending failed.");
		exit(1);
	}

	/// Clear the buffer
	memset(sendReceiveBuffer, '\0', MAX_MESSAGE_SIZE);

	receivedBytes = -1;
	// Receive the reply
	receivedBytes =
	    recvfrom(socketClient, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0,
		     (struct sockaddr *)&clientAddress, &clientAddressLength);

	if (receivedBytes < 0) {
		perror("Receiving failed.");
		exit(1);
	}

	// Translate the address and port to be exhibited
	translatedAddress = inet_ntoa(clientAddress.sin_addr);
	translatedPort = ntohs(clientAddress.sin_port);

	// Print the sender information and contents of the message
	printf(
	    "********** Received message type %c of %d bytes from address %s, "
	    "port %d\n",
	    sendReceiveBuffer[0], receivedBytes, translatedAddress,
	    translatedPort);
	printf("Content:%s\n", sendReceiveBuffer);

	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	memset(msg, '\0', MAX_MESSAGE_SIZE);

	struct cofeePKG hc3;

	service_request(nodeID, idpID, &hc3, seq);

	buildMessage(&hc3, msg);
	// Send the message to the server
	sendLength = -1;
	sendLength =
	    sendto(socketClient, msg, strlen(msg), 0,
		   (struct sockaddr *)&clientAddress2, clientAddressLength2);

	if (sendLength < 0) {
		perror("Sending failed.");
		exit(1);
	}

	// Clear the buffer
	memset(sendReceiveBuffer, '\0', MAX_MESSAGE_SIZE);

	receivedBytes = -1;
	// Receive the reply
	receivedBytes =
	    recvfrom(socketClient, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0,
		     (struct sockaddr *)&clientAddress2, &clientAddressLength2);

	if (receivedBytes < 0) {
		perror("Receiving failed.");
		exit(1);
	}

	// Translate the address and port to be exhibited
	translatedAddress = inet_ntoa(clientAddress2.sin_addr);
	translatedPort = ntohs(clientAddress2.sin_port);

	// Print the sender information and contents of the message
	printf(
	    "********** Received message type %c of %d bytes from address %s, "
	    "port %d\n",
	    sendReceiveBuffer[0], receivedBytes, translatedAddress,
	    translatedPort);
	printf("Content:%s\n", sendReceiveBuffer);

	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	memset(msg, '\0', MAX_MESSAGE_SIZE);

	struct cofeePKG hc4;

	cert_sign(nodeID, idpID, &hc4, seq);

	buildMessage(&hc4, msg);
	// Send the message to the server
	sendLength = -1;
	sendLength =
	    sendto(socketClient, msg, strlen(msg), 0,
		   (struct sockaddr *)&clientAddress2, clientAddressLength2);

	if (sendLength < 0) {
		perror("Sending failed.");
		exit(1);
	}

	// Clear the buffer
	memset(sendReceiveBuffer, '\0', MAX_MESSAGE_SIZE);

	receivedBytes = -1;
	// Receive the reply
	receivedBytes =
	    recvfrom(socketClient, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0,
		     (struct sockaddr *)&clientAddress2, &clientAddressLength2);

	if (receivedBytes < 0) {
		perror("Receiving failed.");
		exit(1);
	}

	// Translate the address and port to be exhibited
	translatedAddress = inet_ntoa(clientAddress2.sin_addr);
	translatedPort = ntohs(clientAddress2.sin_port);

	// Print the sender information and contents of the message
	printf(
	    "********** Received message type %c of %d bytes from address %s, "
	    "port %d\n",
	    sendReceiveBuffer[0], receivedBytes, translatedAddress,
	    translatedPort);
	printf("Content:%s\n", sendReceiveBuffer);

	// Fix the fields before redirecting the assert to SP
	/*seq[0] = sendReceiveBuffer[1];
	  seq[1] = sendReceiveBuffer[2];
	  seq1(seq);

	  sendReceiveBuffer[1] = seq[0];
	  sendReceiveBuffer[2] = seq[1];

	  int k= MSGTYPE_SIZE + SEQUENCE_SIZE;
	  int j = 0;
	  int max = k + ID_SIZE;
	  for(j=0;k<max;k++){
	  sendReceiveBuffer[k] = nodeID[j];
	  j++;
	  }
	  max = k + ID_SIZE;
	  for(j=0;k<max;k++){
	  sendReceiveBuffer[k] = spID[j];
	  j++;
	  }
	  max = k + ID_SIZE;
	  for(j=0;k<max;k++){
	  sendReceiveBuffer[k] = nodeID[j];
	  j++;
	  }
	  max = k + ID_SIZE;
	  for(j=0;k<max;k++){
	  sendReceiveBuffer[k] = spID[j];
	  j++;
	  }*/

	seq[0] = sendReceiveBuffer[1];
	seq[1] = sendReceiveBuffer[2];
	seq1(seq);

	memset(msg, '\0', MAX_MESSAGE_SIZE);

	struct cofeePKG hc5;

	assert_enc(nodeID, spID, &hc5, seq);

	buildMessage(&hc5, msg);

	// Send assert received from Idp to SP
	sendLength = -1;
	sendLength =
	    sendto(socketClient, msg, strlen(msg), 0,
		   (struct sockaddr *)&clientAddress, clientAddressLength);

	if (sendLength < 0) {
		perror("Sending failed.");
		exit(1);
	}

	memset(sendReceiveBuffer, '\0', MAX_MESSAGE_SIZE);

	receivedBytes = -1;
	// Receive the reply
	receivedBytes =
	    recvfrom(socketClient, sendReceiveBuffer, MAX_MESSAGE_SIZE, 0,
		     (struct sockaddr *)&clientAddress, &clientAddressLength);

	if (receivedBytes < 0) {
		perror("Receiving failed.");
		exit(1);
	}

	// Translate the address and port to be exhibited
	translatedAddress = inet_ntoa(clientAddress.sin_addr);
	translatedPort = ntohs(clientAddress.sin_port);

	// Print the sender information and contents of the message
	printf(
	    "********** Received message type %c of %d bytes from address %s, "
	    "port %d\n",
	    sendReceiveBuffer[0], receivedBytes, translatedAddress,
	    translatedPort);
	printf("Content:%s\n", sendReceiveBuffer);

	close(socketClient);
	return 0;
}
