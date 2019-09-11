#include <assert.h>
#include "libxml/QList.h"
#include <stdlib.h>

#ifdef WIN32
#include <malloc.h>
#endif

QList NewQList()
{
   QList self;
   self.Clear = QListClear;
   self.Add = QListAdd;
   self.Insert = QListInsert;
   self.GetData = QListGetData;
   self.MoveToHead = QListMoveToHead;
   self.MoveToTail = QListMoveToTail;
   self.MoveNext = QListMoveNext;
   self.MovePrev = QListMovePrev;
   self.IsEmpty = QListIsEmpty;
   self.GetSize = QListGetSize;
   self.PlaceAtHead = QListPlaceAtHead;
   self.Remove = QListRemove;
   
   self.m_pHead = NULL;
   self.m_pTail = NULL;
   self.m_pCurrent = NULL;
   self.m_nSize = 0;
   return self;
}

QList *MallocQList()
{
   QList *self = (QList *)malloc(sizeof(QList));
   if(!self) return NULL;

   self->Clear = QListClear;
   self->Add = QListAdd;
   self->Insert = QListInsert;
   self->GetData = QListGetData;
   self->MoveToHead = QListMoveToHead;
   self->MoveToTail = QListMoveToTail;
   self->MoveNext = QListMoveNext;
   self->MovePrev = QListMovePrev;
   self->IsEmpty = QListIsEmpty;
   self->GetSize = QListGetSize;
   self->PlaceAtHead = QListPlaceAtHead;
   self->Remove = QListRemove;

   self->m_pHead = NULL;
   self->m_pTail = NULL;
   self->m_pCurrent = NULL;
   self->m_nSize = 0;
   return self;
}


void QListClear(QList *self)
{
   List *pTemp;
   List *p;
   assert(self);
   if (self->m_pHead == NULL)
       return;
   p = self->m_pTail;
   while (p)
   {
      pTemp = p->pPrev;      
      free(p);
      p = NULL;
      p = pTemp;
   }
   self->m_pHead = self->m_pTail = self->m_pCurrent = NULL;
}

int QListAdd(QList *self,void* pData)
{
   List *p;
   
   assert(self);
   if (self->m_pHead == NULL)
      return self->PlaceAtHead(self,pData);

   p = self->m_pTail;
   p->pNext = (List*)malloc(sizeof(List));
   if (p->pNext==NULL)
      return QLIST_OUT_MEMORY;
   
   p = p->pNext;
   p->pPrev = self->m_pTail;
   p->pData = pData;
   p->pNext = NULL;
   self->m_pTail = p;
   self->m_nSize++;
   return QLIST_OK;


}
int      QListInsert(QList *self,void* pData)
{
   List *p;

   assert(self);
   if (self->m_pHead == NULL)
      return self->PlaceAtHead(self,pData);
   
   p = (List*)malloc(sizeof(List));
   if (p==NULL)
      return QLIST_OUT_MEMORY;

   if (self->m_pCurrent->pNext == NULL)
   {
      /* append*/
      self->m_pCurrent->pNext = p;
      p->pPrev = self->m_pCurrent;
      p->pNext = NULL;
   }
   else
   {
      /* insert*/
      p->pNext = self->m_pCurrent->pNext;
      p->pPrev = self->m_pCurrent;
      self->m_pCurrent->pNext->pPrev = p;
      self->m_pCurrent->pNext = p;
   }
   return QLIST_OK;

}
void   *QListGetData(QList *self)
{
   assert(self);
   if (self->m_pCurrent==NULL)
      return NULL;
   return self->m_pCurrent->pData;
}
int      QListMoveToHead(QList *self)
{
   assert(self);
   if (self->m_pHead == NULL)
   {
      self->m_pCurrent = NULL;
      return QLIST_EMPTY;
   }
   self->m_pCurrent = self->m_pHead;
   return QLIST_OK;
}
int      QListMoveToTail(QList *self)
{
   assert(self);

   if (self->m_pTail == NULL)
   {
      self->m_pCurrent = NULL;
      return QLIST_EMPTY;
   }
   self->m_pCurrent = self->m_pTail;
   return QLIST_OK;
}
int      QListMoveNext(QList *self)
{
   assert(self);

   if (self->m_pHead == NULL)
   {
      self->m_pCurrent = NULL;
      return QLIST_EMPTY;
   }
   if (self->m_pCurrent->pNext != NULL)
   {
      self->m_pCurrent = self->m_pCurrent->pNext;
      return QLIST_OK;
   }
   self->m_pCurrent = NULL;
   return QLIST_END;

}
int      QListMovePrev(QList *self)
{
   assert(self);

   if (self->m_pHead == NULL)
   {
      self->m_pCurrent = NULL;
      return QLIST_EMPTY;
   }
   if (self->m_pCurrent->pPrev != NULL)
   {
      self->m_pCurrent = self->m_pCurrent->pPrev;
      return QLIST_OK;
   }
   self->m_pCurrent = NULL;
   return QLIST_BEGIN;

}
int      QListIsEmpty(QList *self)
{
   assert(self);
   return (self->m_pHead==NULL);
}
int      QListGetSize(QList *self)
{
   assert(self);
   return (self->m_nSize);

}

int      QListPlaceAtHead(QList *self,void* pData)
{
   assert(self);

   self->m_pHead = (List*)malloc(sizeof(List));
   if (self->m_pHead==NULL)
      return QLIST_OUT_MEMORY;
   self->m_pCurrent = self->m_pHead;
   self->m_pCurrent->pData = pData;
   self->m_pCurrent->pNext = NULL;
   self->m_pCurrent->pPrev = NULL;
   self->m_pTail = self->m_pCurrent;
   self->m_nSize++;
   return QLIST_OK;
}

int QListRemove(QList *self)
{
      
   List *p;
   List *pPrev;
   List *pNext;

   assert(self);

   if (self->m_pHead == NULL)
      return QLIST_EMPTY;

   p = self->m_pCurrent;

   if(p == NULL) 
      return QLIST_END;
   pPrev = p->pPrev;
   pNext = p->pNext;

   if(pPrev == NULL)
   {

      self->m_pHead = pNext;
      pNext->pPrev = NULL;
   }
   else
   {
      pPrev->pNext = pNext;
      pNext->pPrev = pPrev;
   }

   self->m_pCurrent = pNext;
   free(p);
   p = NULL;
   self->m_nSize--;
   return QLIST_OK;
}




QAutoList   NewQAutoList()
{
   QAutoList self;
   self.list = NewQList();
   self.Release = QAutoListRelease;
   return self;

}
QAutoList   *MallocQAutoList()
{
   QAutoList *self = (QAutoList *) malloc(sizeof(QAutoList));
   if(!self) return NULL;

   self->list = NewQList();
   self->Release = QAutoListRelease;
   return self;


}

void   QAutoListRelease(QAutoList *self)
{
   List *pTemp;
   List *p;

   assert(self);

   if (self->list.m_pHead == NULL)
      return;

   p = self->list.m_pTail;

   while (p)
   {
      pTemp = p->pPrev;
      if(p->pData)
      {
         free(p->pData);
         p->pData = NULL;
      }
      p = pTemp;
   }
   self->list.Clear(&self->list);   
}




QInn   QLIB_API NewQInn()
{
   QInn self;
   self.Push = QInnPush;
        self.Pop = QInnPop;
        self.Release = QInnRelease;
   self.pHead = NULL;
   self.pTail = NULL;
   return self;
}

QInn   QLIB_API *MallocQInn()
{
   QInn *self = (QInn *)malloc(sizeof(QInn));

   if(self == NULL) return NULL;
   self->Push = QInnPush;
        self->Pop = QInnPop;
        self->Release = QInnRelease;

   self->pHead = NULL;
   self->pTail = NULL;
   return self;

}

void   QInnPush(QInn *self,void *data)
{
   Inn *p;
   Inn *temp;
   assert(self);
   
   if(data == NULL) return;

   temp = (Inn *)malloc(sizeof(Inn));
   if(temp == NULL) return ;
   
   temp->data = data;
   temp->pNext = NULL;

   if(self->pHead)
   {
      p = self->pTail;
      p->pNext = temp;
      self->pTail = temp;
   }
   else
   {
      self->pHead = temp;
      self->pTail = temp;
   }
}

void   *QInnPop(QInn *self)
{
   Inn *p;
   void *data;
   assert(self);
   p = self->pHead;
   if(p)
   {
      self->pHead = p->pNext;
      data  = p->data;
      free(p);
      p = NULL;
      return data;
   }
   return NULL;
}

void   QInnRelease(QInn *self)
{
   assert(self);
   self->pHead = NULL;
   self->pTail = NULL;
}
