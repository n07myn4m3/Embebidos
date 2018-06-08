#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""

"""
import serial
import math
import time
import subprocess

#EXTRAER INFORMACION POR EL PUERTO SERIAL
f = open("data.txt",'w')
stampData = serial.Serial('/dev/ttySP0',115200)

#VARIABLES ASOCIADAS AL GIROSCOPIO
Acc       = [0.0,0.0]
Gy        = [0.0,0.0]
Angle     = [0.0,0.0]
A_R       = 16384.0
G_R       = 131.0
RAD_A_DEG = 57.295779

#VARIABLES ASOCIADAS AL BROKER
broker_adress = '192.168.20.2'

topic1 = 'theta'
topic2 = 'phi'
topic3 = 'force'

current_time = 0.0;
last_time    = 0.0;
dt           = 0.0;

def main():

    while True:
		current_time=time.time();
		dt=(current_time-last_time)/1000;
		#----------------------------------------------------------------------------    	
        while(stampData.inWaiting()==0): #Si no existe informacion no haga nada
            pass #Hacer nada
           # print ('kk')

        stampString = stampData.readline()
        pronArray = stampString.split(',')
        
		#if(pronArray[0]=='inicio'):
		#listo=1

        if (len(pronArray) == 6):
               
			AcX=float(pronArray[0])
			AcY=float(pronArray[1])
			AcZ=float(pronArray[2])
			GyX=float(pronArray[3])
			GyY=float(pronArray[4])
			ADC=float(pronArray[5])

            try:
				Acc[0]=math.atan(AcY/(math.sqrt(math.pow(AcX,2) + math.pow(AcZ,2))))*RAD_A_DEG;
				Acc[1]=math.atan(-AcX/(math.sqrt(math.pow(AcY,2) + math.pow(AcZ,2))))*RAD_A_DEG;
			except ZeroDivisionError
				print "division by zero"

			Gy[0] = GyX/G_R;
			Gy[1] = GyY/G_R;
			Angle[0] = 0.98 *(Angle[0]+Gy[0]*dt)+ 0.02*Acc[0];
			Angle[1] = 0.98 *(Angle[1]+Gy[1]*dt)+ 0.02*Acc[1];

			print Angle,ADC
			#time.sleep(0.01)

			cmd1='mosquitto_pub -h ' + broker_adress + ' -t '+ topic1 +' -m ' + str(Angle[0])
			cmd2='mosquitto_pub -h ' + broker_adress + ' -t '+ topic2 +' -m ' + str(Angle[1])
			cmd3='mosquitto_pub -h ' + broker_adress + ' -t '+ topic3 +' -m ' + str(ADC)
			print subprocess.check_output(cmd1, shell=True)
			print subprocess.check_output(cmd2, shell=True)
			print subprocess.check_output(cmd3, shell=True)
		#----------------------------------------------------------------------------------------
		last_time = current_time

       # print stampString
       # f.write(stampString)
try:
    main()
finally:
    stampData.close()
    f.write("the end")
    f.close()



