#include <ESP8266WiFi.h>

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/kentaylor/WiFiManager

// Set these to run example.
const int PIN_LED = 5; // D1 on NodeMCU and WeMos. Controls the onboard LED.
const int TRIGGER_PIN = 0; // D3 on NodeMCU and WeMos.
const int TRIGGER_PIN2 = 13; // D7 on NodeMCU and WeMos.

#define LED 14

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(TRIGGER_PIN, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  //auto connect
  WiFiManager wifiManager;
  digitalWrite(PIN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");



}

void loop() {
  // code here
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    //WiFiManager
    WiFiManager wifiManager;
    digitalWrite(PIN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED, LOW);
    if (!wifiManager.startConfigPortal("OnDemandAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, LOW);
    digitalWrite(PIN_LED, LOW);

  } else {
    digitalWrite(LED, HIGH);
    digitalWrite(PIN_LED, LOW);

  }
}
