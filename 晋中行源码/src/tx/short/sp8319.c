/***************************************************************
* 文 件 名:  sp8319.c
* 功能描述:  亲子关联明细查询	
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

int sp8319()
{   	
    int			iRet=0;
    long		lBegin_date=0;
    long		lEnd_date=0;
    long		tmp_ac_id=0;
    long		sel_ac_id=0;
    
    char    ac_no[25];
    char		tmp_ac_no[25];
    char    sTmp[1024];
    char		tmp_stat[7];
    char    sFilename[1024];
    char    sWherelist[1024];
    FILE *  fp =NULL;
    
    struct    mo_qzgl_c				sMo_qzgl;
    struct    mo_qzgl_hst_c		sMo_qzgl_hst;
    struct    mdm_ac_rel_c		sMdm_ac_rel_1;
    struct    mdm_ac_rel_c		sMdm_ac_rel_2;
    
    memset(ac_no, 			  0x00, sizeof(ac_no));
    memset(tmp_ac_no, 	  0x00, sizeof(tmp_ac_no));
    memset(tmp_stat,  	  0x00, sizeof(tmp_stat));
    memset(sWherelist,    0x00, sizeof(sWherelist));
    memset(sFilename,     0x00, sizeof(sFilename));
    memset(&sMo_qzgl,     0x00, sizeof(sMo_qzgl));
    memset(&sMdm_ac_rel_1,0x00, sizeof(sMdm_ac_rel_1));
    memset(&sMdm_ac_rel_2,0x00, sizeof(sMdm_ac_rel_2));
    
    pub_base_sysinit();
    get_zd_data("0300", ac_no);
    /*
    get_zd_long("0440", &lBegin_date);
    get_zd_long("0450", &lEnd_date);
    */
    pub_base_strpack(ac_no);
    pub_base_old_acno(ac_no);	/**对旧帐号的处理**/
		
    /*根据卡账号从表mdm_ac_rel中获取卡ac_id*/
    iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_1, "ac_no='%s'", ac_no);
    if(iRet == 100)
    {
      vtcp_log("[%s][%d]无此卡号，请检查！！ac_no=[%s]", __FILE__, __LINE__, ac_no);
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
    
    /* 生成下传全路经文件名(批量) */
    pub_base_AllDwnFilName(sFilename);
    fp = fopen(sFilename, "w");
    if (fp == NULL) 
    {
    	sprintf(acErrMsg, "open file error [%s]", sFilename);
			WRITEMSG
			strcpy(g_pub_tx.reply, "S047");
			goto ErrExit;
    }
    
    fprintf(fp, "~@金太阳卡账号|@家长卡账号|@签约日期|@签约转账日期|@签约金额|@解约日期|@关联状态|\n");
    
    sprintf(sTmp, "(child_ac_id=%ld or part_ac_id=%ld) and ", sel_ac_id,sel_ac_id);
    strcat(sWherelist, sTmp);
    /*
    if(lBegin_date > 0)
    {
    	memset(sTmp, 0x00, sizeof(sTmp));
    	sprintf(sTmp, "tx_date>=%ld and ", lBegin_date);
    	strcat(sWherelist, sTmp);
    }
    if(lEnd_date > 0)
    {
    	memset(sTmp, 0x00, sizeof(sTmp));
      sprintf(sTmp, "tx_date<=%ld and ", lEnd_date);
    	strcat(sWherelist, sTmp);
    }
    */
    strcat(sWherelist, " 1=1 order by tx_date");
     
    iRet = Mo_qzgl_Dec_Sel(g_pub_tx.reply, sWherelist);
    if(iRet)
    {
      strcpy(g_pub_tx.reply, "D101");
      sprintf( acErrMsg, "Mo_qzgl_Sel出错!!!" );
      WRITEMSG
      goto ErrExit;
    }
    while(1)
    {
    	memset(&sMo_qzgl, 0x00, sizeof(sMo_qzgl));
    	
      iRet = Mo_qzgl_Fet_Sel(&sMo_qzgl, g_pub_tx.reply);
      if(iRet == 100)
      {
      	break;	
      }
      else if(iRet)
      {
	      strcpy(g_pub_tx.reply, "D102");
	      sprintf( acErrMsg, "Mo_qzgl_Sel出错!!!" );
	      WRITEMSG
	      goto ErrExit;
      }
      
      if(sMo_qzgl.sts[0]=='1')
      	strcpy(tmp_stat,"已签约");
      else if(sMo_qzgl.sts[0]=='*')
      	strcpy(tmp_stat,"已解约");
      
      if(sel_ac_id==sMo_qzgl.child_ac_id)
      {
	    	tmp_ac_id = sMo_qzgl.part_ac_id;
	    	
	    	iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_id=%ld", tmp_ac_id);
		    if(iRet == 100)
		    {
		      vtcp_log("[%s][%d]无此卡id，请检查！！ac_id=[%ld]", __FILE__, __LINE__, tmp_ac_id);
		    	strcpy(g_pub_tx.reply, "M003");
		    	goto ErrExit;
		    }
		    else if(iRet)
		    {
		    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
		    	strcpy(g_pub_tx.reply, "W011");
		    	goto ErrExit;	
		    }
	    	strcpy(tmp_ac_no, sMdm_ac_rel_2.ac_no);
	    	
	    	fprintf(fp, "%s|%s|%ld|%ld|%-.2f|%ld|%s|\n", ac_no, tmp_ac_no,sMo_qzgl.tx_date,sMo_qzgl.hz_date,sMo_qzgl.qy_amt,sMo_qzgl.untx_date,tmp_stat);
	    }
	    else if(sel_ac_id==sMo_qzgl.part_ac_id)
	    {
	    	tmp_ac_id = sMo_qzgl.child_ac_id;
	    	
	    	iRet =Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_2, "ac_id=%ld", tmp_ac_id);
		    if(iRet == 100)
		    {
		      vtcp_log("[%s][%d]无此卡id，请检查！！ac_id=[%ld]", __FILE__, __LINE__, tmp_ac_id);
		    	strcpy(g_pub_tx.reply, "M003");
		    	goto ErrExit;
		    }
		    else if(iRet)
		    {
		    	vtcp_log("[%s][%d]，iRet=[%d]", __FILE__, __LINE__, iRet);
		    	strcpy(g_pub_tx.reply, "W011");
		    	goto ErrExit;	
		    }
	    	strcpy(tmp_ac_no, sMdm_ac_rel_2.ac_no);
	    	
	    	fprintf(fp, "%s|%s|%ld|%ld|%-.2f|%ld|%s|\n", tmp_ac_no, ac_no,sMo_qzgl.tx_date,sMo_qzgl.hz_date,sMo_qzgl.qy_amt,sMo_qzgl.untx_date,tmp_stat);
	    }
    }
    Mo_qzgl_Clo_Sel();
   
    
    fclose(fp);

OkExit:
    strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "success!!");
    WRITEMSG
    set_zd_data(DC_FILE_SND, "1");
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
    
ErrExit:
	if(NULL != fp)
	{
		fclose(fp);	
	}	
    sprintf(acErrMsg, "failed !! reply is[%s]", g_pub_tx.reply);
    WRITEMSG
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply, "0000") == 0)
    {
         strcpy(g_pub_tx.reply, "S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;

}
