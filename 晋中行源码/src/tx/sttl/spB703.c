/*************************************************
* �� �� ��:  spB703.c
* ���������� ֤ȯ�ع�����������
*
* ��    ��:  
* ������ڣ� 2003��1��29��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"
#include "com_parm_c.h"
#include "mo_city_c.h"

int spB703()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
    struct in_mst_c sIn_mst;
	struct com_parm_c sCom_parm;
	struct mo_city_c mo_city_c;
    char flag[2],zzlx[2];
	double lx=0.00;
	double tmpjs=0.00;
	long   tmp_date=0,tmp_trace_no=0;
	double tmp_amt=0.00;
    int ret;
  
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));
    memset(flag,0x0,sizeof(flag));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);
    get_zd_data("0690",flag);
    get_zd_data("0720",zzlx);
    get_zd_data("0210",g_pub_tx.cur_no);
    get_zd_data("0890",g_pub_tx.note_type);
    get_zd_data("0580",g_pub_tx.beg_note_no);
    get_zd_data("0580",g_pub_tx.end_note_no);

    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
      " pact_no='%s' and buy_type='1' and sts in ('2','3')",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ���֤ȯ�ع�����˻����ͽ��׽��Ĵ�С */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,g_pub_tx.tx_amt1) == -1)
    {
        sprintf(acErrMsg,"���׽�����֤ȯ�ع�����ʻ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.ct_ind,flag);

    g_pub_tx.add_ind[0] = '1'; /** ��(���Ǽ�) **/
    g_pub_tx.ac_id = 0;

    memset(g_pub_tx.ac_no,0x0,sizeof(g_pub_tx.ac_no));
    memset(g_pub_tx.name,0x0,sizeof(g_pub_tx.name));
    g_pub_tx.ac_id = sMo_stok_rebuy_prot.ac_id;
	ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id='%ld' ",g_pub_tx.ac_id);
	if (ret)
	{
        sprintf(acErrMsg,"ȡ�ڲ����ļ��������!!!");
        WRITEMSG
		strcpy(g_pub_tx.reply,"W023");
        goto ErrExit;
	}
    if (pub_base_CompDblVal(g_in_mst.bal,g_pub_tx.tx_amt1) == 0)
    {
		g_pub_tx.svc_ind=9002;
        strcpy(sMo_stok_rebuy_prot.sts,"*");
    }
	else
		g_pub_tx.svc_ind=9001;

vtcp_log("[%s][%d]---------g_pub_tx.ac_id=[%d],ac_seqn=[%d]",__FILE__,__LINE__,g_pub_tx.ac_id,g_pub_tx.ac_seqn);
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"��֤ȯ�ع�����ʻ��������������!!!");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"pub_acct_trance() pass!!!!! bal=[%lf]",g_in_mst.bal);
    WRITEMSG
    set_zd_double("0390",g_in_mst.bal);

    sMo_stok_rebuy_prot.bal -=g_pub_tx.tx_amt1;
    /* �޸�֤ȯ�ع�Э��Ǽǲ� */
    ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"br_no='%s' and pact_no='%s' and buy_type='1'",g_pub_tx.tx_br_no,sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"DECLARE for update error [%d] ",ret);
       	WRITEMSG
       	goto ErrExit;
    }   

    ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot1,g_pub_tx.reply);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"Fetch error [%s[[%s][%d] ",g_pub_tx.tx_br_no,sMo_stok_rebuy_prot.pact_no,ret);
       	WRITEMSG
       	Mo_stok_rebuy_prot_Clo_Upd();
       	goto ErrExit;
    }
	/* added by liuxuan 2006-12-1 */
	/*sMo_stok_rebuy_prot.sts[0]='*';*/
	/* added over */
    ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,g_pub_tx.reply);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"Update error [%d]",ret);
       	WRITEMSG
       	Mo_stok_rebuy_prot_Clo_Upd();
       	goto ErrExit;
    }
    Mo_stok_rebuy_prot_Clo_Upd();

	{
		/*** ������ϢӦ�ü��ٶ��ٻ��� ***/
		get_zd_double("0410",&lx);
    	if (pub_base_CompDblVal(lx,0.00) == 0)
			goto AAAAA;
		tmpjs=floor(lx*L360*L100/g_in_mst.rate);
		vtcp_log("intst_acm=[%lf],tmpjs=[%lf]",g_in_mst.intst_acm,tmpjs);
		g_in_mst.intst_acm=g_in_mst.intst_acm-tmpjs;
		vtcp_log("[%s][%d]�������tbl�еĺ�����in_mst�Ļ��� [%d]",__FILE__,__LINE__,g_in_mst.ac_id);
		vtcp_log("intst_acm=[%lf]",g_in_mst.intst_acm);
    	if (pub_base_CompDblVal(g_in_mst.intst_acm,0.00) <= 0)
			g_in_mst.intst_acm=0.00;
/*	���ܻ�����
		ret=sql_execute("update in_mst set intst_acm=%lf where ac_id=%d",g_in_mst.intst_acm,g_in_mst.ac_id);
		if( ret )
		{
        	sprintf(acErrMsg,"ȡ�ڲ����ļ��������!!!");
        	WRITEMSG
			strcpy(g_pub_tx.reply,"W023");
        	goto ErrExit;
		}
*/
	}
AAAAA:
vtcp_log("zzlx is:[%s]",zzlx);
	if ( zzlx[0]=='1' ) 			/*** �ϵĲ�����ͬ�� ***/
	{
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"2"); 	/* ���� */
		strcpy(mo_city_c.cd_ind,"2");		/* ����� */
		mo_city_c.bill_amt=g_pub_tx.tx_amt1+lx;
		strcpy(mo_city_c.sts,"P");
		strcpy(mo_city_c.bill_type,"1"); /* Ʊ�����ࣺͬ�� */
		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
		strcpy(mo_city_c.payee_ac_no,g_pub_tx.ac_no);
		strcpy(mo_city_c.payee_name,g_pub_tx.name);
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
/* 0420 �ű������Ϣ�ĺϼ� */
	set_zd_double("0420",g_pub_tx.tx_amt1+lx);
	double tmp_txamt=0;
	get_zd_double("1208",&tmp_txamt);
	vtcp_log("��0420��õ��Ľ����[%.2f]",tmp_txamt);

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
    /* �����ֽ��ӽ��� */
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
