#!/bin/sh
#
# Rebuild all script
#
#

CURR_DIR=`pwd`

# [1] test if ant can be found
ANT=`which ant 2> /dev/null`
if [ -z $ANT ]
then
	ANT=${ANT_HOME}/bin/ant
	if [ ! -f $ANT ]
	then
		echo "ant not found. Please set ANT_HOME"
	exit 1
	fi
fi

# [2] test if make can be found
MAK=`which make 2> /dev/null`
if [ -z $MAK ]
then
	echo "make not found."
	exit 1
fi

# [3] test if javac can be found
JAVAC=`which javac 2> /dev/null`
if [ -z $JAVAC ]
then
	JAVAC=${JAVA_HOME}/bin/javac
	if [ ! -f $JAVAC ]
	then
		echo "javac not found. Please set JAVA_HOME to java 1.6 or higher"
		exit 1
	fi
fi

###############################
#                             #
#   Build all                 #
#                             #
###############################

make clean
make

#test if files we need were created
if [ ! -f  ../pcscproxy/libpcsclite.so.1 ]
then
	echo "../pcscproxy/libpcsclite.so.1 not found. Cannot continue"
	exit 1
fi
if [ ! -f  ../pcscproxy/64bit/libpcsclite.so.1 ]
then
	echo "../pcscproxy/64bit/libpcsclite.so.1 not found. Cannot continue"
	exit 1
fi

if [ ! -f ../dlopenproxy/libdl_proxy.so.1 ]
then
	echo "../dlopenproxy/libdl_proxy.so.1 not found. Cannot continue"
	exit 1
fi
if [ ! -f ../dlopenproxy/64bit/libdl_proxy.so.1 ]
then
	echo "../dlopenproxy/64bit/libdl_proxy.so.1 not found. Cannot continue"
	cd $CURR_DIR
	exit 1
fi

cd ../pcsccontrol/

if [ -f pcsccontrol.jar ]
then
	rm pcsccontrol.jar
fi

if [ -d bin ]
then
	rm -rf bin
fi

# To build the jar, you need jdk 1.6 (or up)
#export JAVA_HOME=/usr/java/jdk1.6.0_10
#$ANT -f build.xml clean
#$ANT -f makeJarFile.xml clean
#$ANT -f build.xml 
#$ANT -f makeJarFile.xml

# Use the jar from svn, this works always...
cp saved/*.jar .

#test if file we need was created
if [ ! -f pcsccontrol.jar ]
then
	echo "pcsccontrol/pcsccontrol.jar not found. Cannot continue"
	cd $CURR_DIR
	exit 1
fi

###############################
#                             #
#   Linux installer           #   
#                             #
###############################

PACKAGE_NAME=eidtestinfra
PACKAGE=$PACKAGE_NAME.tar.gz

cd $CURR_DIR

echo ""
echo "Creating install package..."

if [ -d $PACKAGE_NAME ] ; then
	rm -rf $PACKAGE_NAME
fi

if [ -f $PACKAGE ] ; then
	rm $PACKAGE
fi

mkdir -p $PACKAGE_NAME/install/lib
mkdir -p $PACKAGE_NAME/install/32bit
mkdir -p $PACKAGE_NAME/install/64bit

cp ../install/license.txt        $PACKAGE_NAME/LICENSE
cp ../install/linux/README       $PACKAGE_NAME/
cp ../install/linux/install.sh   $PACKAGE_NAME/
cp ../install/linux/uninstall.sh $PACKAGE_NAME/

cp ../pcsccontrol/pcsccontrol.jar $PACKAGE_NAME/install/
cp ../pcsccontrol/lib/*.jar       $PACKAGE_NAME/install/lib/
cp ../pcsccontrol/libjpcsc.so     $PACKAGE_NAME/install/
cp ../pcsccontrol/pcsccontrol.sh  $PACKAGE_NAME/install/
cp ../install/linux/setproxy.sh   $PACKAGE_NAME/install/

cp ../pcscproxy/libpcsclite.so.*       $PACKAGE_NAME/install/
cp ../pcscproxy/libpcsclite.so.*       $PACKAGE_NAME/install/32bit
cp ../pcscproxy/64bit/libpcsclite.so.* $PACKAGE_NAME/install/64bit

cp ../dlopenproxy/libdl_proxy.so.*       $PACKAGE_NAME/install/
cp ../dlopenproxy/libdl_proxy.so.*       $PACKAGE_NAME/install/32bit
cp ../dlopenproxy/64bit/libdl_proxy.so.* $PACKAGE_NAME/install/64bit

tar -czf $PACKAGE_NAME.tar.gz $PACKAGE_NAME/

#test if file we need was created
if [ ! -f $PACKAGE_NAME.tar.gz ]
then
	echo "$PACKAGE_NAME.tar.gz not created. Cannot continue"
	exit 1
fi

echo ""
echo "Done. Created $PACKAGE_NAME.tar.gz"
echo ""
