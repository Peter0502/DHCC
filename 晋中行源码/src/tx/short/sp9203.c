/*************************************************
* �� �� ��:  sp9203.c
* ���������� ���гжһ�Ʊ��ѯ
*
* ��    ��:  
* ������ڣ� 
*
* �޸ļ�¼�� 
* ��   ��:   20081126
* �� �� ��:  
* �޸�����:
* ���ױ����ݣ�
------------------------------
tx_def
	insert into tx_def values('9203', '��ѯ���гжһ�Ʊ',
	'10000000000000000000000000000000000000010001110000000000000000000010110000000000000000000010000000000000000000000000010000000000',
	'0', '4', '0')	
-------------------------------	
tx_flow_def
	insert into tx_flow_def values('9203', '0', '9203', '#$')
-------------------------------
tx_sub_def	
	insert into tx_sub_def values('9203','���гжһ�Ʊ��ѯ', 'sp9203','0','0')
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"
#include "td_mst_c.h"

int sp9203()
{
       	int ret=0;
       	char  cTmp[20];	
       	char cType[2];
       	struct mo_bank_po_c mo_bank_po;
       	struct td_mst_c td_mst;
       	
       	memset(cType, 0x00, sizeof(cType));
	memset(cTmp, 0x00, sizeof(cTmp));
       /*memset(td_mst, 0x00, sizeof(struct td_mst_c));*/
       	memset(&mo_bank_po, 0x00, sizeof(struct mo_bank_po_c));
       
       	vtcp_log("����9203����\n");
       
       /*get_zd_data("0680", cType);*/
       	get_zd_data("0370",cTmp);
	pub_base_strpack(cTmp);
	

	vtcp_log("��ѯ���гжһ�Ʊ\n");
	ret = Mo_bank_po_Sel(g_pub_tx.reply, &mo_bank_po, " po_no = '%s'", cTmp);
	if(ret && ret!=100)
	{
		sprintf(acErrMsg,"��ѯ���гжһ�Ʊ�Ǽǲ�����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"B060");
		goto ErrExit;
	}
	else if(ret==100)	
	{
		if(strlen(cTmp) <= 8)
		{
			sprintf(acErrMsg,"û�д˳жһ�Ʊ, ����Ϊ��ϵͳ��Ʊ��!");
			WRITEMSG
			
			ret = Mo_bank_po_Sel(g_pub_tx.reply, &mo_bank_po, " po_no like '%%%s' ", cTmp);
			if(ret && ret!=100)
			{
				sprintf(acErrMsg,"��ѯ���гжһ�Ʊ�Ǽǲ�����!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"B060");
				goto ErrExit;
			}
			else if(ret==100)	
			{
				sprintf(acErrMsg,"û�д˳жһ�Ʊ!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"B061");
				goto ErrExit;			
			}
		}
		else
		{
			sprintf(acErrMsg,"û�д˳жһ�Ʊ��!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"B061");
			goto ErrExit;	
		}
	}       	
	set_zd_data("0910", mo_bank_po.sign_br_no);		/*ǩ�����к�*/
	set_zd_long("0440", mo_bank_po.sign_date);		/*ǩ������*/
	set_zd_long("0450", mo_bank_po.receive_date);		/*��������*/
	set_zd_long("0460", mo_bank_po.cash_date);		/*��������*/
	set_zd_double("0400", mo_bank_po.po_amt);		/*Ʊ����*/
	set_zd_data("0670", mo_bank_po.at_term_ind);		/*Ʊ�ݵ��ڱ�־*/
	set_zd_data("0690", mo_bank_po.po_sts);		/*Ʊ�ݵ��ڱ�־*/
	set_zd_data("0700", mo_bank_po.po_ind);	/*��Ʊ��ʧ��־ 0 ��ʧ 1����*/

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
