#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_base_time.c
* 功能描述：   
*              pub_base_TimeForm    将时间类型结构转化为指定格式的字符串
*              pub_base_GetTime     取得系统时间
*              pub_base_GetTime2    取得系统时间
*              pub_base_nCalElapse  取得两个时间间隔的秒数
*               
* 作    者:
* 完成日期：   2003年12月16日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/

# include	<sys/types.h>
# include	<stdio.h>
# include	<time.h>
#include <sys/timeb.h>/**add zgf 20131017***/

# define	BUF_LEN		80

/**********************************************************************
 * 函 数 名：  pub_base_TimeForm
 * 函数功能：  将时间类型结构转化为指定格式的字符串
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：ptm：   时间结构
 *             format：转换后的字符串
 *      
 *     输  出：无
 *
 *     返回值：转换后的字符串
 *
 * 注   释：   m .... month of year - 01 to 12    
 *             d .... day of month - 01 to 31
 *             y .... four ditigs of year
 *             h .... hour - 00 to 23
 *             i .... minute - 00 to 59
 *             s .... second - 00 to 59  
 *             n .... new line
 *
 * 修改历史：
 *                   
********************************************************************/

/*
extern	long	time();
*/
char *pub_base_TimeForm( struct tm *ptm, char *format)
{
	static	char	buff[BUF_LEN];

	int		ctl_flg = 0;
	int		i = 0;
	int		year;

	if (format == NULL)
		return (char *)ptm;
	else
	if (strcmp(format, "ASC") == 0)
		return asctime(ptm);

	do	{
		if ( ctl_flg )	{
		  ctl_flg -= 1;
		  switch(*format)	{
			  case 	'y' :	
					year  = 1900 + ptm->tm_year;
					sprintf(&buff[i], "%d", year);
					i = i+4;
					break;
			  case	'm' :	
					sprintf(&buff[i], "%02d",ptm->tm_mon+1);
					i = i+2;	
					break;
			  case	'd' :
					sprintf(&buff[i], "%02d", ptm->tm_mday);
					i = i+2;
					break;
			  case	'h' :	
					sprintf(&buff[i], "%02d", ptm->tm_hour);
					i = i+2;
					break;
			  case	'i' :	
					sprintf(&buff[i], "%02d", ptm->tm_min);
					i = i+2;
					break;
			  case	's' :	
					sprintf(&buff[i], "%02d", ptm->tm_sec);
					i = i+2;
					break;
			  case  'n' :	
					buff[i--] = '\n';
					i = i+2;
					break;
			  default :	
					buff[i] = '%', buff[i+1] = *format;
					i = i+2;
					break;
		  }
		} else
		if (*format == '%') 
			ctl_flg = 1;
		else
			buff[i++] = *format;
	} while(*(++format) != 0);

	buff[i] = 0;
	return buff;
}
/**********************************************************************
 * 函 数 名：  pub_base_GetTime
 * 函数功能：  取得系统时间
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：format：格式
 *      
 *     输  出：无
 *
 *     返回值：pub_base_TimeForm(localtime(&clck), format)
 *
 * 修改历史：
 *                   
********************************************************************/
char *pub_base_GetTime( char *format )
{
	time_t		clck;

	clck = time((time_t *)0);
	return pub_base_TimeForm( localtime( &clck ) , format );
}


/**********************************************************************
 * 函 数 名：  pub_base_GetTime2
 * 函数功能：  取得系统时间
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：无
 *      
 *     输  出：hour：   小时
 *             minute： 分钟
 *             secone:  秒
 *
 *     返回值：无
 *
 * 修改历史：
 *                   
********************************************************************/

void pub_base_GetTime2(int *hour, int *minute, int *second)
{
        time_t t;
        struct tm *local_time;

        t = time(0);
        local_time = localtime(&t);

        *hour   = local_time->tm_hour;
        *minute = local_time->tm_min;
        *second = local_time->tm_sec;

        return ;
}


/**********************************************************************
 * 函 数 名：  pub_base_nCalElapse
 * 函数功能：  取得两个时间间隔的秒数
 * 作    者：  
 * 完成时间：  2003年12月16日
 *
 * 参    数：
 *     输  入：OutTime：相对早的时间
 *             InTime： 相对晚的时间
 *      
 *     输  出：无
 *
 *     返回值：两个时间相差的秒数
 *
 * 修改历史：
 *                   
********************************************************************/
int pub_base_nCalElapse(char OutTime[7], char InTime[7])
{
    char    sTimeTmp[3];
    int nOutTime, nInTime;

    MEMCPY_DEBUG(sTimeTmp, OutTime, 2);
    sTimeTmp[2] = '\0';

    nOutTime = atoi(sTimeTmp) * 3600;

    MEMCPY_DEBUG(sTimeTmp, OutTime + 2, 2);
    sTimeTmp[2] = '\0';

    nOutTime = nOutTime + atoi(sTimeTmp) * 60;
    MEMCPY_DEBUG(sTimeTmp, OutTime + 4, 2);
    sTimeTmp[2] = '\0';

    nOutTime = nOutTime + atoi(sTimeTmp);

    MEMCPY_DEBUG(sTimeTmp, InTime, 2);
    sTimeTmp[2] = '\0';

    nInTime = atoi(sTimeTmp) * 3600;

    MEMCPY_DEBUG(sTimeTmp, InTime + 2, 2);
    sTimeTmp[2] = '\0';

    nInTime = nInTime + atoi(sTimeTmp) * 60;

    MEMCPY_DEBUG(sTimeTmp, InTime + 4, 2);
    sTimeTmp[2] = '\0';

    nInTime = nInTime + atoi(sTimeTmp);

    return(nInTime - nOutTime);
}

/***************************************************************************/
/* 程序功能 : 取得日期和时间, 避免2000年问题                               */
/* 作    者 : 唐燕云  Tang Yanyun                                          */
/* 日    期 : 1997年12月05日                                               */
/* 使用限制 : 无                                                           */
/* 参数意义 : char *pcInputTimeStr 字符串,指定起始时间,如为空指针,表示当前 */
/*                  系统时间为起始时间.字符串格式为(必需有字符串结束符) :  */
/*                 "年月日时分秒微秒"(YYYYmmddHHMMSSMicroS)                */
/*            int   iSecNum    取当前系统时间经过多少秒后的时间(可为负数)  */
/*            char *pcTimeStr  字符串,函数执行成功后返回21个字节的字符串,  */
/*                系统日期和时间按"年月日时分秒微秒"(YYYYmmddHHMMSSMicroS) */
/*            的顺序排列, 其中年用4 位表示( 返回时已包含字符串结束符 )     */
/* 返值意义 : char *  返回取得的字符串( 即pcTimeStr )                      */
/* 注意事项 : 调用此函数时函数的参数须有大于等于21个字节的存贮空间, 否则会 */
/*            导致程序运行极不稳定. 使用此函数时须包含time.h :             */
/*             #include <time.h>                                           */
/* 修改记录 : add zgf 20131017                                                             */
/***************************************************************************/
char* appGetTime(char *pcInputTimeStr,int iSecNum,char *pcTimeStr)
{
	struct tm stTime;
#ifdef WIN32
	struct _timeb stTimeB;
#else
	struct  timeb stTimeB;
#endif

	memset((char *)&stTime ,'\0',sizeof(struct tm));
#ifdef WIN32
	memset((char *)&stTimeB,'\0',sizeof(struct _timeb));
#else
	memset((char *)&stTimeB,'\0',sizeof(struct  timeb));
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
	memcpy(&stTime,localtime((time_t *)&(stTimeB.time)),sizeof(struct tm));
#else
	memcpy(&stTime,localtime((long *)&(stTimeB.time)),sizeof(struct tm));
#endif
	strftime(pcTimeStr,(size_t)(14 + 1),"%Y%m%d%H%M%S",&stTime);
	if (pcInputTimeStr == NULL) {
		sprintf(pcTimeStr + 14,"%.3ld",stTimeB.millitm);
	} else {
		strcpy( pcTimeStr + 14,pcInputTimeStr + 14);
	}

	return(pcTimeStr);
}