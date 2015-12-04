/***************************************************************
* 文 件 名:  sp8320.c
* 功能描述:  亲子关联签约查询	
* 作    者:  lq

* 完成日期：    
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_qzgl_c.h"
#include "mo_qzgl_hst_c.h"

int sp8320()
{   	
    int			iRet=0;
    long		child_ac_id=0;
    long		part_ac_id=0;
    long		sel_ac_id=0;
    long		tmp_ac_id=0;
    
    char    child_ac_no[25];
    char    part_ac_no[25];
    char		tmp_ac_no[25];
    char    cFlag[2];
    
    struct    mo_qzgl_c				sMo_qzgl;
    struct    mdm_ac_rel_c		sMdm_ac_rel_1;
    struct    mdm_ac_rel_c		sMdm_ac_rel_2;
    
    memset(child_ac_no, 0x00, sizeof(child_ac_no));
    memset(part_ac_no,  0x00, sizeof(part_ac_no));
    memset(tmp_ac_no,  0x00, sizeof(tmp_ac_no));
    memset(cFlag,   0x00, sizeof(cFlag));
    memset(&sMo_qzgl,     0x00, sizeof(sMo_qzgl));
    memset(&sMdm_ac_rel_1, 0x00, sizeof(sMdm_ac_rel_1));
    memset(&sMdm_ac_rel_2, 0x00, sizeof(sMdm_ac_rel_2));
    
    pub_base_sysinit();
    get_zd_data("0300", child_ac_no);
    get_zd_data("0310", part_ac_no);
    get_zd_data("0680", cFlag);
    pub_base_strpack(child_ac_no);
    pub_base_strpack(part_ac_no);
    pub_base_old_acno(child_ac_no);	/**对旧帐号的处理**/
    pub_base_old_acno(part_ac_no);
    
    if(cFlag[0]=='1')
    {
	    /*根据儿童卡账号从表mdm_ac_rel中获取儿童卡ac_id*/
	    iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_1, "ac_no='%s'", child_ac_no);
	    if(iRet == 100)
	    {
	      vtcp_log("[%s][%d]无此卡号，请检查！！ac_no=[%s]", __FILE__, __LINE__, child_ac_no);
	    	strcpy(g_pub_tx.reply, "M003");
	    	goto ErrExit;
	    }
	    else if(iRet)
	    {
	    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
	    	strcpy(g_pub_tx.reply, "W011");
	    	goto ErrExit;	
	    }
	    child_ac_id=sMdm_ac_rel_1.ac_id;
	
	    
	    /*根据父母卡账号从表mdm_ac_rel中获取父母卡ac_id*/
	    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_no='%s'", part_ac_no);
	    if(iRet == 100)
	    {
	      vtcp_log("[%s][%d]无此卡号，请检查！！ac_no=[%s]", __FILE__, __LINE__, part_ac_no);
	    	strcpy(g_pub_tx.reply, "M003");
	    	goto ErrExit;
	    }
	    else if(iRet)
	    {
	    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
	    	strcpy(g_pub_tx.reply, "W011");
	    	goto ErrExit;	
	    }
	    part_ac_id=sMdm_ac_rel_2.ac_id;
	    
	    iRet = Mo_qzgl_Sel(g_pub_tx.reply, &sMo_qzgl, "child_ac_id=%ld and part_ac_id=%ld and sts='1'order by tx_date", child_ac_id,part_ac_id);
	    vtcp_log("%d,sMo_qzgl.sts=[%s]", iRet, sMo_qzgl.sts);
	    if(iRet != 100 && iRet != 0)
	    {
		    strcpy(g_pub_tx.reply, "D101");
		    sprintf( acErrMsg, "Mo_qzgl_Dec_Upd出错!!!" );
		    WRITEMSG
		    goto ErrExit;
	    }
	    else if(iRet == 100)
	    {
	    	int ret = 0;
	    	set_zd_data("0690", "0"); /**未签约**/
	    	
	    	/**家长卡最多关联三个金太阳卡**/
	    	ret = sql_count("mo_qzgl","part_ac_id=%ld and sts='1'",part_ac_id);
		    if(ret == 3)
		    {
		    	set_zd_data("0690", "2");
		    }
		  	
		  	/**金太阳卡只能关联一个家长卡**/
		    ret = sql_count("mo_qzgl","child_ac_id=%ld and sts='1'",child_ac_id);
		    if(ret > 0)
		    {
		    	set_zd_data("0690", "3");
		    }

	    }
	    else
	    {
	    	
		  	set_zd_data("0690", "1");
	    }
	  }
	  else if(cFlag[0]=='2')
	  {
	  	/*根据卡账号从表mdm_ac_rel中获取卡ac_id*/
	    iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_1, "ac_no='%s'", child_ac_no);
	    if(iRet == 100)
	    {
	      vtcp_log("[%s][%d]无此卡号，请检查！！ac_no=[%s]", __FILE__, __LINE__, child_ac_no);
	    	strcpy(g_pub_tx.reply, "M003");
	    	goto ErrExit;
	    }
	    else if(iRet)
	    {
	    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
	    	strcpy(g_pub_tx.reply, "W011");
	    	goto ErrExit;	
	    }
	    sel_ac_id=sMdm_ac_rel_1.ac_id;
	    
	    iRet = Mo_qzgl_Sel(g_pub_tx.reply, &sMo_qzgl, "child_ac_id=%ld or part_ac_id=%ld and sts='1'order by tx_date", sel_ac_id,sel_ac_id);
	    vtcp_log("%d,sMo_qzgl.sts=[%s]", iRet, sMo_qzgl.sts);
	    if(iRet != 100 && iRet != 0)
	    {
		    strcpy(g_pub_tx.reply, "D101");
		    sprintf( acErrMsg, "Mo_qzgl_Dec_Upd出错!!!" );
		    WRITEMSG
		    goto ErrExit;
	    }
	    if(iRet == 100)
	    {
	    	iRet = Mo_qzgl_Sel(g_pub_tx.reply, &sMo_qzgl, "child_ac_id=%ld or part_ac_id=%ld and sts='*'order by tx_date", sel_ac_id,sel_ac_id);
	      vtcp_log("%d,sMo_qzgl.sts=[%s]", iRet, sMo_qzgl.sts);
	      if(iRet != 100 && iRet != 0)
	      {
		    strcpy(g_pub_tx.reply, "D101");
		    sprintf( acErrMsg, "Mo_qzgl_Dec_Upd出错!!!" );
		    WRITEMSG
		    goto ErrExit;
	      }
	      if (iRet==0){
	    			set_zd_data("0690", "4");  /*已解约*/
	    	}
	    	 if(iRet == 100){
	    	 set_zd_data("0690", "0"); /**未签约**/
	       }
	    }
	  }
	    else
	    {
	    	/**若已签约，查询与其绑定账号，6223中需要返回此绑定账号**/
	    	vtcp_log("%d, sel_ac_id=[%ld],child_ac_id=[%ld],part_ac_id=[%ld]", iRet, sel_ac_id,sMo_qzgl.child_ac_id,sMo_qzgl.part_ac_id);
	    	if(sel_ac_id==sMo_qzgl.child_ac_id)
	    		tmp_ac_id = sMo_qzgl.part_ac_id;
	    	else if(sel_ac_id==sMo_qzgl.part_ac_id)
	    		tmp_ac_id = sMo_qzgl.child_ac_id;
	    	
	    	iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_id=%ld ", tmp_ac_id);
		    if(iRet == 100)
		    {
		      vtcp_log("[%s][%d]无此卡账户id，请检查！！ac_id=[%ld]", __FILE__, __LINE__, tmp_ac_id);
		    	strcpy(g_pub_tx.reply, "M003");
		    	goto ErrExit;
		    }
		    else if(iRet)
		    {
		    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
		    	strcpy(g_pub_tx.reply, "W011");
		    	goto ErrExit;	
		    }
		    strcpy(tmp_ac_no,sMdm_ac_rel_2.ac_no);
		    
		  	set_zd_data("0690", "1");			/**返回签约标志**/
		  	set_zd_long("0440", sMo_qzgl.tx_date);		/**6223中返回签约日期**/
		  	set_zd_long("0450", sMo_qzgl.untx_date);	/**6223中返回解约日期**/
		  	set_zd_data("0330", tmp_ac_no);
			set_zd_double("0390", sMo_qzgl.qy_amt); /**6223中返回签约金额**/   
	    }


OkExit:
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "success!!");
  WRITEMSG
  set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 0;
    
ErrExit:
  sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
  WRITEMSG
  if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply, "0000") == 0)
  {
  	strcpy(g_pub_tx.reply, "S035");
  }
  set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 1;
}
