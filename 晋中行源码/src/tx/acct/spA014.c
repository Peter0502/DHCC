/***************************************************************
* �� �� ��: spA014.c
* �����������ڲ����ֽ�������
* ��    ��: jack
* ������ڣ�2004��1��19��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spA014()
{
	char cJzlx[2];

	memset(cJzlx,0x0,sizeof(cJzlx));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
	g_reply_int=0;

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	strcpy(g_pub_tx.reply,"0000");

	get_zd_data("0670",cJzlx);	/*��������*/
	if (cJzlx[0] == '1')
	{
		strcpy( g_pub_tx.reply,"0001" );
	}
	else if (cJzlx[0] == '2')
	{
		strcpy( g_pub_tx.reply,"0002" );
	}
	else
	{
		sprintf(acErrMsg,"�������ͳ���!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg,"�ڲ��ʽ�����ʳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ڲ��ʽ������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
