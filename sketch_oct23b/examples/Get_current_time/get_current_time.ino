
#include <ESP8266WiFi.h>  //Wi-Fi related functions are provided
#include <ezTime.h>  //ezTime library

const char* ssid     = "SSID here"; //change to your Wifi ssid
const char* password = "password here"; //change to your Wifi password

Timezone GB;  //Initializes a time zone object GB

void setup() {
  Serial.begin(115200);
  delay(100);
  //print trying to connect information
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //add wifi WIFIssid and password to start to connect to
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {//when Wifi has not yet been connected
    delay(500);
    Serial.print(".");//show connecting message
  }

  //print successfully connection message
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

  // get real date and time
  //Wait for the time synchronization to complete,
  //ensuring that the time is successfully synchronized
  //before your code continues to execute
  waitForSync(); 

  // print London current time
  Serial.println("UTC: " + UTC.dateTime()); 
  GB.setLocation("Europe/London");  //Set the time zone of the GB object to the time zone of London
  Serial.println("London time: " + GB.dateTime());

}

void loop() {
  delay(1000);  
  Serial.println(GB.dateTime("l, d-M-y H:i:s.v T")); 
}
/*abbreviation:
 ** "l"：Weekday
 ** "d"：Day of the month
 ** "M"：Month 
 ** "y"：the last two digits of the Year 
 ** "H"：hour (24 hours)
 ** "i"：minutes
 ** "s"：seconds
 ** "v"：millisecond
 ** "T"：Time Zone 
 */