/***************************************************************
* 文 件 名:  gD158.c
* 功能描述:  亲子关联转账
* 作    者:  lq

* 完成日期：    
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_eod_exec_c.h"
#include "mo_qzgl_c.h"
#include "mo_qzgl_hst_c.h"
gD158()
{
		int ret = 0;
		int iErr_flag=0;
		int iErr_flag_1=0;
		double dd_bal_1;
		double dd_bal_2;
		char cReturn[32];
		char tx_date[9];
		char stmp[3];
    struct dd_mst_c dd_mst1;
    struct dd_mst_c dd_mst2;
    struct mo_qzgl_c sMo_qzgl;
    struct mo_qzgl_hst_c sMo_qzgl_hst;
    memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
    memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
    memset(&sMo_qzgl,0x0,sizeof(struct mo_qzgl_c));
    memset(&sMo_qzgl_hst,0x0,sizeof(struct mo_qzgl_hst_c));
    memset(&g_pub_tx,0x0,sizeof(g_pub_tx));
    vtcp_log("[%s][%d],++++++++++++\n",__FILE__,__LINE__);
    
    memset(tx_date,0x00,sizeof(tx_date));
		sprintf(tx_date, "%ld", g_pub_tx.tx_date); 
		
		memset(stmp,0x00,sizeof(stmp));
		strcpy(stmp, tx_date+6);
		if(strncmp(stmp ,"8", 2)!=0 && strncmp(stmp ,"18", 2)!=0 && strncmp(stmp ,"28", 2)!=0)
		{
			vtcp_log("不是8,18,28号 不进行亲子划款[%s]",tx_date);
			goto OkExit;
	  }
    ret=sql_begin(); /*打开事务*/
		MYSQLERR
    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();
    if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
		{
			sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
			WRITEMSG
			goto ErrExit;
		}
		set_zd_long( DC_TRACE_NO,g_pub_tx.trace_no );
		set_zd_long( DC_TRACE_CNT,1 );
		strcpy(g_pub_tx.tx_code,"亲子转账");

    ret = Mo_qzgl_Dec_Sel(g_pub_tx.reply,"1=1 and sts='1' ");
		MYSQLERR
    while(1)
    {
    	iErr_flag=0;
			iErr_flag_1=0;
	    memset(&dd_mst1,0x0,sizeof(struct dd_mst_c));
	    memset(&dd_mst2,0x0,sizeof(struct dd_mst_c));
	    memset(cReturn,0x00,sizeof(cReturn));

	    double	tmpamt=0;
	    long		sys_date_tmp=0;
	    long		hz_date_tmp=0;
	    
	    sys_date_tmp = g_pub_tx.tx_date/100;
	    hz_date_tmp = g_pub_tx.tx_date%100;
	     vtcp_log("[%s][%d],AAAhz_date_tmp=[%ld]\n",__FILE__,__LINE__,hz_date_tmp);
	    ret = Mo_qzgl_Fet_Sel(&sMo_qzgl,g_pub_tx.reply);
	    if(ret==100)
	    {
	      break;
	    }
	    MYERR

			if((sMo_qzgl.hz_date <= hz_date_tmp))
			{
    	
	  	  vtcp_log("[%s][%d],++++++1++++++dd_mst1=[%ld],dd_mst2=[%ld],sts=[%s]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id,sMo_qzgl.sts);
	  	  if(sMo_qzgl.sts[0]=='*')
	  	  	continue;
	  	  
	  	  tmpamt = sMo_qzgl.qy_amt;
	  	  
	  	  ret=Mo_qzgl_hst_Sel(g_pub_tx.reply,&sMo_qzgl_hst,"child_ac_id=%ld and part_ac_id=%ld and tx_date like '%ld%%'",sMo_qzgl.child_ac_id,sMo_qzgl.part_ac_id,sys_date_tmp);
	  	  if(ret && ret!=100)
    	  {
    	    strcpy(g_pub_tx.reply, "D102");
    	    sprintf( acErrMsg, "Mo_qzgl_hst_Sel出错!!![%d]", ret );
    	    WRITEMSG
    	    goto ErrExit;
    	  }
    	  else if(ret==100)
    	 	{
    	 		struct mdm_ac_rel_c g_mdm_ac_rel;
    	 		struct mdm_ac_rel_c g_mdm_ac_rel_1;
					memset(&g_mdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
					memset(&g_mdm_ac_rel_1,0x0,sizeof(struct mdm_ac_rel_c));
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",sMo_qzgl.part_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "账户状态异常" );
						sprintf( acErrMsg, "查找账号活期信息错" );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if(g_mdm_ac_rel.note_sts[0]=='*'  )
					{
						strcpy( cReturn, "销户状态" );
						sprintf( acErrMsg, "销户状态");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if(g_mdm_ac_rel.note_sts[0]=='1'  )
					{
						strcpy( cReturn, "挂失状态" );
						sprintf( acErrMsg, "已挂失");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					
			    ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst1,"ac_id=%ld",sMo_qzgl.part_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "账户状态异常" );
						sprintf( acErrMsg, "查找账号活期信息错" );
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					pub_base_strpack( dd_mst1.ac_sts );
					if ( dd_mst1.ac_sts[0] == '*' )
					{
						strcpy( cReturn, "销户状态" );
						sprintf( acErrMsg, "已销户");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if ( dd_mst1.hold_sts[0]=='1' )
					{
						strcpy( cReturn, "完全冻结状态" );
						sprintf( acErrMsg, "完全冻结");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if ( dd_mst1.hold_sts[0]=='2' )
					{
						strcpy( cReturn, "只进不出状态" );
						sprintf( acErrMsg, "只进不出");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					if(dd_mst1.sttl_type[0]=='S')
					{
						strcpy( cReturn, "账户状态异常" );
						sprintf( acErrMsg, "该户为非结算户,不能转帐");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					dd_bal_1 = dd_mst1.bal - dd_mst1.ctl_amt ;
					if(dd_mst1.hold_sts[0]=='3')
					{
						dd_bal_1 = dd_bal_1 - dd_mst1.hold_amt;
					}
					vtcp_log("dd_bal_1=[%lf],tmpamt[%lf]",dd_bal_1,tmpamt);
			    if(pub_base_CompDblVal(dd_bal_1,tmpamt)<0)
			    {
						strcpy( cReturn, "余额不足" );
						sprintf( acErrMsg, "余额不足");
						WRITEMSG
						iErr_flag = 1;
						goto TrLog;
					}
					
			    
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel_1,"ac_id=%ld",sMo_qzgl.child_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "账户状态异常" );
						sprintf( acErrMsg, "查找账号活期信息错" );
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					} 
					if(g_mdm_ac_rel_1.note_sts[0]=='*'  )
					{
						strcpy( cReturn, "销户状态" );
						sprintf( acErrMsg, "已销户");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					if(g_mdm_ac_rel_1.note_sts[0]=='1'  )
					{
						strcpy( cReturn, "挂失状态" );
						sprintf( acErrMsg, "已挂失");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					
			    ret=Dd_mst_Sel(g_pub_tx.reply,&dd_mst2,"ac_id=%ld",sMo_qzgl.child_ac_id);
					if ( ret )
					{
						sprintf( cReturn, "账户状态异常" );
						sprintf( acErrMsg, "查找账号活期信息错" );
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					pub_base_strpack( dd_mst2.ac_sts );
					if ( dd_mst2.ac_sts[0] == '*' )
					{
						strcpy( cReturn, "销户状态" );
						sprintf( acErrMsg, "已销户");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					if ( dd_mst2.hold_sts[0]=='1' )
					{
						strcpy( cReturn, "完全冻结状态" );
						sprintf( acErrMsg, "完全冻结");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
					if(dd_mst2.sttl_type[0]=='S')
					{
						strcpy( cReturn, "账户状态异常" );
						sprintf( acErrMsg, "该户为非结算户,不能转帐");
						WRITEMSG
						iErr_flag_1 = 1;
						goto TrLog;
					}
	TrLog:	  
			  	vtcp_log("[%s][%d],++++++2++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);
			    if(iErr_flag == 1)
			    {
						vtcp_log("[%s][%d] 父母账号%ld余额不足",__FILE__,__LINE__,dd_mst1.ac_id);
						/*发余额不足通知短信 wudawei 20151127*/
						/*您尾号%s于%d月%d日%d时%02d分余额不足，亲子关联转出失败，请您及时充值。*/
						pub_mob_sendmail_bat( "1", cReturn, "G158",  g_mdm_ac_rel.ac_no, "Q", tmpamt, dd_mst1.bal, 0);
						/*发余额不足通知短信 wudawei 20151127*/
			    	continue;
			    }
			     if(iErr_flag_1 == 1)
			    {
						vtcp_log("[%s][%d] 子账号状态异常",__FILE__,__LINE__);
						/*发余额不足通知短信 wudawei 20151127*/
						/*您尾号%s于%d月%d日%d时%02d分余额不足，亲子关联转出失败，请您及时充值。*/
						pub_mob_sendmail_bat( "2", cReturn, "G158",  g_mdm_ac_rel_1.ac_no, "Q", tmpamt, dd_mst2.bal, 0);
						/*发余额不足通知短信 wudawei 20151127*/
			    	continue;
			    }
			    if (iErr_flag==0 && iErr_flag_1==0)
			    {
			    	/**进行转存**/
			    	vtcp_log("[%s][%d],++++++3++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);
			    	ret=sub_change_acct(dd_mst1,dd_mst2,tmpamt );
			    	MYERR
			    }
			    vtcp_log("[%s][%d],++++++4++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);
			  }
			}
    }
    Mo_qzgl_Clo_Sel();
    set_zd_data(DC_CODE,"");
		set_zd_double("102P",0.00 );
		set_zd_double("102Q",0.00 );
    set_zd_data("102J","");
    vtcp_log("[%s][%d],++++++5++++++dd_mst1=[%ld],dd_mst2=[%ld]\n",__FILE__,__LINE__,dd_mst1.ac_id,dd_mst2.ac_id);

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"亲子关联自动划转程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
  if (strcmp(g_pub_tx.reply,"0000") == 0 || g_pub_tx.reply[0]=='\0')
      strcpy(g_pub_tx.reply,"G009");
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"亲子关联自动划转程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int sub_change_acct(struct dd_mst_c dd_mst1,struct dd_mst_c dd_mst2,double amt)
{
		int ret=0;
		struct mdm_ac_rel_c vmdm_ac_rel;
		struct mo_qzgl_hst_c sMo_qzgl_hst;
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		memset(&sMo_qzgl_hst,0x0,sizeof(sMo_qzgl_hst));
		/*记帐*/
		/**借父母卡**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		g_pub_tx.tx_amt1=amt;
		strcpy(g_pub_tx.opn_br_no,dd_mst1.opn_br_no);
		strcpy(g_pub_tx.tx_br_no,dd_mst1.opn_br_no);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst1.ac_id);
		if(ret)
			goto ErrExit;
		vtcp_log("aaaaaaaaaaa[%s][%lf]\n",vmdm_ac_rel.ac_no,amt);
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst1.ac_id;
		g_pub_tx.ac_seqn =1;
		strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"0" );		/* 增减:0减 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转*/	
		strcpy( g_pub_tx.prt_ind,"0" );		/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );		/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"亲子关联自动转出" );
		strcpy(g_pub_tx.tx_code,"D158");
		strcpy(g_pub_tx.sub_tx_code,"D003");
		/*** 交易记帐处理 ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}
		/* 进行会计记帐 */
		set_zd_data("102J","01");         
		set_zd_double("102F",g_pub_tx.tx_amt1);                     

		
		strcpy(g_pub_tx.prdt_code,dd_mst1.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		
		/**贷儿童卡**/
		memset(&vmdm_ac_rel,0x0,sizeof(struct mdm_ac_rel_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac_rel,"ac_id=%ld and (note_sts = '0' or note_sts = '1')",dd_mst2.ac_id);
		if(ret) goto ErrExit;
		memcpy(g_pub_tx.ac_no,vmdm_ac_rel.ac_no,sizeof(g_pub_tx.ac_no));
		g_pub_tx.ac_id = dd_mst2.ac_id;
		g_pub_tx.ac_seqn = 1 ;
		strcpy(g_pub_tx.opn_br_no,dd_mst2.opn_br_no);
		strcpy( g_pub_tx.ac_get_ind,"00");	/*已经读取了mdm_ac_rel*/
		strcpy( g_pub_tx.ac_id_type,"1" );	/* 账号类型 1-存款 */
		strcpy( g_pub_tx.add_ind,"1" );		/* 增减:0加 */	
		strcpy( g_pub_tx.ct_ind,"2" );		/* 现转:1现2转*/	
		strcpy( g_pub_tx.prt_ind,"0" );/*是否打印存折(有折无折标志):1是0否 */
		strcpy( g_pub_tx.hst_ind,"1" );/*入明细:1日间入2日终单笔入3日终汇总入*/        
		strcpy ( g_pub_tx.intst_type , "1" );	/* 计息类型*/	
		g_pub_tx.svc_ind=1001;	   
		strcpy ( g_pub_tx.brf,"亲子关联自动转入" );
		strcpy(g_pub_tx.tx_code,"D158");
		strcpy(g_pub_tx.sub_tx_code,"D099");

		/*** 交易记帐处理 ***/
		if( pub_acct_trance() )
		{
			sprintf(acErrMsg,"交易记帐处理错误!");
			WRITEMSG
			goto ErrExit;
		}
		/* 进行会计记帐 */
		set_zd_data("101A","01");         
		set_zd_double("1013",g_pub_tx.tx_amt1);                     
		strcpy(g_pub_tx.prdt_code,dd_mst2.prdt_no);
		ret = pubf_acct_proc(g_pub_tx.sub_tx_code);
		if (ret != 0)
		{
			sprintf(acErrMsg,"会计记帐不成功!!");
			WRITEMSG
			goto ErrExit;
		}
		
		sMo_qzgl_hst.child_ac_id	= dd_mst2.ac_id;
		sMo_qzgl_hst.part_ac_id		= dd_mst1.ac_id;
		strcpy(sMo_qzgl_hst.tx_br_no,  g_pub_tx.tx_br_no);
    sMo_qzgl_hst.trace_no			= g_pub_tx.trace_no;
    sMo_qzgl_hst.tx_date			= g_pub_tx.tx_date;
    sMo_qzgl_hst.tx_amt				= amt;
    strcpy(sMo_qzgl_hst.tx_tel, g_pub_tx.tel);
    sMo_qzgl_hst.child_bal   = dd_mst2.bal+amt;
    ret = Mo_qzgl_hst_Ins(sMo_qzgl_hst, g_pub_tx.reply);
    if(ret)
    {
        strcpy(g_pub_tx.reply, "D102");
        sprintf( acErrMsg, "Mo_qzgl_hst_Ins出错!!![%d]", ret );
        WRITEMSG
        Mo_qzgl_Clo_Sel();
        goto ErrExit;
    }
		
		return 0;
ErrExit:
	return 1;
}
 
