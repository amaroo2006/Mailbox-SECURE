#include <WiFi.h>
#include <HTTPClient.h>

// set address for IFTTT Webhook service
const char* serverAddress = "https://maker.ifttt.com/trigger/mailboxOpened/with/key/dp0zVr3XBmwgMu-qR115B_";

// set Wi-Fi credentials
const char* ssid = "SSID-GOES-HERE";
const char* password = "PASSWORD-GOES-HERE";

// create secure Wi-Fi client object
WiFiClientSecure client;

// define pin references
int mailboxPin = 16;
byte mailboxOpen = 0;

/* Setup - connect to wifi */
void setup() {

  // begin serial communication
  Serial.begin(115200);

 
  // Set Wi-Fi to station mode and disconnect from an access point if it was previously connected
  WiFi.mode(WIFI_STA);
  
  delay(100);

  // Display Wi-Fi infomration
  Serial.println(WiFi.macAddress());

  // Scan for networks and output their information if found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  
  if (n == 0) {
      Serial.println("no networks found");
  } 
  else {
      // output all networks found
      Serial.print(n);
      Serial.println(" networks found");
      
      for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
          Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
          delay(10);
      }
  }
  Serial.println("Setup done");

  // Attempt to connect to selected network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.disconnect();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");

  }
  
  // Once connected to Wi-Fi, print the IP address of the sensor device
  IPAddress ip = WiFi.localIP();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(ip.toString());
  
}

/* Main loop - continuously checks for the state of the mailbox. If the mailbox is opened, a request is sent to the IFTTT webhook service, which then sends a notification to the user's mobile device */
void loop() {  
  // print current state of the mailbox
  Serial.print("mailbox open?: ");
  Serial.println(mailboxOpen);
  Serial.print("input: ");

  // check for any change in state of the mailbox
  Serial.println(digitalRead(mailboxPin));
  
  if(mailboxOpen == false && digitalRead(mailboxPin) == HIGH) {
    // if the mailbox is opened, change its state to open and send an alert to the IFTTT webhook service
    mailboxOpen = 1;
    sendRequest();
  } 
  
  else if(mailboxOpen == true && digitalRead(mailboxPin) == LOW) {
    // if the mailbox is closed, changed its state to closed
    mailboxOpen = 0;
    
  }
}

/* sendRequest - makes an http request to the IFTTT webhook url provided */
void sendRequest() {
  // check for an active Wi-Fi connection
  if((WiFi.status() == WL_CONNECTED)) {
    // create an http object that allows for web requests
    HTTPClient http;
    http.begin(serverAddress); // begin communication with the webhook url provided

    // send a request to the webhook url
    int httpCode = http.GET();
    Serial.println(httpCode);

    // verify whether or not the request was successful
    if(httpCode > 0) {
      String payload = http.getString();
      //Serial.println(httpCode);
      Serial.println(payload);
    }
    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); // end communication with the webhook url
  }
}
