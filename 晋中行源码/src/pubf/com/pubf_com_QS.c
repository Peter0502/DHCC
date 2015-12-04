/*************************************************
* �� �� ��: �����в�ѯ����
* ����������
*          cFind_qs_br_no1:�������������е��к�
*          iFind_qs_br_no2�����ظ����������е��к�
* ��    ��: ����
* ������ڣ�2009��08��05��
select * from com_branch where br_type='4' and up_br_no=(select up_br_no from com_branch where br_no='�����˺�')
*************************************************/
#define EXTERN
#include "string.h"
#include "public.h"
#include "com_branch_c.h"
#include "com_tx_br_rel_c.h"

/****��ѯ���е������к�****/
char * cFind_qs_br_no1(char* cBr_no)
{
	int  iRet=0;
	char cReply[5];
	struct com_branch_c sCom;
	
	memset(cReply,0x00,sizeof(cReply));
	memset(&sCom,0x00,sizeof(sCom));
	
	iRet=Com_branch_Sel(cReply,&sCom,"br_type='1'");
	if(iRet)
	{
		vtcp_log("[%s][%d]�����������Ĳ�ѯ����[%s]ʹ�ú궨�����������=%s",__FILE__,__LINE__,sCom.br_no,QS_BR_NO);
		memcpy(cBr_no,QS_BR_NO,BRNO_LEN);/**QS_BR_NOΪ���������ж�Ӧ�ĺ궨��**/
	}
	else
	{
		memcpy(cBr_no,sCom.br_no,sizeof(sCom.br_no)-1);
	}
	return cBr_no;
}
/**��ѯ�������������е��к�**/
int iFind_qs_br_no2(char *cBr_no,char *cQs_no)
{
	int  iRet=0;
	char cReply[5];
	struct com_branch_c sCom;
	struct com_tx_br_rel_c sCom_tx_br_rel;
	
	memset(cReply,0x00,sizeof(cReply));
	memset(&sCom,0x00,sizeof(sCom));
	/*************  ��������������ĵĲ�һ�� ���� MODIFY BY GUJINGYU  2010-12-28
	iRet=Com_branch_Sel(cReply,&sCom,"(br_type='4' or br_type='1') and up_br_no=(select up_br_no from com_branch where br_no='%s')",cBr_no);
	if(iRet)
	{
		vtcp_log("[%s][%d]��ѯ��������������!![%d]\n",__FILE__,__LINE__,iRet);
		return iRet;
	}
	vtcp_log("%s,%d get_ls_qs_br_no='%s'",__FILE__,__LINE__,sCom.br_no); 
	memcpy(cQs_no,sCom.br_no,sizeof(sCom.br_no)-1);
	if(memcmp(cQs_no,"01000",BRNO_LEN)==0)
		cFind_qs_br_no1(cQs_no);***********/
	iRet=Com_tx_br_rel_Sel(cReply,&sCom_tx_br_rel," rel_type='7' and br_no='%s'",cBr_no);
	if(iRet)
	{
		vtcp_log("[%s][%d]��ѯ��������������!![%d]\n",__FILE__,__LINE__,iRet);
		return iRet;
	}
	memcpy(cQs_no,sCom_tx_br_rel.up_br_no,sizeof(sCom_tx_br_rel.up_br_no)-1);
	vtcp_log("%s,%d get_ls_qs_br_no='%s'",__FILE__,__LINE__,cQs_no);
	return 0;
}

int iFind_same_upbrno(char *br_no1 ,char *br_no2)
{
	int  iRet=0;
	char cReply[5];
	struct com_branch_c sCom1;
	struct com_branch_c sCom2;
	
	memset(cReply,0x00,sizeof(cReply));
	memset(&sCom1,0x00,sizeof(sCom1));
	memset(&sCom2,0x00,sizeof(sCom2));
	
	iRet=Com_branch_Sel(cReply,&sCom1," br_no='%s'",br_no1);
	if(iRet)
	{
		vtcp_log("[%s][%d]��ѯ��������������!![%d]\n",__FILE__,__LINE__,iRet);
		return iRet;
	}
	iRet=Com_branch_Sel(cReply,&sCom2," br_no='%s'",br_no2);
	if(iRet)
	{
		vtcp_log("[%s][%d]��ѯ��������������!![%d]\n",__FILE__,__LINE__,iRet);
		return iRet;
	}
	if(memcmp(sCom1.up_br_no,"00005",BRNO_LEN)==0)
	{
		memcpy(sCom1.up_br_no,"01004",BRNO_LEN);
	}
	if(memcmp(sCom2.up_br_no,"00005",BRNO_LEN)==0)
	{
		memcpy(sCom2.up_br_no,"01004",BRNO_LEN);
	}
	if(memcmp(sCom1.up_br_no, sCom2.up_br_no,BRNO_LEN)!=0)
	{
		vtcp_log("[%s][%d]�ϼ�����[%s]��һ��[%s]\n",__FILE__,__LINE__,sCom1.up_br_no, sCom2.up_br_no);
		return -1;
	}
	return 0;
}
