/**************************************************************
用于测试批量程序的独立运行程序
输入参数：批量程序的动态库函数
          该批量程序中的函数名
***************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include "public.h"
#include "svrpub_c.h"
#include "tx_def_c.h"
#include "tx_flow_def_c.h"
#include "tx_sub_def_c.h"
#include "tx_sub_rel_c.h"
#include "tx_dc_rel_c.h"
#include "dc_acc_rel_c.h"
#include "com_item_c.h"
#include "com_def_chrg_c.h"
#include "mo_def_rate_c.h"
#include "com_chrg_rate_c.h"
#include "com_chrg_hst_c.h"
#include "prdt_ac_id_c.h"
#include "com_sys_parm_c.h"
#include "com_eod_exec_c.h"

void  (*func)() ;
struct com_eod_exec_c grz_com_eod_exec;
void main(argc,argv)
int argc;
char *argv[];
{
	void * handle;
	void (* myFun)();
	char libso[50];
	char func_name[20];
	int ret=0,reply=0;
	int flag=0;

	if(argc!=4){
		printf("Usage error!! Use : eodtst libname funcname !\n");
		exit(1);
		}

	db_open();

	flag=atoi(argv[3]);
	ret=sysinit();
	if(ret<0){printf("系统初始化错误！\n");
		printf("rc=[%d]\n",ret);
		exit(1);
		}

	memset(func_name,0x00,sizeof(func_name));

	sprintf( libso,"%s/lib/%s",getenv("HOME") ,argv[1]);
	printf("libso=%d\n",strlen(libso));

	strcpy(func_name,argv[2]);
	printf("libso is [%s],fun is [%s]\n",libso,func_name);

/*根据程序名取com_eod_exec的内容*/
	if(flag==1){
		memset(&grz_com_eod_exec,0x00,sizeof(struct com_eod_exec_c));
		ret=Com_eod_exec_Sel(reply,&grz_com_eod_exec,"prog='%s'",func_name);
		if(ret){
			printf("取日终执行顺序表错误%d\n",reply);
			exit(1);
			}
		printf("sub_tx_code=[%s]\n",grz_com_eod_exec.sub_tx_code);
		set_zd_data(DC_SUB_TX_CODE,grz_com_eod_exec.sub_tx_code);
		get_zd_data( DC_SUB_TX_CODE , g_pub_tx.sub_tx_code );     /* 交易代码 */	
		printf("SUB=[%s]\n",g_pub_tx.sub_tx_code);
		}

	if(eod_sysinit()!=0){/*初始化系统全局变量*/
		printf("系统初始化错误!\n");
		exit(1);
		}

	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
	if(handle == NULL){
		printf("dlerror = [%d] [%s]\n",errno,dlerror());
		printf("打开错误！\n");
		exit(1);
		}
	printf("handle=%d\n",handle);
		
	myFun = (void(*)(int))dlsym(handle,func_name);
	if (myFun == NULL) {
		printf("dlerror = [%d] [%s]\n",errno,dlerror());
		printf("调用错误！\n");
       	dlerror();
       	exit(1);
       	}
	else (*myFun)();
   	printf("调用函数结束！[%s][%d]\n",__FILE__,__LINE__);
		
	if(dlclose(handle)) {
       	printf("关闭错误！\n");
		exit(1);
       	}
	else{
		printf("程序调用结束\n");
		}
	if(memcmp(g_pub_tx.reply,"0000",4)==0)printf("批量处理成功!\n");
		else printf("批量处理失败[%s]\n",g_pub_tx.reply);
	exit(0);
}

int eod_sysinit()
{
	struct com_sys_parm_c syspc;
	int ret=0;
	char brno[6];

	memset(&syspc,0x00,sizeof(syspc));
	strcpy(g_pub_tx.tx_br_no , KJ_BR_NO );   /* 交易机构 */
	printf("brnogpub=[%s]\n",g_pub_tx.tx_br_no);
	set_zd_data( "0030" , g_pub_tx.tx_br_no);
	get_zd_data("0030",brno);
	printf("brno=[%s]\n",brno);
	
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&syspc,"1=1");
	if(ret!=0){
		printf("ret=[%d]\n",ret);
		return ret;
		}
	g_pub_tx.tx_date=syspc.sys_date;  /* 交易日期 */
	set_zd_long( DC_DATE , g_pub_tx.tx_date);
	/*get_zd_long( DC_TRACE_NO , &g_pub_tx.trace_no);    流水号 */
	/*get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);    流水笔次 */
	/*get_zd_long( DC_TIME , &g_pub_tx.tx_time);         交易时间 */
	memcpy(g_pub_tx.tel,"9999",4 );             /* 操作员 */
	set_zd_data(DC_TEL , g_pub_tx.tel); 
	
	printf("系统交易日期:[%ld]\n",g_pub_tx.tx_date);
	return 0;
}
