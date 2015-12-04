/*************************************************************
* �� �� ��: gD023.c
* ���������������������ʼ��
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"


gD023()
{
	int ret = 0;
	struct com_sys_parm_c com_sys_parm_c;

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();

	ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if( ret ) goto ErrExit;

	ret=sql_count( "mo_jl_bal" , "1=1" );
	if( ret<0 ) goto ErrExit;
	if( ret )
	{
		sprintf(acErrMsg,"�Ѿ���������[%d]",ret);
		WRITEMSG
		goto OkExit;
	}

        sprintf(acErrMsg,"�����ʻ����ļ����!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from dd_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"�����ʻ����ļ����!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from td_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"�����ʻ����ļ����!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from ln_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"͸֧�ʻ����ļ����!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from od_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

        sprintf(acErrMsg,"�ڲ��ʻ����ļ����!!");
        WRITEMSG

  	sprintf(acErrMsg,
	"insert into mo_jl_bal select ac_id,ac_seqn,%d,99999999,bal from in_mst",
		com_sys_parm_c.sys_date ); 
	ret=sql_execute( acErrMsg );
	if( ret ) goto ErrExit;

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����ʼ������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G107");
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�����ʼ������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
