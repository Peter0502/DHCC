/*************************************************
* 文 件 名: spF680.c
* 功能描述：批量代理结果打印
*
* 作    者: jack
* 完成日期: 2004年05月28日
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

spF680()  
{ 	
	
	int ret=0;

	char cUnit_no[5];
	char cFlag[2];
	char cDate[9];
	long lTraceno=0;/**文件名加上了流水 ADD BY ZHGF**/

	char execstr[256];

	char filename[100];
	char filename1[100];
	int i=0;

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cDate, 0x0, sizeof(cDate) );
	
	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0700", cFlag );
	get_zd_data( "0460", cDate );
	get_zd_long( "0290", &lTraceno);/**文件名加上了流水**/


	/** 初始化全局变量 **/
	pub_base_sysinit();

	memset(filename,0x0,sizeof(filename));
	pub_base_AllDwnFilName( filename );

	if ( cFlag[0] != '2' )
	{
		memset( filename1, 0x0, sizeof(filename1) );
		sprintf( filename1, "%s/%s%s%s%s%06d.txt", getenv("FILDIR"), cFlag, cUnit_no, cDate,g_pub_tx.tel,lTraceno);/**文件名加上了流水**/
		vtcp_log("%s,%d filename1==[%s]",__FILE__,__LINE__,filename1);
	}
	else
	{
		memset( filename1, 0x0, sizeof(filename1) );
		sprintf( filename1, "%s/DWHZ%s%s%s.txt", getenv("FILDIR"), cUnit_no, cDate,g_pub_tx.tel);
		vtcp_log("%s,%d filename1==[%s]",__FILE__,__LINE__,filename1);
	}

	sprintf(acErrMsg,"取得的文件名为[%s]生成的文件名为[%s]", filename, filename1 );
	WRITEMSG

	if ( access( filename1, 0) )
	{
		sprintf(acErrMsg,"要访问的文件不存在[%s]", filename1 );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}

	memset( execstr, 0x0, sizeof(execstr) );
	sprintf( execstr, "cp %s %s", filename1, filename );

	ret = system( execstr );
	if ( ret )
	{
		sprintf(acErrMsg,"更改下传文件名失败[%s][%s][%d][%s]", filename, strerror(errno), ret, execstr );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
		
	}

	set_zd_data(DC_FILE_SND,"1");

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"批量代理下传文件成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"批量代理下传文件失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
