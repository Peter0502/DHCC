/*********************************************************************
* �� �� �� :  
* �������� :  �ؽ���
* ��    �� :  2014/10/28
* ����ģ�� :  ��Ʋ�Ʒ����Ԥ��
* �������� :  spW211.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5055', '��Ʋ�Ʒ����Ԥ��', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '0', '4', '2',);
insert into tx_flow_def values ('5055', 0, 'W211', '#$');
insert into tx_sub_def values ('W211', '��Ʋ�Ʒ����Ԥ��', 'spW211', '0', '0');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_br_quota_parm_c.h"
#include "cif_ass_reg_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "fn_ac_rel_c.h"
#include "fn_flt_radio_parm_c.h"
#include "mo_prdt_prot_c.h"
#define L365 365
int get_flt_radio(char *,double,double *);

int spW211()
{
	double dBuyAmt=0.00;			/*���*/
	char sProCode[20];				/*��Ʒ����*/
	char sComputMode[1+1];			/*���㷽ʽ*/
	char sRate[17];					/*������*/
	long lEndDate=0;				/*��������*/
	double dRate = 0.000000;		/*����*/
	double dFlt_radio=0.000000;		/*��������*/
	long lDays = 0;
	double dIntst = 0.00;
	int ret = 0;
	struct fn_parm_c sFn_parm;
	double dRatebuf=0.00;
		
	memset(sProCode,0x00,sizeof(sProCode));
	memset(sRate,0x00,sizeof(sRate));
	memset(sComputMode,0x00,sizeof(sComputMode));
	memset(&sFn_parm,0x00,sizeof(sFn_parm));
	/**��ʼ�����ýṹadd by guanjianfeng 20150907**/
	
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	get_zd_double("0390",&dBuyAmt);
	get_zd_data("0370",sProCode);
	get_zd_data("0680",sComputMode);
	
	ret=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sProCode);
    if( ret != 0 && ret != 100)
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D324" );
        goto ErrExit;
    }
    else if(ret  == 100)
    {
        vtcp_log("û�������Ʒ! ret[%d]prdt_code[%s] ",ret,sProCode);
        set_zd_data("0120","D157");
        set_zd_data("0130","û�������Ʒ!");
        strcpy( g_pub_tx.reply,"D104" );
        goto ErrExit;
    }

	if(sComputMode[0] == '0')/*��������ԭ��������*/
    {
    	get_zd_double("0560",&dRate);
    	/*get_zd_double("0560",&dRatebuf);
    	dRate=dRatebuf/100;*/
    	vtcp_log("%s %d::����������=[%.2f] ",__FILE__,__LINE__,dRate);
        lDays = pub_base_CalTrueDays(sFn_parm.ic_date,sFn_parm.mtr_date);
        vtcp_log("ic_date[%ld] end_date%ld] lDays[%ld]",sFn_parm.ic_date,sFn_parm.return_date,lDays);
    }
    else if(sComputMode[0] =='1')/*�������� ������ֹ����*/
    {
    	get_zd_double("0560",&dRate);
    	/*get_zd_double("0560",&dRatebuf);
    	dRate=dRatebuf/100;*/
    	vtcp_log("%s %d::����������=[%.2f] ",__FILE__,__LINE__,dRate);
    	get_zd_long("0450",&lEndDate);   	
    	if(lEndDate > sFn_parm.mtr_date || lEndDate < sFn_parm.ic_date)
    	{
    		set_zd_data("0120","P006");
    		set_zd_data("0130","�������ֹ��������");
			goto ErrExit;
    	}
        lDays = pub_base_CalTrueDays(sFn_parm.ic_date,lEndDate);
        vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_parm.ic_date,lEndDate,lDays);
    }
    else if(sComputMode[0] =='2')/*����ԭ������*/
    {
        dRate = sFn_parm.rate;
        lDays = pub_base_CalTrueDays(sFn_parm.ic_date,sFn_parm.mtr_date);
        vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_parm.ic_date,sFn_parm.return_date,lDays);
    }
    else/*ԭ���� ������ֹ����*/
    {
        dRate = sFn_parm.rate;
        get_zd_long("0450",&lEndDate);
        if(lEndDate > sFn_parm.mtr_date || lEndDate < sFn_parm.ic_date)
    	{
    		set_zd_data("0120","P006");
    		set_zd_data("0130","�������ֹ��������");
			goto ErrExit;
    	}
        lDays = pub_base_CalTrueDays(sFn_parm.ic_date,lEndDate);
        vtcp_log("ic_date[%ld] end_date[%ld] lDays[%ld]",sFn_parm.ic_date,lEndDate,lDays);
    }
    /*�õ���������*/
    if(sFn_parm.float_rate_flag[0] == 'Y')
    {
		ret = get_flt_radio(sProCode,dBuyAmt,&dFlt_radio);
		if(ret == -1)
		{
			set_zd_data("0120","9999");
			set_zd_data("0130","�õ���������ʧ��");
			goto ErrExit;
		}
		vtcp_log("�����ϸ�����Ϊ[%9.2f]",dFlt_radio);
	}
	else
	{
		dFlt_radio = 0.000000;
	}
	/*����������*/
	double TRate = 0.00;
	TRate = dRate * (1+dFlt_radio);
	
	if(lDays > 360 && lDays <367)
    {
		lDays = 365;
    }
	/*����Ԥ������*/
	dIntst = dBuyAmt * lDays * dRate * (1+dFlt_radio)/L365/L100;
	vtcp_log("����Ϊ[%lf]",dIntst);
	set_zd_double("0330",dIntst);
	set_zd_double("0320",TRate);
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    set_zd_data("0120","0000");
	set_zd_data("0130","��Ʋ�Ʒ����Ԥ��ɹ�");
    return 0;
ErrExit:
    vtcp_log("��Ʋ�Ʒ����Ԥ��ʧ��[%s]", g_pub_tx.reply);
    return 1;  
}
int get_flt_radio(char *prdt_code,double tx_amt,double *flt_radio)
{
    int iRet = 0;
    double dBuy_amt = tx_amt;
    double dFlt_radio1 = 0.000000;
    double dFlt_radio2 = 0.000000;
    double dFlt_radio3 = 0.000000;
    struct fn_flt_radio_parm_c sFn_flt_rp;
    memset(&sFn_flt_rp,0x00,sizeof(sFn_flt_rp));

    vtcp_log("�Ѿ��Ϲ����[%9.2f]",dBuy_amt);
    /*ȡ�����Ʒ���ϸ�����*/
    iRet = Fn_flt_radio_parm_Sel(g_pub_tx.reply,&sFn_flt_rp,"prdt_code='%s'",prdt_code);
    if(iRet)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ�������ʱ����!iRet[%d]",iRet);
        strcpy(g_pub_tx.reply, "D355");
        return -1;
    }
    if(sFn_flt_rp.ctl_flag[0]  == '0')
    {
        /*ȡ���ֵ*/
        if('5' >= sFn_flt_rp.cif_lvl[0])
        {
        	if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt) >= 0)
            dFlt_radio1 += sFn_flt_rp.flt_radio2;
        }
        if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt1) >= 0  && (sFn_flt_rp.buy_amt1!=0.00))  
        {
            dFlt_radio2= sFn_flt_rp.flt_radio21;
        }
        if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt2) >= 0  && (sFn_flt_rp.buy_amt2!=0.00))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio22;
        }
        if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt3) >= 0  && (sFn_flt_rp.buy_amt3!=0.00))
        {
            dFlt_radio2= sFn_flt_rp.flt_radio23;
        }
        if(pub_base_CompDblVal(tx_amt,sFn_flt_rp.buy_amt4) >= 0  && (sFn_flt_rp.buy_amt4!=0.00))
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
        vtcp_log("�ϸ�����[%9.2f]",*flt_radio);
    }
    else
    {
        /*�ۼ�*/
        if('5' >= sFn_flt_rp.cif_lvl[0])
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
    return 0;
}