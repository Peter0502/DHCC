/*************************************************
* �� �� ��: spL302.c
* ����������   
*              ����״̬ת��
*
* ��    ��:    lance
* ������ڣ�   2003��01��21��
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

	char typ[2];
		
spL302()  
{ 	
	int ret=0;

	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
	
	/** ���ݳ�ʼ�� **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"���ݳ�ʼ�� PASS!");
	WRITEMSG
		
	/** ȡֵ����ֵ **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	
	/** �������� **/
	/* �������Ƿ��� */
	if ( typ[0]=='*' || typ[0]=='9' )
	{
		sprintf( acErrMsg, "�޸ĵ�״̬������Ҫ��[%s]",typ);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L055");
		ERR_DEAL
	}else{				
		/* ֻ���������޸� modified by liuxuan 20071012*/
		if ( atoi(g_ln_mst.ac_sts)!=5){		/* 5������90����,���Ե��ر��� */
			if ( atoi(g_ln_mst.ac_sts) >  atoi(typ) )
			{
				sprintf( acErrMsg, "�޸ĵ�״̬������Ҫ��,ֻ�������޸�[%s]",typ);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L112");
				ERR_DEAL				
			}
		}
		else if ( atoi(g_ln_mst.ac_sts)==5 && atoi(typ)<3 ){
				sprintf( acErrMsg, "�޸ĵ�״̬������Ҫ��,ֻ�������޸�[%s]",typ);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L112");
				ERR_DEAL				
		}
	}
	
	/* ������״̬δ���ı�ʱ,��ʾ���� */
	if( strcmp(g_ln_mst.ac_sts , typ)==0 )
	{
		sprintf( acErrMsg, "����״̬δ���κθı�[%s]",typ);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L214");
		ERR_DEAL
	}		
		
	if ( g_ln_mst.mtr_date >= g_pub_tx.tx_date  )
	{	
		if ( typ[0]!='4' &&  typ[0]!='1' )
		{
			sprintf( acErrMsg, "����δ����,�޸ĵ�״̬������Ҫ��[%s]",typ);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L054");
			ERR_DEAL			
		}
	}
	

	/* �޸����ļ�״̬ */
	
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d ", g_ln_mst.ac_id , g_pub_tx.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd �α����[%d]",ret);
		WRITEMSG
		ERR_DEAL
	}
	
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
			
	ret = Ln_mst_Fet_Upd( &g_ln_mst , g_pub_tx.reply );
	if (ret==100)
	{
		sprintf( acErrMsg,"�������ļ�������Ӧ��¼[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		ERR_DEAL
	}else if( ret )
 		{
	 		sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		ERR_DEAL
 		}
	
	strcpy( g_ln_mst.ac_sts , typ );
	
	ret = Ln_mst_Upd_Upd( g_ln_mst , g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		ERR_DEAL
	}
	
	Ln_mst_Clo_Upd( );
	sprintf(acErrMsg,"���´������ļ�����״̬��־ PASS");
	WRITEMSG		
	
	/* Pub_tx�ṹ��ֵ */

	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy ( g_pub_tx.brf,"����״̬ת��" );
	g_pub_tx.svc_ind = 3060;			/* �����ת */
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ[%d]",g_pub_tx.svc_ind);
	WRITEMSG
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
	
    	set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */
    	set_zd_double("107F",g_ln_mst.bal);	/* ���� */
	sprintf(acErrMsg,"L302[%lf]",g_ln_mst.bal);
	WRITEMSG  
	   					
	/* ��ӡ���� */
	/* �����ת��Ʊ--ת�벿�� */
	ret = print_LNBKZR( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKZR,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"���һ���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"���һ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* �ʺ� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* �˻���� */
	get_zd_data("0700",typ);			/* ת��״̬ */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) {return ret;}
					

	return 0;
}

/* ��ӡ--�����ת��Ʊ--ת�벿�� */
int print_LNBKZR( )
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
		sprintf(acErrMsg,"LNBKZR���ļ�ʧ��!");
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
	tol_pay = g_ln_mst.ttl_prvd_amt - g_ln_mst.bal;
			
	/* ��Ƿ���� */		
	tol_need_pay = g_ln_mst.bal;

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
	fprintf(fp,"LNBKZR%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	g_ln_mst.bal , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts ,"");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
