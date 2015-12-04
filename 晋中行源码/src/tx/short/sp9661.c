/*************************************************
* �� �� ��: sp9661.c
* �������������ڲ�Ʒת����,����������ʾ���ڲ�Ʒ�б�
* ��    ��: lance
* ������ڣ�2003��06��06��
* �޸ļ�¼��   
*     1. ��    ��:
*        �� �� ��:
*        �޸�����:
TX_CODE	TX_NAME	BIT_MAP	LOG_IND	TX_TYPE	WRK_STS	YW_CNT_IND	YW_PERCENT
9661	����ת����Ʒѡ��	10000000000000000000000100100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000	0	4	0	3	
TX_CODE	FLOW_CODE	SUB_TX_CODE	SUB_TX_RSLT
9661	0	9661	#$
SUB_TX_CODE	SUB_TX_NAME	TX_FUNC	ACCT_IND	CHNL_IND
9661	���ת����Ʒѡ��	sp9661	0	0
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"

sp9661()
{
	/** ���ݳ�ʼ�� **/
	int             ret = 0;
	int             sel_flag = 0;
	int             vcnt = 0;
	char            trust_flg[2];
	char            cif_type[2];
	char            time_typ[2];
	char            wherelist[200];
	char            tmpstr[100];
	char            filename[100];
	FILE           *fp;
	int             i = 0;

	/** ȡֵ����ֵ **/
	get_zd_data("0680", cif_type);	/* �ͻ����� */


	TRACE

	/*
	 * 1�����ˣ� 2����˾ ��3��������ָ�������ص���֯���� 4�����ڻ�����
	 * 5���ڲ�Ա�� 
	 */
	if (cif_type[0] != '0') {
		sprintf(wherelist, " (cif_type='%s' or cif_type='0') ", cif_type);
	} else {
		strcpy(wherelist, " cif_type in ('0','1','2','3','4','5','9') ");
	}
	sprintf(acErrMsg, "%s", wherelist);
	WRITEMSG

	sprintf(acErrMsg, "%s", wherelist);
	WRITEMSG

	sprintf(acErrMsg, "%s", wherelist);
	WRITEMSG

	/* �����´�ȫ·���ļ���(����) */
	pub_base_AllDwnFilName(filename);

	fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	/***��Ʒ�б�***/
	ret = Dd_parm_Dec_Sel(g_pub_tx.reply, "%s order by prdt_no", wherelist);
	if (ret)
		goto ErrExit;

	i = 0;
	while (1) {
		ret = Dd_parm_Fet_Sel(&g_dd_parm, g_pub_tx.reply);
		if (ret == 100)
			break;
		if (ret)
			goto ErrExit;

		sel_flag++;

		if (i == 0) {
			set_zd_data("0240", g_dd_parm.prdt_no);
			set_zd_data("0270", g_dd_parm.title);
		}
		pub_base_strpack(g_dd_parm.title);
		fprintf(fp, "0240|%-30s|%s|\n", g_dd_parm.title, g_dd_parm.prdt_no);

		i++;
		vcnt++;
	}
	Dd_parm_Clo_Sel();

	if (sel_flag == 0) {
		sprintf(acErrMsg, "û���������������ļ�¼![%s]", g_pub_tx.reply);
		WRITEMSG
			strcpy(g_pub_tx.reply, "L123");
		goto ErrExit;
	}
	if (vcnt)
		set_zd_data(DC_FILE_SND, "2");
	fclose(fp);

	/* ������� */

OkExit:
	strcpy(g_pub_tx.reply, "0000");
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
