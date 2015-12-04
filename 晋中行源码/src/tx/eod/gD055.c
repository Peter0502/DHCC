/*************************************************************
* 文 件 名: gD055.c
* 功能描述：积分清理
*
* 作    者: jack
* 完成日期: 2003年03月16日
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
#include "point_point_mst_c.h"
#include "prdt_point_mst_c.h"

gD055()
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

	ret = clr_prt_point_mst();
	if( ret ) 
	{
		sprintf( acErrMsg, "清理产品积分失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = clr_ac_point_mst();
	if( ret ) 
	{
		sprintf( acErrMsg, "清理介质积分失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"积分整理成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"积分整理失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/*-----------------------------------------------*
* 功能: 介质积分主表整理
*
*
*-----------------------------------------------*/
int clr_ac_point_mst()
{
	int ret = 0;

	struct point_point_mst_c sPoint_point_mst;

	ret = Point_point_mst_Dec_Upd( g_pub_tx.reply , " 1 = 1 ");
	if ( ret )
	{
		sprintf(acErrMsg,"执行Point_point_mst_Dec_Upd出错!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sPoint_point_mst, 0x00, sizeof(sPoint_point_mst) );

		ret = Point_point_mst_Fet_Upd( &sPoint_point_mst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Point_point_mst_Fet_Upd出错!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		sPoint_point_mst.yt_point = sPoint_point_mst.point + sPoint_point_mst.used_point;
		sPoint_point_mst.point = 0;
		sPoint_point_mst.used_point = 0;

		ret = Point_point_mst_Upd_Upd( sPoint_point_mst, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Point_point_mst_Upd_Upd出错!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}


/*-----------------------------------------------*
* 功能: 产品积分主表整理
*
*
*-----------------------------------------------*/
int clr_prt_point_mst()
{
	int ret = 0;

	struct prdt_point_mst_c sPrdt_point_mst;

	ret = Prdt_point_mst_Dec_Upd( g_pub_tx.reply , " 1 = 1 ");
	if ( ret )
	{
		sprintf(acErrMsg,"执行Prdt_point_mst_Dec_Upd出错!!!!![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sPrdt_point_mst, 0x00, sizeof(sPrdt_point_mst) );

		ret = Prdt_point_mst_Fet_Upd( &sPrdt_point_mst, g_pub_tx.reply );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Prdt_point_mst_Fet_Upd出错!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}

		sPrdt_point_mst.yt_point = sPrdt_point_mst.point + sPrdt_point_mst.used_point;
		sPrdt_point_mst.point = 0;
		sPrdt_point_mst.used_point = 0;

		ret = Prdt_point_mst_Upd_Upd( sPrdt_point_mst, g_pub_tx.reply  );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Prdt_point_mst_Upd_Upd出错!!!!![%s]",g_pub_tx.reply);
			WRITEMSG
			goto ErrExit;
		}
	}

OkExit:
	return 0;
ErrExit:
	return 1;
}

