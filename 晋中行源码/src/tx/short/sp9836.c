/*************************************************
* �� �� ��:    sp9836.c
* ����������   
*              ���ݴ����˺�ȡ���˻����
*
* ��    ��:    lance
* ������ڣ�   2003��01��25��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
	
sp9836()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn,back_ac_seqn;
	int flag=0, redflag=0;
	char tx_code[8];
	
	
	struct ln_mst_c		ln_mst_tmp;
	struct cif_basic_inf_c        cif_basic_inf; 
	struct cif_id_code_rel_c      cif_id_code_rel; 
		
	/** ���ݳ�ʼ�� **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset (&cif_basic_inf, 0x00, sizeof(struct cif_basic_inf_c));	
	memset (&cif_id_code_rel, 0x00, sizeof(struct cif_id_code_rel_c));	
	
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0310" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("0310",ac_no);	/* ������ʺ� */	
	
	get_zd_data( "0650" , tx_code ); /* ���״��� */
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " , ac_no );			
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	} else if( ret !=0 )
		{
    		sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}
	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]",g_mdm_ac_rel.ac_id);
	WRITEMSG	
	
	/* ���ݴ����˻�ID��ѯ�������ļ����� */
	flag = sql_count("ln_mst", " ac_id=%ld " , g_mdm_ac_rel.ac_id);
	sprintf(acErrMsg,"���ݴ����˻�ID��ѯ�������ļ�����[%d]",flag);
	WRITEMSG	
		
	if( flag==0 )
   	 {
 		set_zd_int( "0660", redflag );	/* �Ƿ�������ű�־ */   
 		sprintf(acErrMsg,"�������ļ������޴˼�¼[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L114");
 		goto ErrExit;
   	 }	

	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , " ac_id=%ld " , g_mdm_ac_rel.ac_id );			
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL

		} else if( ret !=0 )
			{
    				sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%s]",ret);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"D103" );
    				ERR_DEAL
    			}	
    			
    	sprintf(acErrMsg,"ȡ�������ļ�PASS");
	WRITEMSG 
	
	
	/* �ǿ��Ͽ���ר��ͨ�� */

	
	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(ac_no,ln_mst_tmp.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("aaaaaaaaaaaaaaa[%ld]\n",g_ln_mst.cif_no);
	long tmpcif_no=0;
	tmpcif_no=g_ln_mst.cif_no;
	vtcp_log("bbbbbbbbbbbbbbbb[%ld]\n",tmpcif_no);
	if( strcmp( tx_code , "3308" )==0 )	
	{
			sprintf(acErrMsg,"����3308!");
			WRITEMSG

			/* ���ݿͻ��Ų�ѯ�ͻ���Ϣ */		  
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
							" cif_no=%ld " , g_ln_mst.cif_no );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
	
			/* ���ݿͻ��Ų�ѯ�ͻ�֤����Ϣ */		  
			ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
							" cif_no=%ld " , g_ln_mst.cif_no );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C004");
				goto ErrExit;
			}						  
	
			/* ��ѯ�����˻���Ϣ */
			ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &back_ac_seqn );
			if( ret ) {return ret;}	
			if( g_ln_mst.repay_ac_id !=0 )
			{
				ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,back_ac_seqn, &g_dd_mst );
				if( ret ) {return ret;}

				Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
								" ac_id=%ld " , g_ln_mst.repay_ac_id );
				if( ret ) {return ret;}	
			}
	
			/* ������� */
			set_zd_data( "0270", g_ln_mst.name );/* ����� */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* ��Ʒ��� */
			set_zd_data( "0810", g_ln_parm.title );	/* ��Ʒ���� */
			set_zd_data( "0630", g_ln_mst.pact_no );	/* ��ͬ�� */	
			set_zd_data( "0610", g_ln_mst.cal_code );	/* ָ����ϵ���� */		
			set_zd_data( "0210", g_ln_parm.cur_no );/* ���� */
			set_zd_data( "0680", g_ln_parm.get_rate_type );/* ����ȡ�÷�ʽ */	
			set_zd_double( "0840", g_ln_mst.rate );	/* �������� */
			set_zd_double( "0850", g_ln_mst.over_rate );	/* �������� */
			set_zd_double( "0940", g_ln_mst.fine_rate );	/* ��Ϣ���� */					
			set_zd_data( "0690", g_ln_parm.ln_pay_type );/* ����֧����ʽ */	
			set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* �Զ������־ */	
			set_zd_data( "0710", g_ln_mst.intst_type );/* ��Ϣ���� */
			set_zd_long( "0440", g_ln_mst.opn_date );	/* �������� */	
			set_zd_long( "0450", g_ln_mst.mtr_date );	/* �������� */		
			set_zd_double( "0430", g_ln_mst.amt_lmt );	/* �ƻ����Ŷ� */
			set_zd_data( "0250", cif_basic_inf.name );	/* �ͻ����� */	
			set_zd_data( "0670", cif_id_code_rel.id_type );	/* ֤������ */		
			set_zd_data( "0620", cif_id_code_rel.id_no );	/* ֤������ */		
			set_zd_double( "0560", g_ln_mst.intst_acm );	/* ������� */					
			set_zd_data( "0930", g_ln_mst.cmpd_intst_ind );/* ������־ */
			if( g_ln_mst.repay_ac_id !=0 )
			{
				set_zd_long ( "0340", back_ac_seqn );	/* �����˻���� */
				set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* �����˻� */	
				set_zd_data( "0260", g_dd_mst.name );	/* �����˻����� */
			}
		
	}			

	if( strcmp( tx_code , "3105" )==0 )	
	{
			sprintf(acErrMsg,"����3105!");
			WRITEMSG
			/**
			if( g_ln_mst.ac_sts[0]!='8' )
			{
				sprintf(acErrMsg,"�˴���״̬�Ǵ����״̬");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L116");
				goto ErrExit;
			}				
			**/
			/* ���ݿͻ��Ų�ѯ�ͻ���Ϣ */		  
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
							" cif_no=%ld " , g_ln_mst.cif_no );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
	
			/* ���ݿͻ��Ų�ѯ�ͻ�֤����Ϣ */		  
			ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
							" cif_no=%ld " , g_ln_mst.cif_no );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C004");
				goto ErrExit;
			}						  
	
			/* ��ѯ�����˻���Ϣ */
			ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &back_ac_seqn );
			if( ret ) {return ret;}	
			if( g_ln_mst.repay_ac_id !=0 )
			{
				ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,back_ac_seqn, &g_dd_mst );
				if( ret ) {return ret;}

				Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , 
								" ac_id=%ld " , g_ln_mst.repay_ac_id );
				if( ret ) {return ret;}	
			}
	
			/* ������� */
			set_zd_data( "0270", g_ln_mst.name );/* ����� */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* ��Ʒ��� */
			set_zd_data( "0810", g_ln_parm.title );	/* ��Ʒ���� */
			set_zd_data( "0630", g_ln_mst.pact_no );	/* ��ͬ�� */	
			set_zd_data( "0610", g_ln_mst.cal_code );	/* ָ����ϵ���� */		
			set_zd_data( "0210", g_ln_parm.cur_no );/* ���� */
			set_zd_data( "0680", g_ln_parm.get_rate_type );/* ����ȡ�÷�ʽ */	
			set_zd_double( "0840", g_ln_mst.rate );	/* �������� */
			set_zd_double( "0850", g_ln_mst.over_rate );	/* �������� */		
			set_zd_data( "0690", g_ln_parm.ln_pay_type );/* ����֧����ʽ */	
			set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* �Զ������־ */	
			set_zd_data( "0710", g_ln_parm.intst_type );/* ��Ϣ���� */
			set_zd_long( "0440", g_ln_mst.opn_date );	/* �������� */	
			set_zd_long( "0450", g_ln_mst.mtr_date );	/* �������� */		
			set_zd_double( "0430", g_ln_mst.amt_lmt );	/* �ƻ����Ŷ� */
			set_zd_data( "0250", cif_basic_inf.name );	/* �ͻ����� */	
			set_zd_data( "0670", cif_id_code_rel.id_type );	/* ֤������ */		
			set_zd_data( "0620", cif_id_code_rel.id_no );	/* ֤������ */		
			if( g_ln_mst.repay_ac_id !=0 )
			{
				set_zd_long ( "0340", back_ac_seqn );	/* �����˻���� */
				set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* �����˻� */	
				set_zd_data( "0260", g_dd_mst.name );	/* �����˻����� */
			}	
	}
	if( strcmp( tx_code , "3309" )==0 )	
	{
			sprintf(acErrMsg,"����3309!");
			WRITEMSG
			/**
			if( g_ln_mst.ac_sts[0]!='8' )
			{
				sprintf(acErrMsg,"�˴���״̬�Ǵ����״̬");
				WRITEMSG
				strcpy(g_pub_tx.reply,"L116");
				goto ErrExit;
			}				
			**/
			/* ���ݿͻ��Ų�ѯ�ͻ���Ϣ */		  
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
							" cif_no=%ld " , g_ln_mst.cif_no );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ�������Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C006");
				goto ErrExit;
			}
	
			/* ���ݿͻ��Ų�ѯ�ͻ�֤����Ϣ */		  
			ret = Cif_id_code_rel_Sel( g_pub_tx.reply , &cif_id_code_rel , 
							" cif_no=%ld " , g_ln_mst.cif_no );
			if( ret==100 )
			{
				sprintf(acErrMsg,"�ÿͻ��Ų�����[%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C007");
				goto ErrExit;
			}
			else if( ret )
			{
				sprintf(acErrMsg,"ȡ�ͻ�֤����Ϣ�쳣![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"C004");
				goto ErrExit;
			}						  
			/* �жϴ����Ƿ��ǰ��Ҵ��� */
			if( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
			{
				sprintf(acErrMsg,"�˴���ǰ��Ҵ���![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L042");
				goto ErrExit;				
			}	
			/* ������� */
			set_zd_data( "0270", g_ln_mst.name );/* ����� */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* ��Ʒ��� */
			set_zd_data( "0810", g_ln_parm.title );	/* ��Ʒ���� */
			set_zd_data( "0630", g_ln_mst.pact_no );	/* ��ͬ�� */	
			set_zd_data( "0610", g_ln_mst.cal_code );	/* ָ����ϵ���� */		
			set_zd_data( "0210", g_ln_parm.cur_no );/* ���� */
			set_zd_data( "0680", g_ln_parm.get_rate_type );/* ����ȡ�÷�ʽ */	
			set_zd_double( "0840", g_ln_mst.rate );	/* �������� */
			set_zd_double( "0850", g_ln_mst.over_rate );	/* �������� */		
			set_zd_data( "0690", g_ln_parm.ln_pay_type );/* ����֧����ʽ */	
			set_zd_data( "0700", g_ln_parm.auto_pay_ind );/* �Զ������־ */	
			set_zd_data( "0710", g_ln_parm.intst_type );/* ��Ϣ���� */
			set_zd_long( "0440", g_ln_mst.opn_date );	/* �������� */	
			set_zd_long( "0450", g_ln_mst.mtr_date );	/* �������� */		
			set_zd_double( "0430", g_ln_mst.amt_lmt );	/* �ƻ����Ŷ� */
			set_zd_data( "0250", cif_basic_inf.name );	/* �ͻ����� */	
			set_zd_data( "0670", cif_id_code_rel.id_type );	/* ֤������ */		
			set_zd_data( "0620", cif_id_code_rel.id_no );	/* ֤������ */		
			if( g_ln_mst.repay_ac_id !=0 )
			{
				set_zd_long ( "0340", back_ac_seqn );	/* �����˻���� */
				set_zd_data( "0380", g_mdm_ac_rel.ac_no );	/* �����˻� */	
				set_zd_data( "0260", g_dd_mst.name );	/* �����˻����� */
			}	
	}					
	if( strcmp( tx_code , "3312" )==0 )	
	{
			sprintf(acErrMsg,"����3312!");
			WRITEMSG
			
			/* del by lance 20031115
			if( g_ln_mst.ac_sts[0]!='1' )
			{
				sprintf(acErrMsg,"�����ʻ�״̬������,�������˽���![%s]",g_pub_tx.reply);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L137");
				goto ErrExit;
			}			
			******/
					
			/* ������� */
			set_zd_data( "0250", g_ln_mst.name );/* ����� */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* ��Ʒ��� */
			set_zd_data( "0260", g_ln_parm.title );	/* ��Ʒ���� */
			set_zd_data( "0670", g_ln_parm.ln_type );	/* ���ʽ */	
			set_zd_data( "0680", g_ln_parm.cif_type );	/* �ͻ����� */	
			set_zd_data( "0690", g_ln_parm.time_type );	/* ��Ʒʱ�� */	
			set_zd_double( "0410", g_ln_mst.gage_amt );	/* ��ѺƷ��� */
			set_zd_long( "0440", g_ln_mst.mtr_date );	/* �������� */
			set_zd_long( "0460", g_ln_mst.opn_date );	/* �������� */												
	}
	if( strcmp( tx_code , "3322" )==0 )	
	{
			sprintf(acErrMsg,"����3322!");
			WRITEMSG
			
		
					
			/* ������� */
			set_zd_data( "0250", g_ln_mst.name );/* ����� */
			set_zd_data( "0230", g_ln_mst.prdt_no );/* ��Ʒ��� */
			set_zd_data( "0260", g_ln_parm.title );	/* ��Ʒ���� */
			set_zd_data( "0670", g_ln_parm.ln_type );	/* ���ʽ */	
			set_zd_data( "0680", g_ln_parm.cif_type );	/* �ͻ����� */	
			set_zd_data( "0690", g_ln_parm.time_type );	/* ��Ʒʱ�� */	
			set_zd_double( "0410", g_ln_mst.bal );	/* ������� */
			set_zd_long( "0440", g_ln_mst.mtr_date );	/* �������� */
			set_zd_long( "0460", g_ln_mst.opn_date );	/* �������� */												
	}
		
	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	redflag=1;
	set_zd_long( "0350", ln_mst_tmp.ac_seqn );	/* �����˻���� */
	
	set_zd_int( "0660", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/    	
    	goto OkExit;
  }
    	
    	
  	redflag=9;
	set_zd_int( "0660", redflag );	/* �Ƿ�������ű�־ 9-������¼*/ 	 		
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
