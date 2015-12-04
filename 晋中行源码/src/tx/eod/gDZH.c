/*************************************************************
* 文 件 名: gDZH.c
* 功能描述：新增贷款产品，旧贷款产品的转换以及存量数据处理
* 1.个人的贷款产品不变，相对应的应收利息科目和利息收入科目以及表
*外科目转为新的科目，并进行应收利息以及表外科目的账务处理。
* 2.对公的贷款产品进行转换，并将旧的科目余额转到新产品的科目下。
*	应收利息的账务处理。
*
* 作    者: lpj
* 完成日期: 20151111
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
#include "new_old_prdt_c.h"
#include "new_old_accno_c.h"
#include "dc_acc_rel_c.h"
#include "cif_prdt_rel_c.h"
#include "com_branch_c.h"
#include "cif_basic_inf_c.h"	

#include "trace_log_c.h"
#include "dc_log_c.h"
static	char	 prod_no[4];
gDZH()
{
		int ret=0;
		int flag=0;/*记账完成标志，0未完成，1完成*/
		char cAc_no[19];
		struct new_old_prdt_c sNew_old_prdt;
		struct new_old_accno_c sNew_old_accno;
		struct mdm_ac_rel_c sMdm_ac_rel;
		struct ln_parm_c sLn_parm;
		struct ln_mst_c sLn_mst;
		struct com_item_c sCom_item;
		struct dc_acc_rel_c sDc_acc_rel;
		struct cif_basic_inf_c sCif_basic_inf;
	
		
		
		memset(&sNew_old_prdt,0x00,sizeof(sNew_old_prdt));
		memset(&sNew_old_accno,0x00,sizeof(sNew_old_accno));
		memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
		memset(&sLn_parm,0x00,sizeof(sLn_parm));
		memset(&sLn_mst,0x00,sizeof(sLn_mst));
		memset(&sCom_item,0x00,sizeof(sCom_item));
		memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));	
		memset(&sCif_basic_inf,0x00,sizeof(sCif_basic_inf));		
		
		ret=New_old_prdt_Dec_Upd(g_pub_tx.reply," sts='0' and 1=1 order by pact_no");
		if(ret)
		{
			sprintf(acErrMsg,"贷款新旧产品对照表的游标定义出错[%s]",g_pub_tx.reply);
			WRITEMSG
		
			goto ErrExit;
		}
		while(1)
		{
			memset(&sNew_old_prdt,0x00,sizeof(sNew_old_prdt));
			memset(&sNew_old_accno,0x00,sizeof(sNew_old_accno));
			memset(&sLn_mst,0x00,sizeof(sLn_mst));
			memset(&sLn_parm,0x00,sizeof(sLn_parm));
			memset(&sCom_item,0x00,sizeof(sCom_item));
			memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
			memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
			ret=New_old_prdt_Fet_Upd(&sNew_old_prdt,g_pub_tx.reply);
			if(ret==100)
			{
				vtcp_log("%s, %ld 新旧产品转换完毕！退出",__FILE__,__LINE__);
				break;	
			}else if(ret)
			{
					sprintf(acErrMsg,"查询贷款新旧产品对照表出错[%s]",g_pub_tx.reply);
					WRITEMSG
					New_old_prdt_Clo_Sel();
					goto ErrExit;
			}
			
					ret=Ln_mst_Sel(g_pub_tx.reply,&sLn_mst,"pact_no='%s'and ac_sts not in('*','9')",sNew_old_prdt.pact_no);
					if(ret&&ret!=100)
					{
							sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
							WRITEMSG
						
							goto ErrExit;
					}else if(ret==100)
					{
										vtcp_log("%s,%ld核心不存在该借据号%s",__FILE__,__LINE__,sNew_old_prdt.pact_no);	
										sNew_old_prdt.sts[0]='3';
										ret=New_old_prdt_Upd_Upd(sNew_old_prdt,g_pub_tx.reply);
										if(ret)
										{
											sprintf( acErrMsg, "执行New_old_prdt_Upd_Upd出错!ret=[%d]",ret );
													WRITEMSG
												
													goto ErrExit;
										}
										continue;
						
					}
							
					
									flag=0;
									
									ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id=%ld ",sLn_mst.ac_id);
									if(ret)
									{
										sprintf( acErrMsg, "查询账户介质表出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}
									strcpy(cAc_no,sMdm_ac_rel.ac_no);
								
								
								
							if(pub_base_CompDblVal(sLn_mst.bal,sNew_old_prdt.bj_amt)!=0||pub_base_CompDblVal(sLn_mst.in_lo_intst,sNew_old_prdt.in_lo_intst)!=0\
									||pub_base_CompDblVal(sLn_mst.out_lo_intst,sNew_old_prdt.out_lo_intst)!=0)
						{	
							vtcp_log("%s,%ld 金额不符，旧本金【%16.2f】,新本金【%16.2f】,旧表内息【%16.2f】,新表内息【%16.2f】\
															旧表外息【%16.2f】,新表外息【%16.2f】 直接跳过！！！！%s",__FILE__,__LINE__,sLn_mst.bal,sNew_old_prdt.bj_amt,\
															sLn_mst.in_lo_intst,sNew_old_prdt.in_lo_intst,sLn_mst.out_lo_intst,sNew_old_prdt.out_lo_intst,sMdm_ac_rel.ac_no);
							continue;
						}		
						/*判断对公与个人贷款转换时的条件*/
						if(sMdm_ac_rel.ac_no[0]=='1')
						{
									if(strlen(sNew_old_prdt.old_prdt_no)==0)
									{
										vtcp_log("%s,%ld不需要转换",__FILE__,__LINE__);	
										sNew_old_prdt.sts[0]='2';
										ret=New_old_prdt_Upd_Upd(sNew_old_prdt,g_pub_tx.reply);
										if(ret)
										{
											sprintf( acErrMsg, "执行New_old_prdt_Upd_Upd出错!ret=[%d]",ret );
													WRITEMSG
												
													goto ErrExit;
										}
										continue;
										
									}
									ret=Ln_parm_Sel(g_pub_tx.reply,&sLn_parm,"prdt_no='%s'",sNew_old_prdt.old_prdt_no);
									if(ret)
									{
											sprintf( acErrMsg, "执行Ln_parm_Sel出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}	
									
									ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0152'",sLn_parm.dc_code);
									if(ret)
									{
											sprintf( acErrMsg, "执行Dc_acc_rel_Sel出错!ret=[%d]",ret );
											WRITEMSG
										
											goto ErrExit;
									}	
		
									
								ret=New_old_accno_Sel(g_pub_tx.reply,&sNew_old_accno,"old_acchrt='%s'",sDc_acc_rel.acc_hrt);
								if(ret&&ret!=100)
								{
									sprintf( acErrMsg, "执行New_old_accno_Sell(出错!ret=[%d]",ret );
									WRITEMSG
								
									goto ErrExit;
							}else if(ret==100)
							{
										vtcp_log("%s,%ld不需要转换",__FILE__,__LINE__);	
										sNew_old_prdt.sts[0]='2';
										ret=New_old_prdt_Upd_Upd(sNew_old_prdt,g_pub_tx.reply);
										if(ret)
										{
											sprintf( acErrMsg, "执行New_old_prdt_Upd_Upd出错!ret=[%d]",ret );
													WRITEMSG
												
													goto ErrExit;
										}
										continue;
							}else
							{	
								if(pub_base_CompDblVal(sLn_mst.in_lo_intst,0.00)>0\
											||pub_base_CompDblVal(sLn_mst.out_lo_intst,0.00)>0)
								{
									if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
										{
    						        sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
    						        WRITEMSG
    						        goto ErrExit;
										}
										g_pub_tx.trace_cnt=0;
										vtcp_log("cif_no=[%ld],trace_no=%d",sLn_mst.cif_no,g_pub_tx.trace_no);
								}else
								{
										
										vtcp_log("%s,%ld不需要记账",__FILE__,__LINE__);	
										sNew_old_prdt.sts[0]='4';
										ret=New_old_prdt_Upd_Upd(sNew_old_prdt,g_pub_tx.reply);
										if(ret)
										{
											sprintf( acErrMsg, "执行New_old_prdt_Upd_Upd出错!ret=[%d]",ret );
													WRITEMSG
												
													goto ErrExit;
										}
										TRACE
										continue;
								}
							}
								
						}else
						{
									
									
										if(pub_base_CompDblVal(sLn_mst.bal,0.00)>0||pub_base_CompDblVal(sLn_mst.in_lo_intst,0.00)>0\
														||pub_base_CompDblVal(sLn_mst.out_lo_intst,0.00)>0)
											
											{
												if ( pub_base_PubQlsh(&g_pub_tx.trace_no,g_pub_tx.reply))
													{
    									        sprintf(acErrMsg,"取主机流水号错 [%d]",g_pub_tx.trace_no);
    									        WRITEMSG
    									        goto ErrExit;
													}
													g_pub_tx.trace_cnt=0;
													vtcp_log("cif_no=[%ld],trace_no=%d",sLn_mst.cif_no,g_pub_tx.trace_no);
											}else
											{
													vtcp_log("%s,%ld不需要记账",__FILE__,__LINE__);	
													sNew_old_prdt.sts[0]='4';
													ret=New_old_prdt_Upd_Upd(sNew_old_prdt,g_pub_tx.reply);
													if(ret)
													{
														sprintf( acErrMsg, "执行New_old_prdt_Upd_Upd出错!ret=[%d]",ret );
																WRITEMSG
															
																goto ErrExit;
													}
													TRACE
											}
							
							
						}	
							
							if(strcmp(sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no)!=0)
							{
										ret=bj_jzzc(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bj_jzzr(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bnlx_jzzc(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bnlx_jzzr(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
										if(ret&&ret!=2)
										{
											sprintf( acErrMsg, "利息科目记账转入出错!ret=[%d]",ret );
												WRITEMSG
											
												goto ErrExit;
										}
										if(ret==2)
										{
											vtcp_log("%s,%ld 零金额不记账，ac_no='%s'",__FILE__,__LINE__,sMdm_ac_rel.ac_no);
												
										}ret=bwlx_jzzc(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bwlx_jzzr(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
							}else
							{
									vtcp_log("不进行产品转换，本金不需要记账");
									ret=bnlx_jzzc(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bnlx_jzzr(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bwlx_jzzc(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
										ret=bwlx_jzzr(cAc_no,sNew_old_prdt.old_prdt_no,sNew_old_prdt.new_prdt_no);
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
							}
							vtcp_log("记账走到这儿了，说明记账完成！%s,%d",__FILE__,__LINE__);
				
					sNew_old_prdt.sts[0]='1';
					ret=New_old_prdt_Upd_Upd(sNew_old_prdt,g_pub_tx.reply);
					if(ret)
					{
						sprintf( acErrMsg, "执行New_old_prdt_Upd_Upd出错!ret=[%d]",ret );
								WRITEMSG
							
								goto ErrExit;
					}
				
				
		}
   New_old_prdt_Clo_Upd();
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
/*贷款本金科目的转入记账，借 正金额*/

int bj_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	int tag=0;
	int amt_flag=0;
	
	char ln_ac_id[20];
	char acc_hrt[6];
	char wrk_sts[2];
	char type[2];
	long ac_id_t;
	double maga_amt_old=0.00;
	long end_date_old;

	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	struct	cif_basic_inf_c	cif_basic_inf;	
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
	memset ( &cif_basic_inf , 0x00 , sizeof( struct cif_basic_inf_c ) );
	memset ( &cif_prdt_rel , 0x00 , sizeof( struct cif_prdt_rel_c ) );
				

	strcpy(g_pub_tx.ac_no,ac_no); 		/* 贷款账号 */
	g_pub_tx.ac_seqn = 0;				/* 账号序号 */
	strcpy(prod_no,new_prod_no);
	
	vtcp_log("bj_jzzr,trace_no=%d,new_prod_no=%s",g_pub_tx.trace_no,new_prod_no);
		
	/** 处理流程 **/
	
		
	/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}

	sprintf(acErrMsg,"ac_id[%ld] [%d]",g_ln_mst.ac_id,g_ln_mst.ac_seqn );
	WRITEMSG

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
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG

	TRACE
	/* 客户类型是否合乎产品定义 */
	ret = pub_base_check_cltprdt( cif_basic_inf.type, prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"新旧产品客户类型不相同,不可做此交易![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"L139");
		goto ErrExit;;	
	}		
	TRACE
			

	/* 贷款产品属性检查(开户) */


	
	strcpy( type , "1" );
	strcpy(g_ln_mst.prdt_no,prod_no);	/* 贷款产品编号 */
	
	

			
	/* 更新贷款主文件 */
	ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
		WRITEMSG
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
		goto ErrExit;
	}
    
	ret = Cif_prdt_rel_Fet_Upd( &cif_prdt_rel,g_pub_tx.reply );   
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "客户-产品关系表中无此记录" );
		WRITEMSG
		Cif_prdt_rel_Clo_Upd();
	}else if ( ret ){
		sprintf( acErrMsg, "执行Cif_prdt_rel_Fet_Upd出错!ret=[%d]",ret );
		WRITEMSG
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
			goto ErrExit;
		}
		Cif_prdt_rel_Clo_Upd();
		sprintf(acErrMsg,"更新客户-产品关系表 PASS!");
		WRITEMSG		
	}
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
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
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
	sTrace_log.tx_time=0;
	strcpy(sTrace_log.tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"gDZH");
	strcpy(sTrace_log.sub_tx_code,"gDZH");
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
	strcpy(sTrace_log.brf,"贷款本金转入");
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
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.cur_no,"01");
	
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0152'",g_ln_parm.dc_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",g_ln_parm.dc_code);
            WRITEMSG
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
        sDc_log.amt=g_ln_mst.bal;
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,"9999");	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
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
/*贷款本金科目的转出记账，借 负金额*/
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
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
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
	sTrace_log.tx_time=0;
	strcpy(sTrace_log.tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"gDZH");
	strcpy(sTrace_log.sub_tx_code,"gDZH");
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
	strcpy(sTrace_log.brf,"贷款本金转出");
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
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.cur_no,"01");
	/*dc_code根据账户状态来确定*/
	 if( g_ln_mst.ac_sts[0]=='1' )/* 正常 */
    {
    TRACE
        strcpy(cDc_code,g_ln_parm.dc_code); 
    }
    if( g_ln_mst.ac_sts[0]=='2' )/* 逾期90内 */
    {
    TRACE
        strcpy(cDc_code,g_ln_parm.over_dc_code);    
    }
    /**add by ligl2006-9-25 17:31*/
    if(g_ln_mst.ac_sts[0]=='5')/**逾期90外*/
    {
    TRACE
       strcpy(cDc_code,g_ln_parm.over90_dc_code);
    }
    /**end*/
    if( g_ln_mst.ac_sts[0]=='3' )/* 呆滞 */
    {
    TRACE
        strcpy(cDc_code,g_ln_parm.sla_dc_code); 
    }
    if( g_ln_mst.ac_sts[0]=='4' )/* 呆账 */
    {
    TRACE
        strcpy(cDc_code,g_ln_parm.bad_dc_code); 
    }
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='0152'",cDc_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",g_ln_parm.dc_code);
            WRITEMSG
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
        sDc_log.amt=-g_ln_mst.bal;
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,"9999");	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
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
/*贷款表内利息科目的转出记账，借 旧表内科目 负金额*/
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
		goto ErrExit;
	}
	/*判断账户状态是否正常*/
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
             vtcp_log("[%s][%d] 该账户[%d]状态[%s]错误",__FILE__,__LINE__,g_ln_mst.ac_id,g_ln_mst.ac_sts);
             goto ErrExit;
        }
						strcpy(sDc_log.acc_hrt,"13301");
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
	sTrace_log.tx_time=0;
	strcpy(sTrace_log.tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"gDZH");
	strcpy(sTrace_log.sub_tx_code,"gDZH");
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
	strcpy(sTrace_log.brf,"贷款表内息转出");
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
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.cur_no,"01");
	
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,"9999");	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
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
/*贷款应收利息科目的转入记账，贷 新的应收利息科目 正金额*/
int bnlx_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];
	struct ln_parm_c ln_parm_new;

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(&ln_parm_new,0x00,sizeof(ln_parm_new));
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
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
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
	sTrace_log.tx_time=0;
	strcpy(sTrace_log.tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"gDZH");
	strcpy(sTrace_log.sub_tx_code,"gDZH");
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
	strcpy(sTrace_log.brf,"贷款表内息转入");
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
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.cur_no,"01");
	
        	  memcpy(cDc_code,ln_parm_new.dc_code,sizeof(g_ln_parm.dc_code)-1);
        	  strcpy(Data_code,"0155");
        	  sDc_log.amt=g_ln_mst.in_lo_intst;
        
       
				ret=Dc_acc_rel_Sel(g_pub_tx.reply,&sDc_acc_rel,"dc_code='%s' and data_code='%s'",cDc_code,Data_code);
        if(ret)
        {
            sprintf( acErrMsg,"查询会计关系[%s]表错误",cDc_code);
            WRITEMSG
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='1';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,"9999");	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
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
/*贷款表外利息科目的转出记账，呆 表外科目 负金额*/
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
		goto ErrExit;
	}
	/*判断账户状态是否正常*/
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
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
        	  strcpy(sDc_log.acc_hrt,"70501");
        	  sDc_log.amt=-g_ln_mst.out_lo_intst;
        }
	/*登记流水表*/
	g_pub_tx.trace_cnt++;	
	sTrace_log.trace_no=g_pub_tx.trace_no;
	
	sTrace_log.trace_cnt=g_pub_tx.trace_cnt;
	sTrace_log.tx_date=g_pub_tx.tx_date;
	sTrace_log.tx_time=0;
	strcpy(sTrace_log.tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"gDZH");
	strcpy(sTrace_log.sub_tx_code,"gDZH");
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
	strcpy(sTrace_log.brf,"贷款表外息转出");
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
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.cur_no,"01");
	
			sDc_log.dc_ind[0]='2';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,"9999");	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
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
/*贷款表外利息科目的转入记账，贷 新的表外利息科目 正金额*/
int bwlx_jzzr(char *ac_no,char *old_prod_no,char *new_prod_no)
{
	int ret=0;
	struct trace_log_c sTrace_log;
	struct dc_log_c sDc_log;
	struct com_branch_c sCom_branch;
	struct dc_acc_rel_c sDc_acc_rel;
	char   cDc_code[5];
	char Data_code[5];
	struct ln_parm_c ln_parm_new;

    char   cAcc_hrt1[8];/**本金科目**/
    char   cAcc_hrt2[8];/**表内科目**/
    char   cAcc_hrt3[8];/**表外科目**/
    char   cAcc_hrt4[8];/**利息科目**/
	
	memset(&sTrace_log,0x00,sizeof(sTrace_log));
	memset(&sDc_log,0x00,sizeof(sDc_log));
	memset(&sCom_branch,0x00,sizeof(sCom_branch));
	memset(&sDc_acc_rel,0x00,sizeof(sDc_acc_rel));
	memset(&ln_parm_new,0x00,sizeof(ln_parm_new));
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
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG
		/* 取老产品信息 */
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}
	if(g_ln_mst.ac_sts[0]!='1' && g_ln_mst.ac_sts[0]!='2' && g_ln_mst.ac_sts[0]!='3' && g_ln_mst.ac_sts[0]!='4' && g_ln_mst.ac_sts[0]!='5')
        {
        	   sprintf(acErrMsg, "该账户状态错误[%s]!",g_ln_mst.ac_sts);
             WRITEMSG
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
	sTrace_log.tx_time=0;
	strcpy(sTrace_log.tx_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sTrace_log.tx_code,"gDZH");
	strcpy(sTrace_log.sub_tx_code,"gDZH");
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
	strcpy(sTrace_log.brf,"贷款表外息转入");
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
		goto ErrExit;
	}
	strcpy(sDc_log.opn_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_br_no,sCom_branch.br_no);
	strcpy(sDc_log.tx_opn_br_no,g_ln_mst.opn_br_no);
	strcpy(sDc_log.tx_tx_br_no,g_ln_mst.opn_br_no);
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
            vtcp_log("[%s][%d] errcode=[%d]",__FILE__,__LINE__,ret);
            goto ErrExit;
        }
			strcpy(sDc_log.acc_hrt,sDc_acc_rel.acc_hrt);
			sDc_log.dc_ind[0]='2';
			sDc_log.ct_ind[0]='2';
			
			sDc_log.sts[0]='0';
			strcpy(sDc_log.tel,"9999");	
			Dc_log_Debug(&sDc_log);
			ret=Dc_log_Ins(sDc_log,g_pub_tx.reply);
			if(ret)
			{
						sprintf( acErrMsg,"插入流水分录表出错");
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