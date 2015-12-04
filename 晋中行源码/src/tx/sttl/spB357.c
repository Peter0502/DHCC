/*************************************************
* 文 件 名:  spB357.c
* 功能描述： 完成担保帐号和借据号关系的建立和解除
*
* 作    者:  Dongxy
* 完成日期： 2006年9月18日
*
* 修改记录： 
*   日   期: 
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "cassreg_c.h"
#include "mdm_ac_rel_c.h"
#include "ln_mst_c.h"

int spB357()
{
	struct	cassreg_c 	sCassReg;	/* 担保关系登记簿 */
	struct	cassreg_c 	sCassReg1;	/* 担保关系登记簿 */
	struct	mdm_ac_rel_c 	sMdmAcRel;	/* 介质账户关系表 */
	char	cOperCode[2];			/* 操作码 */
	int		ret;

	memset( &sCassReg, 0x00, sizeof( struct cassreg_c ) );
	memset( &sCassReg1, 0x00, sizeof( struct cassreg_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( cOperCode, 0x00, sizeof( cOperCode) );

	/* 数据初始化 */
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0670", cOperCode );		/** 操作类型 **/
	get_zd_data( "0320", g_pub_tx.ac_no );		/** 担保帐号 **/
	get_zd_data( "0330", sCassReg.pact_no );	/** 借据号 **/
	get_zd_data( "0330", sCassReg1.pact_no );	/** 借据号 **/
	get_zd_double("0400",&sCassReg.amt);		/** 担保金额 **/
	get_zd_data("102H",sCassReg.name);		/** 担保户名 **/
	get_zd_double("102I",&sCassReg.bal);		/** 担保户当时余额 **/
	get_zd_data("0070",sCassReg.tel);		/** 操作员 **/
	get_zd_data("0020",sCassReg.br_no);		/** 机构编码 **/
  get_zd_long("0550",&sCassReg.bw_ac_id);/**客户号**/
	vtcp_log("[%s][%d]========操作类型=[%s]",__FILE__,__LINE__,cOperCode);

	/*ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", g_pub_tx.ac_no );
        if( ret == 100 )
    	{
                sprintf( acErrMsg, "无此帐号![%s]", g_pub_tx.ac_no );
        	WRITEMSG
        	strcpy( g_pub_tx.reply, "A029" );
                goto ErrExit;
        }else if( ret )
        {
        	sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
        }
*/
	/*sCassReg.ac_id = sMdmAcRel.ac_id;*/
	strcpy(sCassReg.db_ac_no, g_pub_tx.ac_no);

	if(cOperCode[0] == '0')	/** 前台选择建立 **/
	{

		vtcp_log("[%s][%d]========开始建立",__FILE__,__LINE__);
		/** 借据号不能重复 **/
        	ret = Cassreg_Sel( g_pub_tx.reply, &sCassReg1, " pact_no='%s' ", sCassReg1.pact_no);
        	if( ret == 0 )
        	{
                	sprintf( acErrMsg, "此借据号已存在!" );
                	WRITEMSG
                	strcpy( g_pub_tx.reply, "L020" );
                	goto ErrExit;
        	}
        	else if( ret != 100 )
        	{
                	sprintf( acErrMsg, "执行Cassreg_Sel出错![%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}

              /*  ret = pub_base_CompDblVal(sCassReg.amt,sCassReg.bal);
                if ( ret ==0 || ret ==1)
                {
                        sprintf( acErrMsg, "担保金额大于等于账户余额[%d]", ret );
                        WRITEMSG
                        sprintf( g_pub_tx.reply, "B353" );
                        goto ErrExit;
                }*/
					get_zd_long("0040",&sCassReg.in_trace_no);	/** 建立流水 **/
					get_zd_long("0050",&sCassReg.in_tx_date);	/** 担保日期 **/
					sCassReg.flag[0] = '0';
	        ret = Cassreg_Ins( sCassReg, g_pub_tx.reply );
        	if ( ret )
        	{
        		sprintf( acErrMsg, "执行Cassreg_Ins错误!ret=[%d]", ret );
        		WRITEMSG
        		goto ErrExit;
        	}
	}
	else if(cOperCode[0] == '1')	/** 前台选择删除 **/
	{
		int cnt; double samt = 0;
		
		cnt=sql_count_sum_double("ln_mst", "bal", &samt, "pact_no = '%s'", sCassReg.pact_no);
		
		vtcp_log("[%s][%d]========开始删除 cnt %d (samt %.2lf)",__FILE__,__LINE__, cnt, samt);
		if(cnt>0 && samt>0.005){
        		sprintf( acErrMsg, "贷款未还清");
        		WRITEMSG
                        sprintf( g_pub_tx.reply, "L409" );
        		goto ErrExit;
		}
		vtcp_log("[%s][%d]========开始删除",__FILE__,__LINE__);
		ret = Cassreg_Dec_Upd(g_pub_tx.reply, " pact_no = '%s' ",sCassReg.pact_no);
	  if ( ret )
    {
          	sprintf( acErrMsg, "执行Cassreg_Dec_Upd错[%d]", ret );
          	WRITEMSG
          	goto ErrExit;
    }
    ret = Cassreg_Fet_Upd( &sCassReg, g_pub_tx.reply );
    if( ret &&ret!=100 )
    {
          	sprintf( acErrMsg, "执行Cassreg_Fet_Upd错![%d]", ret );
         		WRITEMSG
         		goto ErrExit;
    }
	/*	if(sCassReg.flag[0] == '1')
		{
                	sprintf( acErrMsg, "此关系已经解除" );
                        sprintf( g_pub_tx.reply, "H020" );
               		WRITEMSG
               		goto ErrExit;
		}
	*/
		get_zd_long("0040",&sCassReg.out_trace_no);	/** 解除流水 **/
		get_zd_long("0050",&sCassReg.out_tx_date);	/** 解除日期 **/
		sCassReg.flag[0] = '1';
	/***	
	ret = Cassreg_Upd_Upd( sCassReg, g_pub_tx.reply );
	        if ( ret )
        	{
                	sprintf( acErrMsg, "修改Cassreg_Upd_Upd错[%d]", ret );
                	WRITEMSG
                	goto ErrExit;
        	}
	**/
		ret=Cassreg_Del_Upd(sCassReg,g_pub_tx.reply);
		if(ret && ret!=100)
		{
		      sprintf(acErrMsg,"删除Cassreg_Del_Upd错[%d]",ret);
			  WRITEMSG
			  goto ErrExit;
		}
		Cassreg_Clo_Upd();
		
	}
	else
	{
		sprintf( acErrMsg, "操作类型错误![%s]",cOperCode );
		WRITEMSG
		strcpy( g_pub_tx.reply, "O020" );
		goto ErrExit;
	}


	vtcp_log("[%s][%d]========开始写流水",__FILE__,__LINE__);
	/* 为记流水赋值 */
	strcpy( g_pub_tx.brf, "担保关联建立" );	/* 备注 */

	/* 记流水日志 */
	strcpy(g_pub_tx.ac_no,sCassReg.pact_no);
	/*strcpy(g_pub_tx.beg_note_no,sCassReg.pact_no);*/
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");	/**/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]========程序结束",__FILE__,__LINE__);

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
