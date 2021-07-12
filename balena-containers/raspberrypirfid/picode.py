import paho.mqtt.client as paho
import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
import time
import os

#define the reader
reader= SimpleMFRC522()

#define MQTT

try:
    broker = os.environ["MQTT_SA"]
except:
    broker = "192.168.178.128"


try:
    port = int(os.environ["MQTT_SP"])
except:
    port = 1883

def on_publish(client,userdata, result):
    print("Sent data via MQTT")
    pass
    
while (True):
    print("Wieder in While Schleife")
    print("Wieder in While Schleife")
    try:
        print(os.environ["MQTT_SA"])
        print(os.environ["MQTT_SP"])
    except:
        print("Failed to read env variables")
    try:
        id,text= reader.read()
        print("Das ist die ID des RFID: ", id)
        print("Das ist der Text des RFID: ", text)

    finally:
        GPIO.cleanup()

    client1= paho.Client()
    
    
    #client1.on_publish= on_publish

    # Connect to MQTT

    client1.connect(broker, port)

    # Publish the message 
    ret= client1.publish("rfidTags", id)
    print("Versendet")
    
    time.sleep(5)


