#ifndef COFEE_H
#define COFEE_H

#define BUFFER_SIZE 90

// #define CLIENT_IP_ADDR "192.168.1.170"
// #define SP_IP_ADDR "192.168.1.162"
#define CLIENT_IP_ADDR "192.168.0.16"
#define SP_IP_ADDR "192.168.0.18"

#define SSID "ssid"
#define PASSWORD "password"

#define IDP_PORT 51515
#define CLIENT_PORT 12333
#define SP_PORT 80

#define IDP_MAX_CONNECTION 2

#define MSG_MAX_SIZE 206
#define MSG_HEADER_SIZE 6

#define MSG_ID_SIZE 2
#define MSG_TYPE_SIZE 1
#define MSG_SEQ_SIZE 1
#define MSG_MAX_DATA_SIZE 200

#define TYPE_KEY_REQ "1"
#define TYPE_KEY_REP "2"
#define TYPE_SRV_MEN_REQ "3"
#define TYPE_SRV_MEN_REP "4"
#define TYPE_AST_REQ "5"
#define TYPE_AST_REP "6"
#define TYPE_SRV_REQ "7"
#define TYPE_SRV_REP "8"
#define TYPE_SND_CER "9"

#define KEY_REQ "key_request"
#define KEY_REP "key_response"
#define SHARE_KEY "send_shared_key"
#define CERT_PLAIN "send_certificate_plain"
#define CERT_SIG "send_certificate_signed"
#define ASSERT_REQ "assertion_request"
#define ASSERT_REP "assertion_response"
#define SERV_REQ "service_request"
#define SERV_REP "service_response"
#define ACK "ack"

#endif