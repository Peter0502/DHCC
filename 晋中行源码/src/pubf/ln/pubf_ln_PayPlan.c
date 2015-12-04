/*************************************************
* �� �� ��: pubf_ln_PayPlan.c
* ����������   
* 			pub_ln_cal_PayPlan     ���㰴�Ҵ���ÿ���軹�������Ϣ
* 			pub_ln_PayPlan         ���ɻ���ƻ�
* 			pub_ln_Chg_PayPlan     Ҫ�ر仯,�������ɻ���ƻ�
*
* ��    ��:    rob
* ������ڣ�   2003��12��28��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"

/**********************************************************************
* �� �� ����  pub_ln_cal_PayPlan
* �������ܣ�  ����ÿ���軹�������Ϣ
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��12��28��
*
* ��    ����
*     ��  �룺ttl_amt   decimail(16,2) �����ܶ�   
*             ttl_cnt   smailint       ���ڴ�
*             pay_typ   char(1)        ���ҷ�ʽ���ȶ�ȱ���
				0�ȶ����Ϣ1�ȶ�����ڻ�Ϣ
*             rate      decimal(8,5)   ����
*	          cnt       smallint       ��������
*      
*     ��  ��:           
*	          amt       decimal(16,2)  ����Ӧ������			
*	          intst     decimal(16,2)  ����Ӧ����Ϣ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
********************************************************************/
int pub_ln_cal_PayPlan ( double ttl_amt , int ttl_cnt , char *pay_typ , 
	double rate , int cnt , double *amt , double *intst )
{
    double tmp1,tmp2;

	/***
	rate = rate/1000;
	****/
	sprintf(acErrMsg,"ttl_amt=[%lf],ttl_cnt=[%ld],rate=[%lf],cnt=[%ld]",ttl_amt,ttl_cnt,rate,cnt);
	WRITEMSG
	switch( atoi( pay_typ ) )
	{
	case 0:		/*�ȶ��*/
		tmp1 = pow((1+rate),ttl_cnt);
		
		tmp2 = ttl_amt * rate * tmp1 /(tmp1-1);
		tmp1 = pow((1+rate),(ttl_cnt - cnt+1));
		*amt = pub_base_PubDround_1(tmp2/tmp1);
		*intst = tmp2 - *amt;
		*intst = pub_base_PubDround_1(*intst);
		break;

	case 1:		/*�ȱ�����*/
		*amt = pub_base_PubDround_1(ttl_amt / ttl_cnt);
		*intst = pub_base_PubDround_1((1.0-(cnt-1.0)/ttl_cnt)*ttl_amt*rate);
		break;
	
	default:
		strcpy(g_pub_tx.reply,"L013");
		sprintf(acErrMsg,"���ҷ�ʽ�������� pay_typ= '%s' ", *pay_typ);
		WRITEMSG
		return 1;

	}		

	return 0;
}


/**********************************************************************
* �� �� ����  pub_ln_PayPlan
* �������ܣ�  ���ɻ���ƻ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��01��20��
*
* ��    ����
*     ��  �룺
*	      ln_mst	�������ļ��ṹ
*             ln_parm   ���������ṹ
*	      cnt        smallint      ��������
*         type        ����ԭ�� 0���� 1��Ϊ�޸Ķ�����
*      
*     ��  ��:  
*             ln_pay_pln ���Ҵ����ƻ��ṹ 
*                    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
********************************************************************/
int pub_ln_PayPlan( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , int cnt , 
				struct ln_pay_pln_c *p_ln_pay_pln , int type )
{
	long fir_day=0,term_opn_date=0;
	long month_cnt=0;
	double tmp1=0.00,tmp2=0.00,ln_amt=0.00;
	int ret=0;
	int iAc_seqn = 0; /***Add By zgf 2015/4/16 ***/
	struct ln_pay_pln_c	t_ln_pay_pln;
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	TRACE
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	memset(p_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);
	memset( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	TRACE
	vtcp_log("[%s][%d][%ld]",__FILE__,__LINE__,g_ln_mst.ac_id);

	if( p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
		type=0;
	}else{
		type=1;
	}
	
	/* ���������� */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 
	if( ret ) return 1;
	sprintf(acErrMsg,"������month_cnt[%ld]",month_cnt);
	WRITEMSG	
	/* ��ɳϵͳ�������޸ĺ������*/
	if( type==1 )
	{
		/* ���ڻ����յ���ǰ */
		if( term_opn_date%100 < p_ln_parm.fst_pay_day )
		{
			month_cnt=month_cnt;
		}else{
			month_cnt=month_cnt+1;	
		}	
	}
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%p_ln_parm.pay_amt_term==0 )
		{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}
	sprintf(acErrMsg,"������p_ln_pay_pln->ttl_cnt[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG	
	/* ����ÿ�ڿ�ʼ�黹���ڡ��������� */
	if(cnt==1)
	{
		term_opn_date = p_ln_mst.ic_date;	
	}else{
		ret = pub_base_deadlineM(p_ln_mst.ic_date,(cnt-1)*p_ln_parm.pay_amt_term,&term_opn_date);
		if(ret) return 1;
	}	
	sprintf(acErrMsg,"term_opn_date[%ld]",term_opn_date);
	WRITEMSG			
	if( p_ln_parm.pay_term_type[0]=='M' )
	{	
		if( p_ln_parm.fst_pay_ind[0]=='M' )
		{
			/* ���ڻ����յ���ǰ */
			if( term_opn_date%100 <= p_ln_parm.fst_pay_day )
			{
				p_ln_pay_pln->beg_date=term_opn_date/100*100+p_ln_parm.fst_pay_day;
				p_ln_pay_pln->end_date=term_opn_date/100*100+p_ln_parm.mon_pay_day;	
			}else{
				/* ���ڻ����յ��պ� */
				if( term_opn_date%10000<=1200 )/*����12�·�*/
				{
			 		p_ln_pay_pln->beg_date=term_opn_date/100*100+100+p_ln_parm.fst_pay_day;
			   		p_ln_pay_pln->end_date=term_opn_date/100*100+100+p_ln_parm.mon_pay_day;
				}else{
					p_ln_pay_pln->beg_date=term_opn_date/10000*10000+10100+p_ln_parm.fst_pay_day;
					p_ln_pay_pln->end_date=term_opn_date/10000*10000+10100+p_ln_parm.mon_pay_day;
				}

			}
		}else if( p_ln_parm.fst_pay_ind[0]=='F' ){
			p_ln_pay_pln->beg_date = term_opn_date;
			p_ln_pay_pln->end_date = term_opn_date;			
		}
	}	
	/* �����޸� */
	if( type==0 )
	{
		if( p_ln_mst.opn_date%10000<=1221 )	/*����1221��֮ǰ*/
		{
			/* ���ճ�ɳ����������Ҫ������˳��һ���� */
			pub_base_deadlineM( p_ln_pay_pln->beg_date , 1 , &p_ln_pay_pln->beg_date );
			pub_base_deadlineM( p_ln_pay_pln->end_date , 1 , &p_ln_pay_pln->end_date );	
		}
	}
	
	/* �����һ���޸�Ϊ�������� */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->beg_date = p_ln_mst.mtr_date;
		p_ln_pay_pln->end_date = p_ln_mst.mtr_date;			
	}
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG			
	/***************************
	 				
	if ( p_ln_parm.pay_term_type[0]=='Y' )
	{
		* ȷ�������� *
		sprintf(acErrMsg,"��!����[%d]",p_ln_parm.pay_amt_term);
		WRITEMSG
		sprintf(acErrMsg,"�ڴ�[%d]",cnt);
		WRITEMSG
		fir_day = (p_ln_mst.ic_date/10000)*10000 + 101;
		sprintf(acErrMsg,"������[%ld]",fir_day);
		WRITEMSG
		* ��ʼ�黹���� *
		month = p_ln_parm.pay_amt_term * cnt * 12;
		sprintf(acErrMsg,"����[%d]",month);
		WRITEMSG
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->beg_date);
		if (ret)
		{
			sprintf(acErrMsg,"���ú���pub_base_deadlineM����!");
			WRITEMSG
			return 1;	
		}
		sprintf(acErrMsg,"��ʼ�黹����[%ld]",p_ln_pay_pln->beg_date);
		WRITEMSG
		
		* �������� *
		month = p_ln_parm.pay_amt_term * (cnt+1) * 12;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->end_date );
		if (ret)
		{
			sprintf(acErrMsg,"���ú���pub_base_deadlineM����!");
			WRITEMSG
			return 1;	
		}	
		p_ln_pay_pln->end_date = pub_base_daynumSUB(p_ln_pay_pln->end_date, 1);
		sprintf(acErrMsg,"��������[%ld]",p_ln_pay_pln->end_date);
		WRITEMSG
		
		* ������ *
		if ( (p_ln_mst.mtr_date - fir_day) > 10000*p_ln_parm.pay_amt_term )
		{
				if ( (p_ln_mst.mtr_date - fir_day) % (10000*p_ln_parm.pay_amt_term) )	
				{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(10000*p_ln_parm.pay_amt_term);
				}else{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(10000*p_ln_parm.pay_amt_term)-1;
				}
		}else{
			p_ln_pay_pln->ttl_cnt=1;
		}					
	}
	
	if ( p_ln_parm.pay_term_type[0]=='Q' )
	{
		* ȷ�������� *
		switch((p_ln_mst.ic_date%10000)/100)
		{
			case 1:
			case 2:
			case 3:	fir_day=(p_ln_mst.ic_date/10000)*10000 + 101;
				break;
			case 4:
			case 5:
			case 6: fir_day=(p_ln_mst.ic_date/10000)*10000 + 401;
				break;
			case 7:
			case 8:
			case 9: fir_day=(p_ln_mst.ic_date/10000)*10000 + 701;
				break;		
			case 10:
			case 11:
			case 12: fir_day=(p_ln_mst.ic_date/10000)*10000 + 1001;
				break;	
		}	
		* ��ʼ�黹���� *
		month = p_ln_parm.pay_amt_term * cnt * 3;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->beg_date );
		if (ret)
		{
			sprintf(acErrMsg,"���ú���pub_base_deadlineM����!");
			WRITEMSG
			return 1;	
		}
		* �������� *
		month = p_ln_parm.pay_amt_term * (cnt+1) * 3;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->end_date );
		if (ret)
		{
			sprintf(acErrMsg,"���ú���pub_base_deadlineM����!");
			WRITEMSG
			return 1;	
		}	
		p_ln_pay_pln->end_date = pub_base_daynumSUB(p_ln_pay_pln->end_date, 1);
		* ������ *
		if ( (p_ln_mst.mtr_date - fir_day) > 300*p_ln_parm.pay_amt_term )
		{
				if ( (p_ln_mst.mtr_date - fir_day) % (300*p_ln_parm.pay_amt_term) )	
				{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(300*p_ln_parm.pay_amt_term);
				}else{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(300*p_ln_parm.pay_amt_term)-1;
				}
		}else{
			p_ln_pay_pln->ttl_cnt=1;
		}					
	}						
	if ( p_ln_parm.pay_term_type[0]=='M' )
	{
		* ȷ�������� *
		fir_day = p_ln_mst.ic_date/100+1;
		* ��ʼ�黹���� *
		month = p_ln_parm.pay_amt_term * cnt;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->beg_date );
		if (ret)
		{
			sprintf(acErrMsg,"���ú���pub_base_deadlineM����!");
			WRITEMSG
			return 1;	
		}
		* �������� *
		month = p_ln_parm.pay_amt_term * (cnt+1) * 3;
		ret = pub_base_deadlineM( fir_day, month, &p_ln_pay_pln->end_date );
		if (ret)
		{
			sprintf(acErrMsg,"���ú���pub_base_deadlineM����!");
			WRITEMSG
			return 1;	
		}	
		p_ln_pay_pln->end_date = pub_base_daynumSUB(p_ln_pay_pln->end_date, 1);
		* ������ *
		if ( (p_ln_mst.mtr_date - fir_day) > 100*p_ln_parm.pay_amt_term )
		{
				if ( (p_ln_mst.mtr_date - fir_day) % (100*p_ln_parm.pay_amt_term) )	
				{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(100*p_ln_parm.pay_amt_term);
				}else{
					p_ln_pay_pln->ttl_cnt=(p_ln_mst.mtr_date - fir_day)/(100*p_ln_parm.pay_amt_term)-1;
				}
		}else{
			p_ln_pay_pln->ttl_cnt=1;
		}					
	}							
	*****************************/
	/* ���㵱��Ӧ�� */
	
	p_ln_mst.rate = p_ln_mst.rate/1000;
	
	ret = pub_base_CompDblVal( p_ln_mst.bal, 0.00 );
	if ( ret==0 )
	{
		TRACE
		if(p_ln_mst.ic_date = g_pub_tx.tx_date )
		{
			TRACE
			ln_amt = p_ln_mst.amt_lmt;
		}
	}else{
		if( type==1)
		{
			TRACE
			ln_amt = p_ln_mst.bal;
		}else if ( type==0 ){						
		/* ȡԭ����ƻ��� */
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d and pay_ind='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn); 
		if( ret==100 )
		{
			sprintf(acErrMsg,"����ƻ�������ؼ�¼[%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L135");
			return 1;
		}else if( ret ){
			sprintf(acErrMsg,"��ѯ����ƻ������쳣����![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
		sprintf(acErrMsg,"rob!!![%lf]",t_ln_pay_pln.ttl_amt);
		WRITEMSG
		ln_amt = t_ln_pay_pln.ttl_amt;	
		TRACE
		}
	}
	sprintf(acErrMsg,"ln_amt=[%lf]!",ln_amt);
	WRITEMSG	
	switch( atoi( p_ln_parm.ln_pay_type ) )
	{
	case 3:		/*�ȶ��*/
		tmp1 = pow((1+p_ln_mst.rate),p_ln_pay_pln->ttl_cnt);
		
		tmp2 = ln_amt * p_ln_mst.rate * tmp1 /(tmp1-1);
		tmp1 = pow((1+p_ln_mst.rate),(p_ln_pay_pln->ttl_cnt - cnt+1));
		p_ln_pay_pln->curr_amt= pub_base_PubDround_1(tmp2/tmp1);
		p_ln_pay_pln->curr_intst = tmp2 - p_ln_pay_pln->curr_amt; 
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1(p_ln_pay_pln->curr_intst);
		break;

	case 4:		/*�ȱ�����*/
		p_ln_pay_pln->curr_amt = pub_base_PubDround_1(ln_amt / p_ln_pay_pln->ttl_cnt);
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1((1.0-(cnt-1.0)/p_ln_pay_pln->ttl_cnt)*ln_amt*p_ln_mst.rate);
		break;
	
	default:
		strcpy(g_pub_tx.reply,"O002");
		sprintf(acErrMsg,"����֧����ʽ�밴�Ҵ���֧����ʽ����,����ϵ���Ļ���! pay_typ= '%s' ",p_ln_parm.ln_pay_type );
		WRITEMSG
		return 1;

	}		
	/* �����һ���޸���Ƿ����Ϊ������� */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}
		
	/* �Ի���ƻ�������ֵ��ֵ */
	
	p_ln_pay_pln->ttl_amt=ln_amt; /* ԭʼ���� */
	p_ln_pay_pln->ac_id=p_ln_mst.ac_id; /* �˺�ID */
	p_ln_pay_pln->ac_seqn=p_ln_mst.ac_seqn; /* �˺���� */	
	p_ln_pay_pln->curr_cnt=cnt; /* ��ǰ�ڴ� */		
			p_ln_pay_pln->wrk_date=g_pub_tx.tx_date; /* �Ǽ����� */	
			strcpy ( p_ln_pay_pln->pay_ind , "0" );	/* ����״̬0-δ�� */
			p_ln_pay_pln->de_ac_id = p_ln_mst.repay_ac_id ;	/* �����˻�ID */
			/* ���ݱ������ɻ��ڻ����˺�ID */ 
			/**Mod by zgf 2014/4/2 32λ-64λǨ��,longָ�븳ֵ��int��ָ����� start*/
			/*ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &p_ln_pay_pln->de_ac_seqn );*/
                        ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &iAc_seqn );
                        /**Mod by zgf 2015/4/16 32λ-64λǨ��,longָ�븳ֵ��int��ָ����� end **/
			if ( ret )
			{
				sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
				WRITEMSG
				return 1;	
			}			
			p_ln_pay_pln->de_ac_seqn = iAc_seqn;/**Mod by zgf 2015/4/16***/		

			sprintf(acErrMsg,"ԭʼ����[%lf]",p_ln_pay_pln->ttl_amt);
			WRITEMSG
			sprintf(acErrMsg,"�˺�ID[%ld]",p_ln_pay_pln->ac_id);
			WRITEMSG
	sprintf(acErrMsg,"�˺����[%d]",p_ln_pay_pln->ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG
	sprintf(acErrMsg,"�Ǽ�����[%ld]",p_ln_pay_pln->wrk_date);
	WRITEMSG
	sprintf(acErrMsg,"����״̬0[%s]",p_ln_pay_pln->pay_ind);
	WRITEMSG
	
	
	sprintf(acErrMsg,"������[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",p_ln_pay_pln->curr_amt);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",p_ln_pay_pln->curr_intst);
	WRITEMSG
	sprintf(acErrMsg,"��ʼ�黹����[%ld]",p_ln_pay_pln->beg_date);
	WRITEMSG
	sprintf(acErrMsg,"���ڹ黹����[%ld]",p_ln_pay_pln->end_date);
	WRITEMSG
				
	return 0;	
}
/**********************************************************************
* �� �� ����  pub_ln_Chg_PayPlan
* �������ܣ�  ����ƻ����
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��04��01��
*
* ��    ����
*     ��  �룺
*	      ln_mst	�������ļ��ṹ
*             ln_parm   ���������ṹ
*	      rate      ���ĺ������
*	      ln_bal	���ĺ�ı���
*	      beg_date	��ʼ���� 
*	      end_date	��ֹ����   
*    
*     ��  ��:   
*                    
*
*     ����ֵ�� 0 �ɹ�
*            -1  ʧ��
*             1  �������Ʋ����޸�
*
* �޸���ʷ��
********************************************************************/
int pub_ln_Chg_PayPlan( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double rate , 
			double ln_bal , long beg_date , long end_date )
{
	long fir_day=0,term_opn_date=0,max_day=0,old_opn_date=0;
	long month_cnt=0;
	int ret=0 , cnt=0 , ln_cnt=0,now_ttl_cnt=0;
	int chg_flag=0;
	long dqr=0;
	double rate_val=0.00;
	struct ln_mst_c    ln_mst_tmp;
	struct ln_mst_c    ln_mst_upd;	
	struct ln_pay_pln_c    t_ln_pay_pln;
	struct ln_pay_pln_c    ln_pay_pln;
	
	memset ( &ln_mst_upd , 0x00 , sizeof(struct ln_mst_c) );			
	memset ( &ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset ( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset ( &ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
		
	/* �ж��Ƿ���Ƿ�� */
	/* ��ѯ����Ƿ��� */
	ret = sql_count( "ln_lo" , "ac_id=%ld and ac_seqn=%d  and pay_type='0'" , p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg, "�ô������Ƿ��,�����������ɻ���ƻ�[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L084");
		return 1;
	}		
	
	/* ��鵱���Ƿ��Ի� */

	ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d and pay_ind='0'"
					 , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����ƻ�����Ϣ���޴���Ϣ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L043" );
		return -1;
	}else if( ret ){
    		sprintf( acErrMsg,"ȡ�����ƻ�����Ϣ�쳣erro code=[%s]",ret);
		WRITEMSG
    		return -1;
  	}
  				
	ret = pub_base_deadlineM( t_ln_pay_pln.beg_date, -1, &dqr);
	if( ret )	return -1;
	sprintf( acErrMsg,"dqr =[%ld]",dqr);
	WRITEMSG 	
	/***	20040830
  	if( g_pub_tx.tx_date >= dqr )
  	****/
  	if( g_pub_tx.tx_date > dqr )
  	{	
  		sprintf( acErrMsg, "���Ҵ����δ��,�����������ɻ���ƻ�");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L085");
		return 1;
	}	
	/* ���ǵ�һ�� */
	if( t_ln_pay_pln.curr_cnt==1 && p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
  		sprintf( acErrMsg, "���Ҵ����δ��,�����������ɻ���ƻ�");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L085");
		return 1;
	}		
	
	/* ���޸�����м�� */
	/* ��� */
	ret = pub_base_CompDblVal( p_ln_mst.bal, ln_bal );
	if ( ret )	/*���ܻ���*/
	{
		p_ln_mst.bal = ln_bal; 
		chg_flag++;
	}else{
		/*��������*/
		p_ln_mst.bal=0;
	}
	/* ��������(��ǰ) */
	if( end_date < p_ln_mst.mtr_date )
	{
		p_ln_mst.mtr_date = end_date ;
		chg_flag++;		
	}
	/* ��������(�ͺ�) */
	if( end_date > p_ln_mst.mtr_date )
	{
		p_ln_mst.mtr_date = end_date ;
		chg_flag++;	
		/******��ʱ������չ������,�����������,�������� 
		if ( p_ln_parm.exp_ind[0]=='N' )
		{
			sprintf(acErrMsg,"�˴������չ��,�������ڲ��ܳ����������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L086");
			return 1;
		}					
		memcpy( ln_mst_tmp , g_ln_mst , sizeof( ln_mst_c ) );
		g_ln_mst.mtr_date = end_date;
		* ����չ�ڼ�� *
   		*���ɷ�չ�ڱ�־*
		if( !strcmp( p_ln_mst.exp_ind[0],p_ln_parm.exp_ind[0] ) )
		{
			strcpy(acErrMsg,"�ô����Ʒ�ѳ���չ�ڴ���������չ��!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L008" );
			return 1;
		}
		* ������չ�ں��Ƿ񳬹������������� *		
		if ( p_ln_parm.term_type[0]=='Y' )    		* �������ͣ��� *
		{	
			p_ln_parm.max_term = p_ln_parm.max_term*12;
			pub_base_deadlineM( p_ln_mst.ic_date,p_ln_parm.max_term,&max_day);
		}else if ( p_ln_parm.term_type[0]=='M' ){ 	* �������ͣ��� *
			pub_base_deadlineM( p_ln_mst.ic_date,p_ln_parm.max_term,&max_day);
		}else if ( p_ln_parm.term_type[0]=='D' ){ 	* �������ͣ��� *		  
			pub_base_deadlineD( p_ln_mst.ic_date,p_ln_parm.max_term,&max_day);
		}
        	if ( p_ln_mst.mtr_date>max_day )
		{
                	strcpy( acErrMsg,"�������޴��ڹ涨���������!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L004" );
			return 1;
		}
		* �޸�չ�ڴ��� *	
		if ( p_ln_mst.exp_ind[0]=='Y' )
		{
			strcpy ( p_ln_mst.exp_ind , "1" );
		}else{
			cnt = atoi ( p_ln_mst.exp_ind );
			sprintf( p_ln_mst.exp_ind , "%d" , cnt+1 );
		}
		*************************************/	
	}
	/* ���� */
	ret = pub_base_CompDblVal( p_ln_mst.rate, rate );
	if ( ret!=0 )
	{
		p_ln_mst.rate = rate;
		/* ���ʼ�� */
		/* ��������Ƿ񳬹��涨�����ϸ����¸���Ľ��� */
		sprintf(acErrMsg,"[%s]",p_ln_parm.rate_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%s]",p_ln_parm.cur_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG		
		ret = pub_base_getllz( p_ln_parm.rate_no, p_ln_parm.cur_no, g_pub_tx.tx_date , &rate_val );
		if( ret )
		{
			sprintf(acErrMsg,"���ø������ʱ��ȡ�����ʺ�������!");
			WRITEMSG
			return -1;	
		}
		rate_val = rate_val/1.2;/* ת���������� */
 	    	if( pub_base_CompDblVal(p_ln_mst.rate,rate_val*(1+p_ln_parm.max_flt_ratio/100) )>0 )
	    	{
                	sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",rate_val,rate_val*(1+p_ln_parm.min_flt_ratio/100),p_ln_parm.min_flt_ratio );
                	WRITEMSG
               	 	strcpy ( g_pub_tx.reply , "A006" );
                	return -1;
            	}
            	if( pub_base_CompDblVal(p_ln_mst.rate,rate_val*(1+p_ln_parm.min_flt_ratio/100) )<0 )
            	{
                	sprintf(acErrMsg,"current rate is too high[%lf]!!",rate_val*(1-p_ln_parm.min_flt_ratio/100));
               	 	WRITEMSG
                	strcpy ( g_pub_tx.reply , "A005" );
                	return -1;
            	}							
		chg_flag++;
	}
	/* û���޸��κ���Ϣ */
	if ( chg_flag==0 )
	{
		sprintf(acErrMsg,"δ�޸��κ���Ϣ,����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L087");
		return 1;
	}
	/* ���´������ļ���¼ */	
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , " ac_id=%ld and ac_seqn=%d " , p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		return -1;
	}
	ret = Ln_mst_Fet_Upd( &ln_mst_upd , g_pub_tx.reply );
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
		WRITEMSG
		return -1;
	}else if (ret == 100){
		return -1;
	}
	p_ln_mst.ic_date = beg_date;
	
	p_ln_mst.ic_date = dqr+1;

	p_ln_mst.lst_date = g_pub_tx.tx_date;
	ret = Ln_mst_Upd_Upd( p_ln_mst , g_pub_tx.reply  );
        if (ret != 0)
        {
               sprintf(acErrMsg,"���´������ļ�����!!");
               WRITEMSG
               return -1;
        }          
	Ln_mst_Clo_Upd();
/*	ret = sql_execute( "UPDATE ln_mst SET bal=%lf , rate=%lf , mtr_date=%ld , lst_date=%ld  WHERE ac_id=%ld and ac_seqn=%d " , 
			p_ln_mst.bal,p_ln_mst.rate,p_ln_mst.mtr_date,g_pub_tx.tx_date, p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if( ret ) return ret;	
*/
	/* ɾ�����ڻ���ƻ� */
	ret = sql_execute( "DELETE from ln_pay_pln WHERE ac_id=%ld and ac_seqn=%d and pay_ind='0' " , 
			 		p_ln_mst.ac_id,p_ln_mst.ac_seqn );
	if( ret ) return ret;			 							
	/* �����µĻ���ƻ� */
	/*****
	old_opn_date = p_ln_mst.opn_date; * ����ԭ���Ŀ������� *
	p_ln_mst.ic_date = beg_date;	
	*****/
	/* ���������� */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 
	if( ret ) return -1;
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%g_ln_parm.pay_amt_term==0 )
		{
			ln_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			ln_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}	
	/* ���ɻ���ƻ� */
	ret = pub_ln_PayPlan( p_ln_mst , p_ln_parm , 1 , &ln_pay_pln , 1 );	
	if (ret)
	{
		sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
		WRITEMSG
		return -1;	
	}
	/* �޸Ļ���ƻ��黹���� */
	ln_pay_pln.beg_date= t_ln_pay_pln.beg_date;
	ln_pay_pln.end_date= t_ln_pay_pln.end_date;
	
	/* �޸����� */
	ln_pay_pln.ttl_cnt = t_ln_pay_pln.ttl_cnt - t_ln_pay_pln.curr_cnt + 1;

	sprintf(acErrMsg,"ttl_cnt=[%d]++curr_cnt=[%d]!",t_ln_pay_pln.ttl_cnt,t_ln_pay_pln.curr_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"now_ttl_cnt=[%d]!",ln_pay_pln.ttl_cnt);
	WRITEMSG
														
	/* ��ӻ���ƻ��� */	
	ret = Ln_pay_pln_Ins( ln_pay_pln , g_pub_tx.reply );
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǻ���ƻ���ʧ��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L077");
		return -1;
	}	
				
	return 0;	
}
/**********************************************************************
* �� �� ����  pub_ln_PayPlan_qry
* �������ܣ�  ��ѯ����ƻ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��01��20��
*
* ��    ����
*     ��  �룺
*	      ln_mst	�������ļ��ṹ
*             ln_parm   ���������ṹ
*	      cnt        smallint      ��������
*         type        ����ԭ�� 0���� 1��Ϊ�޸Ķ�����
*      
*     ��  ��:  
*             ln_pay_pln ���Ҵ����ƻ��ṹ 
*                    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
********************************************************************/
int pub_ln_PayPlan_qry( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , int cnt , 
				struct ln_pay_pln_c *p_ln_pay_pln , int type )
{
	long fir_day=0,term_opn_date=0;
	long month_cnt=0;
	double tmp1=0.00,tmp2=0.00,ln_amt=0.00;
	int ret=0;
	int iAc_seqn = 0; /***Add By zgf 2015/4/16 ***/
	struct ln_pay_pln_c	t_ln_pay_pln;
	TRACE
	memset(p_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	TRACE
	
	if( p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
		type=0;
	}else{
		type=1;
	}
	
	/* ���������� */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 
	if( ret ) return 1;
	sprintf(acErrMsg,"������month_cnt[%ld]",month_cnt);
	WRITEMSG	
	/* ��ɳϵͳ�������޸ĺ������*/
	if( type==1 )
	{
		/* ���ڻ����յ���ǰ */
		if( term_opn_date%100 < p_ln_parm.fst_pay_day )
		{
			month_cnt=month_cnt;
		}else{
			month_cnt=month_cnt+1;	
		}	
	}
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%p_ln_parm.pay_amt_term==0 )
		{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}
	sprintf(acErrMsg,"������p_ln_pay_pln->ttl_cnt[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG	
	/* ����ÿ�ڿ�ʼ�黹���ڡ��������� */
	if(cnt==1)
	{
		term_opn_date = p_ln_mst.ic_date;	
	}else{
		ret = pub_base_deadlineM(p_ln_mst.ic_date,(cnt-1)*p_ln_parm.pay_amt_term,&term_opn_date);
		if(ret) return 1;
	}	
	sprintf(acErrMsg,"term_opn_date[%ld]",term_opn_date);
	WRITEMSG			
	if( p_ln_parm.pay_term_type[0]=='M' )
	{	
		if( p_ln_parm.fst_pay_ind[0]=='M' )
		{
			/* ���ڻ����յ���ǰ */
			if( term_opn_date%100 <= p_ln_parm.fst_pay_day )
			{
				p_ln_pay_pln->beg_date=term_opn_date/100*100+p_ln_parm.fst_pay_day;
				p_ln_pay_pln->end_date=term_opn_date/100*100+p_ln_parm.mon_pay_day;	
			}else{
				/* ���ڻ����յ��պ� */
				if( term_opn_date%10000<=1200 )/*����12�·�*/
				{
			 		p_ln_pay_pln->beg_date=term_opn_date/100*100+100+p_ln_parm.fst_pay_day;
			   		p_ln_pay_pln->end_date=term_opn_date/100*100+100+p_ln_parm.mon_pay_day;
				}else{
					p_ln_pay_pln->beg_date=term_opn_date/10000*10000+10100+p_ln_parm.fst_pay_day;
					p_ln_pay_pln->end_date=term_opn_date/10000*10000+10100+p_ln_parm.mon_pay_day;
				}

			}
		}else if( p_ln_parm.fst_pay_ind[0]=='F' ){
			p_ln_pay_pln->beg_date = term_opn_date;
			p_ln_pay_pln->end_date = term_opn_date;			
		}
	}	
	/* �����޸� */
	if( type==0 )
	{
		if( p_ln_mst.opn_date%10000<=1221 )
		{
			/* ���ճ�ɳ����������Ҫ������˳��һ���� */
			pub_base_deadlineM( p_ln_pay_pln->beg_date , 1 , &p_ln_pay_pln->beg_date );
			pub_base_deadlineM( p_ln_pay_pln->end_date , 1 , &p_ln_pay_pln->end_date );	
		}
	}
	
	/* �����һ���޸�Ϊ�������� */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->beg_date = p_ln_mst.mtr_date;
		p_ln_pay_pln->end_date = p_ln_mst.mtr_date;			
	}
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG	
			
	/* ���㵱��Ӧ�� */
	p_ln_mst.rate = p_ln_mst.rate/1000;
	
	ret = pub_base_CompDblVal( p_ln_mst.bal, 0.00 );
	if ( ret==0 )
	{
		TRACE
		if(p_ln_mst.ic_date = g_pub_tx.tx_date )
		{
			TRACE
			ln_amt = p_ln_mst.amt_lmt;
		}
	}else{						
		/* ȡԭ����ƻ��� */
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d and pay_ind='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn); 
		if( ret==100 )
		{
			sprintf(acErrMsg,"����ƻ�������ؼ�¼[%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L135");
			return 1;
		}else if( ret ){
			sprintf(acErrMsg,"��ѯ����ƻ������쳣����![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
		sprintf(acErrMsg,"rob!!![%lf]",t_ln_pay_pln.ttl_amt);
		WRITEMSG
		ln_amt = t_ln_pay_pln.ttl_amt;	
		TRACE
	}
	sprintf(acErrMsg,"ln_amt=[%lf]!",ln_amt);
	WRITEMSG	
	/****************
	switch( atoi( p_ln_parm.ln_pay_type ) )
	{
	case 3:		*�ȶ��*
		tmp1 = pow((1+p_ln_mst.rate),p_ln_pay_pln->ttl_cnt);
		
		tmp2 = ln_amt * p_ln_mst.rate * tmp1 /(tmp1-1);
		tmp1 = pow((1+p_ln_mst.rate),(p_ln_pay_pln->ttl_cnt - cnt+1));
		p_ln_pay_pln->curr_amt= pub_base_PubDround_1(tmp2/tmp1);
		p_ln_pay_pln->curr_intst = tmp2 - p_ln_pay_pln->curr_amt; 
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1(p_ln_pay_pln->curr_intst);
		break;

	case 4:		*�ȱ�����*
		p_ln_pay_pln->curr_amt = pub_base_PubDround_1(ln_amt / p_ln_pay_pln->ttl_cnt);
		p_ln_pay_pln->curr_intst = pub_base_PubDround_1((1.0-(cnt-1.0)/p_ln_pay_pln->ttl_cnt)*ln_amt*p_ln_mst.rate);
		break;

	default:
		strcpy(g_pub_tx.reply,"O002");
		sprintf(acErrMsg,"����֧����ʽ�밴�Ҵ���֧����ʽ����,����ϵ���Ļ���! pay_typ= '%s' ",p_ln_parm.ln_pay_type );
		WRITEMSG
		return 1;

	}
	*******/
	
	/* ���㻹��ƻ� *
	if(  p_ln_parm.ln_pay_type[0]=='3' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "0" , 
			p_ln_mst.rate , cnt , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}
	if(  p_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "1" , 
			p_ln_mst.rate , cnt , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}			
	***/


	p_ln_pay_pln->ttl_cnt = p_ln_pay_pln->ttl_cnt - p_ln_pay_pln->curr_cnt;
	ln_amt= p_ln_mst.bal;
	
	if(  p_ln_parm.ln_pay_type[0]=='3' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "0" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}
	if(  p_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , p_ln_pay_pln->ttl_cnt , "1" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}		
	

	/* �����һ���޸���Ƿ����Ϊ������� */
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}
		
	/* �Ի���ƻ�������ֵ��ֵ */
	p_ln_pay_pln->ttl_amt=ln_amt; /* ԭʼ���� */
	p_ln_pay_pln->ac_id=p_ln_mst.ac_id; /* �˺�ID */
	p_ln_pay_pln->ac_seqn=p_ln_mst.ac_seqn; /* �˺���� */	
	p_ln_pay_pln->curr_cnt=cnt; /* ��ǰ�ڴ� */		
	p_ln_pay_pln->wrk_date=g_pub_tx.tx_date; /* �Ǽ����� */	
	strcpy ( p_ln_pay_pln->pay_ind , "0" );	/* ����״̬0-δ�� */
	p_ln_pay_pln->de_ac_id = p_ln_mst.repay_ac_id ;	/* �����˻�ID */
	/* ���ݱ������ɻ��ڻ����˺�ID */ 
	/**Mod by zgf 2015/4/16 32λ-64λǨ��,longָ�븳ֵ��int��ָ����� start*/	
	/*ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &p_ln_pay_pln->de_ac_seqn );*/
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &iAc_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
		WRITEMSG
		return 1;	
	}			
	p_ln_pay_pln->de_ac_seqn = iAc_seqn;/**Mod by zgf 2015/4/16***/	
	sprintf(acErrMsg,"orange!!!!!!!!!��[%d]��",p_ln_pay_pln->curr_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"��ǰ�ڴ�[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG				
	sprintf(acErrMsg,"ԭʼ����[%lf]",p_ln_pay_pln->ttl_amt);
	WRITEMSG
	sprintf(acErrMsg,"�˺�ID[%ld]",p_ln_pay_pln->ac_id);
	WRITEMSG
	sprintf(acErrMsg,"�˺����[%d]",p_ln_pay_pln->ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG
	sprintf(acErrMsg,"�Ǽ�����[%ld]",p_ln_pay_pln->wrk_date);
	WRITEMSG
	sprintf(acErrMsg,"����״̬0[%s]",p_ln_pay_pln->pay_ind);
	WRITEMSG
	
	
	sprintf(acErrMsg,"������[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",p_ln_pay_pln->curr_amt);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",p_ln_pay_pln->curr_intst);
	WRITEMSG
	sprintf(acErrMsg,"��ʼ�黹����[%ld]",p_ln_pay_pln->beg_date);
	WRITEMSG
	sprintf(acErrMsg,"���ڹ黹����[%ld]",p_ln_pay_pln->end_date);
	WRITEMSG
				
	return 0;	
}






/**********************************************************************
* �� �� ����  pub_ln_PayPlan_Qry
* �������ܣ�  ��ѯ����ƻ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2004��01��20��
*
* ��    ����
*     ��  �룺
*	      ln_mst	�������ļ��ṹ
*             ln_parm   ���������ṹ
*	      cnt       smallint      ��������
*             ttl_cnt   smallint      ʣ��������
*             ttl_bal   double        ʣ���ܽ��
*      
*     ��  ��:  
*             ln_pay_pln ���Ҵ����ƻ��ṹ 
*                    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
********************************************************************/
int pub_ln_PayPlan_Qry( struct ln_mst_c p_ln_mst, struct ln_parm_c p_ln_parm, int cnt, int ttl_cnt,
                             double ttl_bal, struct ln_pay_pln_c *p_ln_pay_pln )
{
	long fir_day=0,term_opn_date=0;
	long month_cnt=0;
	double tmp1=0.00,tmp2=0.00,ln_amt=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00,tol_amt=0.00;
	int ret=0;
	int type=0;
	int iAc_seqn = 0; /***Add By zgf 2015/4/16 ***/
	struct ln_pay_pln_c	t_ln_pay_pln;

	memset(p_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset( &t_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	
	if( p_ln_mst.opn_date==p_ln_mst.ic_date )
	{
		TRACE
		type=0;
	}else{
		TRACE
		type=1;
	}
	
	/* ���������� */
	month_cnt = pub_base_CalMouths( p_ln_mst.ic_date , p_ln_mst.mtr_date ); 

	/* if( ret ) return 1; */
	sprintf(acErrMsg,"������month_cnt[%ld]",month_cnt);
	WRITEMSG	

	/* ��ɳϵͳ�������޸ĺ������*/
	if( type==1 )
	{
		/* ���ڻ����յ���ǰ */
		if( p_ln_mst.opn_date%100 <= p_ln_parm.fst_pay_day )
		{
			TRACE
			month_cnt=month_cnt;
		}else{
			TRACE
			month_cnt=month_cnt+1;	
		}	
	}
		
	if( p_ln_parm.pay_term_type[0]=='M' )
	{
		if( month_cnt%p_ln_parm.pay_amt_term==0 )
		{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term ;
		}else{
			p_ln_pay_pln->ttl_cnt = month_cnt/p_ln_parm.pay_amt_term +1;
		}	
	}

	sprintf(acErrMsg,"������p_ln_pay_pln->ttl_cnt[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG	
	/* ����ÿ�ڿ�ʼ�黹���ڡ��������� */
	if(cnt==1)
	{
		term_opn_date = p_ln_mst.ic_date;	
	}else{
		ret = pub_base_deadlineM(p_ln_mst.ic_date,(cnt-1)*p_ln_parm.pay_amt_term,&term_opn_date);
		if(ret) return 1;
	}
	sprintf(acErrMsg,"term_opn_date[%ld]",term_opn_date);
	WRITEMSG			
	if( p_ln_parm.pay_term_type[0]=='M' )
	{	
		if( p_ln_parm.fst_pay_ind[0]=='M' )
		{
			/* ���ڻ����յ���ǰ */
			if( term_opn_date%100 <= p_ln_parm.fst_pay_day )
			{
				p_ln_pay_pln->beg_date=term_opn_date/100*100+p_ln_parm.fst_pay_day;
				p_ln_pay_pln->end_date=term_opn_date/100*100+p_ln_parm.mon_pay_day;	
			}else{
				/* ���ڻ����յ��պ� */
				if( term_opn_date%10000<=1200 )/*����12�·�*/
				{
			 		p_ln_pay_pln->beg_date=term_opn_date/100*100+100+p_ln_parm.fst_pay_day;
			   		p_ln_pay_pln->end_date=term_opn_date/100*100+100+p_ln_parm.mon_pay_day;
				}else{
					p_ln_pay_pln->beg_date=term_opn_date/10000*10000+10100+p_ln_parm.fst_pay_day;
					p_ln_pay_pln->end_date=term_opn_date/10000*10000+10100+p_ln_parm.mon_pay_day;
				}

			}
		}else if( p_ln_parm.fst_pay_ind[0]=='F' ){
			p_ln_pay_pln->beg_date = term_opn_date;
			p_ln_pay_pln->end_date = term_opn_date;			
		}
	}	
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG
	/* �����޸� */
	if( type==0 )
	{
		TRACE
		if( p_ln_mst.opn_date%10000<=1221 )	/*����12��21���Ժ󿪻�*/
		{
			/* ���ճ�ɳ����������Ҫ������˳��һ���� */
			pub_base_deadlineM( p_ln_pay_pln->beg_date , 1 , &p_ln_pay_pln->beg_date );
			pub_base_deadlineM( p_ln_pay_pln->end_date , 1 , &p_ln_pay_pln->end_date );	
		}
	}
	
	/* �����һ���޸�Ϊ�������� */
	if(cnt==month_cnt)
	{
		TRACE
		p_ln_pay_pln->beg_date = p_ln_mst.mtr_date;
		p_ln_pay_pln->end_date = p_ln_mst.mtr_date;			
	}
	sprintf(acErrMsg,"Jameslybeg_date[%ld]end_date[%ld]",p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date);
	WRITEMSG	
			
	/* ���㵱��Ӧ�� */
	p_ln_mst.rate = p_ln_mst.rate/1000;
	
	vtcp_log("[%s][%d] ����ID=[%ld],��ʼ����=[%ld],��ֹ����=[%ld],�����־=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
	ret = pub_base_CompDblVal( p_ln_mst.bal, 0.00 );
	if ( ret==0 )
	{
		TRACE
		if(p_ln_mst.ic_date = g_pub_tx.tx_date )
		{
			vtcp_log("[%s][%d] ����ID=[%ld],��ʼ����=[%ld],��ֹ����=[%ld],�����־=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
			TRACE
			ln_amt = p_ln_mst.amt_lmt;
			tol_amt= p_ln_mst.amt_lmt;
		}else{
			vtcp_log("[%s][%d] ����ID=[%ld],��ʼ����=[%ld],��ֹ����=[%ld],�����־=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
			sprintf(acErrMsg,"��ѯ����ƻ������쳣����![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
	}else{						
		/* ȡԭ����ƻ��� */
		vtcp_log("[%s][%d] ����ID=[%ld],��ʼ����=[%ld],��ֹ����=[%ld],�����־=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%ld and pay_ind='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn);
		if( ret==100 )
		{
			Ln_pay_pln_Debug(&t_ln_pay_pln);
			vtcp_log("[%s][%d] ����ID=[%ld],��ʼ����=[%ld],��ֹ����=[%ld],�����־=[%s]",__FILE__,__LINE__,g_ln_mst.ac_id,p_ln_pay_pln->beg_date,p_ln_pay_pln->end_date,t_ln_pay_pln.pay_ind);
			sprintf(acErrMsg,"����ƻ�������ؼ�¼[%d-%d] %d",
				p_ln_mst.ac_id,p_ln_mst.ac_seqn,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L135");
			return 1;
		}else if( ret ){
			sprintf(acErrMsg,"��ѯ����ƻ������쳣����![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L136");
			return 1;
		}			
		sprintf(acErrMsg,"rob!!![%lf]",t_ln_pay_pln.ttl_amt);
		WRITEMSG
		/* ȡǷ�������*
		ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", p_ln_mst.ac_id,p_ln_mst.ac_seqn );
		if(ret)	return 1;
		ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", p_ln_mst.ac_id,p_ln_mst.ac_seqn );
		if(ret)	return 1;
		*/		
		tol_amt= t_ln_pay_pln.ttl_amt;
		ln_amt = ttl_bal;	
	}
	sprintf(acErrMsg,"ln_amt=[%lf]!",ln_amt);
	WRITEMSG	
	
	if(  p_ln_parm.ln_pay_type[0]=='3' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , ttl_cnt , "0" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}
	if(  p_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_ln_cal_PayPlan ( ln_amt , ttl_cnt , "1" , 
			p_ln_mst.rate , 1 , &p_ln_pay_pln->curr_amt , &p_ln_pay_pln->curr_intst );
		if( ret )	return 1;
	}		
	

	/* �����һ���޸���Ƿ����Ϊ������� */
	/****
	if(cnt==month_cnt)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}
	****/
	if(cnt==ttl_cnt && ttl_cnt==1)
	{
		p_ln_pay_pln->curr_amt = p_ln_mst.bal;		
	}	
		
	/* �Ի���ƻ�������ֵ��ֵ */
	p_ln_pay_pln->ttl_amt=tol_amt; /* ԭʼ���� */
	p_ln_pay_pln->ac_id=p_ln_mst.ac_id; /* �˺�ID */
	p_ln_pay_pln->ac_seqn=p_ln_mst.ac_seqn; /* �˺���� */	
	p_ln_pay_pln->curr_cnt=cnt; /* ��ǰ�ڴ� */		
	p_ln_pay_pln->wrk_date=g_pub_tx.tx_date; /* �Ǽ����� */	
	strcpy ( p_ln_pay_pln->pay_ind , "0" );	/* ����״̬0-δ�� */
	p_ln_pay_pln->de_ac_id = p_ln_mst.repay_ac_id ;	/* �����˻�ID */
	/* ���ݱ������ɻ��ڻ����˺�ID */ 
	/**Mod by zgf 2015/4/16 32λ-64λǨ��,longָ�븳ֵ��int��ָ����� start*/
	/*ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &p_ln_pay_pln->de_ac_seqn );*/
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &iAc_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
		WRITEMSG
		return 1;	
	}			
	p_ln_pay_pln->de_ac_seqn = iAc_seqn;/**Mod by zgf 2015/4/16***/

	sprintf(acErrMsg,"orange!!!!!!!!!��[%d]��",p_ln_pay_pln->curr_cnt);
	WRITEMSG	
	sprintf(acErrMsg,"��ǰ�ڴ�[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG				
	sprintf(acErrMsg,"ԭʼ����[%lf]",p_ln_pay_pln->ttl_amt);
	WRITEMSG
	sprintf(acErrMsg,"�˺�ID[%ld]",p_ln_pay_pln->ac_id);
	WRITEMSG
	sprintf(acErrMsg,"�˺����[%d]",p_ln_pay_pln->ac_seqn);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�[%d]",p_ln_pay_pln->curr_cnt);
	WRITEMSG
	sprintf(acErrMsg,"�Ǽ�����[%ld]",p_ln_pay_pln->wrk_date);
	WRITEMSG
	sprintf(acErrMsg,"����״̬0[%s]",p_ln_pay_pln->pay_ind);
	WRITEMSG
	
	
	sprintf(acErrMsg,"������[%d]",p_ln_pay_pln->ttl_cnt);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�Ӧ������[%lf]",p_ln_pay_pln->curr_amt);
	WRITEMSG
	sprintf(acErrMsg,"��ǰ�ڴ�Ӧ����Ϣ[%lf]",p_ln_pay_pln->curr_intst);
	WRITEMSG
	sprintf(acErrMsg,"��ʼ�黹����[%ld]",p_ln_pay_pln->beg_date);
	WRITEMSG
	sprintf(acErrMsg,"���ڹ黹����[%ld]",p_ln_pay_pln->end_date);
	WRITEMSG
				
	return 0;	
}
