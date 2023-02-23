#!/bin/bash

exec 2>&1

RUNS=5

serverlog="spot_server.log"
clientlog="spot_client.log"

> ${serverlog}
> ${clientlog}

for N in 16 32 64 128 256 512 1024 2048 4096 8192
do
  i=0
  while [ $i -lt $RUNS ]
  do
  	port=31000
	quit=0
	while [ "$quit" -ne 1 ]; do
		netstat -a | grep $port >> /dev/null
		if [ $? -gt 0 ]; then
			quit=1
		else
			port=`expr $port + 1`
		fi
	done

	echo "N:" ${N} >>${serverlog} 
	echo "N:" ${N} >>${clientlog} 

    { time ./bin/frontend.exe -r 0 -n 7 -t 1 -d ${N} >>${serverlog} ; } 2>>${serverlog} & 
    { time ./bin/frontend.exe -r 1 -n 7 -t 1 -d ${N} >>${clientlog} ; } 2>>${clientlog} 
    i=$[$i+1]
    sleep 1

  done
done

echo "Done."
