/*************************************************************
* 文 件 名: gD075.c
* 功能描述：日终检查密押正确性
*
* 作    者: jack
* 完成日期: 
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

gD075()
{
	int ret = 0;

	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct ln_mst_c sLn_mst;
	struct in_mst_c sIn_mst;
	struct od_mst_c sOd_mst;
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

	/*--- 活期主文件 ---*/
	ret = Dd_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sDd_mst,0x00,sizeof(sDd_mst) );
		ret = Dd_mst_Fet_Sel( &sDd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," 活期密押验证失败!!![%ld],[%d]", sDd_mst.ac_id, sDd_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Dd_mst_Clo_Sel();

	/*--- 定期 ---*/
	ret = Td_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sTd_mst,0x00,sizeof(sTd_mst) );
		ret = Td_mst_Fet_Sel( &sTd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," 定期密押验证失败!!![%ld],[%d]", sTd_mst.ac_id, sTd_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Td_mst_Clo_Sel();

	/*--- 贷款 ---*/
	ret = Ln_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sLn_mst,0x00,sizeof(sLn_mst) );
		ret = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," 贷款密押验证失败!!![%ld],[%d]", sLn_mst.ac_id, sLn_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Ln_mst_Clo_Sel();

	/*--- 内部账 ---*/
	ret = In_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sIn_mst,0x00,sizeof(sIn_mst) );
		ret = In_mst_Fet_Sel( &sIn_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," 内部账密押验证失败!!![%ld],[%d]", sIn_mst.ac_id, sIn_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = In_mst_Clo_Sel();

	/*--- 透支 ---*/
	ret = Od_mst_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sOd_mst,0x00,sizeof(sOd_mst) );
		ret = Od_mst_Fet_Sel( &sOd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," 透支密押验证失败!!![%ld],[%d]", sOd_mst.ac_id, sOd_mst.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Od_mst_Clo_Sel();

	/*--- 介质账户对照表 ---*/
	ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply , " 1=1" );

	while (1)
	{
		memset( &sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Fet_Sel( &sMdm_ac_rel, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret )
		{
			sprintf(acErrMsg," 介质账户关系表密押验证失败!!![%ld],[%d]", sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn );
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = Mdm_ac_rel_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"验证密押成功完毕!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"验证密押失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
