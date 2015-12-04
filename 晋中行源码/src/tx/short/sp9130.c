/*************************************************
* �� �� ��: sp9130.c
* �����������޸����뷢�Ͷ���
* ��    ��: 
* ������ڣ�
* �޸ļ�¼��   
* 1. ��    ��:2011-01-26
*    �� �� ��:������
*    �޸�����:������еĶ�������ע������
* 2. 
*�������ã�
insert into tx_def values('9130','�޸����뷢�Ͷ���','10000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000','1','6','0');
insert into tx_flow_def values('9130','0','9130','#$');
insert into tx_sub_def values('9130','�޸����뷢�Ͷ���','sp9130','0','0');
*************************************************/
#include <string.h>

#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"
#include "mob_acct_type_c.h"
#include "mdm_ac_rel_c.h"

int sp9130()
{ 
	char cSh_no[100]={0};
  int ret=0;
  char cMessage[257]={0}; 
  long date;

  
  struct mob_sendmail_c sMob_sendmail;
  struct com_sys_parm_c sCom_sys_parm;
  struct mob_acct_type_c sMob_acct_type;
  struct mdm_ac_rel_c sMdm_ac_rel;
  

	ret = sql_begin(); 
	if (ret)
	{
		sprintf(acErrMsg, "������ʧ��!!!");
		WRITEMSG
		goto ErrExit;
	}
	
	memset(&sMob_sendmail,0x0,sizeof(sMob_sendmail));
	memset(&sCom_sys_parm,0x0,sizeof(sCom_sys_parm));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	
	ret= Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(ret)
	{
		vtcp_log("[%s][%d]�õ�ϵͳ���ڴ�![%d]\n",__FILE__,__LINE__,ret);
		goto ErrExit;
	}
	set_zd_long("0050",sCom_sys_parm.sys_date);
	if ( pub_base_sysinit() )
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!!");
		WRITEMSG
		goto ErrExit;
	}	

	
	sprintf(acErrMsg,"��ʼ��ȫ�ֱ��� PASS!");
	WRITEMSG
	get_zd_data("0300",cSh_no);   
	get_zd_data("1250",cMessage);
	
	vtcp_log("%s,%d dddd �˺�=[%s]",__FILE__,__LINE__,cSh_no);
	ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",cSh_no);
	if(ret)
	{
		vtcp_log("[%s][%d]��ѯ�˻�ac_id����![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	sMob_sendmail.ac_id = sMdm_ac_rel.ac_id;
	strcpy( sMob_sendmail.tx_code,g_pub_tx.tx_code);
	strcpy( sMob_sendmail.ac_no,cSh_no);
	strcpy( sMob_sendmail.tel, g_pub_tx.tel );
	sMob_sendmail.trace_no = g_pub_tx.trace_no;
	sMob_sendmail.tx_date = g_pub_tx.tx_date;
	sMob_sendmail.tx_time = g_pub_tx.tx_time;
	strcpy( sMob_sendmail.opt_no, "110" );
	/*
	strcpy(sMob_sendmail.mobile,cSh_no);
	*/
	strcpy( sMob_sendmail.message, cMessage );
	strcpy( sMob_sendmail.timeflag, "0" );
	sMob_sendmail.t_date = 0;
	sMob_sendmail.t_time = 0;
	strcpy( sMob_sendmail.send_level, "0" );
	sMob_sendmail.sene_times = 1;
	sMob_sendmail.send_date = 0;
	sMob_sendmail.send_time = 0;
	sMob_sendmail.sxf_amt = 0.00;
	strcpy(sMob_sendmail.proc_flag, "0" );
	sMob_sendmail.mobseq = 0;
	strcpy( sMob_sendmail.tx_type, "2" );  
	ret = Mob_acct_type_Dec_Sel(g_pub_tx.reply,"ac_no = '%s'",cSh_no);
	if(ret != 0 )
	{
		sprintf(acErrMsg,"Mob_acct_type��׼��������%d��",ret);
		strcpy(g_pub_tx.reply,"D102");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mob_acct_type_Fet_Sel(&sMob_acct_type,g_pub_tx.reply);
	if(ret)
	{
			vtcp_log("[%s][%d]��ѯmob_acct_type�����![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
	}
	strcpy(sMob_sendmail.mobile,sMob_acct_type.mobile);
	Mob_sendmail_Debug(&sMob_sendmail);
	ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
	if (ret)
	{
		sprintf( acErrMsg, "дmob_sendmail��ʧ��!!![%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"MT09");
		goto ErrExit;
	}
	
OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"����������Ϣά���ɹ���[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"����������Ϣά��ʧ�ܣ�[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}

