TARGET32 = libpcsclite.so.1
TARGET64 = 64bit/libpcsclite.so.1

CC = g++
CFLAGS32 = -fPIC -m32
CFLAGS64 = -fPIC -m64 -I/usr/include/c++/4.5/i486-linux-gnu -DLINUX64BIT
LFLAGS = -shared
INCLUDEDIRS = -I. -Ilinux -I/usr/include/PCSC -I../EidTestinfraLib
LIBS32 = -ldl -lpthread
LIBS64 = -ldl -lpthread

HEADERS = APDURequest.h Carddata.h  FileStructure.h  SoftReader.h \
		Transaction.h  ArrayBuffer.h  Control.h  HexUtils.h SoftCard.h  \
		SoftReaderManager.h  WinscardImpl.h  linux/proxy_winscard.h \
		linux/proxy_winscard_imp.h linux/data_winscard.h \
		../EidTestinfraLib/controlFileAccess.h ../EidTestinfraLib/Mutex.h \
		../EidTestinfraLib/util.h ../EidTestinfraLib/xml.h ../EidTestinfraLib/Logger.h \
		../EidTestinfraLib/ThreadDefines.h ../EidTestinfraLib/virtualCardAccess.h

		
SOURCES = APDURequest.cpp  Control.cpp  HexUtils.cpp  SoftCard.cpp \
		SoftReaderManager.cpp  WinscardImpl.cpp  Carddata.cpp  FileStructure.cpp \
		SoftReader.cpp  Transaction.cpp  linux/proxy_winscard.cpp \
		linux/proxy_winscard_imp.cpp  linux/data_winscard.cpp \
		../pcsctool/utils.cpp ../pcsctool/pcscerr2string.cpp \
		../EidTestinfraLib/Logger.cpp ../EidTestinfraLib/controlFileAccess.cpp \
		../EidTestinfraLib/virtualCardAccess.cpp ../EidTestinfraLib/Mutex.cpp \
		../EidTestinfraLib/util.cpp ../EidTestinfraLib/xml.cpp \
# Targets

all: $(TARGET32) $(TARGET64)

$(TARGET32): $(SOURCES) $(HEADERS)
	$(CC) $(INCLUDEDIRS) $(CFLAGS32) $(SOURCES) $(LIBS32) $(LFLAGS) -o $@

$(TARGET64): $(SOURCES) $(HEADERS)
	if [ ! -d 64bit ]; then mkdir 64bit; fi
	$(CC) $(INCLUDEDIRS) $(CFLAGS64) $(SOURCES) $(LIBS64) $(LFLAGS) -o $@

clean:
	-rm *$(TARGET32) $(TARGET64)

