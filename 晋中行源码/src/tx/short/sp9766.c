/*****************************************************
* �� �� ��:  sp9766.c
* ���������� ����֧��������š��������ڡ������к�
			 ��ѯԭҵ����Ϣ
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

int sp9766()
{
	struct mbfeghvsendlist_c	mbfeghvsendlist_v;
	struct mbfenetbank_c	mbfenetbank_v;

	memset(&mbfeghvsendlist_v,0x00,sizeof(struct mbfeghvsendlist_c));
	memset(&mbfenetbank_v,0x00,sizeof(struct mbfenetbank_c));

	pub_base_sysinit();
	if (get_bkno()) goto ErrExit;

	get_zd_data("0860",mbfetran.origtxssno);
	get_zd_long("0440",&g_pub_tx.erro_date);

	g_reply_int=Mbfeghvsendlist_Sel(g_pub_tx.reply,&mbfeghvsendlist_v,
				"txssno='%s' and consigndate='%ld' and odficode='%s'",
				mbfetran.origtxssno,g_pub_tx.erro_date,
				mbfetran.cnapsodfi);
	if ( g_reply_int==100 )
	{
		sprintf(acErrMsg,"��ѯ�����ʽ��ײ����ڣ���˶Ժ����ύ��");
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if ( g_reply_int )
	{
		sprintf(acErrMsg,"��ѯ���������ϸ�����");
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int=Mbfenetbank_Sel(g_pub_tx.reply,&mbfenetbank_v,
				"nbkcode='%s'",mbfeghvsendlist_v.rdficode);
	if ( g_reply_int )
	{
		sprintf(acErrMsg,"��ѯ���������Ƴ���");
		MBFEWRITEMSG
		goto ErrExit;
	}

	set_zd_data("0580",mbfeghvsendlist_v.rdficode);
	set_zd_data("0963",mbfenetbank_v.nbkname);
	set_zd_data("0250",mbfeghvsendlist_v.payeracc);
	set_zd_data("0961",mbfeghvsendlist_v.payername);
	set_zd_data("0260",mbfeghvsendlist_v.recipientacc);
	set_zd_data("0962",mbfeghvsendlist_v.recipientname);
	set_zd_double("0400",mbfeghvsendlist_v.amount);

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
