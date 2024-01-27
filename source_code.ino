#include <DFRobot_sim808.h>
#include <SoftwareSerial.h>

#define MESSAGE_LENGTH 160
char message[MESSAGE_LENGTH];
int messageIndex = 0;
char MESSAGE[300];
char lat[12];
char lon[12];
char wspeed[12];

char phone[16];
char datetime[24];

#define PIN_TX 10
#define PIN_RX 11
SoftwareSerial mySerial(PIN_TX, PIN_RX);
DFRobot_SIM808 sim808(&mySerial); // Connect RX,TX,PWR,

void getGPS();
void readSMS();

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

  Serial.println("Init Success, please send SMS message to me!");
}

void loop()
{
  //*********** Detecting unread SMS ************************
  messageIndex = sim808.isSMSunread();

  //*********** At least, there is one UNREAD SMS ***********
  if (messageIndex > 0)
  {

    readSMS();
    getGPS();

    //************* Turn off the GPS power ************
    sim808.detachGPS();

    Serial.println("Please send SMS message to me!");
  }

  // Wait for 30 seconds before reading GPS again
  delay(30000);
}

void readSMS()
{
  Serial.print("messageIndex: ");
  Serial.println(messageIndex);

  sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);

  //***********In order not to fill SIM Memory, it's better to delete the SMS**********
  sim808.deleteSMS(messageIndex);
  Serial.print("From number: ");
  Serial.println(phone);
  Serial.print("Datetime: ");
  Serial.println(datetime);
  Serial.print("Received Message: ");
  Serial.println(message);
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

  Serial.print(sim808.GPSdata.year);
  Serial.print("/");
  Serial.print(sim808.GPSdata.month);
  Serial.print("/");
  Serial.print(sim808.GPSdata.day);
  Serial.print(" ");
  Serial.print(sim808.GPSdata.hour);
  Serial.print(":");
  Serial.print(sim808.GPSdata.minute);
  Serial.print(":");
  Serial.print(sim808.GPSdata.second);
  Serial.print(":");
  Serial.println(sim808.GPSdata.centisecond);
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

  dtostrf(la, 4, 6, lat);    // Put float value of la into char array of lat. 4 = number of digits before decimal sign. 6 = number of digits after the decimal sign.
  dtostrf(lo, 4, 6, lon);    // Put float value of lo into char array of lon
  dtostrf(ws, 6, 2, wspeed); // Put float value of ws into char array of wspeed

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

