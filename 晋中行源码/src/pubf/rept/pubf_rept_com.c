/*************************************************
* 文 件 名: pubf_rept_com.c
* 功能描述: 日报表常用基础函数
*			pub_rept_com_syer		取出表日期的年
*			pub_rept_com_smon		取出表日期的月
*			pub_rept_com_sday		取出表日期的日
*			pub_rept_com_ssea		取出表日期的季度
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
#include "pubf_rept_fun.h"

char *pub_rept_com_syer()
{
	char	l_return[256];
	sprintf(l_return, "%04d", g_rept_date/100/100);
	return l_return;
}
char *pub_rept_com_smon()
{
	char	l_return[256];
	sprintf(l_return, "%02d", g_rept_date/100%100);
	return l_return;
}
char *pub_rept_com_sday()
{
	char	l_return[256];
	sprintf(l_return, "%02d",g_rept_date%100);
	return l_return;
}
char *pub_rept_com_ssea()
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
char	*pub_rept_com_sbrname()
{
	char	l_name[41];
	struct com_branch_c vcom_branch;

	memset(l_name, 0x00, sizeof(l_name));
	memset(&vcom_branch, 0x00, sizeof(struct com_branch_c));

	g_reply_int=Com_branch_Sel(g_pub_tx.reply,&vcom_branch, \
					  "br_no='%s'",g_brno_tmp.br_no);
	if( 100 == g_reply_int )	
	{
		sprintf(acErrMsg,"no find BRNO[%s]",g_brno_tmp.br_no);
		WRITEMSG
		strcpy(l_name," ");
	}

	strcpy(l_name,vcom_branch.br_name);
	
	TRACE
	return l_name;
ErrExit:
	TRACE
	strcpy(l_name, " ");
	return l_name;
}
