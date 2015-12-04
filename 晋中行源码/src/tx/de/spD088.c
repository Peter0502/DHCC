/*******************************************************************************
*** �� �� ��:  V1.0
*** ��������: shangyongchao
*** ��    ��: 2013/5/16 11:30:53
*** ����ģ��: ��Ʋ�Ʒ
*** ��������: spD088
*** ��������: �����Ʋ�Ʒ�Ϲ������������
*** ʹ��ע��:
*** �޸ļ�¼:
***     �޸�����:
***     �޸�ʱ��:
***     �޸�����:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "cif_ass_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "fn_br_quota_parm_c.h"
#include "fn_flt_radio_parm_c.h"
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "fn_br_profit_c.h"
#include "redempt_rate_c.h"
#include "fn_ac_rel_c.h"
#include "com_branch_c.h"
#include "com_sys_parm_c.h"
#include "com_parm_c.h"
#define L365 365

int spD088()
{
    int iRet = 0;
    char cOper_flag[2];/*1�Ϲ� 2���� 3��أ�LJ20151105��*/
    memset(cOper_flag,0x00,sizeof(cOper_flag));
    /** ��ʼ��ȫ�ֱ��� **/
    pub_base_sysinit();
    get_zd_data("0670",cOper_flag);
    /*����cOper_flag���в�ͬ����*/
    if(cOper_flag[0] == '1')
    {
        /*�Ϲ�*/
        iRet = buy_func();
        if(iRet)
        {
            vtcp_log("�Ϲ�ʧ����");
            goto ErrExit;
        }
        
    }
    else if(cOper_flag[0] == '2')
    {
        /*����*/
        iRet = cancel_func();
        if(iRet)
        {
            vtcp_log("����ʧ����");
            goto ErrExit;
        }
    }
    /*LJ20151105*/
    else if(cOper_flag[0] == '3')
    {
        /*���*/
        iRet = redempt_func();
        if(iRet)
        {
            vtcp_log("���ʧ����");
            goto ErrExit;
        }
    }
    else
    {
        vtcp_log("�������ʹ��� [%s]",cOper_flag);
        strcpy(g_pub_tx.reply, "S083");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    vtcp_log("Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:
    vtcp_log( "Before return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}

/*�Ϲ�������*/
int buy_func()
{
    int iRet = 0;
    int iAc_seqn = 0;/*�˻����*/
    long lDays = 0;
    char cPrdt_code[21];/*��Ʒ����*/
    char cCif_type[2];/*�ͻ�����1����2�Թ�*/
    char cAgent_id_type[2];/*�Թ����Ĵ�����֤������*/
    char cAgent_id[21];/*�Թ����Ĵ�����֤��*/
    char cAgent_name[61];/*�Թ����Ĵ���������*/
    char cAc_no[25];/*�˺�*/
    char cPact_no[21];/*��ͬ��*/
    char cPact_no_tmp[21];
    char cMang[7];/*�ͻ������*/
    char cCif_lvl[2];
    char cCpmc[60]; /*��Ʒ����*/  
    double dAvbal = 0.00;/*�˻��������*/
    double dQuota_amt = 0.00;/*�û��� ʣ�����*/
    double dFlt_radio = 0.00;/*�����ϸ�����*/
    double dTx_amt = 0.00;/*�Ϲ����*/
    
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg,sFn_reg_tmp;
    struct fn_reg_hst_c sFn_reg_hst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;
    struct fn_br_quota_parm_c sFn_br_qp;
    
    memset(cPrdt_code   ,0x00,sizeof(cPrdt_code));
    memset(cCpmc		,0x00,sizeof(cCpmc)); 
    memset(cCif_type    ,0x00,sizeof(cCif_type));
    memset(cAgent_id    ,0x00,sizeof(cAgent_id));
    memset(cAgent_id_type    ,0x00,sizeof(cAgent_id_type));
    memset(cAgent_name  ,0x00,sizeof(cAgent_name));
    memset(cAc_no       ,0x00,sizeof(cAc_no));
    memset(cPact_no     ,0x00,sizeof(cPact_no));
    memset(cPact_no_tmp     ,0x00,sizeof(cPact_no_tmp));
    memset(cMang        ,0x00,sizeof(cMang));
    memset(cCif_lvl,0x00,sizeof(cCif_lvl));
    memset(&sFn_parm        ,0x00,sizeof(sFn_parm));
    memset(&sFn_reg        ,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_tmp        ,0x00,sizeof(sFn_reg_tmp));
    memset(&sFn_reg_hst        ,0x00,sizeof(sFn_reg_hst));
    memset(&sMdm_ac_rel        ,0x00,sizeof(sMdm_ac_rel));
    memset(&sDd_parm        ,0x00,sizeof(sDd_parm));
    memset(&sDd_mst        ,0x00,sizeof(sDd_mst));
    memset(&sFn_br_qp,0x00,sizeof(sFn_br_qp));
    
    get_zd_int("0340",&iAc_seqn);
    get_zd_double("0390",&dTx_amt);
    get_zd_data("0300",cAc_no);
    get_zd_data("0690",cAgent_id_type);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0250",cAgent_name);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0260",cCpmc);      /*ǰ̨��ȡ��Ʒ����*/
    vtcp_log(" ǰ̨���Ͳ�Ʒ����  �� %s ��",cCpmc);
    get_zd_data("0800",cPact_no_tmp);  /**��86���ݸ�Ϊ80��**/
    get_zd_data("0920",cMang);
    /*�ȼ���˺���Ϣ*/
    sprintf(cPact_no,"%012s",cPact_no_tmp);  /*add by lwb 20141030 ��Э���ת����12λ*/
    pub_base_old_acno(cAc_no);
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if (iRet == 100)
    {
        vtcp_log( "�����ڸ��˻�!!ac_id=[%ld]", sMdm_ac_rel.ac_id);
        strcpy(g_pub_tx.reply, "B117");
        set_zd_data("0130","�����ڸ��˻�");
        goto ErrExit;
    }
    else if (iRet)
    {
        vtcp_log( "��ȡ�����˺Ŷ��ձ�ʧ��[%d]", iRet);
        strcpy(g_pub_tx.reply, "C114 ");
        goto ErrExit;
    }
    /*������״̬*/
    iRet = check_note_sts(sMdm_ac_rel.note_sts);
    if(iRet)
    {
        vtcp_log("����״̬������");
        goto ErrExit;
    }
    /*��֤�ͻ�����*/
    iRet = get_cif_type(sMdm_ac_rel,cCif_type,cCif_lvl);
    if(iRet)
    {
        vtcp_log("��֤�ͻ�����ʧ��");
        goto ErrExit;
    }
    vtcp_log("�ͻ�����[%s]",cCif_lvl);
    /*�����Ʋ�Ʒ*/
    vtcp_log("[%s],[%d]��Ʋ�Ʒ���cPrdt_code[%s]",__FILE__,__LINE__,cPrdt_code);
    iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ������ʧ��iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    else if(iRet == 100)
    {
        vtcp_log("û�и���Ʋ�Ʒ");
        strcpy(g_pub_tx.reply,"D104");
        set_zd_data("0130","û�и���Ʋ�Ʒ");
        goto ErrExit;    
    }
    vtcp_log("fn_parm.stat = [ %s ] ",sFn_parm.stat);
    /*��Ʋ�Ʒ״̬*/
    if( sFn_parm.stat[0] != '0' )
    {
        vtcp_log("�ò�Ʒ���Ϲ�״̬[%s] ",sFn_parm.stat);
        strcpy(g_pub_tx.reply,"C334");
        set_zd_data("0130","�ò�Ʒ���Ϲ�״̬");
        goto ErrExit;
    }
    /*����Ƿ���ļ����*/
    if(sFn_parm.beg_date > g_pub_tx.tx_date || sFn_parm.end_date < g_pub_tx.tx_date)
    {
        vtcp_log("�ò�Ʒ���Ϲ�״̬[%s] ",sFn_parm.stat);
        strcpy(g_pub_tx.reply,"C334");
        set_zd_data("0130","�ò�Ʒ���Ϲ�״̬");
        goto ErrExit;
    }
    /*����ǹ��� �ж�ǰ̨���Ĳ�Ʒ���ƺͺ�̨�Ĳ�Ʒ�����ѯ���Ĳ�Ʒ�Ƿ�һ�£�������������ʱ���жϣ�*/
    vtcp_log(" ��̨��ѯ��Ʒ����  �� %s ��",sFn_parm.title);
    if(!strcmp(g_pub_tx.tty,"netb"))   /*���������*/
    {
    		   	
    }
    else if(!strcmp(g_pub_tx.tty,"mobb"))/*������ֻ����� */
    {
    	   	
    }
    else{
    	if(strcmp(sFn_parm.title,cCpmc) != 0)
	    {
	    	vtcp_log(" ��Ʒ���봫�ʹ���  �� %s ��",sFn_parm.prdt_code);
	    	strcpy(g_pub_tx.reply,"P146");
	    	set_zd_data("0130","��Ʒ���봫����� �������� ");
	    	goto ErrExit;
	    }	
    }
    
    /*�Ϲ�����*/
    /*if(strcmp(sFn_parm.chnl_type,g_pub_rule.chnl_type) != 0 && strcmp(sFn_parm.chnl_type,"****") != 0)
    {
        vtcp_log("�����������Ϲ�[%s] ",sFn_parm.chnl_type);
        strcpy(g_pub_tx.reply,"P470");
        goto ErrExit;
    }*/
    if(strcmp(sFn_parm.cif_type,"0") != 0 && strcmp(sFn_parm.cif_type,cCif_type) !=0)
    {
        vtcp_log("�ͻ����Ͳ���[%s] ",sFn_parm.cif_type);
        strcpy(g_pub_tx.reply,"D112");
        set_zd_data("0130","�ͻ����Ͳ���");
        goto ErrExit;
    }
	iRet=Fn_reg_Sel(g_pub_tx.reply,&sFn_reg_tmp,"pact_no = '%s'",cPact_no);
	if(iRet == 0)
	{
		sprintf(acErrMsg,"��Э����Ѵ��� [%s]",cPact_no);
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data("0130",acErrMsg);		
		WRITEMSG	
		goto ErrExit;
	}
	else if(iRet != 100)
	{
		vtcp_log("��ѯfn_regʧ��");
		strcpy(g_pub_tx.reply,"D331");
		goto ErrExit;
	}


    /*��ȡ�û������*/
    iRet = get_br_quota(sFn_parm,&dQuota_amt);
    if(iRet)
    {
        vtcp_log("�õ�������iRet[%d]",iRet);
        goto ErrExit;
    }
    /*�Ϲ�����Ƿ��������*/
    /*sFn_reg.count = dTx_amt / sFn_parm.unit_amt;*/
    
    if(pub_base_CompDblVal(sFn_reg.tx_amt,dQuota_amt) > 0)
    {
        vtcp_log("û���޶��� tx_amt[%lf] quot_amt[%lf]",sFn_reg.tx_amt,dQuota_amt);
        strcpy(g_pub_tx.reply,"D349");
        set_zd_data("0130","û���޶���");
        goto ErrExit;
    }
     sFn_reg.tx_amt  = dTx_amt;
    if(sFn_parm.prdt_type[0] == '1') /*���ʽ��Ʒ���� ��֮ǰ���ж�*/
    {
	    /*�Ƿ���������Ϲ���*//*ǰ̨�Ѿ����ƹ����� 20140411_wjl*/
	    if(cCif_type[0] == '1' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.per_min_amt) < 0)
	    {
	        vtcp_log("��������Ϲ����[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.per_min_amt);
	        strcpy(g_pub_tx.reply,"D338");
	        goto ErrExit;
	    }
	    else if(cCif_type[0] == '2' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.com_min_amt) <0 )
	    {
	        vtcp_log("��������Ϲ����[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.com_min_amt);
	        strcpy(g_pub_tx.reply,"D338");
	        goto ErrExit;
	    }
	}
	if(sFn_parm.prdt_type[0] == '2') /*����ʽ��Ʒ���Ϲ�����ж�����Ѿ��Ϲ����ڿۿ����ж�����Ϲ����*/
	{
		double sTmt = 0.00;
		
		iRet = Fn_reg_Dec_Sel(g_pub_tx.reply,"ac_no = '%s' and prdt_code = '%s' and sts = '1'",cAc_no,cPrdt_code);
	    if(iRet)
	    {
	        vtcp_log("������ƵǼǲ��α����");
	        strcpy(g_pub_tx.reply,"D101");
	        goto ErrExit;
	    }
	    
	    while(1)
	    {      
	        iRet = Fn_reg_Fet_Sel(&sFn_reg,g_pub_tx.reply);
	        if(iRet != 100 && iRet != 0)
	        {
	            vtcp_log("ȡ��ƵǼǲ��α����");
	            strcpy(g_pub_tx.reply,"D330");
	            goto ErrExit;
	        }
	        if(iRet == 0)
	        {       
	        	sTmt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
	        }
	        else
	        	break;
	    }
	    Fn_reg_Clo_Sel();
	    sFn_reg.tx_amt  = dTx_amt;
   		vtcp_log("�Ϲ������ [%.2lf] ",sFn_reg.tx_amt);
   		sFn_reg.count = sql_max_int("fn_reg","count","ac_no = '%s' and prdt_code = '%s'",cAc_no,cPrdt_code); 
   		vtcp_log("sFn_reg.count [%d] ",sFn_reg.count);
	    if(sTmt == 0)
	    {
	    	if(sFn_reg.count == 0)
	    	{
	    		sFn_reg.count = 1;		
	    	}
	    	else 
	    		++sFn_reg.count;
	    	/*�Ƿ���������Ϲ���*//*ǰ̨�Ѿ����ƹ����� 20140411_wjl*/
		    if(cCif_type[0] == '1' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.per_min_amt) < 0)
		    {
		        vtcp_log("��������Ϲ����[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.per_min_amt);
		        strcpy(g_pub_tx.reply,"D338");
		        goto ErrExit;
		    }
		    else if(cCif_type[0] == '2' && pub_base_CompDblVal(sFn_reg.tx_amt,sFn_parm.com_min_amt) <0 )
		    {
		        vtcp_log("��������Ϲ����[%lf][%lf]",sFn_reg.tx_amt,sFn_parm.com_min_amt);
		        strcpy(g_pub_tx.reply,"D338");
		        goto ErrExit;
		    }
	    }
	    else
	    	++sFn_reg.count;
	    sFn_reg.tx_amt  = dTx_amt;
	    
	    	
	}
	
    vtcp_log("�Ϲ����ڶ����� [%.2lf] ",sFn_reg.tx_amt);
    /*��ȡ���渡������*/
    if(sFn_parm.float_rate_flag[0] == 'Y')
    {
        iRet = get_flt_radio(sFn_parm.prdt_code,sFn_reg.tx_amt,cCif_lvl,sMdm_ac_rel.ac_id,&dFlt_radio);
        if(iRet)
        {
            vtcp_log("��ȡ�����ϸ���������");
            goto ErrExit;
        }
        vtcp_log("�����ϸ�����Ϊ[%f]",dFlt_radio);
    }
    else
    {
        dFlt_radio = 0.000000;
    }
    sFn_reg.flt_radio = dFlt_radio;
    /*����Ԥ���������������*/
    lDays = pub_base_CalTrueDays(sFn_parm.ic_date,sFn_parm.mtr_date);/*mod by lwb  ȥ��+1*/
    /*add by lwb ��������Ҫ��һ��Ϊ365��*/ 
    if(lDays== 366)
    {
     	lDays = 365;
     }
    vtcp_log("mtr_date[%ld] - ic_date[%ld] = lDays[%ld]",sFn_parm.ic_date,sFn_parm.mtr_date,lDays);
    /*sFn_reg.intst=sFn_reg.tx_amt * lDays * sFn_parm.rate * (1+ sFn_reg.flt_radio)/L360/L100;*/
		sFn_reg.intst=sFn_reg.tx_amt * lDays * sFn_parm.rate * (1+ sFn_reg.flt_radio)/L365/L100;
     vtcp_log("����Ϊ[%lf]",sFn_reg.intst);
    sFn_reg.profit_amt= sFn_reg.tx_amt * lDays * sFn_parm.profit_rate/L365/L100;
    /*��������*/
    sFn_reg.intst = pub_base_PubDround_1(sFn_reg.intst);
    set_zd_double("1001",sFn_reg.intst);
    set_zd_double("0840",sFn_reg.flt_radio);
    sFn_reg.profit_amt = pub_base_PubDround_1(sFn_reg.profit_amt);
    /*�Ǽ�fn_reg,fn_reg_hst*/
    vtcp_log("LJg_pub_tx.tty=[%s]",g_pub_tx.tty);
    if(!strcmp(g_pub_tx.tty,"netb"))   /*����������Ļ� �����м�Ϊ������   add by lwb 20141114*/
    {
    		strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
    		strcpy(sFn_reg.chnl_type,"0400");   	
    }
    else if(!strcmp(g_pub_tx.tty,"mobb"))/*������ֻ����еĻ� �����м�Ϊ������   LJ 20151116*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
    	strcpy(sFn_reg.chnl_type,"0300");   	
    }
    else{
    	 strcpy(sFn_reg.chnl_type,"0100");	
    }
    sFn_reg.tx_date = g_pub_tx.tx_date;
    sFn_reg.trace_no=g_pub_tx.trace_no;
    strcpy(sFn_reg.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(sFn_reg.pact_no,cPact_no);
    strcpy(sFn_reg.prdt_code,cPrdt_code);
    strcpy(sFn_reg.ac_no,cAc_no);
    sFn_reg.ac_id = sMdm_ac_rel.ac_id;
    sFn_reg.ac_seqn = iAc_seqn;
    strcpy(sFn_reg.cif_type,cCif_type);
    strcpy(sFn_reg.ct_ind,"2");
    /*strcpy(sFn_reg.chnl_type,g_pub_rule.chnl_type);*/

    sFn_reg.ic_date = sFn_parm.ic_date;
    sFn_reg.end_date = sFn_parm.mtr_date;
    strcpy(sFn_reg.profit_flag,"N");
    strcpy(sFn_reg.brf,"�Ϲ���Ʋ�Ʒ");
    strcpy(sFn_reg.sts,"0");
    strcpy(sFn_reg.tel,g_pub_tx.tel);
    strcpy(sFn_reg.mang,cMang);
    strcpy(sFn_reg.agent_id_type,cAgent_id_type);
    strcpy(sFn_reg.agent_id,cAgent_id);
    strcpy(sFn_reg.agent_name,cAgent_name);
    iRet = Fn_reg_Ins(sFn_reg,g_pub_tx.reply);
    if(iRet)
    {
        Fn_reg_Debug(&sFn_reg);
        vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D345");
        goto ErrExit;
    }
    /*�Ǽ���ϸ*/
    sFn_reg_hst.tx_date = g_pub_tx.tx_date;
    sFn_reg_hst.trace_no = g_pub_tx.trace_no;
    strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
    strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
    strcpy(sFn_reg_hst.type,"0");
    sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
    strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
    strcpy(sFn_reg_hst.agent_id,cAgent_id);
    strcpy(sFn_reg_hst.agent_name,cAgent_name);
    sFn_reg_hst.buy_date=sFn_reg_hst.tx_date;
    sFn_reg_hst.buy_trace_no=sFn_reg_hst.trace_no;
    strcpy(sFn_reg_hst.brf,"�Ϲ���Ʋ�Ʒ");
    iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
    if(iRet)
    {
        Fn_reg_hst_Debug(&sFn_reg_hst);
        vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D345");
        goto ErrExit;
    }
    
    /*����˻���ͬʱ���Ŀ��ƽ��*/
    iRet = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id= %ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
    if(iRet)
    {
        vtcp_log("����������ļ������α�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D332 ");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log("����������ļ������α�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"D332");
        goto ErrExit;
    }
    else if(iRet == 100)
    {
        vtcp_log("û������˻�");
        strcpy(g_pub_tx.reply,"D104");
        set_zd_data("0130","���˻�������");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_log("�����˻��˻�״̬������");
        strcpy(g_pub_tx.reply,"B124");
        set_zd_data("0130","�û����˻��˻�״̬������");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' || sDd_mst.hold_sts[0] == '2')
    {
        vtcp_log("�˻���ȫ������ֻ������");
        strcpy(g_pub_tx.reply,"L098");
        goto ErrExit;
    }
    /*����¿������*/
    iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(iRet)
    {
        vtcp_log("��ѯ���ڲ�Ʒ��������iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"W025 ");
        goto ErrExit;
    }
    /*������˻��ı���*/
    if(strcmp(sFn_parm.cur_no,sDd_parm.cur_no) != 0)
    {
        vtcp_log("���ֲ�֧��fn_parm[%s] dd_parm[%s]",sFn_parm.cur_no,sDd_parm.cur_no);
        strcpy(g_pub_tx.reply,"D350");
        set_zd_data("0130","���ֲ�֧��");
        goto ErrExit;
    }
    /*dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.zf_bal - sDd_mst.ctl_amt - sDd_parm.min_bal;*/
    dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.ctl_amt - sDd_parm.min_bal;
    if(pub_base_CompDblVal(sFn_reg.tx_amt,dAvbal) > 0)
    {
        vtcp_log("�˻���������");
        strcpy(g_pub_tx.reply,"P103");
        set_zd_data("0130","�˻���������");
        goto ErrExit;
    }
    sDd_mst.ctl_amt += sFn_reg.tx_amt;
    iRet =  Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
    if(iRet)
    {
        vtcp_log("���»������ļ�ʧ��iRet[%d]",iRet);
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
    /*���ı��������*/
    iRet = upd_br_quota(sFn_parm,sFn_reg.tx_amt,0);
    if(iRet)
    {
        vtcp_log("���»���������[%d]",iRet);
        goto ErrExit;
    }
    strcpy(g_pub_tx.ac_no,cAc_no);
    strcpy(g_pub_tx.brf,"�Ϲ���Ʋ�Ʒ");
    g_pub_tx.tx_amt1 = sFn_reg.tx_amt;
    pub_mob_sendmail(  g_pub_tx.brf, "2219",g_pub_tx.ac_no,"0",g_pub_tx.tx_amt1, 0.00 );
    if( pub_ins_trace_log() )
    {
        vtcp_log("�Ǽǽ�����ˮ����");
        strcpy(g_pub_tx.reply,"P126 ");
        goto ErrExit;
    }
   set_zd_data("0600",cPact_no);
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("�Ϲ��ɹ�");
    return 0;
ErrExit:
    vtcp_log("�Ϲ�ʧ��");
    return 1;
}
/*����������*/
int cancel_func()
{
    int iRet = 0;
    char cAc_no[25];
    char cPrdt_code[21];
    char cFd123[1000];
    char cTmp[21];
    char cBuy_date[9];
    char cBuy_trace_no[10];
    char cAgent_name[61];
    char cAgent_id[21];
    char cAgent_id_type[2];
    double dAmt_tmp = 0.00;
    
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cFd123,0x00,sizeof(cFd123));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    get_zd_data("1230",cFd123);
    get_zd_data("0300",cAc_no);
    get_zd_data("0250",cAgent_name);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0690",cAgent_id_type);

    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if (iRet == 100)
    {
        vtcp_log( "�����ڸ��˻�!!ac_id=[%ld]", sMdm_ac_rel.ac_id);
        strcpy(g_pub_tx.reply, "B117");
        set_zd_data("0130","�����ڸ��˻�");
        goto ErrExit;
    }
    else if (iRet)
    {
        vtcp_log( "��ȡ�����˺Ŷ��ձ�ʧ��[%d]", iRet);
        strcpy(g_pub_tx.reply, "C114 ");
        goto ErrExit;
    }
        
    if(!strcmp(g_pub_tx.tty,"netb"))   /*�������������ˮ�ǵ�������  add by lwb 20141202*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);	
    }
    if(!strcmp(g_pub_tx.tty,"mobb"))   /*������ֻ���������ˮ�ǵ�������  LJ 20151116*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);	
    }
    /*��ȡ123����Ϣ ��ʼ������¼*/
    for(int i = 1; ;i++)
    {
        memset(&sFn_parm,0x00,sizeof(sFn_parm));
        memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
        memset(&sFn_reg,0x00,sizeof(sFn_reg));
        memset(&sDd_mst,0x00,sizeof(sDd_mst));
        memset(cTmp,0x00,sizeof(cTmp));
        memset(cBuy_trace_no,0x00,sizeof(cBuy_trace_no));
        memset(cBuy_date,0x00,sizeof(cBuy_date));
        pub_base_cut_str( cFd123, cTmp, "|", i );
        vtcp_log("cTmp[%s] i[%d]",cTmp,i);
        if( strlen(cTmp) == 0 )
        {
            vtcp_log("����Ϊ��!");
            break;
        }
        strncpy(cBuy_date,cTmp,8);
        strncpy(cBuy_trace_no,cTmp+8,9);
        vtcp_log("cBuy_date[%s],cBuy_trace_no[%s]",cBuy_date,cBuy_trace_no);
        
        /*����fn_reg dd_mst �Ǽ�fn_reg_hst*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld",atol(cBuy_date),atol(cBuy_trace_no));
        if(iRet)
        {
            vtcp_log("������Ʋ�Ʒ�Ǽǲ��α����[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("ȡ��Ʋ�Ʒ�Ǽǲ��α����[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
        /*�ȼ���Ʒ�ǲ����ܳ���*/
        iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
        if(iRet)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ������ʧ��");
            strcpy(g_pub_tx.reply, "D324");
            goto ErrExit;
        }
        if(sFn_parm.cancel_flag[0] != 'Y')
        {
            vtcp_log("����Ʋ�Ʒ���ܳ���");
            strcpy(g_pub_tx.reply, "D351");
            set_zd_data("0130","����Ʋ�Ʒ���ܳ���");
            goto ErrExit;
        }
        if(sFn_parm.stat[0] != '0')
        {
            vtcp_log("�ò�Ʒ���Ϲ�״̬[%s] ",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"C334");
            set_zd_data("0130","�ò�Ʒ���Ϲ�״̬");
            goto ErrExit;
        }
        if(sFn_reg.sts[0] != '0')
        {
            vtcp_log("�ü�¼���Ϲ�״̬[%s] ",sFn_reg.sts);
            strcpy(g_pub_tx.reply,"D352 ");
            set_zd_data("0130","�ü�¼���Ϲ�״̬");
            goto ErrExit;
        }
        /*���Ǳ���������Ĳ��ܳ���*/
        
        if(strcmp(sFn_reg.tx_br_no,g_pub_tx.tx_br_no) != 0)
        {

            vtcp_log("�ü�¼�Ǳ���������[%s] ",sFn_reg.tx_br_no);
            strcpy(g_pub_tx.reply,"D353 ");
            set_zd_data("0130","�ü�¼�Ǳ���������");
            goto ErrExit;
        }
        sFn_reg.sts[0] = '*';
        sFn_reg.date1 = g_pub_tx.tx_date;
        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("������Ʋ�Ʒ�Ǽǲ�����[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333 ");
            goto ErrExit;
        }
        Fn_reg_Clo_Upd();
        /*��dd_mst�Ŀ��ƽ����¸���*/
        iRet = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id= %ld and ac_seqn=%d",sFn_reg.ac_id,sFn_reg.ac_seqn);
        if(iRet)
        {
            vtcp_log("����������ļ������α�ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("����������ļ������α�ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_log("û������˻�");
            strcpy(g_pub_tx.reply,"D104");
            set_zd_data("0130","û�и��˻�");
            goto ErrExit;
        }
        sDd_mst.ctl_amt -= sFn_reg.tx_amt;
        if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) < 0)
        {
            vtcp_log("���ƽ��Ϊ����[%lf]",sDd_mst.ctl_amt);
            strcpy(g_pub_tx.reply,"D354");
            set_zd_data("0130","���ƽ��Ϊ��");
            goto ErrExit;
        }
        iRet =  Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
        if(iRet)
        {
            vtcp_log("���»������ļ�ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"O012");
            goto ErrExit;
        }
        Dd_mst_Clo_Upd();
        /*�Ǽ�fn_reg_hst*/
        sFn_reg_hst.tx_date = g_pub_tx.tx_date;
        sFn_reg_hst.trace_no = g_pub_tx.trace_no;
        strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
        strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
        strcpy(sFn_reg_hst.type,"*");
        sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
        strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
        strcpy(sFn_reg_hst.agent_id,cAgent_id);
        strcpy(sFn_reg_hst.agent_name,cAgent_name);
        sFn_reg_hst.buy_date=sFn_reg.tx_date;
        sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
        strcpy(sFn_reg_hst.brf,"��Ʋ�Ʒ����");
        iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
        if(iRet)
        {
            Fn_reg_hst_Debug(&sFn_reg_hst);
            vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D345");
            goto ErrExit;
        }
        /*���ı��������*/
    	vtcp_log("sFn_reg.tx_amt = %.2lf",sFn_reg.tx_amt);
	dAmt_tmp = sFn_reg.tx_amt;
        iRet = upd_br_quota(sFn_parm,sFn_reg.tx_amt,1);
        if(iRet)
        {
            vtcp_log("���»���������[%d]",iRet);
            goto ErrExit;
        }
    }
    strcpy(g_pub_tx.brf,"������Ʋ�Ʒ");
    strcpy(g_pub_tx.ac_no,cAc_no);
    vtcp_log("sFn_reg.tx_amt = %.2lf",sFn_reg.tx_amt);
    pub_mob_sendmail(  g_pub_tx.brf, "2219",g_pub_tx.ac_no,"1",dAmt_tmp, 0.00 );
    if( pub_ins_trace_log() )
    {
        vtcp_log("�Ǽǽ�����ˮ����");
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("�����ɹ�");
    set_zd_data("0130","�����ɹ�");
    return 0;
ErrExit:
    vtcp_log("����ʧ��");
    return 1;
}
/*LJ20151105���������*/
int redempt_func()
{
    int iRet = 0;
    int  ret = 0;
    int iCount = 0;/*LJ*/
    int iHold_flag = 0;/*0�˻�δ��ȫ�����ᣬ1 �˻���ȫ������*/
    int  iIntst_flag = 0;
    long lDays = 0;
    
    double dAmt_tmp = 0.00;
    double dRedempt_amt = 0.00;
    double dTem_amt = 0.00;/*������ŵ��ڹ���������ؽ��Ĳ�ֵ*/
    double dTmp_amt = 0.00;
    double cTmt = 0.00;
    double sTsy = 0.00;
    double dProfit_rate = 0.000000; /*��������*/
   	double dFct_rate=0.00000000;/*ʵ��������*/
    double dProfit_amt = 0.00;/*������*/
    double dFct_intst = 0.00;/*����ʵ������*/
    double dFredem_amt =0.00; /*ʵ��һ����ص�Ǯ*/
    
    char cAc_no[25];
    char cPrdt_code[21];
    char cFd123[1000];
    char cTmp[21];
    char cBuy_date[9];
    char cBuy_trace_no[10];
    char cAgent_name[61];
    char cAgent_id[21];
    char cAgent_id_type[2];
    char cBrf[21];/*ժҪ*/
    char cAc_no_gz[25],cName[61];
    
    struct fn_parm_c sFn_parm;
    struct fn_reg_c sFn_reg;
    struct fn_reg_hst_c sFn_reg_hst;
    struct dd_mst_c sDd_mst;
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct fn_br_profit_c sFn_br_profit;
    struct fn_ac_rel_c sFn_ac_rel;
    
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cAc_no,0x00,sizeof(cAc_no));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
    memset(&sFn_br_profit,0x00,sizeof(sFn_br_profit));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(cTmp,0x00,sizeof(cTmp));
    memset(cBuy_trace_no,0x00,sizeof(cBuy_trace_no));
    memset(cBuy_date,0x00,sizeof(cBuy_date));
    
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0300",cAc_no);
    get_zd_data("0250",cAgent_name);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0690",cAgent_id_type);
    get_zd_double("0420",&dRedempt_amt);
    /*ǰ̨��ȡ��ؽ��󱣴���ؽ��*/
    dFredem_amt = dRedempt_amt;

    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
    if (iRet == 100)
    {
        vtcp_log( "�����ڸ��˻�!!ac_id=[%ld]", sMdm_ac_rel.ac_id);
        strcpy(g_pub_tx.reply, "B117");
        set_zd_data("0130","�����ڸ��˻�");
        goto ErrExit;
    }
    else if (iRet)
    {
        vtcp_log( "��ȡ�����˺Ŷ��ձ�ʧ��[%d]", iRet);
        strcpy(g_pub_tx.reply, "C114 ");
        goto ErrExit;
    }
        
    if(!strcmp(g_pub_tx.tty,"netb"))   /*�������������ˮ�ǵ�������  add by lwb 20141202*/
    {
    	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);	
    }
    /*�ȼ���Ʒ�ǲ��������*/
    iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
    if(iRet)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ������ʧ��");
        strcpy(g_pub_tx.reply, "D324");
        goto ErrExit;
    }
    if(sFn_parm.prdt_type[0] != '2')
    {
        vtcp_log("�ò�Ʒ���ǿ���ʽ");
        strcpy(g_pub_tx.reply, "D351");
        goto ErrExit;
    }
    if(sFn_parm.redempt[0] != 'Y')
    {
        vtcp_log("����Ʋ�Ʒ�������");
        strcpy(g_pub_tx.reply, "D351");
        set_zd_data("0130","����Ʋ�Ʒ�������");
        goto ErrExit;
    }
    if(sFn_parm.stat[0] != '0')
    {
        vtcp_log("�ò�Ʒ���Ϲ�״̬[%s] ",sFn_parm.stat);
        strcpy(g_pub_tx.reply,"C334");
        set_zd_data("0130","�ò�Ʒ���Ϲ�״̬");
        goto ErrExit;
    }
   	vtcp_log("sFn_parm.rate = [ %.2lf ] ",sFn_parm.rate);
    dFct_rate = sFn_parm.rate; /*��������ʱ������*/
    /* ���ݲ�Ʒ������ʺŽ������,����fn_reg��������ؽ���*/
   iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"ac_no = '%s' and prdt_code = '%s'  and sts = '1' order by count desc",cAc_no,cPrdt_code);
    vtcp_log("�α��� �� %d ��",iRet);
    if(iRet)
    {
        vtcp_log("������ƵǼǲ��α����");
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    /*iRet = sql_max_int("fn_reg","count","ac_no = '%s' and prdt_code = '%s'",cAc_no,cPrdt_code);*/
    iCount = sql_min_long("fn_reg","count","ac_no = '%s' and prdt_code = '%s' and sts = '1'",cAc_no,cPrdt_code);/*LJ*/
    while(1)
    {      
        iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
	        if(iRet != 100 && iRet != 0)
	        {
	            vtcp_log("ȡ��ƵǼǲ��α����");
	            strcpy(g_pub_tx.reply,"D330");
	            goto ErrExit;
	        }
	        
	        if(iRet == 0)
	        {      
	        	lDays =  pub_base_CalTrueDays(sFn_reg.ic_date,g_pub_tx.tx_date); /*��������ʱ������*/
	        	if(sFn_reg.count == iCount) /*�������ؽ������ж�*/
	        	{
	        		dTem_amt = sFn_reg.tx_amt - dRedempt_amt - sFn_reg.redempt_amt;
			        vtcp_log("dRedempt_amt = [ %.2lf ] ",dRedempt_amt);
			        vtcp_log("��һ�ν���ѭ��count = %d ",sFn_reg.count);
			        /*���ָ��˺ͶԹ�*/
			        if(sFn_reg.cif_type[0] == '1')
			        {
			        	if(dTem_amt >= sFn_parm.per_min_amt)
			        	{
			        		sFn_reg.redempt_amt += dRedempt_amt;
			        	}
			        	else
			        	{
			        		sFn_reg.redempt_amt = sFn_reg.tx_amt;
			        		strcpy(sFn_reg.sts,"#");
			        		dRedempt_amt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
			        		
			        	}
			        }
			        else
			        {
			        	if(dTem_amt >= sFn_parm.com_min_amt)
			        	{
			        		sFn_reg.redempt_amt += dRedempt_amt;
			        	}
			        	else
			        	{
			        		sFn_reg.redempt_amt = sFn_reg.tx_amt;
			        		strcpy(sFn_reg.sts,"#");
			        		dRedempt_amt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
			        	}
			        }
			        dFct_intst = 0.00;
			        sTsy = 0.00;
			        sTsy = dRedempt_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;/*��������*/
			        dFct_intst += pub_base_PubDround_1(sTsy);
   					sFn_reg.fct_intst += dFct_intst;
   					sFn_reg.date2 = g_pub_tx.tx_date;
			    	sFn_reg.return_date=g_pub_tx.tx_date;
					sFn_reg.return_trace_no=g_pub_tx.trace_no;
				    sFn_reg.return_capital += dRedempt_amt;
			        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
			        if(iRet)
			        {
			            vtcp_log("������Ʋ�Ʒ�Ǽǲ�����[%d] ",iRet);
			            strcpy(g_pub_tx.reply,"D333 ");
			            goto ErrExit;
			        }
	        	}
	        	if(sFn_reg.tx_amt - sFn_reg.redempt_amt > dRedempt_amt) /*ֱ�ӽ�����ز�������ͽ����ж�*/
	        	{
	        		dFct_intst = 0.00;
			        sTsy = 0.00;
	        		vtcp_log("�ڶ��ν���ѭ��count = %d ",sFn_reg.count);
	        		sFn_reg.redempt_amt += dRedempt_amt;
	        		sTsy = dRedempt_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;/*��������*/
			        dFct_intst += pub_base_PubDround_1(sTsy);
   					sFn_reg.fct_intst += dFct_intst;
				    sFn_reg.date2 = g_pub_tx.tx_date;
			    	sFn_reg.return_date=g_pub_tx.tx_date;
					sFn_reg.return_trace_no=g_pub_tx.trace_no;
				    sFn_reg.return_capital += dRedempt_amt;
			        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
			        if(iRet)
			        {
			            vtcp_log("������Ʋ�Ʒ�Ǽǲ�����[%d] ",iRet);
			            strcpy(g_pub_tx.reply,"D333 ");
			            goto ErrExit;
			        }
   					break;
	        		
	        	}
	        	else
        		{
        			dFct_intst = 0.00;
			        sTsy = 0.00;
        			vtcp_log("�����ν���ѭ��count = %d ",sFn_reg.count);
        			cTmt = sFn_reg.tx_amt - sFn_reg.redempt_amt;
        			sFn_reg.redempt_amt = sFn_reg.tx_amt;
		        	strcpy(sFn_reg.sts,"#");
		        	sTsy = cTmt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;/*��������*/
		        	dFct_intst += pub_base_PubDround_1(sTsy);
					sFn_reg.fct_intst += dFct_intst;
		        	dRedempt_amt -= cTmt;
        		    sFn_reg.date2 = g_pub_tx.tx_date;
			    	sFn_reg.return_date=g_pub_tx.tx_date;
					sFn_reg.return_trace_no=g_pub_tx.trace_no;
				    sFn_reg.return_capital = sFn_reg.tx_amt;
			        iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
			        if(iRet)
			        {
			            vtcp_log("������Ʋ�Ʒ�Ǽǲ�����[%d] ",iRet);
			            strcpy(g_pub_tx.reply,"D333 ");
			            goto ErrExit;
			        }
        		}
        		if(dRedempt_amt == 0)
        			break;

	        }
	        else
	        	break;
    }
    Fn_reg_Clo_Upd();
    ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",cPrdt_code);
    if(ret)
	{
   		vtcp_log("��ѯ��Ʋ�Ʒ�����˺����ñ����");
    	strcpy(g_pub_tx.reply,"S035");
   		goto ErrExit;
	}
    /*���Ǳ���������Ĳ������
    if(strcmp(sFn_reg.tx_br_no,g_pub_tx.tx_br_no) != 0)
    {

        vtcp_log("�ü�¼�Ǳ���������[%s] ",sFn_reg.tx_br_no);
        strcpy(g_pub_tx.reply,"D353 ");
        set_zd_data("0130","�ü�¼�Ǳ���������");
        goto ErrExit;
    }*/
    /*�н��ż���*/
    if(pub_base_CompDblVal(dFredem_amt,0.00) > 0)
    {
    		 /* �������ͻ��˻�(����) */
        ret = my_get_ac_no(sFn_reg.ac_no,cAc_no,cName);
        vtcp_log("�ʺ��� �� %s ��",sFn_reg.ac_no);
        if(ret)
        {
            vtcp_log("û�к��ʵ����˵��˺�![%s]",sFn_reg.ac_no);
            goto ErrExit;
        }
        ret = my_get_ac_hold(sFn_reg.tx_br_no,sFn_reg.ac_id,sFn_reg.ac_seqn,&iHold_flag,cAc_no_gz);
        if(ret)
        {
            vtcp_log("��ѯ�˺��Ƿ񱻶���ʧ��![%s]",sFn_reg.ac_no);
            goto ErrExit;
        }
            if(iHold_flag == 1)
        {
        	  sFn_reg.sts[0]='6';
        	  sFn_reg.return_date = g_pub_tx.tx_date;
        	  vtcp_log("return_date = %d",sFn_reg.return_date);
        	 
        	  ret=Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
        	  if(ret)
        	  {
    
	           	            vtcp_log("update fn_reg_hst error! ret[%d]  ",ret);
	           	            strcpy(g_pub_tx.reply,"D332");
	           	            goto ErrExit;
    	
        	  }
        	  
        }
        else
        {
	        strcpy(g_pub_tx.ac_no,cAc_no);
	        strcpy(g_pub_tx.name,"");
	        strcpy(cBrf,"��������");
	        g_pub_tx.cif_no = 0;
	        g_pub_tx.ac_id=sFn_reg.ac_id;
	        g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
	        g_pub_tx.tx_amt1=dFredem_amt;
	        g_pub_tx.add_ind[0]='1';
	        strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
	        strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
	        g_pub_tx.svc_ind=1001;
	        strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
	        strcpy(g_pub_tx.ac_id_type,"1");
	        strcpy(g_pub_tx.note_type,"");
	        strcpy(g_pub_tx.beg_note_no,"");
	        strcpy(g_pub_tx.brf,"��Ʒ�������");
	        vtcp_log("ac_id[%ld] tx_amt[%lf] �����ͻ��˻�(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
	        if ( pub_acct_trance() )
	        {
	            vtcp_log("���ô�ȡ������ʧ��!");
	            goto ErrExit;
	        }
	        set_zd_data("102K",g_pub_tx.ct_ind);
	        set_zd_double("1002",g_pub_tx.tx_amt1);
	        if ( pubf_acct_proc("LC02") )
	        {
	            vtcp_log("�Ǽǻ����ˮ����!");
	            goto ErrExit;
	        }
	    }
    }
    dTmp_amt = 0.00;
    dFct_intst =0.00;
    /*�������� dFct_intst���Ǻ��ڼ��Ͽ���ʽ�ĵ������ʵ�δ��������*/
    vtcp_log("lDays= %d dFct_rate= %d ",lDays,dFct_rate);
    dTmp_amt = dFredem_amt * lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;
    set_zd_double("0400",dTmp_amt);
    dFct_intst += pub_base_PubDround_1(dTmp_amt);
    vtcp_log("���μ����������dTmp_amt[%lf]������ʵ������dFct_intst[%lf] Ԥ��sFn_reg.intst[%lf]",dTmp_amt,dFct_intst,sFn_reg.intst);
    if(pub_base_CompDblVal(dFct_intst,0.00) > 0)
    {
        /* �跽��224199����Ӧ����-����ʽ����㻧(����) */
        g_pub_tx.tx_amt1=dFct_intst;
        strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
        strcpy(g_pub_tx.name,"");
        g_pub_tx.cif_no=0;
        g_pub_tx.ac_id=0;
        g_pub_tx.ac_seqn=0;
        g_pub_tx.add_ind[0]='0';
        strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
        strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
        g_pub_tx.svc_ind=9001;
        strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
        strcpy(g_pub_tx.ac_id_type,"9");
        strcpy(g_pub_tx.note_type,"");
        strcpy(g_pub_tx.beg_note_no,"");
        strcpy(g_pub_tx.brf,"��Ʒ�������");
        vtcp_log("ac_id[%ld] tx_amt[%lf] �裺����Ӧ����-����ʽ����㻧(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
        if ( pub_acct_trance() )
        {
            vtcp_log("���ô�ȡ������ʧ��!");
            goto ErrExit;
        }
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("102I",g_pub_tx.tx_amt1);
        if ( pubf_acct_proc("LC01") )
        {
            vtcp_log("�Ǽǻ����ˮ����!");
            goto ErrExit;
        }
                    /*ȫ������Ĵ浽�����˺�*/
        if(iHold_flag == 1)
        {
        		
        		
            set_zd_data("096H",cAc_no_gz);
            snprintf(g_pub_tx.brf,20,"%s����",sFn_reg.prdt_code);
            if(func_in_acct(cAc_no_gz,"1",dFct_intst))
            {
                vtcp_log("����ʧ��");
                goto ErrExit;
            }
            /*�������fn_reg��filler���� ��¼���ǹ�����*/
            sprintf(sFn_reg.filler+strlen(sFn_reg.filler),"|%.2lf",dFct_intst);
        }
        /* �������ͻ��˻�(����) */
        else
        {
	        g_pub_tx.tx_amt1=dFct_intst;
	        strcpy(g_pub_tx.ac_no,cAc_no);/*ֱ��ȡ������˺ž�����*/
	        strcpy(g_pub_tx.name,"");
	        g_pub_tx.cif_no = 0;
	        g_pub_tx.ac_id=sFn_reg.ac_id;
	        g_pub_tx.ac_seqn=sFn_reg.ac_seqn;
	        g_pub_tx.add_ind[0]='1';
	        strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
	        strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
	        g_pub_tx.svc_ind=1001;
	        strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
	        strcpy(g_pub_tx.ac_id_type,"1");
	        strcpy(g_pub_tx.note_type,"");
	        strcpy(g_pub_tx.beg_note_no,"");
	        strcpy(g_pub_tx.brf,"��Ʒ�������");
	        vtcp_log("ac_id[%ld] tx_amt[%lf] �����ͻ��˻�(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
	        if ( pub_acct_trance() )
	        {
	            vtcp_log("���ô�ȡ������ʧ��!");
	            goto ErrExit;
	        }
	        set_zd_data("102K",g_pub_tx.ct_ind);
	        set_zd_double("1002",g_pub_tx.tx_amt1);
	        if ( pubf_acct_proc("LC02") )
	        {
	            vtcp_log("�Ǽǻ����ˮ����!");
	            goto ErrExit;
	        }
        }
    }
	    /*�跽��20110199���������� */
	    /*strcpy(g_pub_tx.tx_br_no,QS_BR_NO);�������Ĺ黹���ͻ�*/
	    strcpy(g_pub_tx.name,"");
	    /*mod by lwb 201341202 ����ͱ��𶼴������˺ų�*/
	    strcpy(g_pub_tx.ac_no,sFn_ac_rel.qs_ac_no);
	    /*if( sFn_reg.cif_type[0]=='1' )
	    {
	        strcpy(g_pub_tx.ac_no,sFn_parm.per_ac_no);
	    }
	    else if( sFn_reg.cif_type[0]=='2'||sFn_reg.cif_type[0]=='3'||sFn_reg.cif_type[0]=='4'||sFn_reg.cif_type[0]=='6' ) /add by lwb ���ӿͻ�����Ϊ3,4,6���ж�/
	    {
	        strcpy(g_pub_tx.ac_no,sFn_parm.com_ac_no);
	    }*/
	    /*�ǿ�Ŀ�Ļ� ��������Ϊ��������*/
	    if(strlen(g_pub_tx.ac_no) < 10)
	    {
	        strcpy(g_pub_tx.opn_br_no,QS_BR_NO);
	    }
	    g_pub_tx.cif_no = 0;
	    g_pub_tx.ac_id = 0;
	    g_pub_tx.ac_seqn=0;
	    g_pub_tx.tx_amt1=dFredem_amt;
	    g_pub_tx.add_ind[0]='0';
	    strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
	    strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
	    /*g_pub_tx.svc_ind=1001;*/
	    g_pub_tx.svc_ind=9001;
	    strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
	    strcpy(g_pub_tx.ac_id_type,"9");
	    strcpy(g_pub_tx.note_type,"");
	    strcpy(g_pub_tx.beg_note_no,"");
	    strcpy(g_pub_tx.brf,"��Ʒ�������");
	    vtcp_log(" ac_id[%ld] tx_amt[%lf] �裺�������(����) ",g_pub_tx.ac_id,g_pub_tx.tx_amt1);
	    if ( pub_acct_trance() )
	    {
	        vtcp_log("���ô�ȡ������ʧ��!");
	        goto ErrExit;
	    }
	    set_zd_data("102K",g_pub_tx.ct_ind);
	    set_zd_double("102I",g_pub_tx.tx_amt1);
	    if ( pubf_acct_proc("LC01") )
	    {
	        vtcp_log("�Ǽǻ����ˮ����!");
	        goto ErrExit;
	    }
	 /*�Ǽǻ�������*/
    {
	    ret = Fn_br_profit_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s' and sts='0'",sFn_reg.prdt_code,sFn_reg.tx_br_no);
	    if(ret)
	    {
	        vtcp_log("������»��������ʧ��");
	        strcpy(g_pub_tx.reply,"D346");
	        goto ErrExit;
	    }
	    ret = Fn_br_profit_Fet_Upd(&sFn_br_profit,g_pub_tx.reply);
	    if(ret  == 100)
	    {
	        /*û�м�¼ ˵��û����ع� ��һ�δ���*/
	        vtcp_log("û�л����ķ�����Ϣ���Ǽ�һ��");
	        strcpy(sFn_br_profit.prdt_code,sFn_reg.prdt_code);
	        strcpy(sFn_br_profit.br_no,sFn_reg.tx_br_no);
	        sFn_br_profit.tx_amt =0;
	        sFn_br_profit.redempt_amt = dFredem_amt ;
	        sFn_br_profit.profit_amt = 0;
	        sFn_br_profit.reg_date = g_pub_tx.tx_date;
	        sFn_br_profit.reg_trace_no = g_pub_tx.trace_no;
	        strcpy(sFn_br_profit.sts,"0");
	        strcpy(sFn_br_profit.brf,"������");
	        ret = Fn_br_profit_Ins(sFn_br_profit,g_pub_tx.reply);
	        if(ret)
	        {
	            Fn_br_profit_Debug(&sFn_br_profit);
	            vtcp_log("�Ǽǻ��������ʧ��[%d]",ret);
	            strcpy(g_pub_tx.reply,"D348");
	            goto ErrExit;
	        }
	    }
	    else if(ret == 0)
	    {
	        vtcp_log("�з�����Ϣ ����");
			sFn_br_profit.redempt_amt += dFredem_amt ;
	        ret = Fn_br_profit_Upd_Upd(sFn_br_profit,g_pub_tx.reply);
	        if(ret)
	        {
	            vtcp_log("���»��������ʧ��");
	            strcpy(g_pub_tx.reply,"D346");
	            goto ErrExit;
	        }
	    }
	    Fn_br_profit_Clo_Upd();
    }
    
	    /*�Ǽ�fn_reg_hst*/
	    sFn_reg_hst.tx_date = g_pub_tx.tx_date;
	    sFn_reg_hst.trace_no = g_pub_tx.trace_no;
	    strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
	    strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
	    strcpy(sFn_reg_hst.type,"6");
	    sFn_reg_hst.tx_amt = dFredem_amt;
	    strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
	    strcpy(sFn_reg_hst.agent_id,cAgent_id);
	    strcpy(sFn_reg_hst.agent_name,cAgent_name);
	    sFn_reg_hst.buy_date=sFn_reg.tx_date;
	    sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
	    strcpy(sFn_reg_hst.brf,"����ʽ��Ʒ���");
	    iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
	    if(iRet)
	    {
	        Fn_reg_hst_Debug(&sFn_reg_hst);
	        vtcp_log("�Ǽ���Ʋ�Ʒ�Ǽǲ�����iRet[%d]",iRet);
	        strcpy(g_pub_tx.reply,"D345");
	        goto ErrExit;
	    }
 
    	/*���ı��������*/
		vtcp_log("sFn_reg_hst.tx_amt = %.2lf",sFn_reg_hst.tx_amt); /*�����Ľ�����Ϲ�����Ϊ��ؽ��*/
		dAmt_tmp = sFn_reg_hst.tx_amt;
	    iRet = upd_br_quota(sFn_parm,sFn_reg_hst.tx_amt,1);
	    if(iRet)
	    {
	        vtcp_log("���»���������[%d]",iRet);
	        goto ErrExit;
	    }
	      
	    strcpy(g_pub_tx.brf,"�����Ʋ�Ʒ");
	    strcpy(g_pub_tx.ac_no,cAc_no);
	    pub_mob_sendmail(  g_pub_tx.brf, "2219",g_pub_tx.ac_no,"1",dAmt_tmp, 0.00 );
	    if( pub_ins_trace_log() )
	    {
	        vtcp_log("�Ǽǽ�����ˮ����");
	        strcpy(g_pub_tx.reply,"P126");
	        goto ErrExit;
	    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("��سɹ�");
    set_zd_data("0130","��سɹ�");
    return 0;
ErrExit:
    vtcp_log("���ʧ��");
    return 1;
}

/*�õ������������  ֻ���Ϲ�ʱ�����깺���㷨��һ��*/
int get_br_quota(struct fn_parm_c sfn_parm,double *quota_amt)
{
    int iRet  = 0;
    double dSum_amt= 0.00;/*���Ϲ��ܶ�*/
    struct fn_br_quota_parm_c sFn_br_qp;
    memset(&sFn_br_qp        ,0x00,sizeof(sFn_br_qp));
    if(sfn_parm.quota_flag[0] == 'Y')
    {
        /*��鱾������Ʋ�Ʒ�Ƿ����Ϲ�*/
        iRet = Fn_br_quota_parm_Sel(g_pub_tx.reply,&sFn_br_qp,"prdt_code='%s' and br_no='%s'",\
            sfn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            /*����û���������ж��Ƿ����Ϲ�*/
            if(sfn_parm.quota_flag[1] == 'Y')
            {
                /*��ѯ���е��Ϲ����*/
                iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",\
                    sfn_parm.prdt_code);
                if(iRet)
                {
                    vtcp_log("ͳ��fn_reg����!");
                    strcpy(g_pub_tx.reply, "D331");
                    goto ErrExit;
                }
                vtcp_log("dSum_amt[%lf]",dSum_amt);
                *quota_amt = sfn_parm.sale_amt - dSum_amt;
            }
            else
            {
                *quota_amt = 0.00;
            }    
        }
        else
        {
            /*ֱ�ӷ���ʣ�����*/
            vtcp_log("sFn_br_qp.available_amt[%lf]",sFn_br_qp.available_amt);
            *quota_amt = sFn_br_qp.available_amt;
        }
    }
    else/*����������ȡȫ��*/
    {
        /*��ѯ���е��Ϲ����*/
        iRet = sql_sum_double("fn_reg","tx_amt",&dSum_amt,"prdt_code='%s' and sts='0'",\
            sfn_parm.prdt_code);
        if(iRet)
        {
            vtcp_log("ͳ��fn_reg����!");
            strcpy(g_pub_tx.reply, "D331");
            goto ErrExit;
        }
        vtcp_log("dSum_amt[%lf]",dSum_amt);
        *quota_amt = sfn_parm.sale_amt - dSum_amt;
    }
    vtcp_log("dSum_amt[%lf]",*quota_amt);
    if(pub_base_CompDblVal(*quota_amt,0.00) < 0)
    {
        *quota_amt  = 0.00;
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("��ȡ���ɹ� quota_amt[%lf]",*quota_amt);
    return 0;
ErrExit:
    vtcp_log("��ȡ���ʧ��");
    return 1;
}
/*�õ�����ͻ��������ϸ�����,�жϿͻ����ͺ��˺��Ƿ�һ��*/
int get_flt_radio(char *prdt_code,double tx_amt,char * cif_lvl,long ac_id,double *flt_radio)
{
    int iRet = 0;
    double dBuy_amt = 0.00;
    double dFlt_radio1 = 0.000000;
    double dFlt_radio2 = 0.000000;
    double dFlt_radio3 = 0.000000;
    struct fn_flt_radio_parm_c sFn_flt_rp;
    memset(&sFn_flt_rp        ,0x00,sizeof(sFn_flt_rp));
    /*ͳ������˺������Ʒһ�����˶���*/
    iRet = sql_sum_double("fn_reg","tx_amt",&dBuy_amt,"prdt_code='%s' and ac_id=%ld and sts='0'",\
            prdt_code,ac_id);
    if(iRet)
    {
        vtcp_log("ͳ��fn_reg����!");
        strcpy(g_pub_tx.reply, "D331");
        goto ErrExit;
    }
    vtcp_log("�Ѿ��Ϲ����[%lf]",dBuy_amt);
    /*ȡ�����Ʒ���ϸ�����*/
    iRet = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_rp,"prdt_code='%s'",prdt_code);
    if(iRet)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ�������ʱ����!iRet[%d]",iRet);
        strcpy(g_pub_tx.reply, "D355");
        goto ErrExit;
    }
    if(sFn_flt_rp.ctl_flag[0]  == '0')
    {
        /*ȡ���ֵ*/
        if(cif_lvl[0] >= sFn_flt_rp.cif_lvl[0])
        {
        	if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
            dFlt_radio1 += sFn_flt_rp.flt_radio2;
        }
       /* if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
        {
            dFlt_radio2= sFn_flt_rp.flt_radio21;
        }*/
        /** �ж��¼�4�����������ж���׼  ȡ��󵥱ʽ��׽��ĸ������� ͨ���ȽϽ���ǰ�渡�����ʻ�ȡ */
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt1) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt1 , 0.00)>0))  
        {
            dFlt_radio2= sFn_flt_rp.flt_radio21;
        }
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt2) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt2 , 0.00)>0))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio22;
        }
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt3) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt3 , 0.00)>0))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio23;
        }
                if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt4) >= 0  && (pub_base_CompDblVal(sFn_flt_rp.buy_amt4 , 0.00)>0))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio24;
        }
        /**end 20140630*/
        if(pub_base_CompDblVal(dBuy_amt,sFn_flt_rp.total_amt) >=0)
        {
            dFlt_radio3 += sFn_flt_rp.flt_radio3;
        }
        /*ȡһ������*/
        if(dFlt_radio1 - dFlt_radio2 > 0.000001)
        {
            if(dFlt_radio1 - dFlt_radio3 > 0.000001)
            {
                *flt_radio = dFlt_radio1;
            }
            else
            {    
                *flt_radio = dFlt_radio3;
            }
        }
        else
        {
            if(dFlt_radio2 - dFlt_radio3 > 0.000001)
            {
                *flt_radio = dFlt_radio2;
            }
            else
            {    
                *flt_radio = dFlt_radio3;
            }
        }
        vtcp_log("�ϸ�����[%lf]",*flt_radio);
    }
    else
    {
        /*�ۼ�*/
        if(cif_lvl[0] >= sFn_flt_rp.cif_lvl[0])
        {
            *flt_radio += sFn_flt_rp.flt_radio1;
        }
        if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
        {
            *flt_radio += sFn_flt_rp.flt_radio2;
        }
        if(pub_base_CompDblVal(dBuy_amt,sFn_flt_rp.total_amt) >=0)
        {
            *flt_radio += sFn_flt_rp.flt_radio3;
        }
        vtcp_log("�ϸ�����[%lf]",*flt_radio);
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("��ȡ�ϸ������ɹ�");
    return 0;
ErrExit:
    vtcp_log("��ȡ�ϸ�����ʧ��");
    return 1;
}
/*����˻�����*/
int get_cif_type(struct mdm_ac_rel_c smdm_ac_rel,char * cif_type,char *cif_lvl)
{
    int iRet = 0;
    struct cif_id_code_rel_c sCif_id_code_rel;
    struct cif_basic_inf_c sCif_basic_inf;
    memset(&sCif_id_code_rel,0x00,sizeof(sCif_id_code_rel));
    memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));

    /*ȡ�ͻ�id*/
    iRet = Cif_id_code_rel_Sel(g_pub_tx.reply,&sCif_id_code_rel,"id_type='%s' and id_no='%s'",\
        smdm_ac_rel.id_type,smdm_ac_rel.id_no);
    if(iRet == 100)
    {
        vtcp_log("δ�ڱ��н����ͻ���Ϣ");
        strcpy(g_pub_tx.reply,"B116");
        goto ErrExit;
    }
    else if(iRet)
    {
        vtcp_log("��ѯ�ͻ�֤����Ϣʧ�ܡ�%d��",iRet);
        strcpy(g_pub_tx.reply,"C002");
        goto ErrExit;
    }
    /*ȡ�ͻ�������Ϣ*/
    iRet = Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%ld",sCif_id_code_rel.cif_no);
    if(iRet)
    {
        vtcp_log("��ѯ�ͻ�������Ϣʧ�ܡ�%d��",iRet);
        strcpy(g_pub_tx.reply,"C113");
        goto ErrExit;
    }
    strcpy(cif_type,sCif_basic_inf.type); 
    cif_type[1] ='\0';
    /*strcpy(cif_lvl,sCif_basic_inf.cif_lvl);*/
    strcpy(cif_lvl,"5");
    cif_lvl[1] = '\0';
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("�ͻ����ͺ��˺Ŷ�Ӧ����� cif_lvl[%s]",cif_lvl);
    return 0;
ErrExit:
    vtcp_log("��֤�ͻ�����ʧ��");
    return 1;
}
/*����˺ŵĽ���״̬*/
int check_note_sts(char *note_sts)
{
    /*������*/
    if (note_sts[0] == '1')
    {
        vtcp_log( "�ý����ѹ�ʧ!");
        strcpy(g_pub_tx.reply, "P170");
        goto ErrExit;
    }
    else if (note_sts[0] == '2')
    {
        vtcp_log( "�ý����ѹ�ʧ��֤!");
        strcpy(g_pub_tx.reply, "P171");
        goto ErrExit;
    }
    else if (note_sts[0] == '3')
    {
        vtcp_log( "�ý�����������֤!");
        strcpy(g_pub_tx.reply, "P104");
        goto ErrExit;
    }
    else if (note_sts[0] == '*')
    {
        vtcp_log("�ý���������!");
        strcpy(g_pub_tx.reply, "M012");
        goto ErrExit;
    }
    else if (note_sts[0] == '4')
    {
        vtcp_log( "�ý����������ᣬ���������ʺ�!");
        strcpy(g_pub_tx.reply, "O217");
        goto ErrExit;
    }
    else if(note_sts[0] != '0')
    {
        vtcp_log( "����״̬������!");
        strcpy(g_pub_tx.reply, "M013");
        goto ErrExit;
    }
OkExit:
    return 0;
ErrExit:
    return 1;    
}

/*���»������*/
int upd_br_quota(struct fn_parm_c sfn_parm,double tx_amt,int mode)
{
    int iRet  = 0;
    struct fn_br_quota_parm_c sFn_br_qp;
    memset(&sFn_br_qp        ,0x00,sizeof(sFn_br_qp));
    if(sfn_parm.quota_flag[0] == 'Y')
    {
        /*��鱾������Ʋ�Ʒ�Ƿ����Ϲ�*/
        iRet = Fn_br_quota_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and br_no='%s'",\
            sfn_parm.prdt_code,g_pub_tx.tx_br_no);
        if(iRet)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        iRet = Fn_br_quota_parm_Fet_Upd(&sFn_br_qp,g_pub_tx.reply);
        if(iRet != 0 && iRet != 100)
        {
            vtcp_log("��ѯ��Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
            strcpy(g_pub_tx.reply,"D327");
            goto ErrExit;
        }
        else if(iRet == 100)
        {
            vtcp_log("û���޶����");
        }
        else
        {
            if(mode == 0)
            {
                sFn_br_qp.available_amt -= tx_amt;
            }
            else if(mode == 1)
            {
                sFn_br_qp.available_amt += tx_amt;
            }
            else
            {
                strcpy(g_pub_tx.reply,"S035");
                goto ErrExit;
            }
            iRet = Fn_br_quota_parm_Upd_Upd(sFn_br_qp,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("������Ʋ�Ʒ��������ʧ��iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D348");
                goto ErrExit;
            }
        }
        Fn_br_quota_parm_Clo_Upd();
    }
    else/*û������*/
    {
        
    }
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log("�������ɹ� ");
    return 0;
ErrExit:
    vtcp_log("�������ʧ��");
    return 1;
}


/*���ݿͻ��˺��ҵ������˵��˺� �Ҳ��� �ͱ���shangyongchao 2013/3/9 20:46:46
ac_no ������˺�
new_ac_no ����ac_id�ҵ���״̬�������˺�*/
static int my_get_ac_no(char *ac_no,char *new_ac_no ,char *name)
{
    int iRet = 0;
    long lAc_id = 0;
    struct mdm_ac_rel_c sMdm_ac_rel;
    
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    iRet  = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",ac_no);
    if(iRet != 0 && iRet != 100)
    {
        vtcp_log( "�����˺�ʧ��![%d]", iRet);
        return 1;
    }
    else if(iRet == 100)
    {
        vtcp_log( "�޴��˺�![%s]", ac_no);
        return 1;
    }
    /*�����͹�ʧ�� ֱ�ӷ��ؾ����� ����������*/
    if(sMdm_ac_rel.note_sts[0] == '1' || sMdm_ac_rel.note_sts[0] =='0')
    {
        strcpy(new_ac_no,ac_no);
        /*strcpy(name,sMdm_ac_rel.name);*/
        return 0;
    }
    lAc_id = sMdm_ac_rel.ac_id;
    /*����״̬�� �� ac_id����� �������߹�ʧ���˺� ʵ���Ҳ����ͱ���*/
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld and note_sts in('0','1')",lAc_id);
    if(iRet != 0 && iRet != 100)
    {
        sprintf(acErrMsg, "�����˺�ʧ��![%d]", iRet);
        WRITEMSG
        return 1;
    }
    else if(iRet == 100)
    {
        sprintf(acErrMsg, "�޴˺��������˺�![%s]", ac_no);
        WRITEMSG
        return 1;
    }
    strcpy(new_ac_no,sMdm_ac_rel.ac_no);
    /*strcpy(name,sMdm_ac_rel.name);*/
    return 0;
}

/**���˴���**/
static int my_get_ac_hold(char *br_no,long ac_id ,int ac_seqn,int *hold_flag,char *ac_no)
{
    int ret = 0;
    struct dd_mst_c sDd_mst;
    struct com_parm_c sCom_parm;
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sCom_parm,0x00,sizeof(sCom_parm));
    
    ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
    if(ret)
    {
        vtcp_log("��ѯ�������ļ�ʧ��[%d]",ret);
        strcpy(g_pub_tx.reply,"D230");
        return 1;
    }
    /*ȫ������ĵ��²����������� ���ر�־*/
    if(sDd_mst.hold_sts[0] == '1')
    {
        *hold_flag = 1;
        /*ȡ�Ϲ������Ĺ����˺�*/
       /* ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='LC_GZ' and parm_seqn=%d",br_no);
        if(ret)
        {
            vtcp_log("��ѯ�������������[%d]",ret);
            strcpy(g_pub_tx.reply,"P465");
            return 1;
        }
        strcpy(ac_no,sCom_parm.val);*/
    }
    else
    {
        *hold_flag = 0;
    }
    return 0;
}
/*�ڲ��ʼ���*/
static int func_in_acct(char *ac_no,char *dc_ind,double amt)
{
    memset(&g_pub_tx.name,0x00,sizeof(g_pub_tx.name));
    memset(&g_pub_tx.ac_no,0x00,sizeof(g_pub_tx.ac_no));
    g_pub_tx.cif_no = 0;
    strcpy(g_pub_tx.ac_no,ac_no);
    /*��ˮ����*/
    strcpy(g_pub_tx.ac_wrk_ind,"0000010011");
    vtcp_log("����g_pub_tx.ac_wrk_ind[%s]",g_pub_tx.ac_wrk_ind);
    g_pub_tx.ac_id = 0;
    g_pub_tx.ac_seqn=0;
    g_pub_tx.tx_amt1=amt;
    g_pub_tx.add_ind[0]=dc_ind[0];
    strcpy(g_pub_tx.hst_ind,"1");               /* �ռ�����ϸ */
    g_pub_tx.svc_ind=9001;
    strcpy(g_pub_tx.ct_ind,"2");                /* ת�� */
    strcpy(g_pub_tx.ac_id_type,"9");
    strcpy(g_pub_tx.note_type,"");
    strcpy(g_pub_tx.beg_note_no,"");
    if ( pub_acct_trance() )
    {
        vtcp_log("���ô�ȡ������ʧ��!");
        return 1;
    }
    if(dc_ind[0] == '0')
    {
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("102I",g_pub_tx.tx_amt1);
        if ( pubf_acct_proc("LC01") )
        {
            vtcp_log("�Ǽǻ����ˮ����!");
            return 1;
        }
    }
    else
    {
        set_zd_data("102K",g_pub_tx.ct_ind);
        set_zd_double("1002",g_pub_tx.tx_amt1);
        if ( pubf_acct_proc("LC02") )
        {
            vtcp_log("�Ǽǻ����ˮ����!");
            return 1;
        }
    }
    return 0;
}