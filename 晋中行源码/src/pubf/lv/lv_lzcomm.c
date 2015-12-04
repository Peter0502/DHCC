/******************************************************************
模块名称: 来账交易处理的公共函数
函数功能: 来账交易处理的公共函数
输入参数:
输出参数:
使用说明:
编 写 者: SSH 2005.8.20
*******************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lvpack.h"
#include "exitlvpack.h"
#include "lv_pub.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "prdt_ac_id_c.h"
#include "com_item_c.h"
#include "cashier_c.h"/*YHBP*/

void vLvBrnoDebug(char * , int);

/****Add by SSH,2005.12.2,根据核押状态,调整交易状态****/
static char* GETTAG(TLVPACKAGE *pack,char *name,char *value)
{
	lv_get_tag(pack,name,value);
	return(value);
}
char * GETTAG_SIZE(TLVPACKAGE *pack,char *name,char *value,int size)
{
	char cValue[4096];	
	memset(cValue, 0 , sizeof(cValue));
	lv_get_tag(pack,name,cValue);
	memcpy(value,cValue,size);
	return (value);
}
int lv_adjust_hystat(PAY_IN_AREA *wp_payin,char *hystat)
{
	/** 判断状态 **/
	if (memcmp(hystat,"001",3) == 0)	/** 表示来账核全国押错 **/
	{
		wp_payin->A_stat[0]=STAT_LZNMAC_ERR;
	}
	else if (memcmp(hystat,"002",3) == 0)	/** 表示来账核地方押错 **/
	{
		wp_payin->A_stat[0]=STAT_LZLMAC_ERR;
	}
	if ((wp_payin->A_stat[0]==STAT_LZNMAC_ERR) || (wp_payin->A_stat[0]==STAT_LZLMAC_ERR))
	{
		/****核押错,记在清算中心****/
		char qsbrno[8];
		memset(qsbrno,'\0',sizeof(qsbrno));
		/*find_qsbrno(qsbrno);*/
		memcpy(lvca.brno,"XXXX",sizeof(lvca.brno));
		set_zd_data("0030", lvca.brno);
		pcLvGetRecvno();
		memcpy(wp_payin->A_wssrno, lvca.wssrno, 6);
		vtcp_log("遇到自己处理吧！, 机构流水号  lvca.wssrno=[%s]",lvca.wssrno);
	}
	return(0);
}

/****普通贷记报文来账处理 ****/
int lv_deal_ptdjlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	/* chkflg=1 挂帐检查   chkflg=0 登记  */
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	char cWssrno[7];
	struct lv_lbctl_c wd_lv_lbctl;
	int  iTmpIndex = 0;  	   /* 用于物质循环查找时 包中子报文序号*/
	long  iSubpackIndex = 0;  /* 要处理的子报文序号 当iTmpIndex=iSubpackIndex时进行相应处理 */

	/*********数据初始化******************/	

	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(cWssrno , 0 , sizeof(cWssrno));    
	get_zd_data("0040",cWssrno );   /**流水号**/    
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);
	vLvBrnoDebug(__FILE__,__LINE__);
	/****检查是否普通贷记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_PTDJ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是普通贷记来账\n",__FILE__,__LINE__,wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是普通贷记报文");
		WRITEMSG
			return(-1);
	}
	vLvBrnoDebug(__FILE__,__LINE__);
	while (1) /****循环处理子报文****/
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy*
		***************************************************************/
		vLvBrnoDebug(__FILE__,__LINE__);
		memset(&subpack,0,sizeof(TLVPACKAGE));
		lv_create(&subpack);
		vLvBrnoDebug(__FILE__,__LINE__);
		iRc = lv_fetchpkg(wp_lvpack,&subpack);
		vLvBrnoDebug(__FILE__,__LINE__);
		if (iRc<0)      /*提取报文时出错*/
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");
			WRITEMSG
				lv_destroy(&subpack);
			vLvBrnoDebug(__FILE__,__LINE__);
			return(-1);
		}
		else if (iRc>0)  /*提取报文结束*/
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			vLvBrnoDebug(__FILE__,__LINE__);
			return 0;
		}
		/* add by LiuHuafeng 2006-10-6 14:09  */
		vtcp_log("%s,%d 注意了 iSubpackIndex[%ld]   iTmpIndex[%d]...",__FILE__,__LINE__,iSubpackIndex,iTmpIndex);
		if (iSubpackIndex < iTmpIndex)
		{
			vLvBrnoDebug(__FILE__,__LINE__);
			lv_destroy(&subpack);			  
			return -1;
		}  
		recnt = lv_count_recnt(&subpack);   	/****得到明细业务的笔数****/
		vLvBrnoDebug(__FILE__,__LINE__);
		vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
	#if 0
		{
			if ( iSubpackIndex >= iTmpIndex+recnt) /* 说明iSubpackIndex不在这笔子报文中 */
			{
				iTmpIndex = iTmpIndex + recnt;
				vtcp_log("%s,%d ..... so continue ......",__FL__);
				vtcp_log("%s,%d ..... so continue ......",__FL__);
				vtcp_log("%s,%d iSubpackIndex=[%ld],iTmpIndex=[%d],so continue",__FL__,iSubpackIndex,iTmpIndex);
				vtcp_log("%s,%d ..... so continue ......",__FL__);
				vtcp_log("%s,%d ..... so continue ......",__FL__);
				if ( iTmpIndex > iSubpackIndex )
				{
					vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
					sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
					WRITEMSG
						lv_destroy(&subpack);
					return -1;
				}
				continue;    			
			}
		}
	#endif

		for (index=0; index<recnt; index++)
		{
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{
				char cPayqsno[13];
				char cCashqsno[13];	
				char cSendco[5];	
				char cRececo[5];	
				memset(cPayqsno, 0, sizeof(cPayqsno));
				memset(cCashqsno, 0, sizeof(cCashqsno));
				memset(cSendco, 0, sizeof(cSendco));
				memset(cRececo, 0, sizeof(cRececo));                              	

				vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__);
				memset(&payin,'\0',sizeof(payin));
				memcpy(payin.A_wssrno, cWssrno, sizeof(payin.A_wssrno));
				memcpy(payin.T_recvwssrno, payin.A_wssrno, sizeof(payin.T_recvwssrno));
				vLvBrnoDebug(__FILE__,__LINE__);
				iRc = lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				vtcp_log("%s,%d,payname=[%.60s]",__FILE__,__LINE__,payin.F_payname);
				vtcp_log("%s,%d,cashname=[%.60s]",__FILE__,__LINE__,payin.F_cashname);
				vtcp_log("%s,%d,payname=[%.60s]",__FILE__,__LINE__,payin.T_payname);
				vtcp_log("%s,%d,cashname=[%.60s]",__FILE__,__LINE__,payin.T_cashname);

				vLvBrnoDebug(__FILE__,__LINE__);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d 机构号码是 [%.5s] ",__FL__,payin.T_brno);
				/*          检查放在 J098 pubf_lv_pkg_hang_chk() 中
				iRc = iLvPayinCheck3(&payin);
				lv_adjust_hystat(&payin,subpack.opeStateCode);
				if(iRc<0)
				{
				vtcp_log("%s,%d,收款帐号检查失败",__FILE__,__LINE__);
				sprintf( acErrMsg,"收款帐号检查失败");
				WRITEMSG
				lv_destroy(&subpack);
				return(-1);
				}
				*/
				payin.F_lw_ind[0]= '2';

				memcpy( cPayqsno,payin.T_payqsactno, sizeof(cPayqsno)-1);
				memcpy( cCashqsno,payin.T_cashqsactno, sizeof(cPayqsno)-1);

				vtcp_log("%s,%d,cCashqsno=[%.12s]\n",__FILE__,__LINE__,cCashqsno);     
				vtcp_log("%s,%d,cPayqsno=[%.12s]\n",__FILE__,__LINE__,cPayqsno);	

				/**Add by SSH,20131025,如果cCashqsno为空就不必再查了，节省时间**/
				if(cCashqsno[0]!='\0'){
					if ( iGetSendco(cSendco, cCashqsno) )
					{
						vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
						/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
						WRITEMSG
						return(-1);*/
					}
				}
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d,cSendco=[%s]\n",__FILE__,__LINE__,cSendco);              	
				/**Add by SSH,20131025,如果cPayqsno为空就不必再查了**/
				if(cPayqsno[0]!='\0'){
					if (iGetSendco(cRececo, cPayqsno))
					{
						vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
						/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
						WRITEMSG
						return(-1);*/
					}
				}
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d,cRececo=[%.4s]\n",__FILE__,__LINE__,cRececo);                   

				memcpy( payin.T_sendco, cSendco, sizeof(payin.T_sendco));
				memcpy( payin.T_receco, cRececo, sizeof(payin.T_receco));	

				iLvNewRecFromPayin(&payin);
				vLvBrnoDebug(__FILE__,__LINE__);
				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐写入登记簿开始###############",__FL__);
					vtcp_log("%s,%d ###得到的机构号=%.5s,状态=[%s]###############",__FL__,payin.T_brno,cLvStat);
					vtcp_log("%s,%d ################来帐写入登记簿开始###############",__FL__);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);

					iRc = iLvUpdRecFromPayin(&payin,1);
					vLvBrnoDebug(__FILE__,__LINE__);
					if (iRc)
					{
						sprintf( acErrMsg,"插入普通贷记来账登记簿错误");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}               	
					lv_destroy(&subpack); 
					vLvBrnoDebug(__FILE__,__LINE__);
					/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						vLvBrnoDebug(__FILE__,__LINE__);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								lv_destroy(&subpack);							
							return -1;
						}
						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						vLvBrnoDebug(__FILE__,__LINE__);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
								lv_destroy(&subpack);							
							return -1;
						}    		        	
					}                 		
					set_zd_long("0520", iSubpackIndex+1);	 /* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					/*  已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					lv_destroy(&subpack);/* add by LiuHuafeng 2006-10-6 14:12 */
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);
					/* rem by LiuHuafeng 2006-10-6 16:26
					return 0;
					*********************/
				}
				else	 /* chkflg=1 挂帐检查   */
				{
					char cLvStat[1];
					memset(cLvStat , 0 , sizeof(cLvStat));
					vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);
					vLvBrnoDebug(__FILE__,__LINE__);
					set_zd_data("0500","0"); 
					iRc = pubf_lv_pkg_hang_chk(&payin,"001");
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
					cLvStat[0]=payin.T_stat[0];
					vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);
					vLvBrnoDebug(__FILE__,__LINE__);

					if (iRc == 0)        /* 检查正确 走帐*/
					{
						strcpy(g_pub_tx.reply, "0000" );
					}
					else if (iRc == 100)  /* 挂帐 */
					{
						strcpy(g_pub_tx.reply, "0000"  ); 
					}
					/* begin add by LiuHuafeng 2009-5-13 17:17:26 for 村镇银行 */
					else if( iRc == 999 )
					{
						strcpy(g_pub_tx.reply, "0000"  ); 
					}
					/* end by LiuHuafeng 2009-5-13 17:17:47 */
					else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}
					vLvBrnoDebug(__FILE__,__LINE__);
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);  /*设置挂帐状态*/
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐挂帐检查结束###############",__FL__);
					vtcp_log("%s,%d,找到要处理的iSubpackIndex[%ld]子报文，已处理完毕!! \n",__FILE__,__LINE__,iSubpackIndex);
				}
				/*  已处理完毕 */
				lv_destroy(&subpack);
				return iRc;
			}
			iTmpIndex ++;    			
		}/*end of for */
		if ( iTmpIndex > iSubpackIndex )
		{
			vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
			sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
			WRITEMSG
				lv_destroy(&subpack);
			return -1;
		}
	}	/* end of while */
	lv_destroy(&subpack);	
	return 0;
}

/****实时贷记报文来账处理****/
int lv_deal_ssdjlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	char cPackday[9];
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	PAY_IN_AREA payin_hz;
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;
	int iCheck3Flag=-1;
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   	
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-8-23 18:17:24 */
	char cRcpstat[4];
	memset(cRcpstat , 0 , sizeof(cRcpstat));
	get_zd_data("0890",cRcpstat);
	vtcp_log("%s,%d, rcpstat=[%s]!! \n",__FILE__,__LINE__,cRcpstat);
	/* end by LiuHuafeng 2007-8-23 18:17:45 */
	/* end NEWTCTD1 */
	memset(cPackday, 0 , sizeof(cPackday));

	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
	/****检查是否实时贷记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_SSDJ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是实时贷记来账\n",__FILE__,__LINE__,
			wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是实时贷记报文");WRITEMSG
		return(-1);
	}
	/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
	*   lv_destroy
	***************************************************************/
	memset(&subpack,0,sizeof(TLVPACKAGE));
	lv_create(&subpack);
	iRc=lv_fetchpkg(wp_lvpack,&subpack);
	if (iRc<0)
	{
		vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"提取报文时出错");WRITEMSG
			lv_destroy(&subpack);
		return(-1);
	}
	else if(iRc>0)
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
		lv_destroy(&subpack);
		return 0;
	}
	if (iSubpackIndex < iTmpIndex)
	{
		vLvBrnoDebug(__FILE__,__LINE__);
		lv_destroy(&subpack);
		return -1;
	}  
	/****得到明细业务的笔数****/
	recnt=lv_count_recnt(&subpack);
	vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);

	/* PKG包已处理完毕*/
	if ( iSubpackIndex >= iTmpIndex+recnt)  
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");    
		lv_destroy(&subpack);
		return 0;
	}	
	for (index=0; index<recnt; index++)
	{
		if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
		{
			vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 

			memset(&payin,'\0',sizeof(payin));
			iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
			if (iRc<0)
			{
				vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
				sprintf( acErrMsg,"根据子报文填写payin失败");
				WRITEMSG
				lv_destroy(&subpack);
				return(-1);
			}
			iRc=iLvPayinCheck3(&payin);
			if(payin.A_stat[0]!=STAT_LZRECV2)
			{
			    vtcp_log("%s,%d,收款帐号检查失败",__FILE__,__LINE__);
			    sprintf( acErrMsg,"付款帐号检查失败");WRITEMSG
			    iCheck3Flag=1;
			}else
			{
			    vtcp_log("%s,%d,收款帐号检查成功",__FILE__,__LINE__);
			    iCheck3Flag=0;
			}

			/**NOTE:这里由于没有正式上线该业务所以直接设置为拒绝****/
			/* begin NEWTCTD1 */
			/* rem by LiuHuafeng 2007-9-1 23:53:36
			iCheck3Flag=1;
			*******************************/
			/* end NEWTCTD1 */
			lv_adjust_hystat(&payin,subpack.opeStateCode);      		

			payin.F_lw_ind[0]= '2';
			iLvNewRecFromPayin(&payin);

			if ( chkflg == 0) 	   /*  chkflg=0 登记  */
			{
				char cLvStat[1];
				vLvBrnoDebug(__FILE__,__LINE__);
				memset(cLvStat, 0 , sizeof(cLvStat));
				get_zd_data("0980",cLvStat);
				vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
				/* begin NEWTCTD1 */
				/* begin add by LiuHuafeng 2007-8-23 9:57:21 */
				get_zd_data("0890",cRcpstat);
				/* 增加状态处理根据应答状态进行处理的判断，如果应答状态为拒绝，无论挂帐检查与否都设置为不记帐状态 */
				if(iCheck3Flag!=0)
				{
					memcpy(cRcpstat, "090",3);
					set_zd_data("0890",cRcpstat);
					payin.T_stat[0]=STAT_LZRECV1;/* STAT_LZRECV1临时接收不记帐 */
				}
				iCheck3Flag=atoi(cRcpstat);
				vtcp_log("%s,%d NOTE 实时贷记回执在这里统一设置RCPSTAT的值 ＝[%s] ",__FL__,cRcpstat);
				memcpy(payin.F_rcpstat,cRcpstat+1,sizeof(payin.F_rcpstat));
				/* end by LiuHuafeng 2007-8-24 10:18:19 */
				/* end NEWTCTD1 */
				payin.T_stat[0]=cLvStat[0];
				payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
				get_fd_data(DC_TX_BR_NO,payin.T_brno);
				vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);						

				vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);
				iRc = iLvUpdRecFromPayin(&payin,1);
				if (iRc)
				{
					sprintf( acErrMsg,"插入实时贷记来账登记簿错误");
					WRITEMSG
					lv_destroy(&subpack);
					return(-1);
				}               		
				lv_destroy(&subpack); 
				/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
				if ( iSubpackIndex == 0 )  	   		        
				{
					/******插入来包控制表************/
					iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
					if (iRc)
					{
						vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"产生来包控制表记录错误");
						WRITEMSG
						lv_destroy(&subpack);
						return -1;
					}
					iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
					if (iRc)
					{
						vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"插入来包控制表出错");
						WRITEMSG
						lv_destroy(&subpack);
						return -1;
					}    		        	
				}          
				/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
				iSubpackIndex ++;
				set_zd_long("0520", iSubpackIndex);
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				if (index == recnt-1 )  /*  登记完最后一条明细时，处理实时贷记回执往账  */
				{
					/****实时贷记回执记账****/
					memset(&payin_hz,'\0',sizeof(payin_hz));
					iRc=iLvCreateHz(&payin,&payin_hz);
					vtcp_log("%s,%d wp_payin_hz->A_cashqsactno==[%s]",__FL__,payin_hz.A_cashqsactno);
					vtcp_log("%s,%d wp_payin_hz->A_payqsactno ==[%s]",__FL__,payin_hz.A_payqsactno);
					vtcp_log("%s,%d packday ===[%s] ",__FL__,payin_hz.F_packday);
					if (iCheck3Flag==0)
					{
						memcpy(payin_hz.F_rcpstat,"00",sizeof(payin_hz.F_rcpstat));
					}
					else if (iCheck3Flag>0)
					{
						apitoa(iCheck3Flag,sizeof(payin_hz.F_rcpstat),payin_hz.F_rcpstat);
					}
					else
					{
						apitoa(1,sizeof(payin_hz.F_rcpstat),payin_hz.F_rcpstat);
					}
					vtcp_log("[%s][%d]F_txnum=[%.5s],T_txnum=[%.5s]\n",__FILE__,__LINE__,
						payin_hz.F_txnum,payin_hz.T_txnum);
					memcpy(payin_hz.F_opcd,OPCD_SEND,sizeof(payin_hz.F_opcd));
					payin_hz.T_stat[0]=STAT_WZLR;
					payin_hz.F_packend[0]='1';
					vtcp_log("%s,%d packday ===[%s] ",__FL__,payin_hz.F_packday);
					vtcp_log("%s,%d wp_payin_hz->A_cashqsactno==[%s]",__FL__,payin_hz.A_cashqsactno);
					vtcp_log("%s,%d wp_payin_hz->A_payqsactno ==[%s]",__FL__,payin_hz.A_payqsactno);
					iRc=iLvPayinCheck8(&payin_hz,&wd_lv_wbctl,NULL);	
					vtcp_log("%s,%d wp_payin_hz->A_cashqsactno==[%s]",__FL__,payin_hz.A_cashqsactno);
					vtcp_log("%s,%d wp_payin_hz->A_payqsactno ==[%s]",__FL__,payin_hz.A_payqsactno);
					if (iRc)
					{
						vtcp_log("%s,%d,生成包记录失败[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"生成包记录失败");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}
					vtcp_log("%s,%d packday ===[%s] ",__FL__,payin_hz.F_packday);
					vtcp_log("%s,%d,生成包记录成功! ",__FILE__,__LINE__);		

					iLvNewRecFromPayin(&payin_hz);
					vtcp_log("%s,%d packid=[%s]",__FL__,payin_hz.F_packid);
					vtcp_log("[%s][%d]F_txnum=[%.5s],T_txnum=[%.5s]\n",__FILE__,__LINE__,
						payin_hz.F_txnum,payin_hz.T_txnum);
					memcpy(payin_hz.T_jzday,lvca.tbsdy,sizeof(payin_hz.T_jzday));
					vtcp_log("%s,%d 下边产生实时贷记回执数据",__FL__);
					iRc=iLvUpdRecFromPayin(&payin_hz,1);
					vtcp_log("%s,%d 已经产生实时贷记回执数据",__FL__);
					if (iRc)
					{
						vtcp_log("%s,%d,插入实时贷记回执往帐登记簿错误\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"插入回执登记簿失败");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}
					vtcp_log("%s,%d,插入实时贷记回执往帐登记簿成功! ",__FILE__,__LINE__);	
					vtcp_log("[%s][%d]F_txnum=[%.5s],T_txnum=[%.5s]\n",__FILE__,__LINE__,
						payin_hz.F_txnum,payin_hz.T_txnum);
					vtcp_log("%s,%d ",__FL__);
					/****发送回执报文****/
					/*commit_work(); for test */
					apitoa(wd_lv_wbctl.pack_date,8,cPackday);
					/* begin NEWTCTD1 */
					/* begin add by LiuHuafeng 2007-8-23 15:49:13 更新原来帐状态 */
					{
						char cOld_orderno[9];
						char cOld_orbrno [13];
						long lOld_wtdate=0;
						memset(cOld_orderno, 0 , sizeof(cOld_orderno));
						memset(cOld_orbrno , 0 , sizeof(cOld_orbrno ));
						lOld_wtdate=apatoi(payin.F_wtday,8);
						memcpy(cOld_orderno, payin.F_orderno ,sizeof(cOld_orderno)-1);
						memcpy(cOld_orbrno , payin.F_orbrno ,sizeof(cOld_orbrno )-1);
						g_reply_int=sql_execute("update lv_pkgreg set rcpstat='%.2s' where orderno='%s' and wt_date=%ld and or_br_no='%s' and lw_ind='2' and pkgno='003'",payin_hz.F_rcpstat,cOld_orderno,lOld_wtdate,cOld_orbrno);
						if(g_reply_int)
						{
							vtcp_log("%s,%d 更新原来帐应答状态失败 sqlcode=%d",__FILE__,__LINE__,g_reply_int);
						}
					}
					/* end by LiuHuafeng 2007-8-23 16:35:41 */
					/* end NEWTCTD1 */
					/* begin NEWTCTD2 */
					/* begin add by LiuHuafeng 2007-9-3 1:08:13 
					 分布提交
					**************************************/
					vtcp_log("%s,%d 注意，这里分布提交 ",__FILE__,__LINE__);
					sql_commit();
					/* end by LiuHuafeng 2007-9-3 1:08:36 */
					/* end NEWTCTD2 */
					vtcp_log("%s,%d 准备发送实时贷记回执报文",__FL__);
					iRc=lv_get_ssdjhz_tag(cPackday,wd_lv_wbctl.packid);
					if (iRc)
					{
						vtcp_log("%s,%d,发送实时贷记回执往帐错误\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"发送回执失败");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}
					vtcp_log("%s,%d,发送实时贷记回执往帐成功! ",__FILE__,__LINE__);
					/*  最后一条来帐明细 处理完了 !!! */
					vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
					set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/

				}	            
				return 0;
			}
			else                 /* chkflg=1 挂帐检查   */
			{
				char cLvStat[1];
				memset(cLvStat , 0 , sizeof(cLvStat));
				vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);

				set_zd_data("0500","0"); 
				vLvBrnoDebug(__FILE__,__LINE__);
				iRc = pubf_lv_pkg_hang_chk(&payin,"003");
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d ################来帐挂帐检测结果如下:###iRc=%d############",__FL__,iRc);
				/* begin NEWTCTD1 */
				/* begin add by LiuHuafeng 2007-9-1 0:31:07 */
				get_zd_data("0890",cRcpstat);
				vtcp_log("%s,%d #####rcpstat=[%s]:###############",__FL__,cRcpstat);
/* 增加状态处理根据应答状态进行处理的判断，如果应答状态为拒绝，无论挂帐检查与否忌柚梦患钦首刺� */
				if(iCheck3Flag!=0)
				{
					payin.T_stat[0]=STAT_LZRECV1;/* STAT_LZRECV1临时接收不记帐 */
					set_zd_data("0980",cLvStat);
					memcpy(cRcpstat,"090",3);
					set_zd_data("0890",cRcpstat);
					iRc=999;
				}
				/* end by LiuHuafeng 2007-9-1 0:31:17 */
				/* end NEWTCTD1	*/
				cLvStat[0]=payin.T_stat[0];
				vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
				vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

				if (iRc == 0)        /* 检查正确 走帐*/
				{      
					strcpy(g_pub_tx.reply, "0000" );
				}
				/* begin NEWTCTD1 */
				/* begin add by LiuHuafeng 2007-8-23 13:13:12 */
				else
				{
					vtcp_log("%s,%d 实时贷记来帐挂帐检查失败，所以不需要记帐，直接设置拒绝回执",__FILE__,__LINE__);
					/**20070912 wangyongwei NEWTCTD**/
					/*cLvStat[0]=payin.T_stat[0];*/
					cLvStat[0]=STAT_LZRECV1;
					set_zd_data("0980",cLvStat);
					vLvBrnoDebug(__FILE__,__LINE__);
					return 999;
				}
				/* rem by LiuHuafeng 2007-8-23 13:05:32 实时贷记业务如果检查出错，就直接拒绝，不存在挂帐情况 
				else if(iRc == 100)  * 挂帐 *
				{      
					strcpy(g_pub_tx.reply, "0000"  ); 
				}else	if(iRc)    * 挂帐检查出错 *
				{
					sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}
				*********************/
				/* end NEWTCTD1 */

				cLvStat[0]=payin.T_stat[0];
				set_zd_data("0980",cLvStat);
				vLvBrnoDebug(__FILE__,__LINE__);				

			}
			/*  已处理完毕 */
			vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
			lv_destroy(&subpack);
			return iRc;      	  
		}
		iTmpIndex ++;    
	}
	if ( iTmpIndex > iSubpackIndex )
	{
		vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
		WRITEMSG
			lv_destroy(&subpack);
		return -1;
	}  
	lv_destroy(&subpack);
	return(0);
}

/****定期贷记报文来账处理****/
int lv_deal_dqdjlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	struct lv_lbctl_c wd_lv_lbctl;
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   	

	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	/****检查是否普通贷记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_DQDJ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是定期贷记来账\n",__FILE__,__LINE__,wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是定期贷记报文");WRITEMSG
			return(-1);
	}
	while (1)
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy
		***************************************************************/
		memset(&subpack,0,sizeof(TLVPACKAGE));		
		lv_create(&subpack);
		iRc=lv_fetchpkg(wp_lvpack,&subpack);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");WRITEMSG
				lv_destroy(&subpack);
			return(-1);
		}else if(iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/****得到明细业务的笔数****/
		recnt=lv_count_recnt(&subpack);
		vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
		/* 说明iSubpackIndex不在这笔子报文中 
		if ( iSubpackIndex >= iTmpIndex+recnt)
		{
		vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
		WRITEMSG
		lv_destroy(&subpack);
		return -1;
		}			*/

		for (index=0;index<recnt;index++)
		{
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{
				memset(&payin,'\0',sizeof(payin));
				iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}      
				memcpy(payin.T_jzday,lvca.tbsdy,sizeof(payin.T_jzday));
				payin.F_lw_ind[0]= '2';
				iLvNewRecFromPayin(&payin);

				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);		

					vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);
					iRc=iLvUpdRecFromPayin(&payin,1);
					if (iRc)
					{
						vtcp_log("%s,%d,插入定期贷记来账登记簿错误\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"插入定期贷记来账登记簿错误");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}           		
					lv_destroy(&subpack); 

					/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								lv_destroy(&subpack);
							return -1;
						}
						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
								lv_destroy(&subpack);
							return -1;
						}    		        	
					}                 		
					/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					iSubpackIndex ++;
					set_zd_long("0520", iSubpackIndex);
					/*  这一轮登记 已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					lv_destroy(&subpack);
					return 0;
				}
				else                 /* chkflg=1 挂帐检查   */
				{
					char cLvStat[1];
					memset(cLvStat , 0 , sizeof(cLvStat));
					vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);

					set_zd_data("0500","0"); 
					vLvBrnoDebug(__FILE__,__LINE__);
					iRc = pubf_lv_pkg_hang_chk(&payin,"005");
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
					cLvStat[0]=payin.T_stat[0];
					vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);					

					if (iRc == 0)        /* 检查正确 走帐*/
					{      
						strcpy(g_pub_tx.reply, "0000" );  
					}else if(iRc == 100)  /* 挂帐 */
					{      
						strcpy(g_pub_tx.reply, "0000"  ); 
					}
					/* begin add by LiuHuafeng 2009-5-13 17:17:26 for 村镇银行 */
					else if( iRc == 999 )
					{
						strcpy(g_pub_tx.reply, "0000"  ); 
					}
					/* end by LiuHuafeng 2009-5-13 17:17:47 */
					else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}  
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);
					vLvBrnoDebug(__FILE__,__LINE__);					

				}
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				lv_destroy(&subpack); 
				return iRc;  
			}
			iTmpIndex ++;  
		}/*end of for */
		if ( iTmpIndex > iSubpackIndex )
		{
			vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
			sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
			WRITEMSG
				lv_destroy(&subpack);
			return -1;
		}  
	} /* end of while */
	return(0);
}

/****贷记退汇报文来账处理****/
int lv_deal_djthlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	struct lv_lbctl_c wd_lv_lbctl;
	char cWssrno[7];
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   

	get_zd_data("0040",cWssrno );   /**流水号**/    
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	/****检查是否贷记退汇来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_DJTH,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是贷记退汇来账\n",__FILE__,__LINE__,wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是贷记退汇报文");WRITEMSG
			return(-1);
	}
	while (1)
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy
		***************************************************************/
		memset(&subpack,0,sizeof(TLVPACKAGE));		
		lv_create(&subpack);
		iRc=lv_fetchpkg(wp_lvpack,&subpack);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");WRITEMSG
				lv_destroy(&subpack);
			return(-1);
		}else if(iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/****得到明细业务的笔数****/
		recnt=lv_count_recnt(&subpack);
		vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
		/* 说明iSubpackIndex不在这笔子报文中 
		if ( iSubpackIndex >= iTmpIndex+recnt) 
		{
		iTmpIndex = iTmpIndex + recnt;
		if ( iTmpIndex > iSubpackIndex )
		{
		vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
		WRITEMSG
		lv_destroy(&subpack);
		return -1;
		}
		continue;    			
		}		*/

		for (index=0;index<recnt;index++)
		{
			PAY_IN_AREA opay;/****原往帐支付交易***/			
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{
				vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 

				memset(&payin,'\0',sizeof(payin));
				memcpy(payin.A_wssrno, cWssrno, sizeof(payin.A_wssrno));
				memcpy(payin.T_recvwssrno, payin.A_wssrno, sizeof(payin.T_recvwssrno));
				iRc = lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}		
				payin.F_lw_ind[0]= '2';
				/** rem by LiuHuafeng 2006-10-6 17:18 怎么可以在这里加状态?
				payin.T_stat[0]= 'H';
				payin.T_begstat[0]= 'H';
				***/
				char cPayqsno[13];
				char cCashqsno[13];	
				char cSendco[5];	
				char cRececo[5];	

				memset(cPayqsno, 0, sizeof(cPayqsno));
				memset(cCashqsno, 0, sizeof(cCashqsno));
				memset(cSendco, 0, sizeof(cSendco));
				memset(cRececo, 0, sizeof(cRececo));   

				vtcp_log("%s,%d,payin.T_payqsactno=[%.12s]\n",__FILE__,__LINE__,payin.T_payqsactno);     
				vtcp_log("%s,%d,payin.T_cashqsactno=[%.12s]\n",__FILE__,__LINE__,payin.T_cashqsactno);	

				memcpy( cPayqsno,payin.T_payqsactno, sizeof(cPayqsno)-1);
				memcpy( cCashqsno,payin.T_cashqsactno, sizeof(cPayqsno)-1);

				vtcp_log("%s,%d,cCashqsno=[%.12s]\n",__FILE__,__LINE__,cCashqsno);     
				vtcp_log("%s,%d,cPayqsno=[%.12s]\n",__FILE__,__LINE__,cPayqsno);	

				if ( iGetSendco(cSendco, cCashqsno) )
				{
					vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
					/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
					WRITEMSG
					return(-1);*/
				}
				vtcp_log("%s,%d,cSendco=[%s]\n",__FILE__,__LINE__,cSendco);              	

				if (iGetSendco(cRececo, cPayqsno))
				{
					vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
					/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
					WRITEMSG
					return(-1);*/
				}
				vtcp_log("%s,%d,cRececo=[%.4s]\n",__FILE__,__LINE__,cRececo);                   

				memcpy( payin.T_sendco, cSendco, sizeof(payin.T_sendco));
				memcpy( payin.T_receco, cRececo, sizeof(payin.T_receco));	

				iLvNewRecFromPayin(&payin);			        

				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);							

					iRc = iLvUpdRecFromPayin(&payin,1);
					if (iRc)
					{
						sprintf( acErrMsg,"插入普通贷记来账登记簿错误 iRc===[%d]",iRc);
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}            
					/****更新原交易****/
					memset(&opay,'\0',sizeof(opay));
					memcpy(opay.F_wtday,payin.F_owtday,	sizeof(opay.F_wtday));
					memcpy(opay.F_orbrno,payin.F_acbrno, sizeof(opay.F_orbrno));
					memcpy(opay.F_orderno,payin.F_oorderno, sizeof(opay.F_orderno));
					memcpy(opay.F_opcd,OPCD_SEND, sizeof(opay.F_opcd));
					memcpy(opay.F_pkgno,PKGNO_PTDJ,	sizeof(opay.F_pkgno));
					opay.F_lw_ind[0]= '1';   

					iRc=iLvFetchRecToPayin(&opay);  /*找普通贷记*/
					if (iRc!=0)
					{
						memcpy(opay.F_pkgno,PKGNO_DQDJ,sizeof(opay.F_pkgno));
						iRc=iLvFetchRecToPayin(&opay); /*找定期贷记*/
					}
					if (iRc)
					{
						vtcp_log("%s,%d,未找到原交易,但此笔退汇照单收着\n",__FILE__,__LINE__);
						lvca.rtcode=0;
					}
					else	/***更新状态****/
					{
						opay.T_stat[0]=STAT_WZBACK;/****往帐退回****/
						iRc=iLvUpdRecFromPayin(&opay,0); /**更新**/
						if (iRc)
						{
							vtcp_log("%s,%d,更新原交易错误,但此笔退汇照单收着\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"更新原交易错误,但此笔退汇照单收着");
							WRITEMSG
								lvca.rtcode=0;
						}
						vtcp_log("%s,%d,更新原交易成功!\n",__FILE__,__LINE__);
					}             			
					lv_destroy(&subpack); 

					/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								return -1;
						}
						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
								return -1;
						}    		        	
					}    
					/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					iSubpackIndex ++;
					set_zd_long("0520", iSubpackIndex);
					/*  已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					return 0;
				}
				else	 /* chkflg=1 挂帐检查   */
				{
					char cLvStat[1];
					memset(cLvStat, 0 , sizeof(cLvStat));

					set_zd_data("0500","0"); 
					iRc = pubf_lv_pkg_hang_chk(&payin,"007");
					/* 第一次设置来帐的状态域,以后可能会根据帐户处理检查函数的状态来在此设置该域的值 */
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

					if (iRc == 0)        /* 检查正确 走帐*/
					{
						strcpy(g_pub_tx.reply, "0000" );	
					}
					else if(iRc == 100)  /* 挂帐 */
					{      
						strcpy(g_pub_tx.reply, "0000"  ); 
					}
					/* begin add by xyy 2010-7-22 10:58:46 for 村镇银行 */
					else if( iRc == 999 )
					{
						strcpy(g_pub_tx.reply, "0000"  ); 
					}
					/* end by xyy 2010-7-22 10:58:51 */
					else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}  
					vLvBrnoDebug(__FILE__,__LINE__);
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);

					lv_destroy(&subpack);      		
				}
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				return iRc;			        
			}
			iTmpIndex ++;    			
		}/*end of for */	
	}
	lv_destroy(&subpack);	
	return(0);
}

/****普通借记报文来账处理****/
int lv_deal_ptjjlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	struct lv_lbctl_c wd_lv_lbctl;
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   

	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));

	/****检查是否普通借记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_PTJJ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是普通借记来账\n",__FILE__,__LINE__,
			wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是普通借记报文");
		return(-1);
	}
	while (1)
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy
		***************************************************************/
		memset(&subpack,0,sizeof(TLVPACKAGE));
		lv_create(&subpack);
		vtcp_log("%s,%d,BEFORE FETCH...............\n",__FILE__,__LINE__);

		iRc=lv_fetchpkg(wp_lvpack,&subpack);
		vtcp_log("%s,%d,AFTER FETCH...............\n",__FILE__,__LINE__);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");
			WRITEMSG
				lv_destroy(&subpack);
			return(-1);
		}else if(iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/****得到明细业务的笔数****/
		recnt=lv_count_recnt(&subpack);
		vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
		/* 说明iSubpackIndex不在这笔子报文中 
		if ( iSubpackIndex >= iTmpIndex+recnt)
		{
		iTmpIndex = iTmpIndex + recnt;
		if ( iTmpIndex > iSubpackIndex )
		{
		vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
		WRITEMSG
		lv_destroy(&subpack);
		return -1;
		}
		continue;    			
		}*/

		for (index=0;index<recnt;index++)
		{
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{    		
				char cPayqsno[13];
				char cCashqsno[13];
				char cSendco[5];
				char cRececo[5];
				memset(cPayqsno, 0, sizeof(cPayqsno));
				memset(cCashqsno, 0, sizeof(cCashqsno));
				memset(cSendco, 0, sizeof(cSendco));
				memset(cRececo, 0, sizeof(cRececo));

				vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 
				memset(&payin,'\0',sizeof(payin));
				iRc = lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}
				vtcp_log("%s,%d,F_ywtype=[%.12s]\n",__FILE__,__LINE__,payin.F_ywtype);

				payin.F_lw_ind[0]= '2';

				iLvNewRecFromPayin(&payin);
				vtcp_log("[%s][%d]iLvNewRecFromPayin over!",__FILE__,__LINE__);
			#ifdef __ZPHZ_30103__
				/****NEWYX BEGIN:支票来帐不记录包信息,方便回执拼包****/
				if (memcmp(payin.F_txnum,TXNUM_ZPJL,sizeof(payin.F_txnum))==0||memcmp(payin.F_txnum,TXNUM_TYJL,sizeof(payin
					.F_txnum))==0)
				{
					memset(payin.F_packday,'\0',sizeof(payin.F_packday));
					memset(payin.F_packid,'\0',sizeof(payin.F_packid));
					memset(payin.T_packday,'\0',sizeof(payin.T_packday));
					memset(payin.T_packid,'\0',sizeof(payin.T_packid));
				}
				/****NEWYX END:支票来帐不记录包信息,方便回执拼包****/
			#endif
				vtcp_log("%s,%d,T_ywtype=[%.12s]\n",__FILE__,__LINE__,payin.T_ywtype);
				payin.T_stat[0]= 'H';
				payin.T_begstat[0]= 'H';

				memcpy( cPayqsno,payin.T_payqsactno, sizeof(cPayqsno)-1);
				memcpy( cCashqsno,payin.T_cashqsactno, sizeof(cPayqsno)-1);

				vtcp_log("%s,%d,cCashqsno=[%.12s]\n",__FILE__,__LINE__,cCashqsno);     
				vtcp_log("%s,%d,cPayqsno=[%.12s]\n",__FILE__,__LINE__,cPayqsno);	

				if ( iGetSendco(cSendco, cCashqsno) )
				{
					vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
					/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
					WRITEMSG
					return(-1);*/
				}
				vtcp_log("%s,%d,cSendco=[%s]\n",__FILE__,__LINE__,cSendco);              	

				if (iGetSendco(cRececo, cPayqsno))
				{
					vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
					/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
					WRITEMSG
					return(-1);*/
				}
				vtcp_log("%s,%d,cRececo=[%.4s]\n",__FILE__,__LINE__,cRececo);                   

				memcpy( payin.T_sendco, cSendco, sizeof(payin.T_sendco));
				memcpy( payin.T_receco, cRececo, sizeof(payin.T_receco));	

				/* pubf_lv_print_PAY_IN_AREA(&payin);  */

				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);							
					vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);

					iRc = iLvUpdRecFromPayin(&payin,1);
					if (iRc)
					{
						sprintf( acErrMsg,"插入普通贷记来账登记簿错误");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}               		
					lv_destroy(&subpack); 
					/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						vtcp_log("[%s],[%d],wd_lv_lbctl.o_pack_date===[%d]",__FL__,wd_lv_lbctl.o_pack_date);                       	
						vtcp_log("[%s][%d] wd_lv_lbctl.hz_date==[%d] \n",__FILE__,__LINE__,wd_lv_lbctl.hz_date);

						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								return -1;
						}
						vtcp_log("[%s],[%d],wd_lv_lbctl.o_pack_date===[%d]",__FL__,wd_lv_lbctl.o_pack_date);                       	
						vtcp_log("[%s][%d] wd_lv_lbctl.hz_date==[%d] \n",__FILE__,__LINE__,wd_lv_lbctl.hz_date);

						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
								return -1;
						}   
						vtcp_log("[%s],[%d],wd_lv_lbctl.o_pack_date===[%d]",__FL__,wd_lv_lbctl.o_pack_date);                       	
						vtcp_log("[%s][%d] wd_lv_lbctl.hz_date==[%d] \n",__FILE__,__LINE__,wd_lv_lbctl.hz_date);

					}                 		
					/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					iSubpackIndex ++;
					set_zd_long("0520", iSubpackIndex);
					/*  已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					return 0;
				}
				else	 /* chkflg=1 挂帐检查   */
				{
					set_zd_data("0500","0"); 
					iRc = pubf_lv_pkg_hang_chk(&payin,"002");
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

					if (iRc == 0 || iRc == 100)        /* 走帐*/
					{      
						strcpy(g_pub_tx.reply, "0000" );  
					}else if(iRc == 999)    /* 不用走帐了 */
					{      
						strcpy(g_pub_tx.reply, "0010"  );
						return 999;
					}else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}  
				}
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				lv_destroy(&subpack);  
				return iRc;              	
			}
			iTmpIndex ++;              	
		}/*end of for */
		if ( iTmpIndex > iSubpackIndex )
		{
			vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
			sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
			WRITEMSG
				lv_destroy(&subpack);
			return -1;
		}     		
	}  /* end of while */
	/** 不用记帐 **/
	lv_destroy(&subpack);
	return(0);
}

/****实时借记报文来账处理****/
int lv_deal_ssjjlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	PAY_IN_AREA payin_hz;
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;
	int iCheck2Flag=1;
	char cPack_date[9];
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-9-2 0:49:12 */
	char cRcpstat[4];
	memset(cRcpstat , 0 , sizeof(cRcpstat));
	get_zd_data("0890",cRcpstat);
	vtcp_log("%s,%d chkflg=%d rcpstat=%s",__FILE__,__LINE__,chkflg,cRcpstat);
	/* end by LiuHuafeng 2007-9-2 0:49:37 */
	/* end NEWTCTD1 */

	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
	memset(cPack_date,'\0',sizeof(cPack_date));	

	/****检查是否实时借记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_SSJJ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是实时借记来账\n",__FILE__,__LINE__,	wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是实时借记报文");WRITEMSG
			return(-1);
	}
	/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
	*   lv_destroy
	***************************************************************/
	memset(&subpack,0,sizeof(TLVPACKAGE));	
	lv_create(&subpack);
	iRc=lv_fetchpkg(wp_lvpack,&subpack);
	if (iRc<0)
	{
		vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"提取报文时出错");WRITEMSG
			lv_destroy(&subpack);
		return(-1);
	}else if(iRc>0)
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
		lv_destroy(&subpack);
		return 0;
	}
	/****得到明细业务的笔数****/
	recnt=lv_count_recnt(&subpack);
	vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);

	if ( iSubpackIndex >= iTmpIndex+recnt) 
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");  
		lv_destroy(&subpack);		  
		return 0;  			
	}
	for (index=0; index<recnt; index++)
	{
		if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
		{
			vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 
			memset(&payin,'\0',sizeof(payin));

			iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
			if (iRc<0)
			{
				vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
				sprintf( acErrMsg,"根据子报文填写payin失败");
				lv_destroy(&subpack);
				return(-1);
			}
			 vtcp_log("[%s][%d]wd_lv_addit.addid=[%s]\n",__FL__,payin.T_addid);
			iLvGetRecvBrno(&payin);/****Add by SSH,2005.12.2,设置A_brno**/
			memcpy(lvca.brno,payin.A_brno,sizeof(lvca.brno)); 		/****实时借记,直接记支行****/
			pcLvGetRecvno();
			vtcp_log("[%s][%d]wd_lv_addit.addid=[%s]\n",__FL__,payin.T_addid);

			payin.F_lw_ind[0]= '2';
			iLvNewRecFromPayin(&payin);
			vtcp_log("%s,%d,********此时addid的值为[%s],chkflg[%d]\n",__FILE__,__LINE__,payin.T_addid,chkflg);

			if ( chkflg == 0) 	   /*  chkflg=0 登记   */
			{
				char cLvStat[1];
				vLvBrnoDebug(__FILE__,__LINE__);
				memset(cLvStat, 0 , sizeof(cLvStat));
				get_zd_data("0980",cLvStat);
				vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
				payin.T_stat[0]=cLvStat[0];
				payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
				get_fd_data(DC_TX_BR_NO,payin.T_brno);
				vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);						
				vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);

				iRc = iLvUpdRecFromPayin(&payin,1);
				if (iRc)
				{
					sprintf( acErrMsg,"插入实时贷记来账登记簿错误");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}               		
				lv_destroy(&subpack); 

				/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
				if ( iSubpackIndex == 0 )  	   		        
				{
					/******插入来包控制表************/
					iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
					if (iRc)
					{
						vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"产生来包控制表记录错误");
						WRITEMSG
							return -1;
					}
					vtcp_log("[%s],[%d],wd_lv_lbctl.o_pack_date===[%d]",__FL__,wd_lv_lbctl.o_pack_date);                          	
					vtcp_log("[%s],[%d],wd_lv_lbctl.o_pack_date===[%d]",__FL__,wd_lv_lbctl.o_pack_date);                          	
					vtcp_log("[%s],[%d],wd_lv_lbctl.o_pack_date===[%d]",__FL__,wd_lv_lbctl.o_pack_date);                       	

					iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
					if (iRc)
					{
						vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"插入来包控制表出错");
						WRITEMSG
							return -1;
					}    		        	
				}                 		
				/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
				iSubpackIndex ++;
				set_zd_long("0520", iSubpackIndex);

				if (index == recnt-1 )  /*  登记完最后一条明细时，处理实时借记回执往账  */
				{
					/****实时借记回执往账****/
					memset(&payin_hz,'\0',sizeof(payin_hz));
					iRc=iLvCreateHz(&payin,&payin_hz);
					vtcp_log("%s,%d F_pkgno ===[%.3s] ",__FL__,payin_hz.F_pkgno);	
					vtcp_log("%s,%d F_packid ===[%.8s] ",__FL__,payin_hz.F_packid);						
					vtcp_log("%s,%d F_packday ===[%.8s] ",__FL__,payin_hz.F_packday);						
					vtcp_log("%s,%d F_lw_ind ===[%.1s] ",__FL__,payin_hz.F_lw_ind);	
					vtcp_log("%s,%d F_lvsts ===[%.1s] ",__FL__,payin_hz.F_lvsts);	
					vtcp_log("%s,%d F_cashqsno ===[%.12s] ",__FL__,payin_hz.F_cashqsno);	

					vtcp_log("%s,%d wp_payin_hz->A_cashqsactno==[%s]",__FL__,payin_hz.A_cashqsactno);
					vtcp_log("%s,%d wp_payin_hz->A_payqsactno ==[%s]",__FL__,payin_hz.A_payqsactno);
					vtcp_log("%s,%d packday ===[%s] ",__FL__,payin_hz.F_packday);

					vtcp_log("%s,%d wd_lv_wbctl.o_packid ===[%.8s] ",__FL__,wd_lv_wbctl.o_packid);	
					vtcp_log("%s,%d wd_lv_wbctl.o_pack_date ===[%d] ",__FL__,wd_lv_wbctl.o_pack_date);					
					vtcp_log("%s,%d wd_lv_wbctl.cash_qs_no ===[%.12s] ",__FL__,wd_lv_wbctl.cash_qs_no);							
					/* begin NEWTCTD1 */
					/* begin add by LiuHuafeng 2007-9-2 0:52:04 */
					get_zd_data("0890",cRcpstat);
					vtcp_log("%s,%d chkflg=%d rcpstat=%s",__FILE__,__LINE__,chkflg,cRcpstat);
					iCheck2Flag=atoi(cRcpstat);
					/* end by LiuHuafeng 2007-9-2 0:49:37 */
					/* end NEWTCTD1 */

					vtcp_log("[%s][%d]F_txnum=[%.5s],T_txnum=[%.5s]\n",__FILE__,__LINE__, payin_hz.F_txnum,payin_hz.T_txnum);
					memcpy(payin_hz.F_opcd,OPCD_SEND,sizeof(payin_hz.F_opcd));
					payin_hz.T_stat[0] = STAT_WZLR;
					payin_hz.F_packend[0]='1';

					vtcp_log("%s,%d,Gethere,check2=[%d]\n",__FILE__,__LINE__,iCheck2Flag);
					/****不付款
					iRc=iLvPayinCheck2_Batch(&payin_hz);	
					if(iRc)
					{
						vtcp_log("%s,%d,Gethere,check2=[%d]\n",__FILE__,__LINE__,iCheck2Flag);
						vtcp_log("%s,%d,付款帐号检查失败[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"付款帐号检查失败");
						WRITEMSG
						iCheck2Flag=iRc;
					}else
					{
						vtcp_log("%s,%d,Gethere,check2=[%d]\n",__FILE__,__LINE__,iCheck2Flag);
						iCheck2Flag=0;
					} 
					*****/
					/* rem by LiuHuafeng 2007-9-2 0:53:10
					iCheck2Flag=0;	**TODO:测试付款***/
					vtcp_log("%s,%d,Gethere,check2=[%d]\n",__FILE__,__LINE__,iCheck2Flag);
					/* 放的位置不正确，应该放到chkflg==1的分支中的pubf_lv_pkg_hang_chk中
					if(iCheck2Flag==0){
						****NEWTCTD:如果帐号检查通过则检查是否储蓄帐号,密码是否正确*
						TD_ADD_AREA wd_td;
						struct mdm_ac_rel_c mdm_ac_rel;
						memset(&wd_td,'\0',sizeof(wd_td));
						memset(&mdm_ac_rel,'\0',sizeof(mdm_ac_rel));
						iRc=iLvGetAddit(&payin,"000",&wd_td);
						vtcp_log("%s,%d,Gethere,check2=[%d]\n",__FILE__,__LINE__,iCheck2Flag);
						iRc=nBepsDescrypt(&payin,&wd_td);
						CBshuff(wd_td.pswdcode);
						memcpy(mdm_ac_rel.ac_no,payin.F_payactno,sizeof(mdm_ac_rel.ac_no)-1);
						pub_base_strpack(mdm_ac_rel.ac_no);
						****TODO:??在此是否需要检查新旧账号对照表****
						iRc=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",mdm_ac_rel.ac_no);
						if(iRc){
							vtcp_log("%s,%d,付款帐号非储蓄帐号",__FILE__,__LINE__);
							iCheck2Flag=1;*帐号不存在****
						}
						if(mdm_ac_rel.draw_pwd_yn[0]!='Y'){
							vtcp_log("%s,%d,无密户不允许通兑",__FILE__,__LINE__);
							iCheck2Flag=1;**帐号不存在****
						}
						if(memcmp(mdm_ac_rel.draw_pwd,wd_td.pswdcode,6)!=0){
							vtcp_log("%s,%d,付款帐号密码不正确",__FILE__,__LINE__);
							iCheck2Flag=10;**帐号密码错误****
						}
					}
					****************************************************/
					if (iCheck2Flag==0)
					{
						memcpy(payin_hz.F_rcpstat,"00",sizeof(payin_hz.F_rcpstat));
					}else if(iCheck2Flag>0)
					{
						apitoa(iCheck2Flag,sizeof(payin_hz.F_rcpstat),payin_hz.F_rcpstat);
					}
					else
					{
						apitoa(1,sizeof(payin_hz.F_rcpstat),payin_hz.F_rcpstat);
					}
					memcpy(payin_hz.F_opcd,OPCD_CHECK,sizeof(payin_hz.F_opcd));
					memcpy(payin_hz.F_opcd,OPCD_SEND,sizeof(payin_hz.F_opcd));
					payin_hz.T_stat[0]=STAT_WZCHECK2;
					payin_hz.F_packend[0]='1';
					vtcp_log("%s,%d wp_payin_hz->A_cashqsactno==[%s]",__FL__,payin_hz.A_cashqsactno);
					vtcp_log("%s,%d wp_payin_hz->A_payqsactno ==[%s]",__FL__,payin_hz.A_payqsactno);

					iRc=iLvPayinCheck8(&payin_hz,&wd_lv_wbctl,NULL);	
					if (iRc)
					{
						vtcp_log("%s,%d,生成包记录失败[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"生成包记录失败");
						WRITEMSG
						return(-1);
					}
					vtcp_log("%s,%d F_pkgno ===[%.3s] ",__FL__,payin_hz.F_pkgno);	
					vtcp_log("%s,%d F_packid ===[%.8s] ",__FL__,payin_hz.F_packid);						
					vtcp_log("%s,%d F_packday ===[%.8s] ",__FL__,payin_hz.F_packday);						
					vtcp_log("%s,%d F_lw_ind ===[%.1s] ",__FL__,payin_hz.F_lw_ind);	
					vtcp_log("%s,%d F_lvsts ===[%.1s] ",__FL__,payin_hz.F_lvsts);	
					vtcp_log("%s,%d F_cashqsno ===[%.12s] ",__FL__,payin_hz.F_cashqsno);

					vtcp_log("%s,%d wd_lv_wbctl.o_packid ===[%.8s] ",__FL__,wd_lv_wbctl.o_packid);	
					vtcp_log("%s,%d wd_lv_wbctl.o_pack_date ===[%d] ",__FL__,wd_lv_wbctl.o_pack_date);					
					vtcp_log("%s,%d wd_lv_wbctl.cash_qs_no ===[%.12s] ",__FL__,wd_lv_wbctl.cash_qs_no);							

					iLvNewRecFromPayin(&payin_hz); 
					vtcp_log("%s,%d F_pkgno ===[%.3s] ",__FL__,payin_hz.F_pkgno);	
					vtcp_log("%s,%d F_packid ===[%.8s] ",__FL__,payin_hz.F_packid);						
					vtcp_log("%s,%d F_packday ===[%.8s] ",__FL__,payin_hz.F_packday);						
					vtcp_log("%s,%d F_lw_ind ===[%.1s] ",__FL__,payin_hz.F_lw_ind);	
					vtcp_log("%s,%d F_lvsts ===[%.1s] ",__FL__,payin_hz.F_lvsts);	
					vtcp_log("%s,%d F_cashqsno ===[%.12s] ",__FL__,payin_hz.F_cashqsno);	

					iRc=iLvUpdRecFromPayin(&payin_hz,1);
					if (iRc)
					{
						vtcp_log("%s,%d,插入实时借记回执往帐登记簿错误\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"插入回执登记簿失败");
						WRITEMSG
						return(-1);
					}
					vtcp_log("%s,%d T_pkgno ===[%.3s] ",__FL__,payin_hz.T_pkgno);	
					vtcp_log("%s,%d T_packid ===[%.8s] ",__FL__,payin_hz.T_packid);						
					vtcp_log("%s,%d T_packday ===[%.8s] ",__FL__,payin_hz.T_packday);						
					vtcp_log("%s,%d T_lw_ind ===[%.1s] ",__FL__,payin_hz.T_lw_ind);	
					vtcp_log("%s,%d T_stat ===[%.1s] ",__FL__,payin_hz.T_stat);	
					vtcp_log("%s,%d T_begstat ===[%.1s] ",__FL__,payin_hz.T_begstat);	
					vtcp_log("%s,%d T_cashqsactno ===[%.12s] ",__FL__,payin_hz.T_cashqsactno);	

					vtcp_log("%s,%d,插入实时借记回执往帐登记簿成功! ",__FILE__,__LINE__);	
					vtcp_log("[%s][%d]F_txnum=[%.5s],T_txnum=[%.5s]\n",__FILE__,__LINE__,	payin_hz.F_txnum,payin_hz.T_txnum); 

					vtcp_log("wd_lv_wbctl.pack_date===[%d]    wd_lv_wbctl.pack_date====[%s] ",wd_lv_wbctl.pack_date,wd_lv_wbctl.packid);	
					/****发送回执报文****/
					apitoa(wd_lv_wbctl.pack_date,8,cPack_date);
					vtcp_log("cPack_date=====[%s]   ",cPack_date);	
					/* begin NEWTCTD2 */
					/* begin add by LiuHuafeng 2007-9-3 1:08:13 
					 分布提交
					**************************************/
					{
						char cOld_orderno[9];
						char cOld_orbrno [13];
						long lOld_wtdate=0;
						memset(cOld_orderno, 0 , sizeof(cOld_orderno));
						memset(cOld_orbrno , 0 , sizeof(cOld_orbrno ));
						lOld_wtdate=apatoi(payin_hz.F_owtday,8);
						memcpy(cOld_orderno, payin_hz.F_oorderno ,sizeof(cOld_orderno)-1);
						memcpy(cOld_orbrno , payin_hz.F_o_or_br_no ,sizeof(cOld_orbrno )-1);
						vtcp_log("%s,%d orderno=%s,orbrno=%s,wt_date=%ld",__FILE__,__LINE__,cOld_orderno,cOld_orbrno,lOld_wtdate);
						/**add by wangyongwei NEWTCTD*20070905**/
						char tmp_lvstat[2];
						memset(tmp_lvstat,0,sizeof(tmp_lvstat));
						if(memcmp(payin_hz.F_rcpstat,"00",2)==0){
							tmp_lvstat[0]=STAT_YES_NOTSEND;	
						}else{
							tmp_lvstat[0]=STAT_NO_NOTSEND;
						}
						/**add by wangyongwei NEWTCTD*20070905**/
						g_reply_int=sql_execute("update lv_pkgreg set rcpstat='%.2s' ,lv_sts='%s' where orderno='%s' and wt_date=%ld and or_br_no='%s' and lw_ind='2' and pkgno='004' ",payin_hz.F_rcpstat,tmp_lvstat,cOld_orderno,lOld_wtdate,cOld_orbrno);
						if(g_reply_int)
						{
							vtcp_log("%s,%d 更新原来帐应答状态失败 sqlcode=%d",__FILE__,__LINE__,g_reply_int);
						}						
					}				
					vtcp_log("%s,%d 注意，这里分布提交 ",__FILE__,__LINE__);
					sql_commit();
					/* end by LiuHuafeng 2007-9-3 1:08:36 */
					/* end NEWTCTD2 */

					iRc=lv_get_ssjjhz_tag(cPack_date,wd_lv_wbctl.packid);
					if (iRc)
					{
						vtcp_log("%s,%d,发送实时借记回执往帐错误\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"发送回执失败");
						WRITEMSG
						return(-1);
					}
					/*  取后一条明细 已处理完毕*/	
					vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
					set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
					/* begin NEWTCTD1 */
					/* begin add by LiuHuafeng 2007-9-2 15:14:30 */
					/* 开始更新原来帐借记业务应答状态rcpstat */
					{
						char cOld_orderno[9];
						char cOld_orbrno [13];
						long lOld_wtdate=0;
						memset(cOld_orderno, 0 , sizeof(cOld_orderno));
						memset(cOld_orbrno , 0 , sizeof(cOld_orbrno ));
						lOld_wtdate=apatoi(payin_hz.F_owtday,8);
						memcpy(cOld_orderno, payin_hz.F_oorderno ,sizeof(cOld_orderno)-1);
						memcpy(cOld_orbrno , payin_hz.F_o_or_br_no ,sizeof(cOld_orbrno )-1);
						vtcp_log("%s,%d orderno=%s,orbrno=%s,wt_date=%ld",__FILE__,__LINE__,cOld_orderno,cOld_orbrno,lOld_wtdate);

						g_reply_int=sql_execute("update lv_pkgreg set rcpstat='%.2s' where orderno='%s' and wt_date=%ld and or_br_no='%s' and lw_ind='2' and pkgno='004' ",\
						                       payin_hz.F_rcpstat,cOld_orderno,lOld_wtdate,cOld_orbrno);
						if(g_reply_int)
						{
							vtcp_log("%s,%d 更新原来帐应答状态失败 sqlcode=%d",__FILE__,__LINE__,g_reply_int);
						}
					}				
					/* end by LiuHuafeng 2007-9-2 15:14:36 */

				}
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				return 0;
			}
			else                 /*  chkflg=1 挂帐检查   */
			{
				char cLvStat[1];
				int iRet=0; /**add by YHBP**/
				char cTmpStat[4];
                memset(cTmpStat, 0 , sizeof(cTmpStat));
				memset(cLvStat , 0 , sizeof(cLvStat));

				vtcp_log("%s,%d ################来帐挂帐检查开始###############\n",__FL__);
				vLvBrnoDebug(__FILE__,__LINE__);
				iRc = pubf_lv_pkg_hang_chk(&payin,"004");
				vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############[%d]\n",__FILE__,__LINE__,iRc);
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############\n",__FILE__,__LINE__);
				 /**begin YHBP**/
        get_zd_data("0890",cTmpStat);
        
         vtcp_log("[%s][%d]请注意回执状态[%.3s]\n",__FILE__,__LINE__,cTmpStat);
        if(memcmp(payin.T_txnum,"30103",sizeof(payin.T_txnum))==0 && memcmp(cTmpStat,"000",3)==0)
        {
            TYJL1_ADD_AREA lv_tyjl1;
            TYJL4_ADD_AREA lv_tyjl4;
          	struct cashier_c wd_cashier;
          	struct hv_uniontab_c  Hv_uniontab;
          	
          	memset(&wd_cashier  , 0 , sizeof(wd_cashier));
          	memset(&lv_tyjl1    , 0 , sizeof(lv_tyjl1));
          	memset(&lv_tyjl4    , 0 , sizeof(lv_tyjl4));
          	memset(&Hv_uniontab , 0 , sizeof(Hv_uniontab));
          	
          	vtcp_log("[%s][%d] payin->T_addid =[%s]\n",__FILE__,__LINE__,payin.T_addid);
            vtcp_log("[%s][%d] payin->F_wtday =[%s]\n",__FILE__,__LINE__,payin.F_wtday);
          	iLvGetAddit(&payin,"001",(char *)&lv_tyjl1,sizeof(lv_tyjl1));
            vtcp_log("[%s][%d] 写入附加域文件1=[%s]",__FL__,(char *)&lv_tyjl1);
            
            iLvGetAddit(&payin,"004",(char *)&lv_tyjl4,sizeof(lv_tyjl4));
            vtcp_log("[%s][%d] 写入附加域文件4=[%s]",__FL__,(char *)&lv_tyjl4);
            
            vtcp_log("%s,%d 附加域信息处理完成拉\n",__FILE__,__LINE__);
            memcpy(wd_cashier.pono   ,lv_tyjl1.cpno ,sizeof(wd_cashier.pono)-1);
            wd_cashier.signday = apatoi(lv_tyjl1.cpday,8);
            vtcp_log("%s,%d pono=[%s]\n",__FILE__,__LINE__,wd_cashier.pono);
            vtcp_log("%s,%d signday=[%d]\n",__FILE__,__LINE__,wd_cashier.signday);
            iRet = Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",\
                                  wd_cashier.pono,wd_cashier.signday);
            if (iRet)
            {
                vtcp_log("%s,%d,查找本票基本信息表错误[%d]\n",__FILE__,__LINE__,iRet);
                sprintf(acErrMsg,"插入回执登记簿失败");
                WRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d]准备FETCH 数据啦[%d]\n",__FILE__,__LINE__,iRet);
            iRet = Cashier_Fet_Upd(&wd_cashier,g_pub_tx.reply);
            if (iRet)
            {
                vtcp_log("%s,%d,FET本票基本信息表错误[%d]\n",__FILE__,__LINE__,iRet);
                sprintf(acErrMsg,"FET本票基本信息表错误");
                WRITEMSG
                return -1;
            }
            vtcp_log("[%s][%d]我们要更新银行本票信息喽[%s]\n",__FILE__,__LINE__,wd_cashier.brno);
            /* 最后兑付行号 */
            memcpy(wd_cashier.holdbrno  ,payin.F_cashno,sizeof(wd_cashier.holdbrno)-1);
            
            vtcp_log("[%s][%d]最后兑付行为:[%.12s]\n",__FILE__,__LINE__,payin.F_cashno);
            iRc=Hv_uniontab_Sel(g_pub_tx.reply,&Hv_uniontab,"or_br_no='%.12s' ",payin.F_cashno);
            if(iRc!=0)
            {
                vtcp_log("%s,%d 查询发起行参与者行号错误[%.12s]",__FILE__,__LINE__,payin.F_cashno);
                sprintf(acErrMsg,"[%s][%d]查询发起行参与者行号错误",__FILE__,__LINE__);
                WRITEMSG
                return -1;
            }
            if(memcmp(Hv_uniontab.qs_no,HV_QS_BR_NO,sizeof(Hv_uniontab.qs_no)-1)!=0)
            {
                wd_cashier.cashstat[0]='2'; /**2.系统外他行兑付本行签发**/
            }
            else
            {
                wd_cashier.cashstat[0]='1'; /**1.系统内他行兑付本行签发**/
            }
            /* 最后兑付行名 */
            memcpy(wd_cashier.holdbrname,lv_tyjl4.paybrname2,sizeof(wd_cashier.holdbrname)-1);
            /* 最后兑付帐号 */
            vtcp_log("[%s][%d]最后兑付帐户[%.32s]\n",__FILE__,__LINE__,payin.F_cashactno);
            memcpy(wd_cashier.holdactno,payin.F_cashactno,sizeof(wd_cashier.holdactno)-1);
            /* 最后兑付名字 */
            vtcp_log("[%s][%d]最后兑付户名[%.60s]\n",__FILE__,__LINE__,payin.F_cashname);
            memcpy(wd_cashier.holdname,payin.F_cashname,sizeof(wd_cashier.holdname)-1);
            wd_cashier.lactcls[0] = wd_cashier.stat[0];/* 上次帐户状态 */
            /**modify by NEWBP1**/
            if(wd_cashier.bptype[0]=='0')/**转账本票**/
            {
                wd_cashier.stat[0] = '2';  /* 本票当前状态标志 2-本票兑付 */
            }
            else if (wd_cashier.bptype[0]=='1')/**现金本票**/
            {
            	  wd_cashier.stat[0] = '3';  /* 本票当前状态标志 3-现金解付 */
            }
            /****end by NEWBP1****/
            wd_cashier.cashday = g_pub_tx.tx_date;                                      /* 兑付日期 */
            wd_cashier.jqday   = g_pub_tx.tx_date;                                      /* 结清日期 */
            memcpy(wd_cashier.cashtlrno,payin.T_inputno,sizeof(wd_cashier.cashtlrno)-1);/* 兑付操作员 */
            memcpy(wd_cashier.jqtlrno  ,payin.T_inputno,sizeof(wd_cashier.jqtlrno)-1);  /* 结清操作员 */
            sprintf(wd_cashier.filler,"跨行兑付成功[%.12s]",payin.F_cashno);
            vtcp_log("[%s][%d]准备UPDATE数据啦\n",__FILE__,__LINE__);
            iRet = Cashier_Upd_Upd(wd_cashier,g_pub_tx.reply);
            if (iRet)
            {
                vtcp_log("%s,%d,更新本票基本信息表错误[%d]\n",__FILE__,__LINE__,iRet);
                sprintf(acErrMsg,"更新本票基本信息表错误");
                WRITEMSG
                return -1;
            }
            Cashier_Clo_Upd();
        }
        /**end YHBP**/
				cLvStat[0]=payin.T_stat[0];
				vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
				vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

				set_zd_data("0500","0"); 
				if (iRc == 0 )        /* 走帐*/
				{      
					strcpy(g_pub_tx.reply, "0000" );  
				}
				/* begin NEWTCTD1 */
				else if(iRc == 999 || iRc == 100)    /* 不用走帐了 */
				/* end NEWTCTD1 */
				{      
					strcpy(g_pub_tx.reply, "0010"  );
					return 999;
				}
				else	if(iRc)    /* 挂帐检查出错 */
				{
					sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
					WRITEMSG
					lv_destroy(&subpack);
					return(-1);
				}   
				/* 	cLvStat[0]=payin.T_stat[0];
				set_zd_data("0980",cLvStat); */
				vLvBrnoDebug(__FILE__,__LINE__);
			}
			/*  已处理完毕 */
			vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
			lv_destroy(&subpack); 
			return iRc;  
		}
		iTmpIndex ++;   
	}
	if ( iTmpIndex > iSubpackIndex )
	{
		vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
		WRITEMSG
			lv_destroy(&subpack);
		return -1;
	} 
	return(0);
}  

/****定期借记报文来账处理****/
int lv_deal_dqjjlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	struct lv_lbctl_c wd_lv_lbctl;

	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   	

	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	/****检查是否实时借记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_DQJJ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是定期借记来账\n",__FILE__,__LINE__,	wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是定期借记报文");WRITEMSG
			return(-1);
	}
	/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
	*   lv_destroy
	***************************************************************/
	memset(&subpack,0,sizeof(TLVPACKAGE));
	lv_create(&subpack);
	iRc=lv_fetchpkg(wp_lvpack, &subpack);
	if (iRc<0)
	{
		vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"提取报文时出错");WRITEMSG
			lv_destroy(&subpack);
		return(-1);
	}else if(iRc>0)
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
		lv_destroy(&subpack);
		return 0;
	}
	vtcp_log("\n\n"); 

	/****得到明细业务的笔数****/
	recnt=lv_count_recnt(&subpack);
	vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
	vtcp_log("[%s][%d]  iSubpackIndex=[%ld]",__FILE__,__LINE__, iSubpackIndex);
	vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);		

	if ( iSubpackIndex >= iTmpIndex+recnt) /* 非批量，说明提取报文结束 */
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
		lv_destroy(&subpack);		  
		return 0;
	}	
	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  

	for (index=0; index<recnt; index++)
	{
		if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
		{
			vtcp_log("%s,%d,========找到要处理的INDEX了 iSubpackIndex==[%ld]",__FILE__,__LINE__,iSubpackIndex); 

			memset(&payin,'\0',sizeof(payin));

			vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  

			vtcp_log("%s,%d,payin.A_tbsdy=[%.8s]payin.T_inday=[%.8s]",__FILE__,__LINE__,payin.A_tbsdy,payin.T_inday);
			vtcp_log("%s,%d index=[%d]",__FILE__,__LINE__,index);

			iRc=lv_fetchpack_to_payin(wp_lvpack, &subpack, index, &payin);
			if (iRc<0)
			{
				vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
				sprintf( acErrMsg,"根据子报文填写payin失败");
				WRITEMSG
					lv_destroy(&subpack);
				return(-1);
			}
			vtcp_log("[%s][%d] payin->F_orderno====[%s]\n",__FILE__,__LINE__,payin.F_orderno);
			vtcp_log("[%s][%d] payin->F_acbrno====[%s]\n",__FILE__,__LINE__,payin.F_acbrno);
			vtcp_log("[%s][%d] payin->F_txamt====[%s]\n",__FILE__,__LINE__,payin.F_txamt);
			vtcp_log("[%s][%d] payin->F_payno====[%s]\n",__FILE__,__LINE__,payin.F_payno);
			vtcp_log("[%s][%d] payin->F_payactno====[%s]\n",__FILE__,__LINE__,payin.F_payactno);
			vtcp_log("[%s][%d] payin->F_payname====[%s]\n",__FILE__,__LINE__,payin.F_payname);
			vtcp_log("[%s][%d] payin->F_payaddress====[%s]\n",__FILE__,__LINE__,payin.F_payaddress);
			vtcp_log("[%s][%d] payin->F_content====[%s]\n",__FILE__,__LINE__,payin.F_content);			

			vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());        		
			vtcp_log("%s,%d,payin.A_tbsdy=[%.8s]",__FILE__,__LINE__,payin.A_tbsdy);   
			vtcp_log("%s,%d,payin.T_inday=[%.8s]",__FILE__,__LINE__,payin.T_inday);   

			iRc = iLvGetRecvBrno(&payin); 
			if (iRc)
			{
				vtcp_log("%s,%d,根据接受机构设置A_brno失败",__FILE__,__LINE__);
				sprintf( acErrMsg,"根据接受机构设置A_brno失败");
				WRITEMSG
					lv_destroy(&subpack);
				return(-1);
			}
			payin.A_stat[0]=STAT_LZRECV1;
			payin.F_lw_ind[0]= '2';

			char cPayqsno[13];
			char cCashqsno[13];	
			char cSendco[5];	
			char cRececo[5];	

			memset(cPayqsno, 0, sizeof(cPayqsno));
			memset(cCashqsno, 0, sizeof(cCashqsno));
			memset(cSendco, 0, sizeof(cSendco));
			memset(cRececo, 0, sizeof(cRececo)); 

			vtcp_log("%s,%d,cCashqsno=[%.12s]\n",__FILE__,__LINE__,payin.T_payqsactno);     
			vtcp_log("%s,%d,cPayqsno=[%.12s]\n",__FILE__,__LINE__,payin.T_cashqsactno);  

			memcpy( cPayqsno,payin.T_payqsactno, sizeof(cPayqsno)-1);
			memcpy( cCashqsno,payin.T_cashqsactno, sizeof(cPayqsno)-1);
			vtcp_log("%s,%d,cCashqsno=[%.12s]\n",__FILE__,__LINE__,cCashqsno);     
			vtcp_log("%s,%d,cPayqsno=[%.12s]\n",__FILE__,__LINE__,cPayqsno);	
			if ( iGetSendco(cSendco, cCashqsno) )
			{
				vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
				/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
				WRITEMSG
				return(-1);*/
			}
			vtcp_log("%s,%d,cSendco=[%s]\n",__FILE__,__LINE__,cSendco);              	
			if (iGetSendco(cRececo, cPayqsno))
			{
				vtcp_log("%s,%d,根据支付系统行号得到(发报/收报)中心代号失败",__FILE__,__LINE__);
				/*sprintf( acErrMsg,"根据支付系统行号得到(发报/收报)中心代号失败");
				WRITEMSG
				return(-1);*/
			}
			vtcp_log("%s,%d,cRececo=[%.4s]\n",__FILE__,__LINE__,cRececo);                   

			memcpy( payin.T_sendco, cSendco, sizeof(payin.T_sendco));
			memcpy( payin.T_receco, cRececo, sizeof(payin.T_receco));	      		

			vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  		
			vtcp_log("%s,%d,payin.A_tbsdy=[%.8s]",__FILE__,__LINE__,payin.A_tbsdy);   
			vtcp_log("%s,%d,payin.T_inday=[%.8s]",__FILE__,__LINE__,payin.T_inday);  

			vtcp_log("[%s][%d] payin->F_orderno====[%s]\n",__FILE__,__LINE__,payin.F_orderno);
			vtcp_log("[%s][%d] payin->F_acbrno====[%s]\n",__FILE__,__LINE__,payin.F_acbrno);
			vtcp_log("[%s][%d] payin->F_txamt====[%s]\n",__FILE__,__LINE__,payin.F_txamt);
			vtcp_log("[%s][%d] payin->F_payno====[%s]\n",__FILE__,__LINE__,payin.F_payno);
			vtcp_log("[%s][%d] payin->F_payactno====[%s]\n",__FILE__,__LINE__,payin.F_payactno);
			vtcp_log("[%s][%d] payin->F_payname====[%s]\n",__FILE__,__LINE__,payin.F_payname);
			vtcp_log("[%s][%d] payin->F_payaddress====[%s]\n",__FILE__,__LINE__,payin.F_payaddress);
			vtcp_log("[%s][%d] payin->F_content====[%s]\n",__FILE__,__LINE__,payin.F_content);			

			iLvNewRecFromPayin(&payin);

			vtcp_log("[%s][%d] payin->T_orderno====[%s]\n",__FILE__,__LINE__,payin.T_orderno);
			vtcp_log("[%s][%d] payin->T_acbrno====[%s]\n",__FILE__,__LINE__,payin.T_acbrno);
			vtcp_log("[%s][%d] payin->T_txamt====[%s]\n",__FILE__,__LINE__,payin.T_txamt);
			vtcp_log("[%s][%d] payin->T_payno====[%s]\n",__FILE__,__LINE__,payin.T_payno);
			vtcp_log("[%s][%d] payin->T_payactno====[%s]\n",__FILE__,__LINE__,payin.T_payactno);
			vtcp_log("[%s][%d] payin->T_payname====[%s]\n",__FILE__,__LINE__,payin.T_payname);
			vtcp_log("[%s][%d] payin->T_payaddress====[%s]\n",__FILE__,__LINE__,payin.T_payaddress);
			vtcp_log("[%s][%d] payin->T_content====[%s]\n",__FILE__,__LINE__,payin.T_content);

			vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  
			vtcp_log("%s,%d,payin.T_inday=[%.8s]",__FILE__,__LINE__,payin.T_inday);   

			if ( chkflg == 0) 	   /*  chkflg=0 登记  */
			{
				char cLvStat[1];
				vLvBrnoDebug(__FILE__,__LINE__);
				memset(cLvStat, 0 , sizeof(cLvStat));
				get_zd_data("0980",cLvStat);
				vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
				payin.T_stat[0]=cLvStat[0];
				payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
				get_fd_data(DC_TX_BR_NO,payin.T_brno);
				vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);		

				vtcp_log("[%s][%d] payin->T_orderno====[%s]\n",__FILE__,__LINE__,payin.T_orderno);
				vtcp_log("[%s][%d] payin->T_acbrno====[%s]\n",__FILE__,__LINE__,payin.T_acbrno);
				vtcp_log("[%s][%d] payin->T_txamt====[%s]\n",__FILE__,__LINE__,payin.T_txamt);
				vtcp_log("[%s][%d] payin->T_payno====[%s]\n",__FILE__,__LINE__,payin.T_payno);
				vtcp_log("[%s][%d] payin->T_payactno====[%s]\n",__FILE__,__LINE__,payin.T_payactno);
				vtcp_log("[%s][%d] payin->T_payname====[%s]\n",__FILE__,__LINE__,payin.T_payname);
				vtcp_log("[%s][%d] payin->T_payaddress====[%s]\n",__FILE__,__LINE__,payin.T_payaddress);
				vtcp_log("[%s][%d] payin->T_content====[%s]\n",__FILE__,__LINE__,payin.T_content);

				vtcp_log("%s,%d,登记iSubpackIndex==[%ld]子报文!! \n",__FILE__,__LINE__, iSubpackIndex);
				iRc = iLvUpdRecFromPayin(&payin,1);
				if (iRc)
				{
					sprintf( acErrMsg,"插入定期贷记来账登记簿错误");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				} 
				vtcp_log("%s,%d,插入定期贷记来账登记簿成功!! \n",__FILE__,__LINE__ );

				lv_destroy(&subpack); 

				/**** 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
				if ( iSubpackIndex == 0 )  	   		        
				{
					/******插入来包控制表************/
					iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
					if (iRc)
					{
						vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"产生来包控制表记录错误");
						WRITEMSG
							return -1;
					}
					vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay()); 
					iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
					if (iRc)
					{
						vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"插入来包控制表出错");
						WRITEMSG
							return -1;
					}    
					vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());                   			        	

				}    
				if (index == recnt-1 )  /*  登记完最后一条明细时   */
				{
					vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
					set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/	
				}             		
				/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
				iSubpackIndex ++;
				set_zd_long("0520", iSubpackIndex);
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				return 0;
			}
			else                 /* chkflg=1 挂帐检查   */
			{
				char cLvStat[1];
				memset(cLvStat , 0 , sizeof(cLvStat));
				vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);
				vLvBrnoDebug(__FILE__,__LINE__);

				set_zd_data("0500","0"); 
				iRc = pubf_lv_pkg_hang_chk(&payin,"006");
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
				cLvStat[0]=payin.T_stat[0];
				vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
				vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

				if (iRc == 0 || iRc == 100)        /* 走帐*/
				{      
					strcpy(g_pub_tx.reply, "0000" );  
				}else if(iRc == 999)    /* 不用走帐了 */
				{      
					strcpy(g_pub_tx.reply, "0010"  );
					lv_destroy(&subpack);  					  
					return 999;
				}else	if(iRc)    /* 挂帐检查出错 */
				{
					sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}  
				cLvStat[0]=payin.T_stat[0];
				set_zd_data("0980",cLvStat);
				vLvBrnoDebug(__FILE__,__LINE__);

			}
			/*  已处理完毕 */
			vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
			lv_destroy(&subpack); 
			return iRc;  
		}
		iTmpIndex ++;    			
	}/*end of for */
	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  

	if ( iTmpIndex > iSubpackIndex )
	{
		vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
		WRITEMSG
			lv_destroy(&subpack);
		return -1;
	}  
	lv_destroy(&subpack);
	return(0);
}

/****补发的往帐处理****/
int lv_deal_bfwz(TLVPACKAGE *wp_lvpack)
{
	int iRc=0;
	PAY_IN_AREA payin;
	struct lv_wbctl_c wd_lv_wbctl;
	char cDtlsucamt[19];
	char cDtlsuccnt[9];
	char cTmpamt[16];
	char cTmporderno[9];
	memset(cTmporderno,0 ,sizeof(cTmporderno));
	memset(&wd_lv_wbctl, 0 , sizeof(wd_lv_wbctl));
	memset(&payin, 0 , sizeof(payin));
	memset(cDtlsucamt, 0 , sizeof(cDtlsucamt));
	memset(cDtlsuccnt, 0 , sizeof(cDtlsuccnt));
	memset(cTmpamt, 0 , sizeof(cTmpamt));
	/* 准备PAYIN结构体  **/
	memcpy(payin.F_opcd,OPCD_LR,sizeof(payin.F_opcd));
	GETTAG_SIZE(wp_lvpack,"02C",payin.F_pkgno,sizeof(payin.F_pkgno));
	/*******************add by xyy 20060302******/
	GETTAG(wp_lvpack,"0BC",cTmporderno);
	lv_orderno_add0(cTmporderno, sizeof(payin.F_orderno),payin.F_orderno);
	/****************end ***********************/
	GETTAG_SIZE(wp_lvpack,"0BG",payin.F_txnum,sizeof(payin.F_txnum));
	GETTAG_SIZE(wp_lvpack,"52A",payin.F_orbrno,sizeof(payin.F_orbrno));
	GETTAG_SIZE(wp_lvpack,"58A",payin.F_acbrno,sizeof(payin.F_acbrno));
	GETTAG_SIZE(wp_lvpack,"33G",payin.F_txamt,sizeof(payin.F_txamt));
	GETTAG_SIZE(wp_lvpack,"CC4",payin.F_payno,sizeof(payin.F_payno));
	GETTAG_SIZE(wp_lvpack,"50C",payin.F_payactno,sizeof(payin.F_payactno));
	GETTAG_SIZE(wp_lvpack,"55A",payin.F_payname,sizeof(payin.F_payname));
	GETTAG_SIZE(wp_lvpack,"50B",payin.F_payaddress,sizeof(payin.F_payaddress));
	GETTAG_SIZE(wp_lvpack,"CC5",payin.F_cashno,sizeof(payin.F_cashno));
	GETTAG_SIZE(wp_lvpack,"59C",payin.F_cashactno,sizeof(payin.F_cashactno));
	GETTAG_SIZE(wp_lvpack,"59A",payin.F_cashname ,sizeof(payin.F_cashname));
	GETTAG_SIZE(wp_lvpack,"59B",payin.F_cashaddress,sizeof(payin.F_cashaddress));
	GETTAG_SIZE(wp_lvpack,"CEG",payin.F_ywtype,sizeof(payin.F_ywtype) );  
	GETTAG_SIZE(wp_lvpack,"72A",payin.F_content,sizeof(payin.F_content));  
	GETTAG_SIZE(wp_lvpack,"30A",payin.F_wtday,sizeof(payin.F_wtday));  
	GETTAG_SIZE(wp_lvpack,"30E",payin.F_packday,8);
	/**
	GETTAG_SIZE(wp_lvpack,"0BD",payin.F_packid,sizeof(payin.F_packid));
	**/
	GETTAG_SIZE(wp_lvpack,"0BD",cTmporderno,8);
	lv_orderno_add0(cTmporderno, sizeof(payin.F_packid), payin.F_packid);
	GETTAG_SIZE(wp_lvpack,"011",payin.T_payqsactno,sizeof(payin.T_payqsactno));
	GETTAG_SIZE(wp_lvpack,"012",payin.T_cashqsactno,sizeof(payin.T_cashqsactno));
	GETTAG_SIZE(wp_lvpack,"32B",cDtlsucamt,sizeof(cDtlsucamt)-1);
	memcpy(cTmpamt,cDtlsucamt+3,sizeof(cTmpamt)-1);
	GETTAG_SIZE(wp_lvpack,"B63",cDtlsuccnt,sizeof(cDtlsuccnt)-1);

	payin.F_feeflag[0]=FEEFLAG_ACT;
	payin.T_stat[0]=STAT_WZCHECK2;
	payin.F_packend[0]='1';
	/* 准备插入lv_wbctl 表 */
	GETTAG_SIZE(wp_lvpack,"C15",wd_lv_wbctl.packmy,sizeof(wd_lv_wbctl.packmy)-1);/*包密押*/

	wd_lv_wbctl.pack_date = apatoi (payin.F_packday,8);/* 打包日期*/
	memcpy(wd_lv_wbctl.packid ,payin.F_packid,sizeof(wd_lv_wbctl.packid  ));/*包id     */
	memcpy(wd_lv_wbctl.pkgno  ,payin.F_pkgno,sizeof(wd_lv_wbctl.pkgno    ));/*Pkg编号  */
	memcpy(wd_lv_wbctl.pay_qs_no ,payin.T_payqsactno,sizeof(wd_lv_wbctl.pay_qs_no));  /*发起清算行号*/
	memcpy(wd_lv_wbctl.cash_qs_no,payin.T_cashqsactno,sizeof(wd_lv_wbctl.cash_qs_no));/*接收清算行号*/
	wd_lv_wbctl.in_date = apatoi(pcLvGetDay(),8 ) ;
	GETTAG_SIZE(wp_lvpack,"BS6",lvca.lv_date,8 );/*回执期限（日期）*/	
	wd_lv_wbctl.hz_date = apatoi(lvca.lv_date, 8);
	wd_lv_wbctl.brprtflag[0]='1';/*支行打印标志    */	
	wd_lv_wbctl.qsprtflag[0]='1';/*清算中心打印标志*/
	GETTAG_SIZE(wp_lvpack,"72D",wd_lv_wbctl.content,sizeof(wd_lv_wbctl.content)-1);/*包附加数据 */
	GETTAG_SIZE(wp_lvpack,"BS1",wd_lv_wbctl.zcnode,sizeof(wd_lv_wbctl.zcnode)-1); /*轧差节点类型*/
	GETTAG_SIZE(wp_lvpack,"BS2",lvca.lv_date,8);  /*轧差日期   */
	wd_lv_wbctl.zc_date = apatoi(lvca.lv_date, 8);
	GETTAG_SIZE(wp_lvpack,"BS3",wd_lv_wbctl.zcid,sizeof(wd_lv_wbctl.zcid)-1);   /*轧差场次   */
	GETTAG_SIZE(wp_lvpack,"30I",lvca.lv_date,8);/*原来包日期*/
	wd_lv_wbctl.o_pack_date = apatoi(lvca.lv_date, 8);
	GETTAG_SIZE(wp_lvpack,"0BE",wd_lv_wbctl.o_packid,sizeof(wd_lv_wbctl.o_packid)-1); /*原来包id  */
	GETTAG_SIZE(wp_lvpack,"02D",wd_lv_wbctl.o_pkgno,sizeof(wd_lv_wbctl.o_pkgno)-1);  /*原包类型号*/
	GETTAG_SIZE(wp_lvpack,"CCO",wd_lv_wbctl.ornode,sizeof(wd_lv_wbctl.ornode)-1);  /*发起节点号*/
	GETTAG_SIZE(wp_lvpack,"C37",wd_lv_wbctl.acnode,sizeof(wd_lv_wbctl.acnode)-1);  /*接收节点号*/
	GETTAG_SIZE(wp_lvpack,"BS7",wd_lv_wbctl.ffflag,sizeof(wd_lv_wbctl.ffflag)-1);  /*辖内分发标志*/
	memcpy(wd_lv_wbctl.txnum ,payin.F_txnum,sizeof(wd_lv_wbctl.txnum)-1);/*业务类型号*/
	GETTAG_SIZE(wp_lvpack,"BS4",wd_lv_wbctl.bfflag,sizeof(wd_lv_wbctl.bfflag)-1);/*补发标志*/
	GETTAG_SIZE(wp_lvpack,"BS5",lvca.lv_date,8); /*清算日期*/
	wd_lv_wbctl.qs_date = apatoi(lvca.lv_date, 8);
	GETTAG_SIZE(wp_lvpack,"CIB",wd_lv_wbctl.packstat,sizeof(wd_lv_wbctl.packstat)-1);/*包处理状态      */
	wd_lv_wbctl.stat[0]=PSTAT_LRESP;/*包状态          */
	memcpy(wd_lv_wbctl.br_no,LV_CZ_QSBRNO,sizeof(wd_lv_wbctl.br_no)-1);/*机构号  全部是清算中心*/
	wd_lv_wbctl.dtlsuccnt=atoi(cDtlsuccnt); /*明细业务成功笔数*/
	wd_lv_wbctl.dtlsucamt=atof(cTmpamt); /*明细业务成功金额*/
	wd_lv_wbctl.dtlcnt=atoi(cDtlsuccnt); /*明细业务总笔数*/
	str2dbl(payin.F_txamt,sizeof(payin.F_txamt),0,&wd_lv_wbctl.totamt); /*明细业务总金额*/

	lvca.rtcode=Lv_wbctl_Ins( wd_lv_wbctl, g_pub_tx.reply);
	if (lvca.rtcode)
	{
		vtcp_log("[%s][%d] insert into wbctl error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf( acErrMsg,"处理原往包失败");

		return(-1);
	}
	if (!memcmp(payin.F_pkgno,PKGNO_SSJJHZ,sizeof(payin.F_pkgno)))
	{
		iRc=iLvPayinCheck8(&payin,&wd_lv_wbctl,NULL);
		if (iRc)
		{
			vtcp_log("%s,%d,生成包记录失败[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf( acErrMsg,"生成包记录失败");
			return(-1);
		}
		iLvNewRecFromPayin(&payin);
		iRc=iLvUpdRecFromPayin(&payin,1);
		if (iRc)
		{
			vtcp_log("%s,%d,插入实时借记回执往帐登记簿错误\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"插入回执登记簿失败");
			return(-1);
		}
	}
	return(0);
}

int iLvGetRece_AcValue(PAY_IN_AREA* payin)
{
	char cAc_no[33];
	char cName[61];
	char cReply[5];
	int iRet=0;
	char cTest[33];
	memset(cTest, 0 , sizeof(cTest));
	memset(cAc_no, 0 ,sizeof(cAc_no));
	memset(cName, 0 , sizeof(cName));

	set_zd_data("0660","1");
	set_zd_data("0720","2");

	if ( !memcmp(payin->F_pkgno,PKGNO_PTDJ,sizeof(payin->F_pkgno))
		||!memcmp(payin->F_pkgno,PKGNO_DQDJ,sizeof(payin->F_pkgno))
		||!memcmp(payin->F_pkgno,PKGNO_SSDJ,sizeof(payin->F_pkgno))
		||!memcmp(payin->F_pkgno,PKGNO_DJTH,sizeof(payin->F_pkgno))
		)
	{
		memcpy(cAc_no,payin->F_cashactno,sizeof(cAc_no)-1);
	}
	if ( !memcmp(payin->F_pkgno,PKGNO_PTJJ,sizeof(payin->F_pkgno))
		||!memcmp(payin->F_pkgno,PKGNO_DQJJ,sizeof(payin->F_pkgno))
		||!memcmp(payin->F_pkgno,PKGNO_SSJJ,sizeof(payin->F_pkgno))
		)
	{
		memcpy(cAc_no,payin->F_payactno,sizeof(cAc_no)-1);
	}
	pub_hv_old_acno(cAc_no);
	set_zd_data("0300",cAc_no);

	return 0;
}
/***********************************************************************
*设置来帐交易机构
* 
* 如果是涉及到走帐的业务,那么要查找帐务相关帐号是隶属于那个机构的
*     如果帐号存在但其指定的开户机构关闭,或者不存在那么
*         检查来帐机构是否存在,存在设置开户机构为交易机构,否则为清算中心 
*     否这按找到了正常开户机构重新设置交易机构为开户机构,
* 否这查找来帐机构是否存在,
*        不存在就取清算中心,
*        存在就设置开户机构为来帐机购
* 如果不是涉及到走帐的业务,那么
*     检查来帐机构是否存在,存在设置开户机构为交易机构,否则为清算中心 
* 1 支行挂账标志,0 正常,其他清算挂账,9999表示是村镇银行账务，仅仅插入记录
***********************************************************************/
int iSetLvTxBrno(PAY_IN_AREA* payin)
{
	char cKinbr[BRNO_LEN+1];
	char cOpnbr[BRNO_LEN+1];
	int iRet_kinbr=0,iRet_opnbr=0;
	memset(cKinbr,0,sizeof(cKinbr));
	memset(cOpnbr,0,sizeof(cOpnbr));
	/* 先判断来帐机构是否存在 */
	vtcp_log("%s,%d 包号===[%s]",__FILE__,__LINE__,payin->F_pkgno);
	iRet_kinbr=iLvGetReceKinbr(payin->F_acbrno,cKinbr);
	/* begin add by LiuHuafeng for 村镇银行 */
	if(iRet_kinbr == 9999)
	{
		/* 是否要设置opnbr机构是清算中心？等待测试 */
		vtcp_log("%s,%d 机构是村镇银行，只登记不处理\n",__FILE__,__LINE__);
		strncpy(payin->T_brno,cKinbr,BRNO_LEN);
		
		set_zd_data(DC_TX_BR_NO,cKinbr);
		set_zd_data("0020",cKinbr);
		
		return 9999;
	}
	/* end by LiuHuafeng 2009-5-13 16:44:56 for 村镇银行 */
	vtcp_log("[%s][%d] 来帐接收机构检查为=[%d]",__FILE__,__LINE__,iRet_kinbr);
	/* 涉及到来帐走帐交易需要判断接收帐号是否属于本行帐号,和他的开户行 */
	if (!memcmp(payin->F_pkgno,"001",3)||!memcmp(payin->F_pkgno,"003",3)
		||!memcmp(payin->F_pkgno,"005",3)||!memcmp(payin->F_pkgno,"007",3)
		||!memcmp(payin->F_pkgno,"008",3)||!memcmp(payin->F_pkgno,"011",3)
		||!memcmp(payin->F_pkgno,"010",3)
		)
	{
		/* 再判断来帐帐号所属机构是否存在 */
		iRet_opnbr=iLvGetReceOpnbr(cKinbr,payin->F_cashactno,cOpnbr);
		vtcp_log("[%s][%d] 来帐开户机构检查为=[%d]",__FILE__,__LINE__,iRet_opnbr);
		if (iRet_opnbr==0)
		{
			/* 找到来帐帐号所属机构，那么就按照来帐帐号机构设计交易机构,即接收机构 */
			memcpy(cKinbr,cOpnbr,BRNO_LEN);
			memcpy(payin->T_brno,cKinbr,BRNO_LEN);
			set_zd_data(DC_TX_BR_NO,cKinbr);
			set_zd_data("0020",cKinbr);
			vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=[%d]\n",__FILE__,__LINE__,cOpnbr,iRet_kinbr);
			return 0;
		}
		else
		{
			/* 帐号所属机构不存在,判断接收机构是否存在 */
			if (iRet_kinbr==0)
			{
				memcpy(cOpnbr,cKinbr,BRNO_LEN);
				memcpy(payin->T_brno,cKinbr,BRNO_LEN);
				set_zd_data(DC_TX_BR_NO,cKinbr);
				set_zd_data("0020",cKinbr);
				if (!memcmp(payin->F_pkgno,"001",3)||!memcmp(payin->F_pkgno,"003",3)
					||!memcmp(payin->F_pkgno,"005",3)||!memcmp(payin->F_pkgno,"007",3)
					||!memcmp(payin->F_pkgno,"008",3)||!memcmp(payin->F_pkgno,"011",3)
					||!memcmp(payin->F_pkgno,"010",3)
					)
				{
					vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=[%d]\n",__FILE__,__LINE__,cOpnbr,iRet_opnbr);
					return 1;
				}
				else
				{
					vtcp_log("%s,%d 得到新的交易机构=[%s]非帐务处理来帐,不需要处理挂账=[%d]\n",__FILE__,__LINE__,cOpnbr,iRet_opnbr);
					return 0;
				}
			}
			else
			{
				vtcp_log("%s,%d 得到交易机构错误，设置新的机构是[%s]",__FILE__,__LINE__,cKinbr);
				set_zd_data(DC_TX_BR_NO,cKinbr);
				set_zd_data("0020",cKinbr);
				memcpy(payin->T_brno,cKinbr,BRNO_LEN);
				vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=[%d]\n",__FILE__,__LINE__,cKinbr,iRet_kinbr);
				return iRet_kinbr;
			}
		}
	}
	else
	{
		if (iRet_kinbr!=0)
		{
			set_zd_data(DC_TX_BR_NO,cKinbr);
			set_zd_data("0020",cKinbr);
			memcpy(payin->T_brno,cKinbr,BRNO_LEN);
			vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=[%d]\n",__FILE__,__LINE__,cKinbr,iRet_kinbr);
			vtcp_log("%s %d 接收机构没有找到,需要清算挂账[%d]",__FILE__,__LINE__,iRet_kinbr);
			return iRet_kinbr;
		}
		vtcp_log("%s,%d 得到交易机构错误，设置新的机构是[%s]",__FILE__,__LINE__,cKinbr);
		set_zd_data(DC_TX_BR_NO,cKinbr);
		set_zd_data("0020",cKinbr);
		memcpy(payin->T_brno,cKinbr,BRNO_LEN);
		vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=[%d]\n",__FILE__,__LINE__,cKinbr,iRet_kinbr);
		return 0;
	}
	return 0;
}
/********************************
* 得到接收机构的行号
* ret =0 表明找到了
* ret = 9999 表明是村镇银行的
* 其他 ret!=0 表明没有找到
***********************************/
int iLvGetReceKinbr(char * acbrno,char * brno)
{
	int iRet=0;
	char cAc_br_no[13];
	char cReply   [5];
	struct hv_orno_brno_c hv_orno_brno;
	char cQs_br_no[BRNO_LEN+1];
	struct  com_branch_c    sCom_branch;

	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(&hv_orno_brno,0,sizeof(hv_orno_brno));
	memset(cQs_br_no, 0 , sizeof(cQs_br_no));
	memset(cAc_br_no, 0 , sizeof(cAc_br_no));

	iRet=iFind_qsbrno(cQs_br_no);
	if (iRet)
	{
		vtcp_log("%s,%d 极端情况清算中心也不存在[%d]",__FILE__,__LINE__,iRet);
		memcpy(cQs_br_no,QS_BR_NO,BRNO_LEN);
	}
	memcpy(cAc_br_no,acbrno,sizeof(cAc_br_no)-1);
	iRet=Hv_orno_brno_Sel(cReply,&hv_orno_brno,"or_br_no='%s'",cAc_br_no);
	if (iRet)
	{
		vtcp_log("%s,%d 来帐机构%s不是本行机构，设置交易机构为清算中心",__FILE__,__LINE__,cAc_br_no);
		iRet=iFind_qsbrno(brno);
		if (iRet)
		{
			vtcp_log("%s,%d 极端情况清算中心也不存在[%d]",__FILE__,__LINE__,cAc_br_no,iRet);
		}
		return iRet;
	}
	/* begin add by LiuHuafeng 2009-5-13 16:40:05 for 村镇银行 */
	if(strncmp(hv_orno_brno.br_no,HV_CUNZHEN_BR_NO,4)==0)
	{
		vtcp_log("%s,%d 支付系统号=%s是村镇银行",__FILE__,__LINE__,hv_orno_brno.or_br_no);
		memcpy(brno,hv_orno_brno.br_no,BRNO_LEN);
		return 9999;
	}
	/* end by LiuHuafeng 2009-5-13 16:40:18 */
	/* 检查来帐机构是否存在,状态是否正常 */
	memcpy(sCom_branch.br_no,hv_orno_brno.br_no,BRNO_LEN);
	iRet=Com_branch_Sel(cReply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
	if (iRet)
	{
		vtcp_log("%s,%d 接收机构不存在,设置接收机构为清算中心iRet=[%d]",__FILE__,__LINE__,iRet);
		WRITEMSG
			memcpy(brno,cQs_br_no,BRNO_LEN);
		set_zd_data("0680","6");/* 帐户类型 */
		return iRet;
	}
	if (sCom_branch.wrk_sts[0]=='*')
	{
		vtcp_log("%s,%d 接收机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
		WRITEMSG
			memcpy(brno,cQs_br_no,BRNO_LEN);
		set_zd_data("0680","6");/* 帐户类型 */
		return -1;
	}
	memcpy(brno,hv_orno_brno.br_no,BRNO_LEN);
	vtcp_log("%s,%d 得到最终来帐机构是[%s]",__FL__,brno);
	return 0;
}
/* 检查接收帐号是否存在，开户机构是否正常 */
/* 返回0正常，其他为错误，设置默认开户机构为tx_br_no*/
int iLvGetReceOpnbr(const char * tx_br_no,const char * cash_ac_no,char * opn_br_no)
{
	char cAc_no[33];
	char cReply[5];
	struct hv_orno_brno_c hv_orno_brno;
	struct  prdt_ac_id_c    sPrdt_ac_id;
	struct  dd_mst_c        sDd_mst;
	struct  mdm_ac_rel_c    sMdm_ac_rel;
	struct  com_item_c      sCom_item;
	struct  com_branch_c    sCom_branch;
	int iRet=0;
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(&sCom_item , 0 , sizeof(sCom_item));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&sDd_mst, 0 , sizeof(struct dd_mst_c));
	memset(cReply, 0 , sizeof(cReply));
	memset(&hv_orno_brno,0,sizeof(hv_orno_brno));
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(&sMdm_ac_rel, 0 , sizeof(sMdm_ac_rel));
	memcpy(cAc_no,cash_ac_no,sizeof(cAc_no)-1);
	zip_space(cAc_no);

	pub_base_old_acno(cAc_no);

	if (strlen(cAc_no)==7 || strlen(cAc_no)==0)
	{
		iRet=Com_item_Sel(cReply,&sCom_item,"acc_no='%s'",cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d得到科目错误",__FILE__,__LINE__);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return iRet;
		}
		/* 科目直接用交易机构做为开户机构 */
		memcpy(opn_br_no,tx_br_no,BRNO_LEN);
	}
	else
	{
		iRet=Mdm_ac_rel_Sel(cReply,&sMdm_ac_rel,"ac_no='%s' ",cAc_no);
		if ( iRet==100 )
		{
			vtcp_log("%s,%d 帐号不存在!ac_no=[%s]",__FILE__,__LINE__,cAc_no);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			WRITEMSG
				return iRet;
		}
		if ( iRet )
		{
			vtcp_log("%s,%d 取介质帐号对照表错误!ac_no=[%s],ret=[%d]",__FILE__,__LINE__,cAc_no,iRet);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			WRITEMSG
				return iRet;
		}
		if (sMdm_ac_rel.ac_seqn==9999)
		{
			iRet=Prdt_ac_id_Sel(cReply,&sPrdt_ac_id,"ac_id='%ld' and ac_seqn='1' ",	sMdm_ac_rel.ac_id);
		}else
		{
			iRet=Prdt_ac_id_Sel(cReply,&sPrdt_ac_id,"ac_id='%ld' and ac_seqn=%d ",	sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
		}
		if ( iRet==100 )
		{
			vtcp_log("%s,%d 帐号不存在!ac_id=[%d],ac_seqn=[%d]",__FILE__,__LINE__,sMdm_ac_rel.ac_id,1);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return iRet;
		}
		if ( iRet )
		{
			vtcp_log("%s,%d 取产品帐号对照表错误!ac_id=[%d],ac_seqn=[%d],ret=[%d]",__FILE__,__LINE__,sMdm_ac_rel.ac_id,1,iRet);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return iRet;
		}
		/*** 对公活期 或 内部帐 ***/
		if ( sPrdt_ac_id.ac_id_type[0]=='1'||sPrdt_ac_id.ac_id_type[0]=='9')
		{
			if (sPrdt_ac_id.ac_id_type[0]=='9')
			{
				set_zd_data("0680","6");/* 帐户类型 */
			}else
			{
				set_zd_data("0680","1");/* 帐户类型 */
			}
		}
		else
		{
			vtcp_log("%s,%d 帐号类型错误![%s]",__FILE__,__LINE__,g_pub_tx.ac_id_type);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			set_zd_data("0680","6");/* 帐户类型 */
			return iRet;
		}
		memcpy(sCom_branch.br_no,sMdm_ac_rel.opn_br_no,BRNO_LEN);
		iRet=Com_branch_Sel(cReply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
		if (iRet)
		{
			vtcp_log("%s,%d 帐号对应的开户机构不存在,iRet=[%d]",__FL__,iRet);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			set_zd_data("0680","6");/* 帐户类型 */
			return iRet;
		}
		if (sCom_branch.wrk_sts[0]=='*')
		{
			vtcp_log("%s,%d 帐号对应开户机构[%s]状态被关闭",__FL__,sCom_branch.br_no);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			set_zd_data("0680","6");/* 帐户类型 */
			return -1;
		}
		vtcp_log("%s,%d 帐号对应开户机构[%s]状态正常",__FL__,sCom_branch.br_no);
		memcpy(opn_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1);
		/* 检查帐号开户机构是否正常 */
	}
	vtcp_log("%s,%d 得到的开户机构号码是[%s] ",__FILE__,__LINE__,opn_br_no);
	return 0;
}
void vLvBrnoDebug(char * file,int line)
{
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr, 0  , sizeof(cKinbr));
	get_zd_data(DC_TX_BR_NO,cKinbr);
	vtcp_log("%s,%d ^^^^^^^^^^^^^^   kinbr  ===[%s] ^^^^^^^^^^^^^^^^^^^^",file,line,cKinbr);
}
/****Modified  by SSH,2005.12.6,无语****/
/****普通借记报文回执来账处理****/
int lv_deal_ptjjhzlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	char cWssrno[7];	
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin,opay;
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   
	/* begin NEWTCTD1 */
	/* begin add by LiuHuafeng 2007-8-23 18:10:26 */
	char cRcpstat[4];
	memset(cRcpstat,0 ,sizeof(cRcpstat));
	get_zd_data("0890",cRcpstat);
	/* end by LiuHuafeng 2007-8-23 18:10:28 */
	/* end NEWTCTD1 */

	get_zd_data("0040",cWssrno );   /**流水号**/    
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
	/****检查是否普通借记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_PTJJHZ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是普通借记回执来账\n",__FILE__,__LINE__,	wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是普通借记回执报文");WRITEMSG
			return(-1);
	}
	while (1)
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy
		***************************************************************/
		memset(&subpack,0,sizeof(TLVPACKAGE));		
		lv_create(&subpack);
		iRc=lv_fetchpkg(wp_lvpack,&subpack);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");WRITEMSG
				lv_destroy(&subpack);
			return(-1);
		}else if(iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/****得到明细业务的笔数****/
		recnt=lv_count_recnt(&subpack);
		vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
		/* 说明iSubpackIndex不在这笔子报文中
		if ( iSubpackIndex >= iTmpIndex+recnt) 
		{
		iTmpIndex = iTmpIndex + recnt;
		if ( iTmpIndex > iSubpackIndex )
		{
		vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
		WRITEMSG
		lv_destroy(&subpack);
		return -1;
		}
		continue;    			
		}		 */

		for (index=0;index<recnt;index++)
		{
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{
				vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 

				/* ===========1. 插入普通借记来账登记簿 ======= */
				memset(&payin,'\0',sizeof(payin));
				iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}		
				payin.A_stat[0]=STAT_LZRECV1;				
				iLvNewRecFromPayin(&payin);

				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					/* begin NEWTCTD1 */
					/* begin add by LiuHuafeng 2007-8-27 18:34:50 如果是应答成功，才需要重新取机构号 */
					if (memcmp(payin.F_rcpstat,"00",2)==0)
					/* end by LiuHuafeng 2007-8-27 18:34:59 */
					/* end NEWTCTD1 */
						get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);		

					vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);
					/**add by wangyongwei NEWTCTD 20070905*/
					if(memcmp(payin.T_rcpstat,"00",2)!=0 && payin.T_stat[0]=='I'){
						payin.T_stat[0]=='H';
					}
					/**add by wangyongwei NEWTCTD 20070905*/

					iRc = iLvUpdRecFromPayin(&payin,1);
					if (iRc)
					{
						sprintf( acErrMsg,"插入普通借记来账登记簿错误");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}           
					vtcp_log("%s,%d,========第一步  插入普通借记来账登记簿   成功 !!!==========",__FILE__,__LINE__); 

					/**** ==========2.  处理原借记业务状态 ============= ****/
					memset(&opay,'\0',sizeof(opay));
					/*   因为在 lv_fetchpack_to_payin()中重置一些字段，这里改一下
					memcpy(opay.F_wtday,payin.F_wtday, sizeof(opay.F_wtday));
					memcpy(opay.F_orbrno,payin.F_orbrno, sizeof(opay.F_orbrno));
					memcpy(opay.F_orderno,payin.F_orderno, sizeof(opay.F_orderno));	
					********************************************************/
					/**********根据原委托日期、原发起行行号、原支付交易序号 找原PTJJ往帐记录 *********/
					memcpy(opay.F_wtday,payin.F_owtday, sizeof(opay.F_wtday));
					memcpy(opay.F_orbrno,payin.F_o_ac_br_no, sizeof(opay.F_orbrno));/**modify by xyy YHBP 2008-3-16 21:09**/
					memcpy(opay.F_orderno,payin.F_oorderno, sizeof(opay.F_orderno));		
					memcpy(opay.F_opcd,OPCD_SEND,	sizeof(opay.F_opcd));
					memcpy(opay.F_pkgno,PKGNO_PTJJ,	sizeof(opay.F_pkgno));
					opay.F_lw_ind[0] = '1';       /*  找原PTJJ往帐记录   */

					iRc=iLvFetchRecToPayin(&opay);
					if (iRc)
					{
						vtcp_log("%s,%d,未找到原交易,但此笔回执照单收着\n",__FILE__,__LINE__);
						vtcp_log("%s,%d,未找到原交易,但此笔回执照单收着\n",__FILE__,__LINE__);
						vtcp_log("%s,%d,未找到原交易,但此笔回执照单收着\n",__FILE__,__LINE__);												
						lvca.rtcode=0;
					}else
					{
						/***更新状态****/
						if (memcmp(payin.T_rcpstat,"00",2)==0)
						{
							opay.T_stat[0]=STAT_WZSUCCESS;/****扎差****/
						}
						else
						{
							opay.T_stat[0]=STAT_WZREJECT;/****拒绝****/
						}
						memcpy(opay.T_rcpstat,payin.T_rcpstat,sizeof(opay.T_rcpstat));/**add by xyy NEWYX 2007-3-26 13:50**/
						vtcp_log("[%s][%d]同志们已经来的了这里",__FILE__,__LINE__);
						iRc=iLvUpdRecFromPayin(&opay,0);/**更新**/
						if (iRc)
						{
							vtcp_log("%s,%d,更新原交易错误,但此笔回执照单收着\n",__FILE__,__LINE__);
							lvca.rtcode=0;
						}
						vtcp_log("%s,%d,更新原交易成功!\n",__FILE__,__LINE__);
					}
					lv_destroy(&subpack);     					
					/********************更新原交易结束********************/    
					vtcp_log("%s,%d,========第二步  处理原借记业务状态   成功 !!!==========",__FILE__,__LINE__); 

					/****  3. 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								return -1;
						}
						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
								return -1;
						}    		        	
					}  
					vtcp_log("%s,%d,========第三步  插入来包控制表   成功 !!!==========",__FILE__,__LINE__); 

					/****  4. 登记完最后一条明细时，处理普通借记来帐回执  *****/	
					/** 这里还有些问题，如果包中有多个子包很更新多次  先暂时这样写了**/		  
					if (index == recnt-1 ) 
					{
						vtcp_log("[%s][%d]wd_lv_lbctl.o_pack_date=[%d] ",__FILE__,__LINE__,wd_lv_lbctl.o_pack_date);						
						vtcp_log("[%s][%d]wd_lv_lbctl.o_packid=[%s]",__FILE__,__LINE__,wd_lv_lbctl.o_packid);						
						vtcp_log("[%s][%d]wd_lv_lbctl.cash_qs_no=[%s]",__FILE__,__LINE__,wd_lv_lbctl.cash_qs_no);						

						if ( iSubpackIndex != 0 )  /*  iSubpackIndex == 0  时，已设置 */
						{
							iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
							if (iRc)
							{
								vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
								sprintf( acErrMsg,"产生来包控制表记录错误");
								WRITEMSG
									return -1;
							}	
						}
						/* add NEWYX begin by LiuHuafeng 2007-4-3 15:15
						判断是否是票交回执业务，如果是则不在检查相关往包表
						*************************************************************************/
						if(!memcmp(wd_lv_lbctl.o_packid,"99999999",sizeof(wd_lv_lbctl.o_packid)-1))
						{
						}
						else
						/* end NEWYX by LiuHuafeng 2007-4-3 15:56 */
						{
							iRc = Lv_wbctl_Dec_Upd(g_pub_tx.reply," pack_date=%d and packid = '%s' and pay_qs_no='%s' ",
								wd_lv_lbctl.o_pack_date,wd_lv_lbctl.o_packid,wd_lv_lbctl.cash_qs_no);
							if (iRc)
							{
								vtcp_log("[%s][%d]-----Lv_wbctl_Dec_Upd() 出错 !!!! ",__FILE__,__LINE__);
								sprintf( acErrMsg,"========Lv_wbctl_Dec_Upd() 出错 !!!!=======");
								WRITEMSG
								return -1;
							}						 
							/*  memcpy(wd_lv_wbctl.pay_qs_no,wd_lv_lbctl.cash_qs_no,sizeof(wd_lv_wbctl.pay_qs_no)-1);
							wd_lv_wbctl.pack_date = wd_lv_lbctl.o_pack_date ;
							memcpy(wd_lv_wbctl.packid,wd_lv_lbctl.o_packid,sizeof(wd_lv_wbctl.packid)-1);  
							****remed by chenming 2006.10.11.                                          */
							iRc = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
							if (iRc) /*  没有找到原往包记录也应该报错 ! */
							{
								vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,lvca.rtcode);
								sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
								WRITEMSG
								Lv_wbctl_Debug(&wd_lv_wbctl);
								return -1;
							}
							else
							{
								wd_lv_wbctl.zc_date = wd_lv_lbctl.zc_date;
								wd_lv_wbctl.stat[0]=PSTAT_WRESP;
								iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
								if (iRc)
								{
									vtcp_log("%s,%d,更新原往包控制表失败\n",__FILE__,__LINE__);
									/****更新原包失败依然认为是成功的****/
									sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
									WRITEMSG			     
									return -1;
								}
								Lv_wbctl_Clo_Sel();  
							}	
						}
					}
					vtcp_log("%s,%d,========第四步  处理普通借记来帐回执  更新原往包控制表 成功 !!!==========",__FILE__,__LINE__); 

					/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					iSubpackIndex ++;
					set_zd_long("0520", iSubpackIndex);
					/*  已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					return 0;
				}
				else	 /* chkflg=1 挂帐检查   */
				{
					char cLvStat[1];
					memset(cLvStat , 0 , sizeof(cLvStat));
					vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);

					set_zd_data("0500","0"); 
					iRc = pubf_lv_pkg_hang_chk(&payin,"008");
					vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
					cLvStat[0]=payin.T_stat[0];
					vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

					/* 业务回执状态不是'00'的，不走帐 */
					if (memcmp(payin.T_rcpstat,"00",sizeof(payin.T_rcpstat)) != 0)
					{
						set_zd_double("0390",0.00);
						set_zd_double("0400",0.00);
					}
					if (iRc == 0)        /* 检查正确 走帐*/
					{      
						strcpy(g_pub_tx.reply, "0000" );  
					}else if(iRc == 100)  /* 挂帐 */
					{      
						strcpy(g_pub_tx.reply, "0000"  ); 
						/* rem by LiuHuafeng 2009-10-12 11:45:00
						lv_destroy(&subpack);						
						return 999;						
						**/
					}else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}  
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐挂帐检查结束###############",__FL__);					
				}
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				lv_destroy(&subpack);  
				return iRc;   
			}
			iTmpIndex ++;
		}/*end of for */
		if ( iTmpIndex > iSubpackIndex )
		{
			vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
			sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
			WRITEMSG
				lv_destroy(&subpack);
			return -1;
		}  
	}
	/****子报文循环完毕****/	

	return(0);
}
/****定期借记报文回执来账处理****/
int lv_deal_dqjjhzlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin,opay;
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;
	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   	

	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
	/****检查是否定期借记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_DQJJHZ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是定期借记回执来账\n",__FILE__,__LINE__,wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是定期借记回执报文");
		return(-1);
	}
	while (1)
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy
		***************************************************************/
		memset(&subpack,0,sizeof(TLVPACKAGE));		
		lv_create(&subpack);
		iRc=lv_fetchpkg(wp_lvpack,&subpack);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);			
			sprintf( acErrMsg,"提取报文时出错");
			lv_destroy(&subpack);
			return(-1);
		}else if(iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/****得到明细业务的笔数****/
		recnt=lv_count_recnt(&subpack);
		vtcp_log("%s,%d,定期业务笔数:%d\n",__FILE__,__LINE__,recnt);
		/* 说明iSubpackIndex不在这笔子报文中
		if ( iSubpackIndex >= iTmpIndex+recnt) 
		{
		iTmpIndex = iTmpIndex + recnt;
		if ( iTmpIndex > iSubpackIndex )
		{
		vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
		WRITEMSG
		lv_destroy(&subpack);
		return -1;
		}
		continue;    			
		}		 */

		for (index=0;index<recnt;index++)
		{
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{
				vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 

				/* ===========1. 插入普通借记来账登记簿 ======= */
				memset(&payin,'\0',sizeof(payin));
				iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}		
				if (memcmp(payin.F_rcpstat,"00",2)==0)
				{
					payin.A_stat[0]=STAT_LZRECV2;
				}else
				{
					payin.A_stat[0]=STAT_LZRECV1;
				}
				iLvNewRecFromPayin(&payin);

				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);		

					/* 插入定期借记回执登记簿 */										
					vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);
					iRc = iLvUpdRecFromPayin(&payin,1);
					if (iRc)
					{
						sprintf( acErrMsg,"插入定期借记回执登记簿错误");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}               		
					lv_destroy(&subpack); 
					vtcp_log("%s,%d,========第一步  插入普通借记来账登记簿   成功 !!!==========",__FILE__,__LINE__); 

					/**** ==========2.  处理原借记业务状态 ============= ****/
					memset(&opay,'\0',sizeof(opay));
					/*   因为在 lv_fetchpack_to_payin()中重置一些字段，这里改一下					
					memcpy(opay.F_wtday,payin.F_wtday,sizeof(opay.F_wtday));
					memcpy(opay.F_orbrno,payin.F_orbrno,sizeof(opay.F_orbrno));
					memcpy(opay.F_orderno,payin.F_orderno,sizeof(opay.F_orderno));
					********************************************************/
					/**********根据原委托日期、原发起行行号、原支付交易序号 找原DQJJ往帐记录 *********/
					memcpy(opay.F_wtday,payin.F_owtday, sizeof(opay.F_wtday));
					memcpy(opay.F_orbrno,payin.F_o_or_br_no, sizeof(opay.F_orbrno));
					memcpy(opay.F_orderno,payin.F_oorderno, sizeof(opay.F_orderno));

					memcpy(opay.F_opcd,OPCD_SEND,sizeof(opay.F_opcd));
					memcpy(opay.F_pkgno,PKGNO_DQJJ,sizeof(opay.F_pkgno));
					opay.F_lw_ind[0] = '1';       /*  找原PTJJ往帐记录   */

					iRc=iLvFetchRecToPayin(&opay);
					if (iRc)
					{
						vtcp_log("%s,%d,未找到原交易,但此笔回执照单收着\n",__FILE__,__LINE__);
						lvca.rtcode=0;
					}
					else
					{
						/***更新状态****/
						if (memcmp(payin.T_rcpstat,"00",2)==0)
						{
							opay.T_stat[0]=STAT_WZSUCCESS;/****扎差****/
						}
						else
						{
							opay.T_stat[0]=STAT_WZREJECT;/****拒绝****/
						}
						memcpy(opay.T_rcpstat,payin.T_rcpstat,sizeof(opay.T_rcpstat));/**add by xyy NEWYX 2007-3-26 13:50**/
						vtcp_log("[%s][%d]同志们已经来的了这里",__FILE__,__LINE__);
						iRc=iLvUpdRecFromPayin(&opay,0);/**更新**/
						if (iRc)
						{
							vtcp_log("%s,%d,更新原交易错误,但此笔回执照单收着\n",__FILE__,__LINE__);
							lvca.rtcode=0;
						}
						vtcp_log("%s,%d,更新原交易成功!\n",__FILE__,__LINE__);
					}
					/********************更新原交易结束********************/
					vtcp_log("%s,%d,========第二步  处理原借记业务状态   成功 !!!==========",__FILE__,__LINE__); 

					/****  3. 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								return -1;
						}
						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
								return -1;
						}    		        	
					}    
					vtcp_log("%s,%d,========第三步  插入来包控制表   成功 !!!==========",__FILE__,__LINE__); 

					/****  4. 登记完最后一条明细时，处理普通借记来帐回执  *****/		
					/** 这里还有些问题，如果包中有多个子包很更新多次  先暂时这样写了**/		  				
					if (index == recnt-1 )  
					{
						if ( iSubpackIndex != 0 )  /*  iSubpackIndex == 0  时，已设置 */
						{
							iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
							if (iRc)
							{
								vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
								sprintf( acErrMsg,"产生来包控制表记录错误");
								WRITEMSG
									return -1;
							}	
						}
						memcpy(wd_lv_wbctl.pay_qs_no,wd_lv_lbctl.cash_qs_no, sizeof(wd_lv_wbctl.pay_qs_no)-1);
						wd_lv_wbctl.pack_date = wd_lv_lbctl.o_pack_date ;
						memcpy(wd_lv_wbctl.packid,wd_lv_lbctl.o_packid,sizeof(wd_lv_wbctl.packid)-1);

						vtcp_log("[%s][%d]wd_lv_lbctl.pack_date=[%d] ",__FILE__,__LINE__,wd_lv_lbctl.o_pack_date);						
						vtcp_log("[%s][%d]wd_lv_lbctl.o_packid=[%s]",__FILE__,__LINE__,wd_lv_lbctl.o_packid);						
						vtcp_log("[%s][%d]wd_lv_lbctl.cash_qs_no=[%s]",__FILE__,__LINE__,wd_lv_lbctl.cash_qs_no);						

						iRc = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %d and packid = '%s' and pay_qs_no = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid, wd_lv_wbctl.pay_qs_no);
						if (iRc)
						{
							sprintf( acErrMsg,"定义游标出错 - %d", lvca.rtcode);
							WRITEMSG
								return(-1);
						}
						iRc = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,lvca.rtcode);
							Lv_wbctl_Debug(&wd_lv_wbctl);
							sprintf( acErrMsg,"未查到对应的往包记录!");
							WRITEMSG
								return(-1);
						}
						vtcp_log("[%s][%d]================ ",__FILE__,__LINE__);
						/* beg del by chenggx 2011-6-11 Fet 了两次 怎么会不报错呢?
						iRc = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,更新原往包控制表失败\n",__FILE__,__LINE__);
							****更新原包失败依然认为是成功的****
							lvca.rtcode=0;
						}
						else
						end del by chenggx 2011-6-11 Fet 了两次 怎么会不报错呢?*/
						{
							wd_lv_wbctl.zc_date = wd_lv_lbctl.zc_date ;
							wd_lv_wbctl.stat[0]=PSTAT_WRESP;
							if ((lvca.rtcode = Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
							{
								vtcp_log("%s,%d,更新原往包控制表失败\n",__FILE__,__LINE__);
								/****更新原包失败依然认为是成功的****/
								lvca.rtcode=0;
							}
							Lv_wbctl_Clo_Upd();
						}
					}
					vtcp_log("%s,%d,========第四步  处理普通借记来帐回执  更新原往包控制表 成功 !!!==========",__FILE__,__LINE__); 

					/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					iSubpackIndex ++;
					set_zd_long("0520", iSubpackIndex);
					/*  已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					return 0;
				}
				else	 /* chkflg=1 挂帐检查   */
				{
					char cLvStat[1];
					memset(cLvStat , 0 , sizeof(cLvStat));
					vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);

					set_zd_data("0500","0"); 					
					iRc = pubf_lv_pkg_hang_chk(&payin,"011");
					vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
					cLvStat[0]=payin.T_stat[0];
					vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);					

					/* 业务回执状态不是'00'的，不走帐 add by xyy 2010-6-25 10:28:10 DEBUG*/
					vtcp_log("[%s][%d] rcpstat=[%s][%s]",__FILE__,__LINE__,payin.T_rcpstat,payin.F_rcpstat);
					if (memcmp(payin.T_rcpstat,"00",sizeof(payin.T_rcpstat)) != 0)
					{
						set_zd_double("0390",0.00);
						set_zd_double("0400",0.00);
					}
					/***********end by xyy 2010-6-25 10:28:55************/

					if (iRc == 0)        /* 检查正确 走帐*/
					{      
						strcpy(g_pub_tx.reply, "0000" );  
					}else if(iRc == 100)  /* 挂帐 */
					{      
						strcpy(g_pub_tx.reply, "0000"  ); 
						/* rem by LiuHuafeng 2009-10-12 11:50:24
						lv_destroy(&subpack);						
						return 999;						
						****/
					}else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}  
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐挂帐检查结束###############",__FL__);					
				}
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				lv_destroy(&subpack); 
				return iRc;   
			}
			iTmpIndex ++;
		}/*end of for */
		if ( iTmpIndex > iSubpackIndex )
		{
			vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
			sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
			WRITEMSG
				lv_destroy(&subpack);
			return -1;
		}  
	}
	/****子报文循环完毕****/	
	lv_destroy(&subpack);
	return(0);
}
/****实时贷记回执报文来账处理****/
int lv_deal_ssdjhzlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;

	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   	

	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
	/****检查是否实时贷记回执来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_SSDJHZ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是实时贷记回执来账\n",__FILE__,__LINE__, wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是实时贷记回执报文");WRITEMSG
			return(-1);
	}
	while (1)
	{
		/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
		*   lv_destroy
		***************************************************************/
		memset(&subpack,0,sizeof(TLVPACKAGE));
		lv_create(&subpack);
		iRc = lv_fetchpkg(wp_lvpack,&subpack);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");WRITEMSG
			lv_destroy(&subpack);
			return(-1);
		}
		else if(iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/****得到明细业务的笔数****/
		recnt=lv_count_recnt(&subpack);
		vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);
		/* 说明iSubpackIndex不在这笔子报文中 
		if ( iSubpackIndex >= iTmpIndex+recnt) 
		{
		iTmpIndex = iTmpIndex + recnt;
		if ( iTmpIndex > iSubpackIndex )
		{
		vtcp_log("%s,%d,========这里出现错误了  !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错!!!!=======");
		WRITEMSG
		lv_destroy(&subpack);
		return -1;
		}
		continue;    			
		}*/

		for (index=0;index<recnt;index++)
		{
			if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
			{
				vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__);

				/* ===========1. 插入实时贷记回执来账登记簿 ======= */
				memset(&payin,'\0',sizeof(payin));
				iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
				vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);
				if (iRc<0)
				{
					vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
					sprintf( acErrMsg,"根据子报文填写payin失败");
					WRITEMSG
					lv_destroy(&subpack);
					return(-1);
				}			
				payin.A_stat[0]=STAT_LZRECV1;	
				vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);

				iLvNewRecFromPayin(&payin);
				vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);

				if ( chkflg == 0) 	   /*  chkflg=0 登记  */
				{
					char cLvStat[1];
					vLvBrnoDebug(__FILE__,__LINE__);
					memset(cLvStat, 0 , sizeof(cLvStat));
					get_zd_data("0980",cLvStat);
					vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
					payin.T_stat[0]=cLvStat[0];
					payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
					get_fd_data(DC_TX_BR_NO,payin.T_brno);
					vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);							
					vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);
					/**NEWTCTD 20070912 wangyongwei ****/
					if(memcmp(payin.T_rcpstat,"00",2)!=0){
						payin.T_stat[0]=STAT_LZRECV1;
					}
					/**NEWTCTD 20070912 wangyongwei ****/

					iRc = iLvUpdRecFromPayin(&payin,1);
					if (iRc)
					{
						sprintf( acErrMsg,"插入实时贷记来账登记簿错误");
						WRITEMSG
						lv_destroy(&subpack);
						return(-1);
					}
					lv_destroy(&subpack); 
					vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);

					vtcp_log("%s,%d,========第一步  插入实时贷记回执来账登记簿   成功 !!!==========",__FILE__,__LINE__); 

					/**** ==========2. 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
					if ( iSubpackIndex == 0 )  	   		        
					{
						/******插入来包控制表************/
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
							return -1;
						}
						iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
							sprintf( acErrMsg,"插入来包控制表出错");
							WRITEMSG
							return -1;
						}    		        	
					}    
					vtcp_log("%s,%d,========第二步  插入来包控制表   成功 !!!==========",__FILE__,__LINE__); 
					vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);

					/****  3. 登记完最后一条明细时，处理实时来帐回执  *****/			 					
					/** 这里还有些问题，如果包中有多个子包很更新多次  先暂时这样写了**/							
					if (index == recnt-1 )  
					{
						if ( iSubpackIndex != 0 )  /*  iSubpackIndex == 0  时，已设置 */
						{
							iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
							if (iRc)
							{
								vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
								sprintf( acErrMsg,"产生来包控制表记录错误");
								WRITEMSG
									return -1;
							}	
						}
						wd_lv_wbctl.pack_date = wd_lv_lbctl.o_pack_date ;
						memcpy(wd_lv_wbctl.packid,wd_lv_lbctl.o_packid,sizeof(wd_lv_wbctl.packid)-1);

						iRc = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date =%d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
						if (iRc)
						{
							sprintf( acErrMsg,"定义游标出错 - %d", lvca.rtcode);
							WRITEMSG
								return(-1);
						}
						iRc = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
						if (iRc)
						{
							vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,lvca.rtcode);
							Lv_wbctl_Debug(&wd_lv_wbctl);
							sprintf( acErrMsg,"未查到对应的往包记录!");
							WRITEMSG
								return(-1);
						}
						vtcp_log("[%s][%d]========payin.F_rcpstat==[%.2s] ",__FILE__,__LINE__,payin.F_rcpstat);
						vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);
	
						if (memcmp(payin.F_rcpstat,"00",2)==0)	/****回成功***/
						{
							memcpy(wd_lv_wbctl.packstat,PACKSTAT_SUCCESS,sizeof(wd_lv_wbctl.packstat)-1);
							iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
							if (iRc)
							{
								vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
								Lv_wbctl_Debug(&wd_lv_wbctl);
								sprintf(acErrMsg,"更新原往包失败");
								WRITEMSG
									return(-1);
							}   
							Lv_wbctl_Clo_Upd();

							iRc=lv_dealpkg_resp(&wd_lv_wbctl,"CB1I0000");
							if (iRc)
							{
								vtcp_log("%s,%d,处理原往包错误\n",__FILE__,__LINE__);
								sprintf( acErrMsg,"处理原往包失败");WRITEMSG
									return(-1);
							}                     	    
						}
						else  /****回失败***/
						{
							memcpy(wd_lv_wbctl.packstat,PACKSTAT_REJECT,sizeof(wd_lv_wbctl.packstat)-1);
							iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
							if (iRc)
							{
								vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
								Lv_wbctl_Debug(&wd_lv_wbctl);
								sprintf(acErrMsg,"更新原往包失败");
								WRITEMSG
									return(-1);
							} 							
							Lv_wbctl_Clo_Upd();

							iRc=lv_dealpkg_resp(&wd_lv_wbctl,"CB1O0000");
							if (iRc)
							{
								vtcp_log("%s,%d,处理原往包错误\n",__FILE__,__LINE__);
								sprintf( acErrMsg,"处理原往包失败");WRITEMSG
									return(-1);
							}                     		  
						}
					}
					vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);
					vtcp_log("%s,%d,========第三步  处理实时贷记回执回执  更新原往包控制表 成功 !!!==========",__FILE__,__LINE__); 

					/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
					iSubpackIndex ++;
					set_zd_long("0520", iSubpackIndex);
					/* begin NEWTCTD1 */
					/* begin add by LiuHuafeng 2007-9-1 9:47:34 */
					/* 更新原来往帐应答状态 */
					{
						char cOld_orderno[9];
						char cOld_orbrno [13];
						long lOld_wtdate=0;
						memset(cOld_orderno, 0 , sizeof(cOld_orderno));
						memset(cOld_orbrno , 0 , sizeof(cOld_orbrno ));
						lOld_wtdate=apatoi(payin.F_owtday,8);
						memcpy(cOld_orderno, payin.F_oorderno ,sizeof(cOld_orderno)-1);
						memcpy(cOld_orbrno , payin.F_o_or_br_no ,sizeof(cOld_orbrno )-1);
						vtcp_log("%s,%d orderno=%s,orbrno=%s,wt_date=%ld",__FILE__,__LINE__,cOld_orderno,cOld_orbrno,lOld_wtdate);
						g_reply_int=sql_execute("update lv_pkgreg set rcpstat='%.2s' where orderno='%s' and wt_date=%ld and or_br_no='%s' and lw_ind='1' and pkgno='003' ",payin.F_rcpstat,cOld_orderno,lOld_wtdate,cOld_orbrno);
						if(g_reply_int)
						{
							vtcp_log("%s,%d 更新原来帐应答状态失败 sqlcode=%d",__FILE__,__LINE__,g_reply_int);
						}						
					}
					/* end by LiuHuafeng 2007-9-1 9:47:41 */
					/* 已处理完毕 */
					vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
					return 0;
				}
				else	 /* chkflg=1 挂帐检查   */
				{
					char cLvStat[1];
					memset(cLvStat , 0 , sizeof(cLvStat));
					vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);

					set_zd_data("0500","0"); 
					vLvBrnoDebug(__FILE__,__LINE__);
					iRc = pubf_lv_pkg_hang_chk(&payin,"009");
					vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
					cLvStat[0]=payin.T_stat[0];
					vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
					vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

					if (iRc == 0 || iRc == 100)        /* 走帐*/
					{      
						strcpy(g_pub_tx.reply, "0000" );  
					}
					else if(iRc == 999)    /* 不用走帐了 */
					{      
						strcpy(g_pub_tx.reply, "0010"  );
						lv_destroy(&subpack);
						return 999;
					}else	if(iRc)    /* 挂帐检查出错 */
					{
						sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
						WRITEMSG
							lv_destroy(&subpack);
						return(-1);
					}  
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);
					vLvBrnoDebug(__FILE__,__LINE__);
					vtcp_log("%s,%d payin.F_o_or_br_no==[%s]",__FILE__,__LINE__,payin.F_o_or_br_no);
				}
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				lv_destroy(&subpack);   
				return iRc;   
			}
			iTmpIndex ++;
		}/*end of for */
		if ( iTmpIndex > iSubpackIndex )
		{
			vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
			sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
			WRITEMSG
				lv_destroy(&subpack);
			return -1;
		}  
	}
	/****子报文循环完毕****/	

	return(0);
}
/****实时借记报文回执来账处理****/
int lv_deal_ssjjhzlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	PAY_IN_AREA payin;
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;

	int  iTmpIndex = 0;  	
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   	

	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay());  
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%ld]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	/*********数据初始化******************/
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
	/****检查是否实时借记来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_SSJJHZ,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是实时借记回执来账\n",__FILE__,__LINE__, wp_lvpack->CMTCode);
		sprintf( acErrMsg,"来账不是实时借记回执报文");WRITEMSG
			return(-1);
	}
	/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用
	*   lv_destroy
	***************************************************************/
	memset(&subpack,0,sizeof(TLVPACKAGE));	
	lv_create(&subpack);
	iRc=lv_fetchpkg(wp_lvpack,&subpack);
	if (iRc<0)
	{
		vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"提取报文时出错");WRITEMSG
			lv_destroy(&subpack);
		return(-1);
	}else if(iRc>0)
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
		lv_destroy(&subpack);
		return 0;
	}
	/****得到明细业务的笔数****/
	recnt=lv_count_recnt(&subpack);
	vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);

	if ( iSubpackIndex >= iTmpIndex+recnt) /* 说明iSubpackIndex不在这笔子报文中 */
	{
		vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
		set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
		lv_destroy(&subpack);
		return 0;
	}	
	for (index=0;index<recnt;index++)
	{
		if ( iTmpIndex == iSubpackIndex)  /*  找到要处理的INDEX了 */
		{
			vtcp_log("%s,%d,========找到要处理的INDEX了  !!!==========",__FILE__,__LINE__); 

			/* ===========1. 插入普通借记来账登记簿 ======= */
			memset(&payin,'\0',sizeof(payin));
			iRc=lv_fetchpack_to_payin(wp_lvpack,&subpack,index,&payin);
			if (iRc<0)
			{
				vtcp_log("%s,%d,根据子报文填写payin失败",__FILE__,__LINE__);
				sprintf( acErrMsg,"根据子报文填写payin失败");
				WRITEMSG
					lv_destroy(&subpack);
				return(-1);
			}		 
			if (memcmp(payin.F_rcpstat,"00",2)==0)
			{
				payin.A_stat[0]=STAT_LZRECV2;
			}else
			{
				payin.A_stat[0]=STAT_LZRECV1;
			}		  	
			iLvNewRecFromPayin(&payin);
			if ( chkflg == 0) 	   /*  chkflg=0 登记  */
			{
				char cLvStat[1];
				vLvBrnoDebug(__FILE__,__LINE__);
				memset(cLvStat, 0 , sizeof(cLvStat));
				get_zd_data("0980",cLvStat);
				vtcp_log("%s,%d,得到检查状态=%c]!! \n",__FILE__,__LINE__,cLvStat[0]);
				payin.T_stat[0]=cLvStat[0];
				payin.T_begstat[0]=cLvStat[0];    /** Added by ChenMing   2006.10.10. **/
				get_fd_data(DC_TX_BR_NO,payin.T_brno);
				vtcp_log("%s,%d 写入表中的机构号码是 [%.5s] ",__FL__,payin.T_brno);						

				vtcp_log("%s,%d,登记iSubpackIndex子报文!! \n",__FILE__,__LINE__);

				iRc = iLvUpdRecFromPayin(&payin,1);
				if (iRc)
				{
					sprintf( acErrMsg,"插入实时贷记回执来账登记簿错误");
					WRITEMSG
						lv_destroy(&subpack);
					return(-1);
				}               		
				lv_destroy(&subpack); 
				vtcp_log("%s,%d,========第一步  插入实时借记来账登记簿   成功 !!!==========",__FILE__,__LINE__); 

				/****  2. 因为要循环处理，只有当第一笔明细报文才插入来包控制表 ****/
				if ( iSubpackIndex == 0 )  	   		        
				{
					/******插入来包控制表************/
					iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
					if (iRc)
					{
						vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
						sprintf( acErrMsg,"产生来包控制表记录错误");
						WRITEMSG
							return -1;
					}
					iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
					if (iRc)
					{
						vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
						sprintf( acErrMsg,"插入来包控制表出错");
						WRITEMSG
							return -1;
					}    		        	
				}  
				vtcp_log("%s,%d,========第二步  插入来包控制表   成功 !!!==========",__FILE__,__LINE__); 

				/****  3. 登记完最后一条明细时，处理普通借记来帐回执  *****/						  
				if (index == recnt-1 )  /*  登记完最后一条明细时，处理实时来帐回执  */
				{
					if ( iSubpackIndex != 0 )  /*  iSubpackIndex == 0  时，已设置 */
					{
						iRc = lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
						if (iRc)
						{
							vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"产生来包控制表记录错误");
							WRITEMSG
								return -1;
						}	
					}	
					vtcp_log("[%s][%d]wd_lv_lbctl.o_pack_date=[%d] ",__FILE__,__LINE__,wd_lv_lbctl.o_pack_date);						
					vtcp_log("[%s][%d]wd_lv_lbctl.o_packid=[%s]",__FILE__,__LINE__,wd_lv_lbctl.o_packid);						

					wd_lv_wbctl.pack_date = wd_lv_lbctl.o_pack_date ;
					memcpy(wd_lv_wbctl.packid,wd_lv_lbctl.o_packid,sizeof(wd_lv_wbctl.packid)-1);

					iRc = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date =%d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
					if (iRc)
					{
						sprintf( acErrMsg,"定义游标出错 - %d", lvca.rtcode);
						WRITEMSG
							return(-1);
					}
					iRc = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
					if (iRc)
					{
						vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,lvca.rtcode);
						Lv_wbctl_Debug(&wd_lv_wbctl);
						sprintf( acErrMsg,"未查到对应的往包记录!");
						WRITEMSG
							return(-1);
					}
					vtcp_log("[%s][%d]================ ",__FILE__,__LINE__);

					if (memcmp(payin.F_rcpstat,"00",2)==0)	/****回成功***/
					{
						memcpy(wd_lv_wbctl.packstat,PACKSTAT_SUCCESS,sizeof(wd_lv_wbctl.packstat)-1);
						iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
						if ( iRc ==DB_OK)
						{
							iRc = Lv_wbctl_Clo_Upd();
						}
						if (iRc)
						{
							vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
							Lv_wbctl_Debug(&wd_lv_wbctl);
							sprintf(acErrMsg,"更新原往包失败");
							WRITEMSG
								return(-1);
						}   
						iRc=lv_dealpkg_resp(&wd_lv_wbctl,"CB1I0000");
						if (iRc)
						{
							vtcp_log("%s,%d,处理原往包错误\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"处理原往包失败");WRITEMSG
								return(-1);
						}                     	    
					}
					else  /****回失败***/
					{
						memcpy(wd_lv_wbctl.packstat,PACKSTAT_REJECT,sizeof(wd_lv_wbctl.packstat)-1);
						iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
						if ( iRc ==DB_OK)
						{
							iRc = Lv_wbctl_Clo_Upd();
						}
						if (iRc)
						{
							vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
							Lv_wbctl_Debug(&wd_lv_wbctl);
							sprintf(acErrMsg,"更新原往包失败");
							WRITEMSG
								return(-1);
						}   
						iRc=lv_dealpkg_resp(&wd_lv_wbctl,"CB1O0000");
						if (iRc)
						{
							vtcp_log("%s,%d,处理原往包错误\n",__FILE__,__LINE__);
							sprintf( acErrMsg,"处理原往包失败");WRITEMSG
								return(-1);
						}                    		  
					}
					/*' 最后一条来帐明细 已处理完毕*/	 
					vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
					set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/	 
				}
				vtcp_log("%s,%d,========第三步  处理实时借记来帐回执  更新原往包控制表 成功 !!!==========",__FILE__,__LINE__); 
				/* begin NEWTCTD */
				/* begin add by LiuHuafeng 2007-9-1 21:57:43 */
				/* 更新原来往帐rcpstat  */
				{
					char cOld_orderno[9];
					char cOld_orbrno [13];
					long lOld_wtdate=0;
					memset(cOld_orderno, 0 , sizeof(cOld_orderno));
					memset(cOld_orbrno , 0 , sizeof(cOld_orbrno ));
					lOld_wtdate=apatoi(payin.F_owtday,8);
					memcpy(cOld_orderno, payin.F_oorderno ,sizeof(cOld_orderno)-1);
					memcpy(cOld_orbrno , payin.F_o_or_br_no ,sizeof(cOld_orbrno )-1);
					/**NEWTCTD add by wangyongwei 20070905*/
					char tmp_lvstat[2];
					memset(tmp_lvstat,0,sizeof(tmp_lvstat));
					if(memcmp(payin.F_rcpstat,"00",2)==0){
						tmp_lvstat[0]='6';
					}else{
						tmp_lvstat[0]='5';
					}
					/**NEWTCTD add by 20070905**/
					g_reply_int=sql_execute("update lv_pkgreg set rcpstat='%.2s',lv_sts='%s' where orderno='%s' and wt_date=%ld and or_br_no='%s' and lw_ind='1' and pkgno='004'",payin.F_rcpstat,tmp_lvstat,cOld_orderno,lOld_wtdate,cOld_orbrno);
					if(g_reply_int)
					{
						vtcp_log("%s,%d 更新原来帐应答状态失败 sqlcode=%d",__FILE__,__LINE__,g_reply_int);
					}
				}
				/* end by LiuHuafeng 2007-9-1 21:57:50 */
				/* end NEWTCTD */
				/* 登记完了 iSubpackIndex + 1, 取下一个明细 */
				iSubpackIndex ++;
				set_zd_long("0520", iSubpackIndex);
				/*  已处理完毕 */
				vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
				return 0;
			}
			else	 /* chkflg=1 挂帐检查   */
			{
				char cLvStat[1];
				memset(cLvStat , 0 , sizeof(cLvStat));
				vtcp_log("%s,%d ################来帐挂帐检查开始###############",__FL__);

				set_zd_data("0500","0"); 					
				vLvBrnoDebug(__FILE__,__LINE__);
				iRc = pubf_lv_pkg_hang_chk(&payin,"010");
				vLvBrnoDebug(__FILE__,__LINE__);
				vtcp_log("%s,%d ################来帐挂帐检测结果如下:###############",__FL__);
				cLvStat[0]=payin.T_stat[0];
				vtcp_log("%s,%d 来帐状态====[%c]",__FILE__,__LINE__,cLvStat[0]);
				vtcp_log("%s,%d, iRc====[%d] ======!! \n",__FILE__,__LINE__,iRc);

				/* begin NEWTCTD1 */
				/* begin add by LiuHuafeng 2007-9-1 22:52:02 */
				if(memcmp(payin.F_rcpstat,"00",sizeof(payin.F_rcpstat)))
				{
					/* 如果回执不是00，那么直接进行登记，不需要再记帐处理 */
					payin.T_stat[0]=STAT_LZRECV1;
					cLvStat[0]=payin.T_stat[0];
					set_zd_data("0980",cLvStat);
					strcpy(g_pub_tx.reply, "0000" );
					return 999;
				}
				/* end by LiuHuafeng 2007-9-1 22:56:04 */
				/* end NEWTCTD1 */
				if (iRc == 0)        /* 检查正确 走帐*/
				{      
					strcpy(g_pub_tx.reply, "0000" );  
				}
				else if(iRc == 100)  /* 挂帐 */
				{      
					strcpy(g_pub_tx.reply, "0000"  ); 
				}
				/* begin add by LiuHuafeng 2009-5-13 17:17:26 for 村镇银行 */
				else if( iRc == 999 )
				{
					strcpy(g_pub_tx.reply, "0000"  ); 
				}
				/* end by LiuHuafeng 2009-5-13 17:17:47 */
				else	if(iRc)    /* 挂帐检查出错 */
				{
					sprintf( acErrMsg,"小额来帐 PKG 挂帐检查错误! ");
					WRITEMSG
					lv_destroy(&subpack);
					return(-1);
				}  
				cLvStat[0]=payin.T_stat[0];
				set_zd_data("0980",cLvStat);
				vLvBrnoDebug(__FILE__,__LINE__);				
			}
			/*  已处理完毕 */
			vtcp_log("%s,%d,找到要处理的iSubpackIndex子报文，已处理完毕!! \n",__FILE__,__LINE__);
			lv_destroy(&subpack); 
			return iRc;   
		}
		iTmpIndex ++;
	}/*end of for */
	if ( iTmpIndex > iSubpackIndex )
	{
		vtcp_log("%s,%d,========这里出现错误了 !!!==========",__FILE__,__LINE__);    			  	
		sprintf( acErrMsg,"========查找来帐明细业务时出错 !!!!=======");
		WRITEMSG
			lv_destroy(&subpack);
		return -1;
	} 
	lv_destroy(&subpack);
	return(0);
}
