/*
Basic code for ESP OTA updates. Uses a dedicated update server to fetch the
latest compiled binary after receiving an update notification from a central
MQTT server. Binaries are signed and connection to the update server is
encrypted. The certification (at the moment `localhost`) for the update server
must be contained in the certificates stored on the filesystem in `./cert.ca`.
When trying to fetch the newest binary, we send the current version and a
secret to authenticate ourselves. Version and SECRET layout are following:

    base64encode(VERSION_[NUMBER]{+}.[NUMBER]{+}.[NUMBER]{+}::${SECRET})

The update server must check if we should receive a new binary based on our
current version and the provided secret. 

Program flow:
setup(): 
-  setup the update basics
    * configure the updater to check for a signed binary with known public key and hash
    * import known certificates from filesystem (Mozilla Certs + Owncert)
- setup wifi with hard coded credentials
    * Will loop till connection is established
- setup configuration for MQTT Server but do not yet connect
run():
- Recheck if connected to wifi 
- Check if connected to MQTT Server
    * first time: false
    * connect with random client id to hard coded MQTT server
    * publish to "hello world" to "espStartedTopic" 
    * subscribe to "updateTopic" and connect it with callback() function

--- ASYNC
- callback() -> prints received messages and calls update() function with received message
- update() -> should:
    * parse message and determine the update server
    * setup update client 
    * register known certificates to client
    * call update function and handle return code

 
TODO:
- Problem, if we send the update to a random server than this one can get our
  secret. But MAYBE the we wouldn't send the request because the certificate of
  this server is not good?
- https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266httpUpdate/src/ESP8266httpUpdate.cpp
  has another function called `updateFS`, which could be used to remotely
  update the file system. When we need to update the certifications, we would
  either need to send a new file, which is written to the filesystem and then
  reloaded into the certification store, or we send a whole updated filesystem
  and reload the certifications.
*/


/* OLD DESCRIPTION
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP
 Led, else switch it off It will reconnect to the server if the connection is
 lost using a blocking reconnect function. See the 'mqtt_reconnect_nonblocking'
 example for how to achieve the same result without blocking the main loop. To
 install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences ->
 Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the
 ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <Arduino.h>
#include <base64.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// MQTT imports
#include <PubSubClient.h>

// Update Imports
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

// Encrypt Imports
#include <CertStoreBearSSL.h>

#include <time.h>

#include <FS.h>
#include <LittleFS.h>

// Version will be saved as a Strinvg
const String version = "VERSION_0.0.1";

// Updater Signed and Encryption
//      -> Sign Part
const char pubkey[] PROGMEM = R"EOF(
-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA6+b9uFPLWhxpHVeGNNGD
fuTzhJESPQiotvdXwkOXugjmORLXZ6LyeX3x7jG1Ztv4prXfYG+gdemzjB6ezc7+
skVbxBihHgxhFmliDkG6ciJuHK/oA2iTkWb8fP6+iYlE/9d/bmmP+Gsi3izBsXP0
SOjzFgh4LM0Vf2LhcRvapfkFc3YVtp+3O6NEVgCf1S0xpgPxCyMGXNVv4gJWJZLW
T3zrEMqY3J/fI9KmpP2y9lvuLUruZTGGNCvkqpfI1yet5IpQJZYts2fHsMXfkCi5
3VwD7wzGCjabJaZoHj7CuWH4HgJwhxM3g2uu3DIr6mPKxXr4vV4XrvHlz1UFrc3Q
Olp3gZ5rU9L88UdM47s70LCadNCCd93+LFGdDsvsjWdrRECdyRQpsnoFpW5J5Cpw
6T1oTJf63OOHhB4agHFyAs24ZLaTuVYHHlTeysfHYBIIp3LoAFids2MW7r+asHbc
ZVmxMwsIelnHeM+WUMBXvDjG49DLVCRqADthOmepmgqMD8CeA1zYojYTy7Hkgn7F
dDcb2Mz2PAycvhdcMRgtVOKfRpjxGQQP2n5jZWIYN6rQHb/MnDC63y5urqEgJphW
MDzQIfCSABx+3WG76mDs8L/4HUQj5AKpq0u8uVgMwUGnzGHfy1SGCrHo5OiPDViz
8M0NAeXgwrcKt3HohF66YBkCAwEAAQ==
-----END PUBLIC KEY-----
)EOF";

const char secret[] PROGMEM = R"EOF(a&1y+Z_mbR\!dxTO'56!i@KKxC?BTAF~&1TBZ$$xh_em@jM*8_|n'V"JfS3/qj$t)EOF";

#if MANUAL_SIGNING
BearSSL::PublicKey *signPubKey = nullptr;
BearSSL::HashSHA256 *hash;
BearSSL::SigningVerifier *sign;
#endif
//      -> Encrypt Part
BearSSL::CertStore certStore;

// MQTT and WIFI Constants and Variables
ESP8266WiFiMulti WiFiMulti;
const char *ssid = "........";
const char *password = "........";
// TODO get the right mqtt endpoint, maybe add local one for debugging
const char *mqtt_server = "broker.mqtt-dashboard.com";

// TODO check if we can use the bearssl secure wifi client to connect to the mqtt server 
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// TODO Should run before WiFi connection ?
void setup_updater() {

#if MANUAL_SIGNING
  signPubKey = new BearSSL::PublicKey(pubkey);
  hash = new BearSSL::HashSHA256();
  sign = new BearSSL::SigningVerifier(signPubKey);
  Update.installSignature(hash, sign);
#endif

  LittleFS.begin();
  // Use https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_CertStore/certs-from-mozilla.py
  // To generate `./cert.ar`. `./cert.idx` is generated when data is written to LittleFS
  // Use `make add-our-cert` to add our cert `./self-signed-cert/ourcert.der` to `./data/cert.ar`
  // Normaly our cert should already be in the `cert.ar` which is contained in the repo
  int numCerts =
      certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.print(F("Number of CA certs read: "));
  Serial.println(numCerts);
  if (numCerts == 0) {
    Serial.println(F("No certs found. Did you run certs-from-mozill.py and "
                     "upload the LittleFS directory before running?"));
    return; // Can't connect to anything w/o certs!
  }
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  // TODO figure out what the right configuration for German time is. See code in: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/time.cpp
  configTime(1, 0, "pool.ntp.org", "time.nist.gov"); // UTC

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

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  while (WiFiMulti.run() != WL_CONNECTED) {
    // Give connection a bit time before retrying
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

char* get_server_name(char* update_file_address) {
    // TODO return only server name, e.g. https://servername/path/to/file.bin -> servername
    // TODO maybe have to add '\0' byte if client.probeMaxFragmentLength expects a zero byte
    return update_file_address;
}

int update(char *update_file_address) {
  setClock();
  BearSSL::WiFiClientSecure client;
  char* server_name = get_server_name(update_file_address);

  bool mfln = client.probeMaxFragmentLength(server_name, 443, 1024); 

  Serial.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
  if (mfln) {
    client.setBufferSizes(1024, 1024);
  }

  // TODO check if BearSSL we can already somehow check if the certificate for
  // this server `localhost` is alright, what happens if we fail to verify the
  // server server must be the same as in ESPhttpUpdate.update()
  client.setCertStore(&certStore);

  // The line below is optional. It can be used to blink the LED on the board
  // during flashing The LED will be on during download of one buffer of data
  // from the network. The LED will be off during writing that buffer to flash
  // On a good connection the LED should flash regularly. On a bad connection
  // the LED will be on much longer than it will be off. Other pins than
  // LED_BUILTIN may be used. The second value is used to put the LED on. If the
  // LED is on with HIGH, that value should be passed
  // TODO Do all ESPs have this LED?
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  String header = base64::encode(version + "::" + secret);
  t_httpUpdate_return ret =
      ESPhttpUpdate.update(client, update_file_address, header.c_str());
  //    ESPhttpUpdate.update(client, "https://server/file.bin", VERSION); TODO remove was original

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n",
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

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED,
                 LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED,
                 HIGH); // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      String message = String("hello world from ");
      message.concat(clientId);
      client.publish("espStartedTopic", message.c_str());
      // ... and resubscribe
      client.subscribe("updateTopic");
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
  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_updater();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // Check connection to the WiFi
  if (WiFiMulti.run() != WL_CONNECTED) {
    setup_wifi();
  }

  // Check connection to the MQTT Server
  if (!client.connected()) {
    reconnect();
  }
  // Is this function blocking?
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    // TODO Measure Value
    snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
