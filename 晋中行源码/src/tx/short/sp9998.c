/*************************************************
* �� �� ��:  sp9998.c
* ���������� ���ݹ�Ա�ŵõ�������������Ϣ
*
* ��    ��:  Seagle
* ������ڣ� 
*
* �޸ļ�¼��                
* ��   ��:   20060612
* �� �� ��:      
* �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_tx_list_c.h"
#include "com_sys_parm_c.h"
#include "com_parm_c.h"

int sp9998()
{
	char fiedlst[100];
	char filter[100];
	char filename[100];
	struct com_tel_c vcom_tel,pcom_tel;
	struct com_branch_c vcom_branch;
	struct com_sys_parm_c vcom_sys_parm;
	struct auth_tx_list_c vauth_tx_list;
	struct com_parm_c sCom_parm;
	FILE *fp;
	int ret=0, i=0, j=0;
	char wherelist[100], tmpstr[50];
	char old_menu[5], new_menu[5];
 	vtcp_log("9998��ʼ��[%s][%d]",__FILE__,__LINE__);

	pub_base_sysinit();
 	vtcp_log("9998init over[%s][%d]",__FILE__,__LINE__);

	memset(wherelist, 0x0, sizeof(wherelist));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(old_menu, 0x0, sizeof(old_menu));
	memset(new_menu, 0x0, sizeof(new_menu));

	ret=Com_sys_parm_Sel(g_pub_tx.reply,&vcom_sys_parm," 1=1" );
	if( ret ) goto ErrExit;

	WRITEMSG

TRACE
	ret = Com_tel_Sel( g_pub_tx.reply,&vcom_tel,
		" tel='%s' ",g_pub_tx.tel );
	if( ret )
	{
		sprintf( acErrMsg,"��Ա������[%s]",g_pub_tx.tel );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O120" );
		goto ErrExit;
	}
	if( vcom_tel.csts[0]=='4' )
	{
		sprintf( acErrMsg,"��Ա״̬��[%s]O[%s]",g_pub_tx.tel,vcom_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O062" );
		goto ErrExit;
	}
	if( vcom_tel.csts[0]=='3' )
	{
		sprintf( acErrMsg,"��Ա״̬��[%s]i[%s]",g_pub_tx.tel,vcom_tel.csts );
		WRITEMSG
		strcpy( g_pub_tx.reply,"O063" );
		goto ErrExit;
	}
	/* �Ƚϵ�ǰ���ں��ϴθ�����ʱ��ļ�� */
	struct com_parm_c com_parm_v;
	int pwd_days=0;
	memset(&com_parm_v,'\0',sizeof(com_parm_v));
	pwd_days=pub_base_CalTrueDays(vcom_tel.pwd_date,vcom_sys_parm.sys_date);
	if(ret=Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='HMMTS' and parm_seqn='1'" )){
		sprintf(acErrMsg, "�黻������������![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "S088");
		goto ErrExit;
	}
	if(pwd_days>atoi(com_parm_v.val))
		set_zd_long("0310",vcom_tel.pwd_date);/* �ϴθ������ʱ�� */

	set_zd_data("0020",vcom_tel.br_no );
	set_zd_data("0030",vcom_tel.br_no );
	set_zd_data("0130",vcom_tel.tx_type );
	set_zd_data("0680",vcom_tel.lvl );
	set_zd_long("0470",vcom_sys_parm.sys_date );

	if ( atol(vcom_tel.tx_type) == 0 )
	{
		sprintf(acErrMsg, "�ù�Աû��Ȩ�޵�½ϵͳ,��ͿƼ�����ϵ");
		WRITEMSG
		strcpy(g_pub_tx.reply, "S081");
		goto ErrExit;
	}
	ret = Com_branch_Sel( g_pub_tx.reply,&vcom_branch,
		" br_no='%s' ",vcom_tel.br_no );
	if( ret ) goto ErrExit;

	set_zd_data("0250",vcom_branch.br_name ); /*��������*/


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
