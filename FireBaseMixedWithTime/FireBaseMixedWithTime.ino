#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

const int LED = 5; // D1 on NodeMCU and WeMos. Controls the onboard LED.

const int TRIGGER_PIN = 0; // D3 on NodeMCU and WeMos.
const int TRIGGER_PIN2 = 13; // D7 on NodeMCU and WeMos.
bool initialConfig = false;

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);

char buffer[80];
String LedStatus , timer , scheduleStart , scheduleEnd , timerID , scheduleID;
const int PIN_LED = 14;
bool timerflag , scheduleflag;
FirebaseArduino FirebaseSet;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(PIN_LED, OUTPUT);
  pinMode(LED, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");

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
  timer = Firebase.getString("AAjT5c028d47dd69e/timer/start");
  //get value of schedule start here
  scheduleStart = Firebase.getString("AAjT5c028d47dd69e/schedule/start");
  //get value of schedule end here
  scheduleEnd = Firebase.getString("AAjT5c028d47dd69e/schedule/end");

  timerflag = false;
  scheduleflag = false;
  //get value of on/off here
  LedStatus = Firebase.getString("AAjT5c028d47dd69e/status");
  digitalWrite(PIN_LED, LedStatus.toInt());
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
    Serial.println("timer is set");
    LedStatus = String(!digitalRead(PIN_LED));
    timerflag = true;
    FirebaseSet.setString("AAjT5c028d47dd69e/status" , LedStatus);
    timerID = FirebaseSet.getString("AAjT5c028d47dd69e/timer/ID");
    sendRequest("/api/timer/changeStatus?qrcode=AAjT5c028d47dd69e&id=" + timerID + "&status=" + LedStatus);
    //    FirebaseSet.setString("AAjT5c028d47dd69e/timer/ID", "0");

  }

  if (scheduleStart == buffer && !scheduleflag)
  {
    Serial.println("Schedule is set");
    LedStatus = String(1);
    scheduleflag = true;
    FirebaseSet.setString("AAjT5c028d47dd69e/status" , LedStatus);
    scheduleID = FirebaseSet.getString("AAjT5c028d47dd69e/schedule/ID");
    sendRequest("/api/changeStatus?qrcode=AAjT5c028d47dd69e&status=" + LedStatus);
  }

  if (scheduleEnd == buffer && scheduleflag)
  {
    Serial.println("Schedule is Ended");
    LedStatus = String(0);
    scheduleflag = false;
    FirebaseSet.setString("AAjT5c028d47dd69e/status" , LedStatus);
    //send request to set status and completed and weekly
    sendRequest("/api/schedule/changeStatus?qrcode=AAjT5c028d47dd69e&id=" + scheduleID + "&status=" + LedStatus);
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
      else if (path == "/timer/start")
      {
        timer = data;
        timerflag = false;
      }
      else if (path == "/schedule/end")
      {
        scheduleEnd = data;
        scheduleflag = false;
      }
      else if (path == "/schedule/start")
      {
        scheduleStart = data;
        scheduleflag = false;
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

void sendRequest(String url)
{
  Serial.println(url);
  HTTPClient http;
  http.begin("kingofdarknessahmed.000webhostapp.com" , 80 , url);

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: % d\n", httpCode);
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: % s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

