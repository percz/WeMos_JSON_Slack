/*
 *
 *  Lots of fairly basic (reference) examples of getting Slack to work with a ESP unit, via JSON
 *
 *  Created on: 5 Dec 2017
 *      Author: Alan Childs
 *      This is mainly public domain stuff
 */


//Includes...
// ...this....
#include "WeMos_JSON_Slack.h" // Do not remove!

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <LiquidCrystal.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
// ..build...
#include "wemos_pins.h" //Map to the right pins, in case we change devices
#include "Passwords.h" //XXX: Make sure this doesn't sync to anything!


LiquidCrystal lcd(D8,D9,D4,D5,D6,D7); //WeMos Pinout
ESP8266WiFiMulti wifiMulti; //Note that this creates a WiFiClient called "WiFi" that'll be the /real/ object...
WebSocketsClient webSocket;

bool slackConnected = false; //We haven't got that far yet...
bool displayedConnected = false; //TODO: Take this out


bool connectToSlack() {
  // Step 1: Find WebSocket address via RTM API (https://api.slack.com/methods/rtm.start)
  HTTPClient http;
  //Hackspace Captive Portal ---    http://192.168.1.78/portal/entry?clientMac=60:01:94:4E:F7:A3
  http.begin("https://slack.com/api/rtm.start?token=" SLACK_BOT_TOKEN, SLACK_SSL_FINGERPRINT);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP GET failed with code %d\n", httpCode);
    return false;
  }
  WiFiClient *WiFi = http.getStreamPtr(); //HAVE to use "WiFi" as it's set inside the libs
  WiFi->find("wss:\\/\\/");
  String host = WiFi->readStringUntil('\\');
  String path = WiFi->readStringUntil('"');
  path.replace("\\/", "/");
  if (host == "") Serial.println("[WebSocket] Invalid Auth"); //TODO: Make this a debug-compile option
  Serial.println("[WebSocket] Host=" + host);
  webSocket.beginSSL(host, 443, path, "", "");
  webSocket.onEvent(webSocketEvent);
  return true;
}


void webSocketEvent(WStype_t type, uint8_t *payload, size_t len) { //TODO: This flags up with warnings when I compile in Eclipse ~Percy
  switch (type) {
     case WStype_DISCONNECTED:
      Serial.printf("[WebSocket] Disconnected :-( \n");
      slackConnected = false;
      break;

    case WStype_CONNECTED:
      Serial.printf("[WebSocket] Connected to: %s\n", payload);
      break;

    case WStype_TEXT:
      Serial.printf("[WebSocket] Message: %s\n", payload);
      processSlackMessage((char*)payload);
      break;
  }
}

void processSlackMessage(char *payload) {
  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(payload);
  if (!root.success())
  {
    Serial.println("[JSON] parseObject() failed");
  }
  else if (root["type"] == "message") {
	String chanCode = root["channel"];
	//char chanType = chanCode[0]; //D = Direct Message, C = Channel
	String msgText = root["text"];
    if ( msgText.indexOf(" Boo! ") >= 0 ) helloWorld(chanCode);
    if ( msgText.indexOf(" heat") >= 0 ||
         msgText.indexOf(" Heat") >= 0 ||
		 msgText.indexOf("Heat") >= 0 ||
		 msgText.indexOf("Heating") >= 0
       ) heating(chanCode, msgText);
  }
  else if (root["type"] == "presence_change") {
	    String msgText = root["text"];
		lcd.clear();
	    lcd.setCursor(0,0);
	    String msg = root["user"];
	    lcd.print(msg);
	    lcd.setCursor(0,1);
	    if (root["presence"] == "active") {
	    	lcd.print(" Online");
	    }
	    else //if ( root["presence"] == "away" )
	    {
	    	lcd.print(" Away");

	    }
  }
}


void helloWorld(String chanCode) {   //Just a tester
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "message";
  root["channel"] = chanCode; // Reply to whom said Boo
  root["text"] = "Are you trying to scare me? I don't do that anymore.";
  String json; //Convert back to a single string, so we can send in a big stream dump
  root.printTo(json);
  webSocket.sendTXT(json);
}

void heating(String chanCode, String msgText) {   //Really ugly, really basic, excuse of Natural Language Processing
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "message";
  root["channel"] = chanCode;
  //char chanType = chanCode[0]; //D = Direct Message, C = Channel
  if ( msgText.indexOf(" on ") >= 0 ) {
	  root["text"] = "I've pushed the switch in with my long broom stick for you. We have warmth!";
  }
  if ( msgText.indexOf(" off ") >= 0 ) {
	  root["text"] = "You leaving already? Don't forget to turn off the lights. I can't do that yet.";
  }
  else {
	  root["text"] = "Do you want me to do something with the heating? Just ask!";
  }
  String json; //Convert back to a single string, so we can send in a big stream dump
  root.printTo(json);
  webSocket.sendTXT(json);
}

void setup() {
  ADDWIFIAPS; //Use a Macro to add the AP's that are hidden in a Passwords.h file....
  wifiMulti.addAP("SSID", "passphrase");   //....or just add them like this.
  Serial.begin(115200);
  lcd.begin(16, 2);
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov"); //Probs needed for SSL?
}



void loop() {
  webSocket.loop();
  if (!slackConnected) {



	  	//TODO: Improve the WiFi is connected feedback, probs put it into a Function.
	    if(wifiMulti.run() != WL_CONNECTED) {
	    	displayedConnected = false;
	        Serial.println("WiFi not connected!");
	        delay(1000);
	    } else if (!displayedConnected) {
	    	displayedConnected = true;
	        Serial.println("");
	        Serial.println("WiFi connected");
	        Serial.println("IP address: ");
	        Serial.println(WiFi.localIP());
	    }




      delay(500);
      //connectWifi(); //TODO:
      slackConnected = connectToSlack();
  }
}
