
#ifndef _QFIELD_H_
#define _QFIELD_H_

#include "QString.h"
#include "QList.h"


#ifdef __cplusplus
extern "C"{
#endif 

typedef struct tagQField QField;

struct tagQField
{
	void	(*Init) (QField *self , char *name ,char *value);
	void	(*Release) (QField *self);
	QString		name;
	QString		value;
};


QField	QLIB_API NewQField();
QField	QLIB_API *MallocQField();

void	QFieldInit(QField *self, char *name ,char *value);
void	QFieldRelease(QField *self);


typedef struct tagQFieldList QFieldList;

struct tagQFieldList
{
	char	*(*Find) (QFieldList *self , char *name);
	void	(*Release) (QFieldList *self);
	QList m_listField;
};


QFieldList	QLIB_API NewQFieldList();
QFieldList	QLIB_API *MallocQFieldList();

char	*QFieldListFind (QFieldList *self , char *name);
void	QFieldListRelease(QFieldList *self);


#ifdef __cplusplus
}
#endif


#endif
