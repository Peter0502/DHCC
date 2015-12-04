/***************************************************************
* �� �� ��: spD221.c
* ��������: ����¼��(�Զ�)
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

spD221()
{
    int ret;

    struct mo_city_c mo_city_c,mo_city_c1;
    struct com_parm_c com_parm_c;

    memset(&g_pub_tx, 0x0, sizeof(g_pub_tx));
    memset(&mo_city_c, 0x0, sizeof(struct mo_city_c));
    memset(&mo_city_c1, 0x0, sizeof(struct mo_city_c));
    memset(&com_parm_c, 0x0, sizeof(struct com_parm_c));

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
	/*******
    get_zd_data("0670",mo_city_c.bill_type);
    get_zd_data("0580",mo_city_c.bill_no);
	********/
    get_zd_data("0680",mo_city_c.bill_type);
    strcpy(mo_city_c.cd_ind,"2");
    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);

    strcpy(mo_city_c.in_out_ind,"1");
	strcpy(mo_city_c.cd_ind,"2");

    /* ֻ��������� */
    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"tel='%s'and payer_ac_no='%s' and \
          in_out_ind='1' and cd_ind='2' and bill_amt=%lf and sts='0'",
          g_pub_tx.tel, mo_city_c.payer_ac_no, mo_city_c.bill_amt);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_city_Fet_Upd(&mo_city_c1,g_pub_tx.reply);
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
 
    if (mo_city_c1.tx_date != g_pub_tx.tx_date)
    {
        sprintf(acErrMsg,"���ǵ��콻�ײ�׼¼��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D123");
        goto ErrExit;
    }

    if (strcmp(mo_city_c1.tx_br_no,g_pub_tx.tx_br_no) != 0)
    {
        sprintf(acErrMsg,"����ͬһ���������������ñ�ҵ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"D124");
        goto ErrExit;
    }

    if (strcmp(mo_city_c1.tel,g_pub_tx.tel) != 0)
    {
        sprintf(acErrMsg,"�ù�Ա���������ñ�ҵ��!! [%s]",g_pub_tx.tel);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D125");
        goto ErrExit;
    }

    strcpy(mo_city_c1.trans_mode,"2");
    strcpy(mo_city_c1.bill_type,mo_city_c.bill_type);
    strcpy(mo_city_c1.payer_ac_no,mo_city_c.payer_ac_no);
    strcpy(mo_city_c1.payer_name,mo_city_c.payer_name);
    strcpy(mo_city_c1.payer_br_name,mo_city_c.payer_br_name);
    strcpy(mo_city_c1.payee_ac_no,mo_city_c.payee_ac_no);
    strcpy(mo_city_c1.payee_name,mo_city_c.payee_name);
    strcpy(mo_city_c1.payee_br_name,mo_city_c.payee_br_name);
    /* strcpy(mo_city_c1.bill_no,mo_city_c.bill_no); */
    strcpy(mo_city_c1.brf,mo_city_c.brf);
    strcpy(mo_city_c1.remark,mo_city_c.remark);
    strcpy(mo_city_c1.sts,"6");
vtcp_log("AAASSS");
	g_pub_tx.ac_seqn = mo_city_c1.tx_no%30000;
vtcp_log("AAAAA[%d]",g_pub_tx.ac_seqn);
vtcp_log("AAAAB[%d]",g_pub_tx.svc_ind);
    mo_city_c1.tx_no = g_pub_tx.trace_no;

    /* �������κ� */
    ret = sql_max_int("mo_city","bat_no","trans_mode='2' and \
			in_out_ind='1'",g_pub_tx.tx_date);
    if (ret < 0)
    {
        sprintf(acErrMsg,"��ѯͬ�ǵǼǲ�!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    mo_city_c1.bat_no = ret + 1;

    /* �������κ��Ƿ��Ѵ��ڲ���״̬Ϊ6�� 
    while(1)
    {
        ret = sql_count("mo_city","bat_no=%d and sts='6'",mo_city_c1.bat_no);
        if (ret < 0)
        {
            sprintf(acErrMsg,"ͳ��ͬ�ǵǼǲ�����!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret > 0)
        {
            mo_city_c1.bat_no += 1;
            continue;
        }
        else
        {
            break;
        }
    }
	*/

    ret = Mo_city_Upd_Upd(mo_city_c1, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();

TRACE
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

	/* ���� 40801*/
    set_zd_data("1211",com_parm_c.val);
    set_zd_double("1218",mo_city_c.bill_amt);
    set_zd_data("1215","2");
    set_zd_data("121A","�Զ�����¼�����");

    memset(&com_parm_c,0x0,sizeof(struct com_parm_c));
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=1");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	/* �跽 26102*/
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",mo_city_c.bill_amt);
    set_zd_data("1205","2");
    set_zd_data("120A","�Զ�����¼�����");
    
	/*** �Ǽǽ�����ˮ ***/
	strcpy(g_pub_tx.brf, "�Զ����¼��");
	strcpy(g_pub_tx.no_show, "1");
	g_pub_tx.tx_amt1=0.00;	
vtcp_log("AAAAA[%d]",g_pub_tx.ac_seqn);
vtcp_log("AAAAB[%d]",g_pub_tx.svc_ind);
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����¼��(�Զ�)����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"����¼��(�Զ�)����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

