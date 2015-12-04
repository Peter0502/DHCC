/*************************************************
* 文 件 名:     pubf_ass_br_def
* 功能描述：    1、包括模块pub_ass_get_br_def根据机构编码取机构参数表；
* 作    者:
* 完成日期：    2003年12月30日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_br_def_c.h"

/**********************************************************************
 * 函 数 名：   pub_ass_get_br_def
 * 函数功能：   根据机构编码取考核机构参数表
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
int pub_ass_get_br_def(char br_no[6],struct ass_br_def_c *ass_br_def_c)
{

 memset(ass_br_def_c,0x00,sizeof(struct ass_br_def_c));
 pub_base_strpack(br_no);

 g_reply_int=Ass_br_def_Sel( g_pub_tx.reply,&ass_br_def_c," br_no='%s' ",br_no);
 if ( g_reply_int==100 )
 {
 	sprintf(acErrMsg,"该机构在考核系统中不存在!br_no=[%s]",br_no);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "K101" );
 	return -1;
 }
 else if ( g_reply_int )
 {
 	sprintf(acErrMsg,"机构编码表异常!br_no=[%s][%d]",br_no,g_reply_int);
 	WRITEMSG
 	strcpy ( g_pub_tx.reply , "K101" );
 	return -1;
 }

 return (0);
}
