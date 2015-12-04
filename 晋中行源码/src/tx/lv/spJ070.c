/************************************************************
* 文 件 名:   spJ070.c
* 功能描述：  小额业务支付拼包交易
*
* 作    者:   chenhw
* 完成日期：  2006-8-30 仅以本程序献给天下有情人
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "hv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#include "lv_wbctl_c.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"

spJ070()
{
	int ret=0,count=0;
	int iPackid=0;
	int recid=0;
	int sql_cnt=0;
	int iHzdate;

	char cBrno[6];
	char cTbsdy[9];
	char cPackcd[2];
	char cPkgno[4];
	char cPackid[9];
	char cWtday[9];
	char cCashqsactno[13];
	char cOrdersel[2];
	char cOrderno1[9];
	char cOrderno2[9];
	char cPackend[2];
	char cHzdate[3];
	char sql_stat[2];
	char cTx_br_no[6];
	char cTempBuff[21];
	memset(cTx_br_no,0x00,sizeof(cTx_br_no));

	struct lv_wbctl_c  	g_lv_wbctl;
	struct lv_pkgreg_c  	g_lv_pkgreg;

	memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
	memset(&g_lv_pkgreg ,0x00 ,sizeof(struct lv_pkgreg_c));

	memset(cBrno ,'\0',sizeof(cBrno));
	memset(cTbsdy ,'\0',sizeof(cTbsdy));
	memset(cPackcd,'\0',sizeof(cPackcd));
	memset(cPkgno,'\0',sizeof(cPkgno));
	memset(cPackid,'\0',sizeof(cPackid));
	memset(cWtday,'\0',sizeof(cWtday));
	memset(cCashqsactno,'\0',sizeof(cCashqsactno));
	memset(cOrdersel,'\0',sizeof(cOrdersel));
	memset(cOrderno1,'\0',sizeof(cOrderno1));
	memset(cOrderno2,'\0',sizeof(cOrderno2));
	memset(cPackend,'\0',sizeof(cPackend));
	memset(cHzdate,'\0',sizeof(cHzdate));
	memset(sql_stat,'\0',sizeof(sql_stat));
	memset(cTempBuff,'\0',sizeof(cTempBuff));

	vtcp_log("[%s][%d]spJ070小额业务支付拼包交易BEGIN",__FILE__,__LINE__);

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/** 取得FD数据 **/
	get_fd_data( "0030", &cBrno);		/** 取得交易机构 **/
	get_fd_data( "0050", &cTbsdy);		/** 取得交易机构 **/
	get_fd_data( "0670", &cPackcd);		/**包追加标志**/
	get_fd_data( "0230", &cPkgno);		/**报文编号**/
	get_fd_data( "0530", &cPackid);		/**包序号**/
	get_fd_data( "0440", &cWtday);		/**交易委托日期**/
	get_fd_data( "0500", &cHzdate);		/**回执期限**/
	get_fd_data( "0620", &cTempBuff);	/**接收清算行行号**/
	get_fd_data( "0540", &cOrderno1);	/**起始交易序号**/
	get_fd_data( "0550", &cOrderno2);	/**结束交易序号**/
	get_fd_data( "0680", &cOrdersel);	/**支付交易序号范围**/
	get_fd_data( "0690", &cPackend);	/**包结束标志**/
  zip_space(cTempBuff);
  vtcp_log("[%s][%d]cTempBuff=[%s]\n",__FILE__,__LINE__,cTempBuff);
  memcpy(cCashqsactno,cTempBuff,sizeof(cCashqsactno)-1);
	vtcp_log("[%s][%d]spJ030[%s][%s][%s][%s][%s][%s][%s][%s][%s][%s][%s]BEGIN",__FILE__,__LINE__,cBrno,cPackcd,cPkgno,cPackid,cWtday,cHzdate,cCashqsactno,cOrderno1,cOrderno2,cOrdersel,cPackend);

	if(!memcmp(g_pub_tx.tx_br_no, QS_BR_NO ,5))
			memcpy(cTx_br_no,"%%%%%%",5);
	else
			memcpy(cTx_br_no,g_pub_tx.tx_br_no,5);

	if ( cPackcd[0] == '1' )  /*  对添加到原有包交易进行检查 */
	{
		ret = Lv_wbctl_Sel( g_pub_tx.reply,&g_lv_wbctl,"packid = %s and pack_date = %s",cPackid,cWtday);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"小额往包控制表中不存在该记录");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P025");
			goto ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		if (memcmp(cPkgno,g_lv_wbctl.pkgno,sizeof(cPkgno)-1)!=0)
		{
			sprintf(acErrMsg,"交易类型不一致!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P023");
			goto ErrExit;
		}
		/***检查状态是否为正在拼包***/
		if (g_lv_wbctl.stat[0]!=PSTAT_PACKING)
		{
			sprintf(acErrMsg,"往包控制表状态错!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P024");
			goto ErrExit;
		}
	}

	/** 贷记和贷记退汇要求复核状态 其他为录入 **/
	if (memcmp(cPkgno,PKGNO_PTDJ,sizeof(cPkgno)-1)==0||
		memcmp(cPkgno,PKGNO_DQDJ,sizeof(cPkgno)-1)==0||
		memcmp(cPkgno,PKGNO_DJTH,sizeof(cPkgno)-1)==0)
	{
		sql_stat[0]=STAT_WZCHECK2;
	}
	else
	{
		sql_stat[0]=STAT_WZLR;
	}
	if ( cOrdersel[0]=='0' )
	{
		/***序号范围为所有***/
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"pkgno='%.3s' and cash_qs_no='%.12s' and lv_sts='%s' and br_no like '%s'  and packid is null",
			cPkgno,cCashqsactno,sql_stat,cTx_br_no);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		/****选择序号范围****/
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,
			"pkgno='%.3s' and cash_qs_no='%.12s' and orderno>='%08ld' and orderno<='%08ld' and lv_sts='%s' and br_no  like  '%%%s'  and packid is null  ",
			cPkgno,cCashqsactno,atol(cOrderno1),atol(cOrderno2),sql_stat,cTx_br_no);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	while (1)
	{
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret = Lv_pkgreg_Fet_Upd(&g_lv_pkgreg,g_pub_tx.reply);
		recid++;
		vtcp_log("%s,%d orderno=[%s]",__FL__,g_lv_pkgreg.orderno);
		if (ret!=0  && ret!=100)
		{
			sprintf(acErrMsg,"查找记录出错[%d]",ret);
			vtcp_log("%s,%d here ",__FL__);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		if ( ret==100 )
		{
			if ( recid==1 )
			{
				sprintf(acErrMsg,"没有可拼包的交易[%d]",ret);
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P027");
				goto ErrExit;
			}
			vtcp_log("[%s][%d]spJ030循环记录处理完毕",__FILE__,__LINE__);
			break;
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

		/** 添加回执期限和日期 **/
		if (memcmp(cPkgno,PKGNO_PTJJ,sizeof(cPkgno)-1)==0 || memcmp(cPkgno,PKGNO_DQJJ,sizeof(cPkgno)-1)==0)
		{
			vtcp_log("[%s][%d]spJ030=========hzdate=[%s]",__FILE__,__LINE__,cHzdate);
			iHzdate = apatoi(cHzdate,1);
			vtcp_log("[%s][%d]spJ030=========hzdate=[%d]",__FILE__,__LINE__,iHzdate);
			ret=pub_base_deadlineD(g_lv_pkgreg.wt_date,iHzdate,&g_lv_pkgreg.resp_date);			
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
			if ( ret!=0 )
			{
				sprintf(acErrMsg,"回执期限检查失败!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"P028");
				vtcp_log("%s,%d here ",__FL__);
				goto ErrExit;
			}
			vtcp_log("[%s][%d]spJ030_-------___--cHzdate==[%s]---=====+++",__FILE__,__LINE__,cHzdate);

			ret=iCheckHzdate(cCashqsactno,iHzdate);
			if( ret!=0 )
			{
			sprintf(acErrMsg,"回执期限检查失败!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P029");
			goto ErrExit;
			}

			vtcp_log("[%s][%d]spJ030======lv_pkgreg.resp_date=[%ld]",__FILE__,__LINE__,g_lv_pkgreg.resp_date);
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

		/** 检查交易状态  **/
		if (memcmp(cPkgno,"002",3) ==0 || memcmp(cPkgno,"006",3)==0||memcmp(cPkgno,"012",3)==0)
		{
			if (g_lv_pkgreg.lv_sts[0]!=STAT_WZLR)
			{
				sprintf(acErrMsg,"支付交易状态不符!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P030");
				goto ErrExit;
			}
		}
		else
		{
			if (g_lv_pkgreg.lv_sts[0]!=STAT_WZCHECK2)
			{
				sprintf(acErrMsg,"支付交易状态不符!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P030");
				goto ErrExit;
			}
		}
		if (g_lv_pkgreg.packid[0]!='\0')
		{
			sprintf(acErrMsg,"支付交易已经拼到包[%.8s]!",g_lv_pkgreg.packid);
			WRITEMSG
			vtcp_log("%s,%d here ",__FL__);
			strcpy(g_pub_tx.reply,"P031");
			goto ErrExit;
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

		if (cPackcd[0]=='1')
		{
			/****添加到原有包**/
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
			vtcp_log("%s,%d orderno=[%s]",__FL__,g_lv_pkgreg.orderno);
			ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
				"pack_date='%d' and packid=%s",g_lv_wbctl.pack_date,cPackid);
			vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d here ",__FL__);
			ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
			vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			if(memcmp(cPkgno,PKGNO_TYXX,sizeof(cPkgno)-1)==0)
			if ( memcmp(g_lv_wbctl.txnum, g_lv_pkgreg.txnum , sizeof(g_lv_wbctl.txnum)-1) != 0)
			{
				sprintf(acErrMsg,"本记录业务类型=[%s],包业务类型=[%s]",g_lv_pkgreg.txnum,g_lv_wbctl.txnum);
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P031");
				goto ErrExit;
			}
			if (g_lv_wbctl.stat[0] !=PSTAT_PACKING)
			{
				sprintf(acErrMsg,"支付交易状态不符!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P030");
				goto ErrExit;
			}
			vtcp_log("%s,%d dtlcnt=[%d]",__FL__,g_lv_wbctl.dtlcnt);
			g_lv_wbctl.dtlcnt +=1;
			g_lv_wbctl.totamt +=g_lv_pkgreg.tx_amt;

			memcpy(g_lv_pkgreg.packid,  g_lv_wbctl.packid,  sizeof(g_lv_pkgreg.packid)-1);
			g_lv_pkgreg.pack_date=g_lv_wbctl.pack_date;

			ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			vtcp_log("%s,%d here ",__FL__);
			Lv_wbctl_Clo_Upd();
			vtcp_log("%s,%d here ",__FL__);
			ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
			vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			/****产生一个新包****/
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
			vtcp_log("%s,%d orderno=[%s]",__FL__,g_lv_pkgreg.orderno);
			if (cPackcd[0]=='0')
			{
				/**新生成包***/
				iPackid=iLvGetPackid();
				vtcp_log("%s,%d here ",__FL__);
				if( ret )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,ret);
					strcpy( g_pub_tx.reply,"P026" );
					sprintf(acErrMsg,"SQLERR [%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
			}
			if (recid==1)
			{
				/****产生新宝的第一条记录，设置新包内容,以后就不再设置了*/
				vtcp_log("[%s][%d]spJ030ipackid=[%ld]",__FILE__,__LINE__,iPackid);
				memset(&g_lv_wbctl ,0x00 ,sizeof(struct lv_wbctl_c));
				/** 准备插入数据，先初始化 **/
				g_lv_wbctl.pack_date= apatoi(pcLvGetDay(), 8);
				apitoa(iPackid,sizeof(g_lv_wbctl.packid)-1,g_lv_wbctl.packid);
				vtcp_log("[%s][%d]spJ030ipackid=[%s]",__FILE__,__LINE__,g_lv_wbctl.packid);
				memcpy(g_lv_wbctl.pkgno, g_lv_pkgreg.pkgno, sizeof(g_lv_wbctl.pkgno)-1);
				memcpy(g_lv_wbctl.pay_qs_no, g_lv_pkgreg.pay_qs_no, sizeof(g_lv_wbctl.pay_qs_no)-1);
				memcpy(g_lv_wbctl.cash_qs_no, g_lv_pkgreg.cash_qs_no, sizeof(g_lv_wbctl.cash_qs_no)-1);
				memcpy(g_lv_wbctl.txnum, g_lv_pkgreg.txnum, sizeof(g_lv_wbctl.txnum)-1);
				g_lv_wbctl.in_date= apatoi(cTbsdy, 8);
				memcpy(g_lv_wbctl.br_no, cBrno, sizeof(g_lv_wbctl.br_no)-1);
				g_lv_wbctl.dtlcnt=1;
				g_lv_wbctl.totamt=g_lv_pkgreg.tx_amt;
				g_lv_wbctl.bfflag[0]='0';
				g_lv_wbctl.stat[0]=PSTAT_PACKING;
				g_lv_wbctl.ffflag[0]='0';      /** 辖内分发标志,暂时赋成0 **/
				memcpy(g_lv_wbctl.brprtflag,"000",sizeof(g_lv_wbctl.brprtflag)-1);
				memcpy(g_lv_wbctl.qsprtflag,"000",sizeof(g_lv_wbctl.qsprtflag)-1);
				vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

				ret = Lv_wbctl_Ins(g_lv_wbctl,g_pub_tx.reply);
				vtcp_log("%s,%d here ",__FL__);
				if( ret )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,ret);
					strcpy( g_pub_tx.reply,"P026" );
					sprintf(acErrMsg,"SQLERR [%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				vtcp_log("[%s][%d]spJ030ipackid=[%ld]",__FILE__,__LINE__,iPackid);
				apitoa(iPackid,sizeof(g_lv_wbctl.packid)-1,g_lv_wbctl.packid);

				vtcp_log("[%s][%d]spJ030----cHzdate[%d]=[%s]---",__FILE__,__LINE__,g_lv_wbctl.pack_date,g_lv_wbctl.packid);
				memcpy(g_lv_pkgreg.packid,  g_lv_wbctl.packid,  sizeof(g_lv_pkgreg.packid)-1);
				g_lv_pkgreg.pack_date=g_lv_wbctl.pack_date;

				ret = Lv_pkgreg_Upd_Upd(g_lv_pkgreg,g_pub_tx.reply);
				vtcp_log("%s,%d here ",__FL__);
				if( ret )
				{
					vtcp_log("%s,%d here ret=%d",__FL__,ret);
					strcpy( g_pub_tx.reply,"P026" );
					sprintf(acErrMsg,"SQLERR [%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
				vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

				/**************如果为生成新包,其实只有第一条记录需要生成
				******以后的记录还是追加,所以就修改一下tita中的值,使下一次
				******循环能够走到追加的分支*****************************/
				cPackcd[0]='1';
				apitoa(iPackid,sizeof(cPackid)-1,cPackid);

			}
			else
			{
				/****不应该走到这个分支****/
				sprintf(acErrMsg,"新生成包时出错!");
				WRITEMSG
				vtcp_log("%s,%d here ",__FL__);
				strcpy(g_pub_tx.reply,"P031");
				goto ErrExit;
			}
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
	}
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	Lv_pkgreg_Clo_Upd();
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if (memcmp(cPkgno,PKGNO_DQDJ,sizeof(cPkgno)-1)==0)
	{
		/****定期贷记业务,必须只有一个付款人****/
		char cTableBuf[513];
		sql_cnt=0;
		memset(cTableBuf, 0 , sizeof(cTableBuf));
		sprintf(cTableBuf,"(select count(*) from lv_pkgreg where pkgno='005' and pack_date=%d and packid='%s' group by wt_date,pay_ac_no)",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("[%s][%d]cTableBuf=[%s]",__FILE__,__LINE__,cTableBuf);
		sql_cnt=sql_count(cTableBuf,"1=1");
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
/**
		sql_cnt=sql_count("lv_pkgreg","pack_date=%d and packid='%.8s' group by pay_ac_no",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		ret = sql_execute("select count(*) into sql_cnt from lv_pkgreg where pkgno='005' and pack_date=%d and packid='%s' group by wt_date,pay_ac_no",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
**/
		
		vtcp_log("[%s][%d]sql_cnt=%d",__FILE__,__LINE__,sql_cnt);
		if ( sql_cnt!=1 )
		{
			sprintf(acErrMsg,"必须只有一个付款人!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P031");
			goto ErrExit;
		}
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
	}
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if (memcmp(cPkgno,PKGNO_DQJJ,sizeof(cPkgno)-1)==0)
	{
		/****定期借记业务,必须只有一个收款人****/
		char cTableBuf[513];
		sql_cnt=0;
		memset(cTableBuf, 0 , sizeof(cTableBuf));
		sprintf(cTableBuf,"(select count(*) from lv_pkgreg where pkgno='006' and pack_date=%d and packid='%s' group by wt_date,cash_ac_no)",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("[%s][%d]cTableBuf=[%s]",__FILE__,__LINE__,cTableBuf);
		sql_cnt=sql_count(cTableBuf,"1=1");
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
/**
		sql_cnt=0;
		sql_cnt=sql_count("lv_pkgreg","pack_date=%d and packid='%s' group by wt_date,cash_ac_no",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
**/
		if ( sql_cnt!=1 )
		{
			sprintf(acErrMsg,"必须只有一个收款人!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P072");
			goto ErrExit;
		}
	}
	vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

	if (cHzdate[0]!='0')	/** 加入回执期限 **/
	{
		vtcp_log("[%s][%d]spJ030----cHzdate[0]=[%d]---",__FILE__,__LINE__,cHzdate[0]);
		vtcp_log("[%s][%d]spJ030----cHzdate[%d]=[%s]---",__FILE__,__LINE__,g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
			"pack_date='%d' and packid=%s",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
			vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		g_lv_wbctl.hz_date=apatoi(cHzdate,2);
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		vtcp_log("[%s][%d]spJ030----g_lv_wbctl.stat[0]=[%c]---",__FILE__,__LINE__,g_lv_wbctl.stat[0]);
		ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		Lv_wbctl_Clo_Upd();
	}
	vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

	/****把包记录封闭****/
	if (cPackend[0]=='1')
	{
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret=Lv_wbctl_Dec_Upd(g_pub_tx.reply,
			"pack_date='%d' and packid=%s",g_lv_wbctl.pack_date,g_lv_wbctl.packid);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		ret=Lv_wbctl_Fet_Upd(&g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
		if( ret )
		{
			vtcp_log("%s,%d here ret=%d",__FL__,ret);
			strcpy( g_pub_tx.reply,"P026" );
			sprintf(acErrMsg,"SQLERR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("%s,%d here ",__FL__);
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		g_lv_wbctl.stat[0]=PSTAT_PACKED;
		vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);
		vtcp_log("[%s][%d]spJ030----g_lv_wbctl.stat[0]=[%c]---",__FILE__,__LINE__,g_lv_wbctl.stat[0]);
		ret=Lv_wbctl_Upd_Upd(g_lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d here ",__FL__);
			if( ret )
			{
				vtcp_log("%s,%d here ret=%d",__FL__,ret);
				strcpy( g_pub_tx.reply,"P026" );
				sprintf(acErrMsg,"SQLERR [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			Lv_wbctl_Clo_Upd();
	}
	vtcp_log("[%s][%d]spJ030",__FILE__,__LINE__);

	/* 记流水日志 */
	g_reply_int = pub_ins_trace_log();
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"P031");
		goto ErrExit;
	}
	set_zd_data("0530",g_lv_wbctl.packid);
	set_zd_long("0290",g_lv_wbctl.dtlcnt);
	set_zd_double("0330",g_lv_wbctl.totamt);

	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	vtcp_log("%s,%d here ",__FL__);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
