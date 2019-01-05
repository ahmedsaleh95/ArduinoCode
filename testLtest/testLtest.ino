#include <EEPROM.h>

void writeEEPROM(int strlength , String phrase) {
  EEPROM.begin(512); //Max bytes of eeprom to use
  //  yield();
  Serial.println();
  //write to eeprom
  Serial.println("writing eeprom:");
  int i;
  for ( i = 0 ; i < strlength ; i++)
  {
    EEPROM.write(i , phrase[i]);
  }
  EEPROM.write(i , '#');
  EEPROM.commit();
  Serial.println();
  delay(10);
  Serial.println(char(EEPROM.read(0)));
  EEPROM.end();
}

//void readEEPROM(int startAdr, int maxLength, char* dest) {
//  Serial.println("Reading EEPROM");
//  EEPROM.begin(512);
//  delay(10);
//  for (int i = 0; i < maxLength; i++)
//  {
//    *dest += char(EEPROM.read(startAdr + i));
//    //if(dest[i]='\0') break;//break when end of sting is reached before maxLength
//  }
//  EEPROM.end();
//  //esid.trim();
//  Serial.print("ready reading:");
//  Serial.println(dest);
//}
char str;
int j;
String s;
void setup() {
  Serial.begin(115200);
  delay(100);
  //
  //  strcat(wifi_ssid_private, "SSID1234");
  //  strcat(wifi_password_private, "PW1234");

  writeEEPROM(5 , "aszbff"); //32 byte max length
  //  writeEEPROM(32, 32, wifi_password_private); //32 byte max length
  Serial.println("everything saved...");
  EEPROM.begin(512); //Max bytes of eeprom to use
  //  for (int j = 0; j < 5; j++)
  //  {
  //    Serial.println(char(EEPROM.read(j)));
  //  }
  str = ' ';
  s = "";
  j = 0;
  while (str != '0')
  {
    str = char(EEPROM.read(j));
    s += char(EEPROM.read(j));

    Serial.println(char(EEPROM.read(j)));
    j++;
  }
  Serial.println(s);
  EEPROM.end();
  //  readEEPROM(0, 32, wifi_ssid_private);
  //  readEEPROM(32, 32, wifi_password_private);
}

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println("Loop");
  //  delay(1000);
  //  EEPROM.begin(512);
  //  Serial.println(char(EEPROM.read(0)));
  //  Serial.println(char(EEPROM.read(32)));
  //  EEPROM.end();
}
