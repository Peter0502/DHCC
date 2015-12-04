/*************************************************************
* 文 件 名: spL323.c
* 功能描述：将柜面上提供的减值相对应的贷款，进行减值账务的处理
主要是新贷款存量数据的处理
*
* 作    者: lpj
* 完成日期: 20151120
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容: 
* 
int bj_jzzc(char *ac_no,char *old_prod_no,char *new_prod_no)
贷款账号，旧产品号，新产品号
int bj_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
贷款账号，旧产品号，新产品号

**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mdm_ac_rel_c.h"
#include "ln_parm_c.h"
#include "ln_mst_c.h"
#include "com_item_c.h"
#include "new_jz_prdt_c.h"
#include "dc_acc_rel_c.h"
#include "com_branch_c.h"
#include "trace_log_c.h"
#include "dc_log_c.h"
#include "cif_prdt_rel_c.h"
static	char	 prod_no[4];
char type[2];/*操作标志，1-正常转减值，2-减值转正常*/
spL323()
{
		int ret=0;
		int flag=0;/*减值完成标志，0未减值，1完成减值*/
		char cAc_no[19];
		char ln_acno[19];
		char old_prdt_no[4];
		char new_prdt_no[4];
	
		char prdt_no[4];/*转换的产品号*/
		char wherelist[1024];
		char tmpstr[512];
		struct new_jz_prdt_c sNew_jz_prdt;
		struct mdm_ac_rel_c sMdm_ac_rel;
		struct ln_parm_c sLn_parm;
		struct ln_mst_c sLn_mst;
		struct com_item_c sCom_item;
		struct dc_acc_rel_c sDc_acc_rel;
	
		
		
		memset(&sNew_jz_prdt,0x00,sizeof(sNew_jz_prdt));
		memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
		memset(&sLn_parm,0x00,sizeof(sLn_parm));
		memset(&sLn_mst,0x00,sizeof(sLn_mst));
		memset(&sCom_item,0x00,sizeof(sCom_item));
		memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));	
		memset(wherelist,0x00,sizeof(wherelist));
		memset(tmpstr,0x00,sizeof(tmpstr));	
		/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
		get_zd_data("0310" , ln_acno); 		/* 贷款账号 */
		get_zd_data("0930" , type); 			/* 操作类型1-正常转减值，2-减值转正常 */
		get_zd_data("0240" , prdt_no); 			/* 新产品代码 */
	
		if(type[0]=='1')
		{
				sprintf(tmpstr,"ac_no='%s' and new_prdt_no='%s' and sts='0' ",ln_acno,prdt_no);	
				
		}else if(type[0]=='2')
		{
			sprintf(tmpstr,"ac_no='%s' and old_prdt_no='%s' and sts='1'",ln_acno,prdt_no);	
		}
		strcat(wherelist,tmpstr);
		ret=New_jz_prdt_Dec_Upd(g_pub_tx.reply,wherelist);
		if(ret)
		{
			sprintf(acErrMsg,"贷款新减值产品对照表的游标定义出错[%s]",g_pub_tx.reply);
			WRITEMSG
		
			goto ErrExit;
		}
		
			memset(&sNew_jz_prdt,0x00,sizeof(sNew_jz_prdt));
			memset(&sLn_mst,0x00,sizeof(sLn_mst));
			memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
			ret=New_jz_prdt_Fet_Upd(&sNew_jz_prdt,g_pub_tx.reply);
			 if(ret)
			{
					sprintf(acErrMsg,"查询贷款新旧产品对照表出错[%s]",g_pub_tx.reply);
					WRITEMSG
					strcpy( g_pub_tx.reply ,"L302" );
					New_jz_prdt_Clo_Sel();
					goto ErrExit;
			}
			
					ret=Ln_mst_Sel(g_pub_tx.reply,&sLn_mst,"pact_no='%s'and ac_sts not in('*','9')",sNew_jz_prdt.pact_no);
					if(ret)
					{
							sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
							WRITEMSG
							strcpy( g_pub_tx.reply ,"W019" );
							goto ErrExit;
					}
						
					
					ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld ",sLn_mst.ac_id);
					if(ret&&ret!=100)
					{
							sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
							WRITEMSG
							strcpy( g_pub_tx.reply ,"L182" );
							goto ErrExit;
					}
					
					strcpy(cAc_no,sMdm_ac_rel.ac_no);
								
					if(pub_base_CompDblVal(sLn_mst.bal,sNew_jz_prdt.bj_amt)!=0||pub_base_CompDblVal(sLn_mst.in_lo_intst,sNew_jz_prdt.in_lo_intst)!=0\
									||pub_base_CompDblVal(sLn_mst.out_lo_intst,sNew_jz_prdt.out_lo_intst)!=0)
					{	
						vtcp_log("%s,%ld 金额不符，旧本金【%16.2f】,新本金【%16.2f】,旧表内息【%16.2f】,新表内息【%16.2f】\
														旧表外息【%16.2f】,新表外息【%16.2f】 直接跳过！！！！%s",__FILE__,__LINE__,sLn_mst.bal,sNew_jz_prdt.bj_amt,\
														sLn_mst.in_lo_intst,sNew_jz_prdt.in_lo_intst,sLn_mst.out_lo_intst,sNew_jz_prdt.out_lo_intst,sMdm_ac_rel.ac_no);
						sprintf( acErrMsg, "金额不符",ret );
							WRITEMSG
							strcpy( g_pub_tx.reply ,"P008" );
							goto ErrExit;
					}		
					
					
					{
						if(pub_base_CompDblVal(sLn_mst.bal,0.00)>0||pub_base_CompDblVal(sLn_mst.in_lo_intst,0.00)>0\
										||pub_base_CompDblVal(sLn_mst.out_lo_intst,0.00)>0)
							
							{
								if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
									{
    					        sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
    					        WRITEMSG
    					        strcpy( g_pub_tx.reply ,"P298" );
    					        goto ErrExit;
									}
									g_pub_tx.trace_cnt=0;
									vtcp_log("cif_no=[%ld],trace_no=%d",sLn_mst.cif_no,g_pub_tx.trace_no);
							}else
							{
									vtcp_log("不需要记账");	
									TRACE
									sNew_jz_prdt.sts[0]='1';
									ret=New_jz_prdt_Upd_Upd(sNew_jz_prdt,g_pub_tx.reply);
									if(ret)
										{
										sprintf( acErrMsg, "执行New_jz_prdt_Upd_Upd出错!ret=[%d]",ret );
												WRITEMSG
											strcpy( g_pub_tx.reply ,"L304" );
												goto ErrExit;
									}

							}
					}
						if(type[0]=='1')
						{
								strcpy(old_prdt_no,sNew_jz_prdt.old_prdt_no);	/* 正常产品号*/
								strcpy(new_prdt_no,sNew_jz_prdt.new_prdt_no);	/* 减值产品号*/
						}else if(type[0]=='2')
						{
								strcpy(old_prdt_no,sNew_jz_prdt.new_prdt_no);	/* 减值产品号*/
								strcpy(new_prdt_no,sNew_jz_prdt.old_prdt_no);	/* 正常产品号*/
						}
						if(strcmp(sNew_jz_prdt.old_prdt_no,sNew_jz_prdt.new_prdt_no)!=0)
						{
									ret=bj_jzzc(cAc_no,old_prdt_no,new_prdt_no);
									if(ret&&ret!=2)
									{
										sprintf( acErrMsg, "利息科目记账转出出错!ret=[%d]",ret );
											WRITEMSG
										strcpy( g_pub_tx.reply ,"L304" );
											goto ErrExit;
									}
									if(ret==2)
									{
										vtcp_log("%s,%ld  零金额不记账，ac_no=%s",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
										
									}
									ret=bnlx_jzzc(cAc_no,old_prdt_no,new_prdt_no);
									if(ret&&ret!=2)
									{
										sprintf( acErrMsg, "利息科目记账转出出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}
									if(ret==2)
									{
										vtcp_log("%s,%ld,零金额不记账，ac_no='%s'",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
											
									}
									ret=bwlx_jzzc(cAc_no,old_prdt_no,new_prdt_no);
									if(ret&&ret!=2)
									{
										sprintf( acErrMsg, "利息科目记账转出出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}
									if(ret==2)
									{
										vtcp_log("%s,%ld,零金额不记账，ac_no='%s'",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
											
									}
									/*开始打印转入记账得凭证*/
									ret=print_LNBKCPZC( );
									if(ret)
   								{
   													sprintf( acErrMsg, "执行print_LNBKCPZC出错!ret=[%d]",ret );
																WRITEMSG
															
																goto ErrExit;
   								}
									ret=bj_jzzr(cAc_no,old_prdt_no,new_prdt_no);
									if(ret&&ret!=2)
									{
										sprintf( acErrMsg, "利息科目记账转出出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}
									if(ret==2)
									{
										vtcp_log("%s,%ld  零金额不记账，ac_no=%s",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
										
									}
									
									ret=bnlx_jzzr(cAc_no,old_prdt_no,new_prdt_no);
									if(ret&&ret!=2)
									{
										sprintf( acErrMsg, "利息科目记账转入出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}
									if(ret==2)
									{
										vtcp_log("%s,%ld 零金额不记账，ac_no='%s'",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
											
									}
									ret=bwlx_jzzr(cAc_no,old_prdt_no,new_prdt_no);
									if(ret&&ret!=2)
									{
										sprintf( acErrMsg, "利息科目记账转入出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}
									if(ret==2)
									{
										vtcp_log("%s,%ld 零金额不记账，ac_no='%s'",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
											
									}
									/*开始打印转入记账得凭证*/
   								ret=print_LNBKCPZR();
   								if(ret)
   								{
   													sprintf( acErrMsg, "执行print_LNBKCPZR出错!ret=[%d]",ret );
																WRITEMSG
															
																goto ErrExit;
   								}
						}
					vtcp_log("记账走到这儿了，说明记账完成！%s,%d",__FILE__,__LINE__);	
					sNew_jz_prdt.ac_id=sLn_mst.ac_id;
					sNew_jz_prdt.tx_date=g_pub_tx.tx_date;
					sNew_jz_prdt.trace_no=g_pub_tx.trace_no;
					strcpy(sNew_jz_prdt.tel,g_pub_tx.tel);
					strcpy(sNew_jz_prdt.tx_br_no,g_pub_tx.tx_br_no);
					if(type[0]=='1')
					{
						sNew_jz_prdt.sts[0]='1';
					}else if(type[0]=='2')
					{
						sNew_jz_prdt.sts[0]='0';
					}
					ret=New_jz_prdt_Upd_Upd(sNew_jz_prdt,g_pub_tx.reply);
					if(ret)
					{
						sprintf( acErrMsg, "执行New_jz_prdt_Upd_Upd出错!ret=[%d]",ret );
								WRITEMSG
							strcpy( g_pub_tx.reply ,"L304" );
								goto ErrExit;
					}
   New_jz_prdt_Clo_Upd();
   
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
/*贷款本金科目的减值转入记账，借 正金额*/

int bj_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];

	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	struct	cif_prdt_rel_c	cif_prdt_rel;		

	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
    
	memset ( &cif_prdt_rel , 0x00 , sizeof( struct cif_prdt_rel_c ) );
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    memset(Data_code    , 0 , sizeof(Data_code));
    memset(cDc_code    , 0 , sizeof(cDc_code));
	
		
	memset ( &ln_mst_new , 0x00 , sizeof( struct ln_mst_c ) );
	memset ( &ln_parm_new , 0x00 , sizeof( struct ln_parm_c ) );
				

	strcpy(g_pub_tx.ac_no,ac_no); 		/* 贷款账号 */
	g_pub_tx.ac_seqn = 0;				/* 账号序号 */
	strcpy(prod_no,new_prod_no);
	
	vtcp_log("bj_jzzr,trace_no=%d,new_prod_no=%s",g_pub_tx.trace_no,new_prod_no);
		
	/** 处理流程 **/
	

	/* 取新产品信息 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret ){
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		return -1;
    	}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG		
	TRACE
		

	
	/*开始登记流水和记账分录流水表*/
	strcpy(g_pub_tx.ac_no,ac_no);/*贷款账号*/
	g_pub_tx.ac_seqn=0;
	
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
	
			
	/* 更新贷款主文件 */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
    
	ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "贷款主文件中无此记录" );
		WRITEMSG
		goto ErrExit;
	}else if ( ret ){
		sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		strcpy( g_pub_tx.reply ,"W018" );
		goto ErrExit;
	}

	strcpy( g_ln_mst.prdt_no , prod_no );   		/* 产品代码 */
	
	
		
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		goto ErrExit;
	}else if( ret ){
		sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		goto ErrExit;
	}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG
	
	/*  修改客户-产品关系表 */
	ret = Cif_prdt_rel_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and cif_no=%ld" ,g_ln_mst.ac_id , g_ln_mst.cif_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Cif_prdt_rel_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D101");
		goto ErrExit;
	}
    
	ret = Cif_prdt_rel_Fet_Upd( &cif_prdt_rel,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "客户-产品关系表中无此记录" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D104");
		Cif_prdt_rel_Clo_Upd();
	}else if ( ret ){
		sprintf( acErrMsg, "执行Cif_prdt_rel_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"AT06");
		goto ErrExit;
	}else if( !ret ){

		strcpy( cif_prdt_rel.prdt_code , prod_no );   		/* 产品代码 */
			
		ret = Cif_prdt_rel_Upd_Upd( cif_prdt_rel, g_pub_tx.reply );
		if ( ret == -239 )
		{
			sprintf( acErrMsg, "修改客户-产品关系表错,存在重复记录!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"L149");
			goto ErrExit;
		}else if( ret ){
			sprintf( acErrMsg,"执行Cif_prdt_rel_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}
		Cif_prdt_rel_Clo_Upd();
		sprintf(acErrMsg,"更新客户-产品关系表 PASS!");
		WRITEMSG		
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             strcpy( g_pub_tx.reply ,"L037" );
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
        	
		if(pub_base_CompDblVal(g_ln_mst.bal,0.00)==0)
		{
    			      sprintf(acErrMsg,"金额为零，不进行记流水！ac_no='%s'",g_mdm_ac_rel.ac_no);
    			      WRITEMSG
    			      goto OkExit1;
						
		}						
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	sTrace_log.amt=g_ln_mst.bal;
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=g_pub_tx.tx_time;
	strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"3322");
	strcpy(sTrace_log.sub_tx_code,"L323");
	sTrace_log.svc_ind=0;
	strcpy(sTrace_log.cur_no,"01");
	strcpy(sTrace_log.prdt_no,new_prod_no);
	strcpy(sTrace_log.ac_no,ac_no);
	sTrace_log.ac_seqn=0;
	sTrace_log.ac_id=g_ln_mst.ac_id;
	strcpy(sTrace_log.note_type,"0");
	strcpy(sTrace_log.note_no,"");
	sTrace_log.add_ind[0]='1';
	
	sTrace_log.saving_notarize[0]='0';
	sTrace_log.ct_ind[0]='2';
	strcpy(sTrace_log.brf,"贷款本金减值转入");
	strcpy(sTrace_log.tel,g_pub_tx.tel);
	sTrace_log.hst_ind[0]='0';
	sTrace_log.no_show[0]='0';
	strcpy(sTrace_log.sts,"0");
	Trace_log_Debug(&sTrace_log);
	ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入流水表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"P126" );
		goto ErrExit;
	}
	/*登记流水分录表*/
	sDc_log.tx_date=g_pub_tx.tx_date;
	sDc_log.trace_no=g_pub_tx.trace_no;
	sDc_log.trace_cnt=g_pub_tx.trace_cnt;
	sDc_log.dc_seqn=1;
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_type='6'");
	if(ret)
	{
		sprintf(acErrMsg,"查询机构表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D172" );
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sDc_log.cur_no,"01");
	
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0152'",ln_parm_new.dc_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",g_ln_parm.dc_code);
            WRITEMSG
            strcpy( g_pub_tx.reply ,"L154" );
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
        sDc_log.amt=g_ln_mst.bal;
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,g_pub_tx.tel);	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
            WRITEMSG
            strcpy( g_pub_tx.reply ,"P126" );
            vtcp_log("[%s][%d] errcode=[%d],%s",__FILE__,__LINE__,ret,g_pub_tx.reply);
            goto ErrExit;
			}
	

OkExit:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
OkExit1:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "零金额，处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 2;

ErrExit:
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}
/*贷款本金科目的减值记账，借 负金额*/
int bj_jzzc(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	int tag=0;
	int amt_flag=0;
	double old_ln_amt=0.00;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    memset(Data_code    , 0 , sizeof(Data_code));
    memset(cDc_code    , 0 , sizeof(cDc_code));
	memset ( &ln_mst_new , 0x00 , sizeof( struct ln_mst_c ) );
	memset ( &ln_parm_new , 0x00 , sizeof( struct ln_parm_c ) );
	
	
	strcpy(g_pub_tx.ac_no,ac_no); 		/* 贷款账号 */
	g_pub_tx.ac_seqn = 0;				/* 账号序号 */
	strcpy(prod_no,new_prod_no);
	vtcp_log("bj_jzzc,trace_no=%d",g_pub_tx.trace_no);

	/** 处理流程 **/
	
	/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG	

	old_ln_amt = g_ln_mst.bal ;
	
	/* 取新产品信息 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'", prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		goto ErrExit;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
    		goto ErrExit;
    		}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"313此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG	

	

/*开始登记流水和记账分录流水表*/
	strcpy(g_pub_tx.ac_no,ac_no);/*贷款账号*/
	g_pub_tx.ac_seqn=0;
	
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             strcpy( g_pub_tx.reply ,"L037" );
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
   if(pub_base_CompDblVal(g_ln_mst.bal,0.00)==0)
		{
    			      sprintf(acErrMsg,"金额为零，不进行记流水！ac_no='%s'",g_mdm_ac_rel.ac_no);
    			      WRITEMSG
    			      goto OkExit1;
						
		}	
				
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	sTrace_log.amt=-g_ln_mst.bal;
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=g_pub_tx.tx_time;
	strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"3322");
	strcpy(sTrace_log.sub_tx_code,"L323");
	sTrace_log.svc_ind=0;
	strcpy(sTrace_log.cur_no,"01");
	strcpy(sTrace_log.prdt_no,old_prod_no);
	strcpy(sTrace_log.ac_no,ac_no);
	sTrace_log.ac_seqn=0;
	sTrace_log.ac_id=g_ln_mst.ac_id;
	strcpy(sTrace_log.note_type,"0");
	strcpy(sTrace_log.note_no,"");
	sTrace_log.add_ind[0]='1';
	
	sTrace_log.saving_notarize[0]='0';
	sTrace_log.ct_ind[0]='2';
	strcpy(sTrace_log.brf,"贷款本金减值转出");
	strcpy(sTrace_log.tel,g_pub_tx.tel);
	sTrace_log.hst_ind[0]='0';
	sTrace_log.no_show[0]='0';
	strcpy(sTrace_log.sts,"0");
	Trace_log_Debug(&sTrace_log);
	ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入流水表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"P126" );
		goto ErrExit;
	}
	/*登记流水分录表*/
	sDc_log.tx_date=g_pub_tx.tx_date;
	sDc_log.trace_no=g_pub_tx.trace_no;
	sDc_log.trace_cnt=g_pub_tx.trace_cnt;
	sDc_log.dc_seqn=1;
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_type='6'");
	if(ret)
	{
		sprintf(acErrMsg,"查询机构表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D172" );
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sDc_log.cur_no,"01");
	/*dc_code来确定*/
	
    TRACE
        strcpy(cDc_code,g_ln_parm.dc_code); 
  
    
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0152'",cDc_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",g_ln_parm.dc_code);
            WRITEMSG
            strcpy( g_pub_tx.reply ,"L154" );
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
        sDc_log.amt=-g_ln_mst.bal;
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,g_pub_tx.tel);	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
						strcpy( g_pub_tx.reply ,"P126" );
            WRITEMSG
            vtcp_log("[%s][%d] errcode=[%d],%s",__FILE__,__LINE__,ret,g_pub_tx.reply);
            goto ErrExit;
			}
	


	

OkExit:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
OkExit1:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "零金额，处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 2;

ErrExit:
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}
/*贷款表内利息科目的减值转出记账，借 表内科目 负金额*/
int bnlx_jzzc(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    
    memset(cDc_code    , 0 , sizeof(cDc_code));
    memset(Data_code    , 0 , sizeof(Data_code));
	

		
	
	strcpy(g_pub_tx.ac_no,ac_no);/*贷款账号*/
	g_pub_tx.ac_seqn=0;
	
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
	/*判断账户状态是否正常*/
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             strcpy( g_pub_tx.reply ,"L037" );
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
						
    if(pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)==0)
		{
    			      sprintf(acErrMsg,"金额为零，不进行记流水！ac_no='%s'",g_mdm_ac_rel.ac_no);
    			      WRITEMSG
    			      goto OkExit1;
						
		}	    	 
						sTrace_log.amt=(-1)*g_ln_mst.in_lo_intst;
        	  sDc_log.amt=-g_ln_mst.in_lo_intst;
        
        
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=g_pub_tx.tx_time;
	strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"3322");
	strcpy(sTrace_log.sub_tx_code,"L323");
	sTrace_log.svc_ind=0;
	strcpy(sTrace_log.cur_no,"01");
	strcpy(sTrace_log.prdt_no,old_prod_no);
	strcpy(sTrace_log.ac_no,ac_no);
	sTrace_log.ac_seqn=0;
	sTrace_log.ac_id=g_ln_mst.ac_id;
	strcpy(sTrace_log.note_type,"0");
	strcpy(sTrace_log.note_no,"");
	sTrace_log.add_ind[0]='1';
	
	sTrace_log.ct_ind[0]='2';
	strcpy(sTrace_log.brf,"贷款表内息减值转出");
	strcpy(sTrace_log.tel,g_pub_tx.tel);
	sTrace_log.hst_ind[0]='0';
	sTrace_log.no_show[0]='0';
	strcpy(sTrace_log.sts,"0");
	sTrace_log.saving_notarize[0]='0';
	Trace_log_Debug(&sTrace_log);
	ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入流水表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"P126" );
		goto ErrExit;
	}
	/*登记流水分录表*/
	sDc_log.tx_date=g_pub_tx.tx_date;
	sDc_log.trace_no=g_pub_tx.trace_no;
	sDc_log.trace_cnt=g_pub_tx.trace_cnt;
	sDc_log.dc_seqn=1;
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_type='6'");
	if(ret)
	{
		sprintf(acErrMsg,"查询机构表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D172" );
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sDc_log.cur_no,"01");
	ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0155'",g_ln_parm.dc_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",g_ln_parm.dc_code);
            WRITEMSG
            strcpy( g_pub_tx.reply ,"L154" );
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,g_pub_tx.tel);	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
            WRITEMSG
            strcpy( g_pub_tx.reply ,"P126" );
            vtcp_log("[%s][%d] errcode=[%d],%s",__FILE__,__LINE__,ret,g_pub_tx.reply);
            goto ErrExit;
			}
	

OkExit:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
OkExit1:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "零金额，处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 2;

ErrExit:
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}
/*贷款应收利息科目的减值转入记账，借 减值应收利息科目 正金额*/
int bnlx_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct ln_parm_c ln_parm_new;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&ln_parm_new,0x00,sizeof(ln_parm_new));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    memset(Data_code    , 0 , sizeof(Data_code));
    memset(cDc_code    , 0 , sizeof(cDc_code));
	

	
	strcpy(g_pub_tx.ac_no,ac_no);/*贷款账号*/
	g_pub_tx.ac_seqn=0;
	/* 取新产品信息 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'",new_prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret ){
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		return -1;
    	}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG		
	TRACE
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             strcpy( g_pub_tx.reply ,"L037" );
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
        	
				 if(pub_base_CompDblVal(g_ln_mst.in_lo_intst,0.00)==0)
		{
    			      sprintf(acErrMsg,"金额为零，不进行记流水！ac_no='%s'",g_mdm_ac_rel.ac_no);
    			      WRITEMSG
    			      goto OkExit1;
						
		}	
        	
						sTrace_log.amt=g_ln_mst.in_lo_intst;
        
        
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=g_pub_tx.tx_time;
	strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"3322");
	strcpy(sTrace_log.sub_tx_code,"L323");
	sTrace_log.svc_ind=0;
	strcpy(sTrace_log.cur_no,"01");
	strcpy(sTrace_log.prdt_no,new_prod_no);
	strcpy(sTrace_log.ac_no,ac_no);
	sTrace_log.ac_seqn=0;
	sTrace_log.ac_id=g_ln_mst.ac_id;
	strcpy(sTrace_log.note_type,"0");
	strcpy(sTrace_log.note_no,"");
	sTrace_log.add_ind[0]='1';
	
	sTrace_log.saving_notarize[0]='0';
	sTrace_log.ct_ind[0]='2';
	strcpy(sTrace_log.brf,"贷款表内息减值转入");
	strcpy(sTrace_log.tel,g_pub_tx.tel);
	sTrace_log.hst_ind[0]='0';
	sTrace_log.no_show[0]='0';
	strcpy(sTrace_log.sts,"0");
	Trace_log_Debug(&sTrace_log);
	ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入流水表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"P126" );
		goto ErrExit;
	}
	/*登记流水分录表*/
	sDc_log.tx_date=g_pub_tx.tx_date;
	sDc_log.trace_no=g_pub_tx.trace_no;
	sDc_log.trace_cnt=g_pub_tx.trace_cnt;
	sDc_log.dc_seqn=1;
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_type='6'");
	if(ret)
	{
		sprintf(acErrMsg,"查询机构表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D172" );
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sDc_log.cur_no,"01");
	
        	  memcpy(cDc_code,ln_parm_new.dc_code,sizeof(ln_parm_new.dc_code)-1);
        	  strcpy(Data_code,"0155");
        	  sDc_log.amt=g_ln_mst.in_lo_intst;
        
       
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='%s'",cDc_code,Data_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",cDc_code);
            WRITEMSG
            strcpy( g_pub_tx.reply ,"L154" );
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,g_pub_tx.tel);	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
            WRITEMSG
            strcpy( g_pub_tx.reply ,"P126" );
            vtcp_log("[%s][%d] errcode=[%d],%s",__FILE__,__LINE__,ret,g_pub_tx.reply);
            goto ErrExit;
			}
	

OkExit:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
OkExit1:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "零金额，处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 2;

ErrExit:
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;

}
/*贷款表外利息科目的减值转出记账，贷 表外科目 负金额*/
int bwlx_jzzc(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    
    memset(cDc_code    , 0 , sizeof(cDc_code));
    memset(Data_code    , 0 , sizeof(Data_code));
	

		
	
	strcpy(g_pub_tx.ac_no,ac_no);/*贷款账号*/
	g_pub_tx.ac_seqn=0;
	
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
	/*判断账户状态是否正常*/
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             strcpy( g_pub_tx.reply ,"L037" );
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
				 /**表外欠息*/
        {
        	 if(pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)==0)
		{
    			      sprintf(acErrMsg,"金额为零，不进行记流水！ac_no='%s'",g_mdm_ac_rel.ac_no);
    			      WRITEMSG
    			      goto OkExit1;
						
		}	
						sTrace_log.amt=(-1)*g_ln_mst.out_lo_intst;
        	  sDc_log.amt=-g_ln_mst.out_lo_intst;
        }
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=g_pub_tx.tx_time;
	strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"3322");
	strcpy(sTrace_log.sub_tx_code,"L323");
	sTrace_log.svc_ind=0;
	strcpy(sTrace_log.cur_no,"01");
	strcpy(sTrace_log.prdt_no,old_prod_no);
	strcpy(sTrace_log.ac_no,ac_no);
	sTrace_log.ac_seqn=0;
	sTrace_log.ac_id=g_ln_mst.ac_id;
	strcpy(sTrace_log.note_type,"0");
	strcpy(sTrace_log.note_no,"");
	sTrace_log.add_ind[0]='1';
	
	sTrace_log.ct_ind[0]='2';
	strcpy(sTrace_log.brf,"贷款表外息减值转出");
	strcpy(sTrace_log.tel,g_pub_tx.tel);
	sTrace_log.hst_ind[0]='0';
	sTrace_log.no_show[0]='0';
	strcpy(sTrace_log.sts,"0");
	sTrace_log.saving_notarize[0]='0';
	Trace_log_Debug(&sTrace_log);
	ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入流水表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"P126" );
		goto ErrExit;
	}
	/*登记流水分录表*/
	sDc_log.tx_date=g_pub_tx.tx_date;
	sDc_log.trace_no=g_pub_tx.trace_no;
	sDc_log.trace_cnt=g_pub_tx.trace_cnt;
	sDc_log.dc_seqn=1;
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_type='6'");
	if(ret)
	{
		sprintf(acErrMsg,"查询机构表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D172" );
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sDc_log.cur_no,"01");
	ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0156'",g_ln_parm.dc_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",cDc_code);
            WRITEMSG
            strcpy( g_pub_tx.reply ,"L154" );
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='2';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,g_pub_tx.tel);	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
            WRITEMSG
            strcpy( g_pub_tx.reply ,"P126" );
            vtcp_log("[%s][%d] errcode=[%d],%s",__FILE__,__LINE__,ret,g_pub_tx.reply);
            goto ErrExit;
			}
	

OkExit:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
OkExit1:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "零金额，处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 2;

ErrExit:
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;
}
/*贷款表外利息科目的减值转入记账，贷 减值表外利息科目 正金额*/
int bwlx_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	struct ln_parm_c ln_parm_new;
	char   cDc_code[5];
	char Data_code[5];

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&ln_parm_new,0x00,sizeof(ln_parm_new));
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(cAcc_hrt1   , 0 , sizeof(cAcc_hrt1));
    memset(cAcc_hrt2   , 0 , sizeof(cAcc_hrt2));
    memset(cAcc_hrt3   , 0 , sizeof(cAcc_hrt3));
    memset(cAcc_hrt4   , 0 , sizeof(cAcc_hrt4));
    memset(Data_code    , 0 , sizeof(Data_code));
    memset(cDc_code    , 0 , sizeof(cDc_code));
	

	
	strcpy(g_pub_tx.ac_no,ac_no);/*贷款账号*/
	g_pub_tx.ac_seqn=0;
	/* 取新产品信息 */
	ret = Ln_parm_Sel( g_pub_tx.reply , &ln_parm_new, "prdt_no='%s'", new_prod_no);		
	if( ret==100 )
	{
		sprintf( acErrMsg,"贷款产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L001" );
		return 1;
	}else if( ret ){
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		return -1;
    	}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG		
	TRACE
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L021" );
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             strcpy( g_pub_tx.reply ,"L037" );
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
        	
	 /**表外欠息*/
        {
        		
        	  if(pub_base_CompDblVal(g_ln_mst.out_lo_intst,0.00)==0)
						{
    			      sprintf(acErrMsg,"金额为零，不进行记流水！ac_no='%s'",g_mdm_ac_rel.ac_no);
    			      WRITEMSG
    			      goto OkExit1;
						
						}	
						sTrace_log.amt=g_ln_mst.out_lo_intst;
        	 
        }
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=g_pub_tx.tx_time;
	strcpy(sTrace_log.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"3322");
	strcpy(sTrace_log.sub_tx_code,"L323");
	sTrace_log.svc_ind=0;
	strcpy(sTrace_log.cur_no,"01");
	strcpy(sTrace_log.prdt_no,new_prod_no);
	strcpy(sTrace_log.ac_no,ac_no);
	sTrace_log.ac_seqn=0;
	sTrace_log.ac_id=g_ln_mst.ac_id;
	strcpy(sTrace_log.note_type,"0");
	strcpy(sTrace_log.note_no,"");
	sTrace_log.add_ind[0]='1';
	
	sTrace_log.saving_notarize[0]='0';
	sTrace_log.ct_ind[0]='2';
	strcpy(sTrace_log.brf,"贷款表外息减值转入");
	strcpy(sTrace_log.tel,g_pub_tx.tel);
	sTrace_log.hst_ind[0]='0';
	sTrace_log.no_show[0]='0';
	strcpy(sTrace_log.sts,"0");
	Trace_log_Debug(&sTrace_log);
	ret=Trace_log_Ins(sTrace_log,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入流水表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"P126" );
		goto ErrExit;
	}
	/*登记流水分录表*/
	sDc_log.tx_date=g_pub_tx.tx_date;
	sDc_log.trace_no=g_pub_tx.trace_no;
	sDc_log.trace_cnt=g_pub_tx.trace_cnt;
	sDc_log.dc_seqn=1;
	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_type='6'");
	if(ret)
	{
		sprintf(acErrMsg,"查询机构表出错 ret=[%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D172" );
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sDc_log.cur_no,"01");
	/**表外欠息*/
        {
        	  memcpy(cDc_code,ln_parm_new.dc_code,sizeof(ln_parm_new.dc_code)-1);
        	  strcpy(Data_code,"0156");
        	  sDc_log.amt=g_ln_mst.out_lo_intst;
        }
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='%s'",cDc_code,Data_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",cDc_code);
            WRITEMSG
            strcpy( g_pub_tx.reply ,"L154" );
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='2';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,g_pub_tx.tel);	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
            WRITEMSG
            strcpy( g_pub_tx.reply ,"P126" );
            vtcp_log("[%s][%d] errcode=[%d],%s",__FILE__,__LINE__,ret,g_pub_tx.reply);
            goto ErrExit;
			}
	

OkExit:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 0;
OkExit1:
 		strcpy(g_pub_tx.reply, "0000");
    sprintf(acErrMsg, "零金额，处理成功!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 2;

ErrExit:
    sprintf(acErrMsg, "处理失败!!!!![%s]", g_pub_tx.reply);
    WRITEMSG
    return 1;

}

/* 打印--贷款产品转换传票--转入部分 */
int print_LNBKCPZR( )
{
	char tx_type[9];
	double tol_lo=0.00;
	double in_lo_intst=0.00;
	double out_lo_intst =0.00;
	double tol_need_pay=0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
	char ln_sts[5];	
	char dc_sts[3];	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNJZZHZR打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "转入" );	
	
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 借贷标志 */
	strcpy( dc_sts , "借" );
	
	/* 贷款状态 */
	pub_base_dic_show_str(ln_sts,"ln_ac_sts",g_ln_mst.ac_sts);	
		
	/* 表外欠息 */	
	out_lo_intst = g_ln_mst.out_lo_intst ;
	
	/* 表内欠息 */	
	in_lo_intst = g_ln_mst.in_lo_intst;
			
	/* 尚欠本金 */		
	tol_need_pay = g_ln_mst.bal;

	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(展)" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	}
		
	sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.表外欠息 10.表内欠息     */
	/* 11.尚欠本金、 12.正常利率、 13.逾期利率、14.科目号、   15.展期标志  */
	/* 16.流水号、   17.借贷标志、 18.贷款状态  19.红字标志*/
	fprintf(fp,"LNJZZHZR%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	g_ln_mst.bal , g_ln_mst.opn_date , end_date , out_lo_intst ,in_lo_intst ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts ,"(蓝字)");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
/* 打印--贷款产品转换传票--转出部分 */
int print_LNBKCPZC( )
{
	char tx_type[9];
	double out_lo_intst=0.00;
	double in_lo_intst=0.00;
	double tol_need_pay =0.00;
	char rate[11];
	char over_rate[11];
	char exp_ind[5];
	char ln_sts[5];	
	char dc_sts[3];	
	char ln_acc_hrt[8];
	char data_code[5];
	int ret_tmp=0;
	long end_date =0;
		
    	char filename[51],tmpname[101];
    	FILE *fp;
    	
	pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
        if( fp == NULL )
        {
		sprintf(acErrMsg,"文件名:[%s]!",tmpname);
		WRITEMSG		
		sprintf(acErrMsg,"LNJZZHZC打开文件失败!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"L206" );
		return 1;
        }
        	
	/* 赋值部分 */
	strcpy( tx_type , "转出" );	
	
	/* 取科目号 */
	strcpy( data_code , "0152" );	/* 本金科目 */
	ret_tmp = pub_ln_get_acc_hrt( g_ln_mst , g_ln_parm , data_code , ln_acc_hrt );
	if( ret_tmp )
	{
		sprintf(acErrMsg,"取贷款科目号失败!");
		WRITEMSG		
		return 1;
	}		
	/* 借贷标志 */
	strcpy( dc_sts , "借" );
	
	/* 贷款状态 */
	pub_base_dic_show_str(ln_sts,"ln_ac_sts",g_ln_mst.ac_sts);	
		
	/* 表外欠息 */	
	out_lo_intst = -g_ln_mst.out_lo_intst ;
	
	/* 表内欠息 */	
	in_lo_intst = -g_ln_mst.in_lo_intst;
			
	/* 尚欠本金 */		
	tol_need_pay = g_ln_mst.bal;

	if( g_ln_mst.exp_mtr_date && g_ln_mst.exp_ind[0]!='0' )
	{
		end_date = g_ln_mst.exp_mtr_date;
		strcpy( exp_ind , "(展)" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.exp_rate );		
	}else{
		end_date = g_ln_mst.mtr_date;
		strcpy( exp_ind , "" );
		sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	}
		
	sprintf( rate , "%8.5lf‰" , g_ln_mst.rate );
	sprintf( over_rate , "%8.5lf‰" , g_ln_mst.over_rate);

		
	pub_base_strpack( g_pub_tx.ac_no );
	pub_base_strpack( g_ln_mst.name );
	pub_base_strpack( g_ln_mst.pact_no );
	pub_base_strpack( g_ln_parm.title );
	pub_base_strpack( tx_type );
	pub_base_strpack( rate );
	pub_base_strpack( over_rate );
		
	/*  1.交易类型、  2.贷款账号、  3.借据号、   4.贷款户名、  5.产品名称 	*/
	/*  6.交易金额、  7.开户日期、  8.到期日期、 9.表外欠息 10.表内欠息     */
	/* 11.尚欠本金、 12.正常利率、 13.逾期利率、14.科目号、   15.展期标志  */
	/* 16.流水号、   17.借贷标志、 18.贷款状态  19.红字标志*/
	fprintf(fp,"LNJZZHZC%s|%s|%s|%s|%s|%.2lf|%ld|%ld|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%s|%s|%s|\n",
	tx_type , g_pub_tx.ac_no , g_ln_mst.pact_no , g_ln_mst.name ,  g_ln_parm.title ,
	-g_ln_mst.bal , g_ln_mst.opn_date , end_date , out_lo_intst , in_lo_intst ,
	tol_need_pay , rate , over_rate , ln_acc_hrt,exp_ind , 
	g_pub_tx.trace_no , dc_sts , ln_sts, "(红字)");	
	
	fclose(fp);		
	set_zd_data(DC_FILE_SND,"1");	
	return 0;
}	
