
/*************************************************
* �� �� ��:  sp1725.c
* ���������� ������ҵ��Ϣ��ѯ
*
* ��    ��:  lwb
* ������ڣ� 20141021
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1725', '������ҵ��Ϣ��ѯ', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1725', 0, '1725', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('1725', '������ҵ��Ϣ��ѯ', 'sp1725', '0', '0');
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#define ERR_DEAL if(iRet){  \
		sprintf(acErrMsg,"sql error ");  \
		WRITEMSG  \
		goto ErrExit; \	
	}
#include <string.h>
#include "public.h"
#include "join_cop_inf_c.h"
#define EXTERN


int sp1725()
{
	int iRet = 0;
	int iTtlnum = 0;
	char cWherelist[1024];
	char cTmpstr[50];
	char cJoin_cop_type[10];
	char cId_type[20];
	char filename[50];
	struct join_cop_inf_c v_join_cop_inf;
	struct join_cop_inf_c f_join_cop_inf;
	FILE *fp = NULL;
		
	memset(&v_join_cop_inf,0x00,sizeof(v_join_cop_inf));
	memset(&f_join_cop_inf,0x00,sizeof(f_join_cop_inf));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmpstr,0x00,sizeof(cTmpstr));
	memset(cJoin_cop_type,0x00,sizeof(cJoin_cop_type));
	memset(cId_type,0x00,sizeof(cId_type));
	memset(filename,0x00,sizeof(filename));
	pub_base_sysinit();
	
	get_zd_long("0280",&v_join_cop_inf.cif_no);
	get_zd_long("0290",&v_join_cop_inf.cop_no);
	get_zd_data("0260",v_join_cop_inf.name);
	get_zd_data("0670",v_join_cop_inf.type);
	get_zd_data("0690",v_join_cop_inf.id_type);
	get_zd_data("0620",v_join_cop_inf.id_no);
	get_zd_data("0370",v_join_cop_inf.br_no);
	get_zd_data("0380",v_join_cop_inf.ag_cr_no);

	
	
	if(v_join_cop_inf.cif_no)
	{
		sprintf(cTmpstr,"cif_no=%d and ",v_join_cop_inf.cif_no);	
		strcat(cWherelist,cTmpstr);
	}
	if(v_join_cop_inf.cop_no)
	{
		sprintf(cTmpstr,"cop_no=%d and ",v_join_cop_inf.cop_no);	
		strcat(cWherelist,cTmpstr);
	}	
	if(strlen(v_join_cop_inf.name))
	{
		sprintf(cTmpstr,"name like '%%%%%s%%%%' and ",v_join_cop_inf.name);	
		strcat(cWherelist,cTmpstr);	
	}	
	if(strlen(v_join_cop_inf.type) && v_join_cop_inf.type[0]!='A')
	{
		sprintf(cTmpstr,"type='%c' and ",v_join_cop_inf.type[0]);	
		strcat(cWherelist,cTmpstr);	
	}
	if(strlen(v_join_cop_inf.id_type))
	{
		sprintf(cTmpstr,"id_type='%c' and ",v_join_cop_inf.id_type[0]);	
		strcat(cWherelist,cTmpstr);	
	}	
	if(strlen(v_join_cop_inf.id_no))
	{
		sprintf(cTmpstr,"id_no='%s' and ",v_join_cop_inf.id_no);	
		strcat(cWherelist,cTmpstr);		
	}	
	if(strlen(v_join_cop_inf.br_no))
	{
		sprintf(cTmpstr,"br_no='%s' and ",v_join_cop_inf.br_no);	
		strcat(cWherelist,cTmpstr);		
	}	
	if(strlen(v_join_cop_inf.ag_cr_no))
	{
		sprintf(cTmpstr,"ag_cr_no='%s' and ",v_join_cop_inf.ag_cr_no);	
		strcat(cWherelist,cTmpstr);		
	}	

	strcat(cWherelist," 1=1 order by type ");
	
	vtcp_log("cWherelist = [%s] ",cWherelist);
	
	pub_base_AllDwnFilName(filename);
	
	
	
	iRet = Join_cop_inf_Dec_Sel(g_pub_tx.reply,cWherelist);
	ERR_DEAL
	
	while(1)
	{
		iRet = Join_cop_inf_Fet_Sel(&f_join_cop_inf,g_pub_tx.reply);
		if(iRet == 100)
		{
			if(iTtlnum == 0)
			{
				sprintf(acErrMsg,"�޷��������ļ�¼��");
				WRITEMSG
				strcpy(g_pub_tx.reply,"S049");
				goto ErrExit;
			}
			
			break;	
		}
		else if(iRet)
		{
			sprintf(acErrMsg,"ȡjoin_cop_inf��Ϣ����");
			WRITEMSG
			goto ErrExit;	
		}
		
		if(!iTtlnum)
		{
			fp = fopen(filename,"w");
		
			if(fp == NULL)
			{
				sprintf(acErrMsg,"open file [%s] error",filename);
				WRITEMSG
				strcpy(g_pub_tx.reply,"S047");
				goto ErrExit;	
			}
			fprintf(fp,"~�ͻ�����|������ҵ���|@������ҵ����|������ҵ����|\n");
		}
		
		
		if(f_join_cop_inf.cif_no<-2147000000)  f_join_cop_inf.cif_no=0;
		pub_base_dic_show_str(cJoin_cop_type,"join_type",f_join_cop_inf.type);

		
			
		fprintf(fp,"%ld|%d|%s|%s|\n",f_join_cop_inf.cif_no,f_join_cop_inf.cop_no,cJoin_cop_type, \
		                                          f_join_cop_inf.name);
		iTtlnum++;
			
	}
	
	iRet = Join_cop_inf_Clo_Sel();
	ERR_DEAL

	fclose(fp);
	set_zd_data(DC_FILE_SND,"1");
	
GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	if(fp!=NULL)
		fclose(fp);
	return 1;
	
	
}
