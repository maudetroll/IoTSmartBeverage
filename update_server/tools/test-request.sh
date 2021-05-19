# First should not work -> MD5 matches, 
curl \
-H "User-Agent: ESP8266-http-Update" \
-H "x-ESP8266-STA-MAC: 18:FE:AA:AA:AA:AA" \
-H "x-ESP8266-AP-MAC:  1A:FE:AA:AA:AA:AA" \
-H "x-ESP8266-free-space: 671744" \
-H "x-ESP8266-sketch-size: 373940" \
-H "x-ESP8266-sketch-md5: ${MD5_MATCH}" \
-H "x-ESP8266-chip-size: 4194304" \
-H "x-ESP8266-sdk-version: 1.3.0" \
-H "x-ESP8266-version: ${BASE64_VERSION_SECRET}" \
-H "x-ESP8266-mode: sketch" \
https://localhost:8080/update --cacert ../self-signed-cert/cert.pem > should-not-work

# Second should work -> MD5 does not matches
curl \
-H "User-Agent: ESP8266-http-Update" \
-H "x-ESP8266-STA-MAC: 18:FE:AA:AA:AA:AA" \
-H "x-ESP8266-AP-MAC:  1A:FE:AA:AA:AA:AA" \
-H "x-ESP8266-free-space: 671744" \
-H "x-ESP8266-sketch-size: 373940" \
-H "x-ESP8266-sketch-md5: aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" \
-H "x-ESP8266-chip-size: 4194304" \
-H "x-ESP8266-sdk-version: 1.3.0" \
-H "x-ESP8266-version: ${BASE64_VERSION_SECRET}" \
-H "x-ESP8266-mode: sketch" \
https://localhost:8080/update --cacert ../self-signed-cert/cert.pem > should-work
