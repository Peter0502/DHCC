/*****************************************************
* 文 件 名:  sp8988.c
* 功能描述： 根据账号给平台核对证件类型、证件号码、户名是否正确
* 作    者:  lwb
* 完成日期： 2014年10月9日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#include "stdio.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "cif_email_inf_c.h"

int sp8988()
{
	int iRet = 0;
	char cAc_no[25];
	char cId_type[3];
	char cId_no[25];
	char cName[50];
	char cEmail[21];
	
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct cif_email_inf_c sCif_email_inf;
	
	pub_base_sysinit();
	
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cId_type,0x00,sizeof(cId_type));
	memset(cId_no,0x00,sizeof(cId_no));
	memset(cEmail,0x00,sizeof(cEmail));
	memset(cName,0x00,sizeof(cName));	
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));
	memset(&sCif_email_inf,0x00,sizeof(sCif_email_inf));

	
	get_zd_data("0300",cAc_no);
	get_zd_data("0250",cName);
	get_zd_data("0900",cId_type);
	get_zd_data("0620",cId_no);
	get_zd_data("0310",cEmail);
	
	pub_base_strpack(cAc_no);
	pub_base_strpack(cName);
	pub_base_strpack(cId_no);
	pub_base_strpack(cId_type);
	
	pub_base_old_acno(cAc_no);
	
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s' ",cAc_no);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"介质账号表中无此账号！[%s]",cAc_no);	
		WRITEMSG
		 sprintf(acErrMsg,"账号错误！");
		strcpy(g_pub_tx.reply,"C115");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"查询介质账号表中异常！[%s]",cAc_no);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"L182");
		goto ErrExit;	
	}
	
	if(strcmp(sMdm_ac_rel.id_type,cId_type))
	{
		sprintf(acErrMsg,"证件类型不正确！");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;	
	}
	
	if(strcmp(sMdm_ac_rel.id_no,cId_no))
	{
		sprintf(acErrMsg,"证件号码不正确！");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		goto ErrExit;	
	}
	
	iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id = %d ",sMdm_ac_rel.ac_id );
	if(iRet == 100)
	{
		sprintf(acErrMsg,"活期存款表中无此账号！[%d]",sMdm_ac_rel.ac_id);	
		WRITEMSG
		sprintf(acErrMsg,"账号错误！");
		strcpy(g_pub_tx.reply,"W014");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"查询活期存款表异常！[%d]",sMdm_ac_rel.ac_id);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"W015");
		goto ErrExit;	
	}
	
	if(strcmp(sDd_mst.name,cName))
	{
		sprintf(acErrMsg,"户名不正确！");
		strcpy(g_pub_tx.reply,"H034");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG	
		goto ErrExit;	
	}
	set_zd_long("0290",sDd_mst.cif_no);
	/**
	iRet = Cif_email_inf_Sel(g_pub_tx.reply,&sCif_email_inf,"cif_no = %d ",sDd_mst.cif_no );
	if(iRet == 100)
	{
		sprintf(acErrMsg,"该客户号不存在！[%d]",sMdm_ac_rel.ac_id);	
		WRITEMSG
		sprintf(acErrMsg,"客户号错误！");
		strcpy(g_pub_tx.reply,"C097");
		set_zd_data(DC_GET_MSG,acErrMsg);
		goto ErrExit;
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"查询客户电子信息表异常！[%d]",sMdm_ac_rel.ac_id);	
		WRITEMSG
		strcpy(g_pub_tx.reply,"C097");
		goto ErrExit;	
	}
	*/
	/**修改客户手机联系方式**/
	/**
	if (sCif_email_inf.addr_type[0] == '5')
	{
		iRet=Cif_email_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld and addr_seqn=%d",sCif_email_inf.cif_no,sCif_email_inf.addr_seqn);
		if (iRet)
		{
			sprintf(acErrMsg,"DECLARE Cif_email_inf_rel游标异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C006");
			Cif_email_inf_Clo_Upd();
			goto ErrExit;
		}
		iRet=Cif_email_inf_Fet_Upd(&sCif_email_inf,g_pub_tx.reply);
		if (iRet)
		{
			sprintf(acErrMsg,"FETCH Cif_email_inf游标异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C006");
			Cif_email_inf_Clo_Upd();
			goto ErrExit;
		}
		strcpy(sCif_email_inf.email,cEmail);
		vtcp_log("cif_no==[%d]\n",sCif_email_inf.cif_no);
		vtcp_log("addr_seqn=[%d]\n",sCif_email_inf.addr_seqn);
		vtcp_log("addr_type=[%c]\n",sCif_email_inf.addr_type[0]);
		vtcp_log("email==[%s]\n",sCif_email_inf.email);
		iRet=Cif_email_inf_Upd_Upd(sCif_email_inf,g_pub_tx.reply);
		if (iRet)
		{
			sprintf(acErrMsg,"UPDATE Cif_email_inf游标异常![%s]",g_pub_tx.reply);
			WRITEMSG
			strcpy(g_pub_tx.reply,"C006");
			Cif_email_inf_Clo_Upd();
			goto ErrExit;
		}
	 	vtcp_log("sCif_email_inf.addr_seqn===[%d]\n",sCif_email_inf.addr_seqn);
	 	Cif_email_inf_Clo_Upd();		
	}
	**/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"账号信息核对成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"账号信息核对失败！[%s]",g_pub_tx.reply);
	WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}
