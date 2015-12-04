/*************************************************
* �� �� ��:  spB603.c
* ���������� ծȯͶ��δ������Ϣ
*
* ��    ��:  
* ������ڣ� 2005��1��20��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"
#include "mdm_ac_rel_c.h"
#include "com_parm_c.h"
#include "mo_city_c.h"

int spB603()
{

    struct mo_invest_c sMo_invest;
    struct mdm_ac_rel_c sMdm_ac_rel;
	struct com_parm_c  sCom_parm;
	struct mo_city_c  mo_city_c;
    char zzlx[2];
    int ret;
	long	tmp_date=0,tmp_trace_no=0;
	double	tmp_amt=0.00;

    memset(&sMo_invest, 0x0, sizeof(struct mo_invest_c));
    memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0580",sMo_invest.pact_no); /* Ͷ��Э���� */
    get_zd_char("0690",g_pub_tx.ct_ind);      /* ��ת��־ */
    get_zd_double("0390",&g_pub_tx.tx_amt1);  /* ��Ϣ��� */
	get_zd_data("0890",g_pub_tx.note_type);
	get_zd_data("0590",g_pub_tx.beg_note_no);
	get_zd_data("0590",g_pub_tx.end_note_no);
    get_zd_data("0300",g_pub_tx.ac_no);      /* ת���˺� */
/*	get_zd_data("0700",zzlx); */


 vtcp_log("111111\n");
 
    ret = Mo_invest_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts='2'",sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for update error [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_invest.pact_no,ret);
        WRITEMSG
        goto ErrExit;
    }
     vtcp_log("222222222\n");
    ret = Mo_invest_Fet_Upd(&sMo_invest, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mo_invest error [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_invest.pact_no,ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        goto ErrExit;
    }
    vtcp_log("��ʼ������Ϣ��[%ld]\n",sMo_invest.lx_ac_id);
   
   	/*	����Ϣ�˻���id */
    ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id = '%ld' ",sMo_invest.lx_ac_id);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for Sel error [%s][%ld][%d]",g_pub_tx.tx_br_no,sMo_invest.lx_ac_id,ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mdm_ac_rel_Fet error [%s][%ld][%d]",g_pub_tx.tx_br_no,sMo_invest.lx_ac_id,ret);
        WRITEMSG
        Mdm_ac_rel_Clo_Sel();
        goto ErrExit;
    }
    set_zd_data("0330",sMdm_ac_rel.ac_no);
    Mdm_ac_rel_Clo_Sel();
    
    g_pub_tx.hst_ind[0] = '1';

    /* �޸�Ͷ�ʵǼǲ� */
    /* �޸ļ�����Ϣ */
    sMo_invest.pre_intst = sMo_invest.pre_intst - g_pub_tx.tx_amt1; 
    sMo_invest.trace_no = g_pub_tx.trace_no;
    strcpy(sMo_invest.tel,g_pub_tx.tel);
    strcpy(sMo_invest.chk,g_pub_tx.chk);
    strcpy(sMo_invest.auth,g_pub_tx.auth);
    sMo_invest.wrk_date = g_pub_tx.tx_date;
    strcpy(sMo_invest.br_no,g_pub_tx.tx_br_no);
    
    sprintf(acErrMsg,"AAAAAAAAAAAAAAAAAAA!!!");
    WRITEMSG
    ret = Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Upd mo_invest error !! [%d]",ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        goto ErrExit;
    }
    Mo_invest_Clo_Upd();
   
/***	if ( zzlx[0]=='1' ) 			 ͬ�� ***/
/*	{
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='TCKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�������������[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"2"); 	 ���� */
/*		strcpy(mo_city_c.cd_ind,"2");		 ����� */
/*		get_zd_double("0390",&mo_city_c.bill_amt);
		strcpy(mo_city_c.sts,"P");
		strcpy(mo_city_c.bill_type,"1");   Ʊ�����ࣺͬ�� */
/* 		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
		strcpy(mo_city_c.payee_ac_no,"14204");
		strcpy(mo_city_c.payee_name,"Ӧ��ծȯ��Ϣ");
		get_zd_data("0300",mo_city_c.payer_ac_no);
		get_zd_data("0270",mo_city_c.payer_name);
		ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"�Ǽ�ͬ�ǵǼǲ�����!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
	}
	else if( zzlx[0]=='2' )			 ��� ***/
/***	{
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='DEKM' and parm_seqn='2'" );
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�������������[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
		 �޸Ĵ��֧������״̬ **/
/**		get_zd_long("0440",&tmp_date);
		get_zd_long("0450",&tmp_trace_no);
		get_zd_double("0390",&tmp_amt);
		ret = pub_mbfe_upd_recv(tmp_date,tmp_trace_no,"14204",tmp_amt);
		if( ret )
		{
			sprintf( acErrMsg, "�޸Ĵ�����ʵǼǲ�����[%d]!",ret );
			WRITEMSG
			goto ErrExit;
		}
	} */

    if (g_pub_tx.ct_ind[0] == '1')
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
