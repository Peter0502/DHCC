/*************************************************
* 文 件 名:  sp8403.c
* 功能描述： 网点提示信息查询
*
* 作    者:  lance
* 完成日期： 20030617
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"

int sp8403()
{
	char brno[6];
	long inf_date;
	
	char mailname[51];
	char filename[100];
	char tmpstr[512];

	int i=0;
	int ret=0;
	FILE *fp;

	pub_base_sysinit();

	get_zd_data("0910",brno );
	sprintf(acErrMsg,"机构名字[%s]",brno);
	WRITEMSG	
	get_zd_long("0290",&inf_date );

	/* 组合文件名 */
	if(!inf_date) inf_date=g_pub_tx.tx_date;
	if( !strlen(brno) ) strcpy( brno,g_pub_tx.tx_br_no );
		
	sprintf(mailname, "%s/%ld/%s%ld" ,getenv("MAILDIR"), inf_date , brno , inf_date ); 
	sprintf(acErrMsg,"文件名字[%s]",mailname);
	WRITEMSG
					      	
	fp = fopen( mailname,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",mailname );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S055" );
		goto ErrExit;
	}else{
		fclose(fp);
		pub_base_AllDwnFilName( filename );
		vtcp_log(filename);		
		
		sprintf(tmpstr,"cp %s %s 1>/dev/null 2>/dev/null",mailname, filename);
		system(tmpstr);
		vtcp_log(tmpstr);

		set_zd_data( DC_FILE_SND,"1" );
	}
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
