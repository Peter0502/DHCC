/*************************************************
* 文 件 名:  spB376.c
* 功能描述： 保证金账户和结息账户关系维护功能。
*
* 作    者:  zqbo
* 完成日期： 2013年03月04日
*
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
insert into TX_DEF values ('4376', '保证金与结息账户关联', '00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', '1', '6', '2');
insert into TX_SUB_DEF values ('B376', '保证金与结息账户关联', 'spB376', '0', '0');
insert into TX_FLOW_DEF values ('4376', 0, 'B376', '#$');
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "bzj_js_ac_rel_c.h"

int spB376()
{
    int iRet = 0;
    char cOprateNo[2];
    char sBzj_acno[24];
    char sJs_acno[24];
    char cTmpFiller[60];
    struct mdm_ac_rel_c sMdm_ac_rel_bzj;
    struct mdm_ac_rel_c sMdm_ac_rel_js;
    struct dd_mst_c     sDd_mst_bzj;
    struct dd_mst_c     sDd_mst_js;
    struct bzj_js_ac_rel_c sBzj_js_ac_rel;
    
    /* 数据初始化 */
    memset( sBzj_acno,        0x00, sizeof(sBzj_acno) );
    memset( sJs_acno,         0x00, sizeof(sJs_acno) );
    memset( cTmpFiller,       0x00, sizeof(cTmpFiller) );
    memset( &sMdm_ac_rel_bzj, 0x00, sizeof(struct mdm_ac_rel_c) );
    memset( &sMdm_ac_rel_js,  0x00, sizeof(struct mdm_ac_rel_c) );
    memset( &sDd_mst_bzj,     0x00, sizeof(struct dd_mst_c) );
    memset( &sDd_mst_js,      0x00, sizeof(struct dd_mst_c) );
    memset( &sBzj_js_ac_rel,  0x00, sizeof(struct bzj_js_ac_rel_c) );
    
    /* 从前台屏幕取值 */
    get_zd_data("0670", cOprateNo);    /*操作码 0-增加、1-删除、2-修改*/
    get_zd_data("0300", sBzj_acno);    /*保证金账号*/
    get_zd_data("0310", sJs_acno);     /*结算账号*/
    get_zd_data("0810", cTmpFiller);  /*备注*/
    
    /** 查找旧帐号 **/
    pub_base_old_acno(sBzj_acno);
    pub_base_old_acno(sJs_acno);
    
    /** 开始检查 保证金账号 **/
    iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_bzj, " ac_no='%s' ", sBzj_acno);
    if (iRet == 100)
    {
        sprintf(acErrMsg, "不存在该帐号!!ac_no=[%s]", sBzj_acno);
        WRITEMSG
        strcpy(g_pub_tx.reply, "M003");
        goto ErrExit;
    }
    else if (iRet)
    {
        sprintf(acErrMsg, "查询介质帐号表异常!!ret=[%d]", iRet);
        WRITEMSG
        goto ErrExit;
    }
    iRet = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst_bzj, " ac_id=%ld ", sMdm_ac_rel_bzj.ac_id);
    if (iRet)
    {
    	  sprintf(acErrMsg, "查询活期帐号出错!!ac_id=[%ld]", sMdm_ac_rel_bzj.ac_id);
        WRITEMSG
        strcpy(g_pub_tx.reply, "W050");
        goto ErrExit;
    }
    /* 检查是否为保证金账户 */
    if(memcmp(sDd_mst_bzj.prdt_no,"131",3) && memcmp(sDd_mst_bzj.prdt_no,"142",3) &&
    	 memcmp(sDd_mst_bzj.prdt_no,"132",3) && memcmp(sDd_mst_bzj.prdt_no,"134",3) &&
    	 memcmp(sDd_mst_bzj.prdt_no,"144",3) )
	  {
		    sprintf( acErrMsg, "非保证金账户不允许用此交易![%d][%s]",iRet,sBzj_acno);
		    WRITEMSG
		    sprintf( g_pub_tx.reply, "B202" );
		    goto ErrExit;
	  }
	  /* 检查交易机构是否为保证金开户机构 */
	  if ( strcmp(g_pub_tx.tx_br_no, sDd_mst_bzj.opn_br_no) )
	  {
	  	   sprintf(acErrMsg, "交易机构与开户机构不为同一个机构!!");
         WRITEMSG
         strcpy(g_pub_tx.reply, "B370");
         goto ErrExit;
	  }
	  /* 检查保证金账户的冻结状态 */
    if (sDd_mst_bzj.hold_sts[0] == '1' || sDd_mst_bzj.hold_sts[0] == '2') /*结算账号1-完全冻结,2-只进不出 报错*/
    {
    	  sprintf(acErrMsg, "保证金账户状态出错!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O085");
        goto ErrExit;
    }
    /* 检查保证金账户的状态 1-正常 */
    if (sDd_mst_bzj.ac_sts[0] != '1')
    {
    	  sprintf(acErrMsg, "保证金账户状态出错!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "O085");
        goto ErrExit;
    }
    
    /** 开始检查 结算账号, 操作为1-删 不需要检查**/
    if (cOprateNo[0] == '0' || cOprateNo[0] == '2')
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel_js, " ac_no='%s' ", sJs_acno);
        if (iRet == 100)
        {
            sprintf(acErrMsg, "不存在该帐号!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "M003");
            goto ErrExit;
        }
        else if (iRet)
        {
            sprintf(acErrMsg, "查询介质帐号表异常!!");
            WRITEMSG
            goto ErrExit;
        }
        iRet = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst_js, " ac_id=%ld ", sMdm_ac_rel_js.ac_id);
        if (iRet)
        {
        	  sprintf(acErrMsg, "查询活期帐号出错!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "W050");
            goto ErrExit;
        }
        /* 检查结算账户的冻结状态 */
        if (sDd_mst_js.hold_sts[0] == '1' || sDd_mst_js.hold_sts[0] == '2') /*结算账号1-完全冻结,2-只进不出 报错*/
        {
        	  sprintf(acErrMsg, "结算账户状态出错!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "O085");
            goto ErrExit;
        }
        /* 检查结算账户的状态 1-正常 */
        if (sDd_mst_js.ac_sts[0] != '1')
        {
        	  sprintf(acErrMsg, "结算账户状态出错!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "O085");
            goto ErrExit;
        }
        /**  判断保证金账户和结算账户是否为同一客户id **/
       /* if (sDd_mst_bzj.cif_no != sDd_mst_js.cif_no)
        {
        	  sprintf(acErrMsg, "保证金账户和结算账户应为同一客户!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B371");
            goto ErrExit;
        }*/
    }
    
    /*****
    *
    *  检查完毕
    *  开始1-增、2-删、3-改
    *
    *******/	
    if ( cOprateNo[0] == '0' ) /** 增加 **/
    {
    	 iRet = sql_count("bzj_js_ac_rel", " bzj_ac_id=%ld and sts='1' ", sMdm_ac_rel_bzj.ac_id);
    	 if (iRet > 0)
    	 {
    	 	  sprintf(acErrMsg, "bzj_js_ac_rel表中已经存在记录!!");
          WRITEMSG
          goto ErrExit;
    	 }
    	 strcpy(g_pub_tx.brf, "增加关联");
    	 strcpy(sBzj_js_ac_rel.tx_br_no, g_pub_tx.tx_br_no);    /*交易机构号*/
    	 sBzj_js_ac_rel.bzj_ac_id = sMdm_ac_rel_bzj.ac_id;      /*保证金账号ac_id*/
    	 sBzj_js_ac_rel.bzj_ac_seqn = sMdm_ac_rel_bzj.ac_seqn;  /*保证金账号序号*/
    	 sBzj_js_ac_rel.js_ac_id = sMdm_ac_rel_js.ac_id;        /*结算账号ac_id*/
    	 sBzj_js_ac_rel.js_ac_seqn = sMdm_ac_rel_js.ac_seqn;    /*结算账号序号*/
    	 sBzj_js_ac_rel.tx_date = g_pub_tx.tx_date;             /*登记日期*/
    	 sBzj_js_ac_rel.trace_no = g_pub_tx.trace_no;           /*登记流水号*/
    	 get_zd_data("0070", sBzj_js_ac_rel.tel);               /*操作柜员号*/
    	 sBzj_js_ac_rel.sts[0] = '1';                           /*状态1-正常*/
    	 if(strlen(cTmpFiller) > 0)
    	 {                          
    	     strcpy(sBzj_js_ac_rel.filler, cTmpFiller);         /*备注*/
    	 }
    	 iRet = Bzj_js_ac_rel_Ins(sBzj_js_ac_rel, g_pub_tx.reply);
    	 if (iRet)
    	 {
    	    sprintf( acErrMsg, "插入数据表Bzj_js_ac_rel_Ins出错!" );
		      WRITEMSG
		      sprintf( g_pub_tx.reply, "D108" );
		      goto ErrExit;
    	 }
    }
    else if ( cOprateNo[0] == '1' || cOprateNo[0] == '2' ) /** 删除或修改 **/
    {
    	 iRet = Bzj_js_ac_rel_Dec_Upd(g_pub_tx.reply, " bzj_ac_id=%ld and sts='1' ", sMdm_ac_rel_bzj.ac_id);
    	 if (iRet)
    	 {
    	 	  sprintf( acErrMsg, "更新数据表Bzj_js_ac_rel_Dec_Upd出错!" );
		      WRITEMSG
		      sprintf( g_pub_tx.reply, "D105" );
		      goto ErrExit;
    	     
    	 }
    	 iRet = Bzj_js_ac_rel_Fet_Upd(&sBzj_js_ac_rel, g_pub_tx.reply);
    	 if (iRet == 100)
    	 {
    	    sprintf( acErrMsg, "Bzj_js_ac_rel表中无此记录!" );
		      WRITEMSG
		      sprintf( g_pub_tx.reply, "D104" );
		      goto ErrExit;
    	 }
    	 else if (iRet)
    	 {
    	 	  sprintf( acErrMsg, "更新数据表Bzj_js_ac_rel_Fet_Upd出错!" );
		      WRITEMSG
		      sprintf( g_pub_tx.reply, "D105" );
		      goto ErrExit;
    	 }
    	  
    	 if (cOprateNo[0] == '1') /** 删除 **/
    	 {
    	     strcpy(g_pub_tx.brf, "删除关联");
    	     sBzj_js_ac_rel.sts[0] = '*';
    	     
    	     Bzj_js_ac_rel_Upd_Upd(sBzj_js_ac_rel, g_pub_tx.reply);
    	 }
    	 else                    /**修改**/
    	 {
    	     strcpy(g_pub_tx.brf, "修改关联");
    	     sBzj_js_ac_rel.js_ac_id = sMdm_ac_rel_js.ac_id;        /*结算账号ac_id*/
    	     sBzj_js_ac_rel.js_ac_seqn = sMdm_ac_rel_js.ac_seqn;    /*结算账号序号*/
    	     sBzj_js_ac_rel.tx_date = g_pub_tx.tx_date;             /*登记日期*/
    	     sBzj_js_ac_rel.trace_no = g_pub_tx.trace_no;           /*登记流水号*/
    	     get_zd_data("0070", sBzj_js_ac_rel.tel);               /*操作柜员号*/
    	     if(strlen(cTmpFiller) > 0)
    	     {                          
    	         strcpy(sBzj_js_ac_rel.filler, cTmpFiller);         /*备注*/
    	     }
    	     Bzj_js_ac_rel_Upd_Upd(sBzj_js_ac_rel, g_pub_tx.reply);
    	 }
    	 if(iRet)
    	 {
    	     sprintf( acErrMsg, "更新数据表Bzj_js_ac_rel_Upd_Upd出错!" );
		       WRITEMSG
		       sprintf( g_pub_tx.reply, "D105" );
		       goto ErrExit;
    	 }
    	 Bzj_js_ac_rel_Clo_Upd();
    }
    else
    {
    	sprintf( acErrMsg, "操作码类型错误!" );
		  WRITEMSG
		  sprintf( g_pub_tx.reply, "B332" );
		  goto ErrExit;
    }
    
    /** 记流水日志 **/
    strcpy(g_pub_tx.sub_tx_code, "B376");
    strcpy(g_pub_tx.ac_no, sBzj_acno);
    g_pub_tx.ac_seqn = sMdm_ac_rel_bzj.ac_seqn;
    g_pub_tx.ac_id = sMdm_ac_rel_bzj.ac_id;
    iRet = pub_ins_trace_log();
    if ( iRet )
    {
    		sprintf(acErrMsg, "记流水日志错误");
    		WRITEMSG
    		goto ErrExit;
    		
    }
    
    OkExit:
    	strcpy( g_pub_tx.reply, "0000" );
    	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
    	WRITEMSG
    	set_zd_data(DC_REPLY,g_pub_tx.reply);
    	return 0;
    ErrExit:
    	if(memcmp(g_pub_tx.reply,"0000",4) == 0)
		  {
		  	strcpy( g_pub_tx.reply, "H034" );
	    }
	    sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	    WRITEMSG
	    set_zd_data(DC_REPLY,g_pub_tx.reply);
	    return 1;
}
