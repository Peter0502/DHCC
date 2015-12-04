/*************************************************
* �� �� ��: spL306.c
* ����������   
*              �������ʵ���
*
* ��    ��:    lance
* ������ڣ�   2003��01��24��
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
#include "ln_expand_c.h"
	
char 	sts[2];
char	rate_name[5];
double 	rate=0.00,rate_parm=0.00,old_rate=0.00,new_rate=0.00;
static  struct ln_expand_c 	p_ln_expand;	
	
spL306()  
{ 		
	int ret=0 ,min_ret=0 ,max_ret=0;
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
	/* �����Ϣ�����Ƿ�Ϊ��
	if ( g_ln_mst.intst_acm > 0.005 )
		{
			sprintf(acErrMsg,"�����Ϣ����������![%lf]",g_ln_mst.intst_acm);
		  WRITEMSG
		  strcpy( g_pub_tx.reply,"D147" );
			goto ErrExit;
		} */
				
	/* ����������(��Ϊ����,ֻ���޸ķ�Ϣ����) */
	if( g_ln_parm.ln_pay_type[0]=='3' || g_ln_parm.ln_pay_type[0]=='4' )
	{
		sprintf(acErrMsg,"���Ҵ���![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		if ( sts[0]!='4' || sts[0]!='3' )
		{
			sprintf(acErrMsg,"���Ҵ���ֻ�ܵ������������뷣Ϣ����!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"L063" );
			goto ErrExit;
		}
		
		/* ����������� */
		if( sts[0]=='3' )
		{
			/* ȡ�������� */
		   	if( g_ln_parm.over_rate_type[0]=='0' )/* ȡ�������ʱ�� */
		   	{
		   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&rate_parm);
		   		if( ret ) goto ErrExit;					
		   		rate_parm = rate_parm/1.2;/* ת���������� */
		   	}else{
				rate_parm = g_ln_mst.rate ;
		   	}
		   	
			/**��Ϣ���ʼ��**/
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
						rate_parm*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.over_rate,g_ln_parm.over_rate_max,
					rate_parm*(100+g_ln_parm.over_rate_max)/100.00
								 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L127" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L126" );
				goto ErrExit;
			}	
		}   		
		/* ��鷣Ϣ���� */
		if( sts[0]=='4' )
		{
			/* ȡ��Ϣ���� */
		   	if( g_ln_parm.fine_rate_type[0]=='0' )/* ȡ��Ϣ���ʱ�� */
		   	{
		   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
								g_pub_tx.tx_date,&rate_parm);
		   		if( ret ) goto ErrExit;					
		   		rate_parm = rate_parm/1.2;/* ת���������� */
		   	}else{
				rate_parm = g_ln_mst.rate ;
		   	}
		   	
			/**��Ϣ���ʼ��**/
			if( pub_base_CompDblVal(g_ln_mst.fine_rate,
						rate_parm*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
			{
				vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
				sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
					g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
					rate_parm*(100+g_ln_parm.fine_rate_max)/100.00
								 );
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L217" );
				goto ErrExit;
			}
			if( pub_base_CompDblVal(g_ln_mst.over_rate,
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
			{
				sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
				WRITEMSG
				strcpy ( g_pub_tx.reply , "L218" );
				goto ErrExit;
			}	
		}   						
	}	
		
	/* �����������޸ķ�ʽ */
	if ( g_ln_parm.get_rate_type[0]!='1' )
	{
		sprintf( acErrMsg,"�ô����˻����ʲ����ֹ��޸�");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L058" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"�����������޸ķ�ʽ PASS!");
	WRITEMSG	
					
	/* ���ݴ���״̬�������޸ĵ����� */
	/* չ�� */
	if( g_ln_mst.ac_sts[0] =='1' && sts[0]=='1' )
	{
		if (g_ln_mst.mtr_date < g_pub_tx.tx_date)
		
		{
		  sprintf( acErrMsg,"�ô����˻�����չ��״̬,�����޸���������");
		  WRITEMSG
		  strcpy( g_pub_tx.reply ,"L059" );
		  ERR_DEAL	 		
		}
	}
	/* ���� */
	if( atoi(g_ln_mst.ac_sts)>=2 && atoi(g_ln_mst.ac_sts)<=4 )
	{
		if( sts[0]!='4' && sts[0]!='3')
		{
			sprintf( acErrMsg,"�ô����˻���������״̬,ֻ���޸����������뷣Ϣ����!");
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L060" );
			ERR_DEAL
		}	 		
	}		

	/* �ǼǷֶλ���������� */
	
	/* �ǼǷֶλ����� */
	/* ͳ�ƴ������ļ��л��� */
	strcpy( acm_type,"2");/*�����������ͣ�1ÿ���ۼ�2�䶯�ۼ�3�䶯�ۼƵ��㣩*/
	ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
						g_ln_parm.intst_days_type,acm_type,g_ln_mst.bal,0.00,
						&g_ln_mst.intst_acm,"0",0,0 );
	if( ret )
	{
		sprintf(acErrMsg,"���ú��������������!");
		WRITEMSG
		return -1;	
	}				
	ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '2',
		  g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.intst_acm,
		  g_ln_mst.rate, g_pub_tx.reply);
	if (ret)
	{
		sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
		WRITEMSG
		ERR_DEAL	
	}
	/* �����㸴�� */
	if ( g_ln_mst.cmpd_intst_ind[0]=='Y' )
	{
		TRACE
		
		/* ����ǷϢ���� */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
				g_ln_parm.intst_days_type,acm_type,g_ln_mst.in_lo_intst,0.00,
					&g_ln_mst.in_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}
		TRACE	
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '3',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.in_lo_acm,
			  g_ln_mst.rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			ERR_DEAL	
		}
		/* ����ǷϢ���� */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
				g_ln_parm.intst_days_type,acm_type,g_ln_mst.out_lo_intst,0.00,
					&g_ln_mst.out_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}				
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '4',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.out_lo_acm,
			  g_ln_mst.rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			ERR_DEAL	
		}	
		/* �������� */
		ret = pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
				g_ln_parm.intst_days_type,acm_type,g_ln_mst.cmpd_lo_intst,0.00,
					&g_ln_mst.cmpd_lo_acm,"0",0,0 );
		if( ret )
		{
			sprintf(acErrMsg,"���ú��������������!");
			WRITEMSG
			return -1;	
		}		
		ret = pub_base_InsertSectAcm(g_ln_mst.ac_id, g_ln_mst.ac_seqn, '5',
		 	 g_ln_mst.ic_date, g_pub_tx.tx_date, g_ln_mst.cmpd_lo_acm,
			  g_ln_mst.rate, g_pub_tx.reply);
		if (ret)
		{
			sprintf( acErrMsg,"���ú����ǼǷֶλ�����ʧ��");
			WRITEMSG
			ERR_DEAL	
		}		
	}
	/* ���´������ļ� */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "ִ��Ln_mst_Dec_Upd��!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "�������ļ����޴˼�¼" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "ִ��Ln_mst_Fet_Upd����!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    	/* �޸� */
	g_ln_mst.ic_date = g_pub_tx.tx_date ;		/* ��Ϣ���� */

	if (sts[0]=='1')
	{
		g_ln_mst.rate = rate ;		/* �������� */		
		
		sprintf(acErrMsg,"[%s]",g_ln_parm.rate_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG		
		ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &rate_parm );
		if( ret )
		{
			sprintf(acErrMsg,"���ø������ʱ��ȡ�����ʺ�������!");
			WRITEMSG
			goto ErrExit;	
		}
		rate_parm = rate_parm/1.2;/* ת���������� */
		
				
		/*�������ʼ��*/
		if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_parm*( (100+g_ln_parm.max_flt_ratio)/100.00 ) )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.max_flt_ratio,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%lf][%d] ",
					rate_parm,rate_parm*((100+g_ln_parm.max_flt_ratio)/100.00)
							,g_ln_parm.max_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A006" );
			goto ErrExit;
		}
            	sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_parm,
			rate_parm*((100+g_ln_parm.max_flt_ratio)/100.00) ,g_ln_parm.max_flt_ratio );
		WRITEMSG

		if( pub_base_CompDblVal(g_ln_mst.rate,
					rate_parm*((100+g_ln_parm.min_flt_ratio)/100.00) )<0 )
		{
			sprintf(acErrMsg,"������ʴ���ʱ[%lf][%lf][%d] ", rate_parm,
		 		rate_parm*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A005" );
			goto ErrExit;
		}
		sprintf(acErrMsg,"���������ȷʱ[%lf][%lf][%d] ", rate_parm,
				rate_parm*((100+g_ln_parm.min_flt_ratio)/100.00) ,g_ln_parm.min_flt_ratio );
			WRITEMSG
		sprintf(acErrMsg,"���ò�Ʒ�����������޶�(����) PASS!");
		WRITEMSG

		/* ����޸ĺ�������Ƿ���ϲ�ƷҪ�� *
		* ��������Ƿ񳬹��涨�����ϸ����¸���Ľ���*
		* ���ú����������ʱ��ȡ������(ֻȡ���ʣ����������޸���) *
		
		sprintf(acErrMsg,"[%s]",g_ln_parm.rate_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%s]",g_ln_parm.cur_no);
		WRITEMSG		
		sprintf(acErrMsg,"[%ld]",g_pub_tx.tx_date);
		WRITEMSG		
		ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &rate_parm );
		if( ret )
		{
			sprintf(acErrMsg,"���ø������ʱ��ȡ�����ʺ�������!");
			WRITEMSG
			goto ErrExit;	
		}
		rate_parm = rate_parm/1.2;* ת���������� *
	sprintf(acErrMsg,"���ò�Ʒ�����������޶�(����) PASS!");
	WRITEMSG
			
		min_ret = rate - rate_parm*(100+g_ln_parm.min_flt_ratio)/100;
		max_ret = rate - rate_parm*(100+g_ln_parm.max_flt_ratio)/100;
		
		if ( pub_base_CompDblVal( min_ret , 0.00 ) <0 )
		{
			strcpy(acErrMsg,"�������ʵ��ڹ涨��������ʣ�");
			WRITEMSG
			sprintf(acErrMsg,"[%lf] [%lf]",rate,rate_parm*(100+g_ln_parm.min_flt_ratio)/100);
			WRITEMSG
			sprintf(acErrMsg,"[%d] [%d]",g_ln_parm.min_flt_ratio,g_ln_parm.max_flt_ratio);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L005" );
			goto ErrExit;
		}
		if ( pub_base_CompDblVal( max_ret , 0.00 ) >0 )
		{
			strcpy(acErrMsg,"�������ʸ��ڹ涨��������ʣ�");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L006" );
			goto ErrExit;
		}
	sprintf(acErrMsg,"���ò�Ʒ�����������޶�(����) PASS!");
	WRITEMSG
	****/		

	}

	if (sts[0]=='2')
	{
		g_ln_mst.exp_rate = rate ;		/* չ������ */
		
		if( g_ln_mst.ac_id==100355044 )
		{ 
			;
		}else if( pub_base_CompDblVal(g_ln_mst.exp_rate,g_ln_mst.rate )<0 )
		{
			sprintf(acErrMsg,"չ�����ʹ���[%lf][%lf]!!",g_ln_mst.exp_rate,g_ln_mst.rate);
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L124" );
			goto ErrExit;
		}
		
		TRACE		
		/* �޸�չ�ڵǼǲ������һ����¼ */
   		 ret = Ln_expand_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d and exp_ind='%s' " ,g_ln_mst.ac_id , g_ln_mst.ac_seqn , g_ln_mst.exp_ind);
   		 if ( ret )
   		 {
       			 sprintf( acErrMsg, "ִ��Ln_expand_Dec_Upd��!ret=[%d]",ret );
       			 WRITEMSG
       			 ERR_DEAL
   		 }
    		 TRACE
  		 ret = Ln_expand_Fet_Upd( &p_ln_expand,g_pub_tx.reply );   
    		 if ( ret == 100 )
    		 {
       			sprintf( acErrMsg, "����չ�ڵǼǲ����޴˼�¼" );
        		WRITEMSG
				strcpy(g_pub_tx.reply, "L225");
        		ERR_DEAL
    		 }else if ( ret )
    		 {
       			 sprintf( acErrMsg, "ִ��Ln_expand_Fet_Upd����!ret=[%d]",ret );
       			 WRITEMSG
       			 ERR_DEAL
    		 }
    		 TRACE
    		 p_ln_expand.exp_rate = rate ; /* ����չ������ */
		 ret = Ln_expand_Upd_Upd( p_ln_expand, g_pub_tx.reply );
		 if( ret )
		 {
			sprintf( acErrMsg,"ִ��Ln_expand_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
		TRACE
		Ln_expand_Clo_Upd();
		sprintf(acErrMsg,"���´���չ�ڵǼǲ� PASS!");
		WRITEMSG    		 	
	}
	if (sts[0]=='3')
	{
		g_ln_mst.over_rate = rate ;		/* �������� */	
		
		if( g_ln_parm.over_rate_type[0]=='0' )/* ȡ�������ʱ�� */
		{
	   		ret = pub_base_getllz(g_ln_parm.over_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&rate_parm);
	   		if( ret ) goto ErrExit;					
	   		rate_parm = rate_parm/1.2;/* ת���������� */
		}else{
			rate_parm = g_ln_mst.rate ;
		}		

		/**����\��Ϣ���ʼ��**/
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*(100+g_ln_parm.over_rate_max)/100.00 )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
			g_ln_mst.over_rate,g_ln_parm.over_rate_max,
			rate_parm*(100+g_ln_parm.over_rate_max)/100.00
							 );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L062" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L061" );
			goto ErrExit;
		}		
						
		/**�������ʼ��(ԭ��)**
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*(100+g_ln_parm.over_rate_max)/100 )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.over_rate_max,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
						g_ln_mst.over_rate,g_ln_parm.over_rate_max,rate_parm*(100+g_ln_parm.over_rate_max)/100
							 );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L062" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*(1+g_ln_parm.over_rate_min/100) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high[%lf]!!",
						rate_parm*(1+g_ln_parm.over_rate_min/100));
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L061" );
			goto ErrExit;
		}			
		sprintf(acErrMsg,"����������� PASS!");
		WRITEMSG	
		***************/	

	}
		/* ��鷣Ϣ���� */
	if( sts[0]=='4' )
	{
		g_ln_mst.fine_rate = rate ;		/* �������� */	
		/* ȡ��Ϣ���� */
	   	if( g_ln_parm.fine_rate_type[0]=='0' )/* ȡ��Ϣ���ʱ�� */
	   	{
	   		ret = pub_base_getllz(g_ln_parm.fine_rate_no,g_ln_parm.cur_no,
							g_pub_tx.tx_date,&rate_parm);
	   		if( ret ) goto ErrExit;					
	   		rate_parm = rate_parm/1.2;/* ת���������� */
	   	}else{
			rate_parm = g_ln_mst.rate ;
	   	}
	   	
		/**��Ϣ���ʼ��**/
		if( pub_base_CompDblVal(g_ln_mst.fine_rate,
					rate_parm*(100+g_ln_parm.fine_rate_max)/100.00 )>0 )
		{
			vtcp_log("=[%d][%s]",g_ln_parm.fine_rate_max,g_ln_mst.prdt_no );
			sprintf(acErrMsg,"current rate is too high[%lf][%d][%lf] ",
				g_ln_mst.fine_rate,g_ln_parm.fine_rate_max,
				rate_parm*(100+g_ln_parm.fine_rate_max)/100.00
							 );
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L217" );
			goto ErrExit;
		}
		if( pub_base_CompDblVal(g_ln_mst.over_rate,
					rate_parm*((100+g_ln_parm.over_rate_min)/100.00) )<0 )
		{
			sprintf(acErrMsg,"current rate is too high[%lf]!!",
					rate_parm*((100+g_ln_parm.over_rate_min)/100.00));
			WRITEMSG
			strcpy ( g_pub_tx.reply , "L218" );
			goto ErrExit;
		}	
	}   		
	
	
		
					
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "�޸Ĵ������ļ���,�����ظ���¼!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"ִ��Ln_mst_Upd_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	if( ret )
	{
			sprintf( acErrMsg,"ִ��Ln_mst_CLo_Upd��!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
	}
	sprintf(acErrMsg,"���´������ļ� PASS!");
	WRITEMSG
	
					
	/* Pub_tx�ṹ��ֵ */

	strcpy(g_pub_tx.add_ind,"1");		/* ����:0��1��2���� */	
	sprintf(acErrMsg,"����:[%s]",g_pub_tx.add_ind );
	WRITEMSG

	strcpy( g_pub_tx.ct_ind,"2");		/* ��ת:1��2ת */	
	sprintf(acErrMsg,"����:[%s]",g_pub_tx.ct_ind );
	WRITEMSG

	/* �Ƿ��ӡ����(�������۱�־):1��0�� */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3063;			
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );	
	strcpy ( g_pub_tx.brf,"�������ʵ���" );	
	sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
	WRITEMSG
		
	/*** ���׼��ʴ��� ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"���׼��ʴ������!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG
	
	/* ��ӡ���� */
	/* ���ڴ�Ʊ--���ʵ������� */
	ret = print_LNBKLL( );			
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKLL,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}	
	TRACE			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�������ʵ����ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"�������ʵ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	memset ( &p_ln_expand ,0x00 ,sizeof(struct ln_expand_c) );
	return 0;	
}

int data_set_struct()
{    
	int ret=0;
	
	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* �����ʺ� */
	sprintf(acErrMsg,"�����ʺ�[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* �����˻���� */
	sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_data("0710",sts);			/* �������� */
	sprintf(acErrMsg,"��������[%s]",sts);
	WRITEMSG
	
	
	get_zd_double("0840",&rate);			/* ���� */
	sprintf(acErrMsg,"����[%lf]",rate);
	WRITEMSG	
	new_rate = rate ;
	
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
	if( ret ) {return ret;}		
	
	if( sts[0]=='1' )		/* �������� */
	{
		old_rate= g_ln_mst.rate;
		strcpy(rate_name, "����");	
	}else if( sts[0]=='2' ){	/* չ������ */
		old_rate= g_ln_mst.exp_rate;
		strcpy(rate_name, "չ��");
	}else if( sts[0]=='3' ){	/* �������� */
		old_rate= g_ln_mst.over_rate;
		strcpy(rate_name, "����");
	}else if( sts[0]=='4' ){	/* ��Ϣ���� */
		old_rate= g_ln_mst.fine_rate;
		strcpy(rate_name, "��Ϣ");
	}		 
				
	return 0;
}

/* ��ӡ--�������ʵ�����Ʊ */
int print_LNBKLL( )
{
	char tx_type[9];
	char rate[11];
	char exp_ind[5];
	char old_rate_str[11];
	char rate_str[11];
			
	int ret_tmp=0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	memset(tx_type,0x00,sizeof(tx_type));
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKLL���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */					
	sprintf( old_rate_str , "%8.5lf��" , old_rate );
	sprintf( rate_str , "%8.5lf��" ,new_rate );
	pub_base_strpack( old_rate_str );
	pub_base_strpack( rate_str );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.�������ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.ԭ �� �ʡ� 13.�� �� �ʡ�14.��Ŀ�š�   15.��������  */
	/* 16.��ˮ�š�   */
	fprintf(fp,"LNBKLL%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	0.00 , 0 , 0 , 0.00 , 0.00 ,
	0.00 , old_rate_str , rate_str , "", rate_name , 
	g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
