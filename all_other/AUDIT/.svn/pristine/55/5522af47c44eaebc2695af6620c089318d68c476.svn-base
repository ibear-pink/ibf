#include <assert.h>
#include "libxml/QField.h"


QField NewQField()
{
   QField self;
   self.Init = QFieldInit;
   self.Release = QFieldRelease;
   self.name = NewQString();
   self.value = NewQString();
   return self;
}

QField *MallocQField()
{
   QField *self = (QField *)malloc(sizeof(QField));
   if(!self) return NULL;
   self->Init = QFieldInit;
   self->Release = QFieldRelease;
   self->name = NewQString();
   self->value = NewQString();
   return self;
}

void QFieldInit(QField *self, char *name ,char *value)
{
   assert(self);
/*   assert(name);*/
   if(!name) return ;   
   self->name.ReSet(&self->name,name);
   self->value.ReSet(&self->value,value);
}

void QFieldRelease(QField *self)
{
   assert(self);
   self->name.Release(&self->name);
   self->value.Release(&self->value);   
}


QFieldList NewQFieldList()
{
   QFieldList self;
   self.Find = QFieldListFind;
   self.Release = QFieldListRelease;
   self.m_listField = NewQList();
   return self;
}

QFieldList *MallocQFieldList()
{
   QFieldList *self = (QFieldList *)malloc(sizeof(QFieldList));
   if(!self) return NULL;
   self->Find = QFieldListFind;
   self->Release = QFieldListRelease;
   
   self->m_listField = NewQList();
   return self;
}
char   *QFieldListFind (QFieldList *self , char *name)
{

   List *pTemp;
   List *p;
   QField *field;

   assert(self);

   if(!name) return NULL;

   if (self->m_listField.m_pHead == NULL)
      return NULL;

   p = self->m_listField.m_pTail;

   while (p)
   {
      pTemp = p->pPrev;
      field = (QField *)p->pData;
      if(field->name.Equal(&field->name,name))
      {
         return field->value.m_dataBuff;
      }
      p = pTemp;
   }

   return NULL;
}
void   QFieldListRelease(QFieldList *self)
{
   List *pTemp;
   List *p;


   assert(self);

   if (self->m_listField.m_pHead == NULL)
      return;

   p = self->m_listField.m_pTail;

   while (p)
   {
      pTemp = p->pPrev;   
      ((QField *)(p->pData))->Release(((QField *)(p->pData)));
      free(p->pData);
      p = pTemp;
   }
   self->m_listField.Clear(&self->m_listField);
}
