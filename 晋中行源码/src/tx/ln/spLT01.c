/*************************************************
* �� �� ��: spLT01.c
* ����������   
*              ����ſί��Э������
*
* ��    ��:    lance
* ������ڣ�   2003��04��04��
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
#include "mo_trust_prot_c.h"

static	struct mo_trust_prot_c	mo_trust_prot;
		
spLT01()  
{ 	
	int ret=0;
	int ac_seqn;


	memset (&mo_trust_prot, 0x00, sizeof(struct mo_trust_prot_c));

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	memset(&g_mdm_ac_rel,0x00,sizeof(g_mdm_ac_rel));
	memset(&g_ln_mst,0x00,sizeof(g_ln_mst));
	memset(&g_ln_parm,0x00,sizeof(g_ln_parm));
	vtcp_log("[%s][%d]ttl_prvd_amt===[%s]",__FILE__,__LINE__,g_pub_tx.ac_no);
	vtcp_log("[%s][%d]ttl_prvd_amt===[%d]",__FILE__,__LINE__,g_pub_tx.ac_seqn);
	/* ȡ������Ϣ */
	/*ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}*/
/******************/		
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel, "ac_no='%s'", g_pub_tx.ac_no);
		if( ret==100 )
		{
				sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼[%d][%s]",ret,g_pub_tx.ac_no);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"O001" );
				goto ErrExit;
		} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣[%d][%s]",ret,g_pub_tx.ac_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    	goto ErrExit;
  }
   	/* ȡ�������ļ���Ϣ */
   		vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.ttl_prvd_amt);
   		vtcp_log("[%s][%d]amt_lmt===[%lf]",__FILE__,__LINE__,g_ln_mst.amt_lmt);
	ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d" ,
				        g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		sprintf( acErrMsg,"mdm_ac_rel->ac_id=[%ld]ac_seqn=[%d]",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L002" );
		goto ErrExit;

	}		
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		goto ErrExit;
   	}
 vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.ttl_prvd_amt);
   		vtcp_log("[%s][%d]amt_lmt===[%lf]",__FILE__,__LINE__,g_ln_mst.amt_lmt);

   	/* ȡ���������Ϣ */
	ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm , " prdt_no='%s' " ,
				        g_ln_mst.prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"��������ļ����޴˼�¼ [%s][%d]",g_ln_mst.prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	} 
	else if( ret )
	{
   		sprintf( acErrMsg,"ȡ��������ļ��쳣 [%s][%d]",g_ln_mst.prdt_no,ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
   		goto ErrExit;
   	}
/********************/			
	vtcp_log("[%s][%d]ttl_prvd_amt===[%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.bal);
	vtcp_log("[%s][%d]ttl_prvd_amt===[%lf]",__FILE__,__LINE__,g_ln_mst.ttl_prvd_amt);
	if ( g_ln_parm.trust_prot_ind[0]=='N' )	
	{
		/* ��ί��Э����� */
		goto OkExit;
	}else{
		/* ί��Э����� */
		/* ȡί��Э����Ϣ */
		sprintf(acErrMsg,"~%d@%s trust_no[%s]", __LINE__, __FILE__, g_ln_mst.trust_no);
		WRITEMSG		
		ret = pub_ln_trust_info(g_ln_mst.trust_no, &mo_trust_prot );
		if( ret ) goto ErrExit;
		/* ���ݱ���ȡ��� */
		ret = pub_base_CurToSeqn( mo_trust_prot.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			goto ErrExit;	
		}
		
		sprintf(acErrMsg,"[%d]",ac_seqn);
		WRITEMSG		
		/* ������Ƿ��� */

		ret = pub_base_CompDblVal( mo_trust_prot.trust_amt - mo_trust_prot.out_amt , g_pub_tx.tx_amt1 );
		if( ret<0 )
		{
			sprintf(acErrMsg,"���������ί��Э��ɷſ������![%lf][%lf]", 
					mo_trust_prot.trust_amt-mo_trust_prot.out_amt ,g_pub_tx.tx_amt1 );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L108");
			goto ErrExit;
		}
		
		/* �޸��ѷſ��� */
		ret = sql_execute( "UPDATE mo_trust_prot SET out_amt=%lf WHERE trust_no='%s'" ,g_pub_tx.tx_amt1+mo_trust_prot.out_amt,g_ln_mst.trust_no );
		if( ret ) return ret;
		
		/* ���� */
		/* ��ί���˻� */			
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no)); /*��ac_noΪ��ʱ����ac_id�в��Ҽ�¼*/
		g_pub_tx.ac_id = mo_trust_prot.afund_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/ /*û��ȡѽ��gujy20060828*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"ί���ſ��ί�л�" );	
		/*** ���׼��ʴ��� ***/
    /*
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}*/
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		/* ���л�Ƽ��� */
		set_zd_data("102J",mo_trust_prot.cur_no);      
		set_zd_double("102F",g_pub_tx.tx_amt1);          
		strcpy(g_pub_tx.sub_tx_code,"D003");/**ȡ���ӽ���**/
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		/*	sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* ��ί�д�� */
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no)); /*��ac_noΪ��ʱ����ac_id�в��Ҽ�¼*/
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
			WRITEMSG
		strcpy( g_pub_tx.ac_get_ind,"00");	/*�Ѿ���ȡ��mdm_ac_rel*/ /*��ȡmdm_ac_rel gujy 20060828*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"ί���ſ�-������" );
	
			sprintf(acErrMsg,"<<&&>> %d@%s ac_id[%ld] ac_id_type[%s]", __LINE__, __FILE__, g_pub_tx.ac_id,  g_pub_tx.ac_id_type);
		/*** ���׼��ʴ��� ***/
	/*	if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* ���л�Ƽ��� */
		/*set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("0430",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"L104");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}*/
		
		
	}				  
		  		  				  	
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
	g_ln_mst.opn_date = g_pub_tx.tx_date;		/* �������� */
	get_zd_data("0370",g_pub_tx.ac_no);		/* ��/�ʺ� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* ��� */	
	get_zd_double("0430",&g_pub_tx.tx_amt1);	/* ���Ž�� */
	sprintf(acErrMsg,"���Ž��[%lf]!!",g_pub_tx.tx_amt1);
	WRITEMSG	
	sprintf(acErrMsg,"��/�ʺ�[%s]!!",g_pub_tx.ac_no);
	WRITEMSG
	sprintf(acErrMsg,"���[%d]!!",g_pub_tx.ac_seqn);
	WRITEMSG							
	return 0;
}
