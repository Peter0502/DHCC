/**********************************************************************
* �� �� ����  pub_eod_ln_trust
* �������ܣ�  ���գ�ί�д����
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��06��
*
* ��    ���� 
*     ��  �룺type   char(1)   ����(1-�ſ� 2-���� 3-��Ϣ)    
*      
*     ��  ��: 
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_trust_prot_c.h"

int pub_eod_ln_trust( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *type )
{
	int ret = 0;
	int ac_seqn;
	double amt1,amt2,amt3,amt4;
	
	struct mo_trust_prot_c	mo_trust_prot;
	memset (&mo_trust_prot, 0x00, sizeof(struct mo_trust_prot_c));
	vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n");
	if ( p_ln_parm.trust_prot_ind[0]=='N' )	
	{
		/* ��ί��Э����� */
		return 0;
	}
	
	/* ȡί��Э����Ϣ */
	ret = pub_ln_trust_info(p_ln_mst.trust_no, &mo_trust_prot );
	if( ret ) return -1;;
	/* ���ݱ���ȡ��� */
	/**
	ret = pub_base_CurToSeqn( mo_trust_prot.cur_no , &ac_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
		WRITEMSG
		return -1;;	
	}
	**/
	/********modify by ligl 2006-10-22 19:57*/
	ac_seqn=0;
	/********/
	vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%lf]\n",mo_trust_prot.trust_amt);
	sprintf(acErrMsg,"[%d]",ac_seqn);
	WRITEMSG		
	
	if( type[0]=='1')	/* �ſ�(ֻ�漰������) */
	{
		vtcp_log("byebyebyebyebye[%s][%d]\n",__FILE__,__LINE__);
		return(0);
		/* ������Ƿ��� */
		ret = pub_base_CompDblVal( mo_trust_prot.trust_amt - mo_trust_prot.out_amt , g_pub_tx.tx_amt1 );
		if( ret<0 )
		{
			sprintf(acErrMsg,"���������ί��Э��ɷſ������![%lf][%lf]", 
					mo_trust_prot.trust_amt-mo_trust_prot.out_amt ,g_pub_tx.tx_amt1 );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L108");
			return -1;
		}		
		/* ���� */
		/* ��ί���˻� */			
		g_pub_tx.ac_id = mo_trust_prot.afund_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		/*	 modified by liuxuan ��ǰ��10 ��ɼ��ʲ��� 2006-10-20
		strcpy( g_pub_tx.ac_get_ind,"10");	*�Ѿ���ȡ��mdm_ac_rel*
		*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"��ί���˻�" );	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			return -1;;
		}*/
		/* ���л�Ƽ��� */
		/*set_zd_data("102J",mo_trust_prot.cur_no);      
		set_zd_double("102F",g_pub_tx.tx_amt1);          
		strcpy(g_pub_tx.sub_tx_code,"D003");*//**ȡ���ӽ���**/
		/*strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			return -1;;
		}*/
		/* ��ί�д�� */
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"ί�д����˻�" );
	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			return -1;;
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
			return -1;;
		}	*/		
	}
	if( type[0]=='2')	/* ���� */
	{
		vtcp_log("byebyebyebyebye[%s][%d]\n",__FILE__,__LINE__);
		return(0);
		/* ��ί��Э�黹���˻�(��ָ��������ʻ�) */			
		g_pub_tx.ac_id = mo_trust_prot.amt_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		/*strcpy( g_pub_tx.ac_get_ind,"10");	*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"��ί�л����˻�" );	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			return -1;;
		}*/
		/* ���л�Ƽ��� */
		set_zd_data("101A",mo_trust_prot.cur_no);         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		/*ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			return -1;;
		}*/
		/* ��ί�д�� */
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
		strcpy ( g_pub_tx.brf,"��ί�д����˻�" );
	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			return -1;;
		}*/
		/* ���л�Ƽ��� */
		set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("107F",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"L201");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		/*ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			return -1;;
		}*/
	}		
	if( type[0]=='3')	/* ��Ϣ */
	{
		vtcp_log("LLLLLL����Ϣ��LLLLLLLLLLLLLLLLLLLLLL[%ld]\n",mo_trust_prot.intst_ac_id);
		amt1 = g_pub_tx.tx_amt1;
		amt2 = g_pub_tx.tx_amt2;
		amt3 = g_pub_tx.tx_amt3;
		amt4 = g_pub_tx.tx_amt4;
		g_pub_tx.tx_amt1 += g_pub_tx.tx_amt2+g_pub_tx.tx_amt3+g_pub_tx.tx_amt4;					
		/* ���� */
		/* ��ί��Э����Ϣ�˻� */
			vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%ld]\n",mo_trust_prot.intst_ac_id);

		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.intst_ac_id);
		if(ret) return -1;
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
			vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%s]\n",g_pub_tx.ac_no);
		g_pub_tx.ac_id = mo_trust_prot.intst_ac_id;
		g_pub_tx.ac_seqn = ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"ί�л�Ϣ" );	
		/*** ���׼��ʴ��� ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			return -1;;
		}
		/* ���л�Ƽ��� */
		set_zd_data("101A",mo_trust_prot.cur_no);         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.sub_tx_code,"D099");
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			return -1;;
		}
		/* ����501 ��Ϣ���� */
		memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",mo_trust_prot.ln_ac_id);
		if(ret) return -1;
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		vtcp_log("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL[%lf]\n",g_pub_tx.ac_no);
		g_pub_tx.ac_id = mo_trust_prot.ln_ac_id;
 		g_pub_tx.ac_seqn = mo_trust_prot.ln_ac_seqn ;
 		g_pub_tx.tx_amt1 = -g_pub_tx.tx_amt1;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=3010;	
		strcpy ( g_pub_tx.brf,"��Ϣ����" );
	
		/*** ���׼��ʴ��� ***/
		/*if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			return -1;;
		}*/
		/* ���л�Ƽ��� */
		set_zd_data("0210",g_ln_parm.cur_no); 
		set_zd_double("108H",g_pub_tx.tx_amt1);                                       
		strcpy(g_pub_tx.sub_tx_code,"LN03");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			return -1;;
		}
		g_pub_tx.tx_amt1 = amt1;
		g_pub_tx.tx_amt2 = amt2;
		g_pub_tx.tx_amt3 = amt3;
		g_pub_tx.tx_amt4 = amt4;	
	}
	return 0;
}

