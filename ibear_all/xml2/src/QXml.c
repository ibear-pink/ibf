#include <assert.h>
#include "libxml/QXml.h"
#include "libxml/QXmlParser.h"

/*#include <stdlib.h>*/
#ifdef WIN32
#include <io.h>
#include <share.h>
#else
#include <unistd.h>
#endif


QXml NewQXml()
{
   QXml self;

   self.ToRoot = QXmlNodeToRoot;
   self.ParseFile = QXmlParseFile;
   self.ParseBuff = QXmlParseBuff;
   self.SaveToFile = QXmlSaveToFile;
   self.GetErrText = QXmlGetErrText;
   self.GetDeclareData = QXmlGetDeclareData;
   self.ReSet = QXmlReSet;
   self.Release = QXmlRelease;

   self.m_listDeclare = NewQFieldList();
   self.m_nodeRoot = MallocQXmlNode();
   self.m_nErrorCode = XML_OK;
   self.m_nRow = 0;
   self.m_nCol = 0;

   return self;
}

QXml *MallocQXml()
{
   QXml *self = (QXml *)malloc(sizeof(QXml));
   if(!self) return NULL;
   
   self->ToRoot = QXmlNodeToRoot;
   self->ParseFile = QXmlParseFile;
   self->ParseBuff = QXmlParseBuff;
   self->SaveToFile = QXmlSaveToFile;
   self->GetErrText = QXmlGetErrText;
   self->GetDeclareData = QXmlGetDeclareData;
   self->ReSet = QXmlReSet;
   self->Release = QXmlRelease;

   self->m_listDeclare = NewQFieldList();
   self->m_nodeRoot = MallocQXmlNode();
   self->m_nRow = 0;
   self->m_nCol = 0;
   self->m_nErrorCode = XML_OK;/*by zhuoch 20080821*/
   return self;
}

QXmlNode *QXmlNodeToRoot (QXml *self)
{
   assert(self);
   return self->m_nodeRoot;
}
int QXmlParseFile (QXml *self,char *fileName)
{
   QXmlParser parser = NewQXmlParser();

   int fd;

	assert(self);
#ifdef WIN32
	if(_sopen_s(&fd,fileName,O_RDONLY,_SH_DENYNO,0))
#else
	if((fd = open(fileName,O_RDONLY)) == -1)
#endif
	{
		return XML_IO_OPEN_FILE_ERROR;
	}

	parser.FromFile(&parser,fd);

	parser.Parse(&parser,self);
	parser.Release(&parser);
	#ifdef WIN32
		_close(fd);
	#else
		close(fd);  
	#endif
	return self->m_nErrorCode;

}

int QXmlParseBuff (QXml *self,char *buff)
{
   QXmlParser  parser = NewQXmlParser();
   if(!buff) 
   {
      parser.Release(&parser);
      return XML_DATA_EMPTY;
   }
   if(parser.FromBuff(&parser,buff) == XML_OK)
   {
      parser.Parse(&parser,self);
   }
   parser.Release(&parser);/*by zhuoch 20100518*/
   return self->m_nErrorCode;
}

int QXmlSaveToFile (QXml *self,char *fileName)
{
   QString str = NewQString();
   QXmlNode *node = NULL;
   char *buff;

   int fd;
#ifdef WIN32
   if(_sopen_s(&fd,fileName,O_RDWR|O_CREAT|O_TRUNC,_SH_DENYNO,0666))
#else
   if( (fd = open(fileName,O_RDWR|O_CREAT|O_TRUNC,0666) )  == -1)
#endif
   {
     self->m_nErrorCode = XML_IO_OPEN_FILE_ERROR;
     return self->m_nErrorCode;
   }
   node = self->ToRoot(self);
   
   self->GetDeclareData(self,&str);
   while(node)
   {
      node->GetNodeData(node,&str,0);
      node = node->ToNextNode(node,NULL);
   }
   
   buff = str.m_dataBuff;

   while(*buff)
   {
		#ifdef WIN32
			_write(fd,&(*buff),1);	
		#else
			write(fd,&(*buff),1);
		#endif
      buff++;
   }
   str.Release(&str);
   #ifdef WIN32
		_close(fd);
	#else
		close(fd);
   #endif
   return self->m_nErrorCode;
}

int QXmlGetDeclareData (QXml *self,QString *data)
{
   assert(self);
   if(!data) return XML_DATA_EMPTY;
   if(!self->m_listDeclare.m_listField.IsEmpty(&self->m_listDeclare.m_listField))
   {
      data->AppendString(data,"<?xml ");
      if(self->m_listDeclare.m_listField.MoveToHead(&self->m_listDeclare.m_listField) == QLIST_OK)
      {
         QField *field;
         do
         {
            field = (QField *)self->m_listDeclare.m_listField.GetData(&self->m_listDeclare.m_listField);
            
            data->AppendString(data,field->name.m_dataBuff);
            
            data->AppendString(data,"=");
            if(field->value.Include(&field->value,'"'))
            {
               data->AppendString(data,"'");
               QXmlDeGetEntity(&field->value);
               data->AppendString(data,field->value.m_dataBuff);
               data->AppendString(data,"'");
            }else
            {
               data->AppendString(data,"\"");
               QXmlDeGetEntity(&field->value);
               data->AppendString(data,field->value.m_dataBuff);
               data->AppendString(data,"\"");   
            }
            data->AppendString(data," ");   
         }while(self->m_listDeclare.m_listField.MoveNext(&self->m_listDeclare.m_listField) == QLIST_OK);
      }
      /*by zhuoch 20110919 data->AppendString(data,"?>\n");*/
	  data->AppendString(data,"?>");
   }
   return XML_OK;
}

char *QXmlGetErrText (QXml *self)
{
   return NULL;
}



QXmlNode *MallocQXmlNode()
{
   QXmlNode *self = (QXmlNode *)malloc(sizeof(QXmlNode));
   if(!self) return NULL;

   self->ToParent = QXmlNodeToParent;
   self->ToChild = QXmlNodeToChild;
   self->ToNextNode = QXmlNodeToNextNode;
   self->ToPrevNode = QXmlNodeToPrevNode;
   self->ToFirstChild = QXmlNodeToFirstChild;
   self->ToLastChild =  QXmlNodeToLastChild;
   self->AddChild = QXmlNodeAddChild;
   self->AddNodeToChild = QXmlNodeAddNodeToChild;
   self->SetNodeName = QXmlNodeSetNodeName;
   self->SetNodeValue = QXmlNodeSetNodeValue;
   self->GetNodeName = QXmlNodeGetNodeName;
   self->GetNodeValue = QXmlNodeGetNodeValue;
   self->GetNodeType = QXmlNodeGetNodeType;
   self->GetNodeData = QXmlNodeGetNodeData;
   self->SetAttrib = QXmlNodeSetAttrib;
   self->GetAttrib = QXmlNodeGetAttrib;
   self->RemoveAttrib = QXmlNodeRemoveAttrib;
   self->Destory = QXmlNodeDestory;
   self->Release = QXmlNodeRelease;

   self->m_szName = NewQString();
   self->m_szValue = NewQString();
   self->type = NODE;

   self->m_listAttrib = NewQFieldList();
   self->xmlChildrenNode = NULL;
   self->next = NULL;
   self->prev = NULL;
   self->parent = NULL;
   self->name=NULL;
   self->value=NULL;
   return self;
}

QXmlNode *QXmlNodeToParent (QXmlNode *self)
{
   assert(self);
   return self->parent;
   
}
QXmlNode *QXmlNodeToChild (QXmlNode *self,char *name)
{
   QXmlNode *tempNode;

   assert(self);
   tempNode = self->ToFirstChild(self);
   if(name == NULL) return tempNode;

   while(tempNode)
   {
      if(tempNode->m_szName.Equal(&tempNode->m_szName,name))
         return tempNode;
      tempNode = tempNode->next;
   }
   return NULL;
}
QXmlNode *QXmlNodeToNextNode (QXmlNode *self,char *name)
{
   QXmlNode *tempNode;

   assert(self);
   tempNode = self->next;
   if(name == NULL) return tempNode;

   while(tempNode)
   {
      if(tempNode->m_szName.Equal(&tempNode->m_szName,name))
         return tempNode;
      tempNode = tempNode->next;
   }
   return NULL;
}
QXmlNode *QXmlNodeToPrevNode (QXmlNode *self,char *name)
{
   QXmlNode *tempNode;

   assert(self);
   tempNode = self->prev;
   if(name == NULL) return tempNode;

   while(tempNode)
   {
      if(tempNode->m_szName.Equal(&tempNode->m_szName,name))
         return tempNode;
      tempNode = tempNode->prev;
   }
   return NULL;
}
QXmlNode *QXmlNodeToFirstChild (QXmlNode *self)
{
   QXmlNode *tempNode;

   assert(self);
   tempNode = self->xmlChildrenNode;
   return tempNode;
}
QXmlNode *QXmlNodeToLastChild (QXmlNode *self)
{ 
   QXmlNode *tempNode;

   assert(self);
   tempNode = self->xmlChildrenNode;
   if(tempNode == NULL) return NULL;

   while(tempNode->next)
   {
      tempNode = tempNode->next;
   }
   return tempNode;
}

BOOL QXmlNodeAddChild (QXmlNode *self,char *name ,char *value)
{
   QXmlNode *tempNode = NULL;
   QXmlNode *tempChild = NULL;

   assert(self);
   if(name == NULL)return FALSE; 
   
   tempNode = MallocQXmlNode();
   if(!tempNode) return FALSE;
   tempNode->m_szName.AppendString(&tempNode->m_szName,name);
   tempNode->name=(unsigned char *)tempNode->m_szName.m_dataBuff;
   tempNode->m_szValue.AppendString(&tempNode->m_szValue,value);
	 tempNode->value=(unsigned char *)tempNode->m_szValue.m_dataBuff;
   tempChild = self->ToLastChild(self);

   if(!tempChild) 
      self->xmlChildrenNode = tempNode;
   else
   {
      tempChild->next = tempNode;
      tempNode->prev=tempChild;/*by zhuoch 20080824*/
   }
   tempNode->parent=self;/*by zhuoch 20080824*/
   

   return TRUE;
}

BOOL QXmlNodeAddNodeToChild (QXmlNode *self,QXmlNode *node)
{
   QXmlNode *tempChild;

   assert(self);
   
   tempChild = self->ToLastChild(self);

   if(!tempChild) 
      self->xmlChildrenNode = node;
   else
   {
      node->prev = tempChild;
      tempChild->next = node;
   }
   node->parent=self;/*by zhuoch 20080824*/
   return TRUE;
}

BOOL QXmlNodeSetNodeName (QXmlNode *self , char *name)
{

   assert(self);
   if(name == NULL)return FALSE; 
   
   self->m_szName.ReSet(&self->m_szName,name);
   self->name=(unsigned char *)self->m_szName.m_dataBuff;

   return TRUE;
}
BOOL QXmlNodeSetNodeValue (QXmlNode *self , char *value)
{
   assert(self);
   if(value == NULL)return FALSE; 
   
   self->m_szValue.ReSet(&self->m_szValue,value);
   self->value=(unsigned char *)self->m_szValue.m_dataBuff;

   return TRUE;
}

char *QXmlNodeGetNodeName (QXmlNode *self)
{
   assert(self);
   return self->m_szName.m_dataBuff;
}
char *QXmlNodeGetNodeValue (QXmlNode *self)
{
   assert(self);
   return self->m_szValue.m_dataBuff;
}
int QXmlNodeGetNodeData (QXmlNode *self,QString *data ,int depth)
{
   int i;
   QString depthText;
   assert(self);
   if(!data) return -1;
   /*if(self->m_szName.IsEmpty(&self->m_szName)) return -1;*/

   depthText = NewQString();

   /*case NODE:转换value,保存属性名,转换属性值,value */
   /*case COMMENT:保存字段名a     */
   /*case HOLD:保存属性名,转换属性值,value,*/
   /*by zhuoch 20110921 for(i = 0 ;i < depth ; i++) 
   {
      depthText.AppendString(&depthText,"    ");
   }*/

   if(self->type == COMMENT)
   {
      /*注释 <!-- -->*/
      data->AppendString(data,depthText.m_dataBuff);
   /*   QXmlDeGetEntity(&self->m_szName);*/
      data->AppendString(data,commentHeader);
      data->AppendString(data,self->m_szName.m_dataBuff);
      data->AppendString(data,commentEnd);
      /*by zhuoch 20110919 data->AppendChar(data,'\n');*/
   }else if(self->type == NODE || self->type == HOLD)
   {
   /*   if(self->type == HOLD){ printf("exit\n"); exit(1);}*/
      data->AppendString(data,depthText.m_dataBuff);
      data->AppendChar(data,'<');
      data->AppendString(data,self->m_szName.m_dataBuff);

      if(self->m_listAttrib.m_listField.MoveToHead(&self->m_listAttrib.m_listField) == QLIST_OK)
      {
         QField *field;
         do
         {
            field = (QField *)self->m_listAttrib.m_listField.GetData(&self->m_listAttrib.m_listField);

            data->AppendChar(data,' ');
            
            data->AppendString(data,field->name.m_dataBuff);
            
            data->AppendString(data, "=");
            if(field->value.Include(&field->value,'"'))
            {
               data->AppendChar(data,'\'');
               QXmlDeGetEntity(&field->value);
               data->AppendString(data,field->value.m_dataBuff);
               data->AppendChar(data,'\'');
            }else
            {
               data->AppendChar(data,'"');
               QXmlDeGetEntity(&field->value);
               data->AppendString(data,field->value.m_dataBuff);
               data->AppendChar(data,'"');   
            }
         }while(self->m_listAttrib.m_listField.MoveNext(&self->m_listAttrib.m_listField) == QLIST_OK);
      }
      if(self->m_szValue.IsEmpty(&self->m_szValue)
         && !self->xmlChildrenNode)
      {
         
         /*by zhuoch 20110919 data->AppendString(data,"/>\n");*/
		 data->AppendString(data,"/>");
      }
      else
      { 
         /*有值或子项 */
         /*by zhuoch 20110919 data->AppendString(data,">\n");*/
		 data->AppendString(data,">");
         data->AppendString(data,depthText.m_dataBuff);
         if(!self->m_szValue.IsEmpty(&self->m_szValue))
         {
            if(self->type == NODE )
            {
            /*   printf("[%s]\n",self->m_szValue.m_dataBuff); */
               QXmlDeGetEntity(&self->m_szValue);
            }
            
            if (self->type == HOLD )
            {
               data->AppendString(data,cdataHeader);
               data->AppendString(data,self->m_szValue.m_dataBuff);
               data->AppendString(data,cdataEnd);
            }else
            {
               data->AppendString(data,self->m_szValue.m_dataBuff);
            }
            /*by zhuoch 20110919 data->AppendChar(data,'\n');*/
         }
         if(self->xmlChildrenNode)
         {
            QXmlNode *child = self->xmlChildrenNode;
            while(child)
            {
               child->GetNodeData(child,data,depth+1);
               child = child->next;
            }
            /*by zhuoch 20110919 data->AppendChar(data,'\n');*/
         }
         data->AppendString(data,depthText.m_dataBuff);
         data->AppendString(data,"</");
         data->AppendString(data,self->m_szName.m_dataBuff);
         /*by zhuoch 20110919 data->AppendString(data,">\n");  */ 
		 data->AppendString(data,">"); 
      }
   }
   depthText.Release(&depthText);
   return XML_OK;
}

int QXmlNodeGetNodeType (QXmlNode *self)
{
   assert(self);
   return self->type;
   
}

BOOL QXmlNodeSetAttrib (QXmlNode *self , char *name , char * value)
{
   QField *pAttrib = NULL;
   assert(self);
   if(name == NULL)
   {
      return FALSE;
   }

   if(self->m_listAttrib.m_listField.MoveToHead(&self->m_listAttrib.m_listField) == QLIST_OK)
   {
      /*QField *pAttrib;*/
      do
      {
         pAttrib = (QField *)self->m_listAttrib.m_listField.GetData(&self->m_listAttrib.m_listField);
         if(pAttrib->name.Equal(&pAttrib->name,name))
         {
            pAttrib->value.ReSet(&pAttrib->value,value);
            return TRUE;
         }

         }while(self->m_listAttrib.m_listField.MoveNext(&self->m_listAttrib.m_listField) == QLIST_OK);
   }

   pAttrib = MallocQField();
   pAttrib->name.AppendString(&pAttrib->name,name);
   pAttrib->value.AppendString(&pAttrib->value,value);

   self->m_listAttrib.m_listField.Add(&self->m_listAttrib.m_listField,pAttrib);
   
   return TRUE;   
}
char *QXmlNodeGetAttrib (QXmlNode *self , char *name)
{

   assert(self);

   if(name == NULL)
   {
      return NULL;
   }

   if(self->m_listAttrib.m_listField.MoveToHead(&self->m_listAttrib.m_listField) == QLIST_OK)
   {
      QField *pAttrib;
      do
      {
         pAttrib = (QField *)self->m_listAttrib.m_listField.GetData(&self->m_listAttrib.m_listField);
         if(pAttrib->name.Equal(&pAttrib->name,name))
         {
            return pAttrib->value.m_dataBuff;
         }

      }while(self->m_listAttrib.m_listField.MoveNext(&self->m_listAttrib.m_listField) == QLIST_OK);
   }
   return NULL;

}
BOOL QXmlNodeRemoveAttrib (QXmlNode *self , char *name)
{
   assert(self);

   if(name == NULL)
   {
      return FALSE;
   }

   if(self->m_listAttrib.m_listField.MoveToHead(&self->m_listAttrib.m_listField) == QLIST_OK)
   {
      QField *pAttrib;
      do
      {
         pAttrib = (QField *)self->m_listAttrib.m_listField.GetData(&self->m_listAttrib.m_listField);
         if(pAttrib->name.Equal(&pAttrib->name,name))
         {
            return    self->m_listAttrib.m_listField.Remove(&self->m_listAttrib.m_listField) == QLIST_OK ?TRUE:FALSE;
         }

         }while(self->m_listAttrib.m_listField.MoveNext(&self->m_listAttrib.m_listField) == QLIST_OK);
   }

   return FALSE;
}

void QXmlNodeDestory (QXmlNode *self)
{
   assert(self);
   /*如果是root节点(parentNode == NULL)，清空所有节点*/
  /*如果是第一个节点，就把父节点的第一个节点为下一个节点*/
   /*如果不是第一节点，就将上一个节点的下一个节点。指向下一个节点*/

   if(self->parent != NULL)
   {
      if(self->prev == NULL)
      {
         self->parent->xmlChildrenNode = self->next;
      }
      else
      {
         self->prev->next = self->next;
      }
   }
   
   self->Release(self);
}




int QXmlDeGetEntity (QString *str)
{
   int i=0;
   char c;
   QString newStr = NewQString();

   if(str == NULL) return -1;

   newStr.AllocBuffer(&newStr,str->m_nDataLength);


   while( i < str->m_nDataLength )
   {
      c = str->m_dataBuff[i];
      if ( c == '&' )
      {
         newStr.AppendString(&newStr,entity[0].str);
      }
      else if ( c == '<' )
      {
         newStr.AppendString(&newStr,entity[1].str);

      }
      else if ( c == '>' )
      {
         newStr.AppendString(&newStr,entity[2].str);
      
      }
      else if ( c == '\"' )
      {
         newStr.AppendString(&newStr,entity[3].str);
      
      }
      else if ( c == '\'' )
      {
         newStr.AppendString(&newStr,entity[4].str);      
      }
      else
      {
         newStr.AppendChar(&newStr,c);
      }
      ++i;
   }   
   str->ReSet(str,newStr.m_dataBuff);
   newStr.Release(&newStr);
   return 1;
}
void QXmlReSet (QXml *self)
{
   assert(self);
   self->Release(self);
   self->m_nodeRoot = MallocQXmlNode();
}

void QXmlRelease (QXml *self)
{
   QXmlNode *tempNode;
   QXmlNode *tempNodeNext;

   QXmlNode *child;
   QXmlNode *tempChild;

   assert(self);
   
   self->m_listDeclare.Release(&self->m_listDeclare);
   self->m_nCol = 0;
   self->m_nErrorCode = XML_OK;
   self->m_nRow = 0;

   if(self->m_nodeRoot)
   {
      child = self->m_nodeRoot->xmlChildrenNode;
      
      while(child)
      {
         tempChild = child;
         child = child->next;
      
         
         tempChild->Release(tempChild);
         free(tempChild);
         tempChild = NULL;
      }
      self->m_nodeRoot->xmlChildrenNode = NULL;

      tempNode = self->m_nodeRoot->next;

      while(tempNode)
      {
         tempNodeNext = tempNode;
         tempNode = tempNode->next;
      
         tempNodeNext->Release(tempNodeNext);         
         free(tempNodeNext);
         tempNodeNext = NULL;         
      }      
      self->m_nodeRoot->Release(self->m_nodeRoot);
      free(self->m_nodeRoot);
      self->m_nodeRoot = NULL;
   }

}
void QXmlNodeRelease (QXmlNode *self)
{
   QXmlNode *child;
   QXmlNode *tempChild;

   assert(self);

   self->m_szName.Release(&self->m_szName);
   self->m_szValue.Release(&self->m_szValue);
   self->name=NULL;
   self->value=NULL;
   
   /* 应该处理子节点的数据 :( */
   child = self->xmlChildrenNode;
   while(child)
   {
      tempChild = child;
      child = child->next;
      tempChild->Release(tempChild);
      free(tempChild);
      tempChild = NULL;
   }
   self->m_listAttrib.Release(&self->m_listAttrib);
}
