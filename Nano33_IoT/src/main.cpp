#include <Arduino.h>
#include <SPI.h>
#include <Arduino_LSM6DS3.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiNINA.h>

struct senseData{

  float x = 0.0F;
  float y = 0.0F;
  float z = 0.0F;
};

void setup_wifi();
void reconnect();

static char payload[256];
static senseData data;
StaticJsonDocument<256> doc;

const char* ssid = "";
const char* password = "";
const char mqtt_server[] = "";
const char publishTopic[] = "";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void setup_wifi(){

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while( WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");   
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
}

void reconnect(){

  while(!mqtt.connected()){
    
    Serial.print("Attempting MQTT connection ....");
    String clientID = "nano33_accelerometer-";
    clientID += String(random(0xffff), HEX);
    
    if (mqtt.connect(clientID.c_str())) { 
      
      Serial.println("Connected to MQTT Broker");
      digitalWrite(LED_BUILTIN, HIGH);
    }

    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println("try again in 5 second");
      digitalWrite(LED_BUILTIN, LOW);

    }
       
  }
  
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while(1);
  }

  setup_wifi();
  mqtt.setServer(mqtt_server, 1883);

}

void loop() {

  if (!mqtt.connected())
  {
    reconnect();
  }

  mqtt.loop();

  float x, y, z;

  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
    data.x = x;
    data.y = y;
    data.z = z;

    doc["X"] = data.x;
    doc["Y"] = data.y;
    doc["Z"] = data.z;

    serializeJsonPretty(doc, payload);
    mqtt.publish(publishTopic, payload);
    Serial.println(payload);
   
  }

}