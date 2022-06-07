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

