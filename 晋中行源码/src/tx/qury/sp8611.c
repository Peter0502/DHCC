/*************************************************
* 文 件 名:  sp8611.c
* 功能描述： 同业存放账户信息
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

int sp8611()
{
	char cFileName[50];			/* 临时文件名 */
	int	 ret = 0;
	FILE *fp;

	char cAc_sts[11];

	struct dd_mst_c	sDd_mst;

	memset( &sDd_mst, 0x00, sizeof(sDd_mst) );

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

	ret = Dd_mst_Dec_Sel( g_pub_tx.reply , \
			"prdt_no in('135','136','137') order by prdt_no" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Dec_Sel错[%d]", ret );
		WRITEMSG
		fclose(fp);
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|%s|%s|\n", "开户机构", "产品编号", "余额", "昨日\余额", "账户状态", "户名" );
	
	while(1)
	{
		memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
		
		ret = Dd_mst_Fet_Sel( &sDd_mst, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Dd_mst_Fet_Sel错[%d]", ret );
			WRITEMSG
			fclose(fp);
			goto ErrExit;
		}

		memset( cAc_sts, 0x0, sizeof(cAc_sts) );
		pub_base_strpack( sDd_mst.ac_sts );

		if ( sDd_mst.ac_sts[0] == '0' )
		{
			strcpy( cAc_sts, "开户待确认" );
		}
		else if ( sDd_mst.ac_sts[0] == '1' )
		{
			strcpy( cAc_sts, "正常" );
		}
		else if ( sDd_mst.ac_sts[0] == '3' )
		{
			strcpy( cAc_sts, "挂失结清" );
		}
		else if ( sDd_mst.ac_sts[0] == '4' )
		{
			strcpy( cAc_sts, "开户更正" );
		}
		else if ( sDd_mst.ac_sts[0] == '5' )
		{
			strcpy( cAc_sts, "临时销户" );
		}
		else if ( sDd_mst.ac_sts[0] == '*' )
		{
			strcpy( cAc_sts, "销户" );
		}
		else if ( sDd_mst.ac_sts[0] == '#' )
		{
			strcpy( cAc_sts, "计息结束" );
		}

		fprintf(fp,"%s|%s|%.2lf|%.2lf|%s|%s|\n", sDd_mst.opn_br_no, sDd_mst.prdt_no, sDd_mst.bal, sDd_mst.ys_bal, cAc_sts, sDd_mst.name );
	}
	fclose(fp);

	Dd_mst_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"同业存放账户信息成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	Dd_mst_Clo_Sel( );
	sprintf(acErrMsg,"同业存放账户信息失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
