# app

CC = g++
CFLAGS = -Wall -g -std=c++11
TARGET = test_server
OBJS = utils.o rio.o net.o md5.o memcachedlib.o crc32.o $(TARGET).o

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(TARGET)
