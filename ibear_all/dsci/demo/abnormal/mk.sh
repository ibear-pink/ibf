
. ../make_os.sh

INCLUDE=" -I${DSCI_PATH}/include "
LIB=" -lpthread -L./ -L${DSCI_PATH}/lib/  -ldsci "

rm -f mytest
cc -c demo.c  ${CFLAG} ${INCLUDE}
cc -o mytest demo.o ${CFLAG} ${LIB} 

rm *.o
