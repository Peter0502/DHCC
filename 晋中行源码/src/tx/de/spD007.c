
/***************************************************************
* �� �� ��:     spD007.c
* ����������    �ж��Ƿ���Ŀ�ʺ�
* ��    ��:     jack
* ������ڣ�    2004��12��05��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
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
        vtcp_log("%d@%sȡ��0730��ֵΪ[%s](%d)", __LINE__, __FILE__, flg, strlen(flg));
        if(NULL == flg || 0 == strlen(flg)){
        vtcp_log("%d@%s0730��ֵ��Ϊ[%s]", __LINE__, __FILE__, "1001");
                set_zd_data("0730", "1001");
        }

        pub_base_old_acno( g_pub_tx.ac_no );

        goto OkExit;
        ret = sql_count("sub_dd_mst", "ac_no='%s' and ac_sts='1'", g_pub_tx.ac_no);
        if ( ret < 0 )
        {
                sprintf(acErrMsg, "ͳ�����ʺŴ���!! [%d]", ret);
                WRITEMSG
                goto ErrExit;
        }
        else if ( ret > 0 )
        {
                sprintf(acErrMsg, "���ʺ������ʺţ�����2216����");
                WRITEMSG
                strcpy(g_pub_tx.reply, "O265");
                goto ErrExit;
        }

OkExit:
    sprintf(acErrMsg,"�ж���ת��־�ɹ�![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
        sprintf(acErrMsg,"�ж���ת��־ʧ��![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 1;
}
