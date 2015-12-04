/*************************************************************
* �� �� ��: gD050.c
* �������������䶯�ϴ����ͳ�Ʊ�
*
* ��    ��: lance
* �������: 2003��3��17��
*
* �޸ļ�¼�� 
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
#include "com_sys_parm_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "com_parm_c.h"
#include "mo_chg_bal_c.h"


gD050()
{
    int ret = 0;
    struct com_sys_parm_c com_sys_parm_c;

    memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));

    ret=sql_begin(); /*������*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "������ʧ��!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- ��ʼ��ȫ�ֱ��� --------**/
    pub_base_sysinit();
   
    /* ��ѯϵͳ������ */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯϵͳ���������!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    g_pub_tx.tx_date = com_sys_parm_c.lst_date; /* ���̺��� */

    ret = pub_eod_ins_mochgbal();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�Ǽ����䶯�ϴ����ͳ�Ʊ����!!");
        WRITEMSG
        goto ErrExit;
    }

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�Ǽ����䶯�ϴ����ͳ�Ʊ�ɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"�Ǽ����䶯�ϴ����ͳ�Ʊ�ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/**********************************************************************
 * �� �� ����   pub_eod_ins_mochgbal
 * �������ܣ�   �Ǽ����䶯�ϴ����ͳ�Ʊ�
 * ����/ʱ�䣺  lance/20030317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_ins_mochgbal()
{
    int ret;

    ret = pub_eod_stat_ddmstchgbal();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�ƻ����ʻ����ļ������仯����!!");
        WRITEMSG
        return ret;
    }

    ret = pub_eod_stat_tdmstchgbal();
    if (ret != 0)
    {
        sprintf(acErrMsg,"ͳ�ƶ����ʻ����ļ������仯����!!");
        WRITEMSG
        return ret;
    }
    return 0;
 }
/**********************************************************************
 * �� �� ����   pub_eod_stat_ddmstchgbal
 * �������ܣ�   ͳ�ƻ����ʻ����ļ������仯
 * ����/ʱ�䣺  lance/20030317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_ddmstchgbal()
{
    int ret;
    double max_amt;
    double chg_bal;

    struct dd_mst_c dd_mst_c;
    struct com_parm_c com_parm_c;
    struct mo_chg_bal_c mo_chg_bal_c;

    memset(&dd_mst_c, 0x0, sizeof(struct dd_mst_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='DEQX' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ��������������!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    max_amt = atof(com_parm_c.val);
    sprintf(acErrMsg,"���仯�Ĳ���Ϊ��[%lf]",max_amt);
    WRITEMSG
    
    ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    while(1)
    {
        ret = Dd_mst_Fet_Sel(&dd_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        chg_bal = dd_mst_c.bal - dd_mst_c.ys_bal;
        memset(&mo_chg_bal_c, 0x0, sizeof(struct mo_chg_bal_c));

        if (pub_base_CompDblVal(chg_bal,0.00) < 0 && \
            pub_base_CompDblVal(-chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, dd_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = dd_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = dd_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = dd_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = dd_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;

            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�����䶯�ϴ����ͳ�Ʊ��в������ݴ���[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }

        if (pub_base_CompDblVal(chg_bal,0.00) > 0 && \
            pub_base_CompDblVal(chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, dd_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = dd_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = dd_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = dd_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = dd_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;
         
            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�����䶯�ϴ����ͳ�Ʊ��в������ݴ���[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }
    }
    Dd_mst_Clo_Sel();

    return 0;
}
/**********************************************************************
 * �� �� ����   pub_eod_stat_tdmstchgbal
 * �������ܣ�   ͳ�ƶ����ʻ����ļ������仯
 * ����/ʱ�䣺  lance/20030317
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
 *
********************************************************************/
int pub_eod_stat_tdmstchgbal()
{
    int ret;
    double max_amt;
    double chg_bal;

    struct td_mst_c td_mst_c;
    struct com_parm_c com_parm_c;
    struct mo_chg_bal_c mo_chg_bal_c;

    memset(&td_mst_c, 0x0, sizeof(struct td_mst_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

    ret = Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='DEQX' and \
          parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ��������������!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    max_amt = atof(com_parm_c.val);
    sprintf(acErrMsg,"���仯�Ĳ���Ϊ��[%lf]",max_amt);
    WRITEMSG
    
    ret = Td_mst_Dec_Sel(g_pub_tx.reply,"1=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    while(1)
    {
        ret = Td_mst_Fet_Sel(&td_mst_c, g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        chg_bal = td_mst_c.bal - td_mst_c.ys_bal;
        memset(&mo_chg_bal_c, 0x0, sizeof(struct mo_chg_bal_c));

        if (pub_base_CompDblVal(chg_bal,0.00) < 0 && \
            pub_base_CompDblVal(-chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, td_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = td_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = td_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = td_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = td_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;

            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�����䶯�ϴ����ͳ�Ʊ��в������ݴ���[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }

        if (pub_base_CompDblVal(chg_bal,0.00) > 0 && \
            pub_base_CompDblVal(chg_bal,max_amt) >= 0)
        {
            strcpy(mo_chg_bal_c.br_no, td_mst_c.opn_br_no);
            mo_chg_bal_c.ac_id = td_mst_c.ac_id;
            mo_chg_bal_c.tx_date = g_pub_tx.tx_date;
            mo_chg_bal_c.ac_seqn = td_mst_c.ac_seqn;
            mo_chg_bal_c.y_bal = td_mst_c.ys_bal;
            mo_chg_bal_c.by_bal = td_mst_c.bal;
            mo_chg_bal_c.chg_bal = chg_bal;
         
            ret = Mo_chg_bal_Ins(mo_chg_bal_c, g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�����䶯�ϴ����ͳ�Ʊ��в������ݴ���[%d]",
                        ret);
                WRITEMSG
                return ret;
            }
        }
    }
    Td_mst_Clo_Sel();

    return 0;
}
