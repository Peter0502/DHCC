/*************************************************
* �� �� ��:  spW011.c
* ���������� �������ʵ�λ��Ϣ��ѯ 
*
* ��    ��:  hzh 
* ������ڣ� 2013��6��19�� 
*
* �޸ļ�¼�� 
* ��   ��:	
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_hst_c.h"
#include "ag_unitinfo_c.h"
#include "td_mst_hst_c.h"
#include "com_branch_c.h"
#include "mdm_unprt_hst_c.h"
#include "dd_mst_c.h"
#include "chnl_self_trade_c.h"
#include "ag_qs_gd_rel_c.h"
extern char *get_env_info(char *infoname);
spW011()
{
	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_qs_gd_rel_c sAg_qs_gd_rel;

	char  sUnit_no[4+1];
	char sBankAcctNo[32+1];
	char sUnitName[60+1];
	int ret = 0;
	
	memset(&sAg_unitinfo, 0x00, sizeof(struct ag_unitinfo_c));
	memset(&sAg_qs_gd_rel, 0x00, sizeof(struct ag_qs_gd_rel_c));
	memset(sUnit_no, 0x00, sizeof(sUnit_no));
	memset(sBankAcctNo,0x00,sizeof(sBankAcctNo));
	memset(sUnitName,0x00,sizeof(sUnitName));
	pub_base_sysinit();

	get_zd_data("0920", sUnit_no);
	vtcp_log("[%s],[%d],sUnit_no=%s", __FILE__, __LINE__, sUnit_no);
	get_zd_data("0300", sBankAcctNo);
	vtcp_log("[%s],[%d],sBankAcctNo=%s", __FILE__, __LINE__, sBankAcctNo);
	
	/**У����ҵ�����˻��͹����˻�������ϵ**/
	memset( &sAg_qs_gd_rel, 0x00, sizeof(sAg_qs_gd_rel) );
	ret = Ag_qs_gd_rel_Sel( g_pub_tx.reply, &sAg_qs_gd_rel, \
		"qs_ac_no = '%s' and unit_no = '%s'", sBankAcctNo,sUnit_no );
	if ( ret )
	{
		sprintf(acErrMsg,"��ѯ��������ҵ������˻��͹����˻�������ʧ��");
		WRITEMSG
		strcpy( g_pub_tx.reply,"EB05" );
		goto ErrExit;
	}
	vtcp_log("%s,%d �����������ҵ������˻��͹����˻�������ͨ��!",__FILE__,__LINE__);
	
	/**��ѯ**/	
	ret = Ag_unitinfo_Sel(&g_pub_tx.reply, &sAg_unitinfo,
		    "unit_no='%s'",
		    sUnit_no);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"��λ�����Ϣ������[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"H009");
			return 1;
		}
		else if(ret)
		{
			sprintf(acErrMsg,"��λ�����д�����߿�Ʊ�����ѹ�[%d]",ret);
			WRITEMSG
			strcpy (g_pub_tx.reply,"H048");
			return 1;
		}
		set_zd_data("0250",sAg_unitinfo.unit_name);
		set_zd_data("0300",sAg_unitinfo.bank_acct_no);
		vtcp_log("[%s],[%d],�����˺�Ϊ[%s]�����ɻ���Ϊ[%s]", __FILE__, __LINE__, sAg_unitinfo.bank_acct_no,sAg_unitinfo.unit_name);

GoodExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before bad return: reply [%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
