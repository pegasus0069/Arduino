//Used Components : DHT22, MQ2, BMP280, ESP8266 ESP-01(WiFi Module), PIR Sensor, OLED Display (Blue I2C 128x64 0.96")
#define BLYNK_PRINT Serial

#include <MQ2.h> 
#include <Wire.h>              // include Arduino wire library (required for I2C devices)
#include <SPI.h>
#include <Adafruit_BMP280.h>   // include Adafruit BMP280 sensor library
#include <Adafruit_Sensor.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_SSD1306.h>  // include Adafruit SSD1306 OLED display driver

 
#define OLED_RESET  4    // define display reset pin
Adafruit_SSD1306 display(OLED_RESET);  // initialize Adafruit display library
 
// define device I2C address: 0x76 or 0x77 (0x77 is library default address)
#define BMP280_I2C_ADDRESS  0x76 
Adafruit_BMP280  bmp280;  // initialize Adafruit BMP280 library

char auth[] = "aa7c4d6091bd4f51bd8ab4f9aed0a9a2";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "pegasus007";
char pass[] = "kudduschacha";

#define EspSerial Serial

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);

#define DHTPIN 3          // What digital pin we're connected to

#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321


DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP280 bmp; // BMP-280
BlynkTimer timer_dht;
BlynkTimer timer_bmp;
BlynkTimer timer_mq2;
// MQ2
int pin = A0;
int lpg, co, smoke;

MQ2 mq2(pin);

const unsigned char FabLabIUB [] PROGMEM = {
  // Fab Lab Logo, 128x32px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1f, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x1f, 0xfb, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x07, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x9f, 0xe3, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x8f, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xcf, 0xcf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xcf, 0xcf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8f, 0xcf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc2, 0x10, 0x21, 0x4b, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1b, 0x11, 0xb1, 0x4a, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xea, 0x90, 0xa9, 0x4b, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3a, 0x91, 0xa9, 0x4a, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3b, 0x9d, 0xb9, 0x33, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void sendDHT()
{
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V4, h);
}

  void sendBMP() {
  float t = bmp.readTemperature();
  float p = bmp.readPressure();

  if (isnan(p) || isnan(t)) {
    Serial.println("Failed to read from BMP sensor!");
    return;
  }
  p *= 0.01;
  //p *= 0.00750062;
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V5, p);
  Serial.println(t);
  Serial.println(p);
}
void sendMQ2() {
  Serial.println("Concentration of gas ");
  Serial.print("LPG: ");
  Serial.print(lpg);
  Serial.println(" ppm");
  Serial.print("CO: ");
  Serial.print(co);
  Serial.println(" ppm");
  Serial.print("Smoke: ");
  Serial.print(smoke);
  Serial.println(" ppm");
  Blynk.virtualWrite(V8, lpg);
  Blynk.virtualWrite(V7, co);
  Blynk.virtualWrite(V9, smoke);
  delay(200);
}

void setup(void)
{
  Serial.begin(9600);
  // Initialize the SSD1306 OLED display with I2C address = 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay(); // Make sure the display is cleared
  // Draw the bitmap:
  // DrawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.drawBitmap(0, 0, FabLabIUB, 128, 32, WHITE);

  // Update the display
  display.display();
  delay(6000);  // wait a second

  // Clear the display buffer.
  display.clearDisplay();
  display.setTextSize(1);   // text size = 1
  display.setTextColor(WHITE, BLACK);  // set text color to white and black background
  display.display();        // update the display
  // Initialize the BMP280 sensor
   
  if( bmp280.begin(BMP280_I2C_ADDRESS) == 0 )
  {  // connection error or device address wrong!
    display.setTextSize(2);   // text size = 2
    display.setCursor(5, 17);
    display.print("Connection");
    display.setCursor(35, 37);
    display.print("Error");
    display.display();        // update the display
    while(1);  // stay here
  }
  
  EspSerial.begin(ESP8266_BAUD);
  display.setCursor(0, 12);
  display.print(" Connecting to WiFi!");
  display.display();        // update the display
  display.setTextSize(1);  
  //Blynk.begin(auth, wifi, ssid, pass);
  //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 80);
  Blynk.begin(auth, wifi, ssid, pass, "192.168.1.11", 8080);
  display.clearDisplay();
  display.setCursor(0, 12);
  display.print(" Connected to WiFi!");
  display.display();        // update the display
  display.setTextSize(1);
  // update the display
  display.display();
  delay(2000);
  display.clearDisplay();
  
  dht.begin();
  bmp.begin(0x76);
  mq2.begin();
  timer_dht.setInterval(15000L, sendDHT);
  timer_bmp.setInterval(9000L, sendBMP);
  timer_mq2.setInterval(12000L, sendMQ2);
}
 
char _buffer[9];
 
void loop()
{    
  //MQ2
  /*read the values from the sensor, it returns
  *an array which contains 3 values.
  * 1 = LPG in ppm
  * 2 = CO in ppm
  * 3 = SMOKE in ppm
  */
  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial
  
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  co = mq2.readCO();
  //smoke = values[2];
  smoke = mq2.readSmoke();
  
  delay(1000);
  display.setCursor(0, 0);
  display.print("TEMP:");
  display.setCursor(0, 9);
  display.print("PRES:");
  display.setCursor(68, 0);
  display.print("LPG:");
  display.setCursor(0, 18);
  display.print("CO:");
  display.setCursor(52, 18);
  display.print("SMOKE:");
  display.display();        // update the display
  display.setTextSize(1);   // text size = 1
  // Get Temperature and Pressure from Library
  float temp     = bmp280.readTemperature();   // get temperature
  float pressure = bmp280.readPressure();      // get pressure
  // Print data on the OLED display
  // Print Temperature on the OLED display
  if(temp < 0)
    sprintf(_buffer, "-%02u.%02u", (int)abs(temp), (int)(abs(temp) * 100) % 100 );
  else
    sprintf(_buffer, "%02u.%02u", (int)temp, (int)(temp * 100) % 100 );
  display.setCursor(28, 0);
  display.print(_buffer);
 
  // Print Degree Symbols ( ° )
  display.drawCircle(61, 0, 1, WHITE);
 
  // Print Air Pressure on the OLED display
  sprintf(_buffer, "%04u.%02u", (int)(pressure/100), (int)((uint32_t)pressure % 100));
  display.setCursor(28, 9);
  display.print(_buffer);
  display.setCursor(72, 9);
  display.print("hPa");
 
  // Update the display
  display.display();
  // Print LPG on the OLED display
  display.setCursor(92, 0);
  display.print(lpg);
  display.print("ppm");
  display.setCursor(18, 18);
  display.print(co);
  display.setCursor(88, 18);
  display.print(smoke);
  display.display();   // update the display
  display.setTextSize(1);
  // update the display
  display.display();
  display.clearDisplay();

  Blynk.run();
  timer_dht.run();
  timer_bmp.run();
  timer_mq2.run();
} 
// end of code.
