#!/bin/sh
echo "Running Installer"

CTRL_FILE=/usr/share/BeidTestCtrl.xml  # Don't change this!
INST_PREFIX=/usr/share/eidtestinfra

# If there was a previous install, delete it first
if [ -d $INST_PREFIX ]
then
   rm -rf $INST_PREFIX
fi

kernel=$(uname -m)

# Copy all to $INST_PREFIX
cp -r install $INST_PREFIX
chmod 755 $INST_PREFIX $INST_PREFIX/32bit $INST_PREFIX/64bit ${INST_PREFIX}/lib
chmod -R 755 $INST_PREFIX/32bit $INST_PREFIX/64bit ${INST_PREFIX}/lib
chmod 755 ${INST_PREFIX}/pcsccontrol.sh ${INST_PREFIX}/libjpcsc.so
chmod 755 ${INST_PREFIX}/setproxy.sh
chmod 644 ${INST_PREFIX}/pcsccontrol.jar
ln -s  ${INST_PREFIX}/libpcsclite.so.1  ${INST_PREFIX}/libpcsclite.so
ln -s  ${INST_PREFIX}/libdl_proxy.so.1 ${INST_PREFIX}/libdl_proxy.so
chmod 755 ${INST_PREFIX}/pcsccontrol.sh ${INST_PREFIX}/libjpcsc.so
chmod 755 ${INST_PREFIX}/libdl_proxy.so ${INST_PREFIX}/libdl_proxy.so.1
chmod 755 ${INST_PREFIX}/libpcsclite.so ${INST_PREFIX}/libpcsclite.so.1

if [ ! "$kernel" = "i686" ]; then
	cp -f $INST_PREFIX/64bit/* $INST_PREFIX
fi

# If no control file exists yet, create one and adjust make sure it's world writable
if [ ! -f $CTRL_FILE ]
then
	touch $CTRL_FILE
	chmod 666 $CTRL_FILE
fi

echo
if [ "$kernel" = "i686" ]; then
	echo "Installation finished."
else
	echo "Installation finished. By default, the 64 bit proxy has been installed"
fi
echo ""
echo "See the README file for a quick start guide,"
echo "or the more detailed user guide at https://env.dev.eid.belgium.be/"
echo ""

