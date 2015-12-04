/*********************************************************************
* �� �� �� :  
* �������� :  lixiang
* ��    �� :  2014/10/21
* ����ģ�� :  ��Ʋ�Ʒ��Ϣ��ѯ
* �������� :  spW201.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5051', '��Ʋ�Ʒ��Ϣ��ѯ', '10000000000000000000110010010011100000111111111110000000000001000111111100000000000110000000100000011000000000000000000000000000', '1','6', '2');
insert into tx_flow_def values ('5051', 0, 'W201', '#$');
insert into tx_sub_def values ('W201', '��Ʋ�Ʒ��Ϣ��ѯ', 'spW201', '0', '0');
***********************************************************************/
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"			/*��Ʋ�Ʒ������*/
#include "fn_reg_c.h"			/*��Ʋ�Ʒ�Ǽǲ�*/
#include "fn_br_quota_parm_c.h"		/*��Ʋ�Ʒ������������*/

int spW201()
{
	struct fn_parm_c sFn_parm;  	 	/*��Ʋ�Ʒ������*/
	struct fn_br_quota_parm_c sFn_br_qp;	/*��Ʋ�Ʒ������������*/
	
	char cPrdt_code[20];
	int iRt;
	int ret=0;
	double restAmt=0.00;
	double dTx_amt=0.00;
	double dRedempt_amt = 0.00;
	double dBuying_amt = 0.00;
	double trust_rate=0.00;

	memset(cPrdt_code,0x00,sizeof(cPrdt_code));
	memset(&sFn_br_qp,0x00,sizeof(sFn_br_qp));
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	/*��ƽ̨��ȡ��ѯ����*/
	get_zd_data("0620",cPrdt_code);
	if(!strlen(cPrdt_code))
	{
		strcpy(g_pub_tx.reply,"9999");
		set_zd_data("0130","��ѯ��������Ϊ��");
		sprintf(acErrMsg,"��ѯ��������Ϊ��!");
		WRITEMSG
		vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
		goto ErrExit;
	}
	
	
	/*���ݲ�Ʒ�����ѯ��Ʋ�Ʒ������fn_parm*/
	iRt=-1;
	iRt=Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
	if( iRt==100 )
		{
			vtcp_log("fatal error! table no record ");
			strcpy(g_pub_tx.reply,"T062");
			goto ErrExit;
		}
	else if( iRt )
		{
			vtcp_log("database error! ret[%d] ",iRt);
			strcpy(g_pub_tx.reply,"D324");
			goto ErrExit;
		}
		
	set_zd_data("0250",sFn_parm.title);		/*��Ʒ����*/
	set_zd_data("0670",sFn_parm.prdt_type);		/*��Ʒ����*/
	set_zd_data("0680",sFn_parm.intst_type);	/*��������*/
	set_zd_long("0440",sFn_parm.beg_date);		/*ļ����ʼ����*/
	set_zd_long("0450",sFn_parm.end_date);		/*ļ����������*/
	set_zd_long("0460",sFn_parm.ic_date);		/*��Ʒ��������*/
	set_zd_long("0470",sFn_parm.mtr_date);		/*��Ʒ��������*/
	set_zd_long("0280",sFn_parm.term);		/*����*/
	set_zd_data("0700",sFn_parm.cif_type);		/*�ͻ����ͣ�0���ɣ�1���ˣ�2�Թ�*/
	set_zd_data("0690",sFn_parm.risk_lvl);		/*���յȼ�*/
	set_zd_data("0720",sFn_parm.invest_type);	/*Ͷ�ʷ���*/
	set_zd_data("0210",sFn_parm.cur_no);		/*����*/
	set_zd_double("0840",sFn_parm.rate);		/*Ԥ������껯������*/
	vtcp_log("Ԥ������껯������=[%9.6f]",sFn_parm.rate);
	set_zd_double("0850",sFn_parm.chrg_rate);	/*����������*/
	set_zd_double("0310",sFn_parm.trust_rate);	/*�йܷ���*/
	get_zd_double("0310",&trust_rate);
	vtcp_log("�йܷ���--=[%9.6f]",trust_rate);
	
	set_zd_double("0390",sFn_parm.sale_amt);	/*���۽��*/
	/*�Ѿ��Ϲ����ܶ�*/
	iRt=-1;
	iRt=sql_sum_double("fn_reg","tx_amt",&dTx_amt,"prdt_code='%s' and sts <> '*'",sFn_parm.prdt_code);
   	if( iRt)
    	{
        	vtcp_log("sql_sum_double error! ret[%d] ",iRt);
       		strcpy(g_pub_tx.reply,"D331");
       		goto ErrExit;
    	}
   	set_zd_double("0430",dTx_amt);			/*�ò�Ʒ�Ϲ��ܶ�*/
   	restAmt=sFn_parm.sale_amt-dTx_amt;
   	set_zd_double("0400",restAmt);			/*����ʣ���=�����ܶ�-�Ϲ��ܶ�*/
   	
   	/*���ò�Ʒ�Ƿ����*/
   	if(sFn_parm.quota_flag[0] == 'Y')
		{
        /*��鱾������Ʋ�Ʒ�Ƿ����Ϲ�*/
        	ret = Fn_br_quota_parm_Sel(g_pub_tx.reply,&sFn_br_qp,"prdt_code='%s' and br_no='%s'",\
           	sFn_parm.prdt_code,g_pub_tx.tx_br_no);
       		if(ret != 0 && ret != 100)
        	{
           		vtcp_log("��ѯ��Ʋ�Ʒ��������ʧ��iRet[%d]",ret);
            		strcpy(g_pub_tx.reply,"D327");
           		goto ErrExit;
        	}
        	else if(ret == 100)/*�������û��ȡȫ��*/
        	{
           		if(sFn_parm.quota_flag[1] == 'Y')
            		{
               			 if(sFn_parm.stat[0] == '1')/*ͳ�������Ѿ���ص��ܶ� ֻ��״̬��1��ʱ�������غ��깺*/
               			 {
                    			/*��صĽ�����*/
                    			ret = sql_sum_double("fn_reg","redempt_amt",&dRedempt_amt,"prdt_code='%s' and sts='%s'",\
                       			sFn_parm.prdt_code,sFn_parm.stat);
                    			if(ret)
                    			{
                        			vtcp_log("ͳ��fn_reg����!");
                        			strcpy(g_pub_tx.reply, "D331");
                        			goto ErrExit;
                    			}
            				/*�깺�еĽ�����*/        		
                    			ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,"prdt_code='%s' and sts='4'",\
                  			sFn_parm.prdt_code);
                    			if(ret)
                    			{
                        			vtcp_log("ͳ��fn_reg����!");
                        			strcpy(g_pub_tx.reply, "D331");
                        			goto ErrExit;
                    			}
                		}
				vtcp_log("dRedempt_amt[%lf]",dRedempt_amt);	/*��صĽ�����*/
				vtcp_log("dBuying_amt[%lf]",dBuying_amt);	/*�깺�еĽ�����*/ 
				/* ����ʣ���=���۽��-�Ѿ��Ϲ����ܶ�-�깺�еĽ�����+��صĽ�����*/
				set_zd_double("0320",sFn_parm.sale_amt - (dTx_amt - dRedempt_amt) - dBuying_amt);
          		}
           	 	else
           	 	{
                		set_zd_double("0320",0.00);
           		 }
       		 }
       		 else
        	{
           	 set_zd_double("0320",sFn_br_qp.available_amt);
        	}
		}
		else/*��Ʒ��������ȡȫ�е�*/
		{
			/*ͳ�������Ѿ���ص��ܶ� ֻ��״̬��1��ʱ�������غ��깺*/
			if(sFn_parm.stat[0] == '1')
				{
					ret = sql_sum_double("fn_reg","redempt_amt",&dRedempt_amt,"prdt_code='%s' and sts='%s'",\
					sFn_parm.prdt_code,sFn_parm.stat);
					if(ret)
					{
							vtcp_log("ͳ��fn_reg����!");
							strcpy(g_pub_tx.reply, "D331");
							goto ErrExit;
					}
					ret = sql_sum_double("fn_reg","tx_amt",&dBuying_amt,"prdt_code='%s' and sts='4'",sFn_parm.prdt_code);
 					if(ret)
					{
							vtcp_log("ͳ��fn_reg����!");
							strcpy(g_pub_tx.reply, "D331");
							goto ErrExit;
					}
				}
				vtcp_log("dRedempt_amt[%lf]",dRedempt_amt);	/*��صĽ�����*/
				vtcp_log("dBuying_amt[%lf]",dBuying_amt);	/*�깺�еĽ�����*/
				/* ����ʣ���=���۽��-�Ѿ��Ϲ����ܶ�-�깺�еĽ�����+��صĽ�����*/
				set_zd_double("0430",sFn_parm.sale_amt - (dTx_amt - dRedempt_amt) - dBuying_amt);
		}
	set_zd_double("0300",sFn_parm.add_amt);		/*�������*/
	set_zd_double("0410",sFn_parm.per_min_amt);	/*���������*/
	set_zd_double("0420",sFn_parm.com_min_amt);	/*��λ�����*/
	set_zd_data("0930",sFn_parm.cancel_flag);	/*�Ƿ���*/
	set_zd_data("0710",sFn_parm.float_rate_flag);		/*�Ƿ��и�������*/
	set_zd_double("0970",sFn_parm.manage_rate);	/*�������*/
	/*add end*/
	
   
OkExit:
	strcpy(g_pub_tx.reply,"0000");			/*ͨ�ü��˽ṹ.��Ӧ��*/
	vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
	set_zd_data("0120","0000");	
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	set_zd_data("0120",g_pub_tx.reply);	
	return 1;
}