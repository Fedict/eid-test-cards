#!/bin/bash

#!/bin/bash

count=99
if [ $1 -eq 32 ]
then
  cp -f /usr/share/eidtestinfra/32bit/* /usr/share/eidtestinfra/
elif [ $1 -eq 64 ]
then
    cp -f /usr/share/eidtestinfra/64bit/* /usr/share/eidtestinfra/
else
	echo "Usage:"
	echo "    setproxy.sh 32 : use the 32 bit version of the PCSC proxy"
	echo "  or:"
	echo "    setproxy.sh 64 : use the 64 bit version of the PCSC proxy"
	echo "The 32 bit version is needed when using a 32 bit application,"
	echo "and vice versa"
fi

