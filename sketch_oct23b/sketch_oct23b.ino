#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>


#define DHTTYPE DHT22

#include "arduino_secrets.h"
uint8_t DHTPin = 12;  // set 12 as DHT pin number
uint8_t soilPin = 0;  // set 0 as soil sensor pin number
float TemperatureC;    // declare Celsius Temperature 
float Humidity;       // declare Humidity


int Moisture = 1;          // initial value just in case web page is loaded before readMoisture called
int sensorVCC = 13;        //set sensor VCC pin number
int blueLED = 2;           //set blue Led pin number
DHT dht(DHTPin, DHTTYPE);  // Initialize DHT sensor.




//read WIFI and MQTT name and password from head documents arudino_secrets.h
const char* WIFIssid = SECRET_SSID;
const char* WIFIpassword = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

ESP8266WebServer server(80);                   // set 80 as ESP8266server port number
const char* mqtt_server = "mqtt.cetools.org";  // mqtt server ip

WiFiClient espClient;            // Use WiFiClient Object to create TCP connections
PubSubClient client(espClient);  //Use PubSubClient to handle MQTT communication
long lastMsg = 0;
char msg[50];                    //Used to contain humidity, temperature or moisture message
int value = 0;

Timezone GB;//Initializes a time zone object GB
void setup() {
  // Initialize the BUILTIN_LED pin as an output
  pinMode(BUILTIN_LED, OUTPUT);// Set the built-in LED to output mode
  digitalWrite(BUILTIN_LED, HIGH);// Set the built-in LED to high (usually indicates LED off)

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
 // Once connected to wifi try to connect MQTT server
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
  //print trying to connect information
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(WIFIssid);


  WiFi.begin(WIFIssid, WIFIpassword);  //add wifi WIFIssid and password to try to connect WIFI

  while (WiFi.status() != WL_CONNECTED) {//when Wifi has not yet been connected
    delay(500);
    Serial.print(".");  //show connecting message
  }
  Serial.println("");


  //print successfully connection message
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

  // print London current time
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");//Set the time zone of the GB object to the time zone of London
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

// Publishes formatted messages to the specified MQTT topic
void sendMQTT() {

  if (!client.connected()) {//check if the client is connected
    reconnect();  //try to reconnect MQTT serverthe specified until it is connected
  }

  // Execute the loop() function on the MQTT client to process messages from the MQTT server
  client.loop();

  TemperatureC = dht.readTemperature();  // Gets the values of the temperature from DHT directly
  //Format temperature value as a floating point number with one decimal place.
  snprintf(msg, 50, "%.1f", TemperatureC);
  Serial.print("Publish message for Celsius temperature: ");
  Serial.println(msg);
  // publish the temperature message to to the topic "student/CASA0014/plant/ucfnuax/temperature"
  client.publish("student/CASA0014/plant/ucfnuax/temperature", msg);  


  float TemperatureF;
  TemperatureF = celsiusToFahrenheit(TemperatureC);//convert Celsius degree to Fahrenheit degrees
  snprintf(msg, 50, "%.1f", TemperatureF);
  Serial.print("Publish message for Fahrenheit temperature: ");
  Serial.println(msg);
  // publish the temperature message to to the topic "student/CASA0014/plant/ucfnuax/temperatureF"
  client.publish("student/CASA0014/plant/ucfnuax/temperatureF", msg);

  Humidity = dht.readHumidity();  // Gets the values of the humidity from DHT directly
  //Format humudity value as a floating point number with one decimal place.
  snprintf(msg, 50, "%.0f", Humidity);
  Serial.print("Publish message for humidity: ");
  Serial.println(msg);
  // publish the humudity message to the topic"student/CASA0014/plant/ucfnuax/humidity"
  client.publish("student/CASA0014/plant/ucfnuax/humidity", msg);  

  //Moisture = analogRead(soilPin);  // moisture read by readMoisture function
  //Format moisture value as a floating point number with one decimal place.
  snprintf(msg, 50, "%.1i", Moisture);
  Serial.print("Publish message for moisture: ");
  Serial.println(msg);
   // publish the moisture message to the topic"student/CASA0014/plant/ucfnuax/moisture"
  client.publish("student/CASA0014/plant/ucfnuax/moisture", msg); 
  
}
float celsiusToFahrenheit(float celsius) {
    // Convert degrees Celsius to degrees Fahrenheit using the conversion formula
    float fahrenheit = (celsius * 9 / 5) + 32;
    return fahrenheit;
}
void callback(char* topic, byte* payload, unsigned int length) {
  //when the messages of subscribed topics arrive, print the relative information
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
  while (!client.connected()) {// when MQTT server is not connected
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
  TemperatureC = dht.readTemperature();                                       // Gets the values of the temperature
  Humidity = dht.readHumidity();                                             // Gets the values of the humidity
  //designed to serve a simple web page that displays sensor data from the DHT22 sensor.
  server.send(200, "text/html", SendHTML(TemperatureC, Humidity, Moisture));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

//generates an HTML page with the sensor data and returns it as a string.
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