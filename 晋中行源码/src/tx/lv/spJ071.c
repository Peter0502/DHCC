/************************************************************
* 文 件 名:   spJ071.c
* 功能描述：  小额回执业务拼包交易
*
* 作    者:   chenhw
* 完成日期：  2006-9-4
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


spJ071()
{
	int ret;
	int iCnt;
	int iPackday;
	int iPackid;
	int recid=0;
	int iJzCnt=0;
	int g_reply_int=0;

	char cBrno[6];
	char cTbsdy[9];
	char cTlrno[5];
	char cOptype[2];
	char cOpayqsactno[13];
	char cOpkgno[4];
	char cOpackday[9];
	char cOpackid[9];
	char cPackid[9];
	char cOrderno[9];
	char sql_pkgno[4];
	char sql_stat[2];
    char cTemp[13]; /****YHBP****/

	struct lv_wbctl_c 	g_lv_wbctl;
	struct lv_lbctl_c 	g_lv_lbctl;
	struct lv_pkgreg_c 	g_lv_pkgreg;

	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_lbctl ,0x00 ,sizeof(struct lv_lbctl_c));
	memset(&g_lv_pkgreg ,0x00 ,sizeof(struct lv_pkgreg_c));

	memset(cBrno,'\0',sizeof(cBrno));
	memset(cTbsdy,'\0',sizeof(cTbsdy));
	memset(cTlrno,'\0',sizeof(cTlrno));
	memset(cOptype,'\0',sizeof(cOptype));
	memset(cOpayqsactno,'\0',sizeof(cOpayqsactno));
	memset(cOpkgno,'\0',sizeof(cOpkgno));
	memset(cOpackday,'\0',sizeof(cOpackday));
	memset(cOpackid,'\0',sizeof(cOpackid));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(cOrderno,'\0',sizeof(cOrderno));
	memset(sql_pkgno,'\0',sizeof(sql_pkgno));
	memset(sql_stat,'\0',sizeof(sql_stat));
    memset(cTemp   , 0 , sizeof(cTemp));

	vtcp_log("[%s][%d]spJ071小额回执业务拼包交易BEGIN",__FILE__,__LINE__);

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}


	/** 取得FD数据 **/
	get_fd_data( "0030", &cBrno);           /** 取得交易机构 **/
	get_fd_data( "0050", &cTbsdy);          /** 取得交易日期 **/
	get_fd_data( "0070", &cTlrno);          /** 取得柜员号 **/
	get_fd_data( "0670", &cOptype);		/****0-拼包,1-记帐****/
	get_fd_data( "0520", &cTemp);	/**原发起清算行号**/
	get_fd_data( "0230", &cOpkgno);		/**报文编号**/
	get_fd_data( "0440", &cOpackday);	/**原包日期**/
	get_fd_data( "0530", &cOpackid);	/**包序号**/
    vtcp_log("[%s][%d]cTemp=[%s]\n",__FILE__,__LINE__,cTemp);
    memcpy(cOpayqsactno,cTemp,sizeof(cOpayqsactno)-1);

	vtcp_log("[%s][%d]spJ030[%s][%s][%s][%s][%s][%s]BEGIN",__FILE__,__LINE__,cBrno,cOptype,cOpayqsactno,cOpkgno,cOpackday,cOpackid);

	iPackday=apatoi(pcLvGetDay(),8);    /** 获得拼包日期及拼包编号 **/
	iPackid=iLvGetPackid();
	sprintf( cPackid,"%08d",iPackid);

	memcpy(g_lv_lbctl.pay_qs_no,cOpayqsactno,sizeof(g_lv_lbctl.pay_qs_no)-1);
	memcpy(g_lv_lbctl.packid,cOpackid,sizeof(g_lv_lbctl.packid)-1);
	g_lv_lbctl.pack_date=apatoi(cOpackday,8);
	memcpy(g_lv_lbctl.pkgno,cOpkgno,sizeof(g_lv_lbctl.pkgno)-1);
	vtcp_log("%s,%d packid=[%s],qs_no=[%s],packdate=[%s]",__FILE__,__LINE__,cOpackid,cOpayqsactno,cOpackday);
	/* begin add by LiuHuafeng 20070505 */
	sprintf(cOpackid,"%08ld",atol(cOpackid));
	/*****不知道为什么要这么做YHBP***
	sprintf(cOpayqsactno,"%012ld",atol(cOpayqsactno));
	***************/
	/* end by LiuHuafeng 20070505 */
	ret = Lv_lbctl_Sel( g_pub_tx.reply,&g_lv_lbctl,"pack_date = '%s' and pay_qs_no = '%s' and packid = %s",cOpackday,cOpayqsactno,cOpackid);
	MYSQLERR

	vtcp_log("[%s][%d]spJ071[%s][%ld]\n",__FILE__,__LINE__,g_lv_lbctl.o_packid,g_lv_lbctl.o_pack_date);
	if(g_lv_lbctl.o_packid[0]!='\0'){
		sprintf(acErrMsg,"回执[%s]已经拼过包了",g_lv_lbctl.o_packid);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P032");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]spJ071----------BEGIN",__FILE__,__LINE__);

	iCnt=0;

	if(memcmp(cOpkgno,PKGNO_PTJJ,sizeof(cOpkgno)-1)==0){
		memcpy(sql_pkgno,PKGNO_PTJJHZ,sizeof(sql_pkgno)-1);
	}else{
		memcpy(sql_pkgno,PKGNO_DQJJHZ,sizeof(sql_pkgno)-1);
	}

	iCnt=sql_count("lv_pkgreg","pkgno=%s and cash_qs_no=%s and o_pack_date=%08d and o_packid=%.8s and lv_sts<>'0'",sql_pkgno,g_lv_lbctl.pay_qs_no,g_lv_lbctl.pack_date,g_lv_lbctl.packid);
	if(iCnt==-1){
		strcpy( g_pub_tx.reply,"P026" );
		sprintf(acErrMsg,"SQLERR");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]spJ071----[%d][%d]------",__FILE__,__LINE__,iCnt,g_lv_lbctl.dtlcnt);

	if(iCnt!=g_lv_lbctl.dtlcnt){
		sprintf(acErrMsg,"回执记录数[%d]与来包记录数[%ld]不符!",iCnt,g_lv_lbctl.dtlcnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P033");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]spJ071----------",__FILE__,__LINE__);

	iCnt=0;
	vtcp_log("[%s][%d]spJ071----------",__FILE__,__LINE__);
	iCnt=sql_count("lv_pkgreg","pkgno=%s and cash_qs_no=%s and o_pack_date=%08d and o_packid=%.8s and lv_sts<>'3'",sql_pkgno,g_lv_lbctl.pay_qs_no,g_lv_lbctl.pack_date,g_lv_lbctl.packid);
	if(iCnt==-1){
		strcpy( g_pub_tx.reply,"P026" );
		sprintf(acErrMsg,"SQLERR");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]spJ071----------BEGIN",__FILE__,__LINE__);

	if(iCnt!=0){
		sprintf(acErrMsg,"存在[%d]条没有复核的回执数据!",iCnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P034");
		goto ErrExit;
	}
	
	sql_stat[0]=STAT_WZCHECK2;

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
		"pkgno='%.3s' and cash_qs_no='%.12s' and o_packid =%s and o_pack_date='%08d' and lv_sts ='%s' ",
		sql_pkgno,g_lv_lbctl.pay_qs_no,g_lv_lbctl.packid,g_lv_lbctl.pack_date,sql_stat);
	MYSQLERR

	while(1)
	{

		ret = Lv_pkgreg_Fet_Upd(&g_lv_pkgreg,g_pub_tx.reply);
		recid++;
		if(ret!=0  && ret!=100){
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
                }
		if( ret==100 ){
			if( recid==1 ){
				sprintf(acErrMsg,"没有可拼包的交易[%d]",ret);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P027");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]spJ030循环记录处理完毕",__FILE__,__LINE__);
			break;
		}

		if(g_lv_pkgreg.packid[0]!='\0')
		{
			sprintf(acErrMsg,"回执[%.8s]已经拼过包了");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P032");
			goto ErrExit;
		}

		g_lv_pkgreg.pack_date=iPackday;
		memcpy(g_lv_pkgreg.packid,cPackid,sizeof(g_lv_pkgreg.packid)-1);
		
		ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
		MYSQLERR
		
		/** 统计成功比数与金额 **/
		if(memcmp(g_lv_pkgreg.rcpstat,"00",2)==0){
			g_lv_wbctl.dtlsucamt+=g_lv_pkgreg.tx_amt;
			g_lv_wbctl.dtlsuccnt++;
		}

	}
	Lv_pkgreg_Clo_Sel();

	/****添加往包记录****/

	g_lv_wbctl.pack_date = iPackday;
	memcpy(g_lv_wbctl.packid,cPackid,sizeof(g_lv_wbctl.packid)-1);
	vtcp_log("[%s][%d]spJ030ipackid=[%s]",__FILE__,__LINE__,g_lv_wbctl.packid);

	memcpy(g_lv_wbctl.pkgno, sql_pkgno, sizeof(g_lv_wbctl.pkgno)-1);
	memcpy(g_lv_wbctl.pay_qs_no, g_lv_pkgreg.pay_qs_no, sizeof(g_lv_wbctl.pay_qs_no)-1);
	memcpy(g_lv_wbctl.cash_qs_no, g_lv_pkgreg.cash_qs_no, sizeof(g_lv_wbctl.cash_qs_no)-1);
	memcpy(g_lv_wbctl.txnum, g_lv_pkgreg.txnum, sizeof(g_lv_wbctl.txnum)-1);
	g_lv_wbctl.in_date= apatoi(cTbsdy, 8);
	memcpy(g_lv_wbctl.br_no, cBrno, sizeof(g_lv_wbctl.br_no)-1);
	g_lv_wbctl.hz_date= apatoi(cTbsdy, 8);
	g_lv_wbctl.o_pack_date= g_lv_lbctl.pack_date;
	memcpy(g_lv_wbctl.o_packid, g_lv_lbctl.packid, sizeof(g_lv_wbctl.o_packid)-1);
	memcpy(g_lv_wbctl.o_pkgno, g_lv_lbctl.pkgno, sizeof(g_lv_wbctl.o_pkgno)-1);
	memcpy(g_lv_wbctl.ornode, g_lv_lbctl.ornode, sizeof(g_lv_wbctl.ornode)-1);
	memcpy(g_lv_wbctl.acnode, g_lv_lbctl.acnode, sizeof(g_lv_wbctl.acnode)-1);
	g_lv_wbctl.dtlcnt=g_lv_lbctl.dtlcnt;
	g_lv_wbctl.totamt=g_lv_lbctl.totamt;
	g_lv_wbctl.bfflag[0]='0';
	g_lv_wbctl.ffflag[0]='0';      /** 辖内分发标志,暂时赋成0 **/
	memcpy(g_lv_wbctl.brprtflag,"000",sizeof(g_lv_wbctl.brprtflag)-1);
	memcpy(g_lv_wbctl.qsprtflag,"000",sizeof(g_lv_wbctl.qsprtflag)-1);
	g_lv_wbctl.stat[0]=PSTAT_PACKED;

	vtcp_log("[%s][%d]spJ030[%s][%d]",__FILE__,__LINE__,g_lv_wbctl.packid,g_lv_wbctl.pack_date);

	ret = Lv_wbctl_Ins(g_lv_wbctl,g_pub_tx.reply);
	MYSQLERR
	vtcp_log("[%s][%d]spJ030ipackid=[%ld]",__FILE__,__LINE__,iPackid);



	/** 更新来包记录 **/

	ret=Lv_lbctl_Dec_Upd(g_pub_tx.reply,
		"pack_date = '%s' and pay_qs_no = '%s' and packid = %s",cOpackday,cOpayqsactno,cOpackid);
	MYSQLERR
	ret=Lv_lbctl_Fet_Upd(&g_lv_lbctl,g_pub_tx.reply);
	MYSQLERR

	g_lv_lbctl.o_pack_date = iPackday;
	memcpy(g_lv_lbctl.o_packid, cPackid, sizeof(g_lv_lbctl.o_packid)-1);
	vtcp_log("[%s][%d]-----[%s][%d]---",__FILE__,__LINE__,g_lv_lbctl.o_packid,g_lv_lbctl.o_pack_date);

	ret=Lv_lbctl_Upd_Upd(g_lv_lbctl,g_pub_tx.reply);
	MYSQLERR
	Lv_lbctl_Clo_Upd();	

	/* 记流水日志 */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]g_lv_wbctl.dtlsuccnt==[%d]",__FILE__,__LINE__,g_lv_wbctl.dtlsuccnt);
        set_zd_data("0540",g_lv_wbctl.packid);
        set_zd_long("0550",g_lv_wbctl.dtlsuccnt);
        set_zd_double("0330",g_lv_wbctl.dtlsucamt);


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
