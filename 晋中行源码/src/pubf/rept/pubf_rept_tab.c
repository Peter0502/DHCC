/*************************************************
* 文 件 名: pubf_rept_cal.c
* 功能描述: 日报表统计各类表函数
*
* 完成日期:   2004年05月04
* 修改记录:
*	 1. 日    期:
*		修 改 人:
*		修改内容:
*	 2.
*************************************************/

#define EXTERN
#define EXTERNREPT
#include <string.h>
#include "public.h"
#include "gl_prdt_dyn_c.h"
#include "pubf_rept_fun.h"
#include "gl_mst_c.h"

/***************************************
函数功能：统计总帐(GL_MSt)double型字段
参数：
    入口：	机构号 char *br_no_lst
			科目号 char *acc_hrt
    		字段名 char *sum_field
返回：统计结果
注：币种 只统计人民币
***************************************/
double sum_GlMst_double( char *br_no_lst, char *acc_hrt, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	if( strlen( br_no_lst ) )
	{
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}

	sprintf( tmpstr, "acc_hrt='%s' and cur_no='01'", acc_hrt );
	strcat( wherelist, tmpstr );

	g_reply_int = sql_sum_double( "gl_mst", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "   br_no[%s]", br_no_lst );
		vtcp_log( " acc_hrt[%s]", acc_hrt );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "总帐统计double型字段出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

/*******************************************
函数功能：统计现金台帐(CASH_MST)double型字段
参数：
    入口：	机构号 char *br_no_lst
			操作员 char *tel_lst
    		字段名 char *sum_field
返回：统计结果
注：币种 只统计人民币
***************************************/
double sum_CashMst_double( char *br_no_lst, char *tel_lst, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	if( strlen( br_no_lst ) )
	{
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	if( strlen( tel_lst ) )
	{
		sprintf( tmpstr, "tel in (%s) and ", tel_lst );
		strcat( wherelist, tmpstr );
	}
	strcat( wherelist, "cur_no='01'" );

	g_reply_int = sql_sum_double( "cash_mst", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "  br_no[%s]", br_no_lst );
		vtcp_log( "    tel[%s]", tel_lst );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "现金台帐统计double型字段出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

/*******************************************
函数功能：统计产品动态表(GL_PRDT_DYN)double型字段
参数：
    入口：	机构号 char *br_no_lst
			产品号 char *prdt_no_lst
    		字段名 char *sum_field
返回：统计结果
***************************************/
double sum_GlPrdtDyn_double( char *br_no_lst, char *prdt_no_lst, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	memset( wherelist, 0x00, sizeof( wherelist ) );
	if( strlen( br_no_lst ) )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	if( prdt_no_lst != '\0' )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr, "prdt_no in (%s) and ", prdt_no_lst );
		strcat( wherelist, tmpstr );
	}
	strcat( wherelist, "1=1" );

	vtcp_log("sum_field[%s]",sum_field);
	g_reply_int = sql_sum_double( "gl_prdt_dyn", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "    br_no[%s]", br_no_lst );
		vtcp_log( "  prdt_no[%s]", prdt_no_lst );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "产品动态表统计double型字段出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

/*******************************************
函数功能：统计考核总帐(ASS_GL)double型字段
参数：
    入口：	机构号 char *br_no_lst
			科目号 char *acc_hrt
    		字段名 char *sum_field
返回：统计结果
注：只统计状态为正常的记录
***************************************/
double sum_AssGl_double( char *br_no_lst, char *acc_hrt, char *sum_field )
{
	double _rtn_val;
	char tmpstr[1024];
	char wherelist[1024];

	if( strlen( br_no_lst ) )
	{
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	sprintf( tmpstr, "acc_hrt='%s' and sts='1'", acc_hrt );
	strcat( wherelist, tmpstr );

	g_reply_int = sql_sum_double( "ass_gl", sum_field, &_rtn_val, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "    br_no[%s]", br_no_lst );
		vtcp_log( "  acc_hrt[%s]", acc_hrt );
		vtcp_log( "sum_field[%s]", sum_field );
		sprintf( acErrMsg, "考核总帐统计double型字段出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	return _rtn_val;
ErrExit:
	_rtn_val = 0.0;
	return _rtn_val;
}

