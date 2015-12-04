/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvini.h
  Author:    Τ��   Version:   V1.0     Date:  2004-8-26
  Description:    ���֧��ini�ļ��������
  Others: 
  Function List:  (�������壬ʵ����hvini.c�ļ���)
	1. int hv_readIni(char *filename,char *section,char *item,char *value)
		��ȡini�ļ���ĳһ���ֵ��filenameΪ�ļ�����sectionΪ����,itemΪ����,
		valueΪ��ȡ�õ���ֵ���255���ַ���.
		�ɹ�����0��ʧ�ܷ��أ�1
	2��int hv_writeIni(char *filename,char *section,char *item,char *value)
		����ini�ļ���ĳһ���ֵ��filenameΪ�ļ�����sectionΪ������itemΪ����,
		valueΪҪ���õ�ֵ
		�ɹ�����0��ʧ�ܷ��أ�1
  History:   
    1. Date:         2004-8-26
       Author:       Τ��
       Modification: ����
*************************************************/
#ifndef  _HV_INI_
#define  _HV_INI_
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

/*�궨��*/
#define  INI_OP_SUCCEE     0      /*�����ɹ�*/
#define  INI_OP_FAIL       -1     /*����ʧ��*/
#define  HV_INI_MAX_LEN    1024   /*һ�е���󳤶�*/
#define  HV_INI_NO_EXIST   0      /*������Ҫ�ҵ�section��item*/
#define  HV_FILE_NAME_LEN  50     /*�ļ�������󳤶�*/

/*�ⲿ���ú���*/
int hv_readIni(char *filename,char *section,char *item,char *value);
int hv_writeIni(char *filename,char *section,char *item,char *value);
#endif

