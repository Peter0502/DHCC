/***************************************************************
* �� �� ��: spD086.c
* ������������ǰ��ֹʱ�޸���Ʋ�Ʒ����������ѡ�
            ʵ�������� ���ر������ ��Ʒ��Ϣ��ֹ����
            ��Ʋ�Ʒ��������:
            ʵ�������� ���ر������
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
#include "com_tel_c.h"
int spD086()
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
    long iQXRQ = 0;/*20151103��Ϣ��*/
    double dFct_rate = 0.000000;
    double dCapital_percent = 0.000000;
    long lRt_date = 0;
    long lRe_date = 0;/*20151103�׶�ļ��������*/
    char cPrdt_code[21];
    char cOpr_flag[2];
    char cCapital_percent[11];
    char sTmp_rate[10];
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    struct com_tel_c sCom_tel;
    char cCltype[2];     /*��������  0  �������������Ϣ¼��  1  ��������������*/    
    
    memset(&sCom_tel,0x00,sizeof(sCom_tel));
    memset(cCltype,0x00,sizeof(cCltype));
    memset(sTmp_rate,0x00,sizeof(sTmp_rate));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cOpr_flag,0x00,sizeof(cOpr_flag));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));
    memset(cCapital_percent,0x00,sizeof(cCapital_percent));

		get_zd_data("0340",cCltype);
		get_zd_long("0460",&iQXRQ);
		vtcp_log("aaaaaaa�յ��Ľ׶�����Ϣ�����ǣ� %d ", iQXRQ);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0670",cOpr_flag);
    get_zd_long("0440",&lRt_date);
    get_zd_long("0450",&lRe_date);/*20151103�׶�ļ��������*/
    get_zd_data("0640",cCapital_percent);
    get_zd_data("0730",sTmp_rate);
    dFct_rate=atof(sTmp_rate)/1000000;
    dCapital_percent = atof(cCapital_percent) / 10000;
    vtcp_log("prdt_code[%s],rt_date[%ld],fct_rate[%lf] capital_percent[%lf][%s]333333333333sTmp_rate[%s]",\
            cPrdt_code,lRt_date,dFct_rate,dCapital_percent,cCapital_percent,sTmp_rate);
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
    if(cOpr_flag[0] == '1' || cOpr_flag[0] == '2')
    {	
    
    if(cCltype[0] == '0')  /*��������Ϊ�������������Ϣ¼�룬ֻ¼����Ϣ����������*/
    {
    	if(cOpr_flag[0] == '1')	/*�������� ���ù黹���� ʵ�������� �����������*/
    	{
    	sFn_parm.return_date =g_pub_tx.tx_date;
        sFn_parm.fct_rate = dFct_rate;
        sFn_parm.capital_percent = dCapital_percent;
        strcpy(sFn_parm.tel1,g_pub_tx.tel);
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ���ù黹����");    		
    	}
   	 	if(cOpr_flag[0] == '2')  /*��ǰ��ֹ ���ù黹���� ͬʱ���¼�Ϣ��ֹ���� ʵ�������� �����������*/
   		{
   			strcpy(g_pub_tx.brf,"��Ʋ�Ʒ��ǰ��ֹ");
   			strcpy(sFn_parm.tel1,g_pub_tx.tel);
   			vtcp_log(" sFn_parm.tel1 = %s",sFn_parm.tel1);
			sFn_parm.return_date = lRt_date;
			sFn_parm.fct_rate = dFct_rate;
			sFn_parm.capital_percent = dCapital_percent;	
   			
   		}
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
    		
	if(cOpr_flag[0] == '1')/*�������� ���ù黹���� ʵ�������� �����������*/
	{
    		
    		if(pub_base_CompDblVal(sFn_parm.fct_rate,dFct_rate))
       		{
        		sprintf(acErrMsg,"�����ʵ��������¼��ʱ������");
        		strcpy(g_pub_tx.reply,"H034");
        		set_zd_data(DC_GET_MSG,acErrMsg);
        		WRITEMSG
        		goto ErrExit;
       		}
        
     	strcpy(sFn_parm.tel2,g_pub_tx.tel);
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ���ù黹����");
        *mode = 1;
        /*add by lwb */
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code='%s' and sts ='1'",sFn_parm.prdt_code);
        if(iRet)
        {
            vtcp_log("���������α����! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D332");
            goto ErrExit;
        }
        while(1)
        {
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
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
            sFn_reg.end_date = lRt_date;
            vtcp_log("%s,%d return_rate = %f",__FILE__,__LINE__,sFn_reg.return_rate);
            strcpy(sFn_reg.filler,"��������");
            sFn_reg.return_rate = dFct_rate*(1+sFn_reg.flt_radio); /* add by lwb */
            vtcp_log("lRt_date[%d] ", sFn_parm.return_date);
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("����fn_reg����! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            
        }
        /*add end lwb*/
    }
    else if(cOpr_flag[0] == '2')/*��ǰ��ֹ ���ù黹���� ͬʱ���¼�Ϣ��ֹ���� ʵ�������� �����������*/
    {
        
        if(pub_base_CompDblVal(sFn_parm.fct_rate,dFct_rate))
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
        if(pub_base_CompDblVal(sFn_parm.capital_percent,dCapital_percent))
        {
        	sprintf(acErrMsg,"����Ĺ黹���������¼��ʱ������");
        	
        	strcpy(g_pub_tx.reply,"H034");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	goto ErrExit;	
        }
        strcpy(sFn_parm.tel2,g_pub_tx.tel);
        vtcp_log("sFn_parm.tel2 = %s",sFn_parm.tel2);
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ��ǰ��ֹ");
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
            sFn_reg.end_date = lRt_date;
            sFn_reg.return_rate = dFct_rate*(1+sFn_reg.flt_radio); /* add by lwb */
						vtcp_log("%s,%d return_rate = %f",__FILE__,__LINE__,sFn_reg.return_rate);
            strcpy(sFn_reg.filler,"��ǰ��ֹ");
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
  /*add end l
  wb*/
		}
    
    else if(cOpr_flag[0] == '3')
    {
        strcpy(sFn_parm.stat,"4");
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ��ͣ����");
    }
    else if(cOpr_flag[0] == '4')
    {
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ��������");
        if(strcmp(sFn_parm.stat,"4") == 0)
        {
            strcpy(sFn_parm.stat,"0");/*�����Ϲ�*/
        }
        else if(strcmp(sFn_parm.stat,"6") ==0)
        {
            strcpy(sFn_parm.stat,"1");/*�����깺*/
        }
        else
        {
            vtcp_log("δ֪��״̬[%s]",sFn_parm.stat);
            strcpy(g_pub_tx.reply,"S091");
            goto ErrExit;
        }
    }
    else if(cOpr_flag[0] == '5')
    {
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒ��������");
        strcpy(sFn_parm.stat,"5");
    }
    else if(cOpr_flag[0] == '6')
    {
        strcpy(g_pub_tx.brf,"��Ʋ�Ʒֹͣ�깺");
        strcpy(sFn_parm.stat,"6");
    }
    /*LJ 20151102start*/
    else if(cOpr_flag[0] == '7')
    {
        strcpy(sFn_parm.stat,"0");
        strcpy(g_pub_tx.brf,"����ʽ��Ʒ����");
        sFn_parm.ic_date=iQXRQ;/*20151103*/
        sFn_parm.beg_date=lRt_date;/*20151103*/
        sFn_parm.end_date=lRe_date;/*20151103*/
    }
    /*LJ end*/
    /*LJ 20151104start*/
    else if(cOpr_flag[0] == '8')
    {
        strcpy(sFn_parm.stat,"1");      
     		iRet = sql_execute("update fn_br_profit set sts='0',tx_amt=0.00,redempt_amt=0.00,profit_amt=0.00 where prdt_code='%s' and sts='1'",cPrdt_code);/*LJ20151117*/
     		if( iRet ) 
				{
      		sprintf( acErrMsg, "ִ��sql_execute��![%d]", iRet );
					WRITEMSG
					goto ErrExit;
      	}
        strcpy(g_pub_tx.brf,"���Ž���");
    }
    /*LJ end*/
    else
    {
        vtcp_log("δ֪�Ĳ�������! cOpr_flag[%s] ",cOpr_flag);
        strcpy(g_pub_tx.reply,"S083");
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
