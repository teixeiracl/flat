#include "lib/cofee.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// IDs for the nodes
char nodeID[ID_SIZE] =
    "88076732171524690480570066146926";  // id for the node itself
char spID[ID_SIZE] = "88076732171524690480570066146927";   // id for the SP
char idpID[ID_SIZE] = "88076732171524690480570066146928";  // id for the IdP

// Sequence incrementer: sequence number goes up to 99
char sequenceNumber[2] = "00";

// Generate a sequence of numbers representation in char
void generateSequence(char *sq, int length) {
	callSeed();
	int r, i = 0;
	for (i = 0; i < length; i++) {
		r = rand() % 10;
		sq[i] = (char)r + '0';
	}
}

// Generate nonce
void generateNonce(char *nonce) { generateSequence(nonce, NONCE_SIZE); }

// Generate encryption
void generateEncryption(char *enc) { generateSequence(enc, ENC_SIZE); }

// Generate signature
void generateSignature(char *sig) { generateSequence(sig, SIG_MAX_SIZE); }

// Generate service request
void generateServiceRequest(char *sRequest) {
	generateSequence(sRequest, SREQ_SIZE);
}

// Generate certificate
void generateCert(char *cert) { generateSequence(cert, CERT_SIZE); }

// Generate MAC
void generateMAC(char *mac) { generateSequence(mac, MAC_SIZE); }

// To feed the random seed
void callSeed() { srand(time(NULL)); }

// Calculate the session ID based on the source ID, destination ID and time of
// communication
void getSSID(char *sID, char *dID, char *ssID) {
	int i = 0;
	for (i = 0; i < ID_SIZE; i++) {
		ssID[i] = sID[i];
	}

	int j = 0;
	int max = i + ID_SIZE;
	for (; i < max; i++) {
		ssID[i] = dID[j];
		j++;
	}

	char ct[TIME_SIZE_BROAD];

	// Initialize ct
	for (j = 0; j < TIME_SIZE_BROAD; j++) {
		ct[j] = '#';
	}
	time_t rt;
	time(&rt);
	struct tm *t = localtime(&rt);
	strftime(ct, TIME_SIZE_BROAD, "%Y%m%d%H%M%S%w%j", t);

	int size = 0;
	for (j = 0; j < TIME_SIZE_BROAD; j++) {
		if (ct[j] != '#') {
			size++;
		}
	}
	max = i + size;
	for (j = 0; i < max; i++) {
		ssID[i] = ct[j];
		j++;
	}
}

// Copy an integer array to another integer array
void copyIntegerArray(int length, int *a1, int *a2) {
	int i = 0;
	for (i = 0; i < length; i++) {
		a1[i] = a2[i];
	}
}

// Copy a char array to another char array
void copyCharArray(int length, char *a1, char *a2) {
	int i = 0;
	for (i = 0; i < length; i++) {
		a1[i] = a2[i];
	}
}

// Convert a char array to an int array
void convertCharToInt(char *a1, int *a2, int length) {
	int i;
	for (i = 0; i < length; i++) {
		a2[i] = (int)a1[i];
	}
}

// Convert an int array to a char array
void convertIntToChar(int *a1, char *a2, int length) {
	int i;
	for (i = 0; i < length; i++) {
		a2[i] = (char)a1[i];
	}
}

// Increments the sequence number by 1
void sequenceNumberPlus1() {
	int i = 0;
	for (i = 1; i >= 1; i--) {
		sequenceNumber[i]++;
		if (sequenceNumber[i] > '9') {
			sequenceNumber[i] = '0';
			if (sequenceNumber[i - 1] < '9') {
				sequenceNumber[i - 1] =
				    sequenceNumber[i - 1] + 1;
			} else {
				sequenceNumber[i - 1] = '0';
			}
		} else {
			break;
		}
	}
}

// Increments the sequence by 1
void seq1(char *seq) {
	int i = 0;
	for (i = 1; i >= 1; i--) {
		seq[i]++;
		if (seq[i] > '9') {
			seq[i] = '0';
			if (seq[i - 1] < '9') {
				seq[i - 1] = seq[i - 1] + 1;
			} else {
				seq[i - 1] = '0';
			}
		} else {
			break;
		}
	}
}

void buildMessage(struct cofeePKG *hc, char *message) {
	int i = 0;
	int j = 0;
	int max = 0;
	max = ID_SIZE + ID_SIZE + PAYLOAD_SIZE + SSID_SIZE + SEQUENCE_SIZE +
	      MSGTYPE_SIZE;
	for (i = 0; i < max; i++) {
		message[i] = '#';
	}

	message[0] = (*hc).msgType;
	message[1] = (*hc).sequence[0];
	message[2] = (*hc).sequence[1];

	i = 3;
	max = i + ID_SIZE;
	for (j = 0; i < max; i++) {
		message[i] = (*hc).sourceID[j];
		j++;
	}

	max = i + ID_SIZE;
	for (j = 0; i < max; i++) {
		message[i] = (*hc).destinationID[j];
		j++;
	}

	max = i + SSID_SIZE;
	for (j = 0; i < max; i++) {
		message[i] = (*hc).sessionID[j];
		j++;
	}

	max = i + PAYLOAD_SIZE;
	for (j = 0; i < max; i++) {
		message[i] = (*hc).payload[j];
		j++;
	}
	max = ID_SIZE + ID_SIZE + PAYLOAD_SIZE + SSID_SIZE + SEQUENCE_SIZE +
	      MSGTYPE_SIZE;
}

// Hello message
void hello(char *sID, char *dID, struct cofeePKG *hc) {
	// init();
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];

	// Initialize the vectors
	int i;
	for (i = 0; i < PAYLOAD_SIZE; i++) {
		msg[i] = '#';
		(*hc).payload[i] = '#';
	}
	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = '#';
		(*hc).sourceID[i] = '#';
	}
	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = -1;
	}
	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = '#';
	}

	(*hc).msgType = TYPE_HELLO;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = sequenceNumber[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	strcpy(msg, "HELLO");

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	for (i = 0; i < PAYLOAD_SIZE; i++) {
		(*hc).payload[i] = msg[i];
	}
}

void charVectorInitialization(char *vector, int length) {
	int i = 0;
	for (i = 0; i < length; i++) {
		vector[i] = '#';
	}
}

void cofeePKGInitialization(struct cofeePKG *hc) {
	charVectorInitialization((*hc).payload, PAYLOAD_SIZE);
	charVectorInitialization((*hc).destinationID, ID_SIZE);
	charVectorInitialization((*hc).sourceID, ID_SIZE);
	charVectorInitialization((*hc).sessionID, SSID_SIZE);
	charVectorInitialization((*hc).sequence, SEQUENCE_SIZE);
	(*hc).msgType = '#';
}

// Sending certificate message
void cert_send(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char nonce[NONCE_SIZE];
	char cert[CERT_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;
	for (i = 0; i < PAYLOAD_SIZE; i++) {
		msg[i] = '#';
		(*hc).payload[i] = '#';
	}
	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = '#';
		(*hc).sourceID[i] = '#';
	}
	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = -1;
	}
	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = '#';
	}

	(*hc).msgType = TYPE_CERT_SEND;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateNonce(nonce);
	generateCert(cert);

	for (i = 0; i < NONCE_SIZE; i++) {
		msg[i] = nonce[i];
	}
	int max = i + CERT_SIZE;
	int j = 0;
	for (j = 0; i < max; i++) {
		msg[i] = cert[j];
		j++;
	}

	strcpy((*hc).payload, msg);

	for (i = 0; i < NONCE_SIZE; i++) {
		(*hc).payload[i] = msg[i];
	}
}

void key_exchange(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char mac[MAC_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;

	(*hc).msgType = TYPE_KEY_EXCHANGE;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateMAC(mac);

	for (i = 0; i < MAC_SIZE; i++) {
		msg[i] = mac[i];
	}

	for (i = 0; i < MAC_SIZE; i++) {
		(*hc).payload[i] = mac[i];
	}
}

void service_request(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char nonce[NONCE_SIZE];
	char sRequest[SREQ_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;

	(*hc).msgType = TYPE_SERVICE_REQUEST;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		// printf("hello!!!!!!!!!!!!!!!!!!!  msgType %c\n", seq[i]);
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateNonce(nonce);
	generateServiceRequest(sRequest);

	for (i = 0; i < NONCE_SIZE; i++) {
		(*hc).payload[i] = nonce[i];
	}

	int j = 0;
	int max = i + SREQ_SIZE;
	for (j = 0; i < max; i++) {
		(*hc).payload[i] = sRequest[j];
		j++;
	}
}

void cert_sign(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char sig[SIG_MAX_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;

	(*hc).msgType = TYPE_CERT_SIGN;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateSignature(sig);

	for (i = 0; i < SIG_MAX_SIZE; i++) {
		msg[i] = sig[i];
	}

	for (i = 0; i < SIG_MAX_SIZE; i++) {
		(*hc).payload[i] = sig[i];
	}
}

void assert_sign(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char sig[SIG_MAX_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;

	(*hc).msgType = TYPE_ASSERT_SIGN;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateSignature(sig);

	for (i = 0; i < SIG_MAX_SIZE; i++) {
		msg[i] = sig[i];
	}

	for (i = 0; i < SIG_MAX_SIZE; i++) {
		(*hc).payload[i] = sig[i];
	}
}

void assert_enc(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char enc[ENC_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;

	(*hc).msgType = TYPE_ASSERT_ENC;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateEncryption(enc);
	// generateMAC(mac);

	for (i = 0; i < ENC_SIZE; i++) {
		msg[i] = enc[i];
	}

	for (i = 0; i < ENC_SIZE; i++) {
		(*hc).payload[i] = enc[i];
	}
}

void service_accept(char *sID, char *dID, struct cofeePKG *hc, char *seq) {
	char ssID[SSID_SIZE];
	char msg[PAYLOAD_SIZE];
	char enc[ENC_SIZE];
	int i = 0;

	// Initialize the vectors
	i = 0;

	(*hc).msgType = TYPE_SERVICE_ACCEPT;

	for (i = 0; i < SEQUENCE_SIZE; i++) {
		(*hc).sequence[i] = seq[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).sourceID[i] = sID[i];
	}

	for (i = 0; i < ID_SIZE; i++) {
		(*hc).destinationID[i] = dID[i];
	}

	getSSID(sID, dID, ssID);

	for (i = 0; i < SSID_SIZE; i++) {
		(*hc).sessionID[i] = ssID[i];
	}

	generateEncryption(enc);
	// generateMAC(mac);

	for (i = 0; i < ENC_SIZE; i++) {
		msg[i] = enc[i];
	}

	for (i = 0; i < ENC_SIZE; i++) {
		(*hc).payload[i] = enc[i];
	}
}
