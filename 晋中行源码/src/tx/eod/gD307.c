/***************************************************************
* 文 件 名: gD307.c
* 功能描述: 财政集中支付存款批量划款
*
* 作    者: YW
* 完成日期: 2009-12-23
* 
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
* insert into tx_sub_def 	values('G307','财政支付存款批量划款','gD307','1','0')
* insert into com_parm values('CZJZZ','1','财政集中支付存款临时户','5','500055018759000018') 开户行行号+账号
* insert into com_eod_exec values('391','D307','gD307','财政集中支付批量划款','1111111111','0','0','00','00','1','Y','Y','N','2','Y')	
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"

gD307()
{
	/****先初始化全局变量****/
	
	
	int ret=0;
	double c_bal=0.00, d_bal=0.00;
	char dc_ind;
	struct dd_mst_c  g_dd_mst;
	struct com_branch_c sCom_branch;
	struct com_sys_parm_c sCom_sys_parm;
	struct mdm_ac_rel_c g_mdm_ac_rel;
	struct mdm_ac_rel_c sMdm_ac_rel;	


	char tmp[6];
	char tmpstr[51];
	char tmpstr1[51];
	char parm_code[25]; 
	int i=0;
	
	memset(parm_code,0,sizeof(parm_code));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(tmpstr1,0,sizeof(tmpstr1));		
	memset(&g_dd_mst ,0, sizeof(struct dd_mst_c));
	memset(&sCom_sys_parm,0, sizeof(struct com_sys_parm_c));
	memset(&g_mdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));	
	memcpy(parm_code,"CZJZZ",5);
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/*** 初始化全局变量 ***/
	if(pub_base_sysinit())
	{
		strcpy(acErrMsg,"初始化全局变量出错!");
		WRITEMSG
		goto ErrExit;
	}
	
	if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
	{
		sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
		WRITEMSG
		goto ErrExit;
	}
	ret=Dd_mst_Dec_Sel(g_pub_tx.reply,"prdt_no='12E' and ac_sts='1' and hold_sts='0' and bal<0 ");
	if(ret)
	{
		strcpy(acErrMsg,"定义活期主表游标出错!");
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		i=0;
		memset(&g_dd_mst,0,sizeof(g_dd_mst));
		ret=Dd_mst_Fet_Sel(&g_dd_mst,g_pub_tx.reply);
		if(ret==100) break;
		else if(ret)
		{
		   strcpy(acErrMsg,"读取活期主表游标出错!");
		   WRITEMSG
		   goto ErrExit;
		}
		
		memset(tmpstr,0,sizeof(tmpstr));
    ret = pub_base_GetParm_Str(parm_code,1,tmpstr);
    if(ret)
    {
       vtcp_log("%s,%d,取待清算帐号出错!",__FILE__,__LINE__);
       strcpy(g_pub_tx.reply,"CU06");
       goto ErrExit;
    }

    memcpy(g_pub_tx.ac_no,tmpstr+5,13);
    memcpy(g_pub_tx.opn_br_no,tmpstr,5);
		memcpy(g_pub_tx.tx_br_no,tmpstr,5);
		memset(&sMdm_ac_rel,0,sizeof(sMdm_ac_rel));
		ret = Mdm_ac_rel_Sel1(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
		if(ret)
		{
		    vtcp_log("%s,%d,查询介质与账户关系表出错",__FILE__,__LINE__);	
		    strcpy(g_pub_tx.reply,"CU25");
		    goto ErrExit;
		}
		g_pub_tx.ac_seqn = 1;
    g_pub_tx.ac_id=sMdm_ac_rel.ac_id;    
 		g_pub_tx.tx_amt1=-g_dd_mst.bal;
		strcpy( g_pub_tx.tx_code,"G307"); 
		strcpy(g_pub_tx.sub_tx_code,"WNJZ");
		strcpy( g_pub_tx.cur_no,"01");
		strcpy( g_pub_tx.ac_get_ind,"000");	
		strcpy( g_pub_tx.ac_id_type,"1" );	
		strcpy( g_pub_tx.add_ind,"0" );		
		strcpy( g_pub_tx.ct_ind,"2" );		
		strcpy( g_pub_tx.prt_ind,"0" );
		strcpy( g_pub_tx.hst_ind,"1" );        
		strcpy ( g_pub_tx.intst_type , "1" );	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"财政集中支付批量划款");					
		if( pub_acct_trance() )
		{
				sprintf(acErrMsg,"交易记帐处理错误!");
				WRITEMSG
				goto ErrExit;
		}

		set_zd_double("1001",g_pub_tx.tx_amt1);	
		set_zd_data("0210","01");
	  set_zd_data("0660","1");			
	  set_zd_data("0670","2");				                                          
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		

		memset(&g_mdm_ac_rel,0,sizeof(g_mdm_ac_rel));
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",g_dd_mst.ac_id);
		if(ret)
		{
		    vtcp_log("%s,%d,查询介质与账户关系表出错",__FILE__,__LINE__);	
		    strcpy(g_pub_tx.reply,"CU25");
		    goto ErrExit;
		}
		g_pub_tx.ac_seqn = g_dd_mst.ac_seqn ;
		memcpy(g_pub_tx.tx_br_no,tmpstr,5);
		memcpy(g_pub_tx.opn_br_no,g_dd_mst.opn_br_no,5);
		memcpy(g_pub_tx.ac_no,g_mdm_ac_rel.ac_no,13);
		g_pub_tx.tx_amt1=-g_dd_mst.bal;
		g_pub_tx.ac_id=g_dd_mst.ac_id;
		strcpy(g_pub_tx.sub_tx_code,"D099");		
		strcpy( g_pub_tx.tx_code,"G307");
		strcpy( g_pub_tx.cur_no,"01");
		strcpy( g_pub_tx.ac_get_ind,"000");	
		strcpy( g_pub_tx.ac_id_type,"1" );	
		strcpy( g_pub_tx.add_ind,"1" );		
		strcpy( g_pub_tx.ct_ind,"2" );		
		strcpy( g_pub_tx.prt_ind,"0" );
		strcpy( g_pub_tx.hst_ind,"1" );        
		strcpy( g_pub_tx.intst_type , "1" );	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"财政集中支付批量划款");
		if( pub_acct_trance() )
		{
				sprintf(acErrMsg,"交易记帐处理错误!");
				WRITEMSG
				goto ErrExit;
		}

		set_zd_double("1013",g_pub_tx.tx_amt1);
		set_zd_data("101A","01");
	  set_zd_data("101B","2");			                     
		strcpy(g_pub_tx.prdt_code,g_dd_mst.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
					  		
	}
	Dd_mst_Clo_Sel();


OkExit:
    sql_commit();   /*--提交事务--*/
    strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"268转入处理成功![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 0;
ErrExit:
    if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
    {
        strcpy(g_pub_tx.reply,"G009");
    }
    sql_rollback(); /*--事务回滚--*/
	sprintf(acErrMsg,"268转入处理失败![%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data("0120",g_pub_tx.reply);
    return 1;
}

