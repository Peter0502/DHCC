/***************************************************************
* �� �� ��: spD213.c
* ��������: �������(�ֹ�)
*
* ��    ��: jack
* �������: 2004��3��30��
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
#include "com_tx_br_rel_c.h"

spD213()
{
    int ret=0;
    int sum=0;
    char br_no[6];
	double d_amt=0.00, c_amt=0.00;

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
	memset(br_no, 0x0, sizeof(br_no));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    memset(br_no, 0x0, sizeof(br_no));

    get_zd_data("0910",br_no);
    get_zd_long("0480",&mo_city_c.bat_no);
    /* get_zd_data("0670",mo_city_c.bill_type); */
    get_zd_int("0500",&sum);
    get_zd_double("0390",&g_pub_tx.tx_amt1);

    strcpy(mo_city_c.in_out_ind,"1");
    
    /* ���ý��׻����Ƿ��Ǹ�������ϼ����� */
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
              and br_no='%s'", br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    {
        /* �û������Ǹñ�ҵ����ϼ��������ܴ�ӡ */
        sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no, br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D144");
        goto ErrExit;
    }

	/*******
    ret = sql_count("mo_city","tx_br_no='%s' and bat_no=%d and sts='2'", 
			br_no, mo_city_c.bat_no);
    if (ret < 0)
    {
        sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	else if (ret == 0)
	{
		sprintf(acErrMsg,"û�и���Ʊ��!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D160");
		goto ErrExit;
	}

    if (sum != ret)
    {
        sprintf(acErrMsg,"��������ȷ!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D127");
        goto ErrExit;
    }
	******/

	d_amt = 0.00;
   	ret = sql_sum_double("mo_city","bill_amt",&d_amt, 
		"tx_br_no='%s' and bat_no = %d and sts='2' and trans_mode='1' \
		and in_out_ind='1' and cd_ind='2'", br_no, mo_city_c.bat_no);
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�������!! [%d]",ret);
       	WRITEMSG
       	goto ErrExit;
   	}

	sprintf(acErrMsg, "����������Ϊ: [%lf]", d_amt);
	WRITEMSG

	c_amt = 0.00;
   	ret = sql_sum_double("mo_city","bill_amt",&c_amt, 
		"tx_br_no='%s' and bat_no = %d and sts='2' and trans_mode='1' \
		and in_out_ind='1' and cd_ind='1'", br_no, mo_city_c.bat_no);
   	if (ret != 0)
   	{
       	sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�������!! [%d]",ret);
       	WRITEMSG
       	goto ErrExit;
   	}

	sprintf(acErrMsg, "����跽���Ϊ: [%lf]", c_amt);
	WRITEMSG

	mo_city_c.bill_amt = d_amt - c_amt;
   	if (pub_base_CompDblVal(mo_city_c.bill_amt, g_pub_tx.tx_amt1) != 0)
   	{
       	sprintf(acErrMsg,"����ȷ!! ������=[%lf] ���ܽ��=[%lf]",
               g_pub_tx.tx_amt1, mo_city_c.bill_amt);
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"D128");
    	goto ErrExit;
   	}

    ret = sql_execute("update mo_city set sts='3' , tc_date=%ld where \
		tx_br_no='%s' and bat_no = %d and sts='2' and trans_mode='1' and \
		in_out_ind='1'", g_pub_tx.tx_date, br_no, mo_city_c.bat_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"����ͬ�ǵǼǲ���״̬����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�ֹ����Ʊ�ݺ���");
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
	sprintf(acErrMsg,"�����������ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�����������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

