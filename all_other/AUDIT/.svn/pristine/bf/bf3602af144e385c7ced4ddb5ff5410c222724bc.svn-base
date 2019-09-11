#include <assert.h>

#include <stdlib.h>
#ifdef WIN32
#include <io.h>
#else
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#endif

#include "libxml/QXmlParser.h"

char *xmlHeader = "<?xml";
char *commentHeader = "<!--";
char *commentEnd = "-->";
char *cdataHeader = "<![CDATA[";
char *cdataEnd = "]]>";

#define XMLENTITY 5
	
QXmlEntity entity[XMLENTITY] = 
{
   {"&amp;",  '&' },
   {"&lt;",   '<' },
   {"&gt;",   '>' },
   {"&quot;", '\"' },
   {"&apos;", '\'' }
};

QXmlParser   NewQXmlParser()
{
   QXmlParser self;

   self.Parse = QXmlParserParse;
   self.Worker = QXmlParserWorker;
   self.FromFile = QXmlParserFromFile;
   self.FromBuff = QXmlParserFromBuff;
   self.ParseDeclaration = QXmlParserDeclaration;
   self.ParseCommentText = QXmlParserCommentText;
   self.ParseNode = QXmlParserNode;
   self.ParseAttrib = QXmlParserAttrib;
   self.ReadName = QXmlParserReadName;
   self.ReadValue = QXmlParserReadValue;
   self.ReadText = QXmlParserReadText;
   self.GetEntity = QXmlParserGetEntity;
   self.Release = QXmlParserRelease;

   self.xml = NULL;
   self.m_ioXML = NewQXmlIO();
   self.m_nodeCurr = NULL;
   return self;
}

QXmlParser   *MallocQXmlParser()
{
   QXmlParser *self = (QXmlParser *)malloc(sizeof(QXmlParser));
   if(!self) return NULL;

   self->Parse = QXmlParserParse;
   self->Worker = QXmlParserWorker;
   self->FromFile = QXmlParserFromFile;
   self->FromBuff = QXmlParserFromBuff;
   self->ParseDeclaration = QXmlParserDeclaration;
   self->ParseCommentText = QXmlParserCommentText;
   self->ParseNode = QXmlParserNode;
   self->ParseAttrib = QXmlParserAttrib;
   self->ReadName = QXmlParserReadName;
   self->ReadValue = QXmlParserReadValue;
   self->ReadText = QXmlParserReadText;
   self->GetEntity = QXmlParserGetEntity;
   self->Release = QXmlParserRelease;

   self->xml = NULL;
   self->m_ioXML = NewQXmlIO();
   self->m_nodeCurr = NULL;
   return self;
}

int QXmlParserParse (QXmlParser *self,QXml *xml)
{
   QXmlNode *node ;/*= MallocQXmlNode();*/

   assert(self);
   assert(xml);
   
   self->xml = xml;

   self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
   if ( self->m_ioXML.IsEmpty(&self->m_ioXML) )
   {
      xml->m_nErrorCode = XML_DATA_EMPTY;
      return XML_DATA_EMPTY;
   }
   self->m_nodeCurr = xml->m_nodeRoot;

   while ( !self->m_ioXML.IsEmpty(&self->m_ioXML) )
   {
      self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
      /*执行解析工作*/
      if(self->Worker(self) != XML_OK ) 
      {
         self->xml->m_nCol = self->m_ioXML.m_nCol;
         self->xml->m_nRow = self->m_ioXML.m_nRow;
         break;
      }else
      {
         if(self->m_nodeCurr->type != DECLARE && 
         !self->m_nodeCurr->m_szName.IsEmpty(&self->m_nodeCurr->m_szName))
         {
            node = MallocQXmlNode();
            if(node == NULL)
            {
               self->xml->m_nErrorCode =  XML_OUT_MEMORY;
               break;
            }
            node->parent = self->m_nodeCurr->parent;
            node->prev = self->m_nodeCurr;
            self->m_nodeCurr->next = node;
            self->m_nodeCurr = node;
         }
	 
      }
   }
   if ( xml->m_nErrorCode == XML_OK)
   {
      if(self->m_nodeCurr->prev) 
	 self->m_nodeCurr->prev->next = NULL;

      self->m_nodeCurr->Release(self->m_nodeCurr);
      free(self->m_nodeCurr);
      self->m_nodeCurr = NULL;
   }

   return xml->m_nErrorCode;
}

int QXmlParserWorker (QXmlParser *self)
{
   QString szHeader = NewQString();
   char *p;
   int count=0;
   assert(self);

   szHeader.AllocBuffer(&szHeader,50);

   szHeader.m_nDataLength = self->m_ioXML.GetBuff(&self->m_ioXML,
                              szHeader.m_dataBuff,
                              50);
	 if (szHeader.m_nDataLength<=2)/*by zhuoch 20080830*/
	 {
		szHeader.Release(&szHeader);
		return self->xml->m_nErrorCode;
	 }
   if(strncmp(szHeader.m_dataBuff,
         xmlHeader,
         strlen(xmlHeader)) == 0)
   {
      /*解析XML宣告*/
      self->ParseDeclaration(self);
   }else if (strncmp(szHeader.m_dataBuff,
         commentHeader,
         strlen(commentHeader)) == 0)
   {
      /*解析一个注释   */
      self->ParseCommentText(self);
   }
   else if (szHeader.m_dataBuff[0]=='<')/*by zhuoch 20080830*/
	 {
	 		/*by zhuoch 20080730*/
	 	 	p=szHeader.m_dataBuff + 1;	 
			while(*p==' ')
			{
				p++;
				if (++count>=49){break;}
			}	  
			if ( isalpha( *p ) || 
			*p == '_' )
			{
				/*解析一个节点*/
				self->ParseNode(self);
			}
			else
			{
				if(!self->m_ioXML.IsEmpty(&self->m_ioXML))
				{
				   /*出现了不认识的部分,应该出错*/
				   self->xml->m_nErrorCode = XML_UNKNOW_FORMAT;
				}
			}

   }
   else
   {
		if(!self->m_ioXML.IsEmpty(&self->m_ioXML))
		{
		   /*出现了不认识的部分,应该出错*/
		   self->xml->m_nErrorCode = XML_UNKNOW_FORMAT;
		}
   }
   /*TODO: :) 加入解析头部为 <? 的'处理'类型*/

   szHeader.Release(&szHeader);
   return self->xml->m_nErrorCode;
}


int QXmlParserFromFile (QXmlParser *self,int fd)
{
   self->m_ioXML.FromFile(&self->m_ioXML,fd);
   return XML_OK;

}
int QXmlParserFromBuff (QXmlParser *self,char *buff)
{
   assert(self);
   if(buff == NULL) return XML_DATA_EMPTY;
   self->m_ioXML.FromBuff(&self->m_ioXML,buff);
   return XML_OK;
}

void   QXmlParserDeclaration (QXmlParser *self)
{
   QField *field;
   QString szHeader = NewQString();

   szHeader.AllocBuffer(&szHeader,2);

   assert(self);
   self->m_nodeCurr->type = DECLARE;

   self->m_ioXML.SkipChar(&self->m_ioXML, 5);
   
   if(self->m_ioXML.IsEmpty(&self->m_ioXML))
   {
      self->xml->m_nErrorCode = XML_DECLARE_ERROR;
      szHeader.Release(&szHeader);
      return ;
   }

   while ( !self->m_ioXML.IsEmpty(&self->m_ioXML))
   {
      self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
      
      szHeader.m_nDataLength = self->m_ioXML.GetBuff(&self->m_ioXML,szHeader.m_dataBuff,2);

      if(self->m_ioXML.IsEmpty(&self->m_ioXML)) 
      {
         self->xml->m_nErrorCode = XML_DECLARE_ERROR;
         szHeader.Release(&szHeader);
         return ;   
      }

      if(szHeader.Equal(&szHeader,"?>"))
      {
         self->m_ioXML.SkipChar(&self->m_ioXML, 2);
         szHeader.Release(&szHeader);
         return ;
      }
      
      field = self->ParseAttrib(self);
      if(field)
      {
         /*接受任意的字段
         printf("[%s]:[%s]\n",field->name.m_dataBuff,field->value.m_dataBuff);*/
         self->xml->m_listDeclare.m_listField.Add(&self->xml->m_listDeclare.m_listField,
            field);
      }else
      {
         if(self->xml->m_nErrorCode != XML_OK)
         {
            /* 解析属性时出错*/
            break;
         }
      }
   }
   szHeader.Release(&szHeader);
}

void   QXmlParserCommentText (QXmlParser *self)
{
   QXmlNode *node ;/*= MallocQXmlNode();*/
   /*int endlen = (int)strlen(commentHeader); */
   int endlen = (int)strlen(commentEnd);
   node = self->m_nodeCurr;
   
   assert(self);
   assert(node);

   self->m_ioXML.SkipChar(&self->m_ioXML,(int)strlen(commentHeader));

   node->type = COMMENT;

   if(self->ReadValue(self,&node->m_szName,commentEnd,TRUE) != XML_OK)
   {
      self->xml->m_nErrorCode = XML_COMMENT_ERROR;
   }
   else
   {
      self->m_ioXML.SkipChar(&self->m_ioXML,endlen);
      /*printf("注释:[%s]\n",node->m_szName->m_dataBuff);*/
   }
}

void   QXmlParserNode (QXmlParser *self)
{
   
   QXmlNode *node = NULL;
   /*QString endTag ;
   QString szHeader;*/
   QField *field;

   char c;
   
   assert(self);
   
   self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);

   self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);

   if(c != '<')
   {
      self->xml->m_nErrorCode = XML_NODE_START_ERROR;
      return ; 
   }

   self->m_ioXML.SkipChar(&self->m_ioXML,1);

   node = self->m_nodeCurr;

   node->type = NODE;
   
   if(!node)
   {
      self->xml->m_nErrorCode = XML_OUT_MEMORY;
      return; 
   }
   if(self->ReadName(self,&node->m_szName) != XML_OK)
   {
      return;
   }
   node->name=(unsigned char *)node->m_szName.m_dataBuff;/*by zhuoch 20080821*/
   /*printf("字段[%s]\n",node->m_szName->m_dataBuff);*/
   /*endTag = NewQString();
   
   endTag.AppendString(&endTag,"</");
   endTag.AppendString(&endTag,node->m_szName.m_dataBuff);
   endTag.AppendString(&endTag,">");*/
   
   self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
   /* 读出属性.并检查是不是一个空的节点.或是结束的节点  */
   /*szHeader = NewQString();
   
   szHeader.AllocBuffer(&szHeader,endTag.m_nDataLength);*/
   
   self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);

   while(!self->m_ioXML.IsEmpty(&self->m_ioXML))
   {
      self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
      
      if ( self->m_ioXML.IsEmpty(&self->m_ioXML) )
      {
         self->xml->m_nErrorCode = XML_DATA_EMPTY;         
         /*endTag.Release(&endTag);
         szHeader.Release(&szHeader);*/
         return ;    
      }
      self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);/*by zhuoch 20080730 前移*/
      if ( c == '/' )
      {
				 /*printf("[%s]\n",self->m_ioXML.m_szTempBuff.m_dataBuff);*/
         self->m_ioXML.SkipChar(&self->m_ioXML,1);
         self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);
         if ( c  != '>')
         {
            /* 不允许出现 '/ >' 这样的结尾节点  */
            self->xml->m_nErrorCode = XML_NODE_END_ERROR;
            /*endTag.Release(&endTag);
            szHeader.Release(&szHeader);*/
            return ;
         }
         self->m_ioXML.SkipChar(&self->m_ioXML,1);
         /*endTag.Release(&endTag);
         szHeader.Release(&szHeader);*/
      /*   printf("NODE[%s]\n",node->m_szName.m_dataBuff);*/
   
         return ;         
      }
      else if ( c == '>' )
      {   
         /* 节点的属性读完(无属性),并且有节点的值,或有子节点. */
         self->m_ioXML.SkipChar(&self->m_ioXML,1);
         if(self->ReadText(self,&node->m_szValue) != XML_OK)
         {
            /*endTag.Release(&endTag);
            szHeader.Release(&szHeader);*/
            return ;
         }
         node->value=(unsigned char *)node->m_szValue.m_dataBuff;/*by zhuoch 20080821*/
         /*printf("[%s]\n",self->m_ioXML.m_szTempBuff.m_dataBuff);*/
         /* 去掉 </TAG>这样的结尾 */
         /*self->m_ioXML.SkipChar(&self->m_ioXML,endTag.m_nDataLength);*/
         while(1)/*修改去掉 </TAG>这样的结尾的一个BUG 去掉endTag和szHeader by zhuoch 20080730*/
         {
			self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);
			self->m_ioXML.SkipChar(&self->m_ioXML,1);
			if (c=='>'){break;}
			if ( self->m_ioXML.IsEmpty(&self->m_ioXML) )
			{
				self->xml->m_nErrorCode = XML_DATA_EMPTY;
				break;    
			}
						
         }
         /*endTag.Release(&endTag);
         szHeader.Release(&szHeader);*/
         break;
      }
      else
      {
         /* 这里是解析属性的 */
         
         field = self->ParseAttrib(self);
         if(field){
            /*printf("[%s]:[%s]\n",field->name.m_dataBuff,field->value.m_dataBuff);*/
            node->SetAttrib(node,field->name.m_dataBuff,field->value.m_dataBuff);
            field->Release(field);
            free(field);
            field = NULL;
         }
		else
		{
			if(self->xml->m_nErrorCode != XML_OK)
			{
				/* 解析属性时出错 */
				break;
			}
		}
      }
       /*self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);by zhuoch 20080730 前移*/

   }
   /*endTag.Release(&endTag);
   szHeader.Release(&szHeader);*/
/*   printf("NODE[%s]\n",node->m_szName.m_dataBuff);*/
}
/*解析属性*/
 
QField   *QXmlParserAttrib (QXmlParser *self)
{
   QField *field;
   QString  endText;
   char c;
   assert(self);


   if((field = MallocQField()) == NULL)
   {
      self->xml->m_nErrorCode = XML_OUT_MEMORY;
      return NULL;
   }
   /*printf("%s\n",self->m_ioXML.m_szTempBuff.m_dataBuff);*/
   if(self->ReadName(self,&(field->name)) == XML_OK)
   {
      self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
      /*根据 '='得到得到单引号或双引号*/
      self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);   
      if(c != '=' || self->m_ioXML.IsEmpty(&self->m_ioXML))
      {
      	
         self->xml->m_nErrorCode = XML_ATTRIB_ERROR;   
         field->Release(field);
         free(field);
         return NULL;
      }

      self->m_ioXML.SkipChar(&self->m_ioXML,1);
      self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
      
      endText = NewQString();

      self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);   
      if ( c == '\'' )
      {
         endText.AppendString(&endText,"\'");
      }else if ( c == '"')
      {
         endText.AppendString(&endText,"\"");
      }else
      {
         /* 没有引号  */
         self->xml->m_nErrorCode = XML_NO_QUOTA;
         endText.Release(&endText);

         return NULL;
      }

      self->m_ioXML.SkipChar(&self->m_ioXML,1);

      if(self->ReadValue(self,
         &(field->value),
         endText.m_dataBuff,
         TRUE) != XML_OK)
      {
         field->Release(field);
         free(field);
         field = NULL;
      }
      self->m_ioXML.SkipChar(&self->m_ioXML,endText.m_nDataLength);

      endText.Release(&endText);
      return field;
   }
   return NULL;   
}

int QXmlParserReadName (QXmlParser *self , QString *Name)
{
   char c;
   assert(self);

   self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);

   /* 读出一个字段名.或一个属性名 */
   
   self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);

   if ( !self->m_ioXML.IsEmpty(&self->m_ioXML) &&
      ( isalpha( c ) || c == '_') )
   {
      while(   !self->m_ioXML.IsEmpty(&self->m_ioXML)
         &&   (isalnum( c ) 
         || c == '_'
         || c == '-'
         || c == '.'
         || c == ':')){
         Name->AppendChar(Name,c);
         self->m_ioXML.SkipChar(&self->m_ioXML,1);
         self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);      
      }
   }
   return XML_OK;
}

int      QXmlParserReadValue (QXmlParser *self ,                   
                  QString *Value,
                  char *endTag, /*以什么结束*/
                  BOOL conv )   /*是否进行 '&' 的转换*/

{
   char c;
   int endlen = 0;

   QString szHeader;
   QString szConvHeader;

   assert(self);
   if(endTag == NULL)
   {
      self->xml->m_nErrorCode = XML_DATA_EMPTY;
      return self->xml->m_nErrorCode ;
   }

   endlen = (int)strlen(endTag);


   self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);


   self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);   

   szHeader = NewQString();
   szConvHeader = NewQString();

   szHeader.AllocBuffer(&szHeader,endlen);

   while( !self->m_ioXML.IsEmpty(&self->m_ioXML))
   {
      
      szHeader.m_nDataLength = self->m_ioXML.GetBuff(&self->m_ioXML,
         szHeader.m_dataBuff,
         endlen);

      if(szHeader.Equal(&szHeader,endTag))
      {
         /*读取结束*/
         break;
      }
      else
      {
         if(c == '&' && conv)/* 需要进行特殊字符转义           */ 
         {
            char tempConv;
            szConvHeader.Release(&szConvHeader);
            while(c != ';' && szConvHeader.m_nDataLength <= 6)
            {
               szHeader.AppendChar(&szConvHeader,c);
               self->m_ioXML.SkipChar(&self->m_ioXML,1);
               self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);   
            }
            
            szConvHeader.AppendChar(&szConvHeader,c);
            
            self->m_ioXML.SkipChar(&self->m_ioXML,1);
            
            tempConv = self->GetEntity(self,szConvHeader.m_dataBuff);

            if(tempConv != '\0')
               Value->AppendChar(Value,tempConv);   
            else
               Value->AppendString(Value,szConvHeader.m_dataBuff);   

            szConvHeader.Release(&szConvHeader);
         }
         else
         {
            Value->AppendChar(Value,c);   
            self->m_ioXML.SkipChar(&self->m_ioXML,1);
         }
      }
      self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);
   }
   szHeader.Release(&szHeader);
   
   return self->xml->m_nErrorCode;
}

int	QXmlParserReadText(QXmlParser *self , QString *Text)
{
   char c;
   int endLen = (int)strlen(cdataHeader);
   QString szHeader;
   
   assert(self);
   
   self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
   
   szHeader = NewQString();

   szHeader.AllocBuffer(&szHeader,endLen);

   szHeader.m_nDataLength = self->m_ioXML.GetBuff(&self->m_ioXML,
                              szHeader.m_dataBuff,
                              endLen);

   if(szHeader.Equal(&szHeader,cdataHeader))
   {
      /*<![CDATA[ 除了]]> 其他所有的字符全部正常接收   */
      self->m_ioXML.SkipChar(&self->m_ioXML, endLen);

      endLen = (int)strlen(cdataEnd);
      szHeader.Release(&szHeader);
      szHeader.AllocBuffer(&szHeader,endLen);

      self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);

      while(!self->m_ioXML.IsEmpty(&self->m_ioXML))
      {
         self->m_ioXML.GetBuff(&self->m_ioXML,szHeader.m_dataBuff,endLen);
         if(self->m_ioXML.IsEmpty(&self->m_ioXML))
         {
            szHeader.Release(&szHeader);
            self->xml->m_nErrorCode = XML_NO_CDATA_END;
            return self->xml->m_nErrorCode;
         }
      
         if(szHeader.Equal(&szHeader,cdataEnd))
         {
            self->m_ioXML.SkipChar(&self->m_ioXML, endLen);
            szHeader.Release(&szHeader);
            self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
            break;
         }

         Text->AppendChar(Text,c);
         self->m_ioXML.SkipChar(&self->m_ioXML, 1);
         self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);
      }      
      self->m_nodeCurr->type = HOLD;
   }
   szHeader.Release(&szHeader);
   szHeader.AllocBuffer(&szHeader,2);


   self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);

   while ( !self->m_ioXML.IsEmpty(&self->m_ioXML) )
   {
      self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);

      self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);

      if ( c != '<' )
      {
         /* 如果有值的话.先解析值 */

         if(self->ReadValue(self,Text,"<",TRUE) != XML_OK)
         {
            szHeader.Release(&szHeader);
            return self->xml->m_nErrorCode;
         }
         else
         {            
            self->m_ioXML.GetBuff(&self->m_ioXML,&c,1);
         /*   printf("值[%s]\n",Text->m_dataBuff);   */
            continue;
         }
      } 
      else 
      {
         /* 检查是否还有子节点 */
         szHeader.m_nDataLength = self->m_ioXML.GetBuff(&self->m_ioXML,
                              szHeader.m_dataBuff,
                              2);      
         if ( szHeader.Equal(&szHeader, "</") )
         {
            szHeader.Release(&szHeader);
            return XML_OK;
         }
         else
         {
            QXmlNode *node  = MallocQXmlNode();         
            QXmlNode *nodeParent;         
            
            if(!node)
            {
               self->xml->m_nErrorCode = XML_OUT_MEMORY;
               szHeader.Release(&szHeader);
               return self->xml->m_nErrorCode; 
            }
            node->parent = self->m_nodeCurr;
            
            nodeParent = node->parent;

            self->m_nodeCurr->AddNodeToChild(self->m_nodeCurr,node);
            self->m_nodeCurr = node;

            self->m_ioXML.SkipWhiteSpace(&self->m_ioXML);
            
            if(self->Worker(self) != XML_OK)
            {
               szHeader.Release(&szHeader);
               return self->xml->m_nErrorCode;
            }
            else
            {
               self->m_nodeCurr = nodeParent;   
               continue;
            }
         }
      }
   }

   self->xml->m_nErrorCode = XML_READ_NODE_VALUE;
   szHeader.Release(&szHeader);
   return self->xml->m_nErrorCode;
}


char    QXmlParserGetEntity(QXmlParser *self , char * p)
{
   int i ;
   for( i=0; i<XMLENTITY; i++ )
   {
      if(strcmp(entity[i].str,p) == 0)
      {
         return entity[i].chr;
      }
   }
   return '\0';

}
void   QXmlParserRelease ( QXmlParser *self)
{
   assert(self);
   self->m_ioXML.Release(&self->m_ioXML);   
}
