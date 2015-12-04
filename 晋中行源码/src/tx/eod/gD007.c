/*************************************************************
* �� �� ��: gD007.c
* ���������������������˶�
* �Ѳ��������ļ��м�¼���������ʵ����˶ԣ����д�ڱ�����
**************************************************************/
#define EXTERN
#include <stdio.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "fin_acct_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"

gD007()
{
	int i=0, j=0;
	struct com_sys_parm_c sComSysParm;
	struct com_branch_c sComBranch;
	struct fin_acct_c sFinAcct;
	struct gl_mst_c sGlMst;
	struct com_item_c sComItem;

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );

    g_reply_int=sql_begin(); /*������*/
	if( g_reply_int )
	{
		sprintf(acErrMsg, "���ݿ���� ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_GetSysparm( &sComSysParm );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ����ϵͳ�������![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}
	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_type='1'" );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "ȡ�������Ļ����Ŵ�![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}

	fin_report( "---------- %ld - ���ƺ�˶���ʼ ----------", sComSysParm.lst_date );

	g_reply_int = Fin_acct_Dec_Sel( g_pub_tx.reply, "tx_date='%ld' and acct_flag='1'", sComSysParm.lst_date );
	if( g_reply_int )
	{
		sprintf(acErrMsg, "�������ʺ˶������α����![%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}

	while(1)
	{
		memset( &sFinAcct, 0x00, sizeof( struct fin_acct_c ) );
		memset( &sGlMst, 0x00, sizeof( struct gl_mst_c ) );
		memset( &sComItem, 0x00, sizeof( struct com_item_c ) );
		g_reply_int = Fin_acct_Fet_Sel( &sFinAcct, g_pub_tx.reply );
		if( g_reply_int == 100 ) break;
		else if( g_reply_int )
		{
			sprintf(acErrMsg, "�������ʺ˶����ȡ�α����![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		i++;
		g_reply_int = Com_item_Sel( g_pub_tx.reply, &sComItem, "acc_hrt='%s'", sFinAcct.ac_no );
		if( g_reply_int == 100 )
		{
			fin_report( " %3d ����Ŀ�š�%7s ������!", i, sFinAcct.ac_no );
			vtcp_log( " %3d ����Ŀ�š�%7s ������!", i, sFinAcct.ac_no );
			continue;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg, "�������ʺ˶����ȡ��Ŀ�����ֳ���![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		g_reply_int = Gl_mst_Sel( g_pub_tx.reply, &sGlMst, 
			"br_no='%s' and cur_no='01' and acc_hrt='%s'", sComBranch.br_no, sComItem.acc_hrt );
		if( g_reply_int == 100 )
		{
			fin_report( " %3d ����Ŀ�����֡�%5s ������!", i, sComItem.acc_hrt );
			vtcp_log( " %3d ����Ŀ�����֡�%5s ������!", i, sComItem.acc_hrt );
			continue;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg, "�������ʺ˶����ȡ��Ӧ���˼�¼����![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}

		pub_base_strpack( sFinAcct.ac_no );
		TRACE;
		/* �跽 ���� ˫�� */
		if( ( sGlMst.dc_ind[0] == '1' && sGlMst.dr_bal == sFinAcct.bal ) || 
			( sGlMst.dc_ind[0] == '2' && sGlMst.cr_bal == sFinAcct.bal ) || 
			( sGlMst.dc_ind[0] == '0' && sFinAcct.bal >= 0 && sGlMst.dr_bal == sFinAcct.bal ) || 
			( sGlMst.dc_ind[0] == '0' && sFinAcct.bal < 0 && sGlMst.cr_bal == -sFinAcct.bal ) )
		{
			fin_report( "   %3d ����Ŀ�š�%7s ������ �跽���: %.2lf", i, sFinAcct.ac_no, sGlMst.dr_bal );
			fin_report( "                                  �������: %.2lf", sGlMst.cr_bal );
		}
		else
		{
			fin_report( "** %3d ����Ŀ�š�%7s ���� �跽���: %.2lf", i, sFinAcct.ac_no, sGlMst.dr_bal );
			fin_report( "                                  �������: %.2lf", sGlMst.cr_bal );
			fin_report( "                            �����ļ������: %.2lf", sFinAcct.bal );
		}
	}
	Fin_acct_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"�������˺˶����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	fin_report( "---------- %ld - �������˺˶����ɹ����� ----------", g_pub_tx.tx_date );
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�������˺˶����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	fin_report( "---------- %ld - �������˺˶����ʧ�ܽ��� ----------", g_pub_tx.tx_date );
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
