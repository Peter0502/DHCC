/*************************************************
* �� �� ��:    sp9820.c
* ����������   
*              ���ݴ����˺�ȡ���˻����
*
* ��    ��:    lance
* ������ڣ�   2003��01��20��
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
#include "ln_lo_c.h"
#include "ln_pay_pln_c.h"
#include "cif_basic_inf_c.h"
	
sp9820()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	int flag=0, redflag=0;
	long dqr=0;
	char tx_code[5];
	double to_amt;	/* ����Ӧ����� */
	double tol,tol_lo,pay_amt;
	char cif_type[2];
	
	double	pun_intst=0.00,cmpd_intst_aj=0.00;
		int ttl_cnt=0,curr_cnt=0;
		double bal=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00;		
  	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	char dc_flg[2];
	char dc_inf[5];
	char dc_flg1[2];
	char dc_inf1[5];
	char dc_flg2[2];
	char dc_inf2[5];
		
	FILE *fp;
	struct ln_mst_c		v;
	struct mdm_ac_rel_c		w;
	struct ln_lo_c		ln_lo;
	struct ln_lo_c		ln_lo_tmp;
	struct ln_pay_pln_c	ln_pay_pln;	
	struct ln_pay_pln_c	ln_pay_pln_s;
	struct ln_pay_pln_c	l_ln_pay_pln;	
	struct cif_basic_inf_c cif_basic_inf;	
	struct ln_mst_c		ln_mst_tmp;
	
	/** ���ݳ�ʼ�� **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );

	memset(&ln_lo , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&ln_pay_pln_s , 0x00 , sizeof(struct ln_pay_pln_c) );
	memset( &cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c) );
	memset(&l_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );	
		
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "1091" , ac_no );	/* �����ʺ� */
	pub_base_old_acno( ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("1091",ac_no);	/* ������ʺ� */	
	
	get_zd_data( "0650" , tx_code ); /* ���״��� */
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &g_mdm_ac_rel , " ac_no='%s' " ,
																ac_no );
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L113" );
		ERR_DEAL
	}else if( ret !=0 ){
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
		set_zd_int( "109D", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/
 		sprintf(acErrMsg,"�������ļ������޴˼�¼[%d]",flag);
		WRITEMSG
 		strcpy( g_pub_tx.reply, "L114");
 		goto ErrExit;
   	}	

	 	
	if( flag==1 )	
	{
		ret = Ln_mst_Sel( g_pub_tx.reply , &ln_mst_tmp , 
									" ac_id=%ld " , g_mdm_ac_rel.ac_id );		
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		}else if(ret){
    		sprintf( acErrMsg,"ȡ�������ļ��쳣 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		ERR_DEAL
    	}	
    	sprintf(acErrMsg,"ȡ�������ļ�PASS");
		WRITEMSG 

	
		/* ������� */
		sprintf(acErrMsg,"�����������");
		WRITEMSG
		redflag=1;
		set_zd_long( "1092", g_mdm_ac_rel.ac_seqn );	/* �����˻���� */
		set_zd_int( "109D", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/ 	
	
		/* �ǿ��Ͽ���ר��ͨ�� */

	
		/* ���ø��ݴ�����ʾ�˺ź��˺����ȡ�������ļ�����*/
		ret = pub_ln_lndis_file(ac_no,g_mdm_ac_rel.ac_seqn,
					&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
		if( ret )
		{
			sprintf(acErrMsg,"��ȡ�����Ϣerror");
			WRITEMSG
			goto ErrExit;
		}
	
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
	
		strcpy( dc_flg1 , "1" );
  		strcpy( dc_inf1 , "�ֽ�" );
		strcpy( dc_flg , "2" );
  		strcpy( dc_inf , "ת��" );
		strcpy( dc_flg2 , "8" );
  		strcpy( dc_inf2 , "�ۻ�" );
		set_zd_data("0660",dc_flg);
		set_zd_data("0810",dc_inf);  
	
		/***��ɳ���д����ʱ���۶�˽�Թ��ͻ���֧���ֽ��ת��
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
		******/
		fprintf( fp,"0660|%s|%s|\n",dc_inf1,dc_flg1 );
		fprintf( fp,"0660|%s|%s|\n",dc_inf,dc_flg );
		fprintf( fp,"0660|%s|%s|\n",dc_inf2,dc_flg2 );

		set_zd_data( DC_FILE_SND,"2" );
		fclose(fp);	
		
		if( strcmp( tx_code , "3203" )==0 )	
		{
			sprintf(acErrMsg,"����3203!");
			WRITEMSG
			
			/* �����������Ƿ��ǰ��Ҵ��� */
			if( g_ln_parm.ln_pay_type[0]!='3'&& g_ln_parm.ln_pay_type[0]!='4' )
			{
				sprintf( acErrMsg,"�˴���ǰ��Ҵ���[%s]",g_ln_parm.ln_pay_type);
				WRITEMSG
				strcpy( g_pub_tx.reply ,"L042" );
				ERR_DEAL
			}	
			sprintf(acErrMsg,"�����������Ƿ��ǰ��Ҵ��� PASS!");
			WRITEMSG		
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
		
		
			/* ��ѯ���Ҵ���Ƿ����е�Ƿ����Ϣ */	
			ret= Ln_lo_Dec_Sel( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d and \
							pay_type='0'" , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)
			{
				sprintf( acErrMsg, "ִ��Ln_lo_Dec_Sel��!ret=[%d]", ret);
				WRITEMSG		
				return(1);	
			}			
			TRACE
			flag=0;	/* ��Ƿ���� */
			while(1)
			{
				ret = Ln_lo_Fet_Sel( &ln_lo_tmp, g_pub_tx.reply );
   				if( ret && ret!=100 )
 				{
	 				sprintf(acErrMsg,"fetch Ln_lo error code = [%d]",ret);
	 				WRITEMSG
	 				return 1;
 				}else if(ret==100){
					break;
				}
				flag++;
			
	 			sprintf( acErrMsg,"old_ac_ind= [%s]",ln_lo_tmp.old_ac_ind );
	 			WRITEMSG
	 			sprintf( acErrMsg,"mac= [%s]",ln_lo_tmp.mac );
	 			WRITEMSG			
	 			sprintf(acErrMsg,"pay_pun_intst=%lf",ln_lo_tmp.pay_pun_intst );
	 			WRITEMSG	
	 				
				if(ln_lo_tmp.old_ac_ind[0]=='1'|| ln_lo_tmp.old_ac_ind[0]=='2')
				{
					/* ��ϵͳ���ݴ���ʽ */
					/* ���㷣Ϣ��� */
					TRACE
					/*****
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt , 
								ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst , 
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , 
								&pun_intst , &cmpd_intst_aj);    
					****/
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt , 
								ln_lo_tmp.lo_intst , 
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , 
								&pun_intst , &cmpd_intst_aj);  					        	
		            if (ret) goto ErrExit;   
		        }
		        if( ln_lo_tmp.old_ac_ind[0]=='0' )
				{
					/* ��ϵͳ���ݴ���ʽ */
					/* ���㷣Ϣ��� */
					TRACE
					/****
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt ,
								ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst ,
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date ,
								&pun_intst , &cmpd_intst_aj); 
					****/
					ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm ,
								ln_lo_tmp.lo_amt ,
								ln_lo_tmp.lo_intst ,
								ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date ,
								&pun_intst , &cmpd_intst_aj); 					           	
	               	if (ret) goto ErrExit;	
	            }
	                
				sprintf(acErrMsg,"pun_intst=[%lf]!cmpd_intst_aj=[%lf]", 
													pun_intst,cmpd_intst_aj);
				WRITEMSG	  
			              	        
				ln_lo.lo_amt+=ln_lo_tmp.lo_amt; 			/* Ƿ��� */
				ln_lo.lo_intst+=ln_lo_tmp.lo_intst; 		/* Ƿ��Ϣ */
				ln_lo.pun_intst+=pun_intst+cmpd_intst_aj ; 	/* ��Ϣ */
				ln_lo.pay_lo_amt+=ln_lo_tmp.pay_lo_amt; 			/* �ѻ�Ƿ��� */
				ln_lo.pay_lo_intst+=ln_lo_tmp.pay_lo_intst; 		/* �ѻ�Ƿ��Ϣ */
				ln_lo.pay_pun_intst+=ln_lo_tmp.pay_pun_intst ; 	/* �ѻ���Ϣ */				
				/***
				pay_amt+=ln_lo_tmp.pay_lo_amt+ln_lo_tmp.pay_lo_intst
								   +pun_intst+cmpd_intst_aj;* �ѻ���� *
				****/				
	 			sprintf( acErrMsg,"pay_amt[%lf]",pay_amt);
	 			WRITEMSG
			}
		Ln_lo_Clo_Sel ();
		TRACE
		/* ��ɳ���о��������� */
		/* ȡ������Ӧ����� */
  		ret = Ln_pay_pln_Sel(g_pub_tx.reply, &ln_pay_pln, 
				"ac_id=%ld and ac_seqn=%d", g_ln_mst.ac_id,g_ln_mst.ac_seqn);
		if( ret==100 )
		{
			sprintf(acErrMsg,"����ƻ�����Ϣ���޴���Ϣ![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L043");
			goto ErrExit;
		}else if(ret){
			sprintf(acErrMsg,"��ȡ�����ƻ�����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}		  
		sprintf(acErrMsg,"ȡ�����ƻ�����Ϣ PASS");
		WRITEMSG	
		TRACE
		
		/* ����Ӧ����� */
		ret = pub_base_deadlineM( ln_pay_pln.beg_date, -1, &dqr);
		if( ret ) 	goto ErrExit;
			
		if(dqr > g_pub_tx.tx_date )
		{
			sprintf(acErrMsg,"Ԥ���ѻ���!");
			WRITEMSG
			ln_pay_pln.curr_amt=0;	/* ����Ӧ������ */
			ln_pay_pln.curr_intst=0;/* ����Ӧ����Ϣ */

			if( (g_ln_mst.opn_date==g_ln_mst.ic_date) && ln_pay_pln.curr_cnt==1 )/*��һ��*/
			{						
				sprintf(acErrMsg,"��һ��!");
				WRITEMSG				
				/*���ڳ�ɳ�������ݲ�׼ȷ,���¼��㵱�ڻ���ƻ�*/
				ret = pub_ln_PayPlan_Qry( g_ln_mst , g_ln_parm , ln_pay_pln.curr_cnt, ln_pay_pln.ttl_cnt,g_ln_mst.bal,&l_ln_pay_pln);	
				if (ret)
				{
					sprintf(acErrMsg,"���ú���pub_ln_PayPlan_qry����!");
					WRITEMSG
					return 1;	
				}
				ln_pay_pln.curr_amt= l_ln_pay_pln.curr_amt;				
				ln_pay_pln.curr_intst= l_ln_pay_pln.curr_intst;
			}
			to_amt = ln_pay_pln.curr_amt+ln_pay_pln.curr_intst; /* ����Ӧ����� */
		}else{
				/*���ڳ�ɳ�������ݲ�׼ȷ,���¼��㵱�ڻ���ƻ�*/
				ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*������*/
				curr_cnt= ln_pay_pln.curr_cnt;		/*��ǰ����*/
				/* ȡǷ�������*/
				ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
				if(ret)	return 1;
				ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
				if(ret)	return 1;	
				bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);			/*ʣ�౾��*/				
				ret = pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt, bal, &l_ln_pay_pln);				
				if (ret)
				{
					sprintf(acErrMsg,"���ú���pub_ln_PayPlan_qry����!");
					WRITEMSG
					return 1;	
				}
				ln_pay_pln.curr_amt= l_ln_pay_pln.curr_amt;				
				ln_pay_pln.curr_intst= l_ln_pay_pln.curr_intst;
				to_amt = ln_pay_pln.curr_amt+ln_pay_pln.curr_intst; /* ����Ӧ����� */
		}
		
		if((ln_pay_pln.curr_cnt==ln_pay_pln.ttl_cnt)&&
									ln_pay_pln.pay_ind[0]=='1' )
		{
			to_amt = 0;
		}
		/* ���ܽ�� */
		pay_amt=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+ln_lo.pay_pun_intst;
		tol_lo=ln_lo.lo_amt+ln_lo.lo_intst+ln_lo.pun_intst-pay_amt;/*Ƿ���ܼ�*/
		sprintf(acErrMsg,"tol_lo[%lf],lo_amt[%lf], lo_intst[%lf] ,\
							pun_intst[%lf] , pay_amt[%lf]",tol_lo,ln_lo.lo_amt,
							ln_lo.lo_intst,ln_lo.pun_intst,pay_amt);
		WRITEMSG		
		tol=tol_lo+to_amt;	/* �ܼ� */ 
		sprintf(acErrMsg,"tol_lo[%lf], to_amt[%lf]",tol_lo,to_amt);
		WRITEMSG		
		TRACE
		/* ��� */ 
		set_zd_double("1094",g_ln_mst.bal);/* ����� */
		set_zd_long("109E",ln_pay_pln.ttl_cnt);	/* ������ */
		set_zd_long("109F",ln_pay_pln.curr_cnt-1-flag);	/* �ѻ����� */

		set_zd_int("1096",flag);	/* ��Ƿ���� */
		set_zd_double("1097",ln_lo.lo_amt+ln_lo.lo_intst-ln_lo.pay_lo_amt-ln_lo.pay_lo_intst);/* ��Ƿ�ܶ� */
		set_zd_double("1098",ln_lo.pun_intst-ln_lo.pay_pun_intst);/* ��Ϣ */		
		set_zd_double("109G",tol_lo);/* Ƿ���ܼ� */	
		
		set_zd_double("1095",to_amt);	/* ���ڽ�� */			
		set_zd_double("109I",ln_pay_pln.curr_amt);	/* ����Ӧ������ */		
		set_zd_double("109J",ln_pay_pln.curr_intst);	/* ����Ӧ����Ϣ */							
		set_zd_double("109H",tol);	/* �ܼƽ�� */		
		
		set_zd_data("1093",g_ln_mst.name);/* ���� */

		/* ȡ�ͻ����� */
		ret = Cif_basic_inf_Sel( g_pub_tx.reply , &cif_basic_inf , 
										"cif_no=%ld" , g_ln_mst.cif_no);
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
	
    	goto OkExit;
    	}
  }  	
    	
  	redflag=9;
	set_zd_int( "109D", redflag );	/* �Ƿ�������ű�־ 1-Ψһ��¼*/	 		
		
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
   
