/*************************************************
* �� �� ��:  sp9324.c
* ���������� ATM���峮���ֽ����,�������Ȳ�ѯ���,��ȥ���ֽ����
	     ���̿�����ɹ�Ա��������ֽ�������ӳ�
*
* ��    ��:  andy
* ������ڣ� 2004��3��12��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_connect_c.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "com_tel_connect_c.h"

int 
sp9324()
{
	int             ret;
	char            teller[7];
	char            atm_tel[7];
	char            clearflag[2];
	char            tjfs[2];
	double atm_amt;
	struct com_tel_connect_c g_com_tel_connect;
	struct com_tel_c sCom_tel_c;
	struct com_tel_c st_com_tel;
	struct com_tel_c st_atm_tel;
	struct cash_mst_c st_atm_cash;
	struct com_tel_connect_c st_cash_connect;
	memset(&g_com_tel_connect, 0x00, sizeof(struct com_tel_connect_c));
	memset(&st_atm_cash, 0x00, sizeof(struct cash_mst_c));
	memset(&st_cash_connect, 0x00, sizeof(struct com_tel_connect_c));
	memset(&sCom_tel_c, 0x00, sizeof(sCom_tel_c));
	memset(&st_com_tel, 0x00, sizeof(st_com_tel));
	memset(&st_atm_tel, 0x00, sizeof(st_atm_tel));
	memset(clearflag, 0x00, sizeof(clearflag));
	memset(tjfs, 0x00, sizeof(tjfs));
	memset(teller, 0x00, sizeof(teller));
	memset(atm_tel, 0x00, sizeof(atm_tel));

	/* ϵͳ��ʼ������ʼ��g_pub_tx)���ݳ�ʼ�� */
	pub_base_sysinit();

	get_zd_data("0670", tjfs);	/* ������ʽ */
	get_zd_data("0680", clearflag);	/* ���峮��־ 0�ӳ�1�峮 */
	get_zd_data("0920", teller);	/* �Է���Ա */
	get_zd_data("0070", atm_tel);	/* ATM��Ա */
	get_zd_data("0210", g_pub_tx.cur_no);	/* ���� */
	get_zd_double("0410", &g_pub_tx.tx_amt1);	/* ������ */
		vtcp_log("�峮��־[%s]",clearflag);
		vtcp_log("������ʽ[%s]",tjfs);
	if(!memcmp(clearflag,"1",1)){/*������ATM�峮��Ҫ���������41��*/
		/*****************
		��֤�ӳ�Ϊͬһ������Ա,�Ҽ���Ϊ4
		******************/
		vtcp_log("�峮");
		ret = Com_tel_Sel(g_pub_tx.reply, &st_atm_tel, "tel='%s' ",atm_tel);
		if(ret)
		{
			sprintf(acErrMsg, "��ѯATM��Ա����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		ret = Com_tel_Sel(g_pub_tx.reply, &st_com_tel, "tel='%s' and lvl='4' and br_no='%s' and csts='0' ",teller,st_atm_tel.br_no);
		if(ret)
		{
			sprintf(acErrMsg, "��ѯ�ӳ���Ա����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		ret = Cash_mst_Sel(g_pub_tx.reply, &st_atm_cash, "tel='%s'",atm_tel);
		if(ret)
		{
			sprintf(acErrMsg, "��ѯATM��ԱǮ�����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		set_zd_double("0410", st_atm_cash.bal);
	}else if(!memcmp(clearflag,"0",1)){
		/*����ATM�ӳ�ȡ���Ա���������ATM���Ƚ�*/
		vtcp_log("�ӳ�");
		ret = Cash_mst_Sel(g_pub_tx.reply, &st_atm_cash, "tel='%s'",atm_tel);
		if(ret)
		{
			sprintf(acErrMsg, "Ǯ���ѯ����!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "C118");
			goto ErrExit;
		}
		vtcp_log("�ӳ�ATMǮ��[%f]",st_atm_cash.bal);
		if(st_atm_cash.bal>0.001){
			sprintf(acErrMsg, "Ǯ�䲻Ϊ�ղ��ܼӳ�!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "CU12");
			goto ErrExit;
		}
		ret = Com_tel_connect_Sel(g_pub_tx.reply, &st_cash_connect, "tel='%s' and tw_tel='%s' and ind='A'",teller,atm_tel);
		get_zd_double("0390", &atm_amt);	/* ������ */
		vtcp_log("%s %d cashamt[%f],atmamt[%f]",__FILE__,__LINE__,st_cash_connect.amt,atm_amt);
		if (pub_base_CompDblVal(st_cash_connect.amt,atm_amt)!=0){
			sprintf(acErrMsg, "�ӳ��������Ա������һ��!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "CU13");
			goto ErrExit;
		}
	}
	if(!memcmp(tjfs,"1",1)){/*����*/
		strcpy(g_pub_tx.reply, "0001");
	}else if(!memcmp(tjfs,"2",1)){/*����*/
		strcpy(g_pub_tx.reply, "0002");
	}else{
		sprintf(acErrMsg, "������˽���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "CU12");
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
