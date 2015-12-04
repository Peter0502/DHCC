/*****************************************************
* 文 件 名:  spJ403.c
* 功能描述： 查询大小额专用内部帐户
* 作    者:  xyy
* 完成日期： 2006-10-24 15:23
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lv_fd123.h"
#include "com_parm_c.h"
int spJ403()
{
  int ret=0,i;
  char cCode[6];
  char cKinbr[BRNO_LEN+1];
  struct com_parm_c  com_parm;
  memset(cCode ,0,sizeof(cCode));
  memset(cKinbr ,0,sizeof(cKinbr));
  memset(&com_parm,0x00,sizeof(struct com_parm_c));
  vtcp_log("spJ403 开始！[%s][%d]",__FILE__,__LINE__);
 	
   /*** 初始化全局变量 ***/
   pub_base_sysinit();
	
   get_fd_data("0020",cCode);
   get_fd_data("0030",cKinbr);
   if(memcmp(cKinbr,QS_BR_NO,BRNO_LEN)!=0)
   {
 	  if(memcmp(cKinbr,cCode,BRNO_LEN)!=0)
  	  {
            sprintf( acErrMsg,"不能代他机构进行处理,[%s]", __FILE__,__LINE__,cKinbr);
            strcpy( g_pub_tx.reply, "S043" );
            WRITEMSG
            goto ErrExit;
      }
   }
   g_reply_int = Com_parm_Dec_Sel( g_pub_tx.reply , " parm_code='%s' order by parm_seqn" , cCode );
   vtcp_log("spJ403 开始！[%s][%d]",__FILE__,__LINE__);
   if ( g_reply_int==100 )
   {
	  sprintf(acErrMsg,"没有该机构的大小额专用内部帐户");
	  WRITEMSG
	  strcpy( g_pub_tx.reply, "O043" );
	  goto	ErrExit;
   }
   else if ( g_reply_int )
   {
		sprintf(acErrMsg,"查询该机构的大小额专用内部帐户错误[%d]",ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
   }	
   while(1)
   {
	  g_reply_int = Com_parm_Fet_Sel( &com_parm , g_pub_tx.reply );
	  if ( g_reply_int==100 )
	  {
	    	sprintf(acErrMsg,"没有该机构的大小额专用内部帐户");
	    	WRITEMSG
	    	strcpy( g_pub_tx.reply, "O043" );
            break;
	  }
	  else if ( g_reply_int )
	  {
	    	sprintf(acErrMsg,"查询该机构的大小额专用内部帐户错误[%d]",ret);
	    	strcpy( g_pub_tx.reply, "HV01" );
	    	WRITEMSG
	    	goto ErrExit;
	  }	
      vtcp_log("[%s][%d]查询成功!parm_seqn=[%d]",__FILE__,__LINE__,com_parm.parm_seqn);
      vtcp_log("[%s][%d]val====[%s]",__FILE__,__LINE__,com_parm.val);
      set_zd_data("0330",com_parm.val);/*帐号*/
      set_zd_long("0200",com_parm.parm_seqn);/*序号*/
      set_zd_data("0270",com_parm.parm_name);/*户名*/
      vtcp_log("[%s][%d]返回内部帐号的帐号为[%s]",__FILE__,__LINE__,com_parm.val);
      vtcp_log("[%s][%d]返回内部帐号的户名为[%s]",__FILE__,__LINE__,com_parm.parm_name);
   }
   Com_parm_Clo_Sel();

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
