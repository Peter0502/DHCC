/*************************************************
* �� �� ��: spL205.c
* ����������   
*              ��������
*
* ��    ��:    lance
* ������ڣ�   2003��01��21��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "ln_gage_reg_c.h"
#include "ln_pay_pln_c.h"
#include "mo_po_co_c.h"
static struct ln_mst_c    ln_mst_upd ;
static struct ln_pay_pln_c   ln_pay_pln_upd ;
#include "mob_acct_type_c.h" /*add by zgf 2013-07-25*/
spL205()  
{
	int ret=0;
	double intst=0.00;
	struct ln_gage_reg_c ln_gage_reg;
	struct mo_po_co_c  sMo_po_co;
	struct mob_acct_type_c st_mob_acct_type;/*add by zgf 2013-07-25*/

	memset (&ln_gage_reg,0x00,sizeof(struct ln_gage_reg_c));
	memset (&sMo_po_co,0x00,sizeof(sMo_po_co));
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG

	/** ȡֵ����ֵ **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	vtcp_log("AAAAAAAAAAAAAAAAA\n");

	/** �������� **/
	/* ��ѯ�������ļ� ����� */
	if (g_ln_mst.bal)
	{
		sprintf( acErrMsg, "�����δ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L049");
		ERR_DEAL
	}	
	
	 /*���ӿ��ƶ��Ŵ���ǩԼ��Ϣ���ܽ����˻�����,�����Ƚ��н�Լ----add by zgf 2013-07-25*/
	  memset(&st_mob_acct_type,0x00,sizeof(st_mob_acct_type));
	  ret=Mob_acct_type_Sel(g_pub_tx.reply,&st_mob_acct_type,"ac_no='%s' and mob_sts='1' ",g_ln_mst.pact_no);
	  if(ret==100){
	          vtcp_log("[%s][%d]�˺�[%s]δ��ѯ������ǩԼ��¼,��������!!!",__FILE__,__LINE__,g_ln_mst.pact_no);
	  }else if(ret==0){
	          vtcp_log("[%s][%d]�˺�[%s]���ڶ���ǩԼ��¼!!!",__FILE__,__LINE__,g_ln_mst.pact_no);
	          sprintf(acErrMsg, "���˺Ŵ��ڶ�������ǩԼ��δ��Լ!!");
	          strcpy(g_pub_tx.reply,"F037");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	
	  }else{
	          vtcp_log("[%s][%d]��ѯ��������ǩԼ�����!!!",__FILE__,__LINE__,g_ln_mst.pact_no);
	          sprintf(acErrMsg, "��ѯ��������ǩԼ�����!!");
	          strcpy(g_pub_tx.reply,"XXXX");
	          set_zd_data(DC_GET_MSG,acErrMsg);
	          goto ErrExit;
	  }
	  /*end add by zgf*/
	
	if(memcmp(g_ln_mst.prdt_no,"3Y1",3))
	{		
	/* ��ѯ�������ļ� ǷϢ */
      vtcp_log("[%lf]%%%%%%%%%%%%wzzwzz",g_ln_mst.cmpd_lo_intst);
       vtcp_log("[%lf]%%%%%%%%%%%%wzzwzz",g_ln_mst.in_lo_intst); 
      vtcp_log("[%lf]%%%%%%%%%%%%wzzwzz",g_ln_mst.out_lo_intst);
	double tmp_amt=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
	vtcp_log("1111111tmp_amt[%.2f]",tmp_amt);
	if (pub_base_CompDblVal(tmp_amt , 0.00) )
	/*if (pub_base_CompDblVal(g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst , 0.00) )*/
	{
		sprintf( acErrMsg, "����ǷϢδ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L050");
		ERR_DEAL
	}				
	/* ��ѯ�������ļ� ���� */
	if (pub_base_CompDblVal(g_ln_mst.intst_acm+g_ln_mst.in_lo_acm+g_ln_mst.out_lo_acm+g_ln_mst.cmpd_lo_acm , 0.00) && g_ln_mst.intst_type[0]!='0')
	{
		sprintf( acErrMsg, "�����л���δ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L051");
		ERR_DEAL
	}	

	/* �ֶλ�����Ϣ */
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "1" , &intst , 1 );
	if( ret ) goto ErrExit;
	if ( pub_base_CompDblVal(intst , 0.00) )
	{
		sprintf( acErrMsg, "������Ϣδ����,��������[%lf]",intst);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
		vtcp_log("AAAAAAAAAAAAAAAAAzzzzz\n");
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "2" , &intst , 1 );	
	if( ret ) goto ErrExit;
	if (pub_base_CompDblVal(intst , 0.00) )
	{
		sprintf( acErrMsg, "������Ϣδ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
	
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "3" , &intst , 1 );
	if( ret ) goto ErrExit;
	if (pub_base_CompDblVal(intst , 0.00))
	{
		sprintf( acErrMsg, "������Ϣδ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
	
	ret = pub_base_Get_SectIntst( g_ln_mst.ac_id , g_ln_mst.ac_seqn , "4" , &intst , 1 );
	if( ret ) goto ErrExit;
	if (pub_base_CompDblVal(intst , 0.00))
	{
		sprintf( acErrMsg, "������Ϣδ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L052");
		ERR_DEAL
	}
}
	/* ��Ϊ���Ҵ��� */
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"���Ҵ���");
		WRITEMSG
	
		/* ��ѯ����Ƿ��� */
		/*ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_pub_tx.ac_id,g_pub_tx.ac_seqn ); mod time 20120103*/
		ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if (ret)
		{
			sprintf( acErrMsg, "�ð��Ҵ������Ƿ��,��������[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L053");
			ERR_DEAL
		}		
	}
	/** add by chenchao ���ӿ���,������֮ǰ���������⽻��   2011/9/16 16:14:00 **/
	ret = Mo_po_co_Sel(g_pub_tx.reply,&sMo_po_co,"pact_no='%s'",g_ln_mst.pact_no);
	if(ret && ret!=100 )
	{
		sprintf( acErrMsg, "��ѯ��ͬ��ݹ�ϵ�����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L053");
		ERR_DEAL
	}else if(ret == 0)
	{
		ret = sql_count("ln_mst","ac_id in (select ac_id from ln_gage_reg where pact_no='%s' and sts='0') and ac_sts<>'*'",sMo_po_co.contract_no);
		if(ret<0)
		{
			sprintf( acErrMsg, "���ݵ���Ѻ���ѯ����ʧ��[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L053");
			ERR_DEAL
		}else if(ret==1)
		{
			sprintf( acErrMsg, "���е���Ѻ��δ����,���������⽻��pact_no=[%s]",g_ln_mst.pact_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"T056");
			ERR_DEAL
		}
	}
	/**** add by chenchao end 2011/9/16 16:29:18 ***/
	
	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "2" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.brf,"��������" );
	g_pub_tx.svc_ind = 2;				/* ���� */

	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG

	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG

	/* ���´������ļ� */	
/****	
g_reply_int=sql_execute("update ln_mst set ac_sts='*' \
			where ac_id=%ld and ac_seqn=%d ",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
*****/
	
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	ret = Ln_mst_Fet_Upd( &ln_mst_upd , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}else if (ret == 100){
		goto ErrExit;
	}
	strcpy( ln_mst_upd.ac_sts,"*" );
	ln_mst_upd.lst_date=g_pub_tx.tx_date;
	ret = Ln_mst_Upd_Upd( ln_mst_upd , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"���´������ļ�����!!");
               WRITEMSG
               goto ErrExit;
        }  				
	Ln_mst_Clo_Upd();
	/***���ڰ��Ҵ��������뻹��ƻ�����޸ģ���pay_ind��Ϊ'1'***/
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"���Ҵ���");
		WRITEMSG
		
			ret = Ln_pay_pln_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Ln_pay_pln_Fet_Upd( &ln_pay_pln_upd , g_pub_tx.reply );
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if (ret == 100){
			goto ErrExit;
		}
		strcpy( ln_pay_pln_upd.pay_ind , "1" );
		ret = Ln_pay_pln_Upd_Upd( ln_pay_pln_upd , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"���´������ļ�����!!");
               WRITEMSG
               goto ErrExit;
        }  				
				Ln_pay_pln_Clo_Upd();
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��������ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
	if ((strcmp(g_pub_tx.reply,"0000")==0) || (strcmp(g_pub_tx.reply,"")==0) || \
		 g_pub_tx.reply[0] == '\0')
	{
		strcpy(g_pub_tx.reply,"D114");
	}
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&ln_mst_upd, 0x00, sizeof(struct ln_mst_c));
	memset (&ln_pay_pln_upd, 0x00, sizeof(struct ln_pay_pln_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_ln_parm, 0x00, sizeof(struct ln_parm_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG

	get_zd_data("0370",g_pub_tx.ac_no);		/* �ʺ� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* �˻���� */
	pub_base_old_acno(g_pub_tx.ac_no);
	/*��ȡ�����Ϣ*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}
/*
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret ) {return ret;}
	ret = Ln_mst_Sel( g_pub_tx.reply ,&g_ln_mst, " ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	if( ret ) {return ret;}
	double tmp_amt=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
	if (pub_base_CompDblVal(tmp_amt , 0.00) )
	{
		sprintf( acErrMsg, "����ǷϢδ����,��������[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L050");
		return(-1);
	}
*/
	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */


	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) {return ret;}
					

	return 0;
}
