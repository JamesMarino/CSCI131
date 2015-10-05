CC=gcc
CFLAGS=-Wall -g

OBJFILES=main.o base64.o

LIBRARYFILES=-lgdbm -lgd -lm

Project: $(OBJFILES)
	$(CC) -o Project $(OBJFILES) $(LIBRARYFILES)

clean:
	rm -f Project *.o *.html *~

main.o:base64.h ProjectGroup.h
base64.o:base64.h
