SOURCES = main.cpp pcscerr2string.cpp utils.cpp

TARGET = pcsctool

CPPFLAGS = -Wall -O3
INCLUDE = -I. -I/usr/include/PCSC

all: $(TARGET)

$(TARGET): $(SOURCES)
	g++ $(INCLUDE) $(CPPFLAGS) $(SOURCES) -lpcsclite -o $@

clean:
	- rm $(TARGET)
