/*************************************************************
* �� �� ��: spF781.c
* �����������ж��Ƿ������˺�ҵ��(��ɳ���к�Ϊ:4302),���򱨴�
* ��    ��: jack
* ������ڣ�2003��5��24��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF781()
{
	char cCity[5];
	int ret = 0;

	double tram = 0.00;		/*���׽��*/
	double sxtram = 5.00;	/*�����ѽ��*/

	struct four_tradelog_c sFour_Tradelog;

	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));
	memset(cCity,0x0,sizeof(cCity));

	get_zd_data("0360",cCity);
	if( strncmp(cCity, "4302", 4) != 0)	/*-- Ŀ����Ϊ��ɳ --*/
	{
		strcpy( g_pub_tx.reply, "F001" );
		sprintf( acErrMsg, "�ý���Ŀ���зǱ���!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	set_zd_data("0310",TESTCARD );	/*�ڲ��ʺ�*/
	set_zd_int("0350",1);				/*�����˺����*/
	get_zd_double("0400",&tram);


OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ж�Ŀ���н��׳ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy(g_pub_tx.reply,"F006");
	sprintf(acErrMsg,"�ж�Ŀ���н��׳���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
