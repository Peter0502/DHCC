/*************************************************
* 文 件 名:  sp1714.c
* 功能描述： 二级账户名修改
* 作    者:  zyhli0
* 完成日期： 2010-11-8 9:34:51
*
*insert into tx_def values ('1714','二级账户名修改','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100','0','5','0','3',null);
*insert into tx_flow_def values('1714',0,'1714','#$');
*insert into tx_sub_def values('1714','二级账户名修改','sp1714','0','0');
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
        sprintf( acErrMsg, "sql error" ); \
        WRITEMSG \
        goto ErrExit; \
        }
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "sub_dd_mst_c.h"

int sp1714()
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct sub_dd_mst_c sSub_dd_mst;
	char cAc_no[25];
	char cName[61];
	char cFlag[2];
	char cAc_seqn[9];
	char cPaper_type[1+1];
	char cPaper_no[20+1];
	char cTelephone[20+1];
	int ret=0;
	/* add begin by wzs 20120510 reason:增加小区字段 --*/
	long xq_num=0;
	long lp_num=0;
	long dy_num=0;
	long fj_num=0;
	/*-- add end by wzs 20120510 --*/

	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
	memset(cAc_no,0x00,sizeof(cAc_no));
	memset(cAc_seqn,0x00,sizeof(cAc_seqn));
	memset(cName,0x00,sizeof(cName));
	memset(cFlag,0x00,sizeof(cFlag));
	/*-- add begin by wzs 20120410 reason:增加字段 --*/
	memset(cPaper_type,0x00,sizeof(cPaper_type));
	memset(cPaper_no,0x00,sizeof(cPaper_no));
	memset(cTelephone,0x00,sizeof(cTelephone));
	/*-- add end by wzs 20120410 --*/

	get_zd_data("0300",cAc_no);
	pub_base_old_acno(cAc_no);
	get_zd_data("0780",cAc_seqn);
	get_zd_data("0250",cName);
	get_zd_data("0690",cFlag);
	/*-- add begin by wzs 20120410 reason:增加字段 --*/
	get_zd_data("0680",cPaper_type);
	get_zd_data("0620",cPaper_no);
	get_zd_data("0270",cTelephone);
	/*-- add end by wzs 20120410 --*/

	/* add begin by wzs 20120510 reason:增加小区字段 --*/
	get_zd_long("0530",&xq_num);
	get_zd_long("0540",&lp_num);
	get_zd_long("0550",&dy_num);
	get_zd_long("0470",&fj_num);
	/*-- add end by wzs 20120510 --*/

	pub_base_sysinit();

	vtcp_log(" %s, %d, ac_no[%s] ac_seqn[%s] name[%s] flag[%s] ",__FILE__,__LINE__,cAc_no,cAc_seqn,cName,cFlag);
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel," ac_no='%s' ",cAc_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"介质与账户关系对照表中无此账号[%s]记录!",cAc_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B117");
		goto ErrExit;
	}
	else if(ret)
	{
		sprintf(acErrMsg,"查询介质与账户关系对照表异常[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L182");
		goto ErrExit;
	}
	/* 检查行号 */
	if(memcmp(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no,sizeof(g_pub_tx.tx_br_no)))
	{
		sprintf(acErrMsg,"请输入本机构号账户，不能跨行!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"A039");
		goto ErrExit;
	}
	/* 是否开通二级账业务 */
	/* if(sMdm_ac_rel.kt_flag[0]!='1') */
	if(strcmp(sMdm_ac_rel.draw_pwd_chg,"1"))
	{
		sprintf(acErrMsg,"该账户没有开通二级账户业务!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"SN19");
		goto ErrExit;
	}

	if(cFlag[0]=='1')		/* 查询时只返回子账号户名 */
	{
		ret=Sub_dd_mst_Sel(g_pub_tx.reply,&sSub_dd_mst," ac_no='%s' and sub_ac_seqn=%ld ",cAc_no,atoi(cAc_seqn));
		if(ret==100)
		{
			sprintf(acErrMsg,"虚拟子帐号台帐表中无此账号[ac_no=%s sub_ac_seqn=%ld]记录!",cAc_no,atoi(cAc_seqn));
			WRITEMSG
			strcpy(g_pub_tx.reply,"SN12");
			goto ErrExit;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"查询虚拟子帐号台帐表异常!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		set_zd_data("0250",sSub_dd_mst.name);
		goto OkExit;
	}

	ret=Sub_dd_mst_Dec_Upd(g_pub_tx.reply," ac_no='%s' and sub_ac_seqn=%ld ",cAc_no,atoi(cAc_seqn));
	if(ret)
	{
		sprintf(acErrMsg,"DECLARE sub_dd_mst 异常[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}

	memset(&sSub_dd_mst,0x00,sizeof(sSub_dd_mst));
	ret=Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"FETCH sub_dd_mst 异常[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D102");
		goto ErrExit;
	}

	if(sSub_dd_mst.ac_sts[0]=='*')
	{
		sprintf(acErrMsg,"该子账号处于销户状态，不允许修改户名!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"SN11");
		goto ErrExit;
	}

	pub_base_strpack(cName);
	memcpy(sSub_dd_mst.name,cName,sizeof(sSub_dd_mst.name));
	/*-- add begin by wzs 20120410 reason:增加字段 --*/
	strncpy(sSub_dd_mst.paper_type,cPaper_type,sizeof(sSub_dd_mst.paper_type));
	strncpy(sSub_dd_mst.paper_no,cPaper_no,sizeof(sSub_dd_mst.paper_no));
	strncpy(sSub_dd_mst.telephone,cTelephone,sizeof(sSub_dd_mst.telephone));
	/*-- add end by wzs 20120410 --*/
	/* add begin by wzs 20120510 reason:增加小区字段 --*/
	sSub_dd_mst.xq_num=xq_num;
	sSub_dd_mst.lp_num=lp_num;
	sSub_dd_mst.dy_num=dy_num;
	sSub_dd_mst.fj_num=fj_num;
	/*-- add end by wzs 20120510 --*/
	Sub_dd_mst_Debug(&sSub_dd_mst);

	ret=Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"Update sub_dd_mst 异常[ret=%d]!",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D105");
		goto ErrExit;
	}

	vtcp_log(" %s, %d, 更新成功并登记流水",__FILE__,__LINE__);
	strcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no);
	g_pub_tx.ac_id=sSub_dd_mst.sub_ac_seqn;
   	/* 登记交易流水 */
   	if(pub_ins_trace_log())
   	{
   	  sprintf(acErrMsg,"登记交易流水错误!");
   	  WRITEMSG
   	  strcpy(g_pub_tx.reply,"P126");
   	  goto ErrExit;
   	}

	set_zd_data("0250",sSub_dd_mst.name);
	set_zd_data("0320",sSub_dd_mst.ac_no);
	set_zd_data("0330",sSub_dd_mst.sub_ac_no);
	set_zd_int("0780",sSub_dd_mst.sub_ac_seqn);

	Sub_dd_mst_Clo_Upd();

OkExit:
  sprintf(acErrMsg,"交易成功!");
  WRITEMSG
 	strcpy(g_pub_tx.reply,"0000");
  set_zd_data("0120",g_pub_tx.reply);
  return 0;

ErrExit:
  sprintf(acErrMsg,"交易失败![ %s ]",g_pub_tx.reply);
  WRITEMSG
  set_zd_data("0120",g_pub_tx.reply);
  return 1;
}
