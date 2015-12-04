/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvini.c
  Author:    韦昭   Version:   V1.0     Date:  2004-8-27
  Description:    大额支付ini文件处理程序
  Others: 
  Function List:  (函数定义，实现在hvini.c文件中)
	1. int hv_readIni(char *filename,char *section,char *item,char *value)
		读取ini文件中某一项的值，filename为文件名，section为节名,item为项名,
		value为读取得到的值（最长255个字符）.
		成功返回0，失败返回-1
	2．int hv_writeIni(char *filename,char *section,char *item,char *value)
		设置ini文件中某一项的值，filename为文件名，section为节名，item为项名,
		value为要设置的值
		成功返回0，失败返回-1
	
	内部函数，只供内部调用
	1.  int  fgetline(FILE *fp, char *buffer, int maxlen)
	    读取文件中的一行写入buffer中去，最大取maxlen个字节。
    2.  int  getItem(FILE *fp,char *section,char *item,char *val)
	    获取section下item的值，放入val中，返回值请看函数说明
    3.  int  splitItem(char *str)
	    得到一个item的值，去掉前面的空格等。
    4.  int  writeLine(char *filename,char *buf,int line_no)
	    在filename中的第line_no处修改或插入buf中的数据
		line_no为0时，表示在文件最后插入。
		line_no为正数时表示在原行修改；
		line_no为负数时表示在下一行插入。
  History:   
    1. Date:     2004-8-27
       Author:   韦昭
       Modification:　建立
*************************************************/
#include "hvini.h"

/*内部函数定义区*/
int  fgetline(FILE *fp, char *buffer, int maxlen);
int  getItem(FILE *fp,char *section,char *item,char *val);
int  splitItem(char *str);
int  writeLine(char *filename,char *buf,int line_no);

/*************************************************
  Function:       int hv_readIni(char *filename,char *section,char *item,char *value)
  Description:    读取ini文件中某一项的值，filename为文件名，section为节名,item为项名,
		          value为读取得到的值（最长255个字符）.
  Calls:          getItem()                  
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 char *filename为文件名
            2 char *section为节名
			3 char *item为项名
  Output:   value值，就是item的内容
  Return:   成功返回0，失败返回－1
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
  Description:    设置ini文件中某一项的值，filename为文件名，section为节名，item为项名,
		          value为要设置的值
  Calls:          1 getItem(fp,section,item,val)
                  2 writeLine(filename,buf,n)
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    1 char *filename为文件名;
            2 char *section为节名;
			3 char *item为项名.
  Output:   无
  Return:   成功返回0，失败返回-1
  Others:   
*************************************************/
int hv_writeIni(char *filename,char *section,char *item,char *value)
{
	FILE *fp;
	char val[HV_INI_MAX_LEN];
	char buf[2*HV_INI_MAX_LEN];      /*当section和item都不存在时，buf为两行*/
	int n;
	
	/*变量的初始化*/
	n = 0;

	/*查找要插入的位置*/
	if((fp = fopen(filename,"r"))==NULL)
	{
		printf("Cannot read file:%s\n",filename);
		return  INI_OP_FAIL;
	}
	n = getItem(fp,section,item,val);
	fclose(fp);

    /*插入内容*/
	if( n != 0 )
	{   /*修改或是在确定行数中插入时*/
		snprintf(buf,HV_INI_MAX_LEN,"%s=%s",item,value);
	}
	else
	{   /*在文件最后插入时*/
		snprintf(buf,2*HV_INI_MAX_LEN,"[%s]\n%s=%s",section,item,value);
	}
	writeLine(filename,buf,n);
	
	return INI_OP_SUCCEE;
}
/*-----------------------------------------------*/
/*            以下为内部函数                     */
/*-----------------------------------------------*/

/*************************************************
  Function:       int  fgetline(FILE *fp, char *buffer, int maxlen);
  Description:    设置ini文件中的一行。放入buffer中，返回读取的字数
  Calls:          
  Called By:      
  Table Accessed: 无
  Table Updated:  无
  Input:    
  Output:   无
  Return:   文件结束返回-1,出现错误返回-2，成功返回读到的字数.
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
		  if(i == 0) return -1;                 /* 文件结束 */ 
		} 
		else return -2;                         /* 出现错误 */ 
	}
	
	i = strlen(buffer); 
	if(i > 0 && buffer[i-1] == '\n') 
		buffer[--i] = '\0'; 
	return i;
}

/*************************************************
  Function:       int getItem(FILE *fp,char *section,char *item,char *val)
  Description:    从fp文件中找到对应section下的Item，返回Item所在的行数
                  正数为Item所在的行(存该section和Item);
				  负数为section所在的行(存在section但不存在item);
				  0为不存该section也不存在该Item
  Calls:          fgetline(fp,buf,HV_INI_MAX_LEN)
  Called By:      1. hv_readIni()
                  2. hv_writeIni()
  Table Accessed: 无
  Table Updated:  无
  Input:    
  Output:   无
  Return:   存在要找的section，不存在item返回负的section的行数；
  　　　　　不存在要找的section和item，返回HV_INI_NO_EXIST
  　　　　　成功返回item的行数.
  Others:   
*************************************************/
int getItem(FILE *fp,char *section,char *item,char *val)
{
	int n;
	int valP;                /*用于记录在item行扫描时的位置*/
	int isItem;
	int itemLength;
	int line_no;
	int section_line ;
	char buf[HV_INI_MAX_LEN];

	line_no = 0;
	section_line = line_no;
	itemLength = strlen(item);

    /*第一次扫描，寻找section*/
	while(1)
	{
		n=fgetline(fp,buf,HV_INI_MAX_LEN);
		if(n<0)
		{
			return HV_INI_NO_EXIST;
		}

		line_no++;

		/*如果本行为空或是以;开头，跳过*/
		if(n == 0 || buf[0] == ';')
		{
			continue;
		}
        /*以行不是以[或是{开头和发]或是}结尾的，或是不到3个的，跳过*/
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

	/*找到Item*/
	section_line = line_no;

	while(1)
	{
		n = 0;
		isItem = 0;
		n = fgetline(fp,buf,HV_INI_MAX_LEN);

		if(n<0)
		{
			return -section_line;   /*不存在Item*/
		}

		line_no++;
		
		if(n == 0 || buf[0] == ';')/*空行或是注释*/
		{
			continue;
		}

		if(n>2 && (buf[0] == '[' || buf[0] == '{'))/*已经到下一个section了*/
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
  Description:    在文件filename中的第line_no行插入buf.
  Calls:          fgetline(fp,val,HV_INI_MAX_LEN)
  Called By:      hv_writeIni
  Table Accessed: 无
  Table Updated:  无
  Input:    1 filename,插入的文件名。
            2 buf,插入的内容。
			3 line_no，插入的行数。
  Output:   无
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
		if(line_no > 0 && line==line_no )/*line_no为正数，而且就是当前行的时候，修改*/
		{
			snprintf(val,HV_INI_MAX_LEN,"%s",buf);
		}

		if(line_no < 0 && line == -line_no )/*line_no为负数，而且其绝对等于line时，在后面补上*/
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

	/*再写进去*/
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

	/*删除临时文件*/
	remove(tempfile);
	return 0;
}
