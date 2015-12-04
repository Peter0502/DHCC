/*************************************************
* �� �� ��:  spF016.c
* ���������� POS ����(�µ�POS����׼������������)
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2007-2-9 22:08
* NOTE��ԭ��������sp022.c �����ʲ��ֲ�֪��Ϊ��ȥ���ˣ�Ϊ�˲���ԭ�г����ͻ
*       �ش����������滻6301�е�sp022.c,��׼���ڲ����������
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
* tx_def
*	6301	0	F021	#=0000@1@#$
*	6301	1	D003	#=0000@2@#$
*	6301	2	F016	#=0000@3@#$
*	6301	3	A017	#$
* ---- ע�Ȿ���׽���2007-2-10 û�����������ѱ�������ÿ��Բο�6203 ---
*  ע������com_rollback_ctl
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_self_trade_c.h"
#include "card.h"

int spF016()
{
	int ret=0;
	char crd_no[25];
	char cAcno_qs[25];
	struct chnl_self_trade_c chnl_self_trade;

	memset(&chnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
	memset(crd_no, 0x0, sizeof(crd_no));
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no); /* ���� */ 
	get_zd_double("0400",&g_pub_tx.tx_amt1); /* ���׽�� */ 

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
	/* ǰ̨��������Added by ChenMing 2007.07.28.*/
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

	/* �Կ��Ǽǲ����д��� */
	{
		char cTradetype[4];/* ������� */
		int iSelflag=0;  /*������֯��־ */
		int atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */
		int pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */
		int pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */
		int atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */
		int iDscpt=0;/* ��ת��ѯ��־ */

		iSelflag=0;  /*������֯��־ */                        
		atm_qy_free_flag=0;/* atm��ѯ��ѱ�־ 0��� 1�շ� */  
		pos_qy_free_flag=0;/* pos ��ѯ��ѱ�־ 0��� 1�շ� */ 
		pos_tx_free_flag=0;/* pos ������ѱ�־ 0��� 1�շ� */ 
		atm_tx_free_flag=0;/* atm ������ѱ�־ 0��� 1�շ� */ 
		iDscpt=0;/* ��ת��ѯ��־ */                           

		memset(cTradetype, 0 , sizeof(cTradetype));
		iGetTradeType(cTradetype,&iSelflag,&iDscpt);
		g_reply_int=pub_card_reg(g_pub_tx.tx_amt1,cTradetype,'0',&atm_qy_free_flag,
			&pos_qy_free_flag,&pos_tx_free_flag,&atm_tx_free_flag,iSelflag,iDscpt);
		if(g_reply_int)
		{
			sprintf(acErrMsg,"�Ǽǿ��Ǽǲ�����");
			strncpy(g_pub_tx.reply,"AT06",4);
			WRITEMSG
			vtcp_log("%s,%d �������׵Ǽǲ����� ret=[%d]",__FILE__,__LINE__,g_reply_int);
			goto ErrExit;
		}
		/* ����ATM��������ѱ�־ */
		if(iSelflag==0)
		{
			/* ��������֯�ڵĶ�����ȡ���� */
			vtcp_log("%s,%d �ͻ������������������շ�",__FILE__,__LINE__);
			set_zd_data("0680","2");
		}
		else
		{
			if(pos_tx_free_flag==0)
			{
				/* ������ */
				set_zd_data("0680","0");
				vtcp_log("%s,%d ��ϲ�������",__FILE__,__LINE__);
			}
			else
			{
				set_zd_data("0680","1");
				vtcp_log("%s,%d �շ�û������",__FILE__,__LINE__);
			}
		}
	}
	/*** note �������ڲ�����Ƿ��շ�������ʱ�趨���շ�,����շ���ô������������շѲ��� **/
	vtcp_log("%s %d ע���������ö����շ�",__FILE__,__LINE__);
	set_zd_data("0680","2");
	vtcp_log("%s %d ע���������ö����շ�",__FILE__,__LINE__);
	/* �����趨�շѿ�Ŀ
	* ׼������������֧����Ŀ 1.6Ԫ 52701  *
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_OPEN_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"52701",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������֧����Ŀ����������Ĭ���ʻ�=%s",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0640",cAcno_qs);
	* ׼�����е渶�����ѿ�Ŀ 2Ԫ  139�ʺ� *
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_USER_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"13902",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�����ʻ�����������Ĭ���ʻ�[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0330",cAcno_qs);
	******************************************************/
	/** note �������ڲ�����Ƿ��շ�������ʱ�趨���շ� **/
	/* �����ڲ�������� */
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_QS,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,TESTCARD,sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������Ĵ��̫ԭ���Ϻ����㣩�����������Ĭ���ʻ�[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("1211",cAcno_qs);
	set_zd_data("1214",g_pub_tx.cur_no);
	set_zd_data("1214","2");/* ת�� */
	set_zd_double("1218",g_pub_tx.tx_amt1);
	set_zd_data("1219",g_pub_tx.brf);

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

