/***************************************************************
* 文 件 名: rpt051.c
* 功能描述: ATM长款数据     
*
* 作    者: rob
* 完成日期: 2003年10月07日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradlog_c.h"
#include "atm_trn_jrl_c.h"

int page = 1;
long beg_date=20031001;
struct chnl_self_trade_c sChnl_self_trade;
struct atm_trn_jrl_c sAtm_trn_jrl;
struct chnl_tradlog_c sChnl_tradlog;

int get_ratio_atmckqd(int bli,int bll,char str[100]);
rpt051()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));
	memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));
	memset(&sChnl_tradlog, 0x0, sizeof(struct chnl_tradlog_c));
	memset(&sAtm_trn_jrl, 0x0, sizeof(struct atm_trn_jrl_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Com_sys_parm_Sel(g_pub_tx.reply, &sCom_sys_parm, "1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg, "查询系统参数表错误!! [%d]", ret);
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.tx_date = sCom_sys_parm.lst_date;

	/* 打印atm交易流水清单 */
	ret = atm_ck_list();
	if (ret != 0)
	{
		sprintf(acErrMsg, "打印ATM交易流水清单!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"打印ATM交易流水清单处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"打印ATM交易流水清单处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int atm_ck_list()
{
	int ret = 0; 
	int opncnt=0; 
	FILE    *fp; 
	int     line=0; 
	int  	total_line=54;
	char fname[100]; 
	char filename[100];
	char vpfm[21]; 
	char old_br_no[6];
	char new_br_no[6];
	int first=0;
	
	memset(old_br_no, 0x0, sizeof(old_br_no));
	memset(new_br_no, 0x0, sizeof(new_br_no));
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));

	strcpy(vpfm, "atmckqd");
	pub_rpt_rmfile( "" , "BRatmckqd" , 0 );

sprintf(acErrMsg, "tx_date=%ld", g_pub_tx.tx_date);
WRITEMSG

	ret = Atm_trn_jrl_Dec_Sel(g_pub_tx.reply, "wrk_dt>=%ld and \
		and wrk_dt<=%ld and stat='2' order by wrk_dt, mch_code, curr_seqno", 
		beg_date, g_pub_tx.tx_date);
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	sprintf( filename,"%s","BRatmckqd");
	pub_rpt_openfile( &fp, sChnl_self_trade.tx_br_no, filename); 
	
	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
				get_ratio_atmckqd) ) 
		goto ErrExit; 

	first=0;
	page = 1;
	while(1) 
	{ 
		memset(&sAtm_trn_jrl, 0x0, sizeof(struct atm_trn_jrl_c));
		ret = Atm_trn_jrl_Fet_Sel(&sAtm_trn_jrl, g_pub_tx.reply); 
		if (ret != 0 && ret != 100) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		else if (ret == 100) 
		{ 
			break; 
		}


		if (line > total_line)
		{
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_atmckqd) ) 
				goto ErrExit;

			fprintf(fp,"@HY@\n");
			page = page + 1;
			line = 0;

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_atmckqd) ) 
				goto ErrExit; 
		
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_atmckqd) ) 
				goto ErrExit; 
		}
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_atmckqd) ) 
			goto ErrExit; 
	} 
	Chnl_self_trade_Clo_Sel();

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
			get_ratio_atmckqd) ) 
		goto ErrExit;
	fclose(fp);
	return 0;
ErrExit:
	return 1;
}

int get_ratio_atmtrqd( bli,bll,str )
int bli,bll;
char str[100];
{
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 

	struct com_branch_c sCom_branch;

	memset(&sCom_branch, 0x0, sizeof(struct com_branch_c));
	
	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt,"%%%ds",bll); 
			/******
			if (sChnl_self_trade.info_source[0] == '2')
			{
				sprintf( vstr,fmt, sChnl_self_trade.ac_no1);
			}
			else
			{
				sprintf( vstr,fmt, sChnl_self_trade.ac_no);
			}
			*******/
			sprintf( vstr,fmt, sChnl_self_trade.ac_no);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sChnl_self_trade.tx_code);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sChnl_self_trade.tel);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, sChnl_self_trade.tx_amt); 
			break; 
		case 'E': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sChnl_self_trade.tx_br_no); 
			break; 
		case 'F':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, sChnl_self_trade.trace_no); 
			break; 
		case 'G':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt); 
			break; 
		case 'H':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt); 
			break; 
		case 'I':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, z_amt); 
			break; 
		case 'J':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, z_cnt); 
			break; 
		case 'K':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, c_amt); 
			break; 
		case 'L':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, c_cnt); 
			break; 
		case 'M':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt_w); 
			break; 
		case 'N':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt_w); 
			break; 
		case 'Q': 
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, "br_no='%s'", 
					sChnl_self_trade.tx_br_no); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret); 
				WRITEMSG 
				goto ErrExit; 
			} 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf(str, fmt, sCom_branch.br_name); 
			break;
		case 'T':
			sprintf(fmt, "%%%ds", bll); 
			sprintf(str, fmt, "ATM"); 
			break;
		case '1': 
			sprintf( str, "%08ld", g_pub_tx.tx_date); 
			break; 
		case '2': 
			sprintf(fmt, "%%-%ds", bll); 
			sprintf( str, fmt, sChnl_self_trade.tx_br_no ); 
			break; 
		case '3': 
			sprintf( str, "%02ld", page); 
			break; 
		case '7': 
			sprintf( str, "%04s", "6203"); 
			break; 
		case '8': 
			sprintf( str, "%04s", "6205"); 
			break; 
		case '9': 
			sprintf( str, "%04s", "6206"); 
			break; 
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
