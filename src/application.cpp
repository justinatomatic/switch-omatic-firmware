/**
 ******************************************************************************
 * @file    application.cpp
 * @authors Justin Maynard
 * @version V0.2.1
 * @date    29-July-2014
 * @brief   Wireless SmartHome Buttons
 ******************************************************************************

Todo
  Support for orientation and remap button numbers
  Support for larger button pads

 ******************************************************************************
  Copyright (c) 2014 Omatic Labs, Inc.  All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/  
#include "application.h"
#include "PubSubClient.h"
#include "Spark_NeoPixel.h"
#include "clickButton.h"

SYSTEM_MODE(MANUAL);

// Configuration Section 

#define BROKER_IP_ADDRESS { 192, 168, 23, 20 }
#define MQTT_CLIENT_NAME "SparkButtons_Study"
#define MQTT_WILL_TOPIC "client/status"
#define MQTT_WILL_MESSAGE "Study Buttons Fail"

String buttonpad_name = "Study switch-omatic ";
String colourTopic = "study/buttons";   
String buttonTopic = "study/buttons";  


// Power Cable Orientation
// 1 = down 2 = up 3 = left 4 = right
#define ORIENTATION 1

// End Configuration Section 

// the Buttons
#if ORIENTATION == 1
  const int buttonPin1 = 3;
  const int buttonPin2 = 2;
  const int buttonPin3 = 4;
  const int buttonPin4 = 1;
#endif
#if ORIENTATION == 2
  const int buttonPin1 = 1;
  const int buttonPin2 = 4;
  const int buttonPin3 = 2;
  const int buttonPin4 = 3;
#endif
#if ORIENTATION == 3
  const int buttonPin1 = 4;
  const int buttonPin2 = 3;
  const int buttonPin3 = 1;
  const int buttonPin4 = 2;
#endif
#if ORIENTATION == 4
  const int buttonPin1 = 2;
  const int buttonPin2 = 1;
  const int buttonPin3 = 3;
  const int buttonPin4 = 4;
  
#endif

ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);
ClickButton button2(buttonPin2, LOW, CLICKBTN_PULLUP);
ClickButton button3(buttonPin3, LOW, CLICKBTN_PULLUP);
ClickButton button4(buttonPin4, LOW, CLICKBTN_PULLUP);
char buttonstatestring[5] = "";

// allow us to use itoa() in this scope
extern char* itoa(int a, char* buffer, unsigned char radix);

// LED's 
#define NEOPIN D0
Adafruit_NeoPixel LED = Adafruit_NeoPixel(4, NEOPIN, WS2812);

// MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    // handle message arrived
  payload[length] = '\0';
  char* cstring = (char *) payload;
  long n = atol(cstring);

  String callbackTopic = String(topic);
  String TopicString = String(colourTopic + "/1/colour");
  if (callbackTopic == TopicString) {
    LED.setPixelColor(buttonPin1-1,n);
  }
  TopicString = String(colourTopic + "/2/colour");
  if (callbackTopic == TopicString) {
    LED.setPixelColor(buttonPin2-1,n);
  }
  TopicString = String(colourTopic + "/3/colour");
  if (callbackTopic == TopicString) {
    LED.setPixelColor(buttonPin3-1,n);
  }
  TopicString = String(colourTopic + "/4/colour");
  if (callbackTopic == TopicString) {
    LED.setPixelColor(buttonPin4-1,n);
  }
  LED.show();
}

byte server[] = BROKER_IP_ADDRESS;
TCPClient tcpClient;
PubSubClient client(server, 1883, callback, tcpClient);

 
void setup() {
  WiFi.connect();
  while(WiFi.ready() == false) {
    delay(100);
  }

  // NeoPixels
  pinMode(NEOPIN,OUTPUT);
  // Buttons
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register
  button2.debounceTime   = 20;   // Debounce timer in ms
  button2.multiclickTime = 250;  // Time limit for multi clicks
  button2.longClickTime  = 1000; // time until "held-down clicks" register
  button3.debounceTime   = 20;   // Debounce timer in ms
  button3.multiclickTime = 250;  // Time limit for multi clicks
  button3.longClickTime  = 1000; // time until "held-down clicks" register
  button4.debounceTime   = 20;   // Debounce timer in ms
  button4.multiclickTime = 250;  // Time limit for multi clicks
  button4.longClickTime  = 1000; // time until "held-down clicks" register

  // Setup LED's
  LED.begin();
  LED.show();
  LED.setPixelColor(0,0,0,255);
  LED.setPixelColor(1,0,255,0);
  LED.setPixelColor(2,255,0,0);
  LED.setPixelColor(3,255,255,0);
  LED.show();
  
  // Start MQTT Connections
  client.connect(MQTT_CLIENT_NAME,MQTT_WILL_TOPIC,0,0,MQTT_WILL_MESSAGE);
  String TopicString = String(colourTopic + "/1/colour");
  char TopicChar[TopicString.length()+1];
  TopicString.toCharArray(TopicChar, TopicString.length()+1);
  client.subscribe(TopicChar);
  TopicString = String(colourTopic + "/2/colour");
  TopicString.toCharArray(TopicChar, TopicString.length()+1);
  client.subscribe(TopicChar);
  TopicString = String(colourTopic + "/3/colour");
  TopicString.toCharArray(TopicChar, TopicString.length()+1);
  client.subscribe(TopicChar);
  TopicString = String(colourTopic + "/4/colour");
  TopicString.toCharArray(TopicChar, TopicString.length()+1);
  client.subscribe(TopicChar);
  
  String startString = String(buttonpad_name + " Start");
  char startChar[startString.length()+1];
  startString.toCharArray(startChar, startString.length()+1);
  client.publish(MQTT_WILL_TOPIC,startChar);
  
}

void loop() {
  // Update button state
  button1.Update();
  button2.Update();
  button3.Update();
  button4.Update();

  if (button1.clicks != 0) {
    itoa(button1.clicks, buttonstatestring, 10);
    String TopicString = String(buttonTopic + "/1/state");
    char TopicChar[TopicString.length()+1];
    TopicString.toCharArray(TopicChar, TopicString.length()+1);
    client.publish(TopicChar,buttonstatestring);
  }
  if (button2.clicks != 0) {
    itoa(button2.clicks, buttonstatestring, 10);
    String TopicString = String(buttonTopic + "/2/state");
    char TopicChar[TopicString.length()+1];
    TopicString.toCharArray(TopicChar, TopicString.length()+1);
    client.publish(TopicChar,buttonstatestring);
  } 
  if (button3.clicks != 0){
    itoa(button3.clicks, buttonstatestring, 10);
    String TopicString = String(buttonTopic + "/3/state");
    char TopicChar[TopicString.length()+1];
    TopicString.toCharArray(TopicChar, TopicString.length()+1);
    client.publish(TopicChar,buttonstatestring);
  } 
  if (button4.clicks != 0){
    itoa(button4.clicks, buttonstatestring, 10);
    String TopicString = String(buttonTopic + "/4/state");
    char TopicChar[TopicString.length()+1];
    TopicString.toCharArray(TopicChar, TopicString.length()+1);
    client.publish(TopicChar,buttonstatestring);
  } 
   
if (client.connected ()){
     client.loop();
  }
  else {
    if (client.connect(MQTT_CLIENT_NAME,MQTT_WILL_TOPIC,0,0,MQTT_WILL_MESSAGE)) {
        String statusString = String(buttonpad_name + " ReStart");
        char statusChar[statusString.length()+1];
        statusString.toCharArray(statusChar, statusString.length()+1);
        client.publish(MQTT_WILL_TOPIC,statusChar);
    }
    else {
      // Don't flood the broker with connections and it's already down so we don't need to worry about a delay disconnecting the client
      delay(15*1000);
    }
  }
}


