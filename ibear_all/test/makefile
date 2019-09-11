
include ./makefile.cfg

OBJ = test.o

BIN=test

all:${BIN} install clean

.cpp.o:
	echo "Compiling the $<"
	$(CC) -c ${CFLAGS} ${INCLUDE} $<
.c.o:
	echo "Compiling the $<"
	$(CC) -c ${CFLAGS} ${INCLUDE} $<

clean:
	rm -f test.o

######################################################################

${BIN}: $(OBJ)
	$(CC) ${CFLAGS} -o $@ $? ${LIB}

######################################################################

install:
	
	#mv ${BIN} ${ACCT_BIN}


