/*************************************************
* �� �� ��: spD432.c
* ���������������ִ��Э����Ϣά��
* ����    ��wanghongjie
* ʱ��    ��20110426
* insert into tx_def values       ��1057 ��������Ϣά��   10000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000100  1  1  2  3  ��
* insert into tx_flow_def values  ��1057  0  D432  #$  ��
* insert into tx_sub_def values   ��D432   ��������Ϣά��   spD432  0  0  ��
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "mo_prdt_prot_c.h"
#include "mdm_ac_rel_c.h"

static struct mo_prdt_prot_c    g_mo_prdt_prot;
static struct mdm_ac_rel_c      g_mdm_ac_rel;

spD432()
{
    int ret=0;
    char acno[20];
    double amt=0.00;

    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    
    /** ���ݳ�ʼ�� **/
    memset(&g_mo_prdt_prot, 0x00,sizeof(struct mo_prdt_prot_c));
    memset(acno,0x00,sizeof(acno));
    memset(&g_mdm_ac_rel, 0x00,sizeof(struct mdm_ac_rel_c));
    
    /** ȡֵ����ֵ **/
    get_zd_data("0300",acno);
    get_zd_double("0410",&amt);

    /***�޸�****/
    /* �������ʺŲ�ѯ���˻���ź��˻�ID */
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no='%s'", acno);
    if (ret == 100)   
    {
        sprintf(acErrMsg, "�����ڸ��ʺ�!!ac_no=[%s]", acno);
        WRITEMSG
        strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    } 
    else if (ret)    
    {
        sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel����!ret=[%s]", ret);
        WRITEMSG
        goto ErrExit;
    }
    /*���»���Э��ǼǱ�������*/
    ret=Mo_prdt_prot_Dec_Upd(g_pub_tx.reply,"main_ac_id=%ld ",g_mdm_ac_rel.ac_id);
    if (ret)
    {
        sprintf(acErrMsg,"DECLARE Mo_prdt_prot�α��쳣![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }
    
    ret=Mo_prdt_prot_Fet_Upd(&g_mo_prdt_prot,g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg,"FETCH Mo_prdt_prot�α��쳣![%s]", g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }
    g_mo_prdt_prot.leave_amt=amt;
  
    ret=Mo_prdt_prot_Upd_Upd(g_mo_prdt_prot,g_pub_tx.reply);
    if (ret)
    {
        sprintf(acErrMsg,"UPDATE Mo_prdt_prot�α��쳣![%s]",g_pub_tx.reply);
        WRITEMSG
        strcpy(g_pub_tx.reply,"C006");
        ERR_DEAL;
    }
    Mo_prdt_prot_Clo_Upd( );
    
    /* �Ǽǽ�����ˮ */
    if( pub_ins_trace_log() )
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
        WRITEMSG
        goto ErrExit;
    }
    sprintf(acErrMsg,"�Ǽǽ�����ˮ PASS!");
    WRITEMSG
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"�����ִ��Э����Ϣά���ɹ���[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    sprintf(acErrMsg,"�����ִ��Э����Ϣά��ʧ�ܣ�[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
