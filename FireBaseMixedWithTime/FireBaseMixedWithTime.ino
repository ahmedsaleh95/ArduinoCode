#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager



// Set these to run example.
//#define WIFI_SSID "WE_4215EC"
//#define WIFI_SSID "Office"
//#define WIFI_PASSWORD "j7q15372"
//#define WIFI_PASSWORD "Ats102030"
const int LED = 5; // D1 on NodeMCU and WeMos. Controls the onboard LED.
const int TRIGGER_PIN = 0; // D3 on NodeMCU and WeMos.
const int TRIGGER_PIN2 = 13; // D7 on NodeMCU and WeMos.
bool initialConfig = false;

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);

char buffer[80];
int timezone = 3;
int dst = 0;
String LedStatus , timer , scheduleStart , scheduleEnd;
const int PIN_LED = 14;
bool timerflag;
FirebaseArduino FirebaseSet;



void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(PIN_LED, OUTPUT);
  pinMode(LED, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  // connect to wifi.
  //  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //  Serial.print("connecting");
  //  while (WiFi.status() != WL_CONNECTED) {
  //    Serial.print(".");
  //    delay(500);
  //  }
  //  Serial.println();
  //  Serial.print("connected: ");
  //  Serial.println(WiFi.localIP());
  //---------------------------------------------------------------------------------
//  Serial.println("\n Starting");
//  WiFi.printDiag(Serial); //Remove this line if you do not want to see WiFi password printed
//  if (WiFi.SSID() == "") {
//    Serial.println("We haven't got any access point credentials, so get them now");
//    initialConfig = true;
//  }
//  else {
//    digitalWrite(LED, HIGH); // Turn led off as we are not in configuration mode.
//    WiFi.mode(WIFI_STA); // Force to station mode because if device was switched off while in access point mode it will start up next time in access point mode.
//    unsigned long startedAt = millis();
//    Serial.print("After waiting ");
//    int connRes = WiFi.waitForConnectResult();
//    float waited = (millis() - startedAt);
//    Serial.print(waited / 1000);
//    Serial.print(" secs in setup() connection result is ");
//    Serial.println(connRes);
//  }
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");
    Serial.println("connected...yeey :)");

  //--------------------------------------------------------------------------------
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");

  Firebase.begin("real-time-notiy.firebaseio.com");
  FirebaseSet.begin("real-time-notiy.firebaseio.com");

  //get value of timer here
  timer = Firebase.getString("AAjT5c028d47dd69e/timer");
  //get value of schedule end here
  scheduleEnd = Firebase.getString("AAjT5c028d47dd69e/scheduleEnd");
  //get value of schedule start here
  scheduleStart = Firebase.getString("AAjT5c028d47dd69e/scheduleStart");

  timerflag = false;
  Serial.println("aaaaaaaaaaaaaaaaaa" + timer);
  //get value of on/off here
  LedStatus = Firebase.getString("AAjT5c028d47dd69e/status");
  digitalWrite(PIN_LED, LedStatus.toInt());
  String z = String(digitalRead(PIN_LED));
  Serial.println("vvvvvvvvvvvvvv : " + z);
  Firebase.stream("/AAjT5c028d47dd69e");
}


void loop() {
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    //WiFiManager
    WiFiManager wifiManager;
    if (!wifiManager.startConfigPortal("OnDemandAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    digitalWrite(LED, HIGH); // Turn led off as we are not in configuration mode.
    Serial.println("connected...yeey :)");
  }
  //    if ((digitalRead(TRIGGER_PIN) == LOW) || (digitalRead(TRIGGER_PIN2) == LOW) || (initialConfig)) {
  //     Serial.println("Configuration portal requested.");
  //     digitalWrite(LED, LOW); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.
  //    //Local intialization. Once its business is done, there is no need to keep it around
  //    WiFiManager wifiManager;
  //    if (!wifiManager.startConfigPortal()) {
  //      Serial.println("Not connected to WiFi but continuing anyway.");
  //    } else {
  //      //if you get here you have connected to the WiFi
  //      Serial.println("connected...yeey :)");
  //    }
  //    digitalWrite(LED, HIGH); // Turn led off as we are not in configuration mode.
  //    ESP.reset(); // This is a bit crude. For some unknown reason webserver can only be started once per boot up
  //    // so resetting the device allows to go back into config mode again when it reboots.
  //    delay(2000);
  //  }
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }

  if (Firebase.failed()) {
    Serial.println("streaming error");
    Serial.println(Firebase.error());
  }

  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer, 80, "%Y-%m-%d %H:%M", timeinfo);
  Serial.println(buffer);
  Serial.println("###################################");
  Serial.println(timer);

  // check timer and current time here if same time then turn led on else off
  if (timer == buffer && !timerflag)
  {
    //    digitalWrite(PIN_LED, HIGH);
    Serial.println("timer is set");
    LedStatus = String(!digitalRead(PIN_LED));
    timerflag = true;
    FirebaseSet.setString("AAjT5c028d47dd69e/status" , LedStatus);
    //send request to server
    //    HTTPClient http;
    //    http.begin("kingofdarknessahmed.000webhostapp.com" , 80 , "/api/timer/turn?qrcode=AAjT5c028d47dd69e&status="+LedStatus);
    //
    //    Serial.print("[HTTP] GET...\n");
    //    // start connection and send HTTP header
    //    int httpCode = http.GET();
    //    // httpCode will be negative on error
    //    if (httpCode > 0) {
    //      // HTTP header has been send and Server response header has been handled
    //      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    //      // file found at server
    //      if (httpCode == HTTP_CODE_OK) {
    //        String payload = http.getString();
    //        Serial.println(payload);
    //      }
    //    } else {
    //      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    //    }
    //    http.end();
  }


  if (scheduleStart == buffer)
  {
    //    digitalWrite(PIN_LED, HIGH);
    Serial.println("Schedule is set");
    LedStatus = String(1);
    FirebaseSet.setString("AAjT5c028d47dd69e/status" , LedStatus);
  }

  if (scheduleEnd == buffer)
  {
    //    digitalWrite(PIN_LED, HIGH);
    Serial.println("Schedule is Ended");
    LedStatus = String(0);
    FirebaseSet.setString("AAjT5c028d47dd69e/status" , LedStatus);
  }

  if (LedStatus == "1")
  {
    digitalWrite(PIN_LED, HIGH);
    Serial.println("it is on yaaay");
  }
  else if (LedStatus == "0")
  {
    digitalWrite(PIN_LED, LOW);
    Serial.println("it is off oOps");
  }

  if (Firebase.available()) {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    eventType.toLowerCase();

    Serial.print("event: ");
    Serial.println(eventType);
    if (eventType == "put") {
      Serial.print("path: ");
      Serial.println(event.getString("path"));
      Serial.print("data: ");
      Serial.println(event.getString("data"));
      String path = event.getString("path");
      String data = event.getString("data");
      if (path == "/status")
      {
        LedStatus = data;
        digitalWrite(PIN_LED, data.toInt());
      }
      else if (path == "/timer")
      {
        timer = data;
        timerflag = false;
      }
      else if (path == "/scheduleEnd")
      {
        scheduleEnd = data;
      }
      else if (path == "/scheduleStart")
      {
        scheduleStart = data;
      }

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println(path.c_str() + 1);
      display.println(data);
      display.display();
      // check path if timer make variable equal timer else make variable equal ledStatus

    }
  }
  delay(1000);

}
