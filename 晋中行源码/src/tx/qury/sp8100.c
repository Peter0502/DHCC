/*************************************************
* 文 件 名:  sp8100.c
* 功能描述： 电话银行限额查询
*
* 作    者:ck  
* 完成日期：2010.01.09 
*
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "com_branch_c.h"
#include "dd_mst_c.h"
#include "cif_addr_inf_c.h"
#include "mdm_ac_rel_c.h"

int sp8100()
{
	struct dd_mst_c f_dd_mst;
	struct mdm_ac_rel_c mdm_ac_rel_t1;

	char f_acno0[25];
	char ac_type[21];
	int  ret;
	char note_sts[10];

  
	memset(ac_type,0x0,sizeof(ac_type));
	memset(note_sts,0x0,sizeof(note_sts));
	memset(&g_pub_tx,0,sizeof(g_pub_tx) );
	memset( &mdm_ac_rel_t1 , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset(&f_dd_mst,0x00,sizeof(struct dd_mst_c));

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
 	pub_base_old_acno( f_acno0 );
	pub_base_strpack( f_acno0 );

	if( strlen(f_acno0) )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_t1," ac_no='%s' ",f_acno0 );
		if (ret == 100)
		{
		sprintf(acErrMsg, "无，请检查!ac_no=[%s]",f_acno0);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
		}
	}
		/*冻结状态*/
		memset(note_sts,0x0,sizeof(note_sts));
		pub_base_dic_show_str(note_sts,"mdm_sts",mdm_ac_rel_t1.note_sts);/*介质状态*/
		vtcp_log("[%s][%d],mdm_ac_rel_t1.note_sts[%s]\n",__FILE__, __LINE__,mdm_ac_rel_t1.note_sts);
		set_zd_data("0810", note_sts);
		vtcp_log("[%s][%d],note_sts[%s]\n",__FILE__, __LINE__,note_sts);
	
	
		if( mdm_ac_rel_t1.ac_id)
	{
		ret=Dd_mst_Sel(g_pub_tx.reply,&f_dd_mst," ac_id='%d' ",mdm_ac_rel_t1.ac_id );
		if (ret == 100) 
		{
		sprintf(acErrMsg, "无，请检查!ac_id=[%s]", mdm_ac_rel_t1.ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply, "M003");
		goto ErrExit;
		}
	}
			vtcp_log("[%s][%d],f_dd_mst.ac_id[%d]\n",__FILE__, __LINE__,f_dd_mst.ac_id);
			set_zd_double("0410", f_dd_mst.bal);/*余额*/
			vtcp_log("[%s][%d],f_dd_mst.bal[%f]\n",__FILE__, __LINE__,f_dd_mst.bal);
			set_zd_double("0420", f_dd_mst.hold_amt);/*冻结金额*/
			vtcp_log("[%s][%d],f_dd_mst.hold_amt[%f]\n",__FILE__, __LINE__,f_dd_mst.hold_amt);
			set_zd_double("0430", f_dd_mst.ctl_amt);/*控制金额 */
			vtcp_log("[%s][%d],f_dd_mst.ctl_amt[%f]\n",__FILE__, __LINE__,f_dd_mst.ctl_amt);
			set_zd_data("0260", f_dd_mst.name);/*户名*/
			vtcp_log("[%s][%d],f_dd_mst.name[%s]\n",__FILE__, __LINE__,f_dd_mst.name);
			pub_base_dic_show_str(ac_type,"ac_type",f_dd_mst.ac_type);/*账户类型*/
			vtcp_log("[%s][%d],f_dd_mst.ac_type[%s]\n",__FILE__, __LINE__,f_dd_mst.ac_type);
			set_zd_data("0820", ac_type);/*账户类型*/
			vtcp_log("[%s][%d],ac_type[%s]\n",__FILE__, __LINE__,ac_type);

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
