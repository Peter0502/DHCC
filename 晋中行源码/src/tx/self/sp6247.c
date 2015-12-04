
/********************************************************
交易名称：批量冲正交易sp6247
程序作者：Cole 2013.09.20
功能描述：人行小额往帐拒绝后批量冲正（对包内所有已记账交易进行冲正，可用于小贷记往帐，借记回执往帐等拒绝的情况）
冲账文件：hx_batch_cz_yyyymmdd_序号.txt
文件格式：原平台日期|原平台流水|原核心日期|原核心流水|交易金额\n
					 optdate    opttrace   ohxdate    ohxtrace	tx_amt
注意：要么全部冲正成功，数据库提交；要么都不成功，数据库回滚；
insert into tx_def values('6247','批量冲正','10000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000100','1','1','2');
insert into tx_flow_def values('6247','0','6247','#$');
insert into tx_sub_def values('6247','批量冲正','sp6247','0','0');
********************************************************/
#define ERR_DEAL if( iRet ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_sys_parm_c.h"
#include "in_mst_c.h"
#include "com_item_c.h"
#include "chnl_single_cnaps_c.h"
#include "trace_log_c.h"
#include "dc_log_c.h"
sp6247()
{		
	int i=0;
	int iRet = 0;
	int icount=0;
	int truecnt=0;
	long tot_num = 0;
	double tot_amt = 0.00;
	double trueamt=0.00;
	long tx_date = 0;
	long optdate = 0; 
	long opttrace = 0;
	long ohxdate = 0; 
	long ohxtrace = 0;
	char filename[60] = {0};
	char fldstr[5][20] = {0};
	char tmpstr[100] = {0};
	char cFd28[9] = {0};/*接收代收付标示*/
	char all_filename[248] = {0};/***全路径**/
	char *p;
	FILE *fp = NULL;
	struct chnl_single_cnaps_c stChnl = {0};
	struct dd_mst_c	dd_mst = {0};
	struct com_sys_parm_c	com_sys_parm = {0};
	struct mdm_ac_rel_c mdm_ac_rel = {0};
	struct com_item_c com_item = {0};
	struct in_mst_c in_mst = {0};
	struct trace_log_c stTracelog={0};
	struct dc_log_c stDclog={0};
	struct czfn{
					long optdate;
					long opttrace;
					long ohxdate;
					long ohxtrace;
					double tx_amt;
				} *stCzfn;
	pub_base_sysinit();
	sql_begin();
	get_zd_double("0400",&tot_amt);/**总金额**/
	get_zd_long("0440",&tx_date);/****交易日期*****/
	get_zd_long("0480",&tot_num);/***总笔数***/
	get_zd_data("0830",filename);/**冲账文件名***/
	get_zd_data("0280",cFd28);/**代收付标志***/
	vtcp_log("%s,%d,tot_amt=%f   tx_date=%ld  tot_num=%ld  filename=[%s]!",__FILE__,__LINE__,tot_amt,tx_date,tot_num,filename);
	
	sprintf(all_filename,"%s/txt/%s",getenv("HOME"),filename);
	vtcp_log("%s,%d,all_filename=[%s]!",__FILE__,__LINE__,all_filename);
	
	iRet = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm,"1=1");
	if(iRet)
	{
		sprintf( acErrMsg," 查询系统日期错误 " );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d,all_filename=[%s]!",__FILE__,__LINE__,all_filename);
	pub_base_strpack(all_filename);
	fp = fopen( all_filename,"r" );
	if ( fp==NULL )
	{
		sprintf( acErrMsg," 打开文件出错 [%s]  ", all_filename );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	stCzfn=calloc(tot_num,sizeof(struct czfn));
	if(stCzfn==NULL)
	{
		sprintf( acErrMsg,"分配内存出错");
		WRITEMSG
		strcpy( g_pub_tx.reply, "S047" );
		goto ErrExit;
	}
	while(1){
		memset( tmpstr, 0x00, sizeof(tmpstr) );
		vtcp_log("%s,%d,1111!",__FILE__,__LINE__);
		p = fgets(tmpstr,1024,fp);/*取出一条记录*/
		vtcp_log("%s,%d,tmpstr is [%s]!",__FILE__,__LINE__,tmpstr);
		if(feof(fp) && strlen(tmpstr)==0)
		{
			break;
		}
		pub_base_strpack_all(tmpstr);
		memset(fldstr,0x00,sizeof(fldstr));
		for ( i=0;i<5;i++ )
		{
			iRet=pub_base_cut_str( tmpstr,fldstr[i],"|",i+1 );
			vtcp_log(" 第[%d]个值是：[%s] ",i+1,fldstr[i]);
			pub_base_strpack_all( fldstr[i] );
		}
		/*************************************
		原平台日期|原平台流水|原核心日期|原核心流水|交易金额\n
		optdate    opttrace   ohxdate    ohxtrace  	tx_amt
		*************************************/
			stCzfn[truecnt].optdate=atol(fldstr[0]);
			stCzfn[truecnt].opttrace=atol(fldstr[1]);
			stCzfn[truecnt].ohxdate=atol(fldstr[2]);
			stCzfn[truecnt].ohxtrace=atol(fldstr[3]);
			stCzfn[truecnt].tx_amt=atof(fldstr[4]);
		truecnt++;
		trueamt+=atof(fldstr[4]);
		vtcp_log("%s,%d,2222!",__FILE__,__LINE__);
	}
	fclose(fp);
	
	if(truecnt != tot_num){
		sprintf( acErrMsg," 输入总笔数与文件总笔数不一致【%ld】【%ld】!!！",tot_num,truecnt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
	}
	if(pub_base_CompDblVal(trueamt,tot_amt)){
		sprintf( acErrMsg," 输入总金额与文件总金额不一致【%lf】【%lf】!!！",tot_amt,trueamt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H019");
		goto ErrExit;
	}
	while(truecnt-->0)
	{/*while111 beg*/
		memset(&stChnl,0x00,sizeof(struct chnl_single_cnaps_c));
		vtcp_log("%s,%d,tracecnt is [%d]!",__FILE__,__LINE__,truecnt);
		vtcp_log("%s,%d,[%ld][%ld][%ld][%ld]!",__FILE__,__LINE__,stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace,stCzfn[truecnt].optdate,stCzfn[truecnt].opttrace);
		
		vtcp_log("[%s][%d],代收付标示cFd28=[%s]",__FILE__,__LINE__,cFd28);
		if(!memcmp(cFd28,"DSF",3))
		{
			vtcp_log("[%s][%d],进入此分支,不检查单笔Chnl_single_cnaps表",__FILE__,__LINE__);
			tx_date=stCzfn[truecnt].ohxdate;/*每次都给tx_date赋值，为该笔交易的核心日期*/
		}
		else
		{
			iRet =Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnl,"tx_date=%ld and trace_no=%ld and BBMK='BEPS' and pt_date=%ld and pt_trace_no=%ld and sts='1'",\
				stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace,stCzfn[truecnt].optdate,stCzfn[truecnt].opttrace);
			if(iRet)ERR_DEAL
				tx_date=stCzfn[truecnt].ohxdate;/*每次都给tx_date赋值，为该笔交易的核心日期*/
		}
		
		if(tx_date == com_sys_parm.sys_date)/*当日*/
		{
			iRet =Trace_log_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld order by trace_cnt desc",stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace);
		}
		else
		{
			iRet =Trace_log_rz_Dec_Sel(g_pub_tx.reply,"tx_date=%ld and trace_no=%ld order by trace_cnt desc",stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace);
		}
		icount=0;
		while(1)
		{/*while222 beg*/
			memset(&stDclog,0x00,sizeof(stDclog));
			memset(&stTracelog,0x00,sizeof(stTracelog));
			if(tx_date==com_sys_parm.sys_date)/*当日*/
			{
				iRet=Trace_log_Fet_Sel(&stTracelog,g_pub_tx.reply);
				if((iRet==100 || iRet==1403)&&icount!=0)
					break;
				else if(iRet)
				{
					ERR_DEAL
					Trace_log_Clo_Sel();
				}
				iRet=Dc_log_Sel(g_pub_tx.reply,&stDclog,"tx_date=%ld and trace_no=%ld and trace_cnt=%ld",stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace,stTracelog.trace_cnt);
				if(iRet)
					ERR_DEAL
			}
			else /*隔N日 N>1*/
			{
				iRet=Trace_log_rz_Fet_Sel(&stTracelog,g_pub_tx.reply);
				if((iRet==100 || iRet==1403)&&icount!=0)
					break;
				else if(iRet)
				{
					Trace_log_Clo_Sel();
					ERR_DEAL
				}
				iRet=Dc_log_rz_Sel(g_pub_tx.reply,&stDclog,"tx_date=%ld and trace_no=%ld and trace_cnt=%ld",stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace,stTracelog.trace_cnt);
				if((iRet==100 || iRet==1403)&&icount!=0)
					break;
				else if(iRet)
					ERR_DEAL
			}
				/*内部帐或者科目*/
			if(stTracelog.svc_ind==9001)
			{
		    	memcpy(g_pub_tx.opn_br_no,stTracelog.opn_br_no,sizeof(g_pub_tx.opn_br_no)-1);
				strcpy(g_pub_tx.ac_no,stTracelog.ac_no);		 
		    	g_pub_tx.tx_amt1=0-stTracelog.amt;/*冲正记负值*/
		    	g_pub_tx.ac_id=stTracelog.ac_id;
		    	g_pub_tx.ac_seqn=stTracelog.ac_seqn;
		    	strcpy(g_pub_tx.ac_id_type,"9");/*内部账户*/
		    	vtcp_log("%s,%d,【%ld】  【%ld】!",__FILE__,__LINE__,g_pub_tx.ac_seqn,stTracelog.ac_seqn);
		    	strcpy(g_pub_tx.cur_no,stTracelog.cur_no);/* 币种 */
		    	strcpy(g_pub_tx.ct_ind,stTracelog.ct_ind);
		    	if( stDclog.dc_ind[0] == '1')
		    	{
				strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
		    	}
		    	else if( stDclog.dc_ind[0] =='2')
		    	{
		    		strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
		    	}
		    	strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
		    	strcpy(g_pub_tx.hst_ind,"1");/**日间入明细**/
		    	strcpy(g_pub_tx.ac_wrk_ind,"0000000");
		    	g_pub_tx.svc_ind=stTracelog.svc_ind;/* 内部帐存取 */
		    	strcpy(g_pub_tx.ac_get_ind,"000");/**存取款***/
		    	strcpy(g_pub_tx.brf,stTracelog.brf);
		    	strcat(g_pub_tx.brf,"冲正");
		    	iRet = pub_acct_trance();
		    	if ( iRet )
		    	{
				if(tx_date==com_sys_parm.sys_date)/*当日*/
				{
					Trace_log_Clo_Sel();
				}
				else /*隔N日 N>1*/
				{
					Trace_log_rz_Clo_Sel();
				}
		    	    sprintf(acErrMsg,"调用存取款主控失败!");
		    	    WRITEMSG
		    	    strcpy(g_pub_tx.reply,"T999");
		    	    goto ErrExit;
		    	}
				if( stDclog.dc_ind[0] == '2')/*贷方记账*/ 
		    	{
		    	    set_zd_data("1214",stTracelog.cur_no);
		    	    set_zd_data("1215",stTracelog.ct_ind );
		    	    set_zd_double("1218",g_pub_tx.tx_amt1);
		    	    iRet = pubf_acct_proc("A017");
		    	    if (iRet)
		    	    {
		    	  		if(tx_date==com_sys_parm.sys_date)/*当日*/
						{
							Trace_log_Clo_Sel();
						}
						else /*隔N日 N>1*/
						{
							Trace_log_rz_Clo_Sel();
						}
		    	        sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
		    	        WRITEMSG
		    	        strcpy(g_pub_tx.reply,"T999");
		    	        goto ErrExit;
		    	    }
		    	 }
				else if( stDclog.dc_ind[0] == '1')  /*借方记账*/
		    	{ 
		    	    set_zd_data("1204",stTracelog.cur_no);
		    	    set_zd_data("1205",stDclog.ct_ind );
		    	    set_zd_double("1208",g_pub_tx.tx_amt1);
		    	    iRet = pubf_acct_proc("A016");
		    	    if (iRet)
		    	    {
		    	  		if(tx_date==com_sys_parm.sys_date)/*当日*/
						{
							Trace_log_Clo_Sel();
						}
						else /*隔N日 N>1*/
						{
							Trace_log_rz_Clo_Sel();
						}
		    	        sprintf(acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply);
		    	        WRITEMSG
		    	        strcpy(g_pub_tx.reply,"T999");
		    	        goto ErrExit;
		    	    }
		    	}
			}
			else/*非内部帐和科目，按活期户处理*/
			{
		    	g_pub_tx.ac_id=stTracelog.ac_id;
		    	g_pub_tx.ac_seqn=stTracelog.ac_seqn;
		    	g_pub_tx.ac_id_type[0]='1';
		    	strncpy(g_pub_tx.opn_br_no, stTracelog.opn_br_no ,sizeof(g_pub_tx.opn_br_no)-1);
		       	if(!memcmp(stDclog.acc_hrt,"10101",5))
		      	{/*现金账先冲正到清算的应解汇款账号上去*/
		      	 	pub_base_GetParm_Str("10001",10,g_pub_tx.ac_no);
		      	}
		      	else
		      	{
					strcpy(g_pub_tx.ac_no,stTracelog.ac_no);
				}
				g_pub_tx.tx_amt1=0-stTracelog.amt;
		    	strcpy(g_pub_tx.cur_no,"01");    /* 币种 */
		    	/*strcpy(g_pub_tx.ct_ind,stDclog.dc_ind); 现转*/
		    	g_pub_tx.ct_ind[0]='2';/*转账*/
		    	if( stDclog.dc_ind[0] == '1' )
		    	{
		    		strcpy(g_pub_tx.add_ind,"0");/*增减(借贷):0减(借) 1加(贷)*/
		    	}
		    	else if( stDclog.dc_ind[0] == '2')
		    	{
		    		strcpy(g_pub_tx.add_ind,"1");/*增减(借贷):0减(借) 1加(贷)*/
		    	}
		    	strcpy(g_pub_tx.prt_ind,"0");/*是否打折标志*/
		    	strcpy(g_pub_tx.hst_ind,"1");      /**日间入明细**/
		    	strcpy(g_pub_tx.ac_wrk_ind,"0000000");
		    	g_pub_tx.svc_ind=1001;             /**存取款***/
		    	strcpy(g_pub_tx.ac_get_ind,"000");
		    	strcpy(g_pub_tx.brf,stTracelog.brf);
		    	strcat(g_pub_tx.brf,"冲正");
		    	iRet=pub_acct_trance();
		    	if (iRet)
		    	{
		    		if(tx_date==com_sys_parm.sys_date)/*当日*/
					{
						Trace_log_Clo_Sel();
					}
					else /*隔N日 N>1*/
					{
						Trace_log_rz_Clo_Sel();
					}
		    	    sprintf(acErrMsg,"调用存取款主控失败!");
		    	    WRITEMSG
		    	    strcpy(g_pub_tx.reply,"T999");
		    	    goto ErrExit;
		    	}
		    	if( stDclog.dc_ind[0] == '2')/*贷方记账*/ 
		    	{
		    	    set_zd_data("101A",stTracelog.cur_no);
		    	    set_zd_data("101B", stTracelog.ct_ind);
		    	    set_zd_double("1013", g_pub_tx.tx_amt1);
		    	    iRet = pubf_acct_proc("D099");
		    	    if (iRet)
		    	    {
						if(tx_date==com_sys_parm.sys_date)/*当日*/
						{
							Trace_log_Clo_Sel();
						}
						else /*隔N日 N>1*/
						{
							Trace_log_rz_Clo_Sel();
						}
		    	    	vtcp_log("%s,%d,记账错误!ret[%ld]",__FILE__,__LINE__,iRet);
		    	    	strcpy(g_pub_tx.reply,"T999");
		    	    	goto ErrExit;
		    	    }
		    	}
		    	else if(  stDclog.dc_ind[0] == '1')  /*借方记账*/ 
		    	{
		    	    set_zd_data("102J", stTracelog.cur_no);
		    	    set_zd_data("102K", stTracelog.ct_ind);
		    	    set_zd_double("102F", g_pub_tx.tx_amt1);
		    	    iRet = pubf_acct_proc("D003");
		    	    if (iRet)
		    	    {
						if(tx_date==com_sys_parm.sys_date)/*当日*/
						{
							Trace_log_Clo_Sel();
						}
						else /*隔N日 N>1*/
						{
							Trace_log_rz_Clo_Sel();
						}
		    	        vtcp_log("%s,%d,记账错误!ret[%ld]",__FILE__,__LINE__,iRet);
		    	        strcpy(g_pub_tx.reply,"T999");
		    	        goto ErrExit;
		    	    }
		    	}  
			}
			icount++;
		}/*while222 end*/
		if(tx_date == com_sys_parm.sys_date)/*当日*/
		{
			Trace_log_Clo_Sel();
		}
		else /*隔N日 N>1*/
		{
			Trace_log_rz_Clo_Sel();
		}
		
		iRet = sql_execute("update chnl_single_cnaps set sts ='4' where tx_date=%ld and trace_no=%ld and BBMK='BEPS' and pt_date=%ld and pt_trace_no=%ld and sts='1'",\
			stCzfn[truecnt].ohxdate,stCzfn[truecnt].ohxtrace,stCzfn[truecnt].optdate,stCzfn[truecnt].opttrace);
        if(iRet)
        {
            sprintf(acErrMsg, "修改单笔记账登记簿错误![%d]",iRet);
            WRITEMSG
            goto ErrExit;
        }/*冲账后修改交易状态,Added by YangMeng 2014/7/24 13:27:53*/
        vtcp_log("[%s],[%d],该交易冲账完成!",__FILE__,__LINE__);
	}/*while111 end*/
	
OkExit:
	sql_commit();
	if(stCzfn!=NULL)
	free(stCzfn);
	vtcp_log("%s,%d,批量冲正记账成功!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,g_pub_tx.reply);
	if(fp!=NULL)
	fclose(fp);
	return(0);
ErrExit:
	sql_rollback();
	if(stCzfn!=NULL)
	free(stCzfn);
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	set_zd_data(DC_GET_MSG,acErrMsg);
	vtcp_log("%s,%d,批量冲正记账失败!",__FILE__,__LINE__);
	if(fp!=NULL)
	fclose(fp);
	return(1);
}
