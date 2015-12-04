/*************************************************
* �� �� ��: spL203.c
* ����������   
*              ���Ҵ����
*
* ��    ��:    bob
* ������ڣ�   2003��01��19��
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
static struct ln_pay_pln_c	v_ln_pay_pln;
static struct ln_pay_pln_c	pri_ln_pay_pln;
static struct S_ln_intst vln_intst[LN_INTST_TYPS];
int cnt=0;
int payed_cnt=0;

char ct_flag[2];
double print_tol_amt=0.00;
double tx_amt1=0.00 , tx_amt2=0.00 , tx_amt3=0.00 , tx_amt4=0.00 ;
double tx_amt2_new=0.00 , tx_amt2_old=0.00 , tx_amt3_old=0.00;
long   ln_bj_beg_date=0, ln_bj_end_date=0;
long   ln_lx_beg_date=0, ln_lx_end_date=0;
long   ln_fx_beg_date=0, ln_fx_end_date=0;	

spL203()  
{ 	
	double amt_tmp=0.00,amt_lo=0.00,pay_amt=0.00;
	double pun_intst=0.00,cmpd_intst_aj=0.00;
	char sts[2];
	char amt_str[5];
	int ret=0,i=0,flag=0,pay_flag=0;
	double old_amt1=0.00 , old_amt2=0.00, old_amt3=0.00 , old_amt4=0.00;
	char clear_sts[2];
	int ins_rol_ind=0;	
	
	int ttl_cnt=0,curr_cnt=0;
	double bal=0.00,lo_tol_amt=0.00,lo_pay_amt=0.00;
		
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
	/* ȡǷ�������*/
	ret= sql_sum_double ( "ln_lo" , "lo_amt" , &lo_tol_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if(ret)	goto ErrExit;
	ret= sql_sum_double ( "ln_lo" , "pay_lo_amt" , &lo_pay_amt , "ac_id=%ld and ac_seqn=%d and pay_type='0'", g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if(ret)	goto ErrExit;	
	/* ���°���Ƿ��� */
 	ins_rol_ind=0;	 	/* �Ǽǳ����������־(0-�Ǽ� ��0-���Ǽ�) */	
	ret = Ln_lo_Dec_Upd ( g_pub_tx.reply , 
				"ac_id=%ld and ac_seqn=%d  and pay_type='0'", 
				g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "Ln_lo_Dec_Sel��!ret=[%d]", ret);
		WRITEMSG		
		goto ErrExit;	
	}
	sprintf(acErrMsg,"ac_id[%ld]ac_seqn=[%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn);
	WRITEMSG				
	flag=0;	/* ��Ƿ���� */
	TRACE
	while(1)
	{
		ret = Ln_lo_Fet_Upd( &ln_lo, g_pub_tx.reply );
   		if( ret && ret!=100 )
 		{
	 		sprintf( acErrMsg,"fetch Ln_lo_Fet_Sel error code = [%d]",ret);
	 		WRITEMSG
	 		goto ErrExit;
 		}else if( ret==100 ){
			TRACE
			break;	
		}
		TRACE
	 	sprintf( acErrMsg,"���뻹Ƿ��֣�����");
	 	WRITEMSG
 		/* �Ǽǰ���Ƿ����Ǽǲ� */
 		ret = Ins_ln_lo_rol( ln_lo );
 		if( ret )
 		{
	 		sprintf( acErrMsg,"���õǼǰ���Ƿ����Ǽǲ������쳣����!");
	 		WRITEMSG
	 		goto ErrExit;
		}
 		/***
 		ins_rol_ind++;	 			
	 	*****/
	 	
		/* ����� */
		if(ln_lo.old_ac_ind[0]=='1'||ln_lo.old_ac_ind[0]=='2' )	
		/* ��ϵͳ���ݣ���Ԥ�����Ѿ�����ڻ��߱��⡣����Ҫ�� ��133 ��705*/
		{
	 		sprintf(acErrMsg,"����������ǷϢ����%lf��!",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			/* ���㷣Ϣ��� */
			/****
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt-ln_lo.pay_lo_amt,ln_lo.lo_intst-ln_lo.pay_lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj);  
			   ****/
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt,ln_lo.lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj); 			             	
	        if (ret) goto ErrExit;
	 		sprintf(acErrMsg,"��%lf��!��%lf��",pun_intst,cmpd_intst_aj);
	 		WRITEMSG	                	
	        /*���û���ƻ�����ȷ������ǷϢ�Ļ�����*/
	        i=0;
	        vln_intst[i].intst_amt=ln_lo.lo_amt-ln_lo.pay_lo_amt; 
			vln_intst[i].type=0;
	        i++;
	        vln_intst[i].intst_amt=ln_lo.lo_intst-ln_lo.pay_lo_intst; 
			vln_intst[i].type=1;
	        i++;
	        vln_intst[i].intst_amt=ln_lo.pun_intst+(pun_intst+cmpd_intst_aj)-
									ln_lo.pay_pun_intst;
			vln_intst[i].type=2;
	        i++;
	        ret=pub_ln_OrderPay( &vln_intst,g_ln_parm.pay_seq_code,
											&g_pub_tx.tmp_amt1 );
	        if( ret ) goto ErrExit;
	                
	 		sprintf( acErrMsg,"������ǷϢ����%lf����!",g_pub_tx.tmp_amt1);
	 		WRITEMSG	
	 		               
	        old_amt1=vln_intst[0].pay_amt;
	        if( ln_lo.old_ac_ind[0]=='1' )
	        {
		 		sprintf( acErrMsg,"!!!!!!!lance!old_amt2=[%lf]",old_amt2);
		 		WRITEMSG	                	
	        	old_amt2+=vln_intst[1].pay_amt;
		 		sprintf( acErrMsg,"!!!!!!!lance!old_amt2=[%lf]",old_amt2);
		 		WRITEMSG
	        }else if( ln_lo.old_ac_ind[0]=='2' ){
	            old_amt3+=vln_intst[1].pay_amt;	                	
	        }
	        if( pub_base_CompDblVal(vln_intst[2].pay_amt,ln_lo.pun_intst )>0)
	        {
	           	old_amt4+= ln_lo.pun_intst;
	        }else{
	           	old_amt4+= vln_intst[2].pay_amt;
	        }	                
		    ln_lo.pay_lo_amt+=vln_intst[0].pay_amt;
		    ln_lo.pay_lo_intst+=vln_intst[1].pay_amt;
		    ln_lo.pay_pun_intst+=vln_intst[2].pay_amt;
	 		sprintf( acErrMsg,"pay_lo_amt����%lf����ln_lo.lo_amt=[%lf]!",
									ln_lo.pay_lo_amt,ln_lo.lo_amt);
	 		WRITEMSG
	 		sprintf( acErrMsg,"pay_lo_intst����%lf��ln_lo.lo_intst=[%lf]!",
									ln_lo.pay_lo_intst,ln_lo.lo_intst);
	 		WRITEMSG
	 		sprintf( acErrMsg,"pay_pun_intst����%lf��ln_lo.pun_intst=[%lf]\
						+++[%lf]",ln_lo.pay_pun_intst,ln_lo.pun_intst,
						pun_intst+cmpd_intst_aj);
	 		WRITEMSG	 			 				            

		    pay_flag=0;    /*�Ƿ����־*/
	        ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	        if( ret ) pay_flag++;  
	        ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	        if( ret ) pay_flag++;             
	        ret = pub_base_CompDblVal(pun_intst+cmpd_intst_aj,
												ln_lo.pay_pun_intst);
	        if( ret ) pay_flag++;
	        if( pay_flag==0 )     
	        {
	            TRACE
	            /* �޸��Ƿ����־ 0-δ���� 1-���� */
	            strcpy( ln_lo.pay_type , "1" );
	            flag++;
	            /* ��Ҫ�ǼǴ���Ǽǲ� */
	            /* ���� 
	            ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,
												"1",ln_lo.lo_amt,0.00,"0",1 );
			    if( ret ) 	goto ErrExit;	
		    */            	    
	            /* ��Ϣ */
	          	/* ���� */
	           	/*********
	           	if( ln_lo.old_ac_ind[0]=='1' )
	           	{
	           		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
							,g_pub_tx.tx_date,"2",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	goto ErrExit;	
	            }
	            ******/
	            /* ���� */
	            /*******
	            if( ln_lo.old_ac_ind[0]=='2' )
	            {
	            	ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
							,g_pub_tx.tx_date,"3",ln_lo.lo_intst,0.00,"0",2 );
					if( ret ) 	goto ErrExit;	
	           	}
	           ************/	          	    	
	            /* ����������ϵͳ ֻ��һ�� */
	            /* ��Ϣ *
	           		* ���� *
	             	if( ln_lo.old_ac_ind[0]=='1' )
	              	{
	              		ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
							g_pub_tx.tx_date,"2",ln_lo.pun_intst +
							(pun_intst+cmpd_intst_aj),0.00,"0",2 );
						if( ret ) 	goto ErrExit;	
	              	}
	               	* ���� *
	               	if( ln_lo.old_ac_ind[0]=='2' )
	               	{
	               		ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
							g_pub_tx.tx_date,"3",ln_lo.pun_intst + 
							(pun_intst+cmpd_intst_aj),0.00,"0",2 );
						if( ret ) 	goto ErrExit;	
	              	}	
	           	********/	            	    
	       	}            
	                
	        /* ��Ҫ���µļ��˷�ʽ���˵Ľ�� */
	        g_pub_tx.tx_amt1+=old_amt1;
	        if( pub_base_CompDblVal(vln_intst[2].pay_amt,ln_lo.pun_intst )>0 )
	        {
	           	/* ���˻���ϵͳ����⣬Ҳ��������ĸ�Ϣ�ͷ�Ϣ */ 
	           	g_pub_tx.tx_amt4 = g_pub_tx.tx_amt4 + 
								(vln_intst[2].pay_amt - ln_lo.pun_intst) ;
	        }  
	
	    }else if( ln_lo.old_ac_ind[0]=='0' ){
	 			sprintf(acErrMsg,"������ǷϢ����%lf��!",g_pub_tx.tmp_amt1);
	 			WRITEMSG			
				/* ��ϵͳ���ݴ���ʽ */
				/* ���㷣Ϣ��� */
				/****
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt-ln_lo.pay_lo_amt ,
					ln_lo.lo_intst-ln_lo.pay_lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);  
				*******/
				
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt ,
					ln_lo.lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);  
								          
	            if (ret) break;
	            /*���û���ƻ�����ȷ������ǷϢ�Ļ�����*/
				memset( vln_intst,0,sizeof(vln_intst) );
	            i=0;
	            vln_intst[i].intst_amt=ln_lo.lo_amt-ln_lo.pay_lo_amt; 
				vln_intst[i].type=0;
	            i++;
	            vln_intst[i].intst_amt=ln_lo.lo_intst-ln_lo.pay_lo_intst; 
				vln_intst[i].type=1;
	            i++;
	            vln_intst[i].intst_amt=pun_intst+cmpd_intst_aj-
											ln_lo.pay_pun_intst; 
				vln_intst[i].type=2;
	            i++;
	            ret=pub_ln_OrderPay(&vln_intst,g_ln_parm.pay_seq_code,
														&g_pub_tx.tmp_amt1 );
	           	if( ret ) goto ErrExit;
		 		sprintf( acErrMsg,"������ǷϢ����%lf��!",g_pub_tx.tmp_amt1);
	 			WRITEMSG
	 		
	            g_pub_tx.tx_amt1+=vln_intst[0].pay_amt;
	            g_pub_tx.tx_amt2+=vln_intst[1].pay_amt;
	            g_pub_tx.tx_amt4+=vln_intst[2].pay_amt; 
	            
	            ln_lo.pay_lo_amt+=vln_intst[0].pay_amt;
	            ln_lo.pay_lo_intst+=vln_intst[1].pay_amt; 
	            ln_lo.pay_pun_intst+=vln_intst[2].pay_amt;
	                
	 			sprintf( acErrMsg,"pay_lo_amt����%lf��ln_lo.lo_amt=[%lf]!",
											ln_lo.pay_lo_amt,ln_lo.lo_amt);
	 			WRITEMSG
	 			sprintf(acErrMsg,"pay_lo_intst��%lf��ln_lo.lo_intst=[%lf]!",
											ln_lo.pay_lo_intst,ln_lo.lo_intst);
	 			WRITEMSG
	 			sprintf(acErrMsg,"pay_pun_intst����%lf����ln_lo.pun_intst\
								=[%lf]++[%lf]!",ln_lo.pay_pun_intst,
								ln_lo.pun_intst,pun_intst+cmpd_intst_aj);
	 			WRITEMSG
	 			            
	            pay_flag=0;    /*�Ƿ����־*/
	            ret = pub_base_CompDblVal(ln_lo.lo_amt, ln_lo.pay_lo_amt );
	            if( ret ) pay_flag++;  
	            ret = pub_base_CompDblVal(ln_lo.lo_intst, ln_lo.pay_lo_intst );
	            if( ret ) pay_flag++;             
	            ret = pub_base_CompDblVal( pun_intst+cmpd_intst_aj , 
														ln_lo.pay_pun_intst );
	            if( ret ) pay_flag++;
	            if( pay_flag==0 )     
	            {
	                TRACE
	                /* �޸��Ƿ����־ 0-δ���� 1-���� */
	                strcpy( ln_lo.pay_type , "1" );
	                flag++;
	                /* �ǼǴ���Ǽǲ� */
	                /* ����Ǽǲ�--���� *
	                ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
							ln_lo.shld_pay_date,"1",ln_lo.lo_amt,0.00,"0",1 );
			   		if( ret ) 	goto ErrExit;
	            	* ����Ǽǲ�--��Ϣ *
	            	ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
						ln_lo.shld_pay_date,"2",ln_lo.lo_intst,0.00,"0",2 );
			   		if( ret ) 	goto ErrExit;	
	            	* ����Ǽǲ�--��Ϣ *
	            	ret = pub_ln_ln_reg( g_ln_mst,ln_lo.shld_pay_date,
					   g_pub_tx.tx_date,"4",pun_intst+cmpd_intst_aj,0.00,"0",2);
			   	 	if( ret ) 	goto ErrExit;		
			    	*************/	                	    
	            } 	        
	        }
            ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
            if ( ret )
            {
                 sprintf( acErrMsg,"Ln_lo_Upd_Upd ����[%d]",ret);
	             WRITEMSG
                 goto ErrExit;
            }
            memset( &ln_lo , 0x00 , sizeof( struct ln_lo_c ) );	                
            memset( vln_intst,0,sizeof(vln_intst) );        

/***************************                                                                                                        
		* ��Ƿ�������(�ѹ黹���+������) *
		amt_tmp = ln_lo.pay_amt + g_pub_tx.tmp_amt1 ;
		amt_lo = ln_lo.lo_amt + ln_lo.lo_intst + ln_lo.pun_intst ;
		
		if ( pub_base_CompDblVal( amt_lo , amt_tmp ) == -1 )
		{
			g_pub_tx.tx_amt1+= ln_lo.lo_amt;
			g_pub_tx.tmp_amt1-=(amt_tmp-amt_lo);

			ln_lo.pay_amt = amt_lo ;* �ѹ黹��� *
			strcpy( sts,"1" );
			
			sprintf( acErrMsg,"Ƿ����С��(�ѹ黹���+������)[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
		}		
		else if ( pub_base_CompDblVal( amt_lo , amt_tmp ) == 1 )
		{
			g_pub_tx.tmp_amt1 = 0;
			ln_lo.pay_amt = amt_tmp ;	* �ѹ黹��� *
			strcpy( sts,"0" );

			sprintf( acErrMsg,"Ƿ�������(�ѹ黹���+������)[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			break;
		}
		* ��Ƿ�������(�ѹ黹���+������) *		
		if ( pub_base_CompDblVal( amt_lo , amt_tmp ) == 0 )
		{
			g_pub_tx.tx_amt1+= ln_lo.lo_amt;
			g_pub_tx.tmp_amt1 = 0;
			ln_lo.pay_amt = amt_tmp ;	* �ѹ黹��� *
			strcpy( sts,"1" );
			
			sprintf( acErrMsg,"Ƿ�������(�ѹ黹���+������)[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			break;
		}	
	
		* ���� �ѹ黹���,Ƿ��״̬ *
		ret = sql_execute( "UPDATE ln_lo SET pay_amt=%lf,sts='%s' WHERE ac_id=%ld and ac_seqn=%d and pay_type='0' " , 
			ln_lo.pay_amt,sts,ln_lo.ac_id,ln_lo.ac_seqn );
		if( ret ) return ret;
**************************/		
	}
	Ln_lo_Clo_Upd ();
	sprintf( acErrMsg,"!!!!!cnt[%d]flag[%d]!!!!!!!",cnt,flag);
	WRITEMSG	
	sprintf( acErrMsg,"ǷϢ����ʣ�ࡾ��%lf����!",g_pub_tx.tmp_amt1);
	WRITEMSG
	if ( (cnt-flag)<=0 )
	{
		if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
		{
			sprintf( acErrMsg,"��������Ҫ������[%lf]",g_pub_tx.tmp_amt1);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply,"L039" );
			goto ErrExit;
		}
	}
	sprintf( acErrMsg,"!!!!!cnt[%d]flag[%d]",cnt,flag);
	WRITEMSG
	
		
	if (cnt-flag)
	{
		ins_rol_ind=0;	 	/* �Ǽǳ����������־(0-�Ǽ� ��0-���Ǽ�) */	
 	
		/* ȡ�����ƻ�����Ϣ	*/
		ret = Ln_pay_pln_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d"
					 , g_ln_mst.ac_id,g_ln_mst.ac_seqn );
		if ( ret )
		{
			sprintf( acErrMsg, "ִ��Ln_pay_Dec_Upd��!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
		}
		ret = Ln_pay_pln_Fet_Upd( &ln_pay_pln, g_pub_tx.reply );
		if( ret==100)
		{
			sprintf(acErrMsg,"���һ���ƻ���������Ӧ��¼",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L044");
			ERR_DEAL;
		}else if ( ret ){
			sprintf(acErrMsg,"ȡ���һ���ƻ�����Ϣ�쳣",g_pub_tx.reply);
			WRITEMSG
			ERR_DEAL;
		} 

		/* �Ǽǳ��������� */
 		if( !ins_rol_ind )
 		{
 			/* �Ǽǰ���Ƿ����Ǽǲ� */
 			ret = Ins_ln_pay_pln_rol( ln_pay_pln );
 			if( ret )
 			{
	 			sprintf(acErrMsg,"���õǼǰ��һ���ƻ������Ǽǲ������쳣����!");
	 			WRITEMSG
	 			goto ErrExit;
			}
		}
		ins_rol_ind++;	
		i=ln_pay_pln.curr_cnt;
		
		ttl_cnt= ln_pay_pln.ttl_cnt-ln_pay_pln.curr_cnt+1;		/*������*/
		curr_cnt= ln_pay_pln.curr_cnt;		/*��ǰ����*/

		bal= g_ln_mst.bal-(lo_tol_amt-lo_pay_amt);			/*ʣ�౾��*/
				
		while(i<=(ln_pay_pln.curr_cnt+cnt-flag))/*��ǰ����+��������-Ƿ����*/
		{
			ret= pub_ln_PayPlan_Qry( g_ln_mst, g_ln_parm, curr_cnt, ttl_cnt,bal, &v_ln_pay_pln );

			/****
			ret = pub_ln_PayPlan_qry( g_ln_mst , g_ln_parm , i , &v_ln_pay_pln ,0);	
			*****/
			if (ret)
			{
				sprintf(acErrMsg,"���ú���pub_ln_PayPlan����!");
				WRITEMSG
				ERR_DEAL	
			}
			if ( i<(ln_pay_pln.curr_cnt+cnt-flag) )
			{
				sprintf(acErrMsg,"v_ln_pay_pln.curr_amt=[%lf],\
						v_ln_pay_pln.curr_intst=[%lf]!",v_ln_pay_pln.curr_amt,
						v_ln_pay_pln.curr_intst);
				WRITEMSG			
				pay_amt=v_ln_pay_pln.curr_amt+v_ln_pay_pln.curr_intst;
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1 - pay_amt ;
			}
		
			i++;
			bal=bal-v_ln_pay_pln.curr_amt;
			curr_cnt++;
			ttl_cnt--;			
			
			if ( i <= (ln_pay_pln.curr_cnt+cnt-flag) )
			{
				g_pub_tx.tx_amt1+= v_ln_pay_pln.curr_amt;
				g_pub_tx.tx_amt2+= v_ln_pay_pln.curr_intst;
			
	           	/* ����Ǽǲ�--���� *
	           	ret = pub_ln_ln_reg( g_ln_mst,v_ln_pay_pln.end_date,
					v_ln_pay_pln.end_date,"1",v_ln_pay_pln.curr_amt,0.00,"0",1);
				if( ret ) 	goto ErrExit;
	           	* ����Ǽǲ�--��Ϣ *		
			ret = pub_ln_ln_reg( g_ln_mst,v_ln_pay_pln.end_date,v_ln_pay_pln.end_date,"2",v_ln_pay_pln.curr_intst,0.00,"0",2 );
			if( ret ) 	goto ErrExit;	
								**************/
			}
		
		WRITEMSG

		strcpy(v_ln_pay_pln.rowid, ln_pay_pln.rowid);

		if ( i< ln_pay_pln.ttl_cnt )
		{
			/* �޸Ļ���ƻ� */
			ret = Ln_pay_pln_Upd_Upd( v_ln_pay_pln , g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"���»���ƻ�ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L045");
				goto ErrExit;
			}
			
			sprintf(acErrMsg,"������һ�ڻ���ƻ��� PASS!");
			WRITEMSG						
			memset(&v_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );		
		}else if (i==ln_pay_pln.ttl_cnt ){
			/* �޸Ļ���ƻ� */
			v_ln_pay_pln.pay_ind[0] = '1'; 
			ret = Ln_pay_pln_Upd_Upd( v_ln_pay_pln , g_pub_tx.reply);
			if( ret )
			{
				sprintf(acErrMsg,"���»���ƻ�ʧ��![%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"L045");
				goto ErrExit;
			}			
			sprintf(acErrMsg,"������һ�ڻ���ƻ��� PASS!");
			WRITEMSG						
			memset(&v_ln_pay_pln , 0x00 , sizeof(struct ln_pay_pln_c) );	
		}
	}
	
	Ln_pay_pln_Clo_Upd();
}
		
	/* �Ƚϻ��� */	
	g_pub_tx.tmp_amt1 = pub_base_PubDround_1(g_pub_tx.tmp_amt1);
	if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
	{
		sprintf(acErrMsg,"��������Ӧ������[%lf]",g_pub_tx.tmp_amt1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L046");
		ERR_DEAL				
	}
	
	sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf]",g_pub_tx.tx_amt1);
	WRITEMSG
	/* Pub_tx�ṹ��ֵ */
	set_zd_data("0210",g_ln_parm.cur_no);    
	set_zd_double("0430",g_pub_tx.tx_amt2); 
	set_zd_double("0410",g_pub_tx.tx_amt4);
	set_zd_double("0400",g_pub_tx.tx_amt1);
	

	
	sprintf(acErrMsg,"g_pub_tx.tx_amt1[%lf][%lf][%lf]",g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt4);
	WRITEMSG	
	
	/*���ǼǴ���Ǽǲ�,��ϵͳ����ֻ�Ǽ�һ�� */
	/*** ��ʼ��������  *****/
  /* ����Ǽǲ�--���� */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"1",g_pub_tx.tx_amt1,0.00,"0",1 );
  if( ret ) 	goto ErrExit;
  /* ����Ǽǲ�--��Ϣ */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",2 );
  if( ret ) 	goto ErrExit;	
  /* ����Ǽǲ�--��Ϣ */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"4",g_pub_tx.tx_amt4,0.00,"0",2 );
  if( ret ) 	goto ErrExit;			
  
	/* �������ˮ */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt1 , 0.00 ) )        
	{   
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		g_pub_tx.svc_ind = 3010 ;	
		strcpy ( g_pub_tx.brf,"���һ���𲿷�" );
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
	/* ��Ϣ����ˮ */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
	{   
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		g_pub_tx.svc_ind = 3068 ;	
		strcpy ( g_pub_tx.brf,"���һ�����Ϣ����" );
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
	/* ��Ϣ����ˮ */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )        
	{   
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		g_pub_tx.svc_ind = 3069 ;	
		strcpy ( g_pub_tx.brf,"���һ��Ϣ����" );
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
	
	/* ��ӡ������ */
	tx_amt1 = g_pub_tx.tx_amt1;		/* ���� */
	tx_amt2 = g_pub_tx.tx_amt2+old_amt2+old_amt3; /* ��Ϣ */
	tx_amt2_new = g_pub_tx.tx_amt2;
	tx_amt2_old = old_amt2;          
	tx_amt3_old = old_amt3;/*����Ϣ*/
	tx_amt4 = g_pub_tx.tx_amt4+old_amt4;/*��Ϣ*/

	memset( amt_str , 0x00 , sizeof( amt_str ) );
	 
	if( pub_base_CompDblVal( tx_amt1 , 0.00 ) )/* ���� */
	{
		TRACE
		amt_str[0]='1';
	}else{
		TRACE
		amt_str[0]='*';
	}	
	if( pub_base_CompDblVal( tx_amt2_new , 0.00 ) )/* �����ݱ�����Ϣ */
	{
		TRACE
		amt_str[1]='1';
	}else{
		TRACE
		amt_str[1]='*';
	}
	if( pub_base_CompDblVal( tx_amt4 , 0.00 ) )/*��Ϣ*/
	{
		TRACE
		amt_str[2]='1';
	}else{
		TRACE
		amt_str[2]='*';
	}
	if( pub_base_CompDblVal( tx_amt2_old , 0.00 ) )/* �����ݱ�����Ϣ */
	{
		TRACE
		amt_str[3]='1';
	}else{
		TRACE
		amt_str[3]='*';
	}
	if( pub_base_CompDblVal( tx_amt3_old , 0.00 ) )/* �����ݱ�����Ϣ */
	{
		TRACE
		amt_str[4]='1';
	}else{
		TRACE
		amt_str[4]='*';
	}	
	set_zd_data( "0630" , amt_str );      
	sprintf(acErrMsg,"��ӡ����amt_str=[%s] !",amt_str);
	WRITEMSG	
	                  
	/** ��ϵͳ���ݴ����� */
	set_zd_data("0210",g_ln_parm.cur_no);    
	set_zd_double("1001",old_amt2); 
	set_zd_double("1002",old_amt3);
	set_zd_double("1003",old_amt4);
		        

        g_pub_tx.tx_amt1 = 0.00;
        g_pub_tx.tx_amt2 = old_amt2 ;
        
	/* ���� */
	if( pub_base_CompDblVal(old_amt2, 0.00 ) )
	{
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
			,g_pub_tx.tx_date,"2",old_amt2,0.00,"0",2 );
		if( ret ) 	goto ErrExit;	
	}
	/* ���� */
	if( pub_base_CompDblVal(old_amt3, 0.00 ) )
	{
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
				,g_pub_tx.tx_date,"3",old_amt3,0.00,"0",2 );
		if( ret ) 	goto ErrExit;	
	}	
        /***
        g_pub_tx.tx_amt3 = old_amt3 ;
        ****/
        g_pub_tx.tx_amt4 = old_amt4+old_amt3 ;   
           
        /* ����----ֻ����ϵͳ���ݷ�Ϣ�͸����������� */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
	{
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		g_pub_tx.svc_ind = 3071 ;	
		strcpy ( g_pub_tx.brf,"�����ݰ�����Ϣ����" );
		sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
		WRITEMSG
		
		/*** ���׼��ʴ���(��������Ϣ)***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"���׼��ʴ���(��������Ϣ) PASS!");
		WRITEMSG	                
        }
        if( pub_base_CompDblVal( g_pub_tx.tx_amt4 , 0.00 ) )        
	{        
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		g_pub_tx.svc_ind = 3072 ;	
		strcpy ( g_pub_tx.brf,"�����ݰ��ҷ�Ϣ����" );
		sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
		WRITEMSG
		/*** ���׼��ʴ���(�����ݷ�Ϣ)***/			
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}
		sprintf(acErrMsg,"���׼��ʴ���(�����ݷ�Ϣ) PASS!");
		WRITEMSG
	}

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

	/* �ͻ��ص�--���һ��� */
	ret = print_LNHDAJ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNHDAJ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
	
	/* ���ڴ�Ʊ--���һ���(����) */
	ret = print_LNBKAJBJ( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJBJ,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
	
	/* ���ڴ�Ʊ--���һ���(��Ϣ) */
	ret = print_LNBKAJLX( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJLX,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
	
	/* ���ڴ�Ʊ--���һ���(��Ϣ) */
	ret = print_LNBKAJFX( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJFX,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
/* ��ӡ--���ڴ�Ʊ--�����ݱ�����Ϣ���� */
	ret = print_LNBKAJBN( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJBN,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE	
/* ��ӡ--���ڴ�Ʊ--�����ݱ�����Ϣ���� */
	ret = print_LNBKAJBW( );
	if(ret)
	{
		sprintf(acErrMsg,"print_LNBKAJFW,ʧ������!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L207");
		goto ErrExit;
	}
	TRACE		
	
	pub_mob_sendmail_ln("���Ҵ���黹����",g_pub_tx.tx_code,g_mdm_ac_rel.ac_no,g_pub_tx.add_ind,print_tol_amt,0.0,0.0);
	
					
	if( ct_flag[0]=='1' )	/** �ֽ� **/
		goto ErrExit1;
	else if( ct_flag[0]=='2' ) /*ת��*/
		goto ErrExit2;
    else if (ct_flag[0] == '3')		/*���*/
        goto ErrExit3;
    else if (ct_flag[0] == '8')		/*�ۻ�*/
        goto ErrExit8;

ErrExit:
    sprintf(acErrMsg,"���һ���ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
ErrExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"���һ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"���һ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit3:
    strcpy(g_pub_tx.reply,"0003");
    sprintf(acErrMsg,"���һ���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
ErrExit8:
    strcpy(g_pub_tx.reply,"0008");
    sprintf(acErrMsg,"���һ���ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;	
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_dd_mst, 0x00, sizeof(struct dd_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
  	memset( vln_intst,0,sizeof(vln_intst) );   	
	memset(&ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&v_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	memset(&pri_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
		
	return 0;
	
}
int data_get_struct()
{    
	int ret=0;
	struct ln_mst_hst_c 	S_ln_mst_hst;
	memset( &S_ln_mst_hst,0x00 , sizeof( struct ln_mst_hst_c));

	sprintf(acErrMsg,"���븳ֵ����");
	WRITEMSG
	
	get_zd_data("1091",g_pub_tx.ac_no);		/* �ʺ� */
	get_zd_int("1092",&g_pub_tx.ac_seqn);		/* �˻���� */
	get_zd_int("1099",&cnt);			/* �������� */
	get_zd_double("109C",&g_pub_tx.tmp_amt1);	/* ������ */
	print_tol_amt = g_pub_tx.tmp_amt1;
	get_zd_data("109A",g_pub_tx.note_type);		/* ƾ֤���� */
	get_zd_data("109B",g_pub_tx.beg_note_no);	/* ��ʼƾ֤���� */
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no );

	get_zd_int("109F",&payed_cnt);			/* �ѻ����� */
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
		sprintf(acErrMsg,"���Ҵ���ڴ˴���![%s]",g_ln_parm.ln_pay_type);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L038" );
		return 1;
	}
	TRACE
	/* ���ƾ���������ǰ�����˺���ҵ�� 
	if( g_ln_mst.opn_date!=g_ln_mst.ic_date )
	{
		sprintf(acErrMsg,"���ƾ���������ǰ�����˺���ҵ��[%ld]",g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L043" );
		return 1;
	}
	**/
	/* ���ƾ�������ת���˺���ҵ��  
	ret= Ln_mst_hst_Sel( g_pub_tx.reply, &S_ln_mst_hst, "ac_id=%ld and tx_code='2498'",g_ln_mst.ac_id );
	if( ret!=100 )
	{
		sprintf(acErrMsg,"���ƾ�������ת���˺���ҵ��[%ld]",g_ln_mst.ac_id);
		WRITEMSG
		strcpy( g_pub_tx.reply,"L043" );
		return 1;
	}
	****/

	/* ȡ����ƻ����У����ڵ������� */
	ret = Ln_pay_pln_Sel( g_pub_tx.reply , &pri_ln_pay_pln , "ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if( ret ) {return ret;}
	
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
/* ��ӡ--�ͻ��ص�--���һ���ص� */
int  print_LNHDAJ( )	
{
	struct ln_pay_pln_c	pr_ln_pay_pln;
	
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
	long end_date =0;
	int tmp_cnt=0;
	int tmp_day=0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
 	TRACE   		
	memset(&pr_ln_pay_pln ,0x00 , sizeof(struct ln_pay_pln_c) );
	pub_base_AllDwnFilName( tmpname );
	TRACE
	TRACE
	TRACE
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNHDAJ���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
  	TRACE      	
	/* ��ֵ���� */
	strcpy( tx_type , "���һ���" );		

	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.amt_lmt ;
	
	/* �ۼƻ��� */	
	tol_pay = g_ln_mst.ttl_prvd_amt - g_ln_mst.bal;
			
	/* ��Ƿ���� */		
	tol_need_pay = g_ln_mst.bal;
	
	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(չ)" );
		if( g_pub_tx.tx_date > g_ln_mst.mtr_date )
		{
			sprintf( rate , "%8.5lf��" , g_ln_mst.exp_rate );
		}else{
			sprintf( rate , "%8.5lf��" , g_ln_mst.rate );	
		}			
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf��" , g_ln_mst.rate );
	}
	
	/* Ӧ������ */
	tmp_cnt = pri_ln_pay_pln.curr_cnt;
	
	/* �Ի����� */	
	payed_cnt = payed_cnt + cnt;
/******
	ret = Ln_pay_pln_Sel( g_pub_tx.reply , &pr_ln_pay_pln , "ac_id=%ld and ac_seqn=%d " , g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if( ret==100)
	{
		sprintf(acErrMsg,"���һ���ƻ���������Ӧ��¼",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L044");
		return 1;
	}else if ( ret )
	{
		sprintf(acErrMsg,"ȡ���һ���ƻ�����Ϣ�쳣",g_pub_tx.reply);
		WRITEMSG
		return 1;
	}
*****/
		
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
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.����� 10.�ۼƻ���    */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.չ�ڱ�־�� 15.��������    */
	/* 16.�������� 17.������Ϣ�� 18.������Ϣ��19.��������   20.Ӧ������    */
	/* 21.�ѻ�����   22.��ˮ�� */
	fprintf(fp,"LNHDAJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%d|%.2lf|%.2lf|%.2lf|%d|%d|%d|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	print_tol_amt , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , exp_ind , cnt ,
	tx_amt1 , tx_amt2 , tx_amt4 , ln_pay_pln.ttl_cnt , tmp_cnt ,
	payed_cnt , g_pub_tx.trace_no );	
	TRACE	
	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	TRACE				
	return 0;
}

/* ��ӡ--���ڴ�Ʊ--���𲿷� */
int print_LNBKAJBJ( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
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
	strcpy( tx_type , "����" );	
	strcpy( intro , "�黹����" );		
	
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
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���   */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.���˵���� 15.��ˮ��  */
	fprintf(fp,"LNBKAJBJ%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt1 , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}
	
/* ��ӡ--���ڴ�Ʊ--��Ϣ���� */
int print_LNBKAJLX( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJLX���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "��Ϣ" );	
	strcpy( intro , "�黹��Ϣ" );		
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0158" );	/* ��Ϣ����501 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	
	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.���˵���� 17.��ˮ��*/
	fprintf(fp,"LNBKAJLX%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt2_new , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	

/* ��ӡ--���ڴ�Ʊ--��Ϣ���� */
int print_LNBKAJFX( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJFX���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "��Ϣ" );	
	strcpy( intro , "�黹��Ϣ" );		
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0158" );	/* ��Ϣ����501 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	
	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.���˵���� 17.��ˮ��*/
	fprintf(fp,"LNBKAJFX%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt4 , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
/* ��ӡ--���ڴ�Ʊ--�����ݱ�����Ϣ���� */
int print_LNBKAJBN( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJBN���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ(��)" );	
	strcpy( intro , "�黹��Ϣ" );		
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0155" );	/* ��133 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	
	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.���˵���� 17.��ˮ��*/
	fprintf(fp,"LNBKAJBN%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt2_old , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
/* ��ӡ--���ڴ�Ʊ--�����ݱ�����Ϣ���� */
int print_LNBKAJBW( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double tol_pay=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char fine_rate[11];
	char exp_ind[5];
		
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
	
	char intro[9]; 
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"�ļ���:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNBKAJBW���ļ�ʧ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* ��ֵ���� */
	strcpy( tx_type , "������Ϣ(��)" );	
	strcpy( intro , "�黹��Ϣ" );		
	
	/* ȡ��Ŀ�� */
	strcpy( data_code , "0156" );	/* ��705 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"ȡ�����Ŀ��ʧ��!");
		WRITEMSG		
		return 1;
	}		
	
	/* Ƿ���ܶ� */	
	tol_lo = g_ln_mst.amt_lmt ;
	
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

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( fine_rate );
		
	/*  1.�������͡�  2.�����˺š�  3.��ݺš�   4.�������  5.��Ʒ���� 	*/
	/*  6.���׽�  7.��ʼ���ڡ�  8.�������ڡ� 9.Ƿ���ܶ 10.�ۼƻ���     */
	/* 11.��Ƿ���� 12.�������ʡ� 13.��Ϣ���ʡ�14.��Ŀ�š�   15.չ�ڱ�־  */
	/* 16.���˵���� 17.��ˮ��*/
	fprintf(fp,"LNBKAJBW%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%s|%ld|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	tx_amt3_old , g_ln_mst.opn_date , end_date , tol_lo , tol_pay ,
	tol_need_pay , rate , fine_rate , ln_acc_hrt, exp_ind , 
	intro , g_pub_tx.trace_no );	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
