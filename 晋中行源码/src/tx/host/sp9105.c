/*************************************************
* �� �� ��: sp9105.c
* �������������ݻָ�
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
#define EXTERN
#include "public.h"

int sp9105( )
{
    int ret;
    long qlrq;
    char dev_type[2];
    char str_tmp[256];
    char comm[30];
    char db_path[30],db_name[20];

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
 
    get_zd_data("0670",dev_type);
	get_zd_long("0440",&qlrq);

    memset(db_path, 0x00, sizeof(db_path));
	memset(db_name, 0x00, sizeof(db_name));

	strcpy(db_path, getenv("BFLDIR"));
	strcpy(db_name, getenv("DBNAME"));

    sprintf(acErrMsg,"===[%s]===",dev_type);
    WRITEMSG

	/******�����ڴŴ���******/
	if ( dev_type[0] == '1' )
    {
		sprintf(str_tmp,"%s",db_path);
		chdir(str_tmp);
		sprintf(comm,"tar xv RZQ%d",qlrq);
		sprintf(acErrMsg,"ȡ�Ŵ����� [%s]",comm);
        WRITEMSG

		if (system(comm))
        {
			sprintf(acErrMsg,"�Ŵ����޴����ڱ��� [%s]",comm);
            WRITEMSG
			strcpy(g_pub_tx.reply,"O143");
            goto ErrExit;
		}
	}

    /***** ������Ӳ���� ****/
    /***  ���뵽����Ŀ¼  ***/
	sprintf(acErrMsg,"cd [%s]",db_path);
    WRITEMSG
	chdir(db_path);
		
	/*** ��������Ŀ¼  **/
	sprintf( comm, "rm -fr %d", qlrq);
	system( comm );

	/***  tar �����ļ�  ***/
	sprintf( comm, "tar xvf RZQ%d", qlrq );
	sprintf(acErrMsg,"ȡӲ�̱��� [%s]",comm);

	if ( system(comm) )
    {
		sprintf(acErrMsg,"Ӳ�����޴����ڱ��� [%s]",comm);
        WRITEMSG
		strcpy(g_pub_tx.reply,"O143");
        goto ErrExit;
	}
	sprintf(acErrMsg,"tar xvf succuess");
    WRITEMSG

    /***  ���뵽tar ֮���Ŀ¼:��ʱ��ΪĿ¼  ***/
	memset(str_tmp, 0x00, sizeof(str_tmp));
	sprintf(str_tmp,"%s/%d/%s.exp",db_path,qlrq,db_name);
	chdir(str_tmp);

	sprintf(acErrMsg,"[%s]",str_tmp);
    WRITEMSG
	/*** ��ѹ�ļ� ***/
	sprintf( comm, "%s", "uncompress *.Z");
	if (system(comm))
    {
		sprintf(acErrMsg,"��ѹ������ [%s]",comm);
        WRITEMSG
		strcpy(g_pub_tx.reply,"O144");
        goto ErrExit;
	}
	sprintf(acErrMsg,"��ѹ�ļ��ɹ�");
    WRITEMSG

    ret = sql_commit();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�ύ�������!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = sql_close();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�ر����ݿ����!!!");
        WRITEMSG
        goto ErrExit;
    }

    ret = sql_drop();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ɾ�����ݿ����!!!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"ɾ�����ݿ�ɹ� ");
    WRITEMSG

    /***  ��ʼ�������ݿ�  ***/
	sprintf(str_tmp, "%s/%d", db_path, qlrq);
	chdir(str_tmp);
    sprintf(acErrMsg,"str_tmp=[%s]",str_tmp);

	sprintf(comm,"dbimport %s -d datadbs",db_name);
	if (system(comm))
    {
		sprintf(acErrMsg,"�ָ����ݿ�ʧ�� [%s]",comm);
		strcpy(g_pub_tx.reply,"O145");
        goto ErrExit;
	}
	sprintf(acErrMsg,"�ָ����ݿ�ɹ� ");
    WRITEMSG

    sleep(20);
	/***  ������־  ***/
	sprintf(comm,"ontape -s -U %s 1>/tmp/a.txt 2>>/tmp/a.txt",db_name);
	if (system(comm))
	{
		sprintf(acErrMsg,"������־ʧ�� [%s]",comm);
        WRITEMSG
		strcpy(g_pub_tx.reply,"O146");
		return 1;
	}
	sprintf(acErrMsg,"������־�ɹ� ");
	sleep(60);
	/****  Ӳ�ָ̻����ݿ����  ***/

    ret = sql_database();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ݿ����!!");
        WRITEMSG
        goto ErrExit;
    }
    ret = sql_begin();
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ʼ�������!!");
        WRITEMSG
        goto ErrExit;
    }

    
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"���ݻָ��ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���ݻָ�ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
