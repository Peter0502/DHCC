/*************************************************************
* �� �� ��: gD157.c
* �������������Ĵ����ս�Ϣ���˴��� �ﵽ����������ִ��
*
* ��    ��: zqbo
* �������: 2014��11��21��
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "mo_sxc_c.h"
#include "mo_sxc_hst_c.h"
#include "com_item_c.h"

extern long pub_base_daynumSUB(long date, int day);
#define COUNT_COMMIT 10000

double dJz_amt = 0.00;

gD157()
{
    int  iRet     = 0;
    int  iDays    = 0;
    int  iCount   = 0;
    long lDate    = 0;
    long lAc_id   = 0;
    long lAc_seqn = 1;
    double dIntst_hst = 0.00;    /**ÿ�ʽ����Ϣ**/
    double dSum_intst = 0.00;
    
    struct mo_sxc_hst_c sMo_sxc_hst;
    
    memset(&sMo_sxc_hst,  0x00, sizeof(sMo_sxc_hst));
    
    nOpenBatchLog();
    /* ������ */
    iRet = sql_begin();    
    if ( iRet )
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
    strcpy(g_pub_tx.tx_code, "Z157");
    strcpy(g_pub_tx.sub_tx_code, "Z157");
    /**ȡ��ˮ��**/
    iRet = pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_PubQlshȡ��ˮ�ų���!!!" );
        WRITEMSG
        goto ErrExit;
    }
    set_zd_long(DC_TRACE_NO, g_pub_tx.trace_no);
    
    /**ȡcom_parm�������õĲ�����ֵ**/
    iRet = pub_base_GetParm_double("SXC", 4, &dJz_amt);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double4ȡ��С������Ϣ�������ñ����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    iRet = pub_base_GetParm_int("SXC", 5, &iDays);
    if(iRet)
    {
        sprintf( acErrMsg, "pub_base_GetParm_double5ȡ���ڽ�Ϣ��������������ñ����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /***���� iDays��ǰ������***/
    lDate = pub_base_daynumSUB(g_pub_tx.tx_date, iDays);
    vtcp_log("iDays=[%d],lDate==[%ld]", iDays, lDate);

    iRet = Mo_sxc_hst_Dec_Sel(g_pub_tx.reply, "sts='1' and ic_date<=%ld order by ac_id", lDate); /**order by������for update**/
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D101");
        sprintf( acErrMsg, "Mo_sxc_Dec_Upd����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    while(1)
    {
        /***ÿ��ѭ����ʼ��***/
        dIntst_hst = 0.00;
        memset(&sMo_sxc_hst, 0x00, sizeof(sMo_sxc_hst));
        
        iRet = Mo_sxc_hst_Fet_Sel(&sMo_sxc_hst, g_pub_tx.reply);
        if(iRet == 100)
        {
            if(lAc_id != 0)
            {
                /**---------�˳�ѭ��ʱ�����һ���˻����ۼ���Ϣ����  ���޸�mo_sxc�Ǽǲ�-------**/
                iRet = iAcct_sxc_sum_intst(lAc_id, lAc_seqn, dSum_intst);
                if(iRet)
                {
                    sprintf( acErrMsg, "iAcct_sxc_sum_intst����!!!" );
                    WRITEMSG
                    Mo_sxc_hst_Clo_Sel();
                    goto ErrExit;
                }
            }
            break;
        }
        else if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_hst_Fet_Sel!!!" );
            WRITEMSG
            goto ErrExit;
        }
        vtcp_log("ac_id[%ld]", lAc_id);
        if(sMo_sxc_hst.ac_id != lAc_id && lAc_id != 0) /**-�任�˻�**/  
        {
            /**------��ǰһ���˻��ۼӵ���Ϣ���� ���޸�mo_sxc�Ǽǲ�----------**/
            iRet = iAcct_sxc_sum_intst(lAc_id, lAc_seqn, dSum_intst);
            if(iRet)
            {
                sprintf( acErrMsg, "iAcct_sxc_sum_intst����!!!" );
                WRITEMSG
                Mo_sxc_hst_Clo_Sel();
                goto ErrExit;
            }
            
            dSum_intst = 0.00; /**�ۼ���һ���˻�ʱ���¸�ֵ**/
            
            
            if(iCount >= COUNT_COMMIT)
            {
                vtcp_log("��ʼ�ֲ��ύiCount[%d]==================", iCount);
                sql_commit();
                iRet = sql_begin();    
                if ( iRet )
                {
                    sprintf( acErrMsg, "������ʧ��!!!" );
                    WRITEMSG
                    Mo_sxc_hst_Clo_Sel();
                    goto ErrExit;
                }
                /**����ȡ��ˮ��**/
                iRet = pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply);
                if(iRet)
                {
                    sprintf( acErrMsg, "pub_base_PubQlshȡ��ˮ�ų���!!!" );
                    WRITEMSG
                    Mo_sxc_hst_Clo_Sel();
                    goto ErrExit;
                }
                set_zd_long(DC_TRACE_NO, g_pub_tx.trace_no);
                iCount = 0;
            }
        }
        
        /****--------�����ͷŽ�����Ϣ ���Ǽǽ�Ϣ��ϸ-------***/
        iRet = iSxc_hst_intst(sMo_sxc_hst, sMo_sxc_hst.hst_bal, &dIntst_hst, '3');
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf(acErrMsg, "��Ϣ֧����Ŀ���˴���!!");
            WRITEMSG
            Mo_sxc_hst_Clo_Sel();
            goto ErrExit;
        }
        
        /****--------------���µǼ���ϸ-------------***/
        sMo_sxc_hst.intst += dIntst_hst;
        iRet = sql_execute("update mo_sxc_hst set intst=%lf,ic_date=%ld where ac_id=%ld and ac_seqn=%d and rate_date=%ld and sts='1'", \
                                       sMo_sxc_hst.intst, g_pub_tx.tx_date, sMo_sxc_hst.ac_id, sMo_sxc_hst.ac_seqn, sMo_sxc_hst.rate_date);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "sql_execute(update mo_sxc_hst...)����!!!" );
            WRITEMSG
            Mo_sxc_hst_Clo_Sel();
            goto ErrExit;
        }
        
        dSum_intst += dIntst_hst;     /***�ۼӱ���ac_id�������Ϣ**/
        
        lAc_id   = sMo_sxc_hst.ac_id; /***��¼����ac_id**/
        lAc_seqn = sMo_sxc_hst.ac_seqn;
        
        iCount++;
    }
    Mo_sxc_hst_Clo_Sel();
    

OkExit:
    /*--�ύ����--*/
    sql_commit();        
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "���Ĵ�������ƴ���ɹ�!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;
ErrExit:
    
    /*--����ع�--*/
    sql_rollback();
    if (strcmp(g_pub_tx.reply, "0000") == 0 || strlen(g_pub_tx.reply) == 0)
    {
        strcpy(g_pub_tx.reply, "G009");
    }
    sprintf(acErrMsg, "���Ĵ�������ƴ���ʧ��!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}

int iAcct_sxc_sum_intst(long lAc_id, long lAc_seqn, double dSum_intst)
{
    int iRet = 0;

    struct mo_sxc_c sMo_sxc;
    
    memset(&sMo_sxc,      0x00, sizeof(sMo_sxc));
    memset(&g_dd_mst,     0x00, sizeof(g_dd_mst));
    memset(&g_dd_parm,    0x00, sizeof(g_dd_parm));
    memset(&g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel));
    
    /***���һ����˻�**/
    iRet = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%ld", lAc_id, lAc_seqn);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Dd_mst_Sel����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    iRet = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", g_dd_mst.prdt_no);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Dd_parm_Sel����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    /***����ac_id�ҵ�һ��״̬Ϊ����/��ʧ״̬���˺�***/
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code<>'1010'", g_dd_mst.ac_id);
    if(iRet)
    {
    	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_id=%ld and note_sts in('0','1') and mdm_code='1010'", g_dd_mst.ac_id);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mdm_ac_rel_Sel����!!!" );
            WRITEMSG
            goto ErrExit;
         }
    }
    
    if(pub_base_CompDblVal(dSum_intst, 0.00) > 0)
    {
    	/**�ҵ����Ĵ�Ǽǲ����δ���˽��**/
    	iRet = Mo_sxc_Sel(g_pub_tx.reply, &sMo_sxc, "ac_id=%ld and ac_seqn=%d and sts='1'", lAc_id, lAc_seqn);
    	if(iRet != 0 && iRet != 100)
    	{
    	    strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "Mo_sxc_Sel����!!!" );
            WRITEMSG
            goto ErrExit;
    	}
    	
    	/*****-------���ͷŽ������Ϣ����---------****/
    	if(pub_base_CompDblVal(dSum_intst+sMo_sxc.un_intst, dJz_amt) >= 0) /**ֻ�дﵽ1Ԫ�����ã�����ͻ���**/
    	{
    	    
            iRet = iAcct_sxc_intst(lAc_id, lAc_seqn, dSum_intst+sMo_sxc.un_intst);
            if(iRet)
            {
                sprintf( acErrMsg, "iAcct_intst����!!!" );
                WRITEMSG
                goto ErrExit;
            }
            
            sMo_sxc.un_intst = 0.00;         /**���˺�δ������Ϣ���**/
            sMo_sxc.flag[0] = '1';           /**1-����ҵ��**/
        }
        else
        {
             sMo_sxc.un_intst += dSum_intst;  /**�ۼ�δ������Ϣ**/
        }
        
        /**--------���µǼǲ��Ľ����Ϣ----------**/
        sMo_sxc.intst += dSum_intst;      /**�ۼӽ����Ϣ**/
        iRet = sql_execute("update mo_sxc set intst=%lf, un_intst=%lf, flag='%s' where ac_id=%ld and ac_seqn=%d and sts='1'", \
                                                     sMo_sxc.intst, sMo_sxc.un_intst, sMo_sxc.flag, lAc_id, lAc_seqn);
        if(iRet)
        {
            strcpy(g_pub_tx.reply, "D102");
            sprintf( acErrMsg, "sql_execute(update mo_sxc...)����!!!" );
            WRITEMSG
            goto ErrExit;
        }
    }
    
OkExit:
    sprintf(acErrMsg, "���˴���ɹ�!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    
    sprintf(acErrMsg, "���˴���ʧ��!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}

int iAcct_sxc_intst(long ac_id, long ac_seqn, double intst)
{
    int iRet = 0;
    
    struct com_item_c sCom_item;
    memset(&sCom_item,    0x00, sizeof(sCom_item));
    
    /***�� ������ڻ� ***/
    strcpy(g_pub_tx.ac_no, g_mdm_ac_rel.ac_no);
    vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------", g_pub_tx.ac_no);
    g_pub_tx.ac_id_type[0] = '1';
    strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, g_dd_mst.opn_br_no);
    g_pub_tx.ac_id = g_dd_mst.ac_id;
    g_pub_tx.ac_seqn = g_dd_mst.ac_seqn;
    strcpy(g_pub_tx.cur_no, g_dd_parm.cur_no);
    
    strcpy(g_pub_tx.add_ind, "1");
    g_pub_tx.tx_amt1 = intst;
    strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.prt_ind, "0");
    strcpy(g_pub_tx.hst_ind, "1");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000J1");
    g_pub_tx.svc_ind = 1005;
    strcpy(g_pub_tx.brf, "���Ĵ����ڽ�Ϣ");
    
    iRet = pub_acct_trance();
    if(iRet)
    {
        sprintf( acErrMsg, "pub_acct_trance_jx����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    set_zd_data("101A", g_pub_tx.cur_no);
    set_zd_data("101B", g_pub_tx.ct_ind);
    set_zd_double("1013", g_pub_tx.tx_amt1);
    iRet = pubf_acct_proc("D099");
    if(iRet)
    {
        sprintf( acErrMsg, "pubf_acct_proc����!!!" );
        WRITEMSG
        goto ErrExit;
    }
    
    /***�������Ϣ֧��***/
    get_zd_data("0153", g_pub_tx.ac_no);
    vtcp_log("KKKKK,g_pub_tx.ac_no[%s]-----------------", g_pub_tx.ac_no);
    iRet=Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_hrt='%s'",g_pub_tx.ac_no);
    if(iRet)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf(acErrMsg, "Com_item_Sel��Ϣ֧����Ŀ����!!");
        WRITEMSG
        goto ErrExit;
    }
    memset(g_pub_tx.ac_no, 0x00,sizeof(g_pub_tx.ac_no));
    strcpy(g_pub_tx.ac_no, sCom_item.acc_no);
    strcpy(g_pub_tx.tx_br_no, g_dd_mst.opn_br_no);
    strcpy(g_pub_tx.opn_br_no, g_dd_mst.opn_br_no);
    g_pub_tx.ac_id_type[0] = '9';
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn = 0;
    strcpy(g_pub_tx.ac_id_type, "9");
    strcpy(g_pub_tx.add_ind, "0");    /**�跽 **/
    g_pub_tx.tx_amt1 = intst;
    strcpy(g_pub_tx.hst_ind, "1");
    strcpy(g_pub_tx.ac_wrk_ind, "0000000J1");
    g_pub_tx.svc_ind = 9001;

    iRet = pub_acct_trance();
    if (iRet)
    {
        sprintf(acErrMsg, "��Ϣ֧����Ŀ���˴���!!");
        WRITEMSG
        goto ErrExit;
    }
    
    set_zd_data("1204", g_pub_tx.cur_no);
    set_zd_data("1205", g_pub_tx.ct_ind);
    set_zd_double("1208", g_pub_tx.tx_amt1);
    iRet = pubf_acct_proc("A016");
    if (iRet != 0)
    {
        sprintf(acErrMsg, "pubf_acct_proc��Ƽ��ʲ��ɹ�!!");
        WRITEMSG
        goto ErrExit;
    }
    
    
OkExit:
    sprintf(acErrMsg, "���˴���ɹ�!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
ErrExit:
    
    sprintf(acErrMsg, "���˴���ʧ��!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}