/*********************************************************************
* �� �� �� :  
* �������� :  �ؽ���
* ��    �� :  2014/01/19
* ����ģ�� :  �ֻ����вƸ�����
* �������� :  spMK05.c
* �������� :
* �����б� :
* ʹ��ע�� :
�������ã�
insert into tx_def values ('5025', '�ֻ����вƸ�����', '11000000000000000000110011010011110000111111111110000000001111110111111100000000000110010000100000011000000000000000000000000000', '0', '4', '0',);
insert into tx_flow_def values ('5025', 0, 'MK05', '#$');
insert into tx_sub_def values ('MK05', '�ֻ����вƸ�����', 'spMK05', '0', '0');
***********************************************************************/
#include "public.h"
#include "dd_mst_c.h"
#include "dd_mst_hst_c.h"
#include "td_mst_hst_c.h"
#include "td_mst_c.h"
#include "cif_id_code_rel_c.h"
#include "cif_basic_inf_c.h"
#include "com_sys_parm_c.h"
#include "fn_reg_c.h"
#include "string.h"

#define MONTH 6
long Calculate_Date(long,int);
#define NUM  100										/*ac_id����*/
int spMK05()
{
	int ret=-1;
	char sCustName[60+1];								/*�ͻ�����*/
	char sCertType[1+1];								/*֤������*/
	char sCertNo[20+1];									/*֤����*/
	char sType[4];										/*��ѯ����*/
	long hq_ac_id[NUM]={0};
	long dq_ac_id[NUM]={0};
	char cWherelist[256];
	int i = 0;
	long laterdate=0;
	long sys_date=0;
	double dq_sum[MONTH]={0.00};						/*�����ܶ�*/
	double hq_sum[MONTH]={0.00};						/*�����ܶ�*/
	double lc_sum[MONTH]={0.00};						/*����ܶ�*/
	double qt_sum = 0.00;								/*��ǰ7���ܶ�*/
	
	struct dd_mst_c 			dd_mst;					/*���ڴ�����ļ�*/
	struct td_mst_c				td_mst;					/*���ڴ�����ļ�*/
	struct dd_mst_hst_c			dd_mst_hst;				/*������ϸ��*/
	struct td_mst_hst_c			td_mst_hst;				/*������ϸ��*/
	struct cif_id_code_rel_c 	cif_id_code_rel;		/*�ͻ�֤����ͻ��Ŷ��ձ�*/
	struct cif_basic_inf_c 		cif_basic_inf;  		/*�ͻ�������Ϣ��*/
	struct fn_reg_c				fn_reg;					/*��Ʋ�Ʒ�Ǽǲ�*/
	struct com_sys_parm_c     	com_sys_parm;   		/*����ϵͳ����*/
	
	memset(sCustName,0x00,sizeof(sCustName));
	memset(sCertType,0x00,sizeof(sCertType));
	memset(sCertNo,0x00,sizeof(sCertNo));
	memset(sType,0x00,sizeof(sType));
  	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(&dd_mst,0x00,sizeof(dd_mst));
	memset(&td_mst,0x00,sizeof(td_mst));	
	memset(&fn_reg, 0x00, sizeof(fn_reg));
	memset(&dd_mst_hst, 0x00, sizeof(dd_mst_hst));
	memset(&td_mst_hst, 0x00, sizeof(td_mst_hst));
	memset(&com_sys_parm, 0x00, sizeof(com_sys_parm));
	memset(&cif_basic_inf, 0x00, sizeof(cif_basic_inf));
	memset(&cif_id_code_rel, 0x00, sizeof(cif_id_code_rel));
	
	/**��ʼ�����ýṹ**/
	if (pub_base_sysinit())
	{
	    vtcp_log("��ʼ�����ýṹ����!!");
	    strcpy(g_pub_tx.reply, "CU06");
	    goto ErrExit;
	}
	
	get_zd_data("0250",sCustName);
	get_zd_data("0680",sCertType);
	get_zd_data("0630",sCertNo);
	
	for(int j = 0;j<MONTH;++j)
	{
		dq_sum[j] = hq_sum[j] = lc_sum[j] = 0.00;
	}
  /*�ͻ�֤����ͻ��Ŷ��ձ�*/
  	ret = Cif_id_code_rel_Sel(g_pub_tx.reply,&cif_id_code_rel,"id_type='%s' and id_no='%s'",sCertType,sCertNo);
  	if(ret == 100 || ret == 1403)
  	{
		vtcp_log("%s %d::cif_id_code_rel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"B115");
		set_zd_data("0130","�ͻ�֤����ͻ��Ŷ��ձ��в����ڴ˿ͻ���Ϣ");
		goto ErrExit;	
	}
	else if(ret)
	{
		vtcp_log("%s %d::cif_id_code_rel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","��ѯ�ͻ�֤����ͻ��Ŷ��ձ����");
		goto ErrExit;	
	}
  /*�ͻ�������Ϣ��*/ 
  	ret = Cif_basic_inf_Sel(g_pub_tx.reply,&cif_basic_inf,"cif_no =%ld and name='%s' and sts='1' ",cif_id_code_rel.cif_no,zip_space(sCustName));
	if(ret == 100 || ret == 1403)
	{
		vtcp_log("%s %d::cif_basic_inf_sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"B116");
		set_zd_data("0130","�ͻ�������Ϣ���в����ڴ˿ͻ���Ϣ");
		goto ErrExit;	
	}
	else if(ret)
	{
		vtcp_log("%s %d::Cif_basic_inf_Sel [%d] error ",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"AT06");
		set_zd_data("0130","��ѯ�ͻ�������Ϣ�����");
		goto ErrExit;
	}
	/*ȡϵͳ����*/
	ret=Com_sys_parm_Sel( g_pub_tx.reply,&com_sys_parm, "1=1" );
	if(ret)
	{
		vtcp_log("%s %d::Com_sys_parm_Sel [%d] error ",__FILE__,__LINE__,ret);
	 	strcpy(g_pub_tx.reply,"O072");
	 	set_zd_data("0130","��ѯ����ϵͳ����״̬����");
	 	goto ErrExit;	
	}
	sys_date=com_sys_parm.sys_date / 100;/*ȡ����*/
	/*ȡ����ac_id*/
	int k = 0;
	sprintf(cWherelist,"cif_no =%ld ",cif_id_code_rel.cif_no);
	ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
	if(ret)
	{
		vtcp_log("������ڴ�����ļ��α����");
		strcpy(g_pub_tx.reply,"D101");
		set_zd_data("0130","������ڴ�����ļ��α����");
		goto ErrExit;
	}
	while(1)
	{
		memset(&dd_mst,0x00,sizeof(dd_mst));
		ret = Dd_mst_Fet_Sel(&dd_mst,g_pub_tx.reply);
		if(ret != 100 && ret != 0)
		{
			vtcp_log("ȡ���ڴ�����ļ��α����");
			strcpy(g_pub_tx.reply,"D330");
			set_zd_data("0130","ȡ���ڴ�����ļ��α����");
			Dd_mst_Clo_Sel();
			goto ErrExit;
		}
		else if(ret == 100 || ret == 1403)
		{
			break;
		}
		else
		{
			hq_ac_id[k] = dd_mst.ac_id;
			++k;
		}
	}
	Dd_mst_Clo_Sel();
	/*ȡ����ac_id*/
	sprintf(cWherelist,"cif_no =%ld",cif_id_code_rel.cif_no);
	ret = Td_mst_Dec_Sel(g_pub_tx.reply,"%s",cWherelist);
	if(ret)
	{
		vtcp_log("���嶨�ڴ�����ļ��α����");
		strcpy(g_pub_tx.reply,"D101");
		set_zd_data("0130","���嶨�ڴ�����ļ��α����");
		goto ErrExit;
	}
	int m = 0;
	while(1)
	{
		/*���ڴ���*/
		ret = Td_mst_Fet_Sel(&td_mst,g_pub_tx.reply);
		if(ret != 100 && ret != 0)
		{
			vtcp_log("ȡ���ڴ�����ļ��α����");
			strcpy(g_pub_tx.reply,"D330");
			set_zd_data("0130","ȡ���ڴ�����ļ��α����");
			Td_mst_Clo_Sel();
			goto ErrExit;
		}
		else if(ret == 100 || ret == 1403)
		{
			vtcp_log("ȡ���ڴ��Ϊ��");
			break;
		}
		else
		{
			dq_ac_id[m++] = td_mst.ac_id;
		}
	}
	Td_mst_Clo_Sel();
	/*����ǰ5�����ܶ�*/
	double lc_sum_tmp=0.00;
	for(i=MONTH-1;i>0;--i)
	{
		k = 0;
		laterdate = Calculate_Date(sys_date,i);/*��������*/
		while(1)
		{
			if(hq_ac_id[k] == 0)
			{
				break;
			}
			memset(&dd_mst_hst,0x00,sizeof(dd_mst_hst));	
			ret = Dd_mst_hst_Sel(g_pub_tx.reply,&dd_mst_hst,"ac_id=%ld and substr(tx_date,0,6)<='%ld' order by hst_cnt desc",hq_ac_id[k],laterdate);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("��ѯ���ڴ����ϸʧ��");
				strcpy(g_pub_tx.reply,"W050");
				set_zd_data("0130","��ѯ������ϸ���쳣");
				goto ErrExit;
			}
			/*�����ܶ�hq_sum*/
			hq_sum[i] += dd_mst_hst.bal;	
			/*����ܶ�lc_sum*/
			lc_sum_tmp=0.00;
			ret = sql_sum_double("fn_reg" , "tx_amt" , &lc_sum_tmp,"ac_id=%ld and ((substr(pay_date,0,6) <= '%ld' and '%ld' < substr(return_date,0,6) and sts='2') or (substr(pay_date,0,6)<='%ld' and sts = '1'))",hq_ac_id[k],laterdate,laterdate,laterdate);
			if(ret)
			{
				vtcp_log("��ѯ��ǰ����ܶ�ʧ��");
				strcpy(g_pub_tx.reply,"9999");
				set_zd_data("0130","��ѯ��ǰ����ܶ�ʧ��");
				goto ErrExit;
			}
			lc_sum[i] += lc_sum_tmp;
			vtcp_log("����ܶ�lc_sum[%d] = [%16.2f]",i,lc_sum[i]);
			++k;
		}
		m = 0;
		while(1)
		{
			if(dq_ac_id[m] == 0)
			{
				break;
			}
			memset(&td_mst_hst,0x00,sizeof(td_mst_hst));
			ret = Td_mst_hst_Sel(g_pub_tx.reply,&td_mst_hst,"ac_id=%ld and substr(tx_date,0,6)<='%ld' order by tx_date desc, hst_cnt desc",dq_ac_id[m],laterdate);
			if(ret != 100 && ret != 0)
			{
				vtcp_log("��ѯ���ڴ����ϸʧ��");
				strcpy(g_pub_tx.reply,"W029");
				set_zd_data("0130","��ѯ���ڴ����ϸʧ��");
				goto ErrExit;
			}
			/*�����ܶ�dq_sum*/
			dq_sum[i] += td_mst_hst.bal;
			++m;
			vtcp_log("ǰ[%d]�����ܶ�dq_sum[%d] = [%16.2f]",i,i,dq_sum[i]);
		}	
	}
/*��ǰ�����ܶ�*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&hq_sum[0],"cif_no='%ld' and prdt_no <> '107'",cif_id_code_rel.cif_no);
	vtcp_log("%s %d::��ǰ�»����ܶ�hq_sum=[%f] ",__FILE__,__LINE__,hq_sum[0]);				
    if( ret )
    {
    	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	strcpy(g_pub_tx.reply,"9999");
    	set_zd_data("0130","��ǰ�»����ܶ����");
    	goto ErrExit;
    }
/*��ǰ����֪ͨ����ܶ�*/
	ret = -1;
	ret=sql_sum_double("dd_mst","bal",&qt_sum,"cif_no='%ld' and prdt_no='107'",cif_id_code_rel.cif_no);
	vtcp_log("%s %d::��ǰ����֪ͨ����ܶ�qt_sum=[%f] ",__FILE__,__LINE__,qt_sum); 	
    if( ret )
    {
    	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	strcpy(g_pub_tx.reply,"9999");
    	set_zd_data("0130","��ǰ����֪ͨ����ܶ����");
    	goto ErrExit;
    }
/*��ǰ����ܶ�*/
	k = 0;
	while(1)
	{
		if(hq_ac_id[k] == 0)
		{
			break;
		}
		lc_sum_tmp=0.00;
		ret = sql_sum_double("fn_reg" , "tx_amt" , &lc_sum_tmp,"ac_id=%ld and pay_date<='%ld' and sts = '1'",hq_ac_id[k],com_sys_parm.sys_date);
		if(ret)
		{
			vtcp_log("��ѯ��ǰ����ܶ�ʧ��");
			strcpy(g_pub_tx.reply,"9999");
			set_zd_data("0130","��ѯ��ǰ����ܶ�ʧ��");
			goto ErrExit;
		}
		lc_sum[0] += lc_sum_tmp;
		++k;
		vtcp_log("��ǰ����ܶ�lc_sum = [%16.2f]",lc_sum[0]);
	}
    
/*��ǰ�����ܶ�*/
	ret = -1;
	ret=sql_sum_double("td_mst","bal",&dq_sum[0],"cif_no='%ld'",cif_id_code_rel.cif_no);
	vtcp_log("%s %d::��ǰ�����ܶ�dq_sum=[%f] ",__FILE__,__LINE__,dq_sum[0]); 				
    if( ret )
    {
    	vtcp_log("%s %d::sql_sum_double error! ret[%d] ",__FILE__,__LINE__,ret);
    	strcpy(g_pub_tx.reply,"9999");
    	set_zd_data("0130","��ǰ�����ܶ����");
    	goto ErrExit;
    }
	/*��ǰ�����ܶ�hq_sum[0]*/
	set_zd_double("0300",hq_sum[0]);
	/*��ǰ�����ܶ�dq_sum[0]*/
	set_zd_double("0310",dq_sum[0]);
	/*��ǰ����ܶ�lc_sum[0]*/
	set_zd_double("0320",lc_sum[0]);
	/*��ǰ����֪ͨ����ܶ�qt_sum*/
	set_zd_double("0330",qt_sum);
	/*��ǰ�·��ܶ�hq_sum[0]+dq_sum[0]+lc_sum[0]+qt_sum*/
	set_zd_double("0580",hq_sum[0]+dq_sum[0]+lc_sum[0]+qt_sum);
	/*ǰһ�·��ܶ�hq_sum[1]+dq_sum[1]+lc_sum[1]*/
	set_zd_double("0590",hq_sum[1]+dq_sum[1]+lc_sum[1]);
	/*ǰ���·��ܶ�hq_sum[2]+dq_sum[2]+lc_sum[2]*/
	set_zd_double("0600",hq_sum[2]+dq_sum[2]+lc_sum[2]);
	/*ǰ���·��ܶ�hq_sum[3]+dq_sum[3]+lc_sum[3]*/
	set_zd_double("0610",hq_sum[3]+dq_sum[3]+lc_sum[3]);
	/*ǰ���·��ܶ�hq_sum[4]+dq_sum[4]+lc_sum[4]*/
	set_zd_double("0620",hq_sum[4]+dq_sum[4]+lc_sum[4]);
	/*ǰ���·��ܶ�hq_sum[5]+dq_sum[5]+lc_sum[5]*/
	set_zd_double("0630",hq_sum[5]+dq_sum[5]+lc_sum[5]);
	
OkExit:
	set_zd_data("0120","0000");
	set_zd_data("0130","�ͻ��Ƹ������ɹ�");
	strcpy(g_pub_tx.reply,"0000");
	vtcp_log("�ͻ��Ƹ������ɹ�[%s]", g_pub_tx.reply);
	return 0;
ErrExit:
	vtcp_log("�ͻ��Ƹ�����ʧ��[%s]", g_pub_tx.reply);
	return 0;
}
/*��������sys_date����ǰi���� ����ǰi���µ�����*/
long Calculate_Date(long sys_date,int i)
{
	long Y=0;
	long M=0;
	char date[8];
	char month[4];
	memset(date,0x00,sizeof(date));
	memset(month,0x00,sizeof(month));
	Y = sys_date / 100;
	M = sys_date % 100;
	if(M <= i)
	{
		Y = Y - 1;
		M = M + 12 - i;
	}
	else
	{
		M = M - i;
	}
	sprintf(month,"%02d",M);
	sprintf(date,"%ld",Y);
	strcat(date,month);
	long Tx_Date = strtol(date,NULL,10);
	return Tx_Date;
}