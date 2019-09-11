
include ./makefile.cfg

OBJ = demo.o

BIN=demo

all:${BIN} install clean

.cpp.o:
	echo "Compiling the $<"
	$(CC) -c ${CFLAGS} ${INCLUDE} $<
.c.o:
	echo "Compiling the $<"
	$(CC) -c ${CFLAGS} ${INCLUDE} $<

clean:
	rm -f demo.o

######################################################################

${BIN}: $(OBJ)
	$(CC) ${CFLAGS} -o $@ $? ${LIB}

######################################################################

install:
	
	#mv ${BIN} ${ACCT_BIN}


