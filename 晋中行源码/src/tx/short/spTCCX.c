/*************************************************
* 文 件 名:    spTCCX.c
* 功能描述：   根据文件内容读取卡号，查询结果写入文件
*
* 作    者:    zgf
* 完成日期：   2014年06月24日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
* insert into tx_def values('6231','批量账户信息查询','10000000000000000000000011100100000000010000010000000000000001000100000000000000110000000000000000000000000000000000000000000000','0','5','0');
* insert into tx_flow_def values('6231',0,'6231','#$');
* insert into tx_sub_def values('6231','批量账户信息查询','spTCCX','0','1');
*************************************************/
#include "string.h"
#include <errno.h>
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"

spTCCX()
{ 		
	int	ret = 0;
	int	errflag = 0;
	FILE 	*fpr = NULL;
	FILE 	*fpw = NULL;
	char	tmp[128];
	char	line[1024];
	char	accno[128];
	char 	filename[128];
	struct dd_mst_c	stDd_mst;
	struct mdm_ac_rel_c	stMdm_ac_rel;
	
	memset(tmp, 0x0, sizeof(tmp));
	memset(line, 0x0, sizeof(line));
	memset(accno, 0x0, sizeof(accno));
	memset(filename, 0x0, sizeof(filename));
	memset(&stDd_mst, 0x0, sizeof(stDd_mst));
	memset(&stMdm_ac_rel, 0x0, sizeof(stMdm_ac_rel));

	get_fd_data("0250", filename);
	vtcp_log("[%s][%d] filename=[%s]", __FILE__, __LINE__, filename);
	pub_base_strpack(filename);
	sprintf(tmp, "%s/%s", getenv("FILDIR"), filename);
	fpr = fopen(tmp, "r");
	if (fpr == NULL)
	{
		vtcp_log("[%s][%d] Can not open file [%s]! errno=[%d]:[%s]",
			__FILE__, __LINE__, tmp, errno, strerror(errno));
		strcpy(g_pub_tx.reply, "S047");
		goto ErrExit;
	}	
	
	memset(filename, 0x0, sizeof(filename));
	pub_base_AllDwnFilName(filename);
	vtcp_log("[%s][%d] Send filename=[%s]", __FILE__, __LINE__, filename);
	fpw = fopen(filename, "w");
	if (fpw == NULL)
	{
		vtcp_log("[%s][%d] Can not open file [%s]! errno=[%d]:[%s]",
			__FILE__, __LINE__, filename, errno, strerror(errno));
		strcpy(g_pub_tx.reply, "S047");
		fclose(fpr);
		goto ErrExit;
	}
	
	while (!feof(fpr))
	{
		memset(line, 0x0, sizeof(line));
		if (fgets(line, sizeof(line), fpr) == NULL)
		{
			break;
		}

		vtcp_log("[%s][%d] line=[%s]", __FILE__, __LINE__, line);
		if (strcmp(line, "END") == 0)
		{
			break;
		}
		pub_base_readfield(accno, line, '|' );
		
		errflag = 0;
		memset(&stMdm_ac_rel, 0x0, sizeof(stMdm_ac_rel));
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &stMdm_ac_rel, "ac_no = '%s'", accno);
		if (ret)
		{
			vtcp_log("[%s][%d] Fetch mdm_ac_rel error! ret=[%d]", __FILE__, __LINE__, ret);
			errflag = 1;
		}
		
		if (errflag == 0)
		{
			memset(&stDd_mst, 0x0, sizeof(stDd_mst));
			ret = Dd_mst_Sel(g_pub_tx.reply, &stDd_mst, "ac_id = %ld", stMdm_ac_rel.ac_id);
			if (ret)
			{
				vtcp_log("[%s][%d] Fetch dd_msg error! ret=[%d]", __FILE__, __LINE__, ret);
				errflag = 1;
			}
		}
		if (errflag == 1)
		{
			fprintf(fpw, "%-20s|%-5s|%-30s|%-1s|\n", stMdm_ac_rel.ac_no, stDd_mst.opn_br_no, stDd_mst.name, "E");
		}
		else
		{
			fprintf(fpw, "%-20s|%-5s|%-30s|%-1s|\n", stMdm_ac_rel.ac_no, stDd_mst.opn_br_no, stDd_mst.name, stDd_mst.ac_sts);
		}
	}
	fclose(fpr);
	fclose(fpw);
	
	set_zd_data( DC_FILE_SND,"2" );

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

