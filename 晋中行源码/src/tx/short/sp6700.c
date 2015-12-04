/*************************************************
*公司活期帐号相关信息查询
* 文 件 名: sp6700.c
* 功能描述：根据活期帐号 返回客户姓名
* 作    者: ligl
* 完成日期：2006年09月13日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "td_mst_c.h"
#include "ln_mst_c.h"
#include "sub_dd_mst_c.h"

sp6700()
{
	int ret=0;
	char cAcno[25];
	char draw_pwd[7];
	char tx_code[6];
	struct mdm_ac_rel_c mdm_ac_rel;
	struct dd_mst_c dd_mst;
	struct prdt_ac_id_c prdt_ac_id;
	struct td_mst_c td_mst;
	struct ln_mst_c ln_mst;
	char cFlag[2];
	long  max_seqn=0;
	long  beg_seqn=0;
	int	  find_flag=0;
	struct sub_dd_mst_c  g_sub_dd_mst;
	memset(&mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&td_mst,0x00,sizeof(struct td_mst_c));
	memset(&ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(cAcno,0x00,sizeof(cAcno));
	memset(draw_pwd,0x00,sizeof(draw_pwd));
  memset(cFlag,0x00,sizeof(cFlag));
  memset(tx_code,0x00,sizeof(tx_code));
	memset(&g_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	/** 取值、赋值 **/
	get_zd_data( "0300",cAcno);	/* 账号*/
    pub_base_strpack(cAcno);
	 pub_base_old_acno(cAcno);
	 get_zd_data("0790",draw_pwd);  /* 密码*/
  get_zd_data("0670",cFlag);
  get_zd_data("0650",tx_code);
	pub_base_strpack(cAcno);
	pub_base_sysinit();       /*初始化*/

	pub_base_strpack(draw_pwd);
	if( strlen(draw_pwd) )     /*add by martin 20061214 密码校验*/
	{
	memcpy(g_pub_tx.draw_pwd,draw_pwd,sizeof(g_pub_tx.draw_pwd)-1); /* 密码*/
	pub_base_strpack( g_pub_tx.draw_pwd );
	/*vtcp_log("--[%s][%d]look if (里)g_pub_tx.draw_pwd=[%s]",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
	memcpy(g_pub_tx.ac_no,cAcno,sizeof(g_pub_tx.ac_no)-1);
	strcpy(g_mdm_ac_rel.draw_pwd_yn,"Y");
	ret = pub_base_check_draw("101"); /*** 调用校验支取方式、校验密码函数 ***/
	ERR_DEAL
       	sprintf(acErrMsg,"密码不正确!");
	WRITEMSG
	strcpy( g_pub_tx.reply,"M005" );
	goto ErrExit;
	}

        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no='%s'",cAcno);
	vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  if(ret==100)
  {
					vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
        	sprintf(acErrMsg,"介质帐号关系不存在该帐号!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P102" );
					goto ErrExit;
  }
  if(ret)
  {
					vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
        	sprintf(acErrMsg,"取介质帐号关系表出错!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W011" );
					goto ErrExit;
  }
	vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
/***销户也让查询, delete by wanglei 20061122
  if(mdm_ac_rel.note_sts[0]=='*')
  {
  				vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
        	sprintf(acErrMsg,"介质状态不正常!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P172" );
					goto ErrExit;
  }
***/
	vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id,"ac_id=%ld",mdm_ac_rel.ac_id);
	if(ret==100)
	{
					vtcp_log("--[%s][%d]look cAcno=[%ld]",__FILE__,__LINE__,mdm_ac_rel.ac_id);
         	sprintf(acErrMsg,"产品账号对照表中不存在该记录!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W012" );
					goto ErrExit;
	}
	if(ret)
	{
					vtcp_log("--[%s][%d]look cAcno=[%ld]",__FILE__,__LINE__,mdm_ac_rel.ac_id);
  				sprintf(acErrMsg,"查询产品账号对照表出错!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"W013" );
					goto ErrExit;
	}
	if(cFlag[0]!=prdt_ac_id.ac_id_type[0])
	{
					vtcp_log("--[%s][%d]look cAcno=[%ld]",__FILE__,__LINE__,mdm_ac_rel.ac_id);
         	sprintf(acErrMsg,"产品账号对照表中不存在该记录!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"T046");
					goto ErrExit;
	}
	if(cFlag[0]=='1')
	{
  		ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst,"ac_id=%ld",mdm_ac_rel.ac_id);
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		if(ret==100)
  		{
							vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		       	sprintf(acErrMsg,"活期主文件不存在该帐户ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  		}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		if(ret)
  		{
							vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  						sprintf(acErrMsg,"读活期主表错!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  		}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
			memset(&g_dd_mst,0x00,sizeof(g_dd_mst));
			memset(&g_dd_parm,0x00,sizeof(g_dd_parm));
			memcpy(&g_dd_mst,&dd_mst,sizeof(g_dd_mst));
			ret =Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",g_dd_mst.prdt_no);
			if(ret)
			{
				sprintf(acErrMsg, "查询活期产品主表错误!");
				WRITEMSG
				strcpy(g_pub_tx.reply, "W105");
				goto ErrExit;
			}
/***销户也让查询, delete by wanglei 20061122
  		if(dd_mst.ac_sts[0]=='*')
  		{
  		      	sprintf(acErrMsg,"该活期帐号状态为销户!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;
			}
***/
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,dd_mst.name);
			vtcp_log("--[%s][%d]look prdtno=[%s]",__FILE__,__LINE__,dd_mst.prdt_no);
  		set_zd_data("0250",dd_mst.name);
  		set_zd_data("0910",dd_mst.opn_br_no);
  		set_zd_data("0490",mdm_ac_rel.mdm_code);
  		set_zd_data("0580",mdm_ac_rel.note_no);
  		set_zd_data("0230",dd_mst.prdt_no);
  		set_zd_double("0390", dd_mst.bal);
	  	set_zd_long("0280",dd_mst.cif_no);
  		set_zd_data("0620",mdm_ac_rel.id_no);
  		set_zd_data("0720",mdm_ac_rel.id_type);
		/* 二级账户开户 */
		if(memcmp(tx_code,"2109",4)==0)
		{
			memset(&g_sub_dd_mst,0x00,sizeof(g_sub_dd_mst));
			/* 20120213 by Wang Yongwei 二级账户标志 */
			if(mdm_ac_rel.draw_pwd_chg[0]!='1')
			{
					vtcp_log(" %s, %d, 没有开通二级账户", __FILE__, __LINE__);
					strcpy(g_pub_tx.reply,"SN10");
					goto ErrExit;
			}
			ret = sql_max_long("sub_dd_mst","sub_ac_seqn",&max_seqn,"ac_no='%s'",mdm_ac_rel.ac_no);
			if(ret)
			{
					vtcp_log(" %s, %d, 没有开通二级账户", __FILE__, __LINE__);
					/* strcpy(g_pub_tx.reply,"SN10"); */
					goto ErrExit;
			}
			set_zd_long("0770",max_seqn+1);
			/* 二级账户标志 20120213 */
			set_zd_data("0880",mdm_ac_rel.draw_pwd_chg);

			ret =pub_base_getllz(g_dd_parm.rate_no, g_dd_parm.cur_no, g_pub_tx.tx_date, &g_dd_mst.rate);
			if(ret)
			{
				vtcp_log("%s,%d,取利率错误!",__FILE__,__LINE__);
				goto ErrExit;
			}
				set_zd_double("0840",g_dd_mst.rate);
		}

  }
  if(cFlag[0]=='2')
  {
  		ret=Td_mst_Sel(g_pub_tx.reply,&td_mst,"ac_id=%ld",mdm_ac_rel.ac_id);
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		if(ret==100)
  		{
							vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		       	sprintf(acErrMsg,"定期主文件不存在该帐户ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  		}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		if(ret)
  		{
							vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  						sprintf(acErrMsg,"读定期主表错!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  		}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
/***销户也让查询, delete by wanglei 20061122
  		if(td_mst.ac_sts[0]=='*')
  		{
  		      	sprintf(acErrMsg,"该活期帐号状态为销户!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;
			}
***/
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,td_mst.name);
  		set_zd_data("0250",td_mst.name);
  		set_zd_data("0910",td_mst.opn_br_no);
   		set_zd_data("0230",td_mst.prdt_no);

  }
  if(cFlag[0]=='3')
  {
  		ret=Ln_mst_Sel(g_pub_tx.reply,&ln_mst,"ac_id=%ld",mdm_ac_rel.ac_id);
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		if(ret==100)
  		{
							vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		       	sprintf(acErrMsg,"贷款主文件不存在该帐户ID!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M108" );
							goto ErrExit;
  		}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  		if(ret)
  		{
							vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
  						sprintf(acErrMsg,"读贷款主表错!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"W015" );
							goto ErrExit;
  		}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
/*销户也能查
  		if(ln_mst.ac_sts[0]=='*')
  		{
  		      	sprintf(acErrMsg,"该贷款帐号状态为销户!");
							WRITEMSG
							strcpy( g_pub_tx.reply,"M003" );
							goto ErrExit;
			}
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
			vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,ln_mst.name);
*/
  		set_zd_data("0250",ln_mst.name);
  		set_zd_data("0910",ln_mst.opn_br_no);
  		set_zd_data("0230",ln_mst.prdt_no);
  }
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


