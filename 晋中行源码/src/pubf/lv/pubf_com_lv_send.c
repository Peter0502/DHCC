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

int ret=0;
char *lv_date(long);
int iLvSendPackage(TLVPACKAGE *lv);
/******************************************************************************
****                   拼报文时注意事项(石升辉)                            ****
****                         (2005.11.27)                                  ****
****  1.锁定包时必须指定日期而不能默认是当天,因为小额允许发送以前的包      ****
****  2.取当前日期时用函数pcLvGetDay,而不能用it_txcom.tbsdy,因为在报文中用  ****
****    到的日期都是小额系统日期，而不是综合业务系统日期                   ****
****  3.判断包或者交易记录的状态时尽量使用宏定义，而不要使用其值           ****
****  4.需要对要素集号赋值                                                 ****
****  5.不要对(C15)密押赋值                                                ****
****  6.注意金额在数据库中和在报文中都是以分为单位                         ****
****  7.注意用Dbs函数取出的字段,长度超过6的整数是用doulbe型表示的,而不是   ****
****    int或long型,所以使用sprintf函数时要注意类型                        ****
****  8.必须对附加数据域赋值,对于某种业务，如果表中已经包含了它的附加字段  ****
****    那么可以直接从表中取出来赋上，否则从lv_addit表中取出附加内容,使用  ****
****    方法可参考下例,或者根据iLvGetAddit函数的实现方法来自己写都行      ****
****  9.使用lv_connect时要注意参数,批量报文用LV_SERVER_BATCH,非批量报文用  ****
****    LV_SERVER_RT                                                       ****
****  10.把发送和接收报文放到更新数据库表之后                              ****
****  11.下面lv_get_ptdj_tag是一个拼普通贷记PKG001报文的范例,各人可以参考  ****
****     对于已经写好的拼包函数可不必重写，但必须根据以上的注意事项做出    ****
****     相应修改                                                          ****
****  12.各人写的程序必须加标注,特别是修改(包括增加和删除代码)不是自己写的 ****
****     程序时,必须注明是谁在何日因何原因做出修改                         ****
****  补充                                                                 ****
****      1.请注意在程序正常退出和异常退出时都要关闭SOCKET及destroy包结构  ****
****      1.发送前要插入发送登记簿,调用lv_insSendList即可                  ****
******************************************************************************/

/* 为了编译过去，造了个假的 */
int lv_Ins_Cmthtr(NOTPAY_IN_AREA *notpayin)
{
	return 0;
}
/****Add by SSH,2006.4.8,把发送和接收部分提出来*****/
int iLvSendPackage(TLVPACKAGE *lv)
{
	int iRc=0,iSocketID=0;
	char pcCmtno[4];
	char respCode[9];
	char respBuf[512];
	memset(respCode,'\0',sizeof(respCode));
	memset(respBuf,'\0',sizeof(respBuf));
	memset(pcCmtno,'\0',sizeof(pcCmtno));
	memcpy(pcCmtno,lv->CMTCode,3);
	vtcp_log("%s,%d,发包[.3s]\n",__FL__,pcCmtno);
	iRc = lv_initPara(NULL);
	if (iRc)
	{
		vtcp_log("%s,%d,初始化网络参数错误 return = %d!",__FL__,iRc);
		sprintf( acErrMsg,"初始化网络参数错误!");
		WRITEMSG
		sprintf( acErrMsg,"初始化网络参数错误!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	if (strstr("003,004,009,010,316,321,323,013",pcCmtno)!=NULL)
	{
		iSocketID = lv_connect(LV_SERVER_RT);
	}
	else
	{
		iSocketID = lv_connect(LV_SERVER_BATCH);
	}
	if (iSocketID<=0)
	{
		vtcp_log("%s,%d,连接前置机错误 return = %d!",__FL__,iSocketID);
		sprintf( acErrMsg,"连接前置机错误!");
		WRITEMSG
			sprintf( acErrMsg,"连接前置机错误!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,连接前置机成功 return = %d!",__FL__,iSocketID);	
	iRc = lv_send(iSocketID,lv);
	if (iRc)
	{
		vtcp_log("%s,%d,发送数据包到前置机出错 return = %d!",
			__FL__,iRc);
		lv_netClose(iSocketID);
		sprintf( acErrMsg,"发送数据包到前置机出错!");
		WRITEMSG
		sprintf( acErrMsg,"发送数据包到前置机出错!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,发送数据包到前置机成功 !",__FL__);		
	lv_reset(lv);
	iRc = lv_recv(iSocketID,lv);
	if (iRc)
	{
		vtcp_log("%s,%d,接收数据包到前置机出错 return = %d!",
			__FL__,iRc);
		lv_netClose(iSocketID);
		sprintf( acErrMsg,"从前置机接收数据包出错!");
		WRITEMSG
		sprintf( acErrMsg,"从前置机接收数据包出错!");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d,从前置机接收数据包成功 !",__FL__);		
	iRc = lv_analyze_pack(lv);
	if (iRc)
	{
		vtcp_log("%s,%d,接收到的报文格式错误 return = %d!",__FL__,iRc);
		sprintf( acErrMsg,"接收到的报文格式错误!");
		WRITEMSG
		sprintf( acErrMsg,"接收到的报文格式错误!");
		set_zd_data("0130",acErrMsg);
		lv_netClose(iSocketID);
		return(-1);
	}
	vtcp_log("%s,%d,接收到的报文格式正确 !",__FL__);		
	memset(respCode,0,sizeof(respCode));
	memset(respBuf,0,sizeof(respBuf));
	lv_get_tag(lv,"C42",respCode);
	lv_get_tag(lv,"72A",respBuf);
	if ( respCode[3]!='I' )
	{
		char cErrmsg[100];
		memset(cErrmsg, 0 , sizeof(cErrmsg));
		WRITEMSG
		vtcp_log("%s,%d,报文发送到前置机失败 ERRNO=%s\nERRNOINFO=%s!",__FL__,respCode,respBuf);
		memcpy(cErrmsg,"前置机相应失败,",15);
		memcpy(cErrmsg+15,respBuf+8,sizeof(cErrmsg)-15);
		memcpy(acErrMsg,cErrmsg, sizeof(acErrMsg)-1);
		set_zd_data("0130",acErrMsg);
		vtcp_log("%s,%d errmsg=[%s]",__FL__,acErrMsg);
		lv_netClose(iSocketID);
		return(-1);
	}
	lv_netClose(iSocketID);  

	vtcp_log("%s,%d,iLvSendPackage() 成功 !",__FL__);		
	return(0);
}

struct lv_pkgreg_c lv_pkgreg;

/****Add by SSH,2005.11.27,把一个定长的字符数组转换为以\0结尾的串****/
static char *ADDZERO_a(char *str,int length)
{
	static char buf[513];
	memset(buf,'\0',length+1);
	memcpy(buf,str,length);
	return(buf);
}
/****Add by SSH,2005.11.27,把一个定长的字符数组转换为以\0结尾的串并补充空格****/
static char *ADDZERO_b(char *str,int length)
{
	static char buf[513];
	int i=0;
	memset(buf,'\0',length+1);
	memcpy(buf,str,length);
	buf[length]='\0';
	for (i=0;i<length;i++)
	{
		if (buf[i]=='\0')
			buf[i]=' ';
	}
	return(buf);
}
#define ADDZERO(s) ADDZERO_a(s,sizeof(s))
#define ADDSPZ(s) ADDZERO_b(s,sizeof(s))
/****Add by SSH,2005.11.27,一个拼包范例****/
int lv_Upd_Pkgreg_Stat(char i_stat, char *fmt, ...)
{
	struct  lv_pkgreg_c  wd_lv_pkgreg;
	char sqlbuf[512];
	int i=0;
	va_list vaptr;
	va_start(vaptr, fmt);
	vsprintf(sqlbuf, fmt, vaptr);
	va_end(vaptr);

	memset(&wd_lv_pkgreg, 0x0, sizeof(wd_lv_pkgreg));

	g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "%s", sqlbuf );
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错Lv_pkgreg - %d ", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	while (1)
	{
		g_reply_int = Lv_pkgreg_Fet_Upd(&wd_lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int==100 && i!=0)break;
		if (g_reply_int)
		{
			sprintf( acErrMsg,"取游标出错Lv_pkgreg - %d ", g_reply_int);
			WRITEMSG
				set_zd_data("0130",acErrMsg);
			return(-1);	
		}
		vtcp_log("[%s][%d]i_stat=[%s]",__FILE__,__LINE__,i_stat);
		wd_lv_pkgreg.lv_sts[0] = i_stat;
		vtcp_log("[%s][%d]lv_pkgreg.lv_sts=[%c]",__FILE__,__LINE__,wd_lv_pkgreg.lv_sts[0]);
		g_reply_int = Lv_pkgreg_Upd_Upd(wd_lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf( acErrMsg,"更新记录出错Lv_pkgreg - %d ", g_reply_int);
			set_zd_data("0130",acErrMsg);
			WRITEMSG
				return(-1);
		}
		/***
		Lv_pkgreg_Sel(g_pub_tx.reply,&wd_lv_pkgreg,sqlbuf);
		***/
		i++;
		vtcp_log("[%s][%d]i==[%d]",__FILE__,__LINE__,i);
		vtcp_log("[%s][%d]lv_pkgreg.lv_sts=[%c]",__FILE__,__LINE__,wd_lv_pkgreg.lv_sts[0]);
	}
	g_reply_int = Lv_pkgreg_Clo_Upd();
	if (g_reply_int)
	{
		sprintf( acErrMsg,"关闭更新游标出错 = %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	return 0;
}
int lv_get_ptdj_tag(char *packday,char *packid)
{
	char sql_stat[2];
	char sql_stat2[2];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_orderno[9];
	char cSysday[9];
	struct lv_pkgreg_c lv_pkgreg;
	int iRc=0;
	long txcnt=0;/****明细交易笔数****/
	char tmpBuf[1024];
	PAY_IN_AREA payin;

	TLVPACKAGE pkg001;
	static struct lv_wbctl_c      wd_lv_wbctl;
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(&pkg001, '\0', sizeof(pkg001));
	memset(cSysday   ,0, sizeof(cSysday));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wtday, '\0', sizeof(sql_wtday));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	lv_create(&pkg001);
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	/** 游标所用变量 **/
	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid, packid, sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZCHECK2;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);
	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply,"pack_date = %ld and packid = '%s'",	wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		WRITEMSG
			sprintf( acErrMsg,"未查到对应的往包记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg001);
		return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (DB_NOTFOUND == g_reply_int)
	{
		sprintf( acErrMsg,"无lv_wbctl记录");
		WRITEMSG
			sprintf( acErrMsg,"无lv_wbctl记录");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}else if(g_reply_int)
	{
		sprintf( acErrMsg,"取更新数据失败");
		WRITEMSG
			sprintf( acErrMsg,"取更新数据失败");
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",
			__FL__,wd_lv_wbctl.stat[0]);
		strcpy( g_pub_tx.reply,"P071");
		sprintf( acErrMsg,"包记录状态错误!");
		WRITEMSG
			sprintf( acErrMsg,"包记录状态错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg001);
		return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);
	txcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("%s,%d 看这里",__FL__);
	vtcp_log("[%s][%d]-----dtlcnt=[%d]txcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt,txcnt);

	/** 给游标需要的字段赋值 **/
	/** 开始查找对应明细表中的记录 **/
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and lw_ind = '1' and pay_qs_no = '%s' and packid = '%s' and pack_date = %ld and lv_sts = '%s' ", PKGNO_PTDJ, wd_lv_wbctl.pay_qs_no , wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, sql_stat);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"取lv_pkgreg游标出错 %d", g_reply_int);
		WRITEMSG
			sprintf( acErrMsg,"取lv_pkgreg游标出错 %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	/** 开始拼报文 **/
	/****Modified by SSH,2005.11.24,PKG号****/
	pkg001.opeType = '1';	/** 小额 **/
	pkg001.pkgType = 1;			/** 是pkg包 **/
	memcpy(pkg001.CMTCode,PKGNO_PTDJ,3);	/** PKG号 **/
	memcpy(pkg001.workDate,pcLvGetDay_work(),8);/**注意:是小额日期不是综合业务日期*/
	vtcp_log("%s,%d workdate===[%s] ",__FILE__,__LINE__,pkg001.workDate);

	/*******************************************/
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);
	for (;txcnt>0;txcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++txcnt=[%d]\n",
			__FL__,txcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

		if (g_reply_int!=DB_OK)
		{
			vtcp_log("%s,%d,数据库错误,sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误,没有该记录!");
			lv_destroy(&pkg001);
			WRITEMSG
				sprintf( acErrMsg,"访问数据库错误,没有该记录!");
			set_zd_data("0130",acErrMsg);
			return(-1);
		}
		strcpy(sql_orbrno, lv_pkgreg.or_br_no);

		strcpy(sql_wtday, lv_date(lv_pkgreg.wt_date));

		strcpy(sql_orderno, lv_pkgreg.orderno);

		zip_space(sql_orbrno); zip_space(sql_wtday); zip_space(sql_orderno);

		memset(&payin, '\0', sizeof(payin));
		memcpy(payin.F_orbrno,sql_orbrno,sizeof(payin.F_orbrno));

		memcpy(payin.F_wtday,sql_wtday,sizeof(payin.F_wtday));

		memcpy(payin.F_orderno,sql_orderno,sizeof(payin.F_orderno));

		memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));

		memcpy(payin.F_pkgno,PKGNO_PTDJ,sizeof(payin.F_pkgno));

		payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];

		payin.T_lw_ind[0]=lv_pkgreg.lw_ind[0];

		LV_CP_PAY_IN_FIELD(&payin,(&lv_pkgreg));

		iLvZipAll(&payin);
		iLvResetPayin(&payin);	

		/** 要素集内容 **/
		memset(tmpBuf, '\0', sizeof(tmpBuf));
		memcpy(pkg001.CMTNo,"001",3);		/***要素集号001***/
		pkg001.paymPRI='0';					/***优先级***/
		lv_set_tag(&pkg001,"0BG",ADDZERO(payin.F_txnum));/** 业务类型号 **/
		lv_set_tag(&pkg001,"52A",ADDZERO(payin.F_orbrno));/** 发起行行号 **/
		lv_set_tag(&pkg001,"58A",ADDZERO(payin.F_acbrno));/** 接收行行号 **/
		lv_set_tag(&pkg001,"30A",ADDZERO(payin.F_wtday));/**	委托日期 **/
		lv_set_tag(&pkg001,"0BC",ADDZERO(payin.F_orderno));/**	支付交易序号 **/
		lv_set_tag(&pkg001,"33G",ADDZERO(payin.F_txamt));	/**	金额 **/
		lv_set_tag(&pkg001,"CC4",ADDZERO(payin.F_payno));/**付款人开户行号*/
		lv_set_tag(&pkg001,"50C",ADDZERO(payin.F_payactno));/**付款人账号*/
		lv_set_tag(&pkg001,"50A",ADDZERO(payin.F_payname));	/**付款人名称**/
		lv_set_tag(&pkg001,"50B",ADDZERO(payin.F_payaddress));	/**付款人地址**/
		lv_set_tag(&pkg001,"CC5",ADDZERO(payin.F_cashno));	/**	收款人开户行号*/
		lv_set_tag(&pkg001,"59C",ADDZERO(payin.F_cashactno));/**收款人账号**/
		lv_set_tag(&pkg001,"59A",ADDZERO(payin.F_cashname));/**收款人名称 **/
		lv_set_tag(&pkg001,"59B",ADDZERO(payin.F_cashaddress));/**收款人地址**/
		lv_set_tag(&pkg001,"CEG",ADDZERO(payin.F_ywtype));	/**	业务种类 **/
		lv_set_tag(&pkg001,"72A",ADDZERO(payin.F_content));	/**	附言 **/

		vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
		/****下面拼附加字段****/
		if (memcmp(payin.F_txnum,TXNUM_HD,sizeof(payin.F_txnum))==0)
		{
			/****汇兑业务无附加数据格式***/
			lv_set_tag(&pkg001,"B40","00000000");	/**	附加数据长度**/
		}
		else if (memcmp(payin.F_txnum,TXNUM_WT,sizeof(payin.F_txnum))==0)
		{
			/****委托收款业务****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			tmpBuf[18]='\0';
			lv_set_tag(&pkg001,"B40","00000018");	/**	附加数据长度**/
			lv_set_tag(&pkg001,"72C",tmpBuf);		/**	附加数据 **/
		}
		else if (memcmp(payin.F_txnum,TXNUM_TSCF,sizeof(payin.F_txnum))==0)
		{
			/****托收承付业务****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			tmpBuf[91]='\0';
			lv_set_tag(&pkg001,"B40","00000091");	/**	附加数据长度**/
			lv_set_tag(&pkg001,"72C",tmpBuf);		/**	附加数据 **/
		}
		else if (memcmp(payin.F_txnum,TXNUM_GKDJ,sizeof(payin.F_txnum))==0)
		{
			/****国库贷记业务****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			tmpBuf[111]='\0';
			lv_set_tag(&pkg001,"B40","00000111");	/**	附加数据长度**/
			lv_set_tag(&pkg001,"72C",tmpBuf);		/**	附加数据 **/
		}
		else
		{
			/****其他未定义的附加数据域使用附加数据表来得到****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****没有附加数据****/
				lv_set_tag(&pkg001,"B40","00000000");	/**	附加数据长度**/
			}
			else
			{
				/****有附加数据****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg001,"B40",len_buf);	/**	附加数据长度**/
				lv_set_tag(&pkg001,"72C",tmpBuf);		/**	附加数据 **/
			}
		}
		vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
		lv_batch_end(&pkg001);
	}
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int)
	{
		sprintf( acErrMsg,"关闭Lv_pkgreg游标出错 %d ", g_reply_int);
		WRITEMSG
			sprintf( acErrMsg,"关闭Lv_pkgreg游标出错 %d ", g_reply_int);
		return(-1);
	}
	/*
	change by liuyue 20061228  26号的包，28号发送导致问题
	memcpy(cSysday,(char *)pcLvGetDay(),8);	
	*/
	sprintf(cSysday,"%8d",wd_lv_wbctl.pack_date);
	/** 包头内容 **/
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	lv_pkg_batch(&pkg001,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg001,"011",wd_lv_wbctl.pay_qs_no);	/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg001,"012",wd_lv_wbctl.cash_qs_no);/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg001,"30E",cSysday);	/** 包委托日期 **/
	/*****
	wd_lv_wbctl.pack_date = apatoi(lvca.lv_date, 8);
	****/

	vtcp_log("%s,%d pack_date===[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	lv_pkg_batch(&pkg001,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	/****Deleted by SSH,2005.11.24,密押由韦昭赋****/
	/****lv_pkg_batch(&pkg001,"C15",wd_lv_wbctl.packmy);** 包密押 **/
	/*********/
	/****Modified by SSH,2005.11.24,超过6位的数字是浮点型****/
	vtcp_log("[%s][%d]-----dtlcnt=[%d]\n",__FL__,wd_lv_wbctl.dtlcnt);

	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	vtcp_log("%s,%d tmpBuf===%s",__FL__,tmpBuf);
	lv_pkg_batch(&pkg001,"B63",tmpBuf);				/** 明细业务总笔数 **/

	memcpy(tmpBuf,"RMB",3);
	/****Modified by SSH,2005.11.24,金额以分为单位,15位***/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg001,"32B",tmpBuf);			 /** 明细业务总金额 **/

	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	lv_gen_pack(&pkg001);
	printf("%s,%d 抓贼1 \n",__FILE__,__LINE__);
	dm_show(&(pkg001.dm));
	/****注意: 把数据库更新操作放到发包之前,尽量减少不一致的可能****/
	/*sql_stat2[0]=STAT_WZSEND;*/
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lw_ind = '1' and pkgno = '%s' and lv_sts ='%s' and packid = '%s' and pack_date = %ld and pay_qs_no = '%s' ", PKGNO_PTDJ, sql_stat, wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, wd_lv_wbctl.pay_qs_no );
	WRITEMSG
	if (g_reply_int !=DB_OK)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg001);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);

	wd_lv_wbctl.stat[0] =PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg001);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	g_reply_int = lv_insSendList(&pkg001);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		WRITEMSG
		sprintf( acErrMsg,"更新往包控制表错误");
		lv_destroy(&pkg001);
		vtcp_log("[%s][%d]errmsg--[%s]\n",__FL__,acErrMsg);
		return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);

	dm_show(&(pkg001.dm));
	vtcp_log("%s,%d,发包\n",__FL__);
	iRc=iLvSendPackage(&pkg001);
	if (iRc)
	{
		lv_destroy(&pkg001);
		vtcp_log("[%s][%d]errmsg--[%s]\n",__FL__,acErrMsg);
		return(-1);
	}
	lv_destroy(&pkg001);
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	return(0);
}
/** 拼实时贷记发送包  pkg003 **/              
int lv_get_ssdj_tag(char *packday, char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[1024];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	char    sql_stat[2];
	char    sql_stat2[2];
	TLVPACKAGE pkg003;
	PAY_IN_AREA payin;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	double		TXAMT;
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
	memset(&payin, 0 , sizeof(payin)); /* add by LiuHuafeng 20060429 */
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ADDID,    '\0', sizeof(ADDID));
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

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));

	lv_load_tagifno();
	lv_create(&pkg003);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	vtcp_log("[%s][%d]packid=[%s]\n",__FL__,packid);
	wd_lv_wbctl.pack_date = apatoi( packday, 8);
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZCHECK2;
	sql_stat2[0]=STAT_WZSEND;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);
	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s' ", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错- %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;     /** 得到整数从double变到int **/

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, lv_date(wd_lv_wbctl.pack_date), sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg003.opeType = '1';	/** 小额 **/
	pkg003.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg003.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg003.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg003.CMTCode,PKGNO_SSDJ,3); 	/** 因为是 PKG003包**/
	memcpy(pkg003.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	vtcp_log("[%s][%d]-------packid=[%s],packday=[%s],sql_stat=[%s]\n",__FL__,packid,packday,sql_stat);
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "packid = '%s' and pack_date= %s and lv_sts = '%s' and pkgno = '%s' and lw_ind = '1'", packid, packday, sql_stat, PKGNO_SSDJ);
	if (g_reply_int != DB_OK)
	{ sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL DECLARE SSDJ1 CURSOR FOR 
	select txnum,orbrno,acbrno,wtday,orderno,txamt,payno,payactno,payname,payaddress,cashno,cashactno,cashname,
	cashaddress,ywtype,content,addid from lv_ssdjs where packid =:packid and packday=:packday and stat =:sql_stat;

	EXEC SQL OPEN SSDJ1;

	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	EXEC SQL FETCH SSDJ1 INTO :TXNUM,:ORBRNO,:ACBRNO,:WTDAY,:ORDERNO,:TXAMT,:PAYNO,:PAYACTNO,:PAYNAME,:PAYADDRESS,
	:CASHNO,:CASHACTNO,:CASHNAME,:CASHADDRESS,:YWTYPE,:CONTENT,:ADDID;*/

	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int !=DB_OK && g_reply_int !=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"没有可拼包的记录!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	/** 要素集内容 **/

	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memcpy(pkg003.CMTNo,"001",3);       /** 要素集号001***/
	lv_set_tag(&pkg003,"0BG",lv_pkgreg.txnum);	/** 业务类型号 **/
	lv_set_tag(&pkg003,"52A",lv_pkgreg.or_br_no);	/** 发起行行号 **/
	lv_set_tag(&pkg003,"58A",lv_pkgreg.ac_br_no);	/** 接收行行号 **/
	lv_set_tag(&pkg003,"30A", lv_date(lv_pkgreg.wt_date));	/**	委托日期 **/
	lv_set_tag(&pkg003,"0BC", lv_pkgreg.orderno);	/**	支付交易序号 **/
	TXAMT = lv_pkgreg.tx_amt*100.0;
	vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
	sprintf(tmpBuf,"%015.0f",TXAMT);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg003,"33G",tmpBuf);	/**	金额 **/
	lv_set_tag(&pkg003,"CC4", lv_pkgreg.pay_opn_br_no);	/**	付款人开户行行号 **/
	lv_set_tag(&pkg003,"50C", lv_pkgreg.pay_ac_no);	/**	付款人账号 **/
	lv_set_tag(&pkg003,"50A", lv_pkgreg.pay_name);	/**	付款人名称 **/
	lv_set_tag(&pkg003,"50B", lv_pkgreg.pay_addr);	/**	付款人地址 **/
	lv_set_tag(&pkg003,"CC5", lv_pkgreg.cash_opn_br_no);	/**	收款人开户行行号 **/
	lv_set_tag(&pkg003,"59C", lv_pkgreg.cash_ac_no);	/**	收款人账号 **/
	lv_set_tag(&pkg003,"59A", lv_pkgreg.cash_name);	/**	收款人名称 **/
	lv_set_tag(&pkg003,"59B", lv_pkgreg.cash_addr);	/**	收款人地址 **/
	lv_set_tag(&pkg003,"CEG", lv_pkgreg.lv_yw_ind);	/**	业务种类 **/
	lv_set_tag(&pkg003,"72A", lv_pkgreg.lv_brf);	/**	附言 **/
	/** 拼附加数据 **/
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	apitoa(lv_pkgreg.wt_date,8,payin.F_wtday);
	memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
	memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
	memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(payin.T_addid));

	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	vtcp_log("[%s][%d]==payin.T_addid=[%.8s]\n",__FL__,payin.T_addid);
	vtcp_log("[%s][%d]==payin.T_addid=[%c]\n",__FL__,payin.T_addid[0]);
	if (payin.T_addid[0]=='\0')
	{
		lv_set_tag(&pkg003,"B40","00000000");   /** 附加数据长度**/
		vtcp_log("[%s][%d]==payin.T_addid=[%s]\n",__FL__,payin.T_addid);
	}
	else
	{
		char len_buf[9];
		vtcp_log("[%s][%d]=payin.T_addid=[%s]\n",__FL__,payin.T_addid);
		iLvGetAddit(&payin,"000",tmpBuf,0);
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
		sprintf(len_buf,"%08d",strlen(tmpBuf));
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
		lv_set_tag(&pkg003,"B40",len_buf);  /** 附加数据长度**/
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
		lv_set_tag(&pkg003,"72C",tmpBuf);       /** 附加数据 **/
		vtcp_log("[%s][%d]tmpBuf=[%s]\n\n\n",__FL__,tmpBuf);
	}
	lv_batch_end(&pkg003);
	g_reply_int= Lv_pkgreg_Clo_Sel();
	if ( g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*	EXEC SQL CLOSE SSDJ1;*/

	/** 包头内容 **/
	lv_pkg_batch(&pkg003,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg003,"011",wd_lv_wbctl.pay_qs_no);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg003,"012",wd_lv_wbctl.cash_qs_no);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg003,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg003,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	/** lv_pkg_batch(&pkg003,"C15",wd_lv_wbctl.packmy);	** 包密押 **/

	/** apitoa(wd_lv_wbctl.dtlcnt,sizeof(Cnt)-1,Cnt); **/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg003,"B63",Cnt);	/** 明细业务总笔数 **/

	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg003,"32B",tmpBuf);	/** 明细业务总金额 **/

	/** lv_pkg_batch(&pkg003,"72D","");	** 包附加数据 **/
	vtcp_log("[%s][%d]==========\n",__FL__);

	vtcp_log("[%s][%d]==========\n",__FL__);
	lv_gen_pack(&pkg003);
	vtcp_log("[%s][%d]==========\n",__FL__);
	/**
	dm_show(&(pkg003.dm));
	**/
	vtcp_log("[%s][%d]==========\n",__FL__);

	/** 更新状态为已发送 **/
	vtcp_log("%s,%d,***************************\n",__FL__);
	vtcp_log("stat2=[%s]\n,packid=[%s]\n,packday=[%s]\n,stat=[%s]\n",
		sql_stat2,packid,packday,sql_stat);

	g_reply_int = lv_Upd_Pkgreg_Stat(sql_stat2[0], "lw_ind = '1' and pkgno = '%s' and packid = '%s' and pack_date= '%s'  and lv_sts = '%s'", PKGNO_SSDJ, packid, packday, sql_stat);
	if (g_reply_int !=DB_OK && g_reply_int != DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"没有可拼包的记录!");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] =PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = lv_insSendList(&pkg003);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	/** 发包 **/
	vtcp_log("%s,%d,发包\n",__FL__);
	if (iRc=iLvSendPackage(&pkg003))
	{
		lv_destroy(&pkg003);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg003);
	return(0);

}
/** 拼实时借记发送包 pkg004 **/              
int lv_get_ssjj_tag(char *packday,char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[1024*8];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	char    sql_stat[2];	
	char    sql_stat2[2];	
	TLVPACKAGE pkg004;
	PAY_IN_AREA payin;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	double		TXAMT;
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

	memset(&payin, 0 , sizeof(payin));/* add by lhf0423 */
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ADDID,    '\0', sizeof(ADDID));
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

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(sql_stat, '\0', sizeof(sql_stat));	
	memset(sql_stat2, '\0', sizeof(sql_stat2));	

	lv_load_tagifno();
	lv_create(&pkg004);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8);
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZLR;	
	sql_stat2[0]=STAT_WZSEND;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);
	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;     /** 得到整数从double变到int **/

	/** 给游标需要的字段赋值 **
	memcpy(Txday,wd_lv_wbctl.pack_date, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);
	***********/

	/** 开始拼报文 **/
	pkg004.opeType = '1';	/** 小额 **/
	pkg004.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg004.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg004.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg004.CMTCode,PKGNO_SSJJ,3);	/** 因为是 PKG004包**/
	memcpy(pkg004.CMTNo,"001",3);       /***要素集号001***/
	pkg004.paymPRI='0';                 /***优先级***/
	memcpy(pkg004.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lv_sts = '%s' and lw_ind = '1'", PKGNO_SSJJ, packid, packday, sql_stat);
	if (g_reply_int!=DB_OK )
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	if (wd_dtlcnt!=0)                       /**不用循环**/
	{
		vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");

			lv_destroy(&pkg004);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"没有可拼包的记录!");

			lv_destroy(&pkg004);
			WRITEMSG
				return(-1);
		}
		/** 要素集内容 **/
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORDERNO, lv_pkgreg.orderno);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(PAYNO, lv_pkgreg.pay_opn_br_no);
		strcpy(PAYACTNO, lv_pkgreg.pay_ac_no);
		strcpy(PAYNAME, lv_pkgreg.pay_name);
		strcpy(PAYADDRESS, lv_pkgreg.pay_addr);
		strcpy(CASHNO, lv_pkgreg.cash_opn_br_no);
		strcpy(CASHACTNO, lv_pkgreg.cash_ac_no);
		strcpy(CASHNAME, lv_pkgreg.cash_name);
		strcpy(CASHADDRESS, lv_pkgreg.cash_addr);
		strcpy(YWTYPE, lv_pkgreg.lv_yw_ind);
		strcpy(CONTENT, lv_pkgreg.lv_brf);

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		lv_set_tag(&pkg004,"0BG",TXNUM);	/** 业务类型号 **/
		lv_set_tag(&pkg004,"52A",ORBRNO);	/** 发起行行号 **/
		lv_set_tag(&pkg004,"58A",ACBRNO);	/** 接收行行号 **/
		lv_set_tag(&pkg004,"30A",WTDAY);	/**	委托日期 **/
		lv_set_tag(&pkg004,"0BC",ORDERNO);	/**	支付交易序号 **/
		TXAMT = lv_pkgreg.tx_amt*100.0;
		sprintf(tmpBuf,"%015.0f",TXAMT);
		vtcp_log("[%s][%d]====TXAMT=[%f]\n",__FL__,TXAMT);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg004,"33G",tmpBuf);	/**	金额 **/
		lv_set_tag(&pkg004,"CC4",PAYNO);	/**	付款人开户行行号 **/
		lv_set_tag(&pkg004,"50C",PAYACTNO);	/**	付款人账号 **/
		lv_set_tag(&pkg004,"50A",PAYNAME);	/**	付款人名称 **/
		lv_set_tag(&pkg004,"50B",PAYADDRESS);	/**	付款人地址 **/
		lv_set_tag(&pkg004,"CC5",CASHNO);	/**	收款人开户行行号 **/
		lv_set_tag(&pkg004,"59C",CASHACTNO);	/**	收款人账号 **/
		lv_set_tag(&pkg004,"59A",CASHNAME);	/**	收款人名称 **/
		lv_set_tag(&pkg004,"59B",CASHADDRESS);	/**	收款人地址 **/
		lv_set_tag(&pkg004,"CEG",YWTYPE);	/**	业务种类 **/
		lv_set_tag(&pkg004,"72A",CONTENT);	/**	附言 **/
		/** 拼附加数据 **/

		/**   实时扣税,通兑都有数据域，暂时列入此列   ***/

		memset(tmpBuf,'\0',sizeof(tmpBuf));
		apitoa(lv_pkgreg.wt_date,8,payin.F_wtday);
		memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.T_addid,lv_pkgreg.addid,sizeof(payin.T_addid));
		/****HSYX BEGIN:拼001-005的附加域****/
		if (payin.T_addid[0]=='\0')
		{
			lv_set_tag(&pkg004,"B40","00000000");   /** 附加数据长度**/
		}
		else
		{
			iLvGetAddit(&payin,"000",tmpBuf,0);
		}
		if (tmpBuf[0]!='\0' && memcmp(lv_pkgreg.txnum,"30103",sizeof(lv_pkgreg.txnum)-1)!=0)
		{
			/****有附加数据****/
			char len_buf[9];
			memset(len_buf, 0 , sizeof(len_buf));
			sprintf(len_buf,"%08d",strlen(tmpBuf));
			lv_set_tag(&pkg004,"B40",len_buf);  /** 附加数据长度**/
			lv_set_tag(&pkg004,"72C",tmpBuf);       /** 附加数据 **/
			vtcp_log("[%s][%d]==========tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
		}
		else if (payin.T_addid[0]!='\0')
		{
			vtcp_log("[%s][%d]=============\n",__FILE__,__LINE__);
			/****没有标准000附加数据****/
			/****HSYX BEGIN:拼001-005的附加域****/
			int sIndex=0;
			int sAddLen=0;
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			for (sIndex=1;sIndex<=5;sIndex++)
			{
				char pcTagName[4];
				sprintf(pcTagName,"%03d",sIndex);
				iLvGetAddit(&payin,pcTagName,tmpBuf+sAddLen,0);
				printf("%s,%d,Gethere,第[%d]个附加域\n[%s]\n",__FILE__,__LINE__,sIndex,tmpBuf);
				if (tmpBuf[sAddLen]=='\0')
				{
					/****无此附加域，附加域循环结束****/
					if (sIndex==2)
					{
						/****背书人列表****/
						strcat(tmpBuf+sAddLen,"00");
						sAddLen=strlen(tmpBuf);
					}
					else
					{
						break;
					}
				}
				else
				{
					/****有此附加域，拼附加域****/
					sAddLen=strlen(tmpBuf);
				}
				vtcp_log("[%s][%d]==========tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
			}
			if (tmpBuf[0]=='\0')
			{
				lv_set_tag(&pkg004,"B40","00000000");   /** 附加数据长度**/
			}else
			{
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg004,"B40",len_buf);  /** 附加数据长度**/
				lv_set_tag(&pkg004,"72C",tmpBuf);       /** 附加数据 **/
				vtcp_log("[%s][%d]==========tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
			}
			/****HSYX END:拼001-005的附加域****/
		}
		lv_batch_end(&pkg004);
	}
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE SSJJ1;*/

	/** 包头内容 **/
	lv_pkg_batch(&pkg004,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg004,"011",wd_lv_wbctl.pay_qs_no);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg004,"012",wd_lv_wbctl.cash_qs_no);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg004,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg004,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	/** lv_pkg_batch(&pkg004,"C15",wd_lv_wbctl.packmy);	** 包密押 **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg004,"B63",Cnt);	/** 明细业务总笔数 **/

	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg004,"32B",tmpBuf);	/** 明细业务总金额 **/

	/**    lv_pkg_batch(&pkg004,"72D","");	** 包附加数据 **/
	vtcp_log("[%s][%d]==========\n",__FL__);

	lv_gen_pack(&pkg004);
	/**
	dm_show(&(pkg004.dm));
	**/
	/** 更新状态为已发送 **/
	/** 也更新发起行为其清算行，否则来应答时锁不住表 dxy 20060206 **/
	g_reply_int = lv_Upd_Pkgreg_Stat(sql_stat2[0], "lw_ind = '1' and pkgno = '%s' and packid = '%s' and pack_date = %s and lv_sts = '%s'", PKGNO_SSJJ, packid, packday, sql_stat);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"没有可拼包的记录!");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int )
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = lv_insSendList(&pkg004); 
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	/** 发包 **/
	vtcp_log("%s,%d,发包\n",__FL__);
	if (iRc=iLvSendPackage(&pkg004))
	{
		lv_destroy(&pkg004);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg004);
	return(0);

}
/****Add by SSH,2005.11.28,定期拼包范例****/
int lv_get_dqdj_tag_ssh(char *packday,char *packid)
{
	char sql_stat[2];
	char sql_stat2[2];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_orderno[9];
	int iRc=0;
	long txcnt=0;/****明细交易笔数(表中)****/
	int listcnt=0;/****收款人清单的条数****/
	char preActno[33];/****保存前一笔的付款帐号****/
	char preWtday[9];/****保存前一笔的委托日期****/
	char tmpBuf[1024];
	PAY_IN_AREA payin;

	TLVPACKAGE pkg005;
	static struct lv_wbctl_c      wd_lv_wbctl;
	iRc=0;
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(&pkg005, '\0', sizeof(pkg005));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wtday, '\0', sizeof(sql_wtday));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	lv_create(&pkg005);
	/** 游标所用变量 **/
	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid, packid, sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZCHECK2;
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",
		__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",
		__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int= Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");     

		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",
			__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"包记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");
		WRITEMSG
		lv_destroy(&pkg005);
		return(-1);
	}
	txcnt = wd_lv_wbctl.dtlcnt;

	/** 给游标需要的字段赋值 **/
	/** 开始查找对应明细表中的记录 **/
	/*=====================================================================================
	EXEC SQL DECLARE DQDJ1 CURSOR FOR 
	select orbrno,wtday,orderno
	from lv_dqdjs 
	where 	payqsactno=:wd_lv_wbctl.pay_qs_no and 
	packid =:wd_lv_wbctl.packid and 
	packday=:wd_lv_wbctl.pack_date and 
	stat=:sql_stat;	
	EXEC SQL OPEN DQDJ1;
	*===================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_DQDJ, packid, packday, sql_stat);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		WRITEMSG
		return(-1);
	}
	/** 开始拼报文 **/
	/****Modified by SSH,2005.11.24,PKG号****/
	pkg005.opeType = '1';	/** 小额 **/
	pkg005.pkgType = 1;			/** 是pkg包 **/
	memcpy(pkg005.CMTCode,PKGNO_DQDJ,3);	/** PKG号 **/
	memcpy(pkg005.workDate,pcLvGetDay_work(),8);/**注意:是小额日期不是综合业务日期*/
	/*******************************************/
	listcnt=0;
	memset(preActno,0x15,sizeof(preActno)-1);
	preActno[sizeof(preActno)-1]='\0';
	memset(preWtday,0x15,sizeof(preWtday)-1);
	preWtday[sizeof(preWtday)-1]='\0';
	for (;txcnt>0;txcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++txcnt=[%d]\n",
			__FL__,txcnt);
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int!=DB_OK)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",
				__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");

			lv_destroy(&pkg005);
			WRITEMSG
				return(-1);
		}
		memset(&payin, '\0', sizeof(payin));
		memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		memcpy(payin.F_wtday,lv_date(lv_pkgreg.wt_date),sizeof(payin.F_wtday));
		memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
		memcpy(payin.F_pkgno,PKGNO_DQDJ,sizeof(payin.F_pkgno));
		payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
		payin.T_lw_ind[0]=lv_pkgreg.lw_ind[0];
		LV_CP_PAY_IN_FIELD(&payin,(&lv_pkgreg));
		iLvZipAll(&payin);
		iLvResetPayin(&payin);	
		/** 要素集内容 **/
		if (memcmp(payin.F_payactno,preActno,sizeof(preActno)-1)==0 &&
			memcmp(payin.F_wtday,preWtday,sizeof(preWtday)-1)==0)
		{
			/****延续上一笔业务,只改变收款人清单****/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_orderno));/**	支付交易序号 **/
			sprintf(tmpBuf,"%08d",listcnt+1);/****收款信息编号****/
			lv_set_tag_multi(&pkg005,"B81",tmpBuf);
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_acbrno));/*收款行*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_cashno));/**收款人开户行*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashactno));/**收款人帐号*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashname));/*收款人名称*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashaddress));/**收款人地址*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_txamt));/**	金额 **/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_content));/**附言 **/

			/****下面拼附加字段****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****没有附加数据****/
				lv_set_tag_multi(&pkg005,"B81","00000000");	/**	附加数据长度**/
			}else
			{
				/****有附加数据****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag_multi(&pkg005,"B81",len_buf);	/**	附加数据长度**/
				lv_set_tag_multi(&pkg005,"B81",tmpBuf);		/**	附加数据 **/
			}
			listcnt++;/****收款清单明细笔数加1****/
		}
		else
		{
			listcnt=0;
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			memcpy(pkg005.CMTNo,"003",3);		/***要素集号003***/
			pkg005.paymPRI='0';					/***优先级***/
			memcpy(preActno,payin.F_payactno,sizeof(preActno)-1);
			memcpy(preWtday,payin.F_wtday,sizeof(preWtday)-1);
			lv_set_tag(&pkg005,"0BG",ADDZERO(payin.F_txnum));/** 业务类型号 **/
			lv_set_tag(&pkg005,"CCM",ADDZERO(payin.F_orbrno));/** 付款行行号**/
			lv_set_tag(&pkg005,"30A",ADDZERO(payin.F_wtday));/**	委托日期 **/
			lv_set_tag(&pkg005,"CC4",ADDZERO(payin.F_payno));/**付款人开户行*/
			lv_set_tag(&pkg005,"50C",ADDZERO(payin.F_payactno));/**付款人账号*/
			lv_set_tag(&pkg005,"50A",ADDZERO(payin.F_payname));/**付款人名称**/
			lv_set_tag(&pkg005,"50B",ADDZERO(payin.F_payaddress));/**付款地址*/
			lv_set_tag(&pkg005,"CEG",ADDZERO(payin.F_ywtype));	/**	业务种类 **/
			sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);/****收款清单个数****/
			lv_set_tag(&pkg005,"B61",tmpBuf);		

			lv_set_tag(&pkg005,"B81",ADDZERO(payin.F_orderno));/**	支付交易序号 **/
			sprintf(tmpBuf,"%08d",listcnt+1);/****收款信息编号****/
			lv_set_tag_multi(&pkg005,"B81",tmpBuf);
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_acbrno));/*收款行*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_cashno));/**收款人开户行*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashactno));/**收款人帐号*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashname));/*收款人名称*/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_cashaddress));/**收款人地址*/
			lv_set_tag_multi(&pkg005,"B81",ADDZERO(payin.F_txamt));/**	金额 **/
			lv_set_tag_multi(&pkg005,"B81",ADDSPZ(payin.F_content));/**附言 **/

			/****下面拼附加字段****/
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****没有附加数据****/
				lv_set_tag_multi(&pkg005,"B81","00000000");	/**	附加数据长度**/
			}else
			{
				/****有附加数据****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag_multi(&pkg005,"B81",len_buf);	/**	附加数据长度**/
				lv_set_tag_multi(&pkg005,"B81",tmpBuf);		/**	附加数据 **/
			}
			listcnt++;/****收款清单明细笔数加1****/
		}
	}
	lv_batch_end(&pkg005);
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
		return(-1);
	}
	/****把最后一笔付款记录封起来****/

	/** 包头内容 **/
	vtcp_log("[%s][%d]lv_wbctl.pack_date=[%ld]",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]lvca.lv_date=[%s]",__FILE__,__LINE__,lvca.lv_date);
	lv_pkg_batch(&pkg005,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg005,"011",wd_lv_wbctl.pay_qs_no);	/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg005,"012",wd_lv_wbctl.cash_qs_no);/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg005,"30E",packday);	/** 包委托日期 **/
	/**
	wd_lv_wbctl.pack_date = apatoi(lvca.lv_date, 8);
	**/
	lv_pkg_batch(&pkg005,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	/****Deleted by SSH,2005.11.24,密押由韦昭赋****/
	/****lv_pkg_batch(&pkg005,"C15",wd_lv_wbctl.packmy);** 包密押 **/
	/*********/
	/****Modified by SSH,2005.11.24,超过6位的数字是浮点型****/
	sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg005,"B63",tmpBuf);				/** 明细业务总笔数 **/

	memcpy(tmpBuf,"RMB",3);
	/****Modified by SSH,2005.11.24,金额以分为单位,15位***/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg005,"32B",tmpBuf);			 /** 明细业务总金额 **/
	lv_gen_pack(&pkg005);
	/****注意: 把数据库更新操作放到发包之前,尽量减少不一致的可能****/
	sql_stat2[0]=STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s' and pay_qs_no = '%s' " , sql_stat, packid, packday, PKGNO_DQDJ, wd_lv_wbctl.pay_qs_no);

	if (g_reply_int!=DB_OK)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");
		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	g_reply_int = Lv_wbctl_Clo_Upd();
	if (g_reply_int)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = lv_insSendList(&pkg005);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");
		WRITEMSG
			lv_destroy(&pkg005);
		return(-1);
	}
	vtcp_log("%s,%d,发包\n",__FL__);
	iRc=iLvSendPackage(&pkg005);
	if (iRc)
	{
		lv_destroy(&pkg005);
		WRITEMSG
		return(-1);
	}
	lv_destroy(&pkg005);
	return(0);

}
int lv_get_dqjj_tag(char *packday,char *packid)
{
	vtcp_log("[%s][%d]===============w\n", __FL__);
	char sql_stat[2];
	char sql_stat2[2];
	char sql_orbrno[13];
	char sql_wtday[9];
	char sql_orderno[9];
	int iRc=0;
	int	iSocketID=0;
	long txcnt=0;/****明细交易笔数(表中)****/
	int listcnt=0;/****付款人清单的条数****/
	char preActno[33];/****保存前一笔的收款帐号****/
	char preWtday[9];/****保存前一笔的委托日期****/
	char tmpBuf[1024];
	char respCode[9];
	char respBuf[100];
	char pcDfxyNo[60];
	char cHzDate[3];
	memset(cHzDate, 0 , sizeof(cHzDate));

	vtcp_log("[%s][%d]===============w\n", __FL__);
	PAY_IN_AREA payin;

	TLVPACKAGE pkg006;
	static struct lv_wbctl_c      wd_lv_wbctl;
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(&pkg006, '\0', sizeof(pkg006));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wtday, '\0', sizeof(sql_wtday));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	lv_create(&pkg006);
	/** 游标所用变量 **/
	wd_lv_wbctl.pack_date = apatoi(packday, 8);
	memcpy(wd_lv_wbctl.packid, packid, sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZLR;
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n", __FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n", __FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]=w\n", __FL__);
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");

		lv_destroy(&pkg006);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",
			__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"包记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");
		WRITEMSG
			lv_destroy(&pkg006);
		return(-1);
	}
	txcnt = wd_lv_wbctl.dtlcnt;

	vtcp_log("[%s][%d]===============w\n", __FL__);
	/** 给游标需要的字段赋值 **/
	/** 开始查找对应明细表中的记录 **/
	/*==========================================================================================
	EXEC SQL DECLARE DQJJ1 CURSOR FOR 
	select orbrno,wtday,orderno
	from lv_dqjjs 
	where 	payqsactno=:wd_lv_wbctl.pay_qs_no and 
	packid =:wd_lv_wbctl.packid and 
	packday=:wd_lv_wbctl.pack_date and 
	stat=:sql_stat;	
	EXEC SQL OPEN DQJJ1;
	==========================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_DQJJ, packid, packday, sql_stat);
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/** 开始拼报文 **/
	/****Modified by SSH,2005.11.24,PKG号****/
	pkg006.opeType = '1';	/** 小额 **/
	pkg006.pkgType = 1;			/** 是pkg包 **/
	memcpy(pkg006.CMTCode,PKGNO_DQJJ,3);	/** PKG号 **/
	memcpy(pkg006.workDate,pcLvGetDay_work(),8);/**注意:是小额日期不是综合业务日期*/
	/*******************************************/
	listcnt=0;
	memset(preActno,0x15,sizeof(preActno)-1);
	preActno[sizeof(preActno)-1]='\0';
	memset(preWtday,0x15,sizeof(preWtday)-1);
	preWtday[sizeof(preWtday)-1]='\0';
	for (;txcnt>0;txcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++txcnt=[%d]\n",
			__FL__,txcnt);
		/*EXEC SQL FETCH DQJJ1 INTO :sql_orbrno,:sql_wtday,:sql_orderno;*/
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int != DB_OK)
		{
			vtcp_log("%s,%d,数据库错误,未找到记录sqlcode=%d\n",
				__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误,未找到记录!");
			WRITEMSG
				lv_destroy(&pkg006);
			return(-1);
		}
		memset(&payin, '\0', sizeof(payin));
		memcpy(payin.F_orbrno,lv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		memcpy(payin.F_wtday,lv_date(lv_pkgreg.wt_date),sizeof(payin.F_wtday));
		memcpy(payin.F_orderno,lv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
		memcpy(payin.F_pkgno,PKGNO_DQJJ,sizeof(payin.F_pkgno));
		payin.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
		payin.T_lw_ind[0]=lv_pkgreg.lw_ind[0];
		LV_CP_PAY_IN_FIELD(&payin,(&lv_pkgreg));
		iLvZipAll(&payin);
		iLvResetPayin(&payin);	
		/** 要素集内容 **/
		if (	listcnt==0)
		{
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			memcpy(pkg006.CMTNo,"004",3);		/***要素集号004***/
			pkg006.paymPRI='0';					/***优先级***/
			memcpy(preActno,payin.F_cashactno,sizeof(preActno)-1);
			memcpy(preWtday,payin.F_wtday,sizeof(preWtday)-1);
			lv_set_tag(&pkg006,"0BG",ADDZERO(payin.F_txnum));/** 业务类型号 **/
			lv_set_tag(&pkg006,"CCN",ADDZERO(payin.F_orbrno));/** 收款行行号**/
			lv_set_tag(&pkg006,"30A",ADDZERO(payin.F_wtday));/**	委托日期 **/
			lv_set_tag(&pkg006,"CC5",ADDZERO(payin.F_cashno));/**收款人开户行*/
			lv_set_tag(&pkg006,"59C",ADDZERO(payin.F_cashactno));/*收款人账号*/
			lv_set_tag(&pkg006,"59A",ADDZERO(payin.F_cashname));/*收款人名称**/
			lv_set_tag(&pkg006,"59B",ADDZERO(payin.F_cashaddress));/*收款地址*/
			lv_set_tag(&pkg006,"CEG",ADDZERO(payin.F_ywtype));	/**	业务种类 **/
			sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);/****付款清单个数****/
			lv_set_tag(&pkg006,"B62",tmpBuf);		
			lv_set_tag(&pkg006,"B82",ADDZERO(payin.F_orderno));/**	支付交易序号 **/
		}
		if (listcnt>0)
		{
			lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_orderno));/**	支付交易序号 **/
		}
		sprintf(tmpBuf,"%08d",listcnt+1);/****付款信息编号****/
		lv_set_tag_multi(&pkg006,"B82",tmpBuf);
		lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_acbrno));/*付款行*/
		lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_payno));/**付款人开户行*/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_payactno));/**付款人帐号*/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_payname));/*付款人名称*/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_payaddress));/**付款人地址*/
		lv_set_tag_multi(&pkg006,"B82",ADDZERO(payin.F_txamt));/**	金额 **/
		/****多方协议号****/
		memset(pcDfxyNo,'\0',sizeof(pcDfxyNo));
		iLvGetAddit(&payin,"XYH",pcDfxyNo,sizeof(pcDfxyNo));
		if (pcDfxyNo[0]=='\0')
		{
			memset(pcDfxyNo,'0',sizeof(pcDfxyNo));
		}
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(pcDfxyNo));/**多方协议号 **/
		/****下面拼附加字段****/
		lv_set_tag_multi(&pkg006,"B82",ADDSPZ(payin.F_content));/**附言 **/
		memset(tmpBuf,'\0',sizeof(tmpBuf));
		iLvGetAddit(&payin,"000",tmpBuf,0);
		if (tmpBuf[0]=='\0')
		{
			/****没有附加数据****/
			lv_set_tag_multi(&pkg006,"B82","00000000");	/**	附加数据长度**/
		}else
		{
			/****有附加数据****/
			char len_buf[9];
			sprintf(len_buf,"%08d",strlen(tmpBuf));
			lv_set_tag_multi(&pkg006,"B82",len_buf);	/**	附加数据长度**/
			lv_set_tag_multi(&pkg006,"B82",tmpBuf);		/**	附加数据 **/
		}
		listcnt++;/****付款清单明细笔数加1****/
	}
	/****把最后一笔收款记录封起来****/
	lv_batch_end(&pkg006);
	/*	EXEC SQL CLOSE DQJJ1;*/
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		WRITEMSG
			return(-1);
	}
	/** 包头内容 **/
	vtcp_log("[%s][%d]lv_wbctl.pack_date=[%ld]",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]lvca.lv_date=[%s]",__FILE__,__LINE__,lvca.lv_date);
	lv_pkg_batch(&pkg006,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg006,"011",wd_lv_wbctl.pay_qs_no);	/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg006,"012",wd_lv_wbctl.cash_qs_no);/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg006,"30E",packday);	/** 包委托日期 **/
	/**
	wd_lv_wbctl.pack_date = apatoi(lvca.lv_date, 8);
	**/
	lv_pkg_batch(&pkg006,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	sprintf(cHzDate,"%02d",wd_lv_wbctl.hz_date);
	lv_pkg_batch(&pkg006,"BS6",cHzDate);	/** 回执期限（天数） **/
	/****Deleted by SSH,2005.11.24,密押由韦昭赋****/
	/****lv_pkg_batch(&pkg006,"C15",wd_lv_wbctl.packmy);** 包密押 **/
	/*********/
	/****Modified by SSH,2005.11.24,超过6位的数字是浮点型****/
	sprintf(tmpBuf,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg006,"B63",tmpBuf);				/** 明细业务总笔数 **/

	memcpy(tmpBuf,"RMB",3);
	/****Modified by SSH,2005.11.24,金额以分为单位,15位***/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg006,"32B",tmpBuf);			 /** 明细业务总金额 **/
	lv_gen_pack(&pkg006);
	/****注意: 把数据库更新操作放到发包之前,尽量减少不一致的可能****/
	sql_stat2[0]=STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s' and pay_qs_no = '%s'" , sql_stat, packid, packday, PKGNO_DQJJ, wd_lv_wbctl.pay_qs_no);

	if (g_reply_int != DB_OK)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg006);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");
		WRITEMSG
			lv_destroy(&pkg006);
		return(-1);
	}
	g_reply_int= Lv_wbctl_Clo_Upd();
	if (g_reply_int )
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int= lv_insSendList(&pkg006);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");
		WRITEMSG
			lv_destroy(&pkg006);
		return(-1);
	}
	vtcp_log("%s,%d,发包\n",__FL__);
	iRc=iLvSendPackage(&pkg006);
	if (iRc !=DB_OK)
	{
		lv_destroy(&pkg006);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg006);
	return(0);

}
/** 拼贷记退汇发送包 **/
/** 传入参数为包id **/
int lv_get_djth_tag(char *packday,char *packid)
{
	int     ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[1024];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg007;
	PAY_IN_AREA pay_in;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	char    OTXNUM[6];
	char    OORBRNO[13];
	char    OACBRNO[13];
	char    OWTDAY[9];
	char    OORDERNO[9];
	double	OTXAMT;
	char    OCASHNO[13];
	char    OCASHACTNO[33];
	char    OCASHNAME[61];
	char    OCASHADDRESS[61];
	char    OPAYNO[13];
	char    OPAYACTNO[33];
	char    OPAYNAME[61];
	char    OPAYADDRESS[61];	
	char    RETCODE[3];
	char	CONTENT[61];
	char	YWTYPE[13];
	char    Stat[2];
	char    Stat2[2]; 

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(OTXNUM,  '\0', sizeof(OTXNUM));
	memset(OORBRNO,  '\0', sizeof(OORBRNO));
	memset(OACBRNO,  '\0', sizeof(OACBRNO));
	memset(OWTDAY,  '\0', sizeof(OWTDAY));
	memset(OORDERNO,  '\0', sizeof(OORDERNO));
	memset(OCASHNO,  '\0', sizeof(OCASHNO));
	memset(OCASHACTNO,  '\0', sizeof(OCASHACTNO));
	memset(OCASHNAME,  '\0', sizeof(OCASHNAME));
	memset(OCASHADDRESS,  '\0', sizeof(OCASHADDRESS));
	memset(OPAYNO,    '\0', sizeof(OPAYNO));
	memset(OPAYACTNO, '\0', sizeof(OPAYACTNO));
	memset(OPAYNAME,  '\0', sizeof(OPAYNAME));
	memset(OPAYADDRESS,'\0', sizeof(OPAYADDRESS));
	memset(RETCODE,'\0', sizeof(RETCODE));
	memset(CONTENT,	 '\0', sizeof(CONTENT));
	memset(YWTYPE,	 '\0', sizeof(YWTYPE));
	memset(Stat,   '\0', sizeof(Stat));
	memset(Stat2,   '\0', sizeof(Stat2));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	lv_load_tagifno();
	lv_create(&pkg007);

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld  and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if ( g_reply_int!= DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		WRITEMSG
			lv_destroy(&pkg007);
		return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");
		WRITEMSG
			lv_destroy(&pkg007);
		return(-1);
	}
	wd_dtlcnt =wd_lv_wbctl.dtlcnt;

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg007.opeType = '1';   /** 小额 **/
	pkg007.pkgType = 1;       /** 是pkg包 **/
	memcpy(pkg007.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg007.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg007.CMTCode,PKGNO_DJTH,3); /** 因为是PKG007包**/
	memcpy(pkg007.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	Stat[0] = STAT_WZCHECK2;
	vtcp_log("[%s][%d]========packday=[%s],packid=[%s],stat=[%s]\n",__FL__,packday,packid,Stat);
	/*==========================================================================================
	EXEC SQL DECLARE DJTH1 CURSOR FOR
	select txnum,oacbrno,oorbrno,wtday,orderno,otxnum,oorbrno,oacbrno,owtday,oorderno,otxamt,ocashno,
	ocashactno,ocashname,ocashaddress,opayno,opayactno,opayname,opayaddress,
	retcode,content from lv_djths where packid =:packid  and packday =:packday  and sts =:Stat;

	EXEC SQL OPEN DJTH1;
	==========================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_DJTH, packid, packday, Stat);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{
		vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		/*========================================
		EXEC SQL FETCH DJTH1 INTO :TXNUM,:ORBRNO,:ACBRNO,:WTDAY,:ORDERNO,
		:OTXNUM,:OORBRNO,:OACBRNO,:OWTDAY,:OORDERNO,:OTXAMT,:OCASHNO,
		:OCASHACTNO,:OCASHNAME,:OCASHADDRESS,:OPAYNO,:OPAYACTNO,:OPAYNAME,
		:OPAYADDRESS,:RETCODE,:CONTENT;
		*========================================*/
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");

			lv_destroy(&pkg007);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"数据库错误");

			lv_destroy(&pkg007);
			WRITEMSG
				return(-1);
		}
		/** 要素集内容 **/

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		memcpy(pkg007.CMTNo,"002",3);       /***要素集号002***/
		pkg007.paymPRI='0';                 /***优先级***/
		lv_set_tag(&pkg007,"0BG",lv_pkgreg.txnum);	/** 业务类型号 **/
		lv_set_tag(&pkg007,"52A",lv_pkgreg.or_br_no);	/** 发起行行号 **/
		lv_set_tag(&pkg007,"58A",lv_pkgreg.ac_br_no);	/** 接收行行号 **/
		lv_set_tag(&pkg007,"30A",lv_date(lv_pkgreg.wt_date));	/**	委托日期 **/
		lv_set_tag(&pkg007,"0BC",lv_pkgreg.orderno);	/**	支付交易序号 **/
		lv_set_tag(&pkg007,"0BH",lv_pkgreg.o_txnum);	/**	原交易类型编号 **/
		lv_set_tag(&pkg007,"CC1",lv_pkgreg.o_or_br_no);	/**	原发起行行号 **/
		lv_set_tag(&pkg007,"CC2",lv_pkgreg.or_br_no);	/**	原接受行行号 **/
		lv_set_tag(&pkg007,"051",lv_date(lv_pkgreg.o_wt_date));	/**	原委托日期 **/
		lv_set_tag(&pkg007,"005",lv_pkgreg.o_orderno);/**	原支付交易序号 **/
		vtcp_log("[%s][%d]====OTXAMT=[%.2f]\n",__FL__,OTXAMT);
		OTXAMT = lv_pkgreg.tx_amt*100;
		sprintf(tmpBuf,"%015.0f",OTXAMT);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg007,"33S",tmpBuf);	/**	原金额 **/
		lv_set_tag(&pkg007,"CCK",lv_pkgreg.pay_opn_br_no);/**	原收款人开户行行号 **/
		lv_set_tag(&pkg007,"CQ2",lv_pkgreg.pay_ac_no);	/**	原收款人账号 **/
		lv_set_tag(&pkg007,"CR2",lv_pkgreg.pay_name);	/**	原收款人名称 **/
		lv_set_tag(&pkg007,"59F", lv_pkgreg.pay_addr);	/**	原收款人地址 **/
		lv_set_tag(&pkg007,"CCL", lv_pkgreg.cash_opn_br_no);	/**	原付款人开户行行号 **/
		lv_set_tag(&pkg007,"CQ1",lv_pkgreg.cash_ac_no);	/**	原付款人账号 **/
		lv_set_tag(&pkg007,"CR1",lv_pkgreg.cash_name);	/**	原付款人名称 **/
		lv_set_tag(&pkg007,"50F",lv_pkgreg.cash_addr);	/**	原付款人地址 **/
		lv_set_tag(&pkg007,"CIA",lv_pkgreg.retcode);	/**退汇原因**/
		lv_set_tag(&pkg007,"72A",lv_pkgreg.lv_brf);	/**	附言 **/
		/** 拼附加数据 **/
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ORDERNO, lv_pkgreg.orderno);

		if (memcmp(TXNUM,TXNUM_DJTH,sizeof(TXNUM)-1)==0) /** 贷记退汇业务 **/
		{
			lv_set_tag(&pkg007,"B40","00000000");
		}
		/****************** 贷记退汇只有如上类型，且没有附加数据域
		else
		{
		memset(tmpBuf,'\0',sizeof(tmpBuf));
		memcpy(pay_in.F_wtday, WTDAY,sizeof(pay_in.F_wtday));
		memcpy(pay_in.F_orbrno,ORBRNO,sizeof(pay_in.F_orbrno));
		memcpy(pay_in.F_orderno,ORDERNO,sizeof(pay_in.F_orderno));
		iLvGetAddit(&pay_in,"000",tmpBuf,0);
		if(tmpBuf[0]=='\0'){
		****没有附加数据****
		lv_set_tag(&pkg007,"B40","00000000");   ** 附加数据长度**
		}
		else
		{
		****有附加数据****
		char len_buf[9];
		sprintf(len_buf,"%08d",strlen(tmpBuf));
		lv_set_tag(&pkg007,"B40","0");** 附加数据长度 ,此类业务无格式**
		lv_set_tag(&pkg007,"72C","");       ** 附加数据，此类业务无数据 **
		}
		}
		***************/
		lv_batch_end(&pkg007); 

	}	
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE DJTH1;*/

	/** 包头内容 **/
	lv_pkg_batch(&pkg007,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg007,"011",Payqsactno);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg007,"012",Cashqsactno);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg007,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg007,"0BD",wd_lv_wbctl.packid); /** 包序号 **/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg007,"B63",Cnt);                /** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg007,"32B",tmpBuf);             /** 明细业务总金额 **/
	vtcp_log("[%s][%d]---------Cnt=[%s]tmpBuf=[%s]\n",__FL__,Cnt,tmpBuf);

	lv_gen_pack(&pkg007);
	/**
	dm_show(&(pkg007.dm));
	**/
	/** 更新状态为已发送 **/
	Stat2[0]=STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s'  " , Stat, packid, packday, PKGNO_DJTH);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库错误");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_date=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d pack_dat=[%d] ",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
	vtcp_log("%s,%d stat[0]=[%c] ",__FILE__,__LINE__,wd_lv_wbctl.stat[0]);
	vtcp_log("%s,%d packstat=[%s] ",__FILE__,__LINE__,wd_lv_wbctl.packstat);
	if ((g_reply_int = lv_insSendList(&pkg007)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&pkg007))
	{
		lv_destroy(&pkg007);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg007);
	return(0);
}
/** pkg008 **/
/** 普通借记回执业务发送包 **/
/** 传入参数为包id **/
int lv_get_ptjjhz_tag(char *packday,char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg008;
	static struct lv_wbctl_c      wd_lv_wbctl;
	/****HSYX BEGIN:增加变量定义,拼支票回执包****/
	PAY_IN_AREA pay_in;
	char    PAYNO[13];              /**付款人开户行***/
	char    PAYACTNO[33];   /**付款人帐号**/
	char    PAYNAME[61];    /**付款人名称**/
	char    CASHNO[13];             /**收款人开户行**/
	char    CASHACTNO[33];  /**收款人帐号**/
	char    CASHNAME[61];   /**收款人名称**/
	char    PACKDAY[9];             /**包日期**/
	char    PACKID[9];              /**包序号**/
	char    OWTDAY[9];
	/****HSYX END:增加变量定义,拼支票回执包****/
	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double		TXAMT;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO,  '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	/****HSYX BEGIN:初始化变量****/
	memset(OWTDAY, 0 , sizeof(OWTDAY));
	memset(PAYNO,'\0',sizeof(PAYNO));
	memset(PAYACTNO,'\0',sizeof(PAYACTNO));
	memset(PAYNAME,'\0',sizeof(PAYNAME));
	memset(CASHNO,'\0',sizeof(CASHNO));
	memset(CASHACTNO,'\0',sizeof(CASHACTNO));
	memset(CASHNAME,'\0',sizeof(CASHNAME));
	memset(PACKDAY,'\0',sizeof(PACKDAY));
	memset(PACKID,'\0',sizeof(PACKID));
	/****HSYX END:初始化变量****/

	lv_load_tagifno();
	lv_create(&pkg008);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno,  wd_lv_wbctl.pay_qs_no,  sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg008.opeType = '1';	/** 小额 **/
	pkg008.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg008.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg008.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg008.CMTCode,PKGNO_PTJJHZ,3);	/** 因为是PKG008包**/
	memcpy(pkg008.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	/***Modified by SSH,2006.1.9,处理回执,回执交易记录中保存的是就包信息****/
	/****在往包控制表的opackday和opackid中保留了旧信息可以对应****/
	/*====================================================================
	Stat[0]=STAT_WZCHECK2;
	EXEC SQL DECLARE PTJJHZ CURSOR FOR 
	SELECT  wtday, orderno,norderno,txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday, content
	FROM  lv_ptjjs
	where payqsactno=:wd_lv_wbctl.cash_qs_no 
	and packid =:wd_lv_wbctl.o_packid 
	and packday =:wd_lv_wbctl.o_pack_date 
	and pkgno=:Pkgno and stat <>'0' ;
	EXEC SQL OPEN PTJJHZ;
	*=====================================================================*/

	/***HSYX BEGIN:如果wbctl的opackid为99999999表示为支票业务,否则为普通借记业务**/
	if (memcmp(wd_lv_wbctl.o_packid,"99999999",8)==0)
	{
		apitoa(wd_lv_wbctl.pack_date,sizeof(PACKDAY)-1,PACKDAY);
		memcpy(PACKID,wd_lv_wbctl.packid,sizeof(PACKID)-1);
		vtcp_log("%s,%d,Gethere1,PAYQSACTNO=[%s],PACKDAY=[%s],PACKID=[%s],PKGNO=[%s]\n",__FILE__,__LINE__,wd_lv_wbctl.cash_qs_no,PACKDAY,PACKID,Pkgno);
	}else
	{
		/* rem by LiuHuafeng 20070402 NEWYX
		apitoa(wd_lv_wbctl.o_pack_date,sizeof(PACKDAY)-1,PACKDAY);
		memcpy(PACKID,wd_lv_wbctl.o_packid,sizeof(PACKID)-1);
		*********replace by next tow line *************/
		apitoa(wd_lv_wbctl.pack_date,sizeof(PACKDAY)-1,PACKDAY);
		memcpy(PACKID,wd_lv_wbctl.packid,sizeof(PACKID)-1);
		vtcp_log("%s,%d,Gethere1,PAYQSACTNO=[%s],PACKDAY=[%s],PACKID=[%s],PKGNO=[%s]\n",__FILE__,__LINE__,wd_lv_wbctl.cash_qs_no,PACKDAY,PACKID,Pkgno);
	}
	/****HSYX BEGIN:加包条件****/
	vtcp_log("%s,%d,Gethere1,PAYQSACTNO=[%s],PACKDAY=[%s],PACKID=[%s],PKGNO=[%s]\n",\
		__FILE__,__LINE__,wd_lv_wbctl.cash_qs_no,PACKDAY,PACKID,Pkgno);

	vtcp_log("[%s][%d]packid=[%s]",__FILE__,__LINE__,packid);
	vtcp_log("[%s][%d]packday=[%s]",__FILE__,__LINE__,packday);
	vtcp_log("[%s][%d]wd_lv_wbctl.pay_qs_no=[%s]",__FILE__,__LINE__,wd_lv_wbctl.pay_qs_no);
	/*** dongxy 修改一下 20070320
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts <> '0' and pay_qs_no = '%s'", PKGNO_PTJJHZ, packid, packday, wd_lv_wbctl.pay_qs_no);
	*****************************/
	g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts <> '0' and pay_qs_no = '%s'", PKGNO_PTJJHZ, PACKID, PACKDAY, wd_lv_wbctl.pay_qs_no);
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/***HSYX END:如果wbctl的opackid为空表示为支票业务,否则为普通借记业务**/
	/********** HSYX del begin
	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts <> '0' and pay_qs_no = '%s'", PKGNO_PTJJHZ, packid, packday, wd_lv_wbctl.pay_qs_no);
	if (g_reply_int != DB_OK)
	{
	sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
	WRITEMSG
	return(-1);
	}
	HSYX del end*************/

	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{ 
		/*EXEC SQL FETCH PTJJHZ INTO :WTDAY,:ORDERNO,NORDERNO,:TXNUM,:ORBRNO,:ACBRNO,:INDAY,:TXAMT,:RCPSTAT,
		:JZDAY,:CONTENT;*/
		/** dongxy 修改一下 20070320
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		**/
		g_reply_int = Lv_pkgreg_Fet_Upd(&lv_pkgreg, g_pub_tx.reply);
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("[%s][%d]cashqsactno=[%s]\n",__FL__,wd_lv_wbctl.cash_qs_no);
			vtcp_log("[%s][%d]opackid[%s]\n",	__FL__,wd_lv_wbctl.o_packid); 
			vtcp_log("[%s][%d]opackday=[%d]\n",	__FL__,wd_lv_wbctl.o_pack_date );
			vtcp_log("[%s][%d]pkgno=[%s]\n",__FL__,Pkgno);
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"数据库错误!");
			lv_destroy(&pkg008);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");
			lv_destroy(&pkg008);
			WRITEMSG
				return(-1);
		}
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(ORDERNO, lv_pkgreg.orderno);
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(INDAY, lv_date(lv_pkgreg.in_date));
		TXAMT = lv_pkgreg.tx_amt*100;
		strcpy(RCPSTAT, lv_pkgreg.rcpstat);
		strcpy(JZDAY, lv_date(lv_pkgreg.jz_date));
		strcpy(CONTENT, lv_pkgreg.lv_brf);

		/****HSYX BEGIN :多选了几个字段****/
		strncpy(OWTDAY, lv_date(lv_pkgreg.o_wt_date),8);
		strcpy(PAYNO, lv_pkgreg.pay_opn_br_no);
		strcpy(PAYACTNO, lv_pkgreg.pay_ac_no);
		strcpy(PAYNAME, lv_pkgreg.pay_name);
		strcpy(CASHNO, lv_pkgreg.cash_opn_br_no);
		strcpy(CASHACTNO, lv_pkgreg.cash_ac_no);
		strcpy(CASHNAME, lv_pkgreg.cash_name);
		/****HSYX END :多选了几个字段****/

		/** NORDERNO是新序号，ORDERNO 是原来帐序号 **/
		strcpy(NORDERNO, lv_pkgreg.orderno);
		strcpy(ORDERNO, lv_pkgreg.o_orderno);
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
		vtcp_log("[%s][%d]cashqsactno=[%s]\n",__FL__,wd_lv_wbctl.cash_qs_no);
		vtcp_log("[%s][%d]opackid[%s]\n",	__FL__,wd_lv_wbctl.o_packid); 
		vtcp_log("[%s][%d]opackday=[%d]\n",	__FL__,wd_lv_wbctl.o_pack_date );
		vtcp_log("[%s][%d]pkgno=[%s]\n",__FL__,Pkgno);
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库错误!");

		lv_destroy(&pkg008);
		WRITEMSG
		return(-1);
		}
		/******************/
		/** 要素集内容 **/
		/****HSYX BEGIN:拼支票回执包****/
		if(memcmp(wd_lv_wbctl.o_packid,"99999999",9)==0){
			ZPJL1_ADD_AREA  wd_zp_add1;
			memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
			memcpy(pkg008.CMTNo,"007",3);       /***007－票交回执***/
			vtcp_log("[%s][%d]===============\n",__FILE__,__LINE__);
			/*****delete by xyy 2007-4-23 16:22 NEWYX3
			if (memcmp(TXNUM,"30102",5)==0)
			{
				lv_set_tag(&pkg008,"0BG","30104");    ** 支票业务回执 **
			}else
			{
				lv_set_tag(&pkg008,"0BG","30105");    ** 通用业务回执 **
			}
			*****************replace by next line**************************/
			lv_set_tag(&pkg008,"0BG",TXNUM);
			
			lv_set_tag(&pkg008,"30A",INDAY);    /** 委托日期 **/
			/* rem by LiuHuafeng 20070402
			lv_set_tag(&pkg008,"051",WTDAY);
			** replace by next line ******/
			lv_set_tag(&pkg008,"051",OWTDAY);    /** 原委托日期 **/
			lv_set_tag(&pkg008,"0BC",NORDERNO);     /**     支付交易序号 **/
			lv_set_tag(&pkg008,"005",ORDERNO);  /** 原支付交易序号 **/
			memcpy(pay_in.F_wtday, WTDAY,sizeof(pay_in.F_wtday));
			memcpy(pay_in.F_orbrno,ACBRNO,sizeof(pay_in.F_orbrno));
			/** 应为上面重新赋值了，ORDERNO是原来帐序号 dongxy
			memcpy(pay_in.F_orderno,ORDERNO,sizeof(pay_in.F_orderno));
			**/
			/****NEWYX2:找来帐怎么能用新序号呢****/
			memcpy(pay_in.F_orderno,ORDERNO,sizeof(pay_in.F_orderno));
			memcpy(pay_in.F_pkgno,PKGNO_PTJJ,sizeof(pay_in.F_pkgno));
			memcpy(pay_in.F_opcd,OPCD_RECEIVE,sizeof(pay_in.F_opcd));
			memcpy(pay_in.F_wtday,OWTDAY    ,sizeof(pay_in.F_wtday));/***add by xyy 2007-4-15 16:57 NEWYX2***/
			/** 下面函数里增加条件了，所以赋值 dongxy **/
			pay_in.F_lw_ind[0]=lv_pkgreg.lw_ind[0];
			iRc=iLvFetchRecToPayin(&pay_in);
			if (iRc)
			{
				vtcp_log("%s,%d,数据库错误iRc=%d\n",__FILE__,__LINE__,iRc);
				sprintf( acErrMsg,"数据库错误!");
				lv_destroy(&pkg008);
				WRITEMSG
					return(-1);
			}
			if (memcmp(TXNUM,TXNUM_ZPJLHZ,strlen(TXNUM_ZPJLHZ))==0)/**modify by xyy 2007-4-23 16:23 NEWYX3**/
			{
				ZPJL1_ADD_AREA  wd_zp_add1;
				char pcTmpBuf[32];
				memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
				memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
				iRc=iLvGetAddit(&pay_in,"001",&wd_zp_add1,0);
				memcpy(pcTmpBuf,wd_zp_add1.cpno,sizeof(wd_zp_add1.cpno));
				if (pcTmpBuf[0]=='\0'||pcTmpBuf[0]==' ')
				{
					memset(pcTmpBuf,'0',12);
					pcTmpBuf[12]='\0';
				}else
				{
					sprintf(pcTmpBuf,"%012d",apatoi(wd_zp_add1.cpno,sizeof(wd_zp_add1.cpno)));
				}
				lv_set_tag(&pkg008,"CSH",pcTmpBuf);  /** 支票号码 **/
			}else if(memcmp(TXNUM,TXNUM_TYJLHZ,strlen(TXNUM_TYJLHZ))==0) /**modify by xyy 2007-4-23 16:23 NEWYX3**/
			{
				TYJL1_ADD_AREA  wd_ty_add1;
				char pcTmpBuf[32];
				memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
				memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
				iRc=iLvGetAddit(&pay_in,"001",&wd_ty_add1,0);
				memcpy(pcTmpBuf,wd_ty_add1.cpno,sizeof(wd_ty_add1.cpno));
				if (pcTmpBuf[0]=='\0'||pcTmpBuf[0]==' ')
				{
					memset(pcTmpBuf,'0',12);
					pcTmpBuf[12]='\0';
				}else
				{
					sprintf(pcTmpBuf,"%012d",apatoi(wd_ty_add1.cpno,sizeof(wd_ty_add1.cpno)));
				}
				lv_set_tag(&pkg008,"CSH",pcTmpBuf);  /** 支票号码**/
			}
			lv_set_tag(&pkg008,"CC4",PAYNO);    /** 付款人开户行 **/
			lv_set_tag(&pkg008,"50C",PAYACTNO);    /** 付款人帐号 **/
			lv_set_tag(&pkg008,"50A",PAYNAME);    /** 付款人名称 **/
			lv_set_tag(&pkg008,"CC5",CASHNO);    /** 收款人开户行*/
			lv_set_tag(&pkg008,"59C",CASHACTNO);    /** 收款帐号*/
			lv_set_tag(&pkg008,"59A",CASHNAME);    /** 收款人名称*/
			lv_set_tag(&pkg008,"52A",ORBRNO);    /** 发起行*/
			lv_set_tag(&pkg008,"58A",ACBRNO);    /** 接收行*/
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			sprintf(tmpBuf,"%015.0f",TXAMT);
			printf("[%s][%d]====tmpBuf=[%s]\n",__FILE__,__LINE__,tmpBuf);
			lv_set_tag(&pkg008,"33S",tmpBuf);       /** 金额 **/
			if (memcmp(RCPSTAT,"00",2)==0)
			{
				lv_set_tag(&pkg008,"CIA","00");         /** 业务回执状态 **/
				lv_set_tag(&pkg008,"CCA","          ");         /** 业务回执状态 **/
			}
			else
			{
				char pcReason[11];
				int  iReasonLen;
				memset(pcReason,' ',10);
				pcReason[10]='\0';
				lv_set_tag(&pkg008,"CIA","09");         /** 业务回执状态 **/
				/***Add by SSH,2006.11.14,支持多个退票理由**/
				memset(pcReason,'\0',sizeof(pcReason));
				iRc=iLvGetAddit(&pay_in,"TPL",pcReason,0);
				/****HSYX BEGIN:新版3.1.1必须填够5个理由!****/
				/***测试下***/
				vtcp_log("[%s][%d]--------pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
				/**begin: add by wangyongwei 070410***/
			  if(memcmp(pcReason,"00",2)==0)
			  {
			  	memset(pcReason,0,sizeof(pcReason));
			  	pub_base_full_space(pcReason,10);
			  }
			  else
			  {
          zip_space(pcReason);
          iReasonLen=strlen(pcReason);
			  	if(iReasonLen<10)
			  	{
			      vtcp_log("[%s][%d]--------pcReason=[%s]*******!!!!!!!\n",__FILE__,__LINE__,pcReason);
			  		char tmpReason[11];
			  		memset(tmpReason,0,sizeof(tmpReason));
			  		memcpy(tmpReason,pcReason,iReasonLen);
			  		memset(pcReason,' ',sizeof(pcReason)-1);
			  		memcpy(pcReason+10-iReasonLen,tmpReason,iReasonLen);		
			      vtcp_log("[%s][%d]--------pcReason=[%s]*******!!!!!!!\n",__FILE__,__LINE__,pcReason);
				  }
			  }
		    /**end: add by wangyongwei 070410***/
				/***
				memcpy(pcReason,"2122232425",10);***/
				/****HSYX END:新版3.1.1必须填够5个理由****/
				vtcp_log("[%s][%d]--------pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
				lv_set_tag(&pkg008,"CCA",pcReason);     /** 业务回执状态 **/
			}
			lv_set_tag(&pkg008,"72A",CONTENT);      /** 附言 **/
			/****HSYX END:拼支票回执要素集****/
		}
		else
		{
			memcpy(pkg008.CMTNo,"006",3);       /***要素集号006***/
			pkg008.paymPRI='0';                 /***优先级***/
			lv_set_tag(&pkg008,"30A",INDAY);    /** 委托日期 **/
			lv_set_tag(&pkg008,"BSE",INDAY);    /** 扣款日期 **/

			vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);		
			lv_set_tag(&pkg008,"0BC",NORDERNO);		/**支付交易序号 **/			

			lv_set_tag(&pkg008,"0BH",TXNUM);	/**	原业务类型号 **/	
			/****NEWYX2:发起和接收行号反了****/
			lv_set_tag(&pkg008,"CC1",ACBRNO);   /** 原发起行行号 **/
			lv_set_tag(&pkg008,"CC2",ORBRNO);   /** 原接收行行号 **/
			lv_set_tag(&pkg008,"051",OWTDAY);   /**NEWBP 原委托日期 **/
			lv_set_tag(&pkg008,"005",ORDERNO);  /** 原支付交易序号 **/

			vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
			memset(tmpBuf, '\0', sizeof(tmpBuf));
			sprintf(tmpBuf,"%015.0f",TXAMT);
			vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
			lv_set_tag(&pkg008,"33S",tmpBuf);   	/** 金额 **/
			lv_set_tag(&pkg008,"CIA",RCPSTAT);   	/** 业务回执状态 **/
			/** lv_set_tag(&pkg008,"BSN","");   		开户行手续费 **/
			lv_set_tag(&pkg008,"72A",CONTENT);  	/** 附言 **/
		}
		if (memcmp(RCPSTAT,"00",2)==0)
		{
			/****同意应答****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			lv_pkgreg.lv_sts[0] = STAT_YES_SEND;/* 需要问问董晓燕 by LiuHuafeng 2007-4-6 22:51*/
			/** dongxy 修改一下 20070320
			pcStat[0]=STAT_YES_SEND;
			g_reply_int = sql_execute( "update lv_pkgreg set lv_sts='%s' where wt_date='%s' and pkgno = '%s' and lw_ind = '2' and or_br_no='%s' and orderno='%s' ", pcStat ,WTDAY, PKGNO_PTJJ,  ORBRNO, ORDERNO);
			*******************************/
			g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
			if (g_reply_int)
			{
				vtcp_log("%s,%d,fetch lv_wbctl错[%d]\n",__FILE__,__LINE__,g_reply_int);
				strcpy( g_pub_tx.reply,"P058" );
				WRITEMSG
					return(-1);
			}
		}
		else
		{
			/****不同意应答****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			/** dongxy 修改一下 20070320
			pcStat[0]=STAT_NO_SEND;
			g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%s' where pkgno = '%s' and lw_ind = '2' and wt_date='%s' and or_br_no='%s' and orderno='%s'", pcStat, PKGNO_PTJJ, WTDAY, ORBRNO, ORDERNO);
			**************************************/
			lv_pkgreg.lv_sts[0] = STAT_NO_SEND;
			g_reply_int = Lv_pkgreg_Upd_Upd(lv_pkgreg,g_pub_tx.reply);
			if (g_reply_int)
			{
				vtcp_log("%s,%d,fetch lv_wbctl错[%d]\n",__FILE__,__LINE__,g_reply_int);
				strcpy( g_pub_tx.reply,"P058" );
				WRITEMSG
					return(-1);
			}
		}
		lv_batch_end(&pkg008);
	}
	/*EXEC SQL CLOSE PTJJHZ;*/
	/** dongxy 修改一下 20070320
	g_reply_int = Lv_pkgreg_Clo_Sel();
	******************************/
	g_reply_int = Lv_pkgreg_Clo_Upd();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	/** 包头内容 **/
	lv_pkg_batch(&pkg008,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg008,"011",Payqsactno);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg008,"012",Cashqsactno);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg008,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg008,"0BD",wd_lv_wbctl.packid);		/** 包序号 **/
	/** lv_pkg_batch(&pkg008,"C15","");	包密押 **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	vtcp_log("[%s][%d]dtlcnt===[%s]",__FILE__,__LINE__,Cnt);
	lv_pkg_batch(&pkg008,"B63",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	vtcp_log("[%s][%d]totamt===[%f]",__FILE__,__LINE__,wd_lv_wbctl.totamt);
	lv_pkg_batch(&pkg008,"32B",tmpBuf);	/** 明细业务总金额 **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/**
	dbl2str(&wd_lv_wbctl.dtlsuccnt,0,sizeof(Cnt)-1,0,Cnt);
	**/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlsuccnt);
	vtcp_log("[%s][%d]====Cnt=[%s]\n",__FL__,Cnt);
	lv_pkg_batch(&pkg008,"B41",Cnt);	/** 明细业务成功总笔数 **/
	vtcp_log("[%s][%d]dtlsuccnt===[%s]",__FILE__,__LINE__,Cnt);
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100);
	vtcp_log("[%s][%d]dtlsucamt===[%f]",__FILE__,__LINE__,wd_lv_wbctl.dtlsucamt);
	lv_pkg_batch(&pkg008,"32C",tmpBuf);	/** 明细业务成功总金额 **/
	lv_pkg_batch(&pkg008,"CCO",wd_lv_wbctl.cash_qs_no);/** 原包发起清算行 **/

	/** 报文要求02D字段是"999", 所以加一个条件 dongxy 20070320 **/
	vtcp_log("[%s][%d]---------txnum=[%s]\n",__FILE__,__LINE__,TXNUM);
	/****NEWYX2:新系统有更准确、更简单得判断方法****/
	if (memcmp(wd_lv_wbctl.o_packid,"99999999",8)==0){
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		lv_pkg_batch(&pkg008,"02D","999");		/** 原包类型号 **/
		lv_pkg_batch(&pkg008,"30I",pcLvGetDay_work());	/** 原包委托日期 影像要求是小额日期**/
		vtcp_log("[%s][%d]##################这里啊!\n",__FILE__,__LINE__);
	}
	else
	{
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		lv_pkg_batch(&pkg008,"02D",wd_lv_wbctl.o_pkgno);		/** 原包类型号 **/
		lv_pkg_batch(&pkg008,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** 原包委托日期 **/
	}
	vtcp_log("%s,%d opackid===[%s]\n",__FILE__,__LINE__,wd_lv_wbctl.o_packid);
	lv_pkg_batch(&pkg008,"0BE",wd_lv_wbctl.o_packid);	/** 原包序号 **/
	lv_pkg_batch(&pkg008,"72D","");		/**包附加数据 暂无 **/
	lv_gen_pack(&pkg008);

	Stat2[0] = STAT_WZSEND;

	vtcp_log("[%s][%d]更新lv_pkgreg的时候了packid=[%s]",__FILE__,__LINE__,wd_lv_wbctl.packid);
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_PTJJHZ);
	vtcp_log("[%s][%d]更新了lv_pkgreg的状态lv_sts=[%c]",__FILE__,__LINE__,lv_pkgreg.lv_sts[0]);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库错误!");
		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg008)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&pkg008))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg008);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg008);

	return(0);
}
/** pkg009 **/
/** 实时贷记回执业务发送包 **/
/** 传入参数为包id **/
int lv_get_ssdjhz_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg009;
	PAY_IN_AREA pay_in;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char  cOwtday[9];
	char	WTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double	TXAMT;
	double	FEEAMT2;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(cOwtday, 0 , sizeof(cOwtday));
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO,  '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg009);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]===============\n",__FL__);
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]====\n",__FL__);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld  and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int!= DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");

		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg009.opeType = '1';	/** 小额 **/
	pkg009.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg009.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg009.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg009.CMTCode,PKGNO_SSDJHZ,3);	/** 因为是PKG009包**/
	memcpy(pkg009.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	Stat[0]=STAT_WZLR;
	/*======================================================================
	EXEC SQL DECLARE SSDJHZ CURSOR FOR 
	SELECT  wtday, orderno,norderno,txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday,feeamt2, content
	FROM  lv_ssdjs where payqsactno=:wd_lv_wbctl.cash_qs_no and  packid =:wd_lv_wbctl.o_packid and packday =:wd_lv_wbctl.o_pack_date and pkgno=:Pkgno and stat <>'0' ;

	EXEC SQL OPEN SSDJHZ;

	EXEC SQL FETCH SSDJHZ INTO :WTDAY,:ORDERNO,:NORDERNO,:TXNUM,:ORBRNO,:ACBRNO,:INDAY,:TXAMT,:RCPSTAT,
	:JZDAY,:FEEAMT2,:CONTENT;
	======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %ld and lw_ind = '1' and lv_sts <> '0' and cash_qs_no = '%s'", PKGNO_SSDJHZ, wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, wd_lv_wbctl.cash_qs_no);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int != DB_OK && g_reply_int != DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	apitoa(lv_pkgreg.in_date,8,INDAY);
	apitoa(lv_pkgreg.wt_date,8,WTDAY);	
	strcpy(TXNUM, lv_pkgreg.txnum);
	strcpy(ORBRNO, lv_pkgreg.or_br_no);
	strcpy(ACBRNO, lv_pkgreg.ac_br_no);
	strcpy(cOwtday, lv_date(lv_pkgreg.o_wt_date));
	strcpy(ORDERNO, lv_pkgreg.orderno);
	TXAMT = lv_pkgreg.tx_amt*100.0;
	strcpy(RCPSTAT, lv_pkgreg.rcpstat);
	strcpy(CONTENT, lv_pkgreg.lv_brf);
	FEEAMT2 = lv_pkgreg.df_chrg_amt;

	/* Begin:  Added by chenming   2006-10-11 20:11 应该新生成一个支付交易序号 */
	vtcp_log("%s,%d, ======ORBRNO=[%s] ",__FILE__,__LINE__,ORBRNO);				
	long lOrderno = iLvGetOrderno();
	vtcp_log("%s,%d, ======lOrderno=[%ld] ",__FILE__,__LINE__,lOrderno);		  
	apitoa(lOrderno, sizeof(NORDERNO)-1, NORDERNO);
	vtcp_log("%s,%d, ======NORDERNO=[%s] ",__FILE__,__LINE__,NORDERNO);	  
	/* End:  Added by chenming   2006-10-11 20:11 */   		

	/** 要素集内容 **/
	memcpy(pkg009.CMTNo,"006",3);       /***要素集号006***/
	pkg009.paymPRI='0';                 /***优先级***/
	lv_set_tag(&pkg009,"30A",INDAY);    /** 委托日期 **/

	vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);			
	lv_set_tag(&pkg009,"0BC",NORDERNO);	/**	支付交易序号 **/	

	lv_set_tag(&pkg009,"0BH",TXNUM);	/**	原业务类型号 **/	
	lv_set_tag(&pkg009,"CC1",lv_pkgreg.o_or_br_no);   /** 原发起行行号 **/
	lv_set_tag(&pkg009,"CC2",lv_pkgreg.o_ac_br_no);   /** 原接收行行号 **/
	lv_set_tag(&pkg009,"051",cOwtday);    /** 原委托日期 **/
	lv_set_tag(&pkg009,"005",lv_pkgreg.o_orderno); 		/***原支付交易序号 **/

	vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",TXAMT);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg009,"33S",tmpBuf);   	/** 金额 **/

	lv_set_tag(&pkg009,"CIA",RCPSTAT);   	/** 业务回执状态 **/
	lv_set_tag(&pkg009,"BSE",INDAY);   	/** 扣款日期 **/

	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",FEEAMT2);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg009,"BSN",tmpBuf);   		/** 开户行手续费 **/

	lv_set_tag(&pkg009,"72A",CONTENT);  	/** 附言 **/

	lv_batch_end(&pkg009);

	/*EXEC SQL CLOSE SSDJHZ;*/
	g_reply_int = Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/** 包头内容 **/
	lv_pkg_batch(&pkg009,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg009,"011",Payqsactno);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg009,"012",Cashqsactno);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg009,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg009,"0BD",wd_lv_wbctl.packid);		/** 包序号 **/
	/** lv_pkg_batch(&pkg009,"C15","");	包密押 **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg009,"B63",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg009,"32B",tmpBuf);	/** 明细业务总金额 **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/***
	dbl2str(&wd_lv_wbctl.dtlsuccnt,0,sizeof(Cnt)-1,0,Cnt);
	***/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlsuccnt);
	lv_pkg_batch(&pkg009,"B41",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	/****NEWYX2:成功金额计算不正确****/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100);
	lv_pkg_batch(&pkg009,"32C",tmpBuf);	/** 明细业务总金额 **/

	lv_pkg_batch(&pkg009,"02D",wd_lv_wbctl.o_pkgno);		/** 原包类型号 **/
	lv_pkg_batch(&pkg009,"CCO",wd_lv_wbctl.cash_qs_no);/** 原包发起清算行 **/
	lv_pkg_batch(&pkg009,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** 原包委托日期 **/
	lv_pkg_batch(&pkg009,"0BE",wd_lv_wbctl.o_packid);	/** 原包序号 **/
	lv_pkg_batch(&pkg009,"72D","");		/** 包附加数据 暂无 **/
	lv_gen_pack(&pkg009);
	/**
	dm_show(&(pkg009.dm));
	**/
	Stat2[0] = STAT_WZSEND;

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_SSDJHZ);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"没有发现数据!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg009)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]发包\n",__FL__);
	if (iRc=iLvSendPackage(&pkg009))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg009);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg009);

	return(0);
}
/** pkg010 **/
/** 实时借记回执业务发送包 **/
/** 传入参数为包id **/
int lv_get_ssjjhz_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg010;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	OWTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double		TXAMT;
	double		FEEAMT2;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO, '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));
	memset(OWTDAY, 0 , sizeof(OWTDAY));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg010);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]===============执行到了这里!!!======= ",__FILE__,__LINE__);
	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	vtcp_log("%s,%d wd_lv_wbctl.o_packid ===[%.8s] ",__FL__,wd_lv_wbctl.o_packid);	
	vtcp_log("%s,%d wd_lv_wbctl.o_pack_date ===[%d] ",__FL__,wd_lv_wbctl.o_pack_date);					
	vtcp_log("%s,%d wd_lv_wbctl.cash_qs_no ===[%.12s] ",__FL__,wd_lv_wbctl.cash_qs_no);							

	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg010.opeType = '1';	/** 小额 **/
	pkg010.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg010.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg010.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg010.CMTCode,PKGNO_SSJJHZ,3);	/** 因为是PKG010包**/
	memcpy(pkg010.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	Stat[0]=STAT_WZCHECK2;
	vtcp_log("[%s][%d]========Stat=[%s]\n",__FL__,Stat);
	vtcp_log("[%s][%d]========opackid=[%s]\n",__FL__,wd_lv_wbctl.o_packid);
	vtcp_log("[%s][%d]========opackday=[%d]\n",__FL__,wd_lv_wbctl.o_pack_date);
	vtcp_log("[%s][%d]========cash_qs_no=[%s]\n",__FL__,wd_lv_wbctl.cash_qs_no);	
	/*======================================================================
	EXEC SQL DECLARE SSJJHZ CURSOR FOR 
	SELECT  wtday, orderno, norderno, txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday,feeamt2, content
	FROM  lv_ssjjs where payqsactno=:wd_lv_wbctl.cash_qs_no and packid =:wd_lv_wbctl.o_packid and packday =:wd_lv_wbctl.o_pack_date and pkgno=:Pkgno and stat <>'0';

	EXEC SQL OPEN SSJJHZ;

	EXEC SQL FETCH SSJJHZ INTO :WTDAY,:ORDERNO,:NORDERNO,:TXNUM,:ORBRNO,:ACBRNO,:INDAY,:TXAMT,:RCPSTAT,
	:JZDAY,:FEEAMT2,:CONTENT;
	*======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %ld and lw_ind = '1' and lv_sts <> '0' and cash_qs_no = '%s'", PKGNO_SSJJHZ, wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, wd_lv_wbctl.cash_qs_no);
	if ( g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误 sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);

		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,未找到相应的数据库记录 sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"未找到相应的数据库记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
		return(-1);
	}
	vtcp_log("%s,%d or_br_no=[%s] ac_br_no=[%s] o_wt_date=[%d]",__FILE__,__LINE__,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.o_wt_date);
	vtcp_log("%s,%d o_or_br_no=[%s] o_ac_br_no=[%s] o_wt_date=[%d]",__FILE__,__LINE__,lv_pkgreg.o_or_br_no,lv_pkgreg.o_ac_br_no,lv_pkgreg.o_wt_date);
	strcpy(INDAY, lv_date(lv_pkgreg.in_date));
	strcpy(TXNUM, lv_pkgreg.txnum);
	strcpy(ORBRNO, lv_pkgreg.or_br_no);
	strcpy(ACBRNO, lv_pkgreg.ac_br_no);
	strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
	strncpy(OWTDAY,lv_date(lv_pkgreg.o_wt_date),8);
	strcpy(ORDERNO, lv_pkgreg.orderno);
	TXAMT = lv_pkgreg.tx_amt*100.0;
	strcpy(RCPSTAT, lv_pkgreg.rcpstat);
	strcpy(CONTENT, lv_pkgreg.lv_brf);
	FEEAMT2 = lv_pkgreg.df_chrg_amt;
	/****NEWYX2:发送时切不可新产生序号****/

	/** 要素集内容 **/
	memcpy(pkg010.CMTNo,"006",3);       /***要素集号006***/
	pkg010.paymPRI='0';                 /***优先级***/
	lv_set_tag(&pkg010,"30A",INDAY);    /** 委托日期 **/
	lv_set_tag(&pkg010,"BSE",INDAY);   	/** 扣款日期 **/

	vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);			
	lv_set_tag(&pkg010,"0BC",lv_pkgreg.orderno);	/**	支付交易序号 **/	

	lv_set_tag(&pkg010,"0BH",TXNUM);	/**	原业务类型号 **/	

	/* rem by LiuHuafeng 2007-4-8 12:22 由于查询的是组织的回执，也就是往帐，
		所以原发起行号和原接收行号应该是反过来的 by LiuHuafeng 2007-4-8 12:23
		原发起行号--->本次接收行,原接收行号-->本次发起行
		或者直接用数据表中原的数据就可以了
	lv_set_tag(&pkg010,"CC1",ORBRNO);   ** 原发起行行号 **
	lv_set_tag(&pkg010,"CC2",ACBRNO);   ** 原接收行行号 **
	lv_set_tag(&pkg010,"051",WTDAY);   ** 原委托日期 **
	lv_set_tag(&pkg010,"005",ORDERNO); ** 原支付交易序号 **
	***************************************************************/
	lv_set_tag(&pkg010,"CC1",lv_pkgreg.o_or_br_no);   /** 原发起行行号 **/
	lv_set_tag(&pkg010,"CC2",lv_pkgreg.o_ac_br_no);   /** 原接收行行号 **/
	lv_set_tag(&pkg010,"051",OWTDAY);    /** 原委托日期 **/
	lv_set_tag(&pkg010,"005",lv_pkgreg.o_orderno); /** 原支付交易序号 **/

	vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",TXAMT);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg010,"33S",tmpBuf);   	/** 金额 **/

	lv_set_tag(&pkg010,"CIA",RCPSTAT);   	/** 业务回执状态 **/

	memset(tmpBuf, '\0', sizeof(tmpBuf));
	sprintf(tmpBuf,"%015.0f",FEEAMT2);
	vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
	lv_set_tag(&pkg010,"BSN",tmpBuf);   		/** 开户行手续费 **/

	lv_set_tag(&pkg010,"72A",CONTENT);  	/** 附言 **/

	lv_batch_end(&pkg010);

	/*	EXEC SQL CLOSE SSJJHZ;*/
	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/** 包头内容 **/
	lv_pkg_batch(&pkg010,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg010,"011",Payqsactno);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg010,"012",Cashqsactno);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg010,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg010,"0BD",wd_lv_wbctl.packid);		/** 包序号 **/
	/** lv_pkg_batch(&pkg010,"C15","");	包密押 **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg010,"B63",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg010,"32B",tmpBuf);	/** 明细业务总金额 **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/****NEWYX2:成功笔数算法不对****/
	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlsuccnt);
	lv_pkg_batch(&pkg010,"B41",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	/****NEWYX2:成功金额计算不正确****/
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100);
	lv_pkg_batch(&pkg010,"32C",tmpBuf);	/** 明细业务总金额 **/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	lv_pkg_batch(&pkg010,"02D",wd_lv_wbctl.o_pkgno);		/** 原包类型号 **/
	lv_pkg_batch(&pkg010,"CCO",wd_lv_wbctl.cash_qs_no);/** 原包发起清算行 **/
	lv_pkg_batch(&pkg010,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** 原包委托日期 **/
	lv_pkg_batch(&pkg010,"0BE",wd_lv_wbctl.o_packid);	/** 原包序号 **/
	/****************
	lv_pkg_batch(&pkg010,"BS1",wd_lv_wbctl.zcnode);		** 轧差节点类型 **
	lv_pkg_batch(&pkg010,"BS2",wd_lv_wbctl.zcday);		** 轧差日期 **
	lv_pkg_batch(&pkg010,"BS3",wd_lv_wbctl.zcid);		** 轧差场次 **
	lv_pkg_batch(&pkg010,"BS4",wd_lv_wbctl.bfflag);		** 补发标志 **
	lv_pkg_batch(&pkg010,"BS5",wd_lv_wbctl.qsday);		** 清算日期 **
	lv_pkg_batch(&pkg010,"CIB",wd_lv_wbctl.packstat);		** 包处理状态 **
	lv_pkg_batch(&pkg010,"72D","");		** 包附加数据 暂无 **
	***************/

	lv_gen_pack(&pkg010);
	/**
	dm_show(&(pkg010.dm));
	**/
	Stat2[0] = STAT_WZSEND;
	/*EXEC SQL UPDATE lv_ssjjs set stat =:Stat2 where packid =:wd_lv_wbctl.o_packid  and packday =:wd_lv_wbctl.o_pack_date and stat <>'0';*/
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_SSJJHZ);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));
	vtcp_log("[%s][%d]===============执行到了这里!!!======= ",__FILE__,__LINE__);
	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg010)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	vtcp_log("发包\n");
	pubf_lv_print_dm(&(pkg010.dm));
	if (iRc=iLvSendPackage(&pkg010))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg010);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg010);

	vtcp_log("[%s][%d]===============执行到了这里!!!======= ",__FILE__,__LINE__);

	return(0);
}
/** pkg011 **/
/** 定期借记回执业务发送包 **/
/** 传入参数为包id **/
int lv_get_dqjjhz_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[64];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg011;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	JZDAY[9];
	char	INDAY[9];
	char	ORDERNO[9];
	char	NORDERNO[9];
	char	RCPSTAT[3];
	double		TXAMT;
	char	CONTENT[61];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	memset(NORDERNO, '\0', sizeof(NORDERNO));
	memset(JZDAY,	 '\0', sizeof(JZDAY));
	memset(INDAY,	 '\0', sizeof(INDAY));
	memset(RCPSTAT,	 '\0', sizeof(RCPSTAT));
	memset(CONTENT,	 '\0', sizeof(CONTENT));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg011);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]=wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]=wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg011.opeType = '1';	/** 小额 **/
	pkg011.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg011.packDate, lv_date(wd_lv_wbctl.pack_date), 8);
	pkg011.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg011.CMTCode,PKGNO_DQJJHZ,3);	/** 因为是PKG011包**/
	memcpy(pkg011.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	/****Modified by SSH,2006.1.10,处理回执拼包,记录中的包信息为旧信息****/
	Stat[0]=STAT_WZCHECK2;
	/*================================================================================
	EXEC SQL DECLARE DQJJHZ CURSOR FOR 
	SELECT  wtday, orderno,norderno,txnum, orbrno,acbrno,inday,txamt,rcpstat, jzday, content
	FROM  lv_dqjjs where payqsactno=:wd_lv_wbctl.cash_qs_no and  packid =:wd_lv_wbctl.o_packid and  packday = :wd_lv_wbctl.o_pack_date and stat <>'0';

	EXEC SQL OPEN DQJJHZ;
	*================================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %ld and lw_ind = '1'  and lv_sts ='3' and cash_qs_no = '%s'", PKGNO_DQJJHZ, packid, apatoi( packday, 8 ), wd_lv_wbctl.cash_qs_no);
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{ 
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg011);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"数据库错误!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg011);
			WRITEMSG
				return(-1);
		}
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(INDAY, lv_date(lv_pkgreg.in_date));
		strcpy(ORDERNO, lv_pkgreg.orderno);
		TXAMT = lv_pkgreg.tx_amt*100.0;
		strcpy(RCPSTAT, lv_pkgreg.rcpstat);
		strcpy(CONTENT, lv_pkgreg.lv_brf);
		strcpy(JZDAY, lv_date(lv_pkgreg.jz_date));
		/****NEWYX2:发送时切不可新产生序号****/

		/** 要素集内容 **/
		memcpy(pkg011.CMTNo,"006",3);       /***要素集号006***/
		pkg011.paymPRI='0';                 /***优先级***/
		lv_set_tag(&pkg011,"30A",INDAY);    /** 委托日期 **/
		lv_set_tag(&pkg011,"BSE",INDAY);   	/** 扣款日期 **/

		vtcp_log("[%s][%d]====NORDERNO=[%.8s]",__FILE__,__LINE__,NORDERNO);			
		lv_set_tag(&pkg011,"0BC",ORDERNO);	/**	支付交易序号 **/

		lv_set_tag(&pkg011,"0BH",TXNUM);	/**	原业务类型号 **/	
		/****NEWYX2:发起和接收行反了****/
		lv_set_tag(&pkg011,"CC1",ACBRNO);   /** 原发起行行号 **/
		lv_set_tag(&pkg011,"CC2",ORBRNO);   /** 原接收行行号 **/
		lv_set_tag(&pkg011,"051",WTDAY);    /** 原委托日期 **/
		/****NEWYX2:原支付交易序号****/
		lv_set_tag(&pkg011,"005",lv_pkgreg.o_orderno); 	/** 原支付交易序号 **/
		vtcp_log("[%s][%d]====TXAMT=[%.2f]\n",__FL__,TXAMT);
		memset(tmpBuf, '\0', sizeof(tmpBuf));
		sprintf(tmpBuf,"%015.0f",TXAMT);
		vtcp_log("[%s][%d]====tmpBuf=[%s]\n",__FL__,tmpBuf);
		lv_set_tag(&pkg011,"33S",tmpBuf);   	/** 金额 **/
		lv_set_tag(&pkg011,"CIA",RCPSTAT);   	/** 业务回执状态 **/
		lv_set_tag(&pkg011,"BSN","");   		/** 开户行手续费 **/
		lv_set_tag(&pkg011,"72A",CONTENT);  	/** 附言 **/
		if (memcmp(RCPSTAT,"00",2)==0)
		{
			/****同意应答****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			pcStat[0]=STAT_YES_SEND;
			g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%c' where pkgno = '%s' and lw_ind = '2' and  wt_date='%s' and ac_br_no='%s' and o_orderno='%s'", pcStat[0], PKGNO_DQJJ, WTDAY, ORBRNO, ORDERNO);
		}
		else
		{
			/****不同意应答****/
			char pcStat[2];
			memset(pcStat,'\0',sizeof(pcStat));
			pcStat[0]=STAT_NO_SEND;
			g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%s' where pkgno = '%s' and lw_ind = '2' and  wt_date='%s' and ac_br_no='%s' and o_orderno='%s'", pcStat, PKGNO_DQJJ, WTDAY, ORBRNO, ORDERNO);
		}
		lv_batch_end(&pkg011);
		if ( g_reply_int != DB_OK )
		{
			sprintf( acErrMsg,"更新数据库失败 ");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
				return(-1);
		}
	}
	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int !=DB_OK)
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE DQJJHZ;*/

	/** 包头内容 **/
	lv_pkg_batch(&pkg011,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg011,"011",Payqsactno);			/** 包发起清算行行号 **/
	lv_pkg_batch(&pkg011,"012",Cashqsactno);		/** 包接收清算行行号 **/
	lv_pkg_batch(&pkg011,"30E",packday);		/** 包委托日期 **/
	lv_pkg_batch(&pkg011,"0BD",wd_lv_wbctl.packid);		/** 包序号 **/
	/** lv_pkg_batch(&pkg011,"C15","");	包密押 **/

	sprintf(Cnt,"%08.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg011,"B63",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.totamt*100.0);
	lv_pkg_batch(&pkg011,"32B",tmpBuf);	/** 明细业务总金额 **/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	/***
	dbl2str(&wd_lv_wbctl.dtlsuccnt,0,sizeof(Cnt)-1,0,Cnt);
	****/
	apitoa(wd_lv_wbctl.dtlsuccnt,sizeof(Cnt)-1,Cnt);
	lv_pkg_batch(&pkg011,"B41",Cnt);	/** 明细业务总笔数 **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",wd_lv_wbctl.dtlsucamt*100.0);
	lv_pkg_batch(&pkg011,"32C",tmpBuf);	/** 明细业务总金额 **/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	lv_pkg_batch(&pkg011,"02D",wd_lv_wbctl.o_pkgno);		/** 原包类型号 **/
	lv_pkg_batch(&pkg011,"CCO",wd_lv_wbctl.cash_qs_no);/** 原包发起清算行 **/
	lv_pkg_batch(&pkg011,"30I",lv_date(wd_lv_wbctl.o_pack_date));	/** 原包委托日期 **/
	lv_pkg_batch(&pkg011,"0BE",wd_lv_wbctl.o_packid);	/** 原包序号 **/
	/***********
	lv_pkg_batch(&pkg011,"BS1",wd_lv_wbctl.zcnode);		** 轧差节点类型 **
	lv_pkg_batch(&pkg011,"BS2",wd_lv_wbctl.zcday);		** 轧差日期 **
	lv_pkg_batch(&pkg011,"BS3",wd_lv_wbctl.zcid);		** 轧差场次 **
	lv_pkg_batch(&pkg011,"BS4",wd_lv_wbctl.bfflag);		** 补发标志 **
	lv_pkg_batch(&pkg011,"BS5",wd_lv_wbctl.qsday);		** 清算日期 **
	lv_pkg_batch(&pkg011,"CIB",wd_lv_wbctl.packstat);		** 包处理状态 **
	lv_pkg_batch(&pkg011,"72D","");		** 包附加数据 暂无 **
	**********/

	lv_gen_pack(&pkg011);
	/**
	dm_show(&(pkg011.dm));
	**/
	Stat2[0] = STAT_WZSEND;
	/*EXEC SQL UPDATE lv_dqjjs set stat =:Stat2 where packid =:wd_lv_wbctl.o_packid and packday =:wd_lv_wbctl.o_pack_date and stat<>'0'; */
	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts <> '0' and packid = '%s' and pack_date = %ld and lw_ind = '1' and pkgno = '%s' " ,  wd_lv_wbctl.packid, wd_lv_wbctl.pack_date, PKGNO_DQJJHZ);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据不存在!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg011)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新往包控制表错误");

		lv_destroy(&pkg011);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&pkg011))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg011);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg011);

	return(0);
}
/** 拼通用信息业务发送包 PKG012 **/  
int lv_get_tyxx_tag(char *packday,char *packid)
{
	int		ret=0;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[9];
	char	Txday[9];
	char	tmpBuf[128];
	char	Pkgno[4];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	char    sql_stat[2];
	char    sql_stat2[2];
	TLVPACKAGE pkg012;
	PAY_IN_AREA payin;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	double		TXAMT;
	/*char    INFOLEN[9];*/
	char    CONTENT[256];

	memset(&payin, 0 , sizeof(payin));/* add by lhf0423 */
	memset(CONTENT,    '\0', sizeof(CONTENT));
	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,  '\0', sizeof(ORDERNO));
	/*memset(INFOLEN,  '\0', sizeof(INFOLEN));*/

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Pkgno, '\0', sizeof(Pkgno));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));
	memset(sql_stat, '\0', sizeof(sql_stat));
	memset(sql_stat2, '\0', sizeof(sql_stat2));

	lv_load_tagifno();
	lv_create(&pkg012);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );
	sql_stat[0]=STAT_WZLR;
	sql_stat2[0]=STAT_WZSEND;

	vtcp_log("[%s][%d]wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		strcpy( g_pub_tx.reply,"P071");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;     /** 得到整数从double变到int **/
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, lv_date(wd_lv_wbctl.pack_date), sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg012.opeType = '1';	/** 小额 **/
	pkg012.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg012.packDate , lv_date(wd_lv_wbctl.pack_date), 8);
	pkg012.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg012.CMTCode,PKGNO_TYXX,3);	/** 因为是 PKG012包**/
	memcpy(pkg012.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	/*======================================================================
	EXEC SQL DECLARE TYXX1 CURSOR FOR 
	select txnum,orbrno,acbrno,wtday,orderno,infolen,content from lv_tyxxs where packid =:packid 
	and packday=:packday and stat =:sql_stat;

	EXEC SQL OPEN TYXX1;
	*======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s'", PKGNO_TYXX, packid, packday, sql_stat);
	if ( g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	for (wd_dtlcnt;wd_dtlcnt!=0;wd_dtlcnt--)
	{
		g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);
		vtcp_log("[%s][%d+++++++++++++++wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);
		/*		EXEC SQL FETCH TYXX1 INTO :TXNUM,:ORBRNO,:ACBRNO,:WTDAY,:ORDERNO,:INFOLEN,:CONTENT;*/

		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"访问数据库错误!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg012);
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
			sprintf( acErrMsg,"没有可拼包的记录!");
			set_zd_data("0130",acErrMsg);
			lv_destroy(&pkg012);
			WRITEMSG
				return(-1);
		}
		strcpy(TXNUM, lv_pkgreg.txnum);
		strcpy(ORBRNO, lv_pkgreg.or_br_no);
		strcpy(ACBRNO, lv_pkgreg.ac_br_no);
		strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
		strcpy(ORDERNO, lv_pkgreg.orderno);
		/*strcpy(INFOLEN, lv_pkgreg.infolen);*/
		strcpy(CONTENT, lv_pkgreg.lv_brf);

		/** 要素集内容 **/

		memcpy(pkg012.CMTNo,"005",3);       /***要素集号005***/
		pkg012.paymPRI='0';                 /***优先级***/

		memset(tmpBuf, '\0', sizeof(tmpBuf));
		lv_set_tag(&pkg012,"0BG",TXNUM);	/** 业务类型号 **/
		lv_set_tag(&pkg012,"52A",ORBRNO);	/** 发起行行号 **/
		lv_set_tag(&pkg012,"58A",ACBRNO);	/** 接收行行号 **/
		lv_set_tag(&pkg012,"30A",WTDAY);	/**	委托日期 **/
		lv_set_tag(&pkg012,"0BC",ORDERNO);	/**	信息业务序号(同支付交易序号) **/
		/** 拼附加数据 **/
		if (memcmp(TXNUM,TXNUM_DS,sizeof(TXNUM)-1)==0) /** 代收业务 **/
		{
			lv_set_tag(&pkg012,"B40","00000000");
		}
		else  if(memcmp(TXNUM,TXNUM_DF,sizeof(TXNUM)-1)==0) /** 代付业务 **/
		{
			lv_set_tag(&pkg012,"B40","00000000");
		}
		else     /** 一般信息服务 **/
		{
			memset(tmpBuf,'\0',sizeof(tmpBuf));
			memcpy(payin.F_wtday, WTDAY,sizeof(payin.F_wtday));
			memcpy(payin.F_orbrno,ORBRNO,sizeof(payin.F_orbrno));
			memcpy(payin.F_orderno,ORDERNO,sizeof(payin.F_orderno));
			iLvGetAddit(&payin,"000",tmpBuf,0);
			if (tmpBuf[0]=='\0')
			{
				/****没有附加数据****/
				char pcAutoAdd[384];
				memset(pcAutoAdd,' ',sizeof(pcAutoAdd));
				pcAutoAdd[sizeof(pcAutoAdd)-1]='\0';
				memcpy(pcAutoAdd,"NONE",4);
				memcpy(pcAutoAdd+60,CONTENT,strlen(CONTENT));
				if (CONTENT[0]==' '||CONTENT[0]=='\0')
				{
					memcpy(pcAutoAdd+60,"NONE",4);
				}	
				memcpy(pcAutoAdd+375,"00000000",8);

				lv_set_tag(&pkg012,"B40","00000383");   /** 附加数据长度**/
				lv_set_tag(&pkg012,"72C",pcAutoAdd);   /** 附加数据**/
			}else
			{
				/****有附加数据****/
				char len_buf[9];
				sprintf(len_buf,"%08d",strlen(tmpBuf));
				lv_set_tag(&pkg012,"B40",len_buf);  /** 附加数据长度**/
				lv_set_tag(&pkg012,"72C",tmpBuf);       /** 附加数据 **/
			}
		}
		lv_batch_end(&pkg012);
	}
	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/*EXEC SQL CLOSE TYXX1;*/

	/** 包头内容 **/
	lv_pkg_batch(&pkg012,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg012,"C36",wd_lv_wbctl.pay_qs_no);			/** 包发起节点号 **/
	lv_pkg_batch(&pkg012,"C37",wd_lv_wbctl.cash_qs_no);		/** 包接收节点号 **/
	lv_pkg_batch(&pkg012,"30E",packday);/** 包委托日期 **/
	lv_pkg_batch(&pkg012,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	lv_pkg_batch(&pkg012,"BS7",wd_lv_wbctl.ffflag);	/** 辖内分发标志 **/
	lv_pkg_batch(&pkg012,"0BG",wd_lv_wbctl.txnum);	/** 业务类型号 **/

	sprintf(Cnt,"%04.0f",(double)wd_lv_wbctl.dtlcnt);
	lv_pkg_batch(&pkg012,"B42",Cnt);    /** 明细业务总笔数 **/

	/** lv_pkg_batch(&pkg012,"72D","");  				** 包附加数据 **/

	vtcp_log("[%s][%d]==========\n",__FL__);

	lv_gen_pack(&pkg012);
	/**
	dm_show(&(pkg012.dm));
	**/
	/** 更新状态为已发送 **/
	/*EXEC SQL UPDATE lv_tyxxs set stat =:sql_stat2 where packid =:packid and packday=:packday
	and stat =:sql_stat;*/
	g_reply_int = lv_Upd_Pkgreg_Stat(sql_stat2[0], "lv_sts='%s' and packid ='%s' and pack_date=%s and lw_ind='1' and pkgno='%s' and pay_qs_no='%s'" , sql_stat, packid, packday, PKGNO_TYXX,wd_lv_wbctl.pay_qs_no);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"没有可拼包的记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包控制表错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg012)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新发送登记簿错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	/** 发包 **/
	vtcp_log("%s,%d,发包\n",__FL__);
	if (iRc=iLvSendPackage(&pkg012))
	{
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg012);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg012);
	return(0);
}
/** pkg013 **/
/** 实时信息业务发送包 **/
/** 传入参数为包id **/
int lv_get_ssxx_tag(char *packday,char *packid)
{
	int		ret;
	int		iRc=0;
	long     wd_dtlcnt=0;
	char	Cnt[5];
	char	Txday[9];
	char	Pkgno[4];
	char	tmpBuf[840];
	char	Payqsactno[13];
	char	Cashqsactno[13];
	TLVPACKAGE pkg013;
	static struct lv_wbctl_c      wd_lv_wbctl;

	/** 游标所用变量 **/
	char	TXNUM[6];
	char	ORBRNO[13];
	char	ACBRNO[13];
	char	WTDAY[9];
	char	ORDERNO[9];
	char	ADDID[9];
	char	Stat[2];
	char	Stat2[2];

	memset(Stat, '\0', sizeof(Stat));
	memset(Stat2, '\0', sizeof(Stat2));

	memset(TXNUM,    '\0', sizeof(TXNUM));
	memset(ORBRNO,   '\0', sizeof(ORBRNO));
	memset(ACBRNO,   '\0', sizeof(ACBRNO));
	memset(WTDAY,    '\0', sizeof(WTDAY));
	memset(ORDERNO,	 '\0', sizeof(ORDERNO));
	memset(ADDID,	 '\0', sizeof(ADDID));

	memset(Cnt,    '\0', sizeof(Cnt));
	memset(Pkgno,  '\0', sizeof(Pkgno));
	memset(tmpBuf, '\0', sizeof(tmpBuf));
	memset(Txday,  '\0', sizeof(Txday));
	memset(Payqsactno,    '\0', sizeof(Payqsactno));
	memset(Cashqsactno,   '\0', sizeof(Cashqsactno));

	lv_load_tagifno();
	lv_create(&pkg013);
	vtcp_log("[%s][%d]begin lv_get_ptdj_tag\n",__FL__);
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	wd_lv_wbctl.pack_date = apatoi( packday, 8 );
	memcpy(wd_lv_wbctl.packid,  packid,	 sizeof(wd_lv_wbctl.packid) -1 );

	vtcp_log("[%s][%d]=wd_lv_wbctl.pack_date=[%d]\n",__FL__,wd_lv_wbctl.pack_date);
	vtcp_log("[%s][%d]=wd_lv_wbctl.packid=[%s]\n",__FL__,wd_lv_wbctl.packid);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %ld and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	if (g_reply_int!= DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"未查到对应的往包记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	/** 状态是已组包才可以发送 **/
	if ( wd_lv_wbctl.stat[0] != PSTAT_PACKED )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,wd_lv_wbctl.stat[0]);
		sprintf( acErrMsg,"记录状态错误!");
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply,"P071");
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	wd_dtlcnt = wd_lv_wbctl.dtlcnt;
	vtcp_log("[%s][%d]==========wd_dtlcnt=[%d]\n",__FL__,wd_dtlcnt);

	/** 给游标需要的字段赋值 **/
	memcpy(Txday, packday, sizeof(Txday)-1);
	memcpy(Payqsactno, wd_lv_wbctl.pay_qs_no, sizeof(Payqsactno)-1);
	memcpy(Cashqsactno, wd_lv_wbctl.cash_qs_no, sizeof(Cashqsactno)-1);
	memcpy(Pkgno, wd_lv_wbctl.pkgno, sizeof(Pkgno)-1);

	/** 开始拼报文 **/
	pkg013.opeType = '1';	/** 小额 **/
	pkg013.pkgType = 1;		/** 是pkg包 **/
	memcpy(pkg013.packDate, pcLvGetDay(), sizeof(wd_lv_wbctl.pack_date) -1);
	pkg013.packID = apatoi(wd_lv_wbctl.packid,sizeof(wd_lv_wbctl.packid)-1);

	memcpy(pkg013.CMTCode,"013",3);	/** 因为是PKG013包**/
	memcpy(pkg013.CMTNo,"005",3);       /***要素集号005***/
	memcpy(pkg013.workDate,pcLvGetDay_work(),8);

	/** 开始查找对应明细表中的记录 **/
	Stat[0]=STAT_WZLR;
	/*======================================================================
	EXEC SQL DECLARE SSXX CURSOR FOR 
	SELECT  txnum, orbrno,acbrno, wtday,orderno,addid
	FROM  lv_ssxxs where payqsactno=:wd_lv_wbctl.pay_qs_no and  packid =:packid and packday = :packday and sts =Stat;

	EXEC SQL OPEN SSXX;

	EXEC SQL FETCH SSXX INTO :TXNUM, :ORBRNO, :ACBRNO, :WTDAY, :ORDERNO,:ADDID;
	*======================================================================*/

	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "pkgno = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and lv_sts = '%s' and pay_qs_no = '%s'", PKGNO_SSXX, packid, packday, Stat, wd_lv_wbctl.pay_qs_no);
	if ( g_reply_int != DB_OK)
	{
		sprintf( acErrMsg,"定义查询游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_pkgreg_Fet_Sel(&lv_pkgreg, g_pub_tx.reply);

	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据库无此记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	strcpy(TXNUM, lv_pkgreg.txnum);
	strcpy(ORBRNO, lv_pkgreg.or_br_no);
	strcpy(ACBRNO, lv_pkgreg.ac_br_no);
	strcpy(WTDAY, lv_date(lv_pkgreg.wt_date));
	strcpy(ORDERNO, lv_pkgreg.orderno);
	strcpy(ADDID, lv_pkgreg.addid);

	/** 要素集内容 **/
	lv_set_tag(&pkg013,"0BG",TXNUM);	/** 业务类型号 **/
	lv_set_tag(&pkg013,"52A",ORBRNO);   /** 发起行行号 **/
	lv_set_tag(&pkg013,"58A",ACBRNO);   /** 接收行行号 **/
	lv_set_tag(&pkg013,"30A",WTDAY);    /** 委托日期 **/
	lv_set_tag(&pkg013,"0BC",ORDERNO);	/**	支付交易序号 **/	

	/** 拼附加数据 **/
	/***
	if(memcmp(TXNUM,TXNUM_ZHCX,sizeof(TXNUM)-1)==0) ** 客户账户查询 **
	{
	lv_set_tag(&pkg013,"B40","00000000");
	}
	else  if(memcmp(TXNUM,TXNUM_CXYD,sizeof(TXNUM)-1)==0) ** 客户账户查询应答 **
	{
	lv_set_tag(&pkg013,"B40","00000000");
	}
	else
	{
	***/
	/** 帐号查询和应答ssxxs表中附加字段不全,暂时按如下取值 **/
	memset(tmpBuf,'\0',sizeof(tmpBuf));
	iLvGetAddit_Notpay(WTDAY,ADDID,"000",tmpBuf,0);
	if (tmpBuf[0]=='\0')
	{
		/****没有附加数据****/
		lv_set_tag(&pkg013,"B40","00000000");   /** 附加数据长度**/
	}
	else
	{
		/****有附加数据****/
		char len_buf[9];
		vtcp_log("%s,%d 陈华为注意这里，拼包程序这里没有改 ",__FILE__,__LINE__);
		vtcp_log("%s,%d tmpbuf=[%s]",__FILE__,__LINE__,tmpBuf);
		sprintf(len_buf,"%08d",strlen(tmpBuf));
		vtcp_log("%s,%d tmpbuf+181=[%s]",__FILE__,__LINE__,tmpBuf+181);
		/** 新的包文改为173
		if (memcmp(tmpBuf+181,"04",2)==0)
		**/
		if (memcmp(tmpBuf+173,"04",2)==0)
		{
			/****模拟支票圈存的影像数据****/
			char pcPicture[1024*2];
			int  nBlockCnt=50;
			int i;
			memset(pcPicture,'\0',sizeof(pcPicture));
			sprintf(len_buf,"%08d",strlen(tmpBuf)+nBlockCnt*1000);
			lv_set_tag(&pkg013,"B40",len_buf);  /** 附加数据长度**/
			sprintf(pcPicture+strlen(tmpBuf)-8,"%08d",nBlockCnt*1000);
			lv_set_tag(&pkg013,"72C",tmpBuf);       /** 附加数据 **/
			memset(pcPicture,'0',1000);
			for (i=0;i<nBlockCnt;i++)
				lv_set_tag_multi(&pkg013,"72C",pcPicture);   /** 附加数据 **/
		}
		else
		{ 
			lv_set_tag(&pkg013,"B40",len_buf);  /** 附加数据长度**/
			lv_set_tag(&pkg013,"72C",tmpBuf);       /** 附加数据 **/
		}
	}
	/**	lv_set_tag(&pkg013,"B40",);	信息长度 **/	
	/**	lv_set_tag(&pkg013,"72C",);	信息内容 **/

	lv_batch_end(&pkg013);

	/*EXEC SQL CLOSE SSXX;*/

	g_reply_int=Lv_pkgreg_Clo_Sel();
	if (g_reply_int != DB_OK )
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	/** 包头内容 **/
	lv_pkg_batch(&pkg013,"02C",wd_lv_wbctl.pkgno);	/** 包类型号 **/
	lv_pkg_batch(&pkg013,"C36",Payqsactno);			/** 包发起节点号 **/
	lv_pkg_batch(&pkg013,"C37",Cashqsactno);		/** 包接收节点号 **/
	lv_pkg_batch(&pkg013,"30E",packday);/** 包委托日期 **/
	lv_pkg_batch(&pkg013,"0BD",wd_lv_wbctl.packid);	/** 包序号 **/
	lv_pkg_batch(&pkg013,"BS7",wd_lv_wbctl.ffflag);	/** 辖内分发标志 **/
	lv_pkg_batch(&pkg013,"0BG",wd_lv_wbctl.txnum);	/** 业务类型号 **/
	sprintf(Cnt,"%04d",wd_lv_wbctl.dtlcnt);
	vtcp_log("[%s][%d]========cnt=[%s]\n",__FL__,Cnt);
	lv_pkg_batch(&pkg013,"B42",Cnt);    /** 明细业务总笔数 **/
	/** lv_pkg_batch(&pkg013,"72D","");		包附加数据 暂无 **/

	lv_gen_pack(&pkg013);
	/**
	dm_show(&(pkg013.dm));
	**/
	Stat2[0] = STAT_WZSEND;
	/*EXEC SQL UPDATE lv_ssxxs set stat =:Stat2 where packid =:packid and packday =:packday and stat =:Stat;*/

	g_reply_int = lv_Upd_Pkgreg_Stat(STAT_WZSEND, "lv_sts = '%s' and packid = '%s' and pack_date = %s and lw_ind = '1' and pkgno = '%s' " , Stat, packid, packday, PKGNO_SSXX);
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"访问数据库错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"数据没有找到!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	wd_lv_wbctl.stat[0] = PSTAT_SEND;
	memcpy(wd_lv_wbctl.packstat,PACKSTAT_SEND,sizeof(wd_lv_wbctl.packstat));

	if ((g_reply_int = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"更新往包错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Lv_wbctl_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = lv_insSendList(&pkg013)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_insSendList[%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"更新发送登记簿错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&pkg013);
		WRITEMSG
			return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&pkg013))
	{
		lv_destroy(&pkg013);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&pkg013);

	return(0);
}
/** 自由格式报文发送 **/
/** 传入参数是查询书号 **/
int lv_get_cmt303_tag(char *packday,char *orderno)
{
	int     	ret;
	int		iRc=0;
	TLVPACKAGE 	CMT303;

	static	NOTPAY_IN_AREA	notpay_in;
	static struct hv_fzf_c      hv_fzf;

	memset(&notpay_in, '\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT303);
	/* cmt类的方到 hv_fzf 表中 key: otype,or_br_no,wt_date,orderno,cmtno*/
	if (GetOr_br_noByBr_no(g_pub_tx.tx_br_no, hv_fzf.or_br_no))
	{
		vtcp_log("[%s][%d]得到参与者号错误!",__FL__);
		vtcp_log("得到参与者号错误!");
		lv_destroy(&CMT303);
		return(-1);
	}
	hv_fzf.wt_date = apatoi( packday, 8 );
	memcpy(hv_fzf.orderno, orderno, sizeof(hv_fzf.orderno) -1 );
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '303' and lw_ind='1'  ",  hv_fzf.or_br_no,hv_fzf.wt_date, hv_fzf.orderno))
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c303s error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT303);
		return(-1);
	}
	vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf.hv_fzf_sts[0]);
	/** 状态是复核才可以发送 **/
	if ( hv_fzf.hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf.hv_fzf_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT303);
		return(-1);
	}
	/** 开始拼报文 **/

	CMT303.opeType = '1';   /** 小额 **/
	CMT303.pkgType = 0;     /** 是cmt包 **/
	memcpy(CMT303.packDate , lv_date( hv_fzf.wt_date), 8);
	CMT303.packID = apatoi(hv_fzf.orderno,sizeof(hv_fzf.orderno)-1);

	memcpy(CMT303.CMTCode,"303",3);
	memcpy(CMT303.workDate,pcLvGetDay_work(),8);

	/** 开始组包 **/
	lv_set_tag(&CMT303,"010",hv_fzf.sendco);	/** 发报中心代码 **/
	lv_set_tag(&CMT303,"011",hv_fzf.pay_qs_no);	/** 发起清算行行号 **/
	lv_set_tag(&CMT303,"52A",hv_fzf.or_br_no);	/** 发起行行号 **/
	lv_set_tag(&CMT303,"30A",lv_date(hv_fzf.wt_date));	/** 委托日期 **/
	lv_set_tag(&CMT303,"0B9",hv_fzf.receco);	/** 收报中心代码 **/
	if (hv_fzf.cash_qs_no[0]!='\0')	
	{
		lv_set_tag(&CMT303,"012",hv_fzf.cash_qs_no);	/** 接收清算行行号  **/
		lv_set_tag(&CMT303,"58A",hv_fzf.ac_br_no);	/** 接收行行号  **/
	}
	lv_set_tag(&CMT303,"053",hv_fzf.req_content);	/** 内容  **/

	lv_gen_pack(&CMT303);
	dm_show(&(CMT303.dm));

	/** 组notpay_in **/
	memcpy(notpay_in.cmtno,		"303",					sizeof(hv_fzf.cmtno)-1);
	memcpy(notpay_in.orderno,	hv_fzf.orderno,	sizeof(hv_fzf.orderno)-1);
	memcpy(notpay_in.orbrno,	hv_fzf.or_br_no,		sizeof(hv_fzf.or_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf.pay_qs_no,	sizeof(hv_fzf.pay_qs_no)-1);
	memcpy(notpay_in.acbrno,	hv_fzf.ac_br_no,		sizeof(hv_fzf.ac_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf.cash_qs_no,sizeof(hv_fzf.cash_qs_no)-1);
	/** txamt **/
	/** opkgno **/
	/** owtday **/
	/** oorderno **/
	memcpy(notpay_in.content,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);

	/** 插入发送记录 **/
	/*****************
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	vtcp_log("插入单包交易历史明细错误");
	lv_destroy(&CMT303);
	return(-1);
	}

	ret=lv_Recv_Ins_Tab(&CMT303,&notpay_in,1);
	if(ret)
	{ 
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
	vtcp_log("插入小额报文发送接收历史登记簿错误");
	lv_destroy(&CMT303);
	return(-1);
	}********************/

	hv_fzf.hv_fzf_sts[0] = STAT_WZSEND;
	memcpy(hv_fzf.sendco, lvca.tlrno, sizeof(hv_fzf.sendco)-1);

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT303);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("发包\n");
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn303.dm.length===[%d]\n",CMT303.dm.length);
	dm_show(&CMT303.dm);
	if (iRc=iLvSendPackage(&CMT303))
	{
		lv_destroy(&CMT303);
		return(-1);
	}
	lv_destroy(&CMT303);

	return(0);

}
/** 小额业务撤销报文发送 **/
int lv_get_cmt321_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT321;
	static	NOTPAY_IN_AREA	notpay_in;

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);

	memset(&notpay_in, '\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT321);

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '321' and lw_ind='1'  ",  hv_fzf->or_br_no, hv_fzf->wt_date, hv_fzf->orderno))
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT321);
		return(-1);
	}
	/** 状态是复核才可以发送 **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR)
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT321);
		return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/** 开始拼报文 **/
	CMT321.opeType = '1';   /** 小额 **/
	CMT321.pkgType = 0;     /** 是cmt包 **/

	memcpy(CMT321.CMTCode,"321",3);
	memcpy(CMT321.workDate,pcLvGetDay_work(),8);
	memcpy(CMT321.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT321.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	/** 开始组包 **/
	lv_set_tag(&CMT321,"30A",lv_date(hv_fzf->wt_date));		/** 委托日期 **/
	lv_set_tag(&CMT321,"011",hv_fzf->pay_qs_no);	/** 撤销申请清算行行号 **/
	lv_set_tag(&CMT321,"0BD",hv_fzf->orderno);		/** 撤销申请序号 **/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	lv_set_tag(&CMT321,"02D",hv_fzf->o_cmtno);		/** 原包类型号 **/
	lv_set_tag(&CMT321,"30I",lv_date(hv_fzf->o_pack_date));	/** 包委托日期 **/
	lv_set_tag(&CMT321,"0BE",hv_fzf->o_packid);		/** 包序号 **/
	/** lv_set_tag(&CMT321,"C15","");			密押 **/
	lv_set_tag(&CMT321,"72A",hv_fzf->req_content);	/** 撤销附言 **/

	lv_gen_pack(&CMT321);
	dm_show(&(CMT321.dm));

	/** 组notpay_in **/
	memcpy(notpay_in.cmtno,     "321",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no, sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->res_br_no,    sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpay_in.opkgno,	hv_fzf->o_cmtno,		sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday,	lv_date(hv_fzf->o_wt_date),8);
	memcpy(notpay_in.oorderno,	hv_fzf->o_orderno,	sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,	hv_fzf->filler,		sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	/***************
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	vtcp_log("插入单包交易历史明细错误");
	lv_destroy(&CMT321);
	return(-1);
	}
	**************************/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	ret=lv_Recv_Ins_Tab(&CMT321,&notpay_in,1);
	if (ret)	
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		vtcp_log("插入小额报文发送接收历史登记簿错误");
		lv_destroy(&CMT321);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT321);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&CMT321))
	{
		lv_destroy(&CMT321);
		return(-1);
	}
	lv_destroy(&CMT321);

	return(0);

}
/** 实时业务冲正申请报文发送 **/
int lv_get_cmt323_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT323;
	static	NOTPAY_IN_AREA	notpay_in;

	memset(&notpay_in, '\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT323);
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '323' and lw_ind='1' ",  hv_fzf->req_br_no,  hv_fzf->wt_date, hv_fzf->orderno ))
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT323);
		return(-1);
	}
	/** 状态是复核才可以发送 **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR)
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT323);
		return(-1);
	}
	/** 开始拼报文 **/
	CMT323.opeType = '1';   /** 小额 **/
	CMT323.pkgType = 0;     /** 是cmt包 **/
	memcpy(CMT323.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT323.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT323.CMTCode,"323",3);
	memcpy(CMT323.workDate,pcLvGetDay_work(),8);

	/** 开始组包 **/
	lv_set_tag(&CMT323,"30A",lv_date(hv_fzf->wt_date));		/** 冲正委托日期 **/
	lv_set_tag(&CMT323,"011",hv_fzf->pay_qs_no);	/** 冲正申请清算行行号 **/
	lv_set_tag(&CMT323,"0BD",hv_fzf->orderno);		/** 冲正申请序号 **/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	lv_set_tag(&CMT323,"02D",hv_fzf->o_cmtno);		/** 原包类型号 **/
	lv_set_tag(&CMT323,"30I",lv_date(hv_fzf->o_pack_date));	/** 原包委托日期 **/
	lv_set_tag(&CMT323,"0BE",hv_fzf->o_packid);		/** 原包序号 **/
	/** lv_set_tag(&CMT323,"C15","");			密押 **/
	lv_set_tag(&CMT323,"72A",hv_fzf->req_content);	/** 冲正附言 **/

	lv_gen_pack(&CMT323);
	dm_show(&(CMT323.dm));

	/** 组notpay_in **/
	memcpy(notpay_in.cmtno,     "323",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no, sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpay_in.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday,    lv_date(hv_fzf->o_wt_date), 8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	/*****
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	vtcp_log("插入单包交易历史明细错误");
	lv_destroy(&CMT323);
	return(-1);
	}
	***********************************/
	ret=lv_Recv_Ins_Tab(&CMT323,&notpay_in,1);
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		vtcp_log("插入小额报文发送接收历史登记簿错误");
		lv_destroy(&CMT323);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT323);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&CMT323))
	{
		lv_destroy(&CMT323);
		return(-1);
	}
	lv_destroy(&CMT323);

	return(0);

}
/** 借记业务止付申请报文发送 **/
int lv_get_cmt327_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT327;

	static	NOTPAY_IN_AREA	notpay_in;

	memset(&notpay_in,	0x00,	 sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT327);
	/****NEWYX:增加来往标识****/
	lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '327' and lw_ind='1' ", hv_fzf->or_br_no,  hv_fzf->wt_date, hv_fzf->orderno);
	if (lvca.rtcode )
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT327);
		return(-1);
	}
	/** 状态是复核才可以发送 **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT327);
		return(-1);
	}
	/** 开始拼报文 **/
	CMT327.opeType = '1';   /** 小额 **/
	CMT327.pkgType = 0;     /** 是cmt包 **/
	memcpy(CMT327.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT327.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT327.CMTCode,"327",3);
	memcpy(CMT327.workDate,pcLvGetDay_work(),8);


	/** 开始组包 单笔**/
	if (hv_fzf->reqtype[0]=='0')
	{
		vtcp_log("[%s][%d]开始单笔拼包!\n",__FILE__,__LINE__);
		lv_set_tag(&CMT327,"52A",hv_fzf->req_br_no);		/** 止付申请行 **/
		lv_set_tag(&CMT327,"58A",hv_fzf->res_br_no);		/** 止付应答行 **/
		lv_set_tag(&CMT327,"0BH",hv_fzf->otxnum);		/** 原业务类型号 **/
		lv_set_tag(&CMT327,"051",lv_date(hv_fzf->o_wt_date));		/** 原委托日期 **/
		lv_set_tag(&CMT327,"005",hv_fzf->o_orderno);		/** 原支付交易序号 **/

	}/**正包**/
	else if (hv_fzf->reqtype[0]=='1')
	{
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		lv_set_tag(&CMT327,"02D",hv_fzf->o_cmtno);		/** 原包类型号 **/
		lv_set_tag(&CMT327,"30I",lv_date(hv_fzf->o_pack_date));	/** 原包委托日期 **/
		lv_set_tag(&CMT327,"0BE",hv_fzf->o_packid);		/** 原包序号 **/
	}
	
	
	lv_set_tag(&CMT327,"30A",lv_date(hv_fzf->wt_date));		/** 止付委托日期 **/
	lv_set_tag(&CMT327,"011",hv_fzf->pay_qs_no);	/** 止付申请清算行 **/

	lv_set_tag(&CMT327,"0BD",hv_fzf->orderno);		/** 止付申请序号 **/
	lv_set_tag(&CMT327,"012",hv_fzf->cash_qs_no);	/** 止付应答清算行 **/

	lv_set_tag(&CMT327,"BS8",hv_fzf->reqtype);		/** 止付类型 **/

	/** lv_set_tag(&CMT327,"C15","");			密押 **/
	lv_set_tag(&CMT327,"72A",hv_fzf->req_content);	/** 止付应答附言 **/

	lv_gen_pack(&CMT327);
	dm_show(&(CMT327.dm));

	/** 组notpay_in **/
	memcpy(notpay_in.cmtno,     "327",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->req_br_no,   	sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no,	sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->res_br_no,    sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpay_in.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday , lv_date(    hv_fzf->o_wt_date),  8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	ret=lv_Ins_Cmthtr(&notpay_in);
	if (ret)
	{
		vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
		vtcp_log("插入单包交易历史明细错误");
		lv_destroy(&CMT327);
		return(-1);
	}
	ret=lv_Recv_Ins_Tab(&CMT327,&notpay_in,1);
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		vtcp_log("插入小额报文发送接收历史登记簿错误");
		lv_destroy(&CMT327);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT327);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&CMT327))
	{
		lv_destroy(&CMT327);
		return(-1);
	}
	lv_destroy(&CMT327);

	return(0);

}
/** CMT316小额批量包处理状态通知报文发送 **/
int lv_get_cmt316_tag(char *packday,int dtlid,char *opackid)
{
	int     ret;
	return(0);

}
/** 查询报文发送 cmt301  **/
/** 传入参数是查询书号和查询行行号 **/
int lv_get_cmt301_tag(struct hv_fzf_c *hv_fzf)
{
	int     	ret;
	int		iRc=0;
	char    	tmpBuf[30];               /* 用于生成原货币符号、金额 */
	TLVPACKAGE 	CMT301;

	static  NOTPAY_IN_AREA  	notpayin;

	memset(&notpayin,	0x00 , sizeof(notpayin));

	memset(tmpBuf,		0x00 , sizeof(tmpBuf));

	lv_load_tagifno();
	lv_create(&CMT301);
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '301' and lw_ind='1'  ",  hv_fzf->or_br_no,		hv_fzf->req_date,hv_fzf->orderno))
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c301s error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT301);
		return(-1);
	}
	/** 判断状态**/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT301);
		return(-1);
	}
	/** 开始拼报文 **/
	CMT301.opeType = '1';   /** 小额 **/
	CMT301.pkgType = 0;     /** 是cmt包 **/
	memcpy(CMT301.packDate,lv_date( hv_fzf->wt_date), 8);
	CMT301.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT301.CMTCode,"301",3);
	memcpy(CMT301.workDate,pcLvGetDay_work(),8);

	/** 开始组包 **/
	lv_set_tag(&CMT301,"CJ9",lv_date(hv_fzf->wt_date));   /** 查询日期 **/
	lv_set_tag(&CMT301,"010",hv_fzf->sendco);	/** 发报中心代码 **/
	lv_set_tag(&CMT301,"011",hv_fzf->pay_qs_no);	/** 发起清算行行号 **/
	lv_set_tag(&CMT301,"01C",hv_fzf->or_br_no);   /** 查询行行号 **/
	lv_set_tag(&CMT301,"056",hv_fzf->orderno);   /** 查询书号 **/
	lv_set_tag(&CMT301,"0B9",hv_fzf->receco);   /** 收报中心代码 **/
	lv_set_tag(&CMT301,"012",hv_fzf->cash_qs_no);  /** 接收清算行行号 **/
	lv_set_tag(&CMT301,"01D",hv_fzf->ac_br_no);  /** 查复行行号 **/
	lv_set_tag(&CMT301,"051",lv_date(hv_fzf->o_wt_date));  /** 原委托日期 **/
	lv_set_tag(&CMT301,"CC1",hv_fzf->o_or_br_no);  /** 原发起行行号 **/
	lv_set_tag(&CMT301,"CC2",hv_fzf->o_ac_br_no);  /** 原接收行行号 **/
	lv_set_tag(&CMT301,"CE2",hv_fzf->otype);  /** 原交易种类 **/
	lv_set_tag(&CMT301,"005",hv_fzf->o_orderno);  /** 原支付交易序号 **/

	vtcp_log("[%s][%d]o_tx_amt==[%.2f]\n",__FILE__,__LINE__,hv_fzf->o_tx_amt);
	vtcp_log("[%s][%d]o_tx_amt==[%f]\n",__FILE__,__LINE__,hv_fzf->o_tx_amt*100);
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",hv_fzf->o_tx_amt*100);
	vtcp_log("[%s][%d]tmpbuf==[%s]\n",__FILE__,__LINE__,tmpBuf);
	vtcp_log("[%s][%d]tmpbuf==[%s]\n",__FILE__,__LINE__,tmpBuf);

	lv_set_tag(&CMT301,"CND",tmpBuf);  /** 原货币符号、金额 **/
	lv_set_tag(&CMT301,"053",hv_fzf->req_content);	/** 查询内容  **/

	lv_gen_pack(&CMT301);
	dm_show(&(CMT301.dm));

	/** 组notpayin **/
	memcpy(notpayin.cmtno,     "301",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpayin.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpayin.orbrno,    hv_fzf->req_br_no,   sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpayin.payqsactno,hv_fzf->pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpayin.acbrno,    hv_fzf->res_br_no,    sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpayin.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	sprintf(notpayin.otxamt,"%015f",hv_fzf->o_tx_amt);
	/** opkgno **/
	memcpy(notpayin.owtday,    lv_date(hv_fzf->o_wt_date), 8);
	memcpy(notpayin.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpayin.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	/*******
	ret=lv_Ins_Cmthtr(&notpayin);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	vtcp_log("插入单包交易历史明细错误");
	lv_destroy(&CMT301);
	return(-1);
	}************************************/
	ret=lv_insSendList(&CMT301); 
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		vtcp_log("插入小额报文发送接收历史登记簿错误");

		lv_destroy(&CMT301);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;
	memcpy(hv_fzf->send_tel, lvca.tlrno, sizeof(hv_fzf->send_tel)-1);

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c301 error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT301);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("发包\n");

	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	vtcp_log("!!!!!![%s][%d]cmtn301.dm.length===[%d]\n",CMT301.dm.length);
	if (iRc=iLvSendPackage(&CMT301))
	{
		lv_destroy(&CMT301);
		return(-1);
	}
	lv_destroy(&CMT301);
	return(0);

}
/** 小额业务退回申请报文 cmt319 **/
/** 传入参数是申请行和申请号 **/
int lv_get_cmt319_tag(struct hv_fzf_c *hv_fzf)
{
	int    	 	ret;
	int		iRc=0;
	TLVPACKAGE 	CMT319;

	static  NOTPAY_IN_AREA  notpayin;

	memset(&notpayin,	0x00 , sizeof(notpayin));

	lv_load_tagifno();
	lv_create(&CMT319);
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '319' and lw_ind='1'  ",  hv_fzf->or_br_no, hv_fzf->wt_date, hv_fzf->orderno) )
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT319);
		return(-1);
	}
	/** 状态是复核才可以发送 **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_WZLR )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->tx_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT319);
		return(-1);
	}
	/** 开始拼报文 **/
	CMT319.opeType = '1';   /** 小额 **/
	CMT319.pkgType = 0;     /** 是cmt包 **/
	memcpy(CMT319.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT319.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT319.CMTCode,"319",3);
	memcpy(CMT319.workDate,pcLvGetDay_work(),8);

	/** 开始组包 **/
	if (hv_fzf->reqtype[0]='0')
	{
		vtcp_log("[%s][%d]单笔交易拼包!\n",__FILE__,__LINE__);
		lv_set_tag(&CMT319,"52A",hv_fzf->req_br_no);	/*退回申请行 */
		lv_set_tag(&CMT319,"58A",hv_fzf->ac_br_no);    /**    退回应答行 **/
		lv_set_tag(&CMT319,"0BH",hv_fzf->otxnum);  /** 原业务类型号 **/
		lv_set_tag(&CMT319,"051",lv_date(hv_fzf->o_wt_date));  /**  原委托日期 **/
		lv_set_tag(&CMT319,"005",hv_fzf->o_orderno);  /**  原支付交易序号 **/

	}
	else if (hv_fzf->reqtype[0]=='1')
	{
		vtcp_log("[%s][%d]整包的业务拼包!\n",__FILE__,__LINE__);
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		lv_set_tag(&CMT319,"02D",hv_fzf->o_cmtno);  /** 原包类型号 **/
		lv_set_tag(&CMT319,"30I",lv_date(hv_fzf->o_pack_date));  /** 原包委托日期 **/
		lv_set_tag(&CMT319,"0BE",hv_fzf->o_packid);  /** 原包序号 **/
	}
	lv_set_tag(&CMT319,"30A",lv_date(hv_fzf->wt_date));   /** 委托日期 **/
	lv_set_tag(&CMT319,"011",hv_fzf->pay_qs_no);	/** 退回申请发报行 **/
	lv_set_tag(&CMT319,"0BD",hv_fzf->orderno);   /** 退回申请序号 **/
	lv_set_tag(&CMT319,"012",hv_fzf->cash_qs_no);   /** 退回应答清算行 **/
	lv_set_tag(&CMT319,"BS8",hv_fzf->reqtype);  /** 退回类型 **/

	/**  lv_set_tag(&CMT319,"C15",hv_fzf->miya);  ** 密押 **/
	lv_set_tag(&CMT319,"72A",hv_fzf->req_content);	/** 退回申请附言 **/

	lv_gen_pack(&CMT319);
	dm_show(&(CMT319.dm));

	/** 组notpayin **/
	memcpy(notpayin.cmtno,     "319",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpayin.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpayin.orbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpayin.payqsactno,hv_fzf->pay_qs_no, sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpayin.acbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpayin.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/** txamt **/
	memcpy(notpayin.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpayin.owtday,    lv_date(hv_fzf->o_wt_date),8);
	memcpy(notpayin.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpayin.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	/****
	ret=lv_Ins_Cmthtr(&notpayin);
	if(ret){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	set_zd_data("0130",acErrMsg);
	lv_destroy(&CMT319);
	return(-1);
	}
	**************************************************************/
	ret=lv_insSendList(&CMT319);
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT319);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("[%s][%d]更新往包控制表错误",__FL__);
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT319);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("[%s][%d]关闭游标出错",__FL__);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&CMT319))
	{
		lv_destroy(&CMT319);
		set_zd_data("0130",acErrMsg);
		return(-1);
	}
	lv_destroy(&CMT319);

	return(0);
}
/** 借记业务止付应答报文 cmt328 **/
/** 传入参数是申请行和申请号 **/
int lv_get_cmt328_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	TLVPACKAGE CMT328;

	static  NOTPAY_IN_AREA  notpayin;

	memset(&notpayin, '\0', sizeof(notpayin));

	lv_load_tagifno();
	lv_create(&CMT328);
	/****NEWYX:增加来往标识****/
	if (g_reply_int = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d  and orderno = '%s' and cmtno = '327' and lw_ind='2' ",  hv_fzf->or_br_no,hv_fzf->wt_date, hv_fzf->orderno ))
	{
		sprintf( acErrMsg,"定义游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	if ((g_reply_int = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_ctczs error [%d]\n",__FL__,g_reply_int);
		sprintf( acErrMsg,"未查到对应的记录!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	/** 状态是复核才可以发送 **/
	/** if( hv_fzf->hv_fzf_sts[0] != STAT_WZCHECK1 && hv_fzf->hv_fzf_sts[0] != STAT_WZCHECK2 ) **/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_LZRECV1 )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		sprintf( acErrMsg,"记录状态错误!");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	/** 开始拼报文 **/
	CMT328.opeType = '1';   /** 小额 **/
	CMT328.pkgType = 0;     /** 是cmt包 **/

	memcpy(CMT328.CMTCode,"328",3);
	memcpy(CMT328.workDate,pcLvGetDay_work(),8);
	memcpy(CMT328.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT328.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	/** 开始组包 **/
	lv_set_tag(&CMT328,"30A",lv_date(hv_fzf->res_date));   /** 委托日期 **/
	lv_set_tag(&CMT328,"011",hv_fzf->cash_qs_no);	/** 止付应答清算行 **/
	lv_set_tag(&CMT328,"52A",hv_fzf->res_br_no);	/** 止付应答行 **/
	lv_set_tag(&CMT328,"0BD",hv_fzf->res_orderno);   /** 止付应答序号 **/
	lv_set_tag(&CMT328,"051",lv_date(hv_fzf->req_date));  /** 原止付申请委托日期 **/
	lv_set_tag(&CMT328,"CCO",hv_fzf->pay_qs_no);   /** 原止付申请清算行 **/
	lv_set_tag(&CMT328,"CC1",hv_fzf->req_br_no);   /** 原止付申请行 **/
	lv_set_tag(&CMT328,"0BE",hv_fzf->req_orderno);  /** 原止付申请序号 **/
	lv_set_tag(&CMT328,"BSA",hv_fzf->reqtype);  /** 原止付类型号 **/
	/** lv_set_tag(&CMT328,"C15",hv_fzf->miya);  ** 密押 **/
	lv_set_tag(&CMT328,"BSB",hv_fzf->res_sts);  /** 止付应答 **/
	lv_set_tag(&CMT328,"72A",hv_fzf->res_content);	/** 止付应答附言 **/

	lv_gen_pack(&CMT328);
	dm_show(&(CMT328.dm));

	/** 组notpayin **/
	memcpy(notpayin.cmtno,     "328",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpayin.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpayin.orbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpayin.payqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	memcpy(notpayin.acbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpayin.cashqsactno,hv_fzf->pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
	/** txamt **/
	memcpy(notpayin.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpayin.owtday,    lv_date(hv_fzf->o_wt_date),8);
	memcpy(notpayin.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpayin.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	/***********************
	ret=lv_Ins_Cmthtr(&notpayin);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	sprintf( acErrMsg,"插入单包交易历史明细错误");
	set_zd_data("0130",acErrMsg);
	lv_destroy(&CMT328);
	WRITEMSG
	return(-1);
	}
	*********************************/
	ret=lv_insSendList(&CMT328);
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		sprintf( acErrMsg,"插入小额报文发送接收历史登记簿错误");
		set_zd_data("0130",acErrMsg);
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((g_reply_int = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,g_reply_int);
		set_zd_data("0130",acErrMsg);
		sprintf( acErrMsg,"更新往包控制表错误");
		lv_destroy(&CMT328);
		WRITEMSG
			return(-1);
	}
	if (g_reply_int = Hv_fzf_Clo_Upd())
	{
		sprintf( acErrMsg,"关闭游标出错 - %d", g_reply_int);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&CMT328))
	{
		lv_destroy(&CMT328);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return(-1);
	}
	lv_destroy(&CMT328);
	return(0);
}
/** 查复报文发送 **/
/** 传入参数是查询书号 **/
int lv_get_cmt302_tag(struct hv_fzf_c *hv_fzf )
{
	int		ret;
	int		iRc=0;
	char 		tmpBuf[1024];
	TLVPACKAGE 	CMT302;

	static  NOTPAY_IN_AREA  notpay_in;

	memset(&notpay_in,	0x00, 	sizeof(notpay_in));

	memset(tmpBuf,		0x00,	sizeof(tmpBuf));

	lv_load_tagifno();
	lv_create(&CMT302);
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno='301' and lw_ind='2'  ", hv_fzf->or_br_no, hv_fzf->wt_date, hv_fzf->orderno))
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_c302s error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT302);
		return(-1);
	}
	vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
	/** 状态是复核才可以发送 **change by liuyue 20051126*/
	if ( hv_fzf->hv_fzf_sts[0] != STAT_LZRECV1 )
	{
		vtcp_log("[%s][%d]-----stat=[%c]\n",__FL__,hv_fzf->hv_fzf_sts[0]);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT302);
		return(-1);
	}
	/** 开始拼报文 **/
	CMT302.opeType = '1';   /** 小额 **/
	CMT302.pkgType ='0';     /** 是cmt包 **/
	memcpy(CMT302.packDate, lv_date(hv_fzf->res_date), 8);
	CMT302.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);

	memcpy(CMT302.CMTCode,"302",3);
	memcpy(CMT302.workDate,pcLvGetDay_work(),8);

	/** 开始组包 **/
	lv_set_tag(&CMT302,"CJ1",lv_date(hv_fzf->res_date));	/** 查复日期 **/
	lv_set_tag(&CMT302,"010",hv_fzf->receco);	/** 发报中心 **/
	lv_set_tag(&CMT302,"011",hv_fzf->cash_qs_no);	/** 发起清算行 **/
	lv_set_tag(&CMT302,"01D",hv_fzf->res_br_no);	/** 查复行行号 **/
	lv_set_tag(&CMT302,"043",hv_fzf->res_orderno);	/** 查复书号 **/
	lv_set_tag(&CMT302,"0B9",hv_fzf->sendco);	/** 收报中心  **/
	lv_set_tag(&CMT302,"012",hv_fzf->pay_qs_no);	/** 接收清算行  **/
	lv_set_tag(&CMT302,"01C",hv_fzf->req_br_no);	/** 查询行行号  **/
	lv_set_tag(&CMT302,"CJA",lv_date(hv_fzf->req_date));		/** 原查询日期  **/
	lv_set_tag(&CMT302,"CP1",hv_fzf->req_orderno);	/** 原查询书号  **/
	lv_set_tag(&CMT302,"051",lv_date(hv_fzf->o_wt_date));	/** 原委托日期  **/
	lv_set_tag(&CMT302,"CC1",hv_fzf->o_or_br_no);	/** 原发起行行号  **/
	lv_set_tag(&CMT302,"CC2",hv_fzf->o_ac_br_no);	/** 原接收行行号  **/
	lv_set_tag(&CMT302,"CE2",hv_fzf->otype);	/** 原交易种类  **/
	lv_set_tag(&CMT302,"005",hv_fzf->o_orderno);	/** 原支付交易序号  **/
	memcpy(tmpBuf,"RMB",3);
	sprintf(tmpBuf+3,"%015.0f",hv_fzf->o_tx_amt*100);
	lv_set_tag(&CMT302,"CND",tmpBuf);				/** 原货币符号、金额  **/
	lv_set_tag(&CMT302,"053",hv_fzf->res_content);/** 查复内容  **/

	lv_gen_pack(&CMT302);
	dm_show(&(CMT302.dm));

	/** 组notpay_in **/
	memcpy(notpay_in.cmtno,     "302",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->res_br_no,   	sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->pay_qs_no,	sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->req_br_no,    	sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	/* rem by LiuHuafeng 20060304
	memcpy(notpay_in.otxamt,	tmpBuf,					sizeof(tmpBuf)-1);
	**************************************/
	memcpy(notpay_in.otxamt,	tmpBuf,					sizeof(notpay_in.otxamt));
	/** opkgno **/
	memcpy(notpay_in.owtday,    lv_date(hv_fzf->req_date),8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);

	/** 插入发送记录 **/
	/***
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	vtcp_log("插入单包交易历史明细错误");
	lv_destroy(&CMT302);
	return(-1);
	}*******by liuyue 20060918 没有这个表了******/
	ret=lv_insSendList(&CMT302);
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		vtcp_log("插入小额报文发送接收历史登记簿错误");
		lv_destroy(&CMT302);
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;
	memcpy(hv_fzf->send_tel, lvca.tlrno, sizeof(hv_fzf->send_tel)-1);

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT302);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if (iRc=iLvSendPackage(&CMT302))
	{
		lv_destroy(&CMT302);
		return(-1);
	}
	lv_destroy(&CMT302);

	return(0);

}
/** 小额业务退回应答报文发送 **/
int lv_get_cmt320_tag(struct hv_fzf_c *hv_fzf)
{
	int     ret;
	int		iRc=0;
	char tmpBuf[1024];

	TLVPACKAGE CMT320;
	static  NOTPAY_IN_AREA  notpay_in;

	memset(tmpBuf,		'\0', sizeof(tmpBuf));
	memset(&notpay_in, 	'\0', sizeof(notpay_in));

	lv_load_tagifno();
	lv_create(&CMT320);
	/****NEWYX:增加来往标识****/
	if (lvca.rtcode = Hv_fzf_Dec_Upd(g_pub_tx.reply, "otype = '2' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '319' and lw_ind='2' ",  hv_fzf->or_br_no,  hv_fzf->wt_date, hv_fzf->orderno))
	{
		vtcp_log("定义游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Fet_Upd(hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----hv_fzf error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("未查到对应的记录!");
		lv_destroy(&CMT320);
		return(-1);
	}
	if ( hv_fzf->hv_fzf_sts[0] != STAT_LZRECV1  )
	{
		vtcp_log("[%s][%d]-----stat=[%c][%c]\n",__FL__,hv_fzf->hv_fzf_sts[0],STAT_WZLR);
		vtcp_log("记录状态错误!");
		lv_destroy(&CMT320);
		return(-1);
	}
	/** 开始拼报文 **/
	CMT320.opeType = '1';   /** 小额 **/
	CMT320.pkgType = 0;     /** 是cmt包 **/
	memcpy(CMT320.packDate, lv_date(hv_fzf->wt_date), 8);
	CMT320.packID = apatoi(hv_fzf->orderno,sizeof(hv_fzf->orderno)-1);
	memcpy(CMT320.CMTCode,"320",3);
	memcpy(CMT320.workDate,pcLvGetDay_work(),8);

	/** 开始组包 **/
	lv_set_tag(&CMT320,"30A",lv_date(hv_fzf->res_date));	/** 委托日期 **/
	lv_set_tag(&CMT320,"011",hv_fzf->cash_qs_no);	/** 退回应答清算行 **/
	lv_set_tag(&CMT320,"52A",hv_fzf->res_br_no);	/** 退回应答行 **/
	lv_set_tag(&CMT320,"0BD",hv_fzf->res_orderno);	/**退回应答序号 **/
	lv_set_tag(&CMT320,"CJB",lv_date(hv_fzf->req_date));	/** 原退回申请委托日期 **/
	lv_set_tag(&CMT320,"012",hv_fzf->pay_qs_no	);	/** 原退回申请清算行 **/
	lv_set_tag(&CMT320,"58A",hv_fzf->or_br_no);	/** 原退回申请行  **/
	lv_set_tag(&CMT320,"0BE",hv_fzf->orderno);	/** 原退回申请序号  **/
	vtcp_log("[%s][%d]hv_fzf.reqtype[0]===[%c]\n",__FILE__,__LINE__,hv_fzf->reqtype[0]);
	lv_set_tag(&CMT320,"BS8",hv_fzf->reqtype	);	/** 原退回类型 **/
	/**    lv_set_tag(&CMT320,"C15",hv_fzf->miya);	密押 **/
	lv_set_tag(&CMT320,"018",hv_fzf->res_sts);	/** 退回应答 **/
	lv_set_tag(&CMT320,"72A",hv_fzf->res_content);	/** 退回应答附言 **/

	lv_gen_pack(&CMT320);
	dm_show(&(CMT320.dm));

	/** 组notpay_in
	memcpy(notpay_in.cmtno,     "320",                  sizeof(hv_fzf->cmtno)-1);
	memcpy(notpay_in.orderno,   hv_fzf->res_orderno,    sizeof(hv_fzf->orderno)-1);
	memcpy(notpay_in.orbrno,    hv_fzf->res_br_no,   sizeof(hv_fzf->res_br_no)-1);
	memcpy(notpay_in.payqsactno,hv_fzf->cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);
	memcpy(notpay_in.acbrno,    hv_fzf->req_br_no,    sizeof(hv_fzf->req_br_no)-1);
	memcpy(notpay_in.cashqsactno,hv_fzf->pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);

	memcpy(notpay_in.opkgno,    hv_fzf->o_cmtno,     sizeof(hv_fzf->o_cmtno)-1);
	memcpy(notpay_in.owtday,   lv_date( hv_fzf->o_wt_date),8);
	memcpy(notpay_in.oorderno,  hv_fzf->o_orderno,   sizeof(hv_fzf->o_orderno)-1);
	memcpy(notpay_in.content,   hv_fzf->filler,     sizeof(hv_fzf->filler)-1);
	**/

	/** 插入发送记录 **/
	/*********************************
	ret=lv_Ins_Cmthtr(&notpay_in);
	if(ret)
	{
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FL__);
	vtcp_log("插入单包交易历史明细错误");
	return(-1);
	}
	***********************************/
	ret=lv_insSendList(&CMT320);
	if (ret)
	{
		vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FL__);
		vtcp_log("插入小额报文发送接收历史登记簿错误");
		return(-1);
	}
	hv_fzf->hv_fzf_sts[0] = STAT_WZSEND;

	if ((lvca.rtcode = Hv_fzf_Upd_Upd(*hv_fzf, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FL__,lvca.rtcode);
		vtcp_log("更新往包控制表错误");
		lv_destroy(&CMT320);
		return(-1);
	}
	if (lvca.rtcode = Hv_fzf_Clo_Upd())
	{
		vtcp_log("关闭游标出错 - %d", lvca.rtcode);
		return(-1);
	}
	vtcp_log("发包\n");
	if (iRc=iLvSendPackage(&CMT320))
	{
		lv_destroy(&CMT320);
		return(-1);
	}
	lv_destroy(&CMT320);
	return(0);

}
