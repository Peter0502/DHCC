/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvini.c
  Author:    Τ��   Version:   V1.0     Date:  2004-8-27
  Description:    ���֧��ini�ļ��������
  Others: 
  Function List:  (�������壬ʵ����hvini.c�ļ���)
	1. int hv_readIni(char *filename,char *section,char *item,char *value)
		��ȡini�ļ���ĳһ���ֵ��filenameΪ�ļ�����sectionΪ����,itemΪ����,
		valueΪ��ȡ�õ���ֵ���255���ַ���.
		�ɹ�����0��ʧ�ܷ���-1
	2��int hv_writeIni(char *filename,char *section,char *item,char *value)
		����ini�ļ���ĳһ���ֵ��filenameΪ�ļ�����sectionΪ������itemΪ����,
		valueΪҪ���õ�ֵ
		�ɹ�����0��ʧ�ܷ���-1
	
	�ڲ�������ֻ���ڲ�����
	1.  int  fgetline(FILE *fp, char *buffer, int maxlen)
	    ��ȡ�ļ��е�һ��д��buffer��ȥ�����ȡmaxlen���ֽڡ�
    2.  int  getItem(FILE *fp,char *section,char *item,char *val)
	    ��ȡsection��item��ֵ������val�У�����ֵ�뿴����˵��
    3.  int  splitItem(char *str)
	    �õ�һ��item��ֵ��ȥ��ǰ��Ŀո�ȡ�
    4.  int  writeLine(char *filename,char *buf,int line_no)
	    ��filename�еĵ�line_no���޸Ļ����buf�е�����
		line_noΪ0ʱ����ʾ���ļ������롣
		line_noΪ����ʱ��ʾ��ԭ���޸ģ�
		line_noΪ����ʱ��ʾ����һ�в��롣
  History:   
    1. Date:     2004-8-27
       Author:   Τ��
       Modification:������
*************************************************/
#include "hvini.h"

/*�ڲ�����������*/
int  fgetline(FILE *fp, char *buffer, int maxlen);
int  getItem(FILE *fp,char *section,char *item,char *val);
int  splitItem(char *str);
int  writeLine(char *filename,char *buf,int line_no);

/*************************************************
  Function:       int hv_readIni(char *filename,char *section,char *item,char *value)
  Description:    ��ȡini�ļ���ĳһ���ֵ��filenameΪ�ļ�����sectionΪ����,itemΪ����,
		          valueΪ��ȡ�õ���ֵ���255���ַ���.
  Calls:          getItem()                  
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 char *filenameΪ�ļ���
            2 char *sectionΪ����
			3 char *itemΪ����
  Output:   valueֵ������item������
  Return:   �ɹ�����0��ʧ�ܷ��أ�1
  Others:   
*************************************************/

int hv_readIni(char *filename,char *section,char *item,char *value)
{
	FILE *fp;
	int p;
	if((fp = fopen(filename,"r"))==NULL)
	{
		printf("Cannot read file:%s\n",filename);
		return  INI_OP_FAIL;
	}
	
	p = getItem(fp,section,item,value);
	
	fclose(fp);

	if(p>0)
	   return INI_OP_SUCCEE;
	else 
		return INI_OP_FAIL;
}

/*************************************************
  Function:       int hv_writeIni(char *filename,char *section,char *item,char *value)
  Description:    ����ini�ļ���ĳһ���ֵ��filenameΪ�ļ�����sectionΪ������itemΪ����,
		          valueΪҪ���õ�ֵ
  Calls:          1 getItem(fp,section,item,val)
                  2 writeLine(filename,buf,n)
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 char *filenameΪ�ļ���;
            2 char *sectionΪ����;
			3 char *itemΪ����.
  Output:   ��
  Return:   �ɹ�����0��ʧ�ܷ���-1
  Others:   
*************************************************/
int hv_writeIni(char *filename,char *section,char *item,char *value)
{
	FILE *fp;
	char val[HV_INI_MAX_LEN];
	char buf[2*HV_INI_MAX_LEN];      /*��section��item��������ʱ��bufΪ����*/
	int n;
	
	/*�����ĳ�ʼ��*/
	n = 0;

	/*����Ҫ�����λ��*/
	if((fp = fopen(filename,"r"))==NULL)
	{
		printf("Cannot read file:%s\n",filename);
		return  INI_OP_FAIL;
	}
	n = getItem(fp,section,item,val);
	fclose(fp);

    /*��������*/
	if( n != 0 )
	{   /*�޸Ļ�����ȷ�������в���ʱ*/
		snprintf(buf,HV_INI_MAX_LEN,"%s=%s",item,value);
	}
	else
	{   /*���ļ�������ʱ*/
		snprintf(buf,2*HV_INI_MAX_LEN,"[%s]\n%s=%s",section,item,value);
	}
	writeLine(filename,buf,n);
	
	return INI_OP_SUCCEE;
}
/*-----------------------------------------------*/
/*            ����Ϊ�ڲ�����                     */
/*-----------------------------------------------*/

/*************************************************
  Function:       int  fgetline(FILE *fp, char *buffer, int maxlen);
  Description:    ����ini�ļ��е�һ�С�����buffer�У����ض�ȡ������
  Calls:          
  Called By:      
  Table Accessed: ��
  Table Updated:  ��
  Input:    
  Output:   ��
  Return:   �ļ���������-1,���ִ��󷵻�-2���ɹ����ض���������.
  Others:   
*************************************************/

int  fgetline(FILE *fp, char *buffer, int maxlen)
{
	int  i;

	memset(buffer, 0x00, maxlen);
	if(fgets(buffer, maxlen, fp) == NULL)
	{
		i = strlen(buffer); 
		if(feof(fp) != 0) 
		{ 
		  if(i == 0) return -1;                 /* �ļ����� */ 
		} 
		else return -2;                         /* ���ִ��� */ 
	}
	
	i = strlen(buffer); 
	if(i > 0 && buffer[i-1] == '\n') 
		buffer[--i] = '\0'; 
	return i;
}

/*************************************************
  Function:       int getItem(FILE *fp,char *section,char *item,char *val)
  Description:    ��fp�ļ����ҵ���Ӧsection�µ�Item������Item���ڵ�����
                  ����ΪItem���ڵ���(���section��Item);
				  ����Ϊsection���ڵ���(����section��������item);
				  0Ϊ�����sectionҲ�����ڸ�Item
  Calls:          fgetline(fp,buf,HV_INI_MAX_LEN)
  Called By:      1. hv_readIni()
                  2. hv_writeIni()
  Table Accessed: ��
  Table Updated:  ��
  Input:    
  Output:   ��
  Return:   ����Ҫ�ҵ�section��������item���ظ���section��������
  ����������������Ҫ�ҵ�section��item������HV_INI_NO_EXIST
  �����������ɹ�����item������.
  Others:   
*************************************************/
int getItem(FILE *fp,char *section,char *item,char *val)
{
	int n;
	int valP;                /*���ڼ�¼��item��ɨ��ʱ��λ��*/
	int isItem;
	int itemLength;
	int line_no;
	int section_line ;
	char buf[HV_INI_MAX_LEN];

	line_no = 0;
	section_line = line_no;
	itemLength = strlen(item);

    /*��һ��ɨ�裬Ѱ��section*/
	while(1)
	{
		n=fgetline(fp,buf,HV_INI_MAX_LEN);
		if(n<0)
		{
			return HV_INI_NO_EXIST;
		}

		line_no++;

		/*�������Ϊ�ջ�����;��ͷ������*/
		if(n == 0 || buf[0] == ';')
		{
			continue;
		}
        /*���в�����[����{��ͷ�ͷ�]����}��β�ģ����ǲ���3���ģ�����*/
		if(n<3 || ( (buf[0]=='[' && buf[n-1] != ']') || 
			(buf[0]=='{' && buf[n-1] != '}') )  )
		{
			continue;
		}

		if(buf[0] == '[' || buf[0] == '{')
		{
			buf[n-1] = 0x00; 
			if(strcmp(buf+1, section) == 0) 
                break;
		}
	}

	/*�ҵ�Item*/
	section_line = line_no;

	while(1)
	{
		n = 0;
		isItem = 0;
		n = fgetline(fp,buf,HV_INI_MAX_LEN);

		if(n<0)
		{
			return -section_line;   /*������Item*/
		}

		line_no++;
		
		if(n == 0 || buf[0] == ';')/*���л���ע��*/
		{
			continue;
		}

		if(n>2 && (buf[0] == '[' || buf[0] == '{'))/*�Ѿ�����һ��section��*/
		{
			return -section_line;
		}

		if(n>1)
		{
			valP=0;
			while(valP<strlen(buf))
			{
				if(buf[valP]==' ' || buf[valP] == '\t')
				{
					valP++;
					continue;
				}
                break;
			}
			isItem = memcmp(buf+valP,item,itemLength);
			valP += itemLength;

			if(isItem == 0)
			{
				while(valP<strlen(buf))
				{
					if(buf[valP]==' ' || buf[valP] == '\t')
					{
						valP++;
						continue;
					}
					break;
				}

				if(buf[valP] == '=')
				{
					valP++;
					while(valP<strlen(buf))
					{
						if(buf[valP]==' ' || buf[valP] == '\t')
						{
							valP++;
							continue;
						}
						break;
					}

					memcpy(val,buf+valP,n-valP);
					val[n-valP]='\0';
					return line_no;
				}

				return -section_line;
			}
			continue;
		}
	}
    val[0] = '\0';
	return HV_INI_NO_EXIST;
}

/*************************************************
  Function:       int  writeLine(char *filename,char *buf,int line_no)
  Description:    ���ļ�filename�еĵ�line_no�в���buf.
  Calls:          fgetline(fp,val,HV_INI_MAX_LEN)
  Called By:      hv_writeIni
  Table Accessed: ��
  Table Updated:  ��
  Input:    1 filename,������ļ�����
            2 buf,��������ݡ�
			3 line_no�������������
  Output:   ��
  Return:   
  Others:   
*************************************************/
int  writeLine(char *filename,char *buf,int line_no)
{
	FILE *fp;
	FILE *tempfp;
	char val[HV_INI_MAX_LEN];
	char tempfile[HV_FILE_NAME_LEN];
	int n;
	int line;

	line = 0;

	memcpy(tempfile,filename,strlen(filename));
	sprintf(tempfile,"%s.%d.temp",filename,getpid);
	memcpy(tempfile+strlen(filename),".temp",5);

	tempfp = fopen(tempfile,"w+");
	fp = fopen(filename,"r");
	while(1)
	{
		n = fgetline(fp,val,HV_INI_MAX_LEN);
		if(n<0)
		{
			break;
		}

		line++;
		if(line_no > 0 && line==line_no )/*line_noΪ���������Ҿ��ǵ�ǰ�е�ʱ���޸�*/
		{
			snprintf(val,HV_INI_MAX_LEN,"%s",buf);
		}

		if(line_no < 0 && line == -line_no )/*line_noΪ��������������Ե���lineʱ���ں��油��*/
		{
			fprintf(tempfp,"%s\n",val);
			memcpy(val,buf,strlen(buf));
		}
		fprintf(tempfp,"%s\n",val);
	}
	fclose(fp);
	if(line_no == 0)
	{
		fprintf(tempfp,"%s\n",buf);
	}
	fclose(tempfp);

	/*��д��ȥ*/
	fp = fopen(filename,"w");
	tempfp = fopen(tempfile,"r");
	while(1)
	{
		n = fgetline(tempfp,val,HV_INI_MAX_LEN);
		if(n>=0)
		{
			fprintf(fp,"%s\n",val);
		}
		else
		{
			break;
		} 
	}
	fclose(tempfp);
	fclose(fp);

	/*ɾ����ʱ�ļ�*/
	remove(tempfile);
	return 0;
}
