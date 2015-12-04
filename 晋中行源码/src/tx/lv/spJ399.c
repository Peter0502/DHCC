/******************************************************************
模块名称: 未完成交易信息查询
程序名称: spJ399.pc
函数原型: spJ399()
输入参数:
输出参数:
使用说明:
编 写 者: liuyue
*******************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_fzf_c.h"
#include "hv_zf_c.h"




char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);
char * iNum2RcpStat(char *rcpstat,char *cTmpList ,char *pkgno);

 struct Tis_Search_Unanswer {
	char brno[6];	/**交易机构**/
	char lw_ind[2];
	char pkgno[4];
	char wt_date[9];
	char pay_ac_no[33];
	char cash_ac_no[33];
	double minmoney;
	double maxmoney;
	
};



/*************************************************************************
*                                                                        *
*   函数名:         spJ399()                                      *
*   函数说明:       1.查询没有被回复的记录笔数                          *
*************************************************************************/
int spJ399()
{
	vtcp_log("[%s][%d]进入支付交易高级查询!\n",__FILE__,__LINE__);
	LV_FD123_AREA fd123;
	PAY_IN_AREA   lv_payin;
	struct lv_pkgreg_c lv_pkgreg;
	struct Tis_Search_Unanswer tis22;
	char cWt_date[9];
	char cMinMoney[16];
	char cMaxMoney[16];
	char cPay_ac_no[33];
	char cCash_ac_no[33];
	
	char    cPack_date[9];
	char    cStat[21];
	char 	cRcp_stat[21];
	
	
	
	int ret = 0;
	int iWt_date;
	double dMinMoney = 0.0 ;
	double dMaxMoney  = 0.0;
	FILE *fp;
	int ttlnum = 0;
	
	char cTmplist[100];
	char cSql_buf[1000];
	
	char    cFilename[100];    /*文件名*/

	memset(&fd123,0x00,sizeof(fd123));
	memset(&lv_payin,0x00,sizeof(lv_payin));
	memset(&lv_pkgreg,0x00,sizeof(lv_pkgreg));
	memset(&tis22,	0x00,	sizeof(tis22));
	
	memset(cWt_date,0x00,sizeof(cWt_date));
	memset(cMinMoney,0x00,sizeof(cMinMoney));
	memset(cMaxMoney,0x00,sizeof(cMaxMoney));
	memset(cPay_ac_no,0x00,sizeof(cPay_ac_no));
	memset(cCash_ac_no,0x00,sizeof(cCash_ac_no));
	memset(cTmplist,0x00,sizeof(cTmplist));
	memset(cSql_buf,0x00,sizeof(cSql_buf));
	
	memset(cPack_date,	0x00 ,	sizeof(cPack_date));
	memset(cStat,		0x00 ,	sizeof(cStat));
	memset(cRcp_stat,	0x00 ,	sizeof(cRcp_stat));
	memset(cFilename,	0x00 ,  sizeof(cFilename));
	
	tis22.minmoney = 0.0;
	tis22.maxmoney = 0.0;
	
	
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	
	
	
	pub_base_AllDwnFilName(cFilename);
	fp = fopen(cFilename,"rb");
	if(fp==NULL)
	{
		sprintf(acErrMsg,"打开文件[%s]错误!\n",cFilename);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		
	}
	fclose(fp);
	
	ret =iLvTisToPayin(&lv_payin);
	if(ret)
	{
		vtcp_log("[%s][%d]获取8583内容!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	
	get_zd_data("0880",tis22.brno);
	
	memcpy(cMinMoney,lv_payin.F_ptxamt,sizeof(lv_payin.F_ptxamt));
	memcpy(cMaxMoney,lv_payin.F_jtxamt,sizeof(lv_payin.F_jtxamt));
	memcpy(cPay_ac_no,lv_payin.F_payactno,sizeof(lv_payin.F_payactno));
	memcpy(cCash_ac_no,lv_payin.F_cashactno,sizeof(lv_payin.F_cashactno));

	vtcp_log("[%s][%d]cMinMoney==[%s] cMaxMoney===[%s]\n",__FILE__,__LINE__,cMinMoney,cMaxMoney);	
	vtcp_log("[%s][%d]cPay_ac_no==[%s]\n",__FILE__,__LINE__,cPay_ac_no);
	vtcp_log("[%s][%d]cCash_ac_no=[%s]\n",__FILE__,__LINE__,cCash_ac_no);
	
	
	str2dbl(cMinMoney,sizeof(cMinMoney)-1,2,&dMinMoney);
	str2dbl(cMaxMoney,sizeof(cMaxMoney)-1,2,&dMaxMoney);
	/*****
	dMinMoney = apatoi(cMinMoney,sizeof(cMinMoney)-1)/100;
	dMaxMoney = apatoi(cMaxMoney,sizeof(cMaxMoney)-1)/100;
	****/
	vtcp_log("[%s][%d]111tis22.minmoney===[%d]\n",__FILE__,__LINE__,tis22.minmoney);
	vtcp_log("[%s][%d]tis22.maxmoney===[%d]\n",__FILE__,__LINE__,tis22.maxmoney);
	vtcp_log("[%s][%d]dMinMoney===[%lf]\n",__FILE__,__LINE__,dMinMoney);
	vtcp_log("[%s][%d]dMaxMoney===[%lf]\n",__FILE__,__LINE__,dMaxMoney);
	zip_space(cPay_ac_no);
	zip_space(cCash_ac_no);

	
	vtcp_log("[%s][%d]cPay_ac_no==[%s]\n",__FILE__,__LINE__,cPay_ac_no);
	vtcp_log("[%s][%d]cCash_ac_no=[%s]\n",__FILE__,__LINE__,cCash_ac_no);
	if(cPay_ac_no[0]==0x00)
		memcpy(tis22.pay_ac_no,"",sizeof(tis22.pay_ac_no));
	else 
		memcpy(tis22.pay_ac_no,cPay_ac_no,strlen(cPay_ac_no));
	if(cCash_ac_no[0]==0x00)
		memcpy(tis22.cash_ac_no,"",sizeof(tis22.cash_ac_no));
	else
		memcpy(tis22.cash_ac_no,cCash_ac_no,strlen(cCash_ac_no));
		
	memcpy(tis22.wt_date,lv_payin.F_wtday,sizeof(lv_payin.F_wtday));
	tis22.lw_ind[0] = lv_payin.F_lw_ind[0];
	memcpy(tis22.pkgno,lv_payin.F_pkgno,sizeof(lv_payin.F_pkgno));
	tis22.maxmoney = dMaxMoney;
	tis22.minmoney = dMinMoney;
	
	vtcp_log("[%s][%d]dMinMoney===[%d]\n",__FILE__,__LINE__,tis22.minmoney);
	vtcp_log("[%s][%d]dMaxMoney===[%d]\n",__FILE__,__LINE__,tis22.maxmoney);
	vtcp_log("[%s][%d]cPay_ac_no==[%s]\n",__FILE__,__LINE__,tis22.pay_ac_no);
	vtcp_log("[%s][%d]cCash_ac_no=[%s]\n",__FILE__,__LINE__,tis22.cash_ac_no);
	
	vtcp_log("[%s][%d]tis22===[%s]\n",__FILE__,__LINE__,(char *)&tis22);
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) &&memcmp(tis22.brno,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("%s,%d,非清算中心不能查询他行交易\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
		
	/*****交易类型****/
	if(memcmp(tis22.pkgno,"000",3))/***不是选择了全部的业务类型**/
	{
	  if(tis22.pay_ac_no[0]!=0x00)
	  {
    	if(tis22.cash_ac_no[0]!=0x00)
    	{
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pkgno= '%.3s' and pay_ac_no = '%.*s and cash_ac_no = '%.*s' and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date,tis22.lw_ind[0], tis22.pkgno, strlen(tis22.pay_ac_no), tis22.pay_ac_no, strlen(tis22.cash_ac_no), tis22.cash_ac_no, tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c'  and pkgno= '%.3s' and pay_ac_no = '%.*s and cash_ac_no = '%.*s'  ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], tis22.pkgno, strlen(tis22.pay_ac_no), tis22.pay_ac_no, strlen(tis22.cash_ac_no), tis22.cash_ac_no);
		    }
		  }
		  else
		  {
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pkgno= '%.3s' and pay_ac_no = '%.*s and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date, tis22.lw_ind[0], tis22.pkgno, strlen(tis22.pay_ac_no), tis22.pay_ac_no, tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c'  and pkgno= '%.3s' and pay_ac_no = '%.*s  ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], tis22.pkgno, strlen(tis22.pay_ac_no), tis22.pay_ac_no);
		    }
		  }	
	  }
	  else
	  {
    	if(tis22.cash_ac_no[0]!=0x00)
    	{
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pkgno= '%.3s' and cash_ac_no = '%.*s' and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date, tis22.lw_ind[0], tis22.pkgno, strlen(tis22.cash_ac_no), tis22.cash_ac_no, tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c'  and pkgno= '%.3s' and cash_ac_no = '%.*s'  ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], tis22.pkgno,  strlen(tis22.cash_ac_no), tis22.cash_ac_no);
		    }
		  }
		  else
		  {
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pkgno= '%.3s' and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date,tis22.lw_ind[0], tis22.pkgno,  tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c'  and pkgno= '%.3s' ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], tis22.pkgno );
		    }
		  }	
	  }
	}
	else
	{
	  if(tis22.pay_ac_no[0]!=0x00)
	  {
    	if(tis22.cash_ac_no[0]!=0x00)
    	{
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pay_ac_no = '%.*s and cash_ac_no = '%.*s' and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date,tis22.lw_ind[0], strlen(tis22.pay_ac_no), tis22.pay_ac_no, strlen(tis22.cash_ac_no), tis22.cash_ac_no, tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pay_ac_no = '%.*s and cash_ac_no = '%.*s'  ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], strlen(tis22.pay_ac_no), tis22.pay_ac_no, strlen(tis22.cash_ac_no), tis22.cash_ac_no);
		    }
		  }
		  else
		  {
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pay_ac_no = '%.*s and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date, tis22.lw_ind[0], strlen(tis22.pay_ac_no), tis22.pay_ac_no, tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and pay_ac_no = '%.*s  ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], strlen(tis22.pay_ac_no), tis22.pay_ac_no);
		    }
		  }	
	  }
	  else
	  {
    	if(tis22.cash_ac_no[0]!=0x00)
    	{
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and cash_ac_no = '%.*s' and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date, tis22.lw_ind[0], tis22.pkgno, strlen(tis22.cash_ac_no), tis22.cash_ac_no, tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and cash_ac_no = '%.*s'  ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], tis22.pkgno,  strlen(tis22.cash_ac_no), tis22.cash_ac_no);
		    }
		  }
		  else
		  {
    	  if(tis22.maxmoney!=0.0)
    	  {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' and tx_amt>=%lf and tx_amt <=%lf ", tis22.brno, tis22.wt_date,tis22.lw_ind[0], tis22.pkgno,  tis22.minmoney, tis22.maxmoney );
        }
		    else
		    {
		  	   ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "br_no = '%s' and wt_date = '%s' and lw_ind = '%c' ", tis22.brno,tis22.wt_date, tis22.lw_ind[0], tis22.pkgno );
		    }
		  }	
	  }
	}

	vtcp_log("[%s][%d]查询支付类交易!\n",__FILE__,__LINE__);
	/* ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, cSql_buf); */
	if(ret)
	{
		sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	ttlnum=0;
	while(1)
	{
		ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
		if( ret==100 &&ttlnum>0 )
		{
			break;
		}
		else if(ret==100 &&ttlnum==0)
		{
			vtcp_log("[%s][%d]没有找到对应的记录!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"找不到对应的记录!\n");
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		else if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]数据库操作异常,请与科技科联系!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"数据库操作异常,请与科技科联系!\n");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		if( !ttlnum )
		{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			fprintf( fp,"~PKG编号|交易类型|交易行|顺序号|交易日期|交易金额|发起行|接受行|包序号  |包日期  |交易状态|回执状态|\n" );
		}
		if(lv_pkgreg.pack_date==0)
			strcpy(cPack_date,"        ");
		else
			sprintf(cPack_date,"%08d",lv_pkgreg.pack_date);

			vtcp_log("[%s][%d]第【%d】笔,交易类型=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(lv_pkgreg.pkgno,cTmplist));
			fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.pkgno,iNum2TxType(lv_pkgreg.pkgno,cTmplist),lv_pkgreg.br_no,lv_pkgreg.orderno,lv_pkgreg.wt_date,lv_pkgreg.tx_amt,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.packid,cPack_date,iNum2Stat(lv_pkgreg.lv_sts[0],cStat),iNum2RcpStat(lv_pkgreg.rcpstat,cRcp_stat,lv_pkgreg.pkgno));
			ttlnum++;

	}      
		ret=Lv_pkgreg_Clo_Sel();
		if( !ttlnum )
		{
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}
		else
		{
			fclose(fp);
			set_zd_data( DC_FILE_SND,"1");
		}

OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"查询未完成交易成功[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
	
	
}




	/**交易类型**/
char * iNum2TxType(char *ch,char *cTmpList)
{

	vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);

	char cName[20];
	memset(cName,0x00,sizeof(cName));

	if(!memcmp(ch,"001",3))
	{           
		strcpy(cName,"普通贷记");
	} 
	else if(!memcmp(ch,"002",3))
	{
		strcpy(cName,"普通借记");
	}
	else if(!memcmp(ch,"003",3))
	{
		strcpy(cName,"实时贷记");
	}
	else if(!memcmp(ch,"004",3))
	{
		strcpy(cName,"实时借记");
	}
	else if(!memcmp(ch,"005",3))
	{
		strcpy(cName,"定期贷记");
	}
	else if(!memcmp(ch,"006",3))
	{
		strcpy(cName,"定期借记");
	}
	else if(!memcmp(ch,"007",3))
	{
		strcpy(cName,"贷记退汇");
	}
	else if(!memcmp(ch,"008",3))
	{
		strcpy(cName,"普通借记回执");
	}
	else if(!memcmp(ch,"009",3))
	{
		strcpy(cName,"实时贷记回执");
	}
	else if(!memcmp(ch,"010",3))
	{
		strcpy(cName,"实时借记回执");
	}
	else if(!memcmp(ch,"011",3))
	{
		strcpy(cName,"定期借记回执");
	}
	else if(!memcmp(ch,"012",3))
	{
		strcpy(cName,"通用信息");
	}
	else if(!memcmp(ch,"013",3))
	{
		strcpy(cName,"实时信息");
	}
	else if(!memcmp(ch,"301",3))
	{
		strcpy(cName,"小额查询");
	}
	else if(!memcmp(ch,"302",3))
	{
		strcpy(cName,"小额查复");
	}
	else if(!memcmp(ch,"303",3))
	{
		strcpy(cName,"自由格式");
	}
	else if(!memcmp(ch,"319",3))
	{
		strcpy(cName,"申请退回");
	}
	else if(!memcmp(ch,"320",3))
	{
		strcpy(cName,"退回应答");
	}
	else if(!memcmp(ch,"321",3))
	{
		strcpy(cName,"申请撤销");
	}
	else if(!memcmp(ch,"322",3))
	{
		strcpy(cName,"撤销应答");
	}
	else if(!memcmp(ch,"323",3))
	{
		strcpy(cName,"冲正申请");
	}
	else if(!memcmp(ch,"324",3))
	{
		strcpy(cName,"冲正应答");
	}
	else if(!memcmp(ch,"325",3))
	{
		strcpy(cName,"冲正通知");
	}
	else if(!memcmp(ch,"327",3))
	{
		strcpy(cName,"止付申请");
	}
	else if(!memcmp(ch,"328",3))
	{
		strcpy(cName,"止付应答");
	}
	else
		strcpy(cName,"未知交易类型");
	memcpy(cTmpList,cName,strlen(cName));
	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
	return  cName;
}

/**交易状态**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if(ch=='0')
	{
		strcpy(cTmpList,"无效状态");
	}
	else if(ch=='1')
	{
		strcpy(cTmpList,"往账录入");
	}
	else if(ch=='3')
	{
		strcpy(cTmpList,"往账复核");
	}
	else if(ch=='4')
	{
		strcpy(cTmpList,"往账发送");
	}
	else if(ch=='5')
	{
		strcpy(cTmpList,"往账拒绝");
	}
	else if(ch=='6')
	{
		strcpy(cTmpList,"往账扎差");
	}
	else if(ch=='7')
	{
		strcpy(cTmpList,"往账排队");
	}
	else if(ch=='8')
	{
		strcpy(cTmpList,"往账撤销等待");
	}
	else if(ch=='9')
	{
		strcpy(cTmpList,"往账撤销");
	}
	else if(ch=='A')
	{
		strcpy(cTmpList,"退回冲正止付等待");
	}
	else if(ch=='B')
	{
		strcpy(cTmpList,"往账退回");
	}
	else if(ch=='C')
	{
		strcpy(cTmpList,"往账已冲正");
	}
	else if(ch=='D')
	{
		strcpy(cTmpList,"往账已止付");
	}
	else if(ch=='G')
	{
		strcpy(cTmpList,"往账异常挂帐");
	}
	else if(ch=='H')
	{
		strcpy(cTmpList,"临时接收,不记帐");
	}
	else if(ch=='I')
	{
		strcpy(cTmpList,"正式接收");
	}
	else if(ch=='J')
	{
		strcpy(cTmpList,"全国押错");
	}
	else if(ch=='K')
	{
		strcpy(cTmpList,"地方押错");
	}
	else if(ch=='L')
	{
		strcpy(cTmpList,"全国押和地方押都错");
	}
	else if(ch=='M')
	{
		strcpy(cTmpList,"清算中心挂帐");
	}
	else if(ch=='N')
	{
		strcpy(cTmpList,"支行挂帐");
	}
	else if(ch=='O')
	{
		strcpy(cTmpList,"来账退回等待");
	}
	else if(ch=='P')
	{
		strcpy(cTmpList,"来账退回");
	}
	else if(ch=='Q')
	{
		strcpy(cTmpList,"同意未发送");
	}
	else if(ch=='R')
	{
		strcpy(cTmpList,"同意已发送");
	}
	else if(ch=='S')
	{
		strcpy(cTmpList,"不同意未发送");
	}
	else if(ch=='T')
	{
		strcpy(cTmpList,"不同意已发送");
	}
	else if(ch=='U')
	{
		strcpy(cTmpList,"来账已冲正");
	}
	else if(ch=='V')
	{
		strcpy(cTmpList,"来账已止付");
	}
	else if(ch=='W')
	{
		strcpy(cTmpList,"来账已付款");
	}
	else if(ch=='Z')
	{
		strcpy(cTmpList,"来账异常挂帐");
	}
	else
		strcpy(cTmpList,"未知状态");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}

/***回执状态*****/
char * iNum2RcpStat(char *rcpstat , char *cTmpList,char *pkgno)
{
	vtcp_log("liuyue[%s][%d]rcpstat==[%s]\n",__FILE__,__LINE__,rcpstat);

	if(!memcmp(pkgno,PKGNO_PTJJ,3) ||!memcmp(pkgno,PKGNO_PTJJHZ,3) ||!memcmp(pkgno,PKGNO_DQJJ,3)||!memcmp(pkgno,PKGNO_DQJJHZ,3)||!memcmp(pkgno,PKGNO_SSJJ,3) ||!memcmp(pkgno,PKGNO_SSJJHZ,3))
	{
			if(!memcmp(rcpstat,"00",2))
			{
				strcpy(cTmpList,"成功");
			}	
			else if(!memcmp(rcpstat,"01",2))
			{
				strcpy(cTmpList,"帐号不符");
			}	
			else if(!memcmp(rcpstat,"02",2))
			{
				strcpy(cTmpList,"帐号户名不符");
			}	
			else if(!memcmp(rcpstat,"03",2))
			{
				strcpy(cTmpList,"账户余额不足支付");
			}	
			else if(!memcmp(rcpstat,"10",2))
			{
				strcpy(cTmpList,"账户密码错误");
			}	
			else if(!memcmp(rcpstat,"11",2))
			{
				strcpy(cTmpList,"帐号状态错误");
			}	
			else if(!memcmp(rcpstat,"20",2))
			{
				strcpy(cTmpList,"业务已撤销");
			}	
			else if(!memcmp(rcpstat,"90",2))
			{
				strcpy(cTmpList,"其他错误");
			}
			else 
				strcpy(cTmpList,"未被回执");	
	}
	else
				strcpy(cTmpList,"       ");

	vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;
}



