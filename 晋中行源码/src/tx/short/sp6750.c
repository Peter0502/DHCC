/***************************************************************
* �� �� ��: sp6750.c
* �������������ݻ����˺ŷ��ش����������е�����
* ��    ��: wzw
* ������ڣ�2013��1��4��

insert into TX_DEF       values ('6750', '���ش����������е�����', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000', '1', '6', '2');
insert into TX_FLOW_DEF  values ('6750', 0, '6750', '#$');
insert into TX_SUB_DEF   values ('6750', '���ش����������е�����', 'sp6750', '0', '0');

**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ln_auth_c.h"

sp6750()
{   
	int ret; 
	char cAc_no[20];

	struct ln_auth_c	g_ln_auth;
	memset(&g_ln_auth,0x00,sizeof(struct ln_auth_c));
	memset(cAc_no,0x00,sizeof(cAc_no));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0300", cAc_no);          	
	get_zd_long( "0050", &g_pub_tx.tx_date);

	ret = Ln_auth_Sel(g_pub_tx.reply,&g_ln_auth, "pay_ac_no='%s' and type='12' and sts='X' ",  cAc_no);
	if( ret==100 )
  {
		sprintf(acErrMsg,"�����������в����ڸü�¼");
		WRITEMSG
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"���Ҽ�¼����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	set_zd_double( "0840", g_ln_auth.rate);
	vtcp_log("[%s][%d]  g_ln_auth.rate = %.2f",__FILE__,__LINE__,g_ln_auth.rate);
				

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�������ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
