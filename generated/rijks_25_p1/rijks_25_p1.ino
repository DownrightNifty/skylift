/*
  PARALLEL ARTIFACTS
  https://ahprojects.com/parallel-artifacts
  Adam Harvey 2022
*/

// Auto-generated from: 

#include "display.h"

byte bssid[6] = {};        // store next bssid
uint8_t ncounter = 0;      // network counter
uint8_t ch_counter = 0;    // channel increment counter
uint8_t loop_counter = 0;  // loop counter interval for changing channel
uint8_t ssid_len = 0;      // holder for ssid length
uint8_t channel_pre = 0;
uint8_t packet_length = 0;

// beacon interval is 100.24 milliseconds (0x64)
unsigned long beacon_interval_us = 102400;
unsigned long last_beacon_us = 0;
char* hidden_ssid = "\x00";  // 0-length SSID (Network name unavailable)
#define NUM_TS_PLACES 8
byte time_packet[NUM_TS_PLACES];  // placeholder for timestamp data#define NUM_TS_PLACES 8
char cc[2];                       // placeholer for timestamp data

float WIFI_POWER_DBM = 19.5;  // ESP8266 only




// ---------------------------------------------------------
// START template data
// ---------------------------------------------------------
#define ESP32 1

#ifdef ESP32
#include "WiFi.h"
extern "C" {
#include "esp_wifi.h"
  esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);
  esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void* buffer, int len, bool en_sys_seq);
}
#else
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}
#endif

// #define USE_HIDDEN_SSIDS

#define NN 25
char* ssids[NN] = {
	"RIJKS SA", 
	"RIJKS Data", 
	"RIJKS WiFi", 
	"eduroam", 
	"RIJKS IoT", 
	"RIJKS Data", 
	"RIJKS WiFi", 
	"count", 
	"", 
	"count", 
	"RIJKS WiFi", 
	"RIJKS IoT", 
	"count", 
	"RIJKS WiFi", 
	"Galaxy Note10+abed", 
	"count", 
	"", 
	"RIJKS SA", 
	"RIJKS Data", 
	"eduroam", 
	"Rania's phone", 
	"RIJKS IoT", 
	"eduroam", 
	"count", 
	"eduroam", 
};
uint8_t ssid_lengths[NN] = {
	8, 
	10, 
	10, 
	7, 
	9, 
	10, 
	10, 
	5, 
	0, 
	5, 
	10, 
	9, 
	5, 
	10, 
	18, 
	5, 
	0, 
	8, 
	10, 
	7, 
	13, 
	9, 
	7, 
	5, 
	7, 
};

byte bssids[NN][6] = {
	{0x10, 0xBD, 0x18, 0x5E, 0x29, 0x86}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xA4, 0x6F}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xA4, 0x6E}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xBA, 0xA9}, 
	{0x08, 0x7B, 0x87, 0x88, 0x91, 0x2D}, 
	{0x08, 0x7B, 0x87, 0x88, 0x91, 0x2F}, 
	{0x08, 0x7B, 0x87, 0x88, 0x91, 0x2E}, 
	{0x08, 0x7B, 0x87, 0x88, 0x91, 0x23}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xA4, 0x64}, 
	{0x18, 0x33, 0x9D, 0xF1, 0x57, 0xD3}, 
	{0x10, 0xBD, 0x18, 0x5E, 0x53, 0xDE}, 
	{0x18, 0x33, 0x9D, 0xF1, 0x53, 0x4D}, 
	{0x18, 0x33, 0x9D, 0x71, 0xDF, 0xDC}, 
	{0x08, 0x7B, 0x87, 0x88, 0x91, 0x21}, 
	{0x02, 0x6E, 0x88, 0x82, 0x4F, 0x38}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xB3, 0xDC}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xB4, 0x74}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xA2, 0xB6}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xB7, 0xB0}, 
	{0x18, 0x33, 0x9D, 0xC7, 0xBE, 0x77}, 
	{0x16, 0xBB, 0x6D, 0xDE, 0x45, 0xD4}, 
	{0x18, 0x33, 0x9D, 0x71, 0xDF, 0xD2}, 
	{0x18, 0x33, 0x9D, 0xF1, 0x53, 0x48}, 
	{0x10, 0xBD, 0x18, 0x5E, 0x53, 0xDC}, 
	{0x18, 0x33, 0x9D, 0xF1, 0x57, 0xD8}, 
};

#define N_CHANNELS 13
byte channels[N_CHANNELS] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

// {{TEMPLATE:DBM_LEVELS
// TODO
// TEMPLATE:DBM_LEVELS}}

// ---------------------------------------------------------
// END template data
// ---------------------------------------------------------


// Variables referencing templated data
uint8_t channel = channels[0];  // Transmit on this channel
unsigned long ap_epochs[NN];

// Beacon frame packet structure
uint8_t packet[128] = {
  /*0*/ 0x80, 0x00,                                       // Frame Control
  /*2*/ 0x00, 0x00,                                       // Duration
  /*4*/ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,               // Destination address
  /*10*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              // Source address - overwritten later
  /*16*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,              // BSSID - overwritten to the same as the source address
  /*22*/ 0xc0, 0x6c,                                      // Seq-ctl
  /*24*/ 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,  // Timestamp - the number of microseconds the AP has been active
  /*32*/ 0x64, 0x00,                                      // Beacon interval = 0.102400 seconds
                                                          // /*34*/ 0x01, 0x04,                                    // Capability info
  /*34*/ 0x31, 0x14,                                      // Capability inf
  /*36*/ 0x00
  /* SSID is added after here */
};

uint8_t packet_tail[13] = {
  0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,  // supported rate
  0x03, 0x01, 0x04                                             // DSSS (Current Channel)
};


// ---------------------------------------------------------------
// Convert milliseconds to timestamp for beacon frame
// ---------------------------------------------------------------
void get_timestamp(char* times) {

  long t = micros();
  unsigned int tlen = 8;
  String time_hex_forward = String(t, HEX);

  while (time_hex_forward.length() < tlen * 2) time_hex_forward = "0" + time_hex_forward;
  String time_hex_rev = time_hex_forward;  // reversed
  for (int i = tlen; i > 0; i--) {
    time_hex_rev[(tlen * 2) - (i * 2)] = time_hex_forward.charAt((i * 2) - 2);      // 1
    time_hex_rev[(tlen * 2) - (i * 2) + 1] = time_hex_forward.charAt((i * 2) - 1);  // 2
  }
  // convert to 8-length byte array
  for (int i = 0; i < tlen; i++) {

    cc[0] = time_hex_rev.charAt(i * 2);
    cc[1] = time_hex_rev.charAt((i * 2) + 1);
    long hex_val = strtol(cc, NULL, 16);
    times[i] = hex_val;
  }
}

void set_timestamp(byte* p, byte n) {
  // return time in reverse order as byte array pointer
  long t = millis() + ap_epochs[n];  // random start time
  p[0] = (t & 0x00000000000000FF);
  p[1] = (t & 0x000000000000FF00) >> 8;
  p[2] = (t & 0x0000000000FF0000) >> 16;
  p[3] = (t & 0x00000000FF000000) >> 24;
  p[4] = (t & 0x000000FF00000000) >> 32;
  p[5] = (t & 0x0000FF0000000000) >> 40;
  p[6] = (t & 0x00FF000000000000) >> 48;
  p[7] = (t & 0x00FF000000000000) >> 56;
}


long strtol(const char* __nptr, char** __endptr, int __base);  // additional function for string to long conversion


// ---------------------------------------------------------------
// Send the beacon frame
// ---------------------------------------------------------------
void send_beacon_frame(uint8_t i, uint8_t c) {

  // Set BSSID
  memcpy(&bssid, bssids[i], 6);
  memcpy(&packet[10], bssid, 6);
  memcpy(&packet[16], bssid, 6);

  // Set SSID
  ssid_len = ssid_lengths[i];
  packet[37] = ssid_len;
  memcpy(&packet[38], ssids[i], ssid_len);
  memcpy(&packet[38 + ssid_len], packet_tail, 13);
  packet[50 + ssid_len] = channels[c];

  // store new packet length
  packet_length = 51 + ssid_len;

  // Send the packet
#ifdef ESP32
  esp_wifi_80211_tx(WIFI_IF_STA, packet, packet_length, 0);  // esp32
#else
  wifi_send_pkt_freedom(packet, packet_length, 0);  // esp8266
#endif
}


void change_channel(uint8_t i) {
#ifdef ESP32
  // esp_wifi_set_promiscuous(false);
  esp_wifi_set_channel(channels[i], WIFI_SECOND_CHAN_NONE);  // esp32
  // esp_wifi_set_promiscuous(true);
#else
  wifi_set_channel(channels[i]);                    // esp8266
#endif
}


// ---------------------------------------------------------------
// Initialize the ESP8266
// Set max power and put into promiscious mode
// ---------------------------------------------------------------
void setup() {
  setup_display();
#ifdef ESP32
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setTxPower(WIFI_POWER_19dBm);
  esp_wifi_set_channel(channels[0], WIFI_SECOND_CHAN_NONE);
#else
  WiFi.setOutputPower(WIFI_POWER_DBM);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
#endif
}

// ---------------------------------------------------------------
// Transmit Beacon Frame
// ---------------------------------------------------------------
void loop() {
  // Limit to every 100ms
  if (micros() - last_beacon_us < beacon_interval_us) {
    return;
  }

  // change channel every loop
  loop_counter += 1;
  if (loop_counter > 10) {
    ch_counter = (ch_counter + 1) % N_CHANNELS;
    change_channel(ch_counter);
    loop_counter = 0;
  }
  
  // Send all ssid packets
  for (int i = 0; i < NN; i++) {
    send_beacon_frame(i, ch_counter);
    delay(1);
  }

  // update time of broadcast
  last_beacon_us = micros();

  // display
  run_display();
}