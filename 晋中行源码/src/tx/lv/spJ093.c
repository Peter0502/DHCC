/************************************************************************
* 文 件 名:   spJ093.c
* 功能描述： 代付回执 
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

spJ093()
{
        int     iRet;
        char    cOpflag[2];
        char    cErrcode[61];
        char  pack_id[9];
        char  pack_order_no[9];
        char  pack_wt_day[9];
        char  pack_re_stat[3];
        char  pack_re_content[61];
        char  pack_err_code[61];
        char tmp[9];
        
        memset(tmp            ,0x00, sizeof(tmp));
        memset(cOpflag        ,0x00, sizeof(cOpflag));
        memset(cErrcode       ,0x00, sizeof(cErrcode));
        memset(pack_id        ,0x00, sizeof(pack_id));
        memset(pack_order_no  ,0x00, sizeof(pack_order_no));
        memset(pack_wt_day    ,0x00, sizeof(pack_wt_day));
        memset(pack_re_stat   ,0x00, sizeof(pack_re_stat));
        memset(pack_re_content,0x00, sizeof(pack_re_content));
        memset(pack_err_code  ,0x00, sizeof(pack_err_code));

        get_zd_data("0440", pack_id);
        get_zd_data("0450", pack_order_no);
        get_zd_data("0460", pack_wt_day);
        get_zd_data("0230", pack_re_stat);
        get_zd_data("0961", pack_re_content);

        pub_base_strpack_all(pack_id);
        pub_base_strpack_all(pack_order_no);
        
        memset(tmp,'0',sizeof(tmp));
        memcpy(tmp+8-strlen(pack_id),pack_id,strlen(pack_id));
        memset(pack_id,0x00,sizeof(pack_id));
        memcpy(pack_id,tmp,sizeof(pack_id)-1);
        
        memset(tmp,'0',sizeof(tmp));
        memcpy(tmp+8-strlen(pack_order_no),pack_order_no,strlen(pack_order_no));
        memset(pack_order_no,   0x00, sizeof(pack_order_no));
        memcpy(pack_order_no,tmp,sizeof(pack_order_no)-1);

        vtcp_log("[%s][%d] ======ID[%s]\n",__FILE__,__LINE__,pack_id);
        vtcp_log("[%s][%d] ======ODERNO[%s]\n",__FILE__,__LINE__,pack_order_no);
        vtcp_log("[%s][%d] ======WTDAY[%s]\n",__FILE__,__LINE__,pack_wt_day);
        vtcp_log("[%s][%d] ======REPSTAT[%s]\n",__FILE__,__LINE__,pack_re_stat);
        vtcp_log("[%s][%d] ======REPCONTENT[%s]\n",__FILE__,__LINE__,pack_re_content);

        iRet=lv_dsf_deal40503(pack_id,pack_order_no,pack_wt_day,pack_re_stat,pack_re_content,pack_err_code);
        if(iRet)
        {
             vtcp_log("[%s][%d],交易处理失败![%d]\n",__FILE__,__LINE__,iRet);
             cOpflag[0] = '1';
             set_zd_data("0740", pack_err_code);
             sprintf(acErrMsg,"err=[%s]",pack_err_code);
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
