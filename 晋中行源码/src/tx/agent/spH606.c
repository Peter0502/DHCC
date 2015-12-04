/*************************************************
* 文 件 名:  spH606.c
* 功能描述： 完成个人编号信息查询
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
#include "ag_peracctinfo_c.h"
#include "ag_peritemmap_c.h"

int spH606()
{
	struct ag_peracctinfo_c sPerAcctInfo;
	struct ag_peritemmap_c sPerItemMap;

	char	cFileName[50];			/* 临时文件名 */
	int	ret = 0;
	FILE 	*fp;

	char cPer_no[13];
	char cStat[7];

	memset( &sPerAcctInfo,0x0,sizeof(sPerAcctInfo));
	memset( &sPerItemMap,0x0,sizeof(sPerItemMap));

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cPer_no ,0x0, sizeof(cPer_no));
	
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

	get_zd_data("0590",cPer_no);

		sprintf( acErrMsg, "个人编号为[%s]!!!!!\n\n",cPer_no);
		WRITEMSG
	
	ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , "per_no = '%s' order by unit_no",cPer_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_peritemmap_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|%s|\n", \
				"单位编号","建立日","状态","撤销日","个人缴费号码");

	while(1)
	{
		memset( &sPerItemMap, 0x00, sizeof(sPerItemMap));
		
		ret = Ag_peritemmap_Fet_Sel( &sPerItemMap, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Ag_peritemmap_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset(cStat,0,sizeof(cStat));
		if (sPerItemMap.stat[0] == '0')
		{
			strcpy(cStat,"正常");
		}
		if (sPerItemMap.stat[0] != '0')
		{
			strcpy(cStat,"非正常");
		}
		
		fprintf(fp,"%s|%s|%s|%s|%s|\n",sPerItemMap.unit_no,sPerItemMap.opn_date,cStat,sPerItemMap.cls_date,sPerItemMap.item_no);

	}
	Ag_peritemmap_Clo_Upd( );

	ret = Ag_peracctinfo_Sel(g_pub_tx.reply, &sPerAcctInfo, "per_no = '%s' and per_stat = '0'", cPer_no);
	if (ret != 0)
	{
		sprintf( acErrMsg, "执行Ag_peracctinfo_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	memset(cStat,0,sizeof(cStat));
	if (sPerAcctInfo.per_stat[0] == '0')
	{
		strcpy(cStat,"正常");
	}
	if (sPerAcctInfo.per_stat[0] != '0')
	{
		strcpy(cStat,"非正常");
	}
	fprintf(fp,"`个人编号 %s 名称 %s 状态 %s\n",sPerAcctInfo.per_no,sPerAcctInfo.per_name,cStat);
	fprintf(fp,"`开户行 %s 银行账户类型 %s 银行账号 %s\n",sPerAcctInfo.opn_br_no,sPerAcctInfo.acct_type,sPerAcctInfo.bank_acct_no);
	fprintf(fp,"`开户日期 %s 开户柜员 %s 销户日期 %s 销户柜员 %s\n",sPerAcctInfo.opn_date, sPerAcctInfo.opn_tel, sPerAcctInfo.cls_date, sPerAcctInfo.cls_tel);
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
