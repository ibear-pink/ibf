
. ../make_os.sh

INCLUDE=" -I${DSCI_PATH}/include "
LIB=" -lpthread -L./ -L${DSCI_PATH}/lib/  -ldscpp "

rm -f mytest
$CPP -c demo.cpp ${INCLUDE}
$CPP -o mytest demo.o ${LIB} $OS_LIB

rm *.o

