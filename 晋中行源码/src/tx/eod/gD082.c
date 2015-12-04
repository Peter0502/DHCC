/*************************************************************
* �� �� ��: gD082.c
* �������������գ���ͨ�����ƻ����
*
* ��    ��: lance
* �������: 2003��11��18��
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
#include "ln_pln_inf_c.h"

gD082()
{
	int ret=0;
	long ac_seqn=0;
	double shld_pay_amt=0.00;
	char ln_ac_no[21];
	char dd_ac_no[21];
	char comm[100];
	struct ln_pln_inf_c	ln_pln_inf;
	struct com_holiday_c	com_holiday;		
	
	memset( &com_holiday,0x00,sizeof(struct com_holiday_c )); 
	
	ret=sql_begin(); /*������*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}
	/* ��ʼ��ȫ�ֱ��� */
	pub_base_sysinit();
	TRACE
	
	sprintf( comm," pay_ind='0' and beg_date<=%ld and end_date>=%ld ",g_pub_tx.tx_date,g_pub_tx.tx_date);
	ret= Ln_pln_inf_Dec_Upd( g_pub_tx.reply , comm );
	if (ret)
	{
		sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		/*** ���ݳ�ʼ�� ***/
		memset( &ln_pln_inf,0x00,sizeof(struct ln_pln_inf_c ));    
		memset( dd_ac_no, 0x00, sizeof(dd_ac_no));
		memset( ln_ac_no, 0x00, sizeof(ln_ac_no));
		strcpy( g_pub_tx.tx_code, "G082");		

		ret = Ln_pln_inf_Fet_Upd(&ln_pln_inf, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}       
		else if (ret == 100)
		{
			break;
		}
		strcpy( ln_pln_inf.content,"");
		sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d]",ln_pln_inf.ac_id,ln_pln_inf.ac_seqn);
		WRITEMSG
		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );
				
		ret= Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst,"ac_id=%ld and ac_seqn=%d",ln_pln_inf.ac_id,ln_pln_inf.ac_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"ret=%d ��ȡ������ļ�ʧ��[%d-%d]",
					ret,ln_pln_inf.ac_id,ln_pln_inf.ac_seqn);
			WRITEMSG
			goto ErrExit;   
		}
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;
		strcpy(g_pub_tx.tx_br_no,g_ln_mst.opn_br_no);

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_id =%ld",ln_pln_inf.ac_id);
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
		TRACE
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm,"prdt_no='%s'",g_ln_mst.prdt_no);
		if( ret ) 
		{
			strcpy(acErrMsg,"ȡ�����Ʒ���������!");
			WRITEMSG
			goto ErrExit;
		}

		/* �жϽڼ����Ƿ��� */
		ret = pub_base_date_jjr( g_pub_tx.tx_date , &com_holiday );
		if ( ret==0 && g_ln_parm.holi_ind[0]=='1' ){continue;}
		TRACE
		
		/* ���Ҵ������ */
            	if(g_ln_parm.ln_pay_type[0]=='3'||g_ln_parm.ln_pay_type[0]=='4')
            	{
			strcpy(acErrMsg,"���Ҵ���ڴ˴���!");
			WRITEMSG
			goto ErrExit;
		}            		
				sprintf(acErrMsg,"shld_pay_amt[%.2lf][%.2lf]",
					ln_pln_inf.curr_amt-ln_pln_inf.true_amt,g_ln_mst.bal);
				WRITEMSG
		
		/* �Ƿ��Զ����� */
		if ( g_ln_parm.auto_pay_ind[0]!='Y')
		{
			goto NoPay;
		}
		if( g_ln_mst.repay_ac_id<=0 )
		{
			sprintf(acErrMsg,"�޹黹�˺�[%ld]",g_ln_mst.repay_ac_id);
			WRITEMSG
			goto NoPay;
		}
		if( g_dd_parm.draw_ind[0]=='N' )
		{
			sprintf(acErrMsg,"������ȡ[%s]!",g_dd_parm.draw_ind);
			WRITEMSG
			goto NoPay;
		}

		/* �������˺�״̬ */
		/* ���ݱ������ɻ��ڻ����˺�ID */
		ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
		if ( ret )
		{
			sprintf(acErrMsg,"���ݱ��������˻���Ŵ���[%s]!",g_ln_parm.cur_no);
			WRITEMSG
			goto ErrExit;	
		}
	
		ret = pub_base_currentid_file( g_ln_mst.repay_ac_id,ac_seqn,&g_dd_mst );
		if( ret )
		{
			sprintf(acErrMsg,"�˺Ų�����[%ld|%d]",g_ln_mst.repay_ac_id,ac_seqn);
			WRITEMSG
			goto NoPay;
		}
		sprintf(acErrMsg,"�˺�[%ld]",g_dd_mst.ac_id);
		WRITEMSG
		/* ������ʾ�˺� */
		ret = pub_base_acid_acno( g_dd_mst.ac_id,ac_seqn,&dd_ac_no );	
		if( ret ) {	goto ErrExit; }

		sprintf(acErrMsg,"�˺�[%ld|%d][%s]",g_dd_mst.ac_id,ac_seqn,dd_ac_no);
		WRITEMSG
       	
		ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",
					g_dd_mst.prdt_no);
		if( ret==100 )
		{
			sprintf(acErrMsg,"�����ڸò�Ʒ���!prdt_no=[%s]",g_dd_mst.prdt_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P145");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"��ѯ���ڲ������쳣!!ret=[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		if( g_dd_mst.ac_sts[0]!='1' )
		{
			sprintf(acErrMsg,"�˺�״̬������״̬!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P109");
			goto NoPay;
		}
		/* ����� */
		g_pub_tx.tmp_amt1 = g_dd_mst.bal-g_dd_mst.ctl_amt-g_dd_parm.min_bal;

		ret = pub_base_CompDblVal(g_pub_tx.tmp_amt1,0.00);
		if( ret<=0 )
		{	
			sprintf(acErrMsg,"���㹻���[%.2lf]",g_dd_mst.bal);
			WRITEMSG
			goto NoPay;
		}
	
		/* ���Ƕ���״̬ */
		if( g_dd_mst.hold_sts[0]!='0' )
		{
			g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	

			/**��ȫ�����ֻ�������ӿ���Ϊ0.00**/
			if( g_dd_mst.hold_sts[0]=='1' || g_dd_mst.hold_sts[0]=='2' )
				g_pub_tx.tmp_amt1=0.00;
			else
				g_pub_tx.tmp_amt1 = g_pub_tx.tmp_amt1-g_dd_mst.hold_amt;	

			ret=pub_base_CompDblVal(g_pub_tx.tmp_amt1 , 0.00);
			if( ret<=0 )
			{
				sprintf(acErrMsg,"���㹻���[%s][%.2lf]",
					g_dd_mst.hold_sts,g_pub_tx.tmp_amt1);
				WRITEMSG
				goto NoPay;
			}
		}

		TRACE
				sprintf(acErrMsg,"shld_pay_amt[%.2lf][%.2lf]",
					ln_pln_inf.curr_amt-ln_pln_inf.true_amt,g_ln_mst.bal);
				WRITEMSG
		/* ���ֿۿ���� */
		/* ���������δ�ﵽ�����軹��� */
		shld_pay_amt=0.00;
		if( pub_base_CompDblVal( (ln_pln_inf.curr_amt-ln_pln_inf.true_amt),g_ln_mst.bal )>0 )
		{
			shld_pay_amt=g_ln_mst.bal;
			/* ������,�򲻻� */
			if( pub_base_CompDblVal( g_pub_tx.tmp_amt1,shld_pay_amt )<0 )
			{
				goto NoPay;
			}else{
				strcpy( ln_pln_inf.pay_ind,"1" );
				strcpy( ln_pln_inf.content,"���������ڼƻ������");
				ln_pln_inf.true_amt=ln_pln_inf.true_amt+shld_pay_amt;
			}
		}else{
			shld_pay_amt=ln_pln_inf.curr_amt-ln_pln_inf.true_amt;
			/* ������,�򲻻� */
			if( pub_base_CompDblVal( g_pub_tx.tmp_amt1,shld_pay_amt )<0 )
			{
				goto NoPay;
			}else{
				strcpy( ln_pln_inf.pay_ind,"1" );
				ln_pln_inf.true_amt=ln_pln_inf.true_amt+shld_pay_amt;
			}
		}

		/*д����Ǽǲ�  0�Ǽ�Ƿ�� 1�����ѵǼ�Ƿ�� 2�Ǽǲ�����*/
		ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.ic_date,g_pub_tx.tx_date,"1",shld_pay_amt,0.00,"0",1 );
		if( ret ) goto ErrExit;
		sprintf(acErrMsg,"д����Ǽǲ�PASS");
		WRITEMSG
		
		set_zd_data( "0660" , "2" );	
		/* ������� */
		strcpy( g_pub_tx.ac_no , ln_ac_no);
		g_pub_tx.ac_id = g_ln_mst.ac_id ; 
		g_pub_tx.ac_seqn = g_ln_mst.ac_seqn ;
 		strcpy( g_pub_tx.ac_get_ind,"011");	/*δ��ȡ��mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"3" );	/* �˺����� 3-���� */
		strcpy( g_pub_tx.note_type,"299");
		strcpy( g_pub_tx.beg_note_no, ln_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵln_ac_no*/
   		sprintf(acErrMsg,"*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0�� */
	   	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����3���ջ�����*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
	   	g_pub_tx.svc_ind=3010;
	   	g_pub_tx.tx_amt1=shld_pay_amt;	
		strcpy ( g_pub_tx.brf,"����۱�--����ƻ�" );
	
	   	/*** ���׼��ʴ��� ***/
	   	if( pub_acct_trance() )
	   	{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
	   	}
   		sprintf(acErrMsg,"*&*&*&*&*&*&*&*&*&*&*note_no=[%s]ac_no=[%s]",g_pub_tx.beg_note_no,ln_ac_no);
   		WRITEMSG
	   	/* ���л�Ƽ��� */
		set_zd_data("0210",g_ln_parm.cur_no);  
		set_zd_double("107F",g_pub_tx.tx_amt1);       
		strcpy(g_pub_tx.sub_tx_code,"L201");
		strcpy(g_pub_tx.prdt_code,g_ln_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.tx_amt1=0.00;
		set_zd_double("107F",0.00);    					
			
		/* �ۿͻ������ */
		strcpy( g_pub_tx.ac_no , dd_ac_no);
		sprintf(acErrMsg,"dd_ac_no=[%s][%ld][%ld]",dd_ac_no ,g_dd_mst.ac_id ,g_dd_mst.ac_seqn);
		WRITEMSG

		g_pub_tx.ac_id = g_dd_mst.ac_id ; 
		g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
	   	strcpy( g_pub_tx.ac_get_ind,"00");	/*δ��ȡ��mdm_ac_rel*/
	   	strcpy( g_pub_tx.ac_id_type,"1" );	/* �˺����� 1-���� */
	   	strcpy( g_pub_tx.add_ind,"0" );		/* ����:0��1��2���� */	
	   	strcpy( g_pub_tx.ct_ind,"2" );		/* ��ת:1��2ת */	
	   	strcpy( g_pub_tx.prt_ind,"0" );/*�Ƿ��ӡ����(�������۱�־):1��0��*/
	   	strcpy( g_pub_tx.hst_ind,"1" );/*����ϸ:1�ռ���2���յ�����
																3���ջ�����*/ 
 		strcpy( g_pub_tx.beg_note_no, dd_ac_no);	/*Ϊ���մ�Ʊ��ӡ��ֵdd_ac_no*/
		strcpy ( g_pub_tx.intst_type , "1" );	/* ��Ϣ���� */	
		g_pub_tx.ac_wrk_ind[9]='1';    	    /*���ڷǽ��㻧ǿ�пۿ�*/
		g_pub_tx.ac_wrk_ind[8]='1';         /*���ռ��˱�־ */

	   	g_pub_tx.svc_ind=1001;
	   	g_pub_tx.tx_amt1=shld_pay_amt;	
		strcpy ( g_pub_tx.brf,"�����" );
	
	   	/*** ���׼��ʴ��� ***/
	   	if( pub_acct_trance() )
	   	{
			sprintf(acErrMsg,"���׼��ʴ������!");
			WRITEMSG
			goto ErrExit;
	   	}
		/* ���л�Ƽ��� */
		set_zd_data("102J",g_ln_parm.cur_no);      
		set_zd_double("102F",g_pub_tx.tx_amt1);             
		strcpy(g_pub_tx.sub_tx_code,"D003");/**ȡ���ӽ���**/
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
		sprintf(acErrMsg,"��Ƽ��ʲ��ɹ�!!");
		WRITEMSG
		goto ErrExit;
		}
		set_zd_double("102F",0.00);             
	   	g_pub_tx.tx_amt1=0.00;
	   	
	   	goto Youpay;

NoPay:    	
    		strcpy( ln_pln_inf.pay_ind,"0" );
		strcpy( ln_pln_inf.content,"�������");
Youpay:    	             	               
		sprintf(acErrMsg,"!!!!!!!!!ac_id=[%ld],curr_amt=[%.2lf],true_amt=[%.2lf],shld_amt=[%.2lf]",ln_pln_inf.ac_id,ln_pln_inf.curr_amt,ln_pln_inf.true_amt,shld_pay_amt);
		WRITEMSG		
		ln_pln_inf.lst_date=g_pub_tx.tx_date;
		ret = Ln_pln_inf_Upd_Upd(ln_pln_inf,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"������ͨ�����ƻ������!!");
			WRITEMSG
			goto ErrExit;
		} 
    }
    Ln_pln_inf_Clo_Upd();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"���գ���ͨ�����ƻ����ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");/* ���޸� */
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"���գ���ͨ�����ƻ����ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
