/***************************************************************
* 文 件 名: gD304.c
* 功能描述: 支行和清算中心的利润划到财务中心
*贷:-31205(支行)
*贷:31205 (财务中心)
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

struct com_parm_c sCom_parm_lr;
struct com_parm_c sCom_parm_lrfp;
long kslsh,num_cnt;

gD304()
{
    int ret=0,cwnum=1;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_branch_c com_branch_c;
	struct com_item_c com_item_c;
	char old_amt_ind;
	double d_amt=0.00;
	double c_amt=0.00;
	double bal=0.00;
	char yefx;
	struct pub_tx v_pub_tx;
	struct gl_sub_c gl_sub_c;
	FILE *fp=NULL,*fpcw=NULL;
	char filename[64],filenamecw[64];

	memset(&g_pub_tx, 0x0, sizeof(struct pub_tx));

    ret=sql_begin(); /*打开事务*/
	MYRETERR

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
	set_zd_data( DC_TX_CODE, "D304");
	if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}
	kslsh=g_pub_tx.trace_no;
	num_cnt=1;

	/* 先建立财务中心的传票,下面再一个机构一个机构的往里写 */
    memset(filenamecw,'\0',sizeof(filenamecw));
    sprintf(filenamecw,"%s/report/59801/gD304.txt",getenv("HOME"));
    fpcw = fopen( filenamecw,"w" ); 
    if( fpcw==NULL )
    {
        sprintf(acErrMsg," open file [%s] error ",filenamecw );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        goto ErrExit;
    }
	memset(&sCom_parm_lr, 0x0, sizeof(struct com_parm_c));
	memset(&sCom_parm_lrfp, 0x0, sizeof(struct com_parm_c));
	memset(&com_item_c, 0x0, sizeof(com_item_c));
	ret = Com_item_Sel( g_pub_tx.reply,&com_item_c, "acc_hrt='31205' ");	/* 未分配利润 31205 */
	MYRETERR

	/*ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6') and br_no!='59801' and wrk_sts!='*' order by br_no" );mod by lwb 20150521  增加分行*/
	ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6','7') and br_no!='59801' and wrk_sts!='*' order by br_no" );
	MYRETERR

	while( 1 )
	{
		ret = Com_branch_Fet_Sel( &com_branch_c , g_pub_tx.reply );
		if( ret==100 ) break;
		MYRETERR

		pub_base_sysinit();
		strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
		strcpy( g_pub_tx.opn_br_no,com_branch_c.br_no );
		g_pub_tx.trace_no=kslsh;
		set_zd_long( DC_TRACE_NO , g_pub_tx.trace_no);

		strcpy(g_pub_tx.brf, "利润上划");

	ret=Gl_sub_Sel( g_pub_tx.reply,&gl_sub_c ,
			"br_no='%s' and cur_no='01' and acc_hrt='%s' ",
			com_branch_c.br_no,com_item_c.acc_hrt );
	if( ret==100 )
	{
		gl_sub_c.dr_bal=0.00;
		gl_sub_c.cr_bal=0.00;
	}
	else { MYRETERR }

	/* 汇总借方发生额 */
	d_amt=0;c_amt=0;
	ret = sql_sum_double("dc_log", "amt", &d_amt, 
		"tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='1' and sts='0'",
		com_branch_c.br_no,com_item_c.acc_hrt );
	MYRETERR

	ret = sql_sum_double("dc_log", "amt", &c_amt, 
		"tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='2' and sts='0'",
		com_branch_c.br_no,com_item_c.acc_hrt );
	MYRETERR
	sprintf( acErrMsg, "借方金额为:[%.2lf], 贷方金额为:[%.2lf]",d_amt,c_amt );
	WRITEMSG
	bal=gl_sub_c.cr_bal-gl_sub_c.dr_bal;
	bal=bal+c_amt-d_amt;
	if( pub_base_CompDblVal(bal,0.00)<=0 )
	{
		show_log("NOJZ [%s] [%s] [%c] [%.2lf]",
		com_branch_c.br_no,com_item_c.acc_no,yefx,bal);
		continue;
	}
	show_log("JZ   [%s] [%s] [%c] [%.2lf]",
		com_branch_c.br_no,com_item_c.acc_no,yefx,bal);
	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.add_ind, "1");
	g_pub_tx.tx_amt1 = -bal;

	pub_base_strpack( com_item_c.acc_no);
	strcpy( g_pub_tx.ac_no,com_item_c.acc_no);
	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/ 
	g_pub_tx.svc_ind=9001;  /*内部帐存取*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	g_pub_tx.trace_cnt=++num_cnt;
	set_zd_long( DC_TRACE_CNT,num_cnt);
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
		MYRETERR

	/* 记财务部 贷 31205 */
	strcpy(g_pub_tx.add_ind, "1");
	g_pub_tx.tx_amt1 =bal;
	strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
	strcpy( g_pub_tx.opn_br_no,"59801");
	g_pub_tx.trace_cnt=++num_cnt;
	set_zd_long( DC_TRACE_CNT,num_cnt);
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
		MYRETERR
		/**开始打印传票**/
		/**添加传票**/
    memset(filename,0x0,sizeof(filename));
    sprintf(filename,"%s/report/%s/gD304.txt",getenv("HOME"),com_branch_c.br_no);
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
  sprintf( tmp_cxamt, "%.2f" ,g_pub_tx.tx_amt1);
  pub_rpt_flttomon( tmp_cxamt,tmp_cxamt);	/* 小写金额 */
  numtomoney(g_pub_tx.tx_amt1, vstr1);	/* 大写金额 */
  strcat(vstr1,"       ￥");
  strcat(vstr1,tmp_cxamt);
  sprintf(vstr,"%-64s",vstr1);
  vtcp_log("[%s][%d]刘岳 vstr===[%s]\n",__FILE__,__LINE__,vstr);
	/*	fprintf(fp,"%c%c%c%c",0x1b,0x67,0x1c,0x0E);*/
		fprintf(fp, "                               支行利润上划财务中心传票\n");
		fprintf(fp, "                      ---------------------------------------------------------------\n");
		fprintf(fp,"\n");
		fprintf(fp, "     机构号: %5s               交易日期: %ld             流水号: %ld       \n",com_branch_c.br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fp, "┌─────┬─────┬─────┬────────────────────┐\n");
		fprintf(fp, "│科  目  号│31205     │科  目  名│未分配利润                              │\n");
		fprintf(fp, "├─────┼─────┴─────┴────────────────────┤\n");
		fprintf(fp, "│金额(大写)│%s│\n",vstr); 
		fprintf(fp, "├─────┴──────────┬──┬──────────────────┤\n");
		fprintf(fp, "│                                │ 会 │ 科       目(贷):31205(支行)        │\n");
		fprintf(fp, "│  上列款项已从该单位账户结转    │ 计 │                                    │\n");
		fprintf(fp, "│                                │ 分 │ 对 方 科 目(贷):31205 (财务中心)   │\n");
		fprintf(fp, "│                  (开户行盖章)  │ 录 │                                    │\n");
		fprintf(fp, "└────────────────┴──┴──────────────────┘\n");
		fprintf(fp,"\n\n\n");		
		fclose(fp);

		fpcw = fopen( filenamecw,"a" );
		if( fpcw==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filenamecw );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}
		/*fprintf(fpcw,"%c%c%c%c",0x1b,0x67,0x1c,0x0E);*/
		fprintf(fpcw, "                               支行利润上划财务中心传票\n");
		fprintf(fpcw, "                      ---------------------------------------------------------------\n");
		fprintf(fpcw,"\n");
		fprintf(fpcw, "     机构号: %5s               交易日期: %ld             流水号: %ld       \n","59801",g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fpcw, "┌─────┬─────┬─────┬────────────────────┐\n");
		fprintf(fpcw, "│科  目  号│31205     │科  目  名│未分配利润                              │\n");
		fprintf(fpcw, "├─────┼─────┴─────┴────────────────────┤\n");
		fprintf(fpcw, "│金额(大写)│%s│\n",vstr); 
		fprintf(fpcw, "├─────┴──────────┬──┬──────────────────┤\n");
		fprintf(fpcw, "│                                │ 会 │ 科       目(贷):31205(支行)        │\n");
		fprintf(fpcw, "│  上列款项已从该单位账户结转    │ 计 │                                    │\n");
		fprintf(fpcw, "│                                │ 分 │ 对 方 科 目(贷):31205 (财务中心)   │\n");
		fprintf(fpcw, "│                  (开户行盖章)  │ 录 │                                    │\n");
		fprintf(fpcw, "└────────────────┴──┴──────────────────┘\n");
		fprintf(fpcw,"\n\n\n");	
    if(cwnum==3){
        fprintf(fpcw,"\f");
        cwnum=0;
    }
    fclose(fpcw);
    cwnum++;

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
