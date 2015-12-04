/*************************************************
* 文 件 名:  sp9162.c
* 功能描述： 根据需求，返回IC卡电子钱包对应记账科目和内部账户。 
* 作    者:  wzw
* 完成日期： 2012-3-6
insert into com_parm values('ICACC','1','IC卡电子钱包科目','1','22501');

insert into tx_def      values('9162','返回IC卡电子钱包科目','10000000000100000000000000000100000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('9162',0,'9162','#$');
insert into tx_sub_def  values('9162','返回IC卡电子钱包科目','sp9162','0','0');
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "mdm_ac_rel_c.h"

int sp9162()
{
	int ret=0;
	char cAcc_no[8];
	char cAc_no[20];
	
	struct com_parm_c   sCom_parm;
	struct com_parm_c   v_com_parm;
	struct mdm_ac_rel_c sMdm_ac_rel;
		
	memset( cAcc_no,      0x00, sizeof( cAcc_no));
	memset( cAc_no,       0x00, sizeof( cAc_no));
	memset( &sCom_parm,   0x00, sizeof( struct com_parm_c ) );
	memset( &v_com_parm,  0x00, sizeof( struct com_parm_c ));
	memset( &sMdm_ac_rel, 0x00, sizeof( struct mdm_ac_rel_c));
	
  pub_base_sysinit();
	get_zd_data("0310",cAc_no);
	pub_base_strpack(cAc_no);
	pub_base_old_acno(cAc_no);
vtcp_log("%s%d[%s]",__FILE__,__LINE__,cAc_no);

	ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='ICACC' and parm_seqn=1");
	if( ret )
	{
		sprintf(acErrMsg,"error!!ret=[%s]",ret);
		WRITEMSG
		goto ErrExit;
	}
vtcp_log("%s%d[%s]",__FILE__,__LINE__,sCom_parm.val);
	strcpy(cAcc_no,sCom_parm.val);
		
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'", cAc_no);
	if( ret )
	{
		sprintf(acErrMsg,"error!!ret=[%s]",ret);
		WRITEMSG
		goto ErrExit;
	}
	ret = Com_parm_Sel(g_pub_tx.reply,&v_com_parm,"parm_code='%s' and parm_seqn=11",sMdm_ac_rel.opn_br_no);
	if( ret )
	{
		sprintf(acErrMsg,"error!!ret=[%s]",ret);
		WRITEMSG
		goto ErrExit;
	}
vtcp_log("%s%d[%s]",__FILE__,__LINE__,v_com_parm.val);

	set_zd_data("0640",cAcc_no);
	set_zd_data("0300",v_com_parm.val);
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
