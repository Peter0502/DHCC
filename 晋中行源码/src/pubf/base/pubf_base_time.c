#include "find_debug.h" 
/*************************************************
* �� �� ��:    pubf_base_time.c
* ����������   
*              pub_base_TimeForm    ��ʱ�����ͽṹת��Ϊָ����ʽ���ַ���
*              pub_base_GetTime     ȡ��ϵͳʱ��
*              pub_base_GetTime2    ȡ��ϵͳʱ��
*              pub_base_nCalElapse  ȡ������ʱ����������
*               
* ��    ��:
* ������ڣ�   2003��12��16��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/

# include	<sys/types.h>
# include	<stdio.h>
# include	<time.h>
#include <sys/timeb.h>/**add zgf 20131017***/

# define	BUF_LEN		80

/**********************************************************************
 * �� �� ����  pub_base_TimeForm
 * �������ܣ�  ��ʱ�����ͽṹת��Ϊָ����ʽ���ַ���
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺ptm��   ʱ��ṹ
 *             format��ת������ַ���
 *      
 *     ��  ������
 *
 *     ����ֵ��ת������ַ���
 *
 * ע   �ͣ�   m .... month of year - 01 to 12    
 *             d .... day of month - 01 to 31
 *             y .... four ditigs of year
 *             h .... hour - 00 to 23
 *             i .... minute - 00 to 59
 *             s .... second - 00 to 59  
 *             n .... new line
 *
 * �޸���ʷ��
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
 * �� �� ����  pub_base_GetTime
 * �������ܣ�  ȡ��ϵͳʱ��
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺format����ʽ
 *      
 *     ��  ������
 *
 *     ����ֵ��pub_base_TimeForm(localtime(&clck), format)
 *
 * �޸���ʷ��
 *                   
********************************************************************/
char *pub_base_GetTime( char *format )
{
	time_t		clck;

	clck = time((time_t *)0);
	return pub_base_TimeForm( localtime( &clck ) , format );
}


/**********************************************************************
 * �� �� ����  pub_base_GetTime2
 * �������ܣ�  ȡ��ϵͳʱ��
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺��
 *      
 *     ��  ����hour��   Сʱ
 *             minute�� ����
 *             secone:  ��
 *
 *     ����ֵ����
 *
 * �޸���ʷ��
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
 * �� �� ����  pub_base_nCalElapse
 * �������ܣ�  ȡ������ʱ����������
 * ��    �ߣ�  
 * ���ʱ�䣺  2003��12��16��
 *
 * ��    ����
 *     ��  �룺OutTime��������ʱ��
 *             InTime�� ������ʱ��
 *      
 *     ��  ������
 *
 *     ����ֵ������ʱ����������
 *
 * �޸���ʷ��
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
/* �޸ļ�¼ : add zgf 20131017                                                             */
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