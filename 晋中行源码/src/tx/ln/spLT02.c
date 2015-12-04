/*************************************************
* �� �� ��: spLT02.c
* ����������   
*              �������ί��Э������
*
* ��    ��:    lance
* ������ڣ�   2003��04��05��
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
		
spLT02()  
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
	
	/* ȡ������Ϣ */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm );
	if( ret ) {return ret;}	
	
	if ( g_ln_parm.trust_prot_ind[0]=='N' )	
	{
		/* ��ί��Э����� */
		goto OkExit;
	}else{
		/* ί��Э����� */
		/* ȡί��Э����Ϣ */
		ret = pub_ln_trust_info(g_ln_mst.trust_no, &mo_trust_prot );
		vtcp_log("[%s][%d] ��������: [%ld] \n",__FILE__,__LINE__,g_ln_mst.opn_date);
		/****����Э�����Ĵ���   add 20070824****/
		if(g_ln_mst.opn_date < 20070501)
		{
			if(ret == 100)
			{
				goto OkExit;
			}
		}
		/****����Э�����Ĵ���  end  20070824****/
		else if( ret ) 
		{
			goto ErrExit;
		}
		/* ���ݱ���ȡ��� */
		ret = pub_base_CurToSeqn( mo_trust_prot.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			goto ErrExit;	
		}
		ac_seqn=0;
		sprintf(acErrMsg,"[%d]",ac_seqn);
		WRITEMSG		
		
		/* ���� */
		/* ��ί��Э�黹���˻� */			
		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.amt_ac_id);
		if(ret) goto ErrExit;
		
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = mo_trust_prot.amt_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"ί�л���" );	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* ���л�Ƽ��� */
	/*	set_zd_data("101A",mo_trust_prot.cur_no);         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* ��ί�д�� */
		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.ln_ac_id);
		if(ret) goto ErrExit;
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"ί�д���" );
	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}*/
		/* ���л�Ƽ��� */
		/*set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("107F",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"L201");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
		*/
		
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
	get_zd_data("1071",g_pub_tx.ac_no);		/* ��/�ʺ� */
	get_zd_int("1072",&g_pub_tx.ac_seqn);		/* ��� */	
	get_zd_double("107E",&g_pub_tx.tx_amt1);	/* ���Ž�� */
	sprintf(acErrMsg,"���Ž��[%lf]!!",g_pub_tx.tx_amt1);
	WRITEMSG	
	sprintf(acErrMsg,"��/�ʺ�[%s]!!",g_pub_tx.ac_no);
	WRITEMSG
	sprintf(acErrMsg,"���[%d]!!",g_pub_tx.ac_seqn);
	WRITEMSG							
	return 0;
}
