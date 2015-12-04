/*************************************************
  Copyright (C), 2004, DHC
  File name:     hvini.h
  Author:    韦昭   Version:   V1.0     Date:  2004-8-26
  Description:    大额支付ini文件处理程序
  Others: 
  Function List:  (函数定义，实现在hvini.c文件中)
	1. int hv_readIni(char *filename,char *section,char *item,char *value)
		读取ini文件中某一项的值，filename为文件名，section为节名,item为项名,
		value为读取得到的值（最长255个字符）.
		成功返回0，失败返回－1
	2．int hv_writeIni(char *filename,char *section,char *item,char *value)
		设置ini文件中某一项的值，filename为文件名，section为节名，item为项名,
		value为要设置的值
		成功返回0，失败返回－1
  History:   
    1. Date:         2004-8-26
       Author:       韦昭
       Modification: 建立
*************************************************/
#ifndef  _HV_INI_
#define  _HV_INI_
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

/*宏定义*/
#define  INI_OP_SUCCEE     0      /*操作成功*/
#define  INI_OP_FAIL       -1     /*操作失败*/
#define  HV_INI_MAX_LEN    1024   /*一行的最大长度*/
#define  HV_INI_NO_EXIST   0      /*不存在要找的section和item*/
#define  HV_FILE_NAME_LEN  50     /*文件名的最大长度*/

/*外部调用函数*/
int hv_readIni(char *filename,char *section,char *item,char *value);
int hv_writeIni(char *filename,char *section,char *item,char *value);
#endif

