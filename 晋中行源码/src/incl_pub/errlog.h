/************************************************************************
 * PROGRAM :  errlog.h                                                  *
 * AUTHOR  :  Tang Yanyun  Ã∆—‡‘∆                                       *
 * CREATED DATE :  1997-12-20                                           *
 ************************************************************************/
#ifndef _H_ERRLOG
#define _H_ERRLOG

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <stdarg.h>


#define  LOG_CHG_LOG           '1'	/* change LOG_MODE      */
#define  LOG_CHG_MODE          '2'	/* change LOG_FILE_NAME */
#define  LOG_CHG_SIZE          '3'	/* change LOG_FILE_SIZE */
#define  LOG_CHG_MASK          '4'	/* change LOG_FILE_SIZE */

#define  RPT_TO_TTY		1          /* 00000001 */
#define  RPT_TO_CON		4          /* 00000100 */
#define  RPT_TO_LOG		8          /* 00001000 */

#define  LOG_MSG_SEPERATE	"\n"
#define  LOG_SEPERATE_LEN       1

#define  LOG_MAX_MSG_LEN        256
#define  LOG_MAX_OPEN_RETRY     32	/* max retry open log file   */


/* defined in /usr/include/sys/syslog.h *
#define LOG_EMERG       0       // system is unusable
#define LOG_ALERT       1       // action must be taken immediately
#define LOG_CRIT        2       // critical conditions
#define LOG_ERR         3       // error conditions
#define LOG_WARNING     4       // warning conditions
#define LOG_NOTICE      5       // normal but signification condition
#define LOG_INFO        6       // informational
#define LOG_DEBUG       7       // debug-level messages
 * defined in /usr/include/sys/syslog.h */


extern int   chgLog(char,char *);
/*extern int   errLog(int, ...);*/
extern int errLog(int iErrCode,unsigned int iMask,char cDumpDev,char *pcaDumpAddr,
                  long lDumpLen,char *pcFormat,...);

extern char *logGetTime(char *,int,char *);
extern char *appGetTime(char *,int,char *);
extern int   appSleep(int,int);

extern int   getCfgVal(char *,char *,char *);
extern int   getFileStr(char *,char **);

#endif

