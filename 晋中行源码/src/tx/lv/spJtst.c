/*************************************************
* 文 件 名:  spJ099.c
* 功能描述： 根据挂帐状态  设置走帐/挂账科目等8583走帐相关域
*			 
* 作    者:  ChenMing
* 完成日期： 2006年9月12日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
#include "hv_define.h"

int spJtst()
{ 
	char cLvStat[2];
	char cAc_no [20];
	char cAcno110[20];
	char cKinbr[BRNO_LEN+1];
	int iRet=0;
	char cFinishFlag[2];
	char cCmtno[4];
	char cRespCode[9];
	char cTemp[25];
	
	char cRcpstat[4];
	memset(cRcpstat,0,sizeof(cRcpstat));
	memset(cRespCode,0,sizeof(cRespCode));
	memset(cCmtno, 0 , sizeof(cCmtno));
	memset(cFinishFlag, 0 , sizeof(cFinishFlag));
	memset(cKinbr, 0 , sizeof(cKinbr));
	memset(cAcno110, 0 , sizeof(cAcno110));
	memset(cLvStat, 0 , sizeof(cLvStat));
	memset(cAc_no , 0 , sizeof(cAc_no ));
	memset(cTemp , 0 , sizeof(cTemp ));
		
	get_zd_data("0980",cLvStat);
	get_zd_data("0300",cAc_no );
	get_zd_data("0500",cFinishFlag);
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data("0510",cCmtno);
	get_zd_data("0290",cRespCode); /* 为什么29域传不过来 ？*/
	get_zd_data("0270",cTemp);	
	get_zd_data("0890",cRcpstat);

	vtcp_log("[%s][%d] 1111 cRespCode=[%s]\n",__FILE__,__LINE__,cRespCode);		
	vtcp_log("[%s][%d] 1111 cTemp=[%s]\n",__FILE__,__LINE__,cTemp);				
	vtcp_log("[%s][%d] cmtno=[%s]\n",__FILE__,__LINE__,cCmtno);	
	vtcp_log("[%s][%d] cAc_no=[%s]\n",__FILE__,__LINE__,cAc_no);	
	
	vtcp_log(" %s,%d--- 小额来账 配置帐务子交易 --- ",__FL__);
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-9-2 */
	sprintf(cCmtno,"%03d",atoi(cCmtno));
	vtcp_log("[%s][%d] cmtno=[%s]\n",__FILE__,__LINE__,cCmtno);	
	/* end add by LiuHuafeng 2007-9-2 */
	/* end NEWTCTD1 */
	
	if(!memcmp(cCmtno,"254",3))
	{
		/* 借贷方向在254报文接收中处理 */
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
		if(iRet)
		{
			vtcp_log("%s,%d 没有配置机构%s的110帐号",__FL__,cKinbr);
			strncpy(g_pub_tx.reply,"P076",4);
			goto ErrExit;
		}
		set_zd_data("0300",cAcno110);  /* 110  帐号 */
		set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
	  
		goto OkExit;
	}
	else if(!memcmp(cCmtno,"920",3))
	{
		memcpy(cRespCode,cTemp+16,8);
		vtcp_log("[%s][%d] 2222 cRespCode=[%s]\n",__FILE__,__LINE__,cRespCode);		
		pub_base_strpack(cRespCode);
		vtcp_log("%s,%d 受到920,应答码是[%s] ",__FL__,cRespCode);

		if(cRespCode[3]!='I' && strlen(cRespCode)!=0)
		{
			/* 应答码是拒绝  表明920可能要处理帐务了 */
			strncpy(g_pub_tx.reply,"0002",4);  /* 跳到 JJJ1 */
			vtcp_log("%s,%d 受到920,应答码是[%s],是拒绝类型的报文",__FL__,cRespCode);
			/****NEWYX2 BEGIN:并非所有交易被拒绝都要走帐的****/
			if(memcmp(cTemp+24,PKGNO_TYXX,3)==0||
				memcmp(cTemp+24,PKGNO_SSXX,3)==0){
				vtcp_log("%s,%d,收到拒绝920但原包[%.3s]不需要冲帐",__FILE__,__LINE__,cTemp+24);
		    	strncpy(g_pub_tx.reply,"0001",4);  /* 不是拒绝不走帐，直接奔J099登记 */
		    	set_zd_long("0530",-1);      /* iStep== -1  表示记帐处理结束 */			
			}else{
			}
			/****NEWYX2 BEGIN:并非所有交易被拒绝都要走帐的****/

			goto ErrExit;
		}
		/* begin NEWTCTD1 */
		/* add by LiuHuafeng 20070822 */
		else if(!memcmp(cCmtno,"003",3)
		&& (cLvStat[0]==STAT_LZNMAC_ERR
			||cLvStat[0]==STAT_LZLMAC_ERR
			||cLvStat[0]==STAT_LZAMAC_ERR
			||cLvStat[0]==STAT_LZGZ_QS
			||cLvStat[0]==STAT_LZGZ_BR)
		)
		{
			/* 表明是往帐实时贷记拒绝  不需要帐务处理 */
			set_zd_long("0530",-1);            /*iStep== -1  表示记帐处理结束*/
			strncpy(g_pub_tx.reply,"0001",4);  /*不是拒绝不走帐直接奔J099登记 */
			vtcp_log("%s,%d 实时贷记拒绝不需要帐务处理",__FILE__,__LINE__);
			goto ErrExit;
		}
		/* end by LiuHuafeng 20070822 */
		/* end NEWTCTD1 */
		else
		{
		    set_zd_long("0530",-1);            /* iStep== -1  表示记帐处理结束 */			
		    strncpy(g_pub_tx.reply,"0001",4);  /* 不是拒绝不走帐，直接奔J099登记 */
  	    goto ErrExit;	
  	}
	}
	else if(cFinishFlag[0]=='1')
	{
		strncpy(g_pub_tx.reply,"0001",4);
		vtcp_log("%s,%d 报文识别结束",__FL__);
		goto ErrExit;
	}
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-9-2 */	
	if(!memcmp(cCmtno,"004",3))
	{
		/* 实时借记业务如果应答成功走本分支，这样在这里直接记回执帐 */
		set_zd_data("0660","1");/* 客户来帐方向 */
		set_zd_data("0720","2");/* 110,414 来帐方向 */
		set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
		set_zd_data("0320",LV_SUB02_SEND);/* 设置过渡科目 */
	}
	/* begin add by LiuHuafeng 2007-11-6 21:48:40 NEWTCTD5 */
	else if(!memcmp(cCmtno,PKGNO_SSDJHZ,3))
	{
		double dTxamt009=0.0;
		/* 如果是009，被拒绝，那么才到这里走帐，走红字 */
		set_zd_data("0660","1");/* 客户来帐方向 */
		set_zd_data("0720","2");/* 110,414 来帐方向 */
		set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
		set_zd_data("0320",LV_SUB02_SEND);/* 设置过渡科目 */
		get_zd_double("0400",&dTxamt009);
		vtcp_log("%s,%d 交易金额是%f",__FILE__,__LINE__,dTxamt009);
		set_zd_double("0400",dTxamt009 * -1);
		set_zd_double("0390",dTxamt009 * -1);
	}
	/* end by LiuHaufeng 2007-11-6 21:48:51 NEWTCTD5*/
	else
	{
	/* end add by LiuHuafeng 2007-9-2 */	
	/* end NEWTCTD1 */
		set_zd_data("0660","2");/* 客户来帐方向 */
		set_zd_data("0720","1");/* 110,414 来帐方向 */
		set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
		set_zd_data("0320",LVLZGD);/* 设置过渡科目 */
		vtcp_log("%s,%d,开始设置科目是否拒绝的009也走这里",__FILE__,__LINE__);
	}
	if(cLvStat[0]==STAT_LZRECV2)
	{
		vtcp_log("%s,%d 接收状态为正常接收,帐号=[%s]",__FL__,cAc_no);
	/**add by wangyongwei 20070913 长治需求 现金收款的挂账科目*/
	#ifdef CZ_XQ
		if(memcmp(cCmtno,PKGNO_SSJJHZ,3)==0 && apatoi(cRcpstat,3)==0){
			char cCashacno[20];
			memset(cCashacno,0,sizeof(cCashacno));
			iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cCashacno);
			if(iRet)
			{
				vtcp_log("%s,%d 没有配置机构%s的小额挂账帐号",__FL__,cKinbr);
				strncpy(g_pub_tx.reply,"P076",4);
				goto ErrExit;
			}
			if(memcmp(cCashacno,cAc_no,sizeof(cCashacno)-1)==0){
				iRet=pub_base_GetParm_Str(cKinbr,PARM_CZ_LVLZGZ,cAc_no);
				if(iRet)
				{
					vtcp_log("%s,%d 没有配置机构%s的小额挂账帐号",__FL__,cKinbr);
					strncpy(g_pub_tx.reply,"P076",4);
					goto ErrExit;
				}
				set_zd_data("0300",cAc_no);
				set_zd_data("0680","1");
			}
		}
	#endif	
	/**add by wangyongwei 20070913 长治需求 现金收款的挂账科目*/
	}
	else if(cLvStat[0]==STAT_LZNMAC_ERR ||cLvStat[0]==STAT_LZLMAC_ERR
	 ||cLvStat[0]==STAT_LZAMAC_ERR ||cLvStat[0]==STAT_LZGZ_QS 
	 ||cLvStat[0]==STAT_LZGZ_BR
	)
	{
	/**add by wangyongwei NEWTCTD 20070913*根据长治需求现金解付的挂账科目放到应解汇款*/
	#ifdef CZ_XQ
		if((memcmp(cCmtno,PKGNO_SSDJHZ,3)==0 || memcmp(cCmtno,PKGNO_SSJJHZ,3)==0)&& apatoi(cRcpstat,3)!=0){
			vtcp_log("%s,%d 是否走到这里********接收状态为正常接收,帐号=[%s]",__FL__,cAc_no);
			iRet=pub_base_GetParm_Str(cKinbr,PARM_CZ_LVLZGZ,cAc_no);
			if(iRet)
			{
				vtcp_log("%s,%d 没有配置机构%s的小额挂账帐号",__FL__,cKinbr);
				strncpy(g_pub_tx.reply,"P076",4);
				goto ErrExit;
			}
			set_zd_data("0300",cAc_no);
			set_zd_data("0680","1");
		}else{
			/* 需要重新设置挂账科目 */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAc_no);
			if(iRet)
			{
				vtcp_log("%s,%d 没有配置机构%s的小额挂账帐号",__FL__,cKinbr);
				strncpy(g_pub_tx.reply,"P076",4);
				goto ErrExit;
			}
			set_zd_data("0300",cAc_no);
			set_zd_data("0680","6");
		}
	#else
	/**add by wangyongwei NEWTCTD 20070913*根据长治需求现金解付的挂账科目放到应解汇款*/
		/* 需要重新设置挂账科目 */
		iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAc_no);
		if(iRet)
		{
			vtcp_log("%s,%d 没有配置机构%s的小额挂账帐号",__FL__,cKinbr);
			strncpy(g_pub_tx.reply,"P076",4);
			goto ErrExit;
		}
		set_zd_data("0300",cAc_no);
		set_zd_data("0680","6");
	#endif
	}
	
OkExit:
	memcpy(g_pub_tx.reply,"0000",4);
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

