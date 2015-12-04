/*************************************************
* 文 件 名:    spC709.c
* 功能描述：   完成查询IC卡内其他账号信息的功能。
* 作    者:    wzw
* 完成日期：   2011年12月12日
insert into tx_def      values('3709','IC卡账户信息查询','00000000001100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100','0','4','0');
insert into tx_flow_def values('3709',0,'C709','#$');
insert into tx_sub_def  values('C709','IC卡账户信息查询','spC709',0,0);
*************************************************/
#include "public.h"
#define  EXTERN
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "ic_subac_parm_c.h"
#include "mo_ic_ac_rel_c.h"

spC709()
{
	int     ret=0;
	int     ttlnum=0;
	FILE    *fp=NULL;
	char    cAcno[21];
	char    cName[41];
	int     sub_seqn=0;
	char    filename[100];
	char    wherelist[1024];
	
	struct  mdm_ac_rel_c      sMdm_ac_rel;
	struct  dd_mst_c          sDd_mst;
	struct  ic_subac_parm_c   sIc_subac_parm;
	struct  mo_ic_ac_rel_c    sMo_ic_ac_rel;
	
	memset(cAcno,             0x00, sizeof(cAcno));
	memset(cName,             0x00, sizeof(cName));
	memset(filename,          0x00, sizeof(filename));
	memset(wherelist,         0x00, sizeof(wherelist));
	
	memset(&sMdm_ac_rel,      0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sDd_mst,          0x00, sizeof(struct dd_mst_c));
	memset(&sIc_subac_parm,   0x00, sizeof(struct ic_subac_parm_c));
	memset(&sMo_ic_ac_rel,    0x00, sizeof(struct mo_ic_ac_rel_c));
 
	/** 初始化全局变量 **/
	pub_base_sysinit();
  
	/*取值，赋值*/
	get_zd_data("0300",cAcno);
	pub_base_strpack(cAcno);
	pub_base_old_acno(cAcno);
	get_zd_data("0250",cName);
	pub_base_strpack(cName);
	get_zd_int("0480",&sub_seqn);
	
	sprintf(wherelist,"sts='1'");
	if(strlen(cAcno)>0)
	{
		sprintf(wherelist+strlen(wherelist)," and main_ac_no='%s'", cAcno);
	}
	if(sub_seqn>0)
	{
		sprintf(wherelist+strlen(wherelist)," and sub_seqn=%d", sub_seqn);
	}
	sprintf(wherelist+strlen(wherelist),"  order by sub_seqn");
	
	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName(filename);
  
	fp = fopen(filename, "w");
	if (fp == NULL) 
	{
		sprintf(acErrMsg, "open file error [%s]", filename);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	fprintf(fp, "~@IC卡号|开户机构|客户名称|业务序号|业务描述|电子现金账号|开户日期|$电子账户金额|\n");

	ret=Mo_ic_ac_rel_Dec_Sel(g_pub_tx.reply, wherelist);
	if(ret)
	{
		sprintf(acErrMsg,"定义游标mo_ic_ac_rel出错!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		memset(&sMdm_ac_rel,      0x00, sizeof(struct mdm_ac_rel_c));
		memset(&sDd_mst,          0x00, sizeof(struct dd_mst_c));
		memset(&sMo_ic_ac_rel,    0x00, sizeof(struct mo_ic_ac_rel_c));
		memset(&sIc_subac_parm,   0x00, sizeof(struct ic_subac_parm_c));
		ret=Mo_ic_ac_rel_Fet_Sel(&sMo_ic_ac_rel, g_pub_tx.reply);
		if(ret==100)
			break;
		else if(ret)
		{
			sprintf(acErrMsg,"Fetch mo_ic_ac_rel error!ret=[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}

		ret=Ic_subac_parm_Sel(g_pub_tx.reply, &sIc_subac_parm, "sub_seqn=%d and sts='1'", sMo_ic_ac_rel.sub_seqn);
		if(ret)
		{
			sprintf(acErrMsg,"查询ic_subac_parm表出错!ret=[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", sMo_ic_ac_rel.app_ac_no);
		if(ret)
		{
			sprintf(acErrMsg,"查询mdm_ac_rel表出错!ret=[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}		
		ret=Dd_mst_Sel(g_pub_tx.reply, &sDd_mst, "ac_id=%ld ", sMdm_ac_rel.ac_id);
		if(ret)
		{
			sprintf(acErrMsg,"查询dd_mst表出错!ret=[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		
		fprintf(fp, "%20s|%5s|%40s|%04d|%30s|%20s|%ld|%17.2f|\n", cAcno, sMdm_ac_rel.opn_br_no, cName, sMo_ic_ac_rel.sub_seqn, sIc_subac_parm.sub_name, sMo_ic_ac_rel.app_ac_no, sMo_ic_ac_rel.tx_date, sDd_mst.bal);
		ttlnum++;
	}
	Mo_ic_ac_rel_Clo_Sel();
	
	if(!ttlnum)
	{
		sprintf(acErrMsg,"无符合的记录!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}
	fclose(fp);
	set_zd_data(DC_FILE_SND, "1");
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;	
}
