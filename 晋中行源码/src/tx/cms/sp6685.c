/*************************************************
* �� �� ��: sp6685.c
* �����������˽�������Ŵ�����ά����ȡ��
*
* ��    ��: chenchao
* ������ڣ�2011-8-7 14:41:30
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:

insert into tx_def values('6685','�Ŵ�����ά����ȡ��','10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('6685','0','6685','#$');
insert into tx_sub_def values ('6685','�Ŵ�����ά����ȡ��','sp6685','0','0');

*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "cms_rollback_c.h"
#include "ln_auth_c.h"

int sp6685()
{
	int	iRet = 0;
	char	cGet_data[201];
	char	cType[2];
	
	struct	ln_auth_c	sLn_auth;
	struct	cms_rollback_c	sCms_rollback;
	
	memset(&sLn_auth,0x00,sizeof(sLn_auth));
	memset(cGet_data,0x00,sizeof(cGet_data));
	memset(cType,0x00,sizeof(cType));
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	
	get_zd_data("0300",cGet_data);		/**��֤���˺�  **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pay_ac_no,cGet_data,sizeof(sLn_auth.pay_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0310",cGet_data);		/**�ж�Э���  **/
	zip_space(cGet_data);
	memcpy(sLn_auth.pact_no,cGet_data,sizeof(sLn_auth.pact_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0250",cGet_data);		/**��֤����  **/
	zip_space(cGet_data);
	memcpy(sLn_auth.name,cGet_data,sizeof(sLn_auth.name)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	get_zd_data("0320",cGet_data);		/**ת���˺�    **/
	zip_space(cGet_data);
	memcpy(sLn_auth.save_ac_no,cGet_data,sizeof(sLn_auth.save_ac_no)-1);
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	/*get_zd_data("0260",cGet_data);		*ת���˺Ż���**/
	get_zd_double("0390",&sLn_auth.amt);		/**��֤����  **/
	get_zd_long("0440",&sLn_auth.mtr_date); /*��������*/
	get_zd_double("0840",&sLn_auth.rate);/*����*/
	get_zd_data("0670",cType);

	if(cType[0] == '1')
	{
		memcpy(sLn_auth.type,"12",2);			/** �ұ�֤�� ����ά��**/
	}else if(cType[0] == '2')
	{
		memcpy(sLn_auth.type,"13",2);			/** �ұ�֤��ȡ��**/
	}else{
		vtcp_log("[%s][%d]���ʹ���type=[%d]",__FILE__,__LINE__,cType);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"���ʹ���!!!");
		goto ErrExit;
	}
	
	sLn_auth.sts[0] = 'X';				/** �Ŵ���������� **/
	memcpy(sLn_auth.cms_tx_br_no,g_pub_tx.tx_br_no,sizeof(sLn_auth.cms_tx_br_no)-1);
	iRet = Ln_auth_Ins(sLn_auth,g_pub_tx.reply);
	if(iRet)
	{
		vtcp_log("[%s][%d]����������ʧ��.����ret=[%d]",__FILE__,__LINE__,iRet);
		strcpy( g_pub_tx.reply, "D107" );
		set_zd_data(DC_GET_MSG,"����������ʧ��!!!");
		goto ErrExit;
	}
	
	memset(&sCms_rollback,0x00,sizeof(sCms_rollback));
	get_zd_data("0580",cGet_data);	/** ��Ȩ���� **/
	zip_space(cGet_data);
	memcpy(sCms_rollback.cms_sq_no,cGet_data,sizeof(sCms_rollback.cms_sq_no));
	memset(cGet_data, 0x00, sizeof(cGet_data));
	
	memcpy(sCms_rollback.beg_table," ln_auth ",sizeof(sCms_rollback.beg_table)-1);
	sCms_rollback.do_type[0] = 'I';
	sprintf(sCms_rollback.beg_sql," sts='X' and pact_no='%s' and type='%s' ",sLn_auth.pact_no,sLn_auth.type);
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
	sprintf( acErrMsg, "�Ŵ��ɴ汣֤�������ɹ�!!!");
	WRITEMSG
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4))
		memcpy(g_pub_tx.reply,"H034",4);
	sprintf( acErrMsg, "�Ŵ��ɴ汣֤������ʧ��[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
