/*************************************************
* �� �� �� :  
* �������� :  �⺣��
* ��    �� :  2015/01/19
* ����ģ�� :  �ֻ����вƸ�����
* �������� :  spMK06.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5061', '�ֻ����вƸ�����', '10000000000000000000001111110111111111110000110000010011111011111111111111101010010001000000100000011000000000000000000000000000', '0','4', '0');
insert into tx_flow_def values ('5061', 0, 'MK06', '#$');
insert into tx_sub_def values ('MK06', '�ֻ����вƸ�����', 'spMK06', '0', '0');
*************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "dd_mst_c.h"			/*���ڴ�����ļ�*/
#include "td_mst_c.h"			/*���ڴ�����ļ�*/
#include "mdm_ac_rel_c.h"		/*�������˻���ϵ��*/
#include "com_sys_parm_c.h"		/*����ϵͳ����*/
#include "cif_id_code_rel_c.h"		/*�ͻ�֤����ͻ��Ŷ��ձ�*/
#include "dd_mst_hst_c.h"		/*���ڴ����ϸ*/
#include "td_mst_hst_c.h"		/*���ڴ����ϸ*/
#include "fn_reg_c.h"			/*��Ʋ�Ʒ�Ǽǲ�*/
#include "cif_basic_inf_c.h"		/*�ͻ�������Ϣ��*/

#define CNT 200;

struct dd_mst_hst_c g_dd_mst_hst;
struct td_mst_hst_c g_td_mst_hst;
struct fn_reg_c g_fn_reg;

char l_date[6][10];	/*��ǰʱ��ǰ5����*/

int deal_date(long d_date);	/*ʱ�䴦��*/
double deal_dd_mst(char *a_date,long ac_id);	/*�����ܶ��*/
double deal_td_mst(char *a_date,long ac_id);	/*�����ܶ��*/
double deal_fn_reg(char *a_date,long ac_id);	/*����ܶ��*/

int spMK06()
{
	struct com_sys_parm_c g_com_sys_parm;
	struct dd_mst_c g_dd_mst;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct td_mst_c g_td_mst;
	struct cif_basic_inf_c g_cif_basic_inf;
	
	memset(&g_com_sys_parm,0x00,sizeof(g_com_sys_parm));
	memset(&g_cif_id_code_rel,0x00,sizeof(g_cif_id_code_rel));
	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	
	char custName[60+1];	/*�ͻ�����*/
	char certType[1+1];	/*֤������*/
	char certNo[20+1];	/*֤������*/
	char type[1+1];		/*��ѯ����:1 �ʲ�*/
		
	memset(custName,0x00,sizeof(custName));
	memset(certType,0x00,sizeof(certType));
	memset(certNo,0x00,sizeof(certNo));
	memset(type,0x00,sizeof(type));
	
	double dd_mst_amt=0.00;		/*��ǰ�����ܶ�*/
	double qt_amt=0.00;		/*��ǰ����֪ͨ����ܶ�*/
	double td_mst_amt=0.00;		/*��ǰ�����ܶ�*/
	double fn_reg_amt=0.00;		/*��ǰ����ܶ�*/
	double Amt=0.00;		/*��ǰ�·��ܶ�*/
	
	double de_dd_mst_amt[6]={0.00};	/*�»����ܶ����һ��*/
	double de_td_mst_amt[6]={0.00};	/*�¶����ܶ����һ��*/
	double dfn_reg_amt[6]={0.00};	/*������ܶ�*/
	
	long hqac_id[CNT]={0};	/*�����˺�*/
	long dqac_id[CNT]={0};	/*�����˺�*/
	
	int ret;
	int k,i;
	char bufamt[20];
	long sysdate=0;
	long sys_month=0;
	
	double F_amt[5]={0.00};		/*ǰ��������ܶ�*/
	double amttmp=0.00;
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0250",custName);
	get_zd_data("0680",certType);
	get_zd_data("0630",certNo);
	get_zd_data("0710",type);
	
	/*ȡ��ǰϵͳʱ��*/
	ret = -1;
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&g_com_sys_parm,"1=1");
	if( ret ) 
	{
		vtcp_log("%s %d::��ѯ������ϵ�����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"O005");
		set_zd_data("0130","��ѯ������ϵ�����");
		goto ErrExit;
	}
	sysdate=g_com_sys_parm.sys_date;
	vtcp_log("%s %d::t_date = [%ld]",__FILE__,__LINE__,sysdate);
	sys_month=sysdate/100;
	
	/*ͨ��֤�����ͣ�֤������ȡ�ͻ���*/
	ret = -1;
	zip_space(certNo);
	ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&g_cif_id_code_rel,"id_type='%s' and id_no='%s'",certType,certNo);
	if( 100==ret || 1403==ret ) 
	{
		vtcp_log("%s %d::��֤����Ӧ�Ŀͻ��Ų�����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"C020");
		set_zd_data("0130","��֤����Ӧ�Ŀͻ��Ų�����");
		goto ErrExit;
	}
	else if( ret ) 
	{
		vtcp_log("%s %d::��ѯ�ͻ�֤����ͻ��Ŷ��ձ����!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","��ѯ�ͻ�֤����ͻ��Ŷ��ձ����");
		goto ErrExit;
	}
	
	/*�ͻ���Ϣ��֤*/
	ret = -1;
	zip_space(custName);
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&g_cif_basic_inf,"cif_no='%ld' and name='%s'",g_cif_id_code_rel.cif_no,custName);
	if(ret)
	{
		vtcp_log("%s %d::�ͻ�����֤���Ų�ƥ��!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D249");
		set_zd_data("0130","�ͻ�����֤���Ų�ƥ��");
		goto ErrExit;	
	}
	
	/*��ѯ�����˻�id*/
    	memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
    	k = 0;
	ret = -1;
	ret = Dd_mst_Dec_Sel(g_pub_tx.reply," cif_no='%ld'",g_cif_id_code_rel.cif_no);
	if( ret )
	{
		vtcp_log("[%s][%d]��ѯdd_mst����![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		set_zd_data("0130","��ѯ���ڴ�����ļ��쳣");
		return -1;
	}
	while(1)
	{
		ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
		if(ret==100 || ret == 1403)
		{
			break;
		}
		else if( 0 != ret  && 100 != ret)
		{
			vtcp_log("[%s],[%d] ��ѯdd_mst�쳣!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W015");
			set_zd_data("0130","��ѯ���ڴ�����ļ��쳣");
			Dd_mst_Clo_Sel();
			return -1;
		}
		else 
		{
			hqac_id[k]=g_dd_mst.ac_id;
			k++;
		}
	}
	Dd_mst_Clo_Sel();
    	
    	/*��ѯ�����˻�id*/
    	memset(&g_td_mst,0x00,sizeof(g_td_mst));
	k = 0;
	ret = -1;
	ret = Td_mst_Dec_Sel(g_pub_tx.reply," cif_no='%ld' ",g_cif_id_code_rel.cif_no);
	if( ret )
	{
		vtcp_log("[%s][%d]��ѯtd_mst����![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"W015");
		set_zd_data("0130","��ѯ���ڴ�����ļ��쳣");
		return -1;
	}
	while(1)
	{
		ret=Td_mst_Fet_Sel(&g_td_mst,g_pub_tx.reply);
		if(ret==100 || ret == 1403)
		{
			break;
		}
		else if( 0 != ret  && 100 != ret)
		{
			vtcp_log("[%s],[%d] ��ѯtd_mst�쳣!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"W017");
			set_zd_data("0130","��ѯ���ڴ�����ļ��쳣");
			Td_mst_Clo_Sel();
			return -1;
		}
		else 
		{
			dqac_id[k]=g_td_mst.ac_id;
			k++;
		}
	}
	Td_mst_Clo_Sel();
	
	/*ʱ�䴦��*/
	memset(l_date,0x00,sizeof(l_date));
	ret = -1;
	ret=deal_date(sys_month);
	if(ret != 0)
	{
		vtcp_log("%s %d::���ڴ���ʧ��! ret[%d] ",__FILE__,__LINE__,ret);
    		strcpy(g_pub_tx.reply,"9999");
    		set_zd_data("0130","���ڴ���ʧ��");
    		goto ErrExit;	
	}
	
	/*��ǰ�����ܶ�*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&dd_mst_amt,"cif_no='%ld' and prdt_no <> '107'",g_cif_id_code_rel.cif_no);
	vtcp_log("%s %d::��ǰ�»����ܶ�dd_mst_amt=[%f] ",__FILE__,__LINE__,dd_mst_amt);				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","��ǰ�»����ܶ����");
    		goto ErrExit;
    	}
	
	/*��ǰ����֪ͨ����ܶ�*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&qt_amt,"cif_no='%ld' and prdt_no='107'",g_cif_id_code_rel.cif_no);
	vtcp_log("%s %d::��ǰ����֪ͨ����ܶ�qt_amt=[%f] ",__FILE__,__LINE__,qt_amt); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","��ǰ����֪ͨ����ܶ����");
    		goto ErrExit;
    	}
	
	/*��ǰ�����ܶ�*/
	ret = -1;
	ret=sql_sum_double("td_mst","bal",&td_mst_amt,"cif_no='%ld'",g_cif_id_code_rel.cif_no);
	vtcp_log("%s %d::��ǰ�����ܶ�td_mst_amt=[%f] ",__FILE__,__LINE__,td_mst_amt); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","��ǰ�����ܶ����");
    		goto ErrExit;
    	}
    	
    	/*��ǰ����ܶ�*/
    	double fn_reg_amt_tmp=0.00;
    	k=0;
	while(1)
	{
		if(hqac_id[k] == 0)
		{
			break;	
		}
		fn_reg_amt_tmp=0.00;
		ret=-1;
		ret=sql_sum_double("fn_reg","tx_amt",&fn_reg_amt_tmp,"ac_id='%ld' and pay_date<='%ld' and sts = '1'",hqac_id[k],g_com_sys_parm.sys_date);
		vtcp_log("%s %d::��ǰ�˺�����ܶ�fn_reg_amt_tmp=[%f] ",__FILE__,__LINE__,fn_reg_amt_tmp); 				
    		if( ret )
    		{
    		 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    		 	strcpy(g_pub_tx.reply,"9999");
    		 	set_zd_data("0130","��ǰ�˺�����ܶ����");
    			goto ErrExit;
    		}
		dfn_reg_amt[0]+=fn_reg_amt_tmp;		/*������ܶ�*/
		k++;
	}
    	
    	double dd_mst_amt_tmp=0.00;
	double td_mst_amt_tmp=0.00;
	for(i=1;i<6;i++)
	{
		/*����ǰ����»����ܶ�*/
		k=0;
		while(1)
		{
			dd_mst_amt_tmp=0.00;
			if(hqac_id[k] == 0)
			{
				break;	
			}
			dd_mst_amt_tmp=deal_dd_mst(l_date[i],hqac_id[k]);
			de_dd_mst_amt[i]+=dd_mst_amt_tmp;	/*�»����ܶ�*/
			k++;
				
		}
		
		/*����ǰ����¶����ܶ�*/
		k=0;
		while(1)
		{
			td_mst_amt_tmp=0.00;
			if(dqac_id[k] == 0)
			{
				break;	
			}
			td_mst_amt_tmp=deal_td_mst(l_date[i],dqac_id[k]);
			de_td_mst_amt[i]+=td_mst_amt_tmp;	/*�¶����ܶ�*/	
			k++;	
		}
		
		/*����ǰ���������ܶ�*/
		k=0;
		while(1)
		{
			fn_reg_amt_tmp=0.00;
			if(hqac_id[k] == 0)
			{
				break;	
			}
			fn_reg_amt_tmp=deal_fn_reg(l_date[i],hqac_id[k]);
			dfn_reg_amt[i]+=fn_reg_amt_tmp;		/*������ܶ�*/
			k++;
		}			
	}

	set_zd_double("0560",dd_mst_amt);	/*��ǰ�����ܶ�*/
	set_zd_double("0570",qt_amt);		/*��ǰ����֪ͨ����ܶ�*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",td_mst_amt*100);
	set_zd_data("0300",bufamt);		/*��ǰ�����ܶ�*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",dfn_reg_amt[0]*100);
	set_zd_data("0310",bufamt);		/*��ǰ����ܶ�*/
	Amt=dd_mst_amt+qt_amt+td_mst_amt+dfn_reg_amt[0];
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",Amt*100);
	set_zd_data("0320",bufamt);		/*��ǰ�·��ܶ�*/
	
	/*ǰ�����ܶ���ܶ�=����+����+���*/
	int j=0;
	for(j=0;j<5;j++)
	{
		F_amt[j]=de_dd_mst_amt[j+1]+de_td_mst_amt[j+1]+dfn_reg_amt[j+1];
		vtcp_log("%s %d::F_amt[%d]=%.2f",__FILE__,__LINE__,j,F_amt[j]);	
	}
	
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[0]*100);
	set_zd_data("0330",bufamt);	/*ǰһ�·��ܶ�*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[1]*100);
	set_zd_data("0370",bufamt);	/*ǰ���·��ܶ�*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[2]*100);
	set_zd_data("0380",bufamt);	/*ǰ���·��ܶ�*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[3]*100);
	set_zd_data("0620",bufamt);	/*ǰ���·��ܶ�*/
	memset(bufamt,0x00,sizeof(bufamt));
	sprintf(bufamt,"%.2f",F_amt[4]*100);
	set_zd_data("0750",bufamt);	/*ǰ���·��ܶ�*/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120","0000");
	set_zd_data("0130","�ֻ����вƸ������ɹ���");
	vtcp_log("success !!: reply is[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("fail !!: reply is[%s]", g_pub_tx.reply);
	return 1;
}

int deal_date(long d_date)
{
	long date_tmp=0;
	long date_year=0;
	
	date_tmp=d_date%100;
	date_year=d_date/100;
	sprintf(l_date[0],"%06ld",d_date);

	int i=0;
	for(i=0;i<5;i++)
	{
		if(date_tmp-1 > 0)
		{
			sprintf(l_date[i+1],"%04ld%02ld",date_year,date_tmp-1);
			date_tmp=date_tmp-1;		
		}
		if(date_tmp-1 == 0)
		{
			sprintf(l_date[i+1],"%04ld12",date_year-1);
			date_year=date_year-1;
			date_tmp=12;
		}	
	}
	return 0;
}


double deal_dd_mst(char *a_date,long ac_id)
{
	int ret = -1;
	double he_dd_mst_amt=0.00;
	
	/*�»�����ϸ���һ��*/
	memset(&g_dd_mst_hst,0x00,sizeof(g_dd_mst_hst));
	ret=Dd_mst_hst_Sel(g_pub_tx.reply, &g_dd_mst_hst,"ac_id ='%ld' and substr(tx_date,0,6)<='%s'\
	 order by hst_cnt desc", ac_id,a_date);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::��ѯ�����˻���ϸʧ��!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"W050");
		set_zd_data("0130","��ѯ�����˻���ϸʧ��");
		Dd_mst_Clo_Sel();
		return -1;
	}  
	he_dd_mst_amt=g_dd_mst_hst.bal;	
	return he_dd_mst_amt;	
}

double deal_td_mst(char *a_date,long ac_id)
{
	int ret=-1;
	double he_td_mst_amt=0.00;
	
	memset(&g_td_mst_hst,0x00,sizeof(g_td_mst_hst));
	ret=Td_mst_hst_Sel(g_pub_tx.reply, &g_td_mst_hst,"ac_id ='%ld' and substr(tx_date,0,6)<='%s'\
	 order by hst_cnt desc", ac_id,a_date);
	if (ret != 100 && ret != 0)
	{
		vtcp_log("%s %d::��ѯ�����˻���ϸʧ��!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"W051");
		set_zd_data("0130","��ѯ�����˻���ϸʧ��");
		Td_mst_Clo_Sel();
		return -1;
	}   
	he_td_mst_amt=g_td_mst_hst.bal;
	return he_td_mst_amt;
}

double deal_fn_reg(char *a_date,long ac_id)
{
	int ret=-1;
	double amt_tmp=0.00;

	/*������ܶ�*/
	amt_tmp=0.00;
	ret=sql_sum_double("fn_reg","tx_amt",&amt_tmp,"ac_id=%ld and ((substr(pay_date,0,6)>='%s' and substr(return_date,0,6)<'%s'\
	 and sts='2') or (substr(pay_date,0,6)<='%s' and sts = '1'))",ac_id,a_date,a_date,a_date);
	vtcp_log("%s %d::��ǰ�˺�����ܶ�amt_tmp=[%f] ",__FILE__,__LINE__,amt_tmp); 				
    	if( ret )
    	{
    	 	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	 	strcpy(g_pub_tx.reply,"9999");
    	 	set_zd_data("0130","��ǰ�˺�����ܶ����");
    	 	Dd_mst_Clo_Sel();
    		return -1;
    	}
	return amt_tmp;		
}
