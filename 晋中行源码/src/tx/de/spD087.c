/***************************************************************
* �� �� ��: spD087.c
* ����������ʵ����Ʋ�Ʒ�ͻ��������ǰ��ֹ
���ǵ�ѭ�������⣬ֱ���ڱ������ڼǿ�Ŀ����
* ��    ��: shangyongchao
* ������ڣ�2013/5/21
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "redempt_rate_c.h"
#include "fn_redempt_hst_c.h"
#include "fn_ac_rel_c.h"
#include "fn_br_profit_c.h"
#include "fn_br_quota_parm_c.h"
int spD087()
{
    int iRet = 0;
    long lDays = 0;/*����*/
    double dFct_rate = 0.000000;/*ʵ������*/
    double dFct_intst = 0.00;/*��������*/
    double dTmp_amt = 0.00;/*��ʱ���*/
    double dRtbj_amt = 0.00;/*���ι黹��ʵ�ʱ���*/
    double dProfit_amt = 0.00;/*���η�����*/
    double dProfit_rate = 0.000000;/*��������*/
    char cFd123[1000];
    char cTmp[21];
    char cBuy_date[9];
    char cBuy_trace_no[10];
    char cAgent_id_type[2];/*������֤������*/
    char cAgent_id[21];/*������֤������*/
    char cAgent_name[61];/*����������*/
    struct fn_reg_hst_c sFn_reg_hst;
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    struct fn_redempt_hst_c sFn_redempt_hst;
    struct redempt_rate_c sRedempt_rate;
    struct fn_ac_rel_c sFn_ac_rel;
    struct fn_br_profit_c sFn_br_profit;
    
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));

    pub_base_sysinit();

    get_zd_data("0690",cAgent_id_type);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0250",cAgent_name);
    get_zd_data("1230",cFd123);
    vtcp_log("123��[%s]",cFd123);
    for(int i = 1; ; i++)
    {
        lDays = 0;/*����*/
        dFct_rate = 0.000000;/*ʵ������*/
        dProfit_rate = 0.000000;/*��������*/
        dFct_intst = 0.00;/*��������*/
        dProfit_amt = 0.00;/*���η���*/
        dTmp_amt = 0.00;/*��ʱ���*/
        dRtbj_amt = 0.00;/*���ι黹��ʵ�ʱ���*/
        memset(cBuy_date,0x00,sizeof(cBuy_date));
        memset(cBuy_trace_no,0x00,sizeof(cBuy_trace_no));
        memset(cTmp,0x00,sizeof(cTmp));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_redempt_hst,0x00,sizeof(sFn_redempt_hst));
        memset(&sRedempt_rate,0x00,sizeof(sRedempt_rate));
        memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
        memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
        
        pub_base_cut_str( cFd123, cTmp, "|", i );
        vtcp_log("cTmp[%s] i[%d]",cTmp,i);
        if( strlen(cTmp) == 0 )
        {
            vtcp_log("����Ϊ��!");
            break;
        }
        strncpy(cBuy_date,cTmp,8);
        strncpy(cBuy_trace_no,cTmp+8,9);
        vtcp_log("cBuy_date[%s],cBuy_trace_no[%s]",cBuy_date,cBuy_trace_no);
        /*�������ں���ˮ�����Ϲ���¼ ��ʱ��ÿ��ȫ����ش���*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld",atoi(cBuy_date),atoi(cBuy_trace_no));
        if(iRet)
        {
            vtcp_log("���������α����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("ȡ�����α����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D335");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_log("�޴��Ϲ���¼ iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D336 ");
            goto ErrExit;
        }
        if(strcmp(g_pub_tx.tx_br_no,sFn_reg.tx_br_no) != 0)
        {
            vtcp_log("���Ǳ������Ϲ��Ĳ����ڱ��������tx_br_no[%s]",sFn_reg.tx_br_no);
            strcpy(g_pub_tx.reply,"P511");
            goto ErrExit;
        }
        if(sFn_reg.sts[0] != '1')
        {
            vtcp_log("��¼���ǿۿ�״̬������� sts[%s] ",sFn_reg.sts);
            strcpy(g_pub_tx.reply,"D337");
            goto ErrExit;
        }
        /*��ѯ��Ʒ�Ƿ������*/
        iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(iRet)
        {
            vtcp_log("û�������Ʋ�Ʒ! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
            strcpy(g_pub_tx.reply,"D104");
            goto ErrExit;
        }
        /*����Ӧ�ü�һ�������Ʒ����һ������˶��٣���ֹ����*/
        /*��֤��Ʒ״̬*/
        if(sFn_parm.stat[0] != '1')
        {
            vtcp_log("��Ʋ�Ʒ�����������е�״̬,������� stat[%s] ",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"D338");
            goto ErrExit;
        }
        /*��֤��Ʒ�Ƿ����*/
        if(sFn_parm.redempt[0] != 'Y')
        {
            vtcp_log("��Ʋ�Ʒ��������� redempt[%s] ",sFn_parm.redempt);
            strcpy(g_pub_tx.reply,"D339");
            goto ErrExit;
        }
        /*ȡ��Ʒ�������˺���Ϣ*/
        iRet = Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",sFn_reg.prdt_code);
        if(iRet)
        {
            vtcp_log("û�������Ʋ�Ʒ�������˺���Ϣ! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
            strcpy(g_pub_tx.reply,"D340");
            goto ErrExit;
        }
        /*��ص��ܺͲ��ܴ����Ϲ��ܶ�*/
        iRet = pub_base_CompDblVal(sFn_reg.redempt_amt + sFn_reg.tx_amt,sFn_reg.tx_amt);
        if(iRet > 0)
        {
            vtcp_log("����ܶ�[%lf]�����Ϲ��ܶ�[%lf]",sFn_reg.redempt_amt+sFn_reg.tx_amt,sFn_reg.tx_amt);
            strcpy(g_pub_tx.reply,"D341");
            goto ErrExit;
        }
        else if(iRet == 0)
        {
            strcpy(sFn_reg.sts,"2");/*�ĳ��ѷ�������*/
        }
        /*�����Ƿ��ǿ���ʽ�Ľ��д���*/
        if(sFn_parm.prdt_type[0] == '1')
        {
            /*���ʽ�Ĳ��������ʵ���������*/
        }
        else if(sFn_parm.prdt_type[0] == '2')
        {
            /*����ʽ�Ĵ��������ʵ������ֵ�δ�������� ������ı���ȡ����*/
            /*������Ҳ������ı���ͳ�Ƴ���*/
            if(pub_base_CompDblVal(sFn_reg.tx_amt,sFn_reg.tx_amt - sFn_reg.redempt_amt) == 0)
            {
                dFct_intst += sFn_reg.amt1;
                sFn_reg.amt1 =0.00;
                dProfit_amt += sFn_reg.amt2;
                sFn_reg.amt2 =0.00;
            }
            else
            {
                dTmp_amt = pub_base_PubDround_1((sFn_reg.tx_amt/(sFn_reg.tx_amt - sFn_reg.redempt_amt)) * sFn_reg.amt1);
                vtcp_log("��ر����Ӧ��δ��������[%lf]",dTmp_amt);
                dFct_intst += dTmp_amt;
                sFn_reg.amt1 -= dTmp_amt;/*�ѱ��εļ���*/
                if(pub_base_CompDblVal(sFn_reg.amt1,0.00) < 0)
                {
                    sFn_reg.amt1 = 0.00;
                }
                dTmp_amt = pub_base_PubDround_1((sFn_reg.tx_amt/(sFn_reg.tx_amt - sFn_reg.redempt_amt)) * sFn_reg.amt2);
                vtcp_log("��ر����Ӧ��δͳ�Ƶķ�����[%lf]",dTmp_amt);
                dProfit_amt += dTmp_amt;
                sFn_reg.amt2 -= dTmp_amt;/*�ѱ��εļ���*/
                if(pub_base_CompDblVal(sFn_reg.amt2,0.00) < 0)
                {
                    sFn_reg.amt2 = 0.00;
                }
            }
        }
        else
        {
            vtcp_log("��Ʒ���ʹ���[%s]",sFn_parm.prdt_type);
            strcpy(g_pub_tx.reply,"S092");
            goto ErrExit;
        }

        /*������������ȷ�� ���ر��� ȷ���ǲ��������� ȷ����Ϣ����*/
        /*Ŀǰ��ÿ��ȫ������㣬redempt_amt��ʵ����0*/
        if(sFn_parm.intst_type[0] == '1' || sFn_parm.intst_type[0] == '2')
        {
            lDays = 1 +  pub_base_CalTrueDays(sFn_reg.ic_date,g_pub_tx.tx_date);
            dRtbj_amt = sFn_reg.tx_amt  - sFn_reg.redempt_amt;
        }
        else
        {
            if(sFn_parm.capital_percent - 100.000000 > 0.0000005)
            {               
                vtcp_log("��������������ڰٷ�֮100 ��������! sFn_parm.capital_percent=[%lf]",sFn_parm.capital_percent);
                strcpy(g_pub_tx.reply,"P101");
                goto ErrExit; 
            }
            else if(sFn_parm.capital_percent - 100.000000 < 0.0000005)
            {
                /*�����ղ������� �Ͳ����������� �������ó�0 ��û������*/
                lDays = 0;
                dRtbj_amt = pub_base_PubDround_1((sFn_reg.tx_amt - sFn_reg.redempt_amt) * sFn_parm.capital_percent/100);
            }
            else /*������ȫ������ �ٿ����ǲ��� ������*/
            {
                lDays = 1 +  pub_base_CalTrueDays(sFn_reg.ic_date,g_pub_tx.tx_date);
                dRtbj_amt = sFn_reg.tx_amt  - sFn_reg.redempt_amt;
            }
        }
        sFn_reg.return_capital += dRtbj_amt;
        vtcp_log("ȷ�����ر���[%lf] �ۼƷ��ر���[%lf]",dRtbj_amt,sFn_reg.return_capital);
        sFn_reg.redempt_amt += sFn_reg.tx_amt;/*�ۼ���ؽ��*/
        /*����ط��� ��ʱ����ر���*��ط�����*/
        sFn_redempt_hst.redempt_chrg_amt =  sFn_reg.tx_amt * sFn_parm.redempt_chrg / 100;
        sFn_redempt_hst.redempt_chrg_amt = pub_base_PubDround_1(sFn_redempt_hst.redempt_chrg_amt);
        /*ȡʵ����������� ���������������ʡ��������������� */
        iRet = get_fct_rate("001",sFn_reg.prdt_code,lDays,&sFn_reg.return_rate,&dProfit_rate);
        if(iRet)
        {
            vtcp_log("��ȡ��������ʴ���");
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        /*���������ﶼ���������� �����ʵ������������*/
        dTmp_amt = dRtbj_amt * lDays * sFn_reg.return_rate * (1+ sFn_reg.flt_radio)/L360/L100;
        dFct_intst = pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_intst += dFct_intst;
        vtcp_log("tmp_amt[%lf] dFct_intst[%lf] fct_intst[%lf]",dTmp_amt,dFct_intst,sFn_reg.fct_intst);

        dTmp_amt= dRtbj_amt * lDays * dProfit_rate/L360/L100;
        dProfit_amt = pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_profit_amt += dProfit_amt;
        vtcp_log("tmp_amt[%lf] dProfit_amt[%lf] fct_profit_amt[%lf]",dTmp_amt,dProfit_amt,sFn_reg.fct_profit_amt);

        /*����Ʊ����˻�  �ֻ�-*/
        if(pub_base_CompDblVal(dRtbj_amt,0.00) > 0)
        {
            if(sFn_reg.cif_type[0] == '1')
            {
                strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
            }
            else
            {
                strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
            }
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn = 0;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "9");
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 9001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"��Ʒ�������");
            g_pub_tx.tx_amt1 = dRtbj_amt;
            vtcp_log("����[%f]",g_pub_tx.tx_amt1);
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("����Ʊ����˺Ŵ���!!");
                goto ErrExit;
            }
            /*����Ʊ����˻� ��Ŀ*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC01") )
            {
                vtcp_log("�Ǽǻ����ˮ����!");
                goto ErrExit;
            }
        }
        if(pub_base_CompDblVal(dFct_intst,0.00) > 0)
        {
            /*����������˻� �ֻ�-*/
            strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn = 0;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "9");
            g_pub_tx.add_ind[0]='0';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 9001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"����������");
            g_pub_tx.tx_amt1 = dFct_intst;
            vtcp_log("����[%f]",g_pub_tx.tx_amt1);
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("��������������˺Ŵ���!!");
                goto ErrExit;
            }
            /*����������˻� ��Ŀ*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC01") )
            {
                vtcp_log("�Ǽǻ����ˮ����!");
                goto ErrExit;
            }
        }
        if(pub_base_CompDblVal(sFn_redempt_hst.redempt_chrg_amt,0.00) > 0)
        {
            /*������������ط������� �ֻ�+*/
            strcpy(g_pub_tx.ac_no,sFn_ac_rel.sy_ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = 0;
            g_pub_tx.ac_seqn = 0;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "9");
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 9001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"����������");
            g_pub_tx.tx_amt1 = sFn_redempt_hst.redempt_chrg_amt;
            vtcp_log("����[%f]",sFn_redempt_hst.redempt_chrg_amt);
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("��������������˺Ŵ���!!");
                goto ErrExit;
            }
            /*������������ط������� ��Ŀ*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC02") )
            {
                vtcp_log("�Ǽǻ����ˮ����!");
                goto ErrExit;
            }
        }

        g_pub_tx.tx_amt1 = dRtbj_amt + dFct_intst - sFn_redempt_hst.redempt_chrg_amt;
        vtcp_log("�������[%f]",g_pub_tx.tx_amt1);
        if(pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00) > 0)
        {
            /*�ǿͻ��˵ı�������� �ֻ�+*/
            strcpy(g_pub_tx.ac_no,sFn_reg.ac_no);
            strcpy(g_pub_tx.name,"");
            g_pub_tx.ac_id = sFn_reg.ac_id;
            g_pub_tx.ac_seqn = sFn_reg.ac_seqn;
            strcpy(g_pub_tx.ac_wrk_ind,"00000");
            strcpy(g_pub_tx.ac_get_ind, "00");
            strcpy(g_pub_tx.ac_id_type, "1");
            g_pub_tx.add_ind[0]='1';
            strcpy(g_pub_tx.hst_ind,"1");
            strcpy(g_pub_tx.ct_ind,"2");
            g_pub_tx.svc_ind = 1001;
            strcpy(g_pub_tx.note_type,"");
            strcpy(g_pub_tx.beg_note_no,"");
            strcpy(g_pub_tx.brf,"����������");
            iRet = pub_acct_trance();
            if (iRet)
            {
                vtcp_log("��������������˺Ŵ���!!");
                goto ErrExit;
            }
            /*�ǿͻ��˵ı�������� ��Ŀ*/
            set_zd_data("102K",g_pub_tx.ct_ind);
            set_zd_double("102I",g_pub_tx.tx_amt1);
            if ( pubf_acct_proc("LC02") )
            {
                vtcp_log("�Ǽǻ����ˮ����!");
                goto ErrExit;
            }
        }
        sFn_reg.end_date = g_pub_tx.tx_date;
        strcpy(sFn_reg.profit_flag,"Y");
        sFn_reg.return_date  = g_pub_tx.tx_date;
        sFn_reg.return_trace_no  = g_pub_tx.trace_no;
        strcpy(sFn_reg.filler,"�ͻ���ǰ���");
        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("����fn_reg����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333");
            goto ErrExit;
        }
        /*�Ǽ������ϸ*/
        sFn_redempt_hst.tx_date = g_pub_tx.tx_date;
        sFn_redempt_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_redempt_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_redempt_hst.pact_no,sFn_reg.pact_no);
        strcpy(sFn_redempt_hst.prdt_code,sFn_reg.prdt_code);
        strcpy(sFn_redempt_hst.ac_no,sFn_reg.ac_no);
        sFn_redempt_hst.ac_id  = sFn_reg.ac_id;
        sFn_redempt_hst.ac_seqn  = sFn_reg.ac_seqn;
        /*strcpy(sFn_redempt_hst.chnl_type,g_pub_rule.chnl_type);*/
        sFn_redempt_hst.redempt_amt = sFn_reg.tx_amt;
        sFn_redempt_hst.return_rate = sFn_reg.return_rate;
        sFn_redempt_hst.ic_date = sFn_reg.ic_date;
        sFn_redempt_hst.fct_intst = sFn_reg.fct_intst;
        sFn_redempt_hst.return_capital = dRtbj_amt;
        sFn_redempt_hst.profit_amt = dProfit_amt;
        strcpy(sFn_redempt_hst.profit_flag,"Y");
        strcpy(sFn_redempt_hst.tel,g_pub_tx.tel);
        sFn_redempt_hst.return_date = g_pub_tx.tx_date;
        sFn_redempt_hst.return_trace_no = g_pub_tx.trace_no;
        strcpy(sFn_redempt_hst.sts,"1");
        sFn_redempt_hst.buy_date = sFn_reg.tx_date;
        sFn_redempt_hst.buy_trace_no = sFn_reg.trace_no;
        iRet = Fn_redempt_hst_Ins(sFn_redempt_hst,g_pub_tx.reply);
        if(iRet)
        {
            Fn_redempt_hst_Debug(&sFn_redempt_hst);
            vtcp_log("�Ǽ������ϸ����");
            strcpy(g_pub_tx.reply,"D342");
            goto ErrExit;
        }
        /*�Ǽǻ�������*/
        iRet = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",sFn_reg.prdt_code,g_pub_tx.tx_br_no);
        if(iRet)
        {
            vtcp_log("������»��������ʧ��");
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
        if(iRet  == 100)
        {
            vtcp_info("û�л����ķ�����Ϣ���Ǽ�һ��");
            strcpy(sFn_br_profit.prdt_code,sFn_reg.prdt_code);
            strcpy(sFn_br_profit.br_no,g_pub_tx.tx_br_no);
            sFn_br_profit.tx_amt = sFn_reg.tx_amt;
            sFn_br_profit.redempt_amt = sFn_reg.tx_amt;
            sFn_br_profit.profit_amt = dProfit_amt;
            sFn_br_profit.reg_date = g_pub_tx.tx_date;
            sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
            strcpy(sFn_br_profit.sts,"0");
            strcpy(sFn_br_profit.brf,"��صǼ�");
            iRet = Fn_br_profit_Ins(sFn_br_profit,g_pub_tx.reply);
            if(iRet)
            {
                Fn_br_profit_Debug(&sFn_br_profit);
                vtcp_log("�Ǽǻ��������ʧ��");
                strcpy(g_pub_tx.reply,"D343");
                goto ErrExit;
            }
        }
        else if(iRet == 0)
        {
            vtcp_info("�з�����Ϣ ����");
            sFn_br_profit.tx_amt += sFn_reg.tx_amt;
            sFn_br_profit.redempt_amt += sFn_reg.tx_amt;
            sFn_br_profit.profit_amt += dProfit_amt;
            iRet = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("���»��������ʧ��");
                strcpy(g_pub_tx.reply,"D344");
                goto ErrExit;
            }
        }
        Fn_br_profit_Clo_Upd();
        /*�Ǽǲ�����¼*/
        sFn_reg_hst.tx_date = g_pub_tx.tx_date;
        sFn_reg_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
        strcpy(sFn_reg_hst.type,"6");
        sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
        strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
        strcpy(sFn_reg_hst.agent_id,cAgent_id);
        strcpy(sFn_reg_hst.agent_name,cAgent_name);
        sFn_reg_hst.buy_date=sFn_reg.tx_date;
        sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
        strcpy(sFn_reg_hst.brf,"�ͻ����");
        iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
        if(iRet)
        {
            Fn_reg_hst_Debug(&sFn_reg_hst);
            vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D345");
            goto ErrExit;
        }
        Fn_reg_Clo_Upd();
        /*���ı�������� ���ʱ�ѱ����������ӻ�ȥ*/
        iRet = upd_br_quota(sFn_parm,sFn_reg.tx_amt,1);
        if(iRet)
        {
            vtcp_log("���»���������[%d]",iRet);
            goto ErrExit;
        }
        /*���ط���Ϣ���� Ӧ��д����ֵ  �Ȱ�������¼�Ĳ��԰�*/
        set_zd_double("0390",sFn_reg.tx_amt);
        set_zd_double("0400",sFn_reg.fct_intst);
        set_zd_double("0840",sFn_reg.return_rate);
        set_zd_double("1001",dRtbj_amt);
        set_zd_double("0410",sFn_redempt_hst.redempt_chrg_amt);
        set_zd_double("0420",dRtbj_amt + sFn_reg.fct_intst - sFn_redempt_hst.redempt_chrg_amt);
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/*���ݲ�Ʒ ���� ȡ�������*/
int get_fct_rate(char *rate_code,char * prdt_code,long days, double *fct_rate,double *profit_rate)
{
    int iRet = 0;
    struct redempt_rate_c sRedempt_rate;

    memset(&sRedempt_rate,0x00,sizeof(sRedempt_rate));

    iRet = Redempt_rate_Sel(g_pub_tx.reply,&sRedempt_rate,"prdt_code='%s' and rate_code='%s' and beg_date<=%ld and end_date>=%ld and min_days<=%ld and max_days>=%ld",\
            prdt_code,rate_code,g_pub_tx.tx_date,g_pub_tx.tx_date,days,days);
    if(iRet == 100)
    {
        vtcp_log("û�и���Ʋ�Ʒ�ķֶ�������Ϣ");
        strcpy(g_pub_tx.reply,"D346");
        goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ�ķֶ�������Ϣ����");
        strcpy(g_pub_tx.reply,"D347");
        goto ErrExit;
    }
    *fct_rate = sRedempt_rate.redempt_rate;
    *profit_rate = sRedempt_rate.profit_rate;
    vtcp_log("ȡ����ʵ������[%lf] ��������[%lf]",*fct_rate,*profit_rate);
OkExit:
    vtcp_log("ȡ�ֶ����ʳɹ�");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("ȡ�ֶ�����ʧ��");
    return 1;
}
/*���»������*/
int upd_br_quota(struct fn_parm_c sfn_parm,double tx_amt,int mode)
{
    int iRet  = 0;
    struct fn_br_quota_parm_c sFn_br_qp;
    memset(&sFn_br_qp        ,0x00,sizeof(sFn_br_qp));
    if(sfn_parm.quota_flag[0] == 'Y')
    {
        /*��鱾������Ʋ�Ʒ�Ƿ����Ϲ�*/
        iRet = Fn_br_quota_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",\
            sfn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(iRet)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        iRet = Fn_br_quota_parm_Fet_Upd(&sFn_br_qp,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_info("û���޶����");
        }
        else
        {
            if(mode == 0)
            {
                sFn_br_qp.available_amt -= tx_amt;
            }
            else if(mode == 1)
            {
                sFn_br_qp.available_amt += tx_amt;
            }
            else
            {
                strcpy(g_pub_tx.reply,"S035");
                goto ErrExit;
            }
            iRet = Fn_br_quota_parm_Upd_Upd(sFn_br_qp,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("������Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D348");
                goto ErrExit;
            }
        }
        Fn_br_quota_parm_Clo_Upd();
    }
    else/*û������*/
    {
        
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("�������ɹ� ");
    return 0;
ErrExit:
    vtcp_log("�������ʧ��");
    return 1;
}
