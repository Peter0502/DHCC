/*************************************************************
* 文 件 名: gD335.c
* 功能描述: 手机充值日终划款 
* 作    者: 
* 完成日期: 
*
* sql: 
insert into com_eod_exec values('1002','Z335','gD335','手机充值日终划款','1111111111','0','0','00','00','2','Y','Y','N','2','Y');
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define MYRETERR if(ret){ \
        sprintf(acErrMsg,"sql erro!! [%d]",ret); \
        WRITEMSG \
        return ret; \
	}
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "mob_chg_rel_c.h"
#include "in_mst_c.h"

#define HFSXF	0.003
#define LLSXF	0.01
#define HFAMT	0
#define LLAMT	0

gD335()
{
	int ret=0;
	double amt=0.00;

	char  main_ac_id [10];
	struct com_sys_parm_c com_sys_parm_c;
	struct mob_chg_rel_c s_mob_chg_rel;
	struct dd_parm_c dd_parm_c;
	struct dd_mst_c dd_mst_app;
	struct dd_mst_c dd_mst_c;
	struct dd_mst_c dd_mst_a_c;
	struct in_mst_c in_mst_c;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_ac_rel_c sMdm_ac_rel_1;
	
	memset(&dd_mst_c,0x00,sizeof(struct dd_mst_c) );
	memset(&dd_mst_app,0x00,sizeof(struct dd_mst_c) );
	memset(&dd_mst_a_c,0x00,sizeof(struct dd_mst_c) );
	memset(&in_mst_c,0x00,sizeof(struct in_mst_c) );
	memset(&dd_parm_c,0x00,sizeof(struct dd_parm_c) );
	memset(&com_sys_parm_c, 0x00, sizeof(struct com_sys_parm_c));
	memset(&s_mob_chg_rel, 0x00, sizeof(struct mob_chg_rel_c));
	memset(&g_pub_tx, 0x00, sizeof(g_pub_tx));
	memset(main_ac_id,0x00,sizeof(main_ac_id) );

	ret = sql_begin();	/* 打开事务 */
	MYRETERR


	/* 查询系统参数表 */
	ret = Com_sys_parm_Sel(g_pub_tx.reply, &com_sys_parm_c, "1=1");
	MYRETERR
	
	g_pub_tx.tx_date = com_sys_parm_c.sys_date;
	strcpy(g_pub_tx.cur_no, RMB);
	strcpy(g_pub_tx.tx_code, "Z335");
	strcpy(g_pub_tx.sub_tx_code, "Z335");
	g_pub_tx.trace_cnt = 0;
	strcpy(g_pub_tx.ct_ind, "2");
	strcpy(g_pub_tx.tel, "9999");
	
	  /*取主机流水号*/
    if (pub_base_PubQlsh(&g_pub_tx.trace_no, g_pub_tx.reply))
    {
        sprintf(acErrMsg, "取主机流水号出错! trace_no=[%ld]",g_pub_tx.trace_no);
        WRITEMSG
        goto ErrExit;
    }
	/******划款*********/
	vtcp_log("翻盘后划款开始");
	/*读表数据*/
	ret = Mob_chg_rel_Dec_Sel(g_pub_tx.reply,  "sts!='*' order by filler2 asc");
	  if ( ret )                                                                     
{                                                                              
	sprintf(acErrMsg,"查询关联账户表出错 [%d][%d]",ret,s_mob_chg_rel.main_ac_id);       
	WRITEMSG                                                                     
	   goto ErrExit;                                                                
}  

	while(1)
	{
		memset(&s_mob_chg_rel, 0x00, sizeof(struct mob_chg_rel_c));
		ret = Mob_chg_rel_Fet_Sel(&s_mob_chg_rel, g_pub_tx.reply);
		if (ret == 100) break;
		MYSQLERR
	     vtcp_log("[&s_mob_chg_rel],s_mob_chg_rel[%ld]",s_mob_chg_rel.main_ac_id);      
		sprintf(main_ac_id, "%ld" ,s_mob_chg_rel.main_ac_id );
		if(main_ac_id[0] =='9')
		{
			ret=In_mst_Sel(g_pub_tx.reply,&in_mst_c,"ac_id='%ld' and sts!='*'", s_mob_chg_rel.main_ac_id);
			amt=in_mst_c.bal;
			vtcp_log("借方记帐科目[%ld][%.2lf]",in_mst_c.ac_id,amt);

			if ( ret )
			{
				sprintf(acErrMsg,"取内部主文件错误 [%d][%d]",
					s_mob_chg_rel.main_ac_id,ret);
				WRITEMSG
					goto ErrExit;
			}
			if(in_mst_c.sts[0]=='*' )
			{
			sprintf(acErrMsg,"该内部户已销户 [%d]",in_mst_c.ac_id);
				WRITEMSG
					goto ErrExit;

			}
			memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld",in_mst_c.ac_id);
			MYSQLERR

			strcpy( g_pub_tx.ac_id_type,"9" );
			g_pub_tx.svc_ind=9001; /**处理代码***/
		}else if(main_ac_id[0] =='1')
		{
			ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_a_c,"ac_id='%ld' and ac_sts!='*'", s_mob_chg_rel.main_ac_id);
			amt=dd_mst_a_c.bal;
			vtcp_log("借方记帐科目[%ld][%.2lf]",dd_mst_a_c.ac_id,amt);

			if ( ret )
			{
				sprintf(acErrMsg,"取活期主文件错误 [%d][%d]",
					s_mob_chg_rel.main_ac_id,ret);
				WRITEMSG
					goto ErrExit;
			}
			if(dd_mst_a_c.ac_sts[0]=='*' )
			{
			sprintf(acErrMsg,"该活期户已销户 [%d]",dd_mst_a_c.ac_id);
				WRITEMSG
					goto ErrExit;

			}
			memset(&sMdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld",dd_mst_a_c.ac_id);
			MYSQLERR

			strcpy( g_pub_tx.ac_id_type,"1" );
			g_pub_tx.svc_ind=1001; /**处理代码***/
		}
	vtcp_log("开始转出记账");
	strcpy(g_pub_tx.sub_tx_code,"D003");
	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,sMdm_ac_rel.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
	strcpy(g_pub_tx.ac_no,sMdm_ac_rel.ac_no);
	g_pub_tx.ac_id=sMdm_ac_rel.ac_id;
	g_pub_tx.ac_seqn=0;
	vtcp_log("[%s][%d]fffffffffamt===[%f]\n",__FILE__,__LINE__,amt);
	
	if(strcmp(s_mob_chg_rel.filler2,"0601") == 0)/*划转话费手续费*/
	{
		if(amt>HFAMT)
		{
			amt=amt-HFAMT;
		}
		else
		{
			amt=0;
		}
		amt=amt * HFSXF;
		vtcp_log("[%s][%d]话费手续费转出金额===[%f]\n",__FILE__,__LINE__,amt);
	}
	else if(strcmp(s_mob_chg_rel.filler2,"0602") == 0)/*划转流量手续费*/
	{
		if(amt>LLAMT)
		{
			amt=amt-LLAMT;
		}
		else
		{
			amt=0;
		}
		amt=amt * LLSXF;
		vtcp_log("[%s][%d]流量手续费转出金额===[%f]\n",__FILE__,__LINE__,amt);
	}
	else if((strcmp(s_mob_chg_rel.filler2,"0611") == 0))
	{
		if(amt>HFAMT)
		{
			amt=(amt-HFAMT);
		}
		else
		{
			amt=0;
		}
		vtcp_log("[%s][%d]话费流量转出金额===[%f]\n",__FILE__,__LINE__,amt);
	}
	else if((strcmp(s_mob_chg_rel.filler2,"0612") == 0))
	{
		if(amt>LLAMT)
		{
			amt=(amt-LLAMT);
		}
		else
		{
			amt=0;
		}
		vtcp_log("[%s][%d]话费流量转出金额===[%f]\n",__FILE__,__LINE__,amt);
	}
	
	g_pub_tx.tx_amt1=amt;
	strcpy(g_pub_tx.add_ind,"0"); 
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"000");
	strcpy(g_pub_tx.ac_get_ind,"00");
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/
	strcpy(g_pub_tx.brf,"划转转出");
	strcpy( g_pub_tx.prt_ind,"0" );

	
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		return 1;
	}

		vtcp_log("[%s][%d]fffffffffamt===[%f]\n",__FILE__,__LINE__,amt);
		set_zd_data( "102J", "01" );
    	set_zd_data( "102K", "2" );
		set_zd_double( "102F", g_pub_tx.tx_amt1 );
		
	if ( pubf_acct_proc("D003") )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		return 1;
	}
	if(strcmp(s_mob_chg_rel.filler2,"0601")==0 || strcmp(s_mob_chg_rel.filler2,"0602")==0)
	{
		memset(&sMdm_ac_rel_1,0x00,sizeof(struct mdm_ac_rel_c));
		strcpy(g_pub_tx.ac_no,"51104");
	}
	else
	{
		ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst_app,"ac_id='%ld' ",s_mob_chg_rel.app_ac_id);
		if ( ret )
		{
			sprintf(acErrMsg,"查询活期表出错 [%d][%d]",ret,s_mob_chg_rel.app_ac_id);
			WRITEMSG
			goto ErrExit;
		}
		memset(&sMdm_ac_rel_1,0x00,sizeof(struct mdm_ac_rel_c));
	 	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_1,"ac_id=%ld",dd_mst_app.ac_id);
			MYSQLERR
		strcpy(g_pub_tx.ac_no,sMdm_ac_rel_1.ac_no);
	}
	vtcp_log("开始转入记账");
	strcpy(g_pub_tx.sub_tx_code,"D099");
	strcpy(g_pub_tx.tx_br_no,sMdm_ac_rel.opn_br_no);
	strcpy(g_pub_tx.opn_br_no,sMdm_ac_rel_1.opn_br_no);
	strcpy(g_pub_tx.cur_no,"01");
	g_pub_tx.ac_id=s_mob_chg_rel.app_ac_id;
	g_pub_tx.tx_amt1=amt;
	g_pub_tx.ac_seqn=1;
	strcpy( g_pub_tx.ac_id_type,"1" );
	strcpy(g_pub_tx.ac_get_ind,"00");
	strcpy( g_pub_tx.prt_ind,"0" );
	strcpy(g_pub_tx.add_ind,"1"); 
	strcpy(g_pub_tx.hst_ind,"1"); /**日间入明细**/
	strcpy(g_pub_tx.ac_wrk_ind,"0000000011");
	g_pub_tx.svc_ind=1001; /**处理代码***/
	strcpy(g_pub_tx.ct_ind,"2");	/*** 转账 ***/
	strcpy(g_pub_tx.brf,"划转转入");
	if ( pub_acct_trance() )
	{
		sprintf(acErrMsg,"调用存取款主控失败!");
		WRITEMSG
		goto ErrExit;
	}
	
	set_zd_data("101A","01");
	set_zd_data("101B","2");
	set_zd_double("1013",g_pub_tx.tx_amt1);

	if ( pubf_acct_proc("D099") )
	{
		sprintf(acErrMsg,"登记会计流水错误!");
		WRITEMSG
		goto ErrExit;
	}
}
	Mob_chg_rel_Clo_Sel();
	
OkExit:
	sql_commit();		/*--提交事务--*/
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "日终理财处理成功!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 0;
ErrExit:
	if (strcmp(g_pub_tx.reply, "0000") == 0 || g_pub_tx.reply[0] == '\0') {
		strcpy(g_pub_tx.reply, "G009");
	}
	sql_rollback();		/*--事务回滚--*/
	sprintf(acErrMsg, "日终理财处理失败!!!!![%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data("0120", g_pub_tx.reply);
	return 1;
}
	
