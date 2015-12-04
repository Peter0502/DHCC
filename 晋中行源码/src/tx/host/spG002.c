/*************************************************
* 文 件 名: spG002.c
* 功能描述：调用日终处理主控程序测试
*
* 作    者:  
* 完成日期：
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#include <dlfcn.h>
#include <errno.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_eod_exec_c.h"

struct com_eod_exec_c grz_com_eod_exec; /**全局，用于向子程序传递控制序列**/
char pgm[100];

int spG002( )
{
	int	ret;
	
	get_zd_data("0330",pgm);

	sprintf( acErrMsg, "------------日终pgm test begin!!!![%s]",pgm);
	WRITEMSG
	
	ret = cs_eod_deal( );
	if ( ret ) 
	{
		sprintf( acErrMsg, "日终pgm test ERROR !!!!");
		WRITEMSG
		goto ErrExit;
	}	
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg, "------------日终pgm test OK!!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "------------日终pgm test BAD[%s]!!!!",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * 函  数 名：  pub_eod_deal
 * 函数 功能:   
 *		调用日终处理程序,供外部交易调用
 * 参  数：
 *     输入：  
 *		char	*c_cnt		年终第几次执行 1 第一次，2 第二次
 *		char	*do_type	程序执行时机 
 *					0翻牌前 1翻牌 2翻牌后		
 *		long sys_date,		系统日终日期
 *
********************************************************************/
int cs_eod_deal( )
{
	struct com_sys_parm_c 	sSysParm;	/* 公共系统参数表 */
	struct com_eod_exec_c	sEodExec;	/* 日终处理顺序表 */
	char	cDoType[2];			/* 执行时机 0翻牌前 1翻牌 2翻牌后 */

	int	ret, i;
	
	memset( &sSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
			
	/* 数据初始化 */
	pub_base_sysinit();

	/* 记录日终日志开始 */
	/*pub_eod_WriJk("%s","========★ ☆==============☆ ★========");
   	pub_eod_WriJk("日终TEST[%s]开始",pgm);*/


		/**结束主控的事物**/
		ret=sql_commit();

		if ( pub_base_GetSysparm( &sSysParm ) )
		{
			sprintf( acErrMsg, "取公共系统参数表错!" );
			WRITEMSG
			goto ErrExit;			
		}
	
		set_zd_long( DC_DATE , sSysParm.sys_date );        /* 交易日期 */
		set_zd_data( DC_TEL , "9999" );

		ret = cs_eod_ExecProg( 0, "", sSysParm.sys_date );
		if ( ret )
		{
			sprintf( acErrMsg,"日终TEST[%s]失败",pgm);
			WRITEMSG
		/*	pub_eod_WriJk("日终TEST[%s]失败",pgm);
		        pub_eod_WriJk("========※ ※ =============※ ※========");*/
			goto ErrExit;
		}
 /*   pub_eod_WriJk("日终TEST[%s]成功",pgm);*/

		/**重新启事务**/
		sql_begin();	

   /* pub_eod_WriJk("========● ○==============○ ●========");*/
	
OkExit:
	return 0;
ErrExit:
	return -1;
}
/**********************************************************************
 * 函  数 名：  exec_eod_prog
 * 函数 功能:   
 *		调用日终处理程序，执行并记录日终日志
 *		            
 * 作者/时间：  jack//2003年1月16日
 * 
 * 参  数：
 * 输入：  
 *		char	*c_cnt		年终第几次执行 1 第一次，2 第二次
 *		char	*do_type	程序执行时机 
 *					0翻牌前 1翻牌 2翻牌后		
 *		long sys_date,		系统日终日期
 *
 * 输出：  
 *
 * 返回值：  0:成功  -1：失败
 *           
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *		jack--*--20030305--*--将system调用改为dlopen的动态库调用
 *
********************************************************************/
int	cs_eod_ExecProg( char *fc_cnt, char *do_type, long sys_date)
{
	struct com_eod_exec_c sEodExec;
	struct com_eod_exec_c cExec[256];
	int iSeq[256];
	int	ret = 0;
	int	i = 0;
	int	j = 0;

	void *handle;
	void (* execFun)();
	char libsoname[64];

		/**重新启事务**/
		sql_begin();	

	memset(cExec, 0x0, sizeof(cExec));
	
	/* 声明打开游标  */
	ret = Com_eod_exec_Dec_Sel ( g_pub_tx.reply, "prog='%s'",pgm ); 
	if ( ret )
	{
		sprintf( acErrMsg, "执行Com_eod_exec_Dec_Sel错,[%d]", ret );
		WRITEMSG
		return(-1);
	}
		
		memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );		
	
		/*  从日终顺序表中取出一条记录  */
		ret = Com_eod_exec_Fet_Sel( &sEodExec, g_pub_tx.reply );
		if ( ret == 100 )
		{
			strcpy( sEodExec.sub_tx_code,"RZCS" );
			sEodExec.do_seq=0;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行	Com_eod_exec_Fet_Sel错[%d]", ret );
			WRITEMSG
			return(-1);
		}
	
		pub_base_strpack( sEodExec.prog );
vtcp_log("=========[%s]--",sEodExec.prog );
		/*strcpy( cExecProg[i],sEodExec.prog ); */
		memcpy( &cExec[i],&sEodExec,sizeof(sEodExec) );
vtcp_log("=========[%s]-=",cExec[i].prog );
		iSeq[i] = sEodExec.do_seq;

	Com_eod_exec_Clo_Sel ();

		/**结束事物**/
		ret=sql_commit();

		memset(libsoname,0x0,sizeof(libsoname));

		sprintf( libsoname,"%s/lib/",getenv("HOME") );
		pub_base_strpack( cExec[j].prog );
		strcat( libsoname, cExec[j].prog );
		strcat( libsoname,".so" );
		pub_base_strpack( libsoname );
vtcp_log("=========[%s]==[%d-%d]",libsoname,i,j );

		memcpy( &grz_com_eod_exec,&cExec[j],sizeof(grz_com_eod_exec) );
		memset(&g_mdm_ac_rel,0x0,sizeof(g_mdm_ac_rel));
		memset(&g_dd_mst,0x0,sizeof(g_dd_mst));
		memset(&g_td_mst,0x0,sizeof(g_td_mst));
		memset(&g_ln_mst,0x0,sizeof(g_ln_mst));
		memset(&g_in_mst,0x0,sizeof(g_in_mst));
		memset(&g_od_mst,0x0,sizeof(g_od_mst));

		memset(&g_dd_parm,0x0,sizeof(g_dd_parm));
		memset(&g_td_parm,0x0,sizeof(g_td_parm));
		memset(&g_ln_parm,0x0,sizeof(g_ln_parm));
		memset(&g_in_parm,0x0,sizeof(g_in_parm));
		memset(&g_od_parm,0x0,sizeof(g_od_parm));

		memset(&g_pub_intst,0x0,sizeof(g_pub_intst));
		set_zd_data(DC_SUB_TX_CODE,cExec[j].sub_tx_code);

		handle = (void *)dlopen(libsoname, RTLD_LAZY|RTLD_GLOBAL);
		if ( handle == NULL )
		{
			sprintf( acErrMsg, "打开动态库出错dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* 记录错误日志 */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "[%s]错误退出", cExec[j].prog );
			/*pub_eod_WriJk( "%-40s", acErrMsg );*/
			WRITEMSG

			return(-1);
		}

		execFun = (void(*)(int))dlsym( handle,cExec[j].prog );
		if ( execFun == NULL )
		{
			sprintf( acErrMsg, "生成执行程序出错dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* 记录错误日志 */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "[%s]错误退出", cExec[j].prog );
	/*		pub_eod_WriJk( "%-40s", acErrMsg );*/

			return(-1);
		}
		else
		{
            sprintf(acErrMsg,"cExecProg[j]=[%s]",cExec[j].prog);
            WRITEMSG
			( *execFun )();		/*---执行程序---*/
		}

		if ( dlclose(handle) )
		{
			sprintf( acErrMsg, "关闭错误dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* 记录错误日志 */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "%s错误退出", cExec[j].prog );
	/*		pub_eod_WriJk( "%-40s", acErrMsg );*/

			return(-1);
		}

		if (strcmp(g_pub_tx.reply,"0000"))
		{
			return (-1);
		}

		/* 写日终监控日志结束 */
		pub_base_strpack( cExec[j].prog );
		sprintf( acErrMsg, "%04d %s 结束 %s",cExec[j].do_seq,cExec[j].prog_name, cExec[j].prog);
	/*	pub_eod_WriJk( "%-40s", acErrMsg );*/
		WRITEMSG
		

	return(0);
}

