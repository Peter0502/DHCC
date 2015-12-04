/************************************************************
* 文 件 名:   spJ073.c
* 功能描述：  小额发送交易
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

#define MYPACKERR if( ret ) {\
 sprintf(acErrMsg,"拼报文错误 [%d]",ret);\
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

spJ073()
{
	int ret=0;
	int iPkgno=0;

	char cPackday[9];
	char cPkgno[4];
	char cPackid[9];
	char cPackid_f[9];

	memset(cPackday,'\0',sizeof(cPackday));
	memset(cPkgno,'\0',sizeof(cPkgno));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(cPackid_f,'\0',sizeof(cPackid_f));

	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));

	vtcp_log("[%s][%d]spJ073小额业务发包交易BEGIN",__FILE__,__LINE__);

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

	sprintf( cPackid,"%08d",atoi(cPackid));

	vtcp_log("[%s][%d]spJ030[%s][%s][%s]BEGIN",__FILE__,__LINE__,cPackday,cPkgno,cPackid);

	if(iLvCheckStat()!=0){
		sprintf(acErrMsg,"iLvCheckStat检查出错!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P036");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,cPkgno);

	ret = Lv_wbctl_Sel(g_pub_tx.reply,&g_lv_wbctl,"pack_date = '%s' and packid = %s",cPackday,cPackid);
	MYSQLERR
	/** 状态是已组包才可以发送 **/
	if ( g_lv_wbctl.stat[0] != PSTAT_PACKED ){
		sprintf( acErrMsg,"包记录状态错误!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"P071");
		goto ErrExit;
	}
	zip_space(cPkgno);
  iPkgno = apatoi(cPkgno,3);
	vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,cPkgno);
	vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
	switch(iPkgno)
	{
		case 1:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ptdj_tag(cPackday,cPackid);	/** <ol><li>PKG001普通贷记业务包</li></ol> **/
				vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,acErrMsg);
				set_zd_data("0130",acErrMsg);
				MYPACKERR
				break;
		case 2:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ptjj_tag(cPackday,cPackid);	/** PKG002普通借记业务包 **/
				vtcp_log("[%s][%d]spJ030[%s][%s][%s]BEGIN",__FILE__,__LINE__,cPackday,cPkgno,cPackid);
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 3:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ssdj_tag(cPackday,cPackid);	/** PKG003实时贷记业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 4:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ssjj_tag(cPackday,cPackid);	/** PKG004实时借记业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 5:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_dqdj_tag_ssh(cPackday,cPackid);	/** PKG005定期贷记业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 6:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_dqjj_tag(cPackday,cPackid);	/** PKG006定期借记业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 7:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_djth_tag(cPackday,cPackid);	/** PKG007贷记退汇业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 8:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ptjjhz_tag(cPackday,cPackid);	/** PKG008普通借记回执包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 9:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ssdjhz_tag(cPackday,cPackid);	/** PKG009实时贷记回执包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 10:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ssjjhz_tag(cPackday,cPackid);	/** PKG010实时借记回执包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 11:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_dqjjhz_tag(cPackday,cPackid);	/** PKG011定期借记业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 12:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_tyxx_tag(cPackday,cPackid);	/** PKG012通用信息业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		case 13:
				vtcp_log("[%s][%d]pkgno=[%d]",__FILE__,__LINE__,iPkgno);
				ret = lv_get_ssxx_tag(cPackday,cPackid);	/** PKG013实时信息业务包 **/
				vtcp_log("[%s][%d]",__FILE__,__LINE__);
				MYPACKERR
				break;
		default:
			sprintf(acErrMsg,"pkg包类型错误!");
			vtcp_log("[%s][%d]",__FILE__,__LINE__);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P041");
			goto ErrExit;

	}
	vtcp_log("[%s][%d]spJ030[%s][%s][%s]BEGIN",__FILE__,__LINE__,cPackday,cPkgno,cPackid);
	/*beg add by Chenggx 2011-6-16  晋中*/
	/* 登记交易流水 */
	if ( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
			goto ErrExit;
	}
	/*beg add by Chenggx 2011-6-16  晋中*/
	ret = Lv_wbctl_Sel(g_pub_tx.reply,&g_lv_wbctl,"pack_date = '%s' and packid = %s",cPackday,cPackid);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MYSQLERR
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
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

