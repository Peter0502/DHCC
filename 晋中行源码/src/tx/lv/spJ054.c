/*****************************************************
* 文 件 名:  spJ054.c
* 功能描述： 定期借记回执业务录入
* 作    者:  xyy
* 完成日期： 2006-9-7 16:03
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
static char cFlag[2];
static PAY_IN_AREA payin;
static PAY_IN_AREA payin_hz;
int lv23_Querycurr(PAY_IN_AREA *payin_hz,PAY_IN_AREA *payin,char flag);
int spJ054()
{
	int iRc = 0;
	int ret = 0;
	char cRcpstat[3];
	struct lv_pkgreg_c lv_pkgreg;

  memset(cRcpstat,0,sizeof(cRcpstat));
	memset(&payin,0,sizeof(payin));
	memset(&payin_hz,0,sizeof(payin_hz));
	memset(cFlag,0,sizeof(cFlag));
	memset(&lv_pkgreg,0,sizeof(lv_pkgreg));


	vtcp_log("[%s][%d] 开始定期借记回执录入!\n",__FILE__,__LINE__);

	/*********************
	 * 1. 初始化全局变量 *
	 *********************/

	pub_base_sysinit();	
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

	/*********************
	 * 2. 提取8583域内容 *
	 *********************/

	get_fd_data("0980",cFlag);
	ret = iLvTisToPayin(&payin);
	if(ret)
	{
		vtcp_log("[%s][%d]调用TisTopayin出错 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	memcpy(cRcpstat,payin.F_rcpstat,sizeof(cRcpstat)-1);
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	ret = iLvPayinAutoInfo(&payin);
	if(ret)
	{
		vtcp_log("[%s][%d]iLvPayinAutoInfo出错 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/********************
	 * 3. 进行交易检查  *
	 ********************/
	vtcp_log("[%s][%d]cFlag===[%c]\n",__FILE__,__LINE__,cFlag[0]);
	vtcp_log("[%s][%d]payin.pkgno===[%s]\n",__FILE__,__LINE__,payin.F_pkgno);
	ret = lv23_Querycurr(&payin_hz,&payin,cFlag[0]);
	if(ret)
	{
		vtcp_log("[%s][%d]调用lv23_Querycurr出错 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****基本检查****/
	ret = iLvPayinCheck0(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]基本检查出错 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****要素检查****/
	ret = iLvPayinCheck4(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]要素检查出错 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/****行号检查****/
	ret = iLvPayinCheck1(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]行号检查出错 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/*end by LiuHuafeng 2007-1-16 14:48 */
	/******************
	 * 4. 数据库操作  *
	 ******************/	
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("xier[%s][%d]payin.F_rcpstat==[%s]\n",__FILE__,__LINE__,cRcpstat);
	vtcp_log("[%s][%d]F_orderno =[%s]",__FL__,payin_hz.F_orderno);
	payin_hz.F_lw_ind[0]='1';
	memcpy(payin_hz.F_rcpstat,cRcpstat,sizeof(cRcpstat)-1);
	ret = iLvNewRecFromPayin(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]调用iLvNewRecFromPayin错误 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_o_packid==[%s]\n",__FILE__,__LINE__,payin_hz.T_o_packid);
	vtcp_log("[%s][%d]payin_hz.T_o_pack_date==[%s]\n",__FILE__,__LINE__,payin_hz.T_o_pack_date);
		
	vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.F_rcpstat);
	vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.F_rcpstat);

	/***插入新记录****/
	if(cFlag[0]=='0')
	{
		iLvResetPayin(&payin_hz);	
		vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.T_rcpstat);
		ret=iLvUpdRecFromPayin(&payin_hz,1);
	}
	else
	{
		iLvResetPayin(&payin_hz);
		vtcp_log("[%s][%d]payin_hz.rcpstat==[%s]\n",__FILE__,__LINE__,payin_hz.T_rcpstat);
		ret=iLvUpdRecFromPayin(&payin_hz,0);
	}
	if(ret)
	{
		vtcp_log("[%s][%d]调用iLvUpdRecFromPayin错误 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_rcpstat==[%s]",__FILE__,__LINE__,payin_hz.T_rcpstat);
	/****Modified by SSH,2005.12.26,应根据回执状态更新原来帐****/


	vtcp_log("[%s][%d]!!!!!!!!!!!!!!!!!!!!!\n",__FILE__,__LINE__);
	print_payin_ly(&payin_hz);
	vtcp_log("[%s][%d]!!!!!!!!!!!!!!!!!!!!!\n",__FILE__,__LINE__);
	ret=iUpdRecvTableStat(&payin_hz);
	if(ret)
	{
		vtcp_log("[%s][%d]调用iUpdRecvTableStat错误 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		goto ErrExit;
	}
	/* 检查多方协议号 begin add by LiuHuafeng 2007-1-16 14:48*/
	ret = iLvPayinCheck9(&payin_hz);
	if(ret)
	{
		if(memcmp(cRcpstat,"00",2)==0)
		{
			vtcp_log("[%s][%d]应答为成功但多方协议号检查错误 \n",__FILE__,__LINE__);
			set_zd_data("0130",acErrMsg);
			strcpy( g_pub_tx.reply, "HV01" );
			goto ErrExit;
		}
	}


	/****************
	 * 5. 写流水表  *
	 ****************/	
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		strncpy(g_pub_tx.reply,"S023",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_rcpstat==[%s]",__FILE__,__LINE__,payin_hz.T_rcpstat);
	/************************
	 * 6. 准备前台输出数据  *
	 ************************/
	iRc=ilv10_PreTos();
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d set fd123 error!",__FL__);
		strncpy(g_pub_tx.reply,"P079",4);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]payin_hz.T_rcpstat==[%s]",__FILE__,__LINE__,payin_hz.T_rcpstat);
	if(memcmp(payin_hz.T_rcpstat,"00",2))
		goto OkExit1;
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	sprintf(acErrMsg,"回执为同意[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

OkExit1:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"回执为不同意[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
                memcpy(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


int lv23_Querycurr(PAY_IN_AREA *payin_hz,PAY_IN_AREA *payin,char flag)
{
	int iRc=0;
  char cPkgno1[4];
  char cPkgno2[4];
  char cOpcd  [3];
  
  memset(cOpcd  ,0,sizeof(cOpcd));
  memset(cPkgno1,0,sizeof(cPkgno1));
  memset(cPkgno2,0,sizeof(cPkgno2));
	if(flag=='1')/****修改***/
	{
		vtcp_log("[%s][%d] crdb===[%c] \n",__FILE__,__LINE__,payin->F_crdb[0]);
		
		memcpy(cOpcd  ,payin->F_opcd  ,sizeof(cOpcd  )-1);
		memcpy(cPkgno1,payin->F_pkgno,sizeof(cPkgno1)-1);
		memcpy(cPkgno2,payin->T_pkgno,sizeof(cPkgno2)-1);
		memcpy(payin->F_pkgno,"006",3);
		memcpy(payin->T_pkgno,"006",3);
		vtcp_log("[%s][%d] F_pkgno===[%.3s] \n",__FILE__,__LINE__,payin->F_pkgno);
		if(iLvFetchRecToPayin(payin))
		{
			vtcp_log("[%s][%d]调用iLvFetchRecToPayin错误 \n",__FILE__,__LINE__);
			return 1;
		}

		iRc=iLvResetPayin(payin);
		if(payin->A_stat[0]!=STAT_YES_NOTSEND&&payin->A_stat[0]!=STAT_NO_NOTSEND )
		{
			vtcp_log("[%s][%d]不是录入状态不可以修改 \n",__FILE__,__LINE__);	
			strcpy( g_pub_tx.reply, "HV01" );
			return 1;
		}
    vtcp_log("[%s][%d] T_rcpstat===[%.2s] \n",__FILE__,__LINE__,payin->T_rcpstat);
		if(!memcmp(payin->T_rcpstat,"20",2 ))
		{
			vtcp_log("[%s][%d]当前数据已经撤销不可以修改 \n",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			return 1;
		}
		memcpy(payin->F_pkgno,cPkgno1,sizeof(cPkgno1)-1);
		memcpy(payin->T_pkgno,cPkgno2,sizeof(cPkgno2)-1);
		memcpy(payin->F_opcd ,cOpcd  ,sizeof(cOpcd  )-1);
	}
	if(iRc!=0)
	{
		vtcp_log("[%s][%d]没有未审核的数据 \n",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		return 1;
	}
	else
	{
		memcpy(payin->F_pkgno,PKGNO_DQJJ,sizeof(payin->F_pkgno));
		vtcp_log("[%s][%d] F_pkgno===[%.3s] \n",__FILE__,__LINE__,payin->F_pkgno);
		if(payin->T_stat[0]=='V' && memcmp(payin->F_rcpstat,"20",2)!=0 )
		{
			vtcp_log("[%s][%d]已经终止付业务只能选择已撤销 \n",__FILE__,__LINE__);
			strcpy( g_pub_tx.reply, "HV01" );
			return 1;
		}
		if(flag=='0')
		{
			iRc=iLvCreateHz(payin,payin_hz);
			if(iRc!=0)
			{
				vtcp_log("[%s][%d]没有未审核的数据 \n",__FILE__,__LINE__);
				sprintf( acErrMsg,"没有未审核的数据,[%s][%d]", __FILE__,__LINE__);
				strcpy( g_pub_tx.reply, "HV01" );
				WRITEMSG
				return 1;
			}
		}
		else
		{
			memcpy(payin_hz->F_orderno ,payin->F_oorderno  ,sizeof(payin->F_oorderno));
			memcpy(payin_hz->F_orbrno  ,payin->F_o_or_br_no,sizeof(payin->F_o_or_br_no));
			memcpy(payin_hz->F_wtday   ,payin->F_owtday    ,sizeof(payin->F_owtday));
			payin_hz->F_lw_ind[0]='1';
			payin_hz->T_lw_ind[0]='1';
			memcpy(payin_hz->F_opcd,OPCD_LR,2);
			g_reply_int = iLvFetchRecToPayin(payin_hz);
			if(g_reply_int)
			{
				sprintf( acErrMsg,"调用iLvFetchRecToPayin出错,[%s][%d]", __FILE__,__LINE__);
				WRITEMSG
				return 1;
			}
			iRc=iLvResetPayin(payin_hz);
		}
		memcpy(payin_hz->F_rcpstat,payin->F_rcpstat,sizeof(payin_hz->F_rcpstat));
	}
	return 0;
}


/****************
 * 构造输出内容 *
 ****************/
int ilv10_PreTos()
{
	int iRc;
	/****构造输出内容payout2****/
	iLvSetPayout1(&payin_hz);
	return(0);
}
