/***************************************************************
* �� �� ��:   spD036.c
* ����������	һ�Զ���˵����̿��Ʋ���
*
* ��    ��:     houyi
* ������ڣ�    20101026
insert into tx_def values('2508','һ�Զ����','','1','3','2','3','');
insert into tx_flow_def values('2508',0,'D036','#0001@1@#=0002@2@#$');
insert into tx_flow_def values('2508',2,'PLKD','#$');
insert into tx_flow_def values('2508',1,'PLKH','#$');
insert into tx_sub_def values('D036','һ�Զ����','spD036','0','0');
insert into tx_sub_def values('PLKD','һ�����','spPLKD','0','0');
insert into tx_sub_def values('PLKH','һ����','spPLKH','0','0');
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
spD036()
{
	char opt_ind[2];
	memset(opt_ind,0x00,sizeof(opt_ind));
	pub_base_sysinit();
	get_zd_data("0720",opt_ind);
	if(opt_ind[0]=='1')/**һ����**/
	{
		goto OkExit1;
	}
	else if(opt_ind[0]=='2')/**һ�����**/
	{
		goto OkExit2;
	}
OkExit1:
	strcpy(g_pub_tx.reply,"0001");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy(g_pub_tx.reply,"0002");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}
