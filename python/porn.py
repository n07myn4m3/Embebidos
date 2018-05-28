import paho.mqtt.client as mqtt
broker_adress="192.168.20.5"
client=mqtt.Client("P1")
client.connect(broker_address)
client.publish("ke","ON")
