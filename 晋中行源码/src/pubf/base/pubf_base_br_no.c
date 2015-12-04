#include "find_debug.h" 
/*************************************************
* 文 件 名:     pub_base_br_no
* 功能描述：    1、包括模块pub_base_get_br_parm根据机构编码取机构参数表；
* 作    者:
* 完成日期：    2003年12月30日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "com_branch_c.h"

/**********************************************************************
 * 函 数 名：   pub_base_get_br_parm
 * 函数功能：   根据机构编码去机构参数表
 * 作者/时间：
 *
 * 参数：
 *     输入: 机构编码
 *     输出: 机构参数
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
********************************************************************/
int pub_base_get_br_parm(char br_no[6],struct com_branch_c *com_branch_c)
{

 MEMSET_DEBUG(com_branch_c,0x00,sizeof(struct com_branch_c));
 pub_base_strpack(br_no);

 g_reply_int=Com_branch_Sel( com_branch_c," br_no='%s' ",br_no);
 if ( g_reply_int==100 )
 {
 	sprintf(acErrMsg,"该机构在业务系统中不存在!br_no=[%s]",br_no);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "S019" );
 	return -1;
 }
 else if ( g_reply_int )
 {
 	sprintf(acErrMsg,"机构编码表异常!br_no=[%s][%d]",br_no,g_reply_int);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "S019" );
 	return -1;
 }

 return (0);
}

/**********************************************************************
 * 函  数 名：  pub_base_ChkBrnoLvl
 * 函数 功能:   检查两个机构之间的级别关系
 *             
 * 作者/时间：  Terry.Cui/2005年1月7日
 * 
 * 参  数：
 *     输入：  
 *		char	*dw_br_no	下级机构
 *		char	*up_br_no	上级机构
 *
 *     输出：  
 *            	char    *g_pub_tx.reply	相应码
 *
 *   返回值：   0: 是上级机构
 *	       -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_base_ChkBrnoLvl( char *dw_br_no, char *up_br_no )
{
	struct  com_branch_c	sComBranch;	/* 公共机构 */
	
	int	iret;
	
	MEMSET_DEBUG( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	
	if ( !strcmp( dw_br_no, up_br_no ) )
	{
		sprintf( acErrMsg, "机构号相同" );	
		WRITEMSG
		return(-1);		
	}
	
	iret = Com_branch_Sel( g_pub_tx.reply , &sComBranch, "br_no = '%s'", \
				dw_br_no );
	if ( iret == 100 )
	{
		sprintf( acErrMsg, "下发机构不存在" );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N048" );
		return(-1);		
	}else if ( iret )
	{
		sprintf( acErrMsg, "查找机构表错,iret=[%d]", iret );	
		WRITEMSG
		return(-1);
	}

	if ( strcmp( sComBranch.up_br_no, up_br_no ) )
	{
		sprintf( acErrMsg, "两个机构之间不是上下级关系" );	
		WRITEMSG
		strcpy( g_pub_tx.reply, "N047" );
		return(-1);	
	}	
	return(0);
}

/**********************************************************************
 * 函  数 名：  chk_br_no
 * 函数 功能:   检查机构级别
 *             
 * 作者/时间：  Terry.Cui/2005年1月6日
 * 
 * 参  数：
 *     输入：  
 *		char	*br_no		机构号码
 *     输出：  
 *            	char    *g_pub_tx.reply	相应码
 *
 *   返回值：  0: 总行   1: 管理支行  2: 网点  -1: 失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_base_ChkBrno( char *br_no )
{
	struct  com_branch_c	sComBranch;	/* 公共机构 */

	int	iType;				/* 返回机构类型 */	
	int	ret;
	
	MEMSET_DEBUG( &sComBranch, 0x00, sizeof( struct com_branch_c ) );
	
	ret = Com_branch_Sel(g_pub_tx.reply,&sComBranch,"br_no ='%s'",br_no);
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "无此机构号码,br_no=[%s]", br_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S019" );
		return(-1);
	}else if ( ret )
	{
		sprintf( acErrMsg, "查找公共机构表错,ret=[%d]", ret );	
		WRITEMSG
		return(-1);
	}
	
	return( sComBranch.br_type[0] );
}

