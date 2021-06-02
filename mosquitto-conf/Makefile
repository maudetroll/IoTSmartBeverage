# This makefile should make it easier to create all keys and certificates needed for communication between mosquitto server and client
CA="ca.crt"
CAkey="ca.key"

create_ca:
	# http://www.steves-internet-guide.com/mosquitto-tls/
	# IMPORTANT CA hostname can't be the same as any signed server or client
	# CA key with PW
	openssl genrsa -des3 -out ${CAkey} 2048
	# Generate CA certificate
	openssl req -new -x509 -days 1826 -key ${CAkey} -out ${CA}

SERVER_NAME="server"
SERVER_DIR=${SERVER_NAME}
SERVER_COMMONNAME="192.168.178.123"
MQTT_TLS_PORT="8883"


create_server_key:
	mkdir -p ${SERVER_DIR}
	# Key Private key WITHOUT PW
	openssl genrsa -out ${SERVER_DIR}/${SERVER_NAME}.key 2048
	# Signing request
	openssl req -new -out ${SERVER_DIR}/${SERVER_NAME}.csr -key ${SERVER_DIR}/${SERVER_NAME}.key\
	    -subj "/C=DE/ST=BW/L=KA/O=iot/OU=iot/CN=${SERVER_COMMONNAME}"
	# Sign the request
	openssl x509 -req -in ${SERVER_DIR}/${SERVER_NAME}.csr -CA ${CA} -CAkey ${CAkey} -CAcreateserial -out ${SERVER_DIR}/${SERVER_NAME}.crt -days 360
	# Can be verified
	# openssl verify -CAfile ${CA} ${SERVER_DIR}/${SERVER_NAME}.crt
	#
	# server.crt: OK
	# Copy to st. /etc/mosquitto/certs/{${SERVER_NAME}.crt, ${SERVER_NAME}.key}
	#             /etc/mosquitto/ca_certificates/${CAkey}
	# Generate PEM format from server.crt
	openssl x509 -in ${SERVER_DIR}/${SERVER_NAME}.crt -out ${SERVER_DIR}/${SERVER_NAME}.pem -outform PEM


CLIENT_COMMONNAME="192.168.178.118"
CLIENT_NAME="makefile-client"
CLIENT_FILEPATH=${CLIENT_NAME}/"makefile-client"
CLIENT_PASSWORD="WE DO NOT CURRENTLY USE A CLIENT PASSWORD"

create_client_key:
	mkdir -p ${CLIENT_NAME}
	
	# Create key
	openssl genrsa \
	    -out ${CLIENT_FILEPATH}.key \
	    2048  
	    #-des3 \
	    #-passout pass:${CLIENT_PASSWORD} \
	# Create request
	openssl req \
	    -out ${CLIENT_FILEPATH}.csr \
	    -key ${CLIENT_FILEPATH}.key \
	    -new  \
	    -subj "/C=DE/ST=BW/L=KA/O=iot/OU=iot/CN=${CLIENT_COMMONNAME}"
	    #-passin pass:${CLIENT_PASSWORD} \
	
	# Sign request
	openssl x509 \
	    -req \
	    -in ${CLIENT_FILEPATH}.csr \
	    -CA ${CA} \
	    -CAkey ${CAkey} \
	    -CAcreateserial \
	    -out ${CLIENT_FILEPATH}.crt \
	    -days 180 \
	
	printf "Client Vars:\nCLIENT_COMMONNAME: ${CLIENT_COMMONNAME}\nCLIENT_FILEPATH: ${CLIENT_FILEPATH}\CLIENT_PASSWORD: ${CLIENT_PASSWORD}\n" > client_vars.txt
	# Saved client vars in ${CLIENT_NAME}

example_mosquitto_conf:
	mkdir -p ./config
	printf "# Paths could differ based on Linux distribution\nlistener ${MQTT_TLS_PORT}\nallow_anonymous true\ncafile /mosquitto/config/ca_certificates/${CA}\ncertfile /mosquitto/config/certs/${SERVER_NAME}.crt\nkeyfile /mosquitto/config/certs/${SERVER_NAME}.key\nrequire_certificate true\n" > ./config/mosquitto.conf
	mkdir -p ./config/ca_certificates
	cp ${CA} ./config/ca_certificates/${CA}
	mkdir -p ./config/certs
	cp ${SERVER_NAME}/${SERVER_NAME}.key ./config/certs/${SERVER_NAME}.key
	cp ${SERVER_NAME}/${SERVER_NAME}.crt ./config/certs/${SERVER_NAME}.crt
	mkdir -p log

remove_mosquitto_config:
	rm -rf config

CONTAINER_TOOL="podman"

build_container:
	${CONTAINER_TOOL} image pull eclipse-mosquitto:latest

create_container:
	${CONTAINER_TOOL} create --name mqtt -p ${SERVER_COMMONNAME}:${MQTT_TLS_PORT}:${MQTT_TLS_PORT} -v ./config:/mosquitto/config  -v ./log:/mosquitto/log eclipse-mosquitto

start_container:
	${CONTAINER_TOOL} start mqtt

run_container:
	${CONTAINER_TOOL} run -d --name mqtt -p ${SERVER_COMMONNAME}:${MQTT_TLS_PORT}:${MQTT_TLS_PORT} -v ./config:/mosquitto/config  -v ./log:/mosquitto/log eclipse-mosquitto

rm_container:
	${CONTAINER_TOOL} container rm mqtt
