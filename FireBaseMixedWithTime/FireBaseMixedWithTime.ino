#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <ESP8266HTTPClient.h>



// Set these to run example.
#define WIFI_SSID "WE_4215EC"
//#define WIFI_SSID "Office"
#define WIFI_PASSWORD "j7q15372"
//#define WIFI_PASSWORD "Ats102030"

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

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

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
    HTTPClient http;
    http.begin("kingofdarknessahmed.000webhostapp.com" , 80 , "/api/timer/turn?qrcode=AAjT5c028d47dd69e&status="+LedStatus);

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
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
