#/bin/bash
if [ $(pidof gdb-multiarch) ];
then
  kill -KILL $(pidof gdb-multiarch)
fi

if [ $(pidof openocd) ];
then
  kill -HUP $(pidof openocd)
fi

if [ $(pidof gdb-multiarch) ];
then
  echo "Unsuccesfull - GDB still running!!!"
fi

if [ $(pidof openocd) ];
then
  echo "Unsuccesfull - OpenOCD still running!!!"
fi
