# Linux makefile, run with:  make
# May have to be adapted to your needs, or in order to get it built.

TARGET=libpcsclite.so.1

INCLUDEDIRS=-I. -I/usr/include/PCSC

HEADERS=proxy_winscard.h proxy_winscard_imp.h data_winscard.h

SOURCES=proxy_winscard.cpp proxy_winscard_imp.cpp data_winscard.cpp

LIBS=-ldl

CFLAGS=-fPIC

LDFLAGS=-shared

CC=g++

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)

	$(CC) $(INCLUDEDIRS) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS) $(LDFLAGS)

clean:
	rm $(TARGET)
