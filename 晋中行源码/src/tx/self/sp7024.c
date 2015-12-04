/**********************************************************************************
交易名称：主机对账交易sp7024.c	 
功能描述：只处理转账类，不处理现金类
对账文件：hvps_check_对账日期_流水号.txt
文件格式：
	编号|唯一标识|系统编号|对账日期|交易类型|委托日期|支付交易序号|平台日期|平台流水|发起行行号|来往标识|控制标识|交易金额|借贷标志|主账号|主账户名|第二账号|凭证种类|凭证号码|交易机构|操作员号|核心记账日期|核心记账流水|对账标识|批量标识|手续费|手续费标识|手续费出处|手续费收入科目|

产生的对帐结果文件名：txt目录下 hvps_check_对账日期_流水号.ret
对账结果文件格式：
	明细编号|唯一标识|系统编号|交易类型|对账标识|主机对账结果|核心记账日期|核心记账流水|挂账账号|挂账机构|挂账原因|

insert into tx_def values('7024','批量对帐','10000000000000000000000000000000000000000000000000000000000000000001000000000000010000000000000000000000000000000000000000000100','1','1','2','3','');
insert into tx_flow_def values('7024','0','7024','#$');
insert into tx_sub_def values('7024','批量对帐','sp7024','0','0');
***********************************************************************************/

#define DEWZGD "41101"
#define XEWZGD "41102"
#define DELZGD "41201"
#define XELZGD "41202"

#include <errno.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "chnl_batch_acct_c.h"
#include "chnl_single_cnaps_c.h"
#include "chnl_compare_acct_c.h"

static int compare_info_acct(struct chnl_compare_acct_c stCompare,char *gz_br_no,char *ac_no,char *gz_reason);
static int compare_exit_acct(struct chnl_compare_acct_c stCompare);

sp7024()
{
	int i = 0;
	int	j = 0;
	int	k = 0;
	int	l = 0;
	int iRet = 0;
	long	iCount = 0;
	long	pt_date = 0;
	long	lCom_swno = 0;
	long	lTot_num = 0;	/*登记对账登记薄的总笔数*/
	long	lFact_num = 0;	/*对账总笔数*/
	long	lCount_num = 0;	/*需对账总笔数*/
	char	sBrf[21] = {0};
	char	sCmd[50] = {0};
	char	sSeqno[11];
	char	sAc_no[33] = {0};	/*主账号*/
	char	sGz_br_no[6] = {0};	/*挂账机构*/
	char	sGz_reason[2] = {0};	/*挂账原因*/
	char	sSysCode[5] = {0};
	char	sTmpstr[1024] = {0};
	char	sCheckDate[9] = {0};
	char	sCom_result[11] = {0};
	char	sFilename[60] = {0};
	char	sAll_filename[248] = {0};/***全路径**/
	char	sFilename_rt[400] = {0};
	char	sFilename_ret[400] = {0};
	char	sFldstr[30][100] = {0};
	struct com_sys_parm_c com_sys_parm = {0};
	struct chnl_batch_acct_c stBatch = {0};	/*批量记账登记簿*/
	struct chnl_single_cnaps_c stSingle = {0};	/*单笔记账登记簿*/
	struct chnl_compare_acct_c stCompare = {0};	/*对账登记簿*/
	struct chnl_compare_acct_c stCompareQ = {0};	/*对账记账登记簿*/
	char *p=NULL;
	FILE *fp=NULL;
	FILE *new_fp=NULL;
	
	pub_base_sysinit();
	
	vtcp_log("[%s],[%d],日期、流水,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
	set_zd_long("0460",g_pub_tx.tx_date);
	
	/********获取平台数据***************/
	get_zd_data("0100",sSysCode); 	/*系统编号*/
	get_zd_long("0440",&iCount);	/****对账日期*****/
	sprintf(sCheckDate,"%d",iCount);
	vtcp_log("[%s][%d]对账日期 sCheckDate=[%s]",__FILE__,__LINE__,sCheckDate);
	get_zd_long("0450",&pt_date);/****平台日期*****/
	vtcp_log("[%s][%d]平台日期 pt_date=[%d]",__FILE__,__LINE__,pt_date);
	get_zd_long("0480",&lCount_num);/****总笔数*****/
	vtcp_log("[%s][%d]总笔数 lCount_num=[%d]",__FILE__,__LINE__,lCount_num);
	get_zd_long("0520", &lCom_swno);
	vtcp_log("[%s][%d]平台流水 lCom_swno=[%d]",__FILE__,__LINE__,lCom_swno);
	get_zd_data("0810",sBrf);
	pub_base_strpack(sBrf);
	get_zd_data("0830",sFilename);/**记账文件名***/
	
	iRet = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(iRet){
		sprintf(acErrMsg," 查询系统日期错误 ");
		WRITEMSG
		goto ErrExit;
	}
	/*if(pt_date != com_sys_parm.sys_date)
	{
		sprintf(acErrMsg," 对账日期与核心日期不相等!!平台【%ld】 核心【%ld】 ",pt_date,com_sys_parm.sys_date);
		WRITEMSG
		goto ErrExit;
	}*/
	/******读文件，检查文件是否完整，将文件内容插入对账登记薄*******/
	pub_base_strpack(sFilename);
	pub_base_strpack(sAll_filename);
	sprintf(sAll_filename,"%s/txt/%s",getenv("HOME"),sFilename);
	vtcp_log("sFilename[%s][%s][%d]",sFilename,__FILE__,__LINE__);
	vtcp_log("sAll_filename[%s][%s][%d]",sAll_filename,__FILE__,__LINE__);
	fp = fopen(sAll_filename,"r");
	if (fp == NULL)
	{
		sprintf(acErrMsg," 打开文件出错 [%s]  ",sFilename);
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	while (1){
		memset(sTmpstr,0x0,sizeof(sTmpstr));
		p = fgets(sTmpstr,1024,fp);/*取出一条记录*/
		if (feof(fp))
		{
			vtcp_log("[%s],[%d]文件数据已取完了！",__FILE__,__LINE__);
			break;
		}
		pub_base_strpack_all(sTmpstr);
		vtcp_log("wkwk==========1");
		memset(sFldstr,0x0,sizeof(sFldstr));
		for (i = 0; i < 30; i++)
		{
			iRet = pub_base_cut_str(sTmpstr,sFldstr[i],"|",i+1);
			vtcp_log("[%d] [%s]",i+1,sFldstr[i]);
			pub_base_strpack_all(sFldstr[i]);
		}
		/**********************************************************
		sFldstr[0] 明细编号          sFldstr[15] 主账号户名
		sFldstr[1] 唯一标识          sFldstr[16] 第二账号
		sFldstr[2] 系统编号          sFldstr[17] 凭证种类
		sFldstr[3] 对账日期          sFldstr[18] 凭证号码
		sFldstr[4] 交易类型          sFldstr[19] 交易机构
		sFldstr[5] 委托日期          sFldstr[20] 操作员号
		sFldstr[6] 支付序号          sFldstr[21] 核心日期
		sFldstr[7] 平台日期          sFldstr[22] 核心流水
		sFldstr[8] 平台流水          sFldstr[23] 对账标示  暂时到这
		sFldstr[9] 发起行行号        sFldstr[24] 批量标识
		sFldstr[10] 往来标示         sFldstr[25] 收费标识
		sFldstr[11] 交易金额         sFldstr[26] 手续费标识
		sFldstr[12] 控制标识         sFldstr[27] 手续费金额
		sFldstr[13] 借贷标示         sFldstr[28] 手续费出处
		sFldstr[14] 主账号           sFldstr[29] 手续费收入科目
		************************************************************/
		iRet = Chnl_compare_acct_Dec_Upd(g_pub_tx.reply,"un_marking = '%s' and "
			"syscode = '%s' and com_date = '%s'",sFldstr[1],sFldstr[2],sFldstr[3]);
		if (iRet){
			vtcp_log("[%s],[%d]定义游标出错,iRet=[%d]",__FILE__,__LINE__,iRet);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		iRet = Chnl_compare_acct_Fet_Upd(&stCompare,g_pub_tx.reply);
		if (iRet == 100 || iRet == 1403){
			vtcp_log("[%s],[%d]数据库没有记录则登记对账登记薄,iRet=【%d】",__FILE__,__LINE__,iRet);
			memset(&stCompare,0x00,sizeof(stCompare));
			stCompare.detail_no = atoi(sFldstr[0]);
			memcpy(stCompare.un_marking,sFldstr[1],strlen(sFldstr[1]));
			memcpy(stCompare.syscode,sFldstr[2],strlen(sFldstr[2]));
			memcpy(stCompare.com_date,sFldstr[3],strlen(sFldstr[3]));
			memcpy(stCompare.code_type,sFldstr[4],strlen(sFldstr[4]));
			memcpy(stCompare.wt_date,sFldstr[5],strlen(sFldstr[5]));
			stCompare.orderno = atoi(sFldstr[6]);
			stCompare.pt_date = atoi(sFldstr[7]);
			stCompare.pt_trace_no = atoi(sFldstr[8]);
			memcpy(stCompare.or_br_no,sFldstr[9],strlen(sFldstr[9]));
			memcpy(stCompare.sndrcvflag,sFldstr[10],strlen(sFldstr[10]));
			stCompare.tx_amt = strtod(sFldstr[11],NULL)/100;
			vtcp_log("stCompare.tx_amt的值：[%.2f][%s][%d]",stCompare.tx_amt,__FILE__,__LINE__);
			memcpy(stCompare.con_marking,sFldstr[12],strlen(sFldstr[12]));
			memcpy(stCompare.bor_lend,sFldstr[13],strlen(sFldstr[13]));
			memcpy(stCompare.main_acc_no,sFldstr[14],strlen(sFldstr[14]));
			memcpy(stCompare.main_acc_nm,sFldstr[15],strlen(sFldstr[15]));
			memcpy(stCompare.snd_acc_no,sFldstr[16],strlen(sFldstr[16]));
			memcpy(stCompare.note_type,sFldstr[17],strlen(sFldstr[17]));
			memcpy(stCompare.note_no,sFldstr[18],strlen(sFldstr[18]));
			memcpy(stCompare.tx_br_no,sFldstr[19],strlen(sFldstr[19]));
			memcpy(stCompare.teller,sFldstr[20],strlen(sFldstr[20]));
			stCompare.tx_date = atoi(sFldstr[21]);
			stCompare.trace_no = atoi(sFldstr[22]);
			memcpy(stCompare.com_stat,sFldstr[23],strlen(sFldstr[23]));
			memcpy(stCompare.batch_detail,sFldstr[24],strlen(sFldstr[24]));
			memcpy(stCompare.chrgflag,sFldstr[25],strlen(sFldstr[25]));
			memcpy(stCompare.chrg_ind,sFldstr[26],strlen(sFldstr[26]));
			stCompare.chrg_amt = strtod(sFldstr[27],NULL)/100;
			vtcp_log("stCompare.chrg_amt的值：[%.2f][%s][%d]",stCompare.chrg_amt,__FILE__,__LINE__);
			memcpy(stCompare.chrgacno,sFldstr[28],strlen(sFldstr[28]));
			memcpy(stCompare.feeaccno,sFldstr[29],strlen(sFldstr[29]));
			stCompare.com_pt_date = pt_date;
			stCompare.com_swno = lCom_swno;
			
			iRet = Chnl_compare_acct_Ins(stCompare,g_pub_tx.reply);
			if (iRet){
				Chnl_compare_acct_Debug(&stCompare);
				sprintf(acErrMsg,"插入对账登记簿出错[%d]",iRet);
				WRITEMSG
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			vtcp_log("[%s],[%d]第,lTot_num=【%d】笔登记完成！",__FILE__,__LINE__,lTot_num);
			
		}else if (iRet){
			sprintf(acErrMsg,"查询Chnl_compare_acct表出错【%d】",iRet);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			Chnl_compare_acct_Clo_Upd();
			goto ErrExit;
		}else{
			vtcp_log("[%s],[%d]数据库有记录则更新对账登记薄,iRet=【%d】",__FILE__,__LINE__,iRet);
			memset(&stCompare,0x00,sizeof(stCompare));
			/*stCompare.detail_no = atoi(sFldstr[0]);*/
			memcpy(stCompare.un_marking,sFldstr[1],strlen(sFldstr[1]));
			memcpy(stCompare.syscode,sFldstr[2],strlen(sFldstr[2]));
			memcpy(stCompare.com_date,sFldstr[3],strlen(sFldstr[3]));
			memcpy(stCompare.code_type,sFldstr[4],strlen(sFldstr[4]));
			memcpy(stCompare.wt_date,sFldstr[5],strlen(sFldstr[5]));
			stCompare.orderno = atoi(sFldstr[6]);
			stCompare.pt_date = atoi(sFldstr[7]);
			stCompare.pt_trace_no = atoi(sFldstr[8]);
			memcpy(stCompare.or_br_no,sFldstr[9],strlen(sFldstr[9]));
			memcpy(stCompare.sndrcvflag,sFldstr[10],strlen(sFldstr[10]));
			stCompare.tx_amt = strtod(sFldstr[11],NULL)/100;
			vtcp_log("stCompare.tx_amt的值：[%.2f][%s][%d]",stCompare.tx_amt,__FILE__,__LINE__);
			memcpy(stCompare.con_marking,sFldstr[12],strlen(sFldstr[12]));
			memcpy(stCompare.bor_lend,sFldstr[13],strlen(sFldstr[13]));
			memcpy(stCompare.main_acc_no,sFldstr[14],strlen(sFldstr[14]));
			memcpy(stCompare.main_acc_nm,sFldstr[15],strlen(sFldstr[15]));
			memcpy(stCompare.snd_acc_no,sFldstr[16],strlen(sFldstr[16]));
			memcpy(stCompare.note_type,sFldstr[17],strlen(sFldstr[17]));
			memcpy(stCompare.note_no,sFldstr[18],strlen(sFldstr[18]));
			memcpy(stCompare.tx_br_no,sFldstr[19],strlen(sFldstr[19]));
			memcpy(stCompare.teller,sFldstr[20],strlen(sFldstr[20]));
			stCompare.tx_date = atoi(sFldstr[21]);
			stCompare.trace_no = atoi(sFldstr[22]);
			memcpy(stCompare.com_stat,sFldstr[23],strlen(sFldstr[23]));
			memcpy(stCompare.batch_detail,sFldstr[24],strlen(sFldstr[24]));
			memcpy(stCompare.chrgflag,sFldstr[25],strlen(sFldstr[25]));
			memcpy(stCompare.chrg_ind,sFldstr[26],strlen(sFldstr[26]));
			stCompare.chrg_amt = strtod(sFldstr[27],NULL)/100;
			vtcp_log("stCompare.chrg_amt的值：[%.2f][%s][%d]",stCompare.chrg_amt,__FILE__,__LINE__);
			memcpy(stCompare.chrgacno,sFldstr[28],strlen(sFldstr[28]));
			memcpy(stCompare.feeaccno,sFldstr[29],strlen(sFldstr[29]));
			stCompare.com_pt_date = pt_date;
			stCompare.com_swno = lCom_swno;
			
			iRet = Chnl_compare_acct_Upd_Upd(stCompare,g_pub_tx.reply);
			if (iRet){
        		Chnl_compare_acct_Debug(&stCompare);
				sprintf(acErrMsg,"插入对账登记簿出错[%d]",iRet);
				WRITEMSG
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			vtcp_log("[%s],[%d]第,lTot_num=【%d】笔更新完成！",__FILE__,__LINE__,lTot_num);
		}
		Chnl_compare_acct_Clo_Upd();
		lTot_num ++;
		vtcp_log("[%s],[%d]第,lTot_num=【%d】笔登记完成！",__FILE__,__LINE__,lTot_num);
		/*continue;*/
	}
	fclose(fp);
	
	vtcp_log("[%s][%d]sFilename的值：[%s],",__FILE__,__LINE__,sFilename);
	/******取返回文件名序号********/
	for ( i = 0; i < strlen(sFilename); i++ )
	{
		if(sFilename[i] == '_') 
		{
			j++;
			if(j == 3) k=i+1;
		}
		if(sFilename[i] == '.')
		{
			l = i + 1;
		}
	}
	vtcp_log("I的值：[%d][%s][%d]",i,__FILE__,__LINE__);
	vtcp_log("J的值：[%d][%s][%d]",j,__FILE__,__LINE__);
	vtcp_log("K的值：[%d][%s][%d]",k,__FILE__,__LINE__);
	vtcp_log("L的值：[%d][%s][%d]",l,__FILE__,__LINE__);
	memset(sSeqno,0x00,sizeof(sSeqno));
	memcpy(sSeqno,sFilename + k,l - 1 - k);
	vtcp_log("[%s][%d]cSeq的值：[%s]",__FILE__,__LINE__,sSeqno);
	
	/*****生成对账结果文件**********/
	sprintf(sFilename_ret,"%s/txt/CNAPS2_check_%ld_%s.ret",getenv("HOME"),
		com_sys_parm.sys_date,sSeqno);
	sprintf(sFilename_rt,"CNAPS2_check_%ld_%s.ret",com_sys_parm.sys_date,sSeqno);
	vtcp_log("[%s][%d]sFilename_ret=[%s]",__FILE__,__LINE__,sFilename_ret);
	vtcp_log("[%s][%d]sFilename_rt=[%s]",__FILE__,__LINE__,sFilename_rt);
	new_fp = fopen(sFilename_ret,"w");
	if (new_fp == NULL)
	{
		vtcp_log("[%s][%d]打开文件[%s]错",__FILE__,__LINE__,sFilename_rt);
		strcpy(g_pub_tx.reply,"P157");
		goto ErrExit;
	}
	iRet = Chnl_compare_acct_Dec_Upd(g_pub_tx.reply,"syscode='%s' and com_date='%s'"
		" and com_pt_date=%d and (com_flag = '4' or com_flag is null)",sSysCode,sCheckDate,pt_date);
	if (iRet){
		vtcp_log("定义游标出错,%d",iRet);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	while (1){
		memset(&g_pub_tx,0x00,sizeof(g_pub_tx));
		pub_base_sysinit();
		memset(sCom_result,0x00,sizeof(sCom_result));
		memset(&stCompareQ,0x00,sizeof(struct chnl_compare_acct_c));
		iRet = Chnl_compare_acct_Fet_Upd(&stCompareQ,g_pub_tx.reply);
		if (iRet){
			if (iRet == 100 || iRet == 1403){
				vtcp_log("对账登记薄中没有该笔记录,%d",iRet);
				if (lFact_num != 0){
					strcpy(g_pub_tx.reply,"0000");
				}
				break;
			}else{
				vtcp_log("fet出错,%d",iRet);
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
		}
		if (stCompareQ.batch_detail[0]=='0'){
			iRet = Chnl_single_cnaps_Dec_Upd(g_pub_tx.reply,"chnl_type='3' and "
				"bbmk='%s' and pt_date=%d and pt_trace_no=%d ",
				stCompareQ.syscode,stCompareQ.pt_date,stCompareQ.pt_trace_no);
			if (iRet){
				vtcp_log("定义游标出错,%d",iRet);
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			iRet = Chnl_single_cnaps_Fet_Upd(&stSingle,g_pub_tx.reply);
			if (iRet == 100 || iRet == 1403){
				/*Chnl_single_cnaps_Clo_Upd();*/
				if (stCompareQ.com_stat[0] == '2'){
					vtcp_log("单笔记账登记薄无记录，做补账！com_stat[%s]",stCompareQ.com_stat);
					/*若为补账对账，核心没有记录则没有记账，需做补账处理，接口未写。*/
					iRet = compare_info_acct(stCompareQ,sGz_br_no,sAc_no,sGz_reason);
					if (iRet){
						vtcp_log("自动补账错误,【%d】",iRet);
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]补账失败！",__FILE__,__LINE__);
					}else{
						vtcp_log("[%s][%d]cAc_no=[%s],cGz_br_no=[%s],cGz_reason=[%s]",
							__FILE__,__LINE__,sAc_no,sGz_br_no,sGz_reason);
						if (strlen(sGz_reason) != 0){
							stCompareQ.com_flag[0] = '2';
							strcpy(stCompareQ.gz_br_no,sGz_br_no);
							strcpy(stCompareQ.gz_acct_no,sAc_no);
							strcpy(stCompareQ.gz_reasen,sGz_reason);
							strcpy(sCom_result,"挂账补账");
							stSingle.sts[0]='2';
							vtcp_log("[%s][%d]挂账补账！",__FILE__,__LINE__);
						}else{
							stCompareQ.com_flag[0] = '1';
							strcpy(sCom_result,"正常补账");
							stSingle.sts[0]='1';
							vtcp_log("[%s][%d]正常补账！",__FILE__,__LINE__);
						}
						stCompareQ.tx_date = g_pub_tx.tx_date;
						stCompareQ.trace_no = g_pub_tx.trace_no;
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
						vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
						vtcp_log("[%s][%d]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
						vtcp_log("[%s][%d]补账写文件完成！",__FILE__,__LINE__);
					}
				}else{
					vtcp_log("单笔记账登记薄中没有该笔记录,%d",iRet);
					stCompareQ.com_flag[0] = '4';
					strcpy(sCom_result,"对账错");
					iWriteFile(new_fp,stCompareQ);
					vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
				}
				strcpy(g_pub_tx.reply,"0000");
			}else if (iRet){
				vtcp_log("fet出错,%d",iRet);
				Chnl_single_cnaps_Clo_Upd();
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}else{/*主机对账结果:0-对账平  1-正常补账 2-挂账补账 3-已冲正 4-对账错*/
				if (stCompareQ.com_stat[0]=='0'){/*记账对账*/
					if (stSingle.sts[0]=='0'||stSingle.sts[0]=='4'){
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0] = '0';
						strcpy(sCom_result,"对账平");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账平,请继续!",__FILE__,__LINE__);
					}
				}else if(stCompareQ.com_stat[0]=='1'){/*冲正对账*/
					if (!memcmp(stSingle.sts,"4",1)){
						stCompareQ.com_flag[0] = '0';
						strcpy(sCom_result,"对账平");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账平,请继续!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
					}
				}else if (stCompareQ.com_stat[0]=='2'){/*补账对账*/
					if (stSingle.sts[0]=='1'||stSingle.sts[0]=='2'||
						stSingle.sts[0]=='3'||stSingle.sts[0]=='5'||
						stSingle.sts[0]=='6')
					{
						stCompareQ.com_flag[0] = '1';
						strcpy(sCom_result,"正常补账");
						iWriteFile(new_fp,stCompareQ);
						stCompareQ.tx_date = stSingle.tx_date;
						stCompareQ.trace_no = stSingle.trace_no;
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
						vtcp_log("[%s][%d]已记账,请检查!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0] = '4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
					}
				}else if (!memcmp(stCompareQ.com_stat,"3",1)){/*冲正对账*/
					if (!memcmp(stSingle.sts,"4",1)){
						stCompareQ.com_flag[0] = '0';
						strcpy(sCom_result,"对账平");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]已冲账,请检查!",__FILE__,__LINE__);
					}else{/*需要核心做冲正操作，接口为开发。*/
						iRet = compare_exit_acct(stCompareQ);
						if (iRet){
							vtcp_log("自动冲账错误,【%d】",iRet);
							stCompareQ.com_flag[0] = '4';
							strcpy(sCom_result,"冲账失败");
							iWriteFile(new_fp,stCompareQ);
							vtcp_log("[%s][%d]自动冲账失败,请检查!",__FILE__,__LINE__);
							/**add by zhaoyue 20140714 为了账务的一致性，报错退出**/
							goto ErrExit;
							/**add by zhaoyue 20140714 end**/
						}else{
							stCompareQ.com_flag[0] = '3';
							strcpy(sCom_result,"对账冲账");
							stCompareQ.tx_date =0;
							stCompareQ.trace_no =0;
							stCompareQ.tx_date = g_pub_tx.tx_date;
							stCompareQ.trace_no = g_pub_tx.trace_no;
							iWriteFile(new_fp,stCompareQ);
							stSingle.sts[0]='4';
							vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
							vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
							vtcp_log("[%s][%d]冲账成功,请继续!",__FILE__,__LINE__);
						}
					}
				}else{
					vtcp_log("平台的对账标识不正确，com_stat=【%s】",stCompareQ.com_stat);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				memcpy(stSingle.dz_ind,"1",1);
				vtcp_log("对账状态,Dz_ind=【%s】",stSingle.dz_ind);
				strcpy(stSingle.fill1,sCom_result);
				vtcp_log("第【%d】笔,sCom_result=【%s】com_stat=【%s】",
				lFact_num,sCom_result,stCompareQ.com_stat);
				
				iRet = Chnl_single_cnaps_Upd_Upd(stSingle,g_pub_tx.reply);
				if (iRet)
				{
					Chnl_single_cnaps_Clo_Upd();
					Chnl_compare_acct_Clo_Upd();
					sprintf(acErrMsg,"更新数据库失败！[%d]",iRet);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P102");
					goto ErrExit;
				}
				vtcp_log("[%s],[%d],更新单笔记账薄完成，第[%d]次",__FILE__,__LINE__,lFact_num);
			}
			Chnl_single_cnaps_Clo_Upd();
			vtcp_log("%s,%d,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			vtcp_log("[%s],[%d]tx_date=[%d],trace_no=[%d]",__FILE__,__LINE__,stCompareQ.tx_date,stCompareQ.trace_no);
			vtcp_log("[%s],[%d],对账标识:[%s]",__FILE__,__LINE__,stCompareQ.com_flag);
			vtcp_log("[%s],[%d],对账结果描述:[%s]",__FILE__,__LINE__,sCom_result);
			memcpy(stCompareQ.com_result,sCom_result,strlen(sCom_result));
			vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
			vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
			vtcp_log("[%s][%s]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
			
			iRet = Chnl_compare_acct_Upd_Upd(stCompareQ,g_pub_tx.reply);
			if (iRet)
			{
				Chnl_compare_acct_Clo_Upd();
				sprintf(acErrMsg,"更新数据库失败！[%d]",iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			vtcp_log("[%s],[%d],更新对账登记薄完成，第[%d]次",__FILE__,__LINE__,lFact_num);
			
			lFact_num++;
			continue;
		}else if (stCompareQ.batch_detail[0]=='1'){
			iRet = Chnl_batch_acct_Dec_Upd(g_pub_tx.reply,"dz_ref = '%s'",stCompareQ.un_marking);
			if (iRet){
				vtcp_log("定义游标出错,%d",iRet);
				strcpy(g_pub_tx.reply,"D103");
				Chnl_compare_acct_Clo_Upd();
				goto ErrExit;
			}
			iRet = Chnl_batch_acct_Fet_Upd(&stBatch,g_pub_tx.reply);
			if (iRet == 100 || iRet == 1403){
				/*Chnl_batch_acct_Clo_Upd();*/
				if (stCompareQ.com_stat[0]=='2'){
					vtcp_log("批量记账登记薄中没有该笔记录,进行补账处理！【%d】",iRet);
					/*若为补账对账，核心没有记录则没有记账，需做补账处理，接口未写。*/
					iRet = compare_info_acct(stCompareQ,sGz_br_no,sAc_no,sGz_reason);
					if (iRet){
						vtcp_log("自动补账错误,【%d】",iRet);
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"补账错误");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]补账错误,请检查!",__FILE__,__LINE__);
					}else{
						vtcp_log("[%s][%d]cAc_no=[%s],cGz_br_no=[%s],cGz_reason=[%s]",
							__FILE__,__LINE__,sAc_no,sGz_br_no,sGz_reason);
						if (strlen(sGz_reason) != 0){
							stCompareQ.com_flag[0]='2';
							strcpy(stCompareQ.gz_br_no,sGz_br_no);
							strcpy(stCompareQ.gz_acct_no,sAc_no);
							strcpy(stCompareQ.gz_reasen,sGz_reason);
							strcpy(sCom_result,"挂账补账");
							vtcp_log("[%s][%d]挂账补账,请继续!",__FILE__,__LINE__);
						}else{
							stCompareQ.com_flag[0]='1';
							strcpy(sCom_result,"正常补账");
							vtcp_log("[%s][%d]正常补账,请继续!",__FILE__,__LINE__);
						}
						stCompareQ.tx_date = g_pub_tx.tx_date;
						stCompareQ.trace_no = g_pub_tx.trace_no;
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
						vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
						vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
						vtcp_log("[%s][%d]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
						vtcp_log("[%s][%d]已记账,请检查!",__FILE__,__LINE__);
					}
				}else{
					vtcp_log("单笔记账登记薄中没有该笔记录,%d",iRet);
					stCompareQ.com_flag[0]='4';
					strcpy(sCom_result,"对账错");
					iWriteFile(new_fp,stCompareQ);
					vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
				}
				strcpy(g_pub_tx.reply,"0000");
			}else if (iRet){
				vtcp_log("fet出错,%d",iRet);
				Chnl_batch_acct_Clo_Upd();
				Chnl_compare_acct_Clo_Upd();
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}else{
				/*主机对账结果:0-对账平  1-正常补账 2-挂账补账 3-已冲正 4-对账错*/
				if (stCompareQ.com_stat[0]=='0'){/*记账对账*/
					if (stBatch.sts[0]=='0'||stBatch.sts[0]=='4'){
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0]='0';
						strcpy(sCom_result,"对账平");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账平,请继续!",__FILE__,__LINE__);
					}
				}else if(stCompareQ.com_stat[0]=='1'){/*冲正对账*/
					if (stBatch.sts[0]=='4'){
						stCompareQ.com_flag[0]='0';
						strcpy(sCom_result,"对账平");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账平,请继续!",__FILE__,__LINE__);
					}else{
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
					}
				}else if (stCompareQ.com_stat[0]=='2'){/*补账对账*/
					if (stBatch.sts[0]=='1' || stBatch.sts[0]=='2' || 
						stBatch.sts[0]=='3' || stBatch.sts[0]=='5' || 
						stBatch.sts[0]=='6')
					{
						stCompareQ.com_flag[0]='1';
						strcpy(sCom_result,"正常冲账");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]正常冲账,请继续!",__FILE__,__LINE__);
					}else{
						/*如果核心没有记账怎进行补账处理，接口尚未开发。这里需要写么？？*/
						vtcp_log("单笔记账登记薄中没有该笔记录,%d",iRet);
						stCompareQ.com_flag[0]='4';
						strcpy(sCom_result,"对账错");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账错,请检查!",__FILE__,__LINE__);
					}
				}else if (stCompareQ.com_stat[0]=='3'){/*冲正对账*/
					if (stBatch.sts[0]=='4'){
						stCompareQ.com_flag[0]='0';
						strcpy(sCom_result,"对账平");
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]对账平,请继续!",__FILE__,__LINE__);
					}else{/*需要核心做冲正操作，接口为开发。*/
						iRet = compare_exit_acct(stCompareQ);
						if (iRet){
							vtcp_log("自动冲账错误,【%d】",iRet);
							stCompareQ.com_flag[0]='4';
							strcpy(sCom_result,"冲账失败");
							vtcp_log("[%s][%d]冲账失败,请检查!",__FILE__,__LINE__);
							/**add by zhaoyue 20140714 为了账务的一致性，报错退出**/
							goto ErrExit;
							/**add by zhaoyue 20140714 end**/
						}else{
							stCompareQ.com_flag[0]='3';
							stCompareQ.tx_date = g_pub_tx.tx_date;
							stCompareQ.trace_no = g_pub_tx.trace_no;
							strcpy(sCom_result,"已冲正");
							stBatch.sts[0]='4';
							vtcp_log("[%s][%d]已冲正,请继续!",__FILE__,__LINE__);
						}
						iWriteFile(new_fp,stCompareQ);
						vtcp_log("[%s][%d]tx_date[%d]",__FILE__,__LINE__,stCompareQ.tx_date);
						vtcp_log("[%s][%d]trace_no[%d]",__FILE__,__LINE__,stCompareQ.trace_no);
					}
				}else{
					vtcp_log("平台的对账标识不正确，com_stat=【%s】",stCompareQ.com_stat);
					strcpy(g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				vtcp_log("第【%d】笔,sCom_result=【%s】com_stat=【%s】",
					lFact_num,sCom_result,stCompareQ.com_stat);
				stBatch.dz_ind[0] = '1';
				vtcp_log("对账状态,Dz_ind=【%s】",stBatch.dz_ind);
				
				iRet = Chnl_batch_acct_Upd_Upd(stBatch,g_pub_tx.reply);
				if (iRet)
				{
					Chnl_batch_acct_Clo_Upd();
					Chnl_compare_acct_Clo_Upd();
					sprintf(acErrMsg,"更新数据库失败！[%d]",iRet);
					WRITEMSG
					strcpy(g_pub_tx.reply,"P102");
					goto ErrExit;
				}
				vtcp_log("[%s],[%d],更新批量记账登记薄完成[%d]次",__FILE__,__LINE__,lFact_num);
			}
			Chnl_batch_acct_Clo_Upd();
			vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
			vtcp_log("[%s],[%d]tx_date=[%d],trace_no=[%d]",__FILE__,__LINE__,stCompareQ.tx_date,stCompareQ.trace_no);
			vtcp_log("[%s],[%d],对账标识:[%s]",__FILE__,__LINE__,stCompareQ.com_flag);
			vtcp_log("[%s],[%d],对账结果描述:[%s]",__FILE__,__LINE__,sCom_result);
			
			/*strcpy(stCompareQ.gz_br_no,sGz_br_no);
			strcpy(stCompareQ.gz_acct_no,sAc_no);
			strcpy(stCompareQ.gz_reasen,sGz_reason);*/
			memcpy(stCompareQ.com_result,sCom_result,strlen(sCom_result));
			vtcp_log("[%s][%d]gz_br_no[%s]",__FILE__,__LINE__,stCompareQ.gz_br_no);
			vtcp_log("[%s][%d]gz_acct_no[%s]",__FILE__,__LINE__,stCompareQ.gz_acct_no);
			vtcp_log("[%s][%d]gz_reasen[%s]",__FILE__,__LINE__,stCompareQ.gz_reasen);
			
			iRet = Chnl_compare_acct_Upd_Upd(stCompareQ,g_pub_tx.reply);
			if (iRet)
			{
				Chnl_compare_acct_Clo_Upd();
				sprintf(acErrMsg,"更新数据库失败！[%d]",iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P102");
				goto ErrExit;
			}
			vtcp_log("[%s],[%d],更新对账登记薄完成，第[%d]次",__FILE__,__LINE__,lFact_num);
			
			lFact_num++;
			continue;
			
		}else{
			vtcp_log("批量标识不正确，batch_detail=【%s】",stCompareQ.batch_detail);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		vtcp_log("%s,%d,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	}
	Chnl_compare_acct_Clo_Upd();
	vtcp_log("%s,%d,对账总笔数=[%d]",__FILE__,__LINE__,lFact_num);
	if (new_fp == NULL){
		vtcp_log("今天已对账或未发生业务");
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	fclose(new_fp);
	memset(sCmd,0x0,sizeof(sCmd));
	sprintf(sCmd,"%s/bin/send_back.sh %s",getenv("HOME"),sFilename_rt);
	iRet = system(sCmd);
	if(iRet){
		sprintf(acErrMsg,"回传文件出错[%d]",iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply,"S047");
		goto ErrExit;
	}
	set_zd_data("0820",sFilename_rt);
	vtcp_log("%s,%d,sFilename_rt=[%s]!",__FILE__,__LINE__,sFilename_rt);
	
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);  
	
OkExit:
	/*sql_commit();
	sql_begin();*/
	vtcp_log("%s,%d,对账完成!",__FILE__,__LINE__);
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return(0);
ErrExit:
	/*sql_rollback();
	sql_begin();*/
	if(strlen(g_pub_tx.reply)==0 || !strcmp(g_pub_tx.reply,"0000"))
	{
		strcpy(g_pub_tx.reply,"D103");
	}
	vtcp_log("%s,%d,批量对账失败!",__FILE__,__LINE__);
	if(new_fp!=NULL)
	fclose(new_fp);
	if(fp!=NULL)
	fclose(fp);
	WRITEMSG
	return(1);
}
	
/*==============================================================================
**函数功能：平台和核心对账时自动补账
**所需变量：
**	1、借贷标示：
**		DBIT--借记，CRDT--贷记
**	2、参数：
**		stCompare.main_acc_no--主账号，stCompare.main_acc_nm--主账号户名
**		stCompare.snd_acc_no--第二账号，stCompare.tx_amt--交易金额，
**		stCompare.tx_br_no--交易结构，
**功能描述：根据所得参数进行账务处理，具体处理如下：
**	1、检查主账户信息是否相符，若账号、户名有一不符则进行挂账处理。
**	2、若为强制挂账的交易无需检查主账户信息直接做强制挂账处理。
**	3、检查主账户信息完全正确的直接入账（强制挂账的除外）。
**	4、登记单笔或批量记账登记薄。
================================================================================*/
static int compare_info_acct(struct chnl_compare_acct_c stCompare,char *gz_br_no,char *ac_no,char *gz_reason)
{
	vtcp_log("[%s],[%d],有没有进补账函数（compare_info_acct）!",__FILE__,__LINE__);
	int	iRet=0;
	int	iParm_seqn = 0;	/*com_parm 表 参数序号*/
	char	sGdkm[6] = {0};  /**过渡科目**/
	char	cAc_no1[33] = {0};	/*主账号*/
	char	cAc_name1[61] = {0};	/*户名*/
	char	cGz_ind[2] = {0};	/*挂账标志*/
	char	cGz_br_no[6] = {0};	/*挂账机构*/
	char	cGz_reason[2] = {0};	/*挂账原因*/
	char    cXjinFlag[2] = {0};	/*现金汇款标志*/
	struct	chnl_batch_acct_c stBatch = {0};	/*批量记账登记簿*/
	struct	chnl_single_cnaps_c stSingle = {0};	/*单笔记账登记簿*/
	vtcp_log("【%s】【%d】,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	strcpy(g_pub_tx.reply,"0000");
	/***贷***/
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	g_pub_tx.tx_amt1 = stCompare.tx_amt;
	cGz_ind[0] = stCompare.con_marking[5];
	vtcp_log("[%s][%d]挂账标识cGz_ind=[%s]",__FILE__,__LINE__,cGz_ind);
	strcpy(cAc_no1,stCompare.main_acc_no);
	strcpy(cAc_name1,stCompare.main_acc_nm);
	strcpy(cGz_br_no,stCompare.tx_br_no);
	if (!memcmp(cXjinFlag,"X",1)){
		iParm_seqn = 299;
	}else{
		if (!memcmp(stCompare.syscode,"HVPS",4))
		{
			iParm_seqn = 6;
		}else{
			iParm_seqn = 8;
		}
	}
	iRet = pubf_com_CheckGZ(cAc_no1,cAc_name1,cGz_ind,cGz_br_no,cGz_reason,iParm_seqn);
	if (iRet){
	  sprintf(acErrMsg,"%s,%d,挂账检查函数失败",__FILE__,__LINE__);
	  WRITEMSG
	  goto ErrExit;	
	}
	vtcp_log("[%s][%d]cAc_no1=[%s],cAc_name1=[%s],cGz_br_no=[%s],cGz_reason=[%s]",
		__FILE__,__LINE__,cAc_no1,cAc_name1,cGz_br_no,cGz_reason);
	strcpy(gz_br_no,cGz_br_no);
	strcpy(ac_no,cAc_no1);
	strcpy(gz_reason,cGz_reason);
	vtcp_log("[%s][%d]gz_br_no=[%s],ac_no=[%s],gz_reason=[%s]",
		__FILE__,__LINE__,gz_br_no,ac_no,gz_reason);
	memcpy(g_pub_tx.reply,"0000",4);
	memset(g_pub_tx.ac_no,0x00,sizeof(g_pub_tx.ac_no));
	strcpy(g_pub_tx.ac_no,cAc_no1);
	if (cAc_no1[0] == '9' || strlen(cAc_no1) <10){/*--内部账 --*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	账户类型为内部 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A017");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	账户类型为活期 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D099");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("1017","0");
		
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_double("1013",g_pub_tx.tx_amt1);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"对账补账");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	/****借***/
	g_pub_tx.ac_id = 0;		 /*	test clean */
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.ac_no,stCompare.snd_acc_no);
	g_pub_tx.tx_amt1 = stCompare.tx_amt;
	
	if (g_pub_tx.ac_no[0]	== '9' || strlen(g_pub_tx.ac_no) < 10){ /*--内部账--*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	账户类型为内部 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"0");
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A016");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	账户类型为活期 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"0"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D003");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("102O","0");
		set_zd_data("102J","01");
		set_zd_data("102K","2");
		set_zd_double("102F",g_pub_tx.tx_amt1);
		set_zd_double("102P",0.00);
		set_zd_double("102Q",0.00);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"对账补账");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	g_pub_tx.tx_amt1 = 0.00;		/* 发生额	*/
	strcpy(g_pub_tx.ac_id_type,"0");		 /*	账户类型设为活期 */
	
	/*add fupengfei 2014/7/19 12:41:33 补账过渡记账begin*/
	if(!memcmp(stCompare.syscode,"HVPS",4))
	{
		strcpy(sGdkm,DELZGD);
	}
	if(!memcmp(stCompare.syscode,"BEPS",4))
	{
		strcpy(sGdkm,XELZGD);
	}
	{
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		memset(g_pub_tx.opn_br_no, 0x0, sizeof(g_pub_tx.opn_br_no));
		strncpy(g_pub_tx.opn_br_no, stCompare.tx_br_no ,sizeof(g_pub_tx.opn_br_no)-1);

		vtcp_log("%s,%d,g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		strcpy(g_pub_tx.brf,"补账记账");
		
		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A016");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"0"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1204","01");
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}

		/* 过渡贷方 */
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A017");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"1"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1214","01");
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/*add fupengfei 2014/7/19 12:41:33 补账过渡记账end*/
	
	if (stCompare.batch_detail[0] == '0'){/**登记单笔记账登记薄***/
		
		stSingle.tx_date = g_pub_tx.tx_date;
		sprintf(stSingle.tx_time,"%6ld",g_pub_tx.tx_time);
		stSingle.trace_no = g_pub_tx.trace_no;
		memcpy(stSingle.cur_no,"01",2);
		memcpy(stSingle.ac_no,stCompare.main_acc_no,sizeof(stSingle.ac_no)-1);
		memcpy(stSingle.ac_no1,stCompare.main_acc_no,sizeof(stSingle.ac_no1)-1);
		memcpy(stSingle.ac_no2,stCompare.snd_acc_no,sizeof(stSingle.ac_no2)-1);
		memcpy(stSingle.g_ac_no2,stCompare.gz_acct_no,sizeof(stSingle.g_ac_no2)-1);
		memcpy(stSingle.tx_code,g_pub_tx.tx_code,sizeof(stSingle.tx_code)-1);
		memcpy(stSingle.tx_br_no,g_pub_tx.tx_br_no,sizeof(stSingle.tx_br_no)-1);
		memcpy(stSingle.opn_br_no,g_pub_tx.opn_br_no,sizeof(stSingle.opn_br_no)-1);
		memcpy(stSingle.tel,g_pub_tx.tel,sizeof(stSingle.tel)-1);
		get_zd_data("0100",stSingle.bbmk);
		memcpy(stSingle.ttyn,stSingle.bbmk,sizeof(stSingle.ttyn)-1);
		stSingle.pt_date = stCompare.pt_date;
		vtcp_log("[%s][%d],平台日期=[%d]",__FILE__,__LINE__,stSingle.pt_date);
		sprintf(stSingle.pt_trace_no ,"%ld" ,stCompare.pt_trace_no);
		vtcp_log("[%s][%d],平台流水=[%s]",__FILE__,__LINE__,stSingle.pt_trace_no);
		stSingle.tx_amt = stCompare.tx_amt;
		stSingle.ct_ind[0] = '2';
		memcpy(stSingle.brf,"补账记账",sizeof(stSingle.brf)-1);
		stSingle.sts[0] = '1';
		stSingle.dz_ind[0] = '1';
		stSingle.chnl_type[0] = '3';
		
		iRet = Chnl_single_cnaps_Ins(stSingle,g_pub_tx.reply);
		if (iRet){
			sprintf(acErrMsg,"登记单笔记账登记薄出错!!!");
			WRITEMSG
			Chnl_single_cnaps_Debug(&stSingle);
			strcpy(g_pub_tx.reply,"D107");
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],登记单笔记账登记薄完成（stSingle）!",__FILE__,__LINE__);
	}
	if (stCompare.batch_detail[0] == '1'){/*记完账，登记批量记账登记簿*/
		
		stBatch.tx_date = g_pub_tx.tx_date;
		sprintf(stBatch.tx_time,"%ld",g_pub_tx.tx_time);
		get_zd_data("0830",stBatch.batch_ref);
		strcpy(stBatch.dz_ref,stCompare.un_marking);
		stBatch.trace_no = g_pub_tx.trace_no;
		strcpy(stBatch.cur_no,"01");
		strcpy(stBatch.ac_no,stCompare.main_acc_no);
		strcpy(stBatch.ac_no1,stCompare.main_acc_no);
		strcpy(stBatch.ac_no2,stCompare.snd_acc_no);
		strcpy(stBatch.g_ac_no2,stCompare.gz_acct_no);
		strcpy(stBatch.tx_code,g_pub_tx.tx_code);
		strcpy(stBatch.tx_br_no,g_pub_tx.tx_br_no);
		strcpy(stBatch.opn_br_no,g_pub_tx.opn_br_no);
		strcpy(stBatch.tel,g_pub_tx.tel);
		get_zd_data("0100",stBatch.bbmk);
		strcpy(stBatch.ttyn,g_pub_tx.tty);
		stBatch.pt_date = stCompare.pt_date;
		get_zd_data("0170",stBatch.pt_time);
		sprintf(stBatch.pt_trace_no ,"%ld" ,stCompare.pt_trace_no); 
		stBatch.tx_amt=stCompare.tx_amt;
		strcpy(stBatch.brf,"对账补账");
		stBatch.ct_ind[0]='2';
		stBatch.sts[0]='1';
		stBatch.dz_ind[0]='1';
		
		iRet = Chnl_batch_acct_Ins(stBatch,g_pub_tx.reply);
		if (iRet){
        	Chnl_batch_acct_Debug(&stBatch);
			sprintf(acErrMsg,"插入批量记账登记簿出错[%d]",iRet);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],插入批量记账登记簿完成（stBatch）!",__FILE__,__LINE__);
	}
	vtcp_log("挂账要素gz_br_no=[%s],ac_no[%s],gz_reason=[%s]",gz_br_no,ac_no,gz_reason);
	vtcp_log("[%s],[%d],补账函数执行完了吗？（compare_info_acct）!",__FILE__,__LINE__);
	
	OkExit:
		vtcp_log("【%s】【%d】,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		return 0;
	ErrExit:
		return 1;
}
/*==============================================================================
**函数功能：平台和核心对账时自动冲账
**所需变量：
**	1、借贷标示：
**		DBIT--借记，CRDT--贷记
**	2、参数：
**		stCompare.main_acc_no--主账号，stCompare.main_acc_nm--主账号户名
**		stCompare.snd_acc_no--第二账号，stCompare.tx_amt--交易金额，
**		stCompare.tx_br_no--交易结构，
**功能描述：根据所得参数进行账务处理，具体处理如下：
**	1、更新单笔或批量记账登记薄。
================================================================================*/
static int compare_exit_acct(struct chnl_compare_acct_c stCompare)
{
	vtcp_log("[%s],[%d],进入冲账函数（compare_exit_acct），请继续！",__FILE__,__LINE__);
	int	iRet = 0;
	char	sGdkm[6] = {0};  /***过渡科目**/
	struct	chnl_batch_acct_c stBatch = {0};	/*批量记账登记簿*/
	struct	chnl_single_cnaps_c stSingle = {0};	/*单笔记账登记簿*/
	
	vtcp_log("[%s],[%d],日期、流水,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	pub_base_sysinit();
	vtcp_log("[%s],[%d],日期、流水,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	vtcp_log("【%s】【%d】,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
	strcpy(g_pub_tx.reply,"0000");
	/****借***/
	g_pub_tx.ac_id = 0;		 /*	test clean */
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	strcpy(g_pub_tx.ac_no,stCompare.main_acc_no);
	g_pub_tx.tx_amt1 = -1*stCompare.tx_amt;
	if (g_pub_tx.ac_no[0]	== '9' || strlen(g_pub_tx.ac_no) < 10){ /*--内部账--*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	账户类型为内部 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"0");
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A016");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	账户类型为活期 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"0"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D003");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("102O","0");
		set_zd_data("102J","01");
		set_zd_data("102K","2");
		set_zd_double("102F",g_pub_tx.tx_amt1);
		set_zd_double("102P",0.00);
		set_zd_double("102Q",0.00);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"对账冲账");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s],[%d],冲客户账（借方）成功!",__FILE__,__LINE__);
	/***贷***/
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");
	memset(g_pub_tx.ac_no,0x00,sizeof(g_pub_tx.ac_no));
	strcpy(g_pub_tx.ac_no,stCompare.snd_acc_no);
	g_pub_tx.tx_amt1 = -1*stCompare.tx_amt;
	if(!memcmp(g_pub_tx.ac_no,"41403",5))
	{
		strcpy(g_pub_tx.opn_br_no,"10001");
	}
	if (g_pub_tx.ac_no[0] == '9' || strlen(g_pub_tx.ac_no) <10){/*--内部账 --*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	账户类型为内部 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A017");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
	}else{
		strcpy(g_pub_tx.ac_id_type,"1");		 /*	账户类型为活期 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户*/
		g_pub_tx.svc_ind = 1001;
		strcpy(g_pub_tx.add_ind,"1"); 
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"D099");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		
		strcpy(g_pub_tx.prt_ind,"0");
		set_zd_data("1017","0");
		
		set_zd_data("101A","01");
		set_zd_data("101B","2");
		set_zd_double("1013",g_pub_tx.tx_amt1);
	}
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.ac_wrk_ind,"001001001");
	strcpy(g_pub_tx.hst_ind,"1");
	strcpy(g_pub_tx.brf,"对账冲账");
	iRet = pub_acct_trance();
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
	if (iRet){
		sprintf(acErrMsg,"调用存取款主控失败[%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	
	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	g_pub_tx.tx_amt1 = 0.00;		/*发生额*/
	strcpy(g_pub_tx.ac_id_type,"0");		 /*	账户类型设为活期 */
	
	/*add  fupengfei  2014/6/30 21:22:44 手续费、过渡冲账begin*/
	if (stCompare.chrgflag[0] == '1')
	{
		/******收手续费*******/
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
		g_pub_tx.ac_id=0;
		g_pub_tx.ac_seqn=0;
		strncpy(g_pub_tx.tx_br_no,stCompare.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1);
		strncpy(g_pub_tx.opn_br_no,stCompare.tx_br_no,sizeof(g_pub_tx.opn_br_no)-1);
	
		vtcp_log("[%s],[%d],过度交易机构 2g_pub_tx.tx_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		vtcp_log("[%s],[%d],过度开户机构 2g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		/*by YangMeng 20140630 end*/
		strcpy(g_pub_tx.ac_no, stCompare.feeaccno); /* 手续费科目 */
		g_pub_tx.tx_amt1 = -1*stCompare.chrg_amt ;
		strcpy(g_pub_tx.ac_id_type, "9");	/* 账户类型为内部 */
		strcpy(g_pub_tx.ac_get_ind, "00");
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind, "1");
	
		strcpy(g_pub_tx.sub_tx_code, "A017");
	
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
		strcpy(g_pub_tx.brf,"手续费收入");/***摘要***/
		strcpy(g_pub_tx.ct_ind,"2");/**现转标志**/
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");/***日间入明细***/
		
		iRet = pub_acct_trance();
		if (iRet) {
			sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (iRet) {
			sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s],[%d],g_pub_tx.reply=[%s]!",__FILE__,__LINE__,g_pub_tx.reply);
		
		/********扣手续费*************/
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));
		g_pub_tx.ac_id=0;
		
		if (stCompare.chrg_ind[0] == '2') /*手续费：转账*/
		{
			strcpy(g_pub_tx.ac_no, stCompare.main_acc_no); 
		}
		else if (stCompare.chrg_ind[0] == '1') /*手续费：现金不冲账*/
		{
			/*待确认*/
		}
		g_pub_tx.tx_amt1 = -1*stCompare.chrg_amt;
		if (g_pub_tx.ac_no[0]	== '9' || strlen(g_pub_tx.ac_no) < 10){ /*--内部账--*/
		
		strcpy(g_pub_tx.ac_id_type,"9");		 /*	账户类型为内部 */
		strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
		g_pub_tx.svc_ind = 9001;
		strcpy(g_pub_tx.add_ind,"0");
		memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
		strcpy(g_pub_tx.sub_tx_code,"A016");
		set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_double("1208",g_pub_tx.tx_amt1);
		}else{
			strcpy(g_pub_tx.ac_id_type,"1");		 /*	账户类型为活期 */
			strcpy(g_pub_tx.ac_get_ind,"00");		/* 本程序未读取分户	*/
			g_pub_tx.svc_ind = 1001;
			strcpy(g_pub_tx.add_ind,"0"); 
			memset(g_pub_tx.sub_tx_code,0x00,sizeof(g_pub_tx.sub_tx_code));
			strcpy(g_pub_tx.sub_tx_code,"D003");
			set_zd_data(DC_SUB_TX_CODE,g_pub_tx.sub_tx_code);
			strcpy(g_pub_tx.prt_ind,"0");
			set_zd_data("102O","0");
			set_zd_data("102J","01");
			set_zd_data("102K","2");
			set_zd_double("102F",g_pub_tx.tx_amt1);
			set_zd_double("102P",0.00);
			set_zd_double("102Q",0.00);
		}
		strcpy(g_pub_tx.ct_ind,"2");
		strcpy(g_pub_tx.ac_wrk_ind,"001001001");
		strcpy(g_pub_tx.hst_ind,"1");
		strcpy(g_pub_tx.brf,"对账冲账");
	
		iRet = pub_acct_trance();
		if (iRet) {
			sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
		iRet = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (iRet) {
			sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}
	
	if(!memcmp(stCompare.syscode,"HVPS",4))
	{
		strcpy(sGdkm,DEWZGD);
	}
	if(!memcmp(stCompare.syscode,"BEPS",4))
	{
		strcpy(sGdkm,XEWZGD);
	}
	{
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		memset(g_pub_tx.opn_br_no, 0x0, sizeof(g_pub_tx.opn_br_no));
		strncpy(g_pub_tx.opn_br_no, stCompare.tx_br_no ,sizeof(g_pub_tx.opn_br_no)-1);

		vtcp_log("%s,%d,g_pub_tx.opn_br_no=[%s]!",__FILE__,__LINE__,g_pub_tx.opn_br_no);
		strcpy(g_pub_tx.brf,"对账冲账");
		
		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A016");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=-1*stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"0"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1204","01");
		set_zd_data("1205",g_pub_tx.ct_ind);
		set_zd_double("1208",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}

		/* 过渡贷方 */
		memset(g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code));
		memset(g_pub_tx.brf, 0x0, sizeof(g_pub_tx.brf));
		memset(g_pub_tx.ac_no, 0x0, sizeof(g_pub_tx.ac_no));
		memset(g_pub_tx.add_ind, 0x0, sizeof(g_pub_tx.add_ind));

		g_pub_tx.svc_ind=9001;
		strcpy(g_pub_tx.sub_tx_code, "A017");
		strcpy(g_pub_tx.ac_id_type,"9");
		g_pub_tx.ac_seqn=0;

		strcpy(g_pub_tx.ac_no,sGdkm);
		g_pub_tx.tx_amt1=-1*stCompare.tx_amt;
		strcpy(g_pub_tx.add_ind,"1"); 
		strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
		strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
		strcpy(g_pub_tx.ac_get_ind, "00");
		strcpy(g_pub_tx.note_type,"");
		strcpy(g_pub_tx.beg_note_no,"");
	
		if ( pub_acct_trance() )
		{
			sprintf(acErrMsg,"调用存取款主控失败!");
			WRITEMSG
			goto ErrExit;
		}

		set_zd_data("1214","01");
		set_zd_data("1215",g_pub_tx.ct_ind);
		set_zd_double("1218",g_pub_tx.tx_amt1);

		if ( pubf_acct_proc(g_pub_tx.sub_tx_code) )
		{
			sprintf(acErrMsg,"登记会计流水错误!");
			WRITEMSG
			goto ErrExit;
		}
	}
	/*add  fupengfei  2014/6/30 21:22:44 手续费、过渡冲账 end*/
	vtcp_log("[%s],[%d],冲账函数执行完了吗？（compare_exit_acct）!",__FILE__,__LINE__);
	vtcp_log("[%s],[%d],冲账,tx_date=[%d]trace_no=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date,g_pub_tx.trace_no);
	
	OkExit:
		vtcp_log("【%s】,【%d】,found g_pub_tx.reply=[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		return 0;
	ErrExit:
		return 1;
}
int iWriteFile(FILE	*new_fp,struct chnl_compare_acct_c stCompare)
{
	fprintf(new_fp, "%d|",stCompare.detail_no);
	fprintf(new_fp, "%s|",stCompare.un_marking);
	fprintf(new_fp, "%s|",stCompare.syscode);
	fprintf(new_fp, "%s|",stCompare.code_type);
	fprintf(new_fp, "%s|",stCompare.com_stat);
	fprintf(new_fp, "%s|",stCompare.com_flag);
	fprintf(new_fp, "%d|",stCompare.tx_date);
	fprintf(new_fp, "%d|",stCompare.trace_no);
	fprintf(new_fp, "%s|",stCompare.gz_br_no);
	fprintf(new_fp, "%s|",stCompare.gz_acct_no);
	fprintf(new_fp, "%s|",stCompare.gz_reasen);
	fprintf(new_fp, "%s|",stCompare.sndrcvflag);
	fprintf(new_fp,"\n");
	vtcp_log("【%s】,【%d】,写对账文件完成!",__FILE__,__LINE__);
	return 0;
}

