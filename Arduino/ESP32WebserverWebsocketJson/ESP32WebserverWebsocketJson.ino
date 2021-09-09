// --------------------------------------------------
//
// Code setting up a simple webserver on the ESP32. 
// device used for tests: ESP32-WROOM-32D
//
// Install the following libraries:
// Websockets by Markus Sattler
// ArduinoJson
//
// Written by mo thunderz (last update: 05.09.2021)
//
// --------------------------------------------------

#include <WiFi.h>
#include <WebServer.h> 
#include <WebSocketsServer.h>  // Include Websocket Library
#include <ArduinoJson.h> // Include ArduinoJson Library

// SSID and password of Wifi connection:
const char* ssid = "TYPE_YOUR_SSID_HERE";
const char* password = "TYPE_YOUR_PASSWORD_HERE";

String website = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Lets generate a random number</h1><p>The first random number is: <span id='rand1'>-</span></p><p>The second random number is: <span id='rand2'>-</span></p><p><button type='button' id='BTN_SEND_BACK'>Send info to ESP32</button></p></span></body><script> var Socket; document.getElementById('BTN_SEND_BACK').addEventListener('click', button_send_back); function init() { Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event) { processCommand(event); }; } function button_send_back() { var msg = {brand: 'Gibson',type: 'Les Paul Studio',year: 2010,color: 'white'};Socket.send(JSON.stringify(msg)); } function processCommand(event) {var obj = JSON.parse(event.data);document.getElementById('rand1').innerHTML = obj.rand1;document.getElementById('rand2').innerHTML = obj.rand2; console.log(obj.rand1);console.log(obj.rand2); } window.onload = function(event) { init(); }</script></html>";
StaticJsonDocument<200> doc_tx;
StaticJsonDocument<200> doc_rx;

int randomval = random(100);
int interval = 1000; 
unsigned long previousMillis = 0; 

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
    
    String jsonString = "";
    JsonObject object = doc_tx.to<JsonObject>();
    object["rand1"] = random(100);
    object["rand2"] = random(100);
    serializeJson(doc_tx, jsonString); 
    Serial.println(jsonString); 
    webSocket.broadcastTXT(jsonString); 
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
      
      DeserializationError error = deserializeJson(doc_rx, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        const char* g_brand = doc_rx["brand"];
        const char* g_type = doc_rx["type"];
        const int g_year = doc_rx["year"];
        const char* g_color = doc_rx["color"];
        Serial.println("Received guitar info!");
        Serial.println("Brand: " + String(g_brand));
        Serial.println("Type: " + String(g_type));
        Serial.println("Year: " + String(g_year));
        Serial.println("Color: " + String(g_color));
      }
      Serial.println("");
      break;
  }
}
