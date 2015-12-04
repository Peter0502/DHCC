/************************************************
  Copyright (C),2005,DHCC
  File name:	lv_tag.h
  Author:	Τ��	Version:	V1.0	Date:	2005/7/22
  Description:	С��֧��tag��ά������,������Щ�Ľ�
  Others:	���ڴ�������ȶ������Բ����������������
  Function List:
	(�ⲿ���ú���)
	int  lv_tag_load(TLVTAGINFO *lvti,char *fileName);
	װ���ļ��е�tag���ݱ��ɹ�����0��ʧ�ܷ��أ�1��
    int  lv_tag_write(TLVTAGINFO *lvti,char *fileName);
    ��ti�е�����д�뵽�ļ���ȥ���ɹ�����0��ʧ�ܷ��أ�1��
    void lv_tag_show(TLVTAGINFO *ti);
    �г�tag����ĸ����
    int  lv_tag_add(TLVTAGINFO *ti,char *tagName,int length);
    ����һ��tag��Ϣ����������ˣ��޸ģ������ڣ�����С������롣
    �ɹ�����0��ʧ�ܷ��أ�1
    int  lv_tag_del(TLVTAGINFO *ti,char *tagName);
    ɾ��һ��Tag,�ɹ�����0��ʧ�ܷ��أ�1.
    int  lv_tag_get(TLVTAGINFO *ti,char *tagname);
    ����һ��tag�����ݳ��ȡ�û�з���0

	�ڲ�������ֻ���ڲ�����
  History:
    1. Date:         2004-8-25
       Author:       Τ��
       Modification: ����
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
#define LV_TAG_INFO_MAX_LENGTH 500*LV_TAG_LENGTH /*С���ĵ�����236��������ĵ�����273��
	���趨���Ϊ500��*/
#define LV_TAG_OP_SUCCEE 0      /*�����ɹ�����ֵ*/
#define LV_TAG_OP_FAIL -1       /*����ʧ�ܷ���ֵ*/

/*tag��ṹ�����ڼ�¼���������Ϣ*/
typedef struct _t_lv_taginfo
{
    int  length;
    char data[LV_TAG_INFO_MAX_LENGTH];
}TLVTAGINFO;

/*tag�����ݣ����ڼ�¼һ��tag����Ϣ*/
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

  
  

