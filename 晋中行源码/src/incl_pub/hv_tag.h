/*************************************************
  Copyright (C), 2004, DHC
  File name:     hv_tag.h
  Author:    韦昭   Version:   V1.0     Date:  2004/8/25
  Description:    大额支付tag表维护函数声明：
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
	返回一个tag的内容长度。没有返回0
	
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
    1. Date:         2004-8-25
       Author:       韦昭
       Modification: 建立
*************************************************/
#ifndef  _HV_TAG_
#define  _HV_TAG_
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#define TAG_INFO_MAX_LENGTH  20*20*20*11  /**文件的最大长度*/
#define TAG_NAME_LENGTH 3    /*tag名字长度*/
#define TAG_LENGTH_LENGTH 6  /*记录域有的位数*/
#define TAG_LENGTH 11        /*一个tag信息数据位的位数*/
#define TAG_OP_SUCCEE 0      /*操作成功返回值*/
#define TAG_OP_FAIL -1       /*操作失败返回值*/

/*tag表结构，用于记录整个表的信息*/
typedef struct _t_taginfo
{
	int  length;
	char data[TAG_INFO_MAX_LENGTH];
}TTAGINFO;
/*tag表内容，用于记录一个tag的信息*/
typedef struct _t_tag
{
	int length;
	char tagName[TAG_NAME_LENGTH];
}TTAG;

/**函数声明，每个函数的具体用法在本文件Function List部分找。*/
int    tag_load(TTAGINFO *ti,char *fileName);
int    tag_write(TTAGINFO *ti,char *fileName);
void   tag_show(TTAGINFO *ti);
int    tag_add(TTAGINFO *ti,char *tagName,int length);
int    tag_del(TTAGINFO *ti,char *tagName);
int    tag_get(TTAGINFO *ti,char *tagname);

int    getTag(char *strp,TTAG *tag);
char *searchTag(char *str,char *tagName);
int    tagcmp(char *tag1,char *tag2);
void setTag(char *str,char *tagName,int tagLength);
void num2str(int num,char *str,int length);
int isTag(char *tagName);

#endif

