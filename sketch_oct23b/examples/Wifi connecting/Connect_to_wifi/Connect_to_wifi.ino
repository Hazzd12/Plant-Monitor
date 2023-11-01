#include <ESP8266WiFi.h>

const char* ssid     = "SSID here"; //change to your Wifi ssid
const char* password = "password here"; //change to your Wifi password
const char* host = "iot.io";

void setup() {
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //print successfully connecting WIFI message
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000); 

  // print connecting message
  Serial.println("-------------------------------");  
  Serial.print("Connecting to ");  
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {//connect to host:httpPort
    Serial.println("connection failed");
    return;
  }

  // Builds and executes an HTTP GET request to get Web page data from a specified URI
  
  String url = "/index.html";
  Serial.print("Requesting URL: ");
  Serial.println(host + url);  // The URL

  // send the Get request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  delay(500);  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  // Once all the response data has been read and printed, the connection to the server will be closed
  Serial.println("closing connection"); 
  }