# Code to connect to the ThingsBoard server
# Reads the data from the serial port, connects
# to the server, sends the data in JSON format
# and prints to the topic selected which can be 
# seen in the website.

import serial
import time
import paho.mqtt.client as mqtt
import json

def on_connect(client, userdata, flags, return_code):
    if return_code==0:
        client.connectionOkFlag=True 
        print("Connection succesful") 
    else:
        print("Incorrect connection = ",return_code) 
        client.loop_stop()  

 
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client('python3')
client.on_connect = on_connect
client.on_message = on_message

mqtt.Client.connectionOkFlag = False 
mqtt.Client.incorrectConnectionReturnError = False

PuertoSerial = serial.Serial(port = '/tmp/ttyS1')


# Token is from ThingsBoard.
token = "VrUpeXsOmbW8qYeV69ED"
topic = "v1/devices/me/telemetry"
requestTopic = "v1/devices/me/attributes/request/1"

client.username_pw_set(token, password=None)

# Connect to the website 
client.connect("iot.eie.ucr.ac.cr", 1883, 60)
while not(client.connectionOkFlag):
	client.loop()
	time.sleep(1)
time.sleep(3)


dataToSend = dict()
for i in range(10):
	valueToSend = PuertoSerial.readline().decode().split(',')
	
	dataToSend["Battery (%)"]=valueToSend[0]
	dataToSend["Temp (C)"]=valueToSend[1]
	dataToSend["Humidity (%)"]=valueToSend[2]
	dataToSend["Wind (m/s)"]=valueToSend[3]
	dataToSend["Rain (mm)"]=valueToSend[4]
	dataToSend["Light (lux)"]=valueToSend[5]
	data_out = json.dumps(dataToSend)
	
	print(valueToSend)

	client.publish(topic, data_out, 0)
	time.sleep(5)
	client.loop()
client.disconnect()

