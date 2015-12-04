/*************************************************
* �� �� ��:  spF022.c
* ���������� POS ����
*
* ��    ��:  rob
* ������ڣ� 2003��2��19��
*
* �޸ļ�¼�� 
*   ��   ��:  2003/06/21
*   �� �� ��: rob
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "card.h"

int spF022()
{

	int ret=0;
	char crd_no[25];

	struct chnl_self_trade_c chnl_self_trade;

	memset(&chnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
	memset(crd_no, 0x0, sizeof(crd_no));

    	if (pub_base_sysinit())
    	{
        	sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        	WRITEMSG
        	goto ErrExit;
    	}

	get_zd_data("0370",g_pub_tx.crd_no); /* ���� */ 
	get_zd_double("0400",&g_pub_tx.tx_amt1); /* ���׽�� */ 

    	g_reply_int=pubf_card_chk_msr(3,g_pub_tx.crd_no);
    	if(g_reply_int)
    	{
        	sprintf(acErrMsg,"�����Ǽǲ�cvn����!!");
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "AT01");
        	goto ErrExit;
    	}

	
	strcpy(g_pub_tx.cur_no,"01"); 
	set_zd_data("0210",g_pub_tx.cur_no); 
	strcpy(g_pub_tx.ct_ind,"2"); 
	
	strcpy(g_pub_tx.opn_br_no, g_pub_tx.tx_br_no); 
	
	/* �������豸�����ռǽṹ��ֵ */ 
	chnl_self_trade.tx_date = g_pub_tx.tx_date; 
	chnl_self_trade.tx_time = g_pub_tx.tx_time; 
	chnl_self_trade.trace_no = g_pub_tx.trace_no; 
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no); 

	get_zd_data("0300", crd_no);
	/* rem by LiuHuafeng 20070127
	if (strncmp(crd_no, "694302", 6) == 0)
	******replace by next line**********/
	if ((strncmp(crd_no, CARDHEADCODE , 6) == 0 || strncmp(crd_no, ICCARDHEADCODE , 6) == 0) && memcmp(crd_no+6,CARD_UNION_AREA,2) ==0 )
	{
		get_zd_data("0300", chnl_self_trade.ac_no);
	}
	else
	{
		strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no); 
	}
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code); 
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no); 
	strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no); 
	strcpy(chnl_self_trade.tel,g_pub_tx.tel); 
	strcpy(chnl_self_trade.ttyp,"2"); 
	strcpy(chnl_self_trade.sctp,"PS"); 
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty); 

	/* �����׶���������ˮ�� */
	get_zd_long("0520", &chnl_self_trade.stra);
	get_zd_long("0450", &chnl_self_trade.dz_date);
	get_zd_data("0380", chnl_self_trade.ttyn);
	get_zd_data("0620", chnl_self_trade.spcd);

	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1; 
	chnl_self_trade.sl_sxamt = 0.00; 
	chnl_self_trade.fk_sxamt = 0.00; 
	
	strcpy(chnl_self_trade.dz_flag,"0"); 
	strcpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind); 
	strcpy(chnl_self_trade.info_source,"1"); 
	strcpy(chnl_self_trade.deal_flag,"1"); 
	strcpy(chnl_self_trade.brf_no,"PSXF"); 
	strcpy(chnl_self_trade.add_ind,"0"); 
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

