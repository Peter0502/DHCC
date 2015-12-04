/***************************************************************
* �� �� ��: spD251.c
* ��������: �����������(�ǿͻ���-���)
*
* ��    ��: jack
* �������: 2004��04��01��
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

spD251()
{
    int ret;
	char flag[2];
	char note_type[2];
	char ac_no[20];

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

    get_zd_data("0690",flag);
	if (flag[0] == '1')
	{
    	get_zd_data("0300",g_pub_tx.ac_no);
		strcpy(ac_no, g_pub_tx.ac_no);
	}
	else
	{
    	get_zd_data("0310",g_pub_tx.ac_no);
		get_zd_data("0300", ac_no);
	}

    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",note_type);
    /* get_zd_data("0580",mo_city_c.bill_no); */

    strcpy(mo_city_c.in_out_ind,"2");
    
    
    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payee_ac_no='%s' and in_out_ind='2'\
          and cd_ind='2' and bill_amt=%lf and sts in ('C', 'I')",
		  ac_no,g_pub_tx.tx_amt1);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_city_Fet_Upd(&mo_city_c,g_pub_tx.reply);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"ͬ�ǵǼǲ���û�еǼǸñ�ҵ��! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D122");
        goto ErrExit;
    }

    if (mo_city_c.sts[0]=='O' || mo_city_c.sts[0] == 'P')
    {
        sprintf(acErrMsg,"��Ʊ�Ѿ�����!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D152");
        goto ErrExit;
    }

    if (strcmp(mo_city_c.payee_br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"�����㲻�������ñ�ҵ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D151");
        goto ErrExit;
    }


    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
    strcpy(mo_city_c.sts,"O");
	mo_city_c.tc_date = g_pub_tx.tx_date;

    ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();

    /* �ͻ��ʻ���� */
    g_pub_tx.ac_seqn = 0;
    set_zd_int("0350",g_pub_tx.ac_seqn);
	/* set_zd_data("0300",ac_no); */

    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=3");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

        set_zd_data("1201",com_parm_c.val);
        set_zd_double("1208",g_pub_tx.tx_amt1);
        set_zd_data("1205","2");
        set_zd_data("120A","�����������");

	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�����������");
	g_pub_tx.ac_id = mo_city_c.tx_no;
	g_pub_tx.tx_amt1 = mo_city_c.tx_date;
	strcpy(g_pub_tx.no_show, "1");
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����������ʴ���ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����������ʴ���ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

