#define ONE_WIRE_BUS 2

#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_EC10.h"
#include <EEPROM.h>

#define EC_PIN A0
float voltage,ecValue,temperature = 25;
DFRobot_EC10 ec;

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
WiFiClient  client;
const char * myWriteAPIKey = "KS1RSDHNKRK97L6G";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

float Celcius=0;

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASSWORD";

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float tempcondtosal(float cond, float temp){
 float sal;
 float r,ds;
 int ok=0;
 float a0,a1,a2,a3,a4,a5;
 float b0,b1,b2,b3,b4,b5;
 float c0,c1,c2,c3,c4;
 float r2;
 a0=0.008;
 a1=-0.1692;
 a2=25.3851;
 a3=14.0941;
 a4=-7.0261;
 a5=2.7081;
 b0=0.0005;
 b1=-0.0056;
 b2=-0.0066;
 b3=-0.0375;
 b4=0.0636;
 b5=-0.0144;
 c0=0.6766097;
 c1=0.0200564;
 c2=0.0001104259;
 c3=-0.00000069698;
 c4=0.0000000010031;

   if (temp<0 || 30<temp){
    return 0;
   }
   else{
    if (cond<=0){
     return 0;
    }
    else{
     r=cond/42914;
     r/=(c0+temp*(c1+temp*(c2+temp*(c3+temp*c4))));
     r2=sqrt(r);
     ds=b0+r2*(b1+r2*(b2+r2*(b3+r2*(b4+r2*b5))));
     ds*=((temp-15.0)/(1.0+0.0162*(temp-15.0)));
     sal=a0+r2*(a1+r2*(a2+r2*(a3+r2*(a4+r2*a5))))+ds;
     }
   }
   return sal;
}

void connectToWiFi() {
//Connect to WiFi Network
   Serial.println();
   Serial.println();
   Serial.print("Connecting to WiFi");
   Serial.println("...");
   WiFi.mode(WIFI_STA); 
   WiFi.begin(ssid, password);
   int retries = 0;
while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
   retries++;
   delay(500);
   Serial.print(".");
}
if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
}
if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
    Serial.println(F("Setup ready"));
}

void setup() {
  Serial.begin(115200); // start serial
  delay(10);
  connectToWiFi();
  pinMode(EC_PIN, INPUT);
  sensors.begin();
  ec.begin(); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
  float tot = 0;
  float salinity = 0;
  sensors.requestTemperatures();
  Celcius = sensors.getTempCByIndex(0);
  Serial.print(Celcius);
  static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U)  //time interval: 1s
    {
      timepoint = millis();
      voltage = analogRead(EC_PIN)/1024.0*5000;  // read the voltage
      Serial.print("voltage:");
      Serial.print(voltage);
      temperature = Celcius;  // read your temperature sensor to execute temperature compensation
      ecValue =  ec.readEC(voltage,temperature)*1000;  // convert voltage to EC with temperature compensation
      Serial.print("  temperature:");
      Serial.print(temperature);
      Serial.print("^C  EC:");
      Serial.print(ecValue);
      Serial.println("us/cm");
    }
    ec.calibration(voltage,temperature);  // calibration process by Serail CMD
  salinity = tempcondtosal(ecValue, temperature);
  Serial.print("Salinity: ");
  Serial.println(salinity);
  ThingSpeak.setField(1,temperature);
  ThingSpeak.setField(2,ecValue);
  ThingSpeak.setField(3,salinity);
  int x = ThingSpeak.writeFields(1, myWriteAPIKey);
  //uncomment if you want to get temperature in Fahrenheit
  //int x = ThingSpeak.writeField(myChannelNumber, 1, temperatureF, myWriteAPIKey);

  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
   Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  lastTime = millis();
 }
}
