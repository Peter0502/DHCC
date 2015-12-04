/************************************************************
* �� �� ��:   spJ072.c
* ����������  С��������
*
* ��    ��:   chenhw
* ������ڣ�  2006-9-5
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"P026" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
}

#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_define.h"

static struct lv_wbctl_c g_lv_wbctl;

spJ072()
{
	int ret;

	char cPackday[9];
	char cPkgno[4];
	char cPackid_f[9];
	char cPackid[9];
	char cOpcd[2];
	char cCashqsactno[13];
	char cDtlcnt[9];
	char cTotamt[16];

	memset(cPackday,'\0',sizeof(cPackday));
	memset(cPkgno,'\0',sizeof(cPkgno));
	memset(cPackid_f,'\0',sizeof(cPackid_f));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(cOpcd,'\0',sizeof(cOpcd));
	memset(cCashqsactno,'\0',sizeof(cCashqsactno));
	memset(cDtlcnt,'\0',sizeof(cDtlcnt));
	memset(cTotamt,'\0',sizeof(cTotamt));

	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));

	vtcp_log("[%s][%d]spJ072С��ҵ��������BEGIN",__FILE__,__LINE__);

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}


	/** ȡ��FD���� **/
	get_fd_data( "0440", &cPackday);         /** ȡ��ƴ������ **/
	get_fd_data( "0230", &cPkgno);           /** ȡ�ð����� **/
	get_fd_data( "0530", &cPackid_f);          /** ȡ��ƴ����� **/
	get_fd_data( "0670", &cOpcd);            /** ȡ�ô������� **/

	memcpy(cPackid,cPackid_f,sizeof(cPackid)-1);

	vtcp_log("[%s][%d]spJ030[%s][%s][%s][%s]BEGIN",__FILE__,__LINE__,cPackday,cPkgno,cPackid,cOpcd);

	ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
		"pkgno='%s' and pack_date='%s' and packid=%s",cPkgno,cPackday,cPackid);
	MYSQLERR
             
	ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
	MYSQLERR

	if(g_lv_wbctl.stat[0]!=PSTAT_PACKING){
		sprintf(acErrMsg,"lv_wbctl״̬[%c]��!",g_lv_wbctl.stat[0]);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P036");
		goto ErrExit;
	}

	if(cOpcd[0]=='1'){
		g_lv_wbctl.stat[0]=PSTAT_PACKED;
		ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
		MYSQLERR
	}

	Lv_wbctl_Clo_Upd();

	/* ����ˮ��־ */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		goto ErrExit;
	}

	cOpcd[0]=='1';
	set_zd_data("0670",cOpcd);
	set_zd_data("0520",g_lv_wbctl.cash_qs_no);
	set_zd_long("0540",g_lv_wbctl.dtlcnt);
	set_zd_double("0430",g_lv_wbctl.totamt);

	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
