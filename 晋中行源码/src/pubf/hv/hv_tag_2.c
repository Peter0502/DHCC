#include "find_debug.h"
/*************************************************
  Copyright (C), 2004, DHC
  File name:     hv_tag.c
  Author:    Τ��   Version:   V1.0     Date:  2004/8/25
  Description:   ���֧��tag��ά������ʵ��
  Others: 
  Function List:
	���ⲿ���ú�����
	int    tag_load(TTAGINFO *ti,char *fileName);
	װ���ļ��е�tag���ݱ��ɹ�����0��ʧ�ܷ��أ�1��
	int    tag_write(TTAGINFO *ti,char *fileName);
	��ti�е�����д�뵽�ļ���ȥ���ɹ�����0��ʧ�ܷ��أ�1��
	void   tag_show(TTAGINFO *ti);
	�г�tag����ĸ����
	int    tag_add(TTAGINFO *ti,char *tagName,int length);
	����һ��tag��Ϣ����������ˣ��޸ģ������ڣ�����С������롣
	�ɹ�����0��ʧ�ܷ��أ�1
	int    tag_del(TTAGINFO *ti,char *tagName);
	ɾ��һ��Tag,�ɹ�����0��ʧ�ܷ��أ�1.
	int    tag_get(TTAGINFO *ti,char *tagname);
	����һ��tag�����ݳ��ȣ�û�з���0��û�з���0
	
	�ڲ�������ֻ���ڲ�����
	int    getTag(char *strp,TTAG *tag);
	ȡ��һ��tag����Ϣ������tag��ȥ���ѳ��ȷŵ�length�����ַ���tagName��.
	char *searchTag(char *str,char *tagName);
	�õ�tagName����ʼλ�á�
	int    tagcmp(char *tag1,char *tag2);
	�Ƚ�����tagName��ǰ���ϵ��tag1<tag2,���أ�1��tag1=tag2,����0,tag1>tag2������1
	void setTag(char *str,char *tagName,int tagLength);
	��strλ�ð�һ����ʽ��tagName��tagLengthд�롣
  History:   
    1. Date:     2004-8-25
       Author:   Τ��
       Modification:������
*************************************************/
#include "hv_tag_2.h"
/*************************************************
  Function:       int    tag_load(TTAGINFO *ti,char *fileName)
  Description:    �ڱ������в���ĳ��TAG���ݡ�
  Calls:          fopen()
                  fscanf()
				  fclose()
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 TTAGINFO *ti,tag�ṹָ��;
            2 char *fileName,��ȡ���ļ���.
  Output:   ��
  Return:   �ɹ�����0��ʧ�ܷ��أ�1
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
  Description:    ��ti�ṹ�е�����д���ļ���ȥ
  Calls:          fopen()
                  fprintf()
				  fclose()
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 TTAGINFO *ti,tag�ṹָ��;
            2 char *fileName,��ȡ���ļ���.
  Output:   ��
  Return:   �ɹ�����-1,���򷵻�0
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
  Description:    ��ʾtag���е�����
  Calls:          getTag()
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 TTAGINFO *ti,tag�ṹָ��;
  Output:   ��
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
  Description:    ��ti�ṹ�в���һ��tag��Ϣ
  Calls:          tagcmp()
                  memmove()
				  setTag()
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 TTAGINFO *ti,tag�ṹָ��;
            2 char *tagName,Ҫ�����tag�����֣�
			3 int length��Ҫ�����tag�����ݡ�
  Output:   ��
  Return:   �ɹ�����-1,���򷵻�0
  Others:   
*************************************************/
int    tag_add(TTAGINFO *ti,char *tagName,int length)
{
	int i;
	int cmpReturn;

	for(i = 0;i<ti->length/TAG_LENGTH;i++)/*����ĳ��tag*/
	{	
		cmpReturn = tagcmp(ti->data+1+i*TAG_LENGTH,tagName);
		if(cmpReturn > 0)
		{
			memmove(ti->data+(i+1)*TAG_LENGTH,ti->data+i*TAG_LENGTH,TAG_LENGTH);
			setTag(ti->data+i*TAG_LENGTH, tagName, length);
			ti->length += TAG_LENGTH;
			return TAG_OP_SUCCEE;
		}
		
		if(cmpReturn == 0 )/*���Ѿ��������tag,��֮*/
		{
			setTag(ti->data+i*TAG_LENGTH, 
				tagName,length);
			return TAG_OP_SUCCEE;
		}
	}
	/*�����û��tag�������е�tagName����Ҫ�����С���������*/
	setTag(ti->data+i*TAG_LENGTH,tagName,length);
	ti->length += TAG_LENGTH;

    return TAG_OP_SUCCEE;
}

/*************************************************
  Function:       int    tag_del(TTAGINFO *ti,char *tagName)
  Description:    ��ti�ṹ��ɾ��ĳ��TAG��Ϣ���ݡ�
  Calls:          searchTag(ti->data, tagName)
                  MEMSET_DEBUG(p,0,TAG_LENGTH)
				  memmove(p,p+TAG_LENGTH,ti->length-(p-ti->data)-TAG_LENGTH)
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 TTAGINFO *ti,tag�ṹָ��;
            2 char *tagName,Ҫɾ����tag�����֣�
  Output:   ��
  Return:   �ɹ�����-1,���򷵻�0
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
  Description:    ��ti��ȡ��һ��tag���򳤶�
  Calls:          searchTag(ti->data,tagname)
                  MEMCPY_DEBUG(tagLengthStr,p+TAG_NAME_LENGTH+2,TAG_LENGTH_LENGTH)
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 TTAGINFO *ti,tag�ṹָ��;
            2 char *tagName,Ҫȡ����Ϣ��tag�����֣�
  Output:   ��
  Return:   tag�ĳ��ȣ�û�з���0
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
  Description:    ��strp��һ����ʽȡ��ĳ��TAG���ݡ�
  Calls:          
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    char *strp,��ʼλ��
  ����������TTAG *tag����Žṹ
  Output:   ��
  Return:   �������tag�ĳ��ȡ�
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
  Description:    ��ti�ṹ�в���ĳ��TAGλ�á�
  Calls:          
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    char *str,����ʼλ��
  ����������char *tagName�����ҵ�tag������
  Output:   ��
  Return:   ����tag��λ�ã��Ҳ�������NULL(strstr()����)
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
  Description:    �Ƚ�����tag�Ĵ�С�����淶�е�˳��
  Calls:          
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:
  Output:   ��
  Return:   tag1 < tag2 ���ء�-1
            tag1 = tag2 ���ء�0
			tag1 > tag2 ���ء�1
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
  Description:    ��strλ�ð�һ����ʽ��tagName��tagLengthд��
  Calls:          
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    char *str  ��ʼλ��
            char *tagName��tag��
			int tagLength��tag�򳤶�
  Output:   ��
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
