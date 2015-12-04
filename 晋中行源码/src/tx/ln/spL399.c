/*************************************************
* 文 件 名: spL399.c
* 功能描述：   
*              处理晋中旧贷款转成逾期后要再转回去
*              分户状态改为正常,帐务部分:
*              逾期部分走借方正数,正常部分走借方负数,正好和转逾期相反
* 作    者:    liuxuan
* 完成日期：   2007年05月23日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"   
spL399()  
{
	int ret=0;
	char ori_ac_sts,ac_no[6];
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	

	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));
	memset (&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset (&g_ln_parm, 0x00, sizeof(struct ln_parm_c));

	get_zd_data("0370",g_pub_tx.ac_no);		/* 帐号 */
	get_zd_int("0340",&g_pub_tx.ac_seqn);		/* 账户序号 */

	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
		&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) goto ErrExit;

    /* 检查转出方是否是本行账号 */
    if( strcmp( g_ln_mst.opn_br_no , g_pub_tx.tx_br_no )!=0 )
    {
        sprintf(acErrMsg,"非本机构贷款账号,不可剥离!");
        WRITEMSG
        strcpy ( g_pub_tx.reply , "L222" );
        goto ErrExit;
    }


	ret = pub_ln_check_sts( g_ln_mst,0 ); /*检查贷款户状态*/
	if( ret ) goto ErrExit;

	/* 修改主文件状态 */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d ", g_ln_mst.ac_id , g_pub_tx.ac_seqn );
	if (ret)
	{
		sprintf( acErrMsg,"Prepare Ln_mst_Dec_Upd 游标出错[%d]",ret);
		WRITEMSG
		ERR_DEAL
	}

	memset (&g_ln_mst, 0x00, sizeof(struct ln_mst_c));

	ret = Ln_mst_Fet_Upd( &g_ln_mst , g_pub_tx.reply );
	if (ret==100)
	{
		sprintf( acErrMsg,"贷款主文件中无相应记录[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L014");
		ERR_DEAL
	}else if( ret )
	{
	 		sprintf( acErrMsg,"fetch Ln_mst_Fet_Upd error code = [%d]",ret);
	 		WRITEMSG
	 		strcpy( g_pub_tx.reply ,"D103" );
	 		ERR_DEAL
 	}
	ori_ac_sts=g_ln_mst.ac_sts[0];
	strcpy( g_ln_mst.ac_sts ,"1");

	ret = Ln_mst_Upd_Upd( g_ln_mst , g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg,"Ln_mst_Upd_Upd 出错[%d]",ret);
		WRITEMSG
	 	strcpy( g_pub_tx.reply ,"D103" );
		ERR_DEAL
	}

	Ln_mst_Clo_Upd( );

	/* Pub_tx结构赋值 */

	g_pub_tx.ac_id = g_ln_mst.ac_id;		/* 账户序号 */
	g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;	/* 账户序号 */
	strcpy ( g_pub_tx.ac_id_type , "3" );	/* 账号类型 3-贷款 */
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	strcpy ( g_pub_tx.prt_ind , "0" );	/*是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.hst_ind , "1" );	/*入明细:1日间入2日终单笔入3日终汇总入*/
	strcpy ( g_pub_tx.brf,"调旧贷款" );

	/*** 交易记帐处理 ***/
	if( pub_ins_trace_log())
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		goto ErrExit;
	}
	/* 为内部帐结构赋值 */
	memset(ac_no,'\0',sizeof(ac_no));
	set_zd_data("0210",g_ln_parm.cur_no);/* 对币种赋值 */
	switch(ori_ac_sts){
		case '2':
			strcpy(ac_no,"13001");
			break;
		case '3':
			strcpy(ac_no,"13003");
			break;
		case '4':
			strcpy(ac_no,"13004");
			break;
		case '5':
			strcpy(ac_no,"13002");
			break;
		default:
			sprintf(acErrMsg,"贷款帐户状态不正常,不可做此交易![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"L137");
			goto ErrExit;
	}
	set_zd_data("0210",g_ln_parm.cur_no);
	set_zd_data("1201",ac_no);
	set_zd_data("1204",g_ln_parm.cur_no);
	set_zd_data("1205","2");
	set_zd_double("1208",0-g_ln_mst.bal);
	set_zd_data("120A","调旧贷款");

	g_pub_tx.ac_id =''; 
    get_zd_data("1201",g_pub_tx.ac_no);
    get_zd_data("1206",g_pub_tx.note_type);
    get_zd_data("1207",g_pub_tx.beg_note_no);
    get_zd_data("120A",g_pub_tx.brf);
    get_zd_data("1205",g_pub_tx.ct_ind);
    get_zd_data("1204",g_pub_tx.cur_no);

    strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
    strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
    strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*不核对凭证号，零金额不计流水、明细*/
    strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
    strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
    strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
    g_pub_tx.svc_ind=9001;  /*内部帐存取*/
    strcpy(g_pub_tx.prt_ind,"0");
    strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
	/* 逾期部分记帐 */
/*
    ret=pub_acct_trance();
    if( ret )
    {
        sprintf(acErrMsg,"调用存取款主控失败!");
        WRITEMSG
        goto ErrExit;
    }
	ret = pubf_acct_proc("A016");
	if(ret!=0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
*/


    /* 根据会计代码取出正常贷款时候的本金科目 */
	struct dc_acc_rel_c dc_acc_rel;
	struct com_item_c   com_item;
	memset(&dc_acc_rel,'\0',sizeof(dc_acc_rel));
    ret = Dc_acc_rel_Sel( g_pub_tx.reply , &dc_acc_rel, "dc_code='%s' and data_code='0152'" ,g_ln_parm.dc_code);
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
    sprintf(acErrMsg,"科目控制字[%s]!",dc_acc_rel.acc_hrt);
    WRITEMSG
	memset(&com_item,'\0',sizeof(com_item));
    ret = Com_item_Sel( g_pub_tx.reply,&com_item,"acc_hrt='%s'",dc_acc_rel.acc_hrt);
    if( ret ) goto ErrExit;
    sprintf(acErrMsg,"科目号[%s]!",com_item.acc_no);
    WRITEMSG
/*
	set_zd_data("0330",com_item.acc_no);
	set_zd_double("0430",0-g_ln_mst.bal);
*/
	/* 正常部分 *
	set_zd_data("1201",com_item.acc_no);
	set_zd_data("1204",g_ln_parm.cur_no);
	set_zd_data("1205","2");
	set_zd_double("1208",0-g_ln_mst.bal);
    get_zd_data("1201",g_pub_tx.ac_no);
	ret = pubf_acct_proc("a016");
	if(ret!=0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
*/
	set_zd_data("1211",com_item.acc_no);
	set_zd_double("1218",g_ln_mst.bal);
	ret = pubf_acct_proc("L399");/*调用登记会计流水*/
	if(ret!=0)
	{
		sprintf(acErrMsg,"会计记帐不成功!!");
		WRITEMSG
		goto ErrExit;
	}
/*
	set_zd_data("1211",com_item.acc_no);
	set_zd_data("1214",g_ln_parm.cur_no);
	set_zd_data("1215","2");
	set_zd_double("1218",g_ln_mst.bal);
	set_zd_double("121A","调旧贷款");
*/
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"旧贷款修改成功[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"旧贷款修改失败[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
