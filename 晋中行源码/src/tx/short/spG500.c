/*************************************************
* �� �� ��:  spG500.c
* ���������� ���гжһ�Ʊ��ѯ�鸴�Ǽǡ���ѯ
*
* ��    ��:  gujingyu
* ������ڣ� 2009��4��2��
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
insert into tx_def values ('4375', '�жһ�Ʊ��ѯ�鸴�Ǽ�',  '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
insert into tx_flow_def values('4375','0','g500','#$');
insert into tx_sub_def values ('g500', '�жһ�Ʊ��ѯ�鸴�Ǽ�', 'spG500', '0', '0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "seltelreg_c.h"


spG500()
{
	char            flag[3], tmp_str[101];
	int             ret;
	ret = 0;
	FILE           *fp = NULL;
	char            filename[300];
	char            cFilename[300];	/* �ļ��� */
	char		brf[1001];
	char		cPact_no[41];
	struct seltelreg_c g_seltelreg_c;
	memset(&g_seltelreg_c, 0x00, sizeof(g_seltelreg_c));
	memset(brf, 0x0, sizeof(brf));
	memset(cPact_no,0x00,sizeof(cPact_no));
	

	g_reply_int = 0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	vtcp_log("LINE %d FILE %s ", __LINE__, __FILE__);
	get_zd_data("0750", cPact_no);	/* Ʊ�ݺ� */
	memcpy(g_seltelreg_c.qvocnum,cPact_no,sizeof(g_seltelreg_c.qvocnum)-1);
	get_zd_data("0670",g_seltelreg_c.qmode );

	get_zd_data("0680",g_seltelreg_c.rmode );
	get_zd_data("0450",g_seltelreg_c.qdate);
	vtcp_log("%s",g_seltelreg_c.qdate);
	get_zd_data("0810",g_seltelreg_c.qbrname);
	get_zd_data("0460",g_seltelreg_c.rdate);
	vtcp_log("%s",g_seltelreg_c.rdate);
	get_zd_data("1250",brf);
	vtcp_log("brf=[%s]",brf);
	memcpy(g_seltelreg_c.rtxt, brf, sizeof(g_seltelreg_c.rtxt) - 1);
	memset(brf, 0x0, sizeof(brf));
	get_zd_data("1230",brf);
	memcpy(g_seltelreg_c.cxnr, brf, sizeof(g_seltelreg_c.cxnr) - 1);
	get_zd_double("0400",&g_seltelreg_c.telamt);
	g_seltelreg_c.tx_date = g_pub_tx.tx_date;


	/*************���벿��**************/
	
	Seltelreg_Debug(&g_seltelreg_c);

	ret = Seltelreg_Ins(g_seltelreg_c, g_pub_tx.reply);
	if (ret != 0) {
		sprintf(acErrMsg, "����Seltelregʧ��!ret=[%d]", ret);
		set_zd_data(DC_GET_MSG, acErrMsg);
		strcpy(g_pub_tx.reply, "8888");
		goto ErrExit;
	} else{
		sprintf(acErrMsg, "����Seltelreg�ɹ�!ret=[%d]!");
		set_zd_data(DC_GET_MSG, acErrMsg);
		strcpy(g_pub_tx.reply, "0000");
		goto OkExit;
	}

OkExit:
	sprintf(acErrMsg, "�Ǽǳɹ���[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg, "�Ǽ�ʧ�ܣ�[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}

