/*************************************************************
* 文 件 名: gD020.c
* 功能描述：年终利润结转(31100->31204)
*
* 作    者: jack
* 完成日期: 2003年03月15日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

gD020()
{
	int ret = 0;

	double tram = 0.00;
	char cAdd_ind[2];


	struct in_mst_c sIn_mst;
	struct in_parm_c sIn_parm;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	/*--- 查询产品参数对应表 ---*/
	memset( &sIn_parm, 0x00, sizeof(sIn_parm) );
	ret = In_parm_Sel( g_pub_tx.reply , &sIn_parm , \
			"acc_hrt = '31100'");
	if ( ret )
	{
		sprintf( acErrMsg, "查询内部账产品参数错[%s] !!!", g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	/*----------查询内部账主文件--------*/ 
	memset( &sIn_mst, 0x00, sizeof(sIn_mst) );
	ret = In_mst_Sel( g_pub_tx.reply , &sIn_mst , \
			"prdt_no = '%s'", sIn_parm.prdt_no);
	if ( ret )
	{
		sprintf( acErrMsg, "查询内部账产品参数错[%s] !!!", g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"-------开始内部账记帐取款------");
	WRITEMSG
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");


	strcpy(g_pub_tx.ac_no,"31100");	
	g_pub_tx.tx_amt1 = sIn_mst.bal;		/*交易金额*/
	strcpy(g_pub_tx.brf,"内部账取款");
	strcpy(g_pub_tx.ct_ind,"2");

	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy( g_pub_tx.ac_wrk_ind,"000");
	strcpy(g_pub_tx.add_ind,"0");		/*减少标志*/
	strcpy( g_pub_tx.prt_ind,"0" ); 	/*不登折*/
	g_pub_tx.svc_ind=9001;  			/*内部帐存取*/
	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/

	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"------开始内部账记帐存款------");
	WRITEMSG
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");

	strcpy(g_pub_tx.ac_no,"31204");	
	g_pub_tx.tx_amt1 = tram;		/*交易金额*/
	strcpy(g_pub_tx.brf,"内部账存款");
	strcpy(g_pub_tx.ct_ind,"2");

	strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
	strcpy( g_pub_tx.ac_wrk_ind,"000"); /*不核对凭证号,零金额不计流水,明细*/
	strcpy(g_pub_tx.add_ind,"1");	/*增加标志*/
	strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
	g_pub_tx.svc_ind=9001;  		/*内部帐存取*/
	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.hst_ind,"1"); 	/*日间入明细*/

	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}

	g_pub_tx.ac_id = 0;
	g_pub_tx.ac_seqn = 0;
	strcpy(g_pub_tx.beg_note_no,"");

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"年终利润结转成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"年终利润结转失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
