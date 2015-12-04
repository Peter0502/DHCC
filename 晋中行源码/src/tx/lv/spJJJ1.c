/*************************************************
* 文 件 名:  spJJJ1.c
* 功能描述： 小额收到920拒绝应答的处理1,检查原来来帐,设置相关帐号
*			 
* 作    者:  LiuHuafeng
* 完成日期： 2006-10-14 16:41
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
/**add by YH_BP**/
#include "lv_addit_c.h"
#include "com_parm_c.h"
#include "cashier_c.h"
/**end by YH_BP**/
int spJJJ1()
{
	long iStep=0;
	char cAc_no [20];
	char cAcno110[20];
	char cKinbr[BRNO_LEN+1];
	int iRet=0;
	char cFinishFlag[2];
	char cCmtno[4];
	char cRejectWb[51];/* packid ,packdate,respcode 顺序*/
	struct lv_wbctl_c  sLv_wbctl;
	struct lv_pkgreg_c sLv_pkgreg;
	int iTimes=0;
	char cTmpstr[9];
	
	memset(cRejectWb,0 , sizeof(cRejectWb));

	memset(&sLv_wbctl , 0  ,sizeof(sLv_wbctl ));
	memset(&sLv_pkgreg, 0  ,sizeof(sLv_pkgreg));

	memset(cCmtno, 0 , sizeof(cCmtno));
	memset(cFinishFlag, 0 , sizeof(cFinishFlag));
	memset(cKinbr, 0 , sizeof(cKinbr));
	memset(cAcno110, 0 , sizeof(cAcno110));
	memset(cAc_no , 0 , sizeof(cAc_no ));
	get_zd_data("0300",cAc_no );
	get_zd_data("0500",cFinishFlag);
	get_zd_data(DC_TX_BR_NO,cKinbr);
	get_zd_data("0510",cCmtno);
	get_zd_data("0270",cRejectWb);
	
	get_zd_long("0530",&iStep);
	vtcp_log("%s,%d 得到 iStep---=[%d] ===",__FL__,iStep);	
		
	if(iStep < 0)
	{
		vtcp_log("%s,%d 出错了，不应该到这里来呀!!! ",__FL__);
		strncpy(g_pub_tx.reply,"D102",4);
		goto ErrExit;
	}

	char cRespCode[9];
	memset(cRespCode,0,sizeof(cRespCode));
	/*get_zd_data("0290",cRespCode);	*/
	
	vtcp_log("%s,%d 得到 cAc_no---=[%s] ===",__FL__,cAc_no);	
	vtcp_log("%s,%d 得到 cFinishFlag---=[%s] ===",__FL__,cAc_no);	
	vtcp_log("%s,%d 得到 cKinbr---=[%s] ===",__FL__,cKinbr);
	vtcp_log("%s,%d 得到 cCmtno---=[%s] ===",__FL__,cCmtno);
	vtcp_log("%s,%d 得到 cRejectWb---=[%s] ===",__FL__,cRejectWb);					
	vtcp_log("%s,%d 得到 iStep---=[%d] ===",__FL__,iStep);	

	vtcp_log("%s,%d 得到 cRespCode---=[%s] ===",__FL__,cRespCode);

	memcpy(sLv_pkgreg.packid,cRejectWb,8);
	vtcp_log("%s,%d 得到 sLv_pkgreg.packid---=[%s] ===",__FL__,sLv_pkgreg.packid);		
	memset(cTmpstr,0 , sizeof(cTmpstr));			
	memcpy(cTmpstr,cRejectWb+8,8);	
	vtcp_log("%s,%d 得到 cTmpstr---=[%s] ===",__FL__,cTmpstr);	
	sLv_pkgreg.pack_date = apatoi(cTmpstr,8);
	vtcp_log("%s,%d 得到 sLv_pkgreg.pack_date---=[%d] ===",__FL__,sLv_pkgreg.pack_date);	

	memcpy(cRespCode,cRejectWb+16,8);	
	vtcp_log("%s,%d 得到 cRespCode---=[%s] ===",__FL__,cRespCode);	

	if(strlen(cRespCode) ==0 || cRespCode[3] =='I' )
	{
			vtcp_log("%s,%d 应答码不是拒绝，不应该到这里来呀!!! ",__FL__);
			strncpy(g_pub_tx.reply,"D102",4);
			goto ErrExit;
	}
	
	iTimes=0;

 	g_reply_int = Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"pack_date=%d and packid='%s' order by rowid",sLv_pkgreg.pack_date,sLv_pkgreg.packid);
 	if(g_reply_int)
 	{
 		vtcp_log("%s,%d 定义游标错误",__FL__);
 		strncpy(g_pub_tx.reply,"D102",4);
 		goto ErrExit;
 	}
			
	while(1)
	{

    g_reply_int = Lv_pkgreg_Fet_Sel( &sLv_pkgreg , g_pub_tx.reply );		
		if(g_reply_int==DB_NOTFOUND)
		{
			
			if(iTimes == 0)
			{
				
				vtcp_log("[%s,%d] 一条记录也没找到!!  ",__FL__);
				vtcp_log("[%s,%d] 一条记录也没找到!!  ",__FL__);
				strcpy(acErrMsg,"没有找到原实时贷记往帐记录");
				WRITEMSG
				/* rem by LiuHuafeng 2009-5-15 10:35:36 由于有村镇银行业务所以有可能一条都找不到，
				 * 找不到就按处理完毕处理，直接跳到J099 
				 *return(-1);
				 **begin add by LiuHuafeng for 村镇银行****/
				set_zd_long("0530",-1);
				set_zd_long("0500",1);/* 设置finish flag =1 */
				strncpy(g_pub_tx.reply,"0010",4); /* 不需要帐务处理了 跳转到J099 */
				vtcp_log("[%s,%d] ===== 920 记帐处理完毕 !!! ",__FL__);
				goto ErrExit;	
			}
			 
			 /* 设置0530 为 -1 告诉下一个交易处理结束了,不需要帐务处理了 */
			 set_zd_long("0530",-1);
			 strncpy(g_pub_tx.reply,"0010",4); /* 不需要帐务处理了 跳转到J099 */
		   vtcp_log("[%s,%d] ===== 920 记帐处理完毕 !!! ",__FL__);
			 goto ErrExit;
		}
		if(g_reply_int)
		{
			vtcp_log("%s,%d 读取游标错误",__FL__);
			strncpy(g_pub_tx.reply,"D102",4);
			goto ErrExit;
		}
		
		if(iTimes==iStep)
		{
			/* 是要处理的纪录 */
			/* 设计交易机构 */
			set_zd_data(DC_TX_BR_NO,sLv_pkgreg.br_no);
			memset(cAc_no, 0 ,sizeof(cAc_no));
			/*begin NEWYX add by LiuHuafeng 2007-4-2 17:17 
				如果是借记回执业务，应答码不是00，则不需要检查，同时设置相关金额为0 */
			if(
				  (!memcmp(sLv_pkgreg.pkgno,PKGNO_PTJJHZ,3) ||
				   !memcmp(sLv_pkgreg.pkgno,PKGNO_DQJJHZ,3) ||
				   !memcmp(sLv_pkgreg.pkgno,PKGNO_SSJJHZ,3) ||
				   !memcmp(sLv_pkgreg.pkgno,PKGNO_SSDJHZ,3))
				&&
				  memcmp(sLv_pkgreg.rcpstat,"00",sizeof(sLv_pkgreg.rcpstat)-1)
			)
			{
				set_zd_double("0400",0.0);
				strncpy(g_pub_tx.reply,"0010",4); /* 不需要帐务处理了 跳转到J099 */
				set_zd_long("0530",-1);
				vtcp_log("[%s,%d] ===== 920 记帐处理完毕 !!! ",__FL__);
				goto ErrExit;
			}
			else if(!memcmp(sLv_pkgreg.pkgno,PKGNO_PTJJ,3) ||
				!memcmp(sLv_pkgreg.pkgno,PKGNO_DQJJ,3) ||
				!memcmp(sLv_pkgreg.pkgno,PKGNO_SSJJ,3)
			)
			{
				/* 借计业务也不需要判断帐户状态等，同时金额设置为0 */
				set_zd_double("0400",0.0);
				strncpy(g_pub_tx.reply,"0010",4); /* 不需要帐务处理了 跳转到J099 */
				set_zd_long("0530",-1);
				vtcp_log("[%s,%d] ===== 920 记帐处理完毕 !!! ",__FL__);
				goto ErrExit;
			}
			/* begin add by LiuHuafeng 2009-5-13 17:56:36 for 村镇银行 */
			else if(strncmp(sLv_pkgreg.br_no,"9999",BRNO_LEN)==0)
			{
				set_zd_double("0400",0.0);
				strncpy(g_pub_tx.reply,"0010",4); /* 不需要帐务处理了 跳转到J099 */
				set_zd_long("0530",-1);
				vtcp_log("[%s,%d] ===== 920 记帐处理完毕 村镇银行业务不进行账务处理 !!! ",__FL__);
				goto ErrExit;
			}
			/* end by LiuHuafeng 2009-5-13 17:56:49 */
      else
			{
			/* END NEWYX by LiuHuafeng 2007-4-2 17:30 */
				if(!memcmp(sLv_pkgreg.pkgno,"007",3))
				{
					if(sLv_pkgreg.lv_o_sts[0]==STAT_LZRECV2)
					{
						/* 根据付款人帐号进行处理 */
						strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1);
					}
					else
					{
						/* 根据参数表中得到的挂账帐户设新的帐号 */
						iRet=pub_base_GetParm_Str(sLv_pkgreg.br_no,PARM_LVGZ,cAc_no);
						if(iRet)
						{
							vtcp_log("%s,%d 没有配置机构%s的小额挂账帐号",__FL__,cKinbr);
							strncpy(g_pub_tx.reply,"P076",4);
							goto ErrExit;
						}
					}
				}
				else
				{
					  /**add by YH_BP **/
					  vtcp_log("[%s][%d]sLv_pkgreg.pkgno=[%s]\n",__FILE__,__LINE__,sLv_pkgreg.pkgno);
					  vtcp_log("[%s][%d]sLv_pkgreg.txnum=[%s]\n",__FILE__,__LINE__,sLv_pkgreg.txnum);
					  vtcp_log("[%s][%d]sLv_pkgreg.lw_ind=[%c]\n",__FILE__,__LINE__,sLv_pkgreg.lw_ind[0]);
					  if(memcmp(sLv_pkgreg.pkgno,"010",3)==0 && memcmp(sLv_pkgreg.txnum,"30103",5)==0 && sLv_pkgreg.lw_ind[0]=='1')
					  {
					  	  int iRc =0;
					  	  char   cBr_no[BRNO_LEN+1];
					  	  char   cPayacno[21];
					  	  struct lv_addit_c sLv_addit;
					  	  struct cashier_c  sCashier ;
					  	  struct com_parm_c sCom_parm;
					  	  TYJL1_ADD_AREA lv_tyjl1;
					  	  
					  	  /**add by NEWBP1 只有往帐发送的拒绝才走帐吧**/
					  	  if(sLv_pkgreg.lv_sts[0] !=STAT_WZSEND )
					  	  {
			          	   /* 借计业务也不需要判断帐户状态等，同时金额设置为0 */
			          	   set_zd_double("0400",0.0);
			          	   strncpy(g_pub_tx.reply,"0010",4); /* 不需要帐务处理了 跳转到J099 */
			          	   set_zd_long("0530",-1);
			          	   vtcp_log("[%s,%d] ===== 920 记帐处理完毕 !!! ",__FL__);
			          	   goto ErrExit;
			          }
					  	  /***end by NEWBP1***/
					  	  memset(cPayacno      , 0 , sizeof(cPayacno));
					  	  memset(cBr_no        , 0 , sizeof(cBr_no));
					  	  memset(&lv_tyjl1     , 0 , sizeof(lv_tyjl1));
					  	  memset(&sLv_addit    , 0 , sizeof(sLv_addit));
					  	  memset(&sCashier     , 0 , sizeof(sCashier));
					  	  memset(&sCom_parm    , 0 , sizeof(sCom_parm));
					  	  
					  	  /**先找该交易的附加域吧**/
					  	  vtcp_log("[%s][%d]sLv_pkgreg.in_date=[%d]\n",__FILE__,__LINE__,sLv_pkgreg.in_date);
					  	  vtcp_log("[%s][%d]sLv_pkgreg.addid  =[%s]\n",__FILE__,__LINE__,sLv_pkgreg.addid);
					  	  
					  	  iRc = Lv_addit_Sel(g_pub_tx.reply,&sLv_addit,"tagname='001' and tx_date='%ld' and addid='%s'",\
					  	                                    sLv_pkgreg.in_date,sLv_pkgreg.addid);
					  	  if(iRc)
                {
                    vtcp_log("[%s][%d],查询Lv_addit表失败[%d]",__FILE__,__LINE__, iRc);
                    sprintf(acErrMsg,"查询Lv_addit表失败[%s][%d]",__FILE__,__LINE__);
                    WRITEMSG
                    return(iRc);
                }
                /**从附加域里得到签发日期和本票号码**/
                vtcp_log("[%s][%d]sLv_addit.tagdata=[%s]\n",__FILE__,__LINE__,sLv_addit.tagdata);
                memcpy(&lv_tyjl1,sLv_addit.tagdata,sizeof(lv_tyjl1));
                vtcp_log("[%s][%d]lv_tyjl1.cpday=[%.8s]\n",__FILE__,__LINE__,lv_tyjl1.cpday);
                vtcp_log("[%s][%d]lv_tyjl1.cpno =[%.20s]\n",__FILE__,__LINE__,lv_tyjl1.cpno);
					  	  /**本票信息表中查到brno**/
					  	  iRc = Cashier_Dec_Upd(g_pub_tx.reply,"pono='%.20s' and signday='%.8s' ",\
                                    lv_tyjl1.cpno,lv_tyjl1.cpday);
                if (iRc)
                {
                    vtcp_log("%s,%d,查找本票基本信息表错误[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"插入回执登记簿失败");
                    WRITEMSG
                    return(iRc);
                }
                vtcp_log("[%s][%d]准备FETCH 数据啦[%d]\n",__FILE__,__LINE__,iRc);
                iRc = Cashier_Fet_Upd(&sCashier,g_pub_tx.reply);
                if (iRc!=0 && iRc!=100)
                {
                    vtcp_log("%s,%d,FET本票基本信息表错误[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"FET本票基本信息表错误");
                    WRITEMSG
                    return(iRc);
                }
                else if(iRc == 100)
                {
                   vtcp_log("%s,%d,没有这个本票纪录[%d]\n",__FILE__,__LINE__,iRc);
                   sprintf( acErrMsg,"没有这个本票纪录");
                   WRITEMSG
                   return(iRc);
                }
                printf("[%s][%d]票据状态为[%s]\n",__FILE__,__LINE__,sCashier.stat);
                if(sCashier.stat[0]=='2' || sCashier.stat[0]=='3')
                {
                    sCashier.lactcls[0] = sCashier.stat[0];   /* 本票当前状态标志 0-登记 1-签发 */
                    sCashier.stat[0] = '1';                   /* 本票当前状态标志 0-登记 1-签发 */
                }
                memcpy(cBr_no,sCashier.brno,BRNO_LEN);
                vtcp_log("[%s][%d]本票业务机构cBr_no=[%s]\n",__FILE__,__LINE__,cBr_no);
                iRc = Cashier_Upd_Upd(sCashier,g_pub_tx.reply);
                if (iRc)
                {
                    vtcp_log("%s,%d,更新本票基本信息表错误[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"更新本票基本信息表错误");
                    WRITEMSG
                    return(iRc);
                }
                Cashier_Clo_Upd();
                vtcp_log("[%s][%d]更新本票信息完毕[%s]\n",__FILE__,__LINE__,cBr_no);
                iRc = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn=88",cBr_no);
                if (iRc)
                {
                    vtcp_log("%s,%d,查找本票签发户错误[%d]\n",__FILE__,__LINE__,iRc);
                    sprintf( acErrMsg,"查找本票签发户错误");
                    WRITEMSG
                    return(iRc);
                }
                strncpy(cPayacno,sCom_parm.val,sizeof(cPayacno)-1);
                zip_space(cPayacno);
                memcpy(cAc_no,cPayacno,sizeof(cAc_no)-1);
                vtcp_log("[%s][%d]cAc_no=[%s]\n",__FILE__,__LINE__,cAc_no);
					  	  /**通过brno得到com_parm表中88的内部帐户,赋给cAc_no**/
				    }
				    else/**end by YH_BP**/
				    {
					      strncpy(cAc_no,sLv_pkgreg.pay_ac_no,sizeof(cAc_no)-1);
					  }
				}
				pub_base_old_acno(cAc_no);
				/* 检查付款帐号状态是否正常,基本可以不考虑,先不查了 */
      	
      	
      	/* 现金科目的往帐业务要拒绝到挂账科目上  Added by ChenMing  2006-11-5 15:26  */
      	if( (strlen(cAc_no)==5 && memcmp(cAc_no, "10101", 5 )==0) ||
      		  (strlen(cAc_no)==7 && memcmp(cAc_no,"1010100",7 )==0 ) )
      	{
      		  iRet = iHvHang_Check_Brno(0, sLv_pkgreg.br_no);  /* 设置好机构 */
      	    if(iRet)
      	  	{
      	  		   vtcp_log("%s,%d iHvHang_Check_Brno()  出错 iRet=[%d]",__FILE__,__LINE__,iRet);
   			         sprintf(acErrMsg," iHvHang_Check_Brno()  出错 !! ");
   			         WRITEMSG       		
      	  		   return -1;
      	  	}
      	    
      	    memset(cAc_no, 0 , sizeof(cAc_no));
     				/* 现金科目的往帐业务要拒绝到挂账科目上 */
    				iRet=pub_base_GetParm_Str(cKinbr,PARM_LVGZ,cAc_no);
    				if(iRet)
    				{
    					vtcp_log("%s,%d 系统配置错误，得到机构%s的挂账帐号错误[%d]",__FILE__,__LINE__,iRet);
    					strncpy(acErrMsg,"得到本机构挂账账号错误",50);
    					set_zd_data("0130",acErrMsg);
    					return iRet;
    				}  
    				set_zd_data("0980","5");         /* 拒绝 */       	
      	}
		  	vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);
		  
		  
				/* 这里好像有问题，5开头的也为1  ChenMing 2006-10-23 17:12*/						
				if(cAc_no[0]=='1'|| cAc_no[0]=='5') 
				{
		  	    vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);				
					  set_zd_data("0680","1"); 
				}
				/* 这里应该加上如果是卡 也为1 wudawei 20131008 */
				else if(memcmp(cAc_no,"621",3) == 0 && strlen(cAc_no) > 13 )
				{
						vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);				
					  set_zd_data("0680","1"); 
				}
				/* add end wudawei 20131008 */
				else
				{
		  	   vtcp_log("[%s] [%d] cAc_no====[%s]",__FL__,cAc_no);				
					 set_zd_data("0680","6");
				}
				
				/* 处理完后重新设置0530域中的值 */
				/* 单独配置一个万能记帐,设置分录为 0400 为交易金额 ,0300位客户帐号 0660 是他的借贷
				   0320 为过渡科目,一借一贷,设置0330分录为414, 0720 是其借贷
				 */
				 /* For Test */
				char trance_type[2];
				memset(trance_type,0,2); 
	    	get_zd_data("0680",trance_type);			 
	    	vtcp_log("[%s,%d] trance_type=======[%s]",__FILE__,__LINE__,trance_type);	
					 
				set_zd_double("0400",sLv_pkgreg.tx_amt * -1);
				set_zd_long("0530",iStep+1);
				set_zd_data("0300",cAc_no);
				set_zd_data("0660","1");
				set_zd_data("0320",LVLZGD);/* 设置过渡科目 */
				set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
				set_zd_data("0720","2");/* 414等待清算科目借贷 */
			}
			break;
		}/*  开始时 iTiems==iStep==0  这样会形成死循环!!! 
		else if(iTimes < iStep)
		{
			iTimes++;
		}
		else
			break;    Rem by ChenMing  2006-10-22 20:13 */
		iTimes++;
		
		if( iTimes > iStep )
		{
			 strcpy(acErrMsg,"出错了，不可能到这里来的!!! ");WRITEMSG
			 return(-1);					     	     
		}
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
