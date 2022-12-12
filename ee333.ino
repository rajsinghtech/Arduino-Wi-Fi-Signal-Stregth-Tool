#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

// initialize the LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Specify the number of LEDs on the strip
const int max_Leds = 10;

// Specify the pin that is connected to the data pin of the LED strip
const int ledStripPin = A5;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(max_Leds, ledStripPin, NEO_GRB + NEO_KHZ800);

void setup() {
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // set up the SoftwareSerial port
  Serial.begin(9600);

  // Initialize the NeoPixel library
  strip.begin();

  // initialize the ESP8266
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("ESP8266...");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resetting ESP8266");
  Serial.println("AT+RST");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting CWMODE");
  Serial.println("AT+CWMODE=1");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting CIFSR");
  Serial.println("AT+CIFSR");
  delay(1000);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning");
  lcd.setCursor(0, 1);
  lcd.print("Networks");
  // scan for networks
  Serial.println("AT+CWLAP");

  // read the response from the ESP8266
  String response = Serial.readString();
  

  if (response.indexOf("No AP") != -1) {
    // if no networks are found, print "No networks found" on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No networks");
    lcd.setCursor(0, 1);
    lcd.print("found");
    delay(1000); // delay 1 second between each network
  } else {
    // if networks are found, print the SSID and RSSI of each network on the LCD
    lcd.clear();
    int index = 0;
    while (index != -1) {
      // find the next SSID in the response string
      int ssidStartIndex = response.indexOf("\"", index);
      if (ssidStartIndex == -1) break;
      int ssidEndIndex = response.indexOf("\"", ssidStartIndex + 1);
      String ssid = response.substring(ssidStartIndex + 1, ssidEndIndex);

      // find the next RSSI in the response string
      int rssiStartIndex = response.indexOf(",", ssidEndIndex);
      int rssiEndIndex = response.indexOf(",", rssiStartIndex + 1);
      String rssi = response.substring(rssiStartIndex + 1, rssiEndIndex);

      if(rssi.length() == 0 || ssid.length() == 0){
        index = response.indexOf("\n", rssiEndIndex);
        continue;
      }

      int numLeds = mapRssiToNumLeds(rssi.toInt());

      // Set the first "numLeds" LEDs on the strip to green
      for (int i = 0; i < numLeds; i++) {
        strip.setPixelColor(i, (((float)(max_Leds - i))/max_Leds) * 255, (((float)i)/max_Leds) * 255, 0);
      }

      // Set the remaining LEDs on the strip to black (off)
      for (int i = numLeds; i < 10; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }

      // Show the updated pixel colors on the strip
      strip.show();

      // print the SSID and RSSI on the LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(ssid);
      lcd.setCursor(0, 1);
      lcd.print(rssi);
      delay(1000); // delay 1 second between each network

      // set the index for the next iteration
      index = response.indexOf("\n", rssiEndIndex);
    }
  }
}

// Function to map an RSSI value to a number of LEDs
int mapRssiToNumLeds(int rssi) {
  // Set the minimum and maximum RSSI values
  // that will be used for mapping
  const int minRssi = -100;
  const int maxRssi = -50;

  // Map the RSSI value to the range 0-10
  int numLeds = map(rssi, minRssi, maxRssi, 0, 10);

  // Limit the number of LEDs to the maximum number
  // of LEDs on the strip
  numLeds = min(numLeds, 10);

  return numLeds;
}