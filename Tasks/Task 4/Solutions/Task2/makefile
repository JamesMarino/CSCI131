CC=gcc
CFLAGS=-Wall -g -I/usr/unusual/include/hiredis
LIBRARYDIRECTORIES=-L/usr/unusual/lib

OBJFILES=main.o
LIBRARYFILES=-lhiredis

Manager: $(OBJFILES)
	$(CC) -o Managerqu $(OBJFILES) $(LIBRARYDIRECTORIES) $(LIBRARYFILES)

clean:
	rm -f Manager *.o *~
