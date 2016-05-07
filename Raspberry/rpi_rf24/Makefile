CC=arm-linux-gnueabihf-g++
CFLAGS=-march=armv6zk -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -Ofast -Wall -pthread
INCLUDE=-I/usr/local/include/RF24/.. -I.. -L/usr/local/lib
LIBS=-lrf24

all: rpi_rf24

rpi_rf24: rpi_rf24.o
	$(CC) $(CFLAGS) $(INCLUDE) -o rpi_rf24.out rpi_rf24.o $(LIBS)
clean:
	rm *.o
