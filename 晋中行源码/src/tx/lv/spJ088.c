/************************************************************************
* 文 件 名:   spJ088.c
* 功能描述：  撤销回执
*
* 作    者:   dongxy
* 完成日期：  2006-12-14
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
************************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#define loc_t sys_loc_t


spJ088()
{
	   int	  iRet = 0;
       char   cOpflag         [2];
	   char	  cPack_id        [9];
	   char	  cPack_orderno   [9];
	   char	  cPack_wtday     [9];
	   char	  cPack_re_sts    [3];
	   char	  cPack_re_content[61];
	   char	  cPack_err_code  [61];
	   char   cTemp1          [61];
	   char   cTemp2          [61];
     
     memset(cTemp1          , 0 , sizeof(cTemp1));
     memset(cTemp2          , 0 , sizeof(cTemp2));
	   memset(cOpflag         , 0 , sizeof(cOpflag));
	   memset(cPack_id        , 0 , sizeof(cPack_id));
	   memset(cPack_orderno   , 0 , sizeof(cPack_orderno));
	   memset(cPack_wtday     , 0 , sizeof(cPack_wtday));
	   memset(cPack_re_sts    , 0 , sizeof(cPack_re_sts));
	   memset(cPack_re_content, 0 , sizeof(cPack_re_content));
	   memset(cPack_err_code  , 0 , sizeof(cPack_err_code));

     get_fd_data("0962", cTemp1);
     get_fd_data("0963", cTemp2);
     get_fd_data("0460", cPack_wtday);
     get_fd_data("0210", cPack_re_sts);
     get_fd_data("0961", cPack_re_content);
     zip_space(cTemp1);
     zip_space(cTemp2);
     memcpy(cPack_id     ,cTemp1,sizeof(cPack_id)-1);
     memcpy(cPack_orderno,cTemp2,sizeof(cPack_orderno)-1);
    vtcp_log("[%s][%d] ======cPackid    [%s]\n",__FILE__,__LINE__,cPack_id);
    vtcp_log("[%s][%d] ======ODERNO     [%s]\n",__FILE__,__LINE__,cPack_orderno);
    vtcp_log("[%s][%d] ======WTDAY      [%s]\n",__FILE__,__LINE__,cPack_wtday);
    vtcp_log("[%s][%d] ======REPSTAT    [%s]\n",__FILE__,__LINE__,cPack_re_sts);
    vtcp_log("[%s][%d] ======REPCONTENT [%s]\n",__FILE__,__LINE__,cPack_re_content);
    
    iRet=lv_dsf_deal40505_return(cPack_id,cPack_orderno,cPack_wtday,cPack_re_sts,cPack_re_content,cPack_err_code);
    if(iRet)
    {
        vtcp_log("[%s][%d],交易处理失败![%d]\n",__FILE__,__LINE__,iRet);
        cOpflag[0] = '1';
        set_zd_data("0740", cPack_err_code);
        sprintf(acErrMsg,"err=[%s]",cPack_err_code);
        set_zd_data(DC_GET_MSG,acErrMsg);
        WRITEMSG
        goto ErrExit;
    }
    else
    { 
        cOpflag[0] = '0';
    }
    /*beg add by Chenggx 2011-6-16  晋中 登记流水*/
    /* 登记交易流水 */
    if ( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"登记交易流水错误!");
        WRITEMSG
        goto ErrExit;
    }
    /*beg add by Chenggx 2011-6-16  晋中*/
    /** 开始组织返回的变量 **/
    set_zd_data("0690", cOpflag);   /** 成功失败标志 **/

OkExit:
        strcpy( g_pub_tx.reply, "0000" );
        sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;
ErrExit:
	      if(memcmp(g_pub_tx.reply,"0000",4)==0)
	      {
            memcpy(g_pub_tx.reply,"T063",4);
        }
        sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
