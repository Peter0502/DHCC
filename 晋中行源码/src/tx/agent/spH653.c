/*************************************************
* 文 件 名:  spH653.c
* 功能描述： 客户短信服务品种查询
*
* 作    者:  mike
* 完成日期： 2004年04月24日
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
#include "mob_acct_type_c.h"

int spH653()
{
	char cMobile[14];
	char cSfbz[7];

	char cFileName[50];			/* 临时文件名 */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct mob_acct_type_c sMob_acct_type;

	memset( &sMob_acct_type, 0x00, sizeof(sMob_acct_type) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0610", cMobile );

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

	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "mobile = '%s' \
				order by ac_no,server_type", cMobile );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mob_acct_type_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "~%15s|%20s|%8s|%8s|%8s|%8s|\n", "手机号码", \
			"账   号", "服务种类", "交易日期", "操作柜员", "收费标志" );
	
	while(1)
	{
		memset( &sMob_acct_type, 0x00, sizeof(sMob_acct_type) );
		
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mob_acct_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( cSfbz, 0x0, sizeof(cSfbz) );
		if ( sMob_acct_type.sfbz[0] == '0' )
		{
			strcpy( cSfbz, "不收费" );
		}
		if ( sMob_acct_type.sfbz[0] == '1' )
		{
			strcpy( cSfbz, "收费" );
		}

		fprintf(fp,"%s|%s|%s|%ld|%s|%s|\n", sMob_acct_type.mobile, sMob_acct_type.ac_no, sMob_acct_type.server_type, sMob_acct_type.tx_date, sMob_acct_type.tel, cSfbz );

		++num;

	}
	fclose(fp);

	Mob_acct_type_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

	if (num == 0)
	{
		sprintf(acErrMsg,"未查询到该手机对应的服务品种记录!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "0001" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"生成手机对应的服务品种文件成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Mob_acct_type_Clo_Sel( );
	sprintf(acErrMsg,"生成手机对应的服务品种文件失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
