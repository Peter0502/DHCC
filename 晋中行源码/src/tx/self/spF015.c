/*************************************************
* �� �� ��:  spF015.c
* ���������� ATM ȡ��(���п�������atm��ȡ����������ڲ��˲�������)
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2007-2-5 23:57
* NOTE��ԭ��������sp006.c �����ʲ��ֲ�֪��Ϊ��ȥ���ˣ�Ϊ�˲���ԭ�г����ͻ
*       �ش����������滻6203�ֵ�sp006.c,��׼���ڲ����������
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*	6203	0	F003	#=0000@1@#=0001@3@#=0002@4@#$
*	6203	1	F004	#=0000@2@#$
*	6203	2	D003	#=0000@5@#$
*	6203	3	F004	#=0000@6@#$
*	6203	4	F005	#=0000@5@#$
*	6203	5	M002	#$
*	6203	6	D003	#=0000@7@#$
*	6203	7	F015	#=0000@8@#$    new
*	6203	8	A017	#$             new     
* --------������---------------------------------
*	6203	2	AT2	0330	B	0	@0680==0	0400	������2
*	6203	1	AT1	0640	B	0	@0680!=2	0400	������1
*	6203	0	ATM	0370	B	0	@0680==1	0400	������
*	01	ATM	11111111	99999999	KGBF	ATMȡ���շ�	0	0.00000	2.00	2.00
*	01	AT1	11111111	99999999	KGBF	ATM�����з�	0	0.00000	1.50	1.50
*	01	AT2	11111111	99999999	KGBF	ATMȡ����	0	0.00000	2.00	2.00
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "card.h"
int spF015()
{
	int ret;
	struct chnl_tradeadd_c chnl_tradeadd;
	struct chnl_self_trade_c chnl_self_trade;
	char cAcno_qs[25];

	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	memset(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0370",g_pub_tx.crd_no); /* ���� */
	get_zd_data("0370",g_pub_tx.ac_no); /* ���� */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* ֧ȡ���� */
	get_zd_double("0400",&g_pub_tx.tx_amt1); /* ȡ���� */

	sprintf(acErrMsg,"crd_no=[%s]",g_pub_tx.crd_no);
	WRITEMSG

	strcpy(g_pub_tx.cur_no,"01");
	set_zd_data("0210",g_pub_tx.cur_no);

	/*  �Ǽǽ�����ˮ */ 
	strcpy(g_pub_tx.brf,"ATMȡ��"); 
	strcpy(g_pub_tx.ac_no, TESTCARD);
	g_pub_tx.ac_id= TESTCARDID;

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
			if(atm_tx_free_flag==0)
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
	/* ׼������������֧����Ŀ 1.6Ԫ 52701*/
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_OPEN_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"52701",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�������֧����Ŀ����������Ĭ���ʻ�=%s",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0640",cAcno_qs);
	/* ׼�����е渶�����ѿ�Ŀ 2Ԫ  139�ʺ� */
	memset(cAcno_qs, 0 , sizeof(cAcno_qs));
	ret = pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_CARD_USER_FEE,cAcno_qs);
	if (ret)
	{
		memset(cAcno_qs, 0 , sizeof(cAcno_qs));
		strncpy(cAcno_qs,"13902",sizeof(cAcno_qs)-1);
		vtcp_log("%s,%d û���ҵ�����ʻ�����������Ĭ���ʻ�[%s]",__FILE__,__LINE__,cAcno_qs);
	}
	set_zd_data("0330",cAcno_qs);
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

