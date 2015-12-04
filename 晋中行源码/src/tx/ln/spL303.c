/*************************************************
* �� �� ��: spL303.c
* ����������   
*              ��������
*
* ��    ��:    lance
* ������ڣ�   2003��01��22��
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
#include "ln_reg_c.h"
#include "ln_lo_c.h"
#include "ln_auth_c.h"
 
static struct ln_reg_c		ln_reg;	
static struct ln_lo_c ln_lo;
double	print_tol_amt=0.00;

spL303()  
{ 	
	int ret=0;
	int flag=0;
	int pay_flag=0;
	char typ[2];
	double amt_tmp=0.00;
	
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
	
	/* ������ļ��н���Ƿ�Ϊ0 */	
	if ( !pub_base_CompDblVal(g_ln_mst.bal , 0.00) )
	{
		sprintf(acErrMsg,"�����Ϊ0,���ܺ�������![%s]",g_ln_parm.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L057" );
		ERR_DEAL
	}		
	/*** ��Ҫ�����κ����� ******/
	/* ������״̬�Ƿ�Ϊ���� 	*/
	if( g_ln_mst.ac_sts[0]!='4' )
	{
		sprintf(acErrMsg,"����״̬�Ǵ���,���ܺ���![%s]",g_ln_parm.ac_sts);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L056" );
		ERR_DEAL
	}
	vtcp_log("��������[%.2f]\n",g_ln_mst.bal);
	/* ����Ƿ������ȫ�黹���� */
	flag = pub_base_CompDblVal(g_ln_mst.bal , g_pub_tx.tx_amt1);
	if (flag==-1)	/* ��������� */ 
	{
		sprintf(acErrMsg,"����[%.2f],ʵ�����[%.2f]",g_pub_tx.tx_amt1,g_ln_mst.bal);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L039" );
		ERR_DEAL				
	}		

	
	/* ���´���Ǽǲ�(������ͨ����Ͱ��Ҵ���) */
	strcpy ( typ ,"1" );
	ret = pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,typ,
										g_pub_tx.tx_amt1,0.00,"0",1);
	if( ret )
	{
		sprintf(acErrMsg,"���´���Ǽǲ� PASS!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"���´���Ǽǲ� PASS!");
	WRITEMSG

	/* ���Ҵ��������Ҵ���Ƿ��� */ 
	if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{	
		ret = Ln_lo_Dec_Upd ( g_pub_tx.reply , 
			"ac_id=%ld and ac_seqn=%d  and pay_type='0' ", 
			g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "Ln_lo_Dec_Sel��!ret=[%d]", ret);
			WRITEMSG		
			goto ErrExit;	
		}
		sprintf( acErrMsg, "ac_id[%ld]ac_seqn=[%d]", g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		WRITEMSG				
		while(1)
		{
			ret = Ln_lo_Fet_Upd( &ln_lo, g_pub_tx.reply );
	   		if( ret && ret!=100 )
	 		{
		 		sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
		 		WRITEMSG
		 		goto ErrExit;
	 		}
			else if( ret==100 ){
				TRACE
				break;	
			}
			TRACE
		 	sprintf( acErrMsg,"���뻹Ƿ��֣�����");
		 	WRITEMSG
			/* �����--ֻ������Ƿ���� */
			amt_tmp = ln_lo.pay_lo_amt + g_pub_tx.tx_amt1 ;
			 
			/* �������ڹ黹���ں�,����ʣ�� */
			if ( pub_base_CompDblVal( ln_lo.lo_amt , amt_tmp ) == -1 )
			{
				g_pub_tx.tx_amt1= g_pub_tx.tx_amt1 - ( ln_lo.lo_amt - ln_lo.pay_lo_amt );
	
				ln_lo.pay_lo_amt = ln_lo.lo_amt; /* �ѹ黹��� */	
				sprintf( acErrMsg,"Ƿ����С��(�ѹ黹���+������)��ʣ����[%lf]",g_pub_tx.tx_amt1);
		 		WRITEMSG
			}else{	
				/* �պù黹���ڻ��߲��ܻ��屾�� */
				g_pub_tx.tx_amt1= 0.00;
	
				ln_lo.pay_lo_amt = ln_lo.pay_lo_amt+g_pub_tx.tx_amt1 ;/* �ѹ黹��� */	
				sprintf( acErrMsg,"Ƿ������ڵ���(�ѹ黹���+������)��ʣ����[%lf]",g_pub_tx.tx_amt1);
		 		WRITEMSG
		 	}					
	         
	                pay_flag=0;    /*�Ƿ����־*/
	                ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	                if( ret ) pay_flag++;  
	                ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	                if( ret ) pay_flag++;             
	                ret = pub_base_CompDblVal(ln_lo.pun_intst, ln_lo.pay_pun_intst );
	                if( ret ) pay_flag++;
	                if( pay_flag==0 )     
	                {
	            	    /* �޸��Ƿ����־ 0-δ���� 1-���� */
	            	    strcpy( ln_lo.pay_type , "1" );
	            	    flag++;
	                } 
	                ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
	                if ( ret )
	                {
	                     sprintf( acErrMsg,"Ln_lo_Upd_Upd ����[%d]",ret);
		             WRITEMSG
	                     break;
	                }	                
	   	
		}
		Ln_lo_Clo_Upd ();
		sprintf( acErrMsg,"Ƿ�����³ɹ�!!!!");
		WRITEMSG
	}	
	/*20110807 �������� */
	double sum=0.0;
	char m_sts[2]={'\0','\0'};
	sum=g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst;
	if(sum){
		strcpy(m_sts,"B");
	}else{
		strcpy(m_sts,"1");
	}
	ret=sql_execute(" update ln_auth set sts='%s' where pact_no='%s' and sts in ('X','L')  ",m_sts,g_ln_mst.pact_no);
	if(ret){
		sprintf( acErrMsg,"����ln_authʧ��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}


	/* Pub_tx�ṹ��ֵ */
	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
	strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
	strcpy ( g_pub_tx.brf,"��������" );
	g_pub_tx.svc_ind = 3010;			/* �������� */
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
			
	/* ��ӡ���� */
	/* ���ڴ�Ʊ--���𲿷� */
	ret = print_LNHXBJ( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHXBJ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	
	
OkExit:
    set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"��������[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"��������[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&ln_lo, 0x00, sizeof(struct ln_lo_c)); 	
	memset (&ln_reg, 0x00, sizeof(struct ln_reg_c));
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("1071",g_pub_tx.ac_no);			/* �ʺ� */
	get_zd_int("1072",&g_pub_tx.ac_seqn);		/* �˻���� */

	get_zd_double("107B",&g_pub_tx.tx_amt1);	/* ������� */
	print_tol_amt = g_pub_tx.tx_amt1;	
	vtcp_log("�������[%.2f]\n",g_pub_tx.tx_amt1);
	
	get_zd_data("107C",g_pub_tx.note_type);		/* ƾ֤���� */
	get_zd_data("107D",g_pub_tx.beg_note_no);	/* ��ʼƾ֤���� */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */
					
	return 0;
}

/* ��ӡ--���������Ʊ--���𲿷� */
int print_LNHXBJ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
		
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
		sprintf(acErrMsg,"LNHXBJ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "�������" );	
	
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
	/*  6.���׽�  7.�������ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.�������ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.��ˮ��  */
	fprintf(fp,"LNHXBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
