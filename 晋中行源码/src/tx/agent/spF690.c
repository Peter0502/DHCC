/*************************************************
* 文 件 名: spF690.c
* 功能描述：财政代发工资单打印
*
* 作    者: jack
* 完成日期: 2004年06月28日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <sys/types.h>
#include <errno.h>
#include "ag_itemlist_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"

spF690()  
{ 	
	
	int ret=0;

	char cUnit_no[5];
	char cItem_no[21];
	long lDate = 0;
	char cDate[7];

	char filename[100];
	FILE *fp;
	int i=0;

	struct ag_itemlist_c sAg_itemlist;
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;

	memset( cItem_no, 0x0, sizeof(cItem_no) );
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cDate, 0x0, sizeof(cDate) );

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );
	get_zd_long( "0460", &lDate );

	sprintf( cDate, "%ld", lDate );

	/** 初始化全局变量 **/
	pub_base_sysinit();

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret )
	{
    	sprintf( acErrMsg, "---------->查询单位信息出错[%d]!!!", ret );
    	WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
	ret = Ag_peritemmap_Sel( g_pub_tx.reply, &sAg_peritemmap, \
			"unit_no = '%s' and item_no = '%s' and stat = '0'", \
			cUnit_no, cItem_no );
	if ( ret )
	{
    	sprintf( acErrMsg, "---------->查询个人信息出错[%d]!!!", ret );
    	WRITEMSG
		goto ErrExit;
	}

	memset( &sAg_itemlist, 0x0, sizeof(sAg_itemlist) );
	ret = Ag_itemlist_Sel( g_pub_tx.reply, &sAg_itemlist, \
			"unit_no = '%s' and item_no = '%s' and owe_month = '%s'", \
			cUnit_no, cItem_no, cDate );
	if ( ret )
	{
    	sprintf( acErrMsg, "--->查询代发工资明细记录出错[%d]!!!", ret );
    	WRITEMSG
		goto ErrExit;
	}

	memset(filename,0x0,sizeof(filename));
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
    	sprintf( acErrMsg, "--->打开写工资条文件出错!!!" );
    	WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "\n\n					工	资	条\n\n" );

	pub_base_strpack( sAg_peritemmap.name );
	pub_base_strpack( sAg_peritemmap.ac_no );
	fprintf( fp, "姓名:%s		日期:%s		账号:%s\n",  \
		sAg_peritemmap.name, sAg_itemlist.trn_date, \
		sAg_peritemmap.ac_no );

    sprintf( acErrMsg, "---------->开始查询工资单打印记录 ! " );
    WRITEMSG


	fprintf( fp, "所在单位编号:%s				所在单位名称:%s\n", \
		cUnit_no, sAg_unitinfo.unit_name );
	fprintf( fp, "┌───────┬───────┬───────┬───────┬───────┬───────┐\n" );
	fprintf( fp, "│      基础工资│%14.2f│      职务工资│%14.2f│      级别工资│%14.2f│\n", \
				sAg_itemlist.am00, sAg_itemlist.am01, sAg_itemlist.am03 );
	fprintf( fp, "├───────┼───────┼───────┼───────┼───────┼───────┤\n" );
	fprintf( fp, "│      工龄工资│%14.2f│未入津补贴奖金│%14.2f│      适当补贴│%14.2f│\n", \
				sAg_itemlist.am04, sAg_itemlist.am08, sAg_itemlist.am10 );
	fprintf( fp, "├───────┼───────┼───────┼───────┼───────┼───────┤\n" );
	fprintf( fp, "│    住房公积金│%14.2f│    医疗费统筹│%14.2f│  养老保险统筹│%14.2f│\n", \
				sAg_itemlist.am18, sAg_itemlist.am19, sAg_itemlist.am20 );
	fprintf( fp, "├───────┼───────┼───────┼───────┼───────┼───────┤\n" );
	fprintf( fp, "│  指令集资捐献│%14.2f│      应发工资│%14.2f│      实发工资│%14.2f│\n", \
				sAg_itemlist.am21, sAg_itemlist.am17, sAg_itemlist.am24 );
	fprintf( fp, "└───────┴───────┴───────┴───────┴───────┴───────┘\n" );

	fclose( fp );
	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"生成财政工资条下传文件成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"生成财政工资条下传文件失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
