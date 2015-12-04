/***************************************************************
* 文 件 名: rpt061.c
* 功能描述: 母帐号已入帐但子帐号未入帐清单
*
* 作    者: rob
* 完成日期: 2003年12月07日
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
#include "trace_log_bk_c.h"
#include "note_parm_c.h"
#include "tx_def_c.h"

int page = 1;
struct trace_log_bk_c sTrace_log_bk;

int get_ratio_zzhrz(int bli,int bll,char str[100]);
rpt061()
{
    int ret=0;
	struct com_sys_parm_c sCom_sys_parm;

	memset(&sCom_sys_parm, 0x0, sizeof(struct com_sys_parm_c));

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

	/* 打印子帐号未入帐清单 */
	ret = sub_ac_print();
	if (ret != 0)
	{
		sprintf(acErrMsg, "打印子帐号未入帐清单错误!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"打印子帐号未入帐号清单处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"打印子帐号未入帐清单处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

int sub_ac_print()
{
	int ret = 0; 
	int mcnt=0, zcnt=0, wcnt=0;
	int opncnt=0; 
	FILE    *fp; 
	int     line=0; 
	int  	total_line=55;
	char fname[100]; 
	char filename[100];
	char vpfm[21]; 
	char tx_br_no[6], pre_br_no[6];
	int first=0;

	struct trace_log_bk_c sTrace_log_bk1;
	
	memset(fname, 0x0, sizeof(fname));
	memset(filename, 0x0, sizeof(filename));
	memset(vpfm, 0x0, sizeof(vpfm));
	memset(tx_br_no, 0x0, sizeof(tx_br_no));
	memset(pre_br_no, 0x0, sizeof(pre_br_no));
	memset(&sTrace_log_bk1, 0x0, sizeof(struct trace_log_bk_c));

	strcpy(vpfm, "sub_aclist");
	sprintf( filename,"%s","BRsubaclist");

sprintf(acErrMsg, "tx_date=%ld", g_pub_tx.tx_date);
WRITEMSG

	ret = Trace_log_bk_Dec_Sel(g_pub_tx.reply, "sts='0' and no_show='0' and \
		ac_no in (select distinct ac_no from sub_dd_mst where 1=1 ) \
		and trace_no not in (select distinct trace_no from \
		sub_dd_mst_hst where tx_date=%ld) order by tx_br_no, \
		ac_no, add_ind, amt, ct_ind", g_pub_tx.tx_date);
	if ( ret )
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	}

	first=0;
	line = 0;
	page = 0;
	while(1) 
	{ 
		memset(&sTrace_log_bk, 0x0, sizeof(sTrace_log_bk));
		ret = Trace_log_bk_Fet_Sel(&sTrace_log_bk, g_pub_tx.reply); 
		if ( ret == 100 ) 
		{ 
			if (first != 0)
			{
				pub_rpt_openfile( &fp, pre_br_no, filename);
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_zzhrz) ) 
				goto ErrExit;
				fclose(fp);
			}
			break; 
		}
		else if ( ret ) 
		{ 
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret); 
			WRITEMSG 
			goto ErrExit; 
		} 

		pub_base_strpack(sTrace_log_bk.ac_no); 
		sprintf(acErrMsg, " ----ac_no=[%s], trace_no=[%ld], tx_amt=[%lf]", 
			sTrace_log_bk.ac_no, sTrace_log_bk.trace_no, sTrace_log_bk.amt); 
		WRITEMSG 
		
		ret = sql_count("sub_dd_mst_hst", "ac_no='%s' and add_ind='%s' \ 
			and tx_amt=%lf and up_tx_date=%ld and tx_br_no='%s' and \ 
			ct_ind='%s'", sTrace_log_bk.ac_no, sTrace_log_bk.add_ind, 
			sTrace_log_bk.amt, sTrace_log_bk.tx_date, sTrace_log_bk.tx_br_no, 
			sTrace_log_bk.ct_ind); 
		if ( ret < 0 ) 
		{ 
			sprintf(acErrMsg,"统计该笔子帐号明细错误 [%d]", ret); 
			WRITEMSG 
			Trace_log_Clo_Sel(); 
			goto ErrExit; 
		} 

		zcnt = ret; 
		
		ret = sql_count("trace_log_bk", "ac_no='%s' and amt=%lf and \ 
			add_ind='%s' and ct_ind='%s' and sts='0' and tx_date=%ld and \ 
			tx_br_no='%s' and no_show='0'", sTrace_log_bk.ac_no, 
			sTrace_log_bk.amt, sTrace_log_bk.add_ind, 
			sTrace_log_bk.ct_ind, sTrace_log_bk.tx_date, 
			sTrace_log_bk.tx_br_no); 
		if ( ret < 0 ) 
		{ 
			sprintf(acErrMsg, "统计母帐号的流水错误!! [%d]", ret); 
			WRITEMSG 
			goto ErrExit; 
		} 
		mcnt = ret; 

		if ( strcmp(sTrace_log_bk1.ac_no, sTrace_log_bk.ac_no) == 0 && \
			 strcmp(sTrace_log_bk1.add_ind, sTrace_log_bk.add_ind) == 0 && \
			 strcmp(sTrace_log_bk1.ct_ind, sTrace_log_bk.ct_ind) == 0 && \
			 strcmp(sTrace_log_bk1.tx_br_no, sTrace_log_bk.tx_br_no) == 0 && \
			 pub_base_CompDblVal(sTrace_log_bk1.amt, sTrace_log_bk.amt) == 0 )
		{
		}
		else
		{
			wcnt = 0;
		}
		
		sprintf(acErrMsg, "------ mcnt=[%ld], zcnt=[%ld]", mcnt,zcnt); 
		WRITEMSG 
		if ( mcnt <= zcnt + wcnt) 
		{ 
			sprintf(acErrMsg, "子帐号已经入帐!!");
			WRITEMSG
			continue;
		}

		wcnt = wcnt + 1;
		strcpy(tx_br_no, sTrace_log_bk.tx_br_no);
		memcpy(&sTrace_log_bk1, &sTrace_log_bk, sizeof(sTrace_log_bk));

		if ( strcmp(tx_br_no, pre_br_no) )
		{
			if ( strcmp(pre_br_no, "") )
			{
				pub_rpt_openfile( &fp, pre_br_no, filename);
				if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
						get_ratio_zzhrz) ) 
					goto ErrExit;
				fclose(fp);
			}
			strcpy(pre_br_no, tx_br_no);
			first = 0;
			page = 0;
			line = 0;
		}

		if( first==0 )
		{
			page=1;
			pub_rpt_openfile( &fp, pre_br_no, filename); 

			sprintf(acErrMsg, "---------------------");
			WRITEMSG
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
					get_ratio_zzhrz) ) 
				goto ErrExit; 
	
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_zzhrz) ) 
				goto ErrExit; 
			fclose(fp);
		}
		first = first + 1;
	

		if (line > total_line)
		{
			pub_rpt_openfile( &fp, pre_br_no, filename);
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
					get_ratio_zzhrz) ) 
				goto ErrExit;
			fprintf(fp,"@HY@");
			page = page + 1;
			line = 0;
			fclose(fp);
			pub_rpt_openfile( &fp, pre_br_no, filename);

			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
				get_ratio_zzhrz) ) 
			goto ErrExit; 
	
			if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
					get_ratio_zzhrz) ) 
				goto ErrExit; 
			fclose(fp);
		}
		pub_rpt_openfile( &fp, pre_br_no, filename);
		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_zzhrz) ) 
			goto ErrExit; 
		fclose(fp);
	}
	Trace_log_bk_Clo_Sel();

	return 0;
ErrExit:
	return 1;
}

int get_ratio_zzhrz( bli,bll,str )
int bli,bll;
char str[100];
{
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 
	struct tx_def_c sTx_def;
	struct note_parm_c sNote_parm;

	switch( bli ) 
	{ 
		case 'A': 
			sprintf(fmt,"%%%dld",bll); 
			sprintf( vstr,fmt, sTrace_log_bk.trace_no);
			strcpy(str, vstr); 
			break; 
		case 'B': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sTrace_log_bk.tel);
			strcpy(str, vstr); 
			break; 
		case 'C': 
			memset(&sTx_def, 0x0, sizeof(sTx_def));
			ret = Tx_def_Sel(g_pub_tx.reply, &sTx_def, "tx_code='%s'",
				sTrace_log_bk.tx_code);
			if ( ret )
			{
				sprintf(acErrMsg, "查询交易定义表错误! [%d]", ret);
				WRITEMSG
				return ret;
			}
			sprintf(fmt,"%%%ds",bll); 
			sprintf( vstr,fmt, sTx_def.tx_name);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sTrace_log_bk.tx_br_no); 
			break; 
		case 'E': 
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sTrace_log_bk.ac_no); 
			break; 
		case 'F':
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sTrace_log_bk.note_no); 
			break; 
		case 'G':
			memset(&sNote_parm, 0x0, sizeof(sNote_parm));
			ret = Note_parm_Sel(g_pub_tx.reply, &sNote_parm, "note_type='%s'",
				sTrace_log_bk.note_type);
			if ( ret && ret != 100 )
			{
				sprintf(acErrMsg, "查询介质属性表错误!! [%d]", ret);
				WRITEMSG
				return ret;
			}
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, sNote_parm.name); 
			break; 
		case 'H': 
			sprintf(fmt, "%%%d.2lf", bll); 
			sprintf(str, fmt, sTrace_log_bk.amt); 
			break;
		case '1': 
			sprintf( str, "%08ld", g_pub_tx.tx_date); 
			break; 
		case '2': 
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
