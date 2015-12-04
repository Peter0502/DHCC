/*************************************************
* �� �� ��: spL316.c
* ����������   
*              �����ѺƷ���
*
* ��    ��:    lance
* ������ڣ�   2003��07��31��
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
#include "trace_log_c.h"
#include "ln_gage_reg_c.h"

struct	ln_gage_reg_c	ln_gage_reg;

			
spL316()  
{ 		
	int ret=0;
	char sts[2];

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
		
	/* �ǼǴ����ѺƷ�Ǽǲ� */ 	
	strcpy( ln_gage_reg.pact_no , g_ln_mst.pact_no );	/* ��ݺ� */
	strcpy( ln_gage_reg.name , g_ln_mst.name );	/* ���� */
 	ln_gage_reg.ac_id = g_ln_mst.ac_id;			/* �����˺�ID */
 	ln_gage_reg.ac_seqn = g_ln_mst.ac_seqn;			/* �����˺���� */
	strcpy( ln_gage_reg.sts , "0" );			/* ������־ 0-��� */ 
 	ln_gage_reg.in_tx_date = g_pub_tx.tx_date;			/* �������� */ 	
  	ln_gage_reg.in_trace_no = g_pub_tx.trace_no;		/* ������ˮ�� */ 
  	
  	ret = Ln_gage_reg_Ins( ln_gage_reg , g_pub_tx.reply );
	if( ret )
	{
		sprintf( acErrMsg, "����ln_gage_reg���쳣����!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}		  	
	sprintf(acErrMsg,"�ǼǴ����ѺƷ�Ǽǲ� PASS!");
	WRITEMSG  				

	/* ���´������ļ� */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd��!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    TRACE
	ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "�������ļ����޴˼�¼" );
		WRITEMSG
		ERR_DEAL
	}else if ( ret ){
		sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd����!ret=[%d]",ret );
		WRITEMSG
		ERR_DEAL
	}
    	TRACE
    	/* �޸� */    	
	sprintf( acErrMsg, "[%lf]",g_ln_mst.gage_amt );
	WRITEMSG
	g_ln_mst.gage_amt = g_ln_mst.gage_amt + ln_gage_reg.amt;		/* ��ѺƷ��� */
	sprintf( acErrMsg, "[%lf][%lf]",g_ln_mst.gage_amt,ln_gage_reg.amt );
	WRITEMSG	
	TRACE
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "�޸Ĵ������ļ���,�����ظ���¼!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"ִ��Ln_mst_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
TRACE
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"���´������ļ� PASS!");
	WRITEMSG	


	/* ���� ��70400 */
	set_zd_data( "0210" , "01" );
	g_pub_tx.tx_amt1 = ln_gage_reg.amt;
		
	/* Pub_tx�ṹ��ֵ */
 
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3061;			/* �����ѺƷ������ */
	strcpy ( g_pub_tx.brf,"�����ѺƷ������" );	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG	
	
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
			
	/* ��ӡ����  */
	/* ��ӡ----��ѺƷ��������Ʊ */
	ret = print_LNDYRK( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNDYRK,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����ѺƷ���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����ѺƷ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset(&ln_gage_reg , 0x00 , sizeof(struct ln_gage_reg_c) );
	return 0;	
}

int data_set_struct()
{  
	int ret=0;  
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����ʺ� */
	sprintf(acErrMsg,"�����ʺ�[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_double("0420",&ln_gage_reg.amt);			/* ��ѺƷ��ֵ */
	sprintf(acErrMsg,"���׽��[%ld]",ln_gage_reg.amt);
	WRITEMSG	
	
	get_zd_data("0220",ln_gage_reg.gage_type);			/* ��ѺƷ���� */
	get_zd_data("0810",ln_gage_reg.gage_inf);			/* ��ѺƷ��Ϣ */
			
	g_pub_tx.ac_seqn = 0 ;	/* �����˻���� */
		
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
				&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}
						
	return 0;
}

/* ��ӡ--��ѺƷ��������Ʊ */
int print_LNDYRK( )
{
	char tx_type[9];

	char exp_ind[5];
	char in_out_ind[5];
	char dc_ind[5];
	char gage_name[21];
	
	char rate[11];
	char over_rate[11];
			
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
		sprintf(acErrMsg,"LNDYRK���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "��ѺƷ���" );	
	
	/* ȡ��Ŀ�� */
	strcpy( ln_acc_hrt , "704" );	/* �����Ŀ */		
	
	/* ������־ */
	strcpy( in_out_ind , "���");
	
	/* �����־ */
	strcpy( dc_ind , "����");
	
	/* ��ѺƷ���� */
	pub_base_dic_show_str(gage_name,"gage_type",ln_gage_reg.gage_type);		
	
	if( g_ln_mst.exp_mtr_date )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(չ)" );
		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
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
	/* 16.��ˮ�š�   17.����⡢   18.�����־��19.��ѺƷ���͡�20.��ע  */
	/* 21.��ѺƷ��ֵ��	*/
	fprintf(fp,"LNDYRK%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|%s|%.2lf|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	ln_gage_reg.amt , g_ln_mst.opn_date , end_date , 0.00 , 0.00 ,
	g_ln_mst.bal , rate , over_rate , ln_acc_hrt , exp_ind , 
	g_pub_tx.trace_no , in_out_ind , dc_ind , gage_name , ln_gage_reg.gage_inf ,
	g_ln_mst.gage_amt);	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
