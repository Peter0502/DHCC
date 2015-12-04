
/***************************************************************
* 文 件 名:     spD007.c
* 功能描述：    判断是否是目帐号
* 作    者:     jack
* 完成日期：    2004年12月05日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD007()
{
        int ret=0;
        char    flg[11];
        
        pub_base_sysinit();
        get_zd_data("1021",g_pub_tx.ac_no);

        memset(flg, 0x0, sizeof(flg));
        get_zd_data("0730", flg);
        vtcp_log("%d@%s取行0730域值为[%s](%d)", __LINE__, __FILE__, flg, strlen(flg));
        if(NULL == flg || 0 == strlen(flg)){
        vtcp_log("%d@%s0730域值赋为[%s]", __LINE__, __FILE__, "1001");
                set_zd_data("0730", "1001");
        }

        pub_base_old_acno( g_pub_tx.ac_no );

        goto OkExit;
        ret = sql_count("sub_dd_mst", "ac_no='%s' and ac_sts='1'", g_pub_tx.ac_no);
        if ( ret < 0 )
        {
                sprintf(acErrMsg, "统计子帐号错误!! [%d]", ret);
                WRITEMSG
                goto ErrExit;
        }
        else if ( ret > 0 )
        {
                sprintf(acErrMsg, "该帐号有子帐号，请做2216交易");
                WRITEMSG
                strcpy(g_pub_tx.reply, "O265");
                goto ErrExit;
        }

OkExit:
    sprintf(acErrMsg,"判断现转标志成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
        sprintf(acErrMsg,"判断现转标志失败![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
