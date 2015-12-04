/*************************************************
* �� �� ��:  spJ910.c
* ���������� 910ͨ�û�Ӧ���Ĵ����ӽ���
*			 ����
*			 ��301,302,303,313,314,721,724���ͱ���
*            ��910��Ӧ�ֱ��޸Ķ�Ӧ�ĵǼǲ��Ĵ����
*            ���ֶ�,���������ͱ��ĵ�910��Ӧ������
*            ��
*			 
* ��    ��:  jane
* ������ڣ� 2004��7��19��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

int spJ910()
{
	struct mbfesendcmtmsg_c sendcmtmsg_v;

	g_reply_int = Mbfesendcmtmsg_Sel( g_pub_tx.reply, &sendcmtmsg_v, "respno='%s'", mbfetran.respno );
	if (g_reply_int)
	{
		sprintf( acErrMsg,"��ѯ���Ĳο���Ϊ[%s]��������Ϣ����,[%ld]", mbfetran.respno, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	switch( atoi(sendcmtmsg_v.cmtno ) )
	{
		case 301:
			g_reply_int=sql_execute("update mbfequerymsg set errcode='%s', errdesc='%s' where querymsgno='%s' and querydate='%ld' and querysource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸Ĳ�ѯ��Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 302:
			g_reply_int=sql_execute("update mbfereplymsg set errcode='%s', errdesc='%s' where rplymsgno='%s' and rplydate='%ld' and rplysource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸Ĳ鸴��Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 303:
			g_reply_int=sql_execute("update mbfefreeinfo set errcode='%s', errdesc='%s' where msssno='%s' and consigndate='%ld' and infosource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸����ɸ�ʽ���ĵǼǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 313:
			g_reply_int=sql_execute("update mbfereturnapp set errcode='%s', errdesc='%s' where rtnappno='%s' and consigndate='%ld' and rtnappsource='00'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸��˻�����Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 314:
			g_reply_int=sql_execute("update mbfereturnrply set errcode='%s', errdesc='%s' where origrtnappno='%s' and origrtnappdate='%ld' and rdficode='%s'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.origrtnappdate,sendcmtmsg_v.bkcode);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸��˻�Ӧ��Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 651:
			g_reply_int=sql_execute("insert into mbfeodoprtqryrpl values ('%ld','%s','','','','','','','','','','','','','','','%s','%s')",
				sendcmtmsg_v.cnapsdate,sendcmtmsg_v.respno,mbfetran.errcode,mbfetran.strinfo);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�Ǽ�����״̬��ѯ�ظ��Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 721:
			g_reply_int=sql_execute("update mbfebillaplset set errcode='%s', errdesc='%s' where msssno='%s' and consigndate='%ld'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸����л�Ʊ�Ҹ�����Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;
		case 724:
			g_reply_int=sql_execute("update mbfebillrtureg set errcode='%s', errdesc='%s' where msssno='%s' and consigndate='%ld'",
				mbfetran.errcode,mbfetran.strinfo,sendcmtmsg_v.msssno,sendcmtmsg_v.cnapsdate);
			if (g_reply_int)
			{
				sprintf(acErrMsg,"910��ִ�޸����л�Ʊ�˻�����Ǽǲ�����[%ld]",g_reply_int);
				WRITEMSG
				goto ErrExit;
			}
			break;

		default:break;
	}

OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
