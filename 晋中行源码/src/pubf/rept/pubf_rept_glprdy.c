/***************************************************************
* 文 件 名:     pubf_rept_glprdy.c
* 功能描述：
*					********  声明基础函数  *******
*				1.double f_get_bal_short(char * _prdt_no, int _d_flag);
*				2.char 	*f_get_current_bal_short(char *_prdt_no);
*				3.char 	*f_get_yesterday_bal_short(char *_prdt_no);
*				4.char 	*f_get_tenday_bal_short(char *_prdt_no);
*				5.char 	*f_get_month_bal_short(char *_prdt_no);
*				6.char 	*f_get_season_bal_short(char *_prdt_no);
*				7.char 	*f_get_year_bal_short(char *_prdt_no);
*				8.char  *f_get_current_d_amt_short(char *_prdt_no);
*				9.char  *f_get_current_c_amt_short(char *_prdt_no);
*				10.char *f_get_tenday_d_amt_short(char *_prdt_no);
*				11.char *f_get_tenday_c_amt_short(char *_prdt_no);
*				12.char *f_get_month_d_amt_short(char *_prdt_no);
*				13.char *f_get_month_c_amt_short(char *_prdt_no);
*				14.char *f_get_season_d_amt_short(char *_prdt_no);
*				15.char *f_get_season_c_amt_short(char *_prdt_no);
*				16.char *f_get_year_d_amt_short(char *_prdt_no);
*				17.char *f_get_year_c_amt_short(char *_prdt_no);
*
*				18.char *f_get_max_ln_bal();
*				19.char *f_get_maxsum_ln_bal();
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
#include "gl_prdt_dyn_c.h"
#include "com_branch_c.h"

struct  gl_prdt_dyn_c       vgl_prdt_dyn;
int	ret=0;

/************************************************************
	实现函数  根据科目取当前(本日)余额
	input	:
				int	_prdt_no	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
*************************************************************/
char *f_get_current_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 0));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目取昨日余额
	input	:
				int	_prdt_no	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_yesterday_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 1));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目取旬初余额
	input	:
				int	_prdt_no	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_tenday_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 2));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/***
	实现函数  根据科目号取月初余额
	input	:
				int		_prdt_no		科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal		月初余额
***/
char *f_get_month_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 3));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	实现函数  根据科目号取季初余额
	input	:
				int		_prdt_no		科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal		初余额
***/
char *f_get_season_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 4));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}
/***
	实现函数  根据科目取年初余额
	input	:
				int	_prdt_no	科目号(用整型，目的是调用方便)
	output	:
				char	l_s_bal	发生额/余额
***/
char *f_get_year_bal_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	char	l_dc[3];
	char	l_s_bal[256];

	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	memset(l_dc, 0x00, sizeof(l_dc));

	strcpy(l_prdt_no, _prdt_no);

	sprintf(l_s_bal,"%f", f_get_bal_short(l_prdt_no, 5));

	return l_s_bal;
ErrExit:
	strcpy(l_s_bal,"0.0");
	return l_s_bal;
}

/*
	(公共函数)根据产品号取余额或者期初余额
	根据机构号、科目号计算借方余额
	input	:
				char _prdt_no[8]	科目号
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
****/
double f_get_bal_short(char * _prdt_no, int	_d_flag)
{
	char	l_prdt_no[10];
	double	l_bal = 0.00;

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));

	strncpy(l_prdt_no, _prdt_no,3);

	/* 业务机构 */
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and \
						prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		switch(_d_flag)
		{
			case 0:				/* 当前余额 (本日) */
				l_bal+=vgl_prdt_dyn.cr_bal;
				break;
			case 1:				/* 昨日 */
				l_bal+=vgl_prdt_dyn.lcd_bal;
				break;
			case 2:				/* 本旬	*/
				l_bal+=vgl_prdt_dyn.tdcr_bal;
				break;
			case 3:				/* 本月	*/
				l_bal+=vgl_prdt_dyn.m_cr_bal;
				break;
			case 4:				/* 本季	*/
				l_bal+=vgl_prdt_dyn.q_cr_bal;
				break;
			case 5:				/* 本年	*/
				l_bal+=vgl_prdt_dyn.y_cr_bal;
				break;
			default:
				break;
		}
	}

	Gl_prdt_dyn_Clo_Sel();

	return l_bal;
ErrExit:
	l_bal = 0.00;
	return l_bal;
}

/***********************************************************************/
/*** 
	取借方发生额   本日
***/
char	*f_get_current_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and \
		prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.rdd_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*** 
	取贷方发生额   本日
***/
char	*f_get_current_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.rcd_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取借方发生额   本旬
***/
char	*f_get_tenday_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strncpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.td_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}


/*** 
	取贷方发生额   本旬
***/
char	*f_get_tenday_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strncpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.td_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*** 
	取借方发生额   本月
***/
char	*f_get_month_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strncpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.m_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	取贷方发生额   本月
***/
char	*f_get_month_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.m_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	取借方发生额   本季
***/
char	*f_get_season_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no,_prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.q_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	取贷方发生额   本月
***/
char	*f_get_season_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.q_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	取借方发生额   本年
***/
char	*f_get_year_d_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no, _prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.y_dr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}
/*** 
	取贷方发生额   本年
***/
char	*f_get_year_c_amt_short(char *_prdt_no)
{
	char	l_prdt_no[8];
	double	l_amt;
	char	l_s_amt[256];

	memset(&vgl_prdt_dyn, 0x00, sizeof(struct gl_prdt_dyn_c));
	memset(l_prdt_no, 0x00, sizeof(l_prdt_no));
	memset(l_s_amt, 0x00, sizeof(l_s_amt));

	strcpy(l_prdt_no,_prdt_no);
	
	ret=Gl_prdt_dyn_Dec_Sel(g_pub_tx.reply,"date=%ld and br_no='%s' \
		and prdt_no='%s'",g_rept_date,g_brno_tmp.br_no,l_prdt_no);
	if(ret)
	{
		goto ErrExit;
	}

	while(1)
	{
		ret=Gl_prdt_dyn_Fet_Sel(&vgl_prdt_dyn,g_pub_tx.reply);
		if(ret==100)
		{
			goto ErrExit;
		}

		l_amt+=vgl_prdt_dyn.y_cr_amt;
	}

	Gl_prdt_dyn_Clo_Sel();

	sprintf(l_s_amt, "%f", l_amt);
	return l_s_amt;
ErrExit:
	sprintf(l_s_amt, "%f", 0.00);
	return l_s_amt;
}

/*******************************************************************/
/*** 最高一笔贷款余额  ***/
char *f_get_max_ln_bal()
{
	double	l_bal;
	long	l_date_begin;
	long	l_date_end;
	char	l_s_bal[256];

	memset(l_s_bal, 0x00, sizeof(l_s_bal));
	l_date_end = g_rept_date;
	l_date_begin = g_rept_date/100 + 1;

	ret=sql_max_double("ln_mst","bal",&l_bal,"1=1");
	if(ret)
	{
		goto ErrExit;
	}

	sprintf(l_s_bal, "%f", l_bal);
	return l_s_bal;
ErrExit:
	sprintf(l_s_bal, "%f", 0.00);
	return l_s_bal;
}

/*** 根据产品号取得最多贷款 ***/
char *f_get_maxsum_ln_bal()
{
	double	l_bal;
	char	l_s_bal[256];

	memset(l_s_bal, 0x00, sizeof(l_s_bal));

	/*
	sprintf(l_sql, "select sum(bal) a1 from ln_mst where acchrt in (select acchrt from com_acc where curno[1]='7') group by acid order by a1 desc");

	EXEC SQL prepare qry_sumbal from :l_sql;
	MYSQLERR

	EXEC SQL declare sumbal_cur cursor for qry_sumbal;
	MYSQLERR

	EXEC SQL open sumbal_cur;
	MYSQLERR

	EXEC SQL fetch sumbal_cur into :l_bal :a2;
	MYSQLERR

	if(a2) l_bal = 0.00;

	sprintf(l_s_bal, "%f", l_bal);
	return l_s_bal;
ErrExit:
	sprintf(l_s_bal, "%f", 0.00);
	return l_s_bal;
}*/

	sprintf(l_s_bal, "%f", l_bal);
	return l_s_bal;
ErrExit:
	sprintf(l_s_bal, "%f", 0.00);
	return l_s_bal;
}
