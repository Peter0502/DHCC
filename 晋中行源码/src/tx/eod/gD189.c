/*************************************************************
* �� �� ��: gD189.c
* ��������: ������Ƶ����� �黹�յ���������������ִ�У�����ǰ��
�����Ҫ���պ������ ���Ըĳɷ��̺�ִ��

* ��    ��: shangyongchao
* �������: 2013/5/23

* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"

#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "fn_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "mdm_ac_rel_c.h"
#include "redempt_rate_c.h"
#include "fn_br_profit_c.h"
#define LINE_PAGE 40
#define HD_LINE 3
int gD189()
{
    int ret,iPage,iFlag,iHdCnt,iIntst_flag,iPage_qs;
    long lLine,lLine_qs,lCnt_qs,lDays;
    char cFilename[256],cFilename1[256],cCmd[512],cAc_no[25],cName[61];;
    char cOld_br_no[6];
    double dProfit_rate = 0.000000; /*��������*/
    double dRbj_amt = 0.00;/*�黹����*/
    double dFct_rate=0.00000000;/*ʵ��������*/
    double dProfit_amt = 0.00;/*������*/
    double dFct_intst = 0.00;/*����ʵ������*/
    double dTmp_amt = 0.00;
    FILE * fp=NULL;/*д�����Ϲ���������ϸ*/
    FILE * fp1=NULL;/*д�����Ϲ������Ŀͻ��ص�*/
    FILE *fp_qs =NULL;/*д�������ĵĸ��������Ļ���*/
    /*����һ�� ͳ�������õĽṹ��*/
    struct total_info
    {
        long cnt;/*����*/
        double tx_amt;/*�Ϲ����*/
        double redempt_amt;/*��ؽ��*/
        double return_capital1;/*���η��صı���*/
        double fct_intst1;/*����ʵ������*/
        double profit_amt1;/*���η�����*/
        double trust_amt;/*�йܷѽ��*/
        double return_capital2;/*�ۼƷ��صı���*/
        double fct_intst2;/*�ۼ�ʵ������*/
        double profit_amt2;/*�ۼƷ�����*/
    }sTotal_qs,sTotal_jg;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg,sFn_reg_tmp;
    struct fn_ac_rel_c sFn_ac_rel;
    struct fn_reg_hst_c sFn_reg_hst;
    struct in_mst_c sIn_mst;
    struct com_branch_c sCom_branch;
    struct com_sys_parm_c sCom_sys_parm;
    struct fn_br_profit_c sFn_br_profit;
    
    ret=iPage=iFlag=iHdCnt=iIntst_flag=iPage_qs=0;
    lCnt_qs=lLine_qs=lLine = lDays = 0;
    
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cFilename1,0x00,sizeof(cFilename1));
    memset(cCmd,0x00,sizeof(cCmd));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cOld_br_no,0x00,sizeof(cOld_br_no));
    memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
    memset(&sTotal_qs,0x00,sizeof(sTotal_qs));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_tmp,0x00,sizeof(sFn_reg_tmp));
    memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sCom_branch,0x00,sizeof(sCom_branch));
    memset(&sCom_sys_parm,0x00,sizeof(sCom_sys_parm));
    memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
    
    pub_base_sysinit();
    
    strcpy(g_pub_tx.tx_code,"G189");
    strcpy(g_pub_tx.sub_tx_code,"G189");
    set_zd_data("096C","0100");
    /*set_zd_data("096M",YW_TYPE_ZC);*/
    
    /* ɾ���ļ� */
    strcpy(cFilename,"lcsy");    /* ������� */
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    strcpy(cFilename1,"lcsyhd");    /* �������ص� */
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename1);
    vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    ret=pub_rpt_append_openfile(&fp_qs,QS_BR_NO,cFilename);
    if(ret)
    {
        vtcp_log("%s,%d,���ļ�ʧ��!");
        strcpy(g_pub_tx.reply,"S066");
        goto ErrExit;
    }

    ret = sql_begin();
    if(ret)
    {
        vtcp_log("�����ݿ���� [%d]",ret);
        goto ErrExit;
    }
    /*���黹�������ڴ���*/
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"stat='1' and return_date <=%ld",g_pub_tx.tx_date);
    if(ret)
    {
        vtcp_log("����fn_parm�α�ʧ��! ret=[%d]",ret);
        strcpy(g_pub_tx.reply,"D325");
        goto ErrExit;
    }
    while(1)
    {
        iHdCnt = 0;
        lLine_qs = 0;
        lLine = 0;
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
        memset(&sTotal_qs,0x00,sizeof(sTotal_qs));
        ret=Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("   fn_parm fetch over! ");
            break;
        }
        else if( ret )
        {
            vtcp_log("  fn_parm fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        fprintf(fp_qs,"\n\n\n");
        fprintf(fp_qs, "                                           ��Ʋ�Ʒ[%s]�黹����ͳ��\n",sFn_parm.prdt_code);
        fprintf(fp_qs, "                                                                                                   ����:%d��%d��%d��\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
        fprintf(fp_qs, "==============================================================================================================================\n" );
        fprintf(fp_qs, "%-6s %-20s%-8s%-17s%-17s%-17s%-17s%-17s\n","������","��Ʋ�Ʒ����","�ܱ���","��ƽ��","��ؽ��","��������","ʵ������","������");
        fprintf(fp_qs, "==============================================================================================================================\n" );
    
        vtcp_log("return_date[%ld] ",sFn_parm.return_date);
        /* ȡ������ˮ�� */
        if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
        {
            vtcp_log("ȡ������ˮ�Ŵ� [%d]", g_pub_tx.trace_no);
            goto ErrExit;
        }

        set_zd_long(DC_TRACE_NO,g_pub_tx.trace_no);
        g_pub_tx.trace_cnt = 0;
        set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);
        strcpy(cOld_br_no,"-----");
    
        ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",sFn_parm.prdt_code);
        if(ret)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ�����˺����ñ����");
            strcpy(g_pub_tx.reply,"S035");
            goto ErrExit;
        }
        /*���Ϲ���ͬ����,����Ҫ���Ϲ�����������*/
        ret=Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts='1' order by tx_br_no,ac_id",sFn_parm.prdt_code);
        if( ret )
        {
            vtcp_log("declare cursor error! ret[%d] ",ret);
            strcpy( g_pub_tx.reply,"D332" );
            goto ErrExit;
        }
        while(1)
        {
            lDays = 0;
            dFct_rate = 0.000000;
            dProfit_rate = 0.000000; /*��������*/
            iIntst_flag = 0;
            dFct_intst = 0.00;
            dTmp_amt = 0.00;
            memset(cAc_no,0x00,sizeof(cAc_no));
            memset(cName,0x00,sizeof(cName));
            memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
            memset(&g_in_mst,0x00,sizeof(g_in_mst));
            memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
            memset(&sFn_reg,0x00,sizeof(struct fn_reg_c));
            memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
            ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            if( ret==100 )
            {
                vtcp_log("fn_reg fetch over! ");
                if( lLine>0 )/*����һ�����������Ʒ��β*/
                {
                    iPage++;
                    fprintf( fp, "=======================================================================================================================================\n" );
                    fprintf( fp, "�ϼ�:%8ld��     %-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf               ��%ldҳ\n",\
                        sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,sTotal_jg.return_capital1,\
                        sTotal_jg.fct_intst1,sTotal_jg.profit_amt1,iPage);
                    fprintf(fp,"%c",0x0C);
                    /*����ϸ������м�¼��ͳ�Ƶ���������*/
                    sTotal_qs.cnt += sTotal_jg.cnt;
                    sTotal_qs.tx_amt+=sTotal_jg.tx_amt;
                    sTotal_qs.redempt_amt+= sTotal_jg.redempt_amt;
                    sTotal_qs.return_capital1+= sTotal_jg.return_capital1;
                    sTotal_qs.fct_intst1 += sTotal_jg.fct_intst1;
                    sTotal_qs.profit_amt1+= sTotal_jg.profit_amt1;
                    /*дһ�����������ͳ����Ϣ*/
                    fprintf(fp_qs,"%-6s %-20s%-8ld%-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                        cOld_br_no,sFn_parm.prdt_code,sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,\
                        sTotal_jg.return_capital1,sTotal_jg.fct_intst1,sTotal_jg.profit_amt1);
                    lLine_qs ++;
                    if(lLine_qs % LINE_PAGE == 0)
                    {
                        iPage_qs++;
                        fprintf( fp_qs, "==============================================================================================================================\n" );
                        fprintf( fp_qs, "                                                                                                                     ��%ldҳ\n",iPage_qs);
                        fprintf(fp_qs,"%c",0x0C);
                    }
                }
                if(lLine_qs > 0)/*���������ļ�д �ϼ�β*/
                {
                    iPage_qs++;
                    fprintf( fp_qs, "==============================================================================================================================\n" );
                    fprintf(fp_qs,  "          �ϼ�:            %-8ld%-17.2f%-17.2lf%-17.2lf%-17.2lf%-17.2lf��%ldҳ\n",\
                            sTotal_qs.cnt,sTotal_qs.tx_amt,sTotal_qs.redempt_amt,sTotal_qs.return_capital1,\
                            sTotal_qs.fct_intst1,sTotal_qs.profit_amt1,iPage_qs);
                    fprintf(fp_qs,"%c",0x0C);
                    
                }
                if(iHdCnt > 0)/*�����Ʒ���� �ص��ļ���ҳ*/
                {
                    fprintf(fp1,"\f");
                }
                break;
            }
            else if( ret )
            {
                vtcp_log("fn_reg fetch error! ret[%d] ",ret);
                strcpy(g_pub_tx.reply,"D330");
                goto ErrExit;
            }
            /*�������� �ַ��ʽ�Ϳ���ʽ��ȡ��ͬ���� ȷ����Ϣ����*/
            if(sFn_parm.prdt_type[0] == '1')
            {
                /*���ʽ����ֹ������ͬ��������ͬ������ ��ͬ�ķ�������*/
                /*���ڴ�����ǰ��ֹ�����������������������¼���*/
                dProfit_rate = sFn_parm.profit_rate;
                dFct_rate = sFn_parm.fct_rate;
                lDays = pub_base_CalTrueDays(sFn_reg.ic_date,sFn_reg.end_date);
                vtcp_log("����[%lf],����[%ld]",dFct_rate,lDays);
            }
            else if(sFn_parm.prdt_type[0] == '2')
            {
                /*����ʽ����ʼ���ڲ�ͬ�����÷ֶ����� ͬʱ����δ���������(����������ʼ������δ���˵�)*/
                lDays = pub_base_CalTrueDays(sFn_reg.ic_date,sFn_reg.end_date);
                ret =  get_fct_rate("002",sFn_parm.prdt_code,lDays,&dFct_rate,&dProfit_rate);
                if(ret)
                {
                    vtcp_log("ȡ�ֶ����ʴ���");
                    goto ErrExit;
                }
                vtcp_log("����[%lf],����[%ld]",dFct_rate,lDays);
                /*���ڵ���������������ɵ�δ��������*/
                dFct_intst += sFn_reg.amt1;
                dProfit_amt += sFn_reg.amt2;
                /*
                vtcp_log("���������� prdt_type[%s]",sFn_parm.prdt_type);
                strcpy(g_pub_tx.reply,"S035");
                goto ErrExit;
                */
            }
            else
            {
                vtcp_log("���������� prdt_type[%s]",sFn_parm.prdt_type);
                strcpy(g_pub_tx.reply,"S035");
                goto ErrExit;
            }
            /*ȷ���黹����*/
            /*������ �Ϲ� -���*/
            if(sFn_parm.intst_type[0] == '1' || sFn_parm.intst_type[0] == '2')
            {
                dRbj_amt=sFn_reg.tx_amt - sFn_reg.redempt_amt;
            }
            else
            {
                /*��������  ��������ղ����� ��Ҫ���������ʵ����������0 Ϊ��֤�������� �������� 0*/
                if(sFn_parm.capital_percent  - 100.000000 > 0.0000005)
                {
                    vtcp_log("��������������ڰٷ�֮100 ��������! sFn_parm.capital_percent=[%lf]",sFn_parm.capital_percent);
                    strcpy(g_pub_tx.reply,"P370");
                    goto ErrExit;
                } 
                else if(sFn_parm.capital_percent - 100.000000 < 0.0000005 && sFn_parm.capital_percent!=100.0) /*add by lwb*/
                {
                    lDays = 0;/*��ʧ���� ���������� ������0*/
                    dRbj_amt=pub_base_PubDround_1((sFn_reg.tx_amt - sFn_reg.redempt_amt) * sFn_parm.capital_percent/100);
                }
                else
                {
                    dRbj_amt=sFn_reg.tx_amt - sFn_reg.redempt_amt;
                }
            }
            vtcp_log("ȷ���� ����[%lf],����[%ld],����[%lf]",dFct_rate,lDays,dRbj_amt);
            sFn_reg.return_capital += dRbj_amt;/*��ص�����ǲ������ �Ѿ���һ����ֵ�� �ۼ���*/
            /*�н��ż���*/
            if(pub_base_CompDblVal(dRbj_amt,0.00) > 0)
            {
                 /*�跽��20110199���������� */
                strcpy(g_pub_tx.tx_br_no,QS_BR_NO);/*�������Ĺ黹���ͻ�*/
                strcpy(g_pub_tx.name,"");
                if( sFn_reg.cif_type[0]=='1' )
                {
                    strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
                }
                else if( sFn_reg.cif_type[0]=='2' )
                {
                    strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
                }
                /*�ǿ�Ŀ�Ļ� ��������Ϊ��������*/
                if(strlen(g_pub_tx.ac_no) < 10)
                {
                    strcpy(g_pub_tx.opn_br_no,QS_BR_NO);
                }
                g_pub_tx.cif_no = 0;
                g_pub_tx.ac_id = 0;
                g_pub_tx.ac_seqn=0;
                g_pub_tx.tx_amt1=dRbj_amt;
                g_pub_tx.add_ind[0]='0';
                strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                g_pub_tx.svc_ind=1001;
                strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
                strcpy(g_pub_tx.ac_id_type,"9");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                strcpy(g_pub_tx.brf,"��Ʒ�������");
                vtcp_log(" ac_id[%ld] tx_amt[%lf] �裺�������(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
                if ( pub_acct_trance() )
                {
                    vtcp_log("���ô�ȡ������ʧ��!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("102I",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC01") )
                {
                    vtcp_log("�Ǽǻ����ˮ����!");
                    goto ErrExit;
                }
                /* �������ͻ��˻�(����) */
                ret = my_get_ac_no(sFn_reg.ac_no,cAc_no,cName);
                if(ret)
                {
                    vtcp_log("û�к��ʵ����˵��˺�![%s]",sFn_reg.ac_no);
                    goto ErrExit;
                }
                strcpy(g_pub_tx.ac_no,cAc_no);
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no = 0;
                g_pub_tx.ac_id=sFn_reg.ac_id;
                g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
                g_pub_tx.tx_amt1=dRbj_amt;
                g_pub_tx.add_ind[0]='1';
                strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                g_pub_tx.svc_ind=1001;
                strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
                strcpy(g_pub_tx.ac_id_type,"1");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                strcpy(g_pub_tx.brf,"��Ʒ�������");
                vtcp_log("ac_id[%ld] tx_amt[%lf] �����ͻ��˻�(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
                if ( pub_acct_trance() )
                {
                    vtcp_log("���ô�ȡ������ʧ��!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("102I",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC02") )
                {
                    vtcp_log("�Ǽǻ����ˮ����!");
                    goto ErrExit;
                }
            }
            /*�������� dFct_intst���Ǻ��ڼ��Ͽ���ʽ�ĵ������ʵ�δ��������*/
            dTmp_amt = dRbj_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L360/L100;
            dFct_intst += pub_base_PubDround_1(dTmp_amt);
            sFn_reg.fct_intst += dFct_intst;
            vtcp_log("���μ����������[%lf]������ʵ������[%lf] Ԥ��[%lf]",dTmp_amt,dFct_intst,sFn_reg.intst);
            /*������Ҳ������*/
            dTmp_amt = 0.00;
            dTmp_amt = dRbj_amt * lDays * sFn_parm.profit_rate /L360/L100;
            dProfit_amt += pub_base_PubDround_1(dTmp_amt);
            sFn_reg.fct_profit_amt += dProfit_amt;/*���ǵ����ܴ��ڲ�����ص� �����ۼ���*/
            vtcp_log("����ķ���[%lf] �ܵķ���[%lf]",dProfit_amt,sFn_reg.fct_profit_amt);
            if(pub_base_CompDblVal(dFct_intst,0.00) > 0)
            {
                /* �跽��224199����Ӧ����-����ʽ����㻧(����) */
                g_pub_tx.tx_amt1=dFct_intst;
                strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no=0;
                g_pub_tx.ac_id=0;
                g_pub_tx.ac_seqn=0;
                g_pub_tx.add_ind[0]='0';
                strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                g_pub_tx.svc_ind=1001;
                strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
                strcpy(g_pub_tx.ac_id_type,"9");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                strcpy(g_pub_tx.brf,"��Ʒ�������");
                vtcp_log("ac_id[%ld] tx_amt[%lf] �裺����Ӧ����-����ʽ����㻧(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
                if ( pub_acct_trance() )
                {
                    vtcp_log("���ô�ȡ������ʧ��!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("102I",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC01") )
                {
                    vtcp_log("�Ǽǻ����ˮ����!");
                    goto ErrExit;
                }
                /* �������ͻ��˻�(����) */
                g_pub_tx.tx_amt1=dFct_intst;
                strcpy(g_pub_tx.ac_no,cAc_no);/*ֱ��ȡ������˺ž�����*/
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no = 0;
                g_pub_tx.ac_id=sFn_reg.ac_id;
                g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
                g_pub_tx.add_ind[0]='1';
                strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                g_pub_tx.svc_ind=1001;
                strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
                strcpy(g_pub_tx.ac_id_type,"1");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                strcpy(g_pub_tx.brf,"��Ʒ�������");
                vtcp_log("ac_id[%ld] tx_amt[%lf] �����ͻ��˻�(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
                if ( pub_acct_trance() )
                {
                    vtcp_log("���ô�ȡ������ʧ��!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("102I",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC02") )
                {
                    vtcp_log("�Ǽǻ����ˮ����!");
                    goto ErrExit;
                }
            }
            /*�Ǽǻ�������*/
            ret = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",sFn_reg.prdt_code,sFn_reg.tx_br_no);
            if(ret)
            {
                vtcp_log("������»��������ʧ��");
                strcpy(g_pub_tx.reply,"D346");
                goto ErrExit;
            }
            ret = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
            if(ret  == 100)
            {
                /*û�м�¼ ˵��û����ع� ��һ�δ���*/
                vtcp_log("û�л����ķ�����Ϣ���Ǽ�һ��");
                strcpy(sFn_br_profit.prdt_code,sFn_reg.prdt_code);
                strcpy(sFn_br_profit.br_no,sFn_reg.tx_br_no);
                sFn_br_profit.tx_amt = sFn_reg.tx_amt;
                sFn_br_profit.redempt_amt = sFn_reg.redempt_amt;
                sFn_br_profit.profit_amt = sFn_reg.fct_profit_amt;
                sFn_br_profit.reg_date = g_pub_tx.tx_date;
                sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
                strcpy(sFn_br_profit.sts,"0");
                strcpy(sFn_br_profit.brf,"���ڷ���");
                ret = Fn_br_profit_Ins(sFn_br_profit,g_pub_tx.reply);
                if(ret)
                {
                    Fn_br_profit_Debug(&sFn_br_profit);
                    vtcp_log("�Ǽǻ��������ʧ��[%d]",ret);
                    strcpy(g_pub_tx.reply,"D348");
                    goto ErrExit;
                }
            }
            else if(ret == 0)
            {
                vtcp_log("�з�����Ϣ ����");
                /*Ϊ��֧�ֲ�����أ�����ӱ��εı���(����ʵ�ʹ黹��)*/
                sFn_br_profit.tx_amt += sFn_reg.tx_amt - sFn_reg.redempt_amt;
                /*��ؽ��ö�����Ϊ��ؽ��׻�Ǽ� ���ڷ������治�ᴦ����ؽ��*/
                sFn_br_profit.profit_amt += dProfit_amt;
                ret = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
                if(ret)
                {
                    vtcp_log("���»��������ʧ��");
                    strcpy(g_pub_tx.reply,"D346");
                    goto ErrExit;
                }
            }
            Fn_br_profit_Clo_Upd();
            /*�Ǽǲ�����¼*/
            sFn_reg_hst.tx_date = g_pub_tx.tx_date;
            sFn_reg_hst.trace_no = g_pub_tx.trace_no;
            strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
            strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
            strcpy(sFn_reg_hst.type,"2");
            sFn_reg_hst.tx_amt = dRbj_amt + dFct_intst;/*��¼��ʵ�ʹ黹�ı��������*/
            sFn_reg_hst.buy_date=sFn_reg.tx_date;
            sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
            strcpy(sFn_reg_hst.brf,"��������");
            ret = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
            if(ret)
            {
                Fn_reg_hst_Debug(&sFn_reg_hst);
                vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",ret);
                strcpy(g_pub_tx.reply,"D335");
                goto ErrExit;
            }
        
            sFn_reg.profit_flag[0] ='Y';
            sFn_reg.return_date=g_pub_tx.tx_date;
            sFn_reg.return_trace_no=g_pub_tx.trace_no;
            sFn_reg.sts[0]='2';
            /*sFn_reg.return_rate = dFct_rate;*/
            ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("update fn_reg_hst error! ret[%d]  ",ret);
                strcpy(g_pub_tx.reply,"D332");
                goto ErrExit;
            }
            /* ���ݺ�ͬ���ɱ����嵥 */
            if( strcmp(sFn_reg.tx_br_no,cOld_br_no) )
            {
                if( lLine>0 )/*����һ�����������Ʒ��β*/
                {
                    iPage++;
                    fprintf( fp, "=======================================================================================================================================\n" );
                    fprintf( fp, "�ϼ�:%8ld��     %-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf              ��%ldҳ\n",\
                        sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,sTotal_jg.return_capital1,\
                        sTotal_jg.fct_intst1,sTotal_jg.profit_amt1,iPage);
                    fprintf(fp,"%c",0x0C);
                    /*����ϸ������м�¼��ͳ�Ƶ���������*/
                    sTotal_qs.cnt += sTotal_jg.cnt;
                    sTotal_qs.tx_amt+=sTotal_jg.tx_amt;
                    sTotal_qs.redempt_amt+= sTotal_jg.redempt_amt;
                    sTotal_qs.return_capital1+= sTotal_jg.return_capital1;
                    sTotal_qs.fct_intst1 += sTotal_jg.fct_intst1;
                    sTotal_qs.profit_amt1+= sTotal_jg.profit_amt1;
                    /*дһ�����������ͳ����Ϣ*/
                    fprintf(fp_qs,"%-6s %-20s%-8ld%-17.2lf%-17.2lf%-17.2lf%-17.2lf%-17.2lf\n",\
                        cOld_br_no,sFn_parm.prdt_code,sTotal_jg.cnt,sTotal_jg.tx_amt,sTotal_jg.redempt_amt,\
                        sTotal_jg.return_capital1,sTotal_jg.fct_intst1,sTotal_jg.profit_amt1);
                    lLine_qs ++;
                    if(lLine_qs % LINE_PAGE == 0)
                    {
                        iPage_qs++;
                        fprintf( fp_qs, "======================================================================================================================\n" );
                        fprintf( fp_qs, "                                                                                                             ��%ldҳ\n",iPage_qs);
                        fprintf(fp_qs,"%c",0x0C);
                    }
                }
                memset(&sTotal_jg,0x00,sizeof(sTotal_jg));
                memset(&sCom_branch,0x00,sizeof(sCom_branch));
                ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sFn_reg.tx_br_no);
                if(ret)
                {
                    vtcp_log("select com_branch error! ret[%d]  ",ret);
                    strcpy(g_pub_tx.reply,"T087");
                    goto ErrExit;
                }
                strcpy(cOld_br_no,sFn_reg.tx_br_no);
                /* ���»����ļ� */
                if(fp != NULL)
                {
                    fclose(fp);
                    fp = NULL;
                }
                ret=pub_rpt_append_openfile(&fp,sFn_reg.tx_br_no,cFilename);
                if(ret)
                {
                    vtcp_log("���ļ�ʧ��!");
                    strcpy(g_pub_tx.reply,"S066");
                    goto ErrExit;
                }
                
                fprintf(fp, "                                            ����[%s]��Ʋ�Ʒ[%s]���������嵥\n",cOld_br_no,sFn_parm.prdt_code);
                fprintf(fp, "                                                                                                     ����:%d��%d��%d��\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
                fprintf(fp, "=======================================================================================================================================\n" );
                    fprintf(fp,"%-20s%-17s%-17s%-17s%-17s%-17s%-8s\n","�˺�","�Ϲ����","��ؽ��","��������","������","������","ժ  Ҫ");
                fprintf(fp, "=======================================================================================================================================\n" );
                iPage=0;
                lLine=0;
                /*�ϸ������Ļص���ҳ �¸���Ʒ�����Ǹ��ļ� ��ֹ��ҳ����*/
                if(iHdCnt > 0)
                {
                    fprintf(fp1,"%c",0x0C);
                }
                /* �򿪻ص��ļ� */
                if(fp1 != NULL)
                {
                    fclose(fp1);
                    fp1 = NULL;
                }
                ret=pub_rpt_append_openfile(&fp1,sFn_reg.tx_br_no,cFilename1);
                if(ret)
                {
                    vtcp_log("���ļ�ʧ��!");
                    strcpy(g_pub_tx.reply,"S066");
                    goto ErrExit;
                }
                iHdCnt = 0;/*�»��� �ص�������0*/
            }
            /*ͳ�Ƶ������������Ʒ����Ϣ*/
            sTotal_jg.cnt ++;
            sTotal_jg.tx_amt+=sFn_reg.tx_amt;
            sTotal_jg.redempt_amt+= sFn_reg.redempt_amt;
            sTotal_jg.return_capital1+= dRbj_amt;
            sTotal_jg.fct_intst1 += dFct_intst;
            sTotal_jg.profit_amt1 += dProfit_amt;
            
            fprintf(fp,"%-20s%-17.2lf%-17.2f%-17.2lf%-17.2lf%-17.2lf%-8s\n",\
                sFn_reg.ac_no,sFn_reg.tx_amt,sFn_reg.redempt_amt,\
                sFn_reg.return_capital,dFct_intst,dProfit_amt,"��Ƶ���");
            lLine++;
            if( lLine%LINE_PAGE==0 )
            {
                iPage++;
                fprintf( fp, "=======================================================================================================================================\n" );
                fprintf( fp, "                                                                                                                         ��%ldҳ\n",iPage);
                fprintf(fp,"%c",0x0C);
            }
            /*�ص�3��ÿҳ*/
            if(iHdCnt % HD_LINE ==0 && iHdCnt > 0)
            {
                fprintf(fp1,"%c",0x0C);
            }
            fprintf(fp1,"\n\n\n\n                                      ��Ʋ�Ʒ����                         �ͻ��ص�\n");
            fprintf(fp1,"               �Ϲ�����:%d-%d-%d    �������:%s\n",\
            sFn_reg.tx_date/10000,sFn_reg.tx_date/100%100,sFn_reg.tx_date%100,sCom_branch.br_name);
            
            fprintf(fp1,"               ��Ʒ����:%s\n",sFn_parm.prdt_code);
            fprintf(fp1,"               ��Ʒ����:%s\n",sFn_parm.title);
            fprintf(fp1,"               �Ϲ��ͻ�:%s\n",cName);
            fprintf(fp1,"               �Ϲ����:%-17.2lf  ��ؽ��:%-17.2lf \n",\
                            sFn_reg.tx_amt,sFn_reg.redempt_amt);
            fprintf(fp1,"               ��������:%-17.2lf  ������:%-9.6lf  ����:%-17.2lf\n",\
                            sFn_reg.return_capital,dFct_rate,dFct_intst);
            fprintf(fp1,"               �����˺�:%s\n",cAc_no);
            fprintf(fp1,"               ���˻���:%s\n",cName);
            fprintf(fp1,"               ����д��:%s\n",pub_rpt_pcask(dFct_intst));
            fprintf(fp1,"               ��Сд��:%.2lf\n",dFct_intst);
            fprintf(fp1,"               ��ת��־:%s\n","ת��");
            fprintf(fp1,"               ���������������ת�������˺�\n");
            fprintf(fp1,"               ��������:%8ld         ����Ա:%6s         ��ˮ��:%-8ld\n",g_pub_tx.tx_date,g_pub_tx.tel,g_pub_tx.trace_no);
            iHdCnt ++;
            
        }
        Fn_reg_Clo_Upd();

        /* fn_parm��Ʒ״̬�޸� */
        sFn_parm.stat[0]='2';
        ret=Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
        if(ret)
        {
            vtcp_log("����fn_parmʧ��! ret=[%d]",ret);
            Fn_parm_Clo_Upd();
            strcpy(g_pub_tx.reply,"D325");
            goto ErrExit;
        }
    }
    Fn_parm_Clo_Upd();

OkExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    if(fp1 != NULL)
    {
        fclose(fp1);
        fp1 = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    sql_commit();    /*--�ύ����--*/
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("����ɹ�![%s]", g_pub_tx.reply);
    set_zd_data("0120", g_pub_tx.reply);
    return 0;

ErrExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    if(fp1 != NULL)
    {
        fclose(fp1);
        fp1 = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    sql_rollback();   /*--����ع�--*/
    vtcp_log("����ʧ��![%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

/*���ݿͻ��˺��ҵ������˵��˺� �Ҳ��� �ͱ���shangyongchao 2013/3/9 20:46:46
ac_no ������˺�
new_ac_no ����ac_id�ҵ���״̬�������˺�*/
static int my_get_ac_no(char *ac_no,char *new_ac_no ,char *name)
{
    int iRet = 0;
    long lAc_id = 0;
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    iRet  = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",ac_no);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log( "�����˺�ʧ��![%d]", iRet);
        return 1;
    }
    else if(iRet == 100)
    {
        vtcp_log( "�޴��˺�![%s]", ac_no);
        return 1;
    }
    /*�����͹�ʧ�� ֱ�ӷ��ؾ����� ����������*/
    if(sMdm_ac_rel.note_sts[0] == '1' || sMdm_ac_rel.note_sts[0] =='0')
    {
        strcpy(new_ac_no,ac_no);
        /*strcpy(name,sMdm_ac_rel.name);*/
        return 0;
    }
    lAc_id = sMdm_ac_rel.ac_id;
    /*����״̬�� �� ac_id����� �������߹�ʧ���˺� ʵ���Ҳ����ͱ���*/
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld and note_sts in('0','1')",lAc_id);
    if(iRet != 0 && iRet != 100)
    {
        sprintf(acErrMsg, "�����˺�ʧ��![%d]", iRet);
        WRITEMSG
        return 1;
    }
    else if(iRet == 100)
    {
        sprintf(acErrMsg, "�޴��˺�![%s]", ac_no);
        WRITEMSG
        return 1;
    }
    strcpy(new_ac_no,sMdm_ac_rel.ac_no);
    /*strcpy(name,sMdm_ac_rel.name);*/
    return 0;
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
         strcpy(g_pub_tx.reply,"D343");
         goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ�ķֶ�������Ϣ����");
        strcpy(g_pub_tx.reply,"D344");
        goto ErrExit;
    }
    *fct_rate = sRedempt_rate.redempt_rate;
    *profit_rate = sRedempt_rate.profit_rate;
    vtcp_log("ȡ����ʵ������[%lf]",*fct_rate);
OkExit:
    vtcp_log("ȡ�ֶ����ʳɹ�");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("ȡ�ֶ�����ʧ��");
    return 1;
}


