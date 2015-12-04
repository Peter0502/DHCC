/*************************************************
* 文 件 名:  spH620.c
* 功能描述： 完成单位信息综合查询
*
* 作    者:  jack
* 完成日期： 2004年1月31日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"
#include "ag_unitacctmap_c.h"

int spH620()
{
	struct ag_unitinfo_c sUnitInfo;
	struct ag_unitacctmap_c sUnitAcctMap;

	char	cFileName[50];			/* 临时文件名 */
	int	ret, num=0;
	FILE 	*fp;

	char cUnit_no[5];
	char cStat[7];

	memset( &sUnitInfo,0x0,sizeof(sUnitInfo));
	memset( &sUnitAcctMap,0x0,sizeof(sUnitAcctMap));

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cUnit_no ,0x0, sizeof(cUnit_no));
	
	/* 数据初始化 */
	pub_base_sysinit();

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

	get_zd_data("0920",cUnit_no);

	ret = Ag_unitacctmap_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' order by br_no",cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitacctmap_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|\n", \
				"机构号","机构代收临时账号","类型","状态");

	while(1)
	{
		memset( &sUnitAcctMap, 0x00, sizeof(sUnitAcctMap));
		
		ret = Ag_unitacctmap_Fet_Sel( &sUnitAcctMap, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Ag_unitacctmap_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset(cStat,0,sizeof(cStat));
		if (sUnitAcctMap.stat[0] == '0')
		{
			strcpy(cStat,"正常");
		}
		if (sUnitAcctMap.stat[0] != '0')
		{
			strcpy(cStat,"非正常");
		}
		
		fprintf(fp,"%s|%s|%s|%s|\n",sUnitAcctMap.br_no,sUnitAcctMap.bank_acct_no,sUnitAcctMap.acct_type,cStat);

		num++;	
	}
	Ag_unitacctmap_Clo_Sel( );

	ret = Ag_unitinfo_Sel(g_pub_tx.reply, &sUnitInfo, "unit_no = '%s' and unit_stat = '0'", cUnit_no);
	if (ret != 0)
	{
		sprintf( acErrMsg, "执行Ag_unitinfo_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	fprintf(fp,"--------------------------------------------------------\n");
	fprintf(fp,"` 单位编号 %s  单位名称 %s\n", sUnitInfo.unit_no,sUnitInfo.unit_name);
	fprintf(fp,"` 代收账号类型 %s  代收账号 %s\n", sUnitInfo.acct_type,sUnitInfo.bank_acct_no);
	fprintf(fp,"--------------------------------------------------------\n");

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
