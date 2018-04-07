#include <string.h>

#include <SPI.h>
#include <WiFi.h>

#include "../cofee.h"

// variables for WiFi connection
char ssid[] = "ssid";
char pass[] = "password";
int status = WL_IDLE_STATUS;

// IPAddress idp_ip(192, 168, 1, 146);
// IPAddress client_ip(192, 168, 1, 170);
IPAddress idp_ip(192, 168, 0, 15);
IPAddress client_ip(192, 168, 0, 16);

WiFiClient con;
WiFiServer server(80);

unsigned int client_port = 12333;
unsigned int idp_port = 51515;

char buffer[BUFFER_SIZE];
char msg[BUFFER_SIZE];

int state;

void printWifiData()
{
	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print(F("IP Address: "));
	Serial.println(ip);

	// print your MAC address:
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print(F("MAC address: "));
	Serial.print(mac[5], HEX);
	Serial.print(F(":"));
	Serial.print(mac[4], HEX);
	Serial.print(F(":"));
	Serial.print(mac[3], HEX);
	Serial.print(F(":"));
	Serial.print(mac[2], HEX);
	Serial.print(F(":"));
	Serial.print(mac[1], HEX);
	Serial.print(F(":"));
	Serial.println(mac[0], HEX);
}

bool connect(IPAddress addr, int port)
{
	if (con.connect(addr, port))
	return true;
	
	return false;
}

void recvMsg(char *buffer)
{
	memset(buffer, 0, BUFFER_SIZE);

	int i = 0;
	while (con.available())
		buffer[i++] = con.read();
}

void sendMsg(char *msg)
{
	con.write(msg, BUFFER_SIZE);
	delay(9900);
}

bool runClient(IPAddress addr, int port, char *msg, char *buffer)
{
	if (connect(addr, port)) {
		sendMsg(msg);
		recvMsg(buffer);
		delay(100);
		
		Serial.print(F("> "));
		Serial.println(buffer);
	}
	else {
		Serial.println(F("Failed to connect to server!"));
		delay(1000);
		
		con.stop();
		return false;
	}
	
	con.stop();
	delay(100);
	return true;
}

bool runServer(char *buffer)
{
	con = server.available();
	char msg[BUFFER_SIZE];

	if (con) {
		recvMsg(buffer);
		strcpy(msg, ACK);
		sendMsg(msg);

		delay(1000);
		con.stop();

		return true;
	}

	delay(100);
	return false;
}

void setup()
{
	Serial.begin(115200);
	while (!Serial)
		; // wait for serial port to connect

	// connect to WiFi network
	while (status != WL_CONNECTED) {
		Serial.print(F("Attempting to connect to WPA SSID: "));
		Serial.println(ssid);
		status = WiFi.begin(ssid, pass);
		delay(1000);
	}

	// print connection details
	Serial.println(F("You're connected to the network"));
	printWifiData();

	server.begin();
	state = 0;
}

void loop()
{
	runServer(buffer); // TODO don't leave this until it returns true

	// receives certificate from idp and reply with its own certificate
	if (state == 0 && strcmp(buffer, CERT_PLAIN) == 0) {
		Serial.print("State = ");
		Serial.println(state);

		Serial.println("Received CERT_PLAIN");
		strcpy(msg, CERT_SIG);
		runClient(idp_ip, idp_port, msg, buffer);
		state = 1; // TODO: insert a check to change state (received an ack?)
	}

	// receives shared key sp_c from idp
	else if (state == 1 && strcmp(buffer, SHARE_KEY) == 0) {
		Serial.print("State = ");
		Serial.println(state);

		Serial.println("Received SHARE_KEY");
		state = 2;
	}

	// receives service request from client and replies with service response
	else if (state == 2 && strcmp(buffer, SERV_REQ) == 0) {
		Serial.print("State = ");
		Serial.println(state);

		Serial.println("Received SERV_REQ");
		strcpy(msg, SERV_REP);
		runClient(client_ip, client_port, msg, buffer);
		state = 3;
	}

	delay(100);
}
