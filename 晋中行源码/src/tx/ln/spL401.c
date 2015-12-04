/*************************************************
* �� �� ��: spL401.c
* ����������   
*              �����ף��������ʵ���
*
* ��    ��:    lance
* ������ڣ�   2003��04��01��
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
#include "ln_expand_c.h"
#include "com_rate_c.h"
	
char 	rate_no[4],cur_no[3];
double 	rate,rate_parm;
static  struct ln_expand_c 	p_ln_expand;	
static  struct com_rate_c 	t_com_rate;
		
spL401()  
{ 		
	int ret=0 ,min_ret=0 ,max_ret=0;
	int rate_flag;
	char acm_type[2],ln_flag[2];
			
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();	
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;	
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;	
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
			
	/** �������� **/
	/* ���¹������ʱ� */
	ret = Com_rate_Dec_Upd( g_pub_tx.reply , "cur_no='%s' and\
						rate_code='%s' and end_date=99999999",cur_no,rate_no);
    if ( ret==100 )
	{
		sprintf( acErrMsg, "���ʱ����޷��������ļ�¼ret=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply , "L088" );
		goto ErrExit;
	}else if ( ret ){
	TRACE
		sprintf( acErrMsg, "ִ��Com_rate_Dec_Upd��!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	ret = Com_rate_Fet_Upd( &t_com_rate , g_pub_tx.reply );
	TRACE
	if( ret )
	{
		sprintf( acErrMsg, "ִ��Com_rate_Fet_Upd��!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	t_com_rate.end_date = g_pub_tx.tx_date;
	ret = Com_rate_Upd_Upd( t_com_rate , g_pub_tx.reply );
	if( ret )
	{
	TRACE
		sprintf( acErrMsg, "ִ��Com_rate_Upd_Upd��!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}		
	ret = Com_rate_Clo_Upd( );
	if( ret )
	{
	TRACE
		sprintf( acErrMsg, "ִ��Com_rate_Fet_Upd��!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	/* ����޸ĵļ�¼ */
	strcpy( t_com_rate.cur_no , cur_no );
	t_com_rate.beg_date = g_pub_tx.tx_date;
	t_com_rate.end_date = 99999999;
	t_com_rate.rate_val = rate;
	ret = Com_rate_Ins( t_com_rate , g_pub_tx.reply );
	if( ret )
	{
	TRACE
		sprintf( acErrMsg, "�������ʱ���Ӽ�¼ʧ��!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	
	/* ��ѯ��������������������ļ�¼(����,���ʱ��,�������ʱ�־ */
	ret = Ln_parm_Dec_Sel( g_pub_tx.reply , "get_rate_type='1' and cur_no='%s' and ( rate_no='%s' or over_rate_no='%s' )" ,cur_no , rate_no , rate_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_parm_Dec_Upd��!ret=[%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{		
		ret = Ln_parm_Fet_Sel( &g_ln_parm,g_pub_tx.reply );   
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "������������޼�¼" );
			WRITEMSG
			break;
		}else if ( ret ){
			sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd����!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		if( g_pub_tx.tx_date < g_ln_parm.beg_date||g_pub_tx.tx_date > g_ln_parm.end_date )
		
		if( !strcmp( rate_no , g_ln_parm.rate_no ) )
		{
			rate_flag=1;/* �������� */
		}else{
			rate_flag=2;/* �������� */
		}	
		/* ����Ʒ����ȡ�������ļ������� */
		ret = Ln_mst_Dec_Sel( g_pub_tx.reply , "get_rate_type='1' and cur_no='%s' and ( rate_no='%s' or over_rate_no='%s' )" ,cur_no , rate_no , rate_no );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_parm_Dec_Upd��!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			ret = Ln_mst_Fet_Upd( &g_ln_mst , g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd��!ret=[%d]",ret );
				WRITEMSG
				goto ErrExit;
			}
			/* ȡ����״̬ */
			ret = pub_ln_check_acsts(ln_flag);
			/* ���Ҵ��� */	
			if ( g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4' )
			{
				/* �޸��������� */
				if( rate_flag==2 )
				{
					g_ln_mst.over_rate = rate ;
				}
				/* �޸��������� */
				if( rate_flag==1 )
				{
					g_ln_mst.rate = rate ;
					/* �������ɻ���ƻ� */
					ret = pub_ln_Chg_PayPlan( g_ln_mst , g_ln_parm , g_ln_mst.rate , g_ln_mst.bal , g_pub_tx.tx_date , g_ln_mst.mtr_date );
					if( ret==1) break;
					if( ret==-1 ) goto ErrExit;										
				}
			}else{		
			/* ��ͨ���� */
				/* �޸��������� */
				if( rate_flag==1)
				{
					if(ln_flag[0]=='1')/* ״̬Ϊ���� */
					{
						g_ln_mst.rate = rate;						
						/* �ǼǷֶλ����� */
						strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
						ret = Ins_Acm( acm_type , g_ln_mst.rate );
						if (ret) goto ErrExit;
					}
					if(ln_flag[0]=='Z')/* ״̬Ϊչ�� */
					{						
						g_ln_mst.rate = rate;						
					}
					if(ln_flag[0]=='2'||ln_flag[0]=='3'||ln_flag[0]=='4')/* ״̬Ϊ���� */
					{						
						g_ln_mst.rate = rate;							
					}
					if(ln_flag[0]=='*'||ln_flag[0]=='9')/* ״̬Ϊ���� */
					{						
						break;						
					}				
				}
				/* �޸��������� */					
				if( rate_flag==2)
				{
					if(ln_flag[0]=='1')/* ״̬Ϊ���� */
					{
						g_ln_mst.over_rate = rate;						
					}
					if(ln_flag[0]=='Z')/* ״̬Ϊչ�� */
					{						
						g_ln_mst.over_rate = rate;						
					}
					if(ln_flag[0]=='2'||ln_flag[0]=='3'||ln_flag[0]=='4')/* ״̬Ϊ���� */
					{						
						g_ln_mst.over_rate = rate;						
						/* �ǼǷֶλ����� */
						strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
						ret = Ins_Acm( acm_type , g_ln_mst.over_rate );
						if (ret) goto ErrExit;												
					}
					if(ln_flag[0]=='*'||ln_flag[0]=='9')/* ״̬Ϊ���� */
					{						
						break;						
					}		
				}
			}
			/* ���´������ļ���¼ */								
			ret = Ln_mst_Upd_Upd( g_ln_mst , g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Ln_mst_Upd_Upd��!ret=[%d]",ret );
				WRITEMSG
				goto ErrExit;
			}
			
			/* Pub_tx�ṹ��ֵ */
			strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
			strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
			/* �Ƿ��ӡ����(�������۱�־):1��0�� */
			strcpy ( g_pub_tx.prt_ind , "0" );	
			/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
			strcpy ( g_pub_tx.hst_ind , "1" );
			g_pub_tx.svc_ind = 3060;			
			strcpy ( g_pub_tx.brf,"�������ʵ���" );	
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
		}
		ret = Ln_mst_Clo_Upd( );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_mst_Clo_Sel��!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}	
	}
	ret = Ln_parm_Clo_Sel( );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_parm_Clo_Sel��!ret=[%d]",ret );
		WRITEMSG
		goto ErrExit;
	}						
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�������ʵ����ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�������ʵ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset ( &p_ln_expand ,0x00 ,sizeof(struct ln_expand_c) );
	memset ( &t_com_rate ,0x00 ,sizeof(struct com_rate_c) );
	return 0;	
}

int data_set_struct()
{    				
	get_zd_data("0210",cur_no);			/* ���� */
	sprintf(acErrMsg,"����[%s]",cur_no);
	WRITEMSG
		
	get_zd_data("1161",rate_no);			/* ���ʱ�� */
	sprintf(acErrMsg,"���ʱ��[%s]",rate_no);
	WRITEMSG
	
	get_zd_double("0840",&rate);			/* ���� */
	sprintf(acErrMsg,"����[%lf]",rate);
	WRITEMSG	
						
	return 0;
}
/**********************************************************************
* �� �� ����  Ins_Acm
* �������ܣ�  ����������ļ��и�������
*	      ��ӷֶλ�����
*	      �����ļ��и���������0
*             �޸���Ϣ����
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��04��02��
*
* ��    ���� 
*     ��  �룺acm_type   char(2)   �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩   
*             rate       double    ����(��ӷֶλ�����)
*     ��  ��:           char(4)   ��Ӧ��
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��
*                   
********************************************************************/
int Ins_Acm( char *acm_type , double rate )
{
	int ret=0;
	
	/* ͳ�ƴ������ļ��л��� */
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.bal,0,
						&g_ln_mst.intst_acm,"0",0,0 );
	if( ret )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return -1;	
	}				
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '2',
		  g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.intst_acm,
		  rate, g_pub_tx.reply);
	if (ret)
	{
		sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
		WRITEMSG
		return -1;	
	}
	/* �����㸴�� */
	if ( g_ln_mst.cmpd_intst_ind[0]=='Y' )
	{
		/* ����ǷϢ���� */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.in_lo_intst,0,
					&g_ln_mst.in_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}	
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '3',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.in_lo_acm,
			  rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			return -1;	
		}
		/* ����ǷϢ���� */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.out_lo_intst,0,
					&g_ln_mst.out_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}				
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '4',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.out_lo_acm,
			  rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			return -1;	
		}	
		/* �������� */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,g_ln_parm.intst_days_type,acm_type,g_ln_mst.cmpd_lo_intst,0,
					&g_ln_mst.cmpd_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}		
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '5',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.cmpd_lo_acm,
			  rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			return -1;	
		}		
	}
	
	/* ���ļ��л�����0 */	
	g_ln_mst.intst_acm = 0;			/* ��Ϣ���� */		
	g_ln_mst.in_lo_acm = 0;			/* ����ǷϢ���� */
	g_ln_mst.out_lo_acm= 0;			/* ����ǷϢ���� */
	g_ln_mst.cmpd_lo_acm = 0;		/* �������� */				
	g_ln_mst.lst_date = g_pub_tx.tx_date;	/* �ϱʷ������� */
	g_ln_mst.ic_date = g_pub_tx.tx_date;	/* ��Ϣ���� */	
	
	return 0;
}	
