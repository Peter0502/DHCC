/*************************************************
* 文 件 名: spG001.c
* 功能描述：调用日终批处理主控程序 
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
#include "data_dic_c.h"

struct com_eod_exec_c grz_com_eod_exec; /**全局，用于向子程序传递控制序列**/
char	cCnt[21];

int spG001( )
{
	int	ret;
	
	memset( cCnt, 0x00, sizeof( cCnt ) );
	
	get_zd_data("0670", cCnt );
	if( cCnt[0]=='S' )
	{
		pub_eod_WriJk("%s",
				"★ ☆----------------------------------☆ ★");
   		pub_eod_WriJk("%-40s",
				"★ ☆            日终试结转            ☆ ★");
		pub_eod_WriJk("%s",
				"★ ☆----------------------------------☆ ★");
		sprintf( acErrMsg, "------------日终试结转开始!!!!");
		WRITEMSG
	}
	else
	{
		sprintf( acErrMsg, "------------日终处理开始[%s]!!!!",cCnt);
		WRITEMSG
	}
	
	ret = pub_eod_deal();
	if ( ret ) 
	{
		sprintf( acErrMsg, "调用日终处理主控程序错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"日终一次运行成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"日终一次运行失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * 函  数 名：  pub_eod_deal
********************************************************************/
int pub_eod_deal()
{
	struct com_sys_parm_c 	sSysParm;	/* 公共系统参数表 */
	struct com_eod_exec_c	sEodExec;	/* 日终处理顺序表 */
	struct pub_tx v_pub_tx;
	char	cDoType[2];			/* 执行时机 0翻牌前 1翻牌 2翻牌后 */
	int	ret, i;
	
	memset( &sEodExec, 0x00, sizeof( sEodExec ) );
	memset( &sSysParm, 0x00, sizeof( sSysParm ) );
	memset( &v_pub_tx, 0x00, sizeof( v_pub_tx ) );
			
	/* 数据初始化 */
	pub_base_sysinit();
	memcpy( &v_pub_tx, &g_pub_tx, sizeof( v_pub_tx ) );

	/* 记录日终日志开始 */
	pub_eod_WriJk("%s","========★ ☆==============☆ ★========");
   	pub_eod_WriJk("%-40s","日终处理开始");

	/**结束主控的事物**/
	ret=sql_commit();
	TRACE

	/* 先处理翻牌前的程序 0翻牌前 1翻牌 2翻牌后 */
	for ( i=0; i<=2; i++ )
	{
		sprintf( cDoType, "%d", i );

		/* 根据公共系统参数表，判断状态是否是1 */
		if ( pub_base_GetSysparm( &sSysParm ) )
		{
			sprintf( acErrMsg, "取公共系统参数表错!" );
			WRITEMSG
			goto ErrExit;			
		}
	
		/*** rob *** 20030701 *******/
		set_zd_long( DC_DATE , sSysParm.sys_date );        /* 交易日期 */
		set_zd_data( DC_TEL , "9999" );

		ret = pub_eod_ExecProg( cDoType, sSysParm.sys_date );
		if ( ret )
		{
			sprintf( acErrMsg, "执行日终处理错ret=[%d]",ret );
			WRITEMSG
			pub_eod_WriJk("%-40s","日终处理失败");
		    pub_eod_WriJk("========※ ※ =============※ ※========");
			goto ErrExit;
		}
	}
    pub_eod_WriJk("%-40s","日终处理成功");

	/**重新启事务**/
	sql_begin();	
	TRACE

	/* 修改顺序表所有记录完成，标志为N */
	/* 如果不是年终，直接修改为N */
	/*如果是年终,第一次不修改标志，第二次修改 */


	/*--------------------------------------------------------*/
	if( cCnt[0]!='S' )
	{
		ret = sql_execute("update com_eod_exec set finish='N'" );
		if ( ret )
		{
    		pub_eod_WriJk("%-40s","恢复处理顺序表的状态出错");
			sprintf( acErrMsg, "修改日终处理顺序表错" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "G005" );	
			goto ErrExit;
		}
	}
	else
	{
		ret = sql_execute(
			"update com_eod_exec set finish='N' where eoy_num in('0','2','4')" );
		if ( ret )
		{
    		pub_eod_WriJk("%-40s","恢复处理顺序表的状态出错");
			sprintf( acErrMsg, "修改日终处理顺序表错" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "G005" );	
			goto ErrExit;
		}
	}
	/*--------------------------------------------------------*/
	
	/* 记录日终结束 */
    	pub_eod_WriJk("%-40s","恢复处理顺序表的状态成功");
    pub_eod_WriJk("========● ○==============○ ●========");
	
OkExit:
	set_zd_data( DC_TEL , v_pub_tx.tel );
	return 0;
ErrExit:
	set_zd_data( DC_TEL , v_pub_tx.tel );
	return -1;
}
/**********************************************************************
 * 函  数 名：  exec_eod_prog
 * 函数 功能:   调用日终处理程序，执行并记录日终日志
 * 参  数：
 * 输入：  
 *		char	*do_type	程序执行时机 0翻牌前 1翻牌 2翻牌后		
 *		long sys_date,		系统日终日期
 *
 * 输出：  
 *
 * 返回值：  0:成功  -1：失败
 *           
 * 修改历史： 
 *
********************************************************************/
int	pub_eod_ExecProg( char *do_type, long sys_date)
{
	struct com_eod_exec_c sEodExec;
	struct com_eod_exec_c cExec[512];
	int iSeq[256];
	int	ret = 0;
	int	i = 0;
	int	j = 0;

	void *handle;
	void (* execFun)();
	char libsoname[64];

		/**重新启事务**/
	sql_begin();	
	TRACE

	memset(cExec, 0x0, sizeof(cExec));
	
	/* 声明打开游标  */
	ret = Com_eod_exec_Dec_Sel ( g_pub_tx.reply, 
		"do_type = '%s' and using = 'Y' and finish = 'N' order by do_seq", 
		do_type );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Com_eod_exec_Dec_Sel错,[%d]", ret );
		WRITEMSG
		return(-1);
	}
		
	while ( 1 )
	{
		memset( &sEodExec, 0x00, sizeof( struct com_eod_exec_c ) );
	
		/*  从日终顺序表中取出一条记录  */
		ret = Com_eod_exec_Fet_Sel( &sEodExec, g_pub_tx.reply );
		if ( ret == 100 ) break;
		if ( ret )
		{
			sprintf( acErrMsg, "执行	Com_eod_exec_Fet_Sel错[%d]", ret );
			WRITEMSG
			return(-1);
		}
	
		/* 判断是否处于节假日，节假日是否执行 */
		/* 如果不是节假日，执行；如果是节假日，但是节假日标志为执行，执行 */
		ret = pub_eod_chkHoliday( sys_date );
		if ( ret == -1 )	/* 执行错误 */
		{
			sprintf( acErrMsg, "检查节假日错误" );
			WRITEMSG
			return(-1);
		}
		else if ( ret == 1 )
		{
			/* 检查节假日是否可以执行 */
			if ( sEodExec.holiday[0] == 'N' )
			{
				goto CONTINUE;
			}	
		}
		
		if( cCnt[0]=='S' ) /**试年结***/
		{
			if( sEodExec.eoy_num[0]=='2' ) /**仅正式日终执行**/
				continue;
		}
		else /**正式日终**/
		{
			if( sEodExec.eoy_num[0]=='1' ) /**仅试年结执行**/
				goto CONTINUE;
			if( sEodExec.eoy_num[0]=='4' ) /**仅试年结执行**/
				goto CONTINUE;
		}

		/* 计算执行周期、子周期、开始执行日期和结束执行日期 */
		/* 判断是否处于执行期内 */	
		ret = pub_eod_chkSysdate( sys_date, sEodExec.exec_pd,
					sEodExec.pd_month, sEodExec.exec_beg,
					sEodExec.exec_end );
		if ( ret )
		{
			goto CONTINUE;
		}

		pub_base_strpack( sEodExec.prog );
		/*strcpy( cExecProg[i],sEodExec.prog ); */
		memcpy( &cExec[i],&sEodExec,sizeof(sEodExec) );
		iSeq[i] = sEodExec.do_seq;

		i++;

		continue;

	 CONTINUE:
		/* 修改finish标志 */
		ret = sql_execute("update com_eod_exec set finish = 'Y' \
				where do_seq=%d and prog='%s' ",sEodExec.do_seq,sEodExec.prog );
		if ( ret )
		{
			sprintf( acErrMsg, "修改日终顺序表标志错!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"G005");
			return(-1);
		}
	}

	Com_eod_exec_Clo_Sel ();

		/**结束事物**/
		ret=sql_commit();
		TRACE

	for(j=0; j<i; j++)
	{
		init_data_dic();

		/* 写日终监控日志开始 */		
		sprintf( acErrMsg, "%04d %s 开始 %s",cExec[j].do_seq,cExec[j].prog_name, cExec[j].prog );
		pub_eod_WriJk( "%-40s", acErrMsg );

		memset(libsoname,0x0,sizeof(libsoname));

		sprintf( libsoname,"%s/lib/",getenv("HOME") );
		pub_base_strpack( cExec[j].prog );
		strcat( libsoname, cExec[j].prog );
		strcat( libsoname,".so" );
		pub_base_strpack( libsoname );

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
			pub_eod_WriJk( "%-40s", acErrMsg );

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
			pub_eod_WriJk( "%-40s", acErrMsg );

			return(-1);
		}
		else
		{
            sprintf(acErrMsg,"cExecProg[j]=[%s]",cExec[j].prog);
            WRITEMSG
			( *execFun )();		/*---执行程序---*/
		}

		vtcp_log("prog=[%s][%s][%d]",cExec[j].prog,__FILE__,__LINE__);
		if ( dlclose(handle) )
		{
			sprintf( acErrMsg, "关闭错误dlerror = [%d][%s]",errno,dlerror());
			WRITEMSG
			strcpy(g_pub_tx.reply,"S027");

			/* 记录错误日志 */
			pub_base_strpack( cExec[j].prog );
			sprintf( acErrMsg, "%s错误退出", cExec[j].prog );
			pub_eod_WriJk( "%-40s", acErrMsg );

			return(-1);
		}

		vtcp_log("prog=[%s][%s][%d]",cExec[j].prog,__FILE__,__LINE__);
		if (strcmp(g_pub_tx.reply,"0000"))
		{
			vtcp_log("g_pub_tx.reply=[%s][%s][%d]",g_pub_tx.reply,__FILE__,__LINE__);
			return (-1);
		}

		/* 写日终监控日志结束 */
		pub_base_strpack( cExec[j].prog );
		sprintf( acErrMsg, "%04d %s 结束 %s",cExec[j].do_seq,cExec[j].prog_name, cExec[j].prog);
		pub_eod_WriJk( "%-40s", acErrMsg );
		
		/* 修改日终顺序表是否完成finish标志 */
		/* 如果是年终,但是当前执行顺序第一次的,判断所调用的程序是两次都执行,*/
		/*并且第一次不要求改变标志finish的,不修改finish标志,直接继续下一个程序*/
		
	vtcp_log("CHANGE IND[%s] [%s]-[%s]",cCnt,cExec[j].eoy_num,cExec[j].prog );
		
		sprintf( acErrMsg, "[%s]修改日终顺序表标志!" ,cExec[j].prog );
		WRITEMSG

		/* 修改finish标志 */
		ret = sql_execute("update com_eod_exec set finish = 'Y' \
				where do_seq = %d and prog = '%s' ", iSeq[j], cExec[j].prog );
		if ( ret )
		{
			sprintf( acErrMsg, "修改日终顺序表标志错!" );
			WRITEMSG
			strcpy(g_pub_tx.reply,"G005");
			return(-1);
		}
		sql_commit();/*JYW修改提交成功标志*/

		sprintf( acErrMsg, "[%s]修改日终顺序表标志成功已提交111!", cExec[j].prog );
		WRITEMSG
	}/* end for */
	
	return(0);
}
init_data_dic()
{
	struct data_dic_c data_dic;
	int ret;

	ret = Data_dic_Dec_Sel( g_pub_tx.reply , 
		"data_code>'0220' or data_code[1,3]='015'" );
	if (ret)
	{
		WRITEMSG
		goto ErrExit;
	}
	while(1)
	{
		ret = Data_dic_Fet_Sel( &data_dic , g_pub_tx.reply );
		if( ret==100 ) break;
		if (ret)
		{
			WRITEMSG
			goto ErrExit;
		}
		
		if( data_dic.data_leng==0 ) continue;

		switch( data_dic.data_type[0] )
		{
			case '0':
				set_zd_data( data_dic.data_code,"" );
				break;
			case '1':
				set_zd_long( data_dic.data_code,0 );
				break;
			case '4':
			case '2':
				set_zd_double( data_dic.data_code,0.00 );
				break;
		}
	}
	Data_dic_Clo_Sel();

	vtcp_log("清理数据字典ok");
	return 0;
ErrExit:
	vtcp_log("清理数据字典error");
	return 1;
}
