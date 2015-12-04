/*************************************************
* 文 件 名: sp9592.c
* 功能描述：VTM活期、定期账户户名查询
* 作    者: 孙朋君
* 完成日期：2014年6月6日
* 修改记录：   
*     1. 日    期: 20141029
*        修 改 人: 刘增盈
*        修改内容: 只许查活期 不再查定期了
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}

#define EXTERN
#include "string.h"
#include "public.h"
#include "mo_loss_c.h"
#include "prdt_ac_id_c.h"


sp9592()
{
	int iRet = 0;
	int iAcSeqn = 0;
	char cAcNo[20];
	struct mo_loss_c stMo_loss;
	struct prdt_ac_id_c stPrdt_ac_id;
	
	memset(cAcNo, 0x00, sizeof(cAcNo));
	memset(&stMo_loss, 0x00, sizeof(struct mo_loss_c));
	memset(&stPrdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
	
	get_zd_data("1021", cAcNo);/**帐号**/
	pub_base_old_acno(cAcNo);/**旧账号转化**/
	
	iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", cAcNo);
	if (100 == iRet)
	{
		sprintf(acErrMsg, "介质帐号对照表不存在该记录!![%s]", cAcNo);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	}
	else if (iRet)
	{
		sprintf(acErrMsg, "查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	
	if (0 == memcmp(g_mdm_ac_rel.note_sts, "1", 1))
	{
		iRet = Mo_loss_Sel(g_pub_tx.reply, &stMo_loss, "ac_no=%s and unloss_ind='0'", cAcNo);
		if (100 == iRet)
		{
			sprintf(acErrMsg, "挂失登记簿中不存在该记录!![%s]", cAcNo);
			WRITEMSG
			strcpy(g_pub_tx.reply, "D104");
			goto ErrExit;
		}
		else if (iRet)
		{
			sprintf(acErrMsg, "查找挂失登记簿出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
		set_zd_data("0670", stMo_loss.loss_ind);
	}
	
	iRet = Prdt_ac_id_Sel(g_pub_tx.reply, &stPrdt_ac_id, "ac_id=%ld", g_mdm_ac_rel.ac_id);
	if (100 == iRet)
	{
		sprintf(acErrMsg, "产品帐号对照表不存在该记录!! [%d] [%ld]", iRet, g_mdm_ac_rel.ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
	}
	else if (iRet)
	{
		sprintf(acErrMsg, "查找帐号出错!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D103");
		goto ErrExit;
	}
	
		/*卡的情况*/
	if(9999 == g_mdm_ac_rel.ac_seqn)
	{
		iRet = sql_count("prdt_ac_id", "ac_id=%ld", g_mdm_ac_rel.ac_id);
		if (0 > iRet)
		{
			sprintf(acErrMsg, "sql_count error!! [%d]", iRet);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else if (0 == iRet)
		{
			sprintf(acErrMsg, "账号不存在!![%s]", cAcNo);
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else
		{
			iAcSeqn = stPrdt_ac_id.ac_seqn;
		}
	}
	else
	{
		iAcSeqn = g_mdm_ac_rel.ac_seqn;
	}
	
	if ('1' == stPrdt_ac_id.prdt_no[0])
	{
		iRet = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%d", stPrdt_ac_id.ac_id, iAcSeqn);
		if (100 == iRet)
		{
			sprintf(acErrMsg, "read dd_mst error");
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else if (iRet)
		{
			sprintf(acErrMsg, "查找帐号出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
		
		set_zd_data("0250", g_dd_mst.name);
	}
	/**else if('2' == stPrdt_ac_id.prdt_no[0])
	{
		iRet = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%d", stPrdt_ac_id.ac_id, iAcSeqn);
		if(100 == iRet)
		{
			sprintf(acErrMsg, "read dd_mst error");
			WRITEMSG
			strcpy(g_pub_tx.reply, "M003");
			goto ErrExit;
		}
		else if(iRet)
		{
			sprintf(acErrMsg, "查找帐号出错!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D103");
			goto ErrExit;
		}
		
		set_zd_data("0250", g_td_mst.name);
		
	}**/
	else
	{
		sprintf(acErrMsg, "非活期账号![%d]",iRet);
		WRITEMSG
		strcpy(g_pub_tx.reply, "O266");
		goto ErrExit;
	}
	
	set_zd_data("0220", g_mdm_ac_rel.note_sts);
	set_zd_data("0680", g_mdm_ac_rel.id_type);
	set_zd_data("0620", g_mdm_ac_rel.id_no);
	
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("1021", "");/*处理旧帐号*/
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}