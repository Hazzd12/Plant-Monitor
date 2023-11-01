#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wifi and MQTT
#include "password_secrets.h" 
/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
 */

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;


const char* mqtt_server = "enter mqtt url";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


const char* mqtt_server = "mqtt.cetools.org";  // mqtt server ip

WiFiClient espClient;// Use WiFiClient Object to create TCP connections
PubSubClient client(espClient);  //Use PubSubClient to handle MQTT communication
long lastMsg = 0;  // Used to track the last message sent
char msg[50];  //be used to contain humidity, temperature or moisture message
int value = 0;


void setup() {


  // Initialize the built-in LED and MQTT clients
  pinMode(BUILTIN_LED, OUTPUT);  // Set the built-in LED to output mode
  digitalWrite(BUILTIN_LED, HIGH);  // Set the built-in LED to high (usually indicates LED off)

  Serial.begin(115200);
  delay(100); 

// run initialisation functions
  startWifi();

  // Once connected to wifi try to connect MQTT server
  client.setServer(mqtt_server, 1884);

  //set  callback function
  client.setCallback(callback); 


}

void loop() {
  delay(5000);
  sendMQTT();
}

void startWifi(){
 //print trying to connect information
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);  //add wifi WIFIssid and password to try to connect WIFI

  while (WiFi.status() != WL_CONNECTED) {  //when Wifi has not yet been connected
  delay(500);
  Serial.print(".");//show connecting message
  }

  //print successfully connection message
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

// Publishes formatted messages to the specified MQTT topic
void sendMQTT() {

  if (!client.connected()) {//check if the client is connected
    reconnect();//try to reconnect MQTT serverthe specified until it is connected
  }  
  // Execute the loop() function on the MQTT client to process messages from the MQTT server
  client.loop();
  ++value;  

  snprintf (msg, 50, "hello world #%ld", value);

  Serial.print("Publish message: ");
  Serial.println(msg);
  // publish data to the topic "student/CASA0014/plant/ucfnuax"
  client.publish("student/CASA0014/plant/ucfnuax", msg);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {  // when MQTT server is not connected
    Serial.print("Attempting MQTT connection...");  

    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), SECRET_MQTTUSER, SECRET_MQTTPASS)) {
      Serial.println("connected");
      // ... and subscribe to messages on broker
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
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}