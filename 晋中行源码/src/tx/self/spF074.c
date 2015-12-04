/*************************************************
* 文 件 名:  spF074.c
* 功能描述： 代理业务销凭证处理(取暖销发票)
*
* 作    者:  wyw 
* 完成日期： 2008年4月14日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "note_mst_hst_c.h"
#include "note_parm_c.h"
 #define FP_VERSION "113080713003"
  #define FP_VERSION01 "113080813007"
int spF074()
{

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    int ret=0;
    long note_num=0;
    char note_version[20+1];
    memset(note_version,0,sizeof(note_version));
    get_zd_long("0620",&note_num);   /*凭证号码*/
    sprintf(g_pub_tx.beg_note_no,"%016d",note_num);
    get_zd_data("0630",note_version);  /*发票版本号*/
    get_zd_data("0890",g_pub_tx.note_type); /*凭证类型*/
    sprintf(acErrMsg, "凭证号码: [%s],凭证种类:[%s],发票版本号[%s]", g_pub_tx.beg_note_no,g_pub_tx.note_type,note_version);
	WRITEMSG
 /*多个版本号问题  liuyj 20081020*/
   /* if(memcmp(note_version,FP_VERSION,strlen(note_version)))
    {
	vtcp_log("%s,%d,发票版本号错误!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"N061");
	goto ErrExit;
    }*/
    
  if(memcmp(note_version,FP_VERSION,strlen(note_version)) && memcmp(note_version,FP_VERSION01,strlen(note_version)) )
    {
	vtcp_log("%s,%d,发票版本号错误!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"N061");
	goto ErrExit;
    }
    
 /*多个版本号问题  liuyj end*/
    ret = pub_com_NoteUse(0,0,g_pub_tx.note_type,g_pub_tx.beg_note_no,g_pub_tx.beg_note_no,g_pub_tx.tel);
    if(ret)
    {
	vtcp_log("%s,%d,销凭证号码失败!",__FILE__,__LINE__);
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"N061");
	}
	goto ErrExit;
    }
	vtcp_log("%s,%d,销出柜员凭证失败!",__FILE__,__LINE__);
OkExit:
    strcpy( g_pub_tx.reply, "0000" );
    sprintf(acErrMsg,"success!!");
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

