CC=gcc
CFLAGS=-Wall -g

OBJFILES=main.o base64.o

LIBRARYFILES=-lgdbm -lgd -lm

Main: $(OBJFILES)
$(CC) -o Main $(OBJFILES) $(LIBRARYFILES)

Clean:
rm -f Main *.o *.html *~

main.o:base64.h
base64.o:base64.h
