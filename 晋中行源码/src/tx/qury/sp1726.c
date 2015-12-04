/*************************************************
* 文 件 名:  sp1726.c
* 功能描述： 客户家庭信息查询
*
* 作    者:  zj
* 完成日期： 20141030
*insert into TX_DEF (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('1726', '公司机构信息查询', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '0', '4', '0');
*insert into TX_FLOW_DEF (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('1726', 0, '1726', '#$');
*insert into TX_SUB_DEF (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('1726', '公司机构信息查询', 'sp1726', '0', '0');
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
#include "cif_copa_inf_c.h"
#define EXTERN

int sp1726()
{
	int iRet = 0;
	int iTtlnum = 0;
	char cWherelist[1024];
	char cTmpstr[50];
	
	char cOr_ag_type[10];
	char cOr_ag_mst_type[30];
	char cCop_sts[10];
	char cEco_type[30];
	char cUp_reg_type[30];
	
	char filename[50];
	struct cif_copa_inf_c v_cif_copa_inf;
	struct cif_copa_inf_c f_cif_copa_inf;
	FILE *fp = NULL;
		
	memset(&v_cif_copa_inf,0x00,sizeof(v_cif_copa_inf));
	memset(&f_cif_copa_inf,0x00,sizeof(f_cif_copa_inf));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmpstr,0x00,sizeof(cTmpstr));
	memset(cOr_ag_mst_type,0x00,sizeof(cOr_ag_mst_type));
	memset(cOr_ag_type,0x00,sizeof(cOr_ag_type));
	memset(cCop_sts,0x00,sizeof(cCop_sts));
	memset(cEco_type,0x00,sizeof(cEco_type));

	memset(filename,0x00,sizeof(filename));
	pub_base_sysinit();
	
	get_zd_data("0250",v_cif_copa_inf.sc_bs);
	get_zd_data("0260",v_cif_copa_inf.con_en_name);	
	get_zd_data("0270",v_cif_copa_inf.up_br_name);
	get_zd_long("0280",&v_cif_copa_inf.cif_no);
	get_zd_data("0300",v_cif_copa_inf.ag_cr_no);
	get_zd_data("0820",v_cif_copa_inf.or_ag_type);
	get_zd_data("0480",v_cif_copa_inf.or_ag_mst_type);
	get_zd_data("0580",v_cif_copa_inf.eco_ind_type);
	get_zd_data("0500",v_cif_copa_inf.eco_type);
	/*
	get_zd_data("0810",sCif_copa_inf.ent_tel);
	*/
	get_zd_data("0670",v_cif_copa_inf.up_reg_type);
	get_zd_data("0310",v_cif_copa_inf.up_reg_no);
	get_zd_data("0600",v_cif_copa_inf.up_ag_zz_no);
	get_zd_data("0830",v_cif_copa_inf.up_ag_cr_no);
	get_zd_data("0620",v_cif_copa_inf.check_no);
	get_zd_data("0630",v_cif_copa_inf.reg_ad_area_no);
	get_zd_long("0440",&v_cif_copa_inf.beg_date);
	get_zd_long("0450",&v_cif_copa_inf.end_date);
	get_zd_data("0710",v_cif_copa_inf.cop_sts);
	
	/*客户号*/
	if(v_cif_copa_inf.cif_no)
	{
		sprintf(cTmpstr,"cif_no=%d and ",v_cif_copa_inf.cif_no);	
		strcat(cWherelist,cTmpstr);
	}
	/*机构状态*/
	if(strlen(v_cif_copa_inf.cop_sts) && v_cif_copa_inf.cop_sts[0]!='A')
	{
		sprintf(cTmpstr,"cop_sts='%c' and ",v_cif_copa_inf.cop_sts[0]);	
		strcat(cWherelist,cTmpstr);
	}	
	/*结构信用代码*/
	if(strlen(v_cif_copa_inf.ag_cr_no))
	{
		sprintf(cTmpstr,"ag_cr_no='%s' and ",v_cif_copa_inf.ag_cr_no);	
		strcat(cWherelist,cTmpstr);	
	}	
	/*开户许可证*/
	if(strlen(v_cif_copa_inf.check_no))
	{
		sprintf(cTmpstr,"check_no='%s' and ",v_cif_copa_inf.check_no);
		strcat(cWherelist,cTmpstr);
	}
	/*开始日期*/
	if(v_cif_copa_inf.beg_date)
	{
		sprintf(cTmpstr,"beg_date=%d and ",v_cif_copa_inf.beg_date);
		strcat(cWherelist,cTmpstr);
	}
	/*结束日期*/
	if(v_cif_copa_inf.end_date)
	{
		sprintf(cTmpstr,"end_date=%d and ",v_cif_copa_inf.end_date);
		strcat(cWherelist,cTmpstr);
	}
	/*组织结构类别*/
	if(strlen(v_cif_copa_inf.or_ag_type) && v_cif_copa_inf.or_ag_type[0]!='A')
	{
		sprintf(cTmpstr,"or_ag_type='%c' and ",v_cif_copa_inf.or_ag_type[0]);
		strcat(cWherelist,cTmpstr);
	}
	/*组织机构细分*/
	if(strlen(v_cif_copa_inf.or_ag_mst_type) && v_cif_copa_inf.or_ag_mst_type[0]!='A')
	{
		sprintf(cTmpstr,"or_ag_mst_type='%c' and ",v_cif_copa_inf.or_ag_mst_type[0]);
		strcat(cWherelist,cTmpstr);	
	}
	/*经济类型*/
	if(strlen(v_cif_copa_inf.eco_type) && v_cif_copa_inf.eco_type[0]!='A')
	{
		sprintf(cTmpstr,"eco_type='%c' and ",v_cif_copa_inf.eco_type[0]);
		strcat(cWherelist,cTmpstr);
	}
	/*业务范围*/
	if(strlen(v_cif_copa_inf.sc_bs))
	{
		sprintf(cTmpstr,"sc_bs like '%%%%%s%%%%%' and ",v_cif_copa_inf.sc_bs);
		strcat(cWherelist,cTmpstr);
	}
	/*机构英文名*/
	if(strlen(v_cif_copa_inf.con_en_name))
	{
		sprintf(cTmpstr,"con_en_name like '%%%%%%s%%%%%%' and ",v_cif_copa_inf.con_en_name);
		strcat(cWherelist,cTmpstr);
	}
	/*上级机构名称*/
	if(strlen(v_cif_copa_inf.up_br_name))
	{
		sprintf(cTmpstr,"up_br_name like '%%%%%%s%%%%%%' and ",v_cif_copa_inf.up_br_name);
		strcat(cWherelist,cTmpstr);
	}
	/*上级机构登记注册类型*/
	if(strlen(v_cif_copa_inf.up_reg_type) && v_cif_copa_inf.up_reg_type[0]!='A')
	{
		sprintf(cTmpstr,"up_reg_type='%c' and ",v_cif_copa_inf.up_reg_type[0]);
		strcat(cWherelist,cTmpstr);
	}
	/*上级机构登记注册号码*/
	if(strlen(v_cif_copa_inf.up_reg_no))
	{
		sprintf(cTmpstr,"up_reg_no='%s' and ",v_cif_copa_inf.up_reg_no);
		strcat(cWherelist,cTmpstr);
	}
	/*上级机构组织代码*/
	if(strlen(v_cif_copa_inf.up_ag_zz_no))
	{
		sprintf(cTmpstr,"up_ag_zz_no='%s' and ",v_cif_copa_inf.up_ag_zz_no);
		strcat(cWherelist,cTmpstr);
	}
	/*上级机构信用代码*/
	if(strlen(v_cif_copa_inf.up_ag_cr_no))
	{
		sprintf(cTmpstr,"up_ag_cr_no='%s' and ",v_cif_copa_inf.up_ag_cr_no);
		strcat(cWherelist,cTmpstr);
	}

	/*行政区域号*/
	if(strlen(v_cif_copa_inf.reg_ad_area_no))
	{
		sprintf(cTmpstr,"reg_ad_area_no='%s' and ",v_cif_copa_inf.reg_ad_area_no);
		strcat(cWherelist,cTmpstr);
	}
	/*经济行业分类*/
	if(strlen(v_cif_copa_inf.eco_ind_type))
	{
		sprintf(cTmpstr,"eco_ind_type='%s' and ",v_cif_copa_inf.eco_ind_type);
		strcat(cWherelist,cTmpstr);
	}

	
	strcat(cWherelist," 1=1 ");
	
	vtcp_log("cWherelist = [%s] ",cWherelist);
	
	pub_base_AllDwnFilName(filename);
	

	iRet = Cif_copa_inf_Dec_Sel(g_pub_tx.reply,cWherelist);
	ERR_DEAL
	
	while(1)
	{
		iRet = Cif_copa_inf_Fet_Sel(&f_cif_copa_inf,g_pub_tx.reply);
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
			sprintf(acErrMsg,"取Cif_copa_inf信息错误！");
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
			fprintf(fp,"~客户代码|@机构状态|机构信用代码|开户许可证|营业执照开始日期|结束日期|注册地行政区号 \
										|@组织机构类别|@组织机构细分|经济行业分类|@经济类型|业务范围|机构英文名|财务电话|上级机构名称 \
										|@上级机构登记注册类型|上级机构登记注册号码|上级机构组织代码|上级机构信用代码|\n");
		}
		
		if(f_cif_copa_inf.cif_no<-2147000000)  f_cif_copa_inf.cif_no=0;
		pub_base_dic_show_str(cCop_sts,"cop_sts",f_cif_copa_inf.cop_sts);
		pub_base_dic_show_str(cOr_ag_type,"or_ag_type",f_cif_copa_inf.or_ag_type);
		pub_base_dic_show_str(cOr_ag_mst_type,"or_ag_mst_type",f_cif_copa_inf.or_ag_mst_type);
		pub_base_dic_show_str(cEco_type,"eco_type",f_cif_copa_inf.eco_type);
		pub_base_dic_show_str(cUp_reg_type,"cop_id",f_cif_copa_inf.up_reg_no);
		
			
		fprintf(fp,"%ld|%s|%s|%s|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",f_cif_copa_inf.cif_no,cCop_sts,f_cif_copa_inf.ag_cr_no,	\																		
		                                          f_cif_copa_inf.check_no, f_cif_copa_inf.beg_date,f_cif_copa_inf.end_date, \
		                                          f_cif_copa_inf.reg_ad_area_no,cOr_ag_type,cOr_ag_mst_type,f_cif_copa_inf.eco_ind_type,	\
		                                          cEco_type,f_cif_copa_inf.sc_bs,f_cif_copa_inf.con_en_name,	\
		                                          f_cif_copa_inf.ent_tel,f_cif_copa_inf.up_br_name,cUp_reg_type,	\
		                                          f_cif_copa_inf.up_reg_no,f_cif_copa_inf.up_ag_zz_no,f_cif_copa_inf.up_ag_cr_no);
		iTtlnum++;
			
	}
	
	iRet = Cif_copa_inf_Clo_Sel();
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

