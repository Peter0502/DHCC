/*************************************************************
* �� �� ��: gD002.c
* ����������ɾ�����ձ���,�������ɴ�Ž��ձ����Ŀ¼
*
* ��    ��: jack
* �������: 2003��3��5��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

gD002()
{
	int ret = 0;
	char command[64];
	char dirname[64];

	ret=sql_begin(); /*������*/
	if( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	/*------- ɾ��Ŀ¼ -------*/
	memset( command,0x00,sizeof(command) );
	sprintf( command,"rm -f -r %s/report",getenv("HOME") );

	ret = system( command );
	if( ret )
	{
		sprintf( acErrMsg,"ɾ��command[%s]����[%d]run failed",command,ret );
		WRITEMSG  
		goto ErrExit;
	}

	sprintf( acErrMsg,"ɾ��Ŀ¼[%s]�ɹ�!!!",command );
	WRITEMSG

	/*------- ������Ŀ¼ -------*/
	memset( dirname,0x00,sizeof(dirname) );
	sprintf( dirname,"%s/report",getenv("HOME") );
	ret = modify_dir_main(dirname);
	if( ret )
	{
		sprintf(acErrMsg,"����Ŀ¼[%s]ʧ��",dirname );
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"�������ձ�����Ŀ¼[%s]�ɹ�!!!",dirname );
	WRITEMSG

	/*-------- ��������������Ŀ¼ ----------*/
	ret = create_brno_dir();
	if( ret )
	{
		sprintf(acErrMsg,"������������Ŀ¼ʧ��");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"������Ŀ¼�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"������Ŀ¼ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*------------- ����Ŀ¼���� -------------*/
int modify_dir_main(pathname)
char pathname[100];
{
	int  ret;

	ret=access(pathname,F_OK);
	if( ret && errno==ENOENT )
	{
		ret=mkdir(pathname,S_IRWXU|S_IRWXG|S_IRWXO);
		if( ret )
		{
			sprintf(acErrMsg,"mkdir failed err=%d,pathname=%s",errno,pathname);
			WRITEMSG
			strcpy(g_pub_tx.reply,"0210");
			goto ErrExit;
		}
		sprintf(acErrMsg,"Ŀ¼�����ɹ�[%s]",pathname);
		WRITEMSG
	}
	else if( ret )
	{
		sprintf(acErrMsg,"access() failed err=%d,pathname=%s",errno,pathname);
		WRITEMSG
		strcpy(g_pub_tx.reply,"0210");
		goto ErrExit;
	}

OkExit:
	return(0);
ErrExit:
	return(1);
}

/*------- ѭ������ÿ�������ı�����Ŀ¼ -------*/ 
int create_brno_dir()
{
	int ret = 0;
	char  pathname[100];

	struct com_branch_c sCom_branch;

	ret = Com_branch_Dec_Sel ( g_pub_tx.reply , "1 = 1 order by br_no");
	if( ret )
	{
		sprintf(acErrMsg,"ִ��Com_branch_Dec_Sel����[%s]!!!",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while(1) 
	{
		memset(&sCom_branch,0x0,sizeof(sCom_branch));
		ret = Com_branch_Fet_Sel ( &sCom_branch, g_pub_tx.reply );
		if (ret == 100)
		{
			break;
		}
		if( ret )
		{
			sprintf(acErrMsg,"ִ��Com_branch_Fet_Sel����[%s]!!!",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		pub_base_strpack( sCom_branch.br_no );

		sprintf(pathname,"%s/report/%s",getenv("HOME"),sCom_branch.br_no );

		if( modify_dir_main( pathname ) ) 
		{
			goto ErrExit;
		}
	}

OkExit:
  	return 0;
ErrExit:
	return 1;
}
