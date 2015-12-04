/*************************************************
* 文 件 名: sp9603.c
* 功能描述：删除操作员
*
* 作    者: rob
* 完成日期：20030310
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "cash_mst_c.h"
#include "note_mst_c.h"

int sp9603( )
{
    int ret;
    char flag[2];
    struct com_tel_c com_tel_c;
    struct cash_mst_c cash_mst_c;
    struct note_mst_c note_mst_c;

    memset(&com_tel_c,0x0,sizeof(struct com_tel_c));
    memset(&cash_mst_c,0x0,sizeof(struct cash_mst_c));
    memset(&note_mst_c,0x0,sizeof(struct note_mst_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /******
    get_zd_data("0920",ctel);
    get_zd_data("0910",cbr_no);
    *******/
    get_zd_data("0920",com_tel_c.tel);
    get_zd_data("0910",com_tel_c.br_no);
    get_zd_data("0250",com_tel_c.name);
    get_zd_data("0790",com_tel_c.pwd);
    get_zd_data("0860",com_tel_c.lvl);
    get_zd_data("0640",com_tel_c.beg_menu);
    get_zd_data("0670",flag);

    strcpy(com_tel_c.tx_type,flag);

    pub_base_strpack(com_tel_c.br_no);
    pub_base_strpack(com_tel_c.tel);

    /* 检查柜员号是否存在 */
    ret = Com_tel_Sel(g_pub_tx.reply,&com_tel_c,"tel='%s'",com_tel_c.tel);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询柜员信息表错误!!");
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"柜员号不存在!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O120");
        goto ErrExit;
    }

    /* 检查钱箱是否有钱 */
    strcpy(cash_mst_c.tel,com_tel_c.tel);
    
    ret = Cash_mst_Dec_Upd(g_pub_tx.reply,"tel='%s'", cash_mst_c.tel);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    while(1)
    {
        ret = Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            break;
        }

        if (pub_base_CompDblVal(cash_mst_c.bal,0.00) != 0)
        {
            sprintf(acErrMsg,"该柜员有钱箱余额不为零!! [%lf]",cash_mst_c.bal);
            WRITEMSG
            strcpy(g_pub_tx.reply,"O121");
            goto ErrExit;
        }
        
        strcpy(cash_mst_c.sts,"1");

        ret = Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
        if (ret != 0)
        {
            sprintf(acErrMsg,"UPDATE ERROR !!! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
       
    }
    Cash_mst_Clo_Upd();
    
	/* 检查该柜员的凭证 */
    strcpy(note_mst_c.tel,com_tel_c.tel);
    ret = Note_mst_Dec_Sel(g_pub_tx.reply,"tel='%s'",note_mst_c.tel);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    
    while(1)
    {
        ret = Note_mst_Fet_Sel(&note_mst_c,g_pub_tx.reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            goto ErrExit;
        }
        else if (ret == 100)
        {
            break;
        }

        if (note_mst_c.cnt != 0)
        {
            sprintf(acErrMsg,"该柜员还有多余的凭证!! [%d]",note_mst_c.cnt);
            WRITEMSG
            strcpy(g_pub_tx.reply,"O122");
            goto ErrExit;
        }
    }
    Note_mst_Clo_Sel();

    /* 修改柜员信息表 */
  
    ret = Com_tel_Dec_Upd(g_pub_tx.reply,"tel='%s'",com_tel_c.tel);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Com_tel_Fet_Upd(&com_tel_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
    
    strcpy(com_tel_c.csts, "4"); /* 删除 */

    ret = Com_tel_Upd_Upd(com_tel_c,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改柜员信息表错误!!!");
        WRITEMSG
        goto ErrExit;
    }
    Com_tel_Clo_Upd();
 
       
    strcpy(g_pub_tx.ac_no,com_tel_c.tel);
    strcpy(g_pub_tx.brf,"删除柜员");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"操作员删除成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"操作员删除失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
