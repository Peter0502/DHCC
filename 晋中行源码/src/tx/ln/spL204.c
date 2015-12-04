/*************************************************
* �� �� ��: spL204.c
* ����������   
*              ���Ҵ�����ǰ����
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
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
#include "ln_mst_hst_c.h"

static struct ln_lo_c		ln_lo;
static struct ln_pay_pln_c	ln_pay_pln;
char ct_flag[2];
double print_tol_amt=0.00;
	
spL204()  
{ 	
	int ret=0;
	int pay_clear=0;
	long dqr=0;
	char clear_sts[2];	

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
	
	/* ��ѯ����Ƿ��� */
	ret=sql_count("ln_lo","ac_id=%ld and ac_seqn=%d  and pay_type='0'" , 
								g_pub_tx.ac_id,g_pub_tx.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg, "�ð��Ҵ������Ƿ��,������ǰ����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L047");
		ERR_DEAL
	}		
	
	/* ��鵱���Ƿ��Ի� */

	ret=Ln_pay_pln_Sel(g_pub_tx.reply, &ln_pay_pln, "ac_id=%ld and ac_seqn=%d"
					 , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"�����ƻ�����Ϣ���޴���Ϣ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L043" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡ�����ƻ�����Ϣ�쳣erro code=[%s]",ret);
		WRITEMSG
    	ERR_DEAL
  	}	
  if(ln_pay_pln.curr_cnt==1)
  {
  		sprintf( acErrMsg, "���Ҵ����δ��,������ǰ����");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L048");
		ERR_DEAL
  }	
  else
  {
  	   long tmp_date=0;
  	   if(g_pub_tx.tx_date%100<g_ln_parm.fst_pay_day)
  	   {
  	   	if((g_pub_tx.tx_date/100*100+g_ln_parm.fst_pay_day)==ln_pay_pln.beg_date)
  	   	{
  				sprintf( acErrMsg, "���Ҵ����δ��,������ǰ����");
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"L048");
		    	ERR_DEAL
  	   	}
  	   }
  	   else
  	   {
  	   	ret=pub_base_deadlineM(g_pub_tx.tx_date,1,&tmp_date);
  	   	if((tmp_date/100*100+g_ln_parm.fst_pay_day)==ln_pay_pln.beg_date)
  	   	{
  				sprintf( acErrMsg, "���Ҵ����δ��,������ǰ����");
		    	WRITEMSG
		    	strcpy(g_pub_tx.reply,"L048");
		    	ERR_DEAL
  	   	}
  	  }
  }	
  /*
	pub_base_deadlineM( ln_pay_pln.end_date, -1, &dqr);
	sprintf( acErrMsg, "�����ա�%ld��end_date[%ld]",dqr,ln_pay_pln.end_date);
	WRITEMSG
  	if( g_pub_tx.tx_date > dqr )
  	{	
  		sprintf( acErrMsg, "���Ҵ����δ��,������ǰ����");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L048");
		ERR_DEAL
	}	
	*/
	
	/* �Ǽǰ���Ƿ����Ǽǲ� */
	ret = Ins_ln_pay_pln_rol( ln_pay_pln );
	if( ret )
	{
 		sprintf( acErrMsg,"���õǼǰ��һ���ƻ������Ǽǲ������쳣����!");
 		WRITEMSG
 		goto ErrExit;
	}
		
	/* �۱��� */
	g_pub_tx.tx_amt1 = g_pub_tx.tmp_amt1 ;
	
 	sprintf( acErrMsg,"ǰ���bal=[%lf]�۱���=[%lf]",g_ln_mst.bal,g_pub_tx.tx_amt1);
 	WRITEMSG
 	
 	/* ����Ƿ��� */
 	pay_clear = pub_base_CompDblVal( g_ln_mst.bal-g_pub_tx.tx_amt1 , 0.00 );
 	if(pay_clear<0)
 	{
  		sprintf( acErrMsg,"���������,���ڴ������!");
 		WRITEMSG
 		strcpy( g_pub_tx.reply , "L226");
 		goto ErrExit;
 	}else if(pay_clear>0){
		TRACE
		/***** �����µĻ���ƻ� *********/
		ret = pub_ln_Chg_PayPlan(g_ln_mst, g_ln_parm, g_ln_mst.rate, 
				g_ln_mst.bal-g_pub_tx.tx_amt1, g_pub_tx.tx_date, g_ln_mst.mtr_date);
		if( ret ) goto ErrExit;
		
	 	sprintf( acErrMsg,"�����bal=[%lf]!�۱���=[%lf]",g_ln_mst.bal,g_pub_tx.tx_amt1);
	 	WRITEMSG			
		/***
		g_ln_mst.bal = g_ln_mst.bal + g_pub_tx.tx_amt1;
		****/
	 	sprintf( acErrMsg,"�����bal=[%lf]�۱���=[%lf]",g_ln_mst.bal,g_pub_tx.tx_amt1);
	 	WRITEMSG
	 }else if(pay_clear==0){
	 	TRACE
	 	/* ���»���ƻ��б�־Ϊ�ѻ��� */
		ret = sql_execute( "UPDATE ln_pay_pln SET pay_ind='1'  WHERE ac_id=%ld and ac_seqn=%d " , 
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn );
		if( ret ) goto ErrExit;	
		/*�޸Ĵ�������*/
		g_ln_mst.bal=0.00;
		ret = sql_execute( "UPDATE ln_mst SET bal=0.00  WHERE ac_id=%ld and ac_seqn=%d " , 
				ln_pay_pln.ac_id,ln_pay_pln.ac_seqn );
		if( ret ) goto ErrExit;
	}
 		
	/* �ǼǴ���Ǽǲ� */
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,"1",
											g_pub_tx.tmp_amt1,0.00,"1",1 );
	if( ret ) 	goto ErrExit;	
	
	/* Pub_tx�ṹ��ֵ */
	sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf]bal[%lf]",g_pub_tx.tx_amt1
																,g_ln_mst.bal);
	WRITEMSG	
	set_zd_data( "0210" , g_ln_parm.cur_no );
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	g_pub_tx.svc_ind = 3018 ;
	strcpy ( g_pub_tx.brf,"������ǰ����" );

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
	
	/* ����Ƿ��� */
	ret = pub_ln_chk_clear(g_ln_mst,g_ln_parm,clear_sts);
	if(ret)	goto ErrExit;
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , 0.00 ) )
	{			
		set_zd_data("109K",clear_sts);
		/* �����е�ѺƷ */
	}else{
		set_zd_data("109K","*");
		/* �����е�ѺƷ */
	}	
	
	/* ת�˽��˵�,�ֽ�ɿ */
	ret = print_LNDF( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNDF,ʧ������!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* �ͻ��ص�--������ǰ�����ص� */
	ret = print_LNHDAJTQ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDAJTQ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* ���ڴ�Ʊ--���𲿷� */
	ret = print_LNBKAJBJ( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJBJ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE

	pub_mob_sendmail_ln("���Ҵ�����ǰ�黹����",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);
	
	if( ct_flag[0]=='1' )	/** �ֽ� **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*ת��*/
		goto ErrExit2;
    else if (ct_flag[0] == '3')		/*���*/
        goto ErrExit3;

ErrExit:
    sprintf(acErrMsg,"������ǰ����ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"������ǰ�����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"������ǰ�����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"������ǰ�����ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;
	struct ln_mst_hst_c 	S_ln_mst_hst;
	memset( &S_ln_mst_hst,0x00 , sizeof( struct ln_mst_hst_c));
	
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* �ʺ� */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* �˻���� */
	get_zd_double("0400",&g_pub_tx.tmp_amt1);	/* ������ */
	
	print_tol_amt = g_pub_tx.tmp_amt1;
	
	get_zd_data("0230",g_pub_tx.note_type);		/* ƾ֤���� */
	get_zd_data("0580",g_pub_tx.beg_note_no);	/* ��ʼƾ֤���� */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	get_zd_data("0660",g_pub_tx.ct_ind);			/* ��ת��־ */
	get_zd_data("0660",ct_flag);							/* ��ת��־ */	
	sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
	WRITEMSG
	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) {return ret;}
					
	if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
	{
		sprintf(acErrMsg,"�ô���ǰ��Ҵ���![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L042" );
		return 1;
	}
	/* ���ƾ�������ת���˺���ҵ�� */
	ret= Ln_mst_hst_Sel( g_pub_tx.reply, &S_ln_mst_hst, "ac_id=%ld and tx_code='2498'",g_ln_mst.ac_id );
	if( ret!=100 )
	{
		sprintf(acErrMsg,"���ƾ�������ת���˺���ҵ��[%ld]",g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L043" );
		return ret;
	}
	return 0;
}

/* ��ӡ--ת�˽��˵�,�ֽ�ɿ */
int  print_LNDF()
{ 
    	char tx_type[9];

    	char filename[51],tmpname[101];
    	FILE *fp;
    
    	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNDF���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
	
	strcpy( tx_type , "���һ���" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.�����˺š�	     2.������	      3.���͡�    4.�ܻ����� 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}

/* ��ӡ--�ͻ��ص�--������ǰ�����ص� */
int  print_LNHDAJTQ( )	
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	long end_date =0;
	char exp_ind[5];
	  
    	char filename[51],tmpname[101];
    	FILE *fp;
 	TRACE   		
	TRACE
	pub_base_AllDwnFilName( tmpname );
	TRACE
	TRACE
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDAJTQ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
  	TRACE      	
	/* ��ֵ���� */
	strcpy( tx_type , "������ǰ����" );		

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
	sprintf( fine_rate , "%8.5lf��" , g_ln_mst.fine_rate);
	TRACE		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
	TRACE			
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.չ�ڱ�־�� 15.��ˮ��	*/
	fprintf(fp,"LNHDAJTQ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate ,exp_ind , g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}
/* ��ӡ--���ڴ�Ʊ--������ǰ�������� */
int print_LNBKAJBJ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	char exp_ind[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJBJ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������ǰ����" );	
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0152" );	/* �����Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	
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
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.�������ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.��ˮ�� */	
	fprintf(fp,"LNBKAJBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind,
	g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
