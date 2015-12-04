/*************************************************
  Copyright (C), 2004, DHC
  File name:     tag.c
  Author:    韦昭   Version:   V1.0     Date:  2004/8/20
  Description:   
  Others: 
  Function List:
  History:   
    1. Date:     2004-8-20
       Author:   韦昭
       Modification:　建立
*************************************************/
#define TAGINFO_FILE_NAME "tin_2"

#include <stdio.h>
#include <stdlib.h>
#include "hv_tag_2.h"
#include "find_debug.h"

int isTag(char *tagName);
int isLength(char *tagLength);
void print2(char *str,int strlen);

int main(int argc, char * argv[])
{
	int length = 10;
	char str[12];
	MEMSET_DEBUG(str,'\0',12);
	setTag(str,"A00",34);
	printf("%s\n",str);
	sprintf(str,"%04d",987);
	printf("%s\n",str);
	printf("%s\n",str+4);
	/*
	char str[sizeof(TTAG)];
	MEMSET_DEBUG(str,'\0',sizeof(str));
	setTag(str,"001",9);
	print2(str,sizeof(str));
	
	printf("%s,%d|[%d][%d][%d]\n[%d][%d][%d]\n",
			__FILE__,__LINE__,tagcmp("ddc","ddd"),tagcmp("ddc","ddc"),tagcmp("ddc","ddb"),
			tagcmp("ffffff","ffffd"),tagcmp("f","fd"),tagcmp("f",""));
	*/
	/*
	 TTAGINFO ti;
	 char tagName[4];
	 char tagLength[7];

	 MEMSET_DEBUG(tagName,'\0',4);
	 MEMSET_DEBUG(tagLength,'\0',7);

     printf("tag:Program for taginfo\n");
     printf("Usage:\n");
	 printf("\ttag : print out the taginfo by lines.\n");
	 printf("\ttag -mw :批量增加,输入格式为：tagName tagDataLentgh+回车.\n");
	 printf("\ttag -add/-a tagName tagDataLentgh :Add a tag info.\n");
	 printf("\ttag -del/-d tagName :Del a tag info.\n");
	 printf("\ttag -get/-g tagName :get a tag info. \n");

	 if(argc == 1)
	 {
		 tag_load(&ti,TAGINFO_FILE_NAME);
		 tag_show(&ti);
	     
	 }
	 if( strcmp(argv[1],"-mw") == 0  )
	 {
		 printf("批量输入\n");
		 
		 while(1)
		 {
			 tag_load(&ti,TAGINFO_FILE_NAME);
			 MEMSET_DEBUG(tagName,'\0',4);
			 MEMSET_DEBUG(tagLength,'\0',7);
			 scanf("%s",tagName);
			 if (strcmp(tagName,"q") == 0)
			 {
				 break;
			 }
			 scanf("%s",tagLength);
			 if (strcmp(tagLength,"q") == 0)
			 {
				 break;
			 }

			 if (isTag(tagName)==0)
			 {
				 printf("tagName不合法。\n");
				 continue;
			 }

			 if (isLength(tagLength)==0)
			 {
				 printf("tagLength不合法。\n");
				 continue;
			 }

			 tag_add(&ti,tagName,atoi(tagLength));
			 tag_write(&ti,TAGINFO_FILE_NAME);
		 }
		 
		 return 0;
	 }

	 if( (strcmp(argv[1],"-a") == 0 ) || (strcmp(argv[1],"-add") == 0) )
	 {
	     printf("Now add a tag info :Name :%s\tLength :%s \n",argv[2],argv[3]);
		 tag_load(&ti,TAGINFO_FILE_NAME);
		 tag_add(&ti,argv[2],atoi(argv[3]));
		 tag_write(&ti,TAGINFO_FILE_NAME);
		 return 0;
	 }

	 if ((strcmp(argv[1],"-d") == 0 ) || (strcmp(argv[1],"-del") == 0))
	 {
	     printf("Now delete a tag info :Name : %s \n",argv[2]);
		 tag_load(&ti,TAGINFO_FILE_NAME);
		 tag_del(&ti,argv[2]);
		 tag_write(&ti,TAGINFO_FILE_NAME);
		 return 0;
	 }
	 
	if((strcmp(argv[1],"-g") == 0 ) || (strcmp(argv[1],"-get") == 0))
	 {
	     printf("Now get a tag info %s : Length ",argv[2]);
		 tag_load(&ti,TAGINFO_FILE_NAME);
		 printf("%d\n",tag_get(&ti,argv[2]));
		 return 0;
	 }*/
	 	 
     return 0;
}

int isTag(char *tagName)
{
	int i;
	/*不用检查tagName的长度，使用时一律只取3位。*/
	if(strlen(tagName)<3)
	{
		return 0;
	}

	for(i=0; i<3 ; i++)
	{
		if(!isalnum(tagName[i]))
		{
			return 0;
		}
	}
	tagName[3]='\0';

	return 1;
}

int isLength(char *tagLength)
{
	int i;
	if (strlen(tagLength)>6)
	{
		tagLength[6] = '\0';
	}
	if ( atoi(tagLength) == 0 )
	{
		return 0;
	}

	return 1;
}

void print2(char *str,int strlen)
{
	int i;
	printf("FILE:%s,LINE:%d|show str ,strlen = [%d]\n",__FILE__,__LINE__,strlen);
	for(i=0;i<strlen;i++)
		printf("%2x ",(int)(str[i]));
	printf("\n");
	for(i=0;i<strlen;i++)
		printf(" %c ",str[i]);
	printf("\n");
}
