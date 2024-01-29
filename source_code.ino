#include <DFRobot_sim808.h>
#include <SoftwareSerial.h>

#define MESSAGE_LENGTH 160
char lat[12];
char lon[12];
char wspeed[12];

#define PIN_TX 10
#define PIN_RX 11
SoftwareSerial mySerial(PIN_TX, PIN_RX);
DFRobot_SIM808 sim808(&mySerial); // Connect RX,TX,PWR,

void getGPS();

void setup()
{
  mySerial.begin(9600);
  Serial.begin(9600);

  //******** Initialize sim808 module *************
  while (!sim808.init())
  {
    Serial.print("Sim808 init error\r\n");
    delay(1000);
  }
  delay(3000);

  Serial.println("SIM Init success");

  Serial.println("Init Success, sending location every 30 seconds!");
}

void loop()
{
  // Get GPS location
  getGPS();

  // Wait for 30 seconds before reading GPS again
  delay(30000);
}

void getGPS()
{
  while (!sim808.attachGPS())
  {
    Serial.println("Open the GPS power failure");
    delay(1000);
  }
  delay(3000);

  Serial.println("Open the GPS power success");

  while (!sim808.getGPS())
  {
  }

  Serial.print("latitude :");
  Serial.println(sim808.GPSdata.lat);
  Serial.print("longitude :");
  Serial.println(sim808.GPSdata.lon);
  Serial.print("speed_kph :");
  Serial.println(sim808.GPSdata.speed_kph);
  Serial.print("heading :");
  Serial.println(sim808.GPSdata.heading);
  Serial.println();

  float la = sim808.GPSdata.lat;
  float lo = sim808.GPSdata.lon;
  float ws = sim808.GPSdata.speed_kph;

  dtostrf(la, 4, 6, lat);
  dtostrf(lo, 4, 6, lon);
  dtostrf(ws, 6, 2, wspeed);

  Serial.println("Location:");
  Serial.print("Latitude : ");
  Serial.println(lat);
  Serial.print("Longitude : ");
  Serial.println(lon);

  // Create and print the map link
  char mapLink[150];
  snprintf(mapLink, sizeof(mapLink), "Map Link: http://maps.google.com/maps?q=%s,%s", lat, lon);
  Serial.println(mapLink);
  Serial.println();
}
