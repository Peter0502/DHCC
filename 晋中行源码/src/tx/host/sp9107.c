/*************************************************
* 文 件 名: sp9107.c
* 功能描述：文件清理
*
* 作    者: rob
* 完成日期：20030322
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"

int sp9107( )
{
    int ret;
    char 	filename[256];
	char	fileType[2];			/* 删除标志         */
									/* 0 -- 日终备份    */
									/* 1 -- 报表数据    */
	char 	fileName[64];			/* 文件名 			*/
	char	pathName[256];			/* 目录名           */
	char	fullName[512];			/* 带路径的文件名 	*/
	char	l_cmd[256];				/* 命令             */
	char	l_str_date[9];			/* 字符串日期       */
	long	l_start_date;			/* 删除起始日期 	*/
	long	l_end_date;				/* 删除终止日期 	*/
	long	l_date;					
    long	l_sys_date;				/* 系统日期 		*/

    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    memset( fileType, 0x00, sizeof(fileType) );
	memset( fileName, 0x00, sizeof(fileName) );
	memset( pathName, 0x00, sizeof(pathName) );
	memset( fullName, 0x00, sizeof(fullName) );

    get_zd_data("0670",fileType);
    get_zd_long("0440",&l_start_date);
    get_zd_long("0450",&l_end_date);

    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询公共参数表错误!![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
   
    l_sys_date = com_sys_parm_c.sys_date;

    if( l_end_date > l_sys_date )
    {
        sprintf(acErrMsg,"终止日期不能大于系统日期");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O147");
        goto ErrExit;
    }

    if( l_start_date > l_end_date )
    {
        sprintf(acErrMsg,"开始日期大于终止日期");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O148");
        goto ErrExit;
    }

    if( l_end_date == 0 )
    {
        sprintf(acErrMsg,"终止日期必须填写");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O149");
        goto ErrExit;
    }

    if( fileType[0] == '0' )		/* 数据库备份文件 */
	{
		sprintf(pathName,"%s",getenv("BFLDIR"));	/* 目录名 */
	}else							/* 报表文件 */
	{
		sprintf(pathName,"%s",getenv("RPTDIR"));
	}

	l_date = l_end_date;

    for(;l_date >= l_start_date;)
	{
		if( fileType[0] == '0' )
		{
			memset( l_str_date , 0x00, sizeof( l_str_date));
			memset( fullName , 0x00, sizeof( fullName));
			memset( l_cmd , 0x00, sizeof( l_cmd));

			/* 删除目录 */
			sprintf(l_str_date, "%ld", l_date);
			sprintf(fullName,"%s/%s", pathName, l_str_date);
			sprintf(l_cmd, "rm -fr %s", fullName);
			sprintf(acErrMsg,"l_cmd = [%s]",l_cmd);
            WRITEMSG
			system(l_cmd);

			memset(fileName, 0x00, sizeof(fileName));
			memset(fullName, 0x00, sizeof(fullName));
			memset(l_cmd, 0x00, sizeof(l_cmd));

			/* 删除文件: RZQYYYYMMDD */
			sprintf(fileName, "RZQ%s", l_str_date);
			sprintf(fullName, "%s/%s", pathName, fileName);
			sprintf(l_cmd, "rm -fr %s", fullName);
			sprintf(acErrMsg,"l_cmd = [%s]",l_cmd);
            WRITEMSG
			system(l_cmd);
		}
        else
		{
			memset( fileName, 0x00, sizeof( fileName ));
			memset( fullName, 0x00, sizeof( fullName ));
			memset(l_cmd, 0x00, sizeof( l_cmd ));
			memset(l_str_date, 0x00, sizeof( l_str_date ));

			sprintf(l_str_date, "%ld", l_date);
			sprintf(fileName, "rpt.*.%s.Z",l_str_date);
			sprintf(fullName, "%s/%s", pathName, fileName);
			sprintf(l_cmd, "rm -fr %s", fullName);
			system(l_cmd);
		}
		
		ret = pub_base_deadlineD(l_date,-1,&l_date);
        if (ret != 0)
        {
            sprintf(acErrMsg,"取前一天日期错误!!");
            WRITEMSG
            strcpy(g_pub_tx.reply,"O150");
            goto ErrExit;
        }
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"文件清理成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"文件清理失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
