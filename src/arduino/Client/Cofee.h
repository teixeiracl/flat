#ifndef COFEE_H
#define COFEE_H

// Types of message
#define TYPE_HELLO '0'
#define TYPE_CERT_SEND '1'
#define TYPE_KEY_EXCHANGE '2'
#define TYPE_SERVICE_REQUEST '3'
#define TYPE_CERT_SIGN '4'
#define TYPE_ASSERT_SIGN '5'
#define TYPE_ASSERT_ENC '6'
#define TYPE_SERVICE_ACCEPT '7'

// Sizes of protocol message blocks
#define ADDR_SIZE 32
#define MSGTYPE_SIZE 1
#define SEQUENCE_SIZE 2
#define SSID_SIZE 82
#define PAYLOAD_SIZE 606

// Protocol packet
struct cofee_pkt {
	char type;
	char seq[SEQUENCE_SIZE];
	char src[ADDR_SIZE];
	char dest[ADDR_SIZE];
	char session[SSID_SIZE];
	char payload[PAYLOAD_SIZE];
};

void generate_sequence(char *sq, int length);

void call_seed();

void send_hello(struct cofee_pkg *pkg, char *session, char *src, char *dest);

#endif