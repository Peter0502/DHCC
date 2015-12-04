/*****************************************************
* �� �� ��:  sp8988.c
* ���������� �����˺Ÿ�ƽ̨�˶�֤�����͡�֤�����롢�����Ƿ���ȷ
* ��    ��:  lwb
* ������ڣ� 2014��10��9�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('8988', '֤����Ϣ�˶�', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('8988', 0, '8988', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('8988', '֤����Ϣ�˶�', 'sp8988', '0', '0');
*****************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"

int sp8989()
{
	int iRet = 0;
	char cAc_no[25];
	char cId_type[3];
	char cId_no[25];
	char cName[50];
	
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	
	pub_base_sysinit();
	
	mdmset(cAc_no,0x00,sizeof(cAc_no));
	memset(cId_type,0x00,sizeof(cId_type));
	memset(cId_no,0x00,sizeof(cId_no));
	memset(cName,0x00,sizeof(cName));	
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	
	get_zd_data("0300",cAc_no);
	get_zd_data("0250",cName);
	get_zd_data("0900",cId_type);
	get_zd_data("0620",cId_no);
	
	pub_base_strpack(cAc_no);
	pub_base_strpack(cName);
	pub_base_strpack(cId_no);
	pub_base_strpack(cId_type);
	
	pub_base_old_acno(cAc_no);
	
	iRet = mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s' ",cAc_no);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"�����˺ű����޴��˺ţ�[%s]",cAc_no);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"C115");
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"��ѯ�����˺ű����쳣��[%s]",cAc_no);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"L182");
		goto ErrExit;	
	}
	
	if(strcmp(sMdm_ac_rel.id_type,cId_type))
	{
		sprintf(acErrMsg,"֤�����Ͳ���ȷ��");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
	}
	
	if(strcmp(sMdm_ac_rel.id_no,cId_no))
	{
		sprintf(acErrMsg,"֤�����벻��ȷ��");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
	}
	
	iRet = dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %d ",sMdm_ac_rel.ac_id );
	if(iRet == 100)
	{
		sprintf(acErrMsg,"���ڴ������޴��˺ţ�[%d]",sMdm_ac_rel.ac_id);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"W014");
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"��ѯ���ڴ����쳣��[%d]",sMdm_ac_rel.ac_id);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"W015");
		goto ErrExit;	
	}
	
	if(strcmp(sDd_mst.name,cName))
	{
		sprintf(acErrMsg,"��������ȷ��");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG	
	}
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�˺���Ϣ�˶Գɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"�˺���Ϣ�˶�ʧ�ܣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}