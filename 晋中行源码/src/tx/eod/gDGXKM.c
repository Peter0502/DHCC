/*************************************************************
* 文 件 名: gDGXKM.c
* 功能描述：
* 作    者: cfq
* 完成日期: 20151111
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容: 
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ln_parm_c.h"
#include "dc_acc_rel_c.h"
#include "new_old_accno_c.h"
gDGXKM()
{
	int ret = 0;
	int i=0;
	struct ln_parm_c sLn_parm;
	struct dc_acc_rel_c sDc_acc_rel;
	struct new_old_accno_c sNew_old_accno;
	memset(&sLn_parm,0x00,sizeof(sLn_parm));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(&sNew_old_accno,0x00,sizeof(sNew_old_accno));
	ret = Ln_parm_Dec_Sel(g_pub_tx.reply, "1=1");	
	if(ret)
		{ 	
			sprintf( acErrMsg, "查询贷款产品表出错!ret=[%d]",ret );
			WRITEMSG
			goto ErrExit;
	  }
	while(1)
	{
		memset(&sLn_parm,0x00,sizeof(sLn_parm));
	  memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	  memset(&sNew_old_accno,0x00,sizeof(sNew_old_accno));
		ret = Ln_parm_Fet_Sel(&sLn_parm,g_pub_tx.reply);
		if(ret==100 )
			{ 	
				sprintf( acErrMsg, "查询贷款产品表出错!ret=[%d]",ret );
				WRITEMSG
				break;
		  }
	  ret= Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code = '%s' and data_code='0152'",sLn_parm.dc_code);
	  if(ret == 100)
    	{
    		sprintf(acErrMsg, "数据库无此记录!");
    		WRITEMSG
			  strcpy(g_pub_tx.reply, "D104");
			  goto ErrExit;
    	}
    else if(ret==0)	
    {	
	    ret= New_old_accno_Sel(g_pub_tx.reply,&sNew_old_accno,"old_acchrt='%s'",sDc_acc_rel.acc_hrt);
	    if(ret == 100)
	    	{
	    		sprintf(acErrMsg, "数据库无此记录!");
	    		WRITEMSG
				  strcpy(g_pub_tx.reply, "D104");
				  vtcp_log("[%s][%d]走了几遍[%d]",__FILE__,__LINE__,i++);
				  New_old_accno_Clo_Sel();
				  continue;
				  
	    	} 
     	else if(ret == 0)
       {
		    ret = Dc_acc_rel_Dec_Upd(g_pub_tx.reply, "dc_code='%s' and data_code='0155'",sLn_parm.dc_code);
				if (ret)
						{
							sprintf(acErrMsg, "取得会计代码对应科目错误![%d]", ret);
							WRITEMSG
							strcpy(g_pub_tx.reply, "S051");
							goto ErrExit;
						}
				ret = Dc_acc_rel_Fet_Upd(&sDc_acc_rel, g_pub_tx.reply);
				if (ret )
						{
							sprintf(acErrMsg, "取得会计代码对应科目错误!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "S051");
							goto ErrExit;
						}
				strcpy(sDc_acc_rel.acc_hrt,sNew_old_accno.new_ysx_acchrt);
				ret = Dc_acc_rel_Upd_Upd(sDc_acc_rel, g_pub_tx.reply);
				if (ret)
						{
							sprintf(acErrMsg, "修改会计代码表错误!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "H020");
							goto ErrExit;
						}	
						ret = Dc_acc_rel_Dec_Upd(g_pub_tx.reply, "dc_code='%s' and data_code='0156'",sLn_parm.dc_code);
				if (ret)
						{
							sprintf(acErrMsg, "取得会计代码对应科目错误![%d]", ret);
							WRITEMSG
							strcpy(g_pub_tx.reply, "S051");
							goto ErrExit;
						}
				ret = Dc_acc_rel_Fet_Upd(&sDc_acc_rel, g_pub_tx.reply);
				if (ret )
						{
							sprintf(acErrMsg, "取得会计代码对应科目错误!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "S051");
							goto ErrExit;
						}
				strcpy(sDc_acc_rel.acc_hrt,sNew_old_accno.new_bwx_acchrt);
				ret = Dc_acc_rel_Upd_Upd(sDc_acc_rel, g_pub_tx.reply);
				if (ret)
						{
							sprintf(acErrMsg, "修改会计代码表错误!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "H020");
							goto ErrExit;
						}
						ret = Dc_acc_rel_Dec_Upd(g_pub_tx.reply, "dc_code='%s' and data_code='0158'",sLn_parm.dc_code);
				if (ret)
						{
							sprintf(acErrMsg, "取得会计代码对应科目错误![%d]", ret);
							WRITEMSG
							strcpy(g_pub_tx.reply, "S051");
							goto ErrExit;
						}
				ret = Dc_acc_rel_Fet_Upd(&sDc_acc_rel, g_pub_tx.reply);
				if (ret )
						{
							sprintf(acErrMsg, "取得会计代码对应科目错误!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "S051");
							goto ErrExit;
						}
				strcpy(sDc_acc_rel.acc_hrt,sNew_old_accno.new_lxsr_acchrt);
				ret = Dc_acc_rel_Upd_Upd(sDc_acc_rel, g_pub_tx.reply);
				if (ret)
						{
							sprintf(acErrMsg, "修改会计代码表错误!");
							WRITEMSG
							strcpy(g_pub_tx.reply, "H020");
							goto ErrExit;
						}	     
		    continue; 
	     }
	  }
	}
	Dc_acc_rel_Clo_Upd();

OkExit:
    sql_commit();/*--事务提交--*/
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120", g_pub_tx.reply);
    return 0;


ErrExit:
    sql_rollback();        /*--事务回滚--*/
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    if(memcmp(g_pub_tx.reply, "0000",4)==0)
    memcpy(g_pub_tx.reply,"D045",4);
    set_zd_data("0120", g_pub_tx.reply);
    return 1;
}