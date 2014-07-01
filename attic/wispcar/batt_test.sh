#!/bin/sh
# batt_test.sh
# David Rowe 22 June 2009

if [ $# -ne 2 ]
then
  echo "usage ./batt_test.sh cutoff_Vcode logfile"
  exit 0
fi

/home/stty -F /dev/tts/1 speed 4800 > /dev/null

rm -f $2

# sample initial voltage

dd if=/dev/tts/1 bs=1 count=36 1>w 2>/dev/null&
echo -n 'w' > /dev/tts/1
sleep 2
v=`cut -f 2 -d ' ' w`
echo $v

# stop when voltage reaches a threshold

while [ $v -gt $1 ]
do

  # sample voltage
  
  dd if=/dev/tts/1 bs=1 count=36 1>w 2>/dev/null&
  echo -n 'w' > /dev/tts/1
  sleep 2
  v=`cut -f 2 -d ' ' w`
  i=`cut -f 3 -d ' ' w`
  echo $v $i
  
  echo -n `date` ' ' >> $2
  echo $v $i >> $2    

  sleep 5
  
done
 
  
