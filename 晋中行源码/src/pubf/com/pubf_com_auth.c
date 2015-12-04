#include "find_debug.h" 
/*************************************************
* 文件名:    pubf_com_note.c
* 功能描述： 凭证处理函数族
*           pub_com_ChkNote      凭证检查处理
*
* 作   者:  jack
* 完成日期： 2004年12月16日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define MYSQLERR if( ret ) { \
			sprintf(acErrMsg,"读数据库错[%d]",ret);\
			WRITEMSG \
			goto ErrExit; \
			}
                 
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "note_tel_auth_c.h"
/**操作机构pbrno柜员ptel
   凭证种类ptype
   行为act 10X 管理类（101入库102下发103收缴） 20X内部调剂类
		   30X 使用类
	mode 检查本方0 对方1
****************/
pub_com_NoteAuthChk( pbrno,ptel,act,mode )
 char *pbrno;
 char *ptel;
 int act;
 int mode;
{
	struct note_tel_auth_c note_tel_auth_c;
	int ret=0;
	int chksut;
	chksut=1;

	if( act==101 )  /**入库**/
	{
		/**总行凭证管理员**/
		vtcp_log("HERE!!! brno=[%s],tel=[%s]",pbrno,ptel);
		ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1') ",
				pbrno,ptel );
		if( ret==0 ) chksut=0;
		else if( ret==100 ) {strcpy( g_pub_tx.reply,"N088" );
				vtcp_log("HERE1!!!");
				}
		else { MYSQLERR }
	}
	else if( act==102 )  /**下发**/ /**收缴**/
	{
		if( mode==0 ) /*本方*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2','3') ",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
TRACE
		}
		else if( mode==10 ) /*本方本行对方*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('3')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N091" );
			else { MYSQLERR }
TRACE
		}
		else /*对方*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('2','3','4')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N091" );
			else { MYSQLERR }
TRACE
		}
	}
	else if( act==103 )   /**领入**/
	{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('2','3','4')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
	}
	else if( act==104 )   /**上缴**/
	{
		if( mode==10 ) /*向本支行上缴凭证*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('3')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
		}
		else if( mode==0 ) /*本方*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('2','4')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N088" );
			else { MYSQLERR }
		}
		else /*对方*/
		{
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
			"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2')",
				pbrno,ptel );
			if( ret==0 ) chksut=0;
			else if( ret==100 ) strcpy( g_pub_tx.reply,"N091" );
			else { MYSQLERR }
		}
	}
	else if( act==301 ) /**调剂**/
	{
		if( mode==0 )
		{
			/*管理员权限**/
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2') ",
				pbrno,ptel );
			vtcp_log("管理员权限审查[%s][%d]返回值[%d]",__FILE__,__LINE__,ret);
			if( ret==0 ) strcpy( g_pub_tx.reply,"N090" );
			else if( ret==100 ) chksut=0;
			else { MYSQLERR }
		}
		else
		{
			/*管理员权限**/
			ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
				"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2') ",
				pbrno,ptel );
			vtcp_log("管理员权限审查[%s][%d]返回值[%d]",__FILE__,__LINE__,ret);
			if( ret==0 ) strcpy( g_pub_tx.reply,"N093" );
			else if( ret==100 ) chksut=0;
			else { MYSQLERR }
		}
	}
	else if( act==401 ) /**普通柜员使用的交易**/
	{
		/*管理员权限**/
		ret=Note_tel_auth_Sel( g_pub_tx.reply , &note_tel_auth_c ,
		"br_no='%s' and tel='%s' and sts='0' and substr(lvl,1,1) in('1','2','3','4') ",
				pbrno,ptel );
		vtcp_log("管理员权限审查[%s][%d]返回值[%d]",__FILE__,__LINE__,ret);
		if( ret==0 ) strcpy( g_pub_tx.reply,"N090" );
		else if( ret==100 ) chksut=0;
		else { MYSQLERR }
	}
	else
	{
		chksut=0;
	}

OkExit:
	return chksut;
ErrExit:
	return -1;
}
