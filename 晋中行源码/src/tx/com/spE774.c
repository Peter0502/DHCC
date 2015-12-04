/***************************************************************
* �� �� ��: spD099.c
* ����������ȯ�����
* ��    ��: jack
* ������ڣ�2005��1��11��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

struct st_money_dtl st_mdtl;
int spE774()
{
	int i=0;

  memset( &st_mdtl, 0x00, sizeof(st_mdtl) );

  pub_base_sysinit();
	get_zd_data( "0950", (char *)&st_mdtl );	
	get_zd_data("0210",g_pub_tx.cur_no);				
  for( i=1;i<=13;i++ ) {
  	((char*)&st_mdtl)[i*17-1] = 0x00;
  	vtcp_log("st_mdtl[%d]----[%s][%d]",i,((char *)&st_mdtl+(i-1)*17),atoi(((char *)&st_mdtl+(i-1)*17)));
  }

/****modify by wanglei 20070426 ������98��Ա��ȯ������****/
	if(memcmp(g_pub_tx.tel+2,"98",2) != 0)
	{
		if( i=prt_acct_mbox() ) {
				sprintf(acErrMsg,"����Ǯ�䴦����ʧ��!");
				WRITEMSG
				goto ErrExit;
		}
 
	}
/****end by wanglei 20070426****/
 
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
prt_acct_mbox()
{
	int ret=0;
	char cCer_sign[2]; /***ȯ��ǼǱ�־   wanglei 20070417***/
	double dTotamt = 0.00;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c;
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));
	memset(cCer_sign,0x00,sizeof(cCer_sign));

	strcpy( g_pub_tx.ct_ind,"1" );
	strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );
	get_zd_data("0700",cCer_sign);

	ret=Cash_mst_Dec_Upd(g_pub_tx.reply,
		" br_no='%s' and cur_no='%s' and tel='%s' ",
		g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit1;
	}

	ret=Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
	vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�˹�Ա��Ǯ��[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P154");
		goto ErrExit1;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit1;
	}

	ret = pub_base_ChkCurno( g_pub_tx.cur_no );
	if ( ret )
	{
		sprintf( acErrMsg, "�����ִ�" );
		WRITEMSG
		goto ErrExit1;
	}

	if( cash_mst_c.sts[0]!='0' )
	{
		sprintf(acErrMsg,"�ֽ��˺�δ���û�״̬������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P150");
		goto ErrExit1;
	}
	strcpy(g_pub_tx.ac_no,cash_mst_c.ac_no);

  cash_mst_c.t10000 += atoi(st_mdtl.money100);
  cash_mst_c.t5000 += atoi(st_mdtl.money50);
  cash_mst_c.t2000 += atoi(st_mdtl.money20);
  cash_mst_c.t1000 += atoi(st_mdtl.money10);
  cash_mst_c.t500 += atoi(st_mdtl.money5);
  cash_mst_c.t200 += atoi(st_mdtl.money2);
  cash_mst_c.t100 += atoi(st_mdtl.money1);
  cash_mst_c.t50 += atoi(st_mdtl.money05);
  cash_mst_c.t20 += atoi(st_mdtl.money02);
  cash_mst_c.t10 += atoi(st_mdtl.money01);
  cash_mst_c.t5 += atoi(st_mdtl.money005);
  cash_mst_c.t2 += atoi(st_mdtl.money002);
  cash_mst_c.t1 += atoi(st_mdtl.money001);

  	if( cash_mst_c.t10000<0 || \
		  cash_mst_c.t5000<0 || \
		  cash_mst_c.t2000<0 || \
		  cash_mst_c.t1000<0 || \
		  cash_mst_c.t500<0 || \
		  cash_mst_c.t200<0 || \
		  cash_mst_c.t100<0 || \
		  cash_mst_c.t50<0 || \
		  cash_mst_c.t20<0 || \
		  cash_mst_c.t10<0 || \
		  cash_mst_c.t5<0 || \
		  cash_mst_c.t2<0 || \
		  cash_mst_c.t1<0 )
	{
		sprintf(acErrMsg,"ȯ�𲻶�!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"PM039");
		goto ErrExit1;
	}

	ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"Ǯ�����ļ��쳣���������Ļ�����ϵ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit1;
	}
	Cash_mst_Clo_Upd();

	/****�ǼǴ˽���ʱ��֮ǰ��Ա��ȯ����ܺ����ֽ���������ȡ����򣬲���������Ǽ� add by wanglei 20070501****/
	dTotamt = cash_mst_c.t10000*100+cash_mst_c.t5000*50+cash_mst_c.t2000*20+cash_mst_c.t1000*10+cash_mst_c.t500*5+cash_mst_c.t200*2+cash_mst_c.t100*1+cash_mst_c.t50*0.5+cash_mst_c.t20*0.2+cash_mst_c.t10*0.1+cash_mst_c.t5*0.05+cash_mst_c.t2*0.02+cash_mst_c.t1*0.01;
	if(pub_base_CompDblVal(dTotamt, cash_mst_c.bal) != 0)
	{
		sprintf(acErrMsg,"�ǼǴ˱�֮ǰ�Ĺ�Աȯ��ϼ��ܶ����ֽ�����Totamt[%lf]!",dTotamt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M038");
		goto ErrExit1;
	}

	strcpy( g_pub_tx.dc_code,NULL_DC_CODE );

	cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
    vtcp_log("B TRACE [%d]",g_pub_tx.trace_no );
	cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
	strcpy(cash_mst_hst_c.ac_no,g_pub_tx.ac_no);
	strcpy(cash_mst_hst_c.tw_ac_no,g_pub_tx.name);
	strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
	strcpy(cash_mst_hst_c.add_ind,cCer_sign);   /***��ȯ��ǼǱ�־������ϸ��***/
	cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
	cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(cash_mst_hst_c.brf,g_pub_tx.brf);
	strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
	cash_mst_hst_c.t10000 = atoi(st_mdtl.money100);
  cash_mst_hst_c.t5000 = atoi(st_mdtl.money50);
  cash_mst_hst_c.t2000 = atoi(st_mdtl.money20);
  cash_mst_hst_c.t1000 = atoi(st_mdtl.money10);
  cash_mst_hst_c.t500 = atoi(st_mdtl.money5);
  cash_mst_hst_c.t200 = atoi(st_mdtl.money2);
  cash_mst_hst_c.t100 = atoi(st_mdtl.money1);
  cash_mst_hst_c.t50 = atoi(st_mdtl.money05);
  cash_mst_hst_c.t20 = atoi(st_mdtl.money02);
  cash_mst_hst_c.t10 = atoi(st_mdtl.money01);
  cash_mst_hst_c.t5 = atoi(st_mdtl.money005);
  cash_mst_hst_c.t2 = atoi(st_mdtl.money002);
  cash_mst_hst_c.t1 = atoi(st_mdtl.money001);

	/**��g_pub_tx.tx_amt1��ֵ,�Ҹ�����ϸ�� add by wanglei  20070420**/
	g_pub_tx.tx_amt1 = cash_mst_hst_c.t10000*100+cash_mst_hst_c.t5000*50+cash_mst_hst_c.t2000*20+cash_mst_hst_c.t1000*10+cash_mst_hst_c.t500*5+cash_mst_hst_c.t200*2+cash_mst_hst_c.t100*1+cash_mst_hst_c.t50*0.5+cash_mst_hst_c.t20*0.2+cash_mst_hst_c.t10*0.1+cash_mst_hst_c.t5*0.05+cash_mst_hst_c.t2*0.02+cash_mst_hst_c.t1*0.01;
	cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
	vtcp_log("[%s][%d] tx_amt1 = [%.2lf]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);

	ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"�Ǽ�Ǯ����ϸ����![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit1;
	}

	/**��g_pub_tx.tx_amt1��ֵ��������ˮ add by wanglei 20070410**/
	vtcp_log("[%s][%d] tx_amt1 = [%.2lf]\n",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit1;
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit1:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Ǯ�䴦��ɹ�!");
	WRITEMSG
	return 0;
	
ErrExit1:
	sprintf(acErrMsg,"Ǯ�䴦��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

