
OS_LIB=""
u_name=`uname -s 2>/dev/null`
if [ "$u_name" = "AIX" ]     ## IBM����ʹ��
then
    CFLAG=" -q64 -O2 -brtl "
    CPP="xlC -q64 -O2 -brtl "

elif [ "$u_name" = "HP-UX" ]    ## HP����ʹ��
then 
    CFLAG=" +DD64 -O2 "
    CPP="aCC $CFLAG "

elif [ "$u_name" = "SunOS" ]   ## SUN����ʹ��
then 
    CFLAG=" -xarch=native64 -lsocket -D _SUN_VERSION_ -D_RWSTD_MULTI_THREAD "
    CPP="gcc -m64 "
    OS_LIB=" -lstdc++ "

elif [ "$u_name" = "Linux" ]   ## LINUX����ʹ��
then 
    CFLAG=" -m64 "
    CPP="cc $CFLAG "

else
    echo "invalid os param"
    exit 1
fi

export CFLAG
export CPP
export OS_LIB
