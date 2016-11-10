#!/bin/bash
if [ $# -eq 2]
then
pscp -scp -l pi -pw raspberrz $1 pi@192.168.0.10$2:/home/pi/agent/network/
#pscp -scp -pw raspberrz  *.hpp pi@192.168.0.103:/home/pi/agent
#pscp -scp -pw raspberrz  makeserial.sh pi@192.168.0.103:/home/pi/agent
#pscp -scp -pw raspberrz  makemainserial2.sh pi@192.168.0.103:/home/pi/agent
echo "sent to pi@192.168.0.10$2:/home/pi/agent/network/$1"
else
echo "need 2 argument"
echo "example:"
echo "$0 file_name agent_num"
echo "cheers"
fi
