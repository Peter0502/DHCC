/************************************************
  Copyright (C),2005,DHCC
  File name:	lv_tag.h
  Author:	韦昭	Version:	V1.0	Date:	2005/7/22
  Description:	小额支付tag表维护函数,与大额有些改进
  Others:	由于大额运行稳定，所以并不用这个来代替大额
  Function List:
	(外部调用函数)
	int  lv_tag_load(TLVTAGINFO *lvti,char *fileName);
	装载文件中的tag数据表，成功返回0，失败返回－1。
    int  lv_tag_write(TLVTAGINFO *lvti,char *fileName);
    将ti中的数据写入到文件里去。成功返回0，失败返回－1。
    void lv_tag_show(TLVTAGINFO *ti);
    列出tag表里的各个项。
    int  lv_tag_add(TLVTAGINFO *ti,char *tagName,int length);
    增加一个tag信息，如果存在了，修改，不存在，按从小到大插入。
    成功返回0，失败返回－1
    int  lv_tag_del(TLVTAGINFO *ti,char *tagName);
    删除一个Tag,成功返回0，失败返回－1.
    int  lv_tag_get(TLVTAGINFO *ti,char *tagname);
    返回一个tag的内容长度。没有返回0

	内部函数，只供内部调用
  History:
    1. Date:         2004-8-25
       Author:       韦昭
       Modification: 建立
***********************************************************/
#ifndef  _LV_TAG_
#define  _LV_TAG_
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#define LV_TAG_NAME_LENGTH 3				
#define LV_TAG_LENGTH	LV_TAG_NAME_LENGTH+2+6
#define LV_TAG_INFO_MAX_LENGTH 500*LV_TAG_LENGTH /*小额文档中有236个，大额文档中有273个
	现设定最高为500个*/
#define LV_TAG_OP_SUCCEE 0      /*操作成功返回值*/
#define LV_TAG_OP_FAIL -1       /*操作失败返回值*/

/*tag表结构，用于记录整个表的信息*/
typedef struct _t_lv_taginfo
{
    int  length;
    char data[LV_TAG_INFO_MAX_LENGTH];
}TLVTAGINFO;

/*tag表内容，用于记录一个tag的信息*/
typedef struct _t_lv_tag
{
    char tagName[LV_TAG_NAME_LENGTH];
    int  length;
}TLVTAG;

int  lv_tag_load(TLVTAGINFO *lvti,char *fileName);
int  lv_tag_write(TLVTAGINFO *lvti,char *fileName);
void lv_tag_show(TLVTAGINFO *ti);
int  lv_tag_add(TLVTAGINFO *ti,char *tagName,int length);
int  lv_tag_del(TLVTAGINFO *ti,char *tagName);
int  lv_tag_get(TLVTAGINFO *ti,char *tagname);

#endif

  
  

