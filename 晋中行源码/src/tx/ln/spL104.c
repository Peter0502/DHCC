/*************************************************
* �� �� ��: spL104.c
* ����������   
*              ����ſ�
*
* ��    ��:    lance
* ������ڣ�   2003��01��14��
* �޸ļ�¼��   
*     1. ��    ��:20110826
*        �� �� ��:gongliangliang
*        �޸�����:��Ӹý�ݺ��Ƿ��е���Ѻ��Ʒ�Ĳ�ѯ
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
#include "ln_auth_c.h"
#include "ln_mst_c.h"
#include "mo_po_co_c.h"
#include "ln_gage_reg_c.h"
		
		char ct_flag[2];
		char de_ac_no[21];
		char de_ac_name[61];
spL104()  
{ 		
	int ret=0;
	char save_ac_no[20];
	
	struct mdm_ac_rel_c	mdm_ac_rel_tmp;
	struct ln_auth_c	ln_auth_tmp;
	struct ln_mst_c ln_mst_tmp;
	struct mo_po_co_c	sMo_po_co;
	struct ln_gage_reg_c	sLn_gage_reg;
	
	/** ���ݳ�ʼ�� **/
	memset( &mdm_ac_rel_tmp,0x00,sizeof(struct mdm_ac_rel_c) );
	memset( &ln_auth_tmp,0x00,sizeof(struct ln_auth_c) );	
	memset(ct_flag,0x00,sizeof(ct_flag));	
	memset(&ln_mst_tmp,0x00,sizeof(struct ln_mst_c));
	memset (&sMo_po_co, 0x00, sizeof(sMo_po_co));
	memset (&sLn_gage_reg, 0x00, sizeof(sLn_gage_reg));
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG	
		
	/** ȡֵ����ֵ **/
	if ( data_set_struct() )        ERR_DEAL;
	
	sprintf(acErrMsg,"ȡֵ����ֵ PASS!");
	WRITEMSG
	
	/*�Ըý�ݺ��Ƿ��е���Ѻ��Ʒ������У��*/
	/*add by gong 20110826 ������ҵ����*/
	if(memcmp(g_ln_parm.prdt_no,"3Z",2) != 0)
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&ln_mst_tmp,"ac_id in (select ac_id from mdm_ac_rel where ac_no='%s' )",g_pub_tx.ac_no);
		if(ret)
		{
			sprintf( acErrMsg, "��ѯ�������ļ������!ac_no[%s]ret=[%d]" ,g_pub_tx.ac_no ,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply, "B110" );
			goto ErrExit;
		}
		
		ret = Mo_po_co_Sel(g_pub_tx.reply,&sMo_po_co,"pact_no='%s'",ln_mst_tmp.pact_no);
		if(ret)
		{
			sprintf(acErrMsg,"��ѯ��ͬ��ݹ�ϵ�����!!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D107" );
			goto ErrExit;
		}
		
		ret = Ln_gage_reg_Sel(g_pub_tx.reply,&sLn_gage_reg,"pact_no='%s' ",sMo_po_co.contract_no);
		if(ret==0)
		{
			ret=sql_count("ln_gage_reg","sts in('X','x') and pact_no ='%s'",sLn_gage_reg.pact_no);
			if(ret>0)
			{
				memset(acErrMsg, 0 , sizeof(acErrMsg));
				sprintf( acErrMsg,"������%d�ʵ���Ѻ��Ʒ��Ҫ���",ret);
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L350");
				goto ErrExit;
			}
		}else if(ret&&ret!=100){
			vtcp_log("[%s][%d]���ݿ��ѯ��������",__FILE__,__LINE__);
			goto ErrExit;
		}
	}
	/*end by gong 20110826 ������ҵ����*/
	
	/** �������� **/
	/* ���Ҵ�����ظ��ſ� */
	/* ���Ҵ���Ž�����ͼƻ����Ž����ͬ */
	if ( g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4' )
	{
		ret = pub_base_CompDblVal(g_ln_mst.bal,0.00);
		if( ret!=0 )
		{
			sprintf( acErrMsg,"�˰��Ҵ����ѷ���,�����ظ�����",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L110" );
			ERR_DEAL
		}
		ret = pub_base_CompDblVal(g_ln_mst.amt_lmt,g_pub_tx.tx_amt1);
		if( ret!=0 )
		{
			sprintf( acErrMsg,"������,���Ҵ������һ���Է���",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L111" );
			ERR_DEAL
		}
	}						
	/* Pub_tx�ṹ��ֵ */

	strcpy ( g_pub_tx.add_ind , "1" );		/* ����:0��1��2���� */	

	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3010 ;
	strcpy( g_pub_tx.brf,"����ſ�" );
	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
	
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG

vtcp_log("JJJJ����ttl=[%f]",g_ln_mst.ttl_prvd_amt);
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG

	/* �ǼǴ���ǼǱ� */
	ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_pub_tx.tx_amt1,0.00,"0",0 );	
	if( ret )
	{
            sprintf(acErrMsg,"�ǼǴ���ǼǱ�ʧ��");
            WRITEMSG
            strcpy(g_pub_tx.reply,"L082");
            goto ErrExit;
        }						
	sprintf(acErrMsg,"�ǼǴ���ǼǱ� PASS!");
	WRITEMSG
	
vtcp_log("Jarod1: ct_flag=[%s] [%s][%d]",ct_flag,__FILE__,__LINE__);	
	set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */
vtcp_log("Jarod2: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	if( g_ln_mst.save_ac_id!=0)
	{
		if( ct_flag[0]!='2')
		{
            		sprintf(acErrMsg,"�����ʱ��ָ���ſ��˺�,ֻ����ת�˷ſ�!");
            		WRITEMSG
            		strcpy(g_pub_tx.reply,"L174");
            		goto ErrExit;				
		}
	}

	if( g_ln_parm.ln_type[0]!='9' )
	{
		/* ���ݴ���˻�IDȡ����˻� */
		ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth_tmp , "pact_no='%s'" , g_ln_mst.pact_no );
		if(ret)
		{
			sprintf(acErrMsg,"��ѯ���������Ǽǲ��쳣����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L181");
			goto ErrExit;
		}			
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &mdm_ac_rel_tmp , "ac_id=%ld and mdm_code='%s' and note_sts in( '0','1' )" , g_ln_mst.save_ac_id , ln_auth_tmp.save_mdm_code );
		if(ret==100)
		{
			sprintf(acErrMsg,"����˻�״̬������,�������Ļ�����ϵ!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L183");
			goto ErrExit;
		}else if(ret){						
			sprintf(acErrMsg,"��ѯ�����˺Ŷ��ձ��쳣����![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L182");
			goto ErrExit;	
		}		
		strcpy( save_ac_no , mdm_ac_rel_tmp.ac_no );	
		set_zd_data( "1061" , save_ac_no );
	}
	
	/* ��ӡ���� */
	TRACE
	/* ��ӡ--����ſ�跽��Ʊ */
	ret = print_LNFFJF( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNFFJF,ʧ������!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* ��ӡ--��ƾ֤(���ƽ��˵���ʽ) */
	ret = print_LNFFCK( );
	if(ret)		
	{
		sprintf(acErrMsg,"print_LNFF,ʧ������!" );
		WRITEMSG		
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE		
        if( g_ln_parm.ln_type[0]!='A' && memcmp(g_ln_parm.prdt_no,"3Z",2)!=0 )
        {
		ret = sql_execute2("update draw_loan_limit set sts='2' where pact_no='%s' and sts='1'" ,g_ln_mst.pact_no );
		if(ret)
		{
			sprintf(acErrMsg,"�������ݿ�ʧ��!" );
			WRITEMSG		
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}
        }
	
		pub_mob_sendmail("����ſ�",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,g_pub_tx.tx_amt1,0.0);

	
	if( ct_flag[0]=='1' )	   /*�ֽ�*/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*ת��*/
		goto ErrExit2;
	else if (ct_flag[0]=='3')  /*���*/
		goto ErrExit3;

ErrExit:
    sprintf(acErrMsg,"����ſ�ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
    
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
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����ʺ� */
	vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.ac_no,__FILE__,__LINE__);

	sprintf(acErrMsg,"�����ʺ�[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* �����˻���� */
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_data("0890",g_pub_tx.note_type);		/* ƾ֤���� */	
			vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.note_type,__FILE__,__LINE__);

	get_zd_data("0580",g_pub_tx.beg_note_no);	/* ƾ֤�� */
	vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.beg_note_no,__FILE__,__LINE__);
	
	get_zd_data("0580",g_pub_tx.end_note_no);	/* ƾ֤�� */
	vtcp_log("[%s]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.end_note_no,__FILE__,__LINE__);
	
	get_zd_double("0430",&g_pub_tx.tx_amt1);	/* ������ */
	vtcp_log("[%lf]aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.tx_amt1,__FILE__,__LINE__);

	vtcp_log("[%lf]aaaaaaavbtaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_pub_tx.tx_amt1,__FILE__,__LINE__);


	get_zd_data("0660",g_pub_tx.ct_ind);							/* ��ת��־ */
	get_zd_data("0660",ct_flag);							/* ��ת��־ */	
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
	WRITEMSG
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	get_zd_data("1011" , de_ac_no );
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	get_zd_data("1018" , de_ac_name );	
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	sprintf(acErrMsg,"����˺�![%s]����",de_ac_no,de_ac_name);
	WRITEMSG
		
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}
	vtcp_log("[%lf]aaaaaaavbtaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa[%s][%d]",g_ln_mst.bal,__FILE__,__LINE__);
vtcp_log("Jarod: ct_flag=[%s] [%s] [%d]",ct_flag,__FILE__,__LINE__);	
					
	return 0;
}

/* ��ӡ--����ſ�跽��Ʊ(��ӡ�ڽ�ݵ��������� )  */
int print_LNFFJF( )
{
	char tx_type[9];
    	char filename[51],tmpname[101];
    	FILE *fp;
	
	char rate[11];
	char over_rate[11];
	char fine_rate[11];
	char save_ac_no[21];
	char pay_ac_no[21];	
	
	long end_date=0;
	char exp_ind[5];
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	
	
	memset( &exp_ind ,0 , strlen(exp_ind) );
	
	
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNFFJF���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "����ſ�" );		
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0152" );	/* �����Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}
	
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

	sprintf( over_rate , "%8.5lf��" , g_ln_mst.over_rate);
	sprintf( fine_rate , "%8.5lf��" , g_ln_mst.fine_rate);
	
	pub_base_acid_acno( g_ln_mst.save_ac_id,g_pub_tx.ac_seqn,save_ac_no );
	pub_base_acid_acno( g_ln_mst.repay_ac_id,g_pub_tx.ac_seqn,pay_ac_no );
				
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	pub_base_strpack( fine_rate );
	pub_base_strpack( de_ac_no );
	pub_base_strpack( pay_ac_no );
	pub_base_strpack( ln_acc_hrt );	
					
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.�������ڡ�  8.�������ڡ� 9.������ 10.�ۼƷſ�     */
	/* 11.չ�ڱ�־�� 12.�������ʡ� 13.�������ʡ�14.��Ϣ����   15.�ſ��˺�   */
	/* 16.�����˺�   17.��Ŀ�š�   18.��ˮ��*/
	fprintf(fp,"LNFFJF%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.s|%s|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	g_pub_tx.tx_amt1 , g_ln_mst.opn_date , end_date , g_ln_mst.bal , g_ln_mst.ttl_prvd_amt ,
	exp_ind , rate , over_rate , fine_rate , de_ac_no , pay_ac_no ,ln_acc_hrt ,g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	

/* ��ӡ--��ƾ֤(����ת�˽��˵�) */
int  print_LNFFCK()
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
		sprintf(acErrMsg,"LNFFCK���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
	
	strcpy( tx_type , "����ſ�" );	
	
	pub_base_strpack( de_ac_no );
	pub_base_strpack( de_ac_name );
	pub_base_strpack( tx_type );
			
	/* 1.�����˺š�	     2.������	      3.���͡�    4.�ſ��� 	*/
	fprintf(fp,"LNFFCK%s|%s|%s|%.2lf|\n",de_ac_no , de_ac_name , tx_type , g_pub_tx.tx_amt1 );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}
