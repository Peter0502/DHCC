/*************************************************
* �� �� ��: spL315.c
* ����������   
*              ��������벹��
*
* ��    ��:    lance
* ������ڣ�   2003��06��13��
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

	char	 cb_flag[2];
	char	 dc_flag[2];
	char	 r_str[7];
	char	 bal_dc_ind[2];
	char	 in_dc_ind[2];
	char 	 out_dc_ind[2];
	char	 cmpd_dc_ind[2];

static  double	 bal=0.00,bal_acm=0.00;
static  double	 in_lo_intst=0.00,in_acm=0.00;
static  double	 out_lo_intst=0.00,out_acm=0.00;
static  double	 cmpd_lo_intst=0.00,cmpd_acm=0.00;
static  double	 gage_amt=0.00;
		
spL315()  
{ 	
	int ret=0;
	double acm=0.00;
	char acm_type[2];
		
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
	
	/* ���Ƶ��ղ��������岹���� */
	if( g_pub_tx.tx_date == g_pub_tx.erro_date )
	{
	        sprintf(acErrMsg,"���մ��˲������岹����,������������!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L158");
			goto ErrExit;
	}
	
	/* ȡ������Ϣ */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}

	/* ���Ҵ�������岹���� */
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"���Ҵ�������������ǽ���!");
		WRITEMSG
		strcpy( g_pub_tx.reply , "L223" );
		goto ErrExit;
	}		
	sprintf(acErrMsg,"ac_id[%ld] [%d] [%lf]",g_ln_mst.ac_id,g_ln_mst.ac_seqn,g_ln_mst.ttl_prvd_amt );
	WRITEMSG	
   	sprintf(acErrMsg,"1��־����cb_flag[%s]",cb_flag);
    	WRITEMSG	
	set_zd_data("0210","01");	/* ���� */

	strcpy ( g_pub_tx.ac_get_ind,"111" );  /*�Ѿ���ȡ��mdm_ac_rel, g_ln_mst*/
	/* �������岹 */
	if( pub_base_CompDblVal( bal , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* �������  */
		strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			bal ,g_pub_tx.tx_amt1 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"���ú����������(�������)PASSΪ [%lf]",acm );
		WRITEMSG	
		bal_acm = acm;	
		/****
		if( pub_base_CompDblVal(g_ln_mst.ttl_prvd_amt,0.00)==0 )
		****/
		if( pub_base_CompDblVal(g_ln_mst.amt_lmt,0.00)==0 )
		{
			sprintf(acErrMsg,"������δ�ſ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"L155");
			goto ErrExit;				
		}
		TRACE
		if( cb_flag[0]=='1' )	/* ���� */
		{
			g_pub_tx.tx_amt1 = -bal ;			
		}
		if( cb_flag[0]=='2' )	/* ���� */
		{
			g_pub_tx.tx_amt1 = bal ;
		}
		
		set_zd_double( "1084" , g_pub_tx.tx_amt1 );
		set_zd_data( "0210" , "01" );	
			
		if( g_pub_tx.add_ind[0]=='0' )	/* ��(�岹----����ſ�) ԭΪ 1*/
		{
			set_zd_data( "0660" , "2" ); /* �� ԭΪ1*/
			strcpy( bal_dc_ind, "2" );
			g_pub_tx.svc_ind = 3091 ;/*3091*/
		}
		if( g_pub_tx.add_ind[0]=='1' )	/* ��(�岹----�����)ԭΪ0 */
		{
			set_zd_data( "0660" , "1" ); /* �� ԭΪ2*/;
			strcpy( bal_dc_ind, "1" );
			g_pub_tx.svc_ind = 3091	;	/*3019*/	
		}
		
		/* �岹���� */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "1" , acm );
		if( ret ) 	goto ErrExit;
		
			
		/* Pub_tx�ṹ��ֵ */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*�Ѿ���ȡ��mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		/* �Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"��������" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"����𲹼�" );
		}			
		sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
		WRITEMSG
			sprintf(acErrMsg,"��(�������)PASSΪ [%lf]",g_ln_mst.intst_acm );
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
			sprintf(acErrMsg,"��(�������)PASSΪ [%lf]",g_ln_mst.intst_acm );
			WRITEMSG
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;
		TRACE
   	sprintf(acErrMsg,"2��־����cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "1" , bal );
		if( ret ) 	goto ErrExit;
		TRACE		
	}
	
   	sprintf(acErrMsg,"2��־����cb_flag[%s]",cb_flag);
    	WRITEMSG	
	/* �������ǷϢ���岹 */
	if( pub_base_CompDblVal( in_lo_intst , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* �������  */
		strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			in_lo_intst ,g_pub_tx.tx_amt2 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"���ú����������(���ڻ���)PASSΪ [%lf]",acm );
		WRITEMSG
		
		in_acm = acm;
		
		if( cb_flag[0]=='1' )	/* ���� */
		{
			g_pub_tx.tx_amt2 = -in_lo_intst ;
		}
		if( cb_flag[0]=='2' )	/* ���� */
		{
			g_pub_tx.tx_amt2 = in_lo_intst ;
		}
		
		set_zd_double( "1085" , g_pub_tx.tx_amt2 );
		set_zd_data( "0210" , "01" );	
			
		if( g_pub_tx.add_ind[0]=='1' )	/* ��(�岹----�����Ϣ) */
		{
			set_zd_data( "0690" , "1" ); /* �� */
			strcpy( in_dc_ind, "1" );
			g_pub_tx.svc_ind = 3020 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	/* ��(�岹----���Ϣ) */
		{
			set_zd_data( "0690" , "2" ); /* �� */;
			strcpy( in_dc_ind, "2" );
			g_pub_tx.svc_ind = 3020	;		
		}
		
		/* �岹���� */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "2" , acm );
		if( ret ) 	goto ErrExit;
					
		/* Pub_tx�ṹ��ֵ */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*�Ѿ���ȡ��mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */		
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		/* �Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"�������ǷϢ����" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"�������ǷϢ����" );
		}			
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
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;		
		TRACE
	   	sprintf(acErrMsg,"2��־����cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "2" , in_lo_intst );
		if( ret ) 	goto ErrExit;
		TRACE	
	}
   	sprintf(acErrMsg,"3��־����cb_flag[%s]",cb_flag);
    	WRITEMSG	

	/* �������ǷϢ���岹 */
	if( pub_base_CompDblVal( out_lo_intst , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* �������  */
		strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			out_lo_intst ,g_pub_tx.tx_amt3 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"���ú����������(�������)PASSΪ [%lf]",acm );
		WRITEMSG
		
		out_acm = acm;
				
		if( cb_flag[0]=='1' )	/* ���� */
		{
			g_pub_tx.tx_amt3 = -out_lo_intst ;
		}
		if( cb_flag[0]=='2' )	/* ���� */
		{
			g_pub_tx.tx_amt3 = out_lo_intst ;
		}

		set_zd_double( "1086" , g_pub_tx.tx_amt3 );
		set_zd_data( "0210" , "01" );	

		/* ����Ľ�����ʺͱ���133�෴,����add_ind��ֵ�ͱ���133�෴�ֻ����ܼǶ� */
		char tmp_add_ind;
		if( g_pub_tx.add_ind[0]=='1' )	/* �ֻ��� */
		{
		 	tmp_add_ind='0';
			/*****XRB 1-2*****/
			set_zd_data( "0700" , "1" ); /* �� */
			strcpy( out_dc_ind, "1" );
			g_pub_tx.svc_ind = 3030 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	/* �ֻ��� */
		{
		 	tmp_add_ind='1';
			set_zd_data( "0700" , "2" ); /* �� */;
			strcpy( out_dc_ind, "2" );
			g_pub_tx.svc_ind = 3030	;		
		}
		g_pub_tx.add_ind[0]=tmp_add_ind;
		
		/* �岹���� */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "3" , acm );
		if( ret ) 	goto ErrExit;
					
		/* Pub_tx�ṹ��ֵ */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*�Ѿ���ȡ��mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */		
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		/* �Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"�������ǷϢ����" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"�������ǷϢ����" );
		}			
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
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;		
		TRACE
	   	sprintf(acErrMsg,"2��־����cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "3" , out_lo_intst );
		if( ret ) 	goto ErrExit;
		TRACE
	   	sprintf(acErrMsg,"2����־����cb_flag[%s]",cb_flag);
    	WRITEMSG		
		
	}

   	sprintf(acErrMsg,"ǰ����cmpd_lo_intst[%lf]",cmpd_lo_intst);
    	WRITEMSG	
   	sprintf(acErrMsg,"��־����cb_flag[%s]",cb_flag);
    	WRITEMSG
	/* �����ǷϢ���岹 */
	if( pub_base_CompDblVal( cmpd_lo_intst , 0.00 ) )
	{
		TRACE
		acm = 0.00;
		/* �������  */
		strcpy( acm_type , "2" ); /* �����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
		
		ret = pub_base_CalAcm_Ln( g_pub_tx.erro_date , g_pub_tx.tx_date , 
			g_ln_parm.intst_days_type , acm_type , 
			cmpd_lo_intst ,g_pub_tx.tx_amt4 , &acm , "0",0,0);
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			goto ErrExit;	
		}
		sprintf(acErrMsg,"���ú����������(��������)PASSΪ [%lf]",acm );
		WRITEMSG
	
		cmpd_acm = acm;
		
		if( cb_flag[0]=='1' )	/* ���� */
		{
			g_pub_tx.tx_amt4 = -cmpd_lo_intst ;
		}
		if( cb_flag[0]=='2' )	/* ���� */
		{
			g_pub_tx.tx_amt4 = cmpd_lo_intst ;
		}
		
   	sprintf(acErrMsg,"�У���cmpd_lo_intst[%lf]g_pub_tx.tx_amt4=[%lf]",cmpd_lo_intst,g_pub_tx.tx_amt4);
    	WRITEMSG
		set_zd_double( "1087" , g_pub_tx.tx_amt4 );
		set_zd_data( "0210" , "01" );	

		char tmp_add_ind;
		if( g_pub_tx.add_ind[0]=='1' )	/* �ֻ��� */
		{
			tmp_add_ind='0';
			set_zd_data( "0710" , "1" ); /* �� */
			strcpy( cmpd_dc_ind, "1" );
			g_pub_tx.svc_ind = 3040 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	/* �ֻ��� */
		{
			tmp_add_ind='1';
			set_zd_data( "0710" , "2" ); /* �� */;
			strcpy( cmpd_dc_ind, "2" );
			g_pub_tx.svc_ind = 3040	;		
		}
		g_pub_tx.add_ind[0]=tmp_add_ind;

		/* �岹���� */
		ret = ln_acm_modify( cb_flag , g_pub_tx.add_ind , "4" , acm );
		if( ret ) 	goto ErrExit;
					
		/* Pub_tx�ṹ��ֵ */
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻�ID */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.ac_get_ind,"111" );  /*�Ѿ���ȡ��mdm_ac_rel, g_ln_mst*/
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* ��Ϣ���� */		
		strcpy ( g_pub_tx.ct_ind , "2" );		/* ��ת:1��2ת */	
		/* �Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.prt_ind , "1" );	
		/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"�����ǷϢ����" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"�����ǷϢ����" );
		}			
		sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
		WRITEMSG

   	sprintf(acErrMsg,"zhongzhong����cmpd_lo_intst[%lf]g_pub_tx.tx_amt4=[%lf]",cmpd_lo_intst,g_pub_tx.tx_amt4);
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
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;		
		TRACE
	   	sprintf(acErrMsg,"2��־����cb_flag[%s]",cb_flag);
    	WRITEMSG
		ret = ln_reg_modify( cb_flag , g_pub_tx.add_ind , "4" , cmpd_lo_intst );
		if( ret ) 	goto ErrExit;
		TRACE	
	}
	
	/* �����ѺƷ���岹 *
	if( pub_base_CompDblVal( gage_amt , 0.00 ) )
	{
		if( cb_flag[0]=='1' )	* ���� *
		{
			g_pub_tx.tx_amt1 = -gage_amt ;
		}
		if( cb_flag[0]=='2' )	* ���� *
		{
			g_pub_tx.tx_amt1 = gage_amt ;
		}
		
		set_zd_double( "1088" , g_pub_tx.tx_amt1 );
		set_zd_data( "0210" , "01" );	
			
		if( g_pub_tx.add_ind[0]=='1' )	* ��(�岹----��ѺƷ�������) *
		{
			set_zd_data( "0720" , "2" ); * �� *
			g_pub_tx.svc_ind = 3092 ;							
		}
		if( g_pub_tx.add_ind[0]=='0' )	* ��(�岹----��ѺƷ������) *
		{
			set_zd_data( "0720" , "1" ); * �� *;
			g_pub_tx.svc_ind = 3092	;		
		}
			
		* Pub_tx�ṹ��ֵ *
		g_pub_tx.ac_id = g_ln_mst.ac_id;		* �˻�ID *
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	* �˻���� *
		strcpy ( g_pub_tx.ac_id_type , "3" );	* �˺����� 3-���� *
		strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	* ��Ϣ���� *		
		strcpy ( g_pub_tx.ct_ind , "2" );		* ��ת:1��2ת *	
		* �Ƿ��ӡ����(�������۱�־):1��0�� *
		strcpy ( g_pub_tx.prt_ind , "1" );	
		* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� *
		strcpy ( g_pub_tx.hst_ind , "1" );
		strcpy( g_pub_tx.ac_wrk_ind , "0000000" );
	
		if( cb_flag[0]=='1')
		{
			strcpy ( g_pub_tx.brf,"�����ѺƷ���岹" );
		}else if( cb_flag[0]=='2'){
			strcpy ( g_pub_tx.brf,"�����ѺƷ���岹" );
		}			
		sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
		WRITEMSG
	
		*** �Ǽǽ�����ˮ ***
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
		WRITEMSG
		g_pub_tx.tx_amt1 = 0.00;
		g_pub_tx.tx_amt2 = 0.00;
		g_pub_tx.tx_amt3 = 0.00;
		g_pub_tx.tx_amt4 = 0.00;
			
	}
	*********************/	
	/* ��ӡ���� */
	/* ���ڴ�Ʊ--���𲿷� */
	if( pub_base_CompDblVal( bal , 0.00 ) )		
	{
		ret = print_LNCBBJ( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBBJ,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* ���ڴ�Ʊ--���ڲ��� */
	if( pub_base_CompDblVal( in_lo_intst , 0.00 ) )		
	{
		ret = print_LNCBBN( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBBN,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* ���ڴ�Ʊ--���ⲿ�� */
	if( pub_base_CompDblVal( out_lo_intst , 0.00 ) )		
	{
		ret = print_LNCBBW( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBBW,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE	
	/* ���ڴ�Ʊ--�������� */
	if( pub_base_CompDblVal( cmpd_lo_intst , 0.00 ) )		
	{
		ret = print_LNCBFL( );			
		if(ret)
		{
			sprintf(acErrMsg,"print_LNCBFL,ʧ������!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L207");
			goto ErrExit;
		}
	}	
	TRACE				
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"����������ǳɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�����������ʧ�ܣ�[%s]",g_pub_tx.reply);
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
	
	get_zd_data("0370" , g_pub_tx.ac_no); 		/* �����˺� */	
	g_pub_tx.ac_seqn = 0;				/* �˺���� */

	get_zd_data("0670" , cb_flag); 			/* �岹��־ */
   	sprintf(acErrMsg,"��־����cb_flag[%s]",cb_flag);
    	WRITEMSG
    	TRACE
    	TRACE
	get_zd_data("0680" , g_pub_tx.add_ind); 	/* ������־ */
	get_zd_double("0390" , &bal); 			/* ����� */
	vtcp_log("aaaaaaaaaaaaaa[%lf]\n",bal);
	get_zd_double("0400" , &in_lo_intst); 		/* ����ǷϢ */
	get_zd_double("0410" , &out_lo_intst); 		/* ����ǷϢ */
	get_zd_double("0420" , &cmpd_lo_intst); 	/* ����ǷϢ */
   	sprintf(acErrMsg,"ȡֵ����cmpd_lo_intst[%lf]",cmpd_lo_intst);
    	WRITEMSG	
	get_zd_double("0430" , &gage_amt); 		/* ��ѺƷ��� */				
	get_zd_long("0440" , &g_pub_tx.erro_date); 	/* �������� */
/*	get_zd_data("0650",g_pub_tx.tx_code);���Ǹ�ʲô???? */
	return 0;
}
/* �޸Ĵ���Ǽǲ� */
/**********************************************************************
* �� �� ����  ln_reg_modify
* �������ܣ�  �޸Ĵ���Ǽǲ�
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��06��14��
*
* ��    ����
*     ��  �룺   cb_flag_tmp   char[2]        �岹��־
*                add_ind       char[2]        ������־    
*                type     char[2]             ���ͣ�1����2����ǷϢ��3����ǷϢ��4����ǷϢ
*	         pamt	  double	      ���
*      
*     ��  ��:   
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int ln_reg_modify( char *cb_flag_tmp , char *add_ind , char *type , double pamt )
{
	int ret=0;
	long date_tmp;
	char cb_flag_ind[2];
	
	date_tmp = g_pub_tx.tx_date ;
	g_pub_tx.tx_date = g_pub_tx.erro_date ;

	if( cb_flag_tmp[0]=='1')	/* ���� */
	{
			strcpy( cb_flag_ind , "0" );/*0*/
		
	}
	if( cb_flag_tmp[0]=='2')	/* ���� */
	{
			strcpy( cb_flag_ind , "1" );/*0*/
		
	}
	                           
	if( strcmp(cb_flag_ind , add_ind ) )	/* ���� �� �� ���� ��*/
	{
		TRACE
		ret = pub_ln_Udp_LnReg ( g_ln_mst.ac_id , g_ln_mst.ac_seqn , type , pamt );
		if(ret) return 1;
		TRACE
	}
	if( strcmp(cb_flag_ind , add_ind )==0 )	/* ���� �� �� ���� ��*/
	{
		TRACE
		ret = pub_ln_Ins_ln_reg( g_ln_mst , g_ln_parm , type , pamt , 0.00 ,"0" );
		if(ret) return 1;
		TRACE
	}
	
	g_pub_tx.tx_date = date_tmp ;
	return 0;
}

/* �޸Ĵ���Ǽǲ� */
/**********************************************************************
* �� �� ����  ln_acm_modify
* �������ܣ�  �޸Ĵ������
* ��    �ߣ�  lance
* ���ʱ�䣺  2003��08��11��
*
* ��    ����
*     ��  �룺   cb_flag_tmp   char[2]        �岹��־
*                add_ind       char[2]        ������־    
*                type     char[2]             ���ͣ�1���������2���ڻ�����3���������4��������
*                acm      double	      �޸ĵĻ���
*                
*     ��  ��:   
*
*     ����ֵ�� 0 �ɹ�
*            ��0 ʧ��
*
* �޸���ʷ��	
*                   
********************************************************************/
int ln_acm_modify( char *cb_flag_tmp , char *add_ind , char *type , double acm )
{
	int ret=0;
	char cb_flag_ind[2];

	if( cb_flag_tmp[0]=='1')	/* ���� */
	{
		strcpy( cb_flag_ind , "0" );
	}
	if( cb_flag_tmp[0]=='2')	/* ���� */
	{
		strcpy( cb_flag_ind , "1" );
	}	
	
	if( strcmp(cb_flag_ind , add_ind ) )	/* ���� �� �� ���� ��*/
	{
		/* �������� */
		TRACE
		if( type[0]=='1' )
		{
			g_ln_mst.intst_acm = g_ln_mst.intst_acm - acm;
		}else if( type[0]=='2' ){
			g_ln_mst.in_lo_acm = g_ln_mst.in_lo_acm - acm;
		}else if( type[0]=='3' ){
			g_ln_mst.out_lo_acm = g_ln_mst.out_lo_acm - acm;
		}else if( type[0]=='4' ){
			g_ln_mst.cmpd_lo_acm = g_ln_mst.cmpd_lo_acm - acm;
		}
		TRACE
	}
	if( strcmp(cb_flag_ind , add_ind )==0 )	/* ���� �� �� ���� ��*/
	{
		/* �������� */
		TRACE
		if( type[0]=='1' )
		{
			g_ln_mst.intst_acm = g_ln_mst.intst_acm + acm;
		}else if( type[0]=='2' ){
			g_ln_mst.in_lo_acm = g_ln_mst.in_lo_acm + acm;
		}else if( type[0]=='3' ){
			g_ln_mst.out_lo_acm = g_ln_mst.out_lo_acm + acm;
		}else if( type[0]=='4' ){
			g_ln_mst.cmpd_lo_acm = g_ln_mst.cmpd_lo_acm + acm;
		}
		TRACE
	}
	
	return 0;
}


/* ��ӡ--�岹ƾ֤--���𲿷� */
int print_LNCBBJ( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBBJ���ļ�ʧ��!");
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
	/* ������� */	
	pri_amt = bal ;		/* ����� */
	pri_acm = bal_acm;	/* �岹���� */
	/* �岹��־ */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "����" );
		strcpy( r_str, "(����)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "����" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* �����־ */	
	if( bal_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "�跽" );
	}else if( bal_dc_ind[0]=='2' ){
		strcpy( dc_str , "����" );	
	}	
	/* ������־ */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str , "��" );
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "��" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���   */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.������־  */
	/* 16.�岹��־�� 17.��ˮ�š�   18.�����־��19.�岹������ 20.��������  */
	/* 21.���ֱ�־ */
	fprintf(fp,"LNCBBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}

/* ��ӡ--�岹ƾ֤--���ڲ��� */
int print_LNCBBN( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBBN���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );			
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0155" );	/* ������Ϣ��Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	/* ������� */	
	pri_amt = in_lo_intst ;	/* ���ڽ�� */
	pri_acm = in_acm;	/* ���ڻ��� */
	/* �岹��־ */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "����" );
		strcpy( r_str, "(����)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "����" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* �����־ */	
	if( in_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "�跽" );
	}else if( in_dc_ind[0]=='2' ){
		strcpy( dc_str , "����" );	
	}	
	/* ������־ */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str ,"��");
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "��" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���   */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.������־  */
	/* 16.�岹��־�� 17.��ˮ�š�   18.�����־��19.�岹������ 20.��������  */
	/* 21.���ֱ�־ */
	fprintf(fp,"LNCBBN%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* ��ӡ--�岹ƾ֤--���ⲿ�� */
int print_LNCBBW( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBBW���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );			
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0156" );	/* ������Ϣ��Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	/* ������� */	
	pri_amt = out_lo_intst ;	/* ������ */
	pri_acm = out_acm;	/* ������� */
	/* �岹��־ */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "����" );
		strcpy( r_str, "(����)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "����" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* �����־ */	
	if( out_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "�跽" );
	}else if( out_dc_ind[0]=='2' ){
		strcpy( dc_str , "����" );	
	}	
	/* ������־ */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str , "��" );
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "��" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���   */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.������־  */
	/* 16.�岹��־�� 17.��ˮ�š�   18.�����־��19.�岹������ 20.��������  */
	/* 21.���ֱ�־ */
	fprintf(fp,"LNCBBW%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date ,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
/* ��ӡ--�岹ƾ֤--�������� */
int print_LNCBFL( )
{
	char tx_type[9];
	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	double pri_amt=0.00,pri_acm=0.00;
	char cb_str[5];
	char dc_str[5];
	char add_str[5];
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNCBFL���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ" );			
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0157" );	/* ������Ϣ��Ŀ */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	/* ������� */	
	pri_amt = cmpd_lo_intst ;	/* ������� */
	pri_acm = cmpd_acm;	/* �������� */
	/* �岹��־ */	
	if( cb_flag[0]=='1' )
	{
		strcpy( cb_str , "����" );
		strcpy( r_str, "(����)");
		pri_amt=-pri_amt;
	}else if( cb_flag[0]=='2' ){
		strcpy( cb_str , "����" );	
		strcpy( r_str, "");
		pri_amt=pri_amt;
	}
	/* �����־ */	
	if( cmpd_dc_ind[0]=='1' )
	{
		strcpy( dc_str , "�跽" );
	}else if( cmpd_dc_ind[0]=='2' ){
		strcpy( dc_str , "����" );	
	}	
	/* ������־ */	
	if( g_pub_tx.add_ind[0]=='0' )
	{
		strcpy( add_str , "��" );
	}else if( g_pub_tx.add_ind[0]=='1' ){
		strcpy( add_str , "��" );	
	}
		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( cb_str );
	pub_base_strpack( dc_str );
				
	/*  1.������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���   */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.������־  */
	/* 16.�岹��־�� 17.��ˮ�š�   18.�����־��19.�岹������ 20.��������  */
	/* 21.���ֱ�־ */
	fprintf(fp,"LNCBFL%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|%s|%.2lf|%ld|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	pri_amt , 0 , 0 , 0.00 , 0.00 ,
	0.00 , "" , "" , ln_acc_hrt, add_str , 
	cb_str , g_pub_tx.trace_no , dc_str , pri_acm , g_pub_tx.erro_date ,
	r_str );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
