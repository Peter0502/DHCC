/*************************************************
* �� �� ��:  sp8552.c
* ���������� ��˾�߹���Ϣά����ݲ�ѯ
* ��    ��:  zj
* ������ڣ� 2014��10��21��
insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('8552', '��˾�߹���Ϣ���', '10000000000000000000000001000000000000000000000000000000000001000011100000000000000000000000000000000000000000000000000000000000', '0', '4', '2');
insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('8552', 0, '8552', '#$');
insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('8552', '��˾�߹���Ϣ���', 'sp8552', '0', '0');

* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <string.h>

#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "cif_manage_inf_c.h"

int sp8552()
{
	int iRet = 0;
	long iCif_no = 0;
	int iManage_no = 0;

		
	struct cif_manage_inf_c sCif_manage_inf;
	struct cif_basic_inf_c sCif_basic_inf;
	
	memset(&sCif_manage_inf,0x00,sizeof(sCif_manage_inf));
	memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));
	
	pub_base_sysinit();
	
	get_zd_long("0280",&iCif_no);
	get_zd_int("0290",&iManage_no);
	
	iRet = Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf,"cif_no=%d ",iCif_no);
	if(iRet == 100 )
	{
		sprintf(acErrMsg,"�ͻ�������Ϣ�����޴˿ͻ��ţ�");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C051");
		goto ErrExit;			
	}
	else if( iRet )
	{
		sprintf( acErrMsg, "��ѯ�ͻ���Ϣ���쳣!!ret=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	/*��ʼ��ѯ��˾�߹���Ϣ��*/
	iRet = Cif_manage_inf_Sel(g_pub_tx.reply,&sCif_manage_inf,"cif_no = %d and manage_no = %d",iCif_no,iManage_no);	
	if(iRet == 100)
	{
		sprintf(acErrMsg,"��˾�߹���Ϣ�����޴˼�¼��");
		WRITEMSG
		strcpy(g_pub_tx.reply,"");
		goto ErrExit;				
	}
	else if( iRet )
	{
		sprintf( acErrMsg, "��ѯ��˾�߹���Ϣ���쳣!!ret=[%d]", iRet );
		WRITEMSG
		goto ErrExit;
	}
	
	set_zd_data("0260",sCif_manage_inf.name);
	set_zd_data("0670",sCif_manage_inf.type);
	set_zd_data("0680",sCif_manage_inf.sex);
	set_zd_data("0690",sCif_manage_inf.id_type);
	set_zd_data("0620",sCif_manage_inf.id_no);
	
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
		
}


