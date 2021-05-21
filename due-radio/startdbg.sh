#!/bin/sh
# Brane Ždralo (19.1.2021)

OPENOCD="/usr/local/bin/openocd"

PO=$(pidof openocd)

if [ "${PO}" != "" ]
then
  echo "Killing openocd ..."
  kill -s HUP "${PO}"
  sleep 0.1
fi

echo "Starting openocd ..."
${OPENOCD} -f openocd.cfg -c "set _flash_file ${1}" &
sleep 1
