#include <Arduino.h>
#include <SPI.h>
#include <Arduino_LSM6DS3.h>
#include <ArduinoJson.h>
#include <QubitroMqttClient.h>
#include <WiFiNINA.h>

void receivedMessage(int messageSize);  

/*struct senseData{

  float x = 0.0F;
  float y = 0.0F;
  float z = 0.0F;
};*/

static char payload[256];
//static senseData data;
StaticJsonDocument<256> doc;

#define PERIOD 100

WiFiClient wifiClient;
QubitroMqttClient mqtt(wifiClient);

const char* ssid = "";
const char* password = "";
char deviceID[] = "";
char deviceToken[] = "";
char host[] = "broker.qubitro.com";
int port = 1883;
unsigned long next = 0;

void receivedMessage(int messageSize) 
{
  Serial.print("New message received:");

  while (mqtt.available()) 
  {
    Serial.print((char)mqtt.read());
  }
  Serial.println();
}

void setup() {

  Serial.begin(9600);
  while (!Serial);
  
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while(1);
  }

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\tConnected to the WiFi!");

  mqtt.setId(deviceID);
  mqtt.setDeviceIdToken(deviceID, deviceToken);

  Serial.println("Connecting to Qubitro...");

  if (!mqtt.connect(host, port))
  {
    Serial.println("Connection failed! Error code = ");
    Serial.println(mqtt.connectError());
    while(1);
  }

  Serial.println("Connected to the Qubitro!");
  mqtt.onMessage(receivedMessage);
  mqtt.subscribe(deviceID);
  
}

void loop() {

  mqtt.poll();

  if (millis() > next)
  {
    float x, y, z;

    if (IMU.accelerationAvailable())
    {
      IMU.readAcceleration(x, y, z);
      
      doc["X"] = x;
      doc["Y"] = y;
      doc["Z"] = z;

      serializeJsonPretty(doc, payload);

      next = millis() + PERIOD;
      mqtt.beginMessage(deviceID);

      //send value
      mqtt.print(payload);
      mqtt.endMessage();

    }
    
  }
  
}