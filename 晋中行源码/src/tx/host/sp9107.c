/*************************************************
* �� �� ��: sp9107.c
* �����������ļ�����
*
* ��    ��: rob
* ������ڣ�20030322
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

int sp9107( )
{
    int ret;
    char 	filename[256];
	char	fileType[2];			/* ɾ����־         */
									/* 0 -- ���ձ���    */
									/* 1 -- ��������    */
	char 	fileName[64];			/* �ļ��� 			*/
	char	pathName[256];			/* Ŀ¼��           */
	char	fullName[512];			/* ��·�����ļ��� 	*/
	char	l_cmd[256];				/* ����             */
	char	l_str_date[9];			/* �ַ�������       */
	long	l_start_date;			/* ɾ����ʼ���� 	*/
	long	l_end_date;				/* ɾ����ֹ���� 	*/
	long	l_date;					
    long	l_sys_date;				/* ϵͳ���� 		*/

    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    memset( fileType, 0x00, sizeof(fileType) );
	memset( fileName, 0x00, sizeof(fileName) );
	memset( pathName, 0x00, sizeof(pathName) );
	memset( fullName, 0x00, sizeof(fullName) );

    get_zd_data("0670",fileType);
    get_zd_long("0440",&l_start_date);
    get_zd_long("0450",&l_end_date);

    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
   
    l_sys_date = com_sys_parm_c.sys_date;

    if( l_end_date > l_sys_date )
    {
        sprintf(acErrMsg,"��ֹ���ڲ��ܴ���ϵͳ����");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O147");
        goto ErrExit;
    }

    if( l_start_date > l_end_date )
    {
        sprintf(acErrMsg,"��ʼ���ڴ�����ֹ����");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O148");
        goto ErrExit;
    }

    if( l_end_date == 0 )
    {
        sprintf(acErrMsg,"��ֹ���ڱ�����д");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O149");
        goto ErrExit;
    }

    if( fileType[0] == '0' )		/* ���ݿⱸ���ļ� */
	{
		sprintf(pathName,"%s",getenv("BFLDIR"));	/* Ŀ¼�� */
	}else							/* �����ļ� */
	{
		sprintf(pathName,"%s",getenv("RPTDIR"));
	}

	l_date = l_end_date;

    for(;l_date >= l_start_date;)
	{
		if( fileType[0] == '0' )
		{
			memset( l_str_date , 0x00, sizeof( l_str_date));
			memset( fullName , 0x00, sizeof( fullName));
			memset( l_cmd , 0x00, sizeof( l_cmd));

			/* ɾ��Ŀ¼ */
			sprintf(l_str_date, "%ld", l_date);
			sprintf(fullName,"%s/%s", pathName, l_str_date);
			sprintf(l_cmd, "rm -fr %s", fullName);
			sprintf(acErrMsg,"l_cmd = [%s]",l_cmd);
            WRITEMSG
			system(l_cmd);

			memset(fileName, 0x00, sizeof(fileName));
			memset(fullName, 0x00, sizeof(fullName));
			memset(l_cmd, 0x00, sizeof(l_cmd));

			/* ɾ���ļ�: RZQYYYYMMDD */
			sprintf(fileName, "RZQ%s", l_str_date);
			sprintf(fullName, "%s/%s", pathName, fileName);
			sprintf(l_cmd, "rm -fr %s", fullName);
			sprintf(acErrMsg,"l_cmd = [%s]",l_cmd);
            WRITEMSG
			system(l_cmd);
		}
        else
		{
			memset( fileName, 0x00, sizeof( fileName ));
			memset( fullName, 0x00, sizeof( fullName ));
			memset(l_cmd, 0x00, sizeof( l_cmd ));
			memset(l_str_date, 0x00, sizeof( l_str_date ));

			sprintf(l_str_date, "%ld", l_date);
			sprintf(fileName, "rpt.*.%s.Z",l_str_date);
			sprintf(fullName, "%s/%s", pathName, fileName);
			sprintf(l_cmd, "rm -fr %s", fullName);
			system(l_cmd);
		}
		
		ret = pub_base_deadlineD(l_date,-1,&l_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ȡǰһ�����ڴ���!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O150");
            goto ErrExit;
        }
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�ļ�����ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ļ�����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
