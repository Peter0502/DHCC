/*******************************************************************************
*** �� �� ��:  V1.0
*** ��������: shangyongchao
*** ��    ��: 20151011
*** ����ģ��: ��Ʋ�Ʒ
*** ��������: spH002
*** ��������: �����Ʋ�Ʒ�ͻ�����ĳ������ý����ɿͻ�����
*** ʹ��ע��:
*** �޸ļ�¼:
***     �޸�����:
***     �޸�ʱ��:
***     �޸�����:
*******************************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "de_public.h"
#include "macro.h"
#include "fn_appbuy_reg_c.h"
#include "fn_appbuy_reg_fun.h"
/*�ⲿ��������*/
extern int pub_date_CalTrueDays(long begrq,long endrq);
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_de_call_ddBal(struct dd_mst_c dd_mst_tmp,int type,double *bal);
extern int pub_ins_trace_log();
extern int pub_de_FnQuota(char *prdt_no,double tx_amt,int mode);
extern double pub_num_dround ( double lx );
extern int pub_de_get_FnAcct_parm(char *cPrdt_no,char *dc_rel_code,char *cif_type,char *opn_br_no,char *fnqs_br_no,char *cAcct_prdt_no,char *cAc_no,char *br_type);
/*�ڲ���������*/
static int _cancel_apt_buy(char *flag);

int spH001()
{
    int ret = 0;
    char cOper_flag[2];/*0ԤԼ���� 1�Ϲ����� 2�깺���� 4��س���*/
    memset(cOper_flag,0x00,sizeof(cOper_flag));
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    get_zd_data("0670",cOper_flag);
    vtcp_trace("cOper_flag[%s]",cOper_flag);
    if(cOper_flag[0] == '0' || cOper_flag[0] == '1')
    {
        /*ԤԼ���Ϲ�״̬�ĳ���*/
        ret = _cancel_apt_buy(cOper_flag);
        if(ret)
        {
            vtcp_exit("����ʧ����");
            goto ErrExit;
        }
    }
    else if(cOper_flag[0] == '2')
    {
       /*�깺�ĳ���*/
    }
    else
    {
        vtcp_exit("�������ʹ��� [%s]",cOper_flag);
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }
    vtcp_exit("Before OK return: reply is[%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 0;
ErrExit:
    if(strlen(g_pub_env.reply) == 0 || strcmp(g_pub_env.reply,"0000") == 0)
    {
        strcpy(g_pub_env.reply,"S035");
    }
    vtcp_exit( "Before return: reply is[%s]", g_pub_env.reply);
    set_zd_data(DC_REPLY, g_pub_env.reply);
    return 1;
}

/*����ԤԼ���Ϲ�����
����flag: 0ԤԼ���� 1�Ϲ�����*/
static int _cancel_apt_buy(char *flag)
{
    int ret = 0;
    int iDays = 0;/*���ڼ�Ϣ����*/
    int iIntst_flag = 0;/*�Ƿ�ƻ�����Ϣ 0��1��*/
    long lIc_date = 0;/*��Ϣ����*/
    long lTx_date = 0;
    long lTrace_no = 0;
    double dIntst = 0.00;/*������Ϣ*/
    double dRate = 0.00;/*��������*/
    char cAc_no[33];
    char cAgent_name[NAME_LEN+1];
    char cAgent_id_no[21];
    char cAgent_id_type[2];
    char cDc_prdt_no[21];/*��Ʋ�Ʒ*/
    char cDc_ac_no[33];/*ļ���˺�*/
    char cBr_type[2];/*��������*/
    char cCif_type[2]; /*�ͻ�����1����2�Թ� ����Ϊͬҵ��ͬ����*/
    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;

    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAgent_id_no,0x00,sizeof(cAgent_id_no));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));
    memset(cCif_type,0x00,sizeof(cCif_type));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    get_zd_data("0300",cAc_no);/*����˺�*/
    get_zd_long("0440",&lTx_date);/*ԤԼ������ˮ*/
    get_zd_long("0280",&lTrace_no);/*ԤԼ������ˮ*/
    get_zd_data("0631",cAgent_id_type);/*�Թ��Ĵ�����֤������*/
    get_zd_data("0632",cAgent_id_no);/*�Թ��Ĵ�����֤����*/
    get_zd_data("0250",cAgent_name);/*�Թ��Ĵ���������*/
    strcpy(g_pub_tx.agent_id_no, cAgent_id_type);
    strcpy(g_pub_tx.agent_id_no, cAgent_id_no);
    strcpy(g_pub_tx.agent_id_name, cAgent_name);

    vtcp_trace("ltx_date[%ld],lTrace_no[%ld]",lTx_date,lTrace_no);

    /*����fn_appbuy_reg*/
    ret = Fn_appbuy_reg_Dec_Upd(g_pub_env.reply,"reg_date=%ld and reg_trace=%ld",
            lTx_date,lTrace_no);
    if(ret)
    {
        vtcp_exit("������Ʋ�Ʒ�Ǽǲ��α����[%d] ",ret);
        strcpy(g_pub_env.reply,"D332");
        goto ErrExit;
    }
    ret = Fn_appbuy_reg_Fet_Upd(&sFn_appbuy_reg,g_pub_env.reply);
    if(ret)
    {
        vtcp_exit("ȡ��Ʋ�Ʒ�Ǽǲ��α����[%d] ",ret);
        strcpy(g_pub_env.reply,"D331");
        goto ErrExit;
    }
    /*����Ƿ�����˺ŵļ�¼*/
    ret = Mdm_ac_rel_Sel(g_pub_env.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no);
    if(ret)
    {
        vtcp_exit("��ѯ�˺�ʧ����[%d] ",ret);
        strcpy(g_pub_env.reply,"P084");
        goto ErrExit;
    }
    if(sFn_appbuy_reg.de_ac_id != sMdm_ac_rel.ac_id)
    {
        vtcp_exit("��ͬһ���˺ŵ��Ϲ���¼ de_ac_id[%ld],ac_id[%ld] ",
            sFn_appbuy_reg.de_ac_id,sMdm_ac_rel.ac_id);
        strcpy(g_pub_env.reply,"��ͬһ���˺ŵ��Ϲ���¼");
        goto ErrExit;
    }
    ret = Dd_mst_Sel(g_pub_env.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%d",
        sFn_appbuy_reg.de_ac_id,sFn_appbuy_reg.de_ac_seqn);
    if(ret)
    {
        vtcp_exit("��ѯ�������ļ�ʧ����[%d] ",ret);
        goto ErrExit;
    }
    /*�ݶ�������������Ĳ��ܿ��������*/
    if(strcmp(sFn_appbuy_reg.reg_br_no,g_pub_env.tx_br_no) != 0
        && strcmp(sFn_appbuy_reg.chnl_type,CHNL_BANK_TEL) == 0)
    {
        vtcp_exit("�ü�¼�Ǳ���������[%s] ",sFn_appbuy_reg.reg_br_no);
        strcpy(g_pub_env.reply,"P301");
        goto ErrExit;
    }

    /*�ݶ��Ϲ�����Ҫ����Ʒ���� ԤԼ�Ŀɲ����*/
    if(flag[0] == '1' && strcmp(get_prdt_cparm(sFn_appbuy_reg.prdt_no,"bcancel_yn"),"Y") != 0)
    {
        vtcp_exit("����Ʋ�Ʒ���ܳ���");
        strcpy(g_pub_env.reply, "D324");
        goto ErrExit;
    }

    if(flag[0] == '0' && strcmp(get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"),"6") != 0)
    {
        /*�����ϲ�����������������ΪԤԼ�ڽ���������ת�Ϲ���*/
        vtcp_exit("�ò�Ʒ�ǿ�ԤԼ״̬[%s] ",get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"));
        strcpy(g_pub_env.reply,"D395");
        goto ErrExit;
    }
    else if(flag[0] == '1' && strcmp(get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"),"0") != 0)
    {
        vtcp_exit("�ò�Ʒ��ļ��״̬[%s] ",get_prdt_cparm(sFn_appbuy_reg.prdt_no,"prdt_sts"));
        strcpy(g_pub_env.reply,"D395");
        goto ErrExit;
    }
    vtcp_trace("flag[%s] app_knd[%s] sts[%s]",flag,sFn_appbuy_reg.app_knd,sFn_appbuy_reg.sts);
    /*���һ����ѡ��ʽ�ǲ��ǶԵ�*/
    if(flag[0] == '0')
    {
        strcpy(g_pub_tx.brf,"��Ʋ�ƷԤԼ����");
        /*ԤԼ����*/
        if(sFn_appbuy_reg.app_knd[0] == '1' && sFn_appbuy_reg.sts[0] == '0')
        {
            iIntst_flag = 0;
            strcpy(sFn_appbuy_reg.sts,"1");
            vtcp_trace("ԤԼ��������ûת�Ϲ��ĳ��������账����ƽ��");
        }
        else
        {
            vtcp_exit("�ù����¼��ԤԼ���򣬲���ʹ��ԤԼ����[%d] ",ret);
            set_zd_data(DC_GET_UMSG,"�ù����¼��ԤԼ���򣬲���ʹ��ԤԼ����");
            strcpy(g_pub_env.reply,"P275");
            goto ErrExit;
        }
    }
    else if(flag[0] == '1')
    {
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ�Ϲ�����");
        /*�Ϲ��ĳ��� ����������� ԤԼת���Ϲ��� ��ֱ���Ϲ���*/
        if(sFn_appbuy_reg.app_knd[0] == '2' && sFn_appbuy_reg.sts[0] == '0')
        {
            lIc_date = sFn_appbuy_reg.reg_date;/*��Ϣ��Ϊ�Ϲ���*/
            strcpy(sFn_appbuy_reg.sts,"1");
            vtcp_trace("ֱ���Ϲ��ĳ�������Ҫ���¿��ƽ��");
        }
        else if(sFn_appbuy_reg.app_knd[0] == '1' && sFn_appbuy_reg.app_knd[0] =='5')
        {
            lIc_date = get_prdt_lparm(sFn_appbuy_reg.prdt_no,"coll_beg_date");
            if(lIc_date <= 0)
            {
                vtcp_exit("��ȡ��Ϣ����ʧ����[%ld]",lIc_date);
                strcpy(g_pub_env.reply,"CUD2");
                goto ErrExit;
            }
            strcpy(sFn_appbuy_reg.sts,"7");
            vtcp_trace("ԤԼת�Ϲ��ĳ�������Ҫ���¿��ƽ��");
        }
        else
        {
            vtcp_exit("�ù����¼���Ϲ�״̬������ʹ���Ϲ�����[%d] ",ret);
            set_zd_data(DC_GET_UMSG,"�ù����¼���Ϲ�״̬������ʹ���Ϲ�����");
            strcpy(g_pub_env.reply,"P275");
            goto ErrExit;
        }
        iIntst_flag = 1;
    }

    ret = Fn_appbuy_reg_Upd_Upd(sFn_appbuy_reg,g_pub_env.reply);
    if(ret)
    {
        vtcp_exit("������Ʋ�Ʒ�Ǽǲ�����[%d] ",ret);
        strcpy(g_pub_env.reply,"D332");
        goto ErrExit;
    }
    Fn_appbuy_reg_Clo_Upd();
    /*��ȡ��Ʒ������ļ���˺�*/
    ret = pub_de_get_FnAcct_parm(sFn_appbuy_reg.prdt_no,"0001","0",g_pub_env.tx_br_no,
        QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
    if(ret)
    {
        vtcp_exit("��ȡ���ļ���˺�ʧ����");
        goto ErrExit;
    }
    vtcp_trace("����Ϲ��깺���ϻ���Ϊ[%s]",cDc_ac_no);
    set_zd_data("0310",cDc_ac_no);/*Ϊ��һ���������� todo*/
    if(cBr_type[0] == '1')
    {
        set_zd_data("0910",sFn_appbuy_reg.reg_br_no);
    }
    else
    {
        set_zd_data("0910",QS_BR_NO);
    }
    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));
    ret = pub_de_get_FnAcct_parm(sFn_appbuy_reg.prdt_no,"0005","0",g_pub_env.tx_br_no,
        QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
    if(ret)
    {
        vtcp_exit("��ȡ���ļ���˺�ʧ����");
        goto ErrExit;
    }
    set_zd_data("0320",cDc_ac_no);/*Ϊ��һ���������� todo*/
    if(cBr_type[0] == '1')
    {
        set_zd_data("0640",sFn_appbuy_reg.reg_br_no);
    }
    else
    {
        set_zd_data("0640",QS_BR_NO);
    }
    vtcp_trace("iIntst_flag[%d]",iIntst_flag);
    /*���������Ϣ�黹�ͻ�*/
    if(iIntst_flag == 1)
    {
        /*��������*/
        iDays = pub_date_CalTrueDays(lIc_date,g_pub_env.tx_date);
        if(iDays < 0)
        {
            vtcp_exit("��������ʧ����[%d]",iDays);
            strcpy(g_pub_env.reply,"P382");
            goto ErrExit;
        }
        vtcp_trace("iDays[%d]",iDays);
        strcpy(g_pub_tx.cur_no,get_prdt_cparm(sFn_appbuy_reg.prdt_no,"cur_no"));
        /*��ȡ��������*/
        ret = pub_rule_GetRate(HQLLBH, g_pub_tx.cur_no, g_pub_env.tx_date, &dRate);
        if (ret)
        {
            vtcp_exit("��ȡ����ʧ����[%d]",ret);
            goto ErrExit;
        }
        vtcp_trace("dRate[%lf]",dRate);
        dIntst = sFn_appbuy_reg.app_amt * dRate/L360/L100 * iDays;
        dIntst = pub_num_dround ( dIntst );
        vtcp_trace("������Ϣ[%lf]",dIntst);
        /*��ȡ������Ϣ֧���˺�*/
        memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
        memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
        strcpy(g_pub_tx.opn_br_no,sDd_mst.opn_br_no);
        ret = pub_de_get_acct_parm(sDd_mst.prdt_no,"0153",cDc_prdt_no,cDc_ac_no);
        if(ret)
        {
            vtcp_exit("��ȡ��Ϣ֧�������˺�ʧ����[%d]",ret);
            goto ErrExit;
        }
        set_zd_data("0330",cDc_ac_no);
        set_zd_data("0650",sDd_mst.opn_br_no);
        strcpy(g_pub_tx.opn_br_no,g_pub_env.tx_br_no);
    }

    /*�ָ���Ʒ��ض��*/
    ret = pub_de_FnQuota(sFn_appbuy_reg.prdt_no,sFn_appbuy_reg.app_amt,1);
    if(ret)
    {
        vtcp_exit("����������ret[%d]",ret);
        goto ErrExit;
    }
    strcpy(g_pub_tx.acc_ind,"0");
    strcpy(g_pub_tx.ac_no,cAc_no);
    g_pub_tx.tx_amt1 = sFn_appbuy_reg.app_amt;
    if( pub_ins_trace_log() )
    {
        vtcp_exit("�Ǽǽ�����ˮ����");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    strcpy(g_pub_env.reply,"0001");/*�߹黹�ͻ��������Ϣ�ķ�֧*/
    vtcp_exit("�����ɹ�");
    return 0;
ErrExit:
    vtcp_exit("����ʧ��");
    return 1;
}
