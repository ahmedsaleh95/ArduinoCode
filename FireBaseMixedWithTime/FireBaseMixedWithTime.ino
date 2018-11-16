#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>


// Set these to run example.
#define WIFI_SSID "WE_4215EC"
#define WIFI_PASSWORD "j7q15372"

#define OLED_RESET 3
Adafruit_SSD1306 display(OLED_RESET);

char buffer[80];
int timezone = 3;
int dst = 0;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);


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
  Firebase.stream("/520dd4c0-dddd-11e8-b63e-290de7252586");
  //get value of timer here 

  //get value of on/off here 
  
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
  strftime (buffer, 80, " %Y-%m-%d %H:%M ", timeinfo);
  Serial.println(buffer);
  // check timer and current time here if same time then turn led on else off

  if (Firebase.available()) {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    eventType.toLowerCase();

    Serial.print("event: ");
    Serial.println(eventType);
    if (eventType == "put") {
      Serial.print("data: ");
      Serial.println(event.getString("data"));
      String path = event.getString("path");
      String data = event.getString("data");
      // check path if timer make variable equal timer else make variable equal ledStatus
      
    }
  }
  delay(1000);

}
