/*************************************************************
* �� �� ��: gD084.c
* �������������գ����Ҵ������ļ�����
*
* ��    ��: lance
* �������: 2003��3��31��
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

gD084()
{
    int    ret=0,bj_flag=0,chg_flag=0,i=0,dc_flag=0;
    char ln_ac_no[20],dd_ac_no[20],flag[2];  
    struct com_holiday_c com_holiday;
    struct ln_mst_c ln_mst_tmp;
    
    memset(&com_holiday, 0x00, sizeof(struct com_holiday_c));
    memset(&ln_mst_tmp, 0x00, sizeof(struct ln_mst_c));
    
    ret=sql_begin(); /*������*/
    if(ret)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /* ��ʼ��ȫ�ֱ��� */
    pub_base_sysinit();

    ret=Eod_ln_mst_Dec_Upd(g_pub_tx.reply,"mtr_date<= %ld",g_pub_tx.tx_date);
    if(ret)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	TRACE
    while(1)
    {
      	ret=Eod_ln_mst_Fet_Upd(&ln_mst_tmp, g_pub_tx.reply);
        if (ret&&ret!=100)
        {
			TRACE
            sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }       
        else if(ret==100)
        {
			TRACE
            break;
        }
        g_pub_tx.ac_seqn=ln_mst_tmp.ac_seqn;
		TRACE
        ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld",
															ln_mst_tmp.ac_id);
        if(ret==100)
        {
            sprintf(acErrMsg,"�����˻����ձ��в����ڸü�¼[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W010");
            goto ErrExit;
        }else if(ret&&ret!=100){
            sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
            WRITEMSG
            strcpy (g_pub_tx.reply,"W011");
            goto ErrExit;
        }
		TRACE
        strcpy( ln_ac_no ,g_mdm_ac_rel.ac_no );
        memset( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );
        
        /* ȡ������Ϣ */
        ret = pub_ln_lndis_file(ln_ac_no,g_pub_tx.ac_seqn,
						&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*��ȡ�����Ϣ*/
		if(ret) {goto ErrExit;}
		/*���׻�����ֵ*/
		strcpy( g_pub_tx.tx_br_no, g_ln_mst.opn_br_no);

		TRACE
        if(g_ln_parm.ln_pay_type[0]!='3' && g_ln_parm.ln_pay_type[0]!='4')
        {	
			TRACE
        	continue;
		}
		/* �жϽڼ����Ƿ��� */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ) continue;
		TRACE
      	/* ȡ����״̬ */
        chg_flag = pub_ln_check_stsdate();
		if( chg_flag==-1 )	/* �����Ҫ�����ڴ��� */
		{
			TRACE
			continue;
		}
		/***************ת������*************************/	
		TRACE
		strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ;          
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
		strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
		strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
		strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
														3���ջ�����*/  
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.svc_ind=3060;
		g_pub_tx.tx_amt1=-g_ln_mst.bal;	   	  	   	
		strcpy ( g_pub_tx.brf,"���յ��ڰ��Ҵ���ת��" );
	
		/*** ���׼��ʴ��� ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
		}
		TRACE
		/* ���л�Ƽ��� */
		set_zd_data("0210",g_ln_parm.cur_no);  
		set_zd_double("1074",g_pub_tx.tx_amt1);                                 
		strcpy(g_pub_tx.sub_tx_code,"L30B");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
		}
		TRACE
		/* ���´������ļ� */
		sprintf( flag , "%d" , chg_flag );
		strcpy( g_ln_mst.ac_sts,flag );               	               
		ret = Eod_ln_mst_Upd_Upd(g_ln_mst,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���´������ļ�����!!");
            WRITEMSG
            goto ErrExit;
		} 
	   /**********ת�벿��************/
	   TRACE
       strcpy( g_pub_tx.ac_no , ln_ac_no );
       g_pub_tx.ac_id = g_ln_mst.ac_id ;            
       g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
	   strcpy( g_pub_tx.ac_get_ind,"10");	/*�Ѿ���ȡ��mdm_ac_rel*/
	   strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
	   strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	   strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/
       strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	   g_pub_tx.svc_ind=3060;
	   g_pub_tx.tx_amt1=g_ln_mst.bal;	   	  	   	
       strcpy ( g_pub_tx.brf,"���յ��ڴ������" );
	
	   /*** ���׼��ʴ��� ***/
	   if( pub_acct_trance() )
	   {
            sprintf(acErrMsg,"���׼��ʴ������!");
            WRITEMSG
            goto ErrExit;
	   }
	   /* ���л�Ƽ��� */
       set_zd_data("0210",g_ln_parm.cur_no); 
       set_zd_double("107F",g_pub_tx.tx_amt1);                       
       strcpy(g_pub_tx.sub_tx_code,"L302");
       strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
       ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
       if (ret != 0)
       {
            sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
            WRITEMSG
            goto ErrExit;
       }                     
       TRACE       
    }
    Eod_ln_mst_Clo_Upd();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���մ���ڴ������ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���մ���ڴ������ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
