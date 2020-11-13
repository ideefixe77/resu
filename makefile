CC = gcc
LIBS = 
CFLAGS = -w -O2
SRC = $(wildcard *.c)

resu: $(SRC)
	$(CC) -s -o $@ $^ $(CFLAGS) $(LIBS)
