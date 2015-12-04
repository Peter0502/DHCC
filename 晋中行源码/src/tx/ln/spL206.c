/*************************************************
* 文 件 名: spL206.c
* 功能描述：将mis传来的修改贷款信息插入贷款修改授权表   
*
* 作    者: ZHGF
* 完成日期：2008年04月14日
* 修改记录：   
*     1. 日    期: 20100610
*        修 改 人: 刘勇
*        修改内容: 登记柜员通远程授权信息
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "auth_modify_ln_rate_c.h"
#include "com_sys_parm_c.h"
static int data_get_struct();
struct auth_modify_ln_rate_c g_auth_modify_ln_rate;
struct com_sys_parm_c g_com_sys_parm;
static char flag[2]; /****操作标志****/

spL206()  
{ 	
	int ret=0;
	double intst=0.00;
 	memset(&g_auth_modify_ln_rate,0x0,sizeof(struct auth_modify_ln_rate_c));
	memset (&g_com_sys_parm,0x0,sizeof(struct com_sys_parm_c));
	
        /** 初始化全局变量 **/
	pub_base_sysinit();
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	/*if ( data_init_struct() )	ERR_DEAL;
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG*/
		
	/** 取值、赋值 **/
	if ( data_get_struct() )        ERR_DEAL;
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
	vtcp_log("AAAAAAAAAAAAAAAAA\n");
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&g_com_sys_parm,"1=1");
	if(ret){
		sprintf(acErrMsg,"查询系统日期错误!");
		WRITEMSG
		goto ErrExit;
	}
	/*授权日期取系统日期,有效期只要当天有效**/
	g_auth_modify_ln_rate.auth_date=g_com_sys_parm.sys_date;
	g_auth_modify_ln_rate.end_date=g_com_sys_parm.sys_date;
	/** 处理流程 **/
	vtcp_log("%s,%d flag=[%s]\n",__FILE__,__LINE__,flag);
	if(flag[0] =='1')
	{
		ret=Auth_modify_ln_rate_Sel(g_pub_tx.reply,&g_auth_modify_ln_rate,"ac_no ='%s' and auth_date=%ld and amt=%lf and auth_sts='0'", \ 
			g_auth_modify_ln_rate.ac_no,g_pub_tx.tx_date,g_auth_modify_ln_rate.amt);
		if(ret ==100){
			sprintf(acErrMsg,"查询柜面通远程授权表错误!ret ==[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}else if(ret!=0){
			sprintf(acErrMsg,"查询柜面通远程授权表错误!ret ==[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
		ret=sql_execute("update auth_modify_ln_rate set auth_sts ='1' where ac_no = '%s' and auth_date = %ld and auth_sts='0' and amt=%lf",g_auth_modify_ln_rate.ac_no,g_pub_tx.tx_date,g_auth_modify_ln_rate.amt);
		if(ret)
		{
			sprintf(acErrMsg,"更新柜面通远程授权表错误!ret ==[%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
			goto ErrExit;
		}
		
		
	}else  if(flag[0]=='0'){
		ret=sql_count("auth_modify_ln_rate","ac_no ='%s' and auth_date=%ld and auth_sts='0' and amt=%lf",g_auth_modify_ln_rate.ac_no,g_pub_tx.tx_date,g_auth_modify_ln_rate.amt);
		if(ret>=1){
			sprintf(acErrMsg,"同笔交易存在多笔授权请检查!");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D106");
			goto ErrExit;
		}
		ret=Auth_modify_ln_rate_Ins(g_auth_modify_ln_rate,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"插入柜面通远程授权表错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D107");
			goto ErrExit;
		}
	}
	
OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"插入柜面通远程授权信息表成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"插入柜面通远程授权表失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
}
    
int data_get_struct()
{    
        char tel[10];
        char auth_tel[10];
        char brno[10];
        char auth_brno[10];
        sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG

        memset(flag,0x00,sizeof(flag));
        memset(tel,0x00,sizeof(tel));
        memset(auth_tel,0x00,sizeof(auth_tel));
        memset(brno,0x00,sizeof(brno));
        memset(auth_brno,0x00,sizeof(auth_brno));


        get_zd_data("0700",flag);
        get_zd_data("0300",g_auth_modify_ln_rate.ac_no);	/* 帐号     */
	g_auth_modify_ln_rate.ac_seqn=1;			/* 账户序号 */
	pub_base_old_acno(g_auth_modify_ln_rate.ac_no);
        
        get_zd_long("0440",&g_auth_modify_ln_rate.auth_date);   /*授权日期*/
        get_zd_double("0400",&g_auth_modify_ln_rate.amt);       /*交易金额*/ 
        sprintf(acErrMsg,"交易金额[%f]",g_auth_modify_ln_rate.amt);
	WRITEMSG
        get_zd_data("0920",tel);                                /*被授权柜员*/
        get_zd_data("0910",brno);                               /*被授权机构*/
        get_zd_data("0640",auth_tel);                           /*授权柜员*/
        get_zd_data("0650",auth_brno);                          /*授权机构*/	

        strcpy(g_auth_modify_ln_rate.tel,tel);
        strcpy(g_auth_modify_ln_rate.brno,brno);
        strcpy(g_auth_modify_ln_rate.auth_tel,auth_tel);
        strcpy(g_auth_modify_ln_rate.auth_brno,auth_brno);	
        g_auth_modify_ln_rate.auth_sts[0]='0';                  /* 授权状态 */

        g_auth_modify_ln_rate.auth_type[0] ='0';
        g_auth_modify_ln_rate.acc_no[0]='0';
        g_auth_modify_ln_rate.modify_value[0]='0';
        g_auth_modify_ln_rate.filler1[0]='0';
        g_auth_modify_ln_rate.filler2[0]='0';
        g_auth_modify_ln_rate.auth_no=0;
	
        return 0;
}

