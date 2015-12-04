/*************************************************
* 文 件 名:  sp8612.c
* 功能描述： 存放同业帐户信息
*
* 作    者:  rob
* 完成日期： 2003年12月20日
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

int sp8612()
{
	char cFileName[50];			/* 临时文件名 */
	int	 ret = 0;
	FILE *fp;

	char cAc_sts[16];

	struct in_mst_c	sIn_mst;

	memset( &sIn_mst, 0x00, sizeof(sIn_mst) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	
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

	ret = In_mst_Dec_Sel( g_pub_tx.reply , \
			"prdt_no in('911','917') order by opn_br_no,prdt_no" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行In_mst_Dec_Sel错[%d]", ret );
		WRITEMSG
		fclose(fp);
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|%s|%s|\n", "开户机构", "产品编号",\
	"余额", "昨日余额", "账户状态", "户名" );
	
	while(1)
	{
		memset( &sIn_mst, 0x00, sizeof(sIn_mst) );
		
		ret = In_mst_Fet_Sel( &sIn_mst, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
		sprintf( acErrMsg, "执行In_mst_Fet_Sel错[%d]", ret );
			WRITEMSG
			fclose(fp);
			goto ErrExit;
		}

		memset( cAc_sts, 0x0, sizeof(cAc_sts) );
		pub_base_strpack( sIn_mst.sts );

		if ( sIn_mst.sts[0] == '1' )
		{
			strcpy( cAc_sts, "正常" );
		}
		else if ( sIn_mst.sts[0] == '2' )
		{
			strcpy( cAc_sts, "余额方反向冻结" );
		}
		else if ( sIn_mst.sts[0] == '3' )
		{
			strcpy( cAc_sts, "已科目结转" );
		}
		else if ( sIn_mst.sts[0] == '*' )
		{
			strcpy( cAc_sts, "销户" );
		}

	fprintf(fp,"%s|%s|%.2lf|%.2lf|%s|%s|\n", sIn_mst.opn_br_no,\
	sIn_mst.prdt_no,sIn_mst.bal, sIn_mst.ys_bal, cAc_sts, sIn_mst.name );
	}
	fclose(fp);

	Dd_mst_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"生成批量个人资料文件文件成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	In_mst_Clo_Sel( );
	sprintf(acErrMsg,"生成批量个人资料文件文件失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
