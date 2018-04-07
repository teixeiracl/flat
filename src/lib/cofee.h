#ifndef cofee_h
#define cofee_h

#define PKIdP 1023
#define PKSP 1024
#define PKD 1025
#define SKIdP 1026
#define SKSP 1027
#define SKD 1028
#define SKDSP 1029 

/* Types of messages
 *  hello = 0
 *  cert_send = 1
 *  key_exchange = 2
 *  service_request = 3
 *  cert_sign = 4
 *  assert_sign = 5
 *  assert_enc = 6
 *  service_accept = 7
 */

#define TYPE_HELLO '0'
#define TYPE_CERT_SEND '1'
#define TYPE_KEY_EXCHANGE '2'
#define TYPE_SERVICE_REQUEST '3'
#define TYPE_CERT_SIGN '4'
#define TYPE_ASSERT_SIGN '5'
#define TYPE_ASSERT_ENC '6'
#define TYPE_SERVICE_ACCEPT '7'

#define ENC_SIZE 32
#define SIG_MAX_SIZE 128
#define SREQ_SIZE 24
#define MAC_SIZE 32
#define NONCE_SIZE 4
#define CERT_SIZE 602
#define MSGTYPE_SIZE 1
#define SEQUENCE_SIZE 2
#define ID_SIZE 32
#define SSID_SIZE 82
#define PAYLOAD_SIZE 606
#define TIME_SIZE_BROAD 20
#define TIME_SIZE_REAL 18
#define MAX_MESSAGE_SIZE 755

struct cofeePKG {
	//Message type: 1 byte
	char msgType;

	//Sequence: 1 byte
	char sequence[SEQUENCE_SIZE];

	//Source ID: 32 bytes
	char sourceID[ID_SIZE];

	//Destination ID: 32 bytes
	char destinationID[ID_SIZE];

	//Session ID: 82 bytes
	char sessionID[SSID_SIZE];

	//Payload: max 92 bytes
	char payload[PAYLOAD_SIZE];

};

//IDs for the nodes
extern char nodeID[ID_SIZE]; //id for the node itself
extern char spID[ID_SIZE]; //id for the SP
extern char idpID[ID_SIZE]; //id for the IdP

//Sequence incrementer: sequence number goes up to 99
extern char sequenceNumber[2];

void generateSequence(char *, int);
void generateID(char *);
void generateNonce(char *);
void generateCert(char *);
void callSeed(void);
void getSSID(char*, char*, char*);
void copyIntegerArray(int, int*, int*);
void copyCharArray(int, char*, char*);
void convertCharToInt(char*, int*, int);
void convertIntToChar(int *, char *, int);
void sequenceNumberPlus1(void);
void init(void);
void buildMessage(struct cofeePKG *, char *);
void hello(char*, char*, struct cofeePKG*);
void charVectorInitialization(char *, int);
void cofeePKGInitialization(struct cofeePKG *);
void cert_send(char*, char*, struct cofeePKG*, char*);
void key_exchange(char*, char*, struct cofeePKG*, char*);
void service_request(char*, char*, struct cofeePKG*, char*);
void cert_sign(char*, char*, struct cofeePKG*, char*);
void assert_sign(char*, char*, struct cofeePKG*, char*);
void assert_enc(char*, char*, struct cofeePKG*, char*);
void service_accept(char*, char*, struct cofeePKG*, char*);
void seq1(char *);

/*
//Hello message
char * hello(int *sID, int *dID);

//MAC of a message
char * mac(char *msg);

//Nonce
int nonce(void);

//Service specs
char * serviceEspecs(void);

//Encryption
char * encryption(char *msg);*/

#endif
