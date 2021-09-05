// --------------------------------------------------
//
// Code setting up a simple webserver on the ESP32. 
// device used for tests: ESP32-WROOM-32D
//
// Install Websockets by Markus Sattler
//
// Written by mo thunderz (last update: 05.09.2021)
//
// --------------------------------------------------

#include <WiFi.h>
#include <WebServer.h> 
#include <WebSocketsServer.h>  // Include Websocket Library

// SSID and password of Wifi connection:
const char* ssid = "TYPE_YOUR_SSID_HERE";
const char* password = "TYPE_YOUR_PASSWORD_HERE";

String website = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Lets generate a random number</h1><p>The random number is: <span id='rand'>-</span></p></span></body><script> var Socket; function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function processCommand(event) { document.getElementById('rand').innerHTML = event.data; console.log(event.data);Socket.send('Sending back some random stuff'); } window.onload = function(event) { init(); }</script></html>";

int randomval = random(100);
int interval = 5000; // virtual delay
unsigned long previousMillis = 0; // Tracks the time since last event fired

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81); 

void setup() {
  Serial.begin(115200);                 
 
  WiFi.begin(ssid, password);
  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected to network with IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", []() {
  server.send(200, "text\html", website);
  });
  server.begin(); // init the server
  
  webSocket.begin(); 
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();  // webserver methode that handles all Client
  webSocket.loop(); 
  
  unsigned long currentMillis = millis(); // call millis  and Get snapshot of time
  if ((unsigned long)(currentMillis - previousMillis) >= interval) { // How much time has passed, accounting for rollover with subtraction!
    String str = String(random(100)); 
    int str_len = str.length() + 1; 
    char char_array[str_len];
    str.toCharArray(char_array, str_len);
    webSocket.broadcastTXT(char_array);
    previousMillis = currentMillis;
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: // enum that read status this is used for debugging.
      Serial.println("Client Disconnected");
      break;
    case WStype_CONNECTED:  // Check if a WebSocket client is connected or not
      Serial.println("Client Connected");
      // add code here what to do when connected
      
      break;
    case WStype_TEXT: // check response from client
      for (int i=0; i<length; i++) {    
        Serial.print((char)payload[i]);
      }
      Serial.println("");
      break;
  }
}
