/***************************************************************
* 文 件 名: rpt029.c
* 功能描述: ATM交易统计月报
*
* 作    者: rob
* 完成日期: 2003年6月24日
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
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "chnl_self_trade_c.h"

int page = 1;
double	q_amt=0.00, q_amt_w=0.00, q_amt_l=0.00, z_amt=0.00, c_amt=0.00;
int 	q_cnt=0, q_cnt_w=0, q_cnt_l=0, z_cnt=0, c_cnt=0;
struct chnl_self_trade_c sChnl_self_trade;
char br_no[6];

int get_ratio_atmtrad_y(int bli,int bll,char str[100]);
rpt029()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));
	memset(&sChnl_self_trade, 0x0, sizeof(struct chnl_self_trade_c));

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
	ret = atm_trade_list_y();
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

int atm_trade_list_y()
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
	long beg_date=0;
	
	memset(old_br_no, 0x0, sizeof(old_br_no));
	memset(new_br_no, 0x0, sizeof(new_br_no));
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));

	strcpy(vpfm, "atmtrad");
	sprintf( filename,"%s","BMatmtrad");
	pub_rpt_rmfile( "" , filename , 0 );

	pub_rpt_openfile( &fp, "61000", filename); /*报表放在61000机构*/

sprintf(acErrMsg, "tx_date=%ld", g_pub_tx.tx_date);
WRITEMSG

	line = 0;
	beg_date = g_pub_tx.tx_date / 100;
	beg_date = beg_date * 100 + 1;

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
			get_ratio_atmtrad_y) ) 
		goto ErrExit; 

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
			get_ratio_atmtrad_y) ) 
		goto ErrExit; 

	ret = Chnl_self_trade_Dec_Sel(g_pub_tx.reply, "tx_date>=%ld and \
		tx_date <= %ld and ttyp='1' \
		and tx_code in ('6203', '6205', '6206') and stat='0' and \
		info_source in ('0', '2') order by tx_br_no, tel, tx_date, trace_no", 
		beg_date, g_pub_tx.tx_date);
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
	
	first=0;
	q_amt = 0.00;
	q_cnt = 0;
	q_amt_l = 0.00;
	q_cnt_l = 0;
	q_amt_w = 0.00;
	q_cnt_w = 0;
	z_amt = 0.00;
	z_cnt = 0;
	c_amt = 0.00;
	c_cnt = 0;
	while(1) 
	{ 
		ret = Chnl_self_trade_Fet_Sel(&sChnl_self_trade, g_pub_tx.reply); 
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

		memset(new_br_no, 0x0, sizeof(new_br_no));
		strcpy(new_br_no, sChnl_self_trade.tx_br_no);
	
		if (line > total_line)
		{
			fprintf(fp,"@HY@\n");
			page = page + 1;
			line = 0;

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_atmtrad_y) ) 
				goto ErrExit; 
		
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_atmtrad_y) ) 
				goto ErrExit; 
		}
		if (strcmp(sChnl_self_trade.tx_code, "6203") == 0)
		{
			if (sChnl_self_trade.info_source[0] == '0')
			{
				q_amt += sChnl_self_trade.tx_amt;
				q_cnt += 1;
			}
			else 
			{
				pub_base_strpack( sChnl_self_trade.ac_no );
			   	if( memcmp( sChnl_self_trade.ac_no,"940071732",9) == 0 || \
				  	memcmp( sChnl_self_trade.ac_no,"622368732",9) == 0 || \
					memcmp( sChnl_self_trade.ac_no,"940071730",9) == 0 || \
					memcmp( sChnl_self_trade.ac_no,"622368730",9) == 0 || \
					memcmp( sChnl_self_trade.ac_no,"940071731",9) == 0 || \
					memcmp( sChnl_self_trade.ac_no,"622368731",9) == 0 || \
					memcmp( sChnl_self_trade.ac_no,"622368734",9) == 0 || \
					memcmp( sChnl_self_trade.ac_no,"622368739",9) == 0 )
				{
					q_amt_l += sChnl_self_trade.tx_amt;
					q_cnt_l += 1;
				}
				else
				{
					q_amt_w += sChnl_self_trade.tx_amt;
					q_cnt_w += 1;
				}
			}
		}
		else if ((sChnl_self_trade.tx_code, "6205") == 0)
		{
			z_amt += sChnl_self_trade.tx_amt;
			z_cnt += 1;
		}
		else
		{
			c_amt += sChnl_self_trade.tx_amt;
			c_cnt += 1;
		}

		if (strcmp(new_br_no, old_br_no) != 0)
		{
			if (first != 0)
			{
				q_amt = q_amt/10000;
				q_amt_l = q_amt_l/10000;
				q_amt_w = q_amt_w/10000;
				z_amt = z_amt/10000;
				c_amt = c_amt/10000;

				strcpy(br_no, old_br_no);
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
						get_ratio_atmtrad_y) ) 
					goto ErrExit; 
				q_amt = 0.00;
				q_cnt = 0;
				q_amt_l = 0.00;
				q_cnt_l = 0;
				q_amt_w = 0.00;
				q_cnt_w = 0;
				z_amt = 0.00;
				z_cnt = 0;
				c_amt = 0.00;
				c_cnt = 0;
			}
		}
		first += 1;
		strcpy(old_br_no, new_br_no);
	} 
	Chnl_self_trade_Clo_Sel();

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
			get_ratio_atmtrad_y) ) 
		goto ErrExit;
	fclose(fp);

	return 0;
ErrExit:
	return 1;
}

/* 汇总 
int atm_total_y(char br_no[6])
{
	int ret=0;

	q_amt = 0.00;
	q_cnt = 0;
	q_amt_w = 0.00;
	q_cnt_w = 0.00;
	z_amt = 0.00;
	z_cnt = 0;
	c_amt = 0.00;
	c_cnt = 0;

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &q_amt, 
		"tx_date=%ld and ttyp='1' and tx_code = '6203' and stat='0' and \
		tx_br_no='%s' and info_source='0'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计ATM取款金额错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6203' and stat='0' and tx_br_no='%s' and info_source='0'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计ATM取款次数错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	q_cnt = ret;

	sprintf(acErrMsg, "ATM取款金额为:[%lf], 取款笔数:[%ld]", q_amt, q_cnt);
	WRITEMSG

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &q_amt_w, 
		"tx_date=%ld and ttyp='1' and tx_code = '6203' and stat='0' and \
		tx_br_no='%s' and info_source='2'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计ATM取款金额错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6203' and stat='0' and tx_br_no='%s' and info_source='2'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计ATM取款次数错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	q_cnt_w = ret;

	sprintf(acErrMsg, "ATM取款(外行)金额为:[%lf], 本行取款笔数:[%ld]", 
		q_amt_w, q_cnt_w);
	WRITEMSG

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &z_amt, 
		"tx_date=%ld and ttyp='1' and tx_code = '6205' and stat='0' and \
		tx_br_no='%s'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计ATM取款金额错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6205' and stat='0' and tx_br_no='%s'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计ATM取款次数错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	z_cnt = ret;

	sprintf(acErrMsg, "ATM转帐金额为:[%lf], 转帐笔数:[%ld]", z_amt, z_cnt);
	WRITEMSG

	ret = sql_sum_double("chnl_self_trade", "tx_amt", &c_amt, 
		"tx_date=%ld and ttyp='1' and tx_code = '6206' and stat='0' and \
		tx_br_no='%s'", g_pub_tx.tx_date, br_no);
	if (ret != 0)
	{
		sprintf(acErrMsg, "统计ATM取款金额错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}

	ret = sql_count("chnl_self_trade", "tx_date=%ld and ttyp='1' and \
		tx_code = '6206' and stat='0' and tx_br_no='%s'", 
		g_pub_tx.tx_date, br_no);
	if (ret < 0)
	{
		sprintf(acErrMsg, "统计ATM取款次数错误!! [%d]", ret);
		WRITEMSG
		return ret;
	}
	c_cnt = ret;

	sprintf(acErrMsg, "ATM存款金额为:[%lf], 存款笔数:[%ld]", c_amt, c_cnt);
	WRITEMSG

	return 0;
}*/

int get_ratio_atmtrad_y( bli,bll,str )
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
			sprintf( vstr,fmt, br_no);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, c_amt);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( vstr,fmt, c_cnt);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt); 
			break; 
		case 'E': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt); 
			break; 
		case 'F':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, z_amt); 
			break; 
		case 'G':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, z_cnt); 
			break; 
		case 'H':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt_l); 
			break; 
		case 'I':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt_l); 
			break; 
		case 'J':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf(str, fmt, q_amt_w); 
			break; 
		case 'K':
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, q_cnt_w); 
			break; 
		case 'Q': 
			ret = Com_branch_Sel(g_pub_tx.reply, &sCom_branch, 
				"br_no='61000'");
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
			sprintf( str, fmt, "61000" ); 
			break; 
		case '3': 
			sprintf( str, "%02ld", page); 
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
