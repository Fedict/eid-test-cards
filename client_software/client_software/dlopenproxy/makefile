TARGET32=libdl_proxy.so.1
TARGET64=64bit/libdl_proxy.so.1
TEST32=testdynamic
TEST64=testdynamic64

CC=g++
CFLAGS32=-fPIC -m32
CFLAGS64=-fPIC -m64 -I/usr/include/c++/4.5/i486-linux-gnu -DLINUX64BIT
LFLAGS=-shared

SOURCES=dl_proxy.c

all: $(TARGET32) $(TARGET64) $(TEST32) $(TEST64)

$(TARGET32): $(SOURCES)
	$(CC) $(CFLAGS32) $(SOURCES) $(LIBS_32) $(LFLAGS) -o $@

$(TARGET64): $(SOURCES)
	if [ ! -d 64bit ]; then mkdir 64bit; fi
	$(CC) $(CFLAGS64) $(SOURCES) $(LIBS_64) $(LFLAGS) -o $@

$(TEST32): pcsc.c
	gcc -I/usr/include/PCSC -m32 pcsc.c -ldl -o $@

$(TEST64): pcsc.c
	gcc -I/usr/include/PCSC -I/usr/include/c++/4.5/i486-linux-gnu -m64 pcsc.c -ldl -o $@

clean:
	-rm $(TARGET32) $(TARGET64) $(TEST32) $(TEST64)

