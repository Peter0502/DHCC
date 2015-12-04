/*************************************************
* �� �� ��:  spF017.c
* ���������� POS ���ѳ���
*            (�ʺŲ��Ǳ��е��̻�)
* ��    ��:  LiuHuafeng
* ������ڣ� 2007-2-9 22:08
* NOTE��ԭ��������sp031.c �����ʲ��ֲ�֪��Ϊ��ȥ���ˣ������ߵķֻ��Ϳ�Ŀ��һ�¡�
*       Ϊ�˲���ԭ�г����ͻ
*       �ش����������滻6302�е�sp022.c,��׼���ڲ����������
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
* tx_def
*1	6302	0	F027	#=0000@1@#$
*2	6302	1	D099	#=0000@2@#$
*3	6302	2	F017	#=0000@3@#$
*4	6302	3	A016	#$
*   ע������com_rollback_ctl
* ---- ע�Ȿ���׽���2007-2-10 û�����������ѱ�������ÿ��Բο�6203 ---
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "chnl_marchinfo_c.h"
#include "card.h"
int spF017()
{
	int ret;
	char flag;
	struct chnl_marchinfo_c chnl_marchinfo;
	struct chnl_self_trade_c chnl_self_trade;
	char cAcno_qs[25];
	long oldTx_date;

	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
			goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no);      /*  ���� */
	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* ���ѽ�� */
/*	get_zd_long("0610",&g_pub_tx.old_trace_no);  ԭ�ն���ˮ�� */

	strcpy(g_pub_tx.cur_no,"01");
	set_zd_data("0210",g_pub_tx.cur_no);
	strcpy(g_pub_tx.ct_ind,"2"); /* ת�� */

	get_zd_long("0440", &g_pub_tx.old_trace_no);  /* ���彻�׵�ԭƽ̨��ˮ */
	get_zd_long("0450", &oldTx_date);  /* ���彻�׵�ԭ�������� */

	/* ����ԭ������ˮ��״̬ */
/*	ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"trace_no=%ld",
		g_pub_tx.old_trace_no);
changed by chenming 2007.07.28. */
	ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"stra=%ld and dz_date=%ld ",
		g_pub_tx.old_trace_no, oldTx_date);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯԭPOS���ѵ������豸�����ռǴ���!![%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	ret = Chnl_self_trade_Fet_Upd(&chnl_self_trade,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Fetch chnl_self_trade error [%d]",ret);
		WRITEMSG
			Chnl_self_trade_Clo_Upd();
		goto ErrExit;
	}
	strcpy(chnl_self_trade.stat,"1");
	chnl_self_trade.strb = g_pub_tx.trace_no;

	ret = Chnl_self_trade_Upd_Upd(chnl_self_trade,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Update chnl_self_trade error [%d]",ret);
		WRITEMSG
			Chnl_self_trade_Clo_Upd();
		goto ErrExit;
	}
	Chnl_self_trade_Clo_Upd();

	/* �������豸�����ռǽṹ��ֵ */
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);

	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);

	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	strcpy(chnl_self_trade.tel,g_pub_tx.tel);
	strcpy(chnl_self_trade.ttyp,"2");
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	strcpy(chnl_self_trade.spcd,chnl_marchinfo.mrchno);
	/* �����׶���������ˮ�� */
	get_zd_long("0520", &chnl_self_trade.stra);

	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
	chnl_self_trade.sl_sxamt = 0.00;
	chnl_self_trade.fk_sxamt = 0.00;

	chnl_self_trade.strb = g_pub_tx.old_trace_no;

	strcpy(chnl_self_trade.dz_flag,"0");
	strcpy(chnl_self_trade.ct_ind,"2");
	strcpy(chnl_self_trade.info_source,"1");
	strcpy(chnl_self_trade.deal_flag,"1");
	strcpy(chnl_self_trade.brf_no,"PSCX");
	strcpy(chnl_self_trade.add_ind,"1");
	strcpy(chnl_self_trade.batch_flag,"0");
	strcpy(chnl_self_trade.stat,"0");

	/* �Ǽ������豸�����ռ� */
	ret = pub_base_Insertselftrade(chnl_self_trade);
	if (ret != 0)
	{
		sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
		WRITEMSG
			goto ErrExit;
	}
	/* �����ڲ�������� */
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_QS,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,TESTCARD,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������Ĵ��̫ԭ���Ϻ����㣩�����������Ĭ���ʻ�[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("1201",cAcno_qs);
	set_zd_data("1204",g_pub_tx.cur_no);
	set_zd_data("1204","2");/* ת�� */
	set_zd_double("1208",g_pub_tx.tx_amt1);
	set_zd_data("1209",g_pub_tx.brf);

	/*  �Ǽǽ�����ˮ */
	strcpy(g_pub_tx.brf,"POS����!!");

	strcpy(g_pub_tx.ac_no, TESTCARD);
	g_pub_tx.ac_id= TESTCARDID;
	strcpy(g_pub_tx.ct_ind, "2");

	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
		WRITEMSG
			goto ErrExit;
	}
OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
		sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

