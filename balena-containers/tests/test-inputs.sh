SERVER_ADDRESS=192.168.178.128
SERVER_ADDRESS_MQTT=192.168.178.128
SERVER_PORT=8080
SERVER_PORT_MQTT=1883

# Create two new bestellunge: Happens when the waiter creates orders based on
# the customers requests
RET_1=$(curl --header "Content-Type: application/json" \
  --request POST \
  --data '{ "platz_id": 1, "getraenk_id": 1, "kunde_id": 1 } ' \
  http://${SERVER_ADDRESS}:${SERVER_PORT}/api/createBestellung | jq)

echo $RET_1
RET_1_ID=$(echo $RET_1 | jq .id)
echo $RET_1_ID

RET_2=$(curl --header "Content-Type: application/json" \
  --request POST \
  --data '{ "platz_id": 2, "getraenk_id": 2, "kunde_id": 2 } ' \
  http://${SERVER_ADDRESS}:${SERVER_PORT}/api/createBestellung)

echo $RET_2
RET_2_ID=$(echo $RET_2 | jq .id)
echo $RET_2_ID

# Set Bestellungen to ready: Happens when they are processed by the bartender
GLAS_1_RFID="584190074081"
node -e "const WebSocket = require(\"ws\"); let ws = new WebSocket(\"ws://${SERVER_ADDRESS}:${SERVER_PORT}/web-socket\");" &
sleep 2
kill %1

mosquitto_pub -h ${SERVER_ADDRESS_MQTT} -p ${SERVER_PORT_MQTT} -t rfidTags -m ${GLAS_1_RFID}

# Create web socket connection
curl --header "Content-Type: application/json" \
  --request POST \
  --data "{ \"id\": ${RET_1_ID} }" \
  http://${SERVER_ADDRESS}:${SERVER_PORT}/api/setBestellungsStatusVorbereitet

GLAS_2_RFID="584185682106"
node -e "const WebSocket = require(\"ws\"); let ws = new WebSocket(\"ws://${SERVER_ADDRESS}:${SERVER_PORT}/web-socket\");" &
sleep 2
kill %1

mosquitto_pub -h ${SERVER_ADDRESS_MQTT} -p ${SERVER_PORT_MQTT} -t rfidTags -m ${GLAS_2_RFID}

# Create web socket connection
curl --header "Content-Type: application/json" \
  --request POST \
  --data "{ \"id\": ${RET_2_ID} }" \
  http://${SERVER_ADDRESS}:${SERVER_PORT}/api/setBestellungsStatusVorbereitet

# Now we need to send the write mqtt messages. Here we need the rfid tag and the mac of the right place, so we have to hardcode them from the database
mosquitto_pub -h ${SERVER_ADDRESS_MQTT} -p ${SERVER_PORT_MQTT} -t rfidTags -m ${GLAS_2_RFID}

PLACE_1_MAC="D8:F1:5B:15:02:2D"
mosquitto_pub -h ${SERVER_ADDRESS_MQTT} \
    -p ${SERVER_PORT_MQTT} \
    -t massBeverage \
    -m "${PLACE_1_MAC} , ${GLAS_1_RFID} , 400"

PLACE_2_MAC="D8:F1:5B:15:01:A7"
mosquitto_pub -h ${SERVER_ADDRESS_MQTT} \
    -p ${SERVER_PORT_MQTT} \
    -t massBeverage \
    -m "${PLACE_2_MAC} , ${GLAS_2_RFID} , 400"
