/*************************************************
* 文 件 名:  sp9971.c
* 功能描述： 检查最小介质号 (储蓄部提)
*
* 作    者:  rob
* 完成日期： 2003年8月06日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

int sp9971()
{
	int	ret;
	struct mdm_attr_c mdm_attr_c;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct td_mst_c o_td_mst;

	memset(&mdm_attr_c, 0x0, sizeof(struct mdm_attr_c));
	memset(&v_mdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(&o_td_mst, 0x0, sizeof(struct td_mst_c));

	/*初始系统*/
	pub_base_sysinit();

	/*前台取值*/
	get_zd_data("1023",g_pub_tx.mdm_code); 
	get_zd_data("1021",g_pub_tx.crd_no); 
	get_zd_data("102T",g_pub_tx.beg_note_no);  
	get_zd_data("102T",g_pub_tx.end_note_no); 
	get_zd_double("102F",&g_pub_tx.tx_amt1); 

	pub_base_strpack( g_pub_tx.beg_note_no );
	if( !strlen(g_pub_tx.beg_note_no) )
		goto OkExit;

	/* 查询介质帐号对照表 */
	pub_base_strpack(g_pub_tx.crd_no);
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",
						 g_pub_tx.crd_no);
	if (ret != 0 && ret != 100)
	{
		sprintf(acErrMsg, "查询介质帐号对照表错误!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "P147");
		goto ErrExit;
	}
	else if (ret == 100)
	{
		sprintf(acErrMsg, "介质帐号对照表中不存在该记录!! [%d]", ret);
		WRITEMSG
		strcpy(g_pub_tx.reply, "B117");
		goto ErrExit;
	}
	strcpy(g_pub_tx.mdm_code, sMdm_ac_rel.mdm_code);

	/*** 读取介质属性 ***/
	ret=Mdm_attr_Sel(&g_pub_tx.reply,&mdm_attr_c," mdm_code='%s' ", \
					 g_pub_tx.mdm_code);
	if( ret==100 )
	{
		sprintf(acErrMsg,"该介质属性不存在![%d][%s]",
			g_reply_int,g_pub_tx.mdm_code);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P132");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取介质属性异常![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P133");
		goto ErrExit;
	}

	strcpy(g_pub_tx.note_type,mdm_attr_c.note_type);
	strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);

vtcp_log("PQAAA[%s]",g_pub_tx.beg_note_no );TRACE
	sprintf(acErrMsg,"mdm_attr_c.note_type=[%s]",mdm_attr_c.note_type);
	WRITEMSG

	/*** 特种存单部提处理Add By Azz ***/
	if( !strncmp(g_pub_tx.note_type,"014",3) )
	{
		ret = Td_mst_Sel(g_pub_tx.reply,&o_td_mst,"ac_id=%ld", \
						 sMdm_ac_rel.ac_id);
		if( ret==100 )
		{
			sprintf(acErrMsg,"账号不存在![%ld]",sMdm_ac_rel.ac_id);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P132");
			goto ErrExit;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"查找出错![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}

		if( pub_base_CompDblVal( o_td_mst.bal-g_pub_tx.tx_amt1,100000.00 )<0 )
			strcpy( g_pub_tx.note_type,"013" );
	}

	/*需要凭卡号开户，如卡类：开户时需要输入卡号*/
	if( mdm_attr_c.no_ind[0]=='Y' )
	{
		if( !strlen(g_pub_tx.crd_no) )
		{
			sprintf(acErrMsg,"需要输入卡号");
			WRITEMSG
			strcpy( g_pub_tx.reply,"D111" );
			goto ErrExit;
		}

		sprintf(acErrMsg,"输入的卡号为[%s]",g_pub_tx.crd_no);
		WRITEMSG

		/**检查卡号是否已经开过**/
		ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&v_mdm_ac_rel,"ac_no='%s'", \
							g_pub_tx.crd_no ) ;
		if( ret && ret!=100 ) goto ErrExit;
		else if( !ret )
		{
			sprintf(acErrMsg,"需要输入卡号[%s],ret=[%d]",g_pub_tx.crd_no,ret);
			WRITEMSG
			strcpy( g_pub_tx.reply,"D113" );
			goto ErrExit;
		}
		strcpy( g_pub_tx.ac_no,g_pub_tx.crd_no );

		pub_base_strpack(g_pub_tx.beg_note_no);
		if( strlen(g_pub_tx.crd_no) && !strlen(g_pub_tx.beg_note_no) )
		{
			/*** 根据卡号取凭证号 ***/
			pub_card_CardToNote(g_pub_tx.crd_no,g_pub_tx.beg_note_no);
			strcpy(g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
vtcp_log("PQAAA[%s]",g_pub_tx.end_note_no );
		}
vtcp_log("PAAAA[%s]",g_pub_tx.end_note_no );
	}

	/* 需要凭证销号 */
	if( mdm_attr_c.only_ind[0]=='Y' ) 
	{
vtcp_log("PBBBBBBBBBBBBB[%s]",g_pub_tx.note_type );
		ret=pub_com_ChkNoteMin( g_pub_tx.note_type,g_pub_tx.beg_note_no,
							 g_pub_tx.tel);
		if( ret )
		{
			goto ErrExit;
		}
	}

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
