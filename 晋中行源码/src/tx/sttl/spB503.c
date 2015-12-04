/*************************************************
* �� �� ��:  spB503.c
* ���������� ͬҵ�����Ϣά��
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
#include "com_parm_c.h"

int spB503()
{
    struct mo_ibl_c sMo_ibl_c, sMo_ibl_c1;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
	struct com_parm_c sCom_parm;
    char flag[2];
    char ac_no[20];
    int ret;
  
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));
    memset(&sMo_ibl_c1,0x0,sizeof(struct mo_ibl_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0730",sMo_ibl_c.pact_no);      /*��ͬ��*/
    /* get_zd_data("0230",sMo_ibl_c.prdt_no);      *��Ʒ���*/
    get_zd_data("0250",sMo_ibl_c.op_name);         /*��軧��*/
    get_zd_data("0300",sMo_ibl_c.op_ac_no);     /*�Է��˺�*/
    get_zd_double("0400",&sMo_ibl_c.amt);       /*�����*/
    get_zd_double("0840",&sMo_ibl_c.rate);      /*�������*/
    get_zd_double("0850",&sMo_ibl_c.over_rate); /*��������*/
    get_zd_long("0440",&sMo_ibl_c.matr_date);   /*��������*/
	get_zd_long("0360", &sMo_ibl_c.term);       /* ������� */
    get_zd_data("0670",flag);
    get_zd_data("0310",sMo_ibl_c.kh_ac_no);  /*���뷽�ʺ�(���пͻ��ʺ�)*/
	  get_zd_data("0260",sMo_ibl_c.kh_name);  /*���뷽����(���пͻ���)*/
	  get_zd_data("0520",sMo_ibl_c.op_br_no); /*�����к�*/ 

    strcpy(sMo_ibl_c.br_no, g_pub_tx.tx_br_no);
    sMo_ibl_c.no_pay_bal = sMo_ibl_c.amt; 
    sMo_ibl_c.wrk_date = g_pub_tx.tx_date;
    sMo_ibl_c.trance_no = g_pub_tx.trace_no;
    strcpy(sMo_ibl_c.sts,"1");
    strcpy(sMo_ibl_c.tel,g_pub_tx.tel);
    strcpy(sMo_ibl_c.chk,g_pub_tx.chk);
    strcpy(sMo_ibl_c.auth,g_pub_tx.auth);
    strcpy(sMo_ibl_c.sign,"2");

	/*** delete by rob at 20030706 *****
	if (strlen(ac_no) > 5)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no ='%s'",ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ�Է��˺�id����!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B006");
        	goto ErrExit;
    	}

    	sMo_ibl_c.op_ac_id = sMdm_ac_rel_c.ac_id;
	}
	else
	{
		strcpy(sMo_ibl_c.acc_no, ac_no);
	}
	**********/
	/***     20060921 martin �ӽ������˻���ϵ�����ͻ�����     
	if (strlen(ac_no) > 9)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no ='%s'",ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯ���뷽�˺�id����!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B006");
        	goto ErrExit;
    	}
   	sMo_ibl_c.kh_ac_id = sMdm_ac_rel_c.ac_id;   
	}***/

	/* ��ѯ�����Ʒ���� */
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='TYCC' \
			and parm_seqn = 1"); 
	if (ret != 0) 
	{ 
		sprintf(acErrMsg, "select com_parm error ! [%d]", ret); 
		WRITEMSG 
		goto ErrExit; 
	} 
			
	pub_base_strpack(sCom_parm.val);
	strcpy(sMo_ibl_c.prdt_no, sCom_parm.val);
    
    switch( atoi(flag) )
    {
        case 0 :
            /* ¼�� */
            sprintf(acErrMsg,"¼��!!");
            WRITEMSG
            
            /* ���ú�ͬ���Ƿ��Ѿ����� */
            ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c1,"pact_no='%s'",
                            sMo_ibl_c.pact_no);
            if (ret != 0 && ret != 100)
            {
                sprintf(acErrMsg,"select mo_ibl error [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
            else if (ret == 0)
            {
                sprintf(acErrMsg,"�ú�ͬ���Ѿ�����!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B071");
                goto ErrExit;
            }

            ret = Mo_ibl_Ins(sMo_ibl_c,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"����Ǽǲ��������ݴ���",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "B007");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"ͬҵ�����Ϣ¼��");
            sprintf(acErrMsg,"����Ǽǲ�����pass");
            WRITEMSG

            break;
        case 1 :
            /* �޸� */
            ret = Mo_ibl_Dec_Upd(g_pub_tx.reply,"pact_no = '%s' and sign ='2'",
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
                sprintf(acErrMsg,"û�и����ͬ���ͬ�����!!ret=[%d]",ret);
                WRITEMSG
                Mo_ibl_Clo_Upd();
                strcpy(g_pub_tx.reply,"B003");
                goto ErrExit;
            }

            if (sMo_ibl_c1.sts[0] != '1')
            {
                sprintf(acErrMsg,"��������޸�!!ret=[%d]",ret);
                WRITEMSG
                Mo_ibl_Clo_Upd();
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }
            
	    strcpy(sMo_ibl_c.rowid, sMo_ibl_c1.rowid);
            ret = Mo_ibl_Upd_Upd(sMo_ibl_c, g_pub_tx.reply);
            if ( ret != 0)
            {
                sprintf(acErrMsg,"�޸Ĳ��Ǽǲ�����!! ret =[%d]",ret);
                WRITEMSG
                Mo_ibl_Clo_Upd();
                strcpy(g_pub_tx.reply,"B008");
                goto ErrExit;
            } 

            strcpy(g_pub_tx.brf,"ͬҵ�����Ϣ�޸�");
            Mo_ibl_Clo_Upd();
            sprintf(acErrMsg,"�޸Ĳ��Ǽǲ�pass!! ");
            WRITEMSG
            break;
        case 2 :
            /* ɾ�� */
            ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c1,"pact_no = '%s' and \
                  sign = '2'", sMo_ibl_c.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"û�и����ͬ���ͬ�����!!ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B003");
                goto ErrExit;
            }

            if (sMo_ibl_c1.sts[0] != '1')
            {
                sprintf(acErrMsg,"�������ɾ��!!ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }
            
            ret = sql_execute("delete from mo_ibl where pact_no = '%s' and \
                  sign ='2'", sMo_ibl_c.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"ɾ�����Ǽǲ��м�¼����!! ret =[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B011");
                goto ErrExit;
            }

            strcpy(g_pub_tx.brf,"ͬҵ�����Ϣɾ��");
            sprintf(acErrMsg,"ɾ�����Ǽǲ��м�¼pass!! ret =[%d]",ret);
            WRITEMSG

            break;
        default :
            sprintf(acErrMsg,"�������ͳ���!! type=[%c] 0¼��1�޸�2ɾ��",flag);
            goto ErrExit;
            break;
    }
    
    strcpy(g_pub_tx.prdt_code,sMo_ibl_c.prdt_no);
    strcpy(g_pub_tx.ac_no,ac_no);
    g_pub_tx.tx_amt1 = sMo_ibl_c.amt;

	/******* delete by rob at 20030706
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

