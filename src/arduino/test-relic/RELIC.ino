#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <WiFi.h>

extern "C" {
#include "../include/relic.h"
}

/**************************
 * n = 20, w = 2, t1 = 80,  t2 = 5, t = 85
 * n = 20, w = 4, t1 = 40,  t2 = 3, t = 43
 * n = 20, w = 8, t1 = 20,  t2 = 2, t = 22
 * n = 28, w = 2, t1 = 112, t2 = 5, t = 117
 * n = 28, w = 4, t1 = 56,  t2 = 3, t = 59
 * n = 28, w = 8, t1 = 28,  t2 = 2, t = 30
 * n = 32, w = 2, t1 = 128, t2 = 5, t = 133
 * n = 32, w = 4, t1 = 64,  t2 = 3, t = 67
 * n = 32, w = 8, t1 = 32,  t2 = 2, t = 34
 * n = 48, w = 2, t1 = 192, t2 = 5, t = 197
 * n = 48, w = 4, t1 = 96,  t2 = 3, t = 99
 * n = 48, w = 8, t1 = 48,  t2 = 2, t = 50
 * n = 64, w = 2, t1 = 256, t2 = 6, t = 262
 * n = 64, w = 4, t1 = 128, t2 = 3, t = 131
 * n = 64, w = 8, t1 = 64,  t2 = 2, t = 66
 ***************************/

#define HASH_SIZE_BYTES 32
#define W_PARAM 2
#define T_1 128
#define T_2 5
#define T_PARAM 133

char ssid[] = "insecure";
char pass[] = "h!!p.\\\\mmm:dcc:ufwg:br";
int status = WL_IDLE_STATUS;

void gen_keys(uint8_t priv[T_PARAM][HASH_SIZE_BYTES],
    uint8_t pub[T_PARAM][HASH_SIZE_BYTES]) {

  // Allocating space size t for keys
  bn_t priv_bnt[T_PARAM];

  for (short i = 0; i < T_PARAM; ++i) {

    bn_null(priv_bnt[i]); // initializes a multiple precision int with null
    bn_new(priv_bnt[i]); // allocates and initializes a multiple precision int
    bn_rand(priv_bnt[i], BN_POS, HASH_SIZE_BYTES*8); // assigns a random value to a multiple precision int
    bn_write_bin(priv[i],HASH_SIZE_BYTES,priv_bnt[i]); // saves priv_bnt[i] in priv[i]

    // Hashing private key 2^w - 1 times for public key
    uint8_t times = (int) (pow(2.0,W_PARAM) - 1.0);

    uint8_t aux[HASH_SIZE_BYTES];
    bn_write_bin(aux,HASH_SIZE_BYTES,priv_bnt[i]); // writes priv[i] in aux in big-endian
    for (uint8_t j = 0; j < times; ++j) {

#if HASH_SIZE_BYTES == 20
      md_map_shone(aux,aux,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 28
      md_map_sh224(aux,aux,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 32
      md_map_sh256(aux,aux,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 48
      md_map_sh384(aux,aux,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 64
      md_map_sh512(aux,aux,HASH_SIZE_BYTES);

#endif
    }
    memcpy(pub[i],aux,HASH_SIZE_BYTES); // copying the hashed aux into pub[i]
  }
}

void calc_bs(uint8_t *msg, int len, unsigned long *bs) {
  // Calculating digest of msg
  uint8_t dgst[HASH_SIZE_BYTES];

#if HASH_SIZE_BYTES == 20
  md_map_shone(dgst,msg,len);

#elif HASH_SIZE_BYTES == 28
  md_map_sh224(dgst,msg,len);

#elif HASH_SIZE_BYTES == 32
  md_map_sh256(dgst,msg,len);

#elif HASH_SIZE_BYTES == 48
  md_map_sh384(dgst,msg,len);

#elif HASH_SIZE_BYTES == 64
  md_map_sh512(dgst,msg,len);

#endif

  // Calculating first t1 bit strings of size w from dgst and checksum
  bn_t temp; // auxiliar var to get w bits from dgst
  bn_null(temp);
  bn_new(temp);
  bn_read_bin(temp,dgst,HASH_SIZE_BYTES);

  unsigned int checksum = 0;
  short c = (int) pow(2.0,W_PARAM); // checksum is c - b[i]

  for (short i = 0; i < T_1; ++i) {
    bs[i] = 0;

    // Getting w bits from digest to save in b[i]
    for (uint8_t j = 0; j < W_PARAM; ++j) {
      int ith = bn_get_bit(temp,(W_PARAM*i + j)); // get ith bit
      ith <<= j; // shifting ith to j position
      bs[i] |= ith; // placing new bit
    }
    checksum += c - bs[i];
  }

  // Calculating remaining t2 bit strings from dgst
  unsigned long mask; // gets w first bits

  if (W_PARAM == 2) {
    mask = 0x00000003;
  }
  else if (W_PARAM == 4) {
    mask = 0x0000000f;
  }
  else if (W_PARAM == 8) {
    mask = 0x000000ff;
  }

  for (short i = T_1; i < T_PARAM; ++i) {
    bs[i] = checksum & mask;
    bs[i] %= W_PARAM;
    mask <<= W_PARAM; // shift mask w bits to get next sequence
  }
}

void gen_sig(uint8_t priv[T_PARAM][HASH_SIZE_BYTES], unsigned long *bs,
    uint8_t sig[][HASH_SIZE_BYTES]) {
  // Calculating the signature
  for (short i = 0; i < T_PARAM; ++i) {
    memcpy(sig[i],priv[i],HASH_SIZE_BYTES);
    for (int j = 0; j < bs[i]; ++j) {
#if HASH_SIZE_BYTES == 20
      md_map_shone(sig[i],sig[i],HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 28
      md_map_sh224(sig[i],sig[i],HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 32
      md_map_sh256(sig[i],sig[i],HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 48
      md_map_sh384(sig[i],sig[i],HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 64
      md_map_sh512(sig[i],sig[i],HASH_SIZE_BYTES);

#endif
    }
  }
}

int check_sig(uint8_t pub[T_PARAM][HASH_SIZE_BYTES],
    uint8_t sig[T_PARAM][HASH_SIZE_BYTES], unsigned long *bs) {

  uint8_t check[HASH_SIZE_BYTES]; // aux var to hash the sig

  for (short i = 0; i < T_PARAM; ++i) {
    unsigned long times = pow(2.0,W_PARAM) - 1 - bs[i];
    memcpy(check,sig[i],HASH_SIZE_BYTES);
    for (int j = 0; j < times; ++j) {

#if HASH_SIZE_BYTES == 20
      md_map_shone(check,check,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 28
      md_map_sh224(check,check,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 32
      md_map_sh256(check,check,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 48
      md_map_sh384(check,check,HASH_SIZE_BYTES);

#elif HASH_SIZE_BYTES == 64
      md_map_sh512(check,check,HASH_SIZE_BYTES);

#endif
    }
    if(memcmp(check,pub[i],HASH_SIZE_BYTES) != 0) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

void printWifiData() {
	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);
	Serial.println(ip);
  
	// print your MAC address:
	byte mac[6];
	WiFi.macAddress(mac);
	Serial.print("MAC address: ");
	Serial.print(mac[5], HEX);
	Serial.print(":");
	Serial.print(mac[4], HEX);
	Serial.print(":");
	Serial.print(mac[3], HEX);
	Serial.print(":");
	Serial.print(mac[2], HEX);
	Serial.print(":");
	Serial.print(mac[1], HEX);
	Serial.print(":");
	Serial.println(mac[0], HEX);
  
}
  
void printCurrentNet() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
  
	// print the MAC address of the router you're attached to:
	byte bssid[6];
	WiFi.BSSID(bssid);
	Serial.print("BSSID: ");
	Serial.print(bssid[5], HEX);
	Serial.print(":");
	Serial.print(bssid[4], HEX);
	Serial.print(":");
	Serial.print(bssid[3], HEX);
	Serial.print(":");
	Serial.print(bssid[2], HEX);
	Serial.print(":");
	Serial.print(bssid[1], HEX);
	Serial.print(":");
	Serial.println(bssid[0], HEX);
  
	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.println(rssi);
  
	// print the encryption type:
	byte encryption = WiFi.encryptionType();
	Serial.print("Encryption Type:");
	Serial.println(encryption, HEX);
	Serial.println();
}

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  // initializing RELIC
  core_init();
  pc_param_set_any();
}

void loop() {
  // Variables for signature and keys
  uint8_t priv[T_PARAM][HASH_SIZE_BYTES];
  uint8_t pub[T_PARAM][HASH_SIZE_BYTES];
  uint8_t sig[T_PARAM][HASH_SIZE_BYTES];

  // Message variable
  uint8_t msg[HASH_SIZE_BYTES*8];

  // Checksum variable
  unsigned long bs[T_PARAM];

  // Variable for measuring time
  unsigned long start = micros();

  // Generating keys
  gen_keys(priv,pub);

  unsigned long end = micros();
  unsigned long delta = end - start;
  Serial.print("KG\t");
  Serial.println(delta);

  // Generating random message
  rand_init();
  rand_seed(msg,HASH_SIZE_BYTES*8);
  rand_bytes(msg,HASH_SIZE_BYTES*8);

  start = micros();

  // Calculating checksum
  calc_bs(msg,HASH_SIZE_BYTES*8,bs);

  // Signing
  gen_sig(priv,bs,sig);

  end = micros();
  delta = end - start;
  Serial.print("MS\t");
  Serial.println(delta);

  start = micros();

  // Verifying the signature
  int result = check_sig(pub,sig,bs);

  end = micros();
  delta = end - start;
  Serial.print("SC\t");
  Serial.println(delta);

  // Finishing and cleaning up
  rand_clean();
  core_clean();
}
