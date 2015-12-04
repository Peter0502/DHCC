/*************************************************
* �� �� ��: spL313.c
* ����������   
*              �����Ʒת��
*
* ��    ��:    lance
* ������ڣ�   2003��06��07��
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

static	long	 end_date_new;
static  double	 maga_amt_new;
static	char	 prod_no[4];
	double old_ln_amt=0.00;
			
spL313()  
{ 	
	
	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	
	memset ( &ln_mst_new , 0x00 , sizeof( struct ln_mst_c ) );
	memset ( &ln_parm_new , 0x00 , sizeof( struct ln_parm_c ) );
		
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
	
	/* ȡ�ϲ�Ʒ��Ϣ */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG	

	old_ln_amt = g_ln_mst.bal ;
	
	/* ȡ�²�Ʒ��Ϣ */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����Ʒ�������޴˼�¼ erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡ�����Ʒ�������쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		goto ErrExit;
    		}
	sprintf(acErrMsg,"ȡ�����Ʒ������Ϣ PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"313�����ִ����Ʒ[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG	

	/* del by lance 2003-11-15   �жϴ���״̬�Ƿ����� *

	if( g_ln_mst.ac_sts[0]!='1'  )
	{
		sprintf(acErrMsg,"�����ʻ�״̬������,�������˽���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L137");
		goto ErrExit;
	}
	if( g_ln_mst.exp_ind[0]!='0'  )
	{
		sprintf(acErrMsg,"�����ʻ���չ��,�������˽���![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L138");
		goto ErrExit;
	}	
	*******/
	set_zd_data("0210","01");	/* ���� */
	
	/* ���Ҳ���ת��ͨ */	
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		if( ln_parm_new.ln_pay_type[0]!='3' && ln_parm_new.ln_pay_type[0]!='4' )
		{
			sprintf(acErrMsg,"���Ҵ������ͨ����䲻��ת��![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L141");
			goto ErrExit;			
		}
	}
	
	/* ��ͨ����ת���� */	
	if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		if( ln_parm_new.ln_pay_type[0]=='3' || ln_parm_new.ln_pay_type[0]=='4' )
		{
			sprintf(acErrMsg,"���Ҵ������ͨ����䲻��ת��![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L141");
			goto ErrExit;			
		}
	}
		
	/* ����ת����ʽ��ϸ���� */
	/* ��Ѻ ת ��Ѻ(��Ѻ���仯) */
	if( g_ln_parm.ln_type[0]=='3' && ln_parm_new.ln_type[0]=='3' )
	{
		 amt_flag = pub_base_CompDblVal( g_ln_mst.gage_amt , maga_amt_new );
		 if( amt_flag!=0 )
		 {
			/* ���²�Ʒ�� ��704 */
			set_zd_double("107B",g_ln_mst.gage_amt);
			set_zd_data("107A","1");	/* �� */
			tag++;
			TRACE
		 }		  		 	                           
	}
	/* ��Ѻ ת ����(�����Ŀ,��ѺƷ���) */
	if( g_ln_parm.ln_type[0]=='3' && ln_parm_new.ln_type[0]=='4' )
	{
		 amt_flag = pub_base_CompDblVal( g_ln_mst.gage_amt , maga_amt_new );
		 if( amt_flag!=0 )
		 {
			/* ���²�Ʒ�� ��704 */
			set_zd_double("107B",g_ln_mst.gage_amt);
			set_zd_data("107A","1");	/* �� */
			tag++;
			TRACE
		 }
		set_zd_double("1074",-g_ln_mst.bal);
		tag++;	
		TRACE		                           
	}					 		
	/* ��Ѻ ת ����(�����Ŀ,��ѺƷ���) */
	if( g_ln_parm.ln_type[0]=='3' && ln_parm_new.ln_type[0]=='5' )
	{
		 amt_flag = pub_base_CompDblVal( g_ln_mst.gage_amt , maga_amt_new );
		 if( amt_flag!=0 )
		 {
			/* ���²�Ʒ�� ��704 */
			set_zd_double("107B",g_ln_mst.gage_amt);
			set_zd_data("107A","1");	/* �� */
			tag++;
			TRACE
		 }
		 
		set_zd_double("1074",-g_ln_mst.bal);
		tag++;	
		TRACE		                           
	}
	/* ���� ת ����(�����Ŀ) */
	if( g_ln_parm.ln_type[0]=='4' && ln_parm_new.ln_type[0]=='5' )
	{
		set_zd_double("1074",-g_ln_mst.bal);
		tag++;	
		TRACE		                           
	}
	/* ���� ת ����(�����Ŀ) */
	if( g_ln_parm.ln_type[0]=='5' && ln_parm_new.ln_type[0]=='4' )
	{
		set_zd_double("1074",-g_ln_mst.bal);			                           
		tag++;	
		TRACE	
	}				
	/* ���� ת ��Ѻ(�����Ŀ,����ѺƷ) */
	if( g_ln_parm.ln_type[0]=='5' && ln_parm_new.ln_type[0]=='3' )
	{				
		set_zd_double("1074",-g_ln_mst.bal);	
		tag++;	
		TRACE				                           
	}
	/* ���� ת ��Ѻ(�����Ŀ,����ѺƷ) */
	if( g_ln_parm.ln_type[0]=='4' && ln_parm_new.ln_type[0]=='3' )
	{				
		set_zd_double("1074",-g_ln_mst.bal);	
		tag++;	
		TRACE				                           
	}
	/* ���ޱ仯 */
	if( strcmp( g_ln_parm.time_type , ln_parm_new.time_type ) )
	{
		/* �����Ŀ�ı� */
		set_zd_double("1074",-g_ln_mst.bal);
		/* ������Ҵ���,��Ҫ�������ɻ���ƻ� */
		if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
		{			
			if( g_ln_mst.mtr_date != end_date_new )
			{
				ret = pub_ln_Chg_PayPlan( g_ln_mst , g_ln_parm , g_ln_mst.rate , g_ln_mst.bal , g_pub_tx.tx_date , end_date_new );
				if( ret ) goto ErrExit;
			}		
		}
		tag++;
		TRACE
	}
	
	/* ��Ʒ�ű仯 */
	if( strcmp( g_ln_parm.prdt_no, ln_parm_new.prdt_no) )
	{
		set_zd_double("1074",-g_ln_mst.bal);
		tag++;
	}
	/*** ��ʱ���� gyf.20060811
	if( g_ln_mst.ac_id==100375336 )
	{
		;
	}else{
		tag=0;
	} ***/	
	
	if( tag==0 )
	{
		sprintf(acErrMsg,"�����ִ����Ʒ������ת��,����![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L140");
		goto ErrExit;
	}
	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.tx_amt1 = -g_ln_mst.bal;
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� 
	strcpy ( g_pub_tx.hst_ind , "1" );*/
	strcpy ( g_pub_tx.hst_ind , "0" );/*���� ���ݽ������� modify by martin 2009/12/23 19:38:19*/
	g_pub_tx.svc_ind = 3060;			/* �����ת */

	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
	
	strcpy ( g_pub_tx.brf,"�����Ʒת��(����)" );
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
	
	/*** �Ǽǽ�����ˮ ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG
	
	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.tx_amt1 = g_ln_mst.gage_amt;
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� 
	strcpy ( g_pub_tx.hst_ind , "1" );*/
	strcpy ( g_pub_tx.hst_ind , "0" );/*���� ���ݽ������� modify by martin 2009/12/23 19:38:19*/
	g_pub_tx.svc_ind = 3060;			/* �����ת */

	strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
	/*strcpy ( g_pub_tx.brf,"�����Ʒת��(��ѺƷ����)" );*/ /*����20λgujy 060824*/
	strcpy( g_pub_tx.brf, "��Ʒת��(�ֲ���)");
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
	
	/*** �Ǽǽ�����ˮ ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
	WRITEMSG		

	/* ��ӡ���� */
	/* �����Ʒת����Ʊ--ת������ */
	ret = print_LNBKCPZC( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKCPZC,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	
		

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����Ʒת���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����Ʒת��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    if ( strcmp( g_pub_tx.reply , "L018" )==0 )
    {
    	strcpy( g_pub_tx.reply , "L115" );
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("0310" , g_pub_tx.ac_no); 		/* �����˺� */
	g_pub_tx.ac_seqn = 0;				/* �˺���� */

	get_zd_data("0240" , prod_no); 			/* �²�Ʒ���� */
	get_zd_double("0420" , &maga_amt_new); 		/* �µ�ѺƷ��� */
	get_zd_long("0450" , &end_date_new); 		/* �µ������� */					
	return 0;
}
/* ��ӡ--�����Ʒת����Ʊ--ת������ */
int print_LNBKCPZC( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
	char ln_sts[5];	
	char dc_sts[3];	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKCPZC���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "ת��" );	
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0152" );	/* �����Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	/* �����־ */
	strcpy( dc_sts , "��" );
	
	/* ����״̬ */
	pub_base_dic_show_str(ln_sts,"ln_ac_sts",g_ln_mst.ac_sts);	
		
	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.ttl_prvd_amt ;
	
	/* �ۼƻ��� */	
	tol_pay = g_ln_mst.ttl_prvd_amt - old_ln_amt;
			
	/* ��Ƿ���� */		
	tol_need_pay = old_ln_amt;

	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(չ)" );
		sprintf( rate , "%8.5lf��" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	}
		
	sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.�������ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.�������ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.��ˮ�š�   17.�����־�� 18.����״̬  19.���ֱ�־*/
	fprintf(fp,"LNBKCPZC%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	-old_ln_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts, "(����)");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
