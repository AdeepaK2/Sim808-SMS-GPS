#include <DFRobot_sim808.h>
#include <SoftwareSerial.h>

// ThingSpeak API key
#define THINGSPEAK_API_KEY "YOUR_THINGSPEAK_WRITE_API_KEY"

// SIM808 PINs
#define PIN_TX 10
#define PIN_RX 11
SoftwareSerial mySerial(PIN_TX, PIN_RX);

// Initialize SIM808 module
DFRobot_SIM808 sim808(&mySerial);

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);

  // Initialize sim808 module
  while (!sim808.init()) {
    Serial.println("Sim808 init error");
    delay(1000);
  }

  // Wait a bit before starting main loop
  delay(3000);
  Serial.println("SIM808 init success. Starting...");

  // Turn on GPRS
  setupGPRS();
}

void loop() {
  // Collect and send GPS data
  getGPS();

  // Wait for 30 seconds before sending data again
  delay(30000);
}

void setupGPRS() {
  // Set your APN, username and password for GPRS connection
  const char* apn = "YOUR_APN";
  const char* user = "YOUR_USERNAME";
  const char* pass = "YOUR_PASSWORD";

  // Attempt to connect to GPRS using the provided credentials
  if (!sim808.join(apn, user, pass)) {
    Serial.println("GPRS connection failed, retrying...");
    delay(2000);
    setupGPRS(); // Recursive retry
  } else {
    Serial.println("GPRS connection successful");
  }
}

void getGPS() {
  // Ensure GPS is powered and ready
  if (!sim808.attachGPS()) {
    Serial.println("Failed to power GPS");
    return;
  }

  // Obtain GPS data
  if (sim808.getGPS()) {
    float lat = sim808.GPSdata.lat, lon = sim808.GPSdata.lon;
    Serial.print("Latitude: ");
    Serial.println(lat, 6);
    Serial.print("Longitude: ");
    Serial.println(lon, 6);

    // Send data to ThingSpeak
    sendDataToThingSpeak(lat, lon);
  } else {
    Serial.println("Failed to get GPS data");
  }
}

void sendDataToThingSpeak(float latitude, float longitude) {
  // Construct API URL for updating ThingSpeak channel
  String url = "AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=";
  url += THINGSPEAK_API_KEY;
  url += "&field1=";
  url += String(latitude, 6);
  url += "&field2=";
  url += String(longitude, 6);
  url += "\"";

  sim808.sendATcommand("AT+HTTPTERM", "OK", 1000); // Terminate any existing HTTP connection
  sim808.sendATcommand("AT+HTTPINIT", "OK", 1000); // Initialize HTTP connection
  sim808.sendATcommand("AT+SAPBR=1,1", "OK", 5000); // Open the GPRS context
  sim808.sendATcommand(url.c_str(), "OK", 5000); // Set parameters
  sim808.sendATcommand("AT+HTTPACTION=0", "+HTTPACTION:", 15000); // Start GET action

  // Check for success and read response here (not shown for brevity)

  Serial.println("GPS data sent to ThingSpeak");
}
