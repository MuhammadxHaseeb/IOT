// /****************************************************
//  * ESP32 + DHT22 + MQTT (PUBLISHER ONLY)
//  * Topic:
//  *   home/lab1/temp
//  *   home/lab1/hum
//  ****************************************************/

// #include <Arduino.h>
// #include <WiFi.h>
// #include <PubSubClient.h>
// #include "DHT.h"

// // ---------- WiFi ----------
// char ssid[] = "Wokwi-GUEST";
// char pass[] = "";

// // ---------- MQTT ----------
// const char* mqtt_server = "10.110.38.174";  // Mosquitto / Cloud broker
// const int   mqtt_port   = 1883;

// // Topics
// const char* TOPIC_TEMP = "home/lab2/temp";
// const char* TOPIC_HUM  = "home/lab1/hum";

// // ---------- DHT ----------
// #define DHTPIN  23
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// // ---------- MQTT Client ----------
// WiFiClient espClient;
// PubSubClient mqtt(espClient);

// // ---------- Functions ----------
// void connectWiFi() {
//   Serial.print("Connecting to WiFi");
//   WiFi.begin(ssid, pass);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected");
// }

// void connectMQTT() {
//   while (!mqtt.connected()) {
//     Serial.print("Connecting to MQTT...");
//     if (mqtt.connect("ESP32_Publisher-1")) {
//       Serial.println("connected");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.println(mqtt.state());
//       delay(2000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);

//   dht.begin();
//   connectWiFi();

//   mqtt.setServer(mqtt_server, mqtt_port);
//   connectMQTT();
// }

// void loop() {
//   if (!mqtt.connected()) {
//     connectMQTT();
//   }
//   mqtt.loop();

//   float temperature = dht.readTemperature();
//   float humidity    = dht.readHumidity();

//   if (isnan(temperature) || isnan(humidity)) {
//     Serial.println("DHT read failed");
//     delay(2000);
//     return;
//   }

//   char tBuf[8], hBuf[8];
//   dtostrf(temperature, 4, 2, tBuf);
//   dtostrf(humidity,    4, 2, hBuf);

//   mqtt.publish(TOPIC_TEMP, tBuf);
//   mqtt.publish(TOPIC_HUM,  hBuf);

//   Serial.print("Published -> Temp: ");
//   Serial.print(tBuf);
//   Serial.print(" C | Hum: ");
//   Serial.print(hBuf);
//   Serial.println(" %");

//   delay(5000);  // publish every 5 seconds
// }

#include <WiFi.h> 
#include <PubSubClient.h>
#include <DHTesp.h>


const int DHT_PIN = 23; 
DHTesp dht;
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "192.168.18.22";


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
float temp = 0;
float hum = 0;


void setup_wifi() {
 delay(10);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);


 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);


 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }


 randomSeed(micros());


 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i = 0; i < length; i++) {
   Serial.print((char)payload[i]);
 }}
void reconnect() {
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   String clientId = "ESP32Client-";
   clientId += String(random(0xffff), HEX);
   if (client.connect(clientId.c_str())) {
     Serial.println("Connected");
     client.publish("/ThinkIOT/Publish", "Welcome");
     client.subscribe("/ThinkIOT/Subscribe");
   } else {
     Serial.print("failed, rc=");
     Serial.print(client.state());
     Serial.println(" try again in 5 seconds");
     delay(5000);
   }}
}
void setup() {
 pinMode(2, OUTPUT);    
 Serial.begin(115200);
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 dht.setup(DHT_PIN, DHTesp::DHT22);
}
void loop() {
 if (!client.connected()) {
   reconnect();
 }
 client.loop();


 unsigned long now = millis();
 if (now - lastMsg > 2000) { 
   lastMsg = now;
   TempAndHumidity  data = dht.getTempAndHumidity();


   String temp = String(data.temperature, 2);
   client.publish("/ThinkIOT/temp", temp.c_str()); 
   String hum = String(data.humidity, 1);
   client.publish("/ThinkIOT/hum", hum.c_str()); 


   Serial.print("Temperature: ");
   Serial.println(temp);
   Serial.print("Humidity: ");
   Serial.println(hum);
 }
}
