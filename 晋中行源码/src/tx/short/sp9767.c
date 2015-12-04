/*****************************************************
* �� �� ��:  sp9767.c
* ���������� �����˻�����š��������ڡ������к�
			 ��ѯԭ�˻�������Ϣ
* ��    ��:  jane
* ������ڣ� 2003��6��3�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"

int sp9767()
{
	struct mbfereturnapp_c	mbfereturnapp_v;

	memset(&mbfereturnapp_v,0x00,sizeof(struct mbfereturnapp_c));

	pub_base_sysinit();
	if (get_bkno()) goto ErrExit;

	get_zd_data("0240",mbfetran.origrtnappno);
	get_zd_long("0440",&g_pub_tx.erro_date);
	get_zd_data("0580",mbfetran.rdficode);

	if (!strcmp(mbfetran.rdficode,mbfetran.cnapsodfi))
	{
		sprintf(acErrMsg,"�����������к�");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else 
	{
		g_reply_int=Mbfereturnapp_Sel(g_pub_tx.reply,&mbfereturnapp_v,
					"rtnappno='%s' and consigndate='%ld' and odficode='%s'",
					mbfetran.origrtnappno,g_pub_tx.erro_date,
					mbfetran.rdficode);
		if ( g_reply_int==100 )
		{
			sprintf(acErrMsg,"�޷��ϸ��������˻����룬��˶Ժ����ύ��");
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if ( g_reply_int )
		{
			sprintf(acErrMsg,"��ѯ����˻�����Ǽǲ�����[%d]",g_reply_int);
			MBFEWRITEMSG
			goto ErrExit;
		}

		if (!strcmp(mbfereturnapp_v.procstate,"00"))
		{
			sprintf(acErrMsg,"���˻��������Ѿ��鸴��");
			MBFEWRITEMSG
			goto ErrExit;
		}

		set_zd_data("0870",mbfereturnapp_v.origtxssno);
		set_zd_long("0450",mbfereturnapp_v.origconsigndate);
		set_zd_data("0590",mbfereturnapp_v.odficode);
		set_zd_double("0400",mbfereturnapp_v.origamount);
		set_zd_data("0961",mbfereturnapp_v.strinfo);
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
