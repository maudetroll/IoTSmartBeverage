arduino_cmd := arduino
arduino_repo := ~/repos/Arduino/
mklittlefs := $(arduino_repo)/tools/mklittlefs/mklittlefs
esptool.py := esptool.py
upload.py := $(arduino_repo)/tool/upload.py

# For upload and creation of file system image see: 
# https://raw.githubusercontent.com/earlephilhower/arduino-esp8266littlefs-plugin/master/src/ESP8266LittleFS.java

upload-fs:
	echo "Stub"
	exit
	$(upload.py) --chip esp8266 --port TODOserialPort --baud TODOuploadSpeed write_flash TODOuploadAddress ./image

create-fs:
	# Should check if the contents of data has been updated
	echo "Stub"
	exit
	$(mklittlefs) -c ./data -p TODOspiPage -b TODOspiBlock -s (TODOspiEnd - TODOspistart) ./image

compile: ./build
	arduino-cli compile --fqbn esp8266:esp8266:d1 esp -v  --output-dir build

build:
	mkdir -p build

install-libraries:
	$(arduino_cmd) --install-library PubSubClient PubSubClient:2.8.0

refresh-all-certs: refresh-mozilla-certs add-our-cert

refresh-mozilla-certs:
	mkdir -p data
	tools/certs-from-mozilla.py

add-our-cert: self-signed-cert/ourcert.der
	ar q ./data/certs.ar ./self-signed-cert/ourcert.der

self-signed-cert/ourcert.der: ./self-signed-cert/key.pem
	openssl x509 -outform der -in ./self-signed-cert/cert.pem -out ./self-signed-cert/ourcert.der

