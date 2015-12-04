/***************************************************************
* 文 件 名:   spD035.c
* 功能描述：	4209交易的流程控制部分
*
* 作    者:     houyi
* 完成日期：    20101019
insert into tx_def values('2214','二级账户记账','10000000000000000000000011000111000000000000000000000000000000000011100100000000110000000000000000000000000000000000000000000100','1','3','2','3','');
insert into tx_flow_def values('2214',0,'D035','#=0000@1@#=0001@1@#=0002@2@#$');
insert into tx_flow_def values('2214',1,'D034','#=0000@2@#=0001@#$');
insert into tx_flow_def values('2214',2,'D033','#$');
insert into tx_sub_def values('D035','二级账户记账控制','spD035','0','0');
insert into tx_sub_def values('D034','子账户存取','spD034','0','0');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
spD035()
{
	char opt_ind[2];
	memset(opt_ind,0x00,sizeof(opt_ind));
	pub_base_sysinit();
	get_zd_data("0720",opt_ind);
	if(opt_ind[0]=='2')/**单边借**/
	{
		goto OkExit1;
	}
	else if(opt_ind[0]=='3')/**单边贷**/
	{
		goto OkExit2;
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit1:
	strcpy(g_pub_tx.reply,"0001");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy(g_pub_tx.reply,"0002");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}