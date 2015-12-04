#include "find_debug.h" 
/************************************************************************
 * PROGRAM :  errlog.c                                                  *
 * AUTHOR  :  Tang Yanyun  ������                                       *
 * CREATED DATE :  1997-12-20                                           *
 ************************************************************************/

/*     FUNCTION & SUBROUTINE DESCRIPTION
 *
 *       TYPE      NAME                 DESCRIPTION
 *    --------- ---------- ---------------------------------------- 
 *    int       chgLog        �ı�����¼״̬������¼�ļ���.
 *    int       errLog        ��¼������('c' ����ӿ�).
 *    void      logDispErrMsg ����ѶϢ��ʾ.
 *    void      logMemDump    ��ʾĳһ���ڴ����������.
 *    char *    logGetTime    ��ȷȡ��ʱ�� Mon Wek YYYY-mm-dd HH:MM:SS.MicroS
 *    char *    appGetTime    ��ȷȡ��ʱ�� YYYYmmddHHMMSSMicroS
 *    int       appSleep      ��ȷʱ���sleep
 */

/* ---------------------- INCLUDE FILES DECLARATION -------------------- */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#if defined SCO ||defined AIX
#include <stdarg.h>
#endif

#ifdef WIN32
#include <io.h>
#include <winsock2.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/select.h>
#if !defined  SCO && !defined AIX
#include <pthread.h>
#endif
#endif

#include "errlog.h"                      /* ���峣��, ���Ͻṹ��ȫ�ֱ��� */


/* ---------------------- STATIC GLOBAL DECLARATION -------------------- */
/* int errno; */
static char LOG_FILE_NAME[100]={"trc.log"};    /* �����¼���ļ���       */
static int           LOG_MODE = LOG_DEBUG;     /* ��¼����״̬�ı�־     */
static unsigned int  LOG_MASK = 0xFFFFFFFF;    /* ��¼����״̬�ı�־     */
static int   LOG_MAX_BAK_SIZE = 8192*1024;     /* �����¼�ļ���������� */	

/* -------- CALL FUNCTION & SUBROUTINE PROTOTYPE DECLARATIONS ---------- */
/* defined in errlog.h
int    chgLog(char,char *);
int    errLog();
char  *logGetTime(char *,int,char *);
char  *appGetTime(char *,int,char *);
int    appSleep(int,int);
 */
static void   logDispErrMsg(int,int,char *,char *,long);
static void   logMemDump(int,unsigned char *,long);

/*    for test main *
int main(int argc,char *argv[])
{
  char caStr[60];

  sprintf(caStr,"1234567890");

  errLog(LOG_INFO,0,RPT_TO_TTY,caStr,sizeof(caStr),"%s %d",caStr,10);

  return(0);
}
 */

/*    ROUTINE NAME:chgLog(char cChgCode,char *pcChgStr)
 *    ARGUMENTS:
 *      NAME            TYPE                 DESCRIPTION
 *    --------------- ----------------    -------------------------
 *    cChgCode        char		  �ı����
 *    pcChgStr        char *		  �����¼�ļ����ļ����ֻ�״̬�ִ�
 *
 *    RETURN VALUE(S):
 *      0 : ����
 *     -1 : �ı��ļ�����ʱ������
 *     -2 : �ı�����¼״̬( TEST_MODE )ʱ������
 *     -3 : �ı�������
 *
 *    DESCRIPTION:
 *      �жϸı����:
 *        1. �ı��¼�ļ�����.
 *             ����ļ������Ƿ���ȷ.
 *        2. �ı��¼״̬.
 *             ����¼״̬�����ǲ���ֵ����.
 *        3. ���ı����
 */
int chgLog(char cChgCode,char *pcChgStr)
{
  int iRc;

  switch (cChgCode) {
    case LOG_CHG_LOG:
      iRc = strlen(pcChgStr);
      if ((iRc < 1) || (iRc > 100)) {
          errLog(3,0,RPT_TO_LOG,0,0,"chgLog:Change LOG file name %s error!",pcChgStr);
          return(-1);
      }
      strcpy(LOG_FILE_NAME,pcChgStr);
      break;
    case LOG_CHG_MODE:
/*
 *    switch(pcChgStr[0]) {
 *      case '0':
 *        LOG_MODE = 0;
 *        break;
 *      case '1':
 *        LOG_MODE = 1;
 *        break;
 *      default:
 *        errLog(3,0,RPT_TO_LOG,0,0,"chgLog:Change Mode error!");
 *        return(-2);
 *    }
 */
      LOG_MODE = (int)strtoul(pcChgStr,NULL,0);
      break;
    case LOG_CHG_SIZE:
      iRc = (int)strtoul(pcChgStr,NULL,0);
      LOG_MAX_BAK_SIZE = iRc * 1024;
      break;
    case LOG_CHG_MASK:
      LOG_MASK = (unsigned int)strtoul(pcChgStr,NULL,0);
      break;
    default:
      errLog(3,0,RPT_TO_LOG,0,0,"chgLog: Change Code error!");
      return(-3);
  }
  return(0);
}


/*    ROUTINE NAME:errLog()
 *    ARGUMENTS:
 *      NAME            TYPE                 DESCRIPTION
 *    --------------- ----------------    -------------------------
 *    iErrCode        int                  �������
 *    pcaDispMsg      char *  		   ����ѶϢ�ִ�
 *    cDumpDev        char 	           ��ʾ���豸����
 *    pcaDumpAddr     char *	           ��ʾ���ڴ�����ĵ�ַ
 *    lDumpLen        long 	           ��ʾ���ڴ�����ĳ���
 *
 *    RETURN VALUE(S):
 *       0	: �˺���ִ�гɹ�
 *       1	: �˺���ִ��ʧ��(exit (1))
 *
 *    DESCRIPTION:
 *      1.�ж���ʾ����¼�ļ��Ŀ���λԪ�ǿ���, �����������¼�ļ�.
 *        �����ļ�ʧ��������, ֱ������ 256 ��, ������ʧ��, ����ʾѶϢ����.
 *        �ɹ�����ס�ļ�; �Ƶ��ļ�β.
 *        ����¼״̬���������, Ҫ��Ҫ��ʾ.
 *      2.�ж���ʾ��өĻ�Ŀ���λԪ�ǿ���, ������өĻ.
 *        ����¼״̬���������, Ҫ��Ҫ��ʾ.
 *      3.�ж���ʾ��console�Ŀ���λԪ�ǿ���, ������console.
 *        ����¼״̬���������, Ҫ��Ҫ��ʾ.
 *      4.�ж���ʾ�ڴ�����ĵ�ַ��Ϊ�㼰��ʾ���ȴ����, ����ʾ�ڴ�����.
 */
int errLog(int iErrCode,unsigned int iMask,char cDumpDev,char *pcaDumpAddr,
           long lDumpLen,char *pcFormat,...)
{
  int   iLogFile; 
  int   iFileSize;
  int   iRc;

  char  pcaDispMsg[LOG_MAX_MSG_LEN];

  va_list  argptr;

/*
//int   iErrCode;
  unsigned int iMask;
  char  cDumpDev;
  char *pcaDumpAddr;
  long  lDumpLen;

  char *pcFormat;

  va_start(argptr,iErrCode);
  iErrCode = va_arg(argptr,int);
  iMask    = va_arg(argptr,unsigned int);
  cDumpDev = va_arg(argptr,char);
  pcaDumpAddr = va_arg(argptr,char *);
  lDumpLen = va_arg(argptr,long);

  pcFormat = va_arg(argptr,char *);
  vsnprintf(pcaDispMsg,LOG_MAX_MSG_LEN,pcFormat,argptr);
  va_end(argptr);
*/
#ifndef __DEBUG__
	if(iErrCode!=LOG_ERR)
		/*return(0)*/;
#endif
  va_start(argptr,pcFormat);
  vsnprintf(pcaDispMsg,LOG_MAX_MSG_LEN,pcFormat,argptr);
  va_end(argptr);


  if (iErrCode > LOG_MODE) return(0);
  if ((iMask != 0) && (!(LOG_MASK & iMask))) return(0);

  /* output to logfile */
  if (cDumpDev & RPT_TO_LOG) {
      iRc = 0;
#ifdef WIN32
      while ((iLogFile = open(LOG_FILE_NAME,_O_WRONLY | _O_CREAT,_S_IREAD | _S_IWRITE)) == -1) {
#else
      while ((iLogFile = open(LOG_FILE_NAME, O_WRONLY |  O_CREAT,0777)) == -1) {
#endif
          appSleep(0,100000);
          if ((iRc++) > LOG_MAX_OPEN_RETRY) {
              printf("errLog.c: fatal error occured ! error_no is %d\n",errno);
              printf("errLog: %s CANNOT OPEN FOR ERROR LOG !\n",LOG_FILE_NAME);
              close(iLogFile);
              return(-1);
          }
      }

#ifndef WIN32
      iRc = 0;
      while (lockf(iLogFile,F_LOCK,0) == -1) {
          appSleep(0,100000);
          if ((iRc++) > LOG_MAX_OPEN_RETRY) {
              printf("errLog.c:fatal error occured !!! errno is %d\n",errno);
              printf("errlog:CANNOT LOCK ERROR_LOG %s !!!\n",LOG_FILE_NAME);
          }
      }
#endif

      if ((iFileSize =  lseek(iLogFile,0,SEEK_END)) == -1) {
          printf("errLog.c: fatal error occured ! error_no is %d\n",errno);
          printf("errLog: %s CANNOT JUMP TO END OF FILE !\n",LOG_FILE_NAME);
          close(iLogFile);
          return(-1);
      }

      if (iErrCode == LOG_DEBUG) {
          write(iLogFile,"@MSG:",5);
          write(iLogFile,pcaDispMsg,strlen(pcaDispMsg));
          write(iLogFile,LOG_MSG_SEPERATE,LOG_SEPERATE_LEN);
      } else {
          logDispErrMsg(iLogFile,iErrCode,pcaDispMsg,pcaDumpAddr,lDumpLen); 
      }
      close(iLogFile);

      if (iFileSize >= LOG_MAX_BAK_SIZE) {
          sprintf(pcaDispMsg,"%s.bak",LOG_FILE_NAME);

          if ((iRc = rename(LOG_FILE_NAME, pcaDispMsg)) != 0) {
              printf("fatal error occured ! iRc= %d error_no is %d\n",
                     iRc,errno); 
              printf("errLog: %s CANNOT BE RENAMED !\n",LOG_FILE_NAME);
              return(-1);
          }
      }
  }   /* for if (cDumpDev & RPT_TO_LOG )  */

  if (cDumpDev & RPT_TO_TTY) {    /* output to tty */
      if (iErrCode == LOG_DEBUG) {
          write(1,"@MSG:",5);
          write(1,pcaDispMsg,strlen(pcaDispMsg));
          write(1,LOG_MSG_SEPERATE,LOG_SEPERATE_LEN);
      } else {
          logDispErrMsg(1,iErrCode,pcaDispMsg,pcaDumpAddr,lDumpLen);
      }
  }

  return(0);
} 


/*    ROUTINE NAME:logDispErrMsg(int iFileId,int iErrCode,char *pcaMsgStr,
 *                               char *pcaDumpAddr,long lDumpLen)
 *    ARGUMENTS:
 *      NAME              TYPE                 DESCRIPTION
 *    --------------- ----------------    -------------------------
 *    iFileId		int		  ��¼�ļ��Ĵ���
 *    pcaMsgStr		char *		  ����ѶϢ�ִ��ĵ�ַ
 *    pcaDimpAddr	char *		  ����ʾ���ڴ������ַ
 *    lDumpLen		long		  ����ʾ�ڴ��������ϵĳ���
 *
 *    RETURN VALUE(S):
 *      ��
 *
 *    DESCRIPTION:
 *      1.��¼ʱ�䡡������š�������롡����ѶϢ��ִ�е�·��.
 *      2.��ʾ�ڴ���������.
 */

void logDispErrMsg(int iLogFile,int iErrCode,char *pcaMsgStr,char *pcaDumpAddr,long lDumpLen)
{
  char caTmpBuf[40];
  char caStrBuf[LOG_MAX_MSG_LEN + 64];
  int  iLen;         /* for display flow control */
 
#ifdef WIN32
  iLen = sprintf(caStrBuf,"@%d|1|%d|%.31s\nMSG:%s\n",
         _getpid(),iErrCode,logGetTime(NULL,0,caTmpBuf),pcaMsgStr);
#else
  #if defined SCO || defined AIX 
    iLen = sprintf(caStrBuf,"@%d|1|%d|%.31s\nMSG:%s\n",
           getpid(),iErrCode,logGetTime(NULL,0,caTmpBuf),pcaMsgStr);
  #else
    iLen = sprintf(caStrBuf,"@%d|%d|%d|%.31s\nMSG:%s\n",
           getpid(),pthread_self(),iErrCode,logGetTime(NULL,0,caTmpBuf),pcaMsgStr);
  #endif
#endif
  write(iLogFile,caStrBuf,iLen);

  /* dump memory to fd */
  if ((pcaDumpAddr != 0)&&(lDumpLen > 0)) {
      logMemDump(iLogFile,(unsigned char *)pcaDumpAddr,(long)lDumpLen);
  }
  write (iLogFile,LOG_MSG_SEPERATE,LOG_SEPERATE_LEN);
}

/*
 *    ROUTINE NAME:logMemDump(int iFileId,unsigned char *pucaAddr,long lLength)
 *    ARGUMENTS:
 *      NAME              TYPE                 DESCRIPTION
 *    --------------- ----------------    -------------------------
 *    iFileId		int		  �ļ�����
 *    pucaAddr		unsigned char *	  �ڴ������ַ
 *    lLength		long		  ��ʾ����
 *
 *    RETURN VALUE(S):
 *      ��
 *
 *    DESCRIPTION:
 *      This subroutine dump prescribe memory to prescribe device.
 */

void logMemDump(int iLogFile,unsigned char *pucaAddr,long lLength)
{
  int  i,j,n;
  int  iPage = 20;
  int  iTemp;
  char caTmp[100];
  char caBuf[1650];
  unsigned char  caCmpC[2] = { 0, 31 };
  unsigned char *pcPtr;

  pcPtr = pucaAddr;
  caBuf[0] = '\0';
  while (pcPtr < (pucaAddr + lLength)) {
      for (j=0;j <= (lLength-1)/16;j++) { 
          if (j == (j/iPage)*iPage) {
              strcpy(caTmp,"Displacement ");
              strcat(caTmp,"-1--2--3--4--5--6--7--8-Hex-0--1--2--3--4--5--6");
              strcat(caTmp,"  --ASCII Value--\n");
              write(iLogFile,caTmp,strlen(caTmp));
          }

          iTemp = j*16;
          if (iTemp < 10) {
              sprintf(caTmp,"0000%d",iTemp);
          } else {
              if (iTemp < 100) {
                  sprintf(caTmp,"000%d",iTemp);
              } else {
                  if (iTemp < 1000) {
                      sprintf(caTmp,"00%d",iTemp);
                  } else {
                      if (iTemp < 10000) {
                          sprintf(caTmp,"0%d",iTemp);
                      } else {
                          sprintf(caTmp,"%d",iTemp);
                      }
                  }
              }
          }

          if (iTemp < 16){
              sprintf(caTmp,"%s(0000%x) ",caTmp,iTemp);
          } else {
              if (iTemp < 256) {
                  sprintf(caTmp,"%s(000%x) ",caTmp,iTemp);
              } else {
                  if (iTemp < 4096) {
                      sprintf(caTmp,"%s(00%x) ",caTmp,iTemp);
                  } else {
                      if (iTemp < 65536){
                          sprintf(caTmp,"%s(0%x) ",caTmp,iTemp);
                      } else {
                          sprintf(caTmp,"%s(%x) ",caTmp,iTemp);
                      }
                  }
              }
          }

          for (i=0;(i<16) && (pcPtr < (pucaAddr+lLength));i++) {
              if (*pcPtr < 16) {
                  sprintf(caTmp,"%s0%x ",caTmp,*pcPtr);
              } else {
                  sprintf(caTmp,"%s%2x ",caTmp,*pcPtr);
              }
              pcPtr += 1;
          }

          for (n=0;n < 16-i ;n++) {
               sprintf(caTmp,"%s   ",caTmp);
          }
          sprintf(caTmp,"%s ",caTmp);
          pcPtr = pcPtr - i;

          for (n=0;n < i ;n++) {
/*            if (((unsigned char)(*pcPtr) <= (unsigned char)31) &&
 *                ((unsigned char)(*pcPtr) >= (unsigned char)0)) {
 */
              if (((unsigned char)(*pcPtr) <= (caCmpC[1])) &&
                  ((unsigned char)(*pcPtr) >= (caCmpC[0]))) {

                  sprintf(caTmp,"%s*",caTmp);
              } else {
                  sprintf(caTmp,"%s%c",caTmp,*pcPtr);
              }
              pcPtr += 1;
          }

          strcat(caBuf,caTmp);
          strcat(caBuf,"\n");
          if (j == (j/iPage)*iPage) {
              write(iLogFile,caBuf,strlen(caBuf));
              caBuf[0]='\0';
          }
      } /* end of for    */
  }   /* end of while    */

  write(iLogFile,caBuf,strlen(caBuf));
} /* end of logMemDump   */




/***************************************************************************/
/* ������ : ȡ�����ں�ʱ��, ����2000������                               */
/* ��    �� : ������  Tang Yanyun                                          */
/* ��    �� : 1997��12��05��                                               */
/* ʹ������ : ��                                                           */
/* �������� : char *pcInputTimeStr �ַ���,ָ����ʼʱ��,��Ϊ��ָ��,��ʾ��ǰ */
/*                  ϵͳʱ��Ϊ��ʼʱ��.�ַ�����ʽΪ(�������ַ���������) :  */
/*                 "������ʱ����΢��"(YYYYmmddHHMMSSMicroS)                */
/*            int   iSecNum    ȡ��ǰϵͳʱ�侭����������ʱ��(��Ϊ����)  */
/*            char *pcTimeStr  �ַ���,����ִ�гɹ��󷵻�21���ֽڵ��ַ���,  */
/*                ϵͳ���ں�ʱ�䰴"������ʱ����΢��"(YYYYmmddHHMMSSMicroS) */
/*            ��˳������, ��������4 λ��ʾ( ����ʱ�Ѱ����ַ��������� )     */
/* ��ֵ���� : char *  ����ȡ�õ��ַ���( ��pcTimeStr )                      */
/* ע������ : ���ô˺���ʱ�����Ĳ������д��ڵ���21���ֽڵĴ����ռ�, ����� */
/*            ���³������м����ȶ�. ʹ�ô˺���ʱ�����time.h :             */
/*             #include <time.h>                                           */
/* �޸ļ�¼ :                                                              */
/***************************************************************************/
char* appGetTime(char *pcInputTimeStr,int iSecNum,char *pcTimeStr)
{
  struct tm stTime;
#ifdef WIN32
  struct _timeb stTimeB;
#else
  struct  timeb stTimeB;
#endif

  MEMSET_DEBUG((char *)&stTime ,'\0',sizeof(struct tm));
#ifdef WIN32
  MEMSET_DEBUG((char *)&stTimeB,'\0',sizeof(struct _timeb));
#else
  MEMSET_DEBUG((char *)&stTimeB,'\0',sizeof(struct  timeb));
#endif

  if (pcInputTimeStr == NULL) {
#ifdef WIN32
      _ftime(&stTimeB);
#else
       ftime(&stTimeB);
#endif
  } else {
/*    strptime(pcInputTimeStr + 8,"%Y-%m-%d %H:%M:%S",&stTime); */
/*    stTimeVal.tv_sec  = mktime(&stTime); */
/*    stTimeVal.tv_usec = 0; */

      sscanf(pcInputTimeStr,"%4d%2d%2d%2d%2d%2d",&(stTime.tm_year),
          &(stTime.tm_mon),&(stTime.tm_mday),&(stTime.tm_hour),
          &(stTime.tm_min),&(stTime.tm_sec));

      stTime.tm_year = stTime.tm_year - 1900;
      stTime.tm_mon--;
      stTimeB.time = mktime(&stTime);
  }

  stTimeB.time = stTimeB.time + iSecNum;

  #ifdef AIX
  MEMCPY_DEBUG(&stTime,localtime((time_t *)&(stTimeB.time)),sizeof(struct tm));
  #else
  MEMCPY_DEBUG(&stTime,localtime((long *)&(stTimeB.time)),sizeof(struct tm));
  #endif
  strftime(pcTimeStr,(size_t)(14 + 1),"%Y%m%d%H%M%S",&stTime);
  if (pcInputTimeStr == NULL) {
      sprintf(pcTimeStr + 14,"%.3ld",stTimeB.millitm);
  } else {
      strcpy( pcTimeStr + 14,pcInputTimeStr + 14);
  }

  return(pcTimeStr);
}


/*
char* appGetTime(char *pcInputTimeStr,int iSecNum,char *pcTimeStr)
{
  struct timeval stTimeVal;
  struct tm stTime;

  MEMSET_DEBUG((char *)&stTimeVal,'\0',sizeof(struct timeval));
  if (pcInputTimeStr == NULL) {
      if (gettimeofday(&stTimeVal,NULL) < 0) {
          printf("gettimeofday error\n");
          (*pcTimeStr) = '\0';
          return(pcTimeStr);
      }
  } else {
      strptime(pcInputTimeStr,"%Y%m%d%H%M%S",&stTime);
      stTimeVal.tv_sec  = mktime(&stTime);
      stTimeVal.tv_usec = 0;
  }

  stTimeVal.tv_sec = stTimeVal.tv_sec + (time_t)iSecNum;

  MEMCPY_DEBUG(&stTime,localtime(&(stTimeVal.tv_sec)),sizeof(struct tm));
  strftime(pcTimeStr,(size_t)(14 + 1),"%Y%m%d%H%M%S",&stTime);
  if (pcInputTimeStr == NULL) {
      sprintf(pcTimeStr + 14,"%.6ld",stTimeVal.tv_usec);
  } else {
      strcpy( pcTimeStr + 14,pcInputTimeStr + 14);
  }

  return(pcTimeStr);
}
*/

/***************************************************************************/
/* ������ : ȡ�����ں�ʱ��, ����2000������                               */
/* ��    �� : ������  Tang Yanyun                                          */
/* ��    �� : 1997��12��05��                                               */
/* ʹ������ : ��                                                           */
/* ��ֵ���� : char *  ����ȡ�õ��ַ���( ��pcTimeStr )                      */
/* ע������ : ���ô˺���ʱ�����Ĳ������д��ڵ���35���ֽڵĴ����ռ�, ����� */
/*            ���³������м����ȶ�. ʹ�ô˺���ʱ�����time.h :             */
/*             #include <time.h>                                           */
/* �޸ļ�¼ :                                                              */
/***************************************************************************/
char* logGetTime(char *pcInputTimeStr,int iSecNum,char *pcTimeStr)
{
  struct tm stTime;
#ifdef WIN32
  struct _timeb stTimeB;
#else
  struct  timeb stTimeB;
#endif

  MEMSET_DEBUG((char *)&stTime ,'\0',sizeof(struct tm));
#ifdef WIN32
  MEMSET_DEBUG((char *)&stTimeB,'\0',sizeof(struct _timeb));
#else
  MEMSET_DEBUG((char *)&stTimeB,'\0',sizeof(struct  timeb));
#endif

  if (pcInputTimeStr == NULL) {
#ifdef WIN32
      _ftime(&stTimeB);
#else
       ftime(&stTimeB);
#endif
  } else {
/*    strptime(pcInputTimeStr + 8,"%Y-%m-%d %H:%M:%S",&stTime); */
/*    stTimeVal.tv_sec  = mktime(&stTime); */
/*    stTimeVal.tv_usec = 0; */
      sscanf(pcInputTimeStr + 8,"%d-%d-%d %d:%d:%d",&(stTime.tm_year),
          &(stTime.tm_mon),&(stTime.tm_mday),&(stTime.tm_hour),
          &(stTime.tm_min),&(stTime.tm_sec));
      stTime.tm_year = stTime.tm_year - 1900;
      stTime.tm_mon--;
      stTimeB.time = mktime(&stTime);
  }

  stTimeB.time = stTimeB.time + iSecNum;
#ifdef AIX
  MEMCPY_DEBUG(&stTime,localtime(&(stTimeB.time)),sizeof(struct tm));
#else
  MEMCPY_DEBUG(&stTime,localtime((long *)&(stTimeB.time)),sizeof(struct tm));
#endif
  strftime(pcTimeStr,(size_t)(27 + 1),"%b %a %Y-%m-%d %H:%M:%S",&stTime);
  if (pcInputTimeStr == NULL) {
      sprintf(pcTimeStr + 27,".%.3ld",stTimeB.millitm);
  } else {
      strcpy( pcTimeStr + 27,pcInputTimeStr + 27);
  }

  return(pcTimeStr);
}


/*
char* logGetTime(char *pcInputTimeStr,int iSecNum,char *pcTimeStr)
{
  struct timeval stTimeVal;
  struct tm stTime;

  MEMSET_DEBUG((char *)&stTimeVal,'\0',sizeof(struct timeval));
  if (pcInputTimeStr == NULL) {
      if (gettimeofday(&stTimeVal,NULL) < 0) {
          printf("gettimeofday error\n");
          (*pcTimeStr) = '\0';
          return(pcTimeStr);
      }
  } else {
      strptime(pcInputTimeStr + 8,"%Y-%m-%d %H:%M:%S",&stTime);
      stTimeVal.tv_sec  = mktime(&stTime);
      stTimeVal.tv_usec = 0;
  }

  stTimeVal.tv_sec = stTimeVal.tv_sec + (time_t)iSecNum;

  MEMCPY_DEBUG(&stTime,localtime(&(stTimeVal.tv_sec)),sizeof(struct tm));
  strftime(pcTimeStr,(size_t)(27 + 1),"%b %a %Y-%m-%d %H:%M:%S",&stTime);
  if (pcInputTimeStr == NULL) {
      sprintf(pcTimeStr + 27,".%.6ld",stTimeVal.tv_usec);
  } else {
      strcpy( pcTimeStr + 27,pcInputTimeStr + 27);
  }

  return(pcTimeStr);
}
*/

int appSleep(int iSec,int iUSec)
{
  struct timeval stTimeOut;

  MEMSET_DEBUG((char *)&stTimeOut,'\0',sizeof(struct timeval));
  stTimeOut.tv_sec  = iSec;
  stTimeOut.tv_usec = iUSec;  /* 1,000,000 USec == 1 Sec */
  if ((select(0,NULL,NULL,NULL,&stTimeOut)) < 0) {
      return(-1);
  } else {
      return(0);
  }
}
















/***************************************************************************/
/* �������� : filestr.cpp                                                  */
/* ��    �� : ������  Tang Yanyun                                          */
/* ��    �� : 1997��12��05��  2001/12/05                                   */
/* �������� : int getCfgVal()                                              */
/*                ȡ��ϵͳ�����ļ��е�����ֵ                               */
/* �������� : int getFileStr(char *pcFileName,char *caStrTbl[])            */
/*                ȡ��ϵͳ�����ļ��е��ַ���                               */
/* ��ֵ���� : �μ��������к궨�岿��getCfgVal() ������ֵ����               */
/*            �μ��������к궨�岿��getFileStr()������ֵ����               */
/* ����ʾ�� : �μ����������ѱ�ע����main()��������                         */
/***************************************************************************/
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "errlog.h"
*/
#define MAX_LINE_LEN                160
#define MAX_STR_LEN                 80
#define MAX_STR_NUM                 513

#define CHAR_1                      ' '
#define CHAR_2                      '\t'
#define CHAR_3                      '#'
#define CHAR_4                      '"'
#define CHAR_5                      '\\'

/*** getCfgVal() ����ֵ���� ***/
#define NORMAL_END                  0   /*** ����ִ�гɹ�,��ȷ����������ֵ***/
#define CFG_NOT_FOUND               1   /*** ���������ļ���δ���ҵ������� ***/
#define CFG_STR_NUM_IS_ODD          2   /*** ���������ļ��ַ�������Ϊ���� ***/
#define CFG_FILE_HAS_NULL_STR       3   /*** ���������ļ������ַ���Ϊ��
                                             �������ļ���������""�Ķ���   ***/
#define CFG_FILE_IS_NULL            4   /*** ���������ļ���               ***/
#define TOO_MANY_STR                5   /*** ���������ļ�������̫����ַ���**/
                                        /*** �緵��ֵΪ����,��ΪgetFileStr()
                                             ��������                     ***/
/*** getFileStr() ����ֵ���� ***/
#define CFG_FILE_OPEN_ERROR        -1   /*** ���������ļ���ʧ��         ***/
#define LINE_TOO_LONG              -2   /*** ���������ļ���һ�ж���̫��   ***/
#define  STR_TOO_LONG              -3   /*** ���������ļ��ַ�������̫��   ***/
#define NO_SEC_CHAR_4              -4   /*** û����Եĵڶ���CHAR_4�ַ�   ***/
                                        /*** �緵��ֵΪ�Ǹ���,��Ϊ�ڲ�����
                                             ���ļ����ҵ����ַ����ĸ���   ***/


/*** Ϊ������,��������ʾ���ɲο��˲��ֳ��� ***
main()
{
  int  i,iRc;
  char caFileName[MAX_LINE_LEN],caCfgVal[MAX_STR_LEN];
  char *caStrTbl[MAX_STR_NUM];

  sprintf(caFileName,"%s/cmgetcfg_trc",getenv("PWD"));
  chgLog(LOG_CHG_LOG ,caFileName);
  chgLog(LOG_CHG_MODE,"0");
  chgLog(LOG_CHG_SIZE,"4096");
  chgLog(LOG_CHG_MASK,"0xFFFFFFFF");

  for (i=0;i<MAX_STR_NUM;i++) {
      caStrTbl[i] = (char *)mall oc(MAX_STR_LEN);
  }
  sprintf(caFileName,"%s/kkk",getenv("PWD"));

  iRc = getFileStr(caFileName,caStrTbl);
  fprintf(stdout,"getFileStr() return=%d\n",iRc);
  for (i=0;i<iRc;i++) {
      fprintf(stdout,"caStrTbl[%d]=%s\n",i,caStrTbl[i]);
  }

  for (i=0;i<MAX_STR_NUM;i++) fre e(caStrTbl[i]);

  iRc = getCfgVal(caFileName,"fff",caCfgVal);
  fprintf(stdout,"getCfgVal() return=%d\n",iRc);
  fprintf(stdout,"caCfgVal=%s\n",caCfgVal);

  return(0);
}
***/

/***************************************************************************/
/* ��    �� : ������  Tang Yanyun                                          */
/* ��    �� : 1997��12��05��  1997/12/05                                   */
/* �������� : int getCfgVal()                                              */
/* �������� : char *pcFileName  �ļ�����,��ʹ�þ���·����,                 */
/*                              �ΪMAX_LINE_LEN�ַ�,��'\0'�������ִ�    */
/*            char *pcCfgName   ��ȡ�õ����ò�����,����С��MAX_STR_LEN�ַ� */
/*            char *pcCfgVal    ȡ�õ����ò���ֵ,  ����С��MAX_STR_LEN�ַ� */
/* ��ֵ���� : �μ��������к궨�岿��getCfgVal()������ֵ����                */
/* �������� : ���ļ�pcFileName�ж�������ò���pcCfgName������ֵ����pcCfgVal*/
/*                                                                         */
/* �ļ����� : �����ļ�pcFileName���ַ����Ķ���������� :                   */
/*                                                                         */
/*            1.�ַ���д�ַ�������Ӣ�Ŀո��tab������                      */
/*            2.һ�п��Զ���һ�������ַ���                               */
/*            3.�� #������β���ַ���Ϊע�ͣ������������                   */
/*            4.���ַ����к���Ӣ�Ŀո��tab��#��,����ַ�������" ��" ����  */
/*              ���� :  ����Ϊ"ab#cd ef"   ��õ����ַ���Ϊab#cd ef        */
/*            5.���ַ����к���" ����, ������" ��ǰ����\ ��, \ ��������     */
/*              ���� :  ����Ϊab\"cdef     ��õ����ַ���Ϊab"cdef         */
/*            6.���ַ���ʹ������������" ����ʾ����, ���� : ""              */
/*                                                                         */
/* ʹ������ : 1.�ļ�pcFileName�ж��������ַ�����С��MAX_STR_NUM          */
/*            2.�ļ�pcFileName�ж������ַ���С��MAX_STR_LEN            */
/*            3.�ļ�pcFileName��һ���С��MAX_LINE_LEN�ַ�               */
/* ע    �� : �����ļ��ж�����ַ�������һ������ڵ���getFileStr()����ʱ   */
/*            ʹ�õ��ַ�������Ĵ�СС,�����coredump                      */
/***************************************************************************/

int getCfgVal(char *pcFileName,char *pcCfgName,char *pcCfgVal)
{
  char *caStrTbl[MAX_STR_NUM];
  int   i,iRc,iFileStrNum;

  for (i=0;i<MAX_STR_NUM;i++) {
      caStrTbl[i] = (char *)malloc(MAX_STR_LEN);
  }

  iRc = getFileStr(pcFileName,caStrTbl);
  if (iRc >= MAX_STR_NUM) {
      errLog(3,0,RPT_TO_LOG,0,0,"The %s file is defined too many strings!!!",pcFileName);
      for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]) ;
      return(TOO_MANY_STR);
  }
  if (iRc > 0) {
      if (iRc % 2 == 0) {
          iFileStrNum = iRc;
      } else {
          errLog(3,0,RPT_TO_LOG,0,0,"The %s file's strings NUM is ODD !!!",pcFileName);
          for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]);
          return(CFG_STR_NUM_IS_ODD);
      }
  } else {
      if (iRc == 0) {
          errLog(3,0,RPT_TO_LOG,0,0,"The %s file is defined NULL !!!",pcFileName);
          for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]);
          return(CFG_FILE_IS_NULL);
      } else {
          errLog(3,0,RPT_TO_LOG,0,0,"getFileStr() error!!! iRc=%d",iRc);
          for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]);
          return(iRc);
      }
  }
  for (i=0;i<iFileStrNum;i++) {
      if (strlen(caStrTbl[i]) == 0) {
          errLog(3,0,RPT_TO_LOG,0,0,"The %s file has NULL string !!!",pcFileName);
          for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]);
          return(CFG_FILE_HAS_NULL_STR);
      }
  }
  iRc = 1;
  for (i=0;i<iFileStrNum;i++) {
      if (memcmp(pcCfgName,caStrTbl[i]  ,strlen(caStrTbl[i]  )) == 0) {
          MEMCPY_DEBUG(pcCfgVal ,caStrTbl[i+1],strlen(caStrTbl[i+1]));
          iRc = 0;
          break;
      }
      i++;
  }

  if (iRc == 1) {
      errLog(3,0,RPT_TO_LOG,0,0,"The %s config Val NOT found !!!",pcCfgName);
      for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]);
      return(CFG_NOT_FOUND);
  } else {
      for (i=0;i<MAX_STR_NUM;i++) free(caStrTbl[i]);
      return(NORMAL_END);
  }

/*for (i=0;i<MAX_STR_NUM;i++) fre e(caStrTbl[i]);
  return(iRc); */
}

/***************************************************************************/
/* ��    �� : ������  Tang Yanyun                                          */
/* ��    �� : 1997��12��05��  1997/12/05                                   */
/* �������� : getFileStr(char *pcFileName,char *caStrTbl[])                */
/* �������� : char *pcFileName  �ļ�����,��ʹ�þ���·����,                 */
/*                              �ΪMAX_LINE_LEN�ַ�,��'\0'�������ִ�    */
/*            char *caStrTbl[]  ȡ�õ��ļ������ַ�����                     */
/* ��ֵ���� : �μ��������к궨�岿��getFileStr()������ֵ����               */
/* �������� : ���ļ�pcFileName�ж�����ַ������������ַ�����caStrTbl��     */
/*                                                                         */
/* �ļ����� : �����ļ�pcFileName���ַ����Ķ���������� :                   */
/*                                                                         */
/*            1.�ַ���д�ַ�������Ӣ�Ŀո��tab������                      */
/*            2.һ�п��Զ���һ�������ַ���                               */
/*            3.�� #������β���ַ���Ϊע�ͣ������������                   */
/*            4.���ַ����к���Ӣ�Ŀո��tab��#��,����ַ�������" ��" ����  */
/*              ���� :  ����Ϊ"ab#cd ef"   ��õ����ַ���Ϊab#cd ef        */
/*            5.���ַ����к���" ����, ������" ��ǰ����\ ��, \ ��������     */
/*              ���� :  ����Ϊab\"cdef     ��õ����ַ���Ϊab"cdef         */
/*            6.���ַ���ʹ������������" ����ʾ����, ���� : ""              */
/*                                                                         */
/* ʹ������ : 1.�ļ�pcFileName�ж������ַ���С��MAX_STR_LEN            */
/*            2.�ļ�pcFileName��һ���С��MAX_LINE_LEN�ַ�               */
/* ע    �� : 1.�ڵ��ñ�����ʱ�ַ�������caStrTbl�����ѱ����ռ�             */
/*            2.�����ļ��ж�����ַ�������һ������ڵ���getFileStr()����ʱ */
/*              ʹ�õ��ַ�������Ĵ�СС,�����coredump                    */
/***************************************************************************/

int getFileStr(char *pcFileName,char *caStrTbl[])
{
  char  caLineBuf[MAX_LINE_LEN + 2];
  char  caStrBuf[MAX_STR_LEN + 1];
  int   i,iRc,iStrNum,iStrCharNo,iChar_4Flag,iLineBufLen;
  FILE *pfFileName;

  pfFileName =  fopen( pcFileName, "r" );
  if (pfFileName == NULL) {
      errLog(3,0,RPT_TO_LOG,0,0,"getFileStr:fopen(%s) fails,errno=%d",pcFileName,errno);
      return(CFG_FILE_OPEN_ERROR);
  }

  iStrNum     = 0;
  iStrCharNo  = 0;
  iChar_4Flag = 0;
  caStrBuf[MAX_STR_LEN] = '\0';

  do {
      if (fgets(caLineBuf,sizeof(caLineBuf),pfFileName) == NULL) break;
      iLineBufLen = strlen(caLineBuf);
   /* printf("Before delete line terminator iLineBufLen=%d\n",iLineBufLen); */
      if (caLineBuf[iLineBufLen - 1] == '\n') {
          caLineBuf[iLineBufLen - 1] =  '\0';
          iLineBufLen--;
          if (caLineBuf[iLineBufLen - 1] == '\r') {
              caLineBuf[iLineBufLen - 1] =  '\0';
              iLineBufLen--;
          }
      }
   /* printf("After  delete line terminator iLineBufLen=%d\n",iLineBufLen); */
      if (iLineBufLen >= MAX_LINE_LEN) {
          errLog(3,0,RPT_TO_LOG,0,0,"The line of %s is defined too long!!!",pcFileName);
          return(LINE_TOO_LONG);
      }

      for (i=0;i<iLineBufLen;i++) {

          if ( caLineBuf[i] == CHAR_3) {
              if (iChar_4Flag == 0) {
                  if (iStrCharNo >= 1) {
                      MEMCPY_DEBUG(caStrTbl[iStrNum],caStrBuf,iStrCharNo);
                      *((caStrTbl[iStrNum]) + iStrCharNo) = '\0';
                   /* MEMSET_DEBUG(caStrBuf,'\0',MAX_STR_LEN); */
                      iStrNum++;
                      iStrCharNo = 0;
                  }
                  break;
              }
          }

          if ((caLineBuf[i] == CHAR_1) || (caLineBuf[i] == CHAR_2)) {
              if (iChar_4Flag == 0) {
                  if (iStrCharNo >= 1) {
                      MEMCPY_DEBUG(caStrTbl[iStrNum],caStrBuf,iStrCharNo);
                      *((caStrTbl[iStrNum]) + iStrCharNo) = '\0';
                   /* MEMSET_DEBUG(caStrBuf,'\0',MAX_STR_LEN); */
                      iStrNum++;
                      iStrCharNo = 0;
                  }
                  continue;
              }
          }

          if ( caLineBuf[i] == CHAR_4) {
              if (iChar_4Flag == 0) {
                  if (caLineBuf[i-1] == CHAR_5) {
                      caStrBuf [iStrCharNo-1] = caLineBuf[i];
                      continue;
                  }
                  iChar_4Flag = 1;
                  if (iStrCharNo >= 1) {
                      MEMCPY_DEBUG(caStrTbl[iStrNum],caStrBuf,iStrCharNo);
                      *((caStrTbl[iStrNum]) + iStrCharNo) = '\0';
                   /* MEMSET_DEBUG(caStrBuf,'\0',MAX_STR_LEN); */
                      iStrNum++;
                      iStrCharNo = 0;
                  }
                  continue;
              } else {
                  if (caLineBuf[i-1] == CHAR_5) {
                      caStrBuf [iStrCharNo-1] = caLineBuf[i];
                      continue;
                  } else {
                      MEMCPY_DEBUG(caStrTbl[iStrNum],caStrBuf,iStrCharNo);
                      *((caStrTbl[iStrNum]) + iStrCharNo) = '\0';
                   /* MEMSET_DEBUG(caStrBuf,'\0',MAX_STR_LEN); */
                      iStrNum++;
                      iStrCharNo = 0;
                      iChar_4Flag = 0;
                      continue;
                  }
              }
          }
          caStrBuf[iStrCharNo] = caLineBuf[i];
          iStrCharNo++;
          if (iStrCharNo >= MAX_STR_LEN) {
              errLog(3,0,RPT_TO_LOG,0,0,"The string of %s is defined too long !!!",pcFileName);
              return(STR_TOO_LONG);
          }
      }
      if (iChar_4Flag == 1) {
          errLog(3,0,RPT_TO_LOG,0,0,"The line have NOT the second CHAR_4 !!!");
          return(NO_SEC_CHAR_4);
      }
      if (iStrCharNo >= 1) {
          MEMCPY_DEBUG(caStrTbl[iStrNum],caStrBuf,iStrCharNo);
          *((caStrTbl[iStrNum]) + iStrCharNo) = '\0';
       /* MEMSET_DEBUG(caStrBuf,'\0',MAX_STR_LEN); */
          iStrNum++;
          iStrCharNo = 0;
      }

  } while(!feof(pfFileName));
  fclose(pfFileName);
  return(iStrNum);
}

