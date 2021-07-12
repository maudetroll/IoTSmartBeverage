/**
   httpUpdate.ino

    Created on: 27.11.2015

*/
#define MANUAL_SIGNING 1
#define SET_TWO_WAY_SECRETS

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "secret.h"

const char pubkey[] PROGMEM = R"EOF(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArSf4W99aciiCoDH427w5
FE95jL7R/2tC4pYOyZWq3yTVl0Kq6y33L9GgLS6kCILLBi0KSGykOQX3kEzOnZa2
nesjLZXwTxWeRlq1f0OcRMXXNjbVg+kDepKoinW3ch1fD29sLpzUgtNwIt7fAahe
eZGIsytNMnLPRUf0mjKbWe9xgyT88EETPLzsJ9Lw+CJUBxxenmzh5XbU8H/VwUJq
Kjd2ta8jnK6htBPPMvdYpTpCqE+QY4Tp8VmKv2hnCrb8XlIyEfD5y+y5qrIF4Bg1
vRKQD82QmNZon2ASuqPUz45ZQwVqTQSt8Pg4QI7sViO5LTmJuqAAQecHVEZ8ae3J
BwIDAQAB
-----END PUBLIC KEY-----
)EOF";
#if MANUAL_SIGNING
BearSSL::PublicKey *signPubKey = nullptr;
BearSSL::HashSHA256 *hash;
BearSSL::SigningVerifier *sign;
#endif

BearSSL::X509List *list = nullptr;
BearSSL::PrivateKey *key = nullptr;

#include <time.h>

#include <FS.h>
#include <LittleFS.h>

#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;

#include <base64.h>

#include <PubSubClient.h>

BearSSL::WiFiClientSecure client;
PubSubClient pubsub_client(client);

const String version = "VERSION_0.0.1";

ESP8266WiFiMulti WiFiMulti;

// Everything we need for our sensors

#include "HX711.h"
#define DOUT D1
#define CLK D8
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <MFRC522.h>
#include <PubSubClient.h>
#include <SPI.h>

// Definiere die notwendigen PINs am ESP8266
constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;

// Definiere den MQTT Broker -> Adresse des Raspberry PIs im lokalen Netzwerk
const char *MQTT_BROKER = "192.168.178.128";
const int MQTT_PORT = 1883;

// Definiere den Update Server -> Adresse des Raspberry PIs im lokalen Netzwerk
const String UPDATE_SERVER = "192.168.178.128";
const uint16_t UPDATE_SERVER_PORT = 8081;

// Definiere den Pfad zum Update Binary
const String UPDATE_BIN_PATH = "/firmware.bin.signed";


MFRC522 rfid(SS_PIN, RST_PIN);

String rfidtag;
String tag;
String mac;
double masse;

// Abhängig vom der verwendeteten Wägezelle ist hierbei der Kalibirierungsfaktor
// einzutragen

// float calibration_factor = -339000; // Andere Wägezelle
float calibration_factor = -495000;

// Scale class object
HX711 scale;


void setClock() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // UTC

    Serial.print(F("Waiting for NTP time sync: "));
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        yield();
        delay(500);
        Serial.print(F("."));
        now = time(nullptr);
    }

    Serial.println(F(""));
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

void update_started() {
    Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
    Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
    Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur,
                  total);
}

void update_error(int err) {
    Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    setClock();

    bool mfln = client.probeMaxFragmentLength(
        "server", 443,
        1024); // server must be the same as in ESPhttpUpdate.update()
    Serial.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
    if (mfln) {
        client.setBufferSizes(1024, 1024);
    }

    Serial.print("Local WiFi: ");
    Serial.println(WiFi.localIP());

#if MANUAL_SIGNING
    // Ensure all updates are signed appropriately.  W/o this call, all will be
    // accepted.
    Update.installSignature(hash, sign);
#endif

    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    String header = base64::encode(version + "::" + SECRET);
    t_httpUpdate_return ret = ESPhttpUpdate.update(
        client, UPDATE_SERVER, UPDATE_SERVER_PORT, UPDATE_BIN_PATH);

    switch (ret) {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n",
                      ESPhttpUpdate.getLastError(),
                      ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!pubsub_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (pubsub_client.connect("arduinoClient")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            pubsub_client.publish("sensor-log-topic", "bar sensor is up");
            // ... and resubscribe
            pubsub_client.subscribe("update");
        } else {
            Serial.print("failed, rc=");
            Serial.print(pubsub_client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

/*
 * Setup for our hx711 scale
 */
void scale_setup() {
    scale.begin(DOUT, CLK);

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

void mqtt_setup() {
    pubsub_client.setServer(MQTT_BROKER, MQTT_PORT);
    pubsub_client.setCallback(callback);
    delay(1500);
}

void client_tls_setup() {
    list = new BearSSL::X509List(cert);
    key = new BearSSL::PrivateKey(private_key);
    client.setClientRSACert(list, key);
    delay(1500);
}

void setup() {

    Serial.begin(115200);
    // Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(APSSID, APPSK);

#if MANUAL_SIGNING
    signPubKey = new BearSSL::PublicKey(pubkey);
    hash = new BearSSL::HashSHA256();
    sign = new BearSSL::SigningVerifier(signPubKey);
#endif

    LittleFS.begin();

    Serial.print("Exists");
    Serial.println(LittleFS.exists("/certs.ar"));
    int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"),
                                           PSTR("/certs.ar"));
    Serial.print(F("Number of CA certs read: "));
    Serial.println(numCerts);
    if (numCerts == 0) {
        Serial.println(F("No certs found. Did you run certs-from-mozill.py and "
                         "upload the LittleFS directory before running?"));
        return; // Can't connect to anything w/o certs!
    }
    client.setCertStore(&certStore);

    mqtt_setup();
    client_tls_setup();
    scale_setup();
}

void loop() {
    // wait for WiFi connection

    // WLAN Verbindung aufbauen
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        if (!pubsub_client.connected()) {
            reconnect();
        }
        // Check if this blocks
        Serial.print("Before client.loop");
        pubsub_client.loop();
        Serial.print("After client.loop");

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
        // Zusammensetzen der MQTT-Nachricht
        String mqttMessage = mac + " , " + tag + " , " + String(masse);
        String masseAlsString = String(masse);
        char buf[64];
        mqttMessage.toCharArray(buf, 64);

        // Tatsächlicher Versand
        pubsub_client.publish("massBeverage", buf);
        Serial.println("Versendet");
        rfidtag = "";
        tag = "";
    }
}
