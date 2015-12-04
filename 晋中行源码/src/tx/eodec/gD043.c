/*************************************************************
* �� �� ��: gD043.c
* ������������Ʒ�������ͳ��
*
* ��    ��: mike
* �������: 2003��3��8��
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
#include "trace_log_c.h"
#include "prdt_ac_id_c.h"
#include "ass_prdt_sale_sta_c.h"

gD043()
{
	int ret = 0;
    int flag ; /* 1 ���롡2 ���� */
    long last_trace_no;
    double tx_amt;

    struct trace_log_c trace_log_c;
    struct prdt_ac_id_c prdt_ac_id_c;
    struct ass_prdt_sale_sta_c ass_prdt_sale_sta_c;

    last_trace_no = 0;
    memset(&trace_log_c,0x0,sizeof(struct trace_log_c));
    memset(&prdt_ac_id_c,0x0,sizeof(struct prdt_ac_id_c));
    memset(&ass_prdt_sale_sta_c,0x0,sizeof(struct ass_prdt_sale_sta_c));

	ret=sql_begin(); /*������*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "������ʧ��!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- ��ʼ��ȫ�ֱ��� --------**/
	pub_base_sysinit();

    /* ��ѯҵ����ˮ */
    ret = Trace_log_Dec_Sel(g_pub_tx.reply,"tx_date=%ld order by ac_id,ac_seqn",g_pub_tx.tx_date);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        ret = Trace_log_Fet_Sel(&trace_log_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH FOR SELECT ERROR!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"ҵ����ˮ�еļ�¼ͳ�����!!!");
            WRITEMSG
            break;
        }

        /****
        if (trace_log_c.ac_id == 0)
        {
            sprintf(acErrMsg,"����ˮû���ʺ�!!");
            WRITEMSG
            continue;
        }
        ****/
 
        /* �ж��Ƿ��ǿ������� */
        if ((strcmp(trace_log_c.tx_code,"3101") != 0)  /* ����� */
         && (strcmp(trace_log_c.tx_code,"2101") != 0)  /* �޽��ʻ��ڿ��� */
         && (strcmp(trace_log_c.tx_code,"2102") != 0)  /* �н��ʻ��ڿ��� */
         && (strcmp(trace_log_c.tx_code,"2103") != 0)  /* �޽��ʶ��ڿ��� */
         && (strcmp(trace_log_c.tx_code,"2104")) != 0) /* �н��ʶ��ڿ��� */
        {
            continue;
        }
        
        pub_base_strpack(trace_log_c.sub_tx_code);
        if ((strcmp(trace_log_c.sub_tx_code,"D002") != 0)
          && (strcmp(trace_log_c.sub_tx_code,"L101") != 0))
        {
            sprintf(acErrMsg,"����ˮ���ǿ����ӽ���!!");
            WRITEMSG
            continue;
        }       

        sprintf(acErrMsg,"����ˮ�ǿ����ӽ���!!");
        WRITEMSG

        /* ��ѯ��Ʒ�ʺŶ��ձ� *
        sprintf(acErrMsg,"ac_id=[%ld], ac_seqn = [%d]",trace_log_c.ac_id,
                trace_log_c.ac_seqn);
        WRITEMSG
        ********/

        ret = Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and \
              ac_seqn=%d", trace_log_c.ac_id, trace_log_c.ac_seqn);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�ʺŶ��ձ����!!![%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"��Ʒ�ʺ���û�иü�¼!!!");
            WRITEMSG
            continue;
        }

        TRACE
        strcpy(ass_prdt_sale_sta_c.br_no,trace_log_c.opn_br_no);
        strcpy(ass_prdt_sale_sta_c.prdt_code,prdt_ac_id_c.prdt_no);
        ass_prdt_sale_sta_c.date = g_pub_tx.tx_date;

        TRACE
        ret = Ass_prdt_sale_sta_Sel(g_pub_tx.reply,&ass_prdt_sale_sta_c,
              " prdt_code='%s' and br_no='%s' and \"date\"=%ld ", 
              ass_prdt_sale_sta_c.prdt_code, ass_prdt_sale_sta_c.br_no,
              ass_prdt_sale_sta_c.date);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"��ѯ��Ʒ�������ͳ�Ʊ����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            sprintf(acErrMsg,"���Ʒ�������ͳ�Ʊ��в���һ����¼!!");
            WRITEMSG
            flag = 1; /* ���� */
        }
        else
        {
            sprintf(acErrMsg,"���²�Ʒ�������ͳ���еļ�¼");
            WRITEMSG
            flag = 2; /* ���� */
        }

        TRACE
        if (last_trace_no == trace_log_c.trace_no)
        {
            continue;
        }
        last_trace_no = trace_log_c.trace_no;

        pub_base_strpack(trace_log_c.tx_code);
        if (flag == 1)
        {
            ass_prdt_sale_sta_c.sal_cnt = 1;
            ass_prdt_sale_sta_c.ttl_amt = 0.00;

            ret = sql_sum_double("trace_log","amt",&ass_prdt_sale_sta_c.ttl_amt,
                  "trace_no=%ld", trace_log_c.trace_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"���ܽ����ռǱ��иñʽ��׵ķ�����!!!");
                WRITEMSG
                goto ErrExit;
            } 

            ass_prdt_sale_sta_c.ttl_amt = ass_prdt_sale_sta_c.ttl_amt / 2;

            ret = Ass_prdt_sale_sta_Ins(ass_prdt_sale_sta_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"���Ʒ�������ͳ�Ʊ��в������ݴ���!!");
                WRITEMSG
                goto ErrExit;
            }
        }
        else
        {

            ass_prdt_sale_sta_c.sal_cnt += 1;

            ret = sql_sum_long("trace_log","amt",&tx_amt,
                  "trace_no=%ld", trace_log_c.trace_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"���ܽ����ռǱ��иñʽ��׵ķ�����!!!");
                WRITEMSG
                goto ErrExit;
            } 
            ass_prdt_sale_sta_c.ttl_amt += tx_amt / 2;

            ret = pub_prdt_sale_sta_upd(ass_prdt_sale_sta_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"�������²�Ʒ�������ͳ�Ʊ����!!");
                WRITEMSG
                goto ErrExit;
            }
        }
    }   
    Trace_log_Clo_Sel();

OkExit:
	sql_commit();	/*--�ύ����--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��Ʒ�������ͳ�Ƴɹ�!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    Trace_log_Clo_Sel();
	sql_rollback();	/*--����ع�--*/
	sprintf(acErrMsg,"��Ʒ�������ͳ��ʧ��!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
