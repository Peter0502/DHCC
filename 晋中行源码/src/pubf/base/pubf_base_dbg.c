#include "find_debug.h" 
/*************************************************
* 文 件 名:  pubf_base_dbg.c
* 功能描述： 跟踪程序自己的一些日志处理
* 作    者: gujy
* 完成日期：2006/07/27
* 修改记录：  
* 1. 日   期:
*    修 改 人:
*    修改内容:
* 2. ...
*************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <timeb.h>
#include <time.h>
#define EXTERN

/***************************************************************************/
/* 程序功能 : 取得日期和时间, 避免2000年问题                               */
/* 作    者 : 唐燕云  Tang Yanyun                                          */
/* 日    期 : 1997年12月05日                                               */
/* 使用限制 : 无                                                           */
/* 返值意义 : char *  返回取得的字符串( 即pcTimeStr )                      */
/* 注意事项 : 调用此函数时函数的参数须有大于等于35个字节的存贮空间, 否则会 */
/*            导致程序运行极不稳定. 使用此函数时须包含time.h :             */
/*             #include <time.h>                                           */
/* 修改记录 :                                                              */
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
  /*strftime(pcTimeStr,(size_t)(27 + 1),"%b %a %Y-%m-%d %H:%M:%S",&stTime);
  pcTimeStr = malloc();*/
  sprintf(pcTimeStr, "%02d%02d%02d%02d%02d%02d日期:%04d年%02d月%02d日   时间:%02d:%02d:%02d", stTime.tm_year%100, stTime.tm_mon+1, stTime.tm_mday, stTime.tm_hour, stTime.tm_min, stTime.tm_sec, stTime.tm_year, stTime.tm_mon+1, stTime.tm_mday, stTime.tm_hour, stTime.tm_min, stTime.tm_sec);
  if (pcInputTimeStr == NULL) {
      sprintf(&pcTimeStr[strlen(pcTimeStr)], ".%.3ld",stTimeB.millitm);
  } else {
      strcpy(&pcTimeStr[strlen(pcTimeStr)],pcInputTimeStr + 27);
  }

  return(pcTimeStr);
}
long
   get_file_size( char * filename )
   {
        struct stat f_stat;

        if( stat( filename, &f_stat ) == -1 ){
            return -1;
        }

        return (long)f_stat.st_size;

    }

int
LF_wbuf(char *filename, char *buf)
{
	FILE           *fp;
	const  long  max_file_len = 32*1024*1024;
	char   tmpbuf[255];
	char   strtime[255];
	char bkfnm[255];


	/*printf("[%s][%d] fileSize[%ld] max_file_len[%ld]\n", __FILE__, __LINE__, get_file_size(filename) , max_file_len);*/
	if(get_file_size(filename) - max_file_len > 0.001){
		sprintf(bkfnm, "%s%.12s.bk", filename, logGetTime(NULL, 0, strtime));
		rename(filename, bkfnm);
	}

	if (NULL == (fp = fopen(filename, "a"))) {
		return -1;
	}
	fprintf(fp, buf);
	fclose(fp);

}
void
LF_wform(char *file)
{
	LF_wbuf(file, "*********************************************************************\n");
}

void 
LF_wjymc(char *file, char *jymc)
{
	LF_wform(file);
	LF_wbuf(file, jymc);
	LF_wbuf(file, "\n");
	LF_wform(file);
}

void
LF_printf(char *filename, char *fmt,...)
{

	va_list         argptr;
	int             cnt;
	char            buf[255 * 1024];

	MEMSET_DEBUG(buf, 0x0, sizeof(buf));
	va_start(argptr, fmt);

	cnt = vsprintf(buf, fmt, argptr);
	va_end(argptr);
/*	strcat(buf, "\n");*/

	LF_wbuf(filename, buf);


}
void Dbg_printf(char *fmt, ...)
{

	va_list         argptr;
	int             cnt;
	char            buf[255 * 1024];
	char			filename[100];

	MEMSET_DEBUG(buf, 0x0, sizeof(buf));
	va_start(argptr, fmt);
	MEMSET_DEBUG(filename, 0x0, sizeof(filename));
	if(NULL != getenv("DBGLOG")){
		sprintf(filename, "%s", getenv("DBGLOG"));
	}else{
		sprintf(filename, "/dev/NULL" );
	}
	cnt = vsprintf(buf, fmt, argptr);
	va_end(argptr);
	strcat(buf, "\n");

	LF_wbuf(filename, buf);


}
