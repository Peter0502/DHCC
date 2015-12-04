/***************************************************************
* 文 件 名: spH627.c
* 功能描述：个人缴费明细查询
*
* 作    者: jack
* 完成日期：2004年03月29日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_itemlist_c.h"

spH627()
{
	int ret = 0;
	int num = 0;
	char cItem_no[21];
	char cUnit_no[5];
	char cPay_month[7];

	char cFileName[128];
	FILE *fp;

	struct ag_itemlist_c sAg_itemlist; /*缴费信息明细表*/

	memset(&sAg_itemlist,0x0,sizeof(sAg_itemlist));
	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cPay_month,0x0,sizeof(cPay_month));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );

	/*---------------- 需要进行先插入操作 ------------------*
	ret = Ag_itemlist_Ins(sAg_itemlist,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"插入缴费明细表出错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	*---------------- 需要进行先插入操作 ------------------*/

	memset( cFileName, 0x00, sizeof( cFileName ) );

	/* 根据柜员号、终端号生成临时文件 */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "生成下传全路经文件名(批量)错" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "临时文件错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	ret = Ag_itemlist_Dec_Sel( g_pub_tx.reply , "item = '%s' and \
			unit_no = '%s' and stcd = '1'", cItem_no, cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_itemlist_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%20s|%10s|%10s|%12s|\n", \
				"缴费号码","欠费年月","缴费年月","缴费金额");

	while(1)
	{
		memset( &sAg_itemlist, 0x00, sizeof(sAg_itemlist));
		
		ret = Ag_itemlist_Fet_Sel( &sAg_itemlist, g_pub_tx.reply );
		if ( ret == 100)
		{
			if (num == 0)
			{
				sprintf( acErrMsg, "缴费明细不存在!!!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "H013");
				goto ErrExit;
			}
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Ag_itemlist_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%s|%d|\n", sAg_itemlist.item_no, sAg_itemlist.owe_month, sAg_itemlist.trn_date, sAg_itemlist.am06);

		num++;
	}
	Ag_itemlist_Clo_Upd( );

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"个人缴费信息查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"个人缴费信息查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
