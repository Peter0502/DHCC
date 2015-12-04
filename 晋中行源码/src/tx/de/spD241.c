/***************************************************************
* �� �� ��: spD241.c
* ��������: ����¼��(�Զ�)
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
#include "com_tx_br_rel_c.h"

spD241()
{
    int ret;
	char flag[2];

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;
    struct mdm_ac_rel_c mdm_ac_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));
    memset(&mdm_ac_rel_c, 0x0, sizeof(struct mdm_ac_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0300",mo_city_c.payer_ac_no);
    get_zd_data("0250",mo_city_c.payer_name);
    get_zd_data("0270",mo_city_c.payer_br_name);
    get_zd_data("0310",mo_city_c.payee_ac_no);
    get_zd_data("0260",mo_city_c.payee_name);
    get_zd_data("0830",mo_city_c.payee_br_name);
    get_zd_double("0390",&mo_city_c.bill_amt);
    get_zd_data("0670",mo_city_c.bill_type);
	/***
    get_zd_data("0580",mo_city_c.bill_no);
    get_zd_data("0680",mo_city_c.cd_ind);
	****/
	strcpy(mo_city_c.cd_ind,"2");
    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
	get_zd_data("0690",flag);
	get_zd_data("0910",mo_city_c.payee_br_no);

	sprintf(acErrMsg, " -------> [%s]", mo_city_c.payee_br_no);
	WRITEMSG
    strcpy(mo_city_c.in_out_ind,"2");
    strcpy(mo_city_c.trans_mode,"2");

	/********
    if (flag[0] == '0')
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_c,"ac_no='%s'",
          	mo_city_c.payee_ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ�����˺Ź�ϵ�����!! [%d]",ret);
        	WRITEMSG
        	goto ErrExit;
    	}
	}
 
    strcpy(mo_city_c.payee_br_no,mdm_ac_rel_c.opn_br_no);
	*********/

    /* �жϽ��׻����Ƿ��Ǹ�������ϼ����� */
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
		and br_no='%s'", mo_city_c.payee_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d] [%s]",
				ret, mo_city_c.payee_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
        goto ErrExit;
    }
	else if (ret == 100)
	{
        sprintf(acErrMsg,"���׻�����ϵ����û�ж���������Ʊ�ݴ��ݵ�\
				�ϼ�����!! [%d] [%s]",ret, mo_city_c.payee_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D168");
        goto ErrExit;
	}

    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
		and up_br_no='%s'", g_pub_tx.tx_br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d] [%s]",
				ret, g_pub_tx.tx_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D167");
        goto ErrExit;
    }
	else if (ret == 100)
	{
        sprintf(acErrMsg,"�������Ĳ�����¼��!! [%s]", g_pub_tx.tx_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D241");
        goto ErrExit;
	}

	/**** delete by rob at 20030819**
    if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    {
        * �û������Ǹñ�ҵ����ϼ��������ܴ�ӡ *
        sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                mdm_ac_rel_c.opn_br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D144");
        goto ErrExit;
    }
	*****/

    strcpy(mo_city_c.sts,"H");
    mo_city_c.tx_date = g_pub_tx.tx_date;
    mo_city_c.tx_no = g_pub_tx.trace_no;
    strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
    strcpy(mo_city_c.tel,g_pub_tx.tel);

    /* �������κ� */
    ret = sql_max_int("mo_city","bat_no","in_out_ind='2' and \
			trans_mode='2'",g_pub_tx.tx_date);
    if (ret < 0)
    {
        sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    mo_city_c.bat_no = ret + 1;

    /* �������κ��Ƿ��Ѵ��ڲ���״̬Ϊ6�� 
    while(1)
    {
        ret = sql_count("mo_city","bat_no=%d and sts in ('H','I')",
              mo_city_c.bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret > 0)
        {
            mo_city_c.bat_no += 1;
            continue;
        }
        else
        {
            break;
        }
    }*/

    ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"INSERT ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ��408�ڲ������Ŀ */
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",mo_city_c.bill_amt);
    set_zd_data("1215","2");
    set_zd_data("121A","�Զ�����¼��(����)");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    /* ��408�ⲿ�����Ŀ */
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",mo_city_c.bill_amt);
    set_zd_data("1205","2");
    set_zd_data("120A","�Զ�����¼��(����)");
    
	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�Զ�����¼��");
	strcpy(g_pub_tx.no_show, "1");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����¼��(�Զ�����)����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����¼��(�Զ�����)����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

