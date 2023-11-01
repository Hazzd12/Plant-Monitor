# Plant Monitor

## Overview

This document documents how to make a plant monitor under the [workshop guidance](https://workshops.cetools.org/codelabs/CASA0014-2-Plant-Monitor/index.html) of the course CASA0014. The plant monitor will monitor soil moisture and environment conditions(temperature and moisture), send data to an MQTT server and send email to yourself when the environment conditions are not good.

## Catalog

[TOC]

## 1. Physical Equipments

*  ***Feather Huzzah ESP8266 Wifi***  &times; 1
* ***circuit board***  &times; 1
* ***nail*** &times; 2
* ***resistances***:
  * ***10kΩ*** &times; 2
  * ***100Ω*** &times; 1
  * ***200Ω*** &times; 1
* ***DHT22 sensor***  &times; 1
* ***Cable*** &times; 1
* ***Raspberry Pi*** &times; 1
*  ***Micro usb to usb-b cable*** &times; 1
* ***Plant*** &times; 1

****

## 2. IDE

* ***Arduino IDE***
* ***Visual Studio Code***
* ***MobaXterm***
* ***IntelliJ IDEA***

****

## 3. Arduino Library

In this section you can use the library manager of IDE to find them or download it from official.

* ***ESP8266WiFi***
* ***ESP8266WebServer***
* ***ezTime***
* ***PubSubClient***
* ***DHT  from Adafruit***
* ***DHT_U from Adafruit***

****

## 4. Process

In this section, it will briefly show how to build this plant monitor

 ### 4.1. Connect Feather Huzzah ESP8266 Wifi to MQTT

#### 4.1.1 Assemble the circuit

The sketch below shows the outline design for v2 of the circuit 

![1698801684205](./img/1698801684205.png)

Here is the finished product

*Notice:* Some places need to be soldered

#### 4.1.2 Setup Arduino

You need to create a header file named `secrets.h` to contain your WIFI and MQTT information. The format shows as below:

```
#define SECRET_SSID "xxx"
#define SECRET_PASS "xxx"
#define SECRET_MQTTUSER "xxx"
#define SECRET_MQTTPASS "xxx"
```

Then, in  `sketch_oct23b.ino`  code, you can reference these keys to connect to WIFI and the MQTT server. Here's an easy example of how to connect to WIFI: 

```
...
#include "arduino_secrets.h"
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
...

void setup(){
...
startWifi();
...
}

...

void startWifi() {//function which is put in the setup() to connect to WIFI
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

...
```

 You can see Other code and explanations in the  **`sketch_oct23b.ino`**  where there are detailed annotations, so I won't go into details here.

### 4.1.3 Setup Raspberry Pi

* F



