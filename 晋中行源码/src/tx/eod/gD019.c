/*************************************************************
* 文 件 名: gD019.c
* 功能描述：年终损益转利润
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

gD019()
{
	int ret = 0;

	double tram = 0.00;
	char cAdd_ind[2];


	struct in_mst_c sIn_mst;
	struct in_parm_c sIn_parm;
	struct com_item_c sCom_item;
	struct mdm_ac_rel_c sMdm_ac_rel;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	ret = In_mst_Dec_Sel( g_pub_tx.reply ,"bal != 0.00 and sts = '1'" );
	if ( ret )
	{
		sprintf( acErrMsg, "查询内部账出错 !!!" );
		WRITEMSG
		goto ErrExit;
	}

	while( 1 )
	{
		memset( &sIn_mst, 0x00, sizeof(sIn_mst) );

		ret = In_mst_Fet_Sel ( &sIn_mst , g_pub_tx.reply );
		if (ret == 100)
		{
			break;
		}
		if ( ret )
		{
			sprintf( acErrMsg, "查询内部账主文件错[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*-------- 查询介质账号对照表 ----------*/
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel , \
				" ac_id = %d and ac_seqn = %d", \
					sIn_mst.ac_id,sIn_mst.ac_seqn);
		if ( ret )
		{
			sprintf( acErrMsg, "查询账号介质对照表错[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		/*--- 查询产品参数对应表 ---*/
		memset( &sIn_parm, 0x00, sizeof(sIn_parm) );
		ret = In_parm_Sel( g_pub_tx.reply , &sIn_parm , \
				"prdt_no = '%s'",sIn_mst.prdt_no);
		if ( ret )
		{
			sprintf( acErrMsg, "查询内部账产品参数错[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		if ( sIn_parm.acc_hrt[0] != '5' )
		{
			continue;	/*------ 非损益类科目 ------*/
		}

		memset( &sCom_item, 0x00, sizeof(sCom_item) );
		ret = Com_item_Sel( g_pub_tx.reply, &sCom_item, \
				"acc_hrt = '%s'", sIn_parm.acc_hrt);
		if ( ret )
		{
			sprintf( acErrMsg, "查询公共科目表错[%s] !!!", g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		memset( cAdd_ind, 0x0, sizeof(cAdd_ind) );
		strcpy( cAdd_ind, sCom_item.dc_ind );

		tram = sIn_mst.bal;

		if (sCom_item.dc_ind[0] == '0')
		{
			if (sIn_mst.bal > 0.00)
			{
				strcpy( cAdd_ind, "1" );	/*余额借方*/
			}
			else
			{
				tram = -tram;
				strcpy( cAdd_ind, "2" );	/*余额贷方*/
			}
		}

		sprintf(acErrMsg,"-------开始内部账记帐取款------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");

		/*--损益余额在借方则借31100,如果余额在贷方则借损益--*/
		if ( cAdd_ind[0] == '1' )
		{
			strcpy(g_pub_tx.ac_no,"31100");	
		}
		if ( cAdd_ind[0] == '2' )
		{
			strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
		}

		g_pub_tx.tx_amt1 = tram;		/*交易金额*/
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

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;

		sprintf(acErrMsg,"------开始内部账记帐存款------");
		WRITEMSG
		memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
		memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
		memset(&g_in_parm,0x00,sizeof(struct in_parm_c));

		g_pub_tx.ac_id = 0;
		g_pub_tx.ac_seqn = 0;
		strcpy(g_pub_tx.beg_note_no,"");

		/*--损益余额在借方则贷损益,余额在贷方则贷31100--*/
		if ( cAdd_ind[0] == '1' )
		{
			strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
		}
		if ( cAdd_ind[0] == '2' )
		{
			strcpy(g_pub_tx.ac_no,"31100");	
		}

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
	}

	In_mst_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"年终损益转利润通过!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	In_mst_Clo_Sel();
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"年终损益转利润失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
