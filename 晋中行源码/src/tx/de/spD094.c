/***************************************************************
* �� �� ��: spD094.c
* ��������������ʽ��Ʋ�Ʒ��ǰ������������ѡ�
            ʵ��������  ��Ʒ��Ϣ��ֹ����
            ��Ʋ�Ʒ��������:
            ʵ�������� 
            �������ֻ����״̬


            
* ��    ��: shangyongchao
* ������ڣ�2013/5/21
*
* �޸ļ�¼:
* ��    ��:
* �� �� ��:
* ��    ע:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_ac_rel_c.h"
#include "mdm_ac_rel_c.h"
#include "in_mst_c.h"
#include "com_tel_c.h"
#define L365 365
int spD094()
{
    char cTx_code[9];
    int iFlag = 0 ;
    memset(cTx_code,0x00,sizeof(cTx_code));

    pub_base_sysinit();
    get_zd_data("0650",cTx_code);

    /*add by lwb 20140820 ���ӶԻ������ж�*/
    if(strcmp(g_pub_tx.tx_br_no,QS_BR_NO))
    {
	sprintf(acErrMsg,"���������Ĳ��������˽���!");
	WRITEMSG
	strcpy(g_pub_tx.reply,"P047");
	goto ErrExit;
    }

    if(strcmp(cTx_code,"0001") == 0)
    {
        if(func_0001(&iFlag))
        {
            vtcp_log("�޸���Ʋ�Ʒ����״̬������");
            goto ErrExit;
        }
        vtcp_log("��־iFlag[%d]",iFlag);
        if(iFlag == 1)
        {
            /*ȥ��һ���� ������ȥ�*/
            vtcp_log("������ȥ�[%d]",iFlag);
            goto OkExit_Next;
        }
    }
    else
    {
        vtcp_log("δ֪�Ĳ�������[%s]",cTx_code);
        strcpy(g_pub_tx.reply,"S083");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
OkExit_Next:
    strcpy(g_pub_tx.reply,"0001");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/*��Ҫ�������̵� mode����1 �����������ڴ������ǰ��ֹ
  ��������0
  */
static int func_0001(int *mode)
{
    int iRet = 0;
    double dFct_rate1 = 0.000000;
    double dFct_rate = 0.000000;
    double dCapital_percent = 0.000000;
    long lRt_date = 0;
    char cPrdt_code[21];
    char cOpr_flag[2];
    char cCapital_percent[11];
    char sTmp_rate[10];
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    struct com_tel_c sCom_tel;
    double syamt=0.00;/*ʵ��������*/
    long lDays=0;

    double fct_amt=0.00;
    double fct_profit_amt=0.00;
    int ret=0;
    double  fct_intst=0.00;
    double dProfit_rate=0.00;
    double dFct_intst = 0.00;
    double dTmp_amt = 0.00;
    double dProfit_amt=0.00;
    double fct_intst_tot=0.00;
    struct fn_ac_rel_c sFn_ac_rel;
    struct mdm_ac_rel_c  sMdm_ac_rel;
    struct in_mst_c  sIn_mst;
    char cCltype[2];     /*��������  0  �������������Ϣ¼��  1  ��������������*/    
    
    memset(&sCom_tel,0x00,sizeof(sCom_tel));
    memset(cCltype,0x00,sizeof(cCltype));
    memset(sTmp_rate,0x00,sizeof(sTmp_rate));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cOpr_flag,0x00,sizeof(cOpr_flag));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_ac_rel,0x00,sizeof(sFn_ac_rel));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(cCapital_percent,0x00,sizeof(cCapital_percent));

		get_zd_data("0340",cCltype);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0670",cOpr_flag);
    get_zd_long("0440",&lRt_date);
    get_zd_data("0640",cCapital_percent);
    get_zd_data("0730",sTmp_rate);
  
    
    dFct_rate1=atof(sTmp_rate)/1000000;
    dCapital_percent = atof(cCapital_percent) / 10000;
    vtcp_log("prdt_code[%s],rt_date[%ld],fct_rate[%lf] capital_percent[%lf][%s]333333333333sTmp_rate[%s]",\
            cPrdt_code,lRt_date,dFct_rate1,dCapital_percent,cCapital_percent,sTmp_rate);
    iRet = Fn_parm_Dec_Upd(g_pub_tx.reply,"prdt_code='%s'",cPrdt_code);
    if(iRet)
    {
        vtcp_log("���������Ʋ�Ʒ�����α�ʧ��! prdt_code[%s] iRet[%d]",cPrdt_code,iRet);
        strcpy(g_pub_tx.reply,"D332 ");
        goto ErrExit;
    }
    iRet = Fn_parm_Fet_Upd(&sFn_parm,g_pub_tx.reply);
    if(iRet == 100)
    {
        vtcp_log("û�������Ʋ�Ʒ! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    if(iRet)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ������ʧ��! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
        strcpy(g_pub_tx.reply,"D324");
        goto ErrExit;
    }
    
    /*add by lwb 20140923  ����һ���������������˵Ľ��� ����ԭ���ĸ�Ϊ��Ϣ¼��*/
   
    if(cCltype[0] == '0')  /*��������Ϊ�������������Ϣ¼�룬ֻ¼����Ϣ����������*/
    {
    	if(cOpr_flag[0] == '3')	/*����ʽ�����ǰ�������� ���ù黹���� ʵ�������� */
    	{
    	sFn_parm.return_date =lRt_date;
        sFn_parm.fct_rate = dFct_rate1;
        strcpy(sFn_parm.tel1,g_pub_tx.tel);
        strcpy(g_pub_tx.brf,"������Ʒ�������");    		
    	}
    	 /*������ƹ鼯�˻�����ж�lpj*/
  /*���Ϲ���ͬ����,����Ҫ���Ϲ�����������*/
    ret=Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts='1' and ic_date<%ld order by tx_br_no,ac_id",sFn_parm.prdt_code,lRt_date);
    if( ret )
    {
        vtcp_log("declare cursor error! ret[%d] ",ret);
        strcpy( g_pub_tx.reply,"D332" );
        goto ErrExit;
    }/*��������*/
    fct_intst=0.00;
     fct_profit_amt=0.00;
    while(1)
    {
        lDays = 0;
        dFct_rate = 0.000000;
        dProfit_rate = 0.000000; /*��������*/

        dFct_intst = 0.00;
        dTmp_amt = 0.00;
  
        memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
        memset(&g_in_mst,0x00,sizeof(g_in_mst));
        memset(&sFn_reg,0x00,sizeof(struct fn_reg_c));
        ret=Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
        if( ret==100 )
        {
        	vtcp_log("�Ѿ�ȡ�����ˣ�����");
        	break;      		
        }
        else if( ret )
        {
            vtcp_log("fn_reg fetch error! ret[%d] ",ret);
            strcpy(g_pub_tx.reply,"D330");
            goto ErrExit;
        }
    {          
            dProfit_rate = sFn_parm.profit_rate;
            dFct_rate = dFct_rate1;
            lDays =  pub_base_CalTrueDays(sFn_reg.ic_date,lRt_date);
            
            /*add by lwb ��������Ҫ��һ��Ϊ365��*/ 
           if(lDays == 366)
	          {
		          lDays = 365;
	          }
            vtcp_log("����[%lf],����[%ld]  ic_date [%ld] end_date[%ld] ",dFct_rate,lDays,sFn_reg.ic_date,sFn_reg.end_date);
          		
 	 }
 	 /*�������� dFct_intst���Ǻ��ڼ��Ͽ���ʽ�ĵ������ʵ�δ��������*/
        dTmp_amt =(sFn_reg.tx_amt - sFn_reg.redempt_amt)* lDays * dFct_rate * (1 + sFn_reg.flt_radio)/L365/L100;
        dFct_intst += pub_base_PubDround_1(dTmp_amt);
        sFn_reg.fct_intst += dFct_intst;
        vtcp_log("���μ����������[%lf]������ʵ������[%lf] Ԥ��[%lf]",dTmp_amt,dFct_intst,sFn_reg.intst);
        
        fct_intst+=sFn_reg.fct_intst;
        
 }
 			fct_intst_tot=fct_intst;
 		 ret=Fn_ac_rel_Sel(g_pub_tx.reply,&sFn_ac_rel,"prdt_code='%s'",sFn_parm.prdt_code);
    if(ret)
    {
        vtcp_log("��ѯ��Ʋ�Ʒ�����˺����ñ����");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    ret  = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",sFn_ac_rel.qs_ac_no);
    if(ret != 0 && ret != 100)
    {
        vtcp_log( "�����˺�ʧ��![%d]", iRet);
        
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    }
    else if(ret == 100)
    {
        vtcp_log( "�޴��˺�![%s]", sFn_ac_rel.qs_ac_no);
        
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    }
    ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
    if(ret)
    {
        vtcp_log("��ѯ�ڲ���ʧ��[%d]",ret);
        strcpy(g_pub_tx.reply,"W022");
      
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
       
    }
    vtcp_log("��Ҫ�����������Ϊ%16.2f,����˻����Ϊ%16.2f��%d ,%s",fct_intst_tot,sIn_mst.bal,__LINE__,__FILE__);
    if(pub_base_CompDblVal(sIn_mst.bal,fct_intst_tot)<0)
    	{
    		vtcp_log("��������˻����㣬���ֵ");
        strcpy(g_pub_tx.reply,"A030");
      
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    	}else if(pub_base_CompDblVal(fct_intst_tot,0.00)<=0)
    	{
    		vtcp_log("��������������");
        strcpy(g_pub_tx.reply,"L071");
      
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
    	}
    /*������ƹ鼯�˻�����ж�lpj*/
   	 	
   		sFn_parm.stat[0] = '7';  /*fn_parm ��״̬��Ϊ7 ��������¼��״̬  add by lwb 20140926*/
    }
    else if(cCltype[0] == '1'){   /*��������Ϊ��������������  ��ͨ�����ɷ�������*/
    	
    	vtcp_log(" sFn_parm.tel1 =%s  and g_pub_tx.tel =%s",sFn_parm.tel1,g_pub_tx.tel);
    	if(!strcmp(g_pub_tx.tel,sFn_parm.tel1))
    	{
    			
    		sprintf(acErrMsg,"��˺�¼�벻��Ϊͬһ����Ա��");       
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;	
    	}
    	*mode = 1;
    	/*iRet = Com_tel_Sel(g_pub_tx.reply,&sCom_tel,"tel = '%s' and lvl = '7' and br_no = '10001'",g_pub_tx.tel);
    	if(iRet == 100)
    	{
    		sprintf(acErrMsg,"��Ա���𲻹���");
    		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;	
    	}*/
    	if(strcmp(g_pub_tx.tel,"0016"))
    		{
    		sprintf(acErrMsg,"��Ա���𲻹���");
    		strcpy(g_pub_tx.reply,"H034");
				set_zd_data(DC_GET_MSG,acErrMsg);
				WRITEMSG
				goto ErrExit;
		 }
    if(cOpr_flag[0] == '3')/*��ǰ�����������ù黹���� ͬʱ���¼�Ϣ��ֹ���� ʵ�������� */
    {
        
        if(pub_base_CompDblVal(sFn_parm.fct_rate,dFct_rate1))
        {
        	sprintf(acErrMsg,"�����ʵ��������¼��ʱ������");
        	
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;
        }
        if(sFn_parm.return_date != lRt_date)
        {
        	sprintf(acErrMsg,"����ļ�Ϣ����������¼��ʱ������");
        	
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;	
        }
        
        strcpy(sFn_parm.tel2,g_pub_tx.tel);
        vtcp_log("sFn_parm.tel2 = %s",sFn_parm.tel2);
        strcpy(g_pub_tx.brf,"������Ʒ�������");
     		*mode = 1;
     
    	  /*��ǰ��ֹ�İ�fn_reg�ļ�Ϣ�������ڸ�Ϊ��ǰ�������� ������Ϣ��*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts ='1'",sFn_parm.prdt_code);
        if(iRet)
        {
            vtcp_log("���������α����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        while(1)
        {
						 vtcp_log("��ʼѭ���� ", sFn_parm.return_date);        	
            memset(&sFn_reg,0x00,sizeof(sFn_reg));
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            vtcp_log("��ʼѭ����11 ", sFn_parm.return_date); 
            if(iRet != 0 && iRet != 100)
            {
                vtcp_log("ȡ�����α����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D330");
                goto ErrExit;
            }
            else if(iRet == 100)
            {
                break;
            }
            if(lRt_date!=0)
            
            sFn_reg.return_rate = dFct_rate*(1+sFn_reg.flt_radio); /* add by lwb */
						vtcp_log("%s,%d return_rate = %f",__FILE__,__LINE__,sFn_reg.return_rate);
            strcpy(sFn_reg.filler,"��ǰ��������");
            vtcp_log("lRt_date[%d] ", sFn_parm.return_date);
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("����fn_reg����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
        }
        Fn_reg_Clo_Upd();
    }
  }
  /*add end lwb*/
		
  			else   
        {
            vtcp_log("δ֪��״̬[%s]",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"S091");
            goto ErrExit;
        }
    
    
    iRet = Fn_parm_Upd_Upd(sFn_parm,g_pub_tx.reply);
    vtcp_log("����ѭ����33 ", sFn_parm.return_date); 
    if(iRet)
    {
        vtcp_log("������Ʋ�Ʒ������ʧ��");
        strcpy(g_pub_tx.reply,"D334 ");
        goto ErrExit;
    }
    strcpy(g_pub_tx.ac_no,cPrdt_code);
    if( pub_ins_trace_log() )
    {
        vtcp_log("�Ǽǽ�����ˮ����");
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }
    Fn_reg_Clo_Upd();
    Fn_parm_Clo_Upd();


OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    return 1;
}
