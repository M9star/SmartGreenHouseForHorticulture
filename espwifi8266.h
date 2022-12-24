#include <ESP8266WiFi.h>

const char* SSID = "YOUR_WIFI_SSID"; // replace with your Wi-Fi SSID
const char* PASSWORD = "YOUR_WIFI_PASSWORD"; // replace with your Wi-Fi password

void setup() {
  WiFi.begin(SSID, PASSWORD); // connect to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  // make an HTTP GET request to a web server
  WiFiClient client;
  if (client.connect("www.example.com", 80)) { // replace with your server's IP address and port
    client.println("GET / HTTP/1.1");
    client.println("Host: www.example.com");
    client.println("Connection: close");
    client.println();
  }
  delay(10000); // wait for 10 seconds before making another request
}
/*code contains error that need to solve*/
