//#include <FirebaseArduino.h>
//#include <ESP8266WiFi.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//
//// Set these to run example.
//#define WIFI_SSID "WE_4215EC"
//#define WIFI_PASSWORD "j7q15372"
//
//#define OLED_RESET 3
//Adafruit_SSD1306 display(OLED_RESET);
//
//void setup() {
//  Serial.begin(115200);
//
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
//  display.display();
//
//  // connect to wifi.
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("connecting");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(500);
//  }
//  Serial.println();
//  Serial.print("connected: ");
//  Serial.println(WiFi.localIP());
//
//  Firebase.begin("real-time-notiy.firebaseio.com");
//  Firebase.stream("/LEDStatus");
//}
//
//
//void loop() {
//  if (Firebase.failed()) {
//    Serial.println("streaming error");
//    Serial.println(Firebase.error());
//  }
//
//  if (Firebase.available()) {
//     FirebaseObject event = Firebase.readEvent();
//     String eventType = event.getString("type");
//     eventType.toLowerCase();
//
//     Serial.print("event: ");
//     Serial.println(eventType);
//     if (eventType == "put") {
//       Serial.print("data: ");
//       Serial.println(event.getString("data"));
//       String path = event.getString("path");
//       String data = event.getString("data");
//
//       display.clearDisplay();
//       display.setTextSize(2);
//       display.setTextColor(WHITE);
//       display.setCursor(0,0);
//       display.println(path.c_str()+1);
//       display.println(data);
//       display.display();
//     }
//  }
//}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <time.h>
//WE_4215EC
const char* ssid = "Office";
//j7q15372
const char* password = "Ats102030";

int timezone = 3;
int dst = 0;
char buffer[80];
#define MST (-7)
#define UTC (0)
#define CCT (+8)

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop() {
  //  time_t now = time(nullptr);
  //  Serial.println(ctime(&now));
  //    Serial.println('%m');
        time_t rawtime;
          struct tm * timeinfo;
    time (&rawtime);
      timeinfo = localtime (&rawtime);
    strftime (buffer,80," %d %m %Y %H:%M:%S ",timeinfo);
    Serial.println(buffer);



  delay(1000);
}
