#!/bin/bash

exec 2>&1

N=${1}
IP=${2}
RUNS=${3}

log="spot_server.log"

cd '/psi/SpOT-PSI'

echo "============================="
echo "N: ${N}"

i=0
while [ $i -lt $RUNS ]
do
    port=$(expr 20000 + ${N} '*' 10 + ${i})
    TIMEOUT=$(expr 3 '*' ${N})
    echo -n "${N}, " >> ${log}
    { timeout ${TIMEOUT} /usr/bin/time -f '%e, %U, %S' ./bin/frontend.exe -r 0 -n 7 -t 1 -d ${N} -ip "${IP}:${port}" ; } 2>>${log}
    i=$[$i+1]
    sleep 1
done
