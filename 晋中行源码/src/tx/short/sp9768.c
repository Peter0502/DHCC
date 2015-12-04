/*****************************************************
* �� �� ��:  sp9768.c
* ���������� ���ݽ����кŲ�ѯ�������ձ�����
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

int sp9768()
{
	char recvpart[2];
	struct mbfenetbank_c	mbfenetbank_v;
	struct mbfeeisbank_c	mbfeeisbank_v;
	struct mbfesabk_c	mbfesabk_v;

	memset(&mbfenetbank_v,0x00,sizeof(struct mbfenetbank_c));
	memset(&mbfeeisbank_v,0x00,sizeof(struct mbfeeisbank_c));
	memset(&mbfesabk_v,0x00,sizeof(struct mbfesabk_c));

	pub_base_sysinit();
	if (get_bkno()) goto ErrExit;

	get_zd_data("0670",recvpart);
	get_zd_data("0580",mbfetran.rdficode);

	if (!strcmp(mbfetran.rdficode,mbfetran.cnapsodfi))
	{
		sprintf(acErrMsg,"�����������к�");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else 
	{
		if (!strcmp(recvpart,"1"))
		{
			g_reply_int=Mbfenetbank_Sel(g_pub_tx.reply,&mbfenetbank_v,
						"nbkcode='%s'",mbfetran.rdficode);
			if ( g_reply_int==100 )
			{
				sprintf(acErrMsg,"�޴˲������к�");
				MBFEWRITEMSG
				goto ErrExit;
			}
			else if ( g_reply_int )
			{
				sprintf(acErrMsg,"��ѯ���������Ƴ���[%d]",g_reply_int);
				MBFEWRITEMSG
				goto ErrExit;
			}

			g_reply_int=Mbfesabk_Sel(g_pub_tx.reply,&mbfesabk_v,
						"sabkcode='%s'",mbfenetbank_v.sabkcode);
			if ( g_reply_int )
			{
				sprintf(acErrMsg,"��ѯ�ձ����ĳ���[%d]",g_reply_int);
				MBFEWRITEMSG
				goto ErrExit;
			}

			set_zd_data("0961",mbfenetbank_v.nbkname);
			set_zd_data("0920",mbfesabk_v.blccpc);
		}
		else if (!strcmp(recvpart,"5"))
		{
			g_reply_int=Mbfeeisbank_Sel(g_pub_tx.reply,&mbfeeisbank_v,
						"bnkno='%s'",mbfetran.rdficode);
			if ( g_reply_int==100 )
			{
				sprintf(acErrMsg,"�޴˵������к�");
				MBFEWRITEMSG
				goto ErrExit;
			}
			else if ( g_reply_int )
			{
				sprintf(acErrMsg,"��ѯ���������Ƴ���[%d]",g_reply_int);
				MBFEWRITEMSG
				goto ErrExit;
			}

			set_zd_data("0961",mbfeeisbank_v.bnklname);
			set_zd_data("0920",mbfeeisbank_v.eissiteno);
		}
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
