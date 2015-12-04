/*************************************************
* �� �� ��:  spB607.c
* ���������� ��ȨͶ�ʷֺ����
*
* ��    ��:  rob
* ������ڣ� 2003��1��25��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"
#include "com_parm_c.h"
#include "mo_city_c.h"

int spB607()
{
    struct mo_righ_invest_c sMo_righ_invest;
	struct com_parm_c	sCom_parm;
	struct mo_city_c	mo_city_c;
    char zzlx[2];
	long   tmp_date=0,tmp_trace_no=0;
	double tmp_amt=0.00;
    int ret;
  
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0580",sMo_righ_invest.pact_no);
    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0690",g_pub_tx.ct_ind);
	get_zd_data("0700",zzlx);

    /* �޸�Ȩ����Ͷ�ʵǼǲ� */
    ret = Mo_righ_invest_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",sMo_righ_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error [%d] ",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_righ_invest_Fet_Upd(&sMo_righ_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"fet for update error [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_righ_invest.pact_no,ret);
        WRITEMSG
        Mo_righ_invest_Clo_Upd();
        goto ErrExit;
    }

    sMo_righ_invest.incm_bal = sMo_righ_invest.incm_bal + g_pub_tx.tx_amt1;
    sMo_righ_invest.wrk_date = g_pub_tx.tx_date;
    sMo_righ_invest.trace_no = g_pub_tx.trace_no;

    ret = Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Update mo_righ_invest error ! [%d]",ret);
        WRITEMSG
        Mo_righ_invest_Clo_Upd();
        goto ErrExit;
    }
    g_pub_tx.hst_ind[0] = '1';
    strcpy(g_pub_tx.brf,"��ȨͶ�ʷֺ����");

	if ( zzlx[0]=='1' ) 			/*** ͬ�� ***/
	{
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"2"); 	/* ���� */
		strcpy(mo_city_c.cd_ind,"2");		/* ����� */
		get_zd_double("0390",&mo_city_c.bill_amt);
		strcpy(mo_city_c.sts,"P");
		strcpy(mo_city_c.bill_type,"1"); /* Ʊ�����ࣺͬ�� */
		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
		strcpy(mo_city_c.payee_ac_no,"51403");
		strcpy(mo_city_c.payee_name,"����Ͷ������");
		get_zd_data("0300",mo_city_c.payer_ac_no);
		get_zd_data("0270",mo_city_c.payer_name);
		ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"�Ǽ�ͬ�ǵǼǲ�����!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�������������[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
	}
	else if( zzlx[0]=='2' )			/*** ��� ***/
	{
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='DEKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�������������[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
		/** �޸Ĵ��֧������״̬ **/
		get_zd_long("0440",&tmp_date);
		get_zd_long("0450",&tmp_trace_no);
		get_zd_double("0390",&tmp_amt);
		ret = pub_mbfe_upd_recv(tmp_date,tmp_trace_no,"51403",tmp_amt);
		if( ret )
		{
			sprintf( acErrMsg, "�޸Ĵ�����ʵǼǲ�����[%d]!",ret );
			WRITEMSG
			goto ErrExit;
		}
	}

    if (g_pub_tx.ct_ind[0] == '1')
    {
        /* �����ֽ��ӽ��� */
        goto OkExitCash;
    }
    else
    {
        /* ����ת���ӽ��� */
    	get_zd_data("0300",g_pub_tx.ac_no);
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
        goto OkExit;
    }

OkExitCash:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;


OkExit:
    strcpy( g_pub_tx.reply, "0002" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
