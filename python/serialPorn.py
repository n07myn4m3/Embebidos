import serial
import subprocess

f = open("data.txt",'w')
arduinoData = serial.Serial('/dev/ttyACM0',115200)

broker_adress = 'localhost'

topic1 = 'ax'
topic2 = 'ay'
topic3 = 'ke'


def main():
    while True:
        while(arduinoData.inWaiting()==0): #Si no existe informacion no haga nada
            pass #Hacer nada
        arduinoString = arduinoData.readline() 
        #print arduinoString #Para verificar la existencia de informacion 
        dataArray = arduinoString.split(',')
        if(len(dataArray) == 3):
			#print arduinoString
		    #print dataArray[0]
			f.write(arduinoString)
			cmd1='mosquitto_pub -h ' + broker_adress + ' -t '+ topic1 +' -m ' + dataArray[0]
			cmd2='mosquitto_pub -h ' + broker_adress + ' -t '+ topic2 +' -m ' + dataArray[1]
			cmd3='mosquitto_pub -h ' + broker_adress + ' -t '+ topic3 +' -m ' + dataArray[2]
			print subprocess.check_output(cmd1, shell=True)
			print subprocess.check_output(cmd2, shell=True)
			print subprocess.check_output(cmd3, shell=True)
			#print subprocess.check_output(cmd, shell=True)
		    #print subprocess.check_output(cmd2, shell=True)
        
try:
    main()
finally:
    arduinoData.close()
    f.write("the end")
    f.close()

