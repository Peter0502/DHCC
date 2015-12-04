/*************************************************
* �� �� ��: sp6676.c
* ��������������Ŵ�����չ�ڵ�֪ͨ
*
* ��    ��: chenchao
* ������ڣ�2011-7-23 10:52:36
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6676','�Ŵ�����չ��֪ͨ','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6676','0','6676','#$');
insert into tx_sub_def values ('6676','�Ŵ�����չ��֪ͨ','sp6676','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"

int sp6676()
{
	int	iRet = 0;
	long	cif_no = 0;
	char	cGet_data[201];
	
	struct	cms_rollback_c	sCms_rollback;
	struct	ln_auth_c	sLn_auth;
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	
	pub_base_sysinit();
	
	get_zd_data("0370",cGet_data);	/** ��ݺ� **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_long("0530",&sLn_auth.time_lmt);	/** ���� **/
	get_zd_long("0440",&sLn_auth.mtr_date);	/** �������� **/
	get_zd_double("0840",&sLn_auth.rate);	/** չ������ **/
	get_zd_long("0520",&sLn_auth.cif_no);	/** �ͻ��� **/
	
	get_zd_data("0250",cGet_data);	/** �ͻ�����   **/
	zip_space(cGet_data);
	memcpy(sLn_auth.name,cGet_data,sizeof(sLn_auth.name));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_double("0400",&sLn_auth.amt);	/** չ�ڽ��  **/
	memcpy(sLn_auth.type,"02",2);		/**02-��ʶչ��֪ͨ **/
	sLn_auth.sts[0] = 'X';			/** �Ŵ����� **/
	
	memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]���������ǼǱ�ʧ��.����ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"���������ǼǱ�ʧ��!!!");
		goto ErrExit;
	}
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," ln_auth ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," pact_no='%s' and type='02' and sts='X' ",sLn_auth.pact_no);
	sCms_rollback.sts[0] = '0';
	get_zd_data("1230",sCms_rollback.filler);
	iRet = Cms_rollback_Ins(sCms_rollback,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����ع��ǼǱ�����.����iRet=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����ع��ǼǱ�����!!!"); 
		goto ErrExit;
	}
OkExit:
	sprintf( acErrMsg, "�Ŵ�����չ�ڵ�֪ͨ�ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)){
	memcpy(g_pub_tx.reply,"H034",4);}
	sprintf( acErrMsg, "�Ŵ�����չ�ڵ�֪ͨʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
