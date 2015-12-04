/*************************************************
* 文 件 名:  sp9819.c
* 功能描述： 根据显示账号查询户名和余额
*
* 作    者:  rob
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

int sp9819()
{
    struct in_mst_c sIn_mst_c;
    struct mdm_ac_rel_c sMdm_ac_rel_c;
    char ac_no[24];
    int flag;
    int ret;
   
    memset(&sIn_mst_c,0x0,sizeof(struct in_mst_c));
    memset(&sMdm_ac_rel_c,0x0,sizeof(struct mdm_ac_rel_c));
    memset(ac_no,0x0,sizeof(ac_no));

    get_zd_data("0310",ac_no);
    if (strcmp(ac_no,"") != 0)
    {
        strcpy(sMdm_ac_rel_c.ac_no,ac_no);
        sprintf(acErrMsg,"ac_no=[%s] [%s]!! ",sMdm_ac_rel_c.ac_no,ac_no);
        WRITEMSG
        flag = 1;
    }
    
    memset(ac_no,0x0,sizeof(ac_no));
    get_zd_data("0300",ac_no);
    if (strcmp(ac_no,"") != 0)
    {
        strcpy(sMdm_ac_rel_c.ac_no,ac_no);
        sprintf(acErrMsg,"ac_no=[%s] [%s]!! ",sMdm_ac_rel_c.ac_no,ac_no);
        WRITEMSG
        flag = 2;
    }

    ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_c,"ac_no = '%s' and \
          ac_seqn= 0 ",sMdm_ac_rel_c.ac_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询对方账号错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B004");
        goto ErrExit;
    }

    ret = In_mst_Sel(g_pub_tx.reply,&sIn_mst_c,"ac_id =%ld and ac_seqn = 0",
          sMdm_ac_rel_c.ac_id);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询对方户名错误!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B005");
        goto ErrExit;
    }
    
   
    if (flag == 1)
    {
	    set_zd_data( "0250", sIn_mst_c.name); /*拆借户名*/
	    set_zd_double( "0410", sIn_mst_c.bal); /*账户余额*/
    }
    if (flag == 2)
    {
	    set_zd_data( "0270", sIn_mst_c.name); /*对方户名*/
	    set_zd_double( "0420", sIn_mst_c.bal); /*账户余额*/
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
