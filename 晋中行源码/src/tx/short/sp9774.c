/*************************************************
* 文 件 名: sp9774.c
* 功能描述：查询短信客户账号与服务产品的对应关系 
*
* 作    者: jack
* 完成日期：2003年04月23日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mob_acct_type_c.h"

int sp9774()
{
	int ret = 0;
	char cFlag[2];
	char cMobile[12];
	char cAcno[20];

	char cName[51];
	long lCif_no = 0; 

	char cSerind[51];

	struct	mob_acct_type_c	sMob_acct_type;

	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cSerind, 0x0, sizeof(cSerind) );

	/*--- 初始化 ---*/
	pub_base_sysinit();

	/*--- 取值 ---*/
	get_zd_data( "0700", cFlag );
	get_zd_data( "0610", cMobile );
	get_zd_data( "0310", cAcno );

	if ( cFlag[0] == '0' )
	{
		goto OkExit;
	}

	/*--- 流程 ---*/
	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply , "mobile = '%s' and \
			ac_no = '%s'" , cMobile, cAcno );
	if ( ret != 0 )
	{
		sprintf(acErrMsg,"执行Mob_acct_type_Dec_Sel出错[%d]!!!", ret);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));

		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type , g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		if ( ret != 0 )
		{
			sprintf(acErrMsg,"执行Mob_acct_type_Fet_Sel出错[%d]!!!", ret);
			WRITEMSG
			goto ErrExit;
		}

		strcat( cSerind, sMob_acct_type.server_type );
		strcat( cSerind, ";" );

	}

	Mob_acct_type_Clo_Sel();

    ret = pub_base_acgetname( cAcno, &lCif_no, \
			cName);
	if ( ret != 0 )
	{
		sprintf(acErrMsg,"该账号对应的客户号和户名不存在!!![%s][%d]", cAcno, ret );
		WRITEMSG
		goto ErrExit;
	}


	set_zd_data( "0260", cName);
	set_zd_data( "0810", cSerind );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	Mob_acct_type_Clo_Sel();
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
