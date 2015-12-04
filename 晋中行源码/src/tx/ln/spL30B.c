/*************************************************
* �� �� ��: spL30B.c
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

	double old_ln_amt=0.00;
	double acm_rate=0.00;
				
spL30B()  
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
	/* ȡ��ֶλ���������� */
	ret = pub_ln_GetRate ( g_ln_mst , g_ln_parm , &acm_rate );
	if( ret )	
	{
		sprintf(acErrMsg,"ȡ��������ʧ��!");
		WRITEMSG
		ERR_DEAL
	}
	/* ������� */	
	ret = Insert_intst_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"���������ֶλ��������!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}
	
	
	/* ���ڻ��� */
	ret = Insert_in_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"���ڻ�����ֶλ��������!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}	

	/* ������� */
	ret = Insert_out_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"���������ֶλ��������!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}
	/* �������� */
	ret = Insert_cmpd_acm();
	if( ret )	
	{
		sprintf(acErrMsg,"����������ֶλ��������!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L215" );
		ERR_DEAL
	}
	
	/* Pub_tx�ṹ��ֵ */

	g_pub_tx.ac_id = g_ln_mst.ac_id;	/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "1" );	/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );	/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	g_pub_tx.svc_ind = 3063;		/* �����ת--ת�� */
	strcpy ( g_pub_tx.brf,"����״̬ת��" );

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
    	g_ln_mst.bal=-g_ln_mst.bal;
    	set_zd_double("1074",g_ln_mst.bal);	/* ���� */    	
    	/** ��ɳ���� ����Ϣ������ֶλ��� ****/
    	/***
    	set_zd_double("1075",g_pub_tx.tx_amt2);	* �������Ϣ *				
    	set_zd_double("1076",g_pub_tx.tx_amt3);	* �������Ϣ *
    	set_zd_double("1077",g_pub_tx.tx_amt4);	* �븴����Ϣ *
	*****/
	
	sprintf(acErrMsg,"L30B[%lf][%lf][%lf][%lf]",g_ln_mst.bal,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
	WRITEMSG    	

	/* ��ӡ���� */
	/* �����ת��Ʊ--ת������ */
	ret = print_LNBKZC( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKZC,ʧ������!" );
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

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	old_ln_amt = g_ln_mst.bal ;
	
	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) {return ret;}
					

	return 0;
}
/* ������� */
int Insert_intst_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
		
	/* ���㱾����� */
	strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
	acm = g_ln_mst.intst_acm ;/* ��Ϣ���� */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.bal , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
	WRITEMSG	
	/* �ǼǷֶλ���--������� */		
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '2' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm , acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"���������ǼǷֶλ���ʧ��!");
		WRITEMSG
		return -1;
	}
	
	return 0;
}
/* ���ڻ��� */
int Insert_in_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
			
	/* ������ڻ��� */
	strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
	acm = g_ln_mst.in_lo_acm ;/* ��Ϣ���� */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.in_lo_intst , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
	WRITEMSG	
	/* �ǼǷֶλ���--���ڻ��� */	
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '3' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm ,acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"������ڻ����ǼǷֶλ���ʧ��!");
		WRITEMSG
		return -1;
	}
	
	return 0;
}

/* ������� */
int Insert_out_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
			
	/* ���������� */
	strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
	acm = g_ln_mst.out_lo_acm ;/* ��Ϣ���� */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.out_lo_intst , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
	WRITEMSG	
	/* �ǼǷֶλ���--������� */		
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '4' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm ,acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"�����������ǼǷֶλ���ʧ��!");
		WRITEMSG
		return -1;
	}
	return 0;
}

/* �������� */
int Insert_cmpd_acm()
{
	int  ret=0;
	char intst_type[2];
	char acm_type[2];
	double acm=0.00;
			
	/* ���㸴������ */
	strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
	acm = g_ln_mst.cmpd_lo_acm ;/* ��Ϣ���� */
	
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date , g_pub_tx.tx_date , 
		g_ln_parm.intst_days_type , acm_type , 
		g_ln_mst.cmpd_lo_intst , 0.00 , &acm , "0",0,0);
	if( ret )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return -1;	
	}
	sprintf(acErrMsg,"���ú����������PASSΪ [%lf]",acm );
	WRITEMSG	
	/* �ǼǷֶλ���--�������� */		
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '5' ,
			g_ln_mst.ic_date, g_pub_tx.tx_date , acm ,acm_rate, g_pub_tx.reply);
	
	if( ret )	
	{
		sprintf(acErrMsg,"����������ǼǷֶλ���ʧ��!");
		WRITEMSG
		return -1;
	}
	return 0;
}


	
/* ��ӡ--�����ת��Ʊ--ת������ */
int print_LNBKZC( )
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
		sprintf(acErrMsg,"LNBKZC���ļ�ʧ��!");
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
	fprintf(fp,"LNBKZC%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	-old_ln_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts ,"(����)");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
