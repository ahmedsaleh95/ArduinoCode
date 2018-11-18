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
String LedStatus;
String t;
const int PIN_LED = 5;


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

  Firebase.begin("real-time-notiy.firebaseio.com");
  //get value of timer here
  t = Firebase.getString("520dd4c0-dddd-11e8-b63e-290de7252586/timer");
  Serial.println("aaaaaaaaaaaaaaaaaa" + t);
  delay(3000);
Serial.println("");
}


void loop() {

  Serial.println((String)t);

  Serial.println("###################################");
  Serial.println((String)t);
 
  delay(1000);

}
