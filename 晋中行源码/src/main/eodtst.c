/**************************************************************
���ڲ�����������Ķ������г���
�����������������Ķ�̬�⺯��
          �����������еĺ�����
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
	if(ret<0){printf("ϵͳ��ʼ������\n");
		printf("rc=[%d]\n",ret);
		exit(1);
		}

	memset(func_name,0x00,sizeof(func_name));

	sprintf( libso,"%s/lib/%s",getenv("HOME") ,argv[1]);
	printf("libso=%d\n",strlen(libso));

	strcpy(func_name,argv[2]);
	printf("libso is [%s],fun is [%s]\n",libso,func_name);

/*���ݳ�����ȡcom_eod_exec������*/
	if(flag==1){
		memset(&grz_com_eod_exec,0x00,sizeof(struct com_eod_exec_c));
		ret=Com_eod_exec_Sel(reply,&grz_com_eod_exec,"prog='%s'",func_name);
		if(ret){
			printf("ȡ����ִ��˳������%d\n",reply);
			exit(1);
			}
		printf("sub_tx_code=[%s]\n",grz_com_eod_exec.sub_tx_code);
		set_zd_data(DC_SUB_TX_CODE,grz_com_eod_exec.sub_tx_code);
		get_zd_data( DC_SUB_TX_CODE , g_pub_tx.sub_tx_code );     /* ���״��� */	
		printf("SUB=[%s]\n",g_pub_tx.sub_tx_code);
		}

	if(eod_sysinit()!=0){/*��ʼ��ϵͳȫ�ֱ���*/
		printf("ϵͳ��ʼ������!\n");
		exit(1);
		}

	handle = (void *)dlopen(libso, RTLD_LAZY|RTLD_GLOBAL);
	if(handle == NULL){
		printf("dlerror = [%d] [%s]\n",errno,dlerror());
		printf("�򿪴���\n");
		exit(1);
		}
	printf("handle=%d\n",handle);
		
	myFun = (void(*)(int))dlsym(handle,func_name);
	if (myFun == NULL) {
		printf("dlerror = [%d] [%s]\n",errno,dlerror());
		printf("���ô���\n");
       	dlerror();
       	exit(1);
       	}
	else (*myFun)();
   	printf("���ú���������[%s][%d]\n",__FILE__,__LINE__);
		
	if(dlclose(handle)) {
       	printf("�رմ���\n");
		exit(1);
       	}
	else{
		printf("������ý���\n");
		}
	if(memcmp(g_pub_tx.reply,"0000",4)==0)printf("��������ɹ�!\n");
		else printf("��������ʧ��[%s]\n",g_pub_tx.reply);
	exit(0);
}

int eod_sysinit()
{
	struct com_sys_parm_c syspc;
	int ret=0;
	char brno[6];

	memset(&syspc,0x00,sizeof(syspc));
	strcpy(g_pub_tx.tx_br_no , KJ_BR_NO );   /* ���׻��� */
	printf("brnogpub=[%s]\n",g_pub_tx.tx_br_no);
	set_zd_data( "0030" , g_pub_tx.tx_br_no);
	get_zd_data("0030",brno);
	printf("brno=[%s]\n",brno);
	
	ret=Com_sys_parm_Sel(g_pub_tx.reply,&syspc,"1=1");
	if(ret!=0){
		printf("ret=[%d]\n",ret);
		return ret;
		}
	g_pub_tx.tx_date=syspc.sys_date;  /* �������� */
	set_zd_long( DC_DATE , g_pub_tx.tx_date);
	/*get_zd_long( DC_TRACE_NO , &g_pub_tx.trace_no);    ��ˮ�� */
	/*get_zd_long( DC_TRACE_CNT , &g_pub_tx.trace_cnt);    ��ˮ�ʴ� */
	/*get_zd_long( DC_TIME , &g_pub_tx.tx_time);         ����ʱ�� */
	memcpy(g_pub_tx.tel,"9999",4 );             /* ����Ա */
	set_zd_data(DC_TEL , g_pub_tx.tel); 
	
	printf("ϵͳ��������:[%ld]\n",g_pub_tx.tx_date);
	return 0;
}
