#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""

"""
import serial

f = open("data.txt",'w')
stampData = serial.Serial('/dev/ttySP0',115200)


def main():
    while True:
        while(stampData.inWaiting()==0): #Si no existe informacion no haga nada
            pass #Hacer nada
        stampString = stampData.readline() 
        print stampString 
        f.write(stampString)          
try:
    main()
finally:
    stampData.close()
    f.write("the end")
    f.close()
