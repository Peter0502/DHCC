/*************************************************
* 文 件 名:  spJ091.c
* 功能描述： 小额信息类历史查询主程序
* 作    者:  ligl
* 完成日期： 2006年8月29日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_fd123.h"
#include "find_debug.h"

spJ091()
{
        int ret=0,i;
	struct lv_pkgreg_c lv_pkgreg;
	LV_FD123_AREA lv_fd123;
	char cKinbr[6];
	char cBr_no[6];/*机构号*/
	char cWt_date[9];/*委托日期*/
	char cKinbr2[6];
	char cBr_no2[6];/*机构号*/
	char cWt_date2[9];/*委托日期*/
	char cDate[9];
	char cPkgflag[2];/*标志*/
	char cPkgflag2[2];/*标志*/

	char cTxstat[2];/*交易状态*/
	char cPkgno[4];/*包类型*/
	char cPkgno1[4];
	char cPkgno12[4];

	char cPay_ac_no[33];/**/
	char cCash_ac_no[33];/**/
	long cMintxamt=0.0;/*最小金额*/
	long cMaxtxamt=0.0;/*最大金额*/
	char cFilename[100];/*文件名*/
	int ttlnum=0;	    /**读取的总条数**/
	FILE *fp;
	char wherelist[1024];  /**查询条件**/
	char Pkgnoname[11];
	char RcpStat[3];
	char RcpStatname[21];
	memset(RcpStatname ,0, sizeof(RcpStatname ));
	memset(RcpStat     ,0, sizeof(RcpStat ));
	memset(Pkgnoname   ,'\0', sizeof(Pkgnoname ));
	memset(cBr_no      ,'\0', sizeof(cBr_no ));
	memset(cKinbr      ,'\0', sizeof(cKinbr ));
	memset(cDate   	   ,'\0', sizeof(cDate));	
	memset(cWt_date    ,'\0', sizeof(cWt_date));	
	memset(cPkgflag    ,'\0', sizeof(cPkgflag));
	memset(cBr_no2      ,'\0', sizeof(cBr_no2 ));
	memset(cKinbr2      ,'\0', sizeof(cKinbr2 ));
	memset(cPkgno12   	   ,'\0', sizeof(cPkgno12));	
	memset(cWt_date2    ,'\0', sizeof(cWt_date2));	
	memset(cPkgflag2    ,'\0', sizeof(cPkgflag2));

	memset(cTxstat     ,'\0', sizeof(cTxstat ));
	memset(cPkgno      ,'\0', sizeof(cPkgno  ));
	memset(cPkgno1     ,'\0', sizeof(cPkgno1 ));
	memset(cFilename  ,'\0', sizeof(cFilename));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(&lv_fd123,0x00,sizeof(lv_fd123));
	memset(wherelist ,0,sizeof(wherelist));
	vtcp_log("spJ090开始！[%s][%d]",__FILE__,__LINE__);
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cKinbr);
	get_zd_data("0440",cWt_date);
	get_zd_data("0670",cPkgflag);
	get_zd_data("0770",cBr_no);
	get_zd_data("0230",cPkgno1);
	get_zd_long("0410",&cMintxamt);
	get_zd_long("0420",&cMaxtxamt);
	get_zd_data("123A",cPay_ac_no);
	get_zd_data("123B",cCash_ac_no);
	if(cWt_date[0]=='\0') /****日期为空时候也可以查找*******/
	{
	 	MEMCPY_DEBUG(cDate, "%", 1);
	}
	else
	{
	 	MEMCPY_DEBUG(cDate, cWt_date,sizeof(cDate)-1);
	}
	if(memcmp(cPkgno1,"000",3)==0)
	{
	 	MEMCPY_DEBUG(cPkgno, "%", 1);
	}
	 else
	{
	 	MEMCPY_DEBUG(cPkgno, cPkgno1,sizeof(cPkgno)-1);
	}
	/**组成查询条件**/
	if(memcmp(cKinbr, QS_BR_NO ,5)==0)
	{
		if(cBr_no[0]=='\0')
		{
			if(memcmp(cPkgno,"007",3)==0)
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%'",cDate);
				}
				else
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%' and lv_sts='%s'",cTxstat);
				}
			}
			else
			{
				
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%'",cDate,cPkgno);
				}
				else
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%' AND lv_sts='%s'",cDate,cPkgno,cTxstat);
				}
			}		
		}
		else
		{
			if(memcmp(cPkgno,"007",3)==0)
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%'",cBr_no,cDate);
				}
				else
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%' and lv_sts='%s'",cBr_no,cDate,cTxstat);
				}
			}
			else
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%'  pkgno like '%%%%%s%%%%'",cBr_no,cDate,cPkgno);
				}
				else
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%' AND stat='%s'",cBr_no,cDate,cPkgno,cTxstat);
				}
			}	
		}
	}
	else
	{
		if(memcmp(cBr_no,cKinbr,sizeof(cBr_no))!=0)
		{
			if(cBr_no[0]=='\0')
			{
				sprintf(acErrMsg," 交易行不能为空[%s][%d]",__FILE__,__LINE__);
				WRITEMSG
				goto ErrExit;				
			}
			else
			{
				sprintf(acErrMsg,"[%s][%d]非清算中心不能查询他行交易\n",__FILE__,__LINE__);
				WRITEMSG
				goto ErrExit;
			}	
	        }
	        else
	        {
	        	if(memcmp(cPkgno,"007",3)==0)
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no = '%s' and wt_date LIKE '%%%%%s%%%%'",cBr_no,cDate);
				}
				else
				{
					sprintf(wherelist,"br_no='%s' and wt_date LIKE '%%%%%s%%%%' and lv_sts='%s'",cBr_no,cDate,cTxstat);
				}
			}
			else
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no='%s' and wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%'",cBr_no,cDate,cPkgno);
				}
				else
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%' AND stat='%s'",cBr_no,cDate,cPkgno,cTxstat);
				}
			}		
	        }
	        
	}
	printf("[%s][%d]查询语句:\n%s\n",__FILE__,__LINE__,wherelist);
	/***处理收款帐号和付款帐号这个条件***/
	if(strlen(cPay_ac_no)!=0)
	{
		sprintf(wherelist+strlen(wherelist),"  and pay_ac_no='%s' ",cPay_ac_no);
	}
	printf("[%s][%d]查询语句:\n%s\n",__FILE__,__LINE__,wherelist);
	if(strlen(cCash_ac_no)!=0)
	{	
		sprintf(wherelist+strlen(wherelist),"  and cash_ac_no='%s' ",cCash_ac_no);
	}
	printf("[%s][%d]查询语句:\n%s\n",__FILE__,__LINE__,wherelist);
	/****************完毕******************/
	/*******处理输入的金额的取值范围******************************/
	if(cMintxamt!=0||cMaxtxamt!=0)
	{	
		sprintf(wherelist+strlen(wherelist)," and tx_amt between '%.2f' and '%.2f' ",cMintxamt,cMaxtxamt);
	}
		printf("[%s][%d]查询语句:\n%s\n",__FILE__,__LINE__,wherelist);
	/*开始查询*/
        ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply, wherelist);
        ERR_DEAL
        ttlnum=0;
        while(1)
        {
        	ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
        	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName(cFilename);
			fp = fopen(cFilename,"w");
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}          
			fprintf( fp,"~交  易  行|委托  日期|交易  序号|交易  金额|包  序  号|状   态|发起  行号|接收  行号|录入操作员|复核操作员|\n" );
		}
	   	if(lv_pkgreg.lv_sts[0]== 'H')
	   	{
			memset(RcpStat, 0, sizeof(RcpStat));
			memset(RcpStatname,0, sizeof(RcpStatname));
			if(lv_pkgreg.resp1[3] == 'I')
			{
				MEMCPY_DEBUG(RcpStat,lv_pkgreg.rcpstat,sizeof(lv_pkgreg.rcpstat)-1);
				pub_base_strpack_all(RcpStat);
				if(strlen(RcpStat)==0) 
				{
					MEMCPY_DEBUG(RcpStat,"11",2);
				}
				if(memcmp(RcpStat, "00", 2) == 0)
				{
	                		MEMCPY_DEBUG(RcpStatname,"成功", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "01", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"帐号不存在", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "02", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"账号户名不符", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "03", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"账户余额不足支付", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "10", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"户密码错误", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "11", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"账户状态错误", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "20", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"业务已撤销", sizeof(RcpStatname)-1);
				}else if(memcmp(RcpStat, "90", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"其他错误", sizeof(RcpStatname)-1);
				} 		
			}else
			{
				if(lv_pkgreg.resp1[3] == '0')
				{
					MEMCPY_DEBUG(RcpStatname,"录入取消", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '1')
				{
					MEMCPY_DEBUG(RcpStatname,"往账录入", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '2')
				{
					MEMCPY_DEBUG(RcpStatname,"往账复核挂帐", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '3')
				{
					MEMCPY_DEBUG(RcpStatname,"往账复核", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '4')
				{
					MEMCPY_DEBUG(RcpStatname,"往账发送", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '5')
				{
					MEMCPY_DEBUG(RcpStatname,"往账拒绝", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '6')
				{
					MEMCPY_DEBUG(RcpStatname,"往账轧差", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '7')
				{
					MEMCPY_DEBUG(RcpStatname,"往账排队", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '8')
				{
					MEMCPY_DEBUG(RcpStatname,"往账撤销等待", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '9')
				{
					MEMCPY_DEBUG(RcpStatname,"往账撤销", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'A')
				{
					MEMCPY_DEBUG(RcpStatname,"退汇等待", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'B')
				{
					MEMCPY_DEBUG(RcpStatname,"往账退回", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'C')
				{
					MEMCPY_DEBUG(RcpStatname,"往账冲正", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'D')
				{
					MEMCPY_DEBUG(RcpStatname,"往账止付", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'G')
				{
					MEMCPY_DEBUG(RcpStatname,"往账异常挂帐", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'H')
				{
					MEMCPY_DEBUG(RcpStatname,"临时接收不记帐", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'I')
				{
					MEMCPY_DEBUG(RcpStatname,"正式接收", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'J')
				{
					MEMCPY_DEBUG(RcpStatname,"全国押错", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'K')
				{
					MEMCPY_DEBUG(RcpStatname,"地方押错", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'L')
				{
					MEMCPY_DEBUG(RcpStatname,"全国押地方押都错", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'M')
				{
					MEMCPY_DEBUG(RcpStatname,"中心挂帐", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'N')
				{
					MEMCPY_DEBUG(RcpStatname,"支行挂帐", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'O')
				{
					MEMCPY_DEBUG(RcpStatname,"来账退回等待", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'P')
				{
					MEMCPY_DEBUG(RcpStatname,"来账退回", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'Q')
				{
					MEMCPY_DEBUG(RcpStatname,"同意未发送", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'R')
				{
					MEMCPY_DEBUG(RcpStatname,"同意已发送", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'S')
				{
					MEMCPY_DEBUG(RcpStatname,"不同意未发送", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'T')
				{
					MEMCPY_DEBUG(RcpStatname,"不同意已发送", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'U')
				{
					MEMCPY_DEBUG(RcpStatname,"来账已冲正", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'V')
				{
					MEMCPY_DEBUG(RcpStatname,"来账已止付", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'Z')
				{
					MEMCPY_DEBUG(RcpStatname,"来账异常挂帐", sizeof(RcpStatname)-1);						
				}
				
			}
			
		}
		else
		{
			if(lv_pkgreg.lv_sts[0]== '0')
			{
				MEMCPY_DEBUG(RcpStatname,"录入取消", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '1')
			{
				MEMCPY_DEBUG(RcpStatname,"往账录入", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '2')
			{
				MEMCPY_DEBUG(RcpStatname,"往账复核挂帐", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '3')
			{
				MEMCPY_DEBUG(RcpStatname,"往账复核", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '4')
			{
				MEMCPY_DEBUG(RcpStatname,"往账发送", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '5')
			{
				MEMCPY_DEBUG(RcpStatname,"往账拒绝", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '6')
			{
				MEMCPY_DEBUG(RcpStatname,"往账轧差", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '7')
			{
				MEMCPY_DEBUG(RcpStatname,"往账排队", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '8')
			{
				MEMCPY_DEBUG(RcpStatname,"往账撤销等待", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '9')
			{
				MEMCPY_DEBUG(RcpStatname,"往账撤销", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'A')
			{
				MEMCPY_DEBUG(RcpStatname,"退汇等待", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'B')
			{
				MEMCPY_DEBUG(RcpStatname,"往账退回", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'C')
			{
				MEMCPY_DEBUG(RcpStatname,"往账冲正", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'D')
			{
				MEMCPY_DEBUG(RcpStatname,"往账止付", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'G')
			{
				MEMCPY_DEBUG(RcpStatname,"往账异常挂帐", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'H')
			{
				MEMCPY_DEBUG(RcpStatname,"临时接收不记帐", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'I')
			{
				MEMCPY_DEBUG(RcpStatname,"正式接收", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'J')
			{
				MEMCPY_DEBUG(RcpStatname,"全国押错", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'K')
			{
				MEMCPY_DEBUG(RcpStatname,"地方押错", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'L')
			{
				MEMCPY_DEBUG(RcpStatname,"全国押地方押都错", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'M')
			{
				MEMCPY_DEBUG(RcpStatname,"中心挂帐", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'N')
			{
				MEMCPY_DEBUG(RcpStatname,"支行挂帐", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'O')
			{
				MEMCPY_DEBUG(RcpStatname,"来账退回等待", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'P')
			{
				MEMCPY_DEBUG(RcpStatname,"来账退回", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'Q')
			{
				MEMCPY_DEBUG(RcpStatname,"同意未发送", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'R')
			{
				MEMCPY_DEBUG(RcpStatname,"同意已发送", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'S')
			{
				MEMCPY_DEBUG(RcpStatname,"不同意未发送", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'T')
			{
				MEMCPY_DEBUG(RcpStatname,"不同意已发送", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'U')
			{
				MEMCPY_DEBUG(RcpStatname,"来账已冲正", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'V')
			{
				MEMCPY_DEBUG(RcpStatname,"来账已止付", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'Z')
			{
				MEMCPY_DEBUG(RcpStatname,"来账异常挂帐", sizeof(RcpStatname)-1);						
			}
		}		
		fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,lv_pkgreg.wt_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,lv_pkgreg.packid,RcpStatname,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.input_tel,lv_pkgreg.check_tel);
		ttlnum++;
        }
	ret=Lv_pkgreg_Clo_Sel();
	ERR_DEAL
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

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}
