/*************************************************
  Copyright (C), 2004, DHC
  File name:     hv_tag.h
  Author:    Τ��   Version:   V1.0     Date:  2004/8/25
  Description:    ���֧��tag��ά������������
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
	����һ��tag�����ݳ��ȡ�û�з���0
	
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
    1. Date:         2004-8-25
       Author:       Τ��
       Modification: ����
*************************************************/
#ifndef  _HV_TAG_
#define  _HV_TAG_
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#define TAG_INFO_MAX_LENGTH  20*20*20*11  /**�ļ�����󳤶�*/
#define TAG_NAME_LENGTH 3    /*tag���ֳ���*/
#define TAG_LENGTH_LENGTH 6  /*��¼���е�λ��*/
#define TAG_LENGTH 11        /*һ��tag��Ϣ����λ��λ��*/
#define TAG_OP_SUCCEE 0      /*�����ɹ�����ֵ*/
#define TAG_OP_FAIL -1       /*����ʧ�ܷ���ֵ*/

/*tag��ṹ�����ڼ�¼���������Ϣ*/
typedef struct _t_taginfo
{
	int  length;
	char data[TAG_INFO_MAX_LENGTH];
}TTAGINFO;
/*tag�����ݣ����ڼ�¼һ��tag����Ϣ*/
typedef struct _t_tag
{
	int length;
	char tagName[TAG_NAME_LENGTH];
}TTAG;

/**����������ÿ�������ľ����÷��ڱ��ļ�Function List�����ҡ�*/
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

