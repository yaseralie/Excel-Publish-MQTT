//Receive MQTT message from excel to control lamps
//Using ESP8266
//Receive message from mqtt broker to turn on device
//Reference GPIO  https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

//Setup for MQTT and WiFi============================
#include <ESP8266WiFi.h>
//Library for MQTT:
#include <PubSubClient.h>
//Library for Json format using version 5:
#include <ArduinoJson.h>

//declare topic for subscribe message
const char* topic_sub = "Lamp_Excel";

// Update these with values suitable for your network.
const char* ssid = "SSID";
const char* password = "WiFi Password";
const char* mqtt_server = "1xx.xxx.xxx.xxx";

//for output
int lamp1 = 16; //lamp for mqtt connected D0
int lamp2 = 5; //lamp for start indicator D1
int lamp3 = 4; //lamp for stop indicator D2

WiFiClient espClient;
PubSubClient client(espClient);
//char msg[50];

void setup_wifi() {
  delay(100);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  //Receiving message as subscriber
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  String json_received;
  Serial.print("JSON Received:");
  for (int i = 0; i < length; i++) {
    json_received += ((char)payload[i]);
    //Serial.print((char)payload[i]);
  }
  Serial.println(json_received);

  //Parse json
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json_received);

  //get json parsed value
  //sample of json: {"device":"lamp1","trigger":"on"}
  Serial.print("Command:");
  String device = root["device"];
  String trigger = root["trigger"];
  Serial.println("Turn " + trigger + " " + device);
  Serial.println("-----------------------");
  //Trigger device
  //Lamp1***************************
  if (device == "lamp1")
  {
    if (trigger == "on")
    {
      digitalWrite(lamp1, HIGH);
    }
    else
    {
      digitalWrite(lamp1, LOW);
    }
  }
  //Lamp2***************************
  if (device == "lamp2")
  {
    if (trigger == "on")
    {
      digitalWrite(lamp2, HIGH);
    }
    else
    {
      digitalWrite(lamp2, LOW);
    }
  }
  //Lamp3***************************
  if (device == "lamp3")
  {
    if (trigger == "on")
    {
      digitalWrite(lamp3, HIGH);
    }
    else
    {
      digitalWrite(lamp3, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //once connected to MQTT broker, subscribe command if any
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //subscribe topic
  client.subscribe(topic_sub);
  //setup pin output
  pinMode(lamp1, OUTPUT);
  pinMode(lamp2, OUTPUT);
  pinMode(lamp3, OUTPUT);
  //Reset lamp, turn off all
  digitalWrite(lamp1, LOW);
  digitalWrite(lamp2, LOW);
  digitalWrite(lamp3, LOW);
}

void loop() {
  delay(200);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
