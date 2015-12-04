/*************************************************
* �� �� ��: pubf_ln_intst.c
* ����������   
*              pub_ln_cal_intst       ���Ϣ���
*              pub_ln_cal_AmtIntst    ����������Ϣ
*              pub_ln_cal_ComIntst    ������Ϣ��Ϣ���㣨���㸴��������
*              pub_ln_GetRate	      ȡ�ô�������
*	       	   pub_ln_GetIntstTyp     �������Ϣ�����ж�
*              pub_ln_CalIntst        ������Ϣ
*              pub_ln_Udp_LnReg       �黹ǷϢ��Ǽǲ�����
*	      	   pub_ln_CleanAcm	      �����������
*	           pub_ln_CalPunIntst     �����㷣Ϣ
*	           pub_ln_Ins_ln_reg      �ǼǴ���Ǽǲ�
*	           pub_ln_Ins_ln_lo	      �ǼǴ���Ƿ��Ǽǲ�
*              pub_ln_ln_reg	  	  �Ǽ�/�޸Ĵ���Ǽǲ�
*              pub_ln_Ins_sect_rate	  �ǼǷֶλ�����
*		Ln_eod_pri_InsReg		��Ӵ������մ�ӡ�Ǽǲ�
* ��    ��:    rob
* ������ڣ�   2004��12��28��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "com_sect_acm_c.h"
#include "com_parm_c.h"
#include "ln_reg_c.h"
#include "ln_pay_pln_c.h"
#include "ln_lo_c.h"
#include "ln_acm_reg_c.h"
#include "mo_pun_acc_c.h"
#include "ln_eod_pri_c.h"

/**********************************************************************
* �� �� ����  pub_ln_Ins_sect_rate
* �������ܣ�  �ǼǷֶλ�����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��19��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ
*                p_ln_parm          	��������ṹ    
*                rate		double	����ֵ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_Ins_sect_rate ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double rate )
{	
	char acm_type[2];
	
	/* ͳ�ƴ������ļ��л��� */
	strcpy( acm_type,"2");	/*�����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
	g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
						p_ln_parm.intst_days_type,acm_type,p_ln_mst.bal,0.00,
						&p_ln_mst.intst_acm,"0",0,0 );
	if( g_reply_int )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return 1;	
	}				
	g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '2',
		  p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.intst_acm,
		  rate, g_pub_tx.reply);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
		WRITEMSG
		return 1;	
	}
	/* �����㸴�� */  
	if ( p_ln_mst.cmpd_intst_ind[0]=='Y' )
	{
		TRACE
		
		/* ����ǷϢ���� */
		g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
				p_ln_parm.intst_days_type,acm_type,p_ln_mst.in_lo_intst,0.00,
					&p_ln_mst.in_lo_acm,"0",0,0 );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return 1;;	
		}
		TRACE	
		g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '3',
		 	 p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.in_lo_acm,
			  rate, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			return 1;	
		}
		/* ����ǷϢ���� */
		g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
				p_ln_parm.intst_days_type,acm_type,p_ln_mst.out_lo_intst,0.00,
					&p_ln_mst.out_lo_acm,"0",0,0 );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return 1;	
		}				
		g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '4',
		 	 p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.out_lo_acm,
			  rate, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			return 1;	
		}	
		/* �������� */
		g_reply_int = pub_base_CalAcm_Ln( p_ln_mst.lst_date,g_pub_tx.tx_date,
				p_ln_parm.intst_days_type,acm_type,p_ln_mst.cmpd_lo_intst,0.00,
					&p_ln_mst.cmpd_lo_acm,"0",0,0 );
		if( g_reply_int )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return 1;	
		}		
		g_reply_int = pub_base_InsertSectAcm(p_ln_mst.ac_id, p_ln_mst.ac_seqn, '5',
		 	 p_ln_mst.ic_date, g_pub_tx.tx_date, p_ln_mst.cmpd_lo_acm,
			  rate, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			return 1;	
		}		
	}
	
	return 0;	
	
}
/**********************************************************************
* �� �� ����  pub_ln_GetRate
* �������ܣ�  ȡ�ô�������
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��01��18��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ
*                p_ln_parm          	��������ṹ    
*      
*     ��  ��:    
*                rate		double	����ֵ
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_GetRate ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double *rate )
{
  	int ret=0;
  	struct 	mo_pun_acc_c	mo_pun_acc;
  	memset( &mo_pun_acc, 0x00, sizeof(struct mo_pun_acc_c));
	
	/* ����״̬��� */
	/* ����״̬Ϊ������������ʱ */
	if ( (p_ln_mst.ac_sts[0]=='9') || (p_ln_mst.ac_sts[0]=='*') ) 
	{
		sprintf(acErrMsg,"����״̬����,����״̬Ϊ������������״̬!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L032");
		return 1;	
	}
	
	/* ������ΥԼ��ȡ��Ϣ���� */
	ret = Mo_pun_acc_Sel( g_pub_tx.reply, &mo_pun_acc, \
		"ac_id=%ld and ac_seqn=%d and sts=0 ", p_ln_mst.ac_id, p_ln_mst.ac_seqn );
	if( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "��ѯ��Ϣ���ʵǼǲ�����![%d]", ret );
		WRITEMSG
		return 1;			
	}else if(!ret){
		/*��Ϣ�Ǽǲ����м�¼*/
		*rate= p_ln_mst.fine_rate;
					sprintf(acErrMsg,"1111111111111111111111111111111![%lf]\n",p_ln_mst.rate);
			WRITEMSG
		return 0;
	}	
			
	if ( atoi(p_ln_mst.ac_sts)==1 )	/* ����״̬����ʱ */
	{
		/* ��������˻�չ�ڿ��Ʊ�־��Ϊ0,���Ѿ�չ��,�ҵ�ǰ���ڴ��ڵ������� */
		if ( atoi(p_ln_mst.exp_ind)!=0 && g_pub_tx.tx_date > p_ln_mst.mtr_date )
		{
			*rate = p_ln_mst.exp_rate ;
					sprintf(acErrMsg,"222222222222222222222222222222![%lf]\n",p_ln_mst.exp_rate);
			WRITEMSG
			return 0;
		}
		
		if ( p_ln_parm.get_rate_type[0]=='1' ) /* ����ȡ�÷�ʽΪ�ֹ� */
		{
			*rate = p_ln_mst.rate ;
			sprintf(acErrMsg,"���ú����������ʱ�ű�������,ȡ����ֵ����![%lf]\n",p_ln_mst.rate);
			WRITEMSG
			return 0;
		}else{
			ret = pub_base_getllz(p_ln_parm.rate_no, p_ln_parm.cur_no, 
									g_pub_tx.tx_date, rate);
			if ( ret )
			{
				sprintf(acErrMsg,"���ú����������ʱ�ű�������,ȡ����ֵ����!");
				WRITEMSG
				return 1;	
			}
			*rate = *rate/1.2;/* ת���������� */
			sprintf(acErrMsg,"�������ʱ��ȡ����ֵ PASS!");
			WRITEMSG
		     }
	}else{
		/* ����״̬Ϊ{2-����}{3-����}{4-����} 5-90���� ��ȡ�������� */ 
		if ( p_ln_parm.get_rate_type[0]=='1' ) /* ����ȡ�÷�ʽΪ�ֹ� */
		{
			*rate = p_ln_mst.over_rate ;
	   sprintf(acErrMsg,"333333333333333333333333333333333![%lf]\n",p_ln_mst.rate);
			WRITEMSG

			return 0;
		}else{
			ret = pub_base_getllz(p_ln_parm.over_rate_no, p_ln_parm.cur_no, 
									g_pub_tx.tx_date, rate);
			if ( ret )
			{
				sprintf(acErrMsg,"�����������������ʱ�ű�������,ȡ���ʴ���!");
				WRITEMSG
				return 1;	
			}
			*rate = *rate/1.2;/* ת���������� */			
			sprintf(acErrMsg,"�����������ʱ��ȡ����ֵ PASS!");
			WRITEMSG
		     }
	}

  return 0 ;
}


/**********************************************************************
* �� �� ����  pub_ln_GetIntstTyp
* �������ܣ�  �������Ϣ�����ж�
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��01��18��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ
*                p_ln_parm          	��������ṹ    
*      
*     ��  ��:    
*
*     ����ֵ�� 	 ��Ϣ����  -1ʧ��  1����  2����
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_GetIntstTyp ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm )
{
  	int ret=0;
  	long beg_day=0;	
  	long days=0;
  	char parm_code[6];
  	int  parm_seqn=0;
  	long get_days=0;
  	long over_end_date=0;
  	long over_days=0;
	int type_lx=0;
  	
	struct com_parm_c	com_parm;
	
	memset( &com_parm,0x00,sizeof(struct com_parm_c) );	
	
	/* ����״̬��� */
	/* ����״̬Ϊ������������ʱ */
	if ( (p_ln_mst.ac_sts[0]=='9') || (p_ln_mst.ac_sts[0]=='*') )
	{
		sprintf(acErrMsg,"����״̬����,����״̬Ϊ������������״̬!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L032");
		return -1;	
	}
			
	/* ����״̬:���� ���� ����90����*/
	if ( (p_ln_mst.ac_sts[0]=='3') || (p_ln_mst.ac_sts[0]=='4') || (p_ln_mst.ac_sts[0]=='5') )
	{	
		return 2; /* ���� */
		sprintf(acErrMsg,"����״̬:���� ���� !");
		WRITEMSG
	}

	
	/* ����90������� */
	/* ȡ�������� */
	if ( p_ln_mst.exp_ind[0]=='0' )
	{
		over_end_date = p_ln_mst.mtr_date ;
	}else{
		over_end_date = p_ln_mst.exp_mtr_date;
	}
	/* ���ѵ��� */
	if( g_pub_tx.tx_date > over_end_date )
	{
	TRACE
		over_days = pub_base_CalTrueDays( over_end_date , g_pub_tx.tx_date );

		if( over_days >=90 )
		{
			sprintf(acErrMsg,"����!!!!!!!!!!!");
			WRITEMSG
			return 2;	/* ���� */  
		}else{
			sprintf(acErrMsg,"����!!!!!!!!!!!");
			WRITEMSG
			type_lx=1;	/* ���� */
		}
			
	}else{
		sprintf(acErrMsg,"����!!!!!!!!!!!");
		WRITEMSG
		type_lx=1;	/* ���� */
	}
	
	/* ��com_parm��ȡ��ǷϢ��������� */
	strcpy( parm_code , "L0001" );
	parm_seqn = 1 ;
	
	ret = pub_base_GetParm_long( parm_code , parm_seqn , &get_days );
	if( ret )
	{
			sprintf(acErrMsg,"���ú���ȡǷϢ�������������!");
			WRITEMSG
			return -1;	
	}	
	sprintf(acErrMsg,"���ú���ȡǷϢ��������� PASS![%ld]",get_days);
	WRITEMSG
		
	/* ǷϢ������� */
	ret=sql_min_long("ln_reg","reg_date_end",&beg_day," ac_id=%ld and ac_seqn=%d and type in('2','3') and sts='0' " , p_ln_mst.ac_id , p_ln_mst.ac_seqn );
	if( ret )
	{
		sprintf(acErrMsg,"���ú�����ѯ�������ڴ���!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"���ú�����ѯ�������� PASS[%ld]!",beg_day);
	WRITEMSG
	
	if ( beg_day )	/* ����ǷϢ���� */
	{
		/* ���ú����������� */
		days = pub_base_CalTrueDays( beg_day , g_pub_tx.tx_date );
		sprintf(acErrMsg,"���ú����������� PASS[%ld]!",days);
		WRITEMSG
	
		/* ���������Ƚ� */
		if ( days > get_days ) /* ����ǷϢ������������е��������бȽ� */
		{
			sprintf(acErrMsg,"����!!!!!!!!!!!");
			WRITEMSG
			return 2;	/* ���� */  
		}else{
			sprintf(acErrMsg,"����!!!!!!!!!!!");
			WRITEMSG
			type_lx=1;	/* ���� */
		}
		sprintf(acErrMsg,"����ǷϢ����!!! PASS[%ld]!",beg_day);
		WRITEMSG
	}
	return type_lx;
}

/**********************************************************************
* �� �� ����  pub_ln_CalIntst
* �������ܣ�  ������Ϣ
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��01��18��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ
*                p_ln_parm          	��������ṹ    
*		 intst_typ	char[2]		��Ϣ����( 0�������������Ϣ
*												  1����ǷϢ�������㸴��
*						  						  2����ǷϢ�������㸴��
*					  	  					  	  3����ǷϢ�������㸴�� )
*                mode                   ����ģʽ��0�޸����ݿ�1���޸����ݿ�
*     
*     ��  ��:    intst		double	������Ϣ����
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_CalIntst ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , 
			char *intst_typ ,double *intst, int mode )
{
  	int ret;
  	char type[2];	
  	char acm_type[2];	/* ������������ */	
  	double sect_intst;	/* �ֶ���Ϣ */
  	double mst_intst;	/* ��������Ϣ */
  	double tmp_amt;
  	double acm;
  	double rate;
  	int	jx_date=0;
	int	month_tmp=0;
	int	day_tmp=0; 
	int	jx_ind=0; 	
	
	struct ln_acm_reg_c	ln_acm_reg;
	memset( &ln_acm_reg, 0x00, sizeof(struct ln_acm_reg_c) );
		
	/* �����ļ��м�Ϣ����Ϊ0����Ϣ */
	if ( p_ln_mst.intst_type[0]=='0' )
	{
		*intst = 0;
		sprintf(acErrMsg,"�����ļ��м�Ϣ����Ϊ����Ϣ [%lf]",*intst );
		WRITEMSG
		return 0;	
	}
	TRACE
	
	/*****-------------------- add by rob 20030922 ------------------*********/
	/* ȡ��Ϣ�ղ��� */
	ret= pub_base_GetParm_int( "L0003" , 1 , &jx_date );
	if(ret)	return 1;
	
	month_tmp=g_pub_tx.tx_date%10000/100;
	day_tmp=g_pub_tx.tx_date%100;
		
	sprintf(acErrMsg,"���� [%ld]",day_tmp );
	WRITEMSG	
	sprintf(acErrMsg,"���� [%ld][%d]",jx_date,jx_ind );
	WRITEMSG
	if( day_tmp!=jx_date )
	{
		jx_ind++;	/* �Ƹ�����־ */
	}
	vtcp_log("[%s][%d]heheheehehehe",__FILE__,__LINE__);
	if( day_tmp==jx_date )
	{
		if( p_ln_mst.intst_type[0]=='3' ) /*�½�*/
		{
			jx_ind++;
		}else if( p_ln_mst.intst_type[0]=='4' ){/*����*/
			if( month_tmp==3|| month_tmp==6|| month_tmp==9|| month_tmp==12 )
			{
				jx_ind++;
			}
		}else if( p_ln_mst.intst_type[0]=='5' ){/*���*/
			if( month_tmp==12 )
			{
				jx_ind++;
			}
		}	
	}
	/* ��ʱ���� */
	jx_ind=0;
	vtcp_log("[%s][%d]heheheehehehe",__FILE__,__LINE__);
	
	/* ��Ϣ����Ϊ 0�������������Ϣ */	 	
	if ( intst_typ[0]=='0' )
	{
		/* ����ֶλ�����Ϣ */
		strcpy( type , "1" ); /* 1-����� */
		ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
			p_ln_mst.ac_seqn , type , &sect_intst , mode );
		if( ret )
		{
			sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
			WRITEMSG
			return -1;	
		}	
	TRACE
		sect_intst = pub_base_PubDround_1( sect_intst );
		sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",sect_intst);
		WRITEMSG		
		
		/* ������������л�������Ϣ */
		/* ������������л��� */
		strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
		acm = p_ln_mst.intst_acm ;/* ��Ϣ���� */
		
		ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
			p_ln_parm.intst_days_type , acm_type , 
			p_ln_mst.bal ,g_pub_tx.tx_amt1 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}
		sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
		WRITEMSG

		/* ������������л�����Ϣ */
		/* ȡ����ֵ(������) */
		ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
		if( ret )
		{
			sprintf(acErrMsg,"���ú�������������ʴ���!");
			WRITEMSG
			return -1;	
		}		
		sprintf(acErrMsg,"���ú��������������PASSΪ [%lf]",rate );
		WRITEMSG

		mst_intst = acm*rate/30/1000;
		sprintf(acErrMsg,"ǰ������������л�������ϢPASSΪ [%lf]",mst_intst );
		WRITEMSG
		tmp_amt = pub_base_PubDround_1(mst_intst);
		sprintf(acErrMsg,"tmp_amt������������л�������ϢPASSΪ [%lf]",tmp_amt );
		WRITEMSG
		mst_intst = pub_base_PubDround_1(mst_intst);		
		sprintf(acErrMsg,"������������л�������ϢPASSΪ [%lf]",mst_intst );
		WRITEMSG

		*intst = sect_intst + mst_intst ; 
		*intst = pub_base_PubDround_1( *intst );		
		sprintf(acErrMsg,"��Ϣ����Ϊ [%lf]",*intst );
		WRITEMSG
		
		/* ��Ϣ��Ϊ0ʱ���ж��Ƿ�ﵽ����޶� */
		ret = pub_base_CompDblVal( *intst, 0.00 );
		if ( ret>0 )
		{					
			ret = pub_base_CompDblVal( *intst, p_ln_parm.min_intst );
			if ( ret<0 )
			{			
				*intst = p_ln_parm.min_intst;
				*intst = pub_base_PubDround_1( *intst );
			}
		}
		sprintf(acErrMsg,"������Ϣ����Ϊ [%lf]",*intst );
		WRITEMSG
		
		/* ��Ϣ���Ϊ����ʱ��������*/
		ret = pub_base_CompDblVal( *intst, 0.00 );	
		if ( ret<0 )
		{	
			mode=1;
			*intst=0.00;
		}
		/* ��Ӵ����Ϣ�����ǼǱ� */
		if (!mode)
		{	
			if( pub_base_CompDblVal( acm , 0.00 ) )	
			{
				
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no, p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.intst_acm;
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );														
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}		
		/* ���������л�������,������Ϣ���� */
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"�޼�¼[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.intst_acm = 0;	
	
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"�������ļ��л������� PASS");
			WRITEMSG
		}
					

		return 0;
	}


	/* ��Ϣ����Ϊ 1����ǷϢ�������㸴�� */	 	
	if ( intst_typ[0]=='1' )
	{	
vtcp_log("cal bnlx" );
		sprintf(acErrMsg,"�Ƿ���㸴��cmpd_intst_ind=[%s]",p_ln_mst.cmpd_intst_ind);
		WRITEMSG
		/* ����Ƿ���㸴�� */
		if ( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			TRACE
			*intst = 0;
			sprintf(acErrMsg,"���ִ�����㸴�� [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		TRACE		
		/* ��鸴�����㷽�� */
		if ( p_ln_parm.cmpd_intst_type[0]!='0' )
		{
			*intst = 0;
			sprintf(acErrMsg,"���ִ���ǰ��ջ������� [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		/* �������㷽���ǰ��ջ������� */
		else if ( p_ln_parm.cmpd_intst_type[0]=='0' )
		{				
			/* ����ֶλ������б���ǷϢ��Ϣ */
			strcpy( type , "2" ); /* 2-�������ǷϢ */
			ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
				p_ln_mst.ac_seqn , type , &sect_intst , mode );
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",sect_intst);
			WRITEMSG		
				
			/* ������������б���ǷϢ��������Ϣ */
			/* ������������б���ǷϢ���� */
			strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
			acm = p_ln_mst.in_lo_acm ;/* ����ǷϢ���� */
			
			ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
				p_ln_parm.intst_days_type , acm_type , 
				p_ln_mst.in_lo_intst ,g_pub_tx.tx_amt2, &acm,"0",0,0 );
			if( ret )
			{
				sprintf(acErrMsg,"���ú��������������!");
				WRITEMSG
				return -1;	
			}
			sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
			WRITEMSG
		
			/* ������������б���ǷϢ���� */
			ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������������ʴ���!");
				WRITEMSG
				return -1;	
			}		
			sprintf(acErrMsg,"���ú��������������PASSΪ [%lf]",rate );
			WRITEMSG
		
			mst_intst = acm*rate/30/1000;
			sprintf(acErrMsg,"�������ǷϢ��������ϢPASSΪ [%lf]",mst_intst );
			WRITEMSG
			mst_intst = pub_base_PubDround_1(mst_intst);		
			sprintf(acErrMsg,"�������ǷϢ��������ϢPASSΪ [%lf]",mst_intst );
			WRITEMSG
			
			*intst = sect_intst + mst_intst ; 
			*intst = pub_base_PubDround_1( *intst );
			sprintf(acErrMsg,"������Ϣ����Ϊ [%lf]",*intst );
			WRITEMSG	
				
			/* ��Ϣ���Ϊ����ʱ��������*/
			ret = pub_base_CompDblVal( *intst, 0.00 );	
			if ( ret<0 )
			{	
				mode=1;
				*intst=0.00;
			}
			
		/* ��Ӵ����Ϣ�����ǼǱ� */
		if (!mode)
		{		
	 		if( pub_base_CompDblVal( acm , 0.00 ) )
			{
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no , p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.in_lo_acm;
				if( jx_ind )
				{
					mst_intst=0.00;
				}
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );															
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}
		if( jx_ind )
		{
			*intst = 0;
			sprintf(acErrMsg,"δ����Ϣ�ղ����㸴�� [%lf]",*intst );
			WRITEMSG
			sprintf(acErrMsg,"ԭ���ڻ���Ϊ [%lf]",p_ln_mst.in_lo_acm );
			WRITEMSG			
			return 0;	
		}
		TRACE
		/**** del by ly   �ڼ������������� *********/	
		/* ���������л������� *
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"�޼�¼[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.in_lo_acm = 0;			
				
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"�������ļ��л������� PASS");
			WRITEMSG
		}
		**********/
				

		}
		
		return 0;
	}	


	/* ��Ϣ����Ϊ 2����ǷϢ�������㸴�� */	 	
	if ( intst_typ[0]=='2' )
	{	
		sprintf(acErrMsg,"�Ƿ���㸴��cmpd_intst_ind=[%s]",p_ln_mst.cmpd_intst_ind);
		WRITEMSG
		/* ����Ƿ���㸴�� */
		if ( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			*intst = 0;
			sprintf(acErrMsg,"���ִ�����㸴�� [%lf]",*intst );
			WRITEMSG
			return 0;	
		}				
		/* ��鸴�����㷽�� */
		if ( p_ln_parm.cmpd_intst_type[0]!='0' )
		{
			*intst = 0;
			sprintf(acErrMsg,"���ִ���ǰ��ջ������� [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		/* �������㷽���ǰ��ջ������� */
		else if ( p_ln_parm.cmpd_intst_type[0]=='0' )
		{
			/* ����ֶλ������б���ǷϢ��Ϣ */
			strcpy( type , "3" ); /* 3-�������ǷϢ */			
			ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
				p_ln_mst.ac_seqn , type , &sect_intst , mode );
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",sect_intst);
			WRITEMSG		
				
			/* ������������б���ǷϢ��������Ϣ */
			/* ������������б���ǷϢ���� */
			strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
			acm = p_ln_mst.out_lo_acm ;/* ����ǷϢ���� */
			
			ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
				p_ln_parm.intst_days_type , acm_type , 
				p_ln_mst.out_lo_intst ,g_pub_tx.tx_amt3, &acm,"0",0,0 );
			if( ret )
			{
				sprintf(acErrMsg,"���ú��������������!");
				WRITEMSG
				return -1;	
			}
			sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
			WRITEMSG
		
			/* ������������б���ǷϢ���� */
			ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������������ʴ���!");
				WRITEMSG
				return -1;	
			}		
			sprintf(acErrMsg,"���ú��������������PASSΪ [%lf]",rate );
			WRITEMSG
		
			mst_intst = acm*rate/30/1000;
			sprintf(acErrMsg,"������������л�������ϢPASSΪ [%lf]",mst_intst );
			WRITEMSG

			mst_intst = pub_base_PubDround_1(mst_intst);		
			sprintf(acErrMsg,"�������ǷϢ��������ϢPASSΪ [%lf]",mst_intst );
			WRITEMSG

			*intst = sect_intst + mst_intst ;
			*intst = pub_base_PubDround_1( *intst );			 
			sprintf(acErrMsg,"������Ϣ����Ϊ [%lf]",*intst );
			WRITEMSG
			
			/* ��Ϣ���Ϊ����ʱ��������*/
			ret = pub_base_CompDblVal( *intst, 0.00 );	
			if ( ret<0 )
			{	
				mode=1;
				*intst=0.00;
			}
			
		/* ��Ӵ����Ϣ�����ǼǱ� */
		if (!mode)
		{		
			if( pub_base_CompDblVal( acm , 0.00 ) )
			{
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no , p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.out_lo_acm;
				if( jx_ind )
				{
					mst_intst=0.00;
				}
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );														
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}
		if( jx_ind )
		{
			*intst = 0;
			sprintf(acErrMsg,"δ����Ϣ�ղ����㸴�� [%lf]",*intst );
			WRITEMSG
			sprintf(acErrMsg,"ԭ�������Ϊ [%lf]",p_ln_mst.out_lo_acm );
			WRITEMSG
			return 0;	
		}
		/**** del by ly   �ڼ������������� *********/			
		/* ���������л������� *
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"�޼�¼[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.out_lo_acm = 0;		
	
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"�������ļ��л������� PASS");
			WRITEMSG
		}
		**************/


		}
		
		return 0;
		
	}	

	/* ��Ϣ����Ϊ 3����ǷϢ�������㸴�� */	 	
	if ( intst_typ[0]=='3' )
	{	
		sprintf(acErrMsg,"�Ƿ���㸴��cmpd_intst_ind=[%s]",p_ln_mst.cmpd_intst_ind);
		WRITEMSG
		/* ����Ƿ���㸴�� */
		if ( p_ln_mst.cmpd_intst_ind[0]=='N' )
		{
			*intst = 0;
			sprintf(acErrMsg,"���ִ�����㸴�� [%lf]",*intst );
			WRITEMSG
			return 0;	
		}		
		/* ��鸴�����㷽�� */
		if ( p_ln_parm.cmpd_intst_type[0]!='0' )
		{
			*intst = 0;
			sprintf(acErrMsg,"���ִ���ǰ��ջ������� [%lf]",*intst );
			WRITEMSG
			return 0;	
		}
		/* �������㷽���ǰ��ջ������� */
		else if ( p_ln_parm.cmpd_intst_type[0]=='0' )
		{
			/* ����ֶλ������и���ǷϢ��Ϣ */
			strcpy( type , "4" ); /* 4-����� */			
			ret = pub_base_Get_SectIntst( p_ln_mst.ac_id , 
				p_ln_mst.ac_seqn , type , &sect_intst , mode );
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������ֶλ�����Ϣ����!");
				WRITEMSG
				return -1;	
			}	
			sprintf(acErrMsg,"���ú�������ֶλ�����ϢPASS[%lf]!",sect_intst);
			WRITEMSG		
				
			/* ������������и���ǷϢ��������Ϣ */
			/* ������������и���ǷϢ���� */
			strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
			acm = p_ln_mst.cmpd_lo_acm ;/* ����ǷϢ���� */
			
			ret = pub_base_CalAcm_Ln( p_ln_mst.lst_date , g_pub_tx.tx_date , 
				p_ln_parm.intst_days_type , acm_type , 
				p_ln_mst.cmpd_lo_intst ,g_pub_tx.tx_amt4, &acm,"0",0,0 );
			if( ret )
			{
				sprintf(acErrMsg,"���ú��������������!");
				WRITEMSG
				return -1;	
			}
			sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
			WRITEMSG
		
			/* ������������и���ǷϢ���� */
			ret = pub_ln_GetRate ( p_ln_mst , p_ln_parm , &rate );	
			if( ret )
			{
				sprintf(acErrMsg,"���ú�������������ʴ���!");
				WRITEMSG
				return -1;	
			}		
			sprintf(acErrMsg,"���ú��������������PASSΪ [%lf]",rate );
			WRITEMSG
		
			mst_intst = acm*rate/30/1000;
			sprintf(acErrMsg,"������������л�������ϢPASSΪ [%lf]",mst_intst );
			WRITEMSG
			mst_intst = pub_base_PubDround_1(mst_intst);		
			sprintf(acErrMsg,"���㸴��ǷϢ��������ϢPASSΪ [%lf]",mst_intst );
			WRITEMSG
			
			*intst = sect_intst + mst_intst ;
			*intst = pub_base_PubDround_1( *intst );			 
			sprintf(acErrMsg,"������Ϣ����Ϊ [%lf]",*intst );
			WRITEMSG			
			
			/* ��Ϣ���Ϊ����ʱ��������*/
			ret = pub_base_CompDblVal( *intst, 0.00 );	
			if ( ret<0 )
			{	
				mode=1;
				*intst=0.00;
			}

		/* ��Ӵ����Ϣ�����ǼǱ� */
		if (!mode)
		{		
			if( pub_base_CompDblVal( acm , 0.00 ) )
			{
				ln_acm_reg.ac_id = p_ln_mst.ac_id;
				ln_acm_reg.ac_seqn = p_ln_mst.ac_seqn;
				strcpy( ln_acm_reg.br_no , p_ln_mst.opn_br_no );
				ln_acm_reg.beg_date = p_ln_mst.ic_date;
				ln_acm_reg.end_date = g_pub_tx.tx_date;
				ln_acm_reg.cal_date = g_pub_tx.tx_date;
				ln_acm_reg.rate = rate;
				ln_acm_reg.intst_acm = acm;
				ln_acm_reg.part_intst_acm = p_ln_mst.cmpd_lo_acm;
				if( jx_ind )
				{
					mst_intst=0.00;
				}
				ln_acm_reg.intst = mst_intst;	
				sprintf( ln_acm_reg.intst_type , "%d" , atoi(intst_typ)+1 );															
				strcpy( ln_acm_reg.type , "1" );
				ln_acm_reg.trace_no = g_pub_tx.trace_no;
				TRACE
				sprintf( acErrMsg,"!!rob!ln_acm_reg.intst=[%lf]",ln_acm_reg.intst);
				WRITEMSG	
				TRACE									
				ret = Ln_acm_reg_Ins( ln_acm_reg , g_pub_tx.reply );
				if( ret )	return 1;
				TRACE	
			}
		}
		if( jx_ind )
		{
			*intst = 0;
			sprintf(acErrMsg,"δ����Ϣ�ղ����㸴�� [%lf]",*intst );
			WRITEMSG
			sprintf(acErrMsg,"ԭ��������Ϊ [%lf]",p_ln_mst.cmpd_lo_acm );
			WRITEMSG			
			return 0;	
		}
		/**** del by ly   �ڼ������������� *********/		
		/* ���������л������� *
		if (!mode)
		{
			ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", p_ln_mst.ac_id , p_ln_mst.ac_seqn );
			if( ret )
			{
				sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
				WRITEMSG
				return 1;
			}	
			ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
			if( ret==100 )
			{
				sprintf( acErrMsg,"�޼�¼[%d]",ret);
				WRITEMSG
				return 1;
			}
   			if( ret )
 			{
			 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
			 	WRITEMSG
			 	strcpy( g_pub_tx.reply ,"D103" );
			 	return 1;
 			}
			
			p_ln_mst.cmpd_lo_acm = 0;		
	
			ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
			if ( ret )
			{
				sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
				WRITEMSG
				return 1;
			}		
		
			Ln_mst_Clo_Upd( );
			sprintf(acErrMsg,"�������ļ��л������� PASS");
			WRITEMSG
		}
		***************/

		}
		
		return 0;
	}
	/***д�´��ļ�***/	
  return -1 ;
}
/**********************************************************************
* �� �� ����  pub_ln_Udp_LnReg
* �������ܣ�  �黹ǷϢ��Ǽǲ�����
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��01��18��
*
* ��    ����
*     ��  �룺   ac_id    integer        �˻�ID
*                ac_seqn  smallint       �ʻ����    
*                type     char[2]        �黹���ͣ�1����2����ǷϢ��3����ǷϢ��4����ǷϢ
*	         amt	  double	 �黹���
*      
*     ��  ��:    amt	  double	 �黹���
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_Udp_LnReg ( long ac_id , int ac_seqn , char *type , double pamt )
{
  	int ret;
  	double amt_tmp,amt;
	char sts[2];
	long tx_date=0;
	int flag=0;
	int while_sts=1;

  	struct ln_reg_c	ln_reg;	
	memset( &ln_reg,0x00,sizeof(struct ln_reg_c) );	
	amt=pamt;
	
	sprintf(acErrMsg,"���![%lf]",amt);
	WRITEMSG
	
	
	ret = Ln_reg_Dec_Upd ( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and type=%s and sts='0' order by reg_date_beg ", ac_id,ac_seqn,type );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_reg_Dec_Upd��!ret=[%d]", ret);
		WRITEMSG	
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		return(1);	
	}			

	flag=0;

		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
	while( while_sts )
	{
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		ret = Ln_reg_Fet_Upd( &ln_reg, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_reg_Fet_Upd error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}
		else if( ret==100 )
		break;
		
		amt_tmp = ln_reg.pay_bal + amt;

		flag++;

		/* ����� */
		/* ��Ƿ����С���ѹ黹���+������) */
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	

		if ( pub_base_CompDblVal( ln_reg.lo_bal , amt_tmp ) == -1 )
		{
			amt-=(ln_reg.lo_bal-ln_reg.pay_bal);

			ln_reg.pay_bal = ln_reg.lo_bal ;/* �ѹ黹��� */
			strcpy( sts,"1" );
			tx_date=g_pub_tx.tx_date;

			sprintf( acErrMsg,"Ƿ����С��(�ѹ黹���+������)[%lf]",amt);
	 		WRITEMSG
	 		ln_reg.pay_date = g_pub_tx.tx_date;
	 		strcpy( ln_reg.sts , "1" );

			ret=Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
			if(ret)	return ret;	
			while_sts=1;
			if( flag==1 )
			{
				g_pub_tx.acm_beg_date = ln_reg.reg_date_beg ;
			}	
TRACE
		}
		else if ( pub_base_CompDblVal( ln_reg.lo_bal , amt_tmp ) == 1 )
		{
			amt = 0;
			ln_reg.pay_bal = amt_tmp ;	/* �ѹ黹��� */
			strcpy( sts,"0" );

			sprintf( acErrMsg,"Ƿ�������(�ѹ黹���+������)[%lf]",ln_reg.pay_bal);
	 		WRITEMSG
	 		
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
	 		/* ���� �ѹ黹���,Ƿ��״̬,�������� */
	 		ln_reg.pay_date = 0;
	 		strcpy( ln_reg.sts , "0" );

			Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
			if(ret)	return ret;
			while_sts=0;
			if( flag==1 )
			{
				g_pub_tx.acm_beg_date = ln_reg.reg_date_beg ;
			}
		}else if ( pub_base_CompDblVal( ln_reg.lo_bal , amt_tmp ) == 0 )/* ��Ƿ�������(�ѹ黹���+������) */
		{
			amt = 0;
			ln_reg.pay_bal = amt_tmp ;	/* �ѹ黹��� */
			strcpy( sts,"1" );
			tx_date=g_pub_tx.tx_date;
			
			sprintf( acErrMsg,"Ƿ�������(�ѹ黹���+������)[%lf]",amt);
	 		WRITEMSG
	 		
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
	 		/* ���� �ѹ黹���,Ƿ��״̬,�������� */
	 		ln_reg.pay_date = g_pub_tx.tx_date;
	 		strcpy( ln_reg.sts , "1" );

			Ln_reg_Upd_Upd( ln_reg , g_pub_tx.reply );
			if(ret)	return ret;
			while_sts=0;
			if( flag==1 )
			{
				g_pub_tx.acm_beg_date = ln_reg.reg_date_beg ;
			}
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		}
TRACE
	}
		vtcp_log("[%s][%d] TEARS amt = %.2f\n",__FILE__,__LINE__,amt);	
		Ln_reg_Clo_Upd ();
	if( pub_base_CompDblVal( amt , 0.00 ) )	/* һ���Ǵ���Ǽǲ����������� */
	{
		sprintf( acErrMsg, "���������,�н���![%lf]",amt );
		WRITEMSG
		strcpy( g_pub_tx.reply , "L156" );
		return(-1);		
	}	
	if( flag==0 )
	{
		/*û��ƥ��ļ�¼*/
	}
	sprintf(acErrMsg,"�黹ǷϢ��Ǽǲ����� PASS");
	WRITEMSG
	
	return 0;
}

/*****************************************************
* �� �� ����  pub_ln_CleanAcm
* �������ܣ�  �����������
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��01��18��
*
* ��    ����
*     ��  �룺   ac_id    integer        �˻�ID
*                ac_seqn  smallint       �ʻ����    
*                type     char[2]        ���ͣ�0���� 1��Ϣ
*      
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int pub_ln_CleanAcm ( long ac_id , int ac_seqn , char *type )
{
  	int ret;

  	struct com_sect_acm_c	com_sect_acm;
	
	memset( &com_sect_acm,0x00,sizeof(struct com_sect_acm_c) );	
		
	/* �޸ķֶλ�����,��״̬��Ϊ2���� */
	if ( type[0]=='0' ) /* ����Ϊ���� */	
	{
		ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply, 
			"ac_id=%ld and ac_seqn=%d and intst_type='1' " ,ac_id ,ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Com_sect_acm_Dec_Upd��!ret=[%d]",ret );
			WRITEMSG
			return(-1);
		}
	}
	else if ( type[0]=='1' ) /* ����Ϊ��Ϣ */
	{
		ret = Com_sect_acm_Dec_Upd(g_pub_tx.reply, 
			"ac_id=%ld and ac_seqn=%d and intst_type in('2','3','4') " ,
			ac_id ,ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Com_sect_acm_Dec_Upd��!ret=[%d]",ret );
			WRITEMSG
			return(-1);
		}
	}	
	else 
	{
		sprintf( acErrMsg, "��������ʹ���!����0 ,1");
		WRITEMSG
		return(-1);	
	}

	while(1)
	{
		ret = Com_sect_acm_Fet_Upd( &com_sect_acm, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret )
 		{
	 		sprintf( acErrMsg,"fetch com_sect_acm error code = [%d]",ret);
	 		WRITEMSG
	 		return 1;
 		}
			
		strcpy( com_sect_acm.sts , "2" );	/* �޸�״̬:���� */
			
		ret = Com_sect_acm_Upd_Upd( com_sect_acm, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf( acErrMsg,"Com_sect_acm_Upd_Upd ����[%d]",ret);
			WRITEMSG
			return 1;
		}		
	}
	Com_sect_acm_Clo_Upd( );
	sprintf(acErrMsg,"���·ֶλ�������״̬ PASS");
	WRITEMSG

	/* �޸����ļ��м�¼ */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply, 
		"ac_id=%ld and ac_seqn=%d ",ac_id , ac_seqn );	
	if( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Dec_Upd �α����[%d]",ret);
		WRITEMSG
		return 1;
	}	
	
	ret = Ln_mst_Fet_Upd( &g_ln_mst, g_pub_tx.reply );
	if( ret && ret!=100 )
 	{
		sprintf( acErrMsg,"fetch ln_mst error code = [%d]",ret);
	 	WRITEMSG
	 	return 1;
 	}
   	if( ret && ret!=100 )
 	{
		sprintf( acErrMsg,"fetch ln_mst error code = [%d]",ret);
	 	WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
	 	return 1;
 	}
	
	/* ����Ϊ���� */	
	if ( type[0]=='0' )
	{
		g_ln_mst.intst_acm = 0;			/* ��Ϣ���� */
		g_ln_mst.lst_date = g_pub_tx.tx_date;	/* �ϱʷ������� */		
	}
	/* ����Ϊ��Ϣ */
	if ( type[0]=='1' )
	{
		g_ln_mst.in_lo_acm = 0;			/* ����ǷϢ���� */
		g_ln_mst.out_lo_acm= 0;			/* ����ǷϢ���� */
		g_ln_mst.cmpd_lo_acm = 0;		/* �������� */				
		g_ln_mst.lst_date = g_pub_tx.tx_date;	/* �ϱʷ������� */
	}
					
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Com_sect_acm_Upd_Upd ����[%d]",ret);
		WRITEMSG
		return 1;
	}		
	
	Ln_mst_Clo_Upd( );
	sprintf(acErrMsg,"���´������ļ� PASS");
	WRITEMSG		
		
	return 0 ;
}
/**********************************************************************
* �� �� ����  pub_ln_ln_reg
* �������ܣ�  �ǼǴ���Ǽǲ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��03��31��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ   
*                beg_date		��ʼ����  
*		 tx_date		��������(��ֹ����)
*                type     char[2]       ���ͣ�1����2����ǷϢ��3����ǷϢ��4����ǷϢ
*	         amt	  double	���
*		 acm	  double        ����
*                ind      char[2]       ��Ϣ��־0���ǽ�Ϣ�գ�1����Ϣ������
*                mode     int	        �Ǽ�ģʽ 0�Ǽ�Ƿ�� 1�����ѵǼ�Ƿ�� 2�Ǽǲ�����
*          
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_ln_reg( struct ln_mst_c ln_mst,long beg_date , long tx_date,char *type,
	double amt, double acm , char *ind,int mode )
{
	struct ln_reg_c ln_reg;
	int ret=0;
	
	sprintf(acErrMsg,"���![%lf]",amt);
	WRITEMSG
	
	memset( &ln_reg,0,sizeof(ln_reg) );

	
	if( amt<0.005 ) return 0;
	
	
	ln_reg.ac_id = ln_mst.ac_id;		/* �˻�ID */
	ln_reg.ac_seqn = ln_mst.ac_seqn;	/* �˻���� */	
	if( mode==0 )
	{
		ln_reg.reg_date_beg = beg_date;	/* ��ʼ���� */
		ln_reg.reg_date_end = tx_date;	/* ��ֹ���� */
		ln_reg.pay_date = 0;	/* �������� */
		strcpy ( ln_reg.type,type );	/* ���� */
		ln_reg.lo_bal = amt;		/* Ƿ���� */
		ln_reg.acm = acm;		/* ���� */
		ln_reg.pay_bal = 0.00;		/* �ѹ黹��� */
		ln_reg.sts[0]='0';			/* Ƿ��״̬��0δ����1�ѻ��� */
		strcpy( ln_reg.ind , ind );	/* �Ƿ��Ϣ�� */
		ret = Ln_reg_Ins( ln_reg , g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǴ���Ǽǲ�ʧ��![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L041");
			return ret;
		}
		g_pub_tx.acm_beg_date = ln_reg.reg_date_beg;
		sprintf(acErrMsg,"mode 0!");
		WRITEMSG
	}
	else if( mode==2 )
	{
		ln_reg.reg_date_beg = beg_date;	/* ��ʼ���� */
		ln_reg.reg_date_end = tx_date;	/* ��ֹ���� */
		ln_reg.pay_date = tx_date;	/* �������� */
		strcpy ( ln_reg.type,type );	/* ���� */
		ln_reg.lo_bal = amt;		/* Ƿ���� */
		ln_reg.acm = acm;		/* ���� */		
		ln_reg.pay_bal = amt;		/* �ѹ黹��� */
		ln_reg.sts[0]='1';			/* Ƿ��״̬��0δ����1�ѻ��� */
		strcpy( ln_reg.ind , ind );	/* �Ƿ��Ϣ�� */
		ret = Ln_reg_Ins( ln_reg , g_pub_tx.reply );
		if( ret )
		{
			sprintf(acErrMsg,"�ǼǴ���Ǽǲ�ʧ��![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L041");
			return ret;
		}
		g_pub_tx.acm_beg_date = ln_reg.reg_date_beg;
		sprintf(acErrMsg,"mode 2!");
		WRITEMSG
	}
	else
	{
		sprintf(acErrMsg,"mode 3����!");
		WRITEMSG
		sprintf(acErrMsg,"���![%lf]",amt);
		WRITEMSG
		ret=pub_ln_Udp_LnReg(ln_mst.ac_id,ln_mst.ac_seqn,type,amt);
		if( ret ) return ret;
		sprintf(acErrMsg,"mode 3!PASS");
		WRITEMSG
	}

	return 0;
}
/**********************************************************************
* �� �� ����  pub_ln_CalPunIntst
* �������ܣ�  �����㷣Ϣ
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��03��30��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ
*                p_ln_parm          	��������ṹ
*                amt                    ���
*                beg_date		��ʼ����
*		 end_date		��ֹ����    
*     
*     ��  ��:    pun_intst	double	��Ϣ����(�������Ϣ����)
*		 cmpd_intst	double	��������(��Ϣ����ͬ����)
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_CalPunIntst ( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , double amt , double intst , long beg_date , long end_date , double *pun_intst , double *cmpd_intst )
{
  	int ret;		
  	double fine_rate;
  	double cmpd_rate;
  	long days;
	sprintf(acErrMsg,"beg_date=[%d] end_date=[%d]",beg_date,end_date);
	WRITEMSG
	TRACE
	/* ȡ��Ϣ���� */
	sprintf(acErrMsg,"fine_rate_no[%lf]cur_no[%s]date[%ld]",p_ln_mst.fine_rate, p_ln_parm.cur_no, g_pub_tx.tx_date);
	WRITEMSG
	/*****	
	ret = pub_base_getllz(p_ln_parm.fine_rate_no, p_ln_parm.cur_no, g_pub_tx.tx_date, &rate);
	if ( ret )
	{
		sprintf(acErrMsg,"���������ݷ�Ϣ���ʱ�ű�������,ȡ���ʴ���!");
		WRITEMSG
		return 1;	
	}
	************/
	TRACE
	fine_rate = p_ln_mst.fine_rate;
	cmpd_rate = p_ln_mst.over_rate;
	sprintf(acErrMsg,"��Ϣ����[%lf]!",fine_rate);
	WRITEMSG	
	sprintf(acErrMsg,"��Ϣ��Ϣ����[%lf]!",cmpd_rate);
	WRITEMSG
	fine_rate= fine_rate/30/1000;/* ת���������� */
	cmpd_rate= cmpd_rate/30/1000;/* ת���������� */
	sprintf(acErrMsg,"���ݷ�Ϣ���ʱ��ȡfine_rate����ֵ[%lf]cmpd_rate����ֵ[%lf]",fine_rate,cmpd_rate);
	WRITEMSG
	sprintf(acErrMsg,"beg_date=[%d]end_date=[%d] [%s]",beg_date,end_date,p_ln_parm.intst_days_type);
	WRITEMSG
	/* �������� */
	ret = pub_base_CalDays( beg_date , end_date , p_ln_parm.intst_days_type , &days );
	if ( ret )
	{
		sprintf(acErrMsg,"���ü���������������!");
		WRITEMSG
		return 1;	
	}
	sprintf(acErrMsg,"���ü����������� PASS!����Ϊ[%ld]",days);
	WRITEMSG
	
	/* ��ϢΪ����*��Ϣ���� */
	*pun_intst= amt*days*fine_rate;
	/* ��ϢΪ��Ϣ*��ͬ���� */
	*cmpd_intst= intst*days*cmpd_rate;
	TRACE
	
	*pun_intst = pub_base_PubDround_1( *pun_intst );
	*cmpd_intst = pub_base_PubDround_1( *cmpd_intst );
			
	sprintf(acErrMsg,"��Ϣpun_intst=[%lf]",*pun_intst);
	WRITEMSG
	sprintf(acErrMsg,"��Ϣcmpd_intst=[%lf]",*cmpd_intst);
	WRITEMSG
			
	return 0;
}
/**********************************************************************
* �� �� ����  pub_ln_Ins_ln_reg
* �������ܣ�  ��Ӵ���Ǽǲ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��03��31��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ   
*                p_ln_parm		��������ṹ  
*                type     char[2]       ���ͣ�1����2����ǷϢ��3����ǷϢ��4����ǷϢ
*	         pamt	  double	���
*                ind      char[2]       ��Ϣ��־0���ǽ�Ϣ�գ�1����Ϣ������
*          
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_Ins_ln_reg( struct ln_mst_c p_ln_mst , struct ln_parm_c p_ln_parm , char *type , double pamt , double acm , char *ind)
{
  	int ret;
  	struct ln_reg_c    ln_reg_tmp;
  	
  	memset( &ln_reg_tmp , 0x00 , sizeof(struct ln_reg_c) );
  	sprintf(acErrMsg,"pub_ln_Ins_ln_reg���![%lf]",pamt);
	WRITEMSG
		
  	ln_reg_tmp.ac_id = p_ln_mst.ac_id;
  	ln_reg_tmp.ac_seqn = p_ln_mst.ac_seqn;
   	ln_reg_tmp.reg_date_beg = p_ln_mst.lst_date; 	
  	ln_reg_tmp.reg_date_end = g_pub_tx.tx_date;
vtcp_log("A [%d][%d]",ln_reg_tmp.reg_date_beg,ln_reg_tmp.reg_date_beg);
  	strcpy ( ln_reg_tmp.type , type );
  	ln_reg_tmp.lo_bal = pamt;
  	ln_reg_tmp.acm = acm;
vtcp_log("B [%lf][%lf]",ln_reg_tmp.lo_bal,ln_reg_tmp.acm );
  	strcpy( ln_reg_tmp.ind , ind );
  	strcpy( ln_reg_tmp.sts , "0" );
  	
  	ret = Ln_reg_Ins( ln_reg_tmp , g_pub_tx.reply );
        if(ret)
        {
            sprintf(acErrMsg,"�ǼǴ���ǼǱ�ʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L082");
            return 1;
        }

  return 0 ;
}
/**********************************************************************
* �� �� ����  pub_ln_Ins_ln_lo
* �������ܣ�  �ǼǴ���Ƿ���
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��03��31��
*
* ��    ����
*     ��  �룺   p_ln_mst           	�������ļ��ṹ   
*                p_ln_pay_pln		    ��������ṹ  
*                type     char[2]       ���ͣ�1�ѻ��� 0 δ����
*	             amt	  double	    ������
*                intst    double        ��Ϣ���
*          
*     ��  ��:    
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int pub_ln_Ins_ln_lo( struct ln_mst_c p_ln_mst , 
		struct ln_pay_pln_c p_ln_pay_pln , char *type , double amt , 
		double intst , double pay_amt , double pay_intst )
{
  	int ret=0;
  	struct ln_lo_c    ln_lo_tmp;
  	
  	memset( &ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
  	
  	ln_lo_tmp.ac_id = p_ln_mst.ac_id;
  	ln_lo_tmp.ac_seqn = p_ln_mst.ac_seqn;
  	ln_lo_tmp.lo_term = p_ln_pay_pln.curr_cnt;
  	ln_lo_tmp.lo_amt = amt;
   	ln_lo_tmp.lo_intst = intst; 	
	ln_lo_tmp.pun_intst = 0.00;
	strcpy ( ln_lo_tmp.pay_type , type );
	ln_lo_tmp.over_date = 0;
	ln_lo_tmp.shld_pay_date = p_ln_pay_pln.end_date;
	ln_lo_tmp.pay_lo_amt = pay_amt;
	ln_lo_tmp.pay_lo_intst = pay_intst;
	ln_lo_tmp.pay_pun_intst = 0.00;
	strcpy( ln_lo_tmp.old_ac_ind , "0" );	/* ��ϵͳ���� */   	
  	ret = Ln_lo_Ins( ln_lo_tmp , g_pub_tx.reply );
    if(ret)
    {
    	sprintf(acErrMsg,"�ǼǴ���Ƿ���ʧ��");
        WRITEMSG
       	strcpy(g_pub_tx.reply,"L083");
        return 1;
    }

  return 0 ;
}
/**********************************************************************
* �� �� ����  Ln_eod_pri_InsReg
* �������ܣ�  ��Ӵ������մ�ӡ�Ǽǲ�
* ��    �ߣ�  rob
* ���ʱ�䣺  2003��08��28��
*
* ��    ���� 
*     ��  �룺   
*		ln_ac_no	Char(20)	�����˺�
*		p_ln_mst           		�������ļ��ṹ
*		pay_ac_no	Char(20)	�����˺�
*		pay_ac_seqn	smallint	�����˻����
*		pay_ac_name	Char(50)	�����
*		amt		decimal(16,2)	���׽��
*		acm		decimal(19,2)	����
*		tx_code		char(4)		�������� 	
*		amt_type	Char(1)		������� 1������� 2��������Ϣ��3��������Ϣ��4��������Ϣ��	
*		acm_type	Char(1)		�������� 0����1������������ 2���������ǷϢ������3���������ǷϢ������4�������������5��͸֧
*		beg_date	Integer		��ʼ����	
*		end_date	Integer		��ֹ����	
*		trace_no	Integer		��ˮ��	
*		tx_date		Integer		��������
*	      
*     ��  ��: 
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int Ln_eod_pri_InsReg( char *ln_ac_no, struct ln_mst_c p_ln_mst, char *pay_ac_no, int pay_ac_seqn, char *pay_ac_name, 
			double amt, double acm, char *tx_code, char *amt_type, char *acm_type, long beg_date, long end_date, 
			long trace_no , long tx_date )
{
	int ret=0;
	struct ln_eod_pri_c	S_ln_eod_pri;
	memset( &S_ln_eod_pri, 0x00, sizeof( struct ln_eod_pri_c));
	
	strcpy( S_ln_eod_pri.ln_ac_no , ln_ac_no);
	S_ln_eod_pri.ln_ac_seqn = p_ln_mst.ac_seqn;
	strcpy( S_ln_eod_pri.ln_ac_name , p_ln_mst.name);
	strcpy( S_ln_eod_pri.ln_pact_no , p_ln_mst.pact_no);
	strcpy( S_ln_eod_pri.ln_ac_prdt , p_ln_mst.prdt_no);
	strcpy( S_ln_eod_pri.pay_ac_no , pay_ac_no );
	S_ln_eod_pri.pay_ac_seqn = pay_ac_seqn;
	strcpy(  S_ln_eod_pri.pay_ac_name , pay_ac_name );
	S_ln_eod_pri.amt = amt;
	S_ln_eod_pri.acm = acm;
	S_ln_eod_pri.rate = p_ln_mst.rate;
	S_ln_eod_pri.over_rate = p_ln_mst.over_rate;
	strcpy( S_ln_eod_pri.tx_code , tx_code );
	strcpy( S_ln_eod_pri.amt_type , amt_type );
	strcpy( S_ln_eod_pri.acm_type , acm_type );
	S_ln_eod_pri.beg_date = beg_date;
	S_ln_eod_pri.end_date = end_date;
	S_ln_eod_pri.trace_no = trace_no;
	S_ln_eod_pri.tx_date = tx_date;	
	
	TRACE
	sprintf( acErrMsg,"S_ln_eod_pri.amt=[%lf]acm=[%lf]",S_ln_eod_pri.amt,S_ln_eod_pri.acm);
	WRITEMSG
	ret = Ln_eod_pri_Ins( S_ln_eod_pri , g_pub_tx.reply );
	if( ret )	return 1;
	
	return 0;
}
