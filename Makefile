

CC = g++
CFLAGS = -O3 -Wall -std=c++0x
LIBPATH =
LDFLAGS =

OFILES = main.o rs232.o

all: main

main: $(OFILES)
	$(CC) $(LIBPATH) -o $@ $(OFILES) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

%.o: %.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

clean:
	rm -f *.o
