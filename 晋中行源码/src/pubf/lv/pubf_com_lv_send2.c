/***************************************************************************/
/* 文件名称: lv_sendcomm.pc                                                */
/* 功能描述: 小额拼包发送操作公共函数                                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lvpack.h"
#include "exitlvpack.h"
#include "dmem.h"
#include "lvnet.h"
#include "lv_pub.h"
#include "public.h"
extern char *pcLvGetDay_work();
extern char *pcLvGetDay();
extern char * zip_space(char *);

int ret;
char *lv_date(long);
int iLvSendPackage(TLVPACKAGE *lv);

/****Modified by SSH,20006.1.13****/
/**回执业务往帐中的包信息为原来包的信息,其他都与交易信息一致 **/
/**********************************/
/** pkg002 **/
/** 拼普通借记发送包 **/
/** 传入参数为包id **/
int lv_get_ptjj_tag(char *packday,char *packid)
{
	TLVPACKAGE pkg002;
	PAY_IN_AREA pay_in;
	char cHzDate[3];
	static struct lv_pkgreg_c     lv_pkgreg;
	static struct lv_wbctl_c      wd_lv_wbctl;
	int 	iRc=0;
	int		ret=0;
	int		wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	double	dTxamt;
	char	Stat[2];
	char	Stat2[2];
	/****HSYX BEGIN :放大附加域缓冲区****/
	char    tmpBuf[8192];
	/****HSYX END :放大附加域缓冲区****/

	/** 游标所用变量 **/
	/*****
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	char	PAYNO[13];
	char	PAYACTNO[33];
	char	PAYNAME[61];
	char	PAYADDRESS[61];
	char	CASHNO[13];
	char	CASHACTNO[33];
	char	CASHNAME[61];
	char	CASHADDRESS[61];
	char	YWTYPE[13];
	char	CONTENT[61];
	char	ADDID[9];
	**********/

	memset(&lv_pkgreg, 0 , sizeof(lv_pkgreg));
	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));
	/**
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(PAYNO,    '\0', sizeof(PAYNO));
	memset(PAYACTNO, '\0', sizeof(PAYACTNO));
	memset(PAYNAME,  '\0', sizeof(PAYNAME));
	memset(PAYADDRESS,'\0', sizeof(PAYADDRESS));
	memset(CASHNO,    '\0', sizeof(CASHNO));
	memset(CASHACTNO, '\0', sizeof(CASHACTNO));
	memset(CASHNAME,  '\0', sizeof(CASHNAME));
	memset(CASHADDRESS,'\0', sizeof(CASHADDRESS));
	memset(YWTYPE,	 '\0', sizeof(YWTYPE));
	memset(CONTENT,	 '\0', sizeof(CONTENT));
	memset(ADDID,	 '\0', sizeof(ADDID));
	***/
	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(cHzDate, 0 , sizeof(cHzDate));
	lv_load_tagifno();
	lv_create(&pkg002);

	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	vtcp_log("[%s][%d]===============wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	vtcp_log("[%s][%d]===============wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	ret = Lv_wbctl_Dec_Upd(g_pub_tx.reply,"pack_date = %d and packid = %s ", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	if (ret != 0)
	{
		sprintf( acErrMsg,"定义游标出错-%d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	if ((g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");

		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");

		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	wd_dtlcnt = (wd_lv_wbctl.dtlcnt+0.001)/1;

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg002.opeType = '1';	/** 小额 **/
	pkg002.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg002.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg002.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg002.CMTCode,PKGNO_PTJJ,3);	/** 因为是PKG002包**/
	memcpy(pkg002.workDate,pcLvGetDay_work(),8);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	/** 开始查找对应明细表中的记录 **/
	Stat[0] = STAT_WZLR;
	vtcp_log("[%s][%d]========packday=[%s],packid=[%s],stat=[%s]\n",__FL__,packday,packid,Stat);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	if (g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and lw_ind = '1' and pay_qs_no = '%s' and packid = '%s' and pack_date = %ld and lv_sts = '%s' ", PKGNO_PTJJ, wd_lv_wbctl.pay_qs_no , wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, Stat))
	{
		sprintf( acErrMsg,"取lv_pkgreg游标出错 %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{
		vtcp_log("[%s][%d]+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");

			lv_destroy(&pkg002);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"数据库错误!");

			lv_destroy(&pkg002);
			WRITEMSG
				return(-1);
		}
		vtcp_log("[%s][%d]==-----+++++----===",__FL__);

		/** 要素集内容 **/

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		memcpy(pkg002.CMTNo,"001",3);       /***要素集号001***/
		pkg002.paymPRI='0';                 /***优先级***/
		lv_set_tag(&pkg002,"0BG",lv_pkgreg.txnum);	/** 业务类型号 **/
		lv_set_tag(&pkg002,"52A",lv_pkgreg.or_br_no);	/** 发起行行号 **/
		lv_set_tag(&pkg002,"58A",lv_pkgreg.ac_br_no);	/** 接收行行号 **/
		lv_set_tag(&pkg002,"30A",lv_date(lv_pkgreg.wt_date));	/**	委托日期 **/
		lv_set_tag(&pkg002,"0BC",lv_pkgreg.orderno);	/**	支付交易序号 **/
		dTxamt = lv_pkgreg.tx_amt*100;
		vtcp_log("[%s][%d]====dTxamt=[%.2f]\n",__FL__,dTxamt);
		sprintf(tmpBuf,"%015.0f",dTxamt);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg002,"33G",tmpBuf);	/**	金额 **/
		lv_set_tag(&pkg002,"CC4",lv_pkgreg.pay_opn_br_no);	/**	付款人开户行行号 **/
		lv_set_tag(&pkg002,"50C", lv_pkgreg.pay_ac_no);	/**	付款人账号 **/
		lv_set_tag(&pkg002,"50A", lv_pkgreg.pay_name);	/**	付款人名称 **/
		lv_set_tag(&pkg002,"50B", lv_pkgreg.pay_addr);	/**	付款人地址 **/
		lv_set_tag(&pkg002,"CC5", lv_pkgreg.cash_opn_br_no);	/**	收款人开户行行号 **/
		lv_set_tag(&pkg002,"59C", lv_pkgreg.cash_ac_no);	/**	收款人账号 **/
		lv_set_tag(&pkg002,"59A", lv_pkgreg.cash_name);	/**	收款人名称 **/
		lv_set_tag(&pkg002,"59B", lv_pkgreg.cash_addr);	/**	收款人地址 **/
		lv_set_tag(&pkg002,"CEG", lv_pkgreg.lv_yw_ind);	/**	业务种类 **/
		lv_set_tag(&pkg002,"72A", lv_pkgreg.lv_brf);	/**	附言 **/

		vtcp_log("[%s][%d]==-----+++++----===",__FL__);
		/** 拼附加数据 **/
		if (memcmp(lv_pkgreg.txnum,TXNUM_QTJJ,sizeof(lv_pkgreg.txnum)-1)==0) /** 其他借记业务 **/
		{
			lv_set_tag(&pkg002,"B40","00000000");
		}
		else
		{
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			apitoa(lv_pkgreg.wt_date,8,pay_in.F_wtday);
			memcpy(pay_in.F_orbrno,lv_pkgreg.or_br_no,sizeof(pay_in.F_orbrno));
			memcpy(pay_in.F_orderno,lv_pkgreg.orderno,sizeof(pay_in.F_orderno));
			memcpy(pay_in.T_addid,lv_pkgreg.addid,sizeof(pay_in.T_addid));
			vtcp_log("%s,%d addid ====[%s] ",__FL__,lv_pkgreg.addid);
			memcpy(pay_in.T_inday, pay_in.F_wtday,8);
			iLvGetAddit(&pay_in,"000",tmpBuf,0);
			vtcp_log("[%s][%d]--------------tmpBuf[0]=[%c]\n",__FILE__,__LINE__,tmpBuf[0]);
			/*** dongxy 因为录入交易拼了000域，所以，拼包时不拼那个域了
			if (tmpBuf[0]=='\0')
			***/
			/****NEWYX2:有000还是先拼000,001-005的处理方式是老系统的不得已而为之新系统完全没有必要****/
			if ((memcmp(lv_pkgreg.txnum,"30102",sizeof(lv_pkgreg.txnum)-1)==0 || 
				memcmp(lv_pkgreg.txnum,"30103",sizeof(lv_pkgreg.txnum)-1)==0)&& 
				tmpBuf[0]=='\0')
			{
				/*** HSYX del begin
				lv_set_tag(&pkg002,"B40","00000000");   ** 附加数据长度**
				HSYX del end ****/
				/****HSYX BEGIN:拼001-005的附加域****/
				vtcp_log("[%s][%d]----------没有有附加数据\n",__FILE__,__LINE__);
				int sIndex=0;
				int sAddLen=0;
				memset(tmpBuf,'\0',sizeof(tmpBuf));
				for (sIndex=1;sIndex<=5;sIndex++)
				{
					char pcTagName[4];
					int i=0;
					sprintf(pcTagName,"%03d",sIndex);
					iLvGetAddit(&pay_in,pcTagName,tmpBuf+sAddLen,0);
					vtcp_log("%s,%d,Gethere,第[%d]个附加域\n[%s]\n",__FILE__,__LINE__,sIndex,tmpBuf);
					if (tmpBuf[sAddLen]=='\0')
					{
						/****无此附加域，附加域循环结束****/
						if (sIndex==2)
						{
							/****背书人列表****/
							strcat(tmpBuf+sAddLen,"00");
							sAddLen=strlen(tmpBuf);
						}else
						{
							break;
						}
					}else
					{
						/****有此附加域，拼附加域****/
						sAddLen=strlen(tmpBuf);
					}
					vtcp_log("[%s][%d]-------------sAddLen=[%d]\n",__FILE__,__LINE__,sAddLen);
				}
				if (tmpBuf[0]=='\0')
				{
					lv_set_tag(&pkg002,"B40","00000000");   /** 附加数据长度**/
				}else
				{
					char len_buf[9];
					sprintf(len_buf,"%08d",strlen(tmpBuf));
					lv_set_tag(&pkg002,"B40",len_buf);  /** 附加数据长度**/
					lv_set_tag(&pkg002,"72C",tmpBuf);       /** 附加数据 **/
				}
				vtcp_log("[%s][%d]------72C=[%s]",__FL__,tmpBuf);
				/****HSYX END:拼001-005的附加域****/
			}
			else
			{
				vtcp_log("[%s][%d]----------有附加数据\n",__FILE__,__LINE__);
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg002,"B40",len_buf);  /** 附加数据长度**/
				lv_set_tag(&pkg002,"72C",tmpBuf);       /** 附加数据 **/
				vtcp_log("[%s][%d]-------------72C=[%s]",__FL__,tmpBuf);
			}
			vtcp_log("[%s][%d]------72C=[%s]",__FL__,tmpBuf);
		}
		lv_batch_end(&pkg002);
		vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	}
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	/** 包头内容 **/
	lv_pkg_batch(&pkg002,"02C",wd_lv_wbctl.pkgno);/** 包类型号 **/
	lv_pkg_batch(&pkg002,"011",Payqsactno);/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg002,"012",Cashqsactno);/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg002,"30E",packday);/** 包委托日期 **/
	lv_pkg_batch(&pkg002,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	/** lv_pkg_batch(&pkg002,"C15","");	包密押 **/
	sprintf(cHzDate,"%02d",wd_lv_wbctl.hz_date);
	vtcp_log("%s,%d 回执日期=%s",__FL__,cHzDate);
	vtcp_log("%s,%d 回执日期=%d",__FL__,wd_lv_wbctl.hz_date);
	lv_pkg_batch(&pkg002,"BS6",cHzDate);/** 回执期限（天数） **/

	/** 剩余tag稍后再补 **/
	vtcp_log("[%s][%d]---------wd_lv_wbctl.dtlcnt=[%f],totamt=[%f]\n",__FL__,wd_lv_wbctl.dtlcnt,wd_lv_wbctl.totamt);
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg002,"B63",Cnt);	/** 明细业务总笔数 **/

	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100);
	lv_pkg_batch(&pkg002,"32B",tmpBuf);	/** 明细业务总金额 **/
	vtcp_log("[%s][%d]---------Cnt=[%s]tmpBuf=[%s]\n",__FL__,Cnt,tmpBuf);
	/** 
	lv_pkg_batch(&pkg002,"BSJ",);	转发日期 **/
	lv_pkg_batch(&pkg002,"72D","");	/** 包附加数据 **/
	vtcp_log("[%s][%d]==========\n",__FL__);

	lv_gen_pack(&pkg002);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	dm_show(&(pkg002.dm));
	/****
	dm_show(&(pkg002.dm));
	****/
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	/** 更新状态为已发送 **/
	vtcp_log("[%s][%d]==========\n",__FL__);

	ret = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and pkgno = '%s' and lw_ind = '1' " , Stat, packid, packday, PKGNO_PTJJ);
	if (ret!=DB_OK && ret!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==========\n",__FL__);
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库错误!");

		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==========\n",__FL__);

	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	vtcp_log("[%s][%d]==========\n",__FL__);
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新错误");

		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==========\n",__FL__);
	Lv_wbctl_Clo_Upd();
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	ret = lv_insSendList(&pkg002);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	if (ret!=0)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新错误");

		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);

	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&pkg002))
	{
		lv_destroy(&pkg002);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg002);
	vtcp_log("[%s][%d]==-----+++++----===",__FL__);
	return(0);
}
