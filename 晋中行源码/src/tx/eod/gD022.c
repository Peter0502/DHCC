/*************************************************************
* 文 件 名: gD022.c
* 功能描述：日终滚积数
*
* 作    者: jack
* 完成日期: 2003年03月15日
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

gD022()
{
	int ret = 0;

	ret=sql_begin(); /*打开事务*/
	if( ret ) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	ret = DD_add_acm();
	if( ret ) 
	{
		sprintf( acErrMsg, "活期滚积数失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = TD_add_acm();
	if( ret ) 
	{
		sprintf( acErrMsg, "定期滚积数失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = IN_add_acm();
	if( ret ) 
	{
		sprintf( acErrMsg, "内部账滚积数失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = LN_add_acm();
	if( ret )
	{
		sprintf( acErrMsg, "贷款滚积数失败!!!" );
		WRITEMSG
		goto ErrExit;
	}


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终滚积数成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终滚积数失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}


/*------------------------------------------------*
* 功能: 活期主文件滚积数
*
*
*------------------------------------------------*/
int DD_add_acm()
{
	int ret = 0;

	struct dd_parm_c sDd_parm;
	struct dd_mst_c sDd_mst;

	ret = Dd_parm_Dec_Sel ( g_pub_tx.reply , "acm_calc_type = '1'");
	if ( ret )
	{
		sprintf(acErrMsg,"执行Dd_parm_Dec_Sel失败!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while ( 1 )
	{
		memset( &sDd_parm, 0x00, sizeof(sDd_parm) );

		ret = Dd_parm_Fet_Sel ( &sDd_parm , g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Dd_parm_Fet_Sel失败!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Dd_mst_Dec_Sel ( g_pub_tx.reply , " prdt_no = '%s' and bal != 0.00", sDd_parm.prdt_no ); 
		if ( ret )
		{
			sprintf(acErrMsg,"执行Dd_mst_Dec_Sel失败!!!![%s]prdt[%s]",g_pub_tx.reply,sDd_parm.prdt_no);
			WRITEMSG
			goto ErrExit;
		}

		while ( 1 )
		{
			memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
			ret = Dd_mst_Fet_Sel ( &sDd_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"执行Dd_mst_Fet_Sel失败!!!![%s]",g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			/*------------------积数累加-----------------*/
			sDd_mst.intst_acm = sDd_mst.intst_acm + sDd_mst.bal;

			ret = sql_execute( "update dd_mst set intst_acm = %f \
				where ac_id = %d and ac_seqn = %d ", \
				sDd_mst.intst_acm, sDd_mst.ac_id, sDd_mst.ac_seqn);
			if ( ret )
			{
				sprintf(acErrMsg,"修改活期积数失败!!!!ID[%d]seqn[%d]",\
					sDd_mst.ac_id, sDd_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	Dd_parm_Clo_Sel ();
	Dd_mst_Clo_Sel ();

OkExit:
	return 0;
ErrExit:
	Dd_parm_Clo_Sel ();
	Dd_mst_Clo_Sel ();
	return 1;
}


/*------------------------------------------------*
* 功能: 定期主文件滚积数
*
*
*------------------------------------------------*/
int TD_add_acm()
{
	int ret = 0;

	struct td_parm_c sTd_parm;
	struct td_mst_c sTd_mst;

	ret = Td_parm_Dec_Sel ( g_pub_tx.reply , "acm_calc_type = '1'");
	if ( ret )
	{
		sprintf(acErrMsg,"执行Td_parm_Dec_Sel失败!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while ( 1 )
	{
		memset( &sTd_parm, 0x00, sizeof(sTd_parm) );

		ret = Td_parm_Fet_Sel ( &sTd_parm , g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Td_parm_Fet_Sel失败!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = Td_mst_Dec_Sel ( g_pub_tx.reply , " prdt_no = '%s' and bal != 0.00", sTd_parm.prdt_no ); 
		if ( ret )
		{
			sprintf(acErrMsg,"执行Td_mst_Dec_Sel失败!!!![%s]prdt[%s]",g_pub_tx.reply,sTd_parm.prdt_no);
			WRITEMSG
			goto ErrExit;
		}

		while ( 1 )
		{
			memset( &sTd_mst, 0x00, sizeof(sTd_mst) );
			ret = Td_mst_Fet_Sel ( &sTd_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"执行Td_mst_Fet_Sel失败!!!![%s]",g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			/*------------------积数累加-----------------*/
			sTd_mst.intst_acm = sTd_mst.intst_acm + sTd_mst.bal;

			ret = sql_execute( "update td_mst set intst_acm = %f \
				where ac_id = %d and ac_seqn = %d ", \
				sTd_mst.intst_acm, sTd_mst.ac_id, sTd_mst.ac_seqn);
			if ( ret )
			{
				sprintf(acErrMsg,"修改定期积数失败!!!!ID[%d]seqn[%d]",\
					sTd_mst.ac_id, sTd_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	Td_parm_Clo_Sel ();
	Td_mst_Clo_Sel ();

OkExit:
	return 0;
ErrExit:
	Td_parm_Clo_Sel ();
	Td_mst_Clo_Sel ();
	return 1;
}


/*------------------------------------------------*
* 功能: 内部账主文件滚积数
*
*
*------------------------------------------------*/
int IN_add_acm()
{
	int ret = 0;

	struct in_parm_c sIn_parm;
	struct in_mst_c sIn_mst;

	ret = In_parm_Dec_Sel ( g_pub_tx.reply , "acm_calc_type = '1'");
	if ( ret )
	{
		sprintf(acErrMsg,"执行In_parm_Dec_Sel失败!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while ( 1 )
	{
		memset( &sIn_parm, 0x00, sizeof(sIn_parm) );

		ret = In_parm_Fet_Sel ( &sIn_parm , g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"执行In_parm_Fet_Sel失败!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		ret = In_mst_Dec_Sel ( g_pub_tx.reply , " prdt_no = '%s' and bal != 0.00", sIn_parm.prdt_no ); 
		if ( ret )
		{
			sprintf(acErrMsg,"执行In_mst_Dec_Sel失败!!!![%s]prdt[%s]",g_pub_tx.reply,sIn_parm.prdt_no);
			WRITEMSG
			goto ErrExit;
		}

		while ( 1 )
		{
			memset( &sIn_mst, 0x00, sizeof(sIn_mst) );
			ret = In_mst_Fet_Sel ( &sIn_mst, g_pub_tx.reply );
			if ( ret )
			{
				sprintf(acErrMsg,"执行In_mst_Fet_Sel失败!!!![%s]",g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			/*------------------积数累加-----------------*/
			sIn_mst.intst_acm = sIn_mst.intst_acm + sIn_mst.bal;

			ret = sql_execute( "update in_mst set intst_acm = %f \
				where ac_id = %d and ac_seqn = %d ", \
				sIn_mst.intst_acm, sIn_mst.ac_id, sIn_mst.ac_seqn);
			if ( ret )
			{
				sprintf(acErrMsg,"修改内部账积数失败!!!!ID[%d] \
					seqn[%d]", sIn_mst.ac_id, sIn_mst.ac_seqn);
				WRITEMSG
				goto ErrExit;
			}
		}
	}
	In_parm_Clo_Sel ();
	In_mst_Clo_Sel ();

OkExit:
	return 0;
ErrExit:
	In_parm_Clo_Sel ();
	In_mst_Clo_Sel ();
	return 1;
}

