/************************************************************
* 文 件 名:   spJ074.c
* 功能描述：  小额拆包交易
*
* 作    者:   chenhw
* 完成日期：  2006-9-6
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
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
#include "lv_lbctl_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"

static struct lv_wbctl_c g_lv_wbctl;
static struct lv_lbctl_c g_lv_lbctl;
static struct lv_pkgreg_c g_lv_pkgreg;
static PAY_IN_AREA payin;
static NOTPAY_IN_AREA notpayin,notpay_out;

spJ074()
{
	int ret;
	int iPackid;

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

	memset(&payin,'\0',sizeof(payin));
	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_lbctl ,0x00 ,sizeof(struct lv_lbctl_c));
	memset(&g_lv_pkgreg ,0x00 ,sizeof(struct lv_pkgreg_c));

	vtcp_log("[%s][%d]spJ072小额业务封包交易BEGIN",__FILE__,__LINE__);

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}


	/** 取得FD数据 **/
	get_fd_data( "0440", &cPackday);         /** 取得拼包日期 **/
	get_fd_data( "0230", &cPkgno);           /** 取得包类型 **/
	get_fd_data( "0290", &cPackid);        /** 取得拼包序号 **/
	get_fd_data( "0670", &cOpcd);            /** 取得操作类型 **/

	vtcp_log("[%s][%d]spJ030[%s][%s][%s][%s]BEGIN",__FILE__,__LINE__,cPackday,cPkgno,cPackid,cOpcd);

	Lv_wbctl_Dec_Upd(g_pub_tx.reply,
                "pkgno='%s' and pack_date='%s' and packid='%08d'",cPkgno,cPackday,atoi(cPackid));

	ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
	MYSQLERR

	if(g_lv_wbctl.stat[0]!=PSTAT_PACKING && g_lv_wbctl.stat[0]!=PSTAT_PACKED){
		sprintf(acErrMsg,"lv_wbctl状态[%c]错!",g_lv_wbctl.stat[0]);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P036");
		goto ErrExit;
	}

	if(cOpcd[0]=='1'){
		g_lv_wbctl.stat[0]=PSTAT_INVALID;
		ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
		MYSQLERR

		ret=Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"pkgno='%s' and pack_date=%s and packid = '%08d' ",cPkgno,cPackday,atoi(cPackid));
		MYSQLERR

		while(1)
		{
			ret = Lv_pkgreg_Fet_Upd(&g_lv_pkgreg,g_pub_tx.reply);
			if(ret!=0  && ret!=100){
				sprintf(acErrMsg,"查找记录出错[%d]",ret);
              	WRITEMSG
              	strcpy(g_pub_tx.reply,"D103");
              	goto ErrExit;
           	}
           	if( ret==100 ){
               	vtcp_log("[%s][%d]spJ030循环记录处理完毕",__FILE__,__LINE__);
               	break;
           	}

        	vtcp_log("[%s][%d]spJ030[%ld]BEGIN",__FILE__,__LINE__,g_lv_pkgreg.packid);
			memset(g_lv_pkgreg.packid,'\0',sizeof(g_lv_pkgreg.packid)-1);
        	vtcp_log("[%s][%d]spJ030[%ld]BEGIN",__FILE__,__LINE__,g_lv_pkgreg.packid);
			/** 录入后状态为 packid为空格 packdate为0 **/
			g_lv_pkgreg.pack_date=0;

			ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
           	MYSQLERR

		}
        Lv_pkgreg_Clo_Upd();
	}

	Lv_wbctl_Clo_Upd();

	if(memcmp(cPkgno,PKGNO_PTJJHZ,sizeof(cPkgno)-1)==0 || memcmp(cPkgno,PKGNO_DQJJHZ,sizeof(cPkgno)-1)==0)
	{
        vtcp_log("[%s][%d]--------hzhzhzh-------------",__FILE__,__LINE__);
		/** 回执更新来包记录 **/
		ret=Lv_lbctl_Dec_Upd(g_pub_tx.reply,
			"o_pack_date = '%s' and o_packid = %s",cPackday,cPackid);
		MYSQLERR
		ret=Lv_lbctl_Fet_Upd(&g_lv_lbctl,g_pub_tx.reply);
		MYSQLERR

		g_lv_lbctl.o_pack_date = 0;
		memset(g_lv_lbctl.o_packid,'\0', sizeof(g_lv_lbctl.o_packid));

		ret=Lv_lbctl_Upd_Upd(g_lv_lbctl,g_pub_tx.reply);
		MYSQLERR
		Lv_lbctl_Clo_Upd();
	}

	/* 记流水日志 */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "记流水日志错" );
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
