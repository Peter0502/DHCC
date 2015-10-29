/*******************************************************************************
*** �� �� ��:  V1.0
*** ��������: shangyongchao
*** ��    ��: 20151011
*** ����ģ��: ��Ʋ�Ʒ
*** ��������: spH001
*** ��������: �����Ʋ�Ʒ�Ϲ���ԤԼ
*** ʹ��ע��:
*** �޸ļ�¼:
***     �޸�����:
***     �޸�ʱ��:
***     �޸�����:
*******************************************************************************/
#define EXTERN
#include "public.h"
#include "de_public.h"
#include "macro.h"
#include "fn_appbuy_reg_c.h"
#include "fn_appbuy_reg_fun.h"
/*�ⲿ��������*/
extern int pub_de_get_FnAcct_parm(char *cPrdt_no,char *dc_rel_code,char *cif_type,char *opn_br_no,char *fnqs_br_no,char *cAcct_prdt_no,char *cAc_no,char *br_type);
extern double pub_num_dround ( double lx );
extern int pub_com_CheqClsNo( long ac_id,int ac_seqn,char *note_type,char *cheq_beg_no,char *cheq_end_no);
extern int pub_cif_get_ciflvl(long cif_no, char *cif_lvl);
extern int pub_cif_ac2ciftype(char *ac_no, char *cif_type);
extern int pub_num_amtcmp(double amt1,double amt2);
extern int pub_de_call_ddBal(struct dd_mst_c dd_mst_tmp,int type,double *bal);
extern long pub_time_date();
extern int pub_ins_trace_log();
extern int pub_de_FnQuota(char *prdt_no,double tx_amt,int mode);
/*�ڲ���������*/
static int _buy_func(void);
static int _apt_func(void);
static int _check_note_sts(char *note_sts);

int spH001(void)
{
    int iRet = 0;
    char cOper_flag[2];/*0ԤԼ 1�Ϲ� 2����*/
    memset(cOper_flag,0x00,sizeof(cOper_flag));
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    get_zd_data("0670",cOper_flag);
    vtcp_trace("cOper_flag[%s]",cOper_flag);
    /*����cOper_flag���в�ͬ����*/
    if(cOper_flag[0] == '0')
    {
        /*ԤԼ*/
        iRet = _apt_func();
        if(iRet)
        {
            vtcp_exit("ԤԼʧ����");
            goto ErrExit;
        }
    }
    else if(cOper_flag[0] == '1')
    {
        /*�Ϲ�*/
        iRet = _buy_func();
        if(iRet)
        {
            vtcp_exit("�Ϲ�ʧ����");
            goto ErrExit;
        }
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

/*�Ϲ�����*/
static int _buy_func(void)
{
    int iRet = 0;
    int iAc_seqn = 0;/*�˻����*/
    char cDc_prdt_no[21];/*��Ʋ�Ʒ*/
    char cDc_ac_no[33];/*ļ���˺�*/
    char cBr_type[2];/*��������*/
    char cOpn_time[7];/*Ӫҵ��ʼʱ��*/
    char cClos_time[7];/*Ӫҵ����ʱ��*/
    char cCt_ind[2];/*��ת��־*/
    char cPrdt_no[21]; /*��Ʒ����*/
    char cCif_type[2]; /*�ͻ�����1����2�Թ� ����Ϊͬҵ��ͬ����*/
    char cAgent_id_type[2]; /*�Թ����Ĵ�����֤������*/
    char cAgent_id_no[21]; /*�Թ����Ĵ�����֤��*/
    char cAgent_name[NAME_LEN+1]; /*�Թ����Ĵ���������*/
    char cAc_no[33]; /*�˺�*/
    char cPact_no[21]; /*��ͬ��*/
    char cMang[TEL_LEN+1]; /*�ͻ������*/
    char cCif_lvl[2];
    char cNote_type[4]; /*ƾ֤����*/
    char cNote_no[17]; /*ƾ֤����*/
    char cFn_note_no[17]; /*���ƾ֤��*/
    double dAvbal = 0.00; /*�˻��������*/
    double dFlt_radio = 0.00;/*�����ϸ�����*/
    double dTx_amt = 0.00;/*�Ϲ����*/

    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct dd_mst_c sDd_mst;
    /*��ʼ������*/
    memset(cOpn_time,0x00,sizeof(cOpn_time));
    memset(cClos_time,0x00,sizeof(cClos_time));
    memset(cCt_ind,0x00,sizeof(cCt_ind));
    memset(cPrdt_no,0x00,sizeof(cPrdt_no));
    memset(cCif_type,0x00,sizeof(cCif_type));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id_no,0x00,sizeof(cAgent_id_no));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cPact_no,0x00,sizeof(cPact_no));
    memset(cMang,0x00,sizeof(cMang));
    memset(cCif_lvl,0x00,sizeof(cCif_lvl));
    memset(cNote_type,0x00,sizeof(cNote_type));
    memset(cNote_no,0x00,sizeof(cNote_no));
    memset(cFn_note_no,0x00,sizeof(cFn_note_no));
    memset(cDc_prdt_no,0x00,sizeof(cDc_prdt_no));
    memset(cDc_ac_no,0x00,sizeof(cDc_ac_no));
    memset(cBr_type,0x00,sizeof(cBr_type));

    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));

    get_zd_data("0670",cCt_ind);/*��ת��־*/
    get_zd_int("0360",&iAc_seqn);/*�����˻����*/
    get_zd_double("0390",&dTx_amt);/*���*/
    get_zd_data("0300",cAc_no);/*�����˺�*/
    get_zd_data("0631",cAgent_id_type);/*�Թ��Ĵ�����֤������*/
    get_zd_data("0632",cAgent_id_no);/*�Թ��Ĵ�����֤����*/
    get_zd_data("0250",cAgent_name);/*�Թ��Ĵ���������*/
    get_zd_data("0230",cPrdt_no);/*�����Ʒ��*/
    get_zd_data("0890",cPact_no);/*����Э���*/
    get_zd_data("0840",cMang);/*�ͻ�����*/
    get_zd_data("0581",cNote_type);/*ƾ֤����*/
    get_zd_data("0582",cNote_no);/*ƾ֤����*/
    get_zd_data("0592",cFn_note_no);/*���ƾ֤��*/

    strcpy(g_pub_tx.agent_id_no, cAgent_id_type);
    strcpy(g_pub_tx.agent_id_no, cAgent_id_no);
    strcpy(g_pub_tx.agent_id_name, cAgent_name);

    /*�ȼ���˺���Ϣ*/
    pub_base_old_acno(cAc_no);
    iRet = pub_cif_ac2ciftype(cAc_no,cCif_type);
    if (iRet)
    {
        vtcp_exit( "���ҿͻ����ʹ���[%d]", iRet);
        strcpy(g_pub_env.reply, "P336");
        goto ErrExit;
    }
    /*������״̬*/
    iRet = _check_note_sts(g_mdm_ac_rel.mdm_sts);
    if(iRet)
    {
        vtcp_exit("����״̬������");
        goto ErrExit;
    }
    /*����˻���ͬʱ���Ŀ��ƽ��*/
    iRet = Dd_mst_Dec_Upd(g_pub_env.reply,"ac_id= %ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,iAc_seqn);
    if(iRet)
    {
        vtcp_exit("����������ļ������α�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D227");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_env.reply);
    if(iRet != DB_OK && iRet != DB_NOTFOUND)
    {
        vtcp_exit("����������ļ������α�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D230");
        goto ErrExit;
    }
    else if(iRet == DB_NOTFOUND)
    {
        vtcp_exit("û������˻�");
        strcpy(g_pub_env.reply,"D229");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_exit("�����˻��˻�״̬������");
        strcpy(g_pub_env.reply,"P183");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
            || sDd_mst.zf_sts[0] == '1' || sDd_mst.zf_sts[0] == '2')
    {
        vtcp_exit("�˻���ȫ�������ȫ��ֹ����ֻ������");
        strcpy(g_pub_env.reply,"D130");
        goto ErrExit;
    }
    /*����¿������ ��ʱ��Ҫ����ͨ�����*/
    pub_de_call_ddBal(sDd_mst,0,&dAvbal);
    /*������˻��ı���*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no) != 0)
    {
        vtcp_exit("���ֲ�֧��parm[%s] dd_mst[%s]",get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no);
        strcpy(g_pub_env.reply,"D337");
        goto ErrExit;
    }
    if(pub_num_amtcmp(dTx_amt,dAvbal) > 0)
    {
        vtcp_exit("�˻���������");
        strcpy(g_pub_env.reply,"P066");
        goto ErrExit;
    }
    iRet =  Dd_mst_Upd_Upd(sDd_mst,g_pub_env.reply);
    if(iRet)
    {
        vtcp_exit("���»������ļ�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D227");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
    /*���ݿͻ��Ż�ȡ�ͻ�����*/
    iRet = pub_cif_get_ciflvl(sDd_mst.cif_no,cCif_lvl);
    if(iRet)
    {
        vtcp_exit("��ȡ�ͻ�����ʧ��[%d]",iRet);
        goto ErrExit;
    }
    vtcp_trace("�ͻ�����[%s]�ͻ�����[%s]",cCif_lvl,cCif_type);
    /*�Թ��� ���ͻ�ƾ֤*/
    if(cCif_type[0]  == '2' && strcmp(cNote_type,"299") != 0)
    {
        iRet = pub_com_CheqClsNo(g_mdm_ac_rel.ac_id,iAc_seqn, cNote_type,cNote_no,cNote_no);
        if(iRet)
        {
            vtcp_exit("���ǿͻ�ƾ֤ʧ����[%d]",iRet);
            goto ErrExit;
        }
    }
    /*��Ʋ�Ʒ״̬*/
    if(strcmp(get_prdt_cparm(cPrdt_no, "prdt_sts"),"0") != 0)
    {
        vtcp_exit("�ò�Ʒ���Ϲ�״̬[%s] ",get_prdt_cparm(cPrdt_no, "prdt_sts"));
        strcpy(g_pub_env.reply,"D334");
        goto ErrExit;
    }
    /*����Ƿ���ļ����*/
    if(get_prdt_lparm(cPrdt_no, "coll_beg_date") > g_pub_env.tx_date
        || get_prdt_lparm(cPrdt_no, "coll_end_date") < g_pub_env.tx_date)
    {
        vtcp_exit("�ò�Ʒ���Ϲ�״̬");
        strcpy(g_pub_env.reply,"C334");
        goto ErrExit;
    }
    /*��֤��ת*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_cash_yn"),"N") == 0 && cCt_ind[0] == '1')
    {
        vtcp_exit("�ò�Ʒ�����ֽ���");
        strcpy(g_pub_env.reply,"D392");
        goto ErrExit;
    }
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_tran_yn"),"N") == 0 && cCt_ind[0] == '2')
    {
        vtcp_exit("�ò�Ʒ����ת�˹���");
        strcpy(g_pub_env.reply,"D393");
        goto ErrExit;
    }
    /*�Ϲ����� todo*/
    /*�Ϲ�ʱ��Ŀ��ƣ���������ЧӪҵʱ����*/
    memcpy(cOpn_time,get_prdt_cparm(cPrdt_no,"bopen_time"),sizeof(cOpn_time)-1);
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(g_pub_env.tx_time < atol(cOpn_time) || g_pub_env.tx_time > atol(cClos_time))
    {
        vtcp_exit("���ڲ�Ʒ��Ӫҵʱ����");
        strcpy(g_pub_env.reply,"D396");
        goto ErrExit;
    }
    /*��֤�ͻ�����*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cif_type"),"1") == 0 && cCif_type[0] != '1')/*����*/
    {
        vtcp_exit("�ͻ����Ͳ���[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"2") == 0 && cCif_type[0] != '2')/*�Թ�*/
    {
        vtcp_exit("�ͻ����Ͳ���[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"3") == 0 && cCif_type[0] != '4')/*ͬҵ*/
    {
        vtcp_exit("�ͻ����Ͳ���[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    /*�����Ʒ���*/
    iRet = pub_de_FnQuota(cPrdt_no,dTx_amt,0);
    if(iRet)
    {
        vtcp_exit("����������iRet[%d]",iRet);
        goto ErrExit;
    }
    /*�Ƿ���������Ϲ���*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt")) < 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt")) < 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    /*�Ƿ��������Ϲ���*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt")) > 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt")) > 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    /*��λ�������У��todo*/
    /*��ȡ���渡������ todo*/
    strcpy(sFn_appbuy_reg.flt_type,"");
    sFn_appbuy_reg.flt_radio = dFlt_radio;
    /*������Ǽǲ������ֶθ�ֵ*/
    sFn_appbuy_reg.app_amt  = dTx_amt;
    sFn_appbuy_reg.share_amt = dTx_amt / (get_prdt_dparm(cPrdt_no,"unit_amt"));
    /*��ȡ��ƷԤ�������� todo*/
    sFn_appbuy_reg.rate = 0.00;
    /*����Ԥ������*/
    if(sFn_appbuy_reg.flt_type[0] == '1')/*�����ϸ�*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * (sFn_appbuy_reg.rate + sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    else if(sFn_appbuy_reg.flt_type[0] == '2')/*�����ϸ�*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * sFn_appbuy_reg.rate * (1+ sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    sFn_appbuy_reg.intst = pub_num_dround(sFn_appbuy_reg.intst);
    set_zd_double("0400",sFn_appbuy_reg.intst);
    set_zd_double("0960",sFn_appbuy_reg.flt_radio);
    sFn_appbuy_reg.reg_date = g_pub_env.tx_date;
    sFn_appbuy_reg.reg_trace=g_pub_env.trace_no;
    strcpy(sFn_appbuy_reg.reg_br_no,g_pub_env.tx_br_no);
    strcpy(sFn_appbuy_reg.reg_tel,g_pub_env.tel);
    strcpy(sFn_appbuy_reg.chnl_type,g_pub_rule.chnl_type);
    sFn_appbuy_reg.nat_date=pub_time_date();
    sFn_appbuy_reg.reg_time=g_pub_env.tx_time;
    strcpy(sFn_appbuy_reg.prdt_no,cPrdt_no);
    sFn_appbuy_reg.de_ac_id = sDd_mst.ac_id;
    sFn_appbuy_reg.de_ac_seqn = sDd_mst.ac_seqn;
    sFn_appbuy_reg.cif_no = sDd_mst.cif_no;
    strcpy(sFn_appbuy_reg.cif_type,cCif_type);
    strcpy(sFn_appbuy_reg.note_type,cNote_type);
    strcpy(sFn_appbuy_reg.note_no,cNote_no);
    strcpy(sFn_appbuy_reg.fn_note_no,cFn_note_no);
    strcpy(sFn_appbuy_reg.pact_no,cPact_no);
    strcpy(sFn_appbuy_reg.agent_id_type,cAgent_id_type);
    strcpy(sFn_appbuy_reg.agent_id_no,cAgent_id_no);
    strcpy(sFn_appbuy_reg.agent_name,cAgent_name);
    strcpy(sFn_appbuy_reg.ct_ind,cCt_ind);
    if(cCt_ind[0] == '1')
    {
        strcpy(sFn_appbuy_reg.pay_yn,"Y");
        strcpy(sFn_appbuy_reg.fee_pay_yn,"Y");
    }
    else
    {
        strcpy(sFn_appbuy_reg.pay_yn,"N");/*ת�˹���Ķ��ȳ����ٿ�Ǯ*/
        strcpy(sFn_appbuy_reg.fee_pay_yn,"N");/*�������Ƿ��ѿ�todo*/
    }
    sFn_appbuy_reg.fee_amt = 0.00;/*������ todo*/
    sFn_appbuy_reg.curr_issue = get_prdt_iparm(cPrdt_no,"curr_issue");/*�Ϲ��Ķ��Ǵ�1��ʼ��*/
    strcpy(sFn_appbuy_reg.app_knd,"2");
    strcpy(sFn_appbuy_reg.brf,"�Ϲ���Ʋ�Ʒ");
    strcpy(sFn_appbuy_reg.sts,"0");
    iRet = Fn_appbuy_reg_Ins(sFn_appbuy_reg,g_pub_env.reply);
    if(iRet)
    {
        vtcp_exit("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D331");
        goto ErrExit;
    }

    /*�Ǽ���ˮ*/
    strcpy(g_pub_tx.ac_no,cAc_no);
    strcpy(g_pub_tx.brf,"��Ʋ�Ʒ�Ϲ�");
    g_pub_tx.tx_amt1 = dTx_amt;
    strcpy(g_pub_tx.acc_ind,"0");
    if( pub_ins_trace_log() )
    {
        vtcp_exit("�Ǽǽ�����ˮ����");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    /*��ȡ��Ʒ������ļ���˺�*/
    iRet = pub_de_get_FnAcct_parm(cPrdt_no,"0001","0",g_pub_env.tx_br_no,
        QS_BR_NO,cDc_prdt_no,cDc_ac_no,cBr_type);
    if(iRet)
    {
        vtcp_exit("��ȡ���ļ���˺�ʧ����");
        goto ErrExit;
    }
    if(cCt_ind[0] == '1')
    {
        strcpy(g_pub_env.reply,"0001");/*�ֽ����һ������Ҫ��ȡ�ֽ�*/
    }
    else
    {
        strcpy(g_pub_env.reply,"0002");/*ת�˵Ŀۿ�*/
    }
    vtcp_exit("�Ϲ��ɹ�[%s]",g_pub_env.reply);
    return 0;
ErrExit:
    vtcp_exit("�Ϲ�ʧ��");
    return 1;
}
/*ԤԼ���� ԤԼ�����ƿͻ��ʽ� ֻ���²�Ʒ���*/
static int _apt_func(void)
{
    int iRet = 0;
    int iAc_seqn = 0;/*�˻����*/
    char cOpn_time[7];/*Ӫҵ��ʼʱ��*/
    char cClos_time[7];/*Ӫҵ����ʱ��*/
    char cCt_ind[2];/*��ת��־*/
    char cPrdt_no[21]; /*��Ʒ����*/
    char cCif_type[2]; /*�ͻ�����1����2�Թ� ����Ϊͬҵ��ͬ����*/
    char cAgent_id_type[2]; /*�Թ����Ĵ�����֤������*/
    char cAgent_id_no[21]; /*�Թ����Ĵ�����֤��*/
    char cAgent_name[NAME_LEN+1]; /*�Թ����Ĵ���������*/
    char cAc_no[33]; /*�˺�*/
    char cPact_no[21]; /*��ͬ��*/
    char cMang[TEL_LEN+1]; /*�ͻ������*/
    char cCif_lvl[2];
    char cNote_type[4]; /*ƾ֤����*/
    char cNote_no[17]; /*ƾ֤����*/
    char cFn_note_no[17]; /*���ƾ֤��*/
    double dFlt_radio = 0.00;/*�����ϸ�����*/
    double dTx_amt = 0.00;/*�Ϲ����*/

    struct fn_appbuy_reg_c sFn_appbuy_reg;
    struct dd_mst_c sDd_mst;
    /*��ʼ������*/
    memset(cOpn_time,0x00,sizeof(cOpn_time));
    memset(cClos_time,0x00,sizeof(cClos_time));
    memset(cCt_ind,0x00,sizeof(cCt_ind));
    memset(cPrdt_no,0x00,sizeof(cPrdt_no));
    memset(cCif_type,0x00,sizeof(cCif_type));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id_no,0x00,sizeof(cAgent_id_no));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cPact_no,0x00,sizeof(cPact_no));
    memset(cMang,0x00,sizeof(cMang));
    memset(cCif_lvl,0x00,sizeof(cCif_lvl));
    memset(cNote_type,0x00,sizeof(cNote_type));
    memset(cNote_no,0x00,sizeof(cNote_no));
    memset(cFn_note_no,0x00,sizeof(cFn_note_no));
    memset(&sFn_appbuy_reg,0x00,sizeof(sFn_appbuy_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));

    get_zd_data("0670",cCt_ind);/*��ת*/
    get_zd_int("0360",&iAc_seqn);/*�����˻����*/
    get_zd_double("0390",&dTx_amt);/*���*/
    get_zd_data("0300",cAc_no);/*�����˺�*/
    get_zd_data("0631",cAgent_id_type);/*�Թ��Ĵ�����֤������*/
    get_zd_data("0632",cAgent_id_no);/*�Թ��Ĵ�����֤����*/
    get_zd_data("0250",cAgent_name);/*�Թ��Ĵ���������*/
    get_zd_data("0230",cPrdt_no);/*�����Ʒ��*/
    get_zd_data("0890",cPact_no);/*����Э���*/
    get_zd_data("0840",cMang);/*�ͻ�����*/
    get_zd_data("0581",cNote_type);/*ƾ֤����*/
    get_zd_data("0582",cNote_no);/*ƾ֤����*/
    get_zd_data("0592",cFn_note_no);/*���ƾ֤��*/

    strcpy(g_pub_tx.agent_id_no, cAgent_id_type);
    strcpy(g_pub_tx.agent_id_no, cAgent_id_no);
    strcpy(g_pub_tx.agent_id_name, cAgent_name);

    /*�ȼ���˺���Ϣ*/
    pub_base_old_acno(cAc_no);
    iRet = pub_cif_ac2ciftype(cAc_no,cCif_type);
    if (iRet)
    {
        vtcp_exit( "���ҿͻ����ʹ���[%d]", iRet);
        strcpy(g_pub_env.reply, "P336");
        goto ErrExit;
    }
    /*������״̬*/
    iRet = _check_note_sts(g_mdm_ac_rel.mdm_sts);
    if(iRet)
    {
        vtcp_exit("����״̬������");
        goto ErrExit;
    }
    /*����˻���ͬʱ���Ŀ��ƽ��*/
    iRet = Dd_mst_Dec_Upd(g_pub_env.reply,"ac_id= %ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,iAc_seqn);
    if(iRet)
    {
        vtcp_exit("����������ļ������α�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D227");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_env.reply);
    if(iRet != DB_OK && iRet != DB_NOTFOUND)
    {
        vtcp_exit("����������ļ������α�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D230");
        goto ErrExit;
    }
    else if(iRet == DB_NOTFOUND)
    {
        vtcp_exit("û������˻�");
        strcpy(g_pub_env.reply,"D229");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_exit("�����˻��˻�״̬������");
        strcpy(g_pub_env.reply,"P183");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
            || sDd_mst.zf_sts[0] == '1' || sDd_mst.zf_sts[0] == '2')
    {
        vtcp_exit("�˻���ȫ�������ȫ��ֹ����ֻ������");
        strcpy(g_pub_env.reply,"D130");
        goto ErrExit;
    }
    /*������˻��ı���*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no) != 0)
    {
        vtcp_exit("���ֲ�֧��parm[%s] dd_mst[%s]",get_prdt_cparm(cPrdt_no,"cur_no"),sDd_mst.cur_no);
        strcpy(g_pub_env.reply,"D337");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
    /*���ݿͻ��Ż�ȡ�ͻ�����*/
    iRet = pub_cif_get_ciflvl(sDd_mst.cif_no,cCif_lvl);
    if(iRet)
    {
        vtcp_exit("��ȡ�ͻ�����ʧ��[%d]",iRet);
        goto ErrExit;
    }
    vtcp_trace("�ͻ�����[%s]�ͻ�����[%s]",cCif_lvl,cCif_type);
    
    /*��Ʋ�Ʒ״̬*/
    if(strcmp(get_prdt_cparm(cPrdt_no, "prdt_sts"),"6") != 0)
    {
        vtcp_exit("�ò�Ʒ��ԤԼ״̬[%s] ",get_prdt_cparm(cPrdt_no, "prdt_sts"));
        strcpy(g_pub_env.reply,"D395");
        goto ErrExit;
    }
    /*����Ƿ���ԤԼ��*/
    if(g_pub_env.tx_date >= get_prdt_lparm(cPrdt_no, "coll_beg_date"))
    {
        vtcp_exit("��Ʒ�Ѿ�����ԤԼ��");
        strcpy(g_pub_env.reply,"D394");
        goto ErrExit;
    }
    /*��֤��ת*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_cash_yn"),"N") == 0 && cCt_ind[0] == '1')
    {
        vtcp_exit("�ò�Ʒ�����ֽ���");
        strcpy(g_pub_env.reply,"D392");
        goto ErrExit;
    }
    if(strcmp(get_prdt_cparm(cPrdt_no,"bdpst_tran_yn"),"N") == 0 && cCt_ind[0] == '2')
    {
        vtcp_exit("�ò�Ʒ����ת�˹���");
        strcpy(g_pub_env.reply,"D393");
        goto ErrExit;
    }
    /*�Ϲ����� todo*/
    /*�Ϲ�ʱ��Ŀ��ƣ���������ЧӪҵʱ����*/
    memcpy(cOpn_time,get_prdt_cparm(cPrdt_no,"bopen_time"),sizeof(cOpn_time)-1);
    memcpy(cClos_time,get_prdt_cparm(cPrdt_no,"bclose_time"),sizeof(cClos_time)-1);
    if(g_pub_env.tx_time < atol(cOpn_time) || g_pub_env.tx_time > atol(cClos_time))
    {
        vtcp_exit("���ڲ�Ʒ��Ӫҵʱ����");
        strcpy(g_pub_env.reply,"D396");
        goto ErrExit;
    }
    /*��֤�ͻ�����*/
    if(strcmp(get_prdt_cparm(cPrdt_no,"cif_type"),"1") == 0 && cCif_type[0] != '1')/*����*/
    {
        vtcp_exit("�ͻ����Ͳ���[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"2") == 0 && cCif_type[0] != '2')/*�Թ�*/
    {
        vtcp_exit("�ͻ����Ͳ���[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    else if(strcmp(get_prdt_cparm(cPrdt_no, "cif_type"),"3") == 0 && cCif_type[0] != '4')/*ͬҵ*/
    {
        vtcp_exit("�ͻ����Ͳ���[%s] ",cCif_type);
        strcpy(g_pub_env.reply,"P336");
        goto ErrExit;
    }
    /*��ȡ�û������*/
    iRet = pub_de_FnQuota(cPrdt_no,dTx_amt,0);
    if(iRet)
    {
        vtcp_exit("����������iRet[%d]",iRet);
        goto ErrExit;
    }
    /*�Ƿ���������Ϲ���*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt")) < 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt")) < 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmin_amt"));
        strcpy(g_pub_env.reply,"D338");
        goto ErrExit;
    }
    /*�Ƿ��������Ϲ���*/
    if(cCif_type[0] == '1' && pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt")) > 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_pmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    else if(pub_num_amtcmp(dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt")) > 0)
    {
        vtcp_exit("��������Ϲ����[%lf][%lf]",dTx_amt,get_prdt_dparm(cPrdt_no,"bdpst_cmax_amt"));
        strcpy(g_pub_env.reply,"D391");
        goto ErrExit;
    }
    /*�Թ��� ���ͻ�ƾ֤*/
    if(cCif_type[0]  == '2' && strcmp(cNote_type,"299") != 0)
    {
        iRet = pub_com_CheqClsNo(g_mdm_ac_rel.ac_id,iAc_seqn, cNote_type,cNote_no,cNote_no);
        if(iRet)
        {
            vtcp_exit("���ǿͻ�ƾ֤ʧ����[%d]",iRet);
            goto ErrExit;
        }
    }
    /*��λ�������У��todo*/
    /*��ȡ���渡������ todo*/
    strcpy(sFn_appbuy_reg.flt_type,"");
    sFn_appbuy_reg.flt_radio = dFlt_radio;
    /*������Ǽǲ������ֶθ�ֵ*/
    sFn_appbuy_reg.app_amt  = dTx_amt;
    sFn_appbuy_reg.share_amt = dTx_amt / (get_prdt_dparm(cPrdt_no,"unit_amt"));
    /*��ȡ��ƷԤ�������� todo*/
    sFn_appbuy_reg.rate = 0.00;
    /*����Ԥ������*/
    if(sFn_appbuy_reg.flt_type[0] == '1')/*�����ϸ�*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * (sFn_appbuy_reg.rate + sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    else if(sFn_appbuy_reg.flt_type[0] == '2')/*�����ϸ�*/
    {
        sFn_appbuy_reg.intst=sFn_appbuy_reg.app_amt * get_prdt_iparm(cPrdt_no,"term")
                         * sFn_appbuy_reg.rate * (1+ sFn_appbuy_reg.flt_radio)/get_prdt_iparm(cPrdt_no,"intst_rate_mode")/L100;
    }
    sFn_appbuy_reg.intst = pub_num_dround(sFn_appbuy_reg.intst);
    set_zd_double("0400",sFn_appbuy_reg.intst);
    set_zd_double("0960",sFn_appbuy_reg.flt_radio);
    sFn_appbuy_reg.reg_date = g_pub_env.tx_date;
    sFn_appbuy_reg.reg_trace=g_pub_env.trace_no;
    strcpy(sFn_appbuy_reg.reg_br_no,g_pub_env.tx_br_no);
    strcpy(sFn_appbuy_reg.reg_tel,g_pub_env.tel);
    strcpy(sFn_appbuy_reg.chnl_type,g_pub_rule.chnl_type);
    sFn_appbuy_reg.nat_date=pub_time_date();
    sFn_appbuy_reg.reg_time=g_pub_env.tx_time;
    strcpy(sFn_appbuy_reg.prdt_no,cPrdt_no);
    sFn_appbuy_reg.de_ac_id = sDd_mst.ac_id;
    sFn_appbuy_reg.de_ac_seqn = sDd_mst.ac_seqn;
    sFn_appbuy_reg.cif_no = sDd_mst.cif_no;
    strcpy(sFn_appbuy_reg.cif_type,cCif_type);
    strcpy(sFn_appbuy_reg.note_type,cNote_type);
    strcpy(sFn_appbuy_reg.note_no,cNote_no);
    strcpy(sFn_appbuy_reg.fn_note_no,cFn_note_no);
    strcpy(sFn_appbuy_reg.pact_no,cPact_no);
    strcpy(sFn_appbuy_reg.agent_id_type,cAgent_id_type);
    strcpy(sFn_appbuy_reg.agent_id_no,cAgent_id_no);
    strcpy(sFn_appbuy_reg.agent_name,cAgent_name);
    strcpy(sFn_appbuy_reg.ct_ind,cCt_ind);
    if(cCt_ind[0] == '1')
    {
        strcpy(sFn_appbuy_reg.pay_yn,"Y");
        strcpy(sFn_appbuy_reg.fee_pay_yn,"Y");
    }
    else
    {
        strcpy(sFn_appbuy_reg.pay_yn,"N");/*ת�˹���Ķ��ȳ����ٿ�Ǯ*/
        strcpy(sFn_appbuy_reg.fee_pay_yn,"N");/*�������Ƿ��ѿ�todo*/
    }
    sFn_appbuy_reg.fee_amt = 0.00;/*������ todo*/
    sFn_appbuy_reg.curr_issue = get_prdt_iparm(cPrdt_no,"curr_issue");/*�Ϲ��Ķ��Ǵ�1��ʼ��*/
    strcpy(sFn_appbuy_reg.app_knd,"1");
    strcpy(sFn_appbuy_reg.brf,"ԤԼ��Ʋ�Ʒ");
    strcpy(sFn_appbuy_reg.sts,"0");
    iRet = Fn_appbuy_reg_Ins(sFn_appbuy_reg,g_pub_env.reply);
    if(iRet)
    {
        vtcp_exit("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
        strcpy(g_pub_env.reply,"D331");
        goto ErrExit;
    }

    /*�Ǽ���ˮ*/
    strcpy(g_pub_tx.ac_no,cAc_no);
    strcpy(g_pub_tx.brf,"��Ʋ�ƷԤԼ");
    g_pub_tx.tx_amt1 = dTx_amt;
    strcpy(g_pub_tx.acc_ind,"0");
    if( pub_ins_trace_log() )
    {
        vtcp_exit("�Ǽǽ�����ˮ����");
        strcpy(g_pub_env.reply,"P431");
        goto ErrExit;
    }
    if(cCt_ind[0] == '1')
    {
        strcpy(g_pub_env.reply,"0001");/*�ֽ����һ������Ҫ��ȡ�ֽ�*/
    }
    else
    {
        strcpy(g_pub_env.reply,"0000");
    }
    vtcp_exit("ԤԼ�ɹ�[%s]",g_pub_env.reply);
    return 0;
ErrExit:
    vtcp_exit("�Ϲ�ʧ��");
    return 1;
}
#if 0
/*�õ�����ͻ��������ϸ�����,�жϿͻ����ͺ��˺��Ƿ�һ��*/
int get_flt_radio(char *prdt_code,char *radio_type,double tx_amt,char * cif_lvl,long ac_id,double *flt_radio)
{
    int iRet = 0;
    double dBuy_amt = 0.00;
    struct fn_flt_radio_parm_c sFn_flt_rp;
    memset(&sFn_flt_rp,0x00,sizeof(sFn_flt_rp));
    /*ȡ�����Ʒ���ϸ�����*/
    if(radio_type[0] == '1')
    {
        /*�ͻ�����*/
        iRet = Fn_flt_radio_parm_Sel(g_pub_env.reply,&sFn_flt_rp,"prdt_code='%s' and radio_type ='1' and cif_lvl1 <='%s' and cif_lvl2 >='%s'",prdt_code,cif_lvl,cif_lvl);
        if(iRet != DB_NOTFOUND && iRet != DB_OK)
        {
            vtcp_exit("��ѯ��Ʋ�Ʒ�������ʱ����!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D333");
            goto ErrExit;
        }
        else if(iRet == DB_NOTFOUND)
        {
            vtcp_exit("��Ʋ�Ʒ�������ʱ�δ���ø������ϸ�����!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D343");
            goto ErrExit;
        }
        else
        {
            *flt_radio = sFn_flt_rp.flt_radio1;
        }
    }
    else if(radio_type[0] == '2')
    {
        /*�����Ϲ�*/
        iRet = Fn_flt_radio_parm_Sel(g_pub_env.reply,&sFn_flt_rp,"prdt_code='%s' and radio_type ='2' and buy_amt1 <=%lf and buy_amt2 >=%lf",prdt_code,tx_amt,tx_amt);
        if(iRet != DB_NOTFOUND && iRet != DB_OK)
        {
            vtcp_exit("��ѯ��Ʋ�Ʒ�������ʱ����!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D333");
            goto ErrExit;
        }
        else if(iRet == DB_NOTFOUND)
        {
            vtcp_exit("��Ʋ�Ʒ�������ʱ�δ���ø������ϸ�����!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D343");
            goto ErrExit;
        }
        else
        {
            *flt_radio = sFn_flt_rp.flt_radio2;
        }
    }
    else if(radio_type[0] == '3')
    {
        /*�Ϲ��ܶ� ֻ���Ѿ��Ϲ��˵� ���㱾�ε�*/
        /*ͳ������˺������Ʒһ�����˶���*/
        iRet = sql_sum_double("fn_reg","tx_amt",&dBuy_amt,"prdt_code='%s' and ac_id=%ld and sts='0'",\
                              prdt_code,ac_id);
        if(iRet)
        {
            vtcp_exit("ͳ��fn_reg����!");
            strcpy(g_pub_env.reply, "D330");
            goto ErrExit;
        }
        vtcp_trace("�Ѿ��Ϲ����[%lf]",dBuy_amt);
        iRet = Fn_flt_radio_parm_Sel(g_pub_env.reply,&sFn_flt_rp,"prdt_code='%s' and radio_type ='3' and total_amt1 <=%lf and total_amt2 >= %lf",prdt_code,dBuy_amt,dBuy_amt);
        if(iRet != DB_NOTFOUND && iRet != DB_OK)
        {
            vtcp_exit("��ѯ��Ʋ�Ʒ�������ʱ����!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D333");
            goto ErrExit;
        }
        else if(iRet == DB_NOTFOUND)
        {
            vtcp_exit("��Ʋ�Ʒ�������ʱ�δ���ø������ϸ�����!iRet[%d]",iRet);
            strcpy(g_pub_env.reply, "D343");
            goto ErrExit;
        }
        else
        {
            *flt_radio = sFn_flt_rp.flt_radio3;
        }
    }
    else
    {
        vtcp_exit("����ĸ�����ʽ! radio_type[%s]",radio_type);
        strcpy(g_pub_env.reply, "S035");
        goto ErrExit;
    }
    vtcp_trace("�ϸ�����[%lf]",*flt_radio);
    strcpy(g_pub_env.reply,"0000");
    vtcp_exit("��ȡ�ϸ������ɹ�");
    return 0;
ErrExit:
    vtcp_exit("��ȡ�ϸ�����ʧ��");
    return 1;
}
#endif

/*����˺ŵĽ���״̬*/
static int _check_note_sts(char *note_sts)
{
    /*������*/
    if (note_sts[0] == '1')
    {
        vtcp_exit( "�ý����ѹ�ʧ!");
        strcpy(g_pub_env.reply, "P126");
        goto ErrExit;
    }
    else if (note_sts[0] == '2')
    {
        vtcp_exit( "�ý����ѹ�ʧ��֤!");
        strcpy(g_pub_env.reply, "P108");
        goto ErrExit;
    }
    else if (note_sts[0] == '3')
    {
        vtcp_exit( "�ý�����������֤!");
        strcpy(g_pub_env.reply, "P110");
        goto ErrExit;
    }
    else if (note_sts[0] == '*')
    {
        vtcp_exit("�ý���������!");
        strcpy(g_pub_env.reply, "P109");
        goto ErrExit;
    }
    else if (note_sts[0] == '4')
    {
        vtcp_exit( "�ý����������ᣬ���������ʺ�!");
        strcpy(g_pub_env.reply, "P115");
        goto ErrExit;
    }
    else if(note_sts[0] != '0')
    {
        vtcp_exit( "����״̬������!");
        strcpy(g_pub_env.reply, "P107");
        goto ErrExit;
    }
    return 0;
ErrExit:
    return 1;
}
