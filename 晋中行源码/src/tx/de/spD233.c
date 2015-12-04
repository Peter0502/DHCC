/***************************************************************
* �� �� ��: spD233.c
* ��������: ��������(�ֹ�)
*
* ��    ��: jack
* �������: 2004��3��31��
*
* �޸ļ�¼��
*    ��    ��:
*    �� �� ��:
*    �޸�����:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_city_c.h"
#include "com_parm_c.h"

spD233()
{
    int ret;
    int sum;
	double amt1,amt2;

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}


    get_zd_long("0480",&mo_city_c.bat_no);
	/********
    get_zd_data("0670",mo_city_c.bill_type);
    get_zd_int("0500",&sum);
	********/
    get_zd_double("0390",&g_pub_tx.tx_amt1);

    strcpy(mo_city_c.in_out_ind,"2");

		/********
    if (mo_city_c.bill_type[0] == '6')
    {
        ret = sql_count("mo_city","payer_br_no='%s' and bat_no=%d and \
              sts='B'", g_pub_tx.tx_br_no, mo_city_c.bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        if (sum != ret)
        {
            sprintf(acErrMsg,"��������ȷ!! [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D127");
            goto ErrExit;
        }
		***********/

        ret = sql_sum_double("mo_city","bill_amt",&amt1,
          	"payee_br_no='%s' and cd_ind='2' and bat_no = %d and sts='B'",
          	g_pub_tx.tx_br_no, mo_city_c.bat_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�������!! [%d]",ret);
            WRITEMSG
			strcpy(g_pub_tx.reply,"D174");
            goto ErrExit;
        }

	/*********
        if (pub_base_CompDblVal(mo_city_c.bill_amt, g_pub_tx.tx_amt1) != 0)
        {
            sprintf(acErrMsg,"����ȷ!! ������=[%lf] ���ܽ��=[%lf]",
               g_pub_tx.tx_amt1, mo_city_c.bill_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D128");
            goto ErrExit;
        }

        ret = sql_execute("update mo_city set sts='C' where payer_br_no='%s' \
              and bat_no = %d and bill_type='%s' and sts='B'",
              g_pub_tx.tx_br_no,mo_city_c.bat_no, mo_city_c.bill_type);
        if (ret != 0)
        {
            sprintf(acErrMsg,"����ͬ�ǵǼǲ���״̬����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
    }
    else
    {
        ret = sql_count("mo_city","payee_br_no='%s' and bat_no=%d and \
              bill_type='%s' and sts='B'", g_pub_tx.tx_br_no, mo_city_c.bat_no, 
              mo_city_c.bill_type);
        if (ret < 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

        if (sum != ret)
        {
            sprintf(acErrMsg,"��������ȷ!! [%d]",ret);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D127");
            goto ErrExit;
        }
	***********/

	sprintf(acErrMsg,"����������Ϊ:[%lf]",amt1);
	WRITEMSG

        ret = sql_sum_double("mo_city","bill_amt",&amt2, "payer_br_no='%s' \
				and bat_no = %d and cd_ind='1' and sts='B'",
          		g_pub_tx.tx_br_no, mo_city_c.bat_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�������!! [%d]",ret);
            WRITEMSG
			strcpy(g_pub_tx.reply,"D175");
            goto ErrExit;
        }

	sprintf(acErrMsg,"����跽���Ϊ:[%lf]",amt2);
	WRITEMSG

		if (pub_base_CompDblVal(amt1,amt2) >= 0)
		{
			mo_city_c.bill_amt = amt1 - amt2;
		}
		else if (pub_base_CompDblVal(amt1,amt2) < 0)
		{
			mo_city_c.bill_amt = amt2 - amt1;
		}

	sprintf(acErrMsg,"���ܽ��Ϊ:[%lf]",mo_city_c.bill_amt);
	WRITEMSG

        if (pub_base_CompDblVal(mo_city_c.bill_amt, g_pub_tx.tx_amt1) != 0)
        {
            sprintf(acErrMsg,"����ȷ!! ������=[%lf] ���ܽ��=[%lf]",
               g_pub_tx.tx_amt1, mo_city_c.bill_amt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"D128");
            goto ErrExit;
        }

        ret = sql_execute("update mo_city set sts='C' where bat_no = %d \
				and sts='B'",mo_city_c.bat_no);
        if (ret != 0)
        {
            sprintf(acErrMsg,"����ͬ�ǵǼǲ���״̬����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }

	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�ֹ������������");
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.ac_seqn = mo_city_c.bat_no;
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������(����)����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��������(����)����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

