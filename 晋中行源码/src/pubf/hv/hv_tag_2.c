#include "find_debug.h"
/*************************************************
  Copyright (C), 2004, DHC
  File name:     hv_tag.c
  Author:    韦昭   Version:   V1.0     Date:  2004/8/25
  Description:   大额支付tag表维护函数实现
  Others: 
  Function List:
	（外部调用函数）
	int    tag_load(TTAGINFO *ti,char *fileName);
	装载文件中的tag数据表，成功返回0，失败返回－1。
	int    tag_write(TTAGINFO *ti,char *fileName);
	将ti中的数据写入到文件里去。成功返回0，失败返回－1。
	void   tag_show(TTAGINFO *ti);
	列出tag表里的各个项。
	int    tag_add(TTAGINFO *ti,char *tagName,int length);
	增加一个tag信息，如果存在了，修改，不存在，按从小到大插入。
	成功返回0，失败返回－1
	int    tag_del(TTAGINFO *ti,char *tagName);
	删除一个Tag,成功返回0，失败返回－1.
	int    tag_get(TTAGINFO *ti,char *tagname);
	返回一个tag的内容长度，没有返回0，没有返回0
	
	内部函数，只供内部调用
	int    getTag(char *strp,TTAG *tag);
	取出一个tag的信息，放入tag中去，把长度放到length域，名字放入tagName域.
	char *searchTag(char *str,char *tagName);
	得到tagName的起始位置。
	int    tagcmp(char *tag1,char *tag2);
	比较两个tagName的前后关系，tag1<tag2,返回－1，tag1=tag2,返回0,tag1>tag2，返回1
	void setTag(char *str,char *tagName,int tagLength);
	在str位置按一定格式把tagName和tagLength写入。
  History:   
    1. Date:     2004-8-25
       Author:   韦昭
       Modification:　建立
*************************************************/
#include "hv_tag_2.h"
/*************************************************
  Function:       int    tag_load(TTAGINFO *ti,char *fileName)
  Description:    在报文体中插入某个TAG内容。
  Calls:          fopen()
                  fscanf()
				  fclose()
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 TTAGINFO *ti,tag结构指针;
            2 char *fileName,记取的文件名.
  Output:   无
  Return:   成功返回0，失败返回－1
  Others:   
*************************************************/
int    tag_load(TTAGINFO *ti,char *fileName)
{
	FILE  *fp;
	if((fp=fopen(fileName,"r"))==NULL)
	{
	     printf("Cannot open this file :%s\n",fileName);
	     return TAG_OP_FAIL;
	}
	
	fscanf(fp,"%d,%s\n",&(ti->length),ti->data);
	
	fclose(fp);
    return TAG_OP_SUCCEE;
}

/*************************************************
  Function:       tag_write(TTAGINFO *ti,char *fileName)
  Description:    把ti结构中的数据写到文件里去
  Calls:          fopen()
                  fprintf()
				  fclose()
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 TTAGINFO *ti,tag结构指针;
            2 char *fileName,记取的文件名.
  Output:   无
  Return:   成功返回-1,否则返回0
  Others:   
*************************************************/
int    tag_write(TTAGINFO *ti,char *fileName)
{
	FILE  *fp;
	if((fp=fopen(fileName,"w"))==NULL)
	{
	     printf("Cannot open this file :%s\n",fileName);
	     return TAG_OP_FAIL;
	}
	
	fprintf(fp,"%d,%s",ti->length,ti->data);
	
	fclose(fp);
	
    return TAG_OP_SUCCEE;
}

/*************************************************
  Function:       void   tag_show(TTAGINFO *ti)
  Description:    显示tag表中的内容
  Calls:          getTag()
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 TTAGINFO *ti,tag结构指针;
  Output:   无
  Return:   
  Others:   
*************************************************/
void   tag_show(TTAGINFO *ti)
{
	int i;
	TTAG tag;
	
	printf("-----------------\n");
	printf("|Name\t|Length\t|\n");
	printf("-----------------\n");
	for(i=0;i<(ti->length/TAG_LENGTH);i++)
	{
		getTag(ti->data+i*TAG_LENGTH, &tag);
		printf("| %s\t| %d\t|\n",tag.tagName,tag.length);
	}
    printf("-----------------\n");

	return ;
}

/*************************************************
  Function:       tag_add(TTAGINFO *ti,char *tagName,int length)
  Description:    在ti结构中插入一个tag信息
  Calls:          tagcmp()
                  memmove()
				  setTag()
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 TTAGINFO *ti,tag结构指针;
            2 char *tagName,要插入的tag的名字；
			3 int length，要插入的tag的内容。
  Output:   无
  Return:   成功返回-1,否则返回0
  Others:   
*************************************************/
int    tag_add(TTAGINFO *ti,char *tagName,int length)
{
	int i;
	int cmpReturn;

	for(i = 0;i<ti->length/TAG_LENGTH;i++)/*插入某个tag*/
	{	
		cmpReturn = tagcmp(ti->data+1+i*TAG_LENGTH,tagName);
		if(cmpReturn > 0)
		{
			memmove(ti->data+(i+1)*TAG_LENGTH,ti->data+i*TAG_LENGTH,TAG_LENGTH);
			setTag(ti->data+i*TAG_LENGTH, tagName, length);
			ti->length += TAG_LENGTH;
			return TAG_OP_SUCCEE;
		}
		
		if(cmpReturn == 0 )/*当已经存在这个tag,改之*/
		{
			setTag(ti->data+i*TAG_LENGTH, 
				tagName,length);
			return TAG_OP_SUCCEE;
		}
	}
	/*如果还没有tag或是所有的tagName都比要插入的小，插在最后*/
	setTag(ti->data+i*TAG_LENGTH,tagName,length);
	ti->length += TAG_LENGTH;

    return TAG_OP_SUCCEE;
}

/*************************************************
  Function:       int    tag_del(TTAGINFO *ti,char *tagName)
  Description:    在ti结构中删除某个TAG信息内容。
  Calls:          searchTag(ti->data, tagName)
                  MEMSET_DEBUG(p,0,TAG_LENGTH)
				  memmove(p,p+TAG_LENGTH,ti->length-(p-ti->data)-TAG_LENGTH)
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 TTAGINFO *ti,tag结构指针;
            2 char *tagName,要删除的tag的名字；
  Output:   无
  Return:   成功返回-1,否则返回0
  Others:   
*************************************************/
int    tag_del(TTAGINFO *ti,char *tagName)
{
	char *p;

	p = searchTag(ti->data, tagName);
	if (p == NULL)
	{
		printf("No tag : %s \n",tagName);
		return TAG_OP_FAIL;
	}

	MEMSET_DEBUG(p,0,TAG_LENGTH);
	if (( ti->length-(p-ti->data)-TAG_LENGTH ) > 0)
	{
		memmove(p,p+TAG_LENGTH,ti->length-(p-ti->data)-TAG_LENGTH);
	}
	
	ti->length -= TAG_LENGTH;
	ti->data[ti->length] = '\0';
	
    return TAG_OP_SUCCEE;
}

/*************************************************
  Function:       int    tag_get(TTAGINFO *ti,char *tagname)
  Description:    在ti中取得一个tag的域长度
  Calls:          searchTag(ti->data,tagname)
                  MEMCPY_DEBUG(tagLengthStr,p+TAG_NAME_LENGTH+2,TAG_LENGTH_LENGTH)
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 TTAGINFO *ti,tag结构指针;
            2 char *tagName,要取得信息的tag的名字；
  Output:   无
  Return:   tag的长度，没有返回0
  Others:   
*************************************************/
int    tag_get(TTAGINFO *ti,char *tagname)
{
	char *p;
	int  tagLength;
	char tagLengthStr[TAG_LENGTH_LENGTH+1];

	p=searchTag(ti->data,tagname);
	if (p==NULL)
	{
		printf("No such a tag : %s \n",tagname);
		return 0;
	}
	MEMCPY_DEBUG(tagLengthStr,p+TAG_NAME_LENGTH+2,TAG_LENGTH_LENGTH);
	tagLengthStr[TAG_LENGTH_LENGTH] = '\0';
	tagLength = atoi(tagLengthStr);
	
    return tagLength;
}

/*************************************************
  Function:       int    getTag(char *strp,TTAG *tag)
  Description:    从strp起按一定格式取得某个TAG内容。
  Calls:          
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    char *strp,起始位置
  　　　　　TTAG *tag，存放结构
  Output:   无
  Return:   返回这个tag的长度。
  Others:   
*************************************************/
int    getTag(char *strp,TTAG *tag)
{
	char tagLengthStr[TAG_LENGTH_LENGTH+1];

	MEMCPY_DEBUG(tag->tagName,strp+1,TAG_NAME_LENGTH);
	MEMCPY_DEBUG(tagLengthStr,strp+TAG_NAME_LENGTH+2,TAG_LENGTH_LENGTH);
	tagLengthStr[TAG_LENGTH_LENGTH] = '\0';
	tag->length = atoi(tagLengthStr);

	return tag->length;
}

/*************************************************
  Function:       char *searchTag(char *str,char *tagName)
  Description:    在ti结构中查找某个TAG位置。
  Calls:          
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    char *str,　起始位置
  　　　　　char *tagName　查找的tag的名字
  Output:   无
  Return:   返回tag的位置，找不到返回NULL(strstr()返回)
  Others:   
*************************************************/
char *searchTag(char *str,char *tagName)
{
	char tagStr[TAG_NAME_LENGTH+3];
	char *p;

	tagStr[0] = ':';
	MEMCPY_DEBUG(tagStr+1,tagName,TAG_NAME_LENGTH);
	tagStr[TAG_NAME_LENGTH+1] = ':';
	tagStr[TAG_NAME_LENGTH+2] = '\0';
	p = strstr(str,tagStr);

	return p;
}

/*************************************************
  Function:       int    tagcmp(char *tag1,char *tag2)
  Description:    比较两个tag的大小（按规范中的顺序）
  Calls:          
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:
  Output:   无
  Return:   tag1 < tag2 返回　-1
            tag1 = tag2 返回　0
			tag1 > tag2 返回　1
  Others:   
*************************************************/
int    tagcmp(char *tag1,char *tag2)
{
	int i;
	for( i=0;i<TAG_NAME_LENGTH;i++)
	{
		if( (int)(tag1[i])>(int)(tag2[i]) )
		{
			return 1;
		}
		else if( (int)(tag1[i])<(int)(tag2[i]) )
		{
			return -1;
		}
		else 
		{
			if(i==2)
				return 0;
		}
	}
	return 0;
}

/*************************************************
  Function:       void setTag(char *str,char *tagName,int tagLength)
  Description:    在str位置按一定格式把tagName和tagLength写入
  Calls:          
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    char *str  起始位置
            char *tagName　tag名
			int tagLength　tag域长度
  Output:   无
  Return:   
  Others:   
*************************************************/
void setTag(char *str,char *tagName,int tagLength)
{
	char tempStr[32];
	str[0]=':';
	MEMCPY_DEBUG(str+1,tagName,TAG_NAME_LENGTH);
	str[TAG_NAME_LENGTH+1] = ':';
	num2str(tagLength,str+TAG_NAME_LENGTH+2,TAG_LENGTH_LENGTH);
	
	return ;
}

void num2str(int num,char *str,int length)
{
	char tempStr[32];
	MEMSET_DEBUG(tempStr,'\0',32);
	MEMSET_DEBUG(str,'0',length);
	sprintf(tempStr,"%d",num);
	MEMCPY_DEBUG(str+length-strlen(tempStr),tempStr,length);
	return ;
}
