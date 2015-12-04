/*************************************************
* �� �� ��:  spB504.c
* ���������� ͬҵ�������
*
* ��    ��:  rob
* ������ڣ� 2003��1��17��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"

int spB504()
{
    struct mo_ibl_c sMo_ibl_c,sMo_ibl_c1;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
    struct in_parm_c  sIn_parm;
    char ac_no[20];
    int ret;
  
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));
    memset(&sMo_ibl_c1,0x0,sizeof(struct mo_ibl_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",sMo_ibl_c.pact_no);      /*��ͬ��*/

    /* delete by rob 20030706
	get_zd_data("0230",sMo_ibl_c.prdt_no);      *��Ʒ���*
    get_zd_data("0250",sMo_ibl_c.op_name);         *��軧��*
    get_zd_data("0300",ac_no);                  *�Է��˺�*
    get_zd_double("0400",&sMo_ibl_c.amt);       *�����*
    get_zd_double("0840",&sMo_ibl_c.rate);      *�������*
    get_zd_double("0850",&sMo_ibl_c.over_rate); *��������*
    get_zd_long("0440",&sMo_ibl_c.matr_date);   *��������*


    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no ='%s'",ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�Է��˺�id����!! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B006");
        goto ErrExit;
    }

    sMo_ibl_c.op_ac_id = sMdm_ac_rel_c.ac_id;
	********/

    ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sign = '2' \
          and sts = '1'", sMo_ibl_c.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_ibl_Fet_Upd(&sMo_ibl_c1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"û�и����ͬ���ͬ�������Ѽ���!!ret=[%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B003");
        goto ErrExit;
    }

    strcpy(g_pub_tx.prdt_code,sMo_ibl_c1.prdt_no);
    
	/******* delete by rob at 20030706 
    if(strcmp(sMo_ibl_c1.prdt_no,sMo_ibl_c.prdt_no) != 0)
    {
        sprintf(acErrMsg,"��Ʒ���������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B022");
        goto ErrExit;
    }
    
    strcpy(sMo_ibl_c1.op_name,pub_base_strpack(sMo_ibl_c1.op_name));
    sprintf(acErrMsg,"��軧�� [%s] [%s]",sMo_ibl_c1.op_name,sMo_ibl_c.op_name);
    WRITEMSG
    if(strcmp(sMo_ibl_c1.op_name,sMo_ibl_c.op_name) != 0)
    {
        sprintf(acErrMsg,"��軧���������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B023");
        goto ErrExit;
    }

    if(sMo_ibl_c1.op_ac_id != sMo_ibl_c.op_ac_id)
    {
        sprintf(acErrMsg,"�Է��ʺ��������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B024");
        goto ErrExit;
    }

     
    if(pub_base_CompDblVal(sMo_ibl_c1.amt, sMo_ibl_c.amt) != 0)
    {
        sprintf(acErrMsg,"������������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B025");
        goto ErrExit;
    }

    if(pub_base_CompDblVal(sMo_ibl_c1.rate, sMo_ibl_c.rate) != 0)
    {
        sprintf(acErrMsg,"��������������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B026");
        goto ErrExit;
    }

    if(pub_base_CompDblVal(sMo_ibl_c1.over_rate, sMo_ibl_c.over_rate) != 0)
    {
        sprintf(acErrMsg,"���������������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B027");
        goto ErrExit;
    }

    if(sMo_ibl_c1.matr_date !=  sMo_ibl_c.matr_date)
    {
        sprintf(acErrMsg,"���������������");
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B028");
        goto ErrExit;
    }
	*******/

    strcpy(sMo_ibl_c1.sts,"2");
    sMo_ibl_c1.wrk_date = g_pub_tx.tx_date;
    sMo_ibl_c1.trance_no = g_pub_tx.trace_no;
    strcpy(sMo_ibl_c1.tel, g_pub_tx.tel);
    strcpy(sMo_ibl_c1.chk, g_pub_tx.chk);
    strcpy(sMo_ibl_c1.auth,g_pub_tx.auth);

    ret = Mo_ibl_Upd_Upd(sMo_ibl_c1, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"���²��Ǽǲ�����!! ret =[%s]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B008");
        goto ErrExit;
    }
    Mo_ibl_Clo_Upd();

	memcpy(&sMo_ibl_c, &sMo_ibl_c1, sizeof(struct mo_ibl_c));

    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no = '%s'",
          sMo_ibl_c1.prdt_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�ڲ��˲�Ʒ�����",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W058");
        goto ErrExit;
    }

    /* ������ */
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");
    strcpy(g_pub_tx.name, sMo_ibl_c.op_name);
    g_pub_tx.rate = sMo_ibl_c.rate;
    g_pub_tx.over_rate = sMo_ibl_c.over_rate;
	g_pub_tx.svc_ind = 9000;

    ret = pub_acct_opn_in_mdm();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ʳ���");
        WRITEMSG
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"ac_no=[%s],ac_id=[%ld]",g_pub_tx.ac_no,g_pub_tx.ac_id);
    WRITEMSG
    sprintf(acErrMsg,"�����ʳɹ� PASS!!! ");
    WRITEMSG

    /* ���� */
    /*strcpy(g_pub_tx.ac_id_type,"9");*/
    g_pub_tx.ac_id_type[0] = '9';
    g_pub_tx.ac_id_type[1] = '\0';
    g_pub_tx.add_ind[0] = '0'; /* �� */
    g_pub_tx.ac_seqn = 0;
    g_pub_tx.tx_amt1 = sMo_ibl_c1.amt;
    g_pub_tx.hst_ind[0] = '1';
	strcpy(g_pub_tx.ct_ind, "2");
    strcpy(g_pub_tx.brf,"ͬҵ�������");

    if(pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"��������");
        WRITEMSG
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"�����ɹ� PASS!!! ac_no=[%s] ",g_pub_tx.ac_no);
    WRITEMSG

    /* �޸Ĳ��Ǽǲ� */
    ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sts = '2'",
                  sMo_ibl_c.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"dec for update error !! ret=[%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_ibl_Fet_Upd(&sMo_ibl_c1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"fecth for update error !! [%d]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        goto ErrExit;
    }
    sMo_ibl_c1.ac_id = g_pub_tx.ac_id;
    sMo_ibl_c1.opn_date = g_pub_tx.tx_date;
    sMo_ibl_c1.trance_no = g_pub_tx.trace_no;

    ret = Mo_ibl_Upd_Upd(sMo_ibl_c1, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"���²��Ǽǲ�����!! ret =[%s]",ret);
        WRITEMSG
        Mo_ibl_Clo_Upd();
        strcpy(g_pub_tx.reply,"B008");
        goto ErrExit;
    }
    Mo_ibl_Clo_Upd();

    set_zd_data("0310", g_pub_tx.ac_no);

    set_zd_double("0410",g_pub_tx.tx_amt1);

	/***** delete by rob at 20030706 
    * �Է��˻�ȡ�� *
    g_pub_tx.add_ind[0] = '0';
    strcpy(g_pub_tx.ac_no,ac_no);
    g_pub_tx.ac_id = sMo_ibl_c1.op_ac_id;
    g_pub_tx.ac_seqn = 0;

    if (pub_acct_trance())
    {
        sprintf(acErrMsg,"�Է��˻�ȡ�����");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"g_in_mst.bal=[%lf]",g_in_mst.bal);
    WRITEMSG
    set_zd_double("0420",g_in_mst.bal);

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!");
        WRITEMSG
        goto ErrExit;
    }
	*********/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

