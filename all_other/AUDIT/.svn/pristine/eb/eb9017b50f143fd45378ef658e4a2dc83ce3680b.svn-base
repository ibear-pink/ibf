#include <assert.h>
#include <limits.h>
#include "libxml/QString.h"
#include <memory.h>
#include <ctype.h>



QString   NewQString()
{
   QString self;
   self.Init =  QStringInit;
   self.Release = QStringRelease;
   self.AllocBuffer = QStringAllocBuffer;
   self.GetLength = QStringGetLength;
   self.IsEmpty = QStringIsEmpty;
   self.ReSet = QStringReSet;
   self.Append = QStringAppend;
   self.AppendChar = QStringAppendChar;
   self.AppendInt = QStringAppendInt;
   self.AppendString = QStringAppendString;
   self.GetAt = QStringGetAt;
   self.Include = QStringInclude;
   self.Find = QStringFind;
   self.FindString = QStringFindString;
   self.ToInt = QStringToInt;
   self.CompareString = QStringCompareString;
   self.Equal = QStringEqual;
   self.Replace = QStringReplace;
   self.Mid = QStringMid;
   self.MidIndex = QStringMidIndex;
   self.Left = QStringLeft;
   self.Right = QStringRight;
   self.Trim = QStringTrim;
   self.TrimRight = QStringTrimRight;
   self.TrimLeft = QStringTrimLeft;
   self.MakeUpper = QStringMakeUpper;
   self.MakeLower = QStringMakeLower;
   self.Delete = QStringDelete;
   self.Split = QStringSplit;

   self.Init(&self);
   return self;               
}

QString   *MallocQString()
{
   QString *self = (QString *)malloc(sizeof(QString));
   
   if(!self) return NULL;

   self->Init =  QStringInit;
   self->Release = QStringRelease;
   self->AllocBuffer = QStringAllocBuffer;
   self->GetLength = QStringGetLength;
   self->IsEmpty = QStringIsEmpty;
   self->ReSet = QStringReSet;
   self->Append = QStringAppend;
   self->AppendChar = QStringAppendChar;
   self->AppendInt = QStringAppendInt;
   self->AppendString = QStringAppendString;
   self->GetAt = QStringGetAt;
   self->Include = QStringInclude;
   self->Find = QStringFind;
   self->FindString = QStringFindString;
   self->ToInt = QStringToInt;
   self->CompareString = QStringCompareString;
   self->Equal = QStringEqual;
   self->Replace = QStringReplace;
   self->Mid = QStringMid;
   self->MidIndex = QStringMidIndex;
   self->Left = QStringLeft;
   self->Right = QStringRight;
   self->Trim = QStringTrim;
   self->TrimRight = QStringTrimRight;
   self->TrimLeft = QStringTrimLeft;
   self->MakeUpper = QStringMakeUpper;
   self->MakeLower = QStringMakeLower;
   self->Delete = QStringDelete;
   self->Split = QStringSplit;

   self->Init(self);
   return self;               
}

void QStringInit(QString * self)
{
   assert(self);
   self->m_dataBuff = NULL;
   self->m_nDataLength = 0;
   self->m_nAllocLength = 0;
}
void QStringRelease(QString * self)
{
   assert(self);
   if(self->m_nAllocLength) free(self->m_dataBuff);

   self->m_dataBuff = NULL;
   self->m_nDataLength = 0;
   self->m_nAllocLength = 0;
}

BOOL QStringAllocBuffer(QString * self,int nLen)
{
   assert(nLen >= 0);
   /* delete by houxr*/
   assert(nLen <= INT_MAX - 1);  
   assert(self);
   if (nLen == 0){
      self->Init(self);
   }else{
      char *pData = NULL;
      int bufflen  = (int)sizeof(char)*nLen+(int)sizeof(char);
      pData = (char *)malloc(bufflen);
      if(pData == NULL)
		return FALSE;
      self->m_nAllocLength = nLen;
      memset(pData,0,bufflen);
      self->m_dataBuff = pData;
   }
   return TRUE;   
}

int QStringGetLength(QString * self)
{
   assert(self);
   return self->m_nDataLength;
}

BOOL QStringIsEmpty(QString * self)
{
   assert(self);
   return self->m_nDataLength>0?FALSE:TRUE;
}

void QStringReSet(QString * self,const char *szStr)
{
   assert(self);

   self->Release(self);
   if(!szStr) return;
   self->Append(self,szStr,(int)strlen(szStr));

}
void QStringAppend(QString * self,const char *szStr,int nLen)
{
   int nNewStrSize;
   int nStrSize;
   char *pStr;
   char *tmpData;
   int newAllocSize;

   assert(self);
   if(!szStr) return;         
   nStrSize = 0;
   
   pStr = (char *)szStr;
   
   while(*pStr && nStrSize < nLen){
      pStr ++;
      nStrSize ++;
   }
   
   if(nStrSize < 0)
      return;
   
   nNewStrSize = self->m_nDataLength + nStrSize + (int)sizeof(char);
   
   if(nNewStrSize > self->m_nAllocLength){
      tmpData = self->m_dataBuff;

      newAllocSize = nNewStrSize + /*nNewStrSize;*/nNewStrSize/4;
      if(!self->AllocBuffer(self,newAllocSize))
         return ;
   
      memcpy(self->m_dataBuff,tmpData,self->m_nDataLength);
	  if(tmpData!=NULL)
	  {
		free(tmpData);
	  }
      memcpy(self->m_dataBuff + self->m_nDataLength,szStr,nStrSize);
      
      self->m_nDataLength += nStrSize; 
      self->m_nAllocLength = newAllocSize;
   }else{
      memcpy (self->m_dataBuff + self->m_nDataLength, szStr,nStrSize);
      self->m_nDataLength += nStrSize; 
   }
   self->m_dataBuff[self->m_nDataLength] = 0;
}

void QStringAppendChar(QString * self,char ch)
{
   char tempData[2] = {0,0};
   assert(self);
   tempData[0] = ch;
   self->Append(self,tempData,1);
}
void QStringAppendInt(QString * self,int n)
{
   char tempData[32];
   assert(self);
   #ifdef WIN32
		sprintf_s(tempData,32,"%d",n);  
	#else
		sprintf(tempData,"%d",n);  
	#endif

   self->Append(self,tempData,(int)strlen(tempData));
}
void QStringAppendString(QString * self,const char *str)
{
   assert(self);
   if(!str) return ;
   
   self->Append(self,str,(int)strlen(str));   
}

char QStringGetAt(QString * self,int index)
{
   assert(self);
   if(index<0 || index >= self->m_nDataLength || self->m_nDataLength<=0) 
       return 0;
   
   return self->m_dataBuff[index];
}

BOOL QStringInclude(QString * self,char ch)
{
   assert(self);
   return self->Find(self,ch,0)>=0?TRUE:FALSE;
}

int QStringFind(QString * self,char ch,int offset)
{
   char * lookUp;
   assert(self);
            
   if (offset >= self->m_nDataLength)
      return -1;
   for (lookUp = self->m_dataBuff + offset; * lookUp; lookUp++)
      if (* lookUp == ch)
         return (int)(lookUp - self->m_dataBuff);
   return -1;      
}

int QStringFindString(QString * self,const char *str,int offset)
{
   int strLen = 0;
   int i = 0;
   char *pStr = (char *)str;
   
   assert(self);
   
   if(pStr == NULL) return -1;

   while(*pStr){
      strLen++;
      pStr++;
   }   

   i = offset;
   while(i < self->m_nDataLength){
      if(strncmp(self->m_dataBuff + i, str, strLen) == 0)
         return i;
      i++;
   }
   return -1;   
}

int QStringToInt(QString * self)
{
   assert(self);
   return atoi(self->m_dataBuff);
}

int QStringCompareString(QString * self,const char * lpsz)
{
   assert(self);

   return strcmp(self->m_dataBuff,lpsz);
}

BOOL QStringEqual(QString * self,const char * lpsz)
{
   assert(self);
   
   if(self->m_dataBuff == NULL) return FALSE;
   
   return strcmp(self->m_dataBuff,lpsz) == 0 ? TRUE:FALSE;   
}

int QStringReplace(QString * self,char chOld, char chNew)
{
   int nCount = 0;   

   assert(self);
   
   if (chOld != chNew){
      char *psz = self->m_dataBuff;
      
      for(; * psz; psz++){
         if (*psz == chOld){
            *psz = chNew;
            nCount++;
         }
      }
   }
   return nCount;   
}

char *QStringMid(QString * self,int nFirst)
{
   assert(self);
   return self->MidIndex(self,nFirst, self->m_nDataLength - nFirst);
}
char *QStringMidIndex(QString * self,int nFirst,int nCount)
{
   QString dest = NewQString();
   assert(self);

   nFirst = nFirst < 0 ? 0:nFirst;
   nCount = nCount < 0 ? 0:nCount;

   if(nFirst + nCount > self->m_nDataLength)
      nCount = self->m_nDataLength - nFirst;
   if (nFirst > self->m_nDataLength)
      nCount = 0;
   
   dest.AllocBuffer(&dest,nCount);
   memcpy(dest.m_dataBuff,self->m_dataBuff+nFirst,nCount);
   /*dest.m_dataBuff[nCount] = 0;*/
   return dest.m_dataBuff;
}

char *QStringLeft(QString * self,int nCount)
{
   QString dest = NewQString();
   
   assert(self);
 

   if(nCount < 0)
      nCount = 0;
   else if (nCount > self->m_nDataLength)
      nCount = self->m_nDataLength;
   
   dest.AllocBuffer(&dest,nCount);

   memcpy(dest.m_dataBuff,self->m_dataBuff,nCount);
   dest.m_dataBuff[nCount] = 0;
   return dest.m_dataBuff;   
}
char *QStringRight(QString * self,int nCount)
{
   QString dest = NewQString();
   assert(self);

   if(nCount < 0)
      nCount = 0;
   else if (nCount > self->m_nDataLength)
      nCount = self->m_nDataLength;

   dest.AllocBuffer(&dest,nCount);
   memcpy(dest.m_dataBuff,self->m_dataBuff+
          (self->m_nDataLength-nCount),nCount);   
   return dest.m_dataBuff;   
}

void QStringTrim(QString * self)
{
   char *lpsz = NULL;
   char *lpszLast = NULL;
   int nDataLength;
   int nMoveDataLength;
   int nTmpLength;
   assert(self);

   if(!self->m_dataBuff) return;

   lpsz = self->m_dataBuff;

   while ((*lpsz ==' ' || 
         *lpsz == '\t' || 
          *lpsz =='\n' ||
           *lpsz =='\r')&& *lpsz)
      lpsz++;
  
   /* fix up data and length*/
   nMoveDataLength = (int)(unsigned long)(lpsz - self->m_dataBuff);
   nDataLength = self->m_nDataLength - nMoveDataLength;
   nTmpLength  = (nDataLength + 1) * (int)sizeof(char);
   memmove(self->m_dataBuff, lpsz, nTmpLength);
   
   self->m_nAllocLength-= nMoveDataLength;   
   self->m_nDataLength = nDataLength;
   
   while (*lpsz != '\0'){
      if( *lpsz ==' ' || *lpsz == '\t' || *lpsz =='\n' || *lpsz =='\r'){
         if (lpszLast == NULL)
            lpszLast = lpsz;
         }else{
            lpszLast = NULL;
         }
      lpsz++;
   }

   if(lpszLast != NULL){
      /* truncate at trailing space start*/
      *lpszLast = '\0';
      nDataLength = (int)(unsigned long)(lpszLast - self->m_dataBuff);
      nMoveDataLength = self->m_nDataLength - nDataLength;
      self->m_nDataLength = nDataLength;
      self->m_nAllocLength -= nMoveDataLength;
   }

}
void QStringTrimRight(QString * self)
{
   char *lpsz = NULL;
   char *lpszLast = NULL;
   int nMoveDataLength;
   int nDataLength ;

   assert(self);

   if(!self->m_dataBuff) return;
   lpsz = self->m_dataBuff;
   while (*lpsz != '\0'){
      if (*lpsz ==' ' || *lpsz == '\t' || *lpsz =='\n' || *lpsz =='\r'){
         if (lpszLast == NULL)
            lpszLast = lpsz;
      }else{
         lpszLast = NULL;
      }
      lpsz++;
   }
   
   if (lpszLast != NULL){
      /* truncate at trailing space start*/
      *lpszLast = '\0';
      nDataLength = (int)(unsigned long)(lpszLast - self->m_dataBuff);
      nMoveDataLength = self->m_nDataLength - nDataLength;
      self->m_nDataLength = nDataLength;
      self->m_nAllocLength -= nMoveDataLength;
   }
}      

void QStringTrimLeft(QString * self)
{
   /* find first non-space character*/
   char *lpsz = NULL;
   int nMoveDataLength;
   int nDataLength ;
   int nTmpLength;

   assert(self);

   if(!self->m_dataBuff) return;

   lpsz = self->m_dataBuff;
   while ((*lpsz ==' ' || *lpsz == '\t' || *lpsz =='\n' || *lpsz =='\r')&&
          *lpsz)
     lpsz++;

   /* fix up data and length */
   nMoveDataLength = (int)(unsigned long)(lpsz - self->m_dataBuff);
   nDataLength = self->m_nDataLength - nMoveDataLength ;
   nTmpLength = (nDataLength + 1) * (int)sizeof(char);
   memmove(self->m_dataBuff, lpsz, nTmpLength);
   self->m_nDataLength = nDataLength;
   self->m_nAllocLength -= nMoveDataLength;
}   

char *QStringMakeUpper(QString * self)
{
   char *lpsz;

   assert(self);
   if(!self->m_dataBuff) return "";
   lpsz = self->m_dataBuff;
   while (*lpsz != '\0'){
      if(isalpha(*lpsz)&&islower(*lpsz))
         *lpsz = toupper(*lpsz);
      lpsz++;
   }
   return self->m_dataBuff;
}      

char *QStringMakeLower(QString * self)
{
   char *lpsz;

   assert(self);

   if(!self->m_dataBuff) return "";
   lpsz = self->m_dataBuff;
   while (*lpsz != '\0')
   {
      if(isalpha(*lpsz)&&isupper(*lpsz))
         *lpsz = tolower(*lpsz);
      lpsz++;
   }
   return self->m_dataBuff;

}   

int   QStringDelete(QString * self,int nIndex, int nCount )
{
   int nNewLength ;
   assert(self);
   
   if (nIndex < 0)
      nIndex = 0;
   nNewLength = self->m_nDataLength;
   if (nCount > 0 && nIndex < nNewLength)
   {
      int nBytesToCopy = (nNewLength - (nIndex + nCount)) + 1;
      if(nBytesToCopy <0 ) {   return -1;}
      memmove(self->m_dataBuff + nIndex, self->m_dataBuff + nIndex + nCount, nBytesToCopy * sizeof(char));
      self->m_nDataLength = nNewLength - nCount;
      self->m_nAllocLength -= nCount;
   }
   return nNewLength;
}

QList   QStringSplit(QString * self,char ch )
{
   QList stringlist = NewQList();
   char * lookUp;
   QString word = NewQString();
   QString * newWord = NULL;
   BOOL hasChar = FALSE;
   
   assert(self);   

   for (lookUp = self->m_dataBuff; * lookUp; lookUp++)
   {
      if (* lookUp == ch)
      {
         hasChar = TRUE;
         newWord = MallocQString();
         newWord->AppendString(newWord,word.m_dataBuff);
         stringlist.Add(&stringlist,newWord);

         word.Release(&word);
         lookUp++;
         if(!*lookUp) break;
      }
      word.AppendChar(&word , *lookUp);
   }
   if(!hasChar)
   {
      stringlist.Clear(&stringlist);
   }
   else
   {
      if(!word.IsEmpty(&word))
      {   
         newWord = MallocQString();
         newWord->AppendString(newWord,word.m_dataBuff);
         stringlist.Add(&stringlist,newWord);   
      }
      
   }
   word.Release(&word);
   return stringlist;
}


QStringList NewQStringList()
{
   QStringList self;
   self.list = NewQList();
   self.Release = QStringListRelease;
   return self;
}
QStringList *MallocQStringList()
{
   QStringList *self = (QStringList *) malloc(sizeof(QStringList));
   if(!self) return NULL;

   self->list = NewQList();
   self->Release = QStringListRelease;
   return self;

}
void   QStringListRelease(QStringList * self)
{
   QString *str;
   assert(self);
   if(self->list.MoveToHead(&self->list) == QLIST_OK)
   {
      str = (QString *)self->list.GetData(&self->list);
      while(str)
      {
         str->Release(str);
         free(str);
         str = NULL;
         self->list.MoveNext(&self->list);   
         str = (QString *)self->list.GetData(&self->list);
      }
   }
   self->list.Clear(&self->list);
}
