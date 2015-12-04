/*************************************************************
* �� �� ��: gD087.c
* �������������գ�����ſ��
*
* ��    ��: lance
* �������: 2003��3��28��
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

gD087()
{

    int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
    int    ac_seqn,parm_seqn;
 
    double amt_reg,amt;
    double intst_acm,in_acm,out_acm,cmpd_acm;
    char ln_ac_no[20],dd_ac_no[20],flag[2],parm_code[6];  
    long day_tmp,be_date;
    struct S_ln_intst vln_intst[LN_INTST_TYPS];
    struct com_holiday_c com_holiday;
    struct ln_mst_c ln_mst_tmp;
    struct ln_pay_pln_c ln_pay_pln;

    sprintf(acErrMsg,"�������շſ����!!!!!!!!!!!");
	WRITEMSG
           	 
    memset( vln_intst,0,sizeof(vln_intst) );    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
    memset(&ln_pay_pln, 0x00, sizeof(struct ln_pay_pln_c)); 
       
    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();
    
    /* ������ſ�ƻ���(�ſ�����С�ڵ��ڵ���,״̬Ϊδ��) */    
    ret = Ln_pay_pln_Dec_Upd(g_pub_tx.reply,"pay_ind='2' and \
										beg_date<=%ld ",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	TRACE
    while(1)
    {
      	ret = Ln_pay_pln_Fet_Upd(&ln_pay_pln, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if (ret == 100)
        {
	TRACE
            break;
        }   
 sprintf(acErrMsg,"����ſ�ƻ���[%lf]",ln_pay_pln.ttl_amt);
 WRITEMSG

	TRACE
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id ='%ld'",
														ln_pay_pln.ac_id);
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
        strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
		sprintf(acErrMsg,"ln_ac_no[%s]",ln_ac_no);
		WRITEMSG

        /* ȡ������Ϣ */
        ret = pub_ln_lndis_file(ln_ac_no,ln_pay_pln.ac_seqn,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
		if( ret ) 
		{
		TRACE
		goto ErrExit;
		}

		/*���׻�����ֵ*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);


		/* �жϽڼ����Ƿ��� */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ) continue;

      	/* ȡ����״̬ */
        chg_flag = pub_ln_check_stsdate();
		if( chg_flag!=2 )	/* ֻ������������ */
		{
			continue;
		}
		TRACE
		/* �������˺�״̬ */
		ret = pub_base_currentid_file( ln_pay_pln.de_ac_id,
										ln_pay_pln.de_ac_seqn,&g_dd_mst );
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����[%d][%d]",g_pub_tx.ac_id,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			continue;
		}
	TRACE
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld order by note_sts ",
														g_dd_mst.ac_id);
        if ( ret == 100 )
        {
            sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }else if ( ret && ret!=100){
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
        strcpy( dd_ac_no ,g_mdm_ac_rel.ac_no );
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        	
	TRACE
        ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
														g_dd_mst.prdt_no);
        if( ret==100 )
        {
            sprintf(acErrMsg,"�����ڸò�Ʒ���!prdt_no=[%ld]",g_dd_mst.prdt_no);
            WRITEMSG
            strcpy(g_pub_tx.reply,"P145");
            goto ErrExit;
        }else if( ret ){
            sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
            WRITEMSG
            goto ErrExit;
        }		
		if( g_dd_mst.ac_sts[0]!='1' )
		{
			sprintf(acErrMsg,"�˺�״̬������״̬!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
			continue;
		}
		if( g_dd_mst.hold_sts[0]=='1'||g_dd_mst.hold_sts[0]=='3' )
		{
			sprintf(acErrMsg,"���˺��ѱ�����!");
			WRITEMSG
			continue;
		}
		if( g_dd_parm.dpst_ind[0]=='N' )
		{
			sprintf(acErrMsg,"��������[%s]!",g_dd_parm.dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A009");
			continue;
		}
		
		/* �����(�ͻ�Ҫ��)
		if( g_dd_parm.thr_dpst_ind[0]=='N' && 
								strcmp(g_pub_tx.tx_br_no,g_dd_mst.opn_br_no) )
		{
			sprintf(acErrMsg,"����ͨ��[%s]!",g_dd_parm.thr_dpst_ind);
			WRITEMSG
			strcpy(g_pub_tx.reply,"A010");
			break;
		}
		************/
		
		/* ���´������ļ� */
        ret = Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d",
									ln_pay_pln.ac_id , ln_pay_pln.ac_seqn );

  		sprintf(acErrMsg,"ln_pay_pln.ac_id=[%ld][%d]",
									ln_pay_pln.ac_id,ln_pay_pln.ac_seqn);
  		WRITEMSG

        if (ret != 0)
        {
             sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
             WRITEMSG
             goto ErrExit;
        }
        while(1)
        {
	TRACE
      	     ret = Ln_mst_Fet_Upd(&ln_mst_tmp, g_pub_tx.reply);
             if (ret != 0 && ret != 100)
             {
                 sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
                 WRITEMSG
                 goto ErrExit;
             }else if (ret == 100){
                 break;
             }   
			 TRACE

             /* �޸Ĵ������ļ�,������ */
             /* ���㡢���´������ļ������л��� */
             /* ������� */
	TRACE
		     intst_acm=0.00;
             ret=pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
									g_ln_parm.intst_days_type,"2",g_ln_mst.bal,
										0.00,&intst_acm,"0",0,0 );
             if( ret )
             {
                 sprintf(acErrMsg,"���ú������㱾���������!");
                 WRITEMSG
                 
                 goto ErrExit;	
             }
             g_ln_mst.intst_acm+=intst_acm;
		     sprintf(acErrMsg,"���㱾�����[%lf]",g_ln_mst.intst_acm);
			 WRITEMSG
	TRACE
             /* ���ڻ��� */         
			 in_acm=0.00;
             ret=pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
									g_ln_parm.intst_days_type,"2",
									g_ln_mst.in_lo_intst,0.00,&in_acm,"0",0,0 );
             if( ret )
             {
                 sprintf(acErrMsg,"���ú���������ڻ�������!");
                 WRITEMSG
                 goto ErrExit;	
             }
             g_ln_mst.in_lo_acm+=in_acm;            
      		 sprintf(acErrMsg,"������ڻ���[%lf]",g_ln_mst.in_lo_acm);
		     WRITEMSG

             /* ������� */
             ret =  pub_base_CalAcm_Ln( g_ln_mst.lst_date,g_pub_tx.tx_date,
									 g_ln_parm.intst_days_type,"2",
								 g_ln_mst.out_lo_intst,0.00,&out_acm,"0",0,0);
             if( ret )
             {
                 sprintf(acErrMsg,"���ú�����������������!");
                 WRITEMSG
                 goto ErrExit;	
             }
             g_ln_mst.out_lo_acm+=out_acm;               
			 g_ln_mst.out_lo_acm = pub_base_PubDround_1( g_ln_mst.out_lo_acm );
             sprintf(acErrMsg,"����������[%lf]",g_ln_mst.out_lo_acm);
			 WRITEMSG

             /* �������� */
			 cmpd_acm=0.00;
             ret =  pub_base_CalAcm_Ln(g_ln_mst.lst_date,g_pub_tx.tx_date,
									g_ln_parm.intst_days_type,"2",
								g_ln_mst.cmpd_lo_intst,0.00,&cmpd_acm,"0",0,0);
             if( ret )
             {
                 sprintf(acErrMsg,"���ú������㸴����������!");
                 WRITEMSG
                 goto ErrExit;	
             }                      
             g_ln_mst.cmpd_lo_acm+=cmpd_acm;                          
			 sprintf(acErrMsg,"���㸴������[%lf]",g_ln_mst.cmpd_lo_acm);
			 WRITEMSG

		TRACE
             /* �޸Ĵ������ۼƷ��Ŷ� */

             g_pub_tx.tx_amt1=ln_pay_pln.ttl_amt;
			 /****
			 sprintf(acErrMsg,"��������[%lf]",g_ln_mst.bal);
			 WRITEMSG

             g_ln_mst.bal+=ln_pay_pln.ttl_amt;
			 sprintf(acErrMsg,"��������[%lf]",g_ln_mst.bal);
			 WRITEMSG

             g_ln_mst.ttl_prvd_amt+=ln_pay_pln.ttl_amt;
             g_ln_mst.lst_date=g_pub_tx.tx_date;
			 ****/
             sprintf(acErrMsg,"[%lf][%lf][%lf]",g_pub_tx.tx_amt1,g_ln_mst.bal,
								g_ln_mst.ttl_prvd_amt );
		 	WRITEMSG

		TRACE
             ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply  );
             if ( ret )
             {
                  sprintf( acErrMsg,"Ln_mst_Upd_Upd ����[%d]",ret);
	              WRITEMSG
                  goto ErrExit;
             }		                 
        } 
		TRACE
		Ln_mst_Clo_Upd( );
		sprintf(acErrMsg,"���´������ļ� PASS");
		WRITEMSG    
		/* �ǼǴ���Ǽǲ�--���� */
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_ln_mst.bal,0.00,"0",0 );	
		if( ret )
		{
	            sprintf(acErrMsg,"�ǼǴ���ǼǱ�ʧ��");
	            WRITEMSG
	            strcpy(g_pub_tx.reply,"L082");
	            goto ErrExit;
	        }						
		sprintf(acErrMsg,"�ǼǴ���ǼǱ� PASS!");
		WRITEMSG		         		      
TRACE
        strcpy( ln_pay_pln.pay_ind , "3" );
		ln_pay_pln.end_date = g_pub_tx.tx_date;
        ret = Ln_pay_pln_Upd_Upd( ln_pay_pln, g_pub_tx.reply  );
        if ( ret )
        {
              sprintf( acErrMsg,"Ln_pay_pln_Upd_Upd ����[%d]",ret);
	      	  WRITEMSG
              goto ErrExit;
        }
		TRACE
        /* ����ˮ�ͼ��� */
        if( pub_base_CompDblVal( g_pub_tx.tx_amt1 ,0.00 ) )
        {
            /* �������ˮ */
            strcpy( g_pub_tx.ac_no , ln_ac_no);
			g_pub_tx.ac_id = g_ln_mst.ac_id;
            g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
	     	strcpy( g_pub_tx.ac_get_ind,"00");	/*δ��ȡmdm_ac_rel*/
	     	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	     	strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
	     	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	     	strcpy( g_pub_tx.prt_ind,"0");/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	     	strcpy( g_pub_tx.hst_ind,"1");/*����ϸ:1�ռ���2���յ�����
														  3���ջ�����*/        
            strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	     	g_pub_tx.svc_ind=3010;	
            strcpy ( g_pub_tx.brf,"���մ���ſ�" );

            if( pub_acct_trance() )
	     	{
                  sprintf(acErrMsg,"���׼��ʴ������!");
                  WRITEMSG
                  goto ErrExit;
	     	}   
			TRACE
	     	/* �������Ƽ��� */
            set_zd_data("0210",g_ln_parm.cur_no);    
            set_zd_double("0430",g_pub_tx.tx_amt1);                     
            strcpy(g_pub_tx.sub_tx_code,"L104");
            strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
           	ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
            if (ret != 0)
            {
                 sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
                 WRITEMSG
                 goto ErrExit;
            }
			TRACE
            /* ί��Э�����(�ſ�) */
			sprintf(acErrMsg,"g_ln_parm.trust_prot_ind=[%s]",
									g_ln_parm.trust_prot_ind);
			WRITEMSG

            ret = pub_eod_ln_trust( g_ln_mst , g_ln_parm , "1" );
            if( ret )  goto ErrExit;
             
			TRACE
            /* ������ˮ */
            strcpy( g_pub_tx.ac_no , dd_ac_no );
			g_pub_tx.ac_id = g_dd_mst.ac_id;
            g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	     	strcpy( g_pub_tx.ac_get_ind,"00");	/*δ��ȡmdm_ac_rel*/
	     	strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-���� */
	     	strcpy( g_pub_tx.add_ind,"1" );		/* ����:0��1��2���� */	
	     	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	     	strcpy( g_pub_tx.prt_ind,"0");/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	     	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
														   3���ջ�����*/        
            strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
			g_pub_tx.ac_wrk_ind[9]='1';     /*���ڷǽ��㻧ǿ�пۿ�*/
			g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */

	     	g_pub_tx.svc_ind=1001;	
            strcpy ( g_pub_tx.brf,"���մ���ſ����" );

            if( pub_acct_trance() )
	     	{
                  sprintf(acErrMsg,"���׼��ʴ������!");
                  WRITEMSG
                  goto ErrExit;
	     	}   
			TRACE
	        /* ������Ƽ��� */
            set_zd_data("101A",g_ln_parm.cur_no);    
            set_zd_double("1013",g_pub_tx.tx_amt1);                     
            strcpy(g_pub_tx.sub_tx_code,"D099");
            strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
            ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
            if (ret != 0)
            {
                 sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
                 WRITEMSG
                 goto ErrExit;
            }
			g_pub_tx.tx_amt1 = 0.00;
			TRACE
       }    
    }
    Ln_pay_pln_Clo_Upd( );    		                         
    sprintf(acErrMsg,"���´���ſ�ƻ���ʧ�� PASS");
    WRITEMSG	

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���գ�����ſ��ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���գ�����ſ��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
