/***************************************************************
* �� �� ��: spD006.c
* ��������:	�ж�ת�˷��˺�����
* ��    ��: jack
* ������ڣ�2004��6��1��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spD006()
{
	int ret;
	struct prdt_ac_id_c g_prdt_ac_id;

	get_zd_data("1011",g_pub_tx.ac_no);
	get_zd_data("1171",g_pub_tx.ct_ind);

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", \
						  g_pub_tx.ac_no);
	if( ret )	
	{
		sprintf(acErrMsg,"�����˺Ŷ��ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W012");
		goto ErrExit;
	}

	/*
	if( g_mdm_ac_rel.ac_seqn==9999 )
	{
		ret=sql_count("prdt_ac_id","ac_id=%ld",g_mdm_ac_rel.ac_id);
		if( ret<0 )
		{
			sprintf(acErrMsg,"sql_count error!");
			WRITEMSG
			goto ErrExit;
		}
		else if( ret==1 )
		{ ; }
	}
	*/

	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, "ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
	if( ret )	
	{
		sprintf(acErrMsg,"��Ʒ�˻����ձ��в����ڸü�¼");
		WRITEMSG
		strcpy(g_pub_tx.reply,"W012");
		goto ErrExit;
	}

	if( g_prdt_ac_id.prdt_no[0]=='2' )
	{
		sprintf(acErrMsg,"�����ԴӶ��ڻ�����ת��!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O180");
		goto ErrExit;
	}

ErrExit:
	strcpy(g_pub_tx.reply,"P156");
	sprintf(acErrMsg,"�ж��˻�����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ж��˻����ͳɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}
