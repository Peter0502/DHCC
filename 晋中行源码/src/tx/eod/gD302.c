/***************************************************************
* 文 件 名: gD302.c
* 功能描述: 年末损益转利润 5**->311
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
	long kslsh=0;
	int num=1,num_cnt=1,cwnum=1;
 	FILE *fpcw=NULL;
	char filenamecw[64];
gD302()
{
    int ret=0;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct com_branch_c com_branch_c;
	struct com_item_c com_item_c;
	char old_amt_ind;
 	FILE *fp1=NULL;
	char filename1[64];

	ret=sql_begin(); /*打开事务*/
	MYRETERR

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
            sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
            WRITEMSG
            goto ErrExit;
	}
	kslsh=g_pub_tx.trace_no;
	num_cnt=1;
	cwnum=1;

	memset(&sCom_parm_lr, 0x0, sizeof(struct com_parm_c));
	memset(&sCom_parm_lrfp, 0x0, sizeof(struct com_parm_c));
	/* 取利润结转和利润分配科目 311 31205 */
	ret = Com_parm_Sel( g_pub_tx.reply,&sCom_parm_lr,
		"parm_code='NJKMH' and parm_seqn=1" );
	MYRETERR
	ret = Com_parm_Sel( g_pub_tx.reply,&sCom_parm_lrfp,
		"parm_code='NJKMH' and parm_seqn=2" );
	MYRETERR

	/*ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6') and wrk_sts!='*' order by br_no" );mod by lwb 20150521 增加分行    */
	ret = Com_branch_Dec_Sel( g_pub_tx.reply,
		"br_type not in('0','6','7') and wrk_sts!='*' order by br_no" );
	MYRETERR

	/* 先建立清算中心的传票,下面再一个机构一个机构的往里写 */
    memset(filenamecw,'\0',sizeof(filenamecw));
    sprintf(filenamecw,"%s/report/10001/gD302.txt",getenv("HOME"));
		fpcw = fopen( filenamecw,"a" );
		if( fpcw==NULL )
		{
			sprintf(acErrMsg," open file [%s] error ",filenamecw );
			WRITEMSG
			strcpy( g_pub_tx.reply,"S047" );
			goto ErrExit;
		}
	while( 1 )
	{
		ret = Com_branch_Fet_Sel( &com_branch_c , g_pub_tx.reply );
		if( ret==100 ) break;
		MYRETERR		
		num=1;/***三个为一联***/
   		/**添加传票**/
   		memset(filename1,0x0,sizeof(filename1));
if(memcmp(com_branch_c.br_no,QS_BR_NO,strlen(com_branch_c.br_no))!=0){
   		sprintf(filename1,"%s/report/%s/gD302.txt",getenv("HOME"),com_branch_c.br_no);
   		fp1 = fopen( filename1,"w" ); 
   		if( fp1==NULL )
   		{
   		    sprintf(acErrMsg," open file [%s] error ",filename1 );
   		    WRITEMSG
   		    strcpy( g_pub_tx.reply,"S047" );
   		    goto ErrExit;
   		}
}
   		/**end**/
		ret = Com_item_Dec_Upd( g_pub_tx.reply , 
			"acc_knd!='0' and substr(acc_hrt,1,1)='5' order by acc_no" );
		MYRETERR

		while(1)
		{
			ret = Com_item_Fet_Upd( &com_item_c , g_pub_tx.reply );
			if( ret==100 ) break;
			MYRETERR

			old_amt_ind=com_item_c.amt_dc_ind[0];
			com_item_c.amt_dc_ind[0]='0';		/* 因为公用函数控制了发生额方向所以先置成双向 */
			ret = Com_item_Upd_Upd( com_item_c , g_pub_tx.reply );
			MYRETERR

			ret = sub_change( com_branch_c,com_item_c,fp1);
			MYRETERR

			com_item_c.amt_dc_ind[0]=old_amt_ind;
			ret = Com_item_Upd_Upd( com_item_c , g_pub_tx.reply );
			MYRETERR
		}
		Com_item_Clo_Upd();
		fclose(fp1);
	}
	fclose(fpcw);
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
int sub_change( com_branch_c,com_item_c,fp )
 struct com_branch_c com_branch_c;
 struct com_item_c com_item_c;
 FILE *fp;
{
	int ret=0;
	double d_amt=0.00;
	double c_amt=0.00;
	double bal=0.00;
	char yefx;
	struct gl_sub_c gl_sub_c;

	memset(&g_pub_tx, '\0', sizeof(struct pub_tx));
	pub_base_sysinit();
	strcpy( g_pub_tx.tx_br_no,com_branch_c.br_no );
	strcpy( g_pub_tx.opn_br_no,com_branch_c.br_no );
	strcpy( g_pub_tx.tx_code,"D302");					/* 同 trance.c 里的限制 */
	g_pub_tx.trace_no=kslsh;

	sprintf(acErrMsg, "汇总[%s]的借方金额!!",com_item_c.acc_hrt);
	WRITEMSG

	sprintf(g_pub_tx.brf, "%s机构损益转利润",com_branch_c.br_no);

	ret=Gl_sub_Sel( g_pub_tx.reply,&gl_sub_c ,
			"br_no='%s' and cur_no='01' and acc_hrt='%s' ",
			com_branch_c.br_no,com_item_c.acc_hrt );
	if( ret==100 )
	{
		gl_sub_c.dr_bal=0.00;
		gl_sub_c.cr_bal=0.00;
	}
	else  MYRETERR 


	/* 汇总借方发生额 */
	ret = sql_sum_double("dc_log", "amt", &d_amt, 
		"tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='1' and sts='0' and trace_no<%d",
		com_branch_c.br_no,com_item_c.acc_hrt,kslsh );
	MYRETERR

	ret = sql_sum_double("dc_log", "amt", &c_amt, 
		"tx_opn_br_no='%s' and acc_hrt='%s' and dc_ind='2' and sts='0' and trace_no<%d",
		com_branch_c.br_no,com_item_c.acc_hrt,kslsh );
	MYRETERR

	sprintf( acErrMsg, "科目[%s]借方金额为:[%.2lf], 贷方金额为:[%.2lf] ",com_item_c.acc_hrt,
		d_amt,c_amt);
	WRITEMSG

	if( com_item_c.dc_ind[0]=='1' )
	{
		bal=gl_sub_c.dr_bal-gl_sub_c.cr_bal;
		bal=bal+d_amt-c_amt;
		yefx='1';
	}
	else if( com_item_c.dc_ind[0]=='2' )
	{
		bal=gl_sub_c.cr_bal-gl_sub_c.dr_bal;
		bal=bal+c_amt-d_amt;
		yefx='2';
	}
	else
	{
		bal=gl_sub_c.dr_bal-gl_sub_c.cr_bal;
		bal=bal+d_amt-c_amt;
		if( pub_base_CompDblVal(bal,0.00)<0 )
		{
			bal=bal*(-1);
			yefx='2';
		}
		else
		{
			yefx='1';
		}
	}

	if( pub_base_CompDblVal(bal,0.00)==0 )
	{
	show_log("NOJZ [%s] [%s] [%c] [%.2lf]",
		com_branch_c.br_no,com_item_c.acc_no,yefx,bal);
		return 0;
	}
	show_log("JZ   [%s] [%s] [%c] [%.2lf]",
		com_branch_c.br_no,com_item_c.acc_no,yefx,bal);

	strcpy(g_pub_tx.cur_no, "01");
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.add_ind, "1");
	g_pub_tx.tx_amt1 = bal;

	strcpy( g_pub_tx.opn_br_no,QS_BR_NO);	/* 重置将交易机构置为清算中心 */
	pub_base_strpack( sCom_parm_lr.val );
	strcpy( g_pub_tx.ac_no,sCom_parm_lr.val ); 
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
		vtcp_log("流水[%ld]\n",g_pub_tx.trace_cnt);
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	if (yefx == '1')
	{
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
		MYRETERR
	}
	else if (yefx == '2')
	{
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
		MYRETERR
	}
	else
	{
		strcpy(g_pub_tx.reply, "P404");
		sprintf(acErrMsg, "借贷标志错误!! [%s]", g_pub_tx.add_ind);
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("开始记录损益科目\n");
	strcpy( g_pub_tx.opn_br_no,com_branch_c.br_no);	/* 损益记的是支行 */
	strcpy(g_pub_tx.add_ind, "0");
	strcpy(g_pub_tx.ac_no, com_item_c.acc_no );
	g_pub_tx.trace_no=kslsh;
	g_pub_tx.trace_cnt=++num_cnt;
	set_zd_long( DC_TRACE_CNT,num_cnt);
	ret = pub_acct_trance(); 
	if (ret != 0) 
	{ 
		vtcp_log("流水[%ld]\n",g_pub_tx.trace_cnt);
		sprintf(acErrMsg, "调用存取款主控失败!!"); 
		WRITEMSG 
		goto ErrExit; 
	}

	if (yefx == '2')
	{
		set_zd_double("1208",g_pub_tx.tx_amt1); 
		set_zd_data("120A",g_pub_tx.brf); 
		set_zd_data("1205",g_pub_tx.ct_ind); 
		set_zd_data("1204",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A016");
		MYRETERR
	}
	else 
	{
		set_zd_double("1218",g_pub_tx.tx_amt1); 
		set_zd_data("121A",g_pub_tx.brf); 
		set_zd_data("1215",g_pub_tx.ct_ind); 
		set_zd_data("1214",g_pub_tx.cur_no);
		ret = pubf_acct_proc("A017");
		MYRETERR
	}
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
if(memcmp(com_branch_c.br_no,QS_BR_NO,strlen(com_branch_c.br_no))!=0){
	fprintf(fp,"\n\n\n\n\n\n\n\n\n");
		fprintf(fp, "                          支行年终损益转清算利润传票\n");
		fprintf(fp, "                 ------------------------------------------\n");
		fprintf(fp, "\n");
		fprintf(fp, "     机构号: %5s               交易日期: %ld            流水号: %ld       \n",com_branch_c.br_no,g_pub_tx.tx_date,g_pub_tx.trace_no);
		fprintf(fp, "┌─────┬─────┬─────┬────────────────────┐\n");
		fprintf(fp, "│科  目  号│%-7s   │科  目  名│%-30s          │\n",com_item_c.acc_no,com_item_c.acc_name);
		fprintf(fp, "├─────┼─────┴─────┴────────────────────┤\n");
		fprintf(fp, "│金额(大写)│%s│\n",vstr); 
		fprintf(fp, "├─────┴──────────┬──┬──────────────────┤\n");
		if(yefx=='2')
		{
		fprintf(fp, "│                                │ 会 │ 科       目(借):%-7s            │\n",com_item_c.acc_no);
		}
		else
		{
		fprintf(fp, "│                                │ 会 │ 清 算 科 目(借):%-7s            │\n",sCom_parm_lr.val);
		}	
		fprintf(fp, "│  上列款项已从该单位账户结转    │ 计 │                                    │\n");
		if(yefx=='2')
		{
		fprintf(fp, "│                                │ 分 │ 清 算 科 目(贷):%-7s            │\n",sCom_parm_lr.val);
		}
		else
		{
		fprintf(fp, "│                                │ 分 │ 科       目(贷):%-7s            │\n",com_item_c.acc_no);
		}
		fprintf(fp, "│                  (开户行盖章)  │ 录 │                                    │\n");
		fprintf(fp, "└────────────────┴──┴──────────────────┘\n");
		fprintf(fp,"\n\n\n\n\n\n\n\n\n");
		if(num%2==0)
		{
			fprintf(fp,"\f");
		}
		num++;
}

fprintf(fpcw,"\n\n\n\n\n\n\n\n\n");
		fprintf(fpcw, "                      %s 行损益上划清算中心传票\n",com_branch_c.br_no);
		fprintf(fpcw, "          ---------------------------------------------------------------\n");
		fprintf(fpcw,"\n");
		fprintf(fpcw, "     机构号: %5s               交易日期: %ld             流水号: %ld       \n",QS_BR_NO,g_pub_tx.tx_date,g_pub_tx.trace_no);
fprintf(fpcw, "┌─────┬─────┬─────┬────────────────────┐\n");
fprintf(fpcw, "│科  目  号│311       │科  目  名│本年利润                                │\n");
fprintf(fpcw, "├─────┼─────┴─────┴────────────────────┤\n");
fprintf(fpcw, "│金额(大写)│%s│\n",vstr); 
fprintf(fpcw, "├─────┴──────────┬──┬──────────────────┤\n");
	if(yefx=='2') {
fprintf(fpcw, "│                                │ 会 │ 科       目(借):%-7s            │\n",com_item_c.acc_no);
		}
	else {
fprintf(fpcw, "│                                │ 会 │ 科       目(借):%-7s            │\n",sCom_parm_lr.val);
		}	
fprintf(fpcw, "│  上列款项已从该单位账户结转    │ 计 │                                    │\n");
	if(yefx=='2'){
fprintf(fpcw, "│                                │ 分 │ 对 方 科 目(贷):%-7s            │\n",sCom_parm_lr.val);
		}
		else
		{
fprintf(fpcw, "│                                │ 分 │ 对 方 科 目(贷):%-7s            │\n",com_item_c.acc_no);
		}
fprintf(fpcw, "│                  (开户行盖章)  │ 录 │                                    │\n");
fprintf(fpcw, "└────────────────┴──┴──────────────────┘\n");
fprintf(fpcw,"\n\n\n\n\n\n\n\n\n");
    if(cwnum==2){
        fprintf(fpcw,"\f");
        cwnum=0;
    }
    cwnum++;

	return 0;
ErrExit:
	return 1;
}
