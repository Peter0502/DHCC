/***************************************************************
* 文 件 名:     spJ012.c
* 功能描述：    银行汇票信息查询(模糊查询和精确查询)			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18	09:20 
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

/*****签发日期放入字段FD44中,签发行不用传送，只需要根据类别来对PAY_OPN_BR_NO不同的值而已,票据号码放入FD38字段中,汇票状态放入字段 $FD72中****/
	

spJ012()
{
	int ret = 0;
	char  filename[40];
	FILE *fp;
	HV_NOTPAY_AREA hv_notpayin;
	struct hv_poinfo_c hv_poinfo;
	HV_FD123_AREA  fd123;
	char	cWt_day[9];
	char	cPo_no[9];
	char	cPo_sts[2];
	char  cBank_type[2];
	char	cPay_br_no[13];	
	char  cWherelist[200];
	char  cTmplist[200];
	int   num  =0;
	char  cStat[20];
	char  cPoType[20];
	char  cSign_date[9];
	int   iPo_no = 0;
	int   iTotcnt=0;
	double dTotamt=0.0;

	memset(filename,    0x00,   sizeof(filename));
	memset(cWt_day,     0x00,   sizeof(cWt_day));
	memset(cPo_no,      0x00,   sizeof(cPo_no));
	memset(cPo_sts,     0x00,   sizeof(cPo_sts));
	memset(cBank_type,  0x00,   sizeof(cBank_type));
	memset(cPay_br_no,  0x00,   sizeof(cPay_br_no));
	memset(cWherelist,  0x00,   sizeof(cWherelist));
	memset(cTmplist,    0x00,   sizeof(cTmplist));
	memset(cStat,       0x00,   sizeof(cStat));
	memset(cPoType,     0x00,   sizeof(cPoType));
	memset(&fd123,      0x00,   sizeof(fd123));
	memset(cSign_date,	0x00,	sizeof(cSign_date));
	
	iTotcnt=0;
	dTotamt=0.0;
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));

    /******************************
     *PART 1   获取前台传入的数据 *
     ******************************/

	/***得到大额支付业务8583数据***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	/*****获取TAG值*********/
	get_zd_data("0450",cWt_day);	/**签发日期***/
	zip_space(cWt_day);
	get_zd_data("0720",cPo_sts);	/**汇票状态***/
	get_zd_data("0710",cBank_type);	/**行别标志***/
	get_fd_data("1230",(char *)&fd123);/**获取123域内容**/
	MEMCPY_DEBUG(cPo_no,fd123.po_no,sizeof(cPo_no)-1);

	/********附加域先不处理,先给赋值看看*******/
	if(cBank_type[0]=='0')/****如果为空说明是本行***/
	{
		vtcp_log("++++++tx_br_no==[%s]++++++",hv_notpayin.F_tx_br_no);
		GetOr_br_noByBr_no(hv_notpayin.F_tx_br_no,cPay_br_no);
	}
	else
	{
		MEMCPY_DEBUG(cPay_br_no,hv_notpayin.F_ac_br_no,sizeof(cPay_br_no)-1);/*签发行号*/
	}
	vtcp_log("++++++++++++++++cPay_br_no==[%s]+++++++++++++",cPay_br_no);
	

	/***************************
	*PART 2   操作数据库      *
	***************************/

	/****组合where条件*********/
	vtcp_log("++++++++cpo_no===[%s]\n",fd123.po_no);
	iPo_no = apatoi(cPo_no,sizeof(cPo_no)-1);
 	vtcp_log("[%s][%d]liuyue====[%d]\n",__FILE__,__LINE__,iPo_no);
	if(iPo_no!=0) /** 查询条件中,该条件可以为空****/
	{
		sprintf(cTmplist,"po_no = '%.8s' and   ",fd123.po_no);
		strcat(cWherelist,cTmplist);
	}
	pub_base_strpack(cPay_br_no);
	if(strlen(cPay_br_no)!=0 &&!MEMCPY_DEBUG(g_pub_tx.tx_br_no, QS_BR_NO ,5))
	{
		sprintf(cTmplist, "pay_br_no = '%s' and ",cPay_br_no);
		strcat(cWherelist,cTmplist);
	}

	pub_base_strpack(cWt_day);
	if(strlen(cWt_day)==8)
	{
		vtcp_log("%s,%d 委托日期===[%s]",__FILE__,__LINE__,cWt_day);
		sprintf(cTmplist, " (reg_date ='%s' or sign_date='%s' )  and ",cWt_day,cWt_day);
		strcat(cWherelist,cTmplist);
	}

	if(cPo_sts[0]!='Z')
	{	
		sprintf(cTmplist, "Po_sts='%c' and  ",cPo_sts[0]);
		strcat(cWherelist,cTmplist);
	}


	sprintf(cTmplist, "bank_type='%c' ",cBank_type[0]);
	strcat(cWherelist,cTmplist);

	sprintf(cTmplist,"order by br_no , sign_date desc ,Po_no desc ");
	strcat(cWherelist,cTmplist);
	
	ret = Hv_poinfo_Dec_Sel(g_pub_tx.reply,cWherelist);	
	if(ret)
	{
		sprintf(acErrMsg,"查找汇票信息表错误号码＝[%s] 行号==[%s]ret=[%d]\n",hv_poinfo.po_no,hv_poinfo.pay_br_no);
		WRITEMSG
		return (-1);
	}
	num = 0;
	while(1)
	{
		vtcp_log("++++++++[%d]+++++++\n",__LINE__);
		ret = Hv_poinfo_Fet_Sel(&hv_poinfo,g_pub_tx.reply);
		if(ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"取游标错误 ret==[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret ==100)
		{
			if(num!=0)
			{
			fprintf(fp,"%s|%s|%s|%.2f|%s|%s|%s|%d|%s|\n", "总计","        ","金额  ",dTotamt,"","","笔数",iTotcnt,"");
			num++;	
			}
			break;
		}
		if(!num)
		{
			pub_base_AllDwnFilName( filename);
			fp = fopen(filename,"w");
			if(fp==NULL)
			{
				sprintf(acErrMsg,"open file [%s] failed!\n",filename);
				WRITEMSG
				strcpy(g_pub_tx.reply,"S047");
				goto ErrExit;	
			}
			/****显示标题****/
			fprintf(fp,"~@签发日期  |@签发行号      |@汇票号码    |$汇票金额        |@汇票种类  |@汇票状态    |@付款帐号                      |@付款姓名          |@附言|\n" );
		}/*****END if(!num)****/

			/*******状态转化******/
			iNum2Stat(hv_poinfo.po_sts[0],cStat);
			/*******转换汇票种类*****/
			iNum2PoType(hv_poinfo.po_ind[0],cPoType);
			if(hv_poinfo.sign_date==0)
				MEMCPY_DEBUG(cSign_date,"        ",8);
			else 	
				sprintf(cSign_date,"%08d",hv_poinfo.sign_date);

			vtcp_log("++++++++[%d]+++++++\n",__LINE__);
			fprintf(fp,"%s|%s|%s|%.2f|%s|%s|%s|%s|%s|\n", cSign_date,hv_poinfo.pay_br_no,hv_poinfo.po_no,hv_poinfo.po_bal,cPoType,cStat,hv_poinfo.pay_ac_no,hv_poinfo.pay_name,hv_poinfo.brf);
			num++;	
			iTotcnt++;
			dTotamt+=hv_poinfo.po_bal;
		}/***end while (1)*******/
			ret = Hv_poinfo_Clo_Sel();
			if(ret)
			{
				sprintf(acErrMsg,"sql error\n");
				WRITEMSG
				goto ErrExit;	
			}
			if(!num)
			{
				strcpy(g_pub_tx.reply,"S049");
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data(DC_FILE_SND,"1");
			}

    /**********************
     *PART 4 结尾工作     *
     **********************/
	 
	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	else 
		goto OkExit;
	/*** 输出变量 ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}





int iNum2Stat(char ch,char *cTmpList)
{
		if(ch=='0')
		{
			strcpy(cTmpList,"登记");
		}
		else if(ch=='1')
		{
			strcpy(cTmpList,"签发");
		}
	 	else if(ch=='2')
		{
			strcpy(cTmpList,"资金移存记帐");
		}
 	    else if(ch=='3')
		{
			strcpy(cTmpList,"申请兑付");
		}
		else if(ch=='4')
		{
			strcpy(cTmpList,"汇票兑付");
		}
		else if(ch=='5')
		{
			strcpy(cTmpList,"全额结清");
		}
		else if(ch=='6')
		{
			strcpy(cTmpList,"部分退回");
		}
		else if(ch=='7')
		{
			strcpy(cTmpList,"未用退回");
		}
		else if(ch=='8')
		{
			strcpy(cTmpList,"汇票解付");
		}
		else if(ch=='9')
		{
			strcpy(cTmpList,"汇票签发取消");
		}
		else if(ch=='A')
		{
			strcpy(cTmpList,"解付取消");
		}
		else if(ch=='B')
		{
			strcpy(cTmpList,"发送");
		}
		else if(ch=='C')
		{
			strcpy(cTmpList,"申请兑付取消");
		}
		else if(ch=='D')
		{
			strcpy(cTmpList,"汇票登记取消");
		}
		else if(ch=='E')
		{
		strcpy(cTmpList,"挂失");
		}
		else if(ch=='F')
		{
			strcpy(cTmpList,"解挂失");
		}
		else if(ch=='G')
		{
			strcpy(cTmpList,"逾期未用退回");
		}
		else if(ch=='H')
		{
			strcpy(cTmpList,"未用解付");
		}
		else if(ch=='I')
		{
			strcpy(cTmpList,"未用退回冲帐");
		}
		else if(ch=='J')
		{
			strcpy(cTmpList,"汇票申请注销");
		}
		else if(ch=='K')
		{
			strcpy(cTmpList,"申请退回");
		}
		else
			strcpy(cTmpList,"未知状态");
		return 0;
	}
int iNum2PoType(char ch,char *cTmpList)
{
		if(ch=='0')
		{	
			strcpy(cTmpList,"可转让汇票");
		}
		else if(ch=='1')
		{	
			strcpy(cTmpList,"不可转让汇票");
		}
	    	else if(ch=='2')
		{	
			strcpy(cTmpList,"现金汇票");
		}
		else
			strcpy(cTmpList,"未知种类");


		return (0);
}
	
