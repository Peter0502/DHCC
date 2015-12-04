/*************************************************************
* �� �� ��: gD188.c
* ��������: ���ļ�������ۿ�
*
* ��    ��: shangyongchao
* �������: 2013/5/23 16:07:35
*
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
#include "dd_mst_c.h"
#define LINE_PAGE 40
int gD188()
{
    int ret,iPage,iFail_Cnt,iPage_qs;
    long lLine = 0;
    long lLine_qs = 0;
    long lCnt_qs = 0;
    char cFilename[256],cCmd[512],cFilename_fail[256];;
    char cOld_br_no[6];
    char cAc_no[25];/*�ۿ��˺�*/
    char cName[61];
    double dSum_amt=0.00; /* ��Ʒ�Ϲ����ͳ�� */
    double dTx_amt_jg;/*ͳ�Ƶ���������ĳ����Ʒ���Ϲ����*/
    double dTx_amt;/*ͳ�����е��Ϲ����*/
    FILE * fp=NULL;
    FILE *fp_fail = NULL;
    FILE *fp_qs=NULL;
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;

    ret=iPage=lLine=iFail_Cnt=iPage_qs=0;
    memset(cFilename_fail,0x00,sizeof(cFilename_fail));
    memset(cFilename,0x00,sizeof(cFilename));
    memset(cCmd,0x00,sizeof(cCmd));
    memset(cOld_br_no,0x00,sizeof(cOld_br_no));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));

    pub_base_sysinit();

    strcpy(g_pub_tx.tx_code,"G188");
    strcpy(g_pub_tx.sub_tx_code,"G188");
    set_zd_data("096C","0100");
    /*set_zd_data("096M",YW_TYPE_ZC);by wy*/
    strcpy(g_pub_tx.brf,"��ƻ���");
    strcpy(cFilename,"lckk");    /* ��ƿۿ� */
    /* ɾ���ļ� */
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    vtcp_log(" rm -f %s/report/%s/%s.txt",getenv("HOME"),"*",cFilename);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    strcpy(cFilename_fail,"lcfail");
    sprintf(cCmd,"rm -f %s/report/%s/%s.txt",getenv("HOME"),QS_BR_NO,cFilename_fail);
    vtcp_log("rm -f %s/report/%s/%s.txt",getenv("HOME"),QS_BR_NO,cFilename_fail);
    ret=system(cCmd);
    if( ret )
    {
        vtcp_log("rm file error [%s] [%d]",cCmd,ret);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    /*���������ĵ� ͳ���ļ�*/
    ret=pub_rpt_append_openfile(&fp_qs,QS_BR_NO,cFilename);
    if(ret)
    {
        vtcp_log("���ļ�ʧ��!");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    sql_begin();
    /*���ҵ���ļ�����ڵ�*/
    /*
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"stat='0' and end_date  = %ld",g_pub_tx.tx_date);
    */
    /*�����������Ŀۿ�*/
    ret=Fn_parm_Dec_Upd(g_pub_tx.reply,"stat='5' order by prdt_code");
    if(ret)
    {
        vtcp_log("����fn_parm�α�ʧ��! ret=[%d]",ret);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    while(1)
    {
        dTx_amt_jg = 0.00;
        lLine = 0;
        iPage = 0;
        lLine_qs = 0;
        dTx_amt = 0.00;
        lCnt_qs = 0;
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        ret=Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
        if( ret==100 )
        {
            vtcp_log("fn_parm fetch over!");
            break;
        }
        else if( ret )
        {
            vtcp_log("fn_parm fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }

        vtcp_log("end_date[%ld] ",sFn_parm.end_date);
        /* ȡ������ˮ�� */
        if( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply) )
        {
            vtcp_log("ȡ������ˮ�Ŵ� [%d]", g_pub_tx.trace_no);
            goto ErrExit;
        }
        set_zd_long(DC_TRACE_NO,g_pub_tx.trace_no);
        g_pub_tx.trace_cnt = 0;
        set_zd_long(DC_TRACE_CNT,g_pub_tx.trace_cnt);
        memset(cOld_br_no,0x00,sizeof(cOld_br_no));

        /*����Ҫͳ���ˣ������������� �Ϳۿ�
        ret=sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",sFn_parm.prdt_code);
        if( ret )
        {
            vtcp_log("ͳ��fn_reg����!");
            strcpy(g_pub_tx.reply, "D330");
            goto ErrExit;
        }
        */
        vtcp_log("min_amt[%.2lf] sum[%.2lf] ",sFn_parm.min_amt,dSum_amt);
        /*
        if(pub_base_CompDblVal(dSum_amt,sFn_parm.min_amt) >= 0)
        */
        if(1)
        {
                    /*ÿ����Ʒ��һ����ͷ*/
            fprintf(fp_qs,"\n\n\n");
            fprintf(fp_qs, "                                           ��Ʋ�Ʒ[%s]�ۿ�ͳ��\n",sFn_parm.prdt_code);
            fprintf(fp_qs, "                                                                                                   ����:%d��%d��%d��\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
            fprintf(fp_qs, "======================================================================================================================\n" );
            fprintf(fp_qs, "%-6s %-20s%-8s%-17s%-8s\n","������","��Ʋ�Ʒ����","����","��ƽ��","������");
            fprintf(fp_qs, "======================================================================================================================\n" );

            ret=Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code ='%s'  and sts='0' order by tx_br_no,ac_id",\
                    sFn_parm.prdt_code);
            if( ret )
            {
                vtcp_log(" declare cursor error! ret[%d] ",ret);
                strcpy( g_pub_tx.reply,"D332" );
                goto ErrExit;
            }
            while(1)
            {
                memset(cAc_no,0x00,sizeof(cAc_no));
                memset(cName,0x00,sizeof(cName));
                memset(&sFn_reg,0x00,sizeof(sFn_reg));
                ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
                if( ret==100 )
                {
                    vtcp_log("fn_reg_hst fetch over! ac_id[%ld] ",sFn_reg.ac_id);
                    if(lLine > 0)
                    {
                        vtcp_log("! dTx_amt_jg[%lf]  ",dTx_amt_jg);
                        iPage++;
                        fprintf( fp, "======================================================================================================================\n" );
                        fprintf(fp,  "                  �ϼƱ���:%-8ld       �ϼƽ��:%-17.2f                                         ��%ldҳ\n",lLine,dTx_amt_jg,iPage);
                        fprintf(fp,"%c",0x0C);
                        fprintf(fp_qs, "%-6s %-20s%-8d%-17.2lf%-8s\n",cOld_br_no,sFn_parm.prdt_code,lLine,dTx_amt_jg,"�ۿ�ɹ�");
                        lLine_qs ++;
                        if( lLine_qs%LINE_PAGE==0 )
                        {
                            iPage_qs++;
                            fprintf( fp_qs, "======================================================================================================================\n" );
                            fprintf( fp_qs, "                                                                                                             ��%ldҳ\n",iPage_qs);
                            fprintf(fp_qs,"%c",0x0C);
                        }
                    }
                    if(lLine_qs>0)
                    {
                        iPage_qs++;
                        fprintf( fp_qs, "======================================================================================================================\n" );
                        fprintf(fp_qs,  "               �ϼƱ���:%-8ld          �ϼƽ��:%-17.2f                                         ��%ldҳ\n",lCnt_qs,dTx_amt,iPage_qs);
                        fprintf(fp_qs,"%c",0x0C);
                    }
                    break;
                }
                else if( ret )
                {
                    vtcp_log("fn_reg fetch error! ret[%d] ",ret);
                    strcpy(g_pub_tx.reply,"D330");
                    goto ErrExit;
                }
                memset(&sDd_mst,0x00,sizeof(sDd_mst));
                ret=Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld",sFn_reg.ac_id);
                if( ret )
                {
                    vtcp_log("����dd_mst�α�ʧ��! ret=[%d]",ret);
                    strcpy(g_pub_tx.reply,"D226");
                    goto ErrExit;
                }
                ret=Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
                if( ret )
                {
                    vtcp_log("��dd_mst�α�ʧ��! ret=[%d]",ret);
                    strcpy(g_pub_tx.reply,"D230");
                    goto ErrExit;
                }
                vtcp_log("clt_amt[%.2lf] tx_amt[%.2lf] ",sDd_mst.ctl_amt,sFn_reg.tx_amt);
                sDd_mst.ctl_amt-=sFn_reg.tx_amt;
                if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) <0)
                {
                    vtcp_log( "���ƽ���Ǹ��� �����飡[%lf] ",sDd_mst.ctl_amt);
                    strcpy(g_pub_tx.reply,"S035");
                    goto ErrExit;
                }
                ret=Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
                if( ret )
                {
                    vtcp_log("����dd_mstʧ��! ret=[%d]",ret);
                    strcpy(g_pub_tx.reply,"D226");
                    goto ErrExit;
                }
                Dd_mst_Clo_Upd();
                		/*����˺ű�ȫ������ֻ��������Ӧ���Զ����*/
		/*�˺Ŷ�����Զ���� ���ۿ� syc 201301101*/
                if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2'
			|| (sDd_mst.bal - sDd_mst.hold_amt - sDd_mst.ctl_amt < sFn_reg.tx_amt)) 
                {
                   sFn_reg.sts[0]='*';
                   strcpy(sFn_reg.filler,"�򶳽��ֹ��δ�ۿ�");
                   ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
                   if(ret)
                   {
                       vtcp_log("update fn_reg error! ret[%d]  ",ret);
                       strcpy(g_pub_tx.reply,"D332");
                       goto ErrExit;
                   }
                   continue;
                }
                
                /* �跽���ͻ��˻� */
                strcpy(g_pub_tx.tx_br_no,sFn_reg.tx_br_no);
                /*����ͬac_id�½���״̬�������͹�ʧ���˺ţ����ܴ��������ʵ����*/
                ret = my_get_ac_no(sFn_reg.ac_no,cAc_no,cName);
                if(ret)
                {
                    vtcp_log("û�к��ʵĿۿ��˺�![%s]",sFn_reg.ac_no);
                    goto ErrExit;
                }
                strcpy(g_pub_tx.ac_no,cAc_no);
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no = 0;
                g_pub_tx.ac_id=sFn_reg.ac_id;
                g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
                g_pub_tx.tx_amt1=sFn_reg.tx_amt;
                g_pub_tx.add_ind[0]='0';
                strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                g_pub_tx.svc_ind=1001;
                strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
                strcpy(g_pub_tx.ac_id_type,"1");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
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
                /* ����������ʽ��˻� */
                strcpy(g_pub_tx.name,"");
                g_pub_tx.cif_no = 0;
                if( sFn_reg.cif_type[0]=='1' )
                {
                    strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
                }
                else if( sFn_reg.cif_type[0]=='2'||sFn_reg.cif_type[0]=='3'||sFn_reg.cif_type[0]=='4'||sFn_reg.cif_type[0]=='6' )   /*add by lwb ���ӿͻ�����Ϊ3 ���� 4 ����ͬҵ 6���ڻ���*/
                {
                    strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
                }
                /*����ǿ�Ŀ ������������������*/
                if(strlen(g_pub_tx.ac_no) < 10)
                {
                    strcpy(g_pub_tx.opn_br_no,QS_BR_NO);
                }
                 vtcp_log("%s,%d g_pub_tx.ac_no====[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no);
                 vtcp_log("%s,%d 444444444sFn_parm.prdt_code====[%s]\n",__FILE__,__LINE__,sFn_parm.prdt_code);
                g_pub_tx.ac_id = 0;
                g_pub_tx.ac_seqn=0;
                g_pub_tx.tx_amt1=sFn_reg.tx_amt;
                g_pub_tx.add_ind[0]='1';
                strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
                strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
                /*g_pub_tx.svc_ind=1001;*/
                g_pub_tx.svc_ind=9001;
                strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
                strcpy(g_pub_tx.ac_id_type,"9");
                strcpy(g_pub_tx.note_type,"");
                strcpy(g_pub_tx.beg_note_no,"");
                if ( pub_acct_trance() )
                {
                    vtcp_log("���ô�ȡ������ʧ��!");
                    goto ErrExit;
                }
                set_zd_data("102K",g_pub_tx.ct_ind);
                set_zd_double("1002",g_pub_tx.tx_amt1);
                if ( pubf_acct_proc("LC02") )
                {
                    vtcp_log("�Ǽǻ����ˮ����!");
                    goto ErrExit;
                }
                sFn_reg.pay_date=g_pub_tx.tx_date;
                sFn_reg.pay_trace_no=g_pub_tx.trace_no;
                sFn_reg.sts[0]='1';
                ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
                if(ret)
                {
                    vtcp_log("update fn_reg error! ret[%d]  ",ret);
                    strcpy(g_pub_tx.reply,"D332");
                    goto ErrExit;
                }
                /*�Ǽ���ϸ*/
                sFn_reg_hst.tx_date = g_pub_tx.tx_date;
                sFn_reg_hst.trace_no = g_pub_tx.trace_no;
                strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
                strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
                strcpy(sFn_reg_hst.type,"1");
                sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
                sFn_reg_hst.buy_date=sFn_reg.tx_date;
                sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
                strcpy(sFn_reg_hst.brf,"�����ۿ�");
                ret = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
                if(ret)
                {
                    Fn_reg_hst_Debug(&sFn_reg_hst);
                    vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",ret);
                    strcpy(g_pub_tx.reply,"D335");
                    goto ErrExit;
                }
                vtcp_log(" dTx_amt_jg[%lf]  ",dTx_amt_jg);
                /* ���ɱ����嵥 */
                if( strcmp(sFn_reg.tx_br_no,cOld_br_no) )
                {
                    if(lLine > 0)
                    {
                        vtcp_log("! dTx_amt_jg[%lf]  ",dTx_amt_jg);
                        iPage++;
                        fprintf( fp, "======================================================================================================================\n" );
                        fprintf(fp,  "                  �ϼƱ���:%-8ld��       �ϼƽ��:%-17.2f                                         ��%ldҳ\n",lLine,dTx_amt_jg,iPage);
                        fprintf(fp,"%c",0x0C);
                        /*�������ĵ�ͳ���ļ�*/
                        fprintf(fp_qs, "%-6s %-20s%-8d%-17.2lf%-8s\n",cOld_br_no,sFn_parm.prdt_code,lLine,dTx_amt_jg,"�ۿ�ɹ�");
                        lLine_qs ++;
                        if( lLine_qs%LINE_PAGE==0 )
                        {
                            iPage_qs++;
                            fprintf( fp_qs, "======================================================================================================================\n" );
                            fprintf( fp_qs, "                                                                                                             ��%ldҳ\n",iPage_qs);
                            fprintf(fp_qs,"%c",0x0C);
                        }
                    }
                    dTx_amt_jg = 0.00;
                    vtcp_log(" dTx_amt_jg[%lf]  ",dTx_amt_jg);
                    /* ���ļ� */
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
                    memcpy(cOld_br_no,sFn_reg.tx_br_no,sizeof(sFn_reg.tx_br_no)-1);
                    fprintf(fp, "                                            ����[%s]��ƿۿ��嵥\n",cOld_br_no);
                    fprintf(fp, "                                                                                                   ����:%d��%d��%d��\n",g_pub_tx.tx_date/10000,g_pub_tx.tx_date/100%100,g_pub_tx.tx_date%100);
                    fprintf(fp, "======================================================================================================================\n" );
                    fprintf(fp, "%-20s %-20s%-60s%-17s\n","��Ʋ�Ʒ����","�˺�","����","��ƽ��");
                    fprintf(fp, "======================================================================================================================\n" );
                    iPage=0;
                    lLine=0;
                }
                fprintf(fp, "%-20s %-20s%-60s%-17.2lf\n",sFn_parm.prdt_code,cAc_no,cName,sFn_reg.tx_amt);
                lLine++;
                dTx_amt_jg += sFn_reg.tx_amt;
                if( lLine%LINE_PAGE==0 )
                {
                    iPage++;
                    fprintf( fp, "======================================================================================================================\n" );
                    fprintf( fp, "                                                                                                             ��%ldҳ\n",iPage);
                    fprintf(fp,"%c",0x0C);
                }
                dTx_amt += sFn_reg.tx_amt;
                lCnt_qs ++;
            }
            Fn_reg_Clo_Upd();

            /* fn_parm��Ʒ״̬�޸� */
            sFn_parm.stat[0]='1';
            ret=Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
            if(ret)
            {
                vtcp_log("����fn_parmʧ��! ret=[%d]",ret);
                Fn_parm_Clo_Upd();
                strcpy(g_pub_tx.reply,"D325");
                goto ErrExit;
            }
        }
        else
        {
            if(sFn_parm.end_date ==  g_pub_tx.tx_date)
            {
                if(iFail_Cnt == 0)
                {
                    /*�������ǵ����  ļ��ʧ�ܵ� д���ļ��� �ļ�ֻ���������Ĵ�һ��*/
                    ret=pub_rpt_append_openfile(&fp_fail,QS_BR_NO,cFilename_fail);
                    if(ret)
                    {
                        vtcp_log("���ļ�ʧ��!");
                        strcpy(g_pub_tx.reply,"S035");
                        goto ErrExit;
                    }
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"\n");
                    fprintf(fp_fail,"                            ļ��ʧ����Ʋ�Ʒ�б�\n");
                }
                fprintf( fp_fail, "   ��Ʋ�Ʒ%sļ��ʧ�ܣ���Сļ����%.2lf  ����ļ����%.2lf �ݲ��ۿ�봦��\n",\
                    sFn_parm.prdt_code,sFn_parm.min_amt,dSum_amt );
                if(iFail_Cnt >0 && iFail_Cnt % 40 == 0)
                {
                    fprintf(fp_fail,"\f");
                }
                iFail_Cnt ++;
            }
            vtcp_log("ļ���ܽ��[%.2lf]С�ڴ˲�Ʒ[%s]��Сļ�����[%.2lf]��ļ���ܽ��[%.2lf]�ݲ��ۿ� ",dSum_amt,sFn_parm.prdt_code,sFn_parm.min_amt,sFn_parm.sale_amt);
        }
    }
    Fn_parm_Clo_Upd();

OkExit:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    if(fp_fail != NULL)
    {
        fclose(fp_fail);
        fp_fail = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    sql_commit();     /*--�ύ����--*/
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
    if(fp_fail != NULL)
    {
        fclose(fp_fail);
        fp_fail = NULL;
    }
    if(fp_qs != NULL)
    {
        fclose(fp_qs);
        fp_qs = NULL;
    }
    sql_rollback();   /*--����ع�--*/
    vtcp_log( "����ʧ��![%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}
/*���ݿͻ��˺��ҵ��ܿ��˵��˺� �Ҳ��� �ͱ���shangyongchao 2013/3/9 20:46:46
ac_no ������˺�
new_ac_no ����ac_id�ҵ���״̬�������˺�*/
static int my_get_ac_no(char *ac_no,char *new_ac_no,char *name)
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
        vtcp_log( "�����˺�ʧ��![%d]", iRet);
        return 1;
    }
    else if(iRet == 100)
    {
        vtcp_log("�޴��˺�![%s]", ac_no);
        return 1;
    }
    strcpy(new_ac_no,sMdm_ac_rel.ac_no);
    /*strcpy(name,sMdm_ac_rel.name);*/
    return 0;
}
