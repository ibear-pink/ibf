
#ifndef _DSCI_CPP_H_
#define _DSCI_CPP_H_

/*��ѯ�������*/
class DSCIResultSet
{
public:
    //��ȡ��һ����¼��true:�м�¼��false:�޼�¼
    virtual bool next() = 0;

    //ȡ��ָ���ֶε�ֵ���ֶ���Ŵ�0��ʼ��0,1,2����
    virtual char* getString(int field) = 0;
    virtual int getInt(int field) = 0;
    virtual long getLong(int field) = 0;
    virtual double getDouble(int field) = 0;
    virtual int getBlobSize(int field) = 0;           //����lob�ֶε��ֽ���
    virtual int getBlob(int field, void *value) = 0;  //����lob�ֶε��ֽ������ֶ����ݴ���� *value��

    //��ȡ�ֶθ���
    virtual int getFieldCount() = 0;

    //��ȡ�ܼ�¼���������ڻ���ģʽ��������
    virtual long getRecCount() = 0;
};

/*ֱ��ִ��SQL*/
class DSCIStatement
{
public:
    //ֱ��ִ�в�ѯ���������ָ��dbNo���򲻶����ã�ֱ�ӵ�ָ������ִ�У������������NULL
    //dbNo��1��ʼ��1��ӦDB01,2��ӦDB02����������
    virtual DSCIResultSet* executeQuery(const char* sql) = 0;
    virtual DSCIResultSet* executeQuery(const char* sql, int dbNo) = 0;

    //ֱ��ִ����ɾ�Ĳ��������ָ��dbNo���򲻶����ã�ֱ�ӵ�ָ������ִ��
    //�ɹ������ز���Ӱ�쵽�ļ�¼��(>=0)��ʧ�ܣ�����-1;
    virtual long executeUpdate(const char* sql) = 0;
    virtual long executeUpdate(const char* sql, int dbNo) = 0;

    //����
    virtual void release() = 0;
};

/*Ԥ����SQL*/
class DSCIPreparedStatement
{
public:
    //�󶨱�����paramidΪ������ţ���0��ʼ�㣬��0,1,2......
    //�ɹ�:����0��ʧ��:����-1
    virtual int bind(int paramid, char *value) = 0;
    virtual int bind(int paramid, int *value) = 0;
    virtual int bind(int paramid, long *value) = 0;
    virtual int bind(int paramid, float *value) = 0;
    virtual int bind(int paramid, double *value) = 0;
    virtual int bind(int paramid, void *value, int size) = 0; //����LOB�ֶεİ󶨣���Ҫָ���ֽڳ��ȡ�

    //ֱ��ִ�в�ѯ���������ʧ�ܣ�����NULL
    virtual DSCIResultSet* executeQuery() = 0;

    //ֱ��ִ����ɾ�Ĳ���
    //�ɹ������ز���Ӱ�쵽�ļ�¼��(>=0)��ʧ�ܣ�����-1;
    virtual long executeUpdate() = 0;

    //����
    virtual void release() = 0;
};

/*���ݿ�����*/
class DSCIConnection
{
public:
    //���� DSCIStatement������ֱ��ִ��SQL��䣬���ʧ�ܣ�����NULL
    virtual DSCIStatement* createStatement() = 0;

    //���� DSCIPreparedStatement������Ԥ����ָ����SQL��䣬���ʧ�ܣ�����NULL
    //���ָ��dbNo���򲻶����ã�ֱ�ӵ�ָ�����Ͻ���SQL
    virtual DSCIPreparedStatement* prepareStatement(const char* sql) = 0;
    virtual DSCIPreparedStatement* prepareStatement(const char* sql, int dbNo) = 0;

    //�����Ƿ��Զ��ύ 1 �ǣ�0 ��
    //�ɹ�:����0��ʧ��:����-1
    virtual int setAutocommit(int flag) = 0;

    //�����Ƿ����select����ģʽ 1 �ǣ�0 ��
    virtual void setBufferflag(int flag) = 0;

    //����д��־ģʽ on д��־��off ��д��־��sync ʵʱд����
    virtual void setLogmode(const char *inMode) = 0;

    //��ȡ������
    virtual int getErrCode() = 0;

    //��ȡ������Ϣ
    virtual char* getErrMsg() = 0;

    //�ύ
    virtual int commit() = 0;

    //�ع�
    virtual int rollback() = 0;

    //����
    virtual void release() = 0;
};


/*�������ӹ���*/
extern DSCIConnection* DSCI_MakeConnection(char *errStr);

extern DSCIConnection* DSCI_MakeConnection(char *errStr, char *dbStr);

#endif
