/*************************************************************
* �� �� ��: gDLRE.c
* ���������������޲�
*
* ��    ��: jack
* �������: 2003��09��21��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if(ret) {\
		sprintf(acErrMsg,"SQLERROR [%d]",ret);\
		WRITEMSG\
		goto ErrExit;\
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "ln_mst_hst_c.h"
#include "ln_repair_c.h"
	
	int	ret=0;
	char	ln_ac_no[21];	
gDLRE()
{
	int	repair_cnt=0;
	int	ln_mst_hst_cnt=0;
	int	ln_mst_hst_n=0;
	int	tol_n=0;

	double	tx_amt=0.00;
	double	dd_tx_amt=0.00;
	
	char	where_list[101];
	char	dd_ac_no[21];
	
		
	struct ln_repair_c	S_ln_repair;
	struct ln_mst_hst_c	S_ln_mst_hst;
	
	memset( &S_ln_repair,0x00, sizeof( struct ln_repair_c));
	memset( &S_ln_mst_hst,0x00,sizeof( struct ln_mst_hst_c));
	
	
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
		
	strcpy(g_pub_tx.sub_tx_code,"LNRE");
	strcpy( g_pub_tx.tx_code, "LNRE");
	
	memset( &where_list ,0x0,strlen(where_list));

	
	ret= Ln_repair_Dec_Sel( g_pub_tx.reply, "1=1" );
	if(ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret = Ln_repair_Fet_Sel(&S_ln_repair, g_pub_tx.reply);
		if (ret&&ret!=100 )
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}else if( ret==100 ){
			sprintf(acErrMsg,"�޸��ļ���ѯ����  ����%d����",repair_cnt);
			WRITEMSG			
			break;
		}
		/* ȡ��ˮ�� xxx 7 lines*/
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"Get trace_no error [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );
		
		repair_cnt++;
		sprintf(acErrMsg,"�޸��ļ�  �ڡ�%d����",repair_cnt);
		WRITEMSG		
		sprintf( where_list, " tx_date=20050921 and tx_code='G083' and add_ind='0' and ac_id=%ld ",S_ln_repair.ac_id);
		sprintf(acErrMsg,"��ѯ��ˮʱ����  [%s]",where_list);
		WRITEMSG
		
		/* ȡ������ϸ���� */
		ret= Ln_mst_hst_Dec_Sel( g_pub_tx.reply, where_list );
		if(ret)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{		
			ln_mst_hst_n= 0;
			ret = Ln_mst_hst_Fet_Sel(&S_ln_mst_hst, g_pub_tx.reply);
			if (ret&&ret!=100 )
			{
				sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}else if( ret==100 ){
				sprintf(acErrMsg,"������ϸ��ѯ����  ����%d����",ln_mst_hst_cnt);
				WRITEMSG				
				break;
			}
			ln_mst_hst_cnt++;
			ln_mst_hst_n++;
			tol_n++;
			sprintf(acErrMsg,"��ϸ��ѯ  ac_id=[%ld]ln_type=[%s]�ڡ�%d���� �ڡ�%d����",S_ln_mst_hst.ac_id,S_ln_mst_hst.ln_tx_type,ln_mst_hst_cnt,ln_mst_hst_n);
			WRITEMSG
			sprintf(acErrMsg,"��ϸ��ѯ  ����%d����",tol_n );
			WRITEMSG
			
			tx_amt=0.00;
			dd_tx_amt=0.00;
			tx_amt=-S_ln_mst_hst.tx_amt;
			dd_tx_amt=-S_ln_mst_hst.tx_amt;
			
			/* ȡ������Ϣ */
			ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld ",S_ln_mst_hst.ac_id);
			if(ret)	goto ErrExit;
			strcpy( ln_ac_no, g_mdm_ac_rel.ac_no);
			/* �ж��Ƿ��� */
			ret= Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld ",S_ln_mst_hst.ac_id);
			if(ret)	goto ErrExit;
			ret= Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' ",g_ln_mst.prdt_no);
			if(ret)	goto ErrExit;
	/*-------------------------------------�����-------------------------------*/											
			/* ������Ϣ */
			if( S_ln_mst_hst.ln_tx_type[0]=='1' )
			{
				sprintf(acErrMsg,"�޸�����![%lf]",tx_amt );
				WRITEMSG
				ret=pay_in_lo_intst( &tx_amt );
				if( ret ) goto ErrExit;
				sprintf(acErrMsg,"�޸����ڽ���![%lf]",tx_amt );
				WRITEMSG
			}
			/* ������Ϣ */
			if( S_ln_mst_hst.ln_tx_type[0]=='2' )
			{
				sprintf(acErrMsg,"�޸�����!" );
				WRITEMSG
			        ret=pay_out_lo_intst( &tx_amt );
			        if( ret ) goto ErrExit;	
				sprintf(acErrMsg,"�޸��������![%lf]",tx_amt );
				WRITEMSG		     	 	    
      			}
			/* ������Ϣ */
			if( S_ln_mst_hst.ln_tx_type[0]=='3' )
			{
				sprintf(acErrMsg,"�޸�����!" );
				WRITEMSG
				ret=pay_cmpd_lo_intst( &tx_amt );
				if( ret ) goto ErrExit;	
				sprintf(acErrMsg,"�޸���������![%lf]",tx_amt );
				WRITEMSG		      
		      	}  
		      	/********   
			ret = sql_execute( "UPDATE ln_mst SET intst_acm=0.00,in_lo_acm=0.00,out_lo_acm=0.00,cmpd_lo_acm=0.00  \
										WHERE ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if( ret ) goto ErrExit;  
			***********/
	/*-------------------------------------ȡ���-------------------------------*/
			/* ȡ������Ϣ */
			ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_id=%ld ",g_ln_mst.repay_ac_id);
			if(ret)	goto ErrExit;
			strcpy( dd_ac_no, g_mdm_ac_rel.ac_no);	
			/* �ж��Ƿ�� */
			ret= Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst," ac_id=%ld and ac_seqn=1 ",g_ln_mst.repay_ac_id);
			if(ret)	goto ErrExit;
			ret= Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm," prdt_no='%s' ",g_dd_mst.prdt_no);
			if(ret)	goto ErrExit;
			sprintf(acErrMsg,"�޸����[%ld][%s]!" ,g_ln_mst.repay_ac_id,dd_ac_no);
			WRITEMSG				
		   	/**********ȡ���************/
	           	strcpy( g_pub_tx.ac_no , dd_ac_no);
			g_pub_tx.ac_id = g_dd_mst.ac_id;
	           	g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	           	strcpy( g_pub_tx.ac_get_ind,"011");	/*δ��ȡ��mdm_ac_rel*/
		   	strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-��� */
		   	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
		   	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		   	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
		   	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																	3���ջ�����*/ 
	           	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			g_pub_tx.ac_wrk_ind[9]='1';     /*���ڷǽ��㻧ǿ�пۿ�*/
			g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */
	
			strcpy( g_pub_tx.note_type,"299");
			strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
		   	g_pub_tx.svc_ind=1001;
		   	g_pub_tx.tx_amt1=dd_tx_amt;		   
	           	strcpy ( g_pub_tx.brf,"�˻�Ϣ��" );
		
	   		/*** ���׼��ʴ��� ***/
	  		if( pub_acct_trance() )
	   		{
               		sprintf(acErrMsg,"���׼��ʴ������!");
               		WRITEMSG
               		goto ErrExit;
	   		}
		   		/*** ���л�Ƽ��� ***/
	           	set_zd_data("102J",g_ln_parm.cur_no);      
	           	set_zd_double("102F",g_pub_tx.tx_amt1);          
	           	strcpy(g_pub_tx.sub_tx_code,"D003");/**ȡ���ӽ���**/
	           	strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
	           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
	           	if (ret != 0)
	           	{
	           		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
	           		WRITEMSG
	           		goto ErrExit;
	           	}
	       		
	       }
	       Ln_mst_hst_Clo_Sel();
	}
	Ln_repair_Clo_Sel();
	
OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�����޸��������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"AT12");/* ���޸� */
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�����޸��������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/* ��Ϣ--����ǷϢ */
int pay_in_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;

	sprintf(acErrMsg,"���뻹Ϣ--����ǷϢ[%lf]",bal);
	WRITEMSG
	
	/****
	ret = pub_base_CompDblVal( g_ln_mst.in_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����ǷϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L201" );
	        return 1;
	}
	*****/
		
	/* ȷ�������� */
	/* �������������߸պù��� */
	if( pub_base_CompDblVal( g_ln_mst.in_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt2 = bal ;
		bal = 0.00;
	}else{
		return 1; 
		g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst ;
		bal = bal - g_ln_mst.in_lo_intst;
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"2",-g_pub_tx.tx_amt2,0.00,"1",0 );
	if( ret ) 	return 1;			

	/****
	in_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"����ǷϢ��ʼ����[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
	*****/	
	strcpy( g_pub_tx.ac_no , ln_ac_no);
	g_pub_tx.ac_id = g_ln_mst.ac_id;
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;	
	strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/	
	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "������Ϣ�岹");
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108F",g_pub_tx.tx_amt2);
   	/*** ��Ƽ��� ***/
   	set_zd_data("0210",g_ln_parm.cur_no); 
   	set_zd_double("108F",g_pub_tx.tx_amt2);                                        
   	strcpy(g_pub_tx.sub_tx_code,"L202");
   	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
   	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
   	if (ret != 0)
   	{
   		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
   		WRITEMSG
   		return 1;
   	}
	g_pub_tx.tx_amt2=0.00;	         	
   	set_zd_double("108F",0.00);   	
	return 0;
	
}

/* ��Ϣ--����ǷϢ */
int pay_out_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--����ǷϢ[%lf]",bal);
	WRITEMSG
	/*****
	ret = pub_base_CompDblVal( g_ln_mst.out_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����ǷϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L200" );
	        return 1;
	}
	****/
	
	/* ȷ�������� */
	/* �������������߸պù��� */
	if( pub_base_CompDblVal( g_ln_mst.out_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt3 = bal ;
		bal = 0.00;
	}else{
		return 1; 
		g_pub_tx.tx_amt3 = g_ln_mst.out_lo_intst ;
		bal = bal - g_ln_mst.out_lo_intst;
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"3",-g_pub_tx.tx_amt3,0.00,"1",0 );
	if( ret ) 	return 1;	
			
	/*****
	out_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"����ǷϢ��ʼ����[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
	******/
	strcpy( g_pub_tx.ac_no , ln_ac_no);
	g_pub_tx.ac_id = g_ln_mst.ac_id;
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;	
	strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/	
	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */		
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "������Ϣ�岹");	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
		
	*amt = bal ;
	set_zd_double("108H",g_pub_tx.tx_amt3);
   	/*** ��Ƽ��� ***/
   	set_zd_data("0210",g_ln_parm.cur_no); 
	set_zd_double("108H",g_pub_tx.tx_amt3);                                        
   	strcpy(g_pub_tx.sub_tx_code,"L202");
   	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
   	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
   	if (ret != 0)
   	{
   		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
   		WRITEMSG
   		return 1;
   	}
	g_pub_tx.tx_amt3=0.00;	         	
   	set_zd_double("108H",0.00);   	
	return 0;	
	
}

/* ��Ϣ--����ǷϢ */
int pay_cmpd_lo_intst( double *amt )	
{
	double bal=0.00;
	
	bal = *amt;
	
	sprintf(acErrMsg,"���뻹Ϣ--����ǷϢ[%lf]",bal);
	WRITEMSG
	
	/*********	
	ret = pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst , 0.00 );
	if (ret==0)
	{
		sprintf(acErrMsg,"����ǷϢΪ��,���ù黹!���������û���ƻ���." );	
		WRITEMSG		
	        strcpy( g_pub_tx.reply,"L199" );
	        return 1;
	}
	**********/
	
	/* ȷ�������� */
	/* �������������߸պù��� */
	if( pub_base_CompDblVal( g_ln_mst.cmpd_lo_intst , bal ) >=0 )
	{
		g_pub_tx.tx_amt4 = bal ;
		bal = 0.00;
	}else{
		return 1; 
		g_pub_tx.tx_amt4 = g_ln_mst.cmpd_lo_intst ;
		bal = bal - g_ln_mst.cmpd_lo_intst;
	}
	
	/*д����Ǽǲ�*/
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"4",-g_pub_tx.tx_amt4,0.00,"1",0 );
	if( ret ) 	return 1;	

	/*****
	cmpd_lo_beg_date = g_pub_tx.acm_beg_date ;
	sprintf(acErrMsg,"����ǷϢ��ʼ����[%ld]",g_pub_tx.acm_beg_date);
	WRITEMSG
	*******/
	strcpy( g_pub_tx.ac_no , ln_ac_no);
	g_pub_tx.ac_id = g_ln_mst.ac_id;
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;	
	strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/	
	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */			
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );	/* ����:0��1��2���� */	
	g_pub_tx.svc_ind=3050;
	strcpy( g_pub_tx.brf, "������Ϣ�岹");	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
		
	*amt = bal ;
   	/*** ��Ƽ��� ***/
   	set_zd_data("0210",g_ln_parm.cur_no); 
	set_zd_double("108I",g_pub_tx.tx_amt4);                                      
   	strcpy(g_pub_tx.sub_tx_code,"L202");
   	strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
   	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
   	if (ret != 0)
   	{
   		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
   		WRITEMSG
   		return 1;
   	}
	g_pub_tx.tx_amt4=0.00;	         	
   	set_zd_double("108I",0.00);   				
	return 0;
	
}	
