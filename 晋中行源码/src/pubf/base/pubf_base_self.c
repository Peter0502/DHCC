#include "find_debug.h" 
/***************************************************************
* �� �� ��:     pubf_base_self.c
* ����������    �����豸�Ĺ��ú���
* ���ú���˵����
*
*
* ��    ��:     rob
* ������ڣ�    2003��02��18��
*
* �޸ļ�¼��
* 1. ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_tradelimit_c.h"
#include "chnl_self_trade_c.h"
#include "march_info_c.h"

/**********************************************************************
* ��������    pub_base_InsertTradeAdd()
* �������ܣ�  ���׼�¼�ۼƿ��Ʊ������Ӽ�¼
* ����/ʱ�䣺 rob/2003��02��18��
*
* ������
*     ���룺
*     ���:
* �� �� ֵ: 0 �ɹ�
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_InsertTradeAdd(char tx_type[2])
{
    int ret;
    char wherelist[100];
    struct chnl_tradeadd_c chnl_tradeadd;

    MEMSET_DEBUG(&chnl_tradeadd,0x0,sizeof(struct chnl_tradeadd_c));
    chnl_tradeadd.tx_date = g_pub_tx.tx_date;
    strcpy(chnl_tradeadd.ac_no,g_pub_tx.crd_no);
    strcpy(chnl_tradeadd.tx_type,tx_type); /* atm */
    strcpy(chnl_tradeadd.stat,"0");    /* ���� */

	/*
    sprintf(wherelist," tx_date = %ld and ac_no = '%s'",
            g_pub_tx.tx_date,g_pub_tx.crd_no);
    sprintf(acErrMsg,"insert tradeadd**** [%s] ****",wherelist);
    WRITEMSG
    ret = sql_count("chnl_tradeadd","%s",wherelist);
    if (ret <= 0)
    {
        * ���׼�¼�ۼƿ��Ʊ������Ӽ�¼ */
     
sprintf(acErrMsg,"insert into chnl_tradeadd begin!!");
WRITEMSG

        chnl_tradeadd.add_pswd_cnt = 0;
        ret = Chnl_tradeadd_Ins(chnl_tradeadd,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"���׼�¼�ۼƿ��Ʊ������Ӽ�¼����!! [%d]",ret);
            WRITEMSG
            return 1;
        }
sprintf(acErrMsg,"insert into chnl_tradeadd end!!");
WRITEMSG

    return 0;
}

/**********************************************************************
* ��������    pub_base_Checktradelimit(char chnl)
* �������ܣ�  ��齻������(ÿ��ȡ���� �ۼ�ȡ��� �ۼƴ���)
* ����/ʱ�䣺 rob/2003��02��18��
*
* ������
*     ���룺
*           0 --atm 1--pos
*     ���:
* �� �� ֵ: 0 �ɹ�  !0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_Checktradelimit(char chnl)
{
    struct chnl_tradelimit_c chnl_tradelimit;
    struct chnl_tradeadd_c chnl_tradeadd_c;
    int ret;
    char tx_type[2];

    MEMSET_DEBUG(&chnl_tradelimit,0x0,sizeof(struct chnl_tradelimit_c));
    MEMSET_DEBUG(&chnl_tradeadd_c,0x0,sizeof(struct chnl_tradeadd_c));

    ret = Chnl_tradelimit_Sel(g_pub_tx.reply,&chnl_tradelimit,
          "chnl='%c'",chnl);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������Ʊ����!!! [%d]",ret);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        return ret;
    }

    if (chnl == '0')
    {
        strcpy(tx_type, "1"); /* atm */
    }
    else if (chnl == '1')
    {
        strcpy(tx_type, "3"); /* pos */
    }
    else
    {
		sprintf(acErrMsg,"�������ʹ���!! [%c]",chnl);
		WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        return 1;
    }
    
    ret = Chnl_tradeadd_Sel(g_pub_tx.reply, &chnl_tradeadd_c,"tx_date=%ld and \
      ac_no='%s' and tx_type ='%s'",g_pub_tx.tx_date,g_pub_tx.crd_no,tx_type);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ���׼�¼�ۼƿ��Ʊ����!! [%d]",ret);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT08");
        return ret;
    }
	else if (ret == 100)
	{
		/* ��chnl_tradeadd��������һ����¼ */
		chnl_tradeadd_c.tx_date = g_pub_tx.tx_date;
		strcpy(chnl_tradeadd_c.ac_no,g_pub_tx.crd_no);
		strcpy(chnl_tradeadd_c.tx_type,tx_type);
		chnl_tradeadd_c.add_amt = 0.00;
		chnl_tradeadd_c.add_qk_cnt = 0;
		chnl_tradeadd_c.add_pswd_cnt = 0;
		strcpy(chnl_tradeadd_c.stat,"0");
		ret = Chnl_tradeadd_Ins(chnl_tradeadd_c, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���׼�¼�ۼƿ��Ʊ������Ӽ�¼����!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "AT29");
			return ret;
		}
	}

    /* ���ȡ���ۼƴ��� */
    if (chnl_tradeadd_c.add_qk_cnt + 1 > chnl_tradelimit.wd_cnt)
    {
        sprintf(acErrMsg,"�ۼ�ȡ���������ÿ�������!! ��ȡ�����:[%d], \
			maxȡ�����:[%d]",chnl_tradeadd_c.add_qk_cnt, 
			chnl_tradelimit.wd_cnt);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT09");
        return 1;
    }

    /* ���ÿ��ȡ����Ƿ񳬳����� */
	ret = pub_base_CompDblVal(g_pub_tx.tx_amt1,chnl_tradelimit.wd_amt);
    if ( ret > 0)
    {
        sprintf(acErrMsg, "ÿ��ȡ��������,���ν��׽��:[%lf], ÿ��max\
			ȡ����:[%lf]",g_pub_tx.tx_amt1, chnl_tradelimit.wd_amt);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT10");
        return 1;
    }

    /* ���ÿ��ȡ���ܶ������ */
	ret = pub_base_CompDblVal(chnl_tradeadd_c.add_amt + g_pub_tx.tx_amt1,
			chnl_tradelimit.wd_ttl_max_amt);
    if (ret > 0)
    {
        sprintf(acErrMsg,"ÿ��ȡ���ܶ���ܵ�����!! ��ȡ����:[%lf], \
			���ν��׽��:[%lf], max��ȡ����:[%lf]", chnl_tradeadd_c.add_amt,
			g_pub_tx.tx_amt1, chnl_tradelimit.wd_ttl_max_amt);
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT11");
        return 1;
    }

    return 0;
}
/**********************************************************************
* ��������    pub_base_Insertselftrade(struct chnl_self_trade_c)
* �������ܣ�  �������豸�����ռ������Ӽ�¼
* ����/ʱ�䣺 rob/2003��02��18��
*
* ������
*     ���룺
*            struct chnl_self_trade_c chnl_self_trade
*     ���:
* �� �� ֵ: 0 �ɹ�  !0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_Insertselftrade(struct chnl_self_trade_c chnl_self_trade)
{
    int ret;

    ret = Chnl_self_trade_Ins(chnl_self_trade,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�������豸�����ռ������Ӽ�¼���� [%d]",ret);
        WRITEMSG
        return ret;
    }
    return 0;
}
/**********************************************************************
* ��������    pub_base_Selmrchacno(char mrchno[],struct march_info_c *)
* �������ܣ�  �����̻��Ų�ѯ�̻���Ϣ�������̻�����Ϣ
* ����/ʱ�䣺 rob/2003��02��18��
*
* ������
*     ���룺
*            marchno �̻���
*     ���:
*            struct march_info_c sMarch_info 
* �� �� ֵ: 0 �ɹ�  !0 ʧ��
*
* �޸���ʷ��
*
********************************************************************/
int pub_base_Selmrchacno(char mrchno[],struct march_info_c *sMarch_info)
{
    int ret;
   
    ret=March_info_Sel(g_pub_tx.reply,sMarch_info,"mrchno='%s'",mrchno);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�̻���Ϣ��������!!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    /* �����̻���״̬ */
    if (strcmp(sMarch_info->mrchstat,"0") != 0)
    {
        sprintf(acErrMsg,"���̻���״̬������!!");
        WRITEMSG
        return 1;
    }

    /* ����Э���Ƿ���Ч */
    if ((g_pub_tx.tx_date < sMarch_info->beg_date) 
       || (g_pub_tx.tx_date >= sMarch_info->end_date))
    {
        sprintf(acErrMsg,"���̻���Э���ѹ���!!!");
        WRITEMSG
        return 1;
    }
    return 0;
}
