/*************************************************************
* 文 件 名: rpt007.c
* 功能描述：特约商户资金转入清单
*
* 作    者: rob
* 完成日期: 2003年6月27日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "march_mst_c.h"
#include "march_mst_mx_c.h"

struct march_mst_c g_sMarch_mst;
struct march_mst_mx_c g_sMarch_mst_mx;
int page = 0;

int get_ratio_posmhd(int bli,int bll,char str[100]);
rpt007()
{
	int ret = 0;
	int flag=0;
	int opncnt;
	char vpfm[21];
	int     line; 
	char fname[100];
	char filename[100];
	FILE    *fp; 
	char tmp_str[60];
	
    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    
    sprintf( filename,"%s%ld","posmhzqd",g_pub_tx.tx_date);
    
    sprintf( fname,"rm -f %s/%s/%s%ld.txt",getenv("DATADIR"),
			"POSM","posmhzqd",g_pub_tx.tx_date);
	ret=system( fname ); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg,"rm file error %s %d",fname,ret); 
		WRITEMSG 
		return ret;
	} 
	
	strcpy(vpfm, "posmhzqd");

	ret = March_mst_Dec_Sel(g_pub_tx.reply, "1=1");
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "DECLARE FOR SELECT !! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 

	pub_rpt_openfile( &fp,"POSM", filename); 

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","HH",&opncnt, 
			get_ratio_posmhd) ) 
		goto ErrExit; 
	
	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","AA",&opncnt, 
			get_ratio_posmhd) ) 
		goto ErrExit; 

	while(1) 
	{ 
		ret = March_mst_Fet_Sel(&g_sMarch_mst, g_pub_tx.reply); 
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

		if (g_sMarch_mst.deal_sign[0] == 'Y')
		{
			strcpy(g_sMarch_mst.deal_msg, "已记帐");
		}
		else
		{
			strcpy(tmp_str, "未记帐");
			strcat(tmp_str, g_sMarch_mst.deal_msg);
			strcpy(g_sMarch_mst.deal_msg, tmp_str);
		}

		if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","BB",&opncnt, 
				get_ratio_posmhd) ) 
			goto ErrExit; 

	} 
	
	March_mst_Clo_Sel();

	if( pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001","CC",&opncnt, 
			get_ratio_posmhd) ) 
		goto ErrExit; 
			
	fprintf(fp, "\n\n\n"); 

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成科目日结单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成科目日结单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int get_ratio_posmhd( bli,bll,str )
int bli,bll;
char str[100];
{ 
	char vhm[101]; 
	char vstr[101]; 
	char    l_kmm[31]; 
	char fmt[10]; 
	int i=0; 
	int ret = 0; 
	
	switch( bli ) 
	{ 
		case 'A': 
			strcpy(str, g_sMarch_mst.ac_no); 
			break; 
		case 'B': 
			strcpy(str, "特约商户资金清算转入"); 
			break; 
		case 'C': 
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( vstr,fmt, g_sMarch_mst.tx_amt);
			strcpy(str, vstr); 
			break; 
		case 'D': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, g_sMarch_mst.march_name); 
			break; 
		case 'E': 
			sprintf(fmt,"%%-%ds",bll); 
			sprintf(str, fmt, pub_rpt_pcask(g_sMarch_mst.tx_amt)); 
			break; 
		case 'F':
			sprintf(fmt,"%%%ds",bll); 
			sprintf(str, fmt, g_sMarch_mst_mx.card_no);
			break;
		case 'G':
			sprintf(fmt,"%%%d.2lf",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tx_amt);
			break;
		case 'H':
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tx_date);
			break;
		case 'I':
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tx_time);
			break;
		case 'J':
			sprintf(fmt,"%%%dld",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.trace_no);
			break;
		case 'K':
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.tty_no);
			break;
		case 'L':
			sprintf(fmt,"%%%ds",bll); 
			sprintf( str, fmt, g_sMarch_mst_mx.opn_tx_br_name); 
			break;
		case 'M':
			sprintf(fmt, "%%%dld",bll);
			sprintf(str, fmt, g_sMarch_mst.tx_date);
			break;
		case 'N':
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, g_sMarch_mst.deal_msg);
			break;
		case 'O':
			strcpy( str, "POS");
			break;
		case 'X': 
			sprintf( str, "%04ld", g_pub_tx.tx_date/10000 ); 
			break; 
		case 'W': 
			sprintf( str, "%02ld", g_pub_tx.tx_date/100%100 ); 
			break; 
		case 'V': 
			sprintf( str, "%02ld", g_pub_tx.tx_date%100 ); 
			break; 
		case '1':
			sprintf( str, "%08ld", g_pub_tx.tx_date);
			break;
		case '2':
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt, g_sMarch_mst.march_code );
			break;
		case '3':
			sprintf( str, "%02ld", page);
			break;
		case '4':
			sprintf(fmt, "%%-%d.6lf", bll);
			sprintf( str, fmt, g_sMarch_mst.fee_rate);
			break;
		case '5':
			sprintf( str, "%08ld", g_sMarch_mst.tx_cnt);
			break;
		case '6':
			sprintf(fmt, "%%-%d.2lf", bll);
			sprintf( str, fmt, g_sMarch_mst.tx_amt);
			break;
		case '7':
			sprintf(fmt, "%%-%d.2lf", bll);
			sprintf( str, fmt, g_sMarch_mst.fee_amt);
			break;
		case '8':
			sprintf(fmt, "%%-%d.2lf", bll);
			sprintf( str, fmt, g_sMarch_mst.real_amt);
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
