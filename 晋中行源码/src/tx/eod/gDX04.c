/*************************************************************
* �� �� ��: gDX04.c
* ��������: ���Ӷ������ѣ�ÿ��1�š�����ǩԼ���Ӷ��˱��¼ netbank_yqdz���������ټ�������
* ǩԼ���¼mob_acct_type�������������Ͷ���֪ͨ��
*
* ��    ��: zgf
* �������: 2014��6��20��
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* �޸�����:
* insert into com_eod_exec values(9010,'7774','gDX04','1�ŵ��Ӷ�������','1111111111','4','1','01','01','2','Y','Y','N','2','Y');
**************************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"
#include "netbank_yqdz_c.h"

gDX04()
{
	int ret=0;

	char cMessage[141];
	char tmpacno[20];
	long note_day;		/* ֪ͨ���� */
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	struct ln_mst_c sLn_mst;
	struct ln_parm_c sLn_parm;
	struct mob_acct_type_c sMob_acct_type;
	struct mob_sendmail_c sMob_sendmail;
	struct com_sys_parm_c sCom_sys_parm;
	struct netbank_yqdz_c sNetbankYqdz;
	memset(tmpacno,0x00,sizeof(tmpacno));
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

	strcpy(g_pub_tx.tx_code,"7774");
	
	/*-----------���̺�ͳ��,ȡ��������-------------*/
	ret = pub_base_end_month( sCom_sys_parm.lst_date );

	/*---------------------ͳ�Ƶ��Ӷ���ǩԼ��Ϣ------------------------*/
	g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply," mob_sts='1' ");	
	if(g_reply_int)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel1( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "ִ��Mob_acct_type_Fet_Sel��[%d]", ret );
			WRITEMSG
			Mob_acct_type_Clo_Sel1();
			goto ErrExit;
		}
		memset(tmpacno,0x00,sizeof(tmpacno));
		memcpy(tmpacno,sMob_acct_type.ac_no,strlen(sMob_acct_type.ac_no));
	
		memset( &sNetbankYqdz, 0x0, sizeof(sNetbankYqdz) );
		ret = Netbank_yqdz_Dec_Sel( g_pub_tx.reply , "ac_id = %ld ", sMob_acct_type.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "--->����Netbank_yqdz[%s]���ļ���ѯ�α����[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			Netbank_yqdz_Clo_Sel();
			goto ErrExit;	
		}
		while (1)
		{
			memset( &sNetbankYqdz, 0x0, sizeof(sNetbankYqdz) );
			ret = Netbank_yqdz_Fet_Sel( &sNetbankYqdz, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "[%s][%d]DECLARE FOR FETCH ERROR [%d]",__FILE__,__LINE__, g_reply_int);
				WRITEMSG
				Netbank_yqdz_Clo_Sel();
				Mob_acct_type_Clo_Sel1();
				goto ErrExit;
			}
			pub_mob_sendmail_ln("***","7774",sMob_acct_type.ac_no,"1",0.00,0.00,0.00);	
		}
		
		Netbank_yqdz_Clo_Sel();
	}

	Mob_acct_type_Clo_Sel1();


OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->1�ŵ��Ӷ������ѷ�����Ϣ�ɹ�!!!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	sql_rollback();
    sprintf(acErrMsg,"--->1�ŵ��Ӷ������ѷ�����Ϣʧ��[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
