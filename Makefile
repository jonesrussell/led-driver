CFLAGS=-g -O2 -pipe
CXXFLAGS=$(CFLAGS)
INCLUDE=-I ./include/ -I hidapi/ -I /usr/include/libusb-1.0/
LIBS=-l rt -l pthread -l usb-1.0

all: led-driver

led-driver:: $(shell find . -name '*.cpp' -o -iname '*.h') hid-libusb.o
	g++ -c $(CXXFLAGS) $(INCLUDE) ld-abstract.cpp -o ld-abstract.o
	g++ -c $(CXXFLAGS) $(INCLUDE) ld-lightpack.cpp -o ld-lightpack.o
	g++ -c $(CXXFLAGS) $(INCLUDE) main.cpp -o main.o
	g++ hid-libusb.o ld-abstract.o ld-lightpack.o main.o $(LIBS) -o led-driver

hid-libusb.o: ./hidapi/hidapi.h ./hidapi/linux/hid-libusb.c
	gcc -c $(CFLAGS) $(INCLUDE) ./hidapi/linux/hid-libusb.c -o hid-libusb.o

up: led-driver
	@cp led-driver /tmp
	@strip /tmp/led-driver
	scp /tmp/led-driver root@gw.lan:/usr/bin/led-driver

clean:
	rm -f *.o led-driver
