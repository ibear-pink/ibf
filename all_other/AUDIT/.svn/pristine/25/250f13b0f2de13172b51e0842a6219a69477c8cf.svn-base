
#ifndef _QLIST_H_
#define _QLIST_H_

#if defined(DOODLE_EXPORTS) && defined(WIN32) 
#ifndef QLIB_API
#define QLIB_API __declspec(dllexport)
#endif
#else 
#ifndef QLIB_API
#define QLIB_API
#endif
#endif


#ifndef WIN32

#ifndef BOOL
#define BOOL	int
#define TRUE	1
#define FALSE	0
#endif


#ifndef NULL
#define NULL 0
#endif

#else
#include <windows.h>
#endif
enum
  {
   QLIST_OK,
   QLIST_EMPTY,
   QLIST_BEGIN,
   QLIST_END,
   QLIST_ITEM_NOT_FOUND,
   QLIST_OUT_MEMORY
  };


#ifdef __cplusplus
extern "C"{
#endif 



typedef struct tagQList QList;
typedef struct tagList List;

struct tagList
{
    void *pData;
    struct tagList *pNext;
    struct tagList *pPrev;
};
	
struct tagQList
{
    void(*Clear)(QList *self);
    int (*Add)(QList *self,void* pData);
    int (*Insert)(QList *self,void* pData);
    void *(*GetData)(QList *self);
    int (*MoveToHead)(QList *self);
    int (*MoveToTail)(QList *self);
    int (*MoveNext)(QList *self);
    int (*MovePrev)(QList *self);
    int (*IsEmpty)(QList *self);
    int (*GetSize)(QList *self);
    int (*PlaceAtHead)(QList *self,void* pData);
    int (*Remove)(QList *self);

    List *m_pHead;
    List *m_pTail;
    List *m_pCurrent;
    int m_nSize;
};


QList QLIB_API NewQList();
QList QLIB_API *MallocQList();


void QListClear(QList *self);
int QListAdd(QList *self,void* pData);
int QListInsert(QList *self,void* pData);
void *QListGetData(QList *self);
int QListMoveToHead(QList *self);
int QListMoveToTail(QList *self);
int QListMoveNext(QList *self);
int QListMovePrev(QList *self);
int QListIsEmpty(QList *self);
int QListGetSize(QList *self);
int QListPlaceAtHead(QList *self,void* pData);
int QListRemove(QList *self);



typedef struct tagQAutoList QAutoList;

struct tagQAutoList
{
    void (*Release)(QAutoList *self);
    QList list;
};


QAutoList QLIB_API NewQAutoList();
QAutoList QLIB_API *MallocQAutoList();

void    QAutoListRelease(QAutoList *self);

typedef struct tagQInn QInn;
typedef struct tagInn Inn;

struct tagInn
{
    void *data;
    struct tagInn *pNext;
};

struct tagQInn
{
   void (*Push)(QInn *self,void *data);
   void *(*Pop)(QInn *self);
   void (*Release)(QInn *self);
   Inn *pHead;
   Inn *pTail;
};


QInn QLIB_API NewQInn();
QInn QLIB_API *MallocQInn();

void QInnRelease(QInn *self);

void QInnPush(QInn *self,void *data);
void *QInnPop(QInn *self);


#ifdef __cplusplus
}
#endif

#endif
