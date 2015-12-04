/*************************************************
* 文 件 名:  spF651.c
* 功能描述： 查询短信银行未发送信息(渠道发起)
*
* 作    者:  jack
* 完成日期： 2004年03月25日
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
#include "mob_sendmail_c.h"

int spF651()
{
	char cFileName[50];			/* 临时文件名 */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct mob_sendmail_c sMob_sendmail;

	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );

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

	ret = Mob_sendmail_Dec_Upd( g_pub_tx.reply , "proc_flag = '0'" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mob_sendmail_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
		
		ret = Mob_sendmail_Fet_Upd( &sMob_sendmail, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mob_sendmail_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%s|%s|%ld|%ld|%s|%s|\n", sMob_sendmail.opt_no, sMob_sendmail.mobile, sMob_sendmail.message, "0", sMob_sendmail.tx_date, sMob_sendmail.tx_time, "0", "0");

		++num;

		strcpy( sMob_sendmail.proc_flag, "1" );

		ret = Mob_sendmail_Upd_Upd( sMob_sendmail, g_pub_tx.reply  );
		if( ret )
		{
			sprintf( acErrMsg, "执行Mob_sendmail_Upd_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
	}
	fclose(fp);

	Mob_sendmail_Clo_Upd( );

	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

	if (num == 0)
	{
		sprintf(acErrMsg,"未查询到需要发送的短信记录!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "0001" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"生成短信未处理文件成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Mob_sendmail_Clo_Upd( );
	sprintf(acErrMsg,"生成短信未处理文件失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
