/*************************************************
* �� �� ��:    sp9823.c
* ����������   
*              ���ݴ����˺�,�˻�����Լ������ڴ�������һ�����
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
	
sp9823()  
{ 		
	int ret=0;
	int flag=0, redflag=0;
	long dqr=0;
	double to_amt;	/* ����Ӧ����� */
	double tol,tol_lo;
	int cnt,i;
	double amt;	/* ��ǰ������� */
	double pay_amt,pay_lo_amt; /* ������ */
	double	pun_intst=0.00,cmpd_intst_aj=0.00;
	
		int ttl_cnt=0,curr_cnt=0;
		double bal=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00;
				
	struct ln_mst_c		v;
	struct mdm_ac_rel_c	w;
	struct ln_lo_c		ln_lo;
	struct ln_lo_c		ln_lo_tmp;
	struct ln_pay_pln_c	ln_pay_pln;
	struct ln_pay_pln_c	t_ln_pay_pln;	
	struct ln_pay_pln_c	v_ln_pay_pln;	
	struct ln_pay_pln_c	l_ln_pay_pln;	
		
	/** ���ݳ�ʼ�� **/
	memset(&ln_lo , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_lo_tmp , 0x00 , sizeof(struct ln_lo_c) );
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&t_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&v_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );	
	memset(&l_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );	
		
	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();
	
	/** ȡֵ����ֵ **/	
	
	get_zd_data( "1091" , w.ac_no );	/* �����ʺ� */
	pub_base_old_acno( w.ac_no );  	/** �Ծ��ʺŵĴ��� **/
	set_zd_data("1091",w.ac_no);	/* ������ʺ� */

	get_zd_long( "1092" , &v.ac_seqn );	/* �ʺ���� */
	get_zd_int( "1099" , &cnt );		/* �������� */

	sprintf( acErrMsg," senf acno [%s] [%d]",w.ac_no,v.ac_seqn );
	WRITEMSG
	
	/* ���ݴ����ʺŲ�ѯ�˺Ž��ʹ�ϵ��ȡ���˻�ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",w.ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"ƾ֤(����)���˻���ϵ���޴˼�¼ erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"ȡƾ֤(����)���˻���ϵ���쳣erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
    }

	sprintf(acErrMsg,"ȡ�����˻�IDΪ[%ld]seqn[%d]",
		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	WRITEMSG	
	
	if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && g_mdm_ac_rel.ac_seqn!=v.ac_seqn)
	{
    	sprintf( acErrMsg,"[%d]!=[%d]",g_mdm_ac_rel.ac_seqn,v.ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
	}
	else
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld and ac_seqn=%d ",
			g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		Ln_mst_Clo_Sel ();
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' ",
			g_ln_mst.prdt_no);
		if( ret==100 )
		{
			sprintf( acErrMsg,"�������ļ������޴˼�¼ erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"W030" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		/* �����������Ƿ��ǰ��Ҵ��� */
		if ( g_ln_parm.ln_pay_type[0]!='3'&& g_ln_parm.ln_pay_type[0]!='4' )
		{
			sprintf( acErrMsg,"�˴���ǰ��Ҵ���",ret);
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
    			sprintf( acErrMsg,"�˰��Ҵ�����δ�ſ�!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L109" );		
			goto ErrExit;
		}		
		sprintf(acErrMsg,"�������Ƿ�ſ� PASS!");
		WRITEMSG				
		
		/* ��ѯ���Ҵ���Ƿ����е�Ƿ����Ϣ */	
		ret = Ln_lo_Dec_Sel( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and \
							pay_type='0'" , g_mdm_ac_rel.ac_id,v.ac_seqn );
		if(ret)
		{
				sprintf( acErrMsg, "ִ��Ln_lo_Dec_Sel��!ret=[%d]", ret);
				WRITEMSG		
				return(1);	
		}			
		
		flag=0;	/* ��Ƿ���� */
		while(1)
		{
			ret = Ln_lo_Fet_Sel( &ln_lo_tmp, g_pub_tx.reply );
   			if( ret && ret!=100 )
 			{
	 			sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
	 			WRITEMSG
	 			return 1;
 			}
			else if( ret==100 )
			break;
			
			if( ln_lo_tmp.old_ac_ind[0]=='1' || ln_lo_tmp.old_ac_ind[0]=='2' )
			{
				/* ��ϵͳ���ݴ���ʽ */
				/* ���㷣Ϣ��� */
				TRACE
				/***
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt , ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
		                ****/
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt , ln_lo_tmp.lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
		                
		                if (ret) goto ErrExit;   
		        }
		        if( ln_lo_tmp.old_ac_ind[0]=='0' )
			{
				/* ��ϵͳ���ݴ���ʽ */
				/* ���㷣Ϣ��� */
				TRACE
				/******
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt-ln_lo_tmp.pay_lo_amt , ln_lo_tmp.lo_intst-ln_lo_tmp.pay_lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
	                	******/
				ret = pub_ln_CalPunIntst ( g_ln_mst , g_ln_parm , ln_lo_tmp.lo_amt , ln_lo_tmp.lo_intst , ln_lo_tmp.shld_pay_date , g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);            	
	                	
	                	if (ret) goto ErrExit;	
	                }
	                
			sprintf( acErrMsg, "pun_intst=[%lf]!cmpd_intst_aj=[%lf]", pun_intst,cmpd_intst_aj);
			WRITEMSG
			flag++;
			ln_lo.lo_amt+=ln_lo_tmp.lo_amt; 	/* Ƿ��� */
			ln_lo.lo_intst+=ln_lo_tmp.lo_intst; 	/* Ƿ��Ϣ */
			ln_lo.pun_intst+=pun_intst+cmpd_intst_aj; 	/* ��Ϣ */
			ln_lo.pay_lo_amt+=ln_lo_tmp.pay_lo_amt; 			/* �ѻ�Ƿ��� */
			ln_lo.pay_lo_intst+=ln_lo_tmp.pay_lo_intst; 		/* �ѻ�Ƿ��Ϣ */
			ln_lo.pay_pun_intst+=ln_lo_tmp.pay_pun_intst ; 	/* �ѻ���Ϣ */				
			/***
			pay_lo_amt+=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+pun_intst+cmpd_intst_aj;* �ѻ���� *
			****/
		}
		
		Ln_lo_Clo_Sel ();
		
		/* ȡ������Ӧ����� */
	
  		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &ln_pay_pln , "ac_id=%ld and ac_seqn=%d"
					 , g_mdm_ac_rel.ac_id,v.ac_seqn );
		ret = Ln_pay_pln_Sel( g_pub_tx.reply , &t_ln_pay_pln , "ac_id=%ld and ac_seqn=%d"
					 , g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����ƻ�����Ϣ���޴���Ϣ![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L043");
			goto ErrExit;
		}else if( ret )
		  {
			sprintf(acErrMsg,"��ȡ�����ƻ�����Ϣ�쳣![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		  }		  
		sprintf(acErrMsg,"ȡ�����ƻ�����Ϣ PASS");
		WRITEMSG
		
		/* ����Ӧ����� */
		pub_base_deadlineM( ln_pay_pln.beg_date, -1, &dqr);		
		if(dqr > g_pub_tx.tx_date )
		{
			sprintf(acErrMsg,"Ԥ���ѻ���!");
			WRITEMSG
			ln_pay_pln.curr_amt=0;	/* ����Ӧ������ */
			ln_pay_pln.curr_intst=0;/* ����Ӧ����Ϣ */

			/****
			if( (g_ln_mst.opn_date==g_ln_mst.ic_date) && ln_pay_pln.curr_cnt==1 )*��һ��*
			{						
			
				sprintf(acErrMsg,"��һ��!");
				WRITEMSG
			***/
			/*���ڳ�ɳ�������ݲ�׼ȷ,���¼��㵱�ڻ���ƻ�*/
			ret = pub_ln_PayPlan_Qry( g_ln_mst , g_ln_parm , ln_pay_pln.curr_cnt, ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1,g_ln_mst.bal,&l_ln_pay_pln);	
			if (ret)
			{
				sprintf(acErrMsg,"���ú���pub_ln_PayPlan_qry����!");
				WRITEMSG
				return 1;	
			}
			ln_pay_pln.curr_amt= l_ln_pay_pln.curr_amt;				
			ln_pay_pln.curr_intst= l_ln_pay_pln.curr_intst;
			/***
			}
			****/
			sprintf(acErrMsg,"Ԥ���ѻ���!amt[%lf]intst[%lf]",ln_pay_pln.curr_amt,ln_pay_pln.curr_intst);
			WRITEMSG
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
				TRACE
		}
	
		/* ������ */
		if ( (cnt-flag) <=0 )
		{
			pay_amt = 0;
		}else if ( (cnt-flag) ==1 ){
			/**���ڳ�ɳ��������ת������,���Ի���ƻ��л�����Ҫ���¼��� */
			/****
			pay_amt =t_ln_pay_pln.curr_amt+t_ln_pay_pln.curr_intst ;
			*****/
			sprintf(acErrMsg,"ǰ:curr_amt=[%lf]curr_intst=[%lf]!",ln_pay_pln.curr_amt,ln_pay_pln.curr_intst);
			WRITEMSG			
			ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*������*/
			curr_cnt= ln_pay_pln.curr_cnt;		/*��ǰ����*/
			/* ȡǷ�������*/
			ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;
			ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;	
			bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);			/*ʣ�౾��*/				
			ret = pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt, bal, &v_ln_pay_pln);			
			if (ret)
			{
				sprintf(acErrMsg,"���ú���pub_ln_PayPlan_qry����!");
				WRITEMSG
				return 1;	
			}
			sprintf(acErrMsg,"��:curr_amt=[%lf]curr_intst=[%lf]!",v_ln_pay_pln.curr_amt,v_ln_pay_pln.curr_intst);
			WRITEMSG
			pay_amt=v_ln_pay_pln.curr_amt+v_ln_pay_pln.curr_intst;						

		}else{
			TRACE
			i=t_ln_pay_pln.curr_cnt+1;
			/***
			pay_amt =l_ln_pay_pln.curr_amt+l_ln_pay_pln.curr_intst ;
			****/
			ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*������*/
			curr_cnt= ln_pay_pln.curr_cnt;		/*��ǰ����*/
			/* ȡǷ�������*/
			ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;
			ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
			if(ret)	return 1;	
			bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);
			while ( curr_cnt<(t_ln_pay_pln.curr_cnt+(cnt-flag)) )
			{
				sprintf(acErrMsg,"������㺯����[%ld]��!!!!!",curr_cnt);
				WRITEMSG				
				ret = pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt, bal, &v_ln_pay_pln);						
				if (ret)
				{
					sprintf(acErrMsg,"���ú���pub_ln_PayPlan_qry����!");
					WRITEMSG
					return 1;	
				}
				pay_amt+=v_ln_pay_pln.curr_amt+v_ln_pay_pln.curr_intst;
				i++;
				bal=bal-v_ln_pay_pln.curr_amt;
				curr_cnt++;
				ttl_cnt--;		
				memset(&v_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );
				sprintf(acErrMsg,"++++++++++++++over++++++++++++++++++");
				WRITEMSG				
			}
		}
									
		/* ���ܽ�� */
		pay_lo_amt=ln_lo.pay_lo_amt+ln_lo.pay_lo_intst+ln_lo.pay_pun_intst;
		tol_lo=ln_lo.lo_amt+ln_lo.lo_intst+ln_lo.pun_intst-pay_lo_amt;/* Ƿ���ܼ� */

		tol=tol_lo+pay_amt;	/* �ܼ� */ 
		sprintf(acErrMsg,"tol_lo��%lf��pay_amt[%lf]!",tol_lo,pay_amt);
		WRITEMSG		
		
		/* ��� */ 
		set_zd_double("1094",g_ln_mst.bal);/* ����� */
		set_zd_long("109E",ln_pay_pln.ttl_cnt);	/* ������ */
		set_zd_long("109F",ln_pay_pln.curr_cnt-1-flag);	/* �ѻ��ڴ� */

		set_zd_int("1096",flag);	/* ��Ƿ���� */
		set_zd_double("1097",ln_lo.lo_amt+ln_lo.lo_intst-ln_lo.pay_lo_amt-ln_lo.pay_lo_intst);/* ��Ƿ�ܶ� */
		set_zd_double("1098",ln_lo.pun_intst-ln_lo.pay_pun_intst);/* ��Ϣ */		
		set_zd_double("109G",tol_lo);/* Ƿ���ܼ� */	
		
		set_zd_double("109I",ln_pay_pln.curr_amt);	/* ����Ӧ������ */		
		set_zd_double("109J",ln_pay_pln.curr_intst);	/* ����Ӧ����Ϣ */
		set_zd_double("1095",to_amt);	/* ���ڽ�� */				
		set_zd_double("109H",tol);	/* �ܼƽ�� */		
		
		set_zd_data("1093",g_ln_mst.name);/* ���� */			
	}

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
   
