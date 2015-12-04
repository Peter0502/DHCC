/***************************************************************
* 文 件 名:  pubf_rept_glmst.c
* 功能描述：
*					********  声明基础函数  *******
*			1.char 	*f_prt_null();
*			2.char 	*f_prt_line();
*			3.char	*f_get_brno();				* 取机构名称 	*
*			4.char 	*f_get_year();				* 取系统日期 年*
*			5.char	*f_get_month();				* 取系统日期 月*
*			6.char	*f_get_day();				* 取系统日期 日*
*			7.char 	*f_get_season();			* 取系统日期 季*
*
*					********    计算函数   ********
*			8.double 	f_get_bal(char * _acchrt, int _d_flag, char * _dc_flag);
*			9.double 	f_get_bal_d(char * _acchrt, int	_d_flag);
*					*******************************
*			10.char 	*f_get_current_bal(char *_acchrt);
*			11.char 	*f_get_current_wi_bal(char *_acchrt);
*
*			12.char 	*f_get_yesterday_bal(char *_acchrt);
*			13.char 	*f_get_befyesterday_bal(char *_acchrt);
*
*			14.char 	*f_get_befmonday_bal(char *_acchrt);
*			15.char 	*f_get_yesterday_wi_bal(char *_acchrt);
*
*			16.char 	*f_get_tenday_bal(char *_acchrt);
*			17.char 	*f_get_tenday_wi_bal(char *_acchrt);
*
*			18.char 	*f_get_month_bal(char *_acchrt);
*			19.char 	*f_get_month_wi_bal(char *_acchrt);
*
*			20.char 	*f_get_season_bal(char *_acchrt);
*			21.char 	*f_get_season_wi_bal(char *_acchrt);
*
*			22.char 	*f_get_year_bal(char *_acchrt);
*			23.char 	*f_get_year_wi_bal(char *_acchrt);
*
*			24.char    *f_get_current_d_amt(char *_acchrt);
*			25.char    *f_get_current_d_amt_wi(char *_acchrt);
*
*			26.char    *f_get_current_c_amt(char *_acchrt);
*			27.char    *f_get_current_c_amt_wi(char *_acchrt);
*
*			28.char    *f_get_tenday_d_amt(char *_acchrt);
*			29.char    *f_get_tenday_d_amt_wi(char *_acchrt);
*
*			30.char    *f_get_tenday_c_amt(char *_acchrt);
*			31.char    *f_get_tenday_c_amt_wi(char *_acchrt);
*
*			32.char    *f_get_month_d_amt(char *_acchrt);
*			33.char    *f_get_month_d_amt_wi(char *_acchrt);
*
*			34.char    *f_get_month_c_amt(char *_acchrt);
*			35.char    *f_get_month_c_amt_wi(char *_acchrt);
*
*			36.char    *f_get_season_d_amt(char *_acchrt);
*			37.char    *f_get_season_d_amt_wi(char *_acchrt);
*
*			38.char    *f_get_season_c_amt(char *_acchrt);
*			39.char    *f_get_season_c_amt_wi(char *_acchrt);
*
*			40.char    *f_get_year_d_amt(char *_acchrt);
*			41.char    *f_get_year_d_amt_wi(char *_acchrt);
*
*			42.char    *f_get_year_c_amt(char *_acchrt);
*			43.char    *f_get_year_c_amt_wi(char *_acchrt);
*
*			44.char    *f_get_today_d_cnt(char *_acchrt);
*			45.char    *f_get_today_c_cnt(char *_acchrt);
*
*			46.char    *f_get_tenday_d_cnt(char *_acchrt);
*			47.char    *f_get_tenday_c_cnt(char *_acchrt);
*
*			48.char    *f_get_month_d_cnt(char *_acchrt);
*			49.char    *f_get_month_c_cnt(char *_acchrt);
*
*			50.char    *f_get_season_d_cnt(char *_acchrt);
*			51.char    *f_get_season_c_cnt(char *_acchrt);
*
*			52.char    *f_get_year_d_cnt(char *_acchrt);
*			53.char    *f_get_year_c_cnt(char *_acchrt);
*
*				******** 新增加---取去年同期余额 ********
*			54.double f_get_l_bal(char * _acchrt,int  _d_flag, char * _dc_flag);
*			55.double f_get_l_bal_d(char * _acchrt, int	_d_flag);
*			56.char *f_get_l_current_bal(char *_acchrt);
*			57.char *f_get_l_yesterday_bal(char *_acchrt);
*			58.char *f_get_l_tenday_bal(char *_acchrt);
*			59.char *f_get_l_month_bal(char *_acchrt);
*			60.char *f_get_l_season_bal(char *_acchrt);
*			61.char *f_get_l_year_bal(char *_acchrt);
*
*			62.double f_get_d_bal(char * _acchrt, int	_d_flag);
*			63.double f_get_c_bal(char * _acchrt, int	_d_flag);
*			64.char    *f_get_cc_bal(char  *_acchrt);
*			65.char    *f_get_lc_bal(char  *_acchrt);
*			66.char    *f_get_tc_bal(char  *_acchrt);
*			67.char    *f_get_mc_bal(char  *_acchrt);
*			68.char    *f_get_sc_bal(char  *_acchrt);
*			69.char    *f_get_yc_bal(char  *_acchrt);
*			70.char    *f_get_cd_bal(char  *_acchrt);
*			71.char    *f_get_ld_bal(char  *_acchrt);
*			72.char    *f_get_td_bal(char  *_acchrt);
*			73.char    *f_get_md_bal(char  *_acchrt);
*			74.char    *f_get_sd_bal(char  *_acchrt);
*			75.char    *f_get_yd_bal(char  *_acchrt);
* 作    者:
* 完成日期： 
*
* 修改记录：
* 1. 日    期:  
*    修 改 人:  xxx
*    修改内容: 
*
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "pubf_rept_fun.h"
#include "com_branch_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"
#include "gl_mst_hst_c.h"
#include "gl_tsub_c.h"

struct  com_branch_c 	vcom_branch;
struct  com_item_c 		vcom_item;
struct  gl_mst_c 		vgl_mst;
struct  gl_mst_hst_c 	vgl_mst_hst;
struct  gl_tsub_c 		vgl_tsub;

static	int	ret=0;

/***
	取机构名称 
***/
char	*f_get_brno()
{
	char	l_name[20];

	memset(l_name, 0x00, sizeof(l_name));
	memset(&vcom_branch, 0x00, sizeof(struct com_branch_c));

	ret=Com_branch_Sel(g_pub_tx.reply,&vcom_branch, \
					  "br_no='%s'",g_brno_tmp.br_no);
	if( 100 == ret )	
	{
		strcpy(l_name," ");
	}

	strcpy(l_name,vcom_branch.br_name);
	
	return l_name;
ErrExit:
	strcpy(l_name, "");
	return l_name;
}
char *f_get_year()
{
	char	l_return[256];
	sprintf(l_return, "%04d", g_rept_date/100/100);
	return l_return;
}
char *f_get_month()
{
	char	l_return[256];
	sprintf(l_return, "%02d", g_rept_date/100%100);
	return l_return;
}
char *f_get_day()
{
	char	l_return[256];
	sprintf(l_return, "%02d",g_rept_date%100);
	return l_return;
}
char *f_get_season()
{
	char	l_return[256];
	int		l_month;

	l_month = g_rept_date/100%100;

	if( l_month>=1 && l_month <=3 )
		strcpy( l_return, "一");
	else if( l_month>=4 && l_month <=6 )
		strcpy( l_return, "二");
	else if( l_month>=6 && l_month <=9 )
		strcpy( l_return, "三");
	else if( l_month>=10 && l_month <=12 )
		strcpy( l_return, "四");
	else
		strcpy( l_return, "  ");
	return l_return;
}
/***  
	打印空格
***/
char *f_prt_null()
{
	char	l_return[256];

	memset(l_return, 0x00, sizeof(l_return));

	strcpy(l_return, "  ");
	return l_return;
}

/***  
	打印 "──"
***/
char *f_prt_line()
{
	char	l_return[256];

	memset(l_return, 0x00, sizeof(l_return));

	strcpy(l_return, "       ─");
	return l_return;
}

/***
	取整数万元
***/
char *f_get_current_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal,0x00,sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal,f_get_current_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	实现函数  根据科目取当前(本日)余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额,余额
***/
char *f_get_current_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 0));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 0, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	取整数万元
***/
char *f_get_yesterday_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_yesterday_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/**********************************************************
	实现函数  根据科目取前天余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***********************************************************/
char *f_get_befmonday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	long 	qtrq,ycrq;
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(&vgl_tsub, 0x00, sizeof(struct gl_tsub_c));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	ycrq=ycrq/100*100+1;
	pub_base_deadlineD(ycrq,-1,&qtrq);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	ret=Gl_tsub_Sel(g_pub_tx.reply,&vgl_tsub,"date=%ld and term_type='5' and \
					br_no='%s' and cur_no='01' and acc_hrt='%s'", \
					qtrq,g_brno_tmp.br_no,l_acchrt);
	if(ret==100)
	{
		vgl_tsub.bt_dr_bal=vgl_tsub.bt_cr_bal=0.00;
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0' || vcom_item.dc_ind[0]=='1' )
	{
		sprintf(l_s_bal,"%f", vgl_tsub.bt_dr_bal);
	}
	else
	{
		sprintf(l_s_bal,"%f", vgl_tsub.bt_cr_bal);
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char *f_get_befyesterday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	long qtrq;
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(&vgl_mst_hst, 0x00, sizeof(struct gl_mst_hst_c));

	strcpy(l_acchrt, _acchrt);
	pub_base_deadlineD(g_rept_date,-2,&qtrq);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	ret=Gl_mst_hst_Sel(g_pub_tx.reply,&vgl_mst_hst,"date=%ld and term_type='5' \
					and br_no='%s' and cur_no='01' and acc_hrt='%s'", \
					qtrq,g_brno_tmp.br_no,l_acchrt);
	if(ret)
	{
		vgl_mst_hst.dr_bal=vgl_mst_hst.cr_bal=0.00;
	}

	if( vcom_item.dc_ind[0] == '0' || vcom_item.dc_ind[0]=='1' )
	{
		sprintf(l_s_bal,"%f", vgl_mst_hst.dr_bal);
	}
	else
	{
		sprintf(l_s_bal,"%f", vgl_mst_hst.cr_bal);
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char *f_get_yesterday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 1));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 1, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	取整数万元
***/
char *f_get_tenday_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_tenday_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	实现函数  根据科目取旬初余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_tenday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 2));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 2, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	取整数万元
***/
char *f_get_month_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_month_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	实现函数  根据科目号取月初余额
	input	:
				int		_acchrt		科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal		月初余额
***/
char *f_get_month_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 3));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 3, vcom_item.dc_ind));
	}
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	取整数万元
***/
char *f_get_season_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_season_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	实现函数  根据科目号取季初余额
	input	:
				int		_acchrt		科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal		初余额
***/
char *f_get_season_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 4));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 4, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	取整数万元
***/
char *f_get_year_wi_bal(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_year_bal(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/***
	实现函数  根据科目取年初余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_year_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item, 0x00, sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_bal_d(l_acchrt, 5));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_bal(l_acchrt, 5, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/**********************************************************
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	科目号为单性科目
	根据机构号、科目号计算借方余额
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
				char _dc_flag	借贷方向标志
							 1  借
							 2  贷
	output	:
				char	l_s_amt	发生额
**********************************************************/
double f_get_bal(char * _acchrt, int	_d_flag, char * _dc_flag)
{
	char	l_acchrt[8];
	char	l_tmp[256];
	double	l_bal = 0.00;
	int		_o_flag;

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_tmp, 0x00, sizeof(l_tmp));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_tmp, "%c", _dc_flag[0]);
	_o_flag = atoi(l_tmp);

	/* 业务机构 */
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and cur_no='%s' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no,D_CURNO,l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
			if( _o_flag == 1 ){						/*  借方  */
				l_bal=vgl_mst.dr_bal;
			}else 									/*  贷方  */
			{
				l_bal=vgl_mst.cr_bal;
			}
			break;
		case 1:				/* 昨日     */
			if( _o_flag == 1 ){						/*  借方  */
				l_bal=vgl_mst.ldd_bal;
			}else									/*  贷方  */
			{
				l_bal=vgl_mst.lcd_bal;
			}
			break;
		case 2:				/* 本旬		*/
			if( _o_flag == 1 ){						/*  借方  */
				l_bal=vgl_mst.tddr_bal;
			}else									/*  贷方  */
			{
				l_bal=vgl_mst.tdcr_bal;
			}
			break;
		case 3:				/* 本月		*/
			if( _o_flag == 1 ){						/*  借方  */
				l_bal=vgl_mst.mdr_bal;
			}else									/*  贷方  */
			{
				l_bal=vgl_mst.mcr_bal;
			}
			break;
		case 4:				/* 本季		*/
			if( _o_flag == 1 ){						/*  借方  */
				l_bal=vgl_mst.qdr_bal;
			}else									/*  贷方  */
			{
				l_bal=vgl_mst.qcr_bal;
			}
			break;
		case 5:				/* 本年		*/
			if( _o_flag == 1 ){						/*  借方  */
				l_bal=vgl_mst.ydr_bal;
			}else									/*  贷方  */
			{
				l_bal=vgl_mst.ycr_bal;
			}
			break;
		default:
			break;
	}

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/*******************************************************
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	科目号为双性科目
	根据机构号、科目号计算借方余额
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
*******************************************************/
double f_get_bal_d(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	double	l_bal = 0.00;

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));

	strcpy(l_acchrt, _acchrt);

	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
		cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no,l_acchrt);
	if(ret==100)
	{
		 goto ErrExit;
	}

	/* 业务机构 */
	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
			if(pub_base_CompDblVal(vgl_mst.dr_bal,0.00))
			{
				l_bal=vgl_mst.dr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.cr_bal,0.00))
			{
				l_bal=vgl_mst.cr_bal;
			}
			else l_bal = 0.00;
			break;
		case 1:				/* 昨日 */
			if(pub_base_CompDblVal(vgl_mst.ldd_bal,0.00))
			{
				l_bal=vgl_mst.ldd_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.lcd_bal,0.00))
			{
				l_bal=vgl_mst.lcd_bal;
			}
			else l_bal = 0.00;
			break;
		case 2:				/* 本旬		*/
			if(pub_base_CompDblVal(vgl_mst.tddr_bal,0.00))
			{
				l_bal=vgl_mst.tddr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.tdcr_bal,0.00))
			{
				l_bal=vgl_mst.tdcr_bal;
			}
			else l_bal = 0.00;
			break;
		case 3:				/* 本月	*/
			if(pub_base_CompDblVal(vgl_mst.mdr_bal,0.00))
			{
				l_bal=vgl_mst.mdr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.mcr_bal,0.00))
			{
				l_bal=vgl_mst.mcr_bal;
			}
			else l_bal = 0.00;
			break;
		case 4:				/* 本季	*/
			if(pub_base_CompDblVal(vgl_mst.qdr_bal,0.00))
			{
				l_bal=vgl_mst.qdr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.qcr_bal,0.00))
			{
				l_bal=vgl_mst.qcr_bal;
			}
			else l_bal = 0.00;
			break;
		case 5:				/* 本年	*/
			if(pub_base_CompDblVal(vgl_mst.ydr_bal,0.00))
			{
				l_bal=vgl_mst.ydr_bal;
			}
			else if(pub_base_CompDblVal(vgl_mst.ycr_bal,0.00))
			{
				l_bal=vgl_mst.ycr_bal;
			}
			else l_bal = 0.00;
			break;
		default:
			break;
	}		/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}


/***
	取整数万元
***/
char *f_get_current_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_current_d_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	取借方发生额   本日
***/
char	*f_get_current_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.rdd_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/***
	取整数万元
***/
char *f_get_current_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_current_c_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	取贷方发生额   本日
***/
char	*f_get_current_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.rcd_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/***
	取整数万元
***/
char *f_get_tenday_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_tenday_d_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	取借方发生额   本旬
***/
char	*f_get_tenday_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.tddr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/***
	取整数万元
***/
char *f_get_tenday_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_tenday_c_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	取贷方发生额   本旬
***/
char	*f_get_tenday_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.tdcr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/***
	取整数万元
***/
char *f_get_month_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_month_d_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	取借方发生额   本月
***/
char	*f_get_month_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.mdr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	取整数万元
***/
char *f_get_month_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_amt[256];
	double	l_wi_amt;
	long	l_i_amt;

	memset(l_s_wi_amt, 0x00, sizeof(l_s_wi_amt));

	strcpy(l_s_wi_amt, f_get_month_c_amt(_acchrt));
	l_wi_amt = atof(l_s_wi_amt);
	l_i_amt = rint(l_wi_amt/10000);
	
	sprintf(l_s_wi_amt, "%ld", l_i_amt);
	
	return l_s_wi_amt;
}
/*** 
	取贷方发生额   本月
***/
char	*f_get_month_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.mcr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	取整数万元
***/
char *f_get_season_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_season_d_amt(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	取借方发生额   本季
***/
char	*f_get_season_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.qdr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	取整数万元
***/
char *f_get_season_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_season_c_amt(_acchrt));
	printf("l_s_wi_bal=[%s]\n",l_s_wi_bal);
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	取贷方发生额   本月
***/
char	*f_get_season_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.qcr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	取整数万元
***/
char *f_get_year_d_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_year_d_amt(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	取借方发生额   本年
***/
char	*f_get_year_d_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.ydr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/***
	取整数万元
***/
char *f_get_year_c_amt_wi(char *_acchrt)
{
	char	l_s_wi_bal[256];
	double	l_wi_bal;
	long	l_i_bal;

	memset(l_s_wi_bal, 0x00, sizeof(l_s_wi_bal));

	strcpy(l_s_wi_bal, f_get_year_c_amt(_acchrt));
	l_wi_bal = atof(l_s_wi_bal);
	l_i_bal = rint(l_wi_bal/10000);
	
	sprintf(l_s_wi_bal, "%ld", l_i_bal);
	
	return l_s_wi_bal;
}
/*** 
	取贷方发生额   本年
***/
char	*f_get_year_c_amt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_amt[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_amt, "%f", vgl_mst.ycr_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	取当前结余户数-----------已删除此字段
***/

/*** 
	取本日借笔数
***/
char	*f_get_today_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.rdd_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本日贷笔数
***/
char	*f_get_today_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.rcd_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本旬借笔数
***/
char	*f_get_tenday_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.tddr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本旬贷笔数
***/
char	*f_get_tenday_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.tdcr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本月借笔数
***/
char	*f_get_month_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.mdr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本月贷笔数
***/
char	*f_get_month_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.mcr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本季借笔数
***/
char	*f_get_season_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.qdr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0.00);
	return l_s_count;
}
/*** 
	取本季贷笔数
***/
char	*f_get_season_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.qcr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年借笔数
***/
char	*f_get_year_d_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.ydr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本年贷笔数
***/
char	*f_get_year_c_cnt(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_count[256];

	memset(&vgl_mst, 0x00, sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_count, 0x00, sizeof(l_s_count));

	strcpy(l_acchrt, _acchrt);
	
	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	sprintf(l_s_count, "%d", vgl_mst.ycr_cnt);
	return l_s_count;
ErrExit:
	sprintf(l_s_count, "%d", 0);
	return l_s_count;
}
/*** 
	取本日开户数	-----已删除
***/

/*** 
	取本旬开户数	-----已删除
***/

/*** 
	取本月开户数	-----已删除
***/

/*** 
	取本季开户数	-----已删除
***/

/*** 
	取本年开户数	-----已删除
***/

/*** 
	取本日销户数
***/

/*** 
	取本旬销户数
***/

/*** 
	取本月销户数
***/

/*** 
	取本季销户数
***/

/*** 
	取本年销户数
***/

/*** 
	取昨日结余户数
***/

/*** 
	取本旬初结余户数
***/

/*** 
	取本月初结余户数
***/

/*** 
	取本季初结余户数
***/

/*** 
	取本年初结余户数
***/

/*******************************************************************/
/*  get the same date of last year */

/***
	实现函数  根据科目取当前(本日)余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_l_current_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 0));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 0, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目取昨日余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_l_yesterday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 1));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 1, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目取旬初余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_l_tenday_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 2));
	}else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 2, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目号取月初余额
	input	:
				int		_acchrt		科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal		月初余额
***/
char *f_get_l_month_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 3));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 3, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	实现函数  根据科目号取季初余额
	input	:
				int		_acchrt		科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal		初余额
***/
char *f_get_l_season_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 4));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 4, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	实现函数  根据科目取年初余额
	input	:
				int	_acchrt	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_l_year_bal(char *_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(&vcom_item,0x00,sizeof(struct com_item_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);

	ret=Com_item_Sel(g_pub_tx.reply,&vcom_item,"acc_hrt='%s'",l_acchrt);
	if(ret)
	{
		goto ErrExit;
	}

	if( vcom_item.dc_ind[0] == '0')
	{
		sprintf(l_s_bal,"%f", f_get_l_bal_d(l_acchrt, 5));
	}
	else
	{
		sprintf(l_s_bal,"%f", f_get_l_bal(l_acchrt, 5, vcom_item.dc_ind));
	}

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/*
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	科目号为单性科目
	根据机构号、科目号计算借方余额
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
				char _dc_flag	借贷方向标志
							 1  借
							 2  贷
	output	:
				char	l_s_amt	发生额
*/
double f_get_l_bal(char * _acchrt, int	_d_flag, char * _dc_flag)
{
	char	l_acchrt[8];
	char	l_tmp[256];
	long	l_date;
	double	l_bal = 0.00;
	int		_o_flag;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_tmp, 0x00, sizeof(l_tmp));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_tmp, "%c", _dc_flag[0]);
	_o_flag = atoi(l_tmp);

	l_date = (g_rept_date/10000-1)*10000+g_rept_date%10000;

	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='%s' and acc_hrt='%s'",l_date,g_brno_tmp.br_no, \
					D_CURNO, l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	switch(_d_flag)
	{
		case 0:				/*当前余额(本日)*/
			if( _o_flag == 1 )						/*  借方  */
				l_bal=vgl_mst.dr_bal;
			else 									/*  贷方  */
				l_bal=vgl_mst.cr_bal;
			break;
		case 1:				/*昨日*/
			if( _o_flag == 1 )						/*  借方  */
				l_bal=vgl_mst.ldd_bal;
			else									/*  贷方  */
				l_bal=vgl_mst.lcd_bal;
			break;
		case 2:				/*本旬*/
			if( _o_flag == 1 )						/*  借方  */
				l_bal=vgl_mst.tddr_bal;
			else									/*  贷方  */
				l_bal=vgl_mst.tdcr_bal;
			break;
		case 3:				/*本月*/
			if( _o_flag == 1 )						/*  借方  */
				l_bal=vgl_mst.mdr_bal;
			else									/*  贷方  */
				l_bal=vgl_mst.mcr_bal;
			break;
		case 4:				/*本季*/
			if( _o_flag == 1 )						/*  借方  */
				l_bal=vgl_mst.qdr_bal;
			else									/*  贷方  */
				l_bal=vgl_mst.qcr_bal;
			break;
		case 5:				/* 本年		*/
			if( _o_flag == 1 )						/*  借方  */
				l_bal=vgl_mst.ydr_bal;
			else									/*  贷方  */
				l_bal=vgl_mst.ycr_bal;
			break;
		default:
			break;
	}

	/* 业务机构 */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/*****************************************************
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	科目号为双性科目
	根据机构号、科目号计算借方余额
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
******************************************************/
double f_get_l_bal_d(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	char	l_field[256];
	double	l_bal = 0.00;
	long	l_date;
	int		a1;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_field, 0x00, sizeof(l_field));

	l_date = (g_rept_date/10000-1)*10000+g_rept_date%10000;

	strcpy(l_acchrt, _acchrt);

	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='%s' and acc_hrt='%s'",l_date,g_brno_tmp.br_no, \
					D_CURNO, l_acchrt);
	if(ret==100)
	{
		goto ErrExit;
	}

	/* 业务机构 */
	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
				if(pub_base_CompDblVal(vgl_mst.dr_bal,0.00))
				{
					l_bal=vgl_mst.dr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.cr_bal,0.00))
				{
					l_bal=vgl_mst.cr_bal;
				}
				else l_bal=0.00;
				break;
		case 1:				/* 昨日 */
				if(pub_base_CompDblVal(vgl_mst.ldd_bal,0.00))
				{
					l_bal=vgl_mst.ldd_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.lcd_bal,0.00))
				{
					l_bal=vgl_mst.lcd_bal;
				}
				else l_bal=0.00;
				break;
		case 2:				/* 本旬	*/
				if(pub_base_CompDblVal(vgl_mst.tddr_bal,0.00))
				{
					l_bal=vgl_mst.tddr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.tdcr_bal,0.00))
				{
					l_bal=vgl_mst.tdcr_bal;
				}
				else l_bal=0.00;
				break;
		case 3:				/* 本月	*/
				if(pub_base_CompDblVal(vgl_mst.mdr_bal,0.00))
				{
					l_bal=vgl_mst.mdr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.mcr_bal,0.00))
				{
					l_bal=vgl_mst.mcr_bal;
				}
				else l_bal=0.00;
				break;
		case 4:				/* 本季	*/
				if(pub_base_CompDblVal(vgl_mst.qdr_bal,0.00))
				{
					l_bal=vgl_mst.qdr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.qcr_bal,0.00))
				{
					l_bal=vgl_mst.qcr_bal;
				}
				else l_bal=0.00;
				break;
		case 5:				/* 本年	*/
				if(pub_base_CompDblVal(vgl_mst.ydr_bal,0.00))
				{
					l_bal=vgl_mst.ydr_bal;
				}
				else if(pub_base_CompDblVal(vgl_mst.ycr_bal,0.00))
				{
					l_bal=vgl_mst.ycr_bal;
				}
				else l_bal=0.00;
				break;
		default:
				break;
	}		/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
/***************************************************************/
/*取贷方余额*/
char	*f_get_cc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 0));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_lc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 1));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_tc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 2));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_mc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 3));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_sc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 4));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_yc_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_c_bal(l_acchrt, 5));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}


/***************************************************************/
/* 取借方余额*/
char	*f_get_cd_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 0));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_ld_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 1));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_td_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 2));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_md_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 3));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
char	*f_get_sd_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 4));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

char	*f_get_yd_bal(char	*_acchrt)
{
	char	l_acchrt[8];
	char	l_s_bal[256];

	memset(l_acchrt, 0x00, sizeof(l_acchrt));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_acchrt, _acchrt);
	sprintf(l_s_bal,"%f", f_get_d_bal(l_acchrt, 5));
	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/*
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	借方余额
	科目号为双性科目
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
*/
double f_get_d_bal(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	double	l_bal = 0.00;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));

	strcpy(l_acchrt, _acchrt);

 	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
	 	goto ErrExit;
	}

	/* 业务机构 */
	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
			if(pub_base_CompDblVal(vgl_mst.dr_bal,0.00))
			{
				l_bal =vgl_mst.dr_bal;
			}
			else l_bal = 0.00;
			break;
		case 1:				/* 昨日 */
			if(pub_base_CompDblVal(vgl_mst.ldd_bal,0.00))
			{
				l_bal =vgl_mst.ldd_bal;
			}
			else l_bal = 0.00;
			break;
		case 2:				/* 本旬	*/
			if(pub_base_CompDblVal(vgl_mst.tddr_bal,0.00))
			{
				l_bal =vgl_mst.tddr_bal;
			}
			else l_bal = 0.00;
			break;
		case 3:				/* 本月	*/
			if(pub_base_CompDblVal(vgl_mst.mdr_bal,0.00))
			{
				l_bal =vgl_mst.mdr_bal;
			}
			else l_bal = 0.00;
			break;
		case 4:				/* 本季	*/
			if(pub_base_CompDblVal(vgl_mst.qdr_bal,0.00))
			{
				l_bal =vgl_mst.qdr_bal;
			}
			else l_bal = 0.00;
			break;
		case 5:				/* 本年		*/
			if(pub_base_CompDblVal(vgl_mst.ydr_bal,0.00))
			{
				l_bal =vgl_mst.ydr_bal;
			}
			else l_bal = 0.00;
			break;
		default:
			break;
	}		/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
/*
	(公共函数)根据科目(借、贷科目号)取余额或者期初余额
	贷方余额
	科目号为双性科目
	input	:
				char _acchrt[8]	科目号
				int	 _d_flag	发生额时期
						 	 0  当前余额(本日)
							 1  昨日
							 2  本旬
							 3  本月
							 4  本季
							 5  本年
	output	:
				char	l_s_amt	发生额
*/
double f_get_c_bal(char * _acchrt, int	_d_flag)
{
	char	l_acchrt[8];
	double	l_bal = 0.00;

	memset(&vgl_mst,0x00,sizeof(struct gl_mst_c));
	memset(l_acchrt, 0x00, sizeof(l_acchrt));

	strcpy(l_acchrt, _acchrt);

 	ret=Gl_mst_Sel(g_pub_tx.reply,&vgl_mst,"date=%ld and br_no='%s' and \
					cur_no='01' and acc_hrt='%s'",g_rept_date,g_brno_tmp.br_no, \
					l_acchrt);
	if(ret==100)
	{
	 	goto ErrExit;
	}

	/* 业务机构 */
	switch(_d_flag)
	{
		case 0:				/*  当前余额  (本日) */
				if(pub_base_CompDblVal(vgl_mst.cr_bal,0.00))
				{
					l_bal =vgl_mst.cr_bal;
				}
				else l_bal = 0.00;
				break;
		case 1:				/* 昨日     */
				if(pub_base_CompDblVal(vgl_mst.lcd_bal,0.00))
				{
					l_bal =vgl_mst.lcd_bal;
				}
				else l_bal = 0.00;
				break;
		case 2:				/* 本旬		*/
				if(pub_base_CompDblVal(vgl_mst.tdcr_bal,0.00))
				{
					l_bal =vgl_mst.tdcr_bal;
				}
				else l_bal = 0.00;
				break;
		case 3:				/* 本月		*/
				if(pub_base_CompDblVal(vgl_mst.mcr_bal,0.00))
				{
					l_bal =vgl_mst.mcr_bal;
				}
				else l_bal = 0.00;
				break;
		case 4:				/* 本季		*/
				if(pub_base_CompDblVal(vgl_mst.qcr_bal,0.00))
				{
					l_bal =vgl_mst.qcr_bal;
				}
				else l_bal = 0.00;
				break;
		case 5:				/* 本年		*/
				if(pub_base_CompDblVal(vgl_mst.ycr_bal,0.00))
				{
					l_bal =vgl_mst.ycr_bal;
				}
				else l_bal = 0.00;
				break;
		default:
				break;
	}/* end of switch */

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}
