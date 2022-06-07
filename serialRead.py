import csv
import time
import paho.mqtt.client as mqtt
import json
import serial

def on_connect(client, userdata, flags, connectionOkFlag):
    if connectionOkFlag == 0:
        client.connectionOk = True
        print("Connection succesful")
    else:
        print("No connection, return code ", connectionOkFlag)
        client.loop_stop()

def on_message( client, userdata, message):
    print(message.topic + " " + str(message.payload))


client = mqtt.Client('python3')
client.on_connect = on_connect
client.on_message = on_message

mqtt.Client.connectionIsOk = False
mqtt.Client.suppression = False

