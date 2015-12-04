/*************************************************
* 文 件 名: pubf_rept_cal.c
* 功能描述: 日报表计算函数
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
#include "gl_sub_c.h"
#include "gl_rpt_c.h"
#include "dda_zyywzk_c.h"
#include "trace_log_bk_c.h"
#include "cash_lmt_def_c.h"
#include "sum_tax_new_c.h"
#include "com_sys_parm_c.h"
#include "accountreg_c.h"
#include "in_mst_c.h"
#include "zjgl_mst_c.h"
#include "ass_mst_c.h"
#include "gl_tsub_c.h"
#include "trace_log_rz_c.h"

	double sum_GlPrdtDyn_double( char *br_no_cond, char *prdt_no_lst, char *field );
	double sum_AssGl_double( char *br_no_cond, char *ass_code, char *field );
	double sum_CashMst_double( char *br_no_cond, char *cur_no_v, char *field );
/**************************************************
函数功能: 根据产品号从sum_tax表中取得利息和抠缴人次
应用报表:存款利息所得抠缴个人所得税月报告表
参数:  func 函数名
		   prdt 产品号

返回:
	Jarod 
**************************************************/
char *pub_rept_get_tax(char *func,char *prdt,char *br_no,char flag)
{
	int ret=0;
	long rq1=0,rq2=0;
	struct sum_tax_new_c sSum_tax_new;
	struct com_sys_parm_c sCom_sys_parm;
	char result[18];
	char wherelist[100];
	char tablename[100];
	double sum_double;
	long sum_long;
	
	memset(&sum_long,0x00,sizeof(long));
	memset(&sum_double,0x00,sizeof(double));
	memset(tablename,0x00,sizeof(tablename));
	memset(wherelist,0x00,sizeof(wherelist));
	memset(result,0x0,sizeof(result));	
	memset(&sCom_sys_parm,0x0,sizeof(struct com_sys_parm_c));
	memset(&sSum_tax_new,0x0,sizeof(struct sum_tax_new_c));

/*根据当前系统日期取当月的上下限*/
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(ret)
	{
		sprintf(acErrMsg,"error code is[%d]",ret);
		WRITEMSG
		goto ErrExit;	
	}
	g_pub_tx.tx_date=sCom_sys_parm.lst_date;
	rq1=g_pub_tx.tx_date/100*100+1;
	rq2=g_pub_tx.tx_date/100*100+32;
	
	/*
	if(flag=='1')
	sprintf(wherelist,"prdt_no='%s' and br_no='%s' and intst_tax_rate=0.2",prdt,br_no);
	else if(flag=='0')
	sprintf(wherelist,"prdt_no='%s' and br_no='%s' and intst_tax_rate=0.05",prdt,br_no);
	*/
	if(flag=='0')
	{
		if(!strncmp(func,"LX1",3))
		{
		sprintf(wherelist,"prdt_no='%s' and br_no='%s' and intst_tax_rate=0.05 and tax=0 ",prdt,br_no);
		}
		else
		{
		sprintf(wherelist,"prdt_no='%s' and br_no='%s' and intst_tax_rate=0.05 and tax!=0 ",prdt,br_no);
		}
	}
	else if(flag=='1')
	{
		if(!strncmp(func,"LX1",3))
		{
		sprintf(wherelist,"prdt_no='%s' and br_no='%s' and intst_tax_rate=0.2 and tax=0 ",prdt,br_no);
		}
		else
		{
		sprintf(wherelist,"prdt_no='%s' and br_no='%s' and intst_tax_rate=0.2 and tax!=0 ",prdt,br_no);
		}
	}
	sprintf(tablename,"sum_tax_new");
vtcp_log("flagis[%c],wherelist:[%s] [%s]--[%d]",flag,wherelist,__FILE__,__LINE__);

	/*取利息税人次*/
	if(!strncmp(func,"LXRC",4))
	{
		if( !strncmp(prdt,"101",3) || !strncmp(prdt,"102",3))
		{
			ret=sql_sum_long(tablename,"cnt",&sum_long,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总cnt出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%ld",sum_long);
				return(result);	
		}
		if( !strncmp(prdt,"2",1))
		{
			ret=sql_sum_long(tablename,"cnt",&sum_long,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总cnt出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%ld",sum_long);
				return(result);	
		}
 
	}
	
	/*取利息*/
	if( !strncmp(func,"LX",2) )
	{
		if( !strncmp(prdt,"101",3) || !strncmp(prdt,"102",3))
		{
			ret=sql_sum_double(tablename,"intst",&sum_double,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总intst利息字段出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%.2lf",sum_double);
				return(result);	
		}
		if( !strncmp(prdt,"2",1))
		{
			ret=sql_sum_double(tablename,"intst",&sum_double,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总intst利息字段出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%.2lf",sum_double);
				return(result);	
		}
	}
	
	 /*取不缴税利息*/
	if( !strncmp(func,"LX1",3) )
	{
		if( !strncmp(prdt,"2",1))
		{
			
			ret=sql_sum_double(tablename,"intst",&sum_double,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总intst利息字段出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%.2lf",sum_double);
				return(result);	
		}
	}
	/*取所得税字段*/
	if( !strncmp(func,"SDS",3) )
	{
		if( !strncmp(prdt,"101",3) || !strncmp(prdt,"102",3)|| !strncmp(prdt,"103",3))
		{
			ret=sql_sum_double(tablename,"tax",&sum_double,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总tax所得税字段出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%.2lf",sum_double);
				return(result);	
		}
		if( !strncmp(prdt,"2",1))
		{
			ret=sql_sum_double(tablename,"tax",&sum_double,wherelist);
			if(ret==-1)
			{
				sprintf(acErrMsg,"汇总tax所得税字段出错!!");
				WRITEMSG
				goto ErrExit;
			}
				sprintf(result,"%.2lf",sum_double);
				return(result);	
		}
	}
ErrExit:
	return(0);
}
/*************************************
函数功能：计算
		  主要应用报表：现金收支统计
add by martin
参数：
    入口：方法 char *xjtj
		  代号 char *xjname
		  行号 g_brno_tmp.br_no
返回：
*************************************/
char *pub_rept_XJTJ( char *xjtj, char *xjname )
{
	struct trace_log_rz_c s_trace_log_rz;
	double amt=0.00;
	int ret=0;
	static char sum[20];
	amt=0.0;
	ret=0;
	memset(sum,0x0,sizeof(sum));
	memset( &s_trace_log_rz, 0x00, sizeof( s_trace_log_rz ) );
	long date_min,date_max;

	date_min = g_rept_date/100;
	date_min = date_min*100;
	date_max = date_min+99;
	
	
	if( !strncmp("XJTJ",xjtj,4))
		{
		vtcp_log("before sql_sum_double[%lf]__[%s]__[%d]__", amt ,__FILE__,__LINE__);
		
	  ret=sql_sum_double ( "trace_log_rz" , "amt" , &amt ,"brf='%s' and sts='0' and tx_br_no='%s' and tx_date>%ld and tx_date<%ld ",xjname, g_brno_tmp.br_no , date_min, date_max );
	 /*  \"date\">%ld and \"date\"<%ld", date_min, date_max
	 ret=sql_sum_double ( "trace_log_rz" , "amt" , &amt ,"brf='%s' and sts='0'  ",xjname );
	 */
	  if(ret && ret!=100)
	  {
	 	 vtcp_log("err sql_sum_double.ret[%d]__[%s]__[%d]__", ret ,__FILE__,__LINE__);
	   goto ErrExit;
	  }
	  vtcp_log("out sql_sum_double[%lf]__[%s]__[%d]__", amt ,__FILE__,__LINE__);
	 
	  sprintf(sum,"%.2lf",amt);
	   return sum;
  }else
  goto ErrExit;

ErrExit:
	return(0);

}
/*************************************
函数功能：计算
		  主要应用报表：同城清算轧差
add by martin
参数：
    入口：方法 char *tcqs
		  代号 char *cash_chg_no
		  行号 g_brno_tmp.br_no
返回：
*************************************/
char *pub_rept_TCQS( char *tcqs, char *cash_chg_no )
{
	struct accountreg_c s_accountreg;
	double amt=0.00;
	int ret=0;
	static char cout[5];
	static char sum[20];
	amt=0.0;
	ret=0;
	memset(cout,0x0,sizeof(cout));
	memset(sum,0x0,sizeof(sum));
	memset( &s_accountreg, 0x00, sizeof( s_accountreg ) );
	
	if( !strncmp("TCQSC",tcqs,5))
	{
		vtcp_log("before tx_date='%d' sql_count.ret=[%ld]__[%s]__[%d]__", g_rept_date ,ret,__FILE__,__LINE__);
		ret=sql_count( "accountreg" , "tx_date='%d' and put_flag='1' and stat='2'  and cash_chg_no='%s' ",g_rept_date,cash_chg_no );

	 	  vtcp_log("out from function tx_date='%d' sql_count.ret=[%ld]__[%s]__[%d]__", g_rept_date ,ret,__FILE__,__LINE__);
	 	 
	 	 sprintf(cout,"%d",ret);
	   return cout;
	  
	}else if( !strncmp("TCQSS",tcqs,5))
		{
		vtcp_log("before sql_sum_double[%lf]__[%s]__[%d]__", amt ,__FILE__,__LINE__);
		/*
	  ret=sql_sum_double ( "accountreg" , "amt" , &amt ,"tx_date='%d' and put_flag='1' and stat='2' and br_no='%s' and cash_chg_no='%s' ",g_rept_date, g_brno_tmp.br_no,cash_chg_no );
	 */
	 ret=sql_sum_double ( "accountreg" , "amt" , &amt ,"tx_date='%d' and put_flag='1' and stat='2'  and cash_chg_no='%s' ",g_rept_date,cash_chg_no );
	  if(ret && ret!=100)
	  {
	 	 vtcp_log("sql_sum_double.ret[%d]__[%s]__[%d]__", ret ,__FILE__,__LINE__);
	   goto ErrExit;
	  }
	  vtcp_log("out sql_sum_double[%lf]__[%s]__[%d]__", amt ,__FILE__,__LINE__);
	 
	  sprintf(sum,"%.2lf",amt);
	   return sum;
  }else
  goto ErrExit;

ErrExit:
	return(0);
}
/*************************************
函数功能：计算
		  主要应用报表：存款余额日报表
参数：
    入口：机构号 char *br_no
		  科目号 char *acc_hrt
返回：
*************************************/
char *pub_rept_cal_zhkmye( char *br_no, char *acc_hrt )
{
	struct gl_mst_c sGlMst;
	struct gl_sub_c sGlSub;
	double _rtn_bal_f;
	char _rtn_bal_s[256];

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	if( !strcmp( br_no, "61000" ) )
	{
		g_reply_int = Gl_mst_Sel( g_pub_tx.reply, &sGlMst, 
			"br_no='%s' and cur_no='01' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

		if( sGlMst.dc_ind[0] == '1' )
			_rtn_bal_f = sGlMst.dr_bal;
		else if( sGlMst.dc_ind[0] == '2' )
			_rtn_bal_f = sGlMst.cr_bal;
		else if( sGlMst.dc_ind[0] == '0' )
			_rtn_bal_f = sGlMst.dr_bal - sGlMst.cr_bal;
	}
	else
	{
		g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub, 
			"br_no='%s' and cur_no='01' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

		if( sGlSub.dc_ind[0] == '1' )
			_rtn_bal_f = sGlSub.dr_bal;
		else if( sGlSub.dc_ind[0] == '2' )
			_rtn_bal_f = sGlSub.cr_bal;
		else if( sGlSub.dc_ind[0] == '0' )
			_rtn_bal_f = sGlSub.dr_bal - sGlSub.cr_bal;
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：该机构本日存款余额
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_bsrzhkmye( char *br_no, char *acc_hrt )
{
	struct gl_mst_c sGlMst;
	struct gl_sub_c sGlSub;
	double _rtn_bal_f;
	char _rtn_bal_s[256];

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	if( !strcmp( br_no, "61000" ) )
	{
		g_reply_int = Gl_mst_Sel( g_pub_tx.reply, &sGlMst, "br_no='%s' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

		if( sGlMst.dc_ind[0] == '1' )
			_rtn_bal_f = sGlMst.dr_bal - sGlMst.ldd_bal;
		else if( sGlMst.dc_ind[0] == '2' )
			_rtn_bal_f = sGlMst.cr_bal - sGlMst.lcd_bal;
		else if( sGlMst.dc_ind[0] == '0' )
			_rtn_bal_f = ( ( sGlMst.dr_bal - sGlMst.ldd_bal ) - ( sGlMst.cr_bal - sGlMst.lcd_bal ) );
	}
	else
	{
		g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub, "br_no='%s' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

		if( sGlSub.dc_ind[0] == '1' )
			_rtn_bal_f = sGlSub.dr_bal - sGlSub.ldd_bal;
		else if( sGlSub.dc_ind[0] == '2' )
			_rtn_bal_f = sGlSub.cr_bal - sGlSub.lcd_bal;
		else if( sGlSub.dc_ind[0] == '0' )
			_rtn_bal_f = ( ( sGlSub.dr_bal - sGlSub.ldd_bal ) - ( sGlSub.cr_bal - sGlSub.lcd_bal ) );
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：该机构本日存款余额
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_bnczhkmye( char *br_no, char *acc_hrt )
{
	struct gl_mst_c sGlMst;
	struct gl_sub_c sGlSub;
	double _rtn_bal_f;
	char _rtn_bal_s[256];

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	if( !strcmp( br_no, "61000" ) )
	{
		g_reply_int = Gl_mst_Sel( g_pub_tx.reply, &sGlMst, "br_no='%s' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

		if( sGlMst.dc_ind[0] == '1' )
			_rtn_bal_f = sGlMst.dr_bal - sGlMst.ydr_bal;
		else if( sGlMst.dc_ind[0] == '2' )
			_rtn_bal_f = sGlMst.cr_bal - sGlMst.ycr_bal;
		else if( sGlMst.dc_ind[0] == '0' )
			_rtn_bal_f = ( ( sGlMst.dr_bal - sGlMst.ydr_bal ) - ( sGlMst.cr_bal - sGlMst.ycr_bal ) );
	}
	else
	{
		g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlSub, "br_no='%s' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

		if( sGlSub.dc_ind[0] == '1' )
			_rtn_bal_f = sGlSub.dr_bal - sGlSub.ydr_bal;
		else if( sGlSub.dc_ind[0] == '2' )
			_rtn_bal_f = sGlSub.cr_bal - sGlSub.ycr_bal;
		else if( sGlSub.dc_ind[0] == '0' )
			_rtn_bal_f = ( ( sGlSub.dr_bal - sGlSub.ydr_bal ) - ( sGlSub.cr_bal - sGlSub.ycr_bal ) );
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}
/*************************************
函数功能：计算支行及下属机构的存款科目月平均
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_zhkmypj1( char *up_br_no, char *main_acc )
{
	long date_min,date_max;
	long date_cnt;
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];

vtcp_log( "计算   [%s]   支行   [%s]   科目月平均", up_br_no, main_acc );

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );
	memset( wherelist, 0x00, sizeof( wherelist ) );

	date_min = g_rept_date / 100;
	date_min = date_min * 100;
	date_max = date_min + 99;

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( tmpstr, "gl_prdt_dyn_hst.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "gl_prdt_dyn_hst.\"date\">%ld and gl_prdt_dyn_hst.\"date\"<%ld and ", date_min, date_max );
	strcat( wherelist, tmpstr );

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn_hst:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dd_parm,dc_acc_rel", "gl_prdt_dyn_hst.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn_hst:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,td_parm,dc_acc_rel", "gl_prdt_dyn_hst.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	date_cnt = count_distinct( "gl_prdt_dyn_hst", "\"date\"", "\"date\">%ld and \"date\"<%ld", date_min, date_max );
	vtcp_log( "date_cnt[%ld]", date_cnt );
OkExit:
	sprintf( _rtn_bal_s, "%.2lf", (_ret_bal_f_dd+_ret_bal_f_td)/date_cnt );
	vtcp_log("[%lf][%lf][%s]",_ret_bal_f_dd,_ret_bal_f_td,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的存款科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_zhkmye1( char *up_br_no, char *main_acc )
{
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];

vtcp_log( "计算   [%s]   支行   [%s]   科目余额", up_br_no, main_acc );

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );
	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		if( !strcmp( main_acc, "101" ) )
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		else
			sprintf( tmpstr, "gl_prdt_dyn.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*库存现金统计*/
	if( !strcmp( main_acc, "101" ) )
	{
		strcat( wherelist, "cur_no='01'" );
		g_reply_int = sql_sum_double( "cash_mst", "bal", &_ret_bal_f_dd, wherelist );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "统计[%s]机构现金台账表出错![%ld]", up_br_no, g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		goto OkExit;
	}

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "dd_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "td_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _ret_bal_f_dd+_ret_bal_f_td );
	vtcp_log("[%lf][%lf][%s]",_ret_bal_f_dd,_ret_bal_f_td,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：该机构本日存款余额
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_bsrzhkmye1( char *up_br_no, char *main_acc )
{
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];
	double cr_bal_v, lcd_bal_v;
	long lst_date;

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

vtcp_log( "计算   [%s]   支行   [%s]   本日科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		if( !strcmp( main_acc, "101" ) )
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		else
			sprintf( tmpstr, "gl_prdt_dyn.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "dd_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	/*sprintf( tmpstr, "td_parm.dc_code[1,3]='%s' and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );*/
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	/*g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );*/
	g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	cr_bal_v = _ret_bal_f_dd + _ret_bal_f_td;

vtcp_log( "计算   [%s]   支行   [%s]   昨日科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		lst_date = pub_base_daynumSUB( g_rept_date, 1 );
		sprintf( tmpstr, "gl_prdt_dyn_hst.\"date\"='%ld' and gl_prdt_dyn_hst.br_no in (%s) and ", 
			lst_date, br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn_hst.prdt_cod=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn_hst:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,dd_parm,dc_acc_rel", "gl_prdt_dyn_hst.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn_hst.prdt_cod=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn_hst:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn_hst,td_parm,dc_acc_rel", "gl_prdt_dyn_hst.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	lcd_bal_v = _ret_bal_f_dd + _ret_bal_f_td;    /*昨日科目余额*/

	sprintf( _rtn_bal_s, "%.2lf", cr_bal_v - lcd_bal_v );
	vtcp_log("[%lf]-[%lf]=[%s]", cr_bal_v, lcd_bal_v, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：计算支行及下属机构的科目余额
		  主要应用报表：存款余额日报表
参数：
    入口：上级机构号 char *up_br_no
		  科目号     char *main_acc
返回：该机构本日存款余额
注：统计机构包括up_br_no及up_br_no的下级机构,如果机构号为ALL则统计全行
	统计科目包括所有前三位为main_acc的科目控制字
*************************************/
char *pub_rept_cal_bnczhkmye1( char *up_br_no, char *main_acc )
{
	double _rtn_bal_f;
	double _ret_bal_f_dd;
	double _ret_bal_f_td;
	char _rtn_bal_s[256];
	char br_no_lst[1024];
	char prdt_no_lst[1024];
	struct com_branch_c sComBranch;
	struct dda_zyywzk_c sDdaZyywzk;
	char tmpstr[1024];
	char wherelist[1024];
	char wherelist_tmp[1024];
	double cr_bal_v, y_cr_bal_v;
	long lst_date;

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

vtcp_log( "计算   [%s]   支行   [%s]   本日科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		if( !strcmp( main_acc, "101" ) )
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		else
			sprintf( tmpstr, "gl_prdt_dyn.br_no in (%s) and ", br_no_lst );

		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*存款科目统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and dd_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=dd_parm.prdt_no", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn,dd_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_dd, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计活期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dc_acc_rel.acc_hrt like '%s%%%%' and td_parm.dc_code=dc_acc_rel.dc_code and gl_prdt_dyn.prdt_no=td_parm.prdt_no", main_acc );
	strcpy( wherelist, wherelist_tmp );
	strcat( wherelist, tmpstr );

	vtcp_log("sum gl_prdt_dyn:cr_bal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "gl_prdt_dyn,td_parm,dc_acc_rel", "gl_prdt_dyn.cr_bal", &_ret_bal_f_td, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计定期存款余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	cr_bal_v = _ret_bal_f_dd + _ret_bal_f_td;

vtcp_log( "计算   [%s]   支行   [%s]   年初科目余额", up_br_no, main_acc );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s'", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();

		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( tmpstr, "dda_zyywzk.inst_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}

	strcpy( wherelist_tmp, wherelist );

	/*年初科目余额统计*/
	memset( tmpstr, 0x00, sizeof( tmpstr ) );
	sprintf( tmpstr, "dda_zyywzk.item_no like '%s%%%%'", main_acc );
	strcat( wherelist, tmpstr );

	vtcp_log("sum dda_zyywzk:lst_ybal wherelist[%s]",wherelist);
	g_reply_int = sql_sum_double( "dda_zyywzk", "lst_ybal", &y_cr_bal_v, wherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计年初余额出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( _rtn_bal_s, "%.2lf", cr_bal_v - y_cr_bal_v );
	vtcp_log("[%lf]-[%lf]=[%s]", cr_bal_v, y_cr_bal_v, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/***************************************************
函数功能：统计主要业务状况日报表
参数：
    入口：支行管理机构机构号	char *up_br_no
    	  统计项目				char *item
返回：该支行各项目余额
***************************************************/
char *pub_rept_cal_zyywzkrbb( char *up_br_no, char *item )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];
	char br_no_lst[256];
	char sum_field[100];
	char tmpstr[1024];
	char wherelist[1024];
	struct com_branch_c sComBranch;

	switch( item[0] )
	{
/*存款部分*/
		case 'A': strcpy( sum_field, "cr_de_bal" ); break;    /*存款总额*/
		case 'B': strcpy( sum_field, "cr_de_bal-ys_de_bal" ); break;
		case 'C': strcpy( sum_field, "cr_de_bal-yb_de_bal" ); break;
		case 'D': strcpy( sum_field, "cr_de_bal1" ); break;
		case 'E': strcpy( sum_field, "cr_de_bal1-ys_de_bal1" ); break;
		case 'F': strcpy( sum_field, "cr_de_bal1-yb_de_bal1" ); break;
		case 'G': strcpy( sum_field, "cr_de_bal2" ); break;
		case 'H': strcpy( sum_field, "cr_de_bal2-ys_de_bal2" ); break;
		case 'I': strcpy( sum_field, "cr_de_bal2-yb_de_bal2" ); break;
		case 'J': strcpy( sum_field, "cr_de_bal3" ); break;
		case 'K': strcpy( sum_field, "cr_de_bal3-ys_de_bal3" ); break;
		case 'L': strcpy( sum_field, "cr_de_bal3-yb_de_bal3" ); break;
		case 'M': strcpy( sum_field, "cr_de_bal4" ); break;
		case 'N': strcpy( sum_field, "cr_de_bal4-ys_de_bal4" ); break;
		case 'O': strcpy( sum_field, "cr_de_bal4-yb_de_bal4" ); break;
		case 'P': strcpy( sum_field, "cr_de_bal5" ); break;
		case 'Q': strcpy( sum_field, "cr_de_bal5-ys_de_bal5" ); break;
		case 'R': strcpy( sum_field, "cr_de_bal5-yb_de_bal5" ); break;
		case 'S': strcpy( sum_field, "cr_de_bal6" ); break;
		case 'T': strcpy( sum_field, "cr_de_bal6-ys_de_bal6" ); break;
		case 'U': strcpy( sum_field, "cr_de_bal6-yb_de_bal6" ); break;

		case 'Z': strcpy( sum_field, "yb_de_bal" ); break;
/*贷款部分*/
		case 'a': strcpy( sum_field, "cr_ln_bal" ); break;
		case 'b': strcpy( sum_field, "cr_ln_bal-ys_ln_bal" ); break;
		case 'c': strcpy( sum_field, "cr_ln_bal-yb_ln_bal" ); break;
		case 'd': strcpy( sum_field, "cr_ln_bal1" ); break;
		case 'e': strcpy( sum_field, "cr_ln_bal1-ys_ln_bal1" ); break;
		case 'f': strcpy( sum_field, "cr_ln_bal1-yb_ln_bal1" ); break;
		case 'g': strcpy( sum_field, "cr_ln_bal2" ); break;
		case 'h': strcpy( sum_field, "cr_ln_bal2-ys_ln_bal2" ); break;
		case 'i': strcpy( sum_field, "cr_ln_bal2-yb_ln_bal2" ); break;
		case 'j': strcpy( sum_field, "cr_ln_bal3" ); break;
		case 'k': strcpy( sum_field, "cr_ln_bal3-ys_ln_bal3" ); break;
		case 'l': strcpy( sum_field, "cr_ln_bal3-yb_ln_bal3" ); break;
		case 'm': strcpy( sum_field, "cr_ln_bal4" ); break;
		case 'n': strcpy( sum_field, "cr_ln_bal4-ys_ln_bal4" ); break;
		case 'o': strcpy( sum_field, "cr_ln_bal4-yb_ln_bal4" ); break;
		case 'p': strcpy( sum_field, "cr_ln_bal5" ); break;
		case 'q': strcpy( sum_field, "cr_ln_bal5-ys_ln_bal5" ); break;
		case 'r': strcpy( sum_field, "cr_ln_bal5-yb_ln_bal5" ); break;
		default: goto ErrExit;
	}
	vtcp_log( "统计项目：[%s]", sum_field );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s' order by br_no", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();
		if( !strcmp( up_br_no, "61099" ) )
			sprintf( tmpstr, "br_no in ('%s') and ", up_br_no );
		else
			sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	sprintf( tmpstr, "\"date\"='%ld'", g_rept_date );
	strcat( wherelist, tmpstr );

	vtcp_log( "SUM gl_oprt_dyn wherelist\n%s\n", wherelist );
	g_reply_int = sql_sum_double( "gl_oprt_dyn", sum_field, &_rtn_bal_f, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( acErrMsg, "主要业务状况日报表[%s]出错![%ld]", item, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]",_rtn_bal_f, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/***************************************************
函数功能：统计主要业务状款月报表
参数：
    入口：支行管理机构机构号	char *up_br_no
    	  统计项目				char *item
返回：该支行各项存款余额(n.nn字符串)
***************************************************/
char *pub_rept_cal_zyywzkybb( char *up_br_no, char *item )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];
	char br_no_lst[256];
	char sum_field[100];
	char tmpstr[1024];
	char wherelist[1024];
	struct com_branch_c sComBranch;
	long date_min,date_max;

	date_min = g_rept_date/100;
	date_min = date_min*100;
	date_max = date_min+99;
	switch( item[0] )
	{
/*存款部分*/
/*年初*/
		case 'A': strcpy( sum_field, "yb_de_bal" ); break;
		case 'D': strcpy( sum_field, "yb_de_bal1" ); break;
		case 'G': strcpy( sum_field, "yb_de_bal2" ); break;
		case 'J': strcpy( sum_field, "yb_de_bal3" ); break;
		case 'M': strcpy( sum_field, "yb_de_bal4" ); break;
		case 'P': strcpy( sum_field, "yb_de_bal5" ); break;
		case 'S': strcpy( sum_field, "yb_de_bal6" ); break;
/*月末*
		case 'B': strcpy( sum_field, "ys_de_bal" ); break;
		case 'E': strcpy( sum_field, "ys_de_bal1" ); break;
		case 'H': strcpy( sum_field, "ys_de_bal2" ); break;
		case 'K': strcpy( sum_field, "ys_de_bal3" ); break;
		case 'N': strcpy( sum_field, "ys_de_bal4" ); break;
		case 'Q': strcpy( sum_field, "ys_de_bal5" ); break;
		case 'T': strcpy( sum_field, "ys_de_bal6" ); break;
*/
/*月日均*/
		case 'C': strcpy( sum_field, "cr_de_bal" ); break;
		case 'F': strcpy( sum_field, "cr_de_bal1" ); break;
		case 'I': strcpy( sum_field, "cr_de_bal2" ); break;
		case 'L': strcpy( sum_field, "cr_de_bal3" ); break;
		case 'O': strcpy( sum_field, "cr_de_bal4" ); break;
		case 'R': strcpy( sum_field, "cr_de_bal5" ); break;
		case 'U': strcpy( sum_field, "cr_de_bal6" ); break;

/*贷款部分*/
/*年初*/
		case 'a': strcpy( sum_field, "yb_ln_bal" ); break;
		case 'd': strcpy( sum_field, "yb_ln_bal1" ); break;
		case 'g': strcpy( sum_field, "yb_ln_bal2" ); break;
		case 'j': strcpy( sum_field, "yb_ln_bal3" ); break;
		case 'm': strcpy( sum_field, "yb_ln_bal4" ); break;
		case 'p': strcpy( sum_field, "yb_ln_bal5" ); break;
/*月末*
		case 'b': strcpy( sum_field, "ys_ln_bal" ); break;
		case 'e': strcpy( sum_field, "ys_ln_bal1" ); break;
		case 'h': strcpy( sum_field, "ys_ln_bal2" ); break;
		case 'k': strcpy( sum_field, "ys_ln_bal3" ); break;
		case 'n': strcpy( sum_field, "ys_ln_bal4" ); break;
		case 'q': strcpy( sum_field, "ys_ln_bal5" ); break;
*/
/*月日均*/
		case 'c': strcpy( sum_field, "cr_ln_bal" ); break;
		case 'f': strcpy( sum_field, "cr_ln_bal1" ); break;
		case 'i': strcpy( sum_field, "cr_ln_bal2" ); break;
		case 'l': strcpy( sum_field, "cr_ln_bal3" ); break;
		case 'o': strcpy( sum_field, "cr_ln_bal4" ); break;
		case 'r': strcpy( sum_field, "cr_ln_bal5" ); break;
		default: goto ErrExit;
	}
	vtcp_log( "统计项目：[%s]", sum_field );

	memset( wherelist, 0x00, sizeof( wherelist ) );

	if( strcmp( up_br_no, "ALL" ) )
	{
		memset( br_no_lst, 0x00, sizeof( br_no_lst ) );
		sprintf( br_no_lst, "'%s'", up_br_no );
		g_reply_int = Com_branch_Dec_Sel( g_pub_tx.reply, "up_br_no='%s' order by br_no", up_br_no );
		if( g_reply_int )
		{
			vtcp_log( "up_br_no[%s]", up_br_no );
			sprintf( acErrMsg, "声明查询com_branch游标出错![%ld]", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		while(1)
		{
			memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
			g_reply_int = Com_branch_Fet_Sel( &sComBranch, g_pub_tx.reply );
			if( g_reply_int == 100 ) break;
			if( g_reply_int )
			{
				vtcp_log( "up_br_no[%s]", up_br_no );
				sprintf( acErrMsg, "取查询com_branch游标出错![%ld]", g_reply_int );
				WRITEMSG
				goto ErrExit;
			}
			strcat( br_no_lst, ",'" );
			strcat( br_no_lst, sComBranch.br_no );
			strcat( br_no_lst, "'" );
		}
		Com_branch_Clo_Sel();
		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( tmpstr, "br_no in (%s) and ", br_no_lst );
		strcat( wherelist, tmpstr );
	}
	if( !strncmp( sum_field, "yb_", 3 ) )
		sprintf( tmpstr, "\"date\"='%ld'", g_rept_date );
	else if( !strncmp( sum_field, "cr_", 3 ) )
		sprintf( tmpstr, "\"date\"<'%ld' and \"date\">'%ld'", date_max, date_min );
	strcat( wherelist, tmpstr );

	g_reply_int = sql_sum_double( "gl_oprt_dyn", sum_field, &_rtn_bal_f, wherelist );
	if( g_reply_int )
	{
		vtcp_log( "br_no_lst[%s]", br_no_lst );
		sprintf( acErrMsg, "统计个人贷款余额(比昨日)出错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( !strncmp( sum_field, "cr_", 3 ) )
	_rtn_bal_f = _rtn_bal_f/(g_rept_date%100);

	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]",_rtn_bal_f, _rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：取库存现金限额
		  应用报表：库存现金限额核定表(bb_d_kcxjxehdb.rfc)
参数：
    入口：机构号 char *br_no
返回：    机构库存现金限额
*************************************/
char *pub_rept_cal_kcxjxe( char *br_no )
{
	struct cash_lmt_def_c sCashLmtDef;
	double _rtn_bal_f;
	char _rtn_bal_s[256];

	memset( &sCashLmtDef, 0x00, sizeof( struct cash_lmt_def_c ) );
	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	g_reply_int = Cash_lmt_def_Sel( g_pub_tx.reply, &sCashLmtDef, "br_no='%s'", br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取[%s]机构库存限额出错!!", br_no );
		WRITEMSG
		goto ErrExit;
	}

	_rtn_bal_f = sCashLmtDef.lmt_amt;

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：取库存现金实际数
		  应用报表：库存现金限额核定表(bb_d_kcxjxehdb.rfc)
参数：
    入口：机构号 char *br_no
返回：    机构库存现金实际数
*************************************/
char *pub_rept_cal_kcs( char *br_no )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	g_reply_int = sql_sum_double( "cash_mst", "bal", &_rtn_bal_f, "br_no='%s'", br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计[%s]库存数出错![%ld]", br_no, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：机构当日上介数
		  应用报表：库存现金限额核定表(bb_d_kcxjxehdb.rfc)
参数：
    入口：机构号 char *br_no
返回：    机构当日上介数
*************************************/
char *pub_rept_cal_sjs( char *br_no )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	g_reply_int = sql_sum_double( "dc_log_bk", "amt", &_rtn_bal_f,  
		"tx_tx_br_no='%s' and entry_code='M20200' and acc_hrt='10104' and dc_ind='1'", br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计[%s]上介数出错![%ld]", br_no, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}

/*************************************
函数功能：机构当日上介时间
		  应用报表：库存现金限额核定表(bb_d_kcxjxehdb.rfc)
参数：
    入口：机构号 char *br_no
返回：    机构当日上介时间
*************************************/
char *pub_rept_cal_sjsj( char *br_no )
{
	struct trace_log_bk_c sTraceLogBk;
	int hour,second;
	int trace_no_max;
	long _tx_time_d;
	char _tx_time_s[256];
	double _rtn_bal_f;

	vtcp_log("wjwjwj " );
	memset( _tx_time_s, 0x00, sizeof( _tx_time_s ) );

	g_reply_int = sql_sum_double( "dc_log_bk", "amt", &_rtn_bal_f,  
		"tx_tx_br_no='%s' and entry_code='M20200' and acc_hrt='10104' and dc_ind='1'", br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计[%s]上介数数据库操作出错![%ld]", br_no, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("wjwjwj " );
	g_reply_int = pub_base_CompDblVal( _rtn_bal_f, 0.00 );
	if( g_reply_int == 0 )
		goto ErrExit;
	else if( g_reply_int < 0 )
	{
		sprintf( acErrMsg, "统计[%s]上介数小于零![%lf]", br_no, _rtn_bal_f );
		WRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int > 0 )
	{
		/*取最大流水号*/
		trace_no_max = sql_max_int( "dc_log_bk", "trace_no",  
		"tx_tx_br_no='%s' and entry_code='M20200' and acc_hrt='10104' and dc_ind='1'", br_no );
		if( trace_no_max < 0 )
		{
			sprintf( acErrMsg, "[%s]SJSJ数据库操作出错![%ld]", br_no, g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		/*取时间*/
		memset( &sTraceLogBk, 0x00, sizeof( struct trace_log_bk_c ) );
		g_reply_int = Trace_log_bk_Sel( g_pub_tx.reply, &sTraceLogBk, "trace_no=%ld and trace_cnt=1", trace_no_max );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "[%s]SJSJ数据库操作出错![%ld]", br_no, g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
	}

	vtcp_log("wjwjwj " );
	hour = sTraceLogBk.tx_time / 10000;
	second = ( sTraceLogBk.tx_time - hour * 10000 ) / 100;

	vtcp_log("wjwjwj " );
OkExit:
	sprintf( _tx_time_s, "%02ld:%02ld", hour, second );
	vtcp_log("return value [%s]", _tx_time_s );

	return _tx_time_s;
ErrExit:
	strcpy( _tx_time_s, "     " );

	return _tx_time_s;
}

/*************************************
函数功能：支行罚款额
		  应用报表：库存现金限额核定表(bb_d_kcxjxehdb.rfc)
参数：
    入口：机构号 char *up_br_no
返回：    支行罚款额
*************************************/
char *pub_rept_cal_fke( char *up_br_no )
{
	double _rtn_bal_f;
	char _rtn_bal_s[256];
	double kcxe,kcs;

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

vtcp_log("aaaaaaa[%ld]",__LINE__);
	g_reply_int = sql_sum_double( "cash_lmt_def a,com_branch b", "a.lmt_amt", &kcxe, 
		"a.br_no=b.br_no and b.up_br_no='%s'", up_br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计[%s]支行各机构库存限额合计出错![%ld]", up_br_no, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = sql_sum_double( "cash_mst a,com_branch b", "a.bal", &kcs,  
		"a.br_no=b.br_no and b.up_br_no='%s'", up_br_no );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "统计[%s]支行各机构库存数合计值出错![%ld]", up_br_no, g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_CompDblVal( kcxe, kcs );
	if( g_reply_int == 0 || g_reply_int == 1 )
		goto ErrExit;

	sprintf( acErrMsg, "[%s]库存数超过库存限额 %lf > %lf", up_br_no, kcs, kcxe );
	WRITEMSG

	_rtn_bal_f = 2 * ( ( kcs - kcxe ) / 20000 );

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );

	return _rtn_bal_s;
}
char *pub_rept_gl_mst_field( fld,br_no,acc_hrt )
 char *fld;
 char *br_no;
 char *acc_hrt;
{
	struct gl_mst_c sGlMst;
	struct gl_sub_c sGlSub;
	double _rtn_bal_f=0.00;
	char _rtn_bal_s[256];


	vtcp_log("[%s][%d]hahabr_no=[%s],acc_hrt==[%s]\n",__FILE__,__LINE__,br_no,acc_hrt);
	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	if( !strcmp( br_no, TOT_BR_NO ) )
	{
		g_reply_int = Gl_mst_Sel( g_pub_tx.reply, &sGlMst, 
			"br_no='%s' and cur_no='01' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
		g_reply_int = Gl_sub_Sel( g_pub_tx.reply, &sGlMst, 
			"br_no='%s' and cur_no='01' and acc_hrt='%s'", br_no, acc_hrt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]余额出错!!", br_no, acc_hrt );
			WRITEMSG
			goto ErrExit;
		}

	}

	/** fld ZZ+N/Y/R/S+C/M+J/D/G+YE/FS **/
	if( !strcmp(fld,"ZZNCJYE") )
	{	
		_rtn_bal_f=sGlMst.ydr_bal-sGlMst.ycr_bal;
		if(_rtn_bal_f<0.0005 && !memcmp(acc_hrt,"41600",5))/**add by martin 20070611 ZCFZNB清算不平**/
			_rtn_bal_f = 0.0;
	}
	else if( !strcmp(fld,"ZZNCDYE") )
		_rtn_bal_f=sGlMst.ycr_bal-sGlMst.ydr_bal;
	else if( !strcmp(fld,"ZZNJFS") )
		_rtn_bal_f=sGlMst.ydr_amt;
	else if( !strcmp(fld,"ZZNDFS") )
		_rtn_bal_f=sGlMst.ycr_amt;
	else if( !strcmp(fld,"ZZNCYE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.ycr_bal-sGlMst.ydr_bal;
		else
			_rtn_bal_f=sGlMst.ydr_bal-sGlMst.ycr_bal;
	}
	else if( !strcmp(fld,"ZZNFSE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.ycr_amt-sGlMst.ydr_amt;
		else
			_rtn_bal_f=sGlMst.ydr_amt-sGlMst.ycr_amt;
	}

	if( !strcmp(fld,"ZZYCJYE") )
	{
		_rtn_bal_f=sGlMst.mdr_bal-sGlMst.mcr_bal;
		if(_rtn_bal_f<0.0005 && !memcmp(acc_hrt,"41600",5))/**add by liuyue 20061129 ZCFZB清算不平**/
			_rtn_bal_f = 0.0;
	}
	else if( !strcmp(fld,"ZZYCDYE") )
		_rtn_bal_f=sGlMst.mcr_bal-sGlMst.mdr_bal;
	else if( !strcmp(fld,"ZZYDFE") ) /*这里的FE本来应该是YE但由于开始写错了,所以就这样用吧*/
		_rtn_bal_f=sGlMst.cr_bal;
	else if( !strcmp(fld,"ZZYJFE") )
		_rtn_bal_f=sGlMst.dr_bal;
	else if( !strcmp(fld,"ZZYJFS") )
		_rtn_bal_f=sGlMst.mdr_amt;
	else if( !strcmp(fld,"ZZYDFS") )
		_rtn_bal_f=sGlMst.mcr_amt;
	else if( !strcmp(fld,"ZZJRDFE") )
		_rtn_bal_f=sGlMst.cr_bal;
	else if( !strcmp(fld,"ZZJRJFE") )
		_rtn_bal_f=sGlMst.dr_bal;
	else if( !strcmp(fld,"ZZMJFE") ) /***月借方额**/
		_rtn_bal_f=sGlMst.mdr_bal;
	else if( !strcmp(fld,"ZZMDFE") ) /*月贷方额** liuyue 20061127***/
		_rtn_bal_f=sGlMst.mcr_bal;
	else if( !strcmp(fld,"ZZNDFE") ) /*年贷方额 martin 20070605**/
		_rtn_bal_f=sGlMst.ycr_bal;
	else if( !strcmp(fld,"ZZNJFE") ) /* 20070605*/
		_rtn_bal_f=sGlMst.ydr_bal;
	else if( !strcmp(fld,"ZZYCYE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.mcr_bal-sGlMst.mdr_bal;
		else
			_rtn_bal_f=sGlMst.mdr_bal-sGlMst.mcr_bal;
	}
	else if( !strcmp(fld,"ZZYFSE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.mcr_amt-sGlMst.mdr_amt;
		else
			_rtn_bal_f=sGlMst.mdr_amt-sGlMst.mcr_amt;
	}

	/*****季数据****,by  liuyue  20061129 ****出资产负债的季报****/
	
	if( !strcmp(fld,"ZZSCJYE") )
		_rtn_bal_f = sGlMst.qdr_bal-sGlMst.qcr_bal;
	else if( !strcmp(fld,"ZZSCDYE") ) /**季初贷方额***/
		_rtn_bal_f = sGlMst.qcr_bal-sGlMst.qdr_bal;
	else if( !strcmp(fld,"ZZSCJYE") )
		_rtn_bal_f = sGlMst.qdr_bal-sGlMst.qcr_bal;
	else if( !strcmp(fld,"ZZSCJFE") )
		_rtn_bal_f = sGlMst.qdr_bal;
	else if( !strcmp(fld,"ZZSCDFE") )
		_rtn_bal_f = sGlMst.qcr_bal;
	else if( !strcmp(fld,"ZZSCDFS") ) /*季初贷发生*/
		_rtn_bal_f = sGlMst.qcr_amt;
	else if( !strcmp(fld,"ZZSCJFS") )/*季初借发生*/
		_rtn_bal_f = sGlMst.qcr_amt;
	else if( !strcmp(fld,"ZZSCDZCE") ) /*季初贷发生轧差*/
		_rtn_bal_f = sGlMst.qcr_amt-sGlMst.qdr_amt;
	else if( !strcmp(fld,"ZZSCJZCE") )/*季初借发生轧差 */
		_rtn_bal_f = sGlMst.qdr_amt-sGlMst.qcr_amt;
	else if( !strcmp(fld,"ZZSCYE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.qcr_bal-sGlMst.qdr_bal;
		else
			_rtn_bal_f=sGlMst.qdr_bal-sGlMst.qcr_bal;
	}
		/****损益月数据**,by martin 20070529****/
	else if( !strcmp(fld,"ZZBYDZCE") )/*月初贷发生轧差*/
		_rtn_bal_f = sGlMst.mcr_amt-sGlMst.mdr_amt;
	else if( !strcmp(fld,"ZZBYJZCE") )/*月初借发生轧差 */
		_rtn_bal_f = sGlMst.mdr_amt-sGlMst.mcr_amt;
		/****费用月数据**,by martin 20070529****/
	else if( !strcmp(fld,"ZZFYJZCE") )/*月初借发生轧差*/
		_rtn_bal_f = sGlMst.mdr_amt-sGlMst.mcr_amt;




	if( !strcmp(fld,"ZZZRJYE") )
		_rtn_bal_f=sGlMst.ldd_bal-sGlMst.lcd_bal;
	else if( !strcmp(fld,"ZZZRDYE") )
		_rtn_bal_f=sGlMst.lcd_bal-sGlMst.ldd_bal;
	else if( !strcmp(fld,"ZZBRJFS") )
		_rtn_bal_f=sGlMst.rdd_amt;
	else if( !strcmp(fld,"ZZBRDFS") )
		_rtn_bal_f=sGlMst.rcd_amt;
	else if( !strcmp(fld,"ZZZRYE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.lcd_bal-sGlMst.ldd_bal;
		else
			_rtn_bal_f=sGlMst.ldd_bal-sGlMst.lcd_bal;
	}

	if( !strcmp(fld,"ZZJYE") )
	{
		_rtn_bal_f=sGlMst.dr_bal-sGlMst.cr_bal;
		if(_rtn_bal_f<0.005 && !memcmp(acc_hrt,"41600",5))
			_rtn_bal_f = 0.0;
	}
	else if( !strcmp(fld,"ZZDYE") )
		_rtn_bal_f=sGlMst.cr_bal-sGlMst.dr_bal;
	else if( !strcmp(fld,"ZZYE") )
	{
		if( sGlMst.dc_ind[0]=='2' ) 
			_rtn_bal_f=sGlMst.cr_bal-sGlMst.dr_bal;
		else
			_rtn_bal_f=sGlMst.dr_bal-sGlMst.cr_bal;
	}
	
	
	/****add by liuyue *****/

	vtcp_log("[%s][%d]计算机出来的值_rtn_bal_f==[%lf]\n",__FILE__,__LINE__,_rtn_bal_f);	

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("cal_计算结果 [%s.%s] [%lf][%s]",fld,acc_hrt, _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}
char *pub_rept_gl_rpt_field( fld,br_no,code )
 char *fld;
 char *br_no;
 char *code;
{
	struct gl_rpt_c sGlRpt;
	double _rtn_bal_f=0.00;
	char _rtn_bal_s[256];
	long rq=0;

	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );

	/** GLRPT YC/NC **/
	if( fld[5]=='Y' )
	{
		rq=g_pub_tx.tx_date/100*100+01;
	}
	else if( fld[5]=='N' )
	{
		rq=g_pub_tx.tx_date/10000*10000+101;
	}
	else
	{
		rq=g_pub_tx.tx_date;
	}
		g_reply_int = Gl_rpt_Sel( g_pub_tx.reply, &sGlRpt, 
			"br_no='%s' and \"date\"=%d and code='%s'", br_no,rq, code );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, code );
			WRITEMSG
			goto ErrExit;
		}

	_rtn_bal_f=sGlRpt.amt;

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}


/************************************
 *函数名称:pub_rept_gl_in_mst_field *
 *函数功能:获取某个内部帐的余额     *
 *参数说明: fld-------程序标识号    *
 *          br_no-----当前机构      *
 *          acc_hrt---内部帐号ac_id *
 *函数返回: 转化为串的金额          *
 *AUHTOR  :dadary  20061026 ChangZhi*
 *NOTE    :对于取存放同业保证金是   *
 *         根据名字来取的,如果程序  *
 *         需根据需要改动           *
 ************************************/
char *pub_rept_gl_in_mst_field( fld,br_no,ac_id )
 char *fld;
 char *br_no;
 char *ac_id;
{
	struct in_mst_c sIn_mst;
	
	double _rtn_bal_f=0.00;
	char _rtn_bal_s[256];
	
	char cWherelist[500];
	
	memset(cWherelist,0x00,sizeof(cWherelist));
	
	
	memset(&sIn_mst,0x00,sizeof(sIn_mst));
	memset(_rtn_bal_s,0x00,sizeof(_rtn_bal_s));
	
	
	
	/***如果传入的是 1150102 工行营业部（保证金）  1150402  建行营业部（保证金）
		1150600  存放其他商业银行款项***/
	vtcp_log("[%s][%d]进入表in_mst的处理了,[%s][%s]\n",__FILE__,__LINE__,br_no,ac_id);
	if(strlen(ac_id)==9)/**传入的是ac_id******/
		sprintf(cWherelist,"opn_br_no='%s' and ac_id='%s'    ",br_no,ac_id);
	if(strlen(ac_id)==7)
	{
		if(!strncmp(ac_id,"1150102",7))
			sprintf(cWherelist,"opn_br_no='%s' and name like '%工行%' and name like '%保证金%'  ",br_no);
		if(!strncmp(ac_id,"1150402",7))
			sprintf(cWherelist,"opn_br_no='%s' and name like '%建行%' and name like '%保证金%'  ",br_no);
		if(!strncmp(ac_id,"1150600",7))
			sprintf(cWherelist,"opn_br_no='%s' and name like '%其他商业银行%' and name like '%款项%' ",br_no);
	}

	printf("[%s][%d]br_no=[%s],ac_id==[%s]\n",__FILE__,__LINE__,br_no,ac_id);
	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );


	g_reply_int = In_mst_Sel( g_pub_tx.reply, &sIn_mst, 
			cWherelist);
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, ac_id );
		WRITEMSG
		goto ErrExit;
	}
	/** fld NBZ+ZR/JR/+YE **/
	if( !strcmp(fld,"NBZZRYE") )
		_rtn_bal_f=sIn_mst.ys_bal;
	else if( !strcmp(fld,"NBZJRYE") )
		_rtn_bal_f=sIn_mst.bal;
	else if( !strcmp(fld,"NBZADD") )
		_rtn_bal_f=sIn_mst.bal-sIn_mst.ys_bal;
	vtcp_log("[%s][%d]成功推出表in_mst的处理了,[%s][%s] 取到[%lf]\n",__FILE__,__LINE__,br_no,ac_id,_rtn_bal_f);

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}


/************************************
 *函数名称:pub_rept_zjgl_mst_field  *
 *函数功能:获取某个内部帐的余额     *
 *参数说明: fld-------程序标识号    *
 *          br_no-----当前机构      *
 *          acc_hrt---内部帐号ac_id *
 *函数返回: 转化为串的金额          *
 *AUHTOR  :dadary  20061103 ChangZhi*
 *NOTE    :对于取存放同业保证金是   *
 *         根据名字来取的,如果程序  *
 *         需根据需要改动           *
 ************************************/
char *pub_rept_zjgl_mst_field( fld,br_no,acc_hrt )
 char *fld;
 char *br_no;
 char *acc_hrt;
{
	struct zjgl_mst_c sZjgl_mst;
	
	double _rtn_bal_f=0.00;
	char _rtn_bal_s[256];

	
	
	memset(&sZjgl_mst,0x00,sizeof(sZjgl_mst));
	memset(_rtn_bal_s,0x00,sizeof(_rtn_bal_s));
	
	
	


	printf("[%s][%d]br_no=[%s],ac_id==[%s]\n",__FILE__,__LINE__,br_no,acc_hrt);
	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );


	g_reply_int = Zjgl_mst_Sel( g_pub_tx.reply, &sZjgl_mst, 
			"opn_br_no='%s' and cur_no='01' and acc_no='%s'", br_no, acc_hrt );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
		WRITEMSG
		goto ErrExit;
	}
	/** fld ZJ+ZR/JR/+YE **/
	printf("[%s][%d]进入ZJGL_MST表\n",__FILE__,__LINE__);
	if(strncmp(br_no,QS_BR_NO, 5))
	{
		if( !strcmp(fld,"ZJJRJFE") )
			_rtn_bal_f=sZjgl_mst.dr_bal;
		else if( !strcmp(fld,"ZJJRDFE") )
			_rtn_bal_f=sZjgl_mst.cr_bal;
		else if( !strcmp(fld,"ZJZRJFE") )
			_rtn_bal_f=sZjgl_mst.ys_dr_bal;
		else if( !strcmp(fld,"ZJZRDFE") )
			_rtn_bal_f=sZjgl_mst.ys_cr_bal;
		else if( !strcmp(fld,"ZJJYE") )
			_rtn_bal_f=sZjgl_mst.dr_bal-sZjgl_mst.cr_bal;
		else if( !strcmp(fld,"ZJDYE") )
			_rtn_bal_f=sZjgl_mst.cr_bal-sZjgl_mst.dr_bal;
	}
		
	else if(!strncmp(br_no,QS_BR_NO,5))   /**如果是清算中心,取和***/
	{
		if( !strcmp(fld,"ZJJRJFE") )
			sql_sum_double ( "zjgl_mst" , "dr_bal" , &_rtn_bal_f , " cur_no='01' and acc_no='%s' and opn_br_no not in ('%s','%s','%s') " , acc_hrt , TOT_BR_NO, QS_BR_NO, KJ_BR_NO) ;
		else if( !strcmp(fld,"ZJJRDFE") )
			sql_sum_double ( "zjgl_mst" , "cr_bal" , &_rtn_bal_f , "cur_no='01' and acc_no='%s'  and opn_br_no not in ('%s','%s','%s') " ,  acc_hrt , TOT_BR_NO, QS_BR_NO, KJ_BR_NO) ;
		else if( !strcmp(fld,"ZJZRJFE") )
			sql_sum_double ( "zjgl_mst" , "ys_dr_bal" , &_rtn_bal_f , " cur_no='01' and acc_no='%s' and opn_br_no not in ('%s','%s','%s') " , acc_hrt , TOT_BR_NO, QS_BR_NO, KJ_BR_NO) ;
		else if( !strcmp(fld,"ZJZRDFE") )
			sql_sum_double ( "zjgl_mst" , "ys_cr_bal" , &_rtn_bal_f , " cur_no='01' and acc_no='%s' and opn_br_no not in ('%s','%s','%s') " ,  acc_hrt, TOT_BR_NO, QS_BR_NO, KJ_BR_NO ) ;
	
		vtcp_log("[%s][%d] 老大这是清算中心[%lf] [%s]\n",__FILE__,__LINE__,_rtn_bal_f,fld);
			
	}
	
	if(!strncmp(br_no,QS_BR_NO, 5))
		vtcp_log("[%s][%d] 老大这是清算中心[%lf] [%s]\n",__FILE__,__LINE__,_rtn_bal_f,fld);
	/**
	if(_rtn_bal_f<0.005)
		_rtn_bal_f = _rtn_bal_f * (-1);
	****/
	printf("[%s][%d]退出ZJGL_MST表\n",__FILE__,__LINE__);
OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	WRITEMSG
	vtcp_log("[%s] [%d] [%lf][%s]",__FILE__,__LINE__, _rtn_bal_f,_rtn_bal_s);
	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}


/************************************
 *函数名称:pub_rept_ass_mst_field  *
 *函数功能:获取某个内部帐的余额     *
 *参数说明: fld-------程序标识号    *
 *          br_no-----当前机构      *
 *          acc_hrt---内部帐号ac_id *
 *函数返回: 转化为串的金额          *
 *AUHTOR  :dadary  20061103 ChangZhi*
 *NOTE    :对于取存放同业保证金是   *
 *         根据名字来取的,如果程序  *
 *         需根据需要改动           *
 ************************************/
char *pub_rept_ass_mst_field( fld,br_no,acc_hrt )
 char *fld;
 char *br_no;
 char *acc_hrt;
{
	struct ass_mst_c sAss_mst;
	
	double _rtn_bal_f=0.00;
	char _rtn_bal_s[256];
	char cWherelist[500];
	char cTmplist[100];
	
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmplist,0x00,sizeof(cTmplist));

	
	
	memset(&sAss_mst,0x00,sizeof(sAss_mst));
	memset(_rtn_bal_s,0x00,sizeof(_rtn_bal_s));
	
	
	/****拼凑where 语句****/
	 
	sprintf(cWherelist,"br_no='%s' ", br_no );
	
	if(!strcmp(fld,"ASSCRZJ"))
	{
		sprintf(cTmplist," and  sign='1' ");
		strcat(cWherelist,cTmplist);
	}
	else if(!strcmp(fld,"ASSCCZJ"))
	{
		sprintf(cTmplist," and  sign='2' ");
		strcat(cWherelist,cTmplist);
	}
	else
	{
		sprintf(cTmplist," and pact_no='%s' ",acc_hrt);
		strcat(cWherelist,cTmplist);
	}
	printf("[%s][%d]br_no=[%s],ac_id==[%s]\n",__FILE__,__LINE__,br_no,acc_hrt);
	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );


	g_reply_int = Ass_mst_Sel( g_pub_tx.reply, &sAss_mst, 
			cWherelist );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
		WRITEMSG
		goto ErrExit;
	}
	/** fld ZJ+ZR/JR/+YE **/
	if(strncmp(br_no,QS_BR_NO, 5))
	{
		if( !strcmp(fld,"ASSJRYE") )
			_rtn_bal_f=sAss_mst.bal;
		else if( !strcmp(fld,"ASSZRYE") )
			_rtn_bal_f=sAss_mst.ys_bal;
		else if(!strcmp(fld,"ASSCRZJ"))
			sql_sum_double ( "ass_mst" , "bal" , &_rtn_bal_f , "  br_no='%s' and    sign='1' " , br_no ) ;
		else if( !strcmp(fld,"ASSCCZJ") )
			sql_sum_double ( "ass_mst" , "bal" , &_rtn_bal_f , "  br_no='%s'  and    sign='2' " , br_no ) ;
	}
	else if(!strncmp(br_no,QS_BR_NO, 5))
	{
		if( !strcmp(fld,"ASSJRYE") )
			sql_sum_double ( "ass_mst" , "bal" , &_rtn_bal_f , " pact_no='%s' and br_no not in ('%s','%s','%s') " , acc_hrt, TOT_BR_NO, QS_BR_NO, KJ_BR_NO ) ;
		else if( !strcmp(fld,"ASSZRYE") )
			sql_sum_double ( "ass_mst" , "ys_bal" , &_rtn_bal_f , "  pact_no='%s' and br_no not in ('%s','%s','%s') " , acc_hrt, TOT_BR_NO, QS_BR_NO, KJ_BR_NO ) ;
		else if(!strcmp(fld,"ASSCRZJ") ||!strcmp(fld,"ASSCCZJ") )
			_rtn_bal_f = sAss_mst.bal;
	}
	

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("[%lf][%s]", _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}




char *pub_rept_ZQZZ( fld,br_no,acc_hrt )
 char *fld;
 char *br_no;
 char *acc_hrt;
{
	
	long   qry_date = 0.0;
	double _rtn_bal_f=0.00;
	char _rtn_bal_s[256];
	
	struct gl_tsub_c sGl_tsub;
	struct com_sys_parm_c sCom_sys_parm;
	
	memset(&sGl_tsub,0x00,sizeof(sGl_tsub));
	memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
	memset( _rtn_bal_s, 0x00, sizeof( _rtn_bal_s ) );
	
	
	g_reply_int = Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(g_reply_int)
	{
		vtcp_log("[%s][%d]查找com_sys_parm表错误 ，g_reply_int=[%d]\n",__FILE__,__LINE__,g_reply_int);
		goto ErrExit;
	}
	
	
	qry_date = sCom_sys_parm.lst_date-10000;
	
	vtcp_log("[%s][%d]同期的取数据日期是[%f]\n",__FILE__,__LINE__,qry_date);
	
	


	vtcp_log("[%s][%d]hahabr_no=[%s],acc_hrt==[%s],fld==[%s]\n",__FILE__,__LINE__,br_no,acc_hrt,fld);
	
	
	g_reply_int = Gl_tsub_Sel( g_pub_tx.reply, &sGl_tsub, 
		"br_no='%s' and cur_no='01' and acc_hrt='%s' and \"date\"=%ld ", br_no, acc_hrt ,qry_date);
	if( g_reply_int )
	{
		sprintf( acErrMsg, "取[%s]机构[%s]科目余额出错!!", br_no, acc_hrt );
		WRITEMSG
		goto ErrExit;
	}
	

	/** fld ZZ+N/Y/R/S+C/M+J/D/G+YE/FS **/
	

	if( !strcmp(fld,"ZQZZDFE") )
	{
		_rtn_bal_f=sGl_tsub.r_cr_bal;
	}
	else if( !strcmp(fld,"ZQZZJFE") )
		_rtn_bal_f=sGl_tsub.r_dr_bal;
	else if( !strcmp(fld,"ZQZZJFS") )
		_rtn_bal_f=sGl_tsub.r_dr_amt;
	else if( !strcmp(fld,"ZQZZDFS") )
		_rtn_bal_f=sGl_tsub.r_cr_amt;
	
	
	
	/****add by liuyue *****/

	vtcp_log("[%s][%d]计算机出来的值_rtn_bal_f==[%lf]\n",__FILE__,__LINE__,_rtn_bal_f);	

OkExit:
	sprintf( _rtn_bal_s, "%.2lf", _rtn_bal_f );
	vtcp_log("cal_计算结果 [%s.%s] [%lf][%s]",fld,acc_hrt, _rtn_bal_f,_rtn_bal_s);

	return _rtn_bal_s;
ErrExit:
	strcpy( _rtn_bal_s, "0.00" );
	return _rtn_bal_s;
}
