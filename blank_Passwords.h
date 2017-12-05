/*
 * Passwords.h
 *
 *
 *	This is a blank example of what Passwords.h should look like.
 *
 *  Created on: 5 Dec 2017
 *      Author: Alan Childs
 */

#ifndef PASSWORDS_H_
#define PASSWORDS_H_


//****       WiFi STUFF    *****   (You can add your own WiFi here)
// XXX: ESP8266WiFiMulti.cpp has a bug of sorts; 64Char HEX Keys require a libs edit. 63Char PSK's are fine. ~Percy)
#define ADDWIFIAPS \
wifiMulti.addAP("SSID1", "passphrase"); \
wifiMulti.addAP("SSID2", "passphrase"); \



//****       Slack STUFF    *****
#define SLACK_BOT_TOKEN "????-??????????????????"
#define SLACK_SSL_FINGERPRINT "AC 95 5A 58 B8 4E 0B CD B3 97 D2 88 68 F5 CA C1 0A 81 E3 6E"  // if Slack changes their SSL (SHA1 fingerprint, you would need to update this:






#endif /* PASSWORDS_H_ */
