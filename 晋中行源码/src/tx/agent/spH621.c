/***************************************************************
* �� �� ��: spH621.c
* ����������˰��Ƿ����Ϣ��ѯ
*
* ��    ��: jack
* ������ڣ�2004��6��24��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "pay_agent_c.h"

spH621()
{
	int ret = 0;
	int num = 0;
	char cItem_no[21];
	char cUnit_no[5];

	struct pay_agent_c sPay_agent; /*�ɷ���Ϣ��ϸ��*/

	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	ret = Pay_agent_Sel( g_pub_tx.reply, &sPay_agent, \
			"item_no = '%s' and unit_no = '%s' and result= '1'", \
			cItem_no, cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Pay_agent_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0260", sPay_agent.remark );	/* ��˰������ */
	set_zd_double( "0400", sPay_agent.tram );	/* Ӧ�ɽ�� */
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��˰Ƿ����Ϣ��ѯ�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��˰Ƿ����Ϣ��ѯʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
