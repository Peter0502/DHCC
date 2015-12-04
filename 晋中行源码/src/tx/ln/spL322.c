/*************************************************
* 文 件 名: spL322.c
* 功能描述：   
*              贷款产品减值转换登记
*
* 作    者:    lpj
* 完成日期：   2015年11月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "new_jz_prdt_c.h"
#include "prdt_jz_dz_c.h"

char prod_no[3];
char type[2];
		
spL322()  
{ 	
	
	int ret=0;


	struct	ln_mst_c	ln_mst_new;
	struct	ln_parm_c	ln_parm_new;	
	struct	new_jz_prdt_c sNew_jz_prdt;
	struct	new_jz_prdt_c sNew_jz_prdt1;
	struct	prdt_jz_dz_c sPrdt_jz_dz;
	char wherelist[512];
	char old_prdt_no[4];
	
	memset ( &ln_mst_new , 0x00 , sizeof( struct ln_mst_c ) );
	memset ( &ln_parm_new , 0x00 , sizeof( struct ln_parm_c ) );
	memset ( &sNew_jz_prdt , 0x00 , sizeof( struct new_jz_prdt_c ) );
	memset ( &sNew_jz_prdt1 , 0x00 , sizeof( struct new_jz_prdt_c ) );
	memset ( &sPrdt_jz_dz , 0x00 , sizeof( struct prdt_jz_dz_c ) );
	memset ( wherelist , 0x00 , sizeof( wherelist) );			
	memset ( old_prdt_no , 0x00 , sizeof( old_prdt_no) );	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	

	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
		
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
		goto ErrExit;
	}else if( ret ){
    		sprintf( acErrMsg,"取贷款产品参数表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"D103" );
	    		goto ErrExit;
    	}
	sprintf(acErrMsg,"取贷款产品参数信息 PASS!");
	WRITEMSG		
	sprintf(acErrMsg,"此两种贷款产品[%s][%s]",g_ln_parm.time_type , ln_parm_new.time_type);
	WRITEMSG
	/*减值产品对照检查*/
	if(type[0]=='1')
	{
		sprintf(wherelist,"prdt_no='%s' and jz_prdt_no='%s'",g_ln_parm.prdt_no,prod_no);
	}else if(type[0]=='2')
	{
		sprintf(wherelist,"jz_prdt_no='%s' and prdt_no='%s'",g_ln_parm.prdt_no,prod_no);
	}
	ret=Prdt_jz_dz_Sel(g_pub_tx.reply,&sPrdt_jz_dz,wherelist);
	if(ret)
	{
			sprintf( acErrMsg,"取贷款减值产品对照表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L300" );
	    		goto ErrExit;
	}
	if(type[0]=='1')
	{
		strcpy(old_prdt_no,sPrdt_jz_dz.jz_prdt_no);
	}else if(type[0]=='2')
	{
		strcpy(old_prdt_no,sPrdt_jz_dz.prdt_no);
	}
	if(strcmp(prod_no,old_prdt_no)!=0)
	{
			sprintf( acErrMsg,"不是该贷款产品对应的减值产品！ 原产品名称=[%s]，新产品名称=【%s】",old_prdt_no,prod_no);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L301" );
	    		goto ErrExit;
	}
	
	/*开始更新减值产品转换表*/
	if(type[0]=='1')
	{
				
				ret=New_jz_prdt_Sel(g_pub_tx.reply,&sNew_jz_prdt,"pact_no='%s' and ac_no='%s'",g_ln_mst.pact_no,g_mdm_ac_rel.ac_no);				
				if(ret&&ret!=100)
				{
						sprintf( acErrMsg,"查询贷款减值转换表出错 ret=%d,g_pub_tx.reply=[%s]",ret,g_pub_tx.reply);
							WRITEMSG
						strcpy( g_pub_tx.reply ,"L302" );
				    		goto ErrExit;
				}else if(ret==100)
				{
					vtcp_log("没有减值数据，开始插入");
					strcpy(sNew_jz_prdt1.pact_no,g_ln_mst.pact_no);
					strcpy(sNew_jz_prdt1.ac_no,g_mdm_ac_rel.ac_no);
					strcpy(sNew_jz_prdt1.ac_name,g_ln_mst.name);
					strcpy(sNew_jz_prdt1.old_prdt_no,g_ln_mst.prdt_no);
					strcpy(sNew_jz_prdt1.old_prdt_name,g_ln_parm.title);					
					strcpy(sNew_jz_prdt1.new_prdt_no,prod_no);
					strcpy(sNew_jz_prdt1.new_prdt_name,ln_parm_new.title);
					strcpy(sNew_jz_prdt1.id_no,g_mdm_ac_rel.id_no);
					sNew_jz_prdt1.bj_amt=g_ln_mst.bal;
					sNew_jz_prdt1.in_lo_intst=g_ln_mst.in_lo_intst;
					sNew_jz_prdt1.out_lo_intst=g_ln_mst.out_lo_intst;
					strcpy(sNew_jz_prdt1.sts,"0");
					
					/*开始插入减值转换表数据*/
						New_jz_prdt_Debug(&sNew_jz_prdt1);
					ret=New_jz_prdt_Ins(sNew_jz_prdt1,g_pub_tx.reply);
					if(ret)
					{
						
							sprintf( acErrMsg,"插入贷款减值转换表出错 ret=%d,g_pub_tx.reply=[%s]",ret,g_pub_tx.reply);
							WRITEMSG
							strcpy( g_pub_tx.reply ,"D107" );
				    		goto ErrExit;
						
					}
				}
				
						if(sNew_jz_prdt.sts[0]=='1')
						{
									sprintf( acErrMsg,"该账号已经是减值产品，不能再做减值！！！ ");
									WRITEMSG
									strcpy( g_pub_tx.reply ,"L303" );
						    		goto ErrExit;
						}
				
	}else if(type[0]=='2')
	{
				ret=New_jz_prdt_Sel(g_pub_tx.reply,&sNew_jz_prdt,"pact_no='%s' and ac_no='%s'",g_ln_mst.pact_no,g_mdm_ac_rel.ac_no);				
				if(ret)
				{
						sprintf( acErrMsg,"查询贷款减值转换表出错 ret=%d,g_pub_tx.reply=[%s]",ret,g_pub_tx.reply);
							WRITEMSG
						strcpy( g_pub_tx.reply ,"L302" );
				    		goto ErrExit;
				}
				if(sNew_jz_prdt.sts[0]=='0')
				{
							sprintf( acErrMsg,"该账号已经是正常产品，不能再做转换！！！ ");
							WRITEMSG
							strcpy( g_pub_tx.reply ,"L303" );
				    		goto ErrExit;
				}
		
	}
				
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款减值转换录入成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款减值转换录入失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    if ( strcmp( g_pub_tx.reply , "L018" )==0 )
    {
    	strcpy( g_pub_tx.reply , "L115" );
    }
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    

int data_set_struct()
{    
	int ret=0;
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
	
	get_zd_data("0310" , g_pub_tx.ac_no); 		/* 贷款账号 */
	g_pub_tx.ac_seqn = 0;				/* 账号序号 */
	get_zd_data("0930" , type); 			/* 操作类型1-正常转减值，2-减值转正常 */
	get_zd_data("0240" , prod_no); 			/* 新产品代码 */

	WRITEMSG						
	return 0;
}
