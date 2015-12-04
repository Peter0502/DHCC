/*************************************************************
* 文 件 名: gD092.c
* 功能描述：保证金按季收取担保费
*
* 完成日期: 2003年08月19日
*
* 作    者：dong
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
#include "protect_note_c.h"

gD092()
{
	int ret = 0;
	struct	protect_note_c	pro_note;

	memset( &pro_note , 0x00 , sizeof( struct protect_note_c ));


	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	/** 取出当前需要 收取担保费的纪录 **/
	ret = Protect_note_Dec_Upd(g_pub_tx.reply,"nex_date=%d AND protect_sts='0' ",g_pub_tx.tx_date);
	while(1)
	{
		ret = Protect_note_Fet_Upd( &pro_note , g_pub_tx.reply);
		if ( ret ) break;
		ret=sql_begin(); /*打开事务*/
		if( ret ) 
		{
			sprintf( acErrMsg, "打开事务失败!!!" );
			WRITEMSG
			goto ErrExit;
		}
		g_pub_tx.tx_amt1 = pro_note.amt*pro_note.ratio/100;

		vtcp_log("PACT_NO[%s] amt=[%lf] ratio=[%d] amt1=[%lf] [%s->%d]"
			,pro_note.pact_no,pro_note.amt,pro_note.ratio,g_pub_tx.tx_amt1,__FILE__,__LINE__);
		/*************************
		*   借申请人201 
		*************************/
		set_zd_data("1021",pro_note.pro_ac_no);
		set_zd_int ("1022",1);
		set_zd_data("102J","01");
		set_zd_data("102K","2");					/*转帐*/
		set_zd_data("102O","0");					/*是否打印存折(有折无折标志):1是0否*/
		set_zd_data("102Y","借 申请人帐户");
		set_zd_double("102F",g_pub_tx.tx_amt1);
		if ( pub_base_my_func("D003","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"借 申请人帐户D003错![%s]",g_pub_tx.reply);
			WRITEMSG
			sql_rollback();	/*--事务回滚--*/
			continue;
		}
		/**************************
		*   贷担保业务手续费收入51106
		***************************/
		set_zd_data("1211","51106");
		set_zd_data("121A","贷担保业务手续费收入" );
		set_zd_data("1214","01");
		set_zd_data("1215","2");
		set_zd_double("1218",g_pub_tx.tx_amt1);
		if ( pub_base_my_func("A017","1",g_pub_tx.reply))
		{
			sprintf(acErrMsg,"贷 其他应受应付A017错![%s]",g_pub_tx.reply);
			WRITEMSG
			sql_rollback();	/*--事务回滚--*/
			continue;
		}
		/** 计算下一结息日 **/
		ret = pub_base_deadlineM( g_pub_tx.tx_date, 3 ,&pro_note.nex_date );
		if ( ret )
		{
			sprintf(acErrMsg,"net_date error [%d] [%d]",pro_note.nex_date,ret );
			WRITEMSG
			sql_rollback();	/*--事务回滚--*/
			continue;
		}
		ret = Protect_note_Upd_Upd( pro_note , g_pub_tx.reply);
		if ( ret )
		{
			sprintf(acErrMsg,"note upd error [%d] [%d]",pro_note.pro_ac_no,ret );
			WRITEMSG
			sql_rollback();	/*--事务回滚--*/
			continue;
		}
		ret = sql_commit();	/*--提交事务--*/
		if( ret ) 
		{
			sprintf( acErrMsg, "提交事务失败!!!" );
			WRITEMSG
			goto ErrExit;
		}

	}
	Protect_note_Clo_Upd();

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"保证金按季收取担保费处理成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"保证金按季收取担保费失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
