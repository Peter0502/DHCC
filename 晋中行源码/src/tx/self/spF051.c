/*************************************************
* �� �� ��:  spF050.c
* ���������� POS ���ѳ�������
*
* ��    ��:  rob
* ������ڣ� 2003��6��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"

int spF051()
{

    int ret;
    long cx_date;
    long oldTtrace_no;
    long trace_no;
    char flag[2];
    struct chnl_tradeadd_c chnl_tradeadd;
    struct chnl_self_trade_c chnl_self_trade;

    memset(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
    memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
    memset(flag,0x00,sizeof(flag));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
    get_zd_data("0800",g_pub_tx.draw_pwd); /* ֧ȡ���� */
    get_zd_double("0400",&g_pub_tx.tx_amt1); /* ���׽�� */
	get_zd_long("0440", &oldTtrace_no); /* �����̨��ˮ */
	get_zd_long("0450", &cx_date); /* �������� */
	get_zd_long("0520", &trace_no); /* ��̨��ˮ */
	

	set_zd_data("0210","01");  /* ���� */
	strcpy(g_pub_tx.cur_no, "01");
/*	cx_date = g_pub_tx.tx_date;*/


	sprintf(acErrMsg,"------->��������: [%s], ���: [%lf], ����: [%ld], \
			��ˮ��: [%ld]", g_pub_tx.crd_no, g_pub_tx.tx_amt1, cx_date,
			oldTtrace_no);
	WRITEMSG


    /* ��鿨�ĺϷ��� *
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }

    * ��鿨��֧ȡ��ʽ *
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        goto ErrExit;
    }

    * ���֧ȡ��ʽ�ĺϷ��� *
    ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
                      g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����֧ȡ�������!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        goto ErrExit;
    }
	****/

	/* ��ѯ�����豸�����ռ� */
	ret = Chnl_self_trade_Sel(g_pub_tx.reply,&chnl_self_trade,"stra=%ld and dz_date=%ld and tx_amt=%lf", 
			oldTtrace_no, cx_date,  g_pub_tx.tx_amt1);
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"��ѯ�����豸�����ռǱ����!! [%d]",ret); 
		WRITEMSG 
		sprintf(acErrMsg,"trace_no=[%ld]",oldTtrace_no); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	/* ���ñ���ˮ�Ƿ����� */ 
	if (chnl_self_trade.stat[0] != '0') 
	{ 
		sprintf(acErrMsg,"�ñ���ˮ������������!!"); 
		WRITEMSG 
		strcpy(g_pub_tx.reply, "AT13"); 
		goto ErrExit; 
	}

	/* �������Ŀ��ź�ԭ������ˮ�е��Ƿ����� */ 
	pub_base_strpack(chnl_self_trade.ac_no); 
	pub_base_strpack(g_pub_tx.crd_no); 
	
	if (strcmp(chnl_self_trade.ac_no,g_pub_tx.crd_no) != 0) 
	{ 
		sprintf(acErrMsg,"����Ŀ��ź�ԭ������ˮ�еĿ��Ų�һ��!!"); 
		WRITEMSG 
		sprintf(acErrMsg,"chnl_self_trade: [%s], g_pub_tx: [%s]", 
		chnl_self_trade.ac_no,g_pub_tx.crd_no); 
		WRITEMSG 
		strcpy(g_pub_tx.reply, "AT13"); 
		goto ErrExit; 
	} 
	
	/* ���ñ���ˮ�Ƿ���������� 
		���ܶ��ѳ����Ľ��׽���ȡ�������ܶ��ѳ����Ľ����ٴγ�����
	
	pub_base_strpack(chnl_self_trade.brf_no); 
	if (strcmp(chnl_self_trade.brf_no,"PSXF") != 0) 
	{ 
		sprintf(acErrMsg,"�ñ���ˮ�������������� [%s]",chnl_self_trade.brf_no); 
		WRITEMSG 
		strcpy(g_pub_tx.reply, "AT13"); 
		goto ErrExit; 
	}*/ 

	/* ���ñ���ˮ�Ƿ��ǵ��ս��� */
	if (chnl_self_trade.tx_date != g_pub_tx.tx_date)
	{
		sprintf(acErrMsg,"���ղ�������������!!!"); 
		WRITEMSG 
		strcpy(g_pub_tx.reply, "AT13"); 
		goto ErrExit; 
	}

	set_zd_long("0440", chnl_self_trade.trace_no); /* �����̨��ˮ */

	sprintf(acErrMsg,"update chnl_self_trade set stat='2', strb=%ld where \
			dz_date=%ld and stra=%ld and ac_no='%s' and tx_amt=%lf",
			trace_no,cx_date, oldTtrace_no, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	WRITEMSG	

    /* ����ԭ������ˮ��״̬Ϊ������״̬ */
	ret = sql_execute_ur("update chnl_self_trade set stat='2', strb=%ld where \
			dz_date=%ld and stra=%ld and ac_no='%s' and tx_amt=%lf",
			trace_no,cx_date, oldTtrace_no, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	if (ret <= 0)
	{
		sprintf(acErrMsg,"�޸������豸��״̬����!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
		goto ErrExit;
	}

	sprintf(acErrMsg,"update chnl_self_trade set stat='0' where \
			dz_date=%ld and stra=%ld and ac_no='%s' and tx_amt=%lf",
			cx_date, chnl_self_trade.strb, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	WRITEMSG	
	
	/* ���±���������ˮ��״̬Ϊ����״̬ */
	ret = sql_execute_ur("update chnl_self_trade set stat='0' where \
			dz_date=%ld and stra=%ld and ac_no='%s' and tx_amt=%lf",
			cx_date, chnl_self_trade.strb, g_pub_tx.crd_no, g_pub_tx.tx_amt1);
	if (ret <= 0)
	{
		sprintf(acErrMsg,"�޸������豸��״̬����!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
		goto ErrExit;
	}

	memset(&chnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));

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

	/* �����׶���������ˮ�� */ 
	get_zd_long("0520", &chnl_self_trade.stra);
	get_zd_long("0530", &chnl_self_trade.strb);

	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1; 
	chnl_self_trade.sl_sxamt = 0.00; 
	chnl_self_trade.fk_sxamt = 0.00;

	strcpy(chnl_self_trade.dz_flag,"0"); 
	strcpy(chnl_self_trade.ct_ind,"2"); 
	strcpy(chnl_self_trade.info_source,"0"); 
	strcpy(chnl_self_trade.deal_flag,"1"); 
	strcpy(chnl_self_trade.brf_no,"PXCZ"); 
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

	/* ��ѯ�����ʻ����ļ� */ 
	strcpy(g_pub_tx.cur_no,"01"); 
	
	ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"��ѯ�����ʻ����ļ�����!!!"); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"AT06"); 
		goto ErrExit; 
	} 
	
	g_dd_mst.bal = g_dd_mst.bal - g_pub_tx.tx_amt1; 
	set_zd_double("0420",g_dd_mst.bal);   /* �ʻ���� */ 
	set_zd_data("0270",g_dd_mst.name);    /* ���� */

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


