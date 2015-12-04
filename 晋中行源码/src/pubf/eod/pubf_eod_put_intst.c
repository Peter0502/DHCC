/*************************************************
* �� �� ��:     pubf_eod_put_intst.c
* ����������    ����Ԥ����Ϣ����(ÿ��21Ԥ��)
* ��    ��:     power
* ������ڣ�    2004��07��05��
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

int pub_eod_put_intst()
{
	int	ret=0,year=0,mouth=0,day=0;
	long put_beg_date=0;
	long put_end_date=0;
	struct com_branch_c	sCom_branch;
	memset(&sCom_branch,0x00,sizeof(struct com_branch_c));

	year=g_pub_tx.tx_date/10000;
	mouth=g_pub_tx.tx_date%10000/100;
	day=g_pub_tx.tx_date%100;

	pub_base_deadlineD(g_pub_tx.tx_date,-1,&put_end_date);
	mouth=mouth-1;
	if( mouth<0 )
	{
		mouth=mouth+12;
		year--;
	}
	put_beg_date=year*10000+mouth*100+day;

	ret=Com_branch_Dec_Sel(g_pub_tx.reply," br_type!='0' and wrk_sts!='*' ");
	if( ret )
	{
		sprintf(acErrMsg," com_branch_dec_sel erro [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"O092");
		return 1;
	}

	while(1)
	{
		ret=Com_branch_Fet_Sel(&sCom_branch,g_pub_tx.reply);
		if( ret==100 )	break;
		if( ret )
		{
			sprintf(acErrMsg," com_branch_fet_sel erro [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O092");
			return 1;
		}
		strcpy(g_pub_tx.tx_br_no,sCom_branch.br_no);
		strcpy(g_pub_tx.opn_br_no,sCom_branch.br_no);

		if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
         	 sprintf(acErrMsg,"ȡ������ˮ�Ŵ� [%d]",g_pub_tx.trace_no);
         	 WRITEMSG
			 return 1;
		}
		set_zd_long( DC_TRACE_NO , g_pub_tx.trace_no);   /* ��ˮ�� */

		ret=pub_sub_put_intst_dd(sCom_branch.br_no,put_beg_date,put_end_date);
		if( ret )
		{
			sprintf(acErrMsg," pub_sub_put_intst_dd erro");
			WRITEMSG
			return 1;
		}

		ret=pub_sub_put_intst_td(sCom_branch.br_no,put_beg_date,put_end_date);
		if( ret )
		{
			sprintf(acErrMsg," pub_sub_put_intst_td erro");
			WRITEMSG
			return 1;
		}
	}
	Com_branch_Clo_Sel();
	set_zd_data("1204","");
	set_zd_data("1205","");
	set_zd_double("1208",0.00);
	set_zd_data("1214","");
	set_zd_data("1215","");
	set_zd_double("1218",0.00);

	return 0;
}

int pub_sub_put_intst_dd(char br_no[6],long put_beg_date,long put_end_date)
{
	int ret=0;
	double intst_acm=0.00;
	double rate_val=0.00;
	double put_intst=0.00;

	ret=Dd_parm_Dec_Sel(g_pub_tx.reply," put_ind='Y' order by prdt_no" );
	if( ret )
	{
		sprintf(acErrMsg," dd_parm_Dec_Sel erro [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W025");
		return 1;
	}

	while(1)
	{
		ret=Dd_parm_Fet_Sel(&g_dd_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"dd_parm_Fet_Sel erro [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W025");
			return 1;
		}
		if( g_dd_parm.intst_type[0]=='0' )	/** ����Ϣ **/
			continue;
		if( g_dd_parm.put_ind[0]=='N' )		/** ��Ԥ�� **/
			continue;
		strcpy(g_pub_tx.cur_no,g_dd_parm.cur_no);

		/*** ��������Ϣ���� ***/
		ret=sql_sum_double("gl_prdt_dyn_hst","cr_bal",&intst_acm, \
			" \"date\"<=%ld and \"date\">=%ld and br_no='%s' and prdt_cod='%s'", \
			put_end_date,put_beg_date,br_no,g_dd_parm.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"������Ϣ�������� [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P425");
			return 1;
		}
		if( pub_base_CompDblVal(intst_acm,0.00)==0 )
			continue;

		/*** ȡ���� ***/
		ret = pub_base_getllz(g_dd_parm.rate_no,g_dd_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret )
		{
			sprintf(acErrMsg,"pub_base_getllz erro [%d]",ret);
			WRITEMSG
			return 1;
		}
		put_intst=intst_acm*rate_val/360.00/100.00;
		g_pub_tx.tx_amt1=pub_base_PubDround_1(put_intst);

		if( g_dd_parm.cif_type[0]=='1' || g_dd_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"52103");		/** ���˻��� **/
		else 
			strcpy(g_pub_tx.ac_no,"52101");		/** �Թ����� **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"0");			/** �跽 **/
		strcpy( g_pub_tx.ac_id_type,"9" ); 		/*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤,�������ˮ,��ϸ*/
		strcpy( g_pub_tx.prt_ind,"0" ); 		/*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			return 1;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"���õǼǻ����ˮ����!");
			WRITEMSG
			return 1;
		}

		if( g_dd_parm.cif_type[0]=='1' || g_dd_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"26004");	/** ���˻��� **/
		else 
			strcpy(g_pub_tx.ac_no,"26003");	/** �Թ����� **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"1");	/** ���� **/
		strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤,�������ˮ,��ϸ*/
		strcpy( g_pub_tx.prt_ind,"0" ); 	/*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			return 1;
		}

		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A017");
		if( ret )
		{
			sprintf(acErrMsg,"���õǼǻ����ˮ����!");
			WRITEMSG
			return 1;
		}
	}
	Dd_parm_Clo_Sel();

	return 0;
}

int pub_sub_put_intst_td(char br_no[6],long put_beg_date,long put_end_date)
{
	int ret=0;
	double intst_acm=0.00;
	double rate_val=0.00;
	double put_intst=0.00;

	ret=Td_parm_Dec_Sel(g_pub_tx.reply," put_ind='Y' order by prdt_no" );
	if( ret )
	{
		sprintf(acErrMsg," td_parm_Dec_Sel erro [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"W027");
		return 1;
	}

	while(1)
	{
		ret=Td_parm_Fet_Sel(&g_td_parm,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret )
		{
			sprintf(acErrMsg,"td_parm_Fet_Sel erro [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"W027");
			return 1;
		}
		if( g_td_parm.intst_type[0]=='0' )	/** ����Ϣ **/
			continue;
		if( g_td_parm.put_ind[0]=='N' )		/** ��Ԥ�� **/
			continue;
		strcpy(g_pub_tx.cur_no,g_td_parm.cur_no);

		/*** ��������Ϣ���� ***/
		ret=sql_sum_double("gl_prdt_dyn_hst","cr_bal",&intst_acm, \
			" \"date\"<=%ld and \"date\">=%ld and br_no='%s' and prdt_cod='%s'", \
			put_end_date,put_beg_date,br_no,g_td_parm.prdt_no);
		if( ret )
		{
			sprintf(acErrMsg,"������Ϣ�������� [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P425");
			return 1;
		}
		vtcp_log("Hao --- look [%lf]",intst_acm);
		if( pub_base_CompDblVal(intst_acm,0.00)==0 )
			continue;
		/*** zz��ֲ������21-26�Ļ��� ***/
		if( g_pub_tx.tx_date==20040921 )
		{
			intst_acm=intst_acm*(1+0.2);
		}

		/*** ȡ���� ***/
		ret = pub_base_getllz(g_td_parm.rate_no,g_td_parm.cur_no,
			g_pub_tx.tx_date,&rate_val);
		if( ret )
		{
			sprintf(acErrMsg,"pub_base_getllz erro [%d]",ret);
			WRITEMSG
			return 1;
		}
		put_intst=intst_acm*rate_val/360.00/100.00;
		g_pub_tx.tx_amt1=pub_base_PubDround_1(put_intst);
		vtcp_log("Hao --- look [%lf]",rate_val);
		vtcp_log("Hao --- look [%lf]",g_pub_tx.tx_amt1);

		if( !strcmp(g_td_parm.prdt_no,"242") 
			|| !strcmp(g_td_parm.prdt_no,"243")
			|| !strcmp(g_td_parm.prdt_no,"244")
			|| !strcmp(g_td_parm.prdt_no,"281")
			|| !strcmp(g_td_parm.prdt_no,"282")
			|| !strcmp(g_td_parm.prdt_no,"283") )
			strcpy(g_pub_tx.ac_no,"52105");		/** ֪ͨ��� **/
		else if( g_td_parm.cif_type[0]=='1' || g_td_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"52104");		/** ���˶��� **/
		else 
			strcpy(g_pub_tx.ac_no,"52102");		/** �Թ����� **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"0");			/** �跽 **/
		strcpy( g_pub_tx.ac_id_type,"9" ); 		/*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤,�������ˮ,��ϸ*/
		strcpy( g_pub_tx.prt_ind,"0" ); 		/*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			return 1;
		}
		set_zd_data("1204",g_pub_tx.cur_no);
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A016");
		if( ret )
		{
			sprintf(acErrMsg,"���õǼǻ����ˮ����!");
			WRITEMSG
			return 1;
		}

		if( g_td_parm.cif_type[0]=='1' || g_td_parm.cif_type[0]=='5' )
			strcpy(g_pub_tx.ac_no,"26002");		/** ���˶��� **/
		else 
			strcpy(g_pub_tx.ac_no,"26001");		/** �Թ����� **/
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strcpy(g_pub_tx.add_ind,"1");			/** ���� **/
		strcpy( g_pub_tx.ac_id_type,"9" ); 		/*�˻�����Ϊ�ڲ�*/
		strcpy( g_pub_tx.ac_get_ind,"00" );
		strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤,�������ˮ,��ϸ*/
		strcpy( g_pub_tx.prt_ind,"0" ); 		/*������*/
		g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/
		strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/
		ret=pub_acct_trance();
		if( ret )
		{
			sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
			WRITEMSG
			return 1;
		}
		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);
		ret=pubf_acct_proc("A017");
		if( ret )
		{
			sprintf(acErrMsg,"���õǼǻ����ˮ����!");
			WRITEMSG
			return 1;
		}
	}
	Dd_parm_Clo_Sel();

	return 0;
}
