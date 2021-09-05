#include <WiFi.h>
#include <WebServer.h> 

// SSID and password of Wifi connection:
const char* ssid = "TYPE_YOUR_SSID_HERE";
const char* password = "TYPE_YOUR_PASSWORD_HERE";

String website1 = "<!DOCTYPE html><html><head><title>Page Title</title></head><body style='background-color: #EEEEEE;'><span style='color: #003366;'><h1>Lets generate a random number</h1><p>The random number is: ";
String website2 = "</p></span></body></html>";
String website = "";

int randomval = random(100);
int interval = 5000; // virtual delay
unsigned long previousMillis = 0; // Tracks the time since last event fired

WebServer server(80);

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
  
  website = website1 + String(randomval) + website2;
  server.on("/", []() {
  server.send(200, "text\html", website);
  });
  server.begin(); // init the server
}

void loop() {
  server.handleClient();  // webserver methode that handles all Client

  unsigned long currentMillis = millis(); // call millis  and Get snapshot of time
  if ((unsigned long)(currentMillis - previousMillis) >= interval) { // How much time has passed, accounting for rollover with subtraction!
    randomval = random(100);
    website = website1 + String(randomval) + website2;
  }
}
