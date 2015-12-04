/*************************************************
* �� �� ��:  spB702.c
* ���������� ֤ȯ�ع�����������
*
* ��    ��:  
* ������ڣ� 2003��1��29��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"
#include "com_parm_c.h"
#include "mbfe_pub.h"
#include "mo_city_c.h"

int spB702()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
	struct com_parm_c	sCom_parm;
	struct mbfeghvsendjrn_c mbfeghvsendjrn_v;
	struct mo_city_c	mo_city_c;
    char flag[2],zzlx[2];
	long   tmp_date=0,tmp_trace_no=0;
	double tmp_amt=0.00;
    int ret;
  
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot1,0x0,sizeof(struct mo_stok_rebuy_prot_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mbfeghvsendjrn_v,0x00,sizeof(struct mbfeghvsendjrn_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no);
    get_zd_data("0700",sMo_stok_rebuy_prot.tcht_type);
    get_zd_double("0390",&sMo_stok_rebuy_prot.par_value);
    get_zd_double("0940",&sMo_stok_rebuy_prot.pledge_rate);
    get_zd_data("0680",sMo_stok_rebuy_prot.buy_type);			/*** 1-�����2-���� ***/
    get_zd_double("0400",&sMo_stok_rebuy_prot.bal);
    get_zd_long("0440",&sMo_stok_rebuy_prot.matr_date);
    get_zd_long("0480",&sMo_stok_rebuy_prot.term);
    get_zd_double("0850",&sMo_stok_rebuy_prot.rate);
    get_zd_data("0690",flag);  /*1 �ֽ� 2ת��*/
    get_zd_data("0210",g_pub_tx.cur_no);
    get_zd_data("0890",g_pub_tx.note_type);
    get_zd_data("0580",g_pub_tx.beg_note_no);
    get_zd_data("0580",g_pub_tx.end_note_no);
    get_zd_data("0710",g_pub_tx.tmp_ind);
    get_zd_data("0720",zzlx);   /*ͬ�� ���� */
    get_zd_data("0270",sMo_stok_rebuy_prot.opt_name);

    if (flag[0] == '2')
    {
        get_zd_data("0300",sMo_stok_rebuy_prot.opt_acno);
    }
 
    /* ����������Ϣ�͵Ǽǲ��е���Ϣ�Ƿ�һ�� */
    ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
          "pact_no='%s' and sts='1'",sMo_stok_rebuy_prot.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ֤ȯ�ع�Э��Ǽǲ�����! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B091");
        goto ErrExit;
    }
   
    /* ���֤ȯ���� */
    strcpy(sMo_stok_rebuy_prot1.tcht_type,
             pub_base_strpack(sMo_stok_rebuy_prot1.tcht_type));
    if(strcmp(sMo_stok_rebuy_prot.tcht_type,sMo_stok_rebuy_prot1.tcht_type)!=0)
    {
        sprintf(acErrMsg,"֤ȯ���ͺ͵Ǽǲ��еĲ�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B088");
        goto ErrExit; 
    }

    /* ���֤ȯ��ֵ */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.par_value,
           sMo_stok_rebuy_prot1.par_value) != 0)
    {
        sprintf(acErrMsg,"֤ȯ��ֵ����ĺ͵Ǽǲ��в�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B089");
        goto ErrExit;
    }

    /* ����Ѻ�� */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.pledge_rate,
           sMo_stok_rebuy_prot1.pledge_rate) != 0)
    {
        sprintf(acErrMsg,"��Ѻ�ʺ͵Ǽǲ��в�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B090");
        goto ErrExit;
    }

    /* ����������־ */
    if (strcmp(sMo_stok_rebuy_prot.buy_type,sMo_stok_rebuy_prot1.buy_type)!=0)
    {
        sprintf(acErrMsg,"��������־�͵Ǽǲ���һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B092");
        goto ErrExit;
    }

    /* ������������ѯ */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.bal,
            sMo_stok_rebuy_prot1.bal)!=0)
    {
        sprintf(acErrMsg,"���������͵Ǽǲ��еĲ�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B093");
        goto ErrExit;
    }
 
    /* ���Э�鵽���� */
    if (sMo_stok_rebuy_prot.matr_date != sMo_stok_rebuy_prot1.matr_date)
    {
        sprintf(acErrMsg,"Э�鵽���պ͵Ǽǲ��еĲ�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B094");
        goto ErrExit;
    }
   
    /* ���֤ȯ�ع����� */
    if (sMo_stok_rebuy_prot.term != sMo_stok_rebuy_prot1.term)
    {
        sprintf(acErrMsg,"֤ȯ�ع����޺͵Ǽǲ��еĲ�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B095");
        goto ErrExit;
    }

    /* ��������� */
    if (pub_base_CompDblVal(sMo_stok_rebuy_prot.rate,
              sMo_stok_rebuy_prot1.rate) != 0)
    {
        sprintf(acErrMsg,"�����ʺ͵Ǽǲ��еĲ�һ��!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B096");
        goto ErrExit;
    }
    
    pub_base_strpack(sMo_stok_rebuy_prot1.opt_acno);
    if ( sMo_stok_rebuy_prot1.opt_acno[0] != '2' )
    {
         sprintf(acErrMsg,"�տ�ʽ��Ǽǲ��еĲ�һ��!!");
         WRITEMSG
         strcpy(g_pub_tx.reply,"B129");
         goto ErrExit;
    }

    /* ����Ʒ�Ÿ�ֵ */
    if (sMo_stok_rebuy_prot.buy_type[0] == '1')
    {
        /* �ع���� */
        if (sMo_stok_rebuy_prot.tcht_type[0] == '1')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_NT);
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '2')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_ENT); 
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '4')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_FT); 
        }
		else
        {
            strcpy(g_pub_tx.prdt_code,REBUY_LOAN_QT); 
        }
    }   
    else if (sMo_stok_rebuy_prot.buy_type[0] == '2')
    {
        /* �ع����� */
        if (sMo_stok_rebuy_prot.tcht_type[0] == '1')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_NT);
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '2')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_ENT);
        }
        else if (sMo_stok_rebuy_prot.tcht_type[0] == '4')
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_FT);
        }
		else
        {
            strcpy(g_pub_tx.prdt_code,REBUY_BORROW_QT);
        }
    }
    else
    {
        sprintf(acErrMsg,"��������־����!!");
        WRITEMSG
        goto ErrExit;
    }

    /* ������ */
    strcpy(g_pub_tx.mdm_code,NULL_MDM_CODE);
    strcpy(g_pub_tx.cur_no,"01");

    ret = pub_acct_opn_in_mdm();
    if (ret != 0)
    {
        sprintf(acErrMsg,"�����ʳ���");
        WRITEMSG
        goto ErrExit;
    }

    sprintf(acErrMsg,"ac_no=[%s]",g_pub_tx.ac_no);
    WRITEMSG
    sprintf(acErrMsg,"�����ʳɹ� PASS!!! ");
    WRITEMSG

    /* ���� */
    strcpy(g_pub_tx.ac_id_type,"9");
    g_pub_tx.rate = sMo_stok_rebuy_prot.rate;
    g_pub_tx.add_ind[0] = '1';
    g_pub_tx.hst_ind[0] = '1';
    g_pub_tx.tx_amt1 = sMo_stok_rebuy_prot.bal;

    if(pub_acct_opn_ac())
    {
        sprintf(acErrMsg,"��������");
        WRITEMSG
        goto ErrExit;
    }

    set_zd_data("0310",g_pub_tx.ac_no);
    sMo_stok_rebuy_prot1.ac_id = g_pub_tx.ac_id;

    sprintf(acErrMsg,"�����ɹ� PASS!!! ac_no=[%s] ",g_pub_tx.ac_no);
    WRITEMSG

    /* �޸�֤ȯ�ع�Э��Ǽǲ� */
    strcpy(sMo_stok_rebuy_prot1.sts,"2");
    ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,
          "pact_no='%s' and buy_type='%s'",sMo_stok_rebuy_prot1.pact_no,
           sMo_stok_rebuy_prot1.buy_type);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"declare for update error !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot,g_pub_tx.reply);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"Fetch for update error !! [%d]",ret);
        WRITEMSG
        Mo_stok_rebuy_prot_Clo_Upd();
        goto ErrExit;
    }

    ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot1,g_pub_tx.reply);
    if ( ret != 0 )
    {
        sprintf(acErrMsg,"Update error !! [%d]",ret);
        WRITEMSG
        Mo_stok_rebuy_prot_Clo_Upd();
        goto ErrExit;
    }
    Mo_stok_rebuy_prot_Clo_Upd();

    if (flag[0] == '1') /*flag�ֽ�ת�ʱ�־*/
    {
        /* �����ֽ��ӽ��� */
		if(sMo_stok_rebuy_prot.buy_type[0] == '1') 	/** ��� **/
        	goto OkExitCash_out;
		else
        	goto OkExitCash_in;
    }
    else 
    {
        /* ����ת���ӽ��� */  
        strcpy(g_pub_tx.ac_no,sMo_stok_rebuy_prot1.opt_acno);
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
		if(sMo_stok_rebuy_prot.buy_type[0] == '1') 	/** ��� **/
        	goto OkExit_d;
		else
        	goto OkExit_j;
    }

OkExitCash_in:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExitCash_out:
    strcpy( g_pub_tx.reply, "0003" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit_j:
    strcpy( g_pub_tx.reply, "0002" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
OkExit_d:
    strcpy( g_pub_tx.reply, "0004" );
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

