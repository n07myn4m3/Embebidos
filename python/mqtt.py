import subprocess
cmd = 'mosquitto_pub -h 192.168.20.1 -t "ke" -m "ella no te ama"'
print subprocess.check_output(cmd, shell=True)

#ifconfig wlan0 192.168.20.1 down

#/etc/init.d
