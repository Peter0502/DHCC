/*************************************************
* �� �� ��: spL101.c
* ����������   
*              �����
*
* ��    ��:    lance
* ������ڣ�   2003��01��10��
* �޸ļ�¼��   
*     1. ��    ��:20110827
*        �� �� ��:gongliangliang
*        �޸�����:��ӶԷſ��˺źͻ����˺ŵ�У�飬ֻ���Ǳ��б��˵Ļ����˻�
*     2. 
*************************************************/
#define EXTERN
#include "stdio.h"
#include "string.h" 
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "public.h"
#include "ln_auth_c.h"
#include "draw_loan_limit_c.h"
#include "mo_po_co_c.h"
#include "ln_gage_reg_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"

static	double gage_amt_tmp;	

spL101()  
{ 	
	int ret=0 , flag=0;
	long min_day=0,max_day=0;
	double basic_rate=0.00;
	int	iUpd_flag=0;
	int	iIns_flag=0;
	
	struct ln_auth_c	ln_auth;
	struct draw_loan_limit_c sDraw_limit;
	struct mo_po_co_c	sMo_po_co;
	struct ln_gage_reg_c	sLn_gage_reg,sLnGageReg;
	struct dd_mst_c	g_dd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
			
	/** ��ʼ��ȫ�ֱ��� **/
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&sMdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&ln_auth, 0x00, sizeof(struct ln_auth_c));
	memset (&sMo_po_co, 0x00, sizeof(sMo_po_co));
	memset (&sLn_gage_reg, 0x00, sizeof(sLn_gage_reg));
	memset (&sLnGageReg, 0x00, sizeof(sLnGageReg));
			
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG

	/* ����Ƿ�����������Ǽǲ���������ͬ */
	ret = Ln_auth_Dec_Upd( g_pub_tx.reply , "pact_no='%s'" , g_ln_mst.pact_no );
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Ln_auth_Dec_Upd �α����[%d]",ret);
		WRITEMSG
		ERR_DEAL;
	}
	ret = Ln_auth_Fet_Upd( &ln_auth , g_pub_tx.reply );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�޴�������¼![%d]",ret);
		WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L175" );
		ERR_DEAL ;
	}else if( ret ){
	 	sprintf( acErrMsg,"fetch Ln_auth_Fet_Upd error code = [%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
	 	ERR_DEAL ;
	}
	if( ln_auth.sts[0]!='X' )/*20110728 �������������Ŵ�ϵͳ�����״̬���Ŵ��ṩ����ΪX */
	{
	 	sprintf( acErrMsg,"�˽���Ѵ���,����!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L020" );
	 	ERR_DEAL ;		
	}
	/* ������Ϣ��������Ϣ�Ƚ� */
	/* ����� */
	pub_base_strpack(ln_auth.name);
	
	/*if( strcmp(g_pub_tx.name , ln_auth.name) )	
	{
	 	TRACE
	 	TRACE
	 	sprintf( acErrMsg,"�������������Ϣ����![%s][%s]",g_pub_tx.name , ln_auth.name);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L166" );
	 	ERR_DEAL;
	}*/
	
	/******* edit by gong 20110827 �����Ŵ��������Ĵ�����˺��ˣ�����
	����˻�ID
	if( g_ln_mst.save_ac_id != ln_auth.save_ac_id )
	{
	 	sprintf( acErrMsg,"�ſ��˺���������Ϣ����!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L167" );
	 	ERR_DEAL;
	}	
	 *********end by gong 20110827************************/
	
	/**add by gong 20110827 �������л����˺�/����˺�У��
	
	char br_no_tmp[6];
	memset(br_no_tmp,0x00,sizeof(br_no_tmp));
	memcpy(br_no_tmp,ln_auth.pact_no+1,5);
	
	ret = Dd_mst_Sel(g_pub_tx.reply ,&g_dd_mst,"ac_id in (select ac_id from mdm_ac_rel where ac_no ='%s')",g_pub_tx.ac_no1);
	if(ret)
	{
		sprintf(acErrMsg, "  �����˺�/�����˺ŷǻ����˺�![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B192");
		goto ErrExit;
	}

	if(strcmp(g_dd_mst.opn_br_no,br_no_tmp)!=0)
	{
		vtcp_log("[%s][%d]���ڿ���������[%s],�Ŵ�����������[%s]",__FILE__,__LINE__,g_dd_mst.opn_br_no,br_no_tmp);
		sprintf( acErrMsg,"  �����˺�/����˺ŷǱ�����������![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"N098" );
	 	ERR_DEAL;
	}

	if(ln_auth.cif_no!=g_dd_mst.cif_no)
	{
		vtcp_log("[%s][%d]�Ŵ������Ŀͻ���[%ld],�����˻��ͻ���[%ld]",__FILE__,__LINE__,ln_auth.cif_no,g_dd_mst.cif_no);
		sprintf( acErrMsg,"  �����˺�/����˺ŷǱ����˺�![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"N099" );
		ERR_DEAL ;
	}

   end by gong 20110827 �������л����˺�/����˺�У��**/
	
	/* ������ */
	if( pub_base_CompDblVal( ln_auth.amt , g_ln_mst.amt_lmt ) )
	{
	 	sprintf( acErrMsg,"�ƻ��ſ�����������Ϣ����!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L168" );
	 	ERR_DEAL;
	}
	/* ��ͬ���� */		
	if( pub_base_CompDblVal( ln_auth.rate , g_ln_mst.rate ) )
	{
	 	sprintf( acErrMsg,"��ͬ������������Ϣ����!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L169" );
	 	ERR_DEAL;
	}	
	/****** edit by gong 20110827 �����Ŵ��������Ĵ�����˺��ˣ�����
	�����˺�ID 
	if( g_ln_mst.repay_ac_id!=ln_auth.pay_ac_id )
	{
	 	sprintf( acErrMsg,"�����˺���������Ϣ����!");
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L170" );
	 	ERR_DEAL;
	}
	****************end by gong 20110827****************/
	
	/* �������� */	
	pub_base_deadlineM( g_ln_mst.opn_date,ln_auth.time_lmt-3,&min_day);/**  martin  ԭΪ1���¡�����3��**/
	pub_base_deadlineM( g_ln_mst.opn_date,ln_auth.time_lmt+1,&max_day);
	/** 
	if( g_ln_mst.mtr_date < min_day )
	{
	 	sprintf( acErrMsg,"�������ڹ�С,��������Ϣ����![%ld][%ld][%ld]",min_day,max_day,g_ln_mst.mtr_date);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L171" );
	 	ERR_DEAL;
	}
	
	if( g_ln_mst.mtr_date > max_day )
	{
	 	sprintf( acErrMsg,"�������ڹ���,��������Ϣ����![%ld][%ld][%ld]",min_day,max_day,g_ln_mst.mtr_date);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"L172" );
	 	ERR_DEAL;
	}
	**/
	
	/* ȡ�������� */
	ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , "prdt_no='%s' " , g_pub_tx.prdt_code );
	if(ret)	
	{
		sprintf(acErrMsg,"ȡ���������Ϣʧ��!");
		WRITEMSG
		ERR_DEAL;	
	}
	ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &basic_rate );
	if( ret )
	{
		sprintf(acErrMsg,"���ø������ʱ��ȡ�����ʺ�������!");
		WRITEMSG
		return -1;	
	}
	basic_rate = basic_rate/1.2;/* ת���������� */	
	/* ���ʸ����������� */		
	
	g_ln_mst.rate_flt = pub_base_PubDround_1( g_ln_mst.rate*100/basic_rate );		
	g_ln_mst.over_rate_flt = pub_base_PubDround_1( g_ln_mst.over_rate*100/basic_rate);
	g_ln_mst.fine_rate_flt = pub_base_PubDround_1( g_ln_mst.fine_rate*100/basic_rate);
			
			
	/* �޸������Ǽǲ���״̬ */
	strcpy( ln_auth.sts , "1" );	
	ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_auth_Upd_Upd ����[%d]",ret);
		WRITEMSG
		return 1;
	}
	
	/*add by gong 20110829�޸�mdm_code*/	
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply ,&sMdm_ac_rel,"ac_id ='%ld'",g_ln_mst.save_ac_id);
	if(ret)
	{
		sprintf(acErrMsg, "  ���ݿ��ѯʧ��![%d]",ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B192");
		goto ErrExit;
	}
	strcpy( ln_auth.save_mdm_code , sMdm_ac_rel.mdm_code );	
	ret = Ln_auth_Upd_Upd( ln_auth, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_auth_Upd_Upd ����[%d]",ret);
		WRITEMSG
		return 1;
	}
	/*end by gong 20110829*/
	Ln_auth_Clo_Upd( );
	sprintf(acErrMsg,"��������ϢУ�� PASS!");
	WRITEMSG		
	
	/* ����������Ŀ�/�˺��Ͽ�������˻� */
	if ( strlen( g_pub_tx.crd_no )!= 0 )
	{
	/* ȡ�ÿ�/�˺Ŷ�Ӧ�Ľ��ʴ��� *
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",
					g_pub_tx.crd_no );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�������˻���ϵ��Ϣ���޴˿��Ż��˺ţ�����![%s]", 
					g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L014");
			goto ErrExit;
		}else if( ret )
		  {
			sprintf(acErrMsg,"��ȡ�������˻���ϵ��Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L015");
			goto ErrExit;
		  }
		sprintf(acErrMsg,"ȡ���Ľ��ʴ���[%s]", g_mdm_ac_rel.mdm_code);
		WRITEMSG		  
		strcpy( g_pub_tx.mdm_code , g_mdm_ac_rel.mdm_code ); 
		g_pub_tx.ac_seqn = ALL_SEQN ;
		sprintf(acErrMsg,"ȡ�ÿ�/�˺Ŷ�Ӧ�Ľ��ʴ��� PASS!");
		WRITEMSG
		****/
	strcpy ( g_pub_tx.ac_no , g_pub_tx.crd_no );  
	}
	else
	{
		strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.add_ind , "1" );		/* ����1�� */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/* �Ƿ��ӡ����(�������۱�־)0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
		strcpy( g_pub_tx.brf,"�����������" );		
		
		TRACE
		/***�����˺�***/
		if( pub_acct_opn_cif_mdm() )
		{
			sprintf(acErrMsg,"�����˺Ŵ���!");
			WRITEMSG
			ERR_DEAL
		}
	}

	TRACE
	/* ȷ������ͻ����� */
	strcpy( g_ln_mst.cmpd_intst_ind , g_ln_parm.cmpd_intst_ind );	/*�Ƿ�Ǹ�����־*/
	
	/* Pub_tx�ṹ��ֵ */
	strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����1�� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/* �Ƿ��ӡ����(�������۱�־)0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	g_pub_tx.svc_ind = 3001;
	strcpy( g_pub_tx.brf,"�����" );
	TRACE
	/***�����˻�ID***/
	if( pub_acct_opn_ac() )
	{
		sprintf(acErrMsg,"�����˺�ID����!");
		WRITEMSG
		ERR_DEAL
	}

	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
	
	/*  �Ǽ���ˮ*/
	if( pub_ins_ln_trace_log(ln_auth.amt))
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
    WRITEMSG
	/*
	g_pub_tx.tx_amt1 = ln_auth.amt;
	g_pub_tx.svc_ind = 3001;
	strcpy( g_pub_tx.brf,"�����" );
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		goto ErrExit;
	}	
	********/	
	
	set_zd_data("0310",g_pub_tx.ac_no); 		/* �����˺� */
	set_zd_long("0350",g_ln_mst.ac_seqn);		/* ������� */
	set_zd_data("0720",g_ln_parm.pay_pln_type);	/* �ſ�ƻ����� */
	set_zd_data("0660",g_ln_parm.pay_pln_crt); 	/* ����ƻ��������� */
	set_zd_data("0210",g_pub_tx.cur_no);		/* ���� */	
	/** begin add by chenchao 20110801 �޸�֧�����Ʊ� **/
	/****�޸Ĵ�������ί��֧���޶��*****/
	if( (g_ln_parm.ln_type[0]!='A') )
	{
		memset(&sDraw_limit,0x00,sizeof(sDraw_limit));
		g_reply_int=Draw_loan_limit_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",g_ln_mst.pact_no);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"Draw_loan_limit�����α����");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D101" );
			goto ErrExit;
		}
		g_reply_int=Draw_loan_limit_Fet_Upd(&sDraw_limit,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"Draw_loan_limitȡ�α����");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"B384" );
			goto ErrExit;
		}
		sDraw_limit.sts[0]='1';
		sDraw_limit.ac_id=g_ln_mst.save_ac_id;
		g_reply_int=Draw_loan_limit_Upd_Upd(sDraw_limit,g_pub_tx.reply);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"Draw_loan_limit���¼�¼����");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D384" );
			goto ErrExit;
		}
		Draw_loan_limit_Clo_Upd();
	}
	vtcp_log("[%s][%d]pact_no=[%s]",__FILE__,__LINE__,g_ln_mst.pact_no);
	/** ���Ӵ������Ѻ��ĵǼ� ***/
	ret = Mo_po_co_Sel(g_pub_tx.reply,&sMo_po_co,"pact_no='%s'",g_ln_mst.pact_no);
	if(ret)
	{
		sprintf(acErrMsg,"��ѯ��ͬ��ݹ�ϵ�����!!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D107" );
		goto ErrExit;
	}
	ret = Ln_gage_reg_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",sMo_po_co.contract_no);
	if(ret)
	{
		sprintf(acErrMsg,"�����α����");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D101" );
		goto ErrExit;
	}
	while(1)
	{
		memset(&sLn_gage_reg,0x00,sizeof(sLn_gage_reg));
		vtcp_log("[%s][%d]pact_no=[%s]",__FILE__,__LINE__,g_ln_mst.pact_no);
		ret = Ln_gage_reg_Fet_Upd(&sLn_gage_reg,g_pub_tx.reply);
		if(ret && ret != 100)
		{
			sprintf(acErrMsg,"���Ҵ������Ѻ�ǼǱ�����!!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D107" );
			goto ErrExit;
		}else if(ret == 100){
			if(!iIns_flag && strlen(sLn_gage_reg.pact_no)>0)
			{
				sLnGageReg.ac_id = g_ln_mst.ac_id;
				sLnGageReg.ac_seqn = g_ln_mst.ac_seqn;
				memset(sLnGageReg.name,0x00,sizeof(sLnGageReg.name));
				memcpy(sLnGageReg.name,g_ln_mst.name,sizeof(sLnGageReg.name)-1);
				ret = Ln_gage_reg_Ins(sLnGageReg,g_pub_tx.reply);
				if(ret)
				{
					Ln_gage_reg_Clo_Upd();
					sprintf(acErrMsg,"����������Ѻ�ǼǱ�����!!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"D107" );
					goto ErrExit;
				}
				vtcp_log("[%s][%d]����Ϊ�������",__FILE__,__LINE__);
			}
			break;
		}
		if(sLn_gage_reg.ac_id == 0)
		{
			sLn_gage_reg.ac_id = g_ln_mst.ac_id;
			sLn_gage_reg.ac_seqn = g_ln_mst.ac_seqn;
			memset(sLn_gage_reg.name,0x00,sizeof(sLn_gage_reg.name));
			memcpy(sLn_gage_reg.name,g_ln_mst.name,sizeof(sLn_gage_reg.name)-1);
			ret = Ln_gage_reg_Upd_Upd(sLn_gage_reg,g_pub_tx.reply);
			if(ret)
			{
				Ln_gage_reg_Clo_Upd();
				sprintf(acErrMsg,"���´������Ѻ�ǼǱ�����!!");
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D107" );
				goto ErrExit;
			}
			vtcp_log("[%s][%d]����Ϊ���²���",__FILE__,__LINE__);
			iIns_flag ++;
		}
		memcpy(&sLnGageReg,&sLn_gage_reg,sizeof(sLnGageReg));
	}
	Ln_gage_reg_Clo_Upd();
	/** END add by chenchao 20110801 �޸�֧�����Ʊ� **/
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

int data_set_struct()
{    
	char save_ac_no[20];
	long cif_no_tmp;
	int  ac_seqn_tmp;
	int  ret=0;
	
	g_ln_mst.opn_date = g_pub_tx.tx_date;		/* �������� */
	get_zd_data("0370",g_pub_tx.crd_no);		/* ��/�ʺ� */
	get_zd_data("0670",g_pub_tx.id_type);		/* ֤������ */
	get_zd_data("0620",g_pub_tx.id_no);		/* ֤������ */			
	get_zd_data("0630",g_ln_mst.pact_no);		/* ��ͬ�� */	
	get_zd_double("0430",&g_ln_mst.amt_lmt);	/* �ƻ����Ŷ�����޶� */
	g_pub_tx.tx_amt1=0.00;				/* ����ͬʱ�ķ��Ŷ� */	
	get_zd_double("0840",&g_ln_mst.rate);		/* �������� */	
	get_zd_double("0850",&g_ln_mst.over_rate);	/* �������� */
	get_zd_double("0940",&g_ln_mst.fine_rate);	/* ��Ϣ���� */		
	get_zd_data("0230",g_pub_tx.prdt_code);		/* ��Ʒ���� */
	get_zd_data("0210",g_pub_tx.cur_no);		/* ���� */
	get_zd_data("0690",g_ln_mst.repay_type);	/* ����֧����ʽ */
	get_zd_data("0610",g_ln_mst.trust_no);		/* ί��Э���� */		
	get_zd_long("0450",&g_ln_mst.mtr_date);		/* �������� */
	get_zd_data("0710",g_pub_tx.intst_type);	/* ��Ϣ���� */	
	get_zd_data("0250",g_pub_tx.name);		/* ����� */		
	get_zd_double("0420",&gage_amt_tmp);/* ��ѺƷ��� */
	get_zd_double("0410",&g_ln_mst.advis_amt_pln);/* ������ʷ� */
	get_zd_int("0340",&ac_seqn_tmp );		/* �˺���� */
	get_zd_data("0380",g_pub_tx.ac_no1);		/* �����˺� */
	pub_base_old_acno(g_pub_tx.ac_no1);
	
	sprintf(acErrMsg,"�����ʺ�!!!![%s]",g_pub_tx.ac_no1);
	WRITEMSG
	get_zd_data("0320",save_ac_no);	/* ����˺� */
	pub_base_old_acno(save_ac_no);
	sprintf(acErrMsg,"����˺�!!!![%s]",save_ac_no);
	WRITEMSG   
	/* ȡ����˺�ID */	
	pub_base_strpack(save_ac_no);
	if( strlen(save_ac_no) )
	{
		ret = pub_ln_AcCheck( save_ac_no,ac_seqn_tmp,
			&cif_no_tmp,&g_ln_mst.save_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			return 1;
		}	
	}  		
   	/* ȡ�����˺�ID */	
 	pub_base_strpack(g_pub_tx.ac_no1);
	if( strlen(g_pub_tx.ac_no1) )
	{
		ret = pub_ln_AcCheck( g_pub_tx.ac_no1,ac_seqn_tmp,
			&cif_no_tmp,&g_ln_mst.repay_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"���ú���pub_ln_AcCheck����!");
			WRITEMSG
			return 1;
		}	
	}  		
	return 0;
}

