/*************************************************
* �� �� ��:  spB705.c
* ���������� ֤ȯ�ع����뻹�����
*
* ��    ��:  
* ������ڣ� 2003��1��29��
*
* �޸ļ�¼�� 
*   ��   ��:20031026
*   �� �� ��:jane
*   �޸�����:����¼�뽻��
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"
#include "mo_stok_rebuy_inpb_c.h"
#include "com_parm_c.h"
#include "mbfe_pub.h"
#include "mo_city_c.h"

int spB705()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
	struct mo_stok_rebuy_inpb_c sMoStokRebuyInpb;
    struct in_mst_c sIn_mst;
	struct com_parm_c	sCom_parm;
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mo_city_c	mo_city_c;
    char flag[2],zzlx[2];
    int ret;
	double lx=0.00;
	double tmpjs=0.00;
	double amt_sum=0.00;
  
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMoStokRebuyInpb,0x0,sizeof(struct mo_stok_rebuy_inpb_c));
    memset(flag,0x0,sizeof(flag));
   	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
  	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
  	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
    get_zd_double("0400",&g_pub_tx.tx_amt1);	/* ������ */
    get_zd_data("0690",flag);				/* 1�ֽ�  2ת�� */
    get_zd_data("0720",zzlx);
    get_zd_data("0210",g_pub_tx.cur_no);
    get_zd_data("0890",g_pub_tx.note_type);
    get_zd_data("0580",g_pub_tx.beg_note_no);
    get_zd_data("0580",g_pub_tx.end_note_no);
  	get_zd_double("0410",&lx);					/* ��Ϣ */
vtcp_log("[%s][%d]-------lx=[%f]",__FILE__,__LINE__,lx);
  	amt_sum=g_pub_tx.tx_amt1+lx;
	  set_zd_double("0420",amt_sum);	/* ����+��Ϣ */

	/*����Ƿ��иñ�¼����Ϣ*/
/*	������� û��
	g_reply_int = sql_count( "mo_stok_rebuy_inpb", "pact_no='%s' and amt=%lf and intst=%lf", 
		sMo_stok_rebuy_prot.pact_no,g_pub_tx.tx_amt1,lx);
 	  vtcp_log("%d, %s,%d �õ��ĸ�����hahaha",g_reply_int,__FILE__,__LINE__); 
	if( g_reply_int < 0 )
	{
        sprintf(acErrMsg,"��ѯmo_stok_rebuy_prot!! [%lf][%s][%lf][%d][%d]",g_pub_tx.tx_amt1,sMo_stok_rebuy_prot.pact_no,lx,g_reply_int,g_reply_int);
        WRITEMSG
        goto ErrExit;
	}
	else if( g_reply_int == 0 )
	{
		strcpy( g_pub_tx.reply, "D104" );
        sprintf(acErrMsg,"��δ¼����Ϣ!! [%lf][%s][%lf][%d]", g_pub_tx.tx_amt1,sMo_stok_rebuy_prot.pact_no,lx,g_reply_int);
        WRITEMSG
		goto ErrExit;
	}
	if( g_reply_int>0 )
	{
		g_reply_int=0;
		g_reply_int = Mo_stok_rebuy_inpb_Dec_Upd( g_pub_tx.reply,"pact_no='%s' and amt='%lf' and intst='%lf'", sMo_stok_rebuy_prot.pact_no,g_pub_tx.tx_amt1,lx);
		if( g_reply_int )
		{
        	sprintf(acErrMsg,"�����α��mo_stok_rebuy_prot!! [%lf][%s][%lf][%d]",
				  g_pub_tx.tx_amt1,sMo_stok_rebuy_prot.pact_no,lx,g_reply_int);
        	WRITEMSG
        	goto ErrExit;
		}
		g_reply_int = Mo_stok_rebuy_inpb_Fet_Upd( &sMoStokRebuyInpb, g_pub_tx.reply );
		if( g_reply_int )
		{
        	sprintf(acErrMsg,"ȡ�α��mo_stok_rebuy_prot!! [%lf][%s][%lf][%d]",
				  g_pub_tx.tx_amt1,sMo_stok_rebuy_prot.pact_no,lx,g_reply_int);
        	WRITEMSG
        	goto ErrExit;
		}
		vtcp_log("[%s][%d]----------",__FILE__,__LINE__);
		g_reply_int = Mo_stok_rebuy_inpb_Del_Upd( sMoStokRebuyInpb,g_pub_tx.reply );
		if( g_reply_int )
		{
        	sprintf(acErrMsg,"ִ��Del�α��mo_stok_rebuy_prot!! [%lf][%s][%lf][%d]",
				  g_pub_tx.tx_amt1,sMo_stok_rebuy_prot.pact_no,lx,g_reply_int);
        	WRITEMSG
        	goto ErrExit;
		}
		vtcp_log("[%s][%d]----------",__FILE__,__LINE__);
		Mo_stok_rebuy_inpb_Clo_Upd();
	}
*/

    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot,
     " pact_no='%s' and buy_type='2' and sts='2'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����!! [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_stok_rebuy_prot.pact_no,ret);
        WRITEMSG
        goto ErrExit;
    }

    pub_base_strpack(sMo_stok_rebuy_prot.opt_acno);
    /* ���֤ȯ�ع�����˻����ͽ��׽��Ĵ�С */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,g_pub_tx.tx_amt1)<0 )
    {
        sprintf(acErrMsg,"���׽�����֤ȯ�ع�����ʻ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.ct_ind,flag);

    g_pub_tx.add_ind[0] = '0';
    g_pub_tx.ac_id = 0;	/** �� **/

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
    {
		    g_pub_tx.svc_ind=9001;
        strcpy(sMo_stok_rebuy_prot.sts,"2");
    }

	sprintf(acErrMsg,"g_pub_tx.ac_id=[%ld]",g_pub_tx.ac_id);
    WRITEMSG
    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"��֤ȯ�ع�����ʻ��������������!!!");
        WRITEMSG
        goto ErrExit;
    }
    set_zd_double("0390",g_in_mst.bal);

    sMo_stok_rebuy_prot.bal -=g_pub_tx.tx_amt1;
    /* �޸�֤ȯ�ع�Э��Ǽǲ� */
    ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"pact_no='%s' and \
		buy_type='2'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
       	sprintf(acErrMsg,"DECLARE for update error [%d] ",ret);
       	WRITEMSG
       	goto ErrExit;
    }   

   	ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot1,g_pub_tx.reply);
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"Fetch error [%d] ",ret);
       	WRITEMSG
       	Mo_stok_rebuy_prot_Clo_Upd();
       	goto ErrExit;
   	}

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
		vtcp_log("intst_acm=[%lf]",g_in_mst.intst_acm);
    	if (pub_base_CompDblVal(g_in_mst.intst_acm,0.00) <= 0)
			g_in_mst.intst_acm=0.00;

		ret=sql_execute("update in_mst set intst_acm=%lf where ac_id=%d",g_in_mst.intst_acm,g_in_mst.ac_id);
		if( ret )
		{
        	sprintf(acErrMsg,"ȡ�ڲ����ļ��������!!!");
        	WRITEMSG
			strcpy(g_pub_tx.reply,"W023");
        	goto ErrExit;
		}
	}

AAAAA:
	if ( zzlx[0]=='1' ) 			/*** ͬ�� ***/
	{
		strcpy(mo_city_c.payer_ac_no,g_pub_tx.ac_no);
		strcpy(mo_city_c.payer_name,g_pub_tx.name);
		get_zd_data("0300",mo_city_c.payee_ac_no);
		get_zd_data("0270",mo_city_c.payee_name);
		mo_city_c.tx_date = g_pub_tx.tx_date;
		mo_city_c.tx_no = g_pub_tx.trace_no;
		strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
		strcpy(mo_city_c.tel,g_pub_tx.tel);
		strcpy(mo_city_c.trans_mode,"1");
		strcpy(mo_city_c.in_out_ind,"1"); 	/* ��� */
		strcpy(mo_city_c.cd_ind,"2");		/* ����� */
		mo_city_c.bill_amt=g_pub_tx.tx_amt1+lx;
		strcpy(mo_city_c.sts,"3");
		strcpy(mo_city_c.bill_type,"1"); /* Ʊ�����ࣺͬ�� */
		mo_city_c.bat_no = 0;
		mo_city_c.no = 0;
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
		strcpy(mbfeghvsendjrn_v.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(mbfeghvsendjrn_v.tx_code,g_pub_tx.tx_code);
		mbfeghvsendjrn_v.consigndate=g_pub_tx.tx_date;
		mbfeghvsendjrn_v.trace_no=g_pub_tx.trace_no;
		strcpy(mbfeghvsendjrn_v.procstate,"0");
		strcpy(mbfeghvsendjrn_v.payeracc,g_pub_tx.ac_no);
		get_zd_double("0420",&mbfeghvsendjrn_v.amount);
		strcpy(mbfeghvsendjrn_v.payername,g_pub_tx.name);

vtcp_log("tx_br_no=[%s],tx_code=[%s],signdate=%ld,trace_no=%ld,sts=[%s],payeracc=[%s],amount=[%lf],name=[%s]",mbfeghvsendjrn_v.tx_br_no,g_pub_tx.tx_code,mbfeghvsendjrn_v.consigndate,mbfeghvsendjrn_v.trace_no,mbfeghvsendjrn_v.procstate,mbfeghvsendjrn_v.payeracc,mbfeghvsendjrn_v.amount,mbfeghvsendjrn_v.payername);
		ret = Mbfeghvsendjrn_Ins(mbfeghvsendjrn_v, g_pub_tx.reply);
		if( ret )
    	{
        	sprintf(acErrMsg,"�ǼǴ���ҵǼǲ�����![%d]",g_reply_int);
        	WRITEMSG
        	goto ErrExit;
    	}
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='DEKM' and parm_seqn='1'" );
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�������������[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		set_zd_data("0300",sCom_parm.val);
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
