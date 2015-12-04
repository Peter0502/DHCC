/*************************************************
* 文 件 名:  sp1723.c
* 功能描述： 客户家庭信息查询
*
* 作    者:  lwb
* 完成日期： 20141020
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1723', '公司高管信息查询', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1723', 0, '1723', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('1723', '公司高管信息查询', 'sp1723', '0', '0');
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#define ERR_DEAL if(iRet){  \
		sprintf(acErrMsg,"sql error ");  \
		WRITEMSG  \
		goto ErrExit; \	
	}
#include <string.h>
#include "public.h"
#include "cif_manage_inf_c.h"
#define EXTERN


int sp1723()
{
	int iRet = 0;
	int iTtlnum = 0;
	char cWherelist[1024];
	char cTmpstr[50];
	char cType[10];
	char cSex[5];
	char cId_type[20];
	char filename[50];
	struct cif_manage_inf_c v_cif_manage_inf;
	struct cif_manage_inf_c f_cif_manage_inf;
	FILE *fp = NULL;
		
	memset(&v_cif_manage_inf,0x00,sizeof(v_cif_manage_inf));
	memset(&f_cif_manage_inf,0x00,sizeof(f_cif_manage_inf));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmpstr,0x00,sizeof(cTmpstr));
	memset(cType,0x00,sizeof(cType));
	memset(cSex,0x00,sizeof(cSex));
	memset(cId_type,0x00,sizeof(cId_type));
	memset(filename,0x00,sizeof(filename));
	pub_base_sysinit();
	
	get_zd_long("0280",&v_cif_manage_inf.cif_no);
	get_zd_long("0290",&v_cif_manage_inf.manage_no);
	get_zd_data("0260",v_cif_manage_inf.name);
	get_zd_data("0670",v_cif_manage_inf.type);
	get_zd_data("0680",v_cif_manage_inf.sex);
	get_zd_data("0690",v_cif_manage_inf.id_type);
	get_zd_data("0620",v_cif_manage_inf.id_no);
	
	if(v_cif_manage_inf.cif_no)
	{
		sprintf(cTmpstr,"cif_no=%d and ",v_cif_manage_inf.cif_no);	
		strcat(cWherelist,cTmpstr);
	}
	if(v_cif_manage_inf.manage_no)
	{
		sprintf(cTmpstr,"manage_no=%d and ",v_cif_manage_inf.manage_no);	
		strcat(cWherelist,cTmpstr);
	}		
	if(strlen(v_cif_manage_inf.name))
	{
		sprintf(cTmpstr,"name like '%%%%%s%%%%' and ",v_cif_manage_inf.name);	
		strcat(cWherelist,cTmpstr);	
	}	
	if(strlen(v_cif_manage_inf.type) && v_cif_manage_inf.type[0]!='A')
	{
		sprintf(cTmpstr,"type='%c' and ",v_cif_manage_inf.type[0]);	
		strcat(cWherelist,cTmpstr);	
	}
	if(strlen(v_cif_manage_inf.sex) && v_cif_manage_inf.sex[0]!='A')
	{
		sprintf(cTmpstr,"sex='%c' and ",v_cif_manage_inf.sex[0]);	
		strcat(cWherelist,cTmpstr);	
	}
	if(strlen(v_cif_manage_inf.id_type))
	{
		sprintf(cTmpstr,"id_type='%c' and ",v_cif_manage_inf.id_type[0]);	
		strcat(cWherelist,cTmpstr);	
	}	
	if(strlen(v_cif_manage_inf.id_no))
	{
		sprintf(cTmpstr,"id_no='%s' and ",v_cif_manage_inf.id_no);	
		strcat(cWherelist,cTmpstr);		
	}	
	strcat(cWherelist," 1=1 ");
	
	vtcp_log("cWherelist = [%s] ",cWherelist);
	
	pub_base_AllDwnFilName(filename);
	
	
	
	iRet = Cif_manage_inf_Dec_Sel(g_pub_tx.reply,cWherelist);
	ERR_DEAL
	
	while(1)
	{
		iRet = Cif_manage_inf_Fet_Sel(&f_cif_manage_inf,g_pub_tx.reply);
		if(iRet == 100)
		{
			if(iTtlnum == 0)
			{
				sprintf(acErrMsg,"无符合条件的记录！");
				WRITEMSG
				strcpy(g_pub_tx.reply,"S049");
				goto ErrExit;
			}
			
			break;	
		}
		else if(iRet)
		{
			sprintf(acErrMsg,"取Cif_manage_inf信息错误！");
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
			fprintf(fp,"~客户代码|高管编号|@高管类型|高管名称|@高管性别|\n");
		}
		
		
		if(f_cif_manage_inf.cif_no<-2147000000)  f_cif_manage_inf.cif_no=0;
		pub_base_dic_show_str(cType,"manage_type",f_cif_manage_inf.type);
		pub_base_dic_show_str(cSex,"cif_sex",f_cif_manage_inf.sex);
		
			
		fprintf(fp,"%ld|%d|%s|%s|%s|\n",f_cif_manage_inf.cif_no,f_cif_manage_inf.manage_no, \
		                                          cType,f_cif_manage_inf.name,cSex);
		iTtlnum++;
			
	}
	
	iRet = Cif_manage_inf_Clo_Sel();
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
