/*************************************************
* �� �� ��:  sp9205.c
* ���������� Ǯ�����
*
* ��    ��:  wyb
* ������ڣ� 2006��12��12��
*
* �޸ļ�¼�� 
* ��    ��:  20070410
* �� �� ��:  wanglei
* �޸�����:  ��������������Ϊdouble
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"

int sp9205()
{
	int ret=0;
	char brno[6],tel[5],cur_no[3];

	/***
	acbal[17],mbbal[17];
	***/

	double acbal,mbbal;
	acbal = 0.00;
	mbbal = 0.00;
	struct cash_mst_c g_cash_mst;

  memset( &g_cash_mst, 0x00, sizeof(g_cash_mst) );

  pub_base_sysinit();
	get_zd_data( "0030", brno );					
  get_zd_data("0070",tel);
  get_zd_data("0210",cur_no);
  vtcp_log("cash_mst_info----[%s][%s][%s]",brno,tel,cur_no);
	ret = Cash_mst_Sel(g_pub_tx.reply,&g_cash_mst,"br_no='%s' and tel='%s' and cur_no='%s'",brno,tel,cur_no);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڸù�Ա!!brno=[%s]tel=[%s]curno[%s]",brno,tel,cur_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C119");
		goto ErrExit;
	}
/* del by martin 2008-11-23 9:33
ret = Cash_mst_Sel(g_pub_tx.reply,&g_cash_mst,"br_no='%s' and tel='%s' and cur_no='%s'",brno,tel,cur_no);
	if( ret )
	{
		sprintf(acErrMsg,"�����ڸù�Ա!!brno=[%s]tel=[%s]curno[%s]",brno,tel,cur_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C119");
		goto ErrExit;
	}*/
	double s_sum_amt=0,f_sum_amt=0;
	/* �����ϴ�ȯ������󱾹�Ա���շ�������ϼ� */
	ret=sql_sum_double("cash_mst_hst","amt",&s_sum_amt," add_ind='1' and cur_no='%s' and tel='%s' and tx_date=%d and trace_no >(select nvl(max(trace_no),0) from cash_mst_hst where tel='%s' and tx_date=%d and add_ind='9') ",cur_no,tel,g_pub_tx.tx_date,tel,g_pub_tx.tx_date);
	if( ret )
	{
		sprintf(acErrMsg,"�շ��ϼƳ���!!brno=[%s]tel=[%s]curno[%s]",brno,tel,cur_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C119");
		goto ErrExit;
	}
	/* �����ϴ�ȯ������󱾹�Ա�ĸ���������ϼ� */
	ret=sql_sum_double("cash_mst_hst","amt",&f_sum_amt," add_ind='0' and cur_no='%s' and tel='%s' and tx_date=%d and trace_no >(select nvl(max(trace_no),0) from cash_mst_hst where tel='%s' and tx_date=%d and add_ind='9') ",cur_no,tel,g_pub_tx.tx_date,tel,g_pub_tx.tx_date);
	if( ret )
	{
		sprintf(acErrMsg,"�����ϼƳ���!!brno=[%s]tel=[%s]curno[%s]",brno,tel,cur_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"C119");
		goto ErrExit;
	}
	acbal = s_sum_amt-f_sum_amt;
	vtcp_log("[%s][%d]s_amt[%.2f],f_amt[%.2f]\n",__FILE__,__LINE__,s_sum_amt,f_sum_amt);
	char s_acbal[30];
	memset( s_acbal, 0x00, sizeof(s_acbal) );
	sprintf( s_acbal, "%.2f", acbal);
	set_zd_data("0250",s_acbal);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);

	vtcp_log("[%s][%d] mst.bal =[%.2lf]\n",__FILE__,__LINE__,g_cash_mst.bal);
	/***
	memset( mbbal, 0x00, sizeof(mbbal) );
	sprintf( mbbal, "%.2f", g_cash_mst.t10000*10000+g_cash_mst.t5000*5000+g_cash_mst.t2000*2000+g_cash_mst.t1000*1000+g_cash_mst.t500*500+g_cash_mst.t200*200+g_cash_mst.t100*100+g_cash_mst.t50*50+g_cash_mst.t20*20+g_cash_mst.t10*10+g_cash_mst.t5*5+g_cash_mst.t2*2+g_cash_mst.t1*1 );
	***/

	mbbal = g_cash_mst.t10000*100+g_cash_mst.t5000*50+g_cash_mst.t2000*20+g_cash_mst.t1000*10+g_cash_mst.t500*5+g_cash_mst.t200*2+g_cash_mst.t100*1+g_cash_mst.t50*0.5+g_cash_mst.t20*0.2+g_cash_mst.t10*0.1+g_cash_mst.t5*0.05+g_cash_mst.t2*0.02+g_cash_mst.t1*0.01;
	vtcp_log("bal_info----[%.2lf][%.2lf]",acbal,mbbal);
	set_zd_double("0410",mbbal);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
