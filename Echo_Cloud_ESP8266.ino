/*
 This sketch was created for a video series called Home Automation with the Arduino and the Amazon Echo Part 2 
 That was presented on the ForceTronics YouTube Channel. This code is public domain for anybody to use or modify at your own risk
 Note that this code was leveraged from a Sparkfun example on using their cloud service Phant
 */

// Include the ESP8266 WiFi library
#include <ESP8266WiFi.h>
// Include the SparkFun Phant library.
#include <Phant.h>

// Set your network name and password
const char* ssid     = "NetworkName"; // your wifi network name goes here
const char* password = "NetworkPassword"; // your wifi password goes here

// define constants for pin control and node number
const int light = 4; // NodeMCU GPIO 4 pin is connected to the WiFi AC Switch control
const char parseKey[] = "stamp"; // This is used to parse through data from Phant to find light setting

// declare phant address and security keys
const char* host = "data.sparkfun.com";
const char* streamId   = "...................."; // your phant public key goes here
const char* privateKey = "...................."; // your phant private key goes here

// specify the rate that you post data to cloud
const unsigned long postRate = 1000;
unsigned long lastPost = 0;

void setup() {
  initHardware(); // setup NodeMCU hardware
  connectWiFi(); // connecting to WiFi network
}

void loop() {
  if (lastPost + postRate <= millis())
  {
    if (getFromPhant()) lastPost = millis(); // get data from Phant
    else lastPost = millis(); // Even if we fail delay whole cycle before we try again
  }
}

// function that sets up some initial hardware states
void initHardware()
{
  Serial.begin(9600);
  pinMode(light, OUTPUT); // Initialize the LED_BUILTIN pin as an output
}

// function used to connect to WiFi network
void connectWiFi()
{  
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);
  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

// function that handles getting data from phant cloud
int getFromPhant() {
  // Set phant data
  Phant phant(host, streamId, privateKey);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    // if we fail to connect, return 0.
    Serial.println("connection failed");
    return 0;
  }

  // Get data from phant cloud
  client.print(phant.get());
  client.println();

  int cTrack = 0; // variable that tracks count to spell stamp
  bool match = false; // tracks when we have a match with "stamp" and we can then get control data
  int pCount = 0; // variable used to track when we have control data

  // loop until we get data and server closes connection
  while(1) {
    // if data is available from phant server
    if (client.available()) {
      char c = client.read(); // read a bite of data from server
      if(!match) { // if not true than we have not found the word "stamp" so keep looking
        if(c == parseKey[cTrack]) // check if we have a character match with word "stamp"
        {
          if(cTrack == (sizeof(parseKey)-2)) match = true; // if true it means we found a match for "stamp" in data from phant cloud
          cTrack++; // iterate this count if a character match was found
        } else { // if true means no character match so reset count
          cTrack = 0;
        }
        
      } else { // if true it means we found a match to "stamp" and we are ready to get control data
        if(pCount == 1) { // if true we are at the point in the data to read control data for node oen
          int dControl = c - '0'; // convert char data to an int by subtract an ASCII zero
          
          //make sure data is a one or zer0 and set LED pin with it
          if(dControl == 1) {
            digitalWrite(light, HIGH);
          } else if (dControl == 0) {
            digitalWrite(light, LOW);
          }
          
        }
        pCount++; // iterate the parse counter
      }
    }
    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      client.stop(); // stop client, if you don't have this you will create too many clients and server won't let you connect anymore
      break; // This is how we get out of the loop
    }
  }
 return 1; // Return success
}
