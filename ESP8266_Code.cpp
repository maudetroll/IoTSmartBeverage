#include "HX711.h"
#define DOUT  D1
#define CLK  D8
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
typedef double gramm;

// Definiere die notwendigen PINs am ESP8266
constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;

// Definiere die WLAN-SSID und das Passwort
const char* ssid = "";
const char* password =  "";

// Definiere den MQTT Broker -> Adresse des Raspberry PIs im lokalen Netzwerk
const char* MQTT_BROKER = "";
const int MQTT_PIN= 1883;

double masse;

WiFiClient espClient;
PubSubClient client(espClient);

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

String rfidtag;
String tag;
String mac;

// Abhängig vom der verwendeteten Waeezelle ist hierbei der Kalibirierungsfaktor einzutragen

//float calibration_factor = -339000;
float calibration_factor = -495000;

boolean scaling = false;

HX711 scale;

void setup() {
  scale.begin(DOUT, CLK);
  Serial.begin(9600);

  // WLAN Verbindung aufbauen
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(WiFi.localIP());
  mac = WiFi.macAddress();
  client.setServer(MQTT_BROKER, MQTT_PIN);

  // RFID lesen
  Serial.println("SPI Begin");
  SPI.begin();
  Serial.println("PCD _INIT");
  rfid.PCD_Init();
  

  // Waegezelle einrichten
  Serial.println("Set scale");
  scale.set_scale();
  Serial.println("Tare scale");
  scale.tare(); 
  delay(5000);
  Serial.println("Finished Tare scale");

  long zero_factor = scale.read_average(); 
  Serial.print("Zero factor: "); 
  Serial.println(zero_factor);

}

void loop() {

  // sofern eine Karte vorhanden ist, beginne mit dem Ablauf
  // 1. Lese RFID aus
  // 2. Messe das Gewicht
  // 3. Versende diese Informationen via MQTT

  // Schritt 1: Lese RFID aus
  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
  for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
  }
  Serial.println(tag);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  
  delay(1500);
  

  // Schritt 2: Bestimme das Gewicht der Waegezelle
  // Setze dazu den Kalibrierungsfaktor
  scale.set_scale(calibration_factor); 

  Serial.print("Messe... ");
  Serial.print(scale.get_units(), 3);
  masse = scale.get_units();
  Serial.print(" kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  // Schritt 3: Versende die Informationen via MQTT

  // Verbindung zu WLAN überprüfen
  if (!client.connected()) {
  while (!client.connected()) {
      client.connect("MQTT_BROKER");
      
      delay(100);
    }
  }
  
  // Zusammensetzen der MQTT-Nachricht
  String mqttMessage = mac + " , " + tag + " , "+ String(masse);
  String masseAlsString= String(masse);
  char buf[64];
  mqttMessage.toCharArray(buf, 64);
  

  // Tatsächlicher Versand
  client.publish("massBeverage",buf);
  Serial.println("Versendet"); 
  rfidtag= ""; 
  tag= "";

}
