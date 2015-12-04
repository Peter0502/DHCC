/*********************************
文件名称：spW007
功能：    定活互转子账户查询
完成时间:2011年11月9日
作者:  zhaojiwen 
修改记录：
  1、修改时间
  2、修改人
  3、修改内容
  
*********************************/
#include "stdio.h"
#define  EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "td_parm_c.h"
#include "mdm_ac_rel_c.h"

int spW007()
{
	int iRc=0;
	int ret =0;
	
	char ac_no[30];
	char stmp[100];
	char temp[500];
	char filename[40];
	
	FILE *fp;
	
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	struct mdm_ac_rel_c sMdm_ac_rel;
	
	
	memset(filename,0x00,sizeof(filename));
	memset(ac_no,0x00,sizeof(ac_no));
	memset(stmp,0x00,sizeof(stmp));
	memset(temp,0x00,sizeof(temp));
	memset(&sDd_mst,0x00,sizeof(sDd_mst));	
	memset(&sTd_mst,0x00,sizeof(sTd_mst));	
	memset(&sTd_parm,0x00,sizeof(sTd_parm));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));	
	
	pub_base_sysinit();
	
	get_zd_data("0300",ac_no);
	zip_space(ac_no);
	
	pub_base_AllDwnFilName(filename);
	vtcp_log("%d@%s filename[%s]", __LINE__, __FILE__, filename);

	fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no ='%s'", ac_no);
	if ( ret == 100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
		WRITEMSG
		    strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
		WRITEMSG
		    strcpy (g_pub_tx.reply,"W011");
		return 1;
	}
	ret = Dd_mst_Dec_Sel(g_pub_tx.reply,"ac_id ='%s'", ac_no);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"活期表中不存在该记录[%d]",ret);
			WRITEMSG
		    	strcpy (g_pub_tx.reply,"W010");
			return 1;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"SELECT dd_mst error,error code=[%d]",ret);
			WRITEMSG
		    	strcpy (g_pub_tx.reply,"W011");
			return 1;
		}
	while(1)
	{
		memset(&sDd_mst,0x00,sizeof(sDd_mst));
		ret = Dd_mst_Ret_Sel(g_pub_tx.reply, &sDd_mst);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"活期表查询结束[%d]",ret);
			WRITEMSG
		    	break;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"SELECT dd_mst error,error code=[%d]",ret);
			WRITEMSG
		    	strcpy (g_pub_tx.reply,"W011");
			return 1;
		}
		fprintf(fp,"%d|9|||%s|N|%.2lf|%d|%d|%s",sDd_mst.ac_seqn,sMdm_ac_rel.opn_br_no,sDd_mst.bal,sDd_mst.opn_date,sDd_mst.lst_date,sDd_mst.hold_sts);/**9代表活期**/
	}
	ret = Dd_mst_Clo_Sel();
	
	while(1)
	{
		memset(&sTd_mst,0x00,sizeof(sTd_mst));
		memset(&sTd_parm,0x00,sizeof(sTd_parm));
		ret = Td_mst_Ret_Sel(g_pub_tx.reply, &sTd_mst);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"定期表查询结束[%d]",ret);
			WRITEMSG
		    	break;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"SELECT td_mst error,error code=[%d]",ret);
			WRITEMSG
		    	strcpy (g_pub_tx.reply,"W011");
			return 1;
		}
		ret = Td_parm_Sel(g_pub_tx.reply,&sTd_parm,"prdt_no ='%s'", sTd_mst.prdt_no);
		if ( ret == 100 )
		{
			sprintf(acErrMsg,"定期产品表中不存在该记录[%d]",ret);
			WRITEMSG
		    	strcpy (g_pub_tx.reply,"W010");
			return 1;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"SELECT td_parm error,error code=[%d]",ret);
			WRITEMSG
		    	strcpy (g_pub_tx.reply,"W011");
			return 1;
		}
		memset(stmp,0x00,sizeof(stmp));
		memset(temp,0x00,sizeof(temp));
		sprintf(temp,"%d",sTd_parm.term);
		zip_space(temp);
		memcpy(stmp,temp,sizeof(temp));
		strcat(stmp,sTd_parm.term_type);
		zip_space(stmp);
		fprintf(fp,"%s|%s|%s|%.2lf|%s|%s|%.2lf|%d|%d|%s",sTd_mst.ac_seqn,sTd_mst.prdt_no,stmp,sTd_parm.dpst_min_amt,sMdm_ac_rel.opn_br_no,sTd_parm.tfr_lmt_ind,sDd_mst.bal,sDd_mst.opn_date,sDd_mst.lst_date,sDd_mst.hold_sts);/**9代表活期**/
	}
	ret = Dd_mst_Clo_Sel();
	
	
	fclose(fp);
OkExit:
	set_zd_data( DC_FILE_SND,"1" );
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}