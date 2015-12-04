/*************************************************
* �� �� ��:     pub_ass_cp_stat_upd.c
* ����������    
* ��    ��:     xxx
* ������ڣ�    2003��03��07��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ass_cp_stat_c.h"
#include "prdt_ac_id_c.h"
#include "cif_mger_rel_c.h"
#include "ass_mger_ach_stat_c.h"
#include "ln_reg_c.h"
#include "ass_prdt_sale_sta_c.h"
#include "ass_tel_pflio_stat_c.h"

/**********************************************************************
 * �� �� ����   pub_ass_cp_stat_upd
 * �������ܣ�   ����������ҵ����ͳ�Ʊ�
 * ����/ʱ�䣺  xxx/20030307
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
int pub_ass_cp_stat_upd(struct ass_cp_stat_c ass_cp_stat_c,char *reply)
{
    int ret;

    struct ass_cp_stat_c ass_cp_stat;

    memset(&ass_cp_stat,0x0,sizeof(struct ass_cp_stat_c));

    ret = Ass_cp_stat_Dec_Upd(reply, "gsp_no='%s' and prdt_code='%s' \
          and date=%ld", ass_cp_stat_c.gsp_no, ass_cp_stat_c.prdt_code,
          ass_cp_stat_c.date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Ass_cp_stat_Fet_Upd(&ass_cp_stat,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        Ass_cp_stat_Clo_Upd();
        return ret;
    }
  
    ret = Ass_cp_stat_Upd_Upd(ass_cp_stat_c,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR!![%d]",ret);
        WRITEMSG
        Ass_cp_stat_Clo_Upd();
        return ret;
    }
    Ass_cp_stat_Clo_Upd();
 
    return (0);
}
/**********************************************************************
 * �� �� ����   pub_ass_ttl_upd
 * �������ܣ�   ͳ�Ʋ�����������ҵ����ͳ�Ʊ��е��������ܻ���
 * ����/ʱ�䣺  xxx/20030307
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
int pub_ass_ttl_upd()
{
    int ret;
    int flag;
    
    struct ass_cp_stat_c ass_cp_stat_c;
    struct dd_mst_c dd_mst_c;
    struct td_mst_c td_mst_c;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct cif_mger_rel_c cif_mger_rel_c;

    memset(&ass_cp_stat_c,0x0,sizeof(struct ass_cp_stat_c));
    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&td_mst_c,0x0,sizeof(struct td_mst_c));
    memset(&prdt_ac_id_c,0x0,sizeof(struct prdt_ac_id_c));
    memset(&cif_mger_rel_c,0x0,sizeof(struct cif_mger_rel_c));

    /* ��ѯ�����ʻ����ļ� */
    ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"cif_no in( select cif_no from  Cif_mger_rel ) order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Dd_mst_Fet_Sel(&dd_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        if (dd_mst_c.ac_sts[0] == '*')
        {
            continue;
        }

        /* ��ѯ��Ʒ�ʺŶ��ձ� */
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", dd_mst_c.ac_id, dd_mst_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����޴˼�¼!!!");
            WRITEMSG
            continue;
        }

        /* ���ݿͻ��Ų�ѯ�ͻ�-�ͻ������ϵ�� */
        ret = Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel_c,"cif_no=%ld and \
              mang_type='1'", prdt_ac_id_c.cif_no);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�-�ͻ������ϵ�����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"���˻�û��������!!");
            WRITEMSG
            continue;
        }

        /* ���ÿͻ������Ƿ��� */
        if (cif_mger_rel_c.canl_date != 0 && \
            cif_mger_rel_c.canl_date < g_pub_tx.tx_date)
        {
            sprintf(acErrMsg,"�ÿͻ������Ѿ����!!");
            WRITEMSG
            continue;
        }

        strcpy(ass_cp_stat_c.gsp_no,cif_mger_rel_c.mang);
        ass_cp_stat_c.date = g_pub_tx.tx_date;
        strcpy(ass_cp_stat_c.prdt_code,prdt_ac_id_c.prdt_no);

        /* ����������ҵ����ͳ�Ʊ� */
        ret = Ass_cp_stat_Sel(g_pub_tx.reply,&ass_cp_stat_c,"gsp_no='%s' \
              and prdt_code='%s' and date=%ld", ass_cp_stat_c.gsp_no,
              ass_cp_stat_c.prdt_code, ass_cp_stat_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ������ҵ����ͳ�Ʊ����!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��������ҵ����ͳ�Ʊ��в���һ����¼!!");
            WRITEMSG
            flag = 1; /* ���� */
        }
        else
        {
            sprintf(acErrMsg,"����������ҵ��ͳ�Ʊ��еļ�¼");
            WRITEMSG
            flag = 2; /* ���� */
        }
        if (flag == 1)
        {
            ass_cp_stat_c.opn_ac_amt = 0.00;
            ass_cp_stat_c.opn_ac_cnt = 0;
            ass_cp_stat_c.cls_ac_amt = 0.00;
            ass_cp_stat_c.cls_ac_cnt = 0;

            ass_cp_stat_c.ttl_amt = dd_mst_c.bal;
            ass_cp_stat_c.ttl_cnt = 1;
            ret = Ass_cp_stat_Ins(ass_cp_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��������ҵ����ͳ�Ʊ��в������ݴ���!!");
                WRITEMSG
                return ret;
            }
        }
        else
        {
            ass_cp_stat_c.ttl_amt += dd_mst_c.bal;
            ass_cp_stat_c.ttl_cnt += 1; 
            ret = pub_ass_cp_stat_upd(ass_cp_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��������������ҵ����ͳ�Ʊ����!!");
                WRITEMSG
                return ret;
            }
        }
    }

    /* ��ѯ�����ʻ����ļ� */
    ret = Td_mst_Dec_Sel(g_pub_tx.reply,"cif_no in( select cif_no from  Cif_mger_rel ) order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Td_mst_Fet_Sel(&td_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        if ( td_mst_c.ac_sts[0] == '*' )
        {
            continue;
        }

        /* ��ѯ��Ʒ�ʺŶ��ձ� */
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", td_mst_c.ac_id, td_mst_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����޴˼�¼!!");
            WRITEMSG
            continue;
        }

        /* ���ݿͻ��Ų�ѯ�ͻ�-�ͻ������ϵ�� */
        ret = Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel_c,"cif_no=%ld and \
              mang_type='1'", prdt_ac_id_c.cif_no);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�-�ͻ������ϵ�����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"���˻�û��������!!");
            WRITEMSG
            continue;
        }

        /* ���ÿͻ������Ƿ��� */
        if (cif_mger_rel_c.canl_date != 0 && \
            cif_mger_rel_c.canl_date < g_pub_tx.tx_date)
        {
            sprintf(acErrMsg,"�ÿͻ������Ѿ����!!");
            WRITEMSG
            continue;
        }

        strcpy(ass_cp_stat_c.gsp_no,cif_mger_rel_c.mang);
        ass_cp_stat_c.date = g_pub_tx.tx_date;
        strcpy(ass_cp_stat_c.prdt_code,prdt_ac_id_c.prdt_no);

        /* ����������ҵ����ͳ�Ʊ� */
        ret = Ass_cp_stat_Sel(g_pub_tx.reply,&ass_cp_stat_c,"gsp_no='%s' \
              and prdt_code='%s' and date=%ld", ass_cp_stat_c.gsp_no,
              ass_cp_stat_c.prdt_code, ass_cp_stat_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ������ҵ����ͳ�Ʊ����!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��������ҵ����ͳ�Ʊ��в���һ����¼!!");
            WRITEMSG
            flag = 1; /* ���� */
        }
        else
        {
            sprintf(acErrMsg,"����������ҵ��ͳ�Ʊ��еļ�¼");
            WRITEMSG
            flag = 2; /* ���� */
        }
        if (flag == 1)
        {
            ass_cp_stat_c.opn_ac_amt = 0.00;
            ass_cp_stat_c.opn_ac_cnt = 0;
            ass_cp_stat_c.cls_ac_amt = 0.00;
            ass_cp_stat_c.cls_ac_cnt = 0;

            ass_cp_stat_c.ttl_amt = td_mst_c.bal;
            ass_cp_stat_c.ttl_cnt = 1;
            ret = Ass_cp_stat_Ins(ass_cp_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��������ҵ����ͳ�Ʊ��в������ݴ���!!");
                WRITEMSG
                return ret;
            }
        }
        else
        {
            ass_cp_stat_c.ttl_amt += td_mst_c.bal;
            ass_cp_stat_c.ttl_cnt += 1; 
            ret = pub_ass_cp_stat_upd(ass_cp_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��������������ҵ����ͳ�Ʊ����!!");
                WRITEMSG
                return ret;
            }
        }
    }
    return 0;
}

/**********************************************************************
 * �� �� ����   pub_ass_mger_ach_stat_upd
 * �������ܣ�   ���¿ͻ�����ҵ������ͳ�Ʊ�
 * ����/ʱ�䣺  xxx/20030308
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

int pub_ass_mger_ach_stat_upd(struct ass_mger_ach_stat_c ass_mger_ach_stat_c, 
                              char *reply)
{
    int ret;
    struct ass_mger_ach_stat_c ass_mger_ach_stat;
    
    memset(&ass_mger_ach_stat, 0x0, sizeof(struct ass_mger_ach_stat_c));

    ret = Ass_mger_ach_stat_Dec_Upd(reply," mang_no='%s' and br_no='%s' \
          and date=%ld ", ass_mger_ach_stat_c.mang_no, 
          ass_mger_ach_stat_c.br_no, ass_mger_ach_stat_c.date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Ass_mger_ach_stat_Fet_Upd(&ass_mger_ach_stat,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        Ass_mger_ach_stat_Clo_Upd();
        return ret;
    }
  
    ret = Ass_mger_ach_stat_Upd_Upd(ass_mger_ach_stat_c,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        Ass_mger_ach_stat_Clo_Upd();
        return ret;
    }
    Ass_mger_ach_stat_Clo_Upd();

    return 0;
}
/**********************************************************************
 * �� �� ����   pub_ass_mger_upd_saveamt
 * �������ܣ�   ͳ�Ʋ����¿ͻ�����ҵ������ͳ�Ʊ��е��ܴ�����
 * ����/ʱ�䣺  xxx/20030308
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
int pub_ass_mger_upd_saveamt()
{
    int ret;
    int flag;

    struct ass_mger_ach_stat_c ass_mger_ach_stat_c;
    struct dd_mst_c dd_mst_c;
    struct td_mst_c td_mst_c;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct cif_mger_rel_c cif_mger_rel_c;

    memset(&ass_mger_ach_stat_c,0x0,sizeof(struct ass_mger_ach_stat_c));
    memset(&dd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&td_mst_c,0x0,sizeof(struct td_mst_c));
    memset(&prdt_ac_id_c,0x0,sizeof(struct prdt_ac_id_c));
    memset(&cif_mger_rel_c,0x0,sizeof(struct cif_mger_rel_c));

    /* ��ѯ�����ʻ����ļ� */
    ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Dd_mst_Fet_Sel(&dd_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        if (dd_mst_c.ac_sts[0] == '*')
        {
            continue;
        }

        /* ��ѯ��Ʒ�ʺŶ��ձ� */
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", dd_mst_c.ac_id, dd_mst_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����޴˼�¼!!!");
            WRITEMSG
            continue;
        }

        /* ���ݿͻ��Ų�ѯ�ͻ�-�ͻ������ϵ�� */
        ret = Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel_c,"cif_no=%ld and \
              mang_type='1'", prdt_ac_id_c.cif_no);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�-�ͻ������ϵ�����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"���˻�û��������!!");
            WRITEMSG
            continue;
        }
        /* ���ÿͻ������Ƿ��� */
        if (cif_mger_rel_c.canl_date != 0 && \
            cif_mger_rel_c.canl_date < g_pub_tx.tx_date)
        {
            sprintf(acErrMsg,"�ÿͻ������Ѿ����!!");
            WRITEMSG
            continue;
        }
        
        strcpy(ass_mger_ach_stat_c.mang_no,cif_mger_rel_c.mang);
        ass_mger_ach_stat_c.date = g_pub_tx.tx_date;
        strcpy(ass_mger_ach_stat_c.br_no,dd_mst_c.opn_br_no);

        ret = Ass_mger_ach_stat_Sel(g_pub_tx.reply,&ass_mger_ach_stat_c,
              " mang_no='%s' and br_no='%s' and date=%ld ",
              ass_mger_ach_stat_c.mang_no, ass_mger_ach_stat_c.br_no,
              ass_mger_ach_stat_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�����ҵ������ͳ�Ʊ����!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"�ͻ�����ҵ������ͳ�Ʊ��в���һ����¼!!");
            WRITEMSG
            flag = 1; /* ���� */
        }
        else
        {
            sprintf(acErrMsg,"���¿ͻ�����ҵ������ͳ�Ʊ��еļ�¼");
            WRITEMSG
            flag = 2; /* ���� */
        }

        if (flag == 1)
        {
            ass_mger_ach_stat_c.save_amt = dd_mst_c.bal;
            ass_mger_ach_stat_c.ln_amt = 0.00;
            ass_mger_ach_stat_c.ln_prvd_amt = 0.00;
            ass_mger_ach_stat_c.ln_norm_amt = 0.00;
            ass_mger_ach_stat_c.ln_over_amt = 0.00;
            ass_mger_ach_stat_c.ln_stgn_amt = 0.00;
            ass_mger_ach_stat_c.ln_die_amt = 0.00;
            ass_mger_ach_stat_c.intst_pay_intst = 0.00;
            ass_mger_ach_stat_c.intst_lo_intst = 0.00;

            ret = Ass_mger_ach_stat_Ins(ass_mger_ach_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��ͻ�����ҵ������ͳ�Ʊ��в������ݴ���!!");
                WRITEMSG
                return ret;
            }
        }
        else
        {
            ass_mger_ach_stat_c.save_amt += dd_mst_c.bal;
            ret = pub_ass_mger_ach_stat_upd(ass_mger_ach_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�������¿ͻ�����ҵ������ͳ�Ʊ����!!");
                WRITEMSG
                return ret;
            }
        }
    }
    Dd_mst_Clo_Sel();

    /* ��ѯ�����ʻ����ļ� */
    ret = Td_mst_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Td_mst_Fet_Sel(&td_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }

        if ( td_mst_c.ac_sts[0] == '*' )
        {
            continue;
        }

        /* ���ݿͻ��Ų�ѯ�ͻ�-�ͻ������ϵ�� */
        ret = Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel_c,"cif_no=%ld and \
              mang_type='1'", td_mst_c.cif_no);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�-�ͻ������ϵ�����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"���˻�û��������!!");
            WRITEMSG
            continue;
        }

        /* ��ѯ��Ʒ�ʺŶ��ձ� */
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", td_mst_c.ac_id, td_mst_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����޴˼�¼!!");
            WRITEMSG
            continue;
        }
        

        /* ���ÿͻ������Ƿ��� */
        if (cif_mger_rel_c.canl_date != 0 && \
            cif_mger_rel_c.canl_date < g_pub_tx.tx_date)
        {
            sprintf(acErrMsg,"�ÿͻ������Ѿ����!!");
            WRITEMSG
            continue;
        }
       
        strcpy(ass_mger_ach_stat_c.mang_no,cif_mger_rel_c.mang);
        ass_mger_ach_stat_c.date = g_pub_tx.tx_date;
        strcpy(ass_mger_ach_stat_c.br_no,td_mst_c.opn_br_no);

        ret = Ass_mger_ach_stat_Sel(g_pub_tx.reply,&ass_mger_ach_stat_c,
              " mang_no='%s' and br_no='%s' and date=%ld ",
              ass_mger_ach_stat_c.mang_no, ass_mger_ach_stat_c.br_no,
              ass_mger_ach_stat_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�����ҵ������ͳ�Ʊ����!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"�ͻ�����ҵ������ͳ�Ʊ��в���һ����¼!!");
            WRITEMSG
            flag = 1; /* ���� */
        }
        else
        {
            sprintf(acErrMsg,"���¿ͻ�����ҵ������ͳ�Ʊ��еļ�¼");
            WRITEMSG
            flag = 2; /* ���� */
        }

        if (flag == 1)
        {
            ass_mger_ach_stat_c.save_amt = td_mst_c.bal;
            ass_mger_ach_stat_c.ln_amt = 0.00;
            ass_mger_ach_stat_c.ln_prvd_amt = 0.00;
            ass_mger_ach_stat_c.ln_norm_amt = 0.00;
            ass_mger_ach_stat_c.ln_over_amt = 0.00;
            ass_mger_ach_stat_c.ln_stgn_amt = 0.00;
            ass_mger_ach_stat_c.ln_die_amt = 0.00;
            ass_mger_ach_stat_c.intst_pay_intst = 0.00;
            ass_mger_ach_stat_c.intst_lo_intst = 0.00;

            ret = Ass_mger_ach_stat_Ins(ass_mger_ach_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��ͻ�����ҵ������ͳ�Ʊ��в������ݴ���!!");
                WRITEMSG
                return ret;
            }
        }
        else
        {
            ass_mger_ach_stat_c.save_amt += td_mst_c.bal;
            ret = pub_ass_mger_ach_stat_upd(ass_mger_ach_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�������¿ͻ�����ҵ������ͳ�Ʊ����!!");
                WRITEMSG
                return ret;
            }
        }
    }
    Td_mst_Clo_Sel();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_ass_mger_upd_payintst
 * �������ܣ�   ͳ�Ʋ����¿ͻ�����ҵ������ͳ�Ʊ��е��ѻ�Ϣ���
 * ����/ʱ�䣺  xxx/20030308
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
int pub_ass_mger_upd_payintst()
{
    int ret;
    int flag;

    struct ass_mger_ach_stat_c ass_mger_ach_stat_c;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct cif_mger_rel_c cif_mger_rel_c;
    struct ln_reg_c ln_reg_c;
    struct ln_mst_c ln_mst_c;

    memset(&ass_mger_ach_stat_c,0x0,sizeof(struct ass_mger_ach_stat_c));
    memset(&prdt_ac_id_c,0x0,sizeof(struct prdt_ac_id_c));
    memset(&cif_mger_rel_c,0x0,sizeof(struct cif_mger_rel_c));
    memset(&ln_reg_c,0x0,sizeof(struct ln_reg_c));
    memset(&ln_mst_c,0x0,sizeof(struct ln_mst_c));

    ret = Ln_reg_Dec_Sel(g_pub_tx.reply,"1=1 order by ac_id,ac_seqn");
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Ln_reg_Fet_Sel(&ln_reg_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            break;
        }
        
        if (ln_reg_c.type[0] != '2' && ln_reg_c.type[0] != '3' \
         && ln_reg_c.type[0] != '4')
        {
            continue;
        }

        /* ��ѯ��Ʒ�ʺŶ��ձ� */
        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", ln_reg_c.ac_id, ln_reg_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��Ʒ�ʺŶ��ձ����޴˼�¼!!!");
            WRITEMSG
            continue;
        }

        /* ���ݿͻ��Ų�ѯ�ͻ�-�ͻ������ϵ�� */
        ret = Cif_mger_rel_Sel(g_pub_tx.reply,&cif_mger_rel_c,"cif_no=%ld and \
              mang_type='1'", prdt_ac_id_c.cif_no);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�-�ͻ������ϵ�����!! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"���˻�û��������!!");
            WRITEMSG
            continue;
        }

        /* ���ÿͻ������Ƿ��� */
        if (cif_mger_rel_c.canl_date != 0 && \
            cif_mger_rel_c.canl_date < g_pub_tx.tx_date)
        {
            sprintf(acErrMsg,"�ÿͻ������Ѿ����!!");
            WRITEMSG
            continue;
        }
        
        strcpy(ass_mger_ach_stat_c.mang_no,cif_mger_rel_c.mang);
        ass_mger_ach_stat_c.date = g_pub_tx.tx_date;
 
        ret = Ln_mst_Sel(g_pub_tx.reply,&ln_mst_c,"ac_id=%ld and \
              ac_seqn=%d",ln_reg_c.ac_id,ln_reg_c.ac_seqn);
        if (ret != 0)
        {
            sprintf(acErrMsg,"��ѯ�������ļ�����!!");
            WRITEMSG
            continue;
        }

        strcpy(ass_mger_ach_stat_c.br_no,ln_mst_c.opn_br_no);

        ret = Ass_mger_ach_stat_Sel(g_pub_tx.reply,&ass_mger_ach_stat_c,
              " mang_no='%s' and br_no='%s' and date=%ld ",
              ass_mger_ach_stat_c.mang_no, ass_mger_ach_stat_c.br_no,
              ass_mger_ach_stat_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ�ͻ�����ҵ������ͳ�Ʊ����!!");
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"�ͻ�����ҵ������ͳ�Ʊ��в���һ����¼!!");
            WRITEMSG
            flag = 1; /* ���� */
        }
        else
        {
            sprintf(acErrMsg,"���¿ͻ�����ҵ������ͳ�Ʊ��еļ�¼");
            WRITEMSG
            flag = 2; /* ���� */
        }

        if (flag == 1)
        {
            ass_mger_ach_stat_c.save_amt = 0.00;
            ass_mger_ach_stat_c.ln_amt = 0.00;
            ass_mger_ach_stat_c.ln_prvd_amt = 0.00;
            ass_mger_ach_stat_c.ln_norm_amt = 0.00;
            ass_mger_ach_stat_c.ln_over_amt = 0.00;
            ass_mger_ach_stat_c.ln_stgn_amt = 0.00;
            ass_mger_ach_stat_c.ln_die_amt = 0.00;
            ass_mger_ach_stat_c.intst_pay_intst = ln_reg_c.pay_bal;
            ass_mger_ach_stat_c.intst_lo_intst = 0.00;

            ret = Ass_mger_ach_stat_Ins(ass_mger_ach_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"��ͻ�����ҵ������ͳ�Ʊ��в������ݴ���!!");
                WRITEMSG
                return ret;
            }
        }
        else
        {
            ass_mger_ach_stat_c.intst_pay_intst += ln_reg_c.pay_bal;
            ret = pub_ass_mger_ach_stat_upd(ass_mger_ach_stat_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�������¿ͻ�����ҵ������ͳ�Ʊ����!!");
                WRITEMSG
                return ret;
            }
        }
    }
    Ln_reg_Clo_Sel();
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_prdt_sale_sta_upd
 * �������ܣ�   ���²�Ʒ�������ͳ�Ʊ�
 * ����/ʱ�䣺  xxx/20030308
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
int pub_prdt_sale_sta_upd(struct ass_prdt_sale_sta_c ass_prdt_sale_sta_c,
            char *reply)
{
    int ret;

    struct ass_prdt_sale_sta_c ass_prdt_sale_sta;

    memset(&ass_prdt_sale_sta,0x0,sizeof(struct ass_prdt_sale_sta_c));

    ret = Ass_prdt_sale_sta_Dec_Upd(reply, "br_no='%s' and prdt_code='%s' and \
          date=%ld", ass_prdt_sale_sta_c.br_no, ass_prdt_sale_sta_c.prdt_code,
          ass_prdt_sale_sta_c.date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Ass_prdt_sale_sta_Fet_Upd(&ass_prdt_sale_sta,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        Ass_prdt_sale_sta_Clo_Upd();
        return ret;
    }
  
    ret = Ass_prdt_sale_sta_Upd_Upd(ass_prdt_sale_sta_c,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR!![%d]",ret);
        WRITEMSG
        Ass_prdt_sale_sta_Clo_Upd();
        return ret;
    }
    Ass_prdt_sale_sta_Clo_Upd();
 
    return 0;
}
/**********************************************************************
 * �� �� ����   pub_tel_pflio_stat_upd
 * �������ܣ�   ���²���Աҵ����ͳ�Ʊ�
 * ����/ʱ�䣺  xxx/20030308
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
int pub_tel_pflio_stat_upd(struct ass_tel_pflio_stat_c ass_tel_pflio_stat_c,
             char *reply)
{
    int ret;

    struct ass_tel_pflio_stat_c ass_tel_pflio_stat;

    memset(&ass_tel_pflio_stat,0x0,sizeof(struct ass_tel_pflio_stat_c));

    ret = Ass_tel_pflio_stat_Dec_Upd(reply, " tel='%s' and br_no='%s' and \
          date=%ld and trad_code='%s' and ct_ind='%s' ", 
          ass_tel_pflio_stat_c.tel, ass_tel_pflio_stat_c.br_no, 
          ass_tel_pflio_stat_c.date, ass_tel_pflio_stat_c.trad_code, 
          ass_tel_pflio_stat_c.ct_ind);

    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    ret = Ass_tel_pflio_stat_Fet_Upd(&ass_tel_pflio_stat,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH FOR UPDATE ERROR!! [%d]",ret);
        WRITEMSG
        Ass_tel_pflio_stat_Clo_Upd();
        return ret;
    }
  
    ret = Ass_tel_pflio_stat_Upd_Upd(ass_tel_pflio_stat_c,reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR!![%d]",ret);
        WRITEMSG
        Ass_tel_pflio_stat_Clo_Upd();
        return ret;
    }
    Ass_tel_pflio_stat_Clo_Upd();
    return 0;
}
