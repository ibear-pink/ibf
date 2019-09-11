#include <assert.h>
#include "libxml/QXmlIO.h"
/*#include <stdlib.h>*/
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/*
   getchar 用于从源读取数据
   getbuff 调用getchar来保持临时数据
   skipchar 用于从buff中删除数据
*/

QXmlIO   NewQXmlIO()
{
   QXmlIO self;
   self.FromFile = QXmlIOFromFile;
   self.FromBuff = QXmlIOFromBuff;
   self.GetBuff = QXmlIOGetBuff;
   self.SkipWhiteSpace = QXmlIOSkipWhiteSpace;
   self.SkipChar = QXmlIOSkipChar;
   self.IsEmpty = QXmlIOIsEmpty;
   self.Release = QXmlIORelease;
   self.GetChar = NULL;
   self.m_nRow = 0;
   self.m_nCol = 0;
   self.m_bDataOver = FALSE;
   self.m_szTempBuff = NewQString();
   return self;
}

QXmlIO   *MallocQXmlIO()
{
   QXmlIO *self = (QXmlIO *)malloc(sizeof(QXmlIO));
   if(!self) return NULL;

   self->FromFile = QXmlIOFromFile;
   self->FromBuff = QXmlIOFromBuff;
   self->GetBuff = QXmlIOGetBuff;
   self->SkipWhiteSpace = QXmlIOSkipWhiteSpace;
   self->SkipChar = QXmlIOSkipChar;
   self->IsEmpty = QXmlIOIsEmpty;
   self->Release = QXmlIORelease;
   self->GetChar = NULL;

   self->m_nRow = 0;
   self->m_nCol = 0;
   self->m_bDataOver = FALSE;
   self->m_szTempBuff = NewQString();

   return self;
}

int   QXmlIOFromFile ( QXmlIO *self,int fd)
{
   assert(self);
   if(fd == -1) return -1;
   self->m_nFd = fd;
   self->GetChar = QXmlIOGetFileChar;
   return 1;
}

int QXmlIOFromBuff ( QXmlIO *self,char *buff)
{
   assert(self);
   if(!buff) return -1;
   self->m_pBuff = buff;
   self->GetChar = QXmlIOGetBuffChar;
   return 1;
}

int QXmlIOGetFileChar ( QXmlIO *self)
{
   char buf;
   assert(self);
#ifdef WIN32   
	if(_read(self->m_nFd,&buf,sizeof(buf))<=0)
#else
   if(read(self->m_nFd,&buf,sizeof(buf))<=0)
#endif
   {
      self->m_bDataOver = TRUE;
      return 0;
   }

   if(buf == '\n' || buf =='\r')
   {
      self->m_nRow ++;
      self->m_nCol = 0;
   }
   else
   {
      self->m_nCol ++;
   }
   return buf;
}

int QXmlIOGetBuffChar ( QXmlIO *self)
{
   char buf;
   assert(self);
   if(!(*self->m_pBuff))
   {
      self->m_bDataOver = TRUE;
      return 0;
   }
   buf = *self->m_pBuff++;

   if(buf == '\n' || buf =='\r')
   {
      self->m_nRow ++;
      self->m_nCol = 0;
   }
   else
   {
      self->m_nCol ++;
   }
   return buf;
}


int QXmlIOGetBuff ( QXmlIO *self , char *buff,int len)
{
   char c;
   int getlen = 0;

   assert(self);

   if(buff == NULL) return 0;
   /* 如果缓冲中的数据小于len,则需要重新申请数据*/

   if(self->m_szTempBuff.IsEmpty(&self->m_szTempBuff)
      ||self->m_szTempBuff.GetLength(&self->m_szTempBuff) < len)
   {

      c = self->GetChar(self);
      if(!c) self->m_bDataOver = TRUE;
      while(c)
      {	 
         self->m_szTempBuff.AppendChar(&self->m_szTempBuff,c);
	 if (self->m_szTempBuff.m_nDataLength > len + XML_BUFF_DATA_LENGTH)
	 {
	    /*thanks wangshujuan */
	    break;
	 }
         c = self->GetChar(self);
      }
   }
   if(len > self->m_szTempBuff.m_nDataLength) 
      len = self->m_szTempBuff.m_nDataLength;

   while( getlen < len)
   {
      *buff = self->m_szTempBuff.m_dataBuff[getlen];
      getlen ++;
      buff ++;
   }   
   return getlen;

}

void   QXmlIOSkipWhiteSpace (QXmlIO *self)
{
   char c;

   assert(self);

   self->m_szTempBuff.TrimLeft(&self->m_szTempBuff);

   if(self->m_szTempBuff.IsEmpty(&self->m_szTempBuff))
   {
      c = self->GetChar(self);
      if(!c) self->m_bDataOver = TRUE;
      while(c && self->m_szTempBuff.m_nDataLength < XML_BUFF_DATA_LENGTH)
      {
         /*if(c!='\t' && c!='\n' && c!='\r') */
	if( c!='\n' && c!='\r')
            self->m_szTempBuff.AppendChar(&self->m_szTempBuff,c);
         c = self->GetChar(self);
		 if(!c) self->m_bDataOver = TRUE;
      }
      /*if(c!='\t' && c!='\n' && c!='\r')  */
      if( c!='\n' && c!='\r')
	  self->m_szTempBuff.AppendChar(&self->m_szTempBuff,c);
   }
}
int QXmlIOSkipChar ( QXmlIO *self,int n)
{
   char c ;

   assert(self);

   if(n >= self->m_szTempBuff.GetLength(&self->m_szTempBuff))
   {
      /*缓冲区中的数据并不多*/
      c = self->GetChar(self);
      if(!c) self->m_bDataOver = TRUE;
      while(c && self->m_szTempBuff.m_nDataLength < n + XML_BUFF_DATA_LENGTH)
      {
         self->m_szTempBuff.AppendChar(&self->m_szTempBuff,c);
         c = self->GetChar(self);   
      }
      if(c) self->m_szTempBuff.AppendChar(&self->m_szTempBuff,c);
	
   }
   return self->m_szTempBuff.Delete(&self->m_szTempBuff,0,n);
}
/* 是否有数据*/
BOOL   QXmlIOIsEmpty ( QXmlIO *self)
{
   assert(self);
   return self->m_bDataOver && self->m_szTempBuff.IsEmpty(&self->m_szTempBuff);   
}

void   QXmlIORelease ( QXmlIO *self)
{
   assert(self);   
   self->m_szTempBuff.Release(&self->m_szTempBuff);
}

