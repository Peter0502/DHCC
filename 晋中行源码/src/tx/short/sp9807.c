/*************************************************
* �� �� ��:    sp9807.c
* ����������   
*              ���ݴ����˺�ȡ���˻����,���������, ����65��ȡ�����׵�tx_code
*
* ��    ��:    lance
* ������ڣ�   2003��01��14��
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
#include "ln_auth_c.h"
	
sp9807()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	char tx_code[5];
	char cif_type[2];	
	char rate_flag[2];
	long beg_end_date;  /* ������- ������ */
	long beg_end_days;  /* ������- ������ */
	long max_exp_date;  /* ���չ������ */	
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char save_ac_no[20];
		
	FILE *fp;
	
	struct ln_mst_c		ln_mst_tmp;
	struct cif_basic_inf_c 	cif_basic_inf;	
	struct mdm_ac_rel_c	mdm_ac_rel_tmp;
	struct ln_auth_c	ln_auth_tmp;		
	
	/** ���ݳ�ʼ�� **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	memset( &mdm_ac_rel_tmp,0x00,sizeof(struct mdm_ac_rel_c) );
	memset( &ln_auth_tmp,0x00,sizeof(struct ln_auth_c) );
			
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "0370" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	get_zd_data( "0650" , tx_code ); /* ���״��� */

	set_zd_data("0370",ac_no);	/* ������ʺ� */
	pub_base_strpack(tx_code);
		sprintf( acErrMsg,"0650 ���״���=[%s]",tx_code);
		WRITEMSG	
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
    		sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%d]",ret);
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
 		set_zd_int( "0500", redflag );	/* �Ƿ�������ű�־ */   
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
			strcpy( g_pub_tx.reply ,"L114" );
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
		vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
		set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
		set_zd_double( "0410", g_ln_mst.bal );		/* ��� */	
		if(memcmp(ln_mst_tmp.opn_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1)){
    			sprintf( acErrMsg,"����ͬһ�������������˽���");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D124" );
    			ERR_DEAL
		}
	
	/* ������� */
	sprintf(acErrMsg,"�����������");
	WRITEMSG
	redflag=1;
	set_zd_long( "0340", ln_mst_tmp.ac_seqn );	/* �����˻���� */	
	set_zd_int( "0500", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/    
    	sprintf(acErrMsg,"redflag[%d]ac_seqn[%d]",redflag,ln_mst_tmp.ac_seqn);
	WRITEMSG 

	/* �ǿ��Ͽ���ר��ͨ�� */

	/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
	ret = pub_ln_lndis_file(ac_no,ln_mst_tmp.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ �������¸�����֧��ͨ�ò���20110729*/
	if( ret )
	{
		sprintf(acErrMsg,"��ȡ�����Ϣerror");
		WRITEMSG
		goto ErrExit;
	}
	TRACE
		vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
	/* ����״̬Ϊ����ʱ������������ */
	if ( g_ln_mst.ac_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�˴���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"L035");
		goto ErrExit;
	}
	TRACE	
	/* ��ת��־�ж� */
	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	TRACE	
	strcpy( dc_flg1 , "1" );
  strcpy( dc_inf1 , "�ֽ�" );
	strcpy( dc_flg , "2" );
  strcpy( dc_inf , "ת��" );
	set_zd_data("0660",dc_flg);
	set_zd_data("0810",dc_inf);  
		vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);

/**** ����ɳ����Ҫ���޸� �����ʱ�����۶�˽���Թ��ͻ���֧���ֽ��ת��		
	if( g_ln_parm.cif_type[0]=='2' )
	{		    
		pub_base_strpack(dc_inf);
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
	}else{
		pub_base_strpack(dc_inf1);
		pub_base_strpack(dc_inf);
	
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
	}	
**************/

		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		
	set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);	

	TRACE
	if( strcmp( tx_code , "3102" )==0 )	
	{
			sprintf(acErrMsg,"����3102!");
			WRITEMSG
		  /* �жϷſ�ƻ����� N�� M�ֶ� */
			if ( g_ln_parm.pay_pln_type[0]!='M' )
			{
				sprintf(acErrMsg,"�ſ�ƻ����ͷ��ֶ�[%s],�������˽���!",g_ln_parm.pay_pln_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L075");
				goto ErrExit;
			}	
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* �ƻ��ſ�� */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* �ۼƷ��Ŷ� */
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
			set_zd_double( "0420", g_ln_mst.bal );		/* ��� */	
			set_zd_data( "0720" , g_ln_parm.pay_pln_crt ); /* ����ƻ�������־  */
		
	}			
			
	if( strcmp( tx_code , "3103" )==0 )	
	{
			sprintf(acErrMsg,"����3103!");
			WRITEMSG
			/* �жϷſ�ƻ����� N�� M�ֶ� */
			if ( g_ln_parm.pay_pln_crt[0]!='M' )
			{
				sprintf(acErrMsg,"����ƻ����ͷ��ֶ�[%s],�������˽���!",g_ln_parm.pay_pln_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L080");
				goto ErrExit;
			}	
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* �ƻ��ſ�� */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* �ۼƷ��Ŷ� */
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
			set_zd_double( "0420", g_ln_mst.bal );		/* ��� */	
		
	}	
	
	if( strcmp( tx_code , "3104" )==0 )	
	{
			sprintf(acErrMsg,"����3104!");
			WRITEMSG

			/* ����״̬Ϊ����ʱ������������ 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"�˴�����δ��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			
			*****/
			/* ȡ�ͻ����� */
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , "cif_no=%ld" , g_ln_mst.cif_no);
			if( ret )
			{
				goto ErrExit;
			}
			if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
			{
				strcpy( cif_type , "0" );	/* ��˽ */
			}else{
				strcpy( cif_type , "1" );	/* �Թ� */
			}		
			set_zd_data("109L" , cif_type);				
			sprintf(acErrMsg,"�ͻ�����[%s]!",cif_type);
			WRITEMSG
						
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* �ƻ��ſ�� */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* �ۼƷ��Ŷ� */
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
			set_zd_double( "0420", g_ln_mst.bal );		/* ��� */	
			set_zd_data( "0210", g_ln_parm.cur_no );		/* ���� */
			set_zd_data("0670", g_ln_parm.ln_type); /*��������,added by gujy on 060916*/
			set_zd_data("0630",g_ln_mst.pact_no); /**��ݺ�** added by ligl 2006-9-28 12:05*/
			set_zd_data("0370",g_ln_mst.trust_no);/**ί��Э�� added by ligl 2006-9-28 15:55**/
			TRACE					
			sprintf( acErrMsg,"(!!~~ѡ���������-[%s]", g_ln_parm.ln_type);
			WRITEMSG	
			
			/* ���ݴ���˻�IDȡ����˻� */
			ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth_tmp , "pact_no='%s'" , g_ln_mst.pact_no );
			if(ret)
			{
				sprintf(acErrMsg,"��ѯ���������Ǽǲ��쳣����![%s][%d]",g_ln_mst.pact_no,ret);
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
			
			set_zd_data("1061" , save_ac_no);	/* ����˺� */
						
	}

	TRACE					
	sprintf( acErrMsg,"���״���=[%s]",tx_code);
	WRITEMSG	
	if( strcmp( tx_code , "3204" )==0 )	
	{
			sprintf(acErrMsg,"����3204!");
			WRITEMSG
			
			/* ����״̬Ϊ����ʱ������������ 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"�˴�����δ��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			************/
			/* ��ѯ�����Ƿ�ſ� */ 
			ret = pub_base_CompDblVal( g_ln_mst.bal, 0.00 );
			if( ret==0 )
			{
    				ret = pub_base_CompDblVal( g_ln_mst.ys_bal, 0.00 );
    				if( ret==0 )
    				{
    					sprintf( acErrMsg,"�˰��Ҵ�����δ�ſ�!");
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L109" );		
					goto ErrExit;
				}
			}		
			sprintf(acErrMsg,"�������Ƿ�ſ� PASS!");
			WRITEMSG
					
			/* �����������Ƿ��ǰ��Ҵ��� */
			if ( g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4' )
			{
				sprintf( acErrMsg,"�˴���ǰ��Ҵ���[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L042" );
				ERR_DEAL
			}	
			sprintf(acErrMsg,"�����������Ƿ��ǰ��Ҵ��� PASS!");
			WRITEMSG		
			TRACE

			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
			set_zd_double( "0420", g_ln_mst.bal );		/* ��� */	
			sprintf(acErrMsg,"�������[%lf] PASS!",g_ln_mst.bal);
			WRITEMSG					
			/* ȡ�ͻ����� */
			ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , "cif_no=%ld" , g_ln_mst.cif_no);
			if( ret )
			{
				goto ErrExit;
			}
			if( cif_basic_inf.type[0]=='1' || cif_basic_inf.type[0]=='5' )
			{
				strcpy( cif_type , "0" );	/* ��˽ */
			}else{
				strcpy( cif_type , "1" );	/* �Թ� */
			}		
			set_zd_data("109L" , cif_type);	
					
	}	

	TRACE			
	if( strcmp( tx_code , "3205" )==0 )	
	{
			sprintf(acErrMsg,"����3205!");
			WRITEMSG
			
			/* ����״̬Ϊ����ʱ������������ 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"�˴�����δ��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			******************/
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */
			if( g_ln_mst.repay_type[0]=='3' || g_ln_mst.repay_type[0]=='4')	/* ���Ҵ��� */
			{
				set_zd_data( "0220", "0" );		/* �������� */
			}else{
				set_zd_data( "0220", "1" );		/* �������� */
			}
	}	  
/************3301��ʼ 20110729 �����������Ŵ�ϵͳ��չ��ѯ����***********/
	if( strcmp( tx_code , "3301" )==0 )	
	{
			sprintf(acErrMsg,"����3301!");
			WRITEMSG
			/* ���Ҵ����չ�� */
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"�˴���Ϊ���Ҵ���,����չ��[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L121" );
				ERR_DEAL
			}	
							  
	    /* ����������չ������ */
    	beg_end_date = g_ln_mst.mtr_date - g_ln_mst.opn_date ;
	sprintf( acErrMsg,"beg_end_date[%ld]",beg_end_date);
	WRITEMSG	
	    beg_end_days = pub_base_CalTrueDays( g_ln_mst.opn_date , g_ln_mst.mtr_date );
	    sprintf( acErrMsg,"opn_date[%ld]mtr_date=[%ld]beg_end_days[%ld]",g_ln_mst.opn_date , g_ln_mst.mtr_date,beg_end_days);
	    WRITEMSG	
	    if ( beg_end_date <= 10000 )
	    {
	      	pub_base_deadlineD( g_ln_mst.mtr_date , beg_end_days , &max_exp_date );
	    }else if ( beg_end_date <= 50000 ){
			    pub_base_deadlineD( g_ln_mst.mtr_date , beg_end_days/2 , &max_exp_date );
		  }else{
				  max_exp_date = g_ln_mst.mtr_date + 30000;
			}
	    sprintf(acErrMsg,"����������չ������[%ld] PASS!",max_exp_date);
	    WRITEMSG
	
	    /* ��ɳ����----���5������� *****/
	    max_exp_date = max_exp_date+5;
	    sprintf(acErrMsg,"����������չ������[%ld] PASS!",max_exp_date);
	    WRITEMSG
	    	    
	    /* ��������� */
	/**����ʵ��Ҫ�������ղ�ӦΪ����չ�ڵ�����    
	if ( g_ln_mst.exp_ind[0]!='0' )
	    {
		     g_ln_mst.mtr_date = g_ln_mst.exp_mtr_date ;
	    }
    ******/
    /*20110731 �������� �����Ŵ�ϵͳ �ɴ˱�ȷ�����Խ��еĲ���*/
      ret = Ln_auth_Sel( g_pub_tx.reply , &ln_auth_tmp , "pact_no='%s' and sts='X' and type='02' " , g_ln_mst.pact_no );
			if(ret)
			{
				if(ret ==100){
					sprintf(acErrMsg,"��ѯ���������Ǽǲ��޴˼�¼![%s][%d]",g_ln_mst.pact_no,ret);
				  WRITEMSG
				  strcpy(g_pub_tx.reply,"D104");
				  goto ErrExit;
				}
				else{
				  sprintf(acErrMsg,"��ѯ���������Ǽǲ��쳣����![%s][%d]",g_ln_mst.pact_no,ret);
				  WRITEMSG
				  strcpy(g_pub_tx.reply,"L181");
				  goto ErrExit;
			  }
			}						
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */
			set_zd_double("0420",g_ln_mst.bal);             /*���*/	
			set_zd_long( "0440", g_ln_mst.mtr_date );	/* ��������� */
			set_zd_long( "0470", g_ln_mst.opn_date );	/* ��������� */
			set_zd_long("0460",ln_auth_tmp.mtr_date);/*20110729 �����Ŵ�����չ������*/
			set_zd_double("0840",ln_auth_tmp.rate);/*����*/
			if ( g_ln_mst.exp_ind[0]!='0' )
        {
            set_zd_long("0450",g_ln_mst.exp_mtr_date); /*չ�ڵ�����*/
		}

			set_zd_long( "0530", max_exp_date );		/* ���չ������ */
    	sprintf(acErrMsg,"���չ������[%ld]",max_exp_date );
		  WRITEMSG 					
	}	
	/*************3301����Ϊֹ*******************/ 
	
	if( strcmp( tx_code , "3302" )==0 )	
	{
			sprintf(acErrMsg,"����3302!");
			WRITEMSG
			
			/* ����״̬Ϊ����ʱ������������ 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"�˴�����δ��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			************/
			
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
		  set_zd_data( "0700", g_ln_mst.ac_sts );		/* �˻�״̬ */
		
	}					  

	if( strcmp( tx_code , "3305" )==0 )	
	{
			sprintf(acErrMsg,"����3305!");
			WRITEMSG
			
			/* ����״̬Ϊ����ʱ������������ 
			if ( g_ln_mst.ac_sts[0]=='8' )
			{
					sprintf(acErrMsg,"�˴�����δ��������!");
					WRITEMSG
					strcpy(g_pub_tx.reply,"L120");
					goto ErrExit;
			}	
			************/
			
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
		  set_zd_data( "0700", g_ln_mst.five_sts );		/* �弶����״̬ */
		
	}	
			
	if( strcmp( tx_code , "3314" )==0 )	
	{
		sprintf(acErrMsg,"����3314!");
		WRITEMSG
						
		set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
		set_zd_data( "0630", g_ln_mst.pact_no );	/* ��ݺ� */	
		set_zd_double( "0400", g_ln_mst.gage_amt );	/* ��ѺƷ��� */
		set_zd_double( "0410", g_ln_mst.bal );		/* ������� */		
	}
					
	if( strcmp( tx_code , "3306" )==0 )	
	{
			sprintf(acErrMsg,"����3306!");
			WRITEMSG

			/***** �����������Ƿ��ǰ��Ҵ��� 
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"�˴���Ϊ���Ҵ���[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L122" );
				ERR_DEAL
			}	
			���Ҵ����鷣Ϣ����*****/			
	    if( g_ln_mst.ac_sts[0] =='1' )
	    {
		    sprintf( acErrMsg,"!!!!!!!!!!![%ld]",g_pub_tx.tx_date);
	      WRITEMSG
	 	    if ( g_ln_mst.mtr_date >= g_pub_tx.tx_date )
	 	    {
	 		    rate_flag[0]='1';	/* ���� */
	     	}else{
	 		    rate_flag[0]='2';	/* չ�� */	
	     	}
      }

	    if( g_ln_mst.ac_sts[0] =='2' || g_ln_mst.ac_sts[0] =='3' ||g_ln_mst.ac_sts[0] =='4' || g_ln_mst.ac_sts[0] =='5')
	    {
	 	    rate_flag[0]='3';	/* ���� */
    	}

	    /* ������� */

	    set_zd_data( "0250", g_ln_mst.name );		/* ���� */		
	    set_zd_data( "0700", rate_flag );		/* �˻�״̬ */	
	    set_zd_data( "0690", g_ln_parm.ln_pay_type );	/* �˻����� */		
	}
	
	if( strcmp( tx_code , "3317" )==0 )	
	{
			sprintf(acErrMsg,"����3317!");
			WRITEMSG
			/*���Ҵ�������˽���*/
			if ( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
			{
				sprintf( acErrMsg,"�˴���Ϊ���Ҵ���,�������˽���[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L230" );
				ERR_DEAL
			}
			ret = pub_base_CompDblVal( g_ln_mst.fine_rate, 0.00 );
			if ( ret <= 0 )
			{
				strcpy(g_pub_tx.reply,"L229");
				sprintf(acErrMsg,"���˻���δ���巣Ϣ����!");
				WRITEMSG
				goto ErrExit;
			}	
			set_zd_double( "0400", g_ln_mst.amt_lmt );	/* �ƻ��ſ�� */	
			set_zd_double( "0410", g_ln_mst.ttl_prvd_amt );	/* �ۼƷ��Ŷ� */
			set_zd_data( "0250", g_ln_mst.name );		/* ���� */	
			set_zd_double( "0420", g_ln_mst.bal );		/* ��� */	
			set_zd_double( "0840", g_ln_mst.fine_rate );		/* ��Ϣ���� */	
			set_zd_data( "0720" , g_ln_parm.pay_pln_crt ); /* ����ƻ�������־  */
		
	}			
	/****��4519���׷���һЩ����  add 20070524
	if( strcmp( tx_code , "3399" )==0 )	
	{
		sprintf(acErrMsg,"����3399!");
		WRITEMSG
						
		set_zd_data( "0250", g_ln_mst.name );		* ���� *	
		set_zd_data( "0630", g_ln_mst.pact_no );	* ��ݺ� *	
		set_zd_double( "0410", g_ln_mst.bal );		* ������� *		
	}
	end 20070524****/
	vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
	set_zd_double( "0560", g_ln_mst.intst_acm );    /* ������� add by martin 2008/12/10 15:18 */	 		
    	goto OkExit;
    	sprintf(acErrMsg,"redflag[%d]",redflag);
	WRITEMSG 
    	  	
  }  

	vtcp_log("[%s][%d] ln.name = [%s]\n",__FILE__,__LINE__,g_ln_mst.name);
    	TRACE
  	redflag=9;
	set_zd_int( "0500", redflag );	/* �Ƿ�������ű�־ 9-������¼*/ 	
	
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	sprintf(acErrMsg,"redflag[%d]ac_seqn[%d]",redflag,ln_mst_tmp.ac_seqn);
	WRITEMSG 
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
