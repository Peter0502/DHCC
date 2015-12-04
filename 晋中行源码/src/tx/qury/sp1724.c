
/*************************************************
* �� �� ��:  sp1724.c
* ���������� ��˾�ɶ���Ϣ��ѯ
*
* ��    ��:  lwb
* ������ڣ� 20141021
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1724', '��˾�ɶ���Ϣ��ѯ', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1724', 0, '1724', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('1724', '��˾�ɶ���Ϣ��ѯ', 'sp1724', '0', '0');
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
#include "cif_shareholder_inf_c.h"
#define EXTERN


int sp1724()
{
	int iRet = 0;
	int iTtlnum = 0;
	char cWherelist[1024];
	char cTmpstr[50];
	char cHolder_type[10];
	char cSex[5];
	char cId_type[20];
	double dRate = 0.0;
	char filename[50];
	struct cif_shareholder_inf_c v_cif_shareholder_inf;
	struct cif_shareholder_inf_c f_cif_shareholder_inf;
	FILE *fp = NULL;
		
	memset(&v_cif_shareholder_inf,0x00,sizeof(v_cif_shareholder_inf));
	memset(&f_cif_shareholder_inf,0x00,sizeof(f_cif_shareholder_inf));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmpstr,0x00,sizeof(cTmpstr));
	memset(cHolder_type,0x00,sizeof(cHolder_type));
	memset(cSex,0x00,sizeof(cSex));
	memset(cId_type,0x00,sizeof(cId_type));
	memset(filename,0x00,sizeof(filename));
	pub_base_sysinit();
	
	get_zd_long("0280",&v_cif_shareholder_inf.cif_no);
	get_zd_long("0290",&v_cif_shareholder_inf.shareholder_no);
	get_zd_data("0260",v_cif_shareholder_inf.name);
	get_zd_data("0670",v_cif_shareholder_inf.type);
	get_zd_data("0680",v_cif_shareholder_inf.sex);
	get_zd_data("0690",v_cif_shareholder_inf.id_type);
	get_zd_data("0620",v_cif_shareholder_inf.id_no);
	get_zd_data("0370",v_cif_shareholder_inf.br_no);
	get_zd_data("0380",v_cif_shareholder_inf.ag_cr_no);
	get_zd_double("0400",&v_cif_shareholder_inf.rate);
	get_zd_double("0410",&dRate);
	
	
	if(v_cif_shareholder_inf.cif_no)
	{
		sprintf(cTmpstr,"cif_no=%d and ",v_cif_shareholder_inf.cif_no);	
		strcat(cWherelist,cTmpstr);
	}
	if(v_cif_shareholder_inf.shareholder_no)
	{
		sprintf(cTmpstr,"shareholder_no=%d and ",v_cif_shareholder_inf.shareholder_no);	
		strcat(cWherelist,cTmpstr);
	}	
	if(strlen(v_cif_shareholder_inf.name))
	{
		sprintf(cTmpstr,"name like '%%%%%s%%%%' and ",v_cif_shareholder_inf.name);	
		strcat(cWherelist,cTmpstr);	
	}	
	if(strlen(v_cif_shareholder_inf.type) && v_cif_shareholder_inf.type[0]!='A')
	{
		sprintf(cTmpstr,"type='%c' and ",v_cif_shareholder_inf.type[0]);	
		strcat(cWherelist,cTmpstr);	
	}
	if(strlen(v_cif_shareholder_inf.sex) && v_cif_shareholder_inf.sex[0]!='A')
	{
		sprintf(cTmpstr,"sex='%c' and ",v_cif_shareholder_inf.sex[0]);	
		strcat(cWherelist,cTmpstr);	
	}
	if(strlen(v_cif_shareholder_inf.id_type))
	{
		sprintf(cTmpstr,"id_type='%c' and ",v_cif_shareholder_inf.id_type[0]);	
		strcat(cWherelist,cTmpstr);	
	}	
	if(strlen(v_cif_shareholder_inf.id_no))
	{
		sprintf(cTmpstr,"id_no='%s' and ",v_cif_shareholder_inf.id_no);	
		strcat(cWherelist,cTmpstr);		
	}	
	if(strlen(v_cif_shareholder_inf.br_no))
	{
		sprintf(cTmpstr,"br_no='%s' and ",v_cif_shareholder_inf.br_no);	
		strcat(cWherelist,cTmpstr);		
	}	
	if(strlen(v_cif_shareholder_inf.ag_cr_no))
	{
		sprintf(cTmpstr,"ag_cr_no='%s' and ",v_cif_shareholder_inf.ag_cr_no);	
		strcat(cWherelist,cTmpstr);		
	}	
	if(dRate != 0.0 )
	{
		sprintf(cTmpstr,"rate> =%f and rate < =%f and ",v_cif_shareholder_inf.rate,dRate);	
		strcat(cWherelist,cTmpstr);		
	}	
		
	strcat(cWherelist," 1=1 order by type ");
	
	vtcp_log("cWherelist = [%s] ",cWherelist);
	
	pub_base_AllDwnFilName(filename);
	
	
	
	iRet = Cif_shareholder_inf_Dec_Sel(g_pub_tx.reply,cWherelist);
	ERR_DEAL
	
	while(1)
	{
		iRet = Cif_shareholder_inf_Fet_Sel(&f_cif_shareholder_inf,g_pub_tx.reply);
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
			sprintf(acErrMsg,"ȡcif_shareholder_inf��Ϣ����");
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
			fprintf(fp,"~�ͻ�����|�ɶ����|@�ɶ�����|�ɶ�����|@�ɶ��Ա�|��֯��������|���ô���|ռ�ɱ���|\n");
		}
		
		
		if(f_cif_shareholder_inf.cif_no<-2147000000)  f_cif_shareholder_inf.cif_no=0;
		pub_base_dic_show_str(cHolder_type,"holder_type",f_cif_shareholder_inf.type);
		pub_base_dic_show_str(cSex,"cif_sex",f_cif_shareholder_inf.sex);
		
			
		fprintf(fp,"%ld|%d|%s|%s|%s|%s|%s|%lf|\n",f_cif_shareholder_inf.cif_no,f_cif_shareholder_inf.shareholder_no,cHolder_type, \
		                                          f_cif_shareholder_inf.name,cSex,f_cif_shareholder_inf.br_no,f_cif_shareholder_inf.ag_cr_no,f_cif_shareholder_inf.rate);
		iTtlnum++;
			
	}
	
	iRet = Cif_shareholder_inf_Clo_Sel();
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