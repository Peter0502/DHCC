/*************************************************
* 文 件 名:  sp9812.c
* 功能描述： 根据显示账号查询户名
*
* 作    者:  
* 完成日期： 2003年1月17日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

int sp9812()
{
    struct in_mst_c sIn_mst_c;
    struct dd_mst_c sDd_mst_c;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
	struct com_item_c com_item_c;
    int ret;
   
    memset(&sIn_mst_c,0x0,sizeof(struct in_mst_c));
    memset(&sDd_mst_c,0x0,sizeof(struct dd_mst_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
	memset(&com_item_c, 0x0, sizeof(struct com_item_c));

    get_zd_data("0300",sMdm_ac_rel_c.ac_no);

    sprintf(acErrMsg,"ac_no=[%s]!! ",sMdm_ac_rel_c.ac_no);
    WRITEMSG
	if (strlen(sMdm_ac_rel_c.ac_no) > 7)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s' ",sMdm_ac_rel_c.ac_no);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询对方账号错误!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
    	}
		if( sMdm_ac_rel_c.ac_seqn=9999 )
			sMdm_ac_rel_c.ac_seqn=1;			/** 取活期人民币帐号信息 ***/

		if( sMdm_ac_rel_c.ac_no[0]=='9' && strlen(sMdm_ac_rel_c.ac_no)!=19 )
		{
    	ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_c,"ac_id =%ld and ac_seqn = 0",
          	sMdm_ac_rel_c.ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B005");
        	goto ErrExit;
    	}
    	sprintf(acErrMsg,"name=[%s]!! ",sIn_mst_c.name);
    	WRITEMSG
		set_zd_data( "0270", sIn_mst_c.name); /*对方户名*/
		}
		else
		{
    	ret = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_c,"ac_id =%ld and ac_seqn=%d",
          	sMdm_ac_rel_c.ac_id,sMdm_ac_rel_c.ac_seqn);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B005");
        	goto ErrExit;
    	}
    	sprintf(acErrMsg,"name=[%s]!! ",sDd_mst_c.name);
    	WRITEMSG
		set_zd_data( "0270", sDd_mst_c.name); /*对方户名*/
		}
    } 
	else
	{
		ret = Com_item_Sel(g_pub_tx.reply,&com_item_c,"acc_no='%s'",
				sMdm_ac_rel_c.ac_no);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"查询科目表错误!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B172");
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"该科目不存在!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"B173");
			goto ErrExit;
		}
    	sprintf(acErrMsg,"acc_name=[%s]!! ",com_item_c.acc_name);
    	WRITEMSG
		set_zd_data( "0270", com_item_c.acc_name); /*对方户名*/
	}
   
    

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
