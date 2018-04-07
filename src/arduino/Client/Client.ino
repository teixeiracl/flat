#include <string.h>

#include <SPI.h>
#include <WiFi.h>

#include "../cofee.h"

// variables for WiFi connection
char ssid[] = "ssid";
char pass[] = "password";
int status = WL_IDLE_STATUS;

// IPAddress idp_ip(192, 168, 1, 146);
// IPAddress sp_ip(192, 168, 1, 162);
IPAddress idp_ip(192, 168, 0, 15);
IPAddress sp_ip(192, 168, 0, 18);

WiFiClient con;
WiFiServer server(12333);

unsigned int sp_port = 80;
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
	// sends key request to idp
	if (state == 0) {
		Serial.print("State = ");
		Serial.println(state);

		strcpy(msg, KEY_REQ);
		runClient(idp_ip, idp_port, msg, buffer);
		state = 1; // TODO: insert a check to change state (received an ack?)
	}
	
	runServer(buffer); // TODO don't leave this until it returns true if state == 1

	// receives key response from idp and sends assertion to idp
	if (state == 1 && strcmp(buffer, KEY_REP) == 0) {
		Serial.print("State = ");
		Serial.println(state);

		Serial.println("Received KEY_REP");
		strcpy(msg, ASSERT_REQ);
		runClient(idp_ip, idp_port, msg, buffer);
		state = 2;
	}
	
	// receives assert responde and sends service request to sp
	else if (state == 2 && strcmp(buffer, ASSERT_REP) == 0) {
		Serial.print("State = ");
		Serial.println(state);

		Serial.println("Received ASSERT_REP");
		strcpy(msg, SERV_REQ);
		runClient(sp_ip, sp_port, msg, buffer);
		state = 3;
	}
	
	// receives service response from sp
	else if (state == 3 && strcmp(buffer, SERV_REP) == 0) {
		Serial.print("State = ");
		Serial.println(state);

		Serial.println("Received SERV_REP. Success!");
		state = 4;
	}

	
	delay(100);
}
