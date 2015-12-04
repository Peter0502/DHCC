/*************************************************
* �� �� ��:  spB608.c
* ���������� ��ȨͶ�ʳ��ۼ���
*
* ��    ��:  
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

int spB608()
{
    struct mo_righ_invest_c sMo_righ_invest;
    struct mdm_ac_rel_c sMdm_ac_rel;
	struct com_parm_c	sCom_parm;
	struct mo_city_c  mo_city_c;
    char flag[2],zzlx[2];
    double sol_amt=0.00,buy_amt=0.00,sy_amt=0.00,tmp_amt=0.00;
	long   tmp_date=0,tmp_trace_no=0;
    int ret,sol_num;
  
    memset(&sMo_righ_invest,0x0,sizeof(struct mo_righ_invest_c));
    memset(&sMdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&g_ln_mst,0x0,sizeof(struct in_mst_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0580",sMo_righ_invest.pact_no); /* Ͷ��Э���� */
    get_zd_double("0390",&sol_amt);              /* ������ */
	get_zd_int("0530",&sol_num);				 /* �������� */
	get_zd_double("0400",&buy_amt);				 /* �ɱ��� */
	get_zd_double("0410",&sy_amt);				 /* ������ */
    get_zd_data("0310",g_pub_tx.ac_no);          /* Ͷ���ʺ� */
    get_zd_data("0690",flag);                    /* ��ת��־ */
    get_zd_data("0690",g_pub_tx.ct_ind);         /* ��ת��־ */
    get_zd_data("0700",zzlx);              
 
    ret = Mo_righ_invest_Dec_Upd(g_pub_tx.reply," pact_no='%s' and sts='2' ",sMo_righ_invest.pact_no);
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
    
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s' and \
          ac_seqn = '0'",g_pub_tx.ac_no);   
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ����!!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (sMdm_ac_rel.ac_id != sMo_righ_invest.ac_id)
    {
        sprintf(acErrMsg,"Ͷ���ʺź͵Ǽǲ��е��˺Ų�ͬ!!! [%ld][%ld]",
             sMdm_ac_rel.ac_id,sMo_righ_invest.ac_id);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B058");
        goto ErrExit;
    }
   
    ret = In_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=0",
          sMo_righ_invest.ac_id);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�ڲ��ʻ����ļ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* Ͷ���ʻ�ȡ�� */
    g_pub_tx.tx_amt1= buy_amt;		/*** ���ɱ��ۼ��� ***/
    g_pub_tx.add_ind[0] = '1';		/*** �Ǵ������������� ***/
    g_pub_tx.ac_id = sMo_righ_invest.ac_id;
    g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.brf,"Ȩ����Ͷ�ʳ���");
    g_pub_tx.hst_ind[0] = '1';
	if( pub_base_CompDblVal(g_in_mst.bal,buy_amt)==0 )
		g_pub_tx.svc_ind=9002;
	else
		g_pub_tx.svc_ind=9001;

	if ( pub_base_CompDblVal(buy_amt,0.00)!=0 )
	{
    	if (pub_acct_trance())
    	{
        	sprintf(acErrMsg,"Ͷ���ʻ�ȡ�����!!!");
        	WRITEMSG
        	goto ErrExit;
    	}
    }

    /* �޸�Ȩ����Ͷ�ʵǼǲ���*/
    sMo_righ_invest.wrk_date = g_pub_tx.tx_date;
    sMo_righ_invest.trace_no = g_pub_tx.trace_no;
	sMo_righ_invest.num=sMo_righ_invest.num-sol_num;
	sMo_righ_invest.amt=sMo_righ_invest.amt-buy_amt;
	sMo_righ_invest.incm_bal=sMo_righ_invest.incm_bal+sy_amt;
	if( pub_base_CompDblVal(g_in_mst.bal,0.00)==0 
		&& sMo_righ_invest.num==0 )
    	strcpy(sMo_righ_invest.sts,"*");
    
    ret = Mo_righ_invest_Upd_Upd(sMo_righ_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�޸�Ȩ����Ͷ�ʵǼǲ����� !! [%d]",ret);
        WRITEMSG
        Mo_righ_invest_Clo_Upd();
        strcpy(g_pub_tx.reply,"B036");
        goto ErrExit;
    }
    Mo_righ_invest_Clo_Upd();
    sprintf(acErrMsg,"�޸�Ȩ����Ͷ�ʵǼǲ�  pass!! ");
    WRITEMSG

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
		strcpy(mo_city_c.payee_ac_no,sMdm_ac_rel.ac_no);
		strcpy(mo_city_c.payee_name,"����Ͷ��");
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
		ret = pub_mbfe_upd_recv(tmp_date,tmp_trace_no,sMdm_ac_rel.ac_no,tmp_amt);
		if( ret )
		{
			sprintf( acErrMsg, "�޸Ĵ�����ʵǼǲ�����[%d]!",ret );
			WRITEMSG
			goto ErrExit;
		}
	}

    if (flag[0] == '1')
    {
        /* �����ֽ��ӽ��� */
        goto OkExitCash;
    }
    else
    {
        /* ����ת���ӽ��� */
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

