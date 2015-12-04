/*************************************************
* �� �� ��:    sp3468.c
* ����������   
*          ֧�������׷����������׵ĺ������ڼ���ˮ
* ��    ��:    wjp
* ������ڣ�   2013��01��31��

insert into tx_def values('3468','֧����������ˮ','00000000000000000000000000001000000000000001100000000000000000000000000000000000000000000000000000000000000000000000000000000100','1','4','0','','');
insert into tx_flow_def values('3468','0','3468','#$');
insert into tx_sub_def values('3468','֧����������ˮ','sp3468','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "in_mst_hst_c.h"

int sp3468()
{
	int  ret = 0;
	long max_trace=0;
	long max_date=0;
	struct in_mst_hst_c in_mst_hst;

	/** ��ʼ��ȫ�ֱ��� **/
	pub_base_sysinit();

	/** ȡֵ����ֵ **/
	memset(&in_mst_hst,0x00,sizeof(in_mst_hst));
	ret=sql_max_long_xier("in_mst_hst","tx_date",&max_date,"ac_id=900004617");
  if(ret)
  {
    sprintf(acErrMsg,"ȡ������ڴ���iRet=[%d]",ret);
    WRITEMSG
    goto ErrExit;
  }
	
sprintf(acErrMsg,"xxȡ�������[%ld]",max_date);
    WRITEMSG
	
	ret=sql_max_long_xier("in_mst_hst","trace_no",&max_trace,"ac_id=900004617 and tx_date=%ld",max_date);
  if(ret)
  {
    sprintf(acErrMsg,"ȡ�����ˮ�Ŵ���iRet=[%d]",ret);
    WRITEMSG
    goto ErrExit;
  }
	set_zd_long("0440",max_date);
	set_zd_long("0450",max_trace);
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
