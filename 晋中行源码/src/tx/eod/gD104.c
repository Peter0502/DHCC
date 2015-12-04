/*************************************************************
* �� �� ��: gD104.c
* �������������Ż������ۼƷ�����
*
* ��    ��: jack
* �������: 2003��05��04��
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
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"

gD104()
{
	int ret = 0;

	double dJfs = 0.00;
	double dDfs = 0.00;

	long lYearM = 0;
	long lMonthF = 0;
	long lMonthL = 0;

	struct mob_acct_type_c sMob_acct_type;
	struct mob_sendmail_c sMob_sendmail;
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;

	ret=sql_begin(); /*������*/
	if( ret ) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply , \
			"server_type = '1006'" );
	if( ret ) 
	{
		sprintf( acErrMsg, "ִ��Mob_acct_type_Dec_Selʧ�� !!!" );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x00, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_acct_type_Fet_Selʧ�� !!!" );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sMob_acct_type.ac_no );
		if ( ret )
		{
			sprintf( acErrMsg, "��ѯ[%s]�����˺Ŷ��ձ����", sMob_acct_type.ac_no );
			WRITEMSG
			goto ErrExit;
		}

		lYearM = g_pub_tx.tx_date / 100;
		lMonthF = lYearM * 100 + 1;
		lMonthL = lYearM * 100 + 31;

		ret = sql_sum_double ( "dd_mst_hst" , "tx_amt" , &dJfs , \
			"ac_id = %ld and ac_seqn = 1 and \
			tx_date betwin (%ld, %ld) and add_ind = '0'", \
			sMdm_ac_rel.ac_id, lMonthF, lMonthL);
		if ( ret )
		{
			strcpy( acErrMsg, "�𾴵Ŀͻ�:ϵͳ������,���Ժ�����!" );
			sprintf( acErrMsg, "ͳ�ƻ����˺Ž跽���������!!!ret[%d]" ,ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double ( "dd_mst_hst" , "tx_amt" , &dDfs , \
			"ac_id = %ld and ac_seqn = 1 and add_ind = '1' and \
			tx_date betwin (%ld, %ld)", \
			sMdm_ac_rel.ac_id, lMonthF, lMonthL);
		if ( ret )
		{
			strcpy( acErrMsg, "�𾴵Ŀͻ�:ϵͳ������,���Ժ�����!" );
			sprintf( acErrMsg, "ͳ�ƻ����˺Ŵ������������!!!ret[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( acErrMsg, 0x0, sizeof(acErrMsg) );
		sprintf( acErrMsg, "�𾴵Ŀͻ�:���Ļ����˻�%s�����ۼ�ȡ��%.2fԪ���%.2fԪ.��ҵ����", dJfs, dDfs );

		memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );

		strcpy( sMob_sendmail.tx_code, g_pub_tx.tx_code );
		strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
		strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
		sMob_sendmail.trace_no = g_pub_tx.trace_no;
		sMob_sendmail.tx_date = g_pub_tx.tx_date;
		sMob_sendmail.tx_time = g_pub_tx.tx_time;
		strcpy( sMob_sendmail.opt_no, "110" );
		strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
		strcpy( sMob_sendmail.message, acErrMsg );
		strcpy(	sMob_sendmail.timeflag, "0" );
		sMob_sendmail.t_date = 0;
		sMob_sendmail.t_time = 0;
		strcpy(	sMob_sendmail.send_level, "1" );
		sMob_sendmail.sene_times = 1;
		sMob_sendmail.send_date = 0;
		sMob_sendmail.send_time = 0;
		sMob_sendmail.sxf_amt = 0.00;
		strcpy(	sMob_sendmail.proc_flag, "1" );

		ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
		if ( ret != 0 ) 
		{
			sprintf( acErrMsg, "дmob_sendmail��ʧ��!!![%s][%d]", sMob_acct_type.ac_no, ret );
			WRITEMSG
			goto ErrExit;
		}
	}

	Mob_acct_type_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����ͳ�Ƶ���ĩ�������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"����ͳ�Ƶ���ĩ�������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
