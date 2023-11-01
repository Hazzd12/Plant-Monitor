#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <ESP8266HTTPClient.h>
#include <ezTime.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
// #include <ArduinoJson.h>

#define DHTTYPE DHT22

#include "arduino_secrets.h"
uint8_t DHTPin = 12;  // set 12 as DHT pin number
uint8_t soilPin = 0;  // set 0 as soil sensor pin number
float Temperature;    // declare Temperature
float Humidity;       // declare Humidity


int Moisture = 1;          // initial value just in case web page is loaded before readMoisture called
int sensorVCC = 13;        //set sensor VCC pin number
int blueLED = 2;           //set blue Led pin number
DHT dht(DHTPin, DHTTYPE);  // Initialize DHT sensor.




//read WIFI and MQTT name and password from head documents arudino_secrets.h
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

ESP8266WebServer server(80);                   // set 80 as ESP8266server port number
const char* mqtt_server = "mqtt.cetools.org";  // mqtt_server ip

WiFiClient espClient;            // Use WiFiClient Object to create TCP connections
PubSubClient client(espClient);  //Use PubSubClient  to handle MQTT message
char msg[50];                    //used to contain humidity, temperature or moisture value
int value = 0;

Timezone GB;
void setup() {
  // Initialize the BUILTIN_LED pin as an output
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  // Set up the outputs to control the soil sensor
  // switch and the blue LED for status indicator
  pinMode(sensorVCC, OUTPUT);
  digitalWrite(sensorVCC, LOW);
  pinMode(blueLED, OUTPUT);
  digitalWrite(blueLED, HIGH);

  // open serial connection for debug info
  Serial.begin(115200);
  delay(100);

  // start DHT sensor
  pinMode(DHTPin, INPUT);
  dht.begin();

  // run initialisation functions
  startWifi();       //Wifi connection function
  startWebserver();  //Start web server function
  syncDate();        //time synchronization and printing

  // start MQTT server
  client.setServer(mqtt_server, 1884);  //set mqtt.cetools.org:1884 as the MQTT server ip
  client.setCallback(callback);         //set  callback function
}
void loop() {
  // handler for receiving requests to webserver
  server.handleClient();


  //get Moisture information and send it to MQTT when current minute is changed
  if (minuteChanged()) {
    readMoisture();
    sendMQTT();
    Serial.println(GB.dateTime("H:i:s"));  // UTC.dateTime("l, d-M-y H:i:s.v T")
    Serial.println(Moisture);
  }

  client.loop();
}



void readMoisture() {
  // power the sensor
  digitalWrite(sensorVCC, HIGH);
  digitalWrite(blueLED, LOW);
  delay(100);

  // read the value from the sensor:
  //The Analog Pin(soilPin) reads the voltage value(from the nails)
  //analogRead()function performs an (A/D conversion) on the specified
  //analog pin, converting the voltage value to a digital value
  Moisture = analogRead(soilPin);
  //Moisture = map(analogRead(soilPin), 0,320, 0, 100);    // note: if mapping work out max value by dipping in water
  //stop power
  digitalWrite(sensorVCC, LOW);
  digitalWrite(blueLED, HIGH);
  delay(100);

  //print moisture value
  Serial.print("Wet ");
  Serial.println(Moisture);
}

void startWifi() {
  //print connection information
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, password);  //insert wifi ssid and password to start to connect to

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  //show connecting message
  }
  Serial.println("");


  //print successfully connecting WIFI message
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  // get real date and time
  //Wait for the time synchronization to complete,
  //ensuring that the time is successfully synchronized
  //before your code continues to execute
  waitForSync();

  // print real Time
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
}

void startWebserver() {
  // when connected and IP address obtained start HTTP server

  //set handle_OnConnect function as the handler function to handle the root path HTTP request
  server.on("/", handle_OnConnect);
  //set handle_NotFound function as the handler function to handle requests for pages that cannot be found
  server.onNotFound(handle_NotFound);

  server.begin();  //start HTTP Server
  Serial.println("HTTP server started");
}

void sendMQTT() {

  if (!client.connected()) {
    reconnect();  //function used to connect MQTT serverthe specified
  }
  client.loop();

  Temperature = dht.readTemperature();  // Gets the values of the temperature from DHT directly
  //Format temperature value as a floating point number with one decimal place.
  snprintf(msg, 50, "%.1f", Temperature);
  Serial.print("Publish message for temperature: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnuax/temperature", msg);  // publish the temperature message to the topic

  Humidity = dht.readHumidity();  // Gets the values of the humidity from DHT directly
  //Format humudity value as a floating point number with one decimal place.
  snprintf(msg, 50, "%.0f", Humidity);
  Serial.print("Publish message for humidity: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnuax/humidity", msg);  // publish the humudity message to the topic

  Moisture = analogRead(soilPin);  // moisture read by readMoisture function
  //Format moisture value as a floating point number with one decimal place.
  snprintf(msg, 50, "%.0i", Moisture);
  Serial.print("Publish message for moisture: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnuax/moisture", msg);  // publish the moisture message to the topic
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0014/plant/ucfnuax/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void handle_OnConnect() {
  Temperature = dht.readTemperature();                                       // Gets the values of the temperature
  Humidity = dht.readHumidity();                                             // Gets the values of the humidity
  server.send(200, "text/html", SendHTML(Temperature, Humidity, Moisture));  //
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat, float Humiditystat, int Moisturestat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 DHT22 Report</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP8266 Huzzah DHT22 Report</h1>\n";

  ptr += "<p>Temperature: ";
  ptr += (int)Temperaturestat;
  ptr += " C</p>";
  ptr += "<p>Humidity: ";
  ptr += (int)Humiditystat;
  ptr += "%</p>";
  ptr += "<p>Moisture: ";
  ptr += Moisturestat;
  ptr += "</p>";
  ptr += "<p>Sampled on: ";
  ptr += GB.dateTime("l,");
  ptr += "<br>";
  ptr += GB.dateTime("d-M-y H:i:s T");
  ptr += "</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}