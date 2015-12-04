/*************************************************
* �� �� ��:    pubf_ln_rol.c
* ����������   
*              get_ln_mst_intst    	 ������ˮ�ţ���ѯ������ϸ�ļ���ȡ����Ϣ���ͣ�����
*	      	   pub_ln_Udp_LnReg_Back	 �黹ǷϢ��Ǽǲ�����(����)
*	           pub_ln_Del_LnReg		 ������ˮ����ѯ������ϸ��ɾ����ؼ�¼
*              pub_ln_mst_acm_Back	 �������ļ��л�������(��������ʱ)
*              pub_ln_sect_acm_Back	 �ֶλ������л�������(���������ʱ)
*              Ins_ln_lo_rol		 �Ǽǰ���Ƿ����Ǽǲ�(���м�¼��ʱ�����һ����ʱ, ����Ƿ���������)
*
* ��    ��:    rob
* ������ڣ�   2003��07��06��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "ln_mst_hst_c.h"
#include "ln_reg_c.h"
#include "trace_log_c.h"
#include "ln_acm_reg_c.h"
#include "com_sect_acm_c.h"
#include "ln_lo_c.h"
#include "ln_lo_rol_c.h"
#include "ln_pay_pln_c.h"
#include "ln_pay_pln_rol_c.h"

/**********************************************************************
* �� �� ����  get_ln_mst_intst
* �������ܣ�  ������ˮ�ţ���ѯ������ϸ�ļ���ȡ����Ϣ���ͣ�����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��06��27��
*
* ��    ����
*     ��  �룺   ptrace_log		 ��ˮ�ṹ
*                acm_type  char[2]       ��Ϣ����    
*	         acm	  double	 ����
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int get_ln_mst_intst( ptrace_log , acm_type , acm )
  struct trace_log_c *ptrace_log;
  char *acm_type;
  double *acm;
 {
	int ret;
	struct ln_mst_hst_c	t_ln_mst_hst;
	memset( &t_ln_mst_hst , 0x00 , sizeof( struct ln_mst_hst_c ) );
	
	TRACE
	
	sprintf( acErrMsg , "��ˮtrace_no=%ld����trace_cnt=%d" , ptrace_log->trace_no , ptrace_log->trace_cnt);
	WRITEMSG
	
	TRACE
	
	ret = Ln_mst_hst_Sel( g_pub_tx.reply , &t_ln_mst_hst , "trace_no=%ld and trace_cnt=%d and tx_date=%ld " , ptrace_log->trace_no , ptrace_log->trace_cnt ,ptrace_log->tx_date);
	if( ret )	return 1;
	TRACE
	if( t_ln_mst_hst.ln_tx_type[0]=='0' )	/* ���� */
	{
		strcpy( acm_type , "1" );
	}else if( t_ln_mst_hst.ln_tx_type[0]=='1' ){	/* ���� */
		strcpy( acm_type , "2" );
	}else if( t_ln_mst_hst.ln_tx_type[0]=='2' ){	/* ���� */
		strcpy( acm_type , "3" );
	}else if( t_ln_mst_hst.ln_tx_type[0]=='3' || t_ln_mst_hst.ln_tx_type[0]=='5' || t_ln_mst_hst.ln_tx_type[0]=='6' || t_ln_mst_hst.ln_tx_type[0]=='7' ){	/* ���� */
		strcpy( acm_type , "4" );
	}		
	
	*acm = t_ln_mst_hst.intst_acm ;
	return 0;
 } 
	
	
/**********************************************************************
* �� �� ����  pub_ln_Udp_LnReg_Back
* �������ܣ�  �黹ǷϢ��Ǽǲ�����(����)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��06��27��
*
* ��    ����
*     ��  �룺   ac_id    integer        �˻�ID
*                ac_seqn  smallint       �ʻ����    
*                type     char[2]        �黹���ͣ�1����2����ǷϢ��3����ǷϢ��4����ǷϢ
*	         pamt	  double	 �黹���
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_Udp_LnReg_Back ( long ac_id , int ac_seqn , char *type , double pamt )
{
  	int ret;
  	double amt_tmp,amt;
	char sts[2];
	long tx_date=0;
	int flag=0;

  	struct ln_reg_c	ln_reg;	
	memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
	amt=pamt;

	sprintf(acErrMsg,"QQac_id[%ld]ac_seqn[%d]type[%s]",ac_id,ac_seqn,type);
	WRITEMSG	
	sprintf(acErrMsg,"���![%lf]",amt);
	WRITEMSG
	
	
	if( pub_base_CompDblVal( amt , 0.00 ) )
	{	
		ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type='%s' and sts='0'  order by reg_date_beg desc", ac_id,ac_seqn,type );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_reg_Dec_Sel��!ret=[%d]", ret);
			WRITEMSG		
			return(1);	
		}			
		
		flag=0;
	
		while(1)
		{
			ret = Ln_reg_Fet_Upd( &ln_reg, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		return 1;
	 		}
			else if( ret==100 ){
				break;
			}
	
			flag++;
			
			/* ������� */
			/* ���δ�����¼ */
			
			/* ������������ѻ�Ϣ��� */
			amt_tmp = amt - ln_reg.pay_bal;
			sprintf(acErrMsg,"amt![%lf]amt_tmp![%lf]",amt,amt_tmp);
			WRITEMSG
						
			if ( pub_base_CompDblVal( amt_tmp , 0.00 ) ==1 )
			{
				TRACE
				amt-=ln_reg.pay_bal;   /* ʣ���� */
				sprintf(acErrMsg,"���![%lf]",amt);
				WRITEMSG
				ln_reg.pay_bal = 0.00 ;/* �ѹ黹��� */
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd�쳣����!");
					WRITEMSG
					return 1;			
				}							
				
			}else if ( pub_base_CompDblVal( amt_tmp , 0.00 ) != 1 )	/* ��������С�ڻ�����ѻ�Ϣ��� */
			{
				TRACE
				ln_reg.pay_bal = ln_reg.pay_bal - amt ;	/* �ѹ黹��� */
				amt = 0.00;
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd�쳣����!");
					WRITEMSG
					return 1;			
				}

				break;
			}
					
		}
		Ln_reg_Clo_Upd ();
	}

	if( pub_base_CompDblVal( amt , 0.00 ) )
	{
		sprintf(acErrMsg,"���![%lf]",amt);
		WRITEMSG
		memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
		/***		
		ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type='%s' and sts='1' \
										ORDER BY reg_date DESC ", ac_id,ac_seqn,type );
		***/
		ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type='%s' and sts='1'  order by reg_date_beg desc", ac_id,ac_seqn,type );

		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_reg_Dec_Sel��!ret=[%d]", ret);
			WRITEMSG		
			return(1);	
		}			
		
		flag=0;
	
		while(1)
		{
			ret = Ln_reg_Fet_Upd( &ln_reg, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		return 1;
	 		}
			else if( ret==100 )
				break;
	
			flag++;
			
			/* ����� */
			/* ������������ѻ�Ϣ��� */
			amt_tmp = amt - ln_reg.pay_bal;
			
			if ( pub_base_CompDblVal( amt_tmp , 0.00 ) == 1 )
			{
				amt-=ln_reg.pay_bal;
	
				ln_reg.pay_bal = 0.00 ;/* �ѹ黹��� */
				strcpy( ln_reg.sts,"0" );
				ln_reg.pay_date=0;
				
		 		/* ���� �ѹ黹���,Ƿ��״̬,�������� *
				sprintf( acErrMsg,"UPDATE ln_reg SET pay_bal=0.00,pay_date=0,sts='0'\
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
		 		WRITEMSG
				ret = sql_execute( "UPDATE ln_reg SET pay_bal=0.00,pay_date=0,sts='0'\
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
				if( ret ) return ret;
				*/
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd�쳣����!");
					WRITEMSG
					return 1;			
				}
			}		
			else if ( pub_base_CompDblVal( amt_tmp , 0.00 ) != 1 )
			{
				ln_reg.pay_bal = ln_reg.pay_bal - amt ;	/* �ѹ黹��� */
				amt =0.00;
				strcpy( ln_reg.sts,"0" );
				ln_reg.pay_date=0;
		 		
		 		/* ���� �ѹ黹���,Ƿ��״̬,�������� *
				sprintf( acErrMsg,"UPDATE ln_reg SET pay_bal=%lf,pay_date=0,sts='0' \
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.pay_bal,ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
		 		WRITEMSG
				ret = sql_execute( "UPDATE ln_reg SET pay_bal=%lf,pay_date=0,sts='0' \
								WHERE ac_id=%ld and ac_seqn=%d and type='%s' and reg_date=%ld" , 
								ln_reg.pay_bal,ln_reg.ac_id,ln_reg.ac_seqn,ln_reg.type,ln_reg.reg_date );
				if( ret ) return ret;
				*/
				ret = Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
				if( ret )
				{					
					sprintf(acErrMsg,"Ln_reg_Upd_Upd�쳣����!");
					WRITEMSG
					return 1;			
				}
				break;
			}
		}
		Ln_reg_Clo_Upd ();		

	}
	
	return 0;
}	 	


/**********************************************************************
* �� �� ����  pub_ln_Del_LnReg
* �������ܣ�  ������ˮ����ѯ������ϸ��ɾ����ؼ�¼
*             ������Ϣֻ�����м����������,1-����ſ� 2-��ͨ���Ϣʱ�����²�������Ϣ
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��06��27��
*
* ��    ����
*     ��  �룺   ptrace_log		 ��ˮ�ṹ
*      		 amt_type		 ������� 1-����ſ� 2-��ͨ���Ϣʱ�����²�������Ϣ
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_Del_LnReg( ptrace_log , amt_type )
  struct trace_log_c *ptrace_log ;
  char amt_type[2];
{
	int ret=0;
	int del_flag=0;
	char tmpstr[500];
	char con_list[51];
	struct ln_mst_hst_c	t_ln_mst_hst;
	struct ln_reg_c		t_ln_reg;
	
	memset( &t_ln_mst_hst , 0x00 , sizeof( struct ln_mst_hst_c ) );
	memset( &t_ln_reg , 0x00 , sizeof( struct ln_reg_c ) );

	memset( &tmpstr , 0x00 , sizeof( tmpstr ) );
	memset( &con_list , 0x00 , sizeof( con_list ) );	
		
	/* ȡ������ϸ */
	ret = Ln_mst_hst_Sel( g_pub_tx.reply , &t_ln_mst_hst , "trace_no=%ld and trace_cnt=%d and tx_date=%ld " , ptrace_log->trace_no , ptrace_log->trace_cnt, g_pub_tx.tx_date );
	if( ret )	return 1;	
	
	sprintf(acErrMsg,"trace_no[%ld]trace_cnt[%d]",ptrace_log->trace_no , ptrace_log->trace_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"���![%lf]",ptrace_log->amt);
	WRITEMSG
	sprintf(acErrMsg,"����![%s]",t_ln_mst_hst.ln_tx_type);
	WRITEMSG
		
	/* ƴ��������� */
	if( amt_type[0]=='1' )
	{
		strcpy( tmpstr,"ac_id=%ld and ac_seqn=%d and reg_date_beg=%ld  and lo_bal=%lf and ind='0' " );
	}else if( amt_type[0]=='2' ){
		strcpy( tmpstr,"ac_id=%ld and ac_seqn=%d and reg_date_end=%ld  and lo_bal=%lf and ind='0' " );
	}else{
		sprintf(acErrMsg,"������ʹ���![%s]",amt_type);
		WRITEMSG
		return 1;		
	}
	/* ���� */
	/***
	if( t_ln_mst_hst.ln_tx_type[0]=='5' || t_ln_mst_hst.ln_tx_type[0]=='6' || t_ln_mst_hst.ln_tx_type[0]=='7' )
	{
	*******/
	if( t_ln_mst_hst.ln_tx_type[0]=='3' )
	{	
		strcpy( con_list , "and type='4' and sts='1' " );
	}
	
	/* ������� *
	if( t_ln_mst_hst.ln_tx_type[0]=='4' )
	{
		strcpy( con_list , "and type in ('2','3') and sts='1'" );
	}		
	*******/
	
	/* ������������ */
	if( t_ln_mst_hst.ln_tx_type[0]=='1' )	
	{
		TRACE
		strcpy( con_list , "and type='2' and sts='1' " );
	}	
	/* ������������ */				
	if( t_ln_mst_hst.ln_tx_type[0]=='2' )
	{
		TRACE	
		strcpy( con_list , "and type='3' and sts='1' " );
	}			
	
	/* ����� */
	if( t_ln_mst_hst.ln_tx_type[0]=='0' )
	{	
		TRACE
		strcpy( con_list , "and type='1' " );
	}	
	sprintf(acErrMsg,"����1tmpstr![%s]",tmpstr);
	WRITEMSG
	sprintf(acErrMsg,"con_list![%s]",con_list);
	WRITEMSG
	/***
	strcat(tmpstr,con_list);
	*******/
	
	sprintf(acErrMsg,"����3tmpstr![%s]",tmpstr);
	WRITEMSG	
	sprintf(acErrMsg,"[%ld][%d][%ld][%lf]",t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->amt);
	WRITEMSG	
	/* ɾ������ */
	del_flag=0;	/* ��¼ɾ�������� */
	ret = Ln_reg_Dec_Upd( g_pub_tx.reply , tmpstr , t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->amt );
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Ln_reg_Dec_Upd �α����[%d]",ret);
		WRITEMSG
		return 1;
	}
	ret = Ln_reg_Fet_Upd( &t_ln_reg , g_pub_tx.reply );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����ѹ黹��,�����������![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply , "L179");
		return 1;		
	}else if( ret ){
		sprintf( acErrMsg,"��ѯ����Ǽǲ���Ϣ�쳣����![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply , "L180");
		return 1;
	}
	if( del_flag )
	{
		Ln_reg_Clo_Upd( );
		return 0;
	}else{		
		del_flag++;
	}
	
	ret = Ln_reg_Del_Upd(t_ln_reg, g_pub_tx.reply );
	if( ret )
	{
		sprintf( acErrMsg,"Ln_reg_Del_Upd�쳣����![%d]",ret);
		WRITEMSG
		return 1;
	}
	Ln_reg_Clo_Upd( );		
	
	
	/* ���ݴ�����ϸ��Ϣȡ����Ǽǲ��м�¼----���� *
	if( t_ln_mst_hst.ln_tx_type[0]=='5' || t_ln_mst_hst.ln_tx_type[0]=='6' || t_ln_mst_hst.ln_tx_type[0]=='7' )
	{
		ret=sql_execute("delete from ln_reg where ac_id= %ld and ac_seqn=%d and reg_date=%ld and pay_date=%ld and type='4' and ind='0' and sts='1'",
				t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->tx_date );
		if( ret )	return 1;		
	}
	* ���ݴ�����ϸ��Ϣȡ����Ǽǲ��м�¼----������� *
	if( t_ln_mst_hst.ln_tx_type[0]=='4' )
	{
		ret=sql_execute("delete from ln_reg where ac_id= %ld and ac_seqn=%d and reg_date=%ld and pay_date=%ld  and ind='0' and sts='1' and type in ('2' ,'3') ",
				t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->tx_date );
		if( ret )	return 1;		
	}	
	* ���ݴ�����ϸ��Ϣȡ����Ǽǲ��м�¼-----���� *
	if( t_ln_mst_hst.ln_tx_type[0]=='0' )
	{
		ret=sql_execute("delete from ln_reg where ac_id= %ld and ac_seqn=%d and reg_date=%ld and pay_date=%ld  and ind='0' and sts='1' and type ='1'",
				t_ln_mst_hst.ac_id,t_ln_mst_hst.ac_seqn,ptrace_log->tx_date,ptrace_log->tx_date );
		if( ret )	return 1;		
	}
	*************/		
	return 0;
}

/**********************************************************************
* �� �� ����  pub_ln_mst_acm_Back
* �������ܣ�  �������ļ��л�������(��������ʱ)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��08��
*
* ��    ����
*     ��  �룺   trace_log	��ˮ�ṹ
*      
*     ��  ��:    ln_mst		�������ļ��ṹ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_mst_acm_Back ( struct trace_log_c	*s_trace_log , struct ln_mst_c *ln_mst_tmp) 
{
	int ret=0;
	int flag=0;
	
	struct ln_acm_reg_c	ln_acm_reg_tmp;
	
	memset( ln_mst_tmp , 0x00 , sizeof( struct ln_mst_c ) );
	memset( &ln_acm_reg_tmp , 0x00 , sizeof( struct ln_acm_reg_c ) );
		
	ret = Ln_acm_reg_Dec_Sel( g_pub_tx.reply , "trace_no=%ld and ac_id=%ld and ac_seqn=%d  and type='1' ", s_trace_log->trace_no,s_trace_log->ac_id,s_trace_log->ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_acm_reg_Dec_Sel��!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}			
	flag=0;
	
	while(1)
	{
		ret = Ln_acm_reg_Fet_Sel( &ln_acm_reg_tmp, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			break;
		}

		flag++;
		ln_mst_tmp->lst_date = ln_acm_reg_tmp.beg_date ; 
		ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date ;
		
		if( ln_acm_reg_tmp.intst_type[0]=='1' )	/*�������*/
		{
			ln_mst_tmp->intst_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else if( ln_acm_reg_tmp.intst_type[0]=='2' ){	/*����ǷϢ����*/
			ln_mst_tmp->in_lo_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else if( ln_acm_reg_tmp.intst_type[0]=='3' ){	/*����ǷϢ����*/
			ln_mst_tmp->out_lo_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else if( ln_acm_reg_tmp.intst_type[0]=='4' ){	/*����ǷϢ����*/
			ln_mst_tmp->cmpd_lo_acm = ln_acm_reg_tmp.intst_acm;
			ln_mst_tmp->ic_date = ln_acm_reg_tmp.beg_date;
		}else{
			break;
		}
	}
	Ln_acm_reg_Clo_Sel();
	
	sprintf( acErrMsg, "ִ�д���!flag=[%d]", flag);
	WRITEMSG
	
	return 0;
		
}

/**********************************************************************
* �� �� ����  pub_ln_sect_acm_Back
* �������ܣ�  �ֶλ������л�������(���������ʱ)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��07��08��
*
* ��    ����
*     ��  �룺   trace_log	��ˮ�ṹ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_sect_acm_Back ( struct trace_log_c	*s_trace_log ) 
{
	int ret=0;
	int flag_ln=0;
	int flag_acm=0;
	
	struct com_sect_acm_c	com_sect_acm_tmp;
	struct ln_acm_reg_c	ln_acm_reg_tmp;
	
	memset( &com_sect_acm_tmp , 0x00 , sizeof( struct com_sect_acm_c ) );
	memset( &ln_acm_reg_tmp , 0x00 , sizeof( struct ln_acm_reg_c ) );
		
	ret = Ln_acm_reg_Dec_Sel( g_pub_tx.reply , "trace_no=%ld  and ac_id=%ld and ac_seqn=%d  and type='0' ", s_trace_log->trace_no,s_trace_log->ac_id,s_trace_log->ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_acm_reg_Dec_Sel��!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}			
	flag_ln=0;
	flag_acm=0;	
	
	while(1)
	{
		ret = Ln_acm_reg_Fet_Sel( &ln_acm_reg_tmp, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_acm_reg_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			TRACE
			break;
		}

		flag_ln++;
		sprintf( acErrMsg, "flag_ln=[%d]", flag_ln);
		WRITEMSG		
		
		TRACE
		/* ����ln_reg_acm����Ϣ�޸ķֶλ����������� */
		ret = Com_sect_acm_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and beg_date=%ld and end_date=%ld and intst_acm=%lf and intst_type='%s' and sts='1' ", ln_acm_reg_tmp.ac_id,ln_acm_reg_tmp.ac_seqn,ln_acm_reg_tmp.beg_date,ln_acm_reg_tmp.end_date,ln_acm_reg_tmp.intst_acm,ln_acm_reg_tmp.intst_type );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Com_sect_acm_Dec_Upd��!ret=[%d]", ret);
			WRITEMSG	
			sprintf( acErrMsg, "ac_id=%ld and ac_seqn=%d and beg_date=%ld and end_date=%ld and intst_acm=%lf and intst_type='%s' and sts='1' ", ln_acm_reg_tmp.ac_id,ln_acm_reg_tmp.ac_seqn,ln_acm_reg_tmp.beg_date,ln_acm_reg_tmp.end_date,ln_acm_reg_tmp.intst_acm,ln_acm_reg_tmp.intst_type);
			WRITEMSG		
			return(1);	
		}			
		sprintf( acErrMsg, "trace_no=%ld  and ac_id=%ld and ac_seqn=%d  and type='0' ", s_trace_log->trace_no,s_trace_log->ac_id,s_trace_log->ac_seqn );
		WRITEMSG
		sprintf( acErrMsg, "ac_id=%ld and ac_seqn=%d and beg_date=%ld and end_date=%ld and intst_acm=%lf and intst_type='%s' and sts='1' ", ln_acm_reg_tmp.ac_id,ln_acm_reg_tmp.ac_seqn,ln_acm_reg_tmp.beg_date,ln_acm_reg_tmp.end_date,ln_acm_reg_tmp.intst_acm,ln_acm_reg_tmp.intst_type);
		WRITEMSG			
		
		while(1)
		{
			ret = Com_sect_acm_Fet_Upd( &com_sect_acm_tmp, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		TRACE
		 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		return 1;
	 		}else if( ret==100 ){
				sprintf( acErrMsg,"�޷��ڷֶλ������в鵽��ؼ�¼,�������Ļ�����ϵ!",ret);
		 		WRITEMSG
		 		strcpy( g_pub_tx.reply , "L186");
		 		break;
		 	}		 			 	
		 		
		 	/****
	 		}else if( ret==100 && flag_acm==0 ){
				sprintf( acErrMsg,"�޷��ڷֶλ������в鵽��ؼ�¼,�������Ļ�����ϵ!",ret);
		 		WRITEMSG
		 		strcpy( g_pub_tx.reply , "L186");
		 		return 1;
			}else if( ret==100 && flag_acm!=0){
				TRACE
				break;
			}
			********/
			flag_acm++;
			sprintf( acErrMsg, "flag_acm!=[%d]", flag_acm);
			WRITEMSG			
			/* �޸�״̬ */
			strcpy( com_sect_acm_tmp.sts , "0" );	/* �޸�״̬Ϊ"δ��Ϣ" */
			ret= Com_sect_acm_Upd_Upd (com_sect_acm_tmp, g_pub_tx.reply);		
		        if (ret)
		        {
		               sprintf(acErrMsg,"���·ֶλ��������!!");
		               WRITEMSG
		               return 1;
		        }    					
		}
		Com_sect_acm_Clo_Upd ();		
					
	}
	Ln_acm_reg_Clo_Sel();
	
	sprintf( acErrMsg, "ִ�д���!flag_acm=[%d]++flag_ln=[%d]", flag_acm,flag_ln);
	WRITEMSG
	
	return 0;
		
}



/**********************************************************************
* �� �� ����  Ins_ln_lo_rol
* �������ܣ�  �Ǽǰ���Ƿ����Ǽǲ�(���м�¼Ϊ  �����һ����ʱ����Ƿ���������)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��10��
*
* ��    ����
*     ��  �룺   ln_lo		 ���Ҵ���ǷϢ��ṹ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int Ins_ln_lo_rol( struct ln_lo_c  ln_lo_tmp )
{
	int ret=0;
	struct ln_lo_rol_c	ln_lo_rol;
	memset( &ln_lo_rol , 0x00 , sizeof( struct ln_lo_rol_c ) );
	
	TRACE
	
	sprintf( acErrMsg , "��ˮtrace_no=[%ld]" , g_pub_tx.trace_no );
	WRITEMSG
	
	TRACE
	
	ln_lo_rol.tx_date = g_pub_tx.tx_date;
	ln_lo_rol.trace_no = g_pub_tx.trace_no;
	strcpy ( ln_lo_rol.rol_ind , "0" ); 	/* 0-���� 1-���� */
	ln_lo_rol.ac_id = ln_lo_tmp.ac_id;
	ln_lo_rol.ac_seqn = ln_lo_tmp.ac_seqn; 
	ln_lo_rol.lo_term = ln_lo_tmp.lo_term;
	ln_lo_rol.lo_amt = ln_lo_tmp.lo_amt;
	ln_lo_rol.lo_intst = ln_lo_tmp.lo_intst;
	ln_lo_rol.pun_intst = ln_lo_tmp.pun_intst;
	strcpy(	ln_lo_rol.pay_type , ln_lo_tmp.pay_type );
	ln_lo_rol.over_date = ln_lo_tmp.over_date;
	ln_lo_rol.shld_pay_date = ln_lo_tmp.shld_pay_date;
	ln_lo_rol.pay_lo_amt = ln_lo_tmp.pay_lo_amt;
	ln_lo_rol.pay_lo_intst = ln_lo_tmp.pay_lo_intst;
	ln_lo_rol.pay_pun_intst = ln_lo_tmp.pay_pun_intst;
	strcpy(	ln_lo_rol.mac , ln_lo_tmp.mac);
	strcpy(	ln_lo_rol.old_ac_ind , ln_lo_tmp.old_ac_ind );

	ret = Ln_lo_rol_Ins( ln_lo_rol , g_pub_tx.reply );
	if( ret )	
	{
		sprintf( acErrMsg , "�Ǽǰ���Ƿ����Ǽǲ��쳣����![%d]" , ret );
		WRITEMSG		
		return 1;
	}
	TRACE
	return 0;
 } 

/**********************************************************************
* �� �� ����  Ins_ln_pay_pln_rol
* �������ܣ�  �Ǽǰ��һ���ƻ������Ǽǲ�(���м�¼Ϊ  �����һ����ʱ, �����ƻ���������)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��10��
*
* ��    ����
*     ��  �룺   ln_lo		 ���Ҵ���ǷϢ��ṹ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int Ins_ln_pay_pln_rol( struct ln_pay_pln_c  ln_pay_pln_tmp )
{
	int ret=0;
	struct ln_pay_pln_rol_c	ln_pay_pln_rol;
	memset( &ln_pay_pln_rol , 0x00 , sizeof( struct ln_pay_pln_rol_c ) );
	
	TRACE
	
	sprintf( acErrMsg , "��ˮtrace_no=[%ld]" , g_pub_tx.trace_no );
	WRITEMSG
	
	TRACE
	
	ln_pay_pln_rol.tx_date = g_pub_tx.tx_date;
	ln_pay_pln_rol.trace_no = g_pub_tx.trace_no;
	strcpy ( ln_pay_pln_rol.rol_ind , "0" ); 	/* 0-���� 1-���� */

	ln_pay_pln_rol.ac_id = ln_pay_pln_tmp.ac_id;
	ln_pay_pln_rol.ac_seqn = ln_pay_pln_tmp.ac_seqn;
	ln_pay_pln_rol.de_ac_id = ln_pay_pln_tmp.de_ac_id;
	ln_pay_pln_rol.de_ac_seqn = ln_pay_pln_tmp.de_ac_seqn;
	ln_pay_pln_rol.ttl_amt = ln_pay_pln_tmp.ttl_amt;
	ln_pay_pln_rol.ttl_cnt = ln_pay_pln_tmp.ttl_cnt;
	ln_pay_pln_rol.curr_cnt = ln_pay_pln_tmp.curr_cnt;
	ln_pay_pln_rol.curr_amt = ln_pay_pln_tmp.curr_amt;
	ln_pay_pln_rol.curr_intst = ln_pay_pln_tmp.curr_intst;
	ln_pay_pln_rol.wrk_date = ln_pay_pln_tmp.wrk_date;
	ln_pay_pln_rol.beg_date = ln_pay_pln_tmp.beg_date;
	ln_pay_pln_rol.end_date = ln_pay_pln_tmp.end_date;
	strcpy ( ln_pay_pln_rol.pay_ind , ln_pay_pln_tmp.pay_ind);
	
	ln_pay_pln_rol.ic_date_aj = g_ln_mst.ic_date;
	
	ret = Ln_pay_pln_rol_Ins( ln_pay_pln_rol , g_pub_tx.reply );
	if( ret )	
	{
		sprintf( acErrMsg , "�Ǽǰ��һ���ƻ������Ǽǲ��쳣����![%d]" , ret );
		WRITEMSG		
		return 1;
	}
	TRACE
	return 0;
 } 

/**********************************************************************
* �� �� ����  Reback_ln_pay_pln
* �������ܣ�  �ع����һ���ƻ���(���� ���һ���ƻ������Ǽǲ��ع�)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��11��
*
* ��    ����
*     ��  �룺   ln_pay_pln_rol		 ���Ҵ���ǷϢ������ṹ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/	
int Reback_ln_pay_pln( long trace_no )
{
	int ret=0;
	
	struct ln_pay_pln_rol_c	ln_pay_pln_rol_t;
	struct ln_pay_pln_c	ln_pay_pln;
	
	memset( &ln_pay_pln_rol_t , 0x00 , sizeof( struct ln_pay_pln_rol_c ) );
	memset( &ln_pay_pln , 0x00 , sizeof( struct ln_pay_pln_c ) );	

	TRACE
	
	sprintf( acErrMsg , "��ˮtrace_no=%ld" , trace_no );
	WRITEMSG
	sprintf( acErrMsg , "tx_date=%ld,trace_no=%ld" , g_pub_tx.tx_date , trace_no );
	WRITEMSG	
	TRACE	
	
	/* ȡ�� ���һ���ƻ������������� */
	ret = Ln_pay_pln_rol_Sel( g_pub_tx.reply , &ln_pay_pln_rol_t , "tx_date=%ld and trace_no=%ld " , g_pub_tx.tx_date , trace_no );
	if( ret&ret!=100 )	
	{
		return 1;
	}else if( ret==100 ){
		return 0;
	}
	
	TRACE
	if( ln_pay_pln_rol_t.rol_ind[0]=='1' )
	{
		/* ֻ��ԵǼǲ��޸�һ��,���Ѿ��޸Ĺ���,���������޸� */
		return 0;
	}
	
	/* �޸ķ�/����ƻ��������� */	
	ret = Ln_pay_pln_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d and pay_ind in ( '0','1' ) " , ln_pay_pln_rol_t.ac_id,ln_pay_pln_rol_t.ac_seqn );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}
	ret = Ln_pay_pln_Fet_Upd( &ln_pay_pln , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}else if (ret == 100){
		return 0;
	}

	ln_pay_pln.ac_id = ln_pay_pln_rol_t.ac_id;
	ln_pay_pln.ac_seqn = ln_pay_pln_rol_t.ac_seqn;
	ln_pay_pln.de_ac_id = ln_pay_pln_rol_t.de_ac_id;
	ln_pay_pln.de_ac_seqn = ln_pay_pln_rol_t.de_ac_seqn;
	ln_pay_pln.ttl_amt = ln_pay_pln_rol_t.ttl_amt;
	ln_pay_pln.ttl_cnt = ln_pay_pln_rol_t.ttl_cnt;
	ln_pay_pln.curr_cnt = ln_pay_pln_rol_t.curr_cnt;
	ln_pay_pln.curr_amt = ln_pay_pln_rol_t.curr_amt;
	ln_pay_pln.curr_intst = ln_pay_pln_rol_t.curr_intst;
	ln_pay_pln.wrk_date = ln_pay_pln_rol_t.wrk_date;
	ln_pay_pln.beg_date = ln_pay_pln_rol_t.beg_date;
	ln_pay_pln.end_date = ln_pay_pln_rol_t.end_date;
	strcpy( ln_pay_pln.pay_ind , ln_pay_pln_rol_t.pay_ind);
	
	ret = Ln_pay_pln_Upd_Upd( ln_pay_pln , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"���°��һ���ƻ������!!");
               WRITEMSG
               return 1;
        }          
	Ln_pay_pln_Clo_Upd();
	
	/* �޸� ���һ���ƻ������������� */
	ret=sql_execute("update ln_pay_pln_rol set rol_ind='1'  \
		where tx_date=%ld and trace_no=%ld ", 
		g_pub_tx.tx_date , trace_no );
	if( ret ) return 1;		
	return 0;
}



/**********************************************************************
* �� �� ����  Reback_ln_lo
* �������ܣ�  �ع�����Ƿ���(���� ����Ƿ����Ǽǲ��ع�)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��11��
*
* ��    ����
*     ��  �룺   ln_lo_rol		 ���Ҵ���ǷϢ������ṹ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/		
int Reback_ln_lo( long trace_no )
{
	int ret=0;
	
	struct ln_lo_rol_c	ln_lo_rol_t;
	struct ln_lo_c	ln_lo;
	
	memset( &ln_lo_rol_t , 0x00 , sizeof( struct ln_lo_rol_c ) );
	memset( &ln_lo , 0x00 , sizeof( struct ln_lo_c ) );	

	TRACE
	
	sprintf( acErrMsg , "��ˮtrace_no=%ld" , trace_no );
	WRITEMSG
	
	TRACE	
	
	/* ȡ�� ����Ƿ������������� */
	ret = Ln_lo_rol_Sel( g_pub_tx.reply , &ln_lo_rol_t , "tx_date=%ld and trace_no=%ld and rol_ind='0'" , g_pub_tx.tx_date , trace_no );
	if( ret&&ret!=100 )
	{
			return 1;
	}else if(ret ==100 ){
		  return 0;
	}
			
	if( ln_lo_rol_t.rol_ind[0]=='1' )
	{
		/* ֻ��ԵǼǲ��޸�һ��,���Ѿ��޸Ĺ���,���������޸� */
		return 0;
	}

	
	/* �޸ķ�/����ƻ��������� */	
	ret = Ln_lo_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d and lo_term=%d " , ln_lo_rol_t.ac_id,ln_lo_rol_t.ac_seqn,ln_lo_rol_t.lo_term );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}
	ret = Ln_lo_Fet_Upd( &ln_lo , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		return 1;
	}else if (ret == 100){
		return 0;
	}
	
	ln_lo.ac_id = ln_lo_rol_t.ac_id;
	ln_lo.ac_seqn = ln_lo_rol_t.ac_seqn;
	ln_lo.lo_term = ln_lo_rol_t.lo_term;
	ln_lo.lo_amt = ln_lo_rol_t.lo_amt;
	ln_lo.lo_intst = ln_lo_rol_t.lo_intst;
	ln_lo.pun_intst = ln_lo_rol_t.pun_intst;
	strcpy( ln_lo.pay_type , ln_lo_rol_t.pay_type );
	ln_lo.over_date = ln_lo_rol_t.over_date;
	ln_lo.shld_pay_date = ln_lo_rol_t.shld_pay_date;
	ln_lo.pay_lo_amt = ln_lo_rol_t.pay_lo_amt;
	ln_lo.pay_lo_intst = ln_lo_rol_t.pay_lo_intst;
	ln_lo.pay_pun_intst = ln_lo_rol_t.pay_pun_intst;
	strcpy( ln_lo.mac , ln_lo_rol_t.mac );
	strcpy( ln_lo.old_ac_ind , ln_lo_rol_t.old_ac_ind );

	ret = Ln_lo_Upd_Upd( ln_lo , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"���°���Ƿ������!!");
               WRITEMSG
               return 1;
        }          
	Ln_lo_Clo_Upd();
	
	/* �޸� ����Ƿ�������������״̬ */
	ret=sql_execute("update ln_lo_rol set rol_ind='1' \
		where tx_date=%ld and trace_no=%ld and lo_term=%d", 
		g_pub_tx.tx_date , trace_no , ln_lo_rol_t.lo_term );
	if( ret ) return 1;	
	return 0;
}


/**********************************************************************
* �� �� ����  check_ln_mst_no
* �������ܣ�  ������ˮ�ţ���ѯ������ϸ��,�Ƿ��д˽��׺���(������Ƿ������һ��)
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��23��
*
* ��    ����
*     ��  �룺   ptrace_log		 ��ˮ�ṹ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int check_ln_mst_no( ptrace_log )
  struct trace_log_c *ptrace_log;
 {
	int ret=0;
	int flag=0;
	long max_trace_no=0;
	struct ln_mst_hst_c	S_ln_mst_hst;
	struct trace_log_c	LS_trace_log;
	memset( &S_ln_mst_hst, 0x00 , sizeof( struct ln_mst_hst_c ));
	
	TRACE
	
	sprintf( acErrMsg , "��ˮtrace_no=%ld����trace_cnt=%d" , ptrace_log->trace_no , ptrace_log->trace_cnt);
	WRITEMSG
	
	TRACE
    sprintf(acErrMsg,"max_trace_no=[%ld]ptrace_log->trace_no=[%ld]!!",max_trace_no , ptrace_log->trace_no);
    WRITEMSG
    sprintf(acErrMsg,"ac_id=%ld and ac_seqn=%d and tx_date=%ld", 
							ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
    WRITEMSG	
	/* ȡ��Ȼ��ϸ�����ڸ��˻������һ����ˮ�� *
	ret = sql_max_long( "ln_mst_hst", "trace_no", &max_trace_no, 
				"ac_id=%ld and ac_seqn=%d and tx_date=%ld and tx_code!='5701'",
				ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
	if( ret )	return 1;
	TRACE

	if( max_trace_no > ptrace_log->trace_no )
	{
               sprintf(acErrMsg,"����δ��������!!");
               WRITEMSG
               sprintf(acErrMsg,"max_trace_no=[%ld]ptrace_log->trace_no=[%ld]!!"
											,max_trace_no , ptrace_log->trace_no);
               WRITEMSG
               sprintf(acErrMsg,"ac_id=%ld and ac_seqn=%d and tx_date=%ld",
										ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
               WRITEMSG
               strcpy(  g_pub_tx.reply ,"L231" );
               return 1;
	}
	******/
	
	ret = Ln_mst_hst_Dec_Sel( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and tx_date=%ld and trace_no>%ld and tx_code!='5701'",
				ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date,ptrace_log->trace_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_mst_hst_Dec_Sel��!ret=[%d]", ret);
		WRITEMSG		
		return 1;	
	}			
	
	while(1)
	{
		ret = Ln_mst_hst_Fet_Sel( &S_ln_mst_hst, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_mst_hst_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}else if( ret==100 ){
			break;
		}else if( ret==0 ){
			flag++;
			ret = Trace_log_Sel_rol( g_pub_tx.reply, &LS_trace_log, "trace_no=%ld and tx_date=%ld", S_ln_mst_hst.trace_no,g_pub_tx.tx_date );
			if(ret) return 1;
			if( LS_trace_log.sts[0]=='1' )
			{
				continue;
			}else{
				sprintf(acErrMsg,"����δ��������!!");
				WRITEMSG
				sprintf(acErrMsg,"NOW_trace_no=[%ld]ptrace_log->trace_no=[%ld]!!"
									,S_ln_mst_hst.trace_no , ptrace_log->trace_no);
				WRITEMSG
				sprintf(acErrMsg,"ac_id=%ld and ac_seqn=%d and tx_date=%ld",
								ptrace_log->ac_id , ptrace_log->ac_seqn ,ptrace_log->tx_date);
				WRITEMSG
				strcpy(  g_pub_tx.reply ,"L231" );
				return 1;
			}
		}	
	}
	Ln_mst_hst_Clo_Sel();
	sprintf( acErrMsg, "���������%d��", flag);
	WRITEMSG	
	return 0;
 } 
	
