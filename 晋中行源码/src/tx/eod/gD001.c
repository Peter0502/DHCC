/*************************************************************
* �� �� ��: gD001.c
* ������������ǰ��鲿��
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
#include "com_sys_parm_c.h"
#include "mbfe_sys_ctl_c.h"

gD001()
{
	int ret = 0;
	int br_type;

	struct com_sys_parm_c sCom_sys_parm;
	struct mbfe_sys_ctl_c sMbfe_sys_ctl;

	memset(&sCom_sys_parm,0x0,sizeof(sCom_sys_parm));
	memset(&sMbfe_sys_ctl,0x0,sizeof(sMbfe_sys_ctl));

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();
	/* ��齻�׻����Ƿ��ǿƼ���0 */
	br_type = pub_base_ChkBrno( g_pub_tx.tx_br_no );
	if ( br_type < 0 )
	{
		sprintf( acErrMsg, "�������ִ�л�����" );
		WRITEMSG
		goto ErrExit;
	}
	if ( br_type!='0' )
	{
		sprintf( acErrMsg, "�˻����ǿƼ�����Ȩִ�����մ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S043" );
		goto ErrExit;
	}
	/*-------------------------------------------------*/

	/* ���ݹ���ϵͳ�������ж�״̬�Ƿ���1 */
	ret = pub_base_GetSysparm( &sCom_sys_parm );
	if ( ret )
	{
		sprintf( acErrMsg, "ȡ����ϵͳ�������!" );
		WRITEMSG
		goto ErrExit;			
	}
	/* �ж��Ƿ����˱��� */
	if ( sCom_sys_parm.sys_sts != 1 )
	{
		sprintf( acErrMsg, "�����������ݱ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "G002" );
		goto ErrExit;
	}
	
	/* ȡ�����еĻ�������,�ж��Ƿ��Ѿ�ǩ��,�Ƽ�����ǩ�� */
	ret = sql_count( "com_branch", "wrk_sts = '1' and br_type != '0'" );
	if ( ret < 0 )
	{
		sprintf( acErrMsg, "ִ��sql_count ����,[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret > 0 )
	{
		sprintf( acErrMsg, "���л���û��ǩ��" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "G003" );
		goto ErrExit;
	}
	/*-----------------------------------------*/

	/* �жϴ��֧��ϵͳ�Ƿ��Ѿ����� JYW*/
/*****JYW
	ret = Mbfe_sys_ctl_Sel( g_pub_tx.reply, &sMbfe_sys_ctl, "par_code='05'" );
	if( ret )
	{
		sprintf( acErrMsg, "�����֧��ϵͳ�Ƿ����г���!");
		WRITEMSG
		goto ErrExit;
	}
	pub_base_strpack(sMbfe_sys_ctl.par_value);
	if( strcmp( sMbfe_sys_ctl.par_value, "00" ) )
	{
		sprintf( acErrMsg, "�����֧��ϵͳ��δ����!");
		strcpy( g_pub_tx.reply, "G012" );
		WRITEMSG
		goto ErrExit;
	}
****JYW*****/
	/*-----------------------------------------*/

	/*-------- �޸�ϵͳ״̬Ϊ����״̬ ---------*/
	ret = sql_execute("update com_sys_parm set sys_sts = 2 ");
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�ϵͳ״̬Ϊ����ʱ����!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "G005" );
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ǰ���ͨ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ǰ���ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
