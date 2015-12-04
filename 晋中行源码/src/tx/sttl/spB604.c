/*************************************************
* �� �� ��:  spB604.c
* ���������� ծȯͶ�ʳ��۶Ҹ�����
*
* ��    ��:  rob
* ������ڣ� 2003��1��20��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"
#include "com_parm_c.h"
#include "mo_city_c.h"

int spB604()
{
    struct mo_invest_c sMo_invest,sMo_invest1;
    struct mdm_ac_rel_c sMdm_ac_rel;   
    struct in_mst_c sIn_mst;
    struct in_parm_c sIn_parm;
	struct com_parm_c  sCom_parm;
	struct mo_city_c  mo_city_c;
    char flag[2],zqzl[2],zzlx[2];
	long   tmp_date=0,tmp_trace_no=0;
	double	tmp_amt=0.00;
    int ret;

    memset(&sMo_invest, 0x0, sizeof(struct mo_invest_c));
    memset(&sMo_invest1, 0x0, sizeof(struct mo_invest_c));
    memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
    memset(&sIn_mst, 0x0, sizeof(struct in_mst_c));
		memset(&sIn_parm,0x0,sizeof(struct in_parm_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&mo_city_c,0x00,sizeof(struct mo_city_c));
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0580",sMo_invest.pact_no);  /* Ͷ��Э���� */
    get_zd_char("0690",flag);                /* ��ת��־ */
    get_zd_double("0390",&sMo_invest1.sol_amt); /* ������ */
    get_zd_double("0400",&sMo_invest1.par_bal); /* Ʊ��� */
    get_zd_double("0410",&sMo_invest1.pre_intst); /* Ӧ����Ϣ */
	get_zd_double("0420",&sMo_invest1.overflow_amt);
	get_zd_double("0430",&sMo_invest1.abate_amt);
	get_zd_double("1002",&sMo_invest1.buy_amt);		/* ����ɱ���?? */
	strcpy(g_pub_tx.cur_no,"01");
	get_zd_data("0670",zqzl);

    ret = Mo_invest_Sel(g_pub_tx.reply,&sMo_invest,"pact_no ='%s'",sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Ͷ��Э���Ŵ���!! [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_invest.pact_no,ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B041");
        goto ErrExit;
    }
    
   vtcp_log("��ʼ���ҵǼǲ����˻�[%ld]\n",sMo_invest.ac_id);
     /* �ҵǼǲ����˻���id */
    ret = Mdm_ac_rel_Dec_Sel(g_pub_tx.reply,"ac_id = '%ld' ",sMo_invest.ac_id);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for Sel error [%s][%ld][%d]",g_pub_tx.tx_br_no,sMo_invest.ac_id,ret);
        WRITEMSG
        goto ErrExit;
    }
    ret = Mdm_ac_rel_Fet_Sel(&sMdm_ac_rel, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mdm_ac_rel_Fet error [%s][%ld][%d]",g_pub_tx.tx_br_no,sMo_invest.ac_id,ret);
        WRITEMSG
        Mdm_ac_rel_Clo_Sel();
        goto ErrExit;
    }
    Mdm_ac_rel_Clo_Sel();
 
	if( sMo_invest.sts[0]!='2' )
	{
        sprintf(acErrMsg,"Ͷ��Э��״̬����ȷ!![%s]",sMo_invest.sts);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B041");
        goto ErrExit;
	}

    if (pub_base_CompDblVal(sMo_invest.par_bal,sMo_invest1.par_bal) )
    {
        sprintf(acErrMsg,"Ʊ��۴��ڵǼǲ����Ʊ���!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B085");
        goto ErrExit;
    }
    if (pub_base_CompDblVal(sMo_invest.buy_amt,sMo_invest1.buy_amt) == -1)
    {
        sprintf(acErrMsg,"����ɱ����������!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"P422");
        goto ErrExit;
    }
  
    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no= '%s' ",
          sMdm_ac_rel.ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�����˺Ź�ϵ����� [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    if (sMdm_ac_rel.ac_id != sMo_invest.ac_id)
    {
        sprintf(acErrMsg,"Ͷ���˺�������� !!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B058");
        goto ErrExit;
    }
   
    sMo_invest.sol_amt += sMo_invest1.sol_amt;
    sMo_invest.sol_date = g_pub_tx.tx_date;

    ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn = %d",
          sMo_invest.ac_id,sMdm_ac_rel.ac_seqn);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯͶ���˻����ļ�����!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"B059");
        goto ErrExit;
    }

    ret = In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no='%s'",
          sMo_invest.bkrl_code);
    if (ret != 0)
    {
        sprintf(acErrMsg,"��ѯ�ڲ���Ʒ���������!!![%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W058");
        goto ErrExit;
    }

    g_pub_tx.ac_seqn = 0;
    g_pub_tx.ac_id = 0;
    g_pub_tx.hst_ind[0] = '1';

    strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
    g_pub_tx.ac_id = sMdm_ac_rel.ac_id;
    g_pub_tx.ac_seqn = 0;

	if( zqzl[0]=='1' && sIn_parm.invst_type[0]!='1' )
	{
        sprintf(acErrMsg,"�˱�Э�鹺���ծȯ���Ƕ���ծȯ!![%s]",sMo_invest.bkrl_code);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P420");
        goto ErrExit;
	}
	if( zqzl[0]=='2' && sIn_parm.invst_type[0]!='2' )
	{
        sprintf(acErrMsg,"�˱�Э�鹺���ծȯ�����г���ծȯ!![%s]",sMo_invest.bkrl_code);
        WRITEMSG
        strcpy(g_pub_tx.reply,"P421");
        goto ErrExit;
	}

    if (sIn_parm.invst_type[0] == '1' || sIn_parm.invst_type[0] == '3')
    {
        /* ����ծȯͶ��  ��ת��ծȯͶ�� */
		    g_pub_tx.tx_amt1 = sMo_invest1.par_bal;
		vtcp_log("DDD��ʼƱ��ۼ���g_pub_tx.tx_amt1=[%ld]sMo_invest1.par_bal=[%ld]\n",g_pub_tx.tx_amt1,sMo_invest1.par_bal);
    }
    else if (sIn_parm.invst_type[0] == '2')
    {
        /* �г���ծȯͶ����� */
        g_pub_tx.tx_amt1 = sMo_invest1.par_bal;
        vtcp_log("CCC��ʼƱ��ۼ���g_pub_tx.tx_amt1=[%ld]sMo_invest1.par_bal=[%ld]\n",g_pub_tx.tx_amt1,sMo_invest1.par_bal);
    }
    else 
    {
        WRITEMSG
        goto ErrExit;
    }
   
    if ( pub_base_CompDblVal(sIn_mst.bal,g_pub_tx.tx_amt1)<0 )
	{
        sprintf(acErrMsg,"Ʊ��۴��ڵǼǲ����Ʊ���!! ");
        WRITEMSG
        strcpy(g_pub_tx.reply,"B085");
        goto ErrExit;
	}
	else if( pub_base_CompDblVal(g_pub_tx.tx_amt1,0.00)>0 )
    {
    	vtcp_log("KKK��ʼƱ��ۼ���g_pub_tx.tx_amt1=[%ld]sMo_invest1.par_bal=[%ld]\n",g_pub_tx.tx_amt1,sMo_invest1.par_bal);
        sprintf(acErrMsg,"bal=[%lf],tx_amt=[%lf]",sIn_mst.bal,g_pub_tx.tx_amt1);
        WRITEMSG
        g_pub_tx.add_ind[0] = '1';	/** ����,�������� **/
		strcpy(g_pub_tx.ct_ind,flag);
		if( pub_base_CompDblVal(sIn_mst.bal,g_pub_tx.tx_amt1)==0 )
			g_pub_tx.svc_ind=9002;
		else
			g_pub_tx.svc_ind=9001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"Ͷ���˻�ȡ�����!!");
            WRITEMSG
            goto ErrExit;
        }
		ret = pubf_acct_proc("460B");
		if( ret )
		{
        	sprintf(acErrMsg,"�Ǽǻ����ˮ����!");
        	WRITEMSG
        	goto ErrExit;
		}
    }
    if (sIn_parm.invst_type[0] == '2')
   {
    sMo_invest.par_bal -= sMo_invest1.par_bal;
    sMo_invest.buy_amt -= sMo_invest1.buy_amt;


   vtcp_log("��ʼ��Ϣ������sMo_invest.lx_ac_id=[%ld]\n",sMo_invest.lx_ac_id);
   
    /*  ��Ϣ�ļ���  */
    memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=0 ",
          sMo_invest.lx_ac_id);
    vtcp_log(" ��Ϣ���ʽ��sIn_mst.bal=[%ld]\n",sIn_mst.bal);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯͶ���˻����ļ�����!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}
    	  strcpy(g_pub_tx.ac_no,"");
		    g_pub_tx.ac_id=sMo_invest.lx_ac_id;
        g_pub_tx.add_ind[0] = '1';	/** ����,�������� **/
	    	g_pub_tx.tx_amt1=sIn_mst.bal;
	      ret=In_parm_Sel(g_pub_tx.reply,&sIn_parm,"prdt_no=%s",sIn_mst.prdt_no);
	      vtcp_log("��Ϣ��Ʒ��sIn_parm.acc_hrt=[%s]\n",sIn_parm.acc_hrt);
	      if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯͶ���˻����ļ�����!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}
    	set_zd_data("0153",sIn_parm.acc_hrt);  /*��Ϣ��Ŀ*/
		    set_zd_double("1002",sIn_mst.bal);  /*  ��Ϣ��� */
		    strcpy(g_pub_tx.ct_ind,flag);
			   g_pub_tx.svc_ind=9002;
			    vtcp_log("269 ��Ϣ���ʽ��sIn_mst.bal=[%ld]\n",sIn_mst.bal);
       if (pub_acct_trance())
        {
            sprintf(acErrMsg,"Ͷ���˻�ȡ�����!!");
            WRITEMSG
            goto ErrExit;
        }
     sMo_invest.pre_intst -= sIn_mst.bal;
    
    /* ��۵Ĵ��� */
    if (pub_base_CompDblVal(sMo_invest1.overflow_amt,0.00) > 0)
    {
    vtcp_log("KIsMo_invest.overflow_ac_id��ֵ!![%ld]\n",sMo_invest.overflow_ac_id);
		memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=0 ",
          sMo_invest.overflow_ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯͶ���˻����ļ�����!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}

		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.overflow_ac_id;
        g_pub_tx.add_ind[0] = '1';	/** ����,�������� **/
		g_pub_tx.tx_amt1=sMo_invest1.overflow_amt;
		strcpy(g_pub_tx.ct_ind,flag);
		if ( pub_base_CompDblVal(sMo_invest1.overflow_amt,sIn_mst.bal)==0 )
			g_pub_tx.svc_ind=9002;
		else 
			g_pub_tx.svc_ind=9001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"Ͷ���˻�ȡ�����!!");
            WRITEMSG
            goto ErrExit;
        }
		sMo_invest.overflow_amt -= sMo_invest1.overflow_amt;
    }

    /* �ۼ۵Ĵ��� */
    if (pub_base_CompDblVal(sMo_invest1.abate_amt,0.00) > 0)
    {
    	vtcp_log("KIsMo_invest.abate_ac_id��ֵ!![%ld]\n",sMo_invest.abate_ac_id);
		memset(&sIn_mst,0x00,sizeof(struct in_mst_c));
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=0 ",
          sMo_invest.abate_ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"��ѯͶ���˻����ļ�����!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"B059");
        	goto ErrExit;
    	}
 
		strcpy(g_pub_tx.ac_no,"");
		g_pub_tx.ac_id=sMo_invest.abate_ac_id;
		g_pub_tx.tx_amt1=sMo_invest1.abate_amt;
        g_pub_tx.add_ind[0] = '0';	/** �跽,�������� **/
		strcpy(g_pub_tx.ct_ind,flag);
		if ( pub_base_CompDblVal(sMo_invest1.abate_amt,sIn_mst.bal)==0 )
			g_pub_tx.svc_ind=9002;
		else 
			g_pub_tx.svc_ind=9001;
        if (pub_acct_trance())
        {
            sprintf(acErrMsg,"Ͷ���˻�ȡ�����!!");
            WRITEMSG
            goto ErrExit;
        }
		sMo_invest.abate_amt -= sMo_invest1.abate_amt;
    }

    sMo_invest.trace_no = g_pub_tx.trace_no;   
    strcpy(sMo_invest.tel, g_pub_tx.tel);
    strcpy(sMo_invest.chk, g_pub_tx.chk);
    strcpy(sMo_invest.auth,g_pub_tx.auth);
 
    double abc=0;
    get_zd_double("1002",&abc);   /*��Ϣ��� */
    vtcp_log(" ��Ϣ�����[%.0f]\n",abc);    
    get_zd_double("0400",&sMo_invest1.par_bal); /* Ʊ��� */
  	get_zd_double("0420",&sMo_invest1.overflow_amt);
	  get_zd_double("0430",&sMo_invest1.abate_amt);
	if( pub_base_CompDblVal(sMo_invest.par_bal,0.00)==0
		&& pub_base_CompDblVal(sMo_invest.overflow_amt,0.00)==0
		&& pub_base_CompDblVal(sMo_invest.abate_amt,0.00)==0
		)
		strcpy(sMo_invest.sts,"*");
}
    /* �޸�Ͷ�ʵǼǲ� */
    ret = Mo_invest_Dec_Upd(g_pub_tx.reply,"br_no='%s' and pact_no='%s' and sts='2'",g_pub_tx.tx_br_no,sMo_invest.pact_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"declare for update error [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Mo_invest_Fet_Upd(&sMo_invest1,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"Fetch mo_invest error !! [%s][%s][%d]",g_pub_tx.tx_br_no,sMo_invest.pact_no,ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        goto ErrExit;
    }

    ret = Mo_invest_Upd_Upd(sMo_invest,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"�޸�Ͷ�ʵǼǲ�����!! [%d]",ret);
        WRITEMSG
        Mo_invest_Clo_Upd();
        strcpy(g_pub_tx.reply,"B057");
        goto ErrExit;
    }

		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",sMo_invest.ac_id);
		if( ret )
		{
			sprintf( acErrMsg, "��ѯ�����ʺŽ��ʶ��ձ����[%d]!",ret );
			WRITEMSG
			goto ErrExit;
		}
    get_zd_data("0300",g_pub_tx.ac_no);      /* ת���˺� */
    if (flag[0] == '1')
    {
        /* �����ֽ��ӽ��� */
        goto OkExitCash;
    }
    else
    {
        /* ����ת���ӽ��� */
        g_pub_tx.ac_id = 0;
        g_pub_tx.ac_seqn = 0;
        goto OkExit;
    }

OkExitCash:
    strcpy( g_pub_tx.reply, "0001" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;


OkExit:
    strcpy( g_pub_tx.reply, "0002" );
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

