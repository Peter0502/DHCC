/***************************************************************
* �� �� ��: spD231.c
* ��������: ����¼��(�ֹ�����)
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

spD231()
{
    int ret;
	char flag[2];

    struct mo_city_c mo_city_c;
    struct com_parm_c com_parm_c;
    struct com_tx_br_rel_c com_tx_br_rel_c;

    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));
    memset(&com_tx_br_rel_c, 0x0, sizeof(struct com_tx_br_rel_c));

	/*** ��ʼ��ȫ�ֱ��� ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"��ʼ��ȫ�ֱ�������!");
		WRITEMSG
		goto ErrExit;
	}

    get_zd_data("0300",g_pub_tx.ac_no);
    get_zd_double("0390",&mo_city_c.bill_amt);
    get_zd_data("0680",mo_city_c.cd_ind);

    strcpy(mo_city_c.in_out_ind,"2");
    strcpy(mo_city_c.trans_mode,"1");

    /*******
    get_zd_data("0670",mo_city_c.bill_type);
    get_zd_data("0580",mo_city_c.bill_no);
	*******/

    get_zd_data("0300",mo_city_c.payer_ac_no);
    get_zd_data("0310",mo_city_c.payee_ac_no);

    if (mo_city_c.cd_ind[0] == '1')
    {
        get_zd_data("0910",mo_city_c.payer_br_no);
        get_zd_data("0830",mo_city_c.payer_br_name);
    }
    else
    {
        get_zd_data("0910",mo_city_c.payee_br_no);
        get_zd_data("0830",mo_city_c.payee_br_name);
    }
	/******
    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
	******/
    strcpy(mo_city_c.sts,"A");

    mo_city_c.tx_no = g_pub_tx.trace_no;
    mo_city_c.tx_date = g_pub_tx.tx_date;
    strcpy(mo_city_c.tx_br_no, g_pub_tx.tx_br_no);
    strcpy(mo_city_c.tel, g_pub_tx.tel);

    get_zd_data("0910",g_pub_tx.ac_no);
	get_zd_data("0790",flag);

	/***
    	ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
              and br_no='%s'", g_pub_tx.ac_no);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d]",ret);
        	WRITEMSG
			strcpy(g_pub_tx.reply,"D167");
        	goto ErrExit;
    	}
		else if (ret == 100)
		{
        	sprintf(acErrMsg,"���׻�����ϵ��û�и�����ŵ��ϼ�����!! [%d]",ret);
        	WRITEMSG
			strcpy(g_pub_tx.reply,"D168");
        	goto ErrExit;
		}

    	if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    	{
        	* �û������Ǹñ�ҵ����ϼ��������ܴ�ӡ *
        	sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                g_pub_tx.ac_no);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"D144");
        	goto ErrExit;
    	}
	***/

    ret = Mo_city_Ins(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
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

    if (mo_city_c.cd_ind[0] == '1')
    {
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",mo_city_c.bill_amt);
        set_zd_data("1205","2");
        set_zd_data("120A","�ֹ�¼�����(����)");
    }
    else
    {
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",mo_city_c.bill_amt);
        set_zd_data("1215","2");
        set_zd_data("121A","�ֹ�¼�����(����)");
    }

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (mo_city_c.cd_ind[0] == '1')
    {
        set_zd_data("1211",com_parm_c.val);
        set_zd_double("1218",mo_city_c.bill_amt);
        set_zd_data("1215","2");
        set_zd_data("121A","�ֹ�¼�����(����)");
    }
    else
    {
        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",mo_city_c.bill_amt);
        set_zd_data("1205","2");
        set_zd_data("120A","�ֹ�¼�����(����)");
    }
    
	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�ֹ�����¼��");
	strcpy(g_pub_tx.no_show, "1");

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ֹ�ͬ����������¼�봦��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�ֹ�ͬ����������¼�봦��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

