/*************************************************************
* �� �� ��: spH004.c
* ��������: �˹�ȷ�����ļ���ɹ����ȴ����տۿ�
*
* ��    ��: syc
* �������: 2015/10/26 10:14:05
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
**************************************************************/
#define EXTERN
#include "public.h"
#include "de_public.h"
#include "fn_base_inf_c.h"
#include "fn_appbuy_reg_c.h"
#include "mdm_ac_rel_c.h"
/*�ⲿ��������*/
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_ins_trace_log();
int spH004()
{
    int ret = 0;
    char cClos_time[7];
    char cPrdt_no[21];   /**��Ʒ���**/
    struct fn_base_inf_c sFn_base_inf;

    memset(cClos_time ,0x00 ,sizeof(cClos_time));
    memset(cPrdt_no ,0x00 ,sizeof(cPrdt_no));
    memset(&sFn_base_inf,0x00 ,sizeof(struct fn_base_inf_c));
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();

    /**�õ����׻���,��Ʒ����**/

    get_zd_data("0230", cPrdt_no);
    vtcp_trace("prdt_code=[%s]", cPrdt_no);

    ret = Fn_base_inf_Dec_Upd(g_pub_env.reply, " prdt_no ='%s' ", cPrdt_no);
    if(ret)
    {
        vtcp_exit("����fn_base_inf�α�ʧ��! ret=[%d]", ret);
        goto ErrExit;
    }
    ret = Fn_base_inf_Fet_Upd(&sFn_base_inf, g_pub_env.reply);
    if(ret == DB_NOTFOUND)
    {
        vtcp_exit( "û�������Ʒ ret=[%d]", ret);
        goto ErrExit;
    }
    else if(ret)
    {
        vtcp_exit( "��fn_base_inf�α�ʧ��! ret=[%d]", ret);
        goto ErrExit;
    }
    /*�ж������Ʒ�Ƿ��ֹ������*/
    if(sFn_base_inf.auto_flag[0] != '2')
    {
        vtcp_exit( "����Ʋ�ƷΪ�Զ����������˹�����!");
        strcpy(g_pub_env.reply, "D334");
        goto ErrExit;
    }
    /*ֻ��ļ��״̬����������*/
    if(sFn_base_inf.prdt_sts[0] != '0')
    {
        vtcp_exit( "��Ʋ�Ʒ״̬ ������������! prdt_sts=[%s]", sFn_base_inf.prdt_sts);
        strcpy(g_pub_env.reply, "D334");
        goto ErrExit;
    }
    /*ļ�������һ������Ӫҵʱ�����֮��������ɹ������ֹ�����ڴ���ʱ�Ϲ�*/
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(g_pub_env.tx_time < atol(cClos_time))
    {
        vtcp_exit( "ļ���ڵĲ�Ʒ�����ڲ�ƷӪҵʱ���������ʧ�ܴ���=[%s]",cClos_time);
        set_zd_data(DC_GET_UMSG,"ļ��״̬�Ĳ�Ʒ�����ڲ�ƷӪҵʱ���������ʧ�ܴ���");
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }

    strcpy(sFn_base_inf.prdt_sts,"5");/*������Ʒ����*/
    ret = Fn_base_inf_Upd_Upd(sFn_base_inf,g_pub_env.reply);
    if (ret)
    {
        vtcp_exit("������Ʋ�Ʒ������ʧ��!");
        strcpy(g_pub_env.reply, "D325");
        goto ErrExit;
    }
    Fn_base_inf_Clo_Upd();
    strcpy(g_pub_tx.ac_no,cPrdt_no);
    strcpy(g_pub_tx.brf,"��Ʋ�Ʒ��������");
    strcpy(g_pub_tx.acc_ind,"0");
    if( pub_ins_trace_log() )
    {
        vtcp_exit("�Ǽǽ�����ˮ����");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    strcpy(g_pub_env.reply, "0000");
    vtcp_exit( "����ɹ�![%s]", g_pub_env.reply);
    set_zd_data("0120", g_pub_env.reply);
    return 0;

ErrExit:
    if(strlen(g_pub_env.reply) == 0 || strcmp(g_pub_env.reply,"0000") == 0)
    {
        strcpy(g_pub_env.reply,"S035");
    }
    vtcp_exit("����ʧ��![%s]", g_pub_env.reply);
    set_zd_data("0120", g_pub_env.reply);
    return 1;
}


