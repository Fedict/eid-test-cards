TARGET32 = EidInfraTest
TARGET64 = EidInfraTest64

CC=g++
CFLAGS32 = -O3 -Wall -fmessage-length=0 -MMD -MP -I. -m32
CFLAGS64 = -O3 -Wall -fmessage-length=0 -MMD -MP -I. -m64 -I/usr/include/c++/4.5/i486-linux-gnu
LIBS32 = -ldl -lpthread ../../ThirdParty/unittest-cpp-1.3/UnitTest++/libUnitTest++.a
LIBS64 = -ldl -lpthread ../../ThirdParty/unittest-cpp-1.3/UnitTest++/libUnitTest_64++.a
INC = -I../EidTestinfraLib -I../pcscproxy -I../pcscproxy/linux -I../pcsctool \
	-I. -I../../ThirdParty/unittest-cpp-1.3/UnitTest++/src/ -I/usr/include/PCSC

HEADERS = CommandTestHeader.h utils2.h
SRC = CommandTest.cpp ControlFileTest.cpp EidInfraTest.cpp SoftCardTest.cpp utils2.cpp XMLTest.cpp \
	commandtestfuctions.cpp ControlTest.cpp FileStructureTest.cpp SoftReaderManagerTest.cpp \
	VirtualCardTest.cpp \
	../pcsctool/utils.cpp ../pcsctool/pcscerr2string.cpp \
	../EidTestinfraLib/Logger.cpp ../EidTestinfraLib/controlFileAccess.cpp \
	../EidTestinfraLib/virtualCardAccess.cpp ../EidTestinfraLib/Mutex.cpp \
	../EidTestinfraLib/util.cpp ../EidTestinfraLib/xml.cpp \
	../pcscproxy/APDURequest.cpp  ../pcscproxy/Control.cpp ../pcscproxy/HexUtils.cpp \
	../pcscproxy/SoftCard.cpp ../pcscproxy/SoftReaderManager.cpp ../pcscproxy/WinscardImpl.cpp \
	../pcscproxy/Carddata.cpp ../pcscproxy/FileStructure.cpp ../pcscproxy/SoftReader.cpp \
	../pcscproxy/Transaction.cpp ../pcscproxy/linux/proxy_winscard.cpp \
	../pcscproxy/linux/proxy_winscard_imp.cpp ../pcscproxy/linux/data_winscard.cpp

# Targets

all: $(TARGET32) $(TARGET64)

$(TARGET32): $(SRC) $(HEADERS) $(LIBS32)
	$(CC) $(CFLAGS_) $(INC) $(SRC) $(LIBS32) -o $@

$(TARGET64): $(SRC) $(HEADERS) $(LIBS64)
	$(CC) $(CFLAGS64) $(INC) $(SRC) $(LIBS64) -o $@

clean:
	-rm $(TARGET32) $(TARGET64)

