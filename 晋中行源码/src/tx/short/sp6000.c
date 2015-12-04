/*************************************************
* 文 件 名: sp6000.c
* 功能描述：根据原帐号 返回补换卡后的新账户
* 作    者: 
* 完成日期：2012年07月09日
*************************************************/
#define  ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

sp6000()
{
	int ret=0;
	char cAcno[25];
	struct mdm_ac_rel_c mdm_ac_rel;
	struct mdm_ac_rel_c mdm_ac_rel1;
	struct dd_mst_c dd_mst;
	
	memset(&mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&mdm_ac_rel1, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(cAcno,0x00,sizeof(cAcno));
	/** 取值、赋值 **/
	get_zd_data( "0300",cAcno);	/* 账号*/
	pub_base_sysinit();       /*初始化*/


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

  if(mdm_ac_rel.note_sts[0]!='3')
  {
  				vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);
        	sprintf(acErrMsg,"该账户未作补换卡!");
					WRITEMSG
					strcpy( g_pub_tx.reply,"P101" );
					goto ErrExit;
  }

	vtcp_log("--[%s][%d]look cAcno=[%s]",__FILE__,__LINE__,cAcno);

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
			/**返回来查询补换卡的新卡号**/
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel1,"ac_id=%ld and note_sts='0'",dd_mst.ac_id);
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
			set_zd_data("0310",mdm_ac_rel1.ac_no);
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


