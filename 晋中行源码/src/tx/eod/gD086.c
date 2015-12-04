/*************************************************************
* �� �� ��: gD086.c
* �������������գ����Ҵ���Ƿ���ÿ�ռ��
*
* ��    ��: lance
* �������: 2003��3��30��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_holiday_c.h"
#include "ln_pay_pln_c.h"
#include "ln_lo_c.h"
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}

gD086()
{
	int cnt=0;
	int payed_cnt=0;
	int ins_rol_ind=0;
	char amt_str[5];
	char ct_flag[2];

    	int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0,pay_flag=0;
    	int    ac_seqn,parm_seqn,flag=0;

	double tx_amt1=0.00 , tx_amt2=0.00 , tx_amt3=0.00 , tx_amt4=0.00 ;
	double tx_amt2_new=0.00 , tx_amt2_old=0.00 , tx_amt3_old=0.00;
	double old_dd_bal=0.00;

	double amt_tmp=0.00,amt_lo=0.00,pay_amt=0.00;
    double amt_reg=0.00,amt=0.00,pun_intst=0.00;
	double	cmpd_intst_aj=0.00; /* �������� */
    char ln_ac_no[20],dd_ac_no[20],parm_code[6],old_ac_ind[2];  
    long day_tmp=0,be_date=0;
	long	ac_id_old=0;
	double	old_amt1=0.00,old_amt2=0.00,old_amt3=0.00,old_amt4=0.00;
    struct S_ln_intst vln_intst[LN_INTST_TYPS];
    struct com_holiday_c com_holiday;
    struct ln_mst_c ln_mst_tmp;
    struct ln_lo_c ln_lo;    
	struct ln_pay_pln_c  ln_pay_pln;
	struct ln_pay_pln_c  v_ln_pay_pln;
         	 
    memset( vln_intst,0,sizeof(vln_intst) );    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
    memset(&ln_lo, 0x00, sizeof(struct ln_lo_c)); 
	old_amt1=old_amt2=old_amt3=old_amt4=0.00;
         
    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();

	ret=sql_begin(); /*������*/
    if (ret != 0)
    {
        sprintf(acErrMsg,"���������[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    
    /* ������Ƿ���(��������С�ڵ��ڵ���,״̬Ϊδ����) */    
	/*
    ret = Ln_lo_Dec_Upd_Hold(g_pub_tx.reply," pay_type='0' and shld_pay_date<=%ld ",g_pub_tx.tx_date);
	*/
    ret = Ln_lo_Dec_Upd(g_pub_tx.reply," pay_type='0' and shld_pay_date<=%ld ",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	TRACE
	i=0;
	ac_id_old=0;
    while(1)
    {
		/*���ݳ�ʼ��*/
		ret=0;bj_flag=0;chg_flag=0;i=0;dc_flag=0;pay_flag=0;
		tx_amt1=0.00 ; tx_amt2=0.00 ; tx_amt3=0.00 ; tx_amt4=0.00 ;
		tx_amt2_new=0.00 ; tx_amt2_old=0.00 ; tx_amt3_old=0.00;
		old_dd_bal=0.00;

		amt_tmp=0.00;amt_lo=0.00;pay_amt=0.00;
		amt_reg=0.00;amt=0.00;pun_intst=0.00;
		cmpd_intst_aj=0.00; /* �������� */
		day_tmp=0;be_date=0;
		ac_id_old=0;
		old_amt1=0.00;old_amt2=0.00;old_amt3=0.00;old_amt4=0.00;

		g_pub_tx.ac_no[0]='\0';
		g_pub_tx.ac_id=0;      
		g_pub_tx.ac_seqn=0;

		memset(dd_ac_no,0x00,sizeof(dd_ac_no));
		memset( vln_intst,0,sizeof(vln_intst) );

		g_pub_tx.tmp_amt1=0.00;
		g_pub_tx.tx_amt1=0.00;
		g_pub_tx.tx_amt2=0.00;
		g_pub_tx.tx_amt3=0.00;
		g_pub_tx.tx_amt4=0.00;

    	if(ret != 0)
  		{
			sprintf( acErrMsg, "������ʧ��!!!" );
			WRITEMSG
			goto ErrExit;
    	}
     	strcpy( g_pub_tx.tx_code, "G086");
	/*
      	ret = Ln_lo_Fet_Upd_Hold(&ln_lo, g_pub_tx.reply);
	*/
      	ret = Ln_lo_Fet_Upd(&ln_lo, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }else if (ret == 100){
			TRACE
			/*
			sql_commit();
			*/
			break;
        }   
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
            sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
            WRITEMSG
            goto ErrExit;
		}
		TRACE
		if ( i ==0 ) ac_id_old = ln_lo.ac_id;
		i++;
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld", 
																ln_lo.ac_id);
        if ( ret == 100 )
        {
            sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }
        else if ( ret && ret!=100)
        {
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
		TRACE
        strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
		strcpy( g_pub_tx.ac_no , ln_ac_no );

        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        
        /* ȡ������Ϣ */
        ret = pub_ln_lndis_file(ln_ac_no,ln_lo.ac_seqn, &g_mdm_ac_rel,
									&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
		if( ret ) { 
			sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit; }

		/*���׻�����ֵ*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

		/* �жϽڼ����Ƿ��� */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ) { /*sql_rollback();*/continue;}
		TRACE
	
      	/******** ȡ����״̬ 
        chg_flag = pub_ln_check_stsdate();
		if( chg_flag!=2 )	* ֻ������������ *
		{
			continue;
		}	
		****************/
	
		/* �Ƿ��Զ����� */
		if ( g_ln_parm.auto_pay_ind[0]!='Y')
		{
			TRACE
			/*sql_rollback();*/
			continue;

		}
		/* �������˺�״̬ */
		/* ���ݱ���ȡ��� */
		/* ���ݱ������ɻ��ڻ����˺�ID */ 
		ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"���ú������ݱ��������˻���Ŵ���!");
			WRITEMSG
			goto ErrExit;	
		}
		ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst );
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
			WRITEMSG
			goto ErrExit;
		}
		TRACE
		sprintf(acErrMsg,"amt1[%lf]amt2[%lf]amt4[%lf]dd_bal[%lf]",g_pub_tx.tx_amt1,
			g_pub_tx.tx_amt2,g_pub_tx.tx_amt4,g_dd_mst.bal);
		WRITEMSG	
		TRACE	
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld order by note_sts",
															g_dd_mst.ac_id);
        if ( ret == 100 )
        {
            sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }
        else if ( ret && ret!=100)
        {
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
		TRACE
        strcpy( dd_ac_no ,g_mdm_ac_rel.ac_no );
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        	
        ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
															g_dd_mst.prdt_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"�����ڸò�Ʒ���!!prdt_no=[%s]",g_dd_mst.prdt_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P145");
            goto ErrExit;
        }else if( ret ){
            sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
            WRITEMSG
            goto ErrExit;
        }		
		TRACE
		if( g_dd_mst.ac_sts[0]!='1' )
		{
			sprintf(acErrMsg,"�˺�״̬������״̬!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
            goto ErrExit;
		}
		/* �˺���ȫ�������ֻ������ */
		if( g_dd_mst.hold_sts[0]=='1'||g_dd_mst.hold_sts[0]=='2' )
		{
			sprintf(acErrMsg,"���˺��ѱ�����!");
			WRITEMSG
            goto ErrExit;
		}
		if( g_dd_parm.draw_ind[0]=='N' )
		{
			sprintf(acErrMsg,"������ȡ[%s]!",g_dd_parm.draw_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A023");
			sql_rollback();
			continue;
		}
		/* �����(�ͻ�Ҫ��)  	
		if( g_dd_parm.thr_draw_ind[0]=='N' && strcmp(g_pub_tx.tx_br_no,g_dd_mst.opn_br_no) )
		{
			sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A011");
			sql_rollback();
			continue;
		}
		************/
		
		/* ����� */
		ret = pub_base_CompDblVal(g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal,0.00);
		if( ret<=0 )
		{	
			continue;
		}else{
			g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;
		}
	
		/* ���ǲ��ֶ���״̬ */
		if( g_dd_mst.hold_sts[0]=='3' )
		{
			ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1-g_dd_mst.hold_amt , 0.00);
			if( ret<=0 )
			{
				continue;
			}else{
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	
			}
		}
		old_dd_bal= g_pub_tx.tmp_amt1;	/*ԭʼ���ô����*/
		sprintf(acErrMsg,"ԭʼ���ô���%lf��!",old_dd_bal);
		WRITEMSG


		TRACE

		/* ���´������ļ� */
            /*���㷣Ϣ*/
            /* ���Ǿ�ϵͳ���� */
vtcp_log("AAAAAAAAA[%d][%d][%s]",ln_lo.shld_pay_date,g_pub_tx.tx_date,ln_lo.old_ac_ind);
		/* ����� */
		if(ln_lo.old_ac_ind[0]=='1'||ln_lo.old_ac_ind[0]=='2' )	
		/* ��ϵͳ���ݣ���Ԥ�����Ѿ�����ڻ��߱��⡣����Ҫ�� ��133 ��705*/
		{
	 		sprintf(acErrMsg,"����������ǷϢ����%lf��!",g_pub_tx.tmp_amt1);
	 		WRITEMSG
			memset( vln_intst,0,sizeof(vln_intst) );
			/* ���㷣Ϣ��� */
			/****
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt-ln_lo.pay_lo_amt,ln_lo.lo_intst-ln_lo.pay_lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj);            	
			   ******/
			ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
			   ln_lo.lo_amt,ln_lo.lo_intst,
			   ln_lo.shld_pay_date,g_pub_tx.tx_date,&pun_intst,&cmpd_intst_aj);            	
	        if (ret) {
			sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;        	
			}
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
	        if( ret ) {
	 		sprintf( acErrMsg,"������ǷϢ����%lf����!",g_pub_tx.tmp_amt1);
	 		WRITEMSG	
			goto ErrExit;
			}
	                
	 		               
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
			    if( ret ) {	
				sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
				WRITEMSG
				goto ErrExit;	
				}
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
	
	        }
	        else if( ln_lo.old_ac_ind[0]=='0' )
	      {
				memset( vln_intst,0,sizeof(vln_intst) );
	 			sprintf(acErrMsg,"������ǷϢ����%lf��!",g_pub_tx.tmp_amt1);
	 			WRITEMSG			
				/* ��ϵͳ���ݴ���ʽ */
				/* ���㷣Ϣ��� */
				/*****
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt-ln_lo.pay_lo_amt ,
					ln_lo.lo_intst-ln_lo.pay_lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);   
				******/
				ret=pub_ln_CalPunIntst(g_ln_mst, g_ln_parm, 
					ln_lo.lo_amt ,
					ln_lo.lo_intst , ln_lo.shld_pay_date , 
					g_pub_tx.tx_date , &pun_intst , &cmpd_intst_aj);   				         
	            if (ret) { /*sql_commit();*/break;}
	            /*���û���ƻ�����ȷ������ǷϢ�Ļ�����*/
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
	           	if( ret ) {
		 		sprintf( acErrMsg,"������ǷϢ����%lf��!",g_pub_tx.tmp_amt1);
	 			WRITEMSG
				goto ErrExit;
				}
	 		
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
	            } 	        
	        }
	/*
            ret = Ln_lo_Upd_Upd_Hold(ln_lo, g_pub_tx.reply  );
	*/
            ret = Ln_lo_Upd_Upd(ln_lo, g_pub_tx.reply  );
            if ( ret )
            {
                 sprintf( acErrMsg,"Ln_lo_Upd_Upd ����[%d]",ret);
	             WRITEMSG
                 goto ErrExit;
            }
            memset( &ln_lo , 0x00 , sizeof( struct ln_lo_c ) );	                
			/** ������  **/
			if ( ln_lo.ac_id != ac_id_old )
		
	/* �Ƚϻ��� */	
	g_pub_tx.tmp_amt1 = pub_base_PubDround_1(g_pub_tx.tmp_amt1);
	if ( pub_base_CompDblVal( g_pub_tx.tmp_amt1 , 0.00 ) )
	{
		sprintf(acErrMsg,"��������Ӧ������[%lf]",g_pub_tx.tmp_amt1);
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"L046");
		ERR_DEAL				
		**/
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
  if( ret ){
	sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
	WRITEMSG
 	goto ErrExit;
	}
  /* ����Ǽǲ�--��Ϣ */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"2",g_pub_tx.tx_amt2,0.00,"0",2 );
  if( ret ){
	sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
	WRITEMSG
 	goto ErrExit;	
	}
  /* ����Ǽǲ�--��Ϣ */
  ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date,g_pub_tx.tx_date,"4",g_pub_tx.tx_amt4,0.00,"0",2 );
  if( ret ) {	
	sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
	WRITEMSG
	goto ErrExit;	
	}
  		
 	
	/* �������ˮ */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt1 , 0.00 ) )        
	{ 
		/*�ǼǴ������մ�ӡ�Ǽǲ�*
		ret=Ln_eod_pri_InsReg( ln_ac_no, g_ln_mst, dd_ac_no, g_dd_mst.ac_seqn, g_dd_mst.name, 
				g_pub_tx.tx_amt1, 0, g_pub_tx.tx_code, "1", "1", ln_lo.shld_pay_date, g_pub_tx.tx_date, 
				g_pub_tx.trace_no , g_pub_tx.tx_date );
		if(ret)	goto ErrExit;
		***/
			
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/		
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
		/*�ǼǴ������մ�ӡ�Ǽǲ�*
		ret=Ln_eod_pri_InsReg( ln_ac_no, g_ln_mst, dd_ac_no, g_dd_mst.ac_seqn, g_dd_mst.name, 
				g_pub_tx.tx_amt1, 0, g_pub_tx.tx_code, "1", "1", ln_lo.shld_pay_date, g_pub_tx.tx_date, 
				g_pub_tx.trace_no, g_pub_tx.tx_date );
		if(ret)	goto ErrExit;
		*******/
		
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
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
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/		
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
	
	sprintf(acErrMsg,"���׼��ʴ��� PASS!");
	WRITEMSG

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
		amt_str[0]='1';
	}else{
		amt_str[0]='*';
	}	
	if( pub_base_CompDblVal( tx_amt2_new , 0.00 ) )/* �����ݱ�����Ϣ */
	{
		amt_str[1]='1';
	}else{
		amt_str[1]='*';
	}
	if( pub_base_CompDblVal( tx_amt4 , 0.00 ) )/*��Ϣ*/
	{
		amt_str[2]='1';
	}else{
		amt_str[2]='*';
	}
	if( pub_base_CompDblVal( tx_amt2_old , 0.00 ) )/* �����ݱ�����Ϣ */
	{
		amt_str[3]='1';
	}else{
		amt_str[3]='*';
	}
	if( pub_base_CompDblVal( tx_amt3_old , 0.00 ) )/* �����ݱ�����Ϣ */
	{
		amt_str[4]='1';
	}else{
		amt_str[4]='*';
	}	
	set_zd_data( "0630" , amt_str );      
	
	                  
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
		if( ret ){ 	
			sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
			WRITEMSG
			goto ErrExit;	
			}
	}
	/* ���� */
	if( pub_base_CompDblVal(old_amt3, 0.00 ) )
	{
		ret = pub_ln_ln_reg( g_ln_mst,g_pub_tx.tx_date
				,g_pub_tx.tx_date,"3",old_amt3,0.00,"0",2 );
		if( ret ){
			sprintf(acErrMsg,"���״���!%s %d",__FILE__,__LINE__);
			WRITEMSG
		 	goto ErrExit;	
			}	
	}
        /***
        g_pub_tx.tx_amt3 = old_amt3 ;
        ****/
        g_pub_tx.tx_amt4 = old_amt4+old_amt3 ;   
		    strcpy( g_pub_tx.opn_br_no, g_ln_mst.opn_br_no); /*���׻�����ֵ2009/2/26 19:07:34 by martin*/
        /* ����----ֻ����ϵͳ���ݷ�Ϣ�͸����������� */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt2 , 0.00 ) )        
	{
		strcpy( g_pub_tx.ac_wrk_ind,"0000" );
		g_pub_tx.ac_id = g_ln_mst.ac_id;		/* �˻���� */
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* �˻���� */
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/		
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
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/		
		strcpy ( g_pub_tx.ac_id_type , "3" );	/* �˺����� 3-���� */
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		strcpy ( g_pub_tx.add_ind , "0" );		/* ����:0��1��2���� */	
		strcpy ( g_pub_tx.prt_ind , "0" );	/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy ( g_pub_tx.hst_ind , "1" );	/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
		g_pub_tx.svc_ind = 3072 ;	
		strcpy ( g_pub_tx.brf,"�����ݰ�����Ϣ����" );
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
				ac_id_old=ln_lo.ac_id;
	
		/*�ǻ����ˮ*/
		set_zd_data( "0660" , "2" );		
		strcpy(g_pub_tx.sub_tx_code,"L203");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
		/*�ǿͻ���*/
		sprintf(acErrMsg,"�ǿͻ��ʽ��tx_amt[%lf] bal[%lf]",g_pub_tx.tmp_amt1,g_dd_mst.bal);
		WRITEMSG
		/***del by lance
		g_pub_tx.tmp_amt1=g_dd_mst.bal-g_pub_tx.tmp_amt1;
		***/
		g_pub_tx.tmp_amt1=old_dd_bal-g_pub_tx.tmp_amt1; /*�ۿ���*/

		sprintf(acErrMsg,"OVER�ǿͻ��ʽ��tx_amt[%lf] bal[%lf]",g_pub_tx.tmp_amt1,g_dd_mst.bal);
		WRITEMSG
		sprintf(acErrMsg,"OVER�ǿͻ��ʽ��tmp_tx_amt[%lf] bal[%lf]",g_pub_tx.tmp_amt1,old_dd_bal);
		WRITEMSG

	   	/**********ȡ���************/
        strcpy( g_pub_tx.ac_no , dd_ac_no);
		sprintf(acErrMsg,"dd_ac_no=[%s][%ld][%ld]",dd_ac_no ,g_dd_mst.ac_id ,g_dd_mst.ac_seqn);
		WRITEMSG

		sprintf(acErrMsg,"tx_amt1=[%lf][%lf][%lf][%lf]",
		g_pub_tx.tx_amt1,g_pub_tx.tx_amt2,g_pub_tx.tx_amt3,g_pub_tx.tx_amt4);
		WRITEMSG
		g_pub_tx.ac_id = g_dd_mst.ac_id ; 
        g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	   	strcpy( g_pub_tx.ac_get_ind,"00");	/*δ��ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵdd_ac_no*/			   	
	   	strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-���� */
	   	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/ 
        strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.ac_wrk_ind[9]='1';     /*���ڷǽ��㻧ǿ�пۿ�*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */

	   	g_pub_tx.svc_ind=1001;
		/***
	   	g_pub_tx.tx_amt1=tx_amt1;	
		***/
	   	g_pub_tx.tx_amt1=g_pub_tx.tmp_amt1;	
        strcpy ( g_pub_tx.brf,"���հ��һ������ڻ�" );
	
	   	/*** ���׼��ʴ��� ***/
	   	if( pub_acct_trance() )
	   	{
             sprintf(acErrMsg,"���׼��ʴ������!");
             WRITEMSG
             goto ErrExit;
	   	}
	    /* ���л�Ƽ��� */
        set_zd_data("102J",g_ln_parm.cur_no);      
		/***
        set_zd_double("102F",g_pub_tx.tmp_amt1);             
		****/
        set_zd_double("102F",g_pub_tx.tmp_amt1);             
		g_pub_tx.tx_amt1=g_pub_tx.tmp_amt1;
        strcpy(g_pub_tx.sub_tx_code,"D003");/**ȡ���ӽ���**/
        strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
        ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
        if (ret != 0)
        {
             sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
             WRITEMSG
             goto ErrExit;
        }
	/*		sql_commit();	*--�ύ����--*/
    }
	/*
    Ln_lo_Clo_Upd_Hold( );    		                         
	*/
    Ln_lo_Clo_Upd( );    		                         
    sprintf(acErrMsg,"���չ黹����Ƿ����¼ PASS");
    WRITEMSG	
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���չ黹����Ƿ����¼�������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	sql_commit();	/*--�ύ����--*/
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���չ黹����Ƿ����¼�������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
