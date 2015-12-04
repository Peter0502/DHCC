/*************************************************
* 文 件 名: spL310.c
* 功能描述：   
*              贷款剥离
*
* 作    者:    lance
* 完成日期：   2003年06月04日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define EXTERN
#include "public.h"
#include "ln_pay_pln_c.h"
#include "dc_acc_rel_c.h"
#include "ass_br_def_c.h"
#include "ass_gl_c.h"

	char brno[6];
spL310()  
{ 		
	int ret=0;
	long pay_date,term_opn_date;
	int i=0;
	int ln_cnt=0,month_cnt=0;
	int a=1;
	
	char dc_inf[5];
	
	
  struct ln_pay_pln_c	ln_pay_pln;	
	struct dc_acc_rel_c 	dc_acc_rel_tmp;
	struct ass_br_def_c 	ass_br_def_out;
	struct ass_br_def_c 	ass_br_def_in;	
	struct ass_gl_c 	ass_gl_out;
	struct ass_gl_c 	ass_gl_in;
	struct ln_mst_c 	p_ln_mst;
				
	memset(&dc_acc_rel_tmp,0x00,sizeof(struct dc_acc_rel_c));
	memset(&ln_pay_pln,0x00,sizeof(struct ln_pay_pln_c));	
	memset(&ass_br_def_out,0x00,sizeof(struct ass_br_def_c));	
	memset(&ass_br_def_in,0x00,sizeof(struct ass_br_def_c));	
	memset(&ass_gl_out,0x00,sizeof(struct ass_gl_c));
	memset(&ass_gl_in,0x00,sizeof(struct ass_gl_c));
	memset(&p_ln_mst,0x00,sizeof(struct ln_mst_c));
						
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
	
	/** 取值、赋值 **/
	if ( data_set_struct() )    ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG	
	
	sprintf(acErrMsg,"读取贷款户信息[%s][%d]",g_pub_tx.ac_no,g_pub_tx.ac_seqn);
	WRITEMSG		
	
	
	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG
	
	
	/* 检查转入转出方是否是同一网点 */
	if( strcmp( g_ln_mst.opn_br_no , brno )==0 )
	{
		sprintf(acErrMsg,"转入机构与贷款现所属机构为同一机构,不可剥离!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L221" );		
		goto ErrExit;
	}
	
	/* 检查转出方是否是本行账号 */
	if( strcmp( g_ln_mst.opn_br_no , g_pub_tx.tx_br_no )==0 )
	{
		sprintf(acErrMsg,"非本机构贷款账号,不可剥离!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L222" );		
		goto ErrExit;
	}
		
	/* 取科目号 */
	/* 根据贷款状态取会计代码 */
	if ( g_ln_mst.ac_sts[0]=='1' )	/* 正常 */
	{
		strcpy( dc_inf , g_ln_parm.dc_code );
	}
	if ( g_ln_mst.ac_sts[0]=='2' )	/* 逾期 */
	{
		strcpy( dc_inf , g_ln_parm.over_dc_code );
	}	
	if ( g_ln_mst.ac_sts[0]=='3' )	/* 呆滞 */
	{
		strcpy( dc_inf , g_ln_parm.sla_dc_code );
	}	
	if ( g_ln_mst.ac_sts[0]=='4' )	/* 呆账 */
	{
		strcpy( dc_inf , g_ln_parm.bad_dc_code );
	}
	/* 根据会计代码取科目号 */
	ret = Dc_acc_rel_Sel( g_pub_tx.reply , &dc_acc_rel_tmp , "dc_code='%s' and data_code='0158' " , dc_inf );
	if( ret && ret!=100 )
	{
		sprintf(acErrMsg,"取科目号异常出错,请与中心机房联系!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L128" );		
		goto ErrExit;
	}else if ( ret==100 ){
		sprintf(acErrMsg,"无此科目号,请与中心机房联系!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L129" );		
		goto ErrExit;
	}				
	sprintf(acErrMsg,"科目号[%s]系!",dc_acc_rel_tmp.acc_hrt);
	WRITEMSG	


	/* 根据机构号在考核机构定义表中取考核机构代码(移入) */
	/* 考核机构不存在 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s'" , brno );
	if( ret && ret!=100 )
	{
		sprintf(acErrMsg,"取考核机构信息异常出错,请与中心机房联系!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L130" );		
		goto ErrExit;
	}else if ( ret==100 ){
		sprintf(acErrMsg,"此机构无对应的考核机构!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L131" );		
		goto ErrExit;
	}
	/* 考核机构尚未启用 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s' and sts='*' " ,brno );
	if( ret==0 )
	{
		sprintf(acErrMsg,"该考核机构尚未启用!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}
	
	/* 考核机构尚未启用 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s' and sts='1' and beg_date>%ld" ,brno , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"该考核机构尚未启用!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}

	/* 考核机构过期 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_in , "sub_br_no='%s' and sts='1' and end_date<%ld" ,brno , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"该考核机构已过期!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L133" );		
		goto ErrExit;
	}	
	sprintf(acErrMsg,"该考核机构(移入)[%s]原[%s]!",ass_br_def_in.br_no,brno);
	WRITEMSG		
	
	/* 根据机构号在考核机构定义表中取考核机构代码（移出） */
	/* 考核机构不存在 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s'" , g_ln_mst.opn_br_no );
	if( ret && ret!=100 )
	{
		sprintf(acErrMsg,"取考核机构信息异常出错,请与中心机房联系!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L130" );		
		goto ErrExit;
	}else if ( ret==100 ){
		sprintf(acErrMsg,"此机构无对应的考核机构!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L131" );		
		goto ErrExit;
	}
		
	/* 考核机构尚未启用 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s' and sts='*' " ,g_ln_mst.opn_br_no );
	if( ret==0 )
	{
		sprintf(acErrMsg,"该考核机构尚未启用!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}

	/* 考核机构尚未启用 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s' and sts='1' and beg_date>%ld" ,g_ln_mst.opn_br_no , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"该考核机构尚未启用!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L132" );		
		goto ErrExit;
	}

	
	/* 考核机构过期 */
	ret = Ass_br_def_Sel( g_pub_tx.reply , &ass_br_def_out , "sub_br_no='%s' and sts='1' and end_date<%ld" ,g_ln_mst.opn_br_no , g_pub_tx.tx_date );
	if( ret==0 )
	{
		sprintf(acErrMsg,"该考核机构已过期!");
		WRITEMSG
		strcpy ( g_pub_tx.reply , "L133" );		
		goto ErrExit;
	}	
	sprintf(acErrMsg,"该考核机构(移出)[%s]原[%s]!",ass_br_def_out.br_no,g_ln_mst.opn_br_no);
	WRITEMSG
			
	
	/* 更新考核总账-移出机构 */
	ret = Ass_gl_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and ass_code='%s'and sts='1'",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
	if( ret )
	{
		sprintf(acErrMsg,"不存在此考核项目![%s][%s]",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}	
	ret = Ass_gl_Fet_Upd( &ass_gl_out , g_pub_tx.reply );
	if( ret==100 )
	{
		memset( &ass_gl_out,0x00,sizeof(struct ass_gl_c) );
		strcpy( ass_gl_out.br_no,ass_br_def_out.br_no );
		strcpy( ass_gl_out.ass_code,dc_acc_rel_tmp.acc_hrt );
		ass_gl_out.in_bal=0.00;
		ass_gl_out.out_bal=0.00;
		ass_gl_out.out_bal = ass_gl_out.out_bal-g_ln_mst.in_lo_intst;		
		ass_gl_out.in_intst_bal=0.00;
		ass_gl_out.out_intst_bal=0.00;
		ass_gl_out.sts[0]='1';
		ret = Ass_gl_Ins( ass_gl_out,g_pub_tx.reply );
		if( ret )
		{
	        	sprintf(acErrMsg,"Insert!! [%d]",ret);
	        	WRITEMSG
	        	goto ErrExit;
		}
	}else if(ret){
		sprintf(acErrMsg,"取考核项目异常出错![%s][%s]",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}else{
	
		ass_gl_out.out_bal = ass_gl_out.out_bal-g_ln_mst.in_lo_intst;
			
		ret = Ass_gl_Upd_Upd( ass_gl_out , g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"不存在此考核项目![%s][%s]",ass_br_def_out.br_no,dc_acc_rel_tmp.acc_hrt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L134");
			goto ErrExit;
		}	
		Ass_gl_Clo_Upd();
	}
	
	/* 更新考核总账-移入机构 */
	ret = Ass_gl_Dec_Upd( g_pub_tx.reply ,"br_no='%s' and ass_code='%s'and sts='1'",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
	if( ret )
	{
		sprintf(acErrMsg,"不存在此考核项目![%s][%s]",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}	
	ret = Ass_gl_Fet_Upd( &ass_gl_in , g_pub_tx.reply );
	if( ret==100 )
	{
		memset( &ass_gl_in,0x00,sizeof(struct ass_gl_c) );
		strcpy( ass_gl_in.br_no,ass_br_def_in.br_no );
		strcpy( ass_gl_in.ass_code,dc_acc_rel_tmp.acc_hrt );
		ass_gl_in.in_bal=0.00;
		ass_gl_in.out_bal=0.00;
		ass_gl_in.out_bal = ass_gl_in.out_bal+g_ln_mst.in_lo_intst;
		ass_gl_in.in_intst_bal=0.00;
		ass_gl_in.out_intst_bal=0.00;
		ass_gl_in.sts[0]='1';
		ret = Ass_gl_Ins( ass_gl_in,g_pub_tx.reply );
		if( ret )
		{
	        	sprintf(acErrMsg,"Insert!! [%d]",ret);
	        	WRITEMSG
	        	goto ErrExit;
		}
	}else if(ret){
		sprintf(acErrMsg,"取考核项目异常出错![%s][%s]",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L134");
		goto ErrExit;
	}else{
	
		ass_gl_in.out_bal = ass_gl_in.out_bal+g_ln_mst.in_lo_intst;
			
		ret = Ass_gl_Upd_Upd( ass_gl_in , g_pub_tx.reply);
		if( ret )
		{
			sprintf(acErrMsg,"不存在此考核项目![%s][%s]",ass_br_def_in.br_no,dc_acc_rel_tmp.acc_hrt);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L134");
			goto ErrExit;
		}	
		Ass_gl_Clo_Upd();
	}

	/* Pub_tx结构赋值 */
	g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst;	
	g_pub_tx.cif_no = g_ln_mst.cif_no;			/* 客户号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "0" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3059;			/* 贷款结转 */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.brf,"贷款剥离(移出)" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG

	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG


	
	/* 修改贷款主文件,更新所属网点号 */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d  ", g_mdm_ac_rel.ac_id , g_pub_tx.ac_seqn );
	if( ret )
	{
		sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	ret = Ln_mst_Fet_Upd( &p_ln_mst, g_pub_tx.reply );
	if( ret==100 )
	{
		sprintf( acErrMsg,"无记录[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
   	if( ret )
 	{
	 	sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
		goto ErrExit;
 	}
			
	strcpy( p_ln_mst.opn_br_no , brno );		
	
	ret = Ln_mst_Upd_Upd( p_ln_mst, g_pub_tx.reply  );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}		
		
	Ln_mst_Clo_Upd( );
	sprintf(acErrMsg,"贷款主文件中积数清零 PASS");
	WRITEMSG		


	/* Pub_tx结构赋值 */
	g_pub_tx.tx_amt2 = g_ln_mst.in_lo_intst;	
	g_pub_tx.cif_no = g_ln_mst.cif_no;			/* 客户号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.intst_type , g_ln_mst.intst_type );	/* 计息类型 */	
	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "1" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3059;			/* 贷款结转 */
	strcpy( g_pub_tx.ac_wrk_ind , "0110000" );
	strcpy ( g_pub_tx.brf,"贷款剥离(移入)" );
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG

	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
		
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款剥离成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款剥离失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	
	return 0;
	
}

int data_set_struct()
{    
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款账号 */	
	get_zd_data("0910",brno);			/* 接收网点 */
		
	g_pub_tx.ac_seqn=0;				/* 贷款账号序号 */

	sprintf(acErrMsg,"进入赋值函数贷款账号[%s]网点[%s]",g_pub_tx.ac_no,brno);
	WRITEMSG						
	return 0;
}

