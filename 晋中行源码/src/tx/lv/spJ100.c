/************************************************************
* 文 件 名:   spJ100.c
* 功能描述：  小额业务解付
*	            
* 作    者:   LiuHuafeng
* 完成日期：  2007-9-3 22:02:23
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
insert into tx_def values('5829','小额解付','10000000000000000000000000000000000000001000000000000000000000010000000000000000000000000000000000000000000000000000000000100000','1','3','2')
insert into tx_sub_def values('5829','小额解付','spJ100',0,0);
insert into tx_flow_def values('5829',0,'5829','#=0000@1@#$');
insert into tx_flow_def values('5829',1,'A016','#=0000@2@#$');
insert into tx_flow_def values('5829',1,'WNJZ','#=0000@2@#$');
insert into tx_flow_def values('5829',2,'M002','#$');

**************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
int spJ100()
{
	struct lv_pkgreg_c sLv_pkgreg;
	struct lv_pkgreg_c sLv_pkgreg_hz;
	char cTxtype[2];
	char cOrbrno[13];
	long lWtday;
	char cOrderno[9];
	char cLwind[2];
	memset(cLwind , 0 , sizeof(cLwind));
	memset(cOrderno , 0 , sizeof(cOrderno));
	memset(cOrbrno, 0 , sizeof(cOrbrno));
	memset(cTxtype , 0 , sizeof(cTxtype));
	memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));
	memset(&sLv_pkgreg_hz, 0 , sizeof(sLv_pkgreg_hz));
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	/****填充payin*********/
	get_zd_data("0720",cTxtype);
	vtcp_log("%s,%d txtype=%s,wtday=%ld,orbrno=%s,orderno=%s",__FILE__,__LINE__,cTxtype,lWtday,cOrbrno,cOrderno);
	get_zd_long("0440",&lWtday);
	vtcp_log("%s,%d txtype=%s,wtday=%ld,orbrno=%s,orderno=%s",__FILE__,__LINE__,cTxtype,lWtday,cOrbrno,cOrderno);
	get_zd_data("0520",cOrbrno);
vtcp_log("%s,%d txtype=%s,wtday=%ld,orbrno=%s,orderno=%s",__FILE__,__LINE__,cTxtype,lWtday,cOrbrno,cOrderno);
		sprintf(cOrbrno,"%012.0f",atof(cOrbrno));
	vtcp_log("%s,%d txtype=%s,wtday=%ld,orbrno=%s,orderno=%s",__FILE__,__LINE__,cTxtype,lWtday,cOrbrno,cOrderno);
	get_zd_data("0780",cOrderno);
	sprintf(cOrderno,"%0ld",atol(cOrderno));
	vtcp_log("%s,%d txtype=%s,wtday=%ld,orbrno=%s,orderno=%s",__FILE__,__LINE__,cTxtype,lWtday,cOrbrno,cOrderno);
	
	if(cTxtype[0]=='0')
	{
		/* 通兑 处理来帐回执的情况*/
		cLwind[0]='1';
	}
	else
	{
		/* 通存 处理拒绝的情况*/
		cLwind[0]='1';
	}
	g_reply_int=Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"lw_ind='%s' and wt_date=%ld and or_br_no='%s' and orderno=%s",cLwind,lWtday,cOrbrno,cOrderno);
	if(g_reply_int)
	{
		vtcp_log("%s,%d declare cursor error sqlcode=%d",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	g_reply_int=Lv_pkgreg_Fet_Upd(&sLv_pkgreg,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("%s,%d fetch cursor error sqlcode=%d",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	if(sLv_pkgreg.lv_sts[0]=='B'){
		strcpy( g_pub_tx.reply, "9102" );
		sprintf(acErrMsg,"已解付交易重新打印凭证,任意键继续...");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		set_zd_data("0640",sLv_pkgreg.resp_trace_no);
		vtcp_log("%s,%d,流水号是[%s]\n",__FILE__,__LINE__,sLv_pkgreg.resp_trace_no);
		goto ErrExit;
	}
	if(!memcmp(sLv_pkgreg.pkgno,PKGNO_SSDJ,sizeof(sLv_pkgreg.pkgno)-1))
	{
		if(memcmp(sLv_pkgreg.pay_ac_no,"10101",5))
		{
			vtcp_log("%s,%d pay_ac_no=%s",__FILE__,__LINE__,sLv_pkgreg.pay_ac_no);
			/*strcpy( g_pub_tx.reply, "HV01" );*/
			g_reply_int=Lv_pkgreg_Sel(g_pub_tx.reply,&sLv_pkgreg_hz,"o_orderno='%s' and o_or_br_no='%s' and o_wt_date=%d and lw_ind='2'",sLv_pkgreg.orderno,sLv_pkgreg.or_br_no,sLv_pkgreg.wt_date);
			strcpy( g_pub_tx.reply, "9101" );
			sprintf(acErrMsg,"转帐通存业务打印客户凭证,任意键继续...");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			set_zd_data("0640",sLv_pkgreg_hz.check_trace_no);
			vtcp_log("%s,%d,流水号是[%s]\n",__FILE__,__LINE__,sLv_pkgreg_hz.check_trace_no);
			goto ErrExit;
		}
		if(!memcmp(sLv_pkgreg.rcpstat,"00",sizeof(sLv_pkgreg.rcpstat)-1))
		{
			strcpy( g_pub_tx.reply, "9101" );
			sprintf(acErrMsg,"现金通存成功业务打印客户凭证,任意键继续....");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			set_zd_data("0640",sLv_pkgreg.resp_trace_no);
			vtcp_log("%s,%d,流水号是[%s]\n",__FILE__,__LINE__,sLv_pkgreg.resp_trace_no);
			goto ErrExit;
		}
		if(sLv_pkgreg.lv_sts[0]==STAT_WZREJECT || sLv_pkgreg.lv_sts[0]==STAT_WZCZ )
		{
			sLv_pkgreg.beg_sts[0]=sLv_pkgreg.lv_sts[0];
			sLv_pkgreg.lv_sts[0]=STAT_WZBACK;
		}
		else 
		{
			vtcp_log("%s,%d lv_sts=%s",__FILE__,__LINE__,sLv_pkgreg.lv_sts);
			strcpy( g_pub_tx.reply, "HV01" );
			sprintf(acErrMsg,"实时贷记往帐状态不是拒绝或冲正状态不可以做解付");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto ErrExit;
		}
	}
	if(!memcmp(sLv_pkgreg.pkgno,PKGNO_SSJJ,sizeof(sLv_pkgreg.pkgno)-1))
	{
		char cKinbr[8];
		char cGzactno[20];
		memset(cGzactno, 0 , sizeof(cGzactno));
		memset(cKinbr, 0 , sizeof(cKinbr));
		get_zd_data(DC_TX_BR_NO,cKinbr);
		pub_base_GetParm_Str(cKinbr,8,cGzactno);
		vtcp_log("%s,%d 挂帐帐号==%s",__FILE__,__LINE__,cGzactno);
		if(memcmp(sLv_pkgreg.rcpstat,"00",sizeof(sLv_pkgreg.rcpstat)-1))
		{
			vtcp_log("%s,%d rcpstat=%s",__FILE__,__LINE__,sLv_pkgreg.rcpstat);
			strcpy( g_pub_tx.reply, "9101" );
			sprintf(acErrMsg,"通兑失败业务打印客户凭证,任意键继续...");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			set_zd_data("0640",sLv_pkgreg.resp_trace_no);
			vtcp_log("%s,%d,流水号是[%s]\n",__FILE__,__LINE__,sLv_pkgreg.resp_trace_no);
			goto ErrExit;
		}
		if(memcmp(sLv_pkgreg.cash_ac_no,cGzactno,13))
		{
			vtcp_log("%s,%d cash_ac_no=%s",__FILE__,__LINE__,sLv_pkgreg.cash_ac_no);
			/*strcpy( g_pub_tx.reply, "HV01" );*/
			g_reply_int=Lv_pkgreg_Sel(g_pub_tx.reply,&sLv_pkgreg_hz,"o_orderno='%s' and o_or_br_no='%s' and o_wt_date=%d and lw_ind='2'",sLv_pkgreg.orderno,sLv_pkgreg.or_br_no,sLv_pkgreg.wt_date);
			strcpy( g_pub_tx.reply, "9101" );
			sprintf(acErrMsg,"转帐通兑业务打印客户凭证,任意键继续...");
			set_zd_data(DC_GET_MSG,acErrMsg);
			vtcp_log("%s,%d,流水号是[%s]\n",__FILE__,__LINE__,sLv_pkgreg_hz.input_trace_no);
			set_zd_data("0640",sLv_pkgreg_hz.input_trace_no);
			WRITEMSG
			goto ErrExit;
		}
		if(sLv_pkgreg.lv_sts[0]!=STAT_WZSUCCESS)
		{
			vtcp_log("%s,%d lv_sts=%s",__FILE__,__LINE__,sLv_pkgreg.lv_sts);
			strcpy( g_pub_tx.reply, "HV01" );
			sprintf(acErrMsg,"实时借记往帐状态不是扎差状态不可以做解付");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			goto ErrExit;
		}
		sLv_pkgreg.lv_sts[0]=STAT_WZBACK;
	}
	get_zd_data("0040",sLv_pkgreg.resp_trace_no);
	g_reply_int=Lv_pkgreg_Upd_Upd(sLv_pkgreg,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("%s,%d update lv_pkgreg error sqlcode=%d",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;
	}
	Lv_pkgreg_Clo_Upd( );
	set_zd_double("0400",sLv_pkgreg.tx_amt);
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

