/*************************************************
* �� �� ��: spL201.c
* ����������   
*              �����
*
* ��    ��:    lance
* ������ڣ�   2003��01��19��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define EXTERN
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#include "public.h"
#include "ln_reg_c.h"
#include "mo_infrm_c.h"

static struct ln_reg_c		ln_reg;	
static struct mo_infrm_c	mo_infrm;

	char repay_ind[2];
	char ct_flag[2];
	double print_tol_amt = 0.00;
		
spL201()  
{ 	
	int ret=0;
	double acm_intst=0.0;	/* ���������Ϣ */
	int flag;			/* ����/�����־ */
	char flag_typ[2];
	int day_flag=0;		/* ��ǰ�����־ */
	long note_day;		/* ֪ͨ���� */
	char typ[2];
	double vbjlx,vbnlx,vbylx,vfllx;
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
		
	/* �ж��Ƿ������������ */
	/* del in ��ɳ����
	if( repay_ind[0]=='L' )
	{
			vbjlx=vbnlx=vbylx=vfllx=0.00;
			TRACE
			
			* ����鱾����Ϣ 
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
			if( ret ) { ERR_DEAL }
			**	
			
			TRACE		
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
			if( ret ) { ERR_DEAL }	
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
			if( ret ) { ERR_DEAL }	
			ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
			if( ret ) { ERR_DEAL }		
			
		  ret = pub_base_CompDblVal(vbjlx+vbnlx+vbylx+vfllx+g_ln_mst.in_lo_intst+g_ln_mst.out_lo_intst+g_ln_mst.cmpd_lo_intst, 0.00 );
		  if( ret>0 )
	   	  {
				sprintf(acErrMsg,"������ǷϢδ����,��������������!");
				WRITEMSG
				strcpy( g_pub_tx.reply,"L117" );
				goto ErrExit;	   
		  } 	
	}
	*******************/
	
	if( g_pub_tx.tmp_ind[0]=='2' ) /* ���汾�� */
	{
		/* �������������Ϣ */
		ret = pub_ln_CalIntst ( g_ln_mst , g_ln_parm , "0" , &acm_intst , 0 );
		if( ret ) goto ErrExit;
			
		sprintf(acErrMsg,"���[%.2lf] [%.2lf]",g_pub_tx.tmp_amt1,acm_intst );
		WRITEMSG

		/* �������Ƿ���ڱ�����Ϣ��� */
		ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1, acm_intst );
		if( ret<0 )
		{
			sprintf(acErrMsg,"����С[%.2lf] [%.2lf]",g_pub_tx.tmp_amt1 ,
								acm_intst );
			WRITEMSG
			strcpy( g_pub_tx.reply,"L040" );
			goto ErrExit;
		}
	
		/* �ǼǴ���Ǽǲ� */
		/* �������Ϣ�����ж� */
		flag = pub_ln_GetIntstTyp ( g_ln_mst , g_ln_parm );
		sprintf( flag_typ , "%d" , flag ); 
		
		ret = pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,flag_typ,acm_intst,g_ln_mst.intst_acm,"0",2);
		if( ret ) goto ErrExit;
		
		g_pub_tx.tx_amt1= acm_intst;	/* �ɹ黹��Ϣ��� */	

		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
		g_pub_tx.svc_ind=3011;
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		strcpy( g_pub_tx.brf,"������˰����" );

		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}

		sprintf(acErrMsg,"�ǼǴ���Ǽǲ��ɹ� PASS!");
		WRITEMSG

		g_pub_tx.tmp_amt1 -= g_pub_tx.tx_amt1;
	}

	/* ������С�ڴ�������ͱ�����Ϣ֮�� */
	ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,g_ln_mst.bal);
	if( ret>0 ) 
	{
		sprintf(acErrMsg,"��������ڴ�������[%.2lf] [%.2lf] ac_id[%ld]",g_pub_tx.tmp_amt1,
			g_ln_mst.bal ,g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L039" );
		goto ErrExit;
	}
	sprintf(acErrMsg,"������С�ڴ������� PASS!");
	WRITEMSG

	if( pub_base_CompDblVal(g_pub_tx.tmp_amt1,0.00)>0 )
	{
		/* ��ǰ������ */
		if ( g_ln_mst.mtr_date > g_pub_tx.tx_date )
		{
			/* �����ǰ�����޶� */ 	
			if ( pub_base_CompDblVal(g_ln_parm.fdrq_lmt_amt,0.00)>0 )
			{
			  if ( pub_base_CompDblVal(g_ln_parm.fdrq_lmt_amt,g_ln_mst.bal)<0 )
			  {
					ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,
												g_ln_parm.fdrq_lmt_amt );
					if( ret<0 )
					{
						sprintf(acErrMsg,"���С����ǰ�����޶�[%.2lf] [%.2lf]" ,
										g_pub_tx.tmp_amt1,g_ln_parm.fdrq_lmt_amt );
						WRITEMSG
						strcpy( g_pub_tx.reply,"L040" );
						goto ErrExit;
					}
			  }
			}
			sprintf(acErrMsg,"�����ǰ�����޶� PASS!");
			WRITEMSG
					sprintf(acErrMsg,"֪ͨ����[%s]",
									g_ln_parm.fdrp_inf_days );			
			sprintf(acErrMsg,"�����ǰ�����޶� PASS!");
			WRITEMSG
						
			/* ��ǰ������ǰ֪ͨ���� */
			if ( g_ln_parm.fdrp_inf_days )
			{
				/* ȡ֪ͨ���� */
				note_day = pub_base_daynumSUB(g_pub_tx.tx_date,
												g_ln_parm.fdrp_inf_days);
				
				ret = Mo_infrm_Dec_Upd(g_pub_tx.reply, 
						" ac_id=%ld and ac_seqn=%d and infrm_date=%ld and \
							prm_draw_date=%ld and sts='0' " , g_ln_mst.ac_id,
							g_ln_mst.ac_seqn,note_day,g_pub_tx.tx_date );
				if ( ret )
				{
					sprintf( acErrMsg, "ִ��Mo_infrm_Dec_Upd��!ret=[%d]",ret );
					WRITEMSG
					return(-1);
				}
				ret = Mo_infrm_Fet_Upd( &mo_infrm, g_pub_tx.reply );
				if( ret==100)
				{
					sprintf(acErrMsg,"��ǰ������֪ͨ,֪ͨ�ǼǱ�������Ӧ��¼"
								,g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O008");
					ERR_DEAL;
				}else if ( ret )
				{
					sprintf(acErrMsg,"ȡ֪ͨ�ǼǱ���Ϣ�쳣",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O009");
					ERR_DEAL;
				}
				
				/* �Ƚ�֪ͨ�Ļ���Ľ�������Ҫ���Ľ�� */	
				ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,
											mo_infrm.prm_draw_amt );
				if ( !ret )
				{
					sprintf(acErrMsg,"��������֪ͨ����",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O010");
					ERR_DEAL;				
				}	
							
				strcpy( mo_infrm.sts , "2" );			/* �޸�״̬:����֧ȡ */
				mo_infrm.occ_date = g_pub_tx.tx_date;	/* ʵ�ʷ������� */
				mo_infrm.fct_wd_amt = g_pub_tx.tmp_amt1;/* ʵ��ȡ���� */				
				ret = Mo_infrm_Upd_Upd( mo_infrm, g_pub_tx.reply  );
				if ( ret )
				{
					sprintf( acErrMsg,"Mo_infrm_Upd_Upd ����[%d]",ret);
					WRITEMSG
					return 1;
				}		
	
				Mo_infrm_Clo_Upd( );
				sprintf(acErrMsg,"����֪ͨ�ǼǱ� PASS");
				WRITEMSG
			}
			sprintf(acErrMsg,"��ǰ������ǰ֪ͨ���� PASS!");
			WRITEMSG
			
			/* ???�Ƿ�����շѼ��ʹ��� */
		}
		
		g_pub_tx.tx_amt1 = g_pub_tx.tmp_amt1 ;
		
		sprintf(acErrMsg,"�ж��Ƿ�����ǰ���� PASS!");
		WRITEMSG				

		/* ���´���Ǽǲ� */
		strcpy ( typ ,"1");
		ret = pub_ln_ln_reg(g_ln_mst,g_ln_mst.lst_date,g_pub_tx.tx_date,typ,g_pub_tx.tx_amt1,0.00,"0",1);
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"���´���Ǽǲ� PASS!");
		WRITEMSG

		/* Pub_tx�ṹ��ֵ */
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.intst_type,"1" );	/* ��Ϣ���� */	
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
		g_pub_tx.svc_ind=3010;
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		strcpy( g_pub_tx.brf,"�������" );

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
	
	/* ��� */
	/* ����Ƿ��� */
	ret = pub_ln_chk_clear(g_ln_mst,g_ln_parm,clear_sts);
	if(ret)	goto ErrExit;
	if( pub_base_CompDblVal( g_ln_mst.gage_amt , 0.00 ) )
	{			
		set_zd_data("107J",clear_sts);
		/* �����е�ѺƷ */
	}else{
		set_zd_data("107J","*");
		/* �����е�ѺƷ */
	}	
	
	set_zd_double("107E",acm_intst); /*��Ϣ���*/
	set_zd_double("107F",g_pub_tx.tx_amt1); /*�������*/
	g_pub_tx.tx_amt1=g_pub_tx.tx_amt1+acm_intst;
	set_zd_double("107H",g_pub_tx.tx_amt1);	/*�ϼ�*/
	set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);
	
	sprintf(acErrMsg,"��� PASS!g_pub_tx.tx_amt1[%lf],acm_intst[%lf]",
							g_pub_tx.tx_amt1,acm_intst);
	WRITEMSG	


	set_zd_data("0210",g_ln_parm.cur_no);/* �Ա��ָ�ֵ */
	
	/* ��ӡ���� */
	
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
	
	/* �ͻ��ص�--����ص� */
	ret = print_LNHDBJ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDBJ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE
	
	/* ���ڴ�Ʊ--���𲿷� */
	ret = print_LNBKBJ( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKBJ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE	

/*pub_mob_sendmail_ln("����黹",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);	*/	
pub_mob_sendmail_ln("����黹",g_pub_tx.tx_code,g_ln_mst.pact_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);			

	if( ct_flag[0]=='1' )	/** �ֽ� **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*ת��*/
		goto ErrExit2;
	else if (ct_flag[0] == '3')		/*���*/
        goto ErrExit3;
    else if (ct_flag[0] == '9')	/* ���ֻ��� */
        goto GoodExit;
    else if (ct_flag[0] == '8')	/* �ۻ� */
        goto ErrExit8;       
ErrExit:
	strcpy(g_pub_tx.reply,"L153");
    sprintf(acErrMsg,"�����ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"������ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
ErrExit8:
    strcpy(g_pub_tx.reply,"0008");
    sprintf(acErrMsg,"������ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
GoodExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"������ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	
	memset (&ln_reg, 0x00, sizeof(struct ln_reg_c));
	memset (&mo_infrm, 0x00, sizeof(struct mo_infrm_c));
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("1071",g_pub_tx.ac_no);			/* �ʺ� */
	get_zd_int("1072",&g_pub_tx.ac_seqn);		/* �˻���� */
	get_zd_data("107A",g_pub_tx.tmp_ind);		/* ���汾�� */
	get_zd_double("107B",&g_pub_tx.tmp_amt1);	/* �������� */
	print_tol_amt = g_pub_tx.tmp_amt1;
	get_zd_data("107C",g_pub_tx.note_type);		/* ƾ֤���� */
	get_zd_data("107D",g_pub_tx.beg_note_no);	/* ��ʼƾ֤���� */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );
	get_zd_data("107I",repay_ind);

	get_zd_data("0660",g_pub_tx.ct_ind);			/* ��ת��־ */
	get_zd_data("0660",ct_flag);				/* ��ת��־ */	
	if( g_pub_tx.ct_ind[0]=='9' )
	{
		strcpy( g_pub_tx.ct_ind , "2" );
	}
	sprintf(acErrMsg,"��ת��־![%s]",g_pub_tx.ct_ind);
	WRITEMSG
	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}

	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */

	/*ret = pub_base_check_mdm_sts( g_mdm_ac_rel ) *������״̬*
	if( ret ) return ret; */

	ret = pub_ln_check_sts( g_ln_mst,0 ); /*�����״̬*/
	if( ret ) {return ret;}
					
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"���Ҵ���ڴ˴���![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L038" );
		return 1;
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
	
	strcpy( tx_type , "�����" );	
	
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( tx_type );
			
	/* 1.�����˺š�	     2.������	      3.���͡�    4.�ܻ����� 	*/
	fprintf(fp,"LNDF%s|%s|%s|%.2lf|\n",g_pub_tx.ac_no , g_ln_mst.name , tx_type , print_tol_amt );	
	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
		
	return 0;
}

/* ��ӡ--�ͻ��ص�--����ص� */
int  print_LNHDBJ( )	
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	long end_date =0;
	char exp_ind[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
	TRACE
	TRACE
 	TRACE   		
	pub_base_AllDwnFilName( tmpname );
	TRACE
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDBJ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
  	TRACE      	
	/* ��ֵ���� */
	strcpy( tx_type , "�����" );		

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
	TRACE		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
	TRACE			
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.�������ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.�������ʡ�14.չ�ڱ�־�� 15.��ˮ��	*/
	fprintf(fp,"LNHDBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate ,exp_ind ,g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}

/* ��ӡ--���ڴ�Ʊ--���𲿷� */
int print_LNBKBJ( )
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
		sprintf(acErrMsg,"LNBKBJ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "�����" );	
	
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
	fprintf(fp,"LNBKBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
