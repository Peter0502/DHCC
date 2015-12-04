/***
**     	Author     	Flyger Zhuang
**	Copyright	Union Tech. BeiJing
**	Date	   	2003.12.02
**	E-mail		flyger@163.net
System-define Header File
***/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <varargs.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
/**
#include <stdarg.h>
#include <varargs.h>
**/

#define	HSMLOGFILEMAXSIZE	1000000

void HsmGetTme(char *pcMsgBuf,int iMsgLen,const char *pcFmtBuf);
FILE *HsmOpnLog();
int HsmClsLog(FILE *pfHsmLog);
void HsmErrLog();
void HsmSucLog();
void HsmDugLog();

