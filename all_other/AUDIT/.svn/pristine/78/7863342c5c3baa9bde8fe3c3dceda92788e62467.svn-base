
#ifndef _DSCI_CPP_H_
#define _DSCI_CPP_H_

/*查询结果集合*/
class DSCIResultSet
{
public:
    //获取下一条记录，true:有记录；false:无记录
    virtual bool next() = 0;

    //取出指定字段的值，字段序号从0开始，0,1,2……
    virtual char* getString(int field) = 0;
    virtual int getInt(int field) = 0;
    virtual long getLong(int field) = 0;
    virtual double getDouble(int field) = 0;
    virtual int getBlobSize(int field) = 0;           //返回lob字段的字节数
    virtual int getBlob(int field, void *value) = 0;  //返回lob字段的字节数，字段内容存放在 *value中

    //获取字段个数
    virtual int getFieldCount() = 0;

    //获取总记录条数，仅在缓冲模式下有意义
    virtual long getRecCount() = 0;
};

/*直接执行SQL*/
class DSCIStatement
{
public:
    //直接执行查询操作，如果指定dbNo，则不读配置，直接到指定库上执行，如果出错，返回NULL
    //dbNo从1开始，1对应DB01,2对应DB02，依次类推
    virtual DSCIResultSet* executeQuery(const char* sql) = 0;
    virtual DSCIResultSet* executeQuery(const char* sql, int dbNo) = 0;

    //直接执行增删改操作，如果指定dbNo，则不读配置，直接到指定库上执行
    //成功，返回操作影响到的记录数(>=0)，失败，返回-1;
    virtual long executeUpdate(const char* sql) = 0;
    virtual long executeUpdate(const char* sql, int dbNo) = 0;

    //销毁
    virtual void release() = 0;
};

/*预解析SQL*/
class DSCIPreparedStatement
{
public:
    //绑定变量，paramid为参数序号，从0开始算，如0,1,2......
    //成功:返回0，失败:返回-1
    virtual int bind(int paramid, char *value) = 0;
    virtual int bind(int paramid, int *value) = 0;
    virtual int bind(int paramid, long *value) = 0;
    virtual int bind(int paramid, float *value) = 0;
    virtual int bind(int paramid, double *value) = 0;
    virtual int bind(int paramid, void *value, int size) = 0; //对于LOB字段的绑定，需要指定字节长度。

    //直接执行查询操作，如果失败，返回NULL
    virtual DSCIResultSet* executeQuery() = 0;

    //直接执行增删改操作
    //成功，返回操作影响到的记录数(>=0)，失败，返回-1;
    virtual long executeUpdate() = 0;

    //销毁
    virtual void release() = 0;
};

/*数据库连接*/
class DSCIConnection
{
public:
    //创建 DSCIStatement，用于直接执行SQL语句，如果失败，返回NULL
    virtual DSCIStatement* createStatement() = 0;

    //创建 DSCIPreparedStatement，用于预解析指定的SQL语句，如果失败，返回NULL
    //如果指定dbNo，则不读配置，直接到指定库上解析SQL
    virtual DSCIPreparedStatement* prepareStatement(const char* sql) = 0;
    virtual DSCIPreparedStatement* prepareStatement(const char* sql, int dbNo) = 0;

    //设置是否自动提交 1 是，0 否
    //成功:返回0，失败:返回-1
    virtual int setAutocommit(int flag) = 0;

    //设置是否采用select缓冲模式 1 是，0 否
    virtual void setBufferflag(int flag) = 0;

    //设置写日志模式 on 写日志，off 不写日志，sync 实时写磁盘
    virtual void setLogmode(const char *inMode) = 0;

    //获取错误码
    virtual int getErrCode() = 0;

    //获取错误信息
    virtual char* getErrMsg() = 0;

    //提交
    virtual int commit() = 0;

    //回滚
    virtual int rollback() = 0;

    //销毁
    virtual void release() = 0;
};


/*创建连接工厂*/
extern DSCIConnection* DSCI_MakeConnection(char *errStr);

extern DSCIConnection* DSCI_MakeConnection(char *errStr, char *dbStr);

#endif
