/***************************************************************
* �� �� ��: spD252.c
* ��������: ��������ͨ��(�ǿͻ���-���)
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
#include "com_tx_br_rel_c.h"

spD252()
{
    int ret;

    struct mo_city_c mo_city_c; /***ͬ�ǵǼǱ�***/
    struct com_parm_c com_parm_c; /***����������***/
    struct com_tx_br_rel_c com_tx_br_rel_c; /***���׻�����ϵ��****/

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

    get_zd_data("0300", g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );
    get_zd_double("0390",&g_pub_tx.tx_amt1);
    get_zd_data("0670",mo_city_c.bill_type);
    /* get_zd_data("0580",mo_city_c.bill_no); */

    strcpy(mo_city_c.in_out_ind,"2");
    
    
	/*** delelet by xxx ***
    ret = Mo_city_Dec_Upd(g_pub_tx.reply,"payee_ac_no='%s' and in_out_ind='2'\
          and cd_ind='2' and bill_amt='%lf' and sts \
          in ('B','H')", g_pub_tx.ac_no, g_pub_tx.tx_amt1);
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
 
    * �ж��Ƿ��Ǹý��׻������ϼ����� *
    ret = Com_tx_br_rel_Sel(g_pub_tx.reply,&com_tx_br_rel_c,"rel_type=3 \
          and br_no='%s'", mo_city_c.payee_br_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ���׻�����ϵ�����!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    if (strcmp(com_tx_br_rel_c.up_br_no,g_pub_tx.tx_br_no) != 0)
    {
        * �û������Ǹñ�ҵ����ϼ��������ܴ�ӡ *
        sprintf(acErrMsg,"bat_no=[%d],br_no=[%s]",mo_city_c.bat_no,
                mo_city_c.tx_br_no);
        WRITEMSG
		strcpy(g_pub_tx.reply,"D166");
        goto ErrExit;
    }

    get_zd_data("0810",mo_city_c.brf);
    get_zd_data("0820",mo_city_c.remark);
    strcpy(mo_city_c.sts,"P");

    ret = Mo_city_Upd_Upd(mo_city_c, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    Mo_city_Clo_Upd();
	*****/

	/* ��408���Ŀ--�� */
    ret=Com_parm_Sel(g_pub_tx.reply,&com_parm_c,"parm_code='TCKM' and \
        parm_seqn=2");
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�������������!! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

       /****** 
    set_zd_data("1201",com_parm_c.val);
    set_zd_double("1208",g_pub_tx.tx_amt1);
    set_zd_data("1205","2");
    set_zd_data("120A","��������ͨ��");
	*******/

 	strcpy(g_pub_tx.ac_no, com_parm_c.val);
	get_zd_double("1208",&g_pub_tx.tx_amt1);
	get_zd_data("1206",g_pub_tx.note_type);
	get_zd_data("1207",g_pub_tx.beg_note_no);
	get_zd_data("120A",g_pub_tx.brf);
	get_zd_data("1205",g_pub_tx.ct_ind); 
	get_zd_data("1204",g_pub_tx.cur_no); 
	strcpy( g_pub_tx.ac_id_type,"9" ); /*�˻�����Ϊ�ڲ�*/ 
	strcpy( g_pub_tx.ac_get_ind,"00" ); /*������δ��ȡ�ֻ�*/ 
	strcpy( g_pub_tx.ac_wrk_ind,"000" ); /*���˶�ƾ֤�ţ��������ˮ����ϸ*/ 
	strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no); 
	strcpy(g_pub_tx.add_ind,"0");/*�跽��־*/ 
	strcpy( g_pub_tx.prt_ind,"0" ); /*������*/ 
	g_pub_tx.svc_ind=9001;  /*�ڲ��ʴ�ȡ*/ 
	strcpy(g_pub_tx.prt_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /*�ռ�����ϸ*/

	ret=pub_acct_trance();
	if( ret )
	{
		sprintf(acErrMsg,"���ô�ȡ������ʧ��!");
		WRITEMSG
		goto ErrExit;
	}


	/*** �Ǽǽ�����ˮ 
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}***/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��������ͨ�洦��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"��������ͨ�洦��ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

