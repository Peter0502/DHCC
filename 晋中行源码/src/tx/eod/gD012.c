/*************************************************
* �� �� ��: gD012.c
* �������������̺�����Զ�ͳ����Ϣ(��ĩ���,���ڵ���֪ͨ,
			�����֪ͨ)
*
* ��    ��: jack
* �������: 2004��07��08��
* �޸�����:
* �� �� ��:
* �޸�����:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"

gD012()
{
	int ret=0;

	char cMessage[141];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	struct ln_mst_c sLn_mst;
	struct ln_parm_c sLn_parm;
	struct mob_acct_type_c sMob_acct_type;
	struct mob_sendmail_c sMob_sendmail;
	struct com_sys_parm_c sCom_sys_parm;

	ret = sql_begin();
	if ( ret )
	{
		sprintf( acErrMsg, "������ʧ��!!" );
		WRITEMSG
		goto ErrExit;
	}

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	memset( &sCom_sys_parm, 0x0, sizeof(sCom_sys_parm) );
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	if ( ret )
	{
		sprintf( acErrMsg, "��ѯϵͳ���ڳ���!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/*-----------���̺�ͳ��,ȡ��������-------------*/
	ret = pub_base_end_month( sCom_sys_parm.lst_date );
	if ( ret == 0 )
	{
		/*--------------ͳ�ƻ�����ĩ���-------------*/
		ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply, \
				"server_type='1003'" );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_acct_type_Dec_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
        	
		while (1)
		{
			memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
			ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf( acErrMsg, "ִ��Mob_acct_type_Fet_Sel��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
				"ac_no = '%s'", sMob_acct_type.ac_no );
			if ( ret )
			{
				sprintf( acErrMsg, "--->��ȡ�˻�[%s]���ʱ����[%d]!!", \
					sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;	/*�����ɹ�������*/
			}

			memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
			ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
				"ac_id = %ld", sMdm_ac_rel.ac_id );
			if ( ret )
			{
				sprintf( acErrMsg, "--->��ȡ����[%s]���ļ�����[%d]!!", \
					sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;	/*�����ɹ�������*/
			}

			memset( cMessage, 0x0, sizeof(cMessage) );
			memset( &sMob_sendmail, 0x0, sizeof(sMob_sendmail) );

			sprintf( cMessage, "�𾴵Ŀͻ�:�����˻�%s��ĩ�������Ϊ%.2lfԪ", \
				sMob_acct_type.ac_no, sDd_mst.bal );

			strcpy( sMob_sendmail.tx_code, "�Զ�" );
			strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
			sMob_sendmail.ac_id = sMob_acct_type.ac_id;
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = sCom_sys_parm.sys_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
			strcpy( sMob_sendmail.message, cMessage );
			strcpy(	sMob_sendmail.timeflag, "0" );
			sMob_sendmail.t_date = 0;
			sMob_sendmail.t_time = 0;
			strcpy(	sMob_sendmail.send_level, "1" );
			sMob_sendmail.sene_times = 1;
			sMob_sendmail.send_date = 0;
			sMob_sendmail.send_time = 0;
			sMob_sendmail.sxf_amt = 0.00;
			strcpy(	sMob_sendmail.proc_flag, "0" );
        		
			ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
			if ( ret ) 
			{
				sprintf( acErrMsg, "д�˺�[%s]��¼mob_sendmail\
					��ʧ��!!![%d]", sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;		/*�����ɹ������*/
			}
				
		}

		Mob_acct_type_Clo_Sel();
	}

	/*---------------------ͳ�ƶ��ڵ���------------------------*/
	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply, "server_type='1004'" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mob_acct_type_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_acct_type_Fet_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sMob_acct_type.ac_no );
		if ( ret )
		{
			sprintf( acErrMsg, "--->��ȡ�˻�[%s]���ʱ����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	/*�����ɹ�������*/
		}

/*-----������¼------------------------------------------------
		memset( &sTd_mst, 0x0, sizeof(sTd_mst) );
		ret = Td_mst_Sel( g_pub_tx.reply, &sTd_mst, \
			"ac_id = %ld and mtr_date = %ld", \
			sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->��ȡ����[%s]���ļ�����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	
		}
--------------------------------------------------------------*/

		ret = Td_mst_Dec_Sel( g_pub_tx.reply , \
			"ac_id = %ld and mtr_date = %ld", \
			sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->���嶨��[%s]���ļ���ѯ�α����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	
		}
		
		while (1)
		{
			memset( &sTd_mst, 0x0, sizeof(sTd_mst) );
			ret = Td_mst_Fet_Sel( &sTd_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				continue;	/*�����ɹ�������������¼*/
			}
			
			memset( &sTd_parm, 0x0, sizeof(sTd_parm) );
			ret = Td_parm_Sel( g_pub_tx.reply, &sTd_parm,  \
				"prdt_no = '%s'", sTd_mst.prdt_no );
			if ( ret )
			{
				continue;
			}
			pub_base_strpack( sTd_parm.title );

			memset( cMessage, 0x0, sizeof(cMessage) );
			memset( &sMob_sendmail, 0x0, sizeof(sMob_sendmail) );
                	
			sprintf( cMessage, "�𾴵Ŀͻ�:����%s�˺�Ϊ%s�ѵ���\
				���Ϊ%.2lfԪ", sTd_parm.title, sMob_acct_type.ac_no, sTd_mst.bal );
                	
			strcpy( sMob_sendmail.tx_code, "�Զ�" );
			strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
			sMob_sendmail.ac_id = sMob_acct_type.ac_id;
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = sCom_sys_parm.sys_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
			strcpy( sMob_sendmail.message, cMessage );
			strcpy(	sMob_sendmail.timeflag, "0" );
			sMob_sendmail.t_date = 0;
			sMob_sendmail.t_time = 0;
			strcpy(	sMob_sendmail.send_level, "1" );
			sMob_sendmail.sene_times = 1;
			sMob_sendmail.send_date = 0;
			sMob_sendmail.send_time = 0;
			sMob_sendmail.sxf_amt = 0.00;
			strcpy(	sMob_sendmail.proc_flag, "0" );
        		
			ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "д�˺�[%s]��¼mob_sendmail\
					��ʧ��!!![%d]", sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;		/*�����ɹ������*/
			}
		}
		Td_mst_Clo_Sel();
	}

	Mob_acct_type_Clo_Sel();

	/*---------------------ͳ�ƴ����------------------------*/
	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply, "server_type='1005'" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mob_acct_type_Dec_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_acct_type_Fet_Sel��[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sMob_acct_type.ac_no );
		if ( ret )
		{
			sprintf( acErrMsg, "--->��ȡ�˻�[%s]���ʱ����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	/*�����ɹ�������*/
		}
/*-----������¼------------------------------------------------
		memset( &sLn_mst, 0x0, sizeof(sLn_mst) );
		ret = Ln_mst_Sel( g_pub_tx.reply, &sLn_mst, \
			"ac_id = %ld and mtr_date = %ld", \
			sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->��ȡ����[%s]���ļ�����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;
		}
-----------------------------------------------------------*/

		ret = Ln_mst_Dec_Sel( g_pub_tx.reply , "ac_id = %ld and \
			mtr_date = %ld", sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->�������[%s]���ļ���ѯ�α����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	
		}
		
		while (1)
		{
			memset( &sLn_mst, 0x0, sizeof(sLn_mst) );
			ret = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				continue;
			}
			
			memset( &sLn_parm, 0x0, sizeof(sLn_parm) );
			ret = Ln_parm_Sel( g_pub_tx.reply, &sLn_parm, \
				"prdt_no = '%s'", sTd_mst.prdt_no );
			if ( ret )
			{
				continue;
			}
		
			memset( cMessage, 0x0, sizeof(cMessage) );
			memset( &sMob_sendmail, 0x0, sizeof(sMob_sendmail) );
                	
			sprintf( cMessage, "�𾴵Ŀͻ�:����%s�˺�%s�ѵ�\
				�����Ϊ%.2lfԪ", sLn_parm.title, sMob_acct_type.ac_no, sLn_mst.bal );
                	
			strcpy( sMob_sendmail.tx_code, "�Զ�" );
			strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = sCom_sys_parm.sys_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
			strcpy( sMob_sendmail.message, cMessage );
			strcpy(	sMob_sendmail.timeflag, "0" );
			sMob_sendmail.t_date = 0;
			sMob_sendmail.t_time = 0;
			strcpy(	sMob_sendmail.send_level, "1" );
			sMob_sendmail.sene_times = 1;
			sMob_sendmail.send_date = 0;
			sMob_sendmail.send_time = 0;
			sMob_sendmail.sxf_amt = 0.00;
			strcpy(	sMob_sendmail.proc_flag, "0" );
        		
			ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
			if ( ret ) 
			{
				sprintf( acErrMsg, "д�˺�[%s]��¼mob_sendmail\
					��ʧ��!!![%d]", sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;		/*�����ɹ������*/
			}
			
		}
		Ln_mst_Clo_Sel();
	}

	Mob_acct_type_Clo_Sel();


OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->ͳ�ƶ����Զ�������Ϣ�ɹ�!!!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	sql_rollback();
    sprintf(acErrMsg,"--->ͳ�ƶ����Զ�������Ϣʧ��[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
