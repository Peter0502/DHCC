/***************************************************************
* 文 件 名:     pubf_base_debug.c
* 功能描述：    
*
*			  1  SETMEM( ) 
       
* 作    者:               
* 完成日期：    2003年12月16日
*
* 修改记录：    
* 1. 日    期: 	2003/12/30
*    修 改 人: 	rob	/ lance
*    修改内容: 	补充 5 - 11
*
**************************************************************/
#include <stdio.h>
#include "public.h"
#include "svrpub_c.h"
#include <stdarg.h>


#define LV_DEBUG_LOG "/home/newsys/usr/lhf/lv_debuglog"
static char cLogfile_debug[512];
extern void pub_base_get_date(int *year, int *month, int *day);
/**********************************************************************
* 函数名：    SETMEM()
* 函数功能：  监控针对使用memset造成8583全局变量pub_val内存溢出
* 作者/时间：    2003年12月16日
* 
* 参数：
* 修改历史：
*
********************************************************************/
int lv_fprintf_debug(char *filename, char *buf)
{
    FILE           *fp;
    const  long  max_file_len = 32*1024*1024;
    char   tmpbuf[255];
    char   strtime[255];
    char bkfnm[255];


    if(get_file_size(filename) - max_file_len > 0.001){
        sprintf(bkfnm, "%s%.12s.bk", filename, logGetTime(NULL, 0, strtime));
        rename(filename, bkfnm);
    }
    fp = fopen(filename, "a");
    if (fp == NULL) {
        return -1;
    }
    fprintf(fp, buf);
    fclose(fp);
}
void lv_log_debug(char *filename, char *fmt,...)
{
    va_list         argptr;
    int             cnt;
    char            buf[1024 * 1024];

    memset(buf, 0x0, sizeof(buf));
    va_start(argptr, fmt);

    cnt = vsprintf(buf, fmt, argptr);
    va_end(argptr);
    strcat(buf, "\n");

    lv_fprintf_debug(filename, buf);
}

int SETMEM(void * vBuf, int iSet, size_t iSize,int fileline,char * filename)
{
	char cTxday[9];
	int iYear=0,iMonth=0,iDay=0;
	int iHour=0,iMin=0,iSec=0;
	memset(cTxday, 0 , sizeof(cTxday));
	memset(cLogfile_debug, 0 , sizeof(cLogfile_debug));
	pub_base_get_date(&iYear, &iMonth, &iDay);
	sprintf(cTxday,"%04d%02d%02d",iYear,iMonth,iDay);
	sprintf(cLogfile_debug,"%s%s.txt",LV_DEBUG_LOG,cTxday);
	pub_base_GetTime2(&iHour, &iMin, &iSec);
	
	if(vBuf >=Pu_Val && vBuf <=(char*)Pu_Val+sizeof(struct PVal_St) *PuVal_num)
	{
		lv_log_debug(cLogfile_debug,"[%02d%02d%02d]发现在程序%s第%d行使用MEMSET函数首地址在PU_BAL变量地址范围内,请查证\n",iHour,iMin,iSec,filename,fileline);
		lv_log_debug(cLogfile_debug,"PU_BAL首地址为%ld,初始化变量的首地址为%ld\n\n",Pu_Val,vBuf);
	}
	if(vBuf+iSize >=Pu_Val && vBuf+iSize <=(char*)Pu_Val+sizeof(struct PVal_St) *PuVal_num)
	{
		lv_log_debug(cLogfile_debug,"[%02d%02d%02d]发现在程序%s第%d行使用MEMSET函数尾地址在PU_BAL变量地址范围内,请查证\n",iHour,iMin,iSec,filename,fileline);
		lv_log_debug(cLogfile_debug,"PU_BAL尾地址为%ld,初始化变量的尾地址为%ld\n\n",(char *)Pu_Val+sizeof(struct PVal_St) *PuVal_num,vBuf+iSize);
	}
	memset(vBuf,iSet,iSize);
	return 0;
}

int CPYMEM(void * vBuf1, const void * vBuf2, size_t iSize,int fileline,char * filename)
{
	char cTxday[9];
	int iYear=0,iMonth=0,iDay=0;
	int iHour=0,iMin=0,iSec=0;
	memset(cTxday, 0 , sizeof(cTxday));
	memset(cLogfile_debug, 0 , sizeof(cLogfile_debug));
	pub_base_get_date(&iYear, &iMonth, &iDay);
	sprintf(cTxday,"%04d%02d%02d",iYear,iMonth,iDay);
	sprintf(cLogfile_debug,"%s%s.txt",LV_DEBUG_LOG,cTxday);
	pub_base_GetTime2(&iHour, &iMin, &iSec);
	if(vBuf1 >=Pu_Val && vBuf1 <=(char *)Pu_Val+sizeof(struct PVal_St) *PuVal_num)
	{
		lv_log_debug(cLogfile_debug,"[%02d%02d%02d]发现在程序%s第%d行使用MEMCPY函数首地址在PU_BAL变量地址范围内,请查证\n",iHour,iMin,iSec,filename,fileline);
		lv_log_debug(cLogfile_debug,"PU_BAL首地址为%ld,初始化变量的首地址为%ld\n\n",Pu_Val,vBuf1);
	}
	if(vBuf1+iSize >=Pu_Val && vBuf1+iSize <=(char *)Pu_Val+sizeof(struct PVal_St) *PuVal_num)
	{
		lv_log_debug(cLogfile_debug,"[%02d%02d%02d]发现在程序%s第%d行使用MEMCPY函数尾地址在PU_BAL变量地址范围内,请查证\n",iHour,iMin,iSec,filename,fileline);
		lv_log_debug(cLogfile_debug,"PU_BAL尾地址为%ld,初始化变量的尾地址为%ld\n\n",(char *)Pu_Val+sizeof(struct PVal_St) *PuVal_num,vBuf1+iSize);
	}
	memcpy(vBuf1,vBuf2,iSize);
	return 0;
}
