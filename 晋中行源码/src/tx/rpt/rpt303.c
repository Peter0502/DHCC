/***************************************************************
* 文 件 名: rpt303.c
* 功能描述: 利润转未分配利润的报表   借 311   贷 31205
**************************************************************/
#define MYRETERR if(ret){ \
        sprintf(acErrMsg,"sql erro!! [%d]",ret); \
        WRITEMSG \
		goto ErrExit; \
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "gl_sub_c.h"
#include "com_item_c.h"
#include "com_branch_c.h"
#include "dc_log_bk_c.h"

	struct com_parm_c sCom_parm_lr;
	struct com_parm_c sCom_parm_lrfp;
int lsh=1;
rpt303()
{
    int ret=0;
	struct com_branch_c com_branch_c;
	struct dc_log_bk_c  dc_log_bk_c;
	FILE *fp=NULL;
	char filename[64];

    ret=sql_begin(); /*打开事务*/
	MYRETERR

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6') and wrk_sts!='*' order by br_no" );
	MYRETERR

	while( 1 )
	{
		ret = Com_branch_Fet_Sel( &com_branch_c , g_pub_tx.reply );
		if( ret==100 ) break;
		MYRETERR

		memset(&dc_log_bk_c,'\0',sizeof(dc_log_bk_c));
		ret = Dc_log_bk_Sel(g_pub_tx.reply, &dc_log_bk_c, "tx_tx_br_no='%s' and trace_no=15028 and acc_hrt='31100'",com_branch_c.br_no);
		MYRETERR

	strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
	strcpy( g_pub_tx.opn_br_no,com_branch_c.br_no );
	g_pub_tx.trace_no=15028;
	g_pub_tx.tx_date=20061231;

		/**开始打印传票**/
		/**添加传票**/
    memset(filename,0x00,sizeof(filename));
    sprintf(filename,"%s/report/%s/gD303.txt",getenv("HOME"),com_branch_c.br_no);
    fp = fopen( filename,"w" ); 
    if( fp==NULL )
    {
        sprintf(acErrMsg," open file [%s] error ",filename );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        goto ErrExit;
    }
    /**end**/
	/**开始打印传票**/
  char tmp_cxamt[21],vstr1[64],vstr[65];
  memset(vstr1,0x00,sizeof(vstr1));
  memset(vstr,0x00,sizeof(vstr));
  sprintf( tmp_cxamt, "%.2f" ,dc_log_bk_c.amt);
  pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* 小写金额 */
  numtomoney(dc_log_bk_c.amt, vstr1);	/* 大写金额 */
  strcat(vstr1,"       ￥");
  strcat(vstr1,tmp_cxamt);
  sprintf(vstr,"%-64s",vstr1);
  vtcp_log("[%s][%d]刘岳 vstr===[%s]\n",__FILE__,__LINE__,vstr);
  
	/*	fprintf(fp,"%c%c%c%c",0x1b,0x67,0x1c,0x0E);*/
		fprintf(fp, "                               支行利润结转传票\n");
		fprintf(fp, "                  ---------------------------------------------------\n");
		fprintf(fp,"\n");		
		fprintf(fp, "     机构号: %5s               交易日期: %ld            流水号: %ld       \n",com_branch_c.br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fp, "┌─────┬─────┬─────┬────────────────────┐\n");
		fprintf(fp, "│科  目  号│311       │科  目  名│本年利润                                │\n");
		fprintf(fp, "├─────┼─────┴─────┴────────────────────┤\n");
		fprintf(fp, "│金额(大写)│%s│\n",vstr); 
		fprintf(fp, "├─────┴──────────┬──┬──────────────────┤\n");
		fprintf(fp, "│                                │ 会 │ 科       目(借):311                │\n");
		fprintf(fp, "│  上列款项已从该单位账户结转    │ 计 │                                    │\n");
		fprintf(fp, "│                                │ 分 │ 对 方 科 目(贷):31205              │\n");
		fprintf(fp, "│                  (开户行盖章)  │ 录 │                                    │\n");
		fprintf(fp, "└────────────────┴──┴──────────────────┘\n");
		fprintf(fp,"\n\n\n");
		fclose(fp);
	}
	Com_branch_Clo_Sel();

OkExit:
    ret=sql_commit();   /*--提交事务--*/
	MYRETERR
    strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
        strcpy(g_pub_tx.reply,"G009");
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}
