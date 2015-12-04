/***
**     	Author     	Flyger Zhuang
**	Copyright	Union Tech. BeiJing
**	Date	   	2003.12.02
**	E-mail		flyger@163.net
***/

#include "HsmLog.h"

/***
	function :	HsmGetTme()
	input	 :	pcFmtBuf
			iMsgLen
	output	 :	pcMsgBuf	
***/
void HsmGetTme(char *pcMsgBuf,int iMsgLen,const char *pcFmtBuf)
{
	time_t        clock;

	clock = time((time_t *)0);
	strftime(pcMsgBuf,iMsgLen,pcFmtBuf,localtime(&clock));
}

/***
	function :	HsmOpnLog()
***/
FILE *HsmOpnLog(char *pcFileName)
{
    FILE 	*pfHsmLog;

    if ((pfHsmLog = fopen(pcFileName,"a")) == NULL)
	return(stderr);
		
    if (ftell(pfHsmLog) >= HSMLOGFILEMAXSIZE) {
	fclose(pfHsmLog);
	if ((pfHsmLog = fopen(pcFileName,"w")) == NULL)
	   return(stderr);
    }

    return(pfHsmLog);
}

/***
	function :	HsmClsLog()
***/
int HsmClsLog(FILE *pfHsmLog)
{
    if ((pfHsmLog == stderr) || (pfHsmLog = stdout))
	return(0);
    return(fclose(pfHsmLog));
}


/***
	function :	HsmErrLog()
***/
void HsmErrLog(va_alist)
va_dcl
{
    va_list 	args;
    char	*pcFmtBuf;
    char	pcMsgBuf[25],pcFileName[512];
    FILE	*pfHsmLog;

    memset(pcFileName,0x00,sizeof(pcFileName));
    
    if (getenv("HSM_TRACE_FILE") == NULL) {
    	memcpy(pcFileName,"./unionhsm.log",14);
    } else sprintf(pcFileName,"%s",getenv("HSM_TRACE_FILE"));
       
    pfHsmLog = HsmOpnLog(pcFileName);
	
    memset(pcMsgBuf,0x00,sizeof(pcMsgBuf));
    HsmGetTme(pcMsgBuf,sizeof(pcMsgBuf),"%Y-%m-%d %H:%M:%S");
    
    fprintf(pfHsmLog,"\n");
    fprintf(pfHsmLog,"\n");
    fprintf(pfHsmLog,"----------------------------------------------------\n");
    fprintf(pfHsmLog,"----------------------------------------------------\n");
    fprintf(pfHsmLog,"%10s [%d]\n",pcMsgBuf,getpid());
    
    
    va_start(args);
    pcFmtBuf = va_arg(args,char *);
    vfprintf(pfHsmLog,pcFmtBuf,args);
    va_end(args);
    
/**
    fprintf(pfHsmLog," [ERROR CODE = %d]",errno);	
**/
    fprintf(pfHsmLog,"\n");
    fflush(pfHsmLog);	
    
    fclose(pfHsmLog);
}

/***
	function :	HsmSucLog()
***/
void HsmSucLog(va_alist)
va_dcl
{
    va_list args;
    char	*pcFmtBuf;
    char	pcMsgBuf[15],pcFileName[512];
    FILE	*pfHsmLog;
    
    memset(pcFileName,0x00,sizeof(pcFileName));
    
    if (getenv("HSM_TRACE_FILE") == NULL) {
    	memcpy(pcFileName,"./unionhsm.log",14);
    } else sprintf(pcFileName,"%s",getenv("HSM_TRACE_FILE"));
       
    pfHsmLog = HsmOpnLog(pcFileName);
    
    memset(pcMsgBuf,0,sizeof(pcMsgBuf));
    HsmGetTme(pcMsgBuf,sizeof(pcMsgBuf),"%Y%m%d%H%M%S");
    
    fprintf(pfHsmLog,"%10s [%d]::",pcMsgBuf,getpid());
    
    va_start(args);
    pcFmtBuf = va_arg(args,char *);
    vfprintf(pfHsmLog,pcFmtBuf,args);
    va_end(args);
    
    fprintf(pfHsmLog,"\n");
    fflush(pfHsmLog);
    
    fclose(pfHsmLog);
}

/***
	function :	HsmDugLog()
***/
void HsmDugLog(va_alist)
va_dcl
{
    va_list args;
    char	*pcFmtBuf;
    char	pcMsgBuf[25],pcFileName[512];
    FILE	*pfHsmLog;
    
    memset(pcFileName,0x00,sizeof(pcFileName));
    
    if (getenv("HSM_TRACE_FILE") == NULL) {
    	memcpy(pcFileName,"./unionhsm.log",14);
    } else sprintf(pcFileName,"%s",getenv("HSM_TRACE_FILE"));
       
    pfHsmLog = HsmOpnLog(pcFileName);
    
    memset(pcMsgBuf,0,sizeof(pcMsgBuf));
    HsmGetTme(pcMsgBuf,sizeof(pcMsgBuf),"%Y-%m-%d %H:%M:%S");
    
    va_start(args);
    pcFmtBuf = va_arg(args,char *);
    vfprintf(pfHsmLog,pcFmtBuf,args);
    va_end(args);
    
    fprintf(pfHsmLog,"\n");
    fflush(pfHsmLog);
    
    fclose(pfHsmLog);
}

void HsmDugLogTime(va_alist)
va_dcl
{
    va_list args;
    char	*pcFmtBuf;
    char	pcMsgBuf[25],pcFileName[512];
    FILE	*pfHsmLog;
    
    memset(pcFileName,0x00,sizeof(pcFileName));
    
    if (getenv("HSM_TRACE_FILE") == NULL) {
    	memcpy(pcFileName,"./unionhsm.log",14);
    } else sprintf(pcFileName,"%s",getenv("HSM_TRACE_FILE"));
    
    pfHsmLog = HsmOpnLog(pcFileName);
    
    memset(pcMsgBuf,0,sizeof(pcMsgBuf));
    HsmGetTme(pcMsgBuf,sizeof(pcMsgBuf),"%Y-%m-%d %H:%M:%S");
    
    fprintf(pfHsmLog,"\n");
    fprintf(pfHsmLog,"%10s [%d]\n",pcMsgBuf,getpid());
    
    va_start(args);
    pcFmtBuf = va_arg(args,char *);
    vfprintf(pfHsmLog,pcFmtBuf,args);
    va_end(args);
    
    fprintf(pfHsmLog,"\n");
    fflush(pfHsmLog);
    
    fclose(pfHsmLog);
}


/***
	function :	HsmDugLog()
***/
void HsmRcdLog(va_alist)
va_dcl
{
    va_list args;
    char	*pcFmtBuf;
    char	pcMsgBuf[25],pcFileName[512];
    FILE	*pfHsmLog;
    
    memset(pcFileName,0x00,sizeof(pcFileName));
    
    if (getenv("HSM_RECORD_FILE") == NULL) {
    	memcpy(pcFileName,"./unionhsmrecord.log",20);
    } else sprintf(pcFileName,"%s",getenv("HSM_RECORD_FILE"));
           
    pfHsmLog = HsmOpnLog(pcFileName);
    
    memset(pcMsgBuf,0,sizeof(pcMsgBuf));
    HsmGetTme(pcMsgBuf,sizeof(pcMsgBuf),"%Y-%m-%d %H:%M:%S");
    
    va_start(args);
    pcFmtBuf = va_arg(args,char *);
    vfprintf(pfHsmLog,pcFmtBuf,args);
    va_end(args);
    
    fprintf(pfHsmLog,"\n");
    fflush(pfHsmLog);
    
    fclose(pfHsmLog);
}

void HsmRcdLogTime(va_alist)
va_dcl
{
    va_list args;
    char	*pcFmtBuf;
    char	pcMsgBuf[25],pcFileName[512];
    FILE	*pfHsmLog;
    
    memset(pcFileName,0x00,sizeof(pcFileName));
    
    if (getenv("HSM_RECORD_FILE") == NULL) {
    	memcpy(pcFileName,"./unionhsmrecord.log",20);
    } else sprintf(pcFileName,"%s",getenv("HSM_RECORD_FILE"));
    
    pfHsmLog = HsmOpnLog(pcFileName);
    
    memset(pcMsgBuf,0,sizeof(pcMsgBuf));
    HsmGetTme(pcMsgBuf,sizeof(pcMsgBuf),"%Y-%m-%d %H:%M:%S");
    
    fprintf(pfHsmLog,"\n");
    fprintf(pfHsmLog,"%10s [%d]\n",pcMsgBuf,getpid());
    
    va_start(args);
    pcFmtBuf = va_arg(args,char *);
    vfprintf(pfHsmLog,pcFmtBuf,args);
    va_end(args);
    
    fprintf(pfHsmLog,"\n");
    fflush(pfHsmLog);
    
    fclose(pfHsmLog);
}
