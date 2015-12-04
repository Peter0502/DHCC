/*************************************************
* 文 件 名:  spF073.c
* 功能描述： ATM卡号明细查询和分户查询
*
* 作    者:  rob
* 完成日期： 2003年2月22日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"
#include "dd_mst_hst_c.h"
#include "td_parm_c.h"

int spF073()
{

    int ret=0,i=0,j=0;
    char flag[2];
    double ky_amt=0.0;
    char s_txamt[17];
    char fd95[423];
    char mx_tmp[45];
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct dd_mst_c sDd_mst;
    struct dd_mst_hst_c sDd_mst_hst;
    struct td_mst_c sTd_mst;
    struct td_parm_c sTd_parm;
    
    memset(s_txamt,0,sizeof(s_txamt));
    memset(&sDd_mst_hst,0,sizeof(struct dd_mst_hst_c));
    memset(&sTd_mst,0,sizeof(struct td_mst_c));
    memset(&sTd_parm,0,sizeof(struct td_parm_c));
    memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
    memset(flag,0,sizeof(flag));
    /*通过71域来区分明细查询和多账户查询 1.明细查询,2.多账户查询*/
    memset(fd95,' ',sizeof(fd95));
    fd95[468]='\0';
    memset(mx_tmp,0,sizeof(mx_tmp));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }

    get_zd_data("0300",g_pub_tx.crd_no);   /* 卡号 */
    get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
    get_zd_data("0710",flag);              /*区分明细查询和多账户查询*/

    sprintf(acErrMsg, "卡号为: [%s]", g_pub_tx.crd_no);
	WRITEMSG
    /* 检查卡的合法性 */
    if (pub_base_CheckCrdNo())
    {
        sprintf(acErrMsg,"检查卡的合法性错误!!");
        WRITEMSG
		strcpy(g_pub_tx.reply, "AT01");
        goto ErrExit;
    }
    /* 检查卡的支取方式 */
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
       	sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"AT02");
       	goto ErrExit;
    }

    	/* 检查支取方式的合法性 */
     ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no, g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
    if(ret != 0)
    {
       	sprintf(acErrMsg,"卡的支取密码错误!!!");
       	WRITEMSG
       	strcpy(g_pub_tx.reply,"AT04");
       	goto ErrExit;
    }

    /* 查询活期帐户主文件 */
    strcpy(g_pub_tx.cur_no,"01");
    ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&g_dd_mst);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询活期帐户主文件错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT06");
        goto ErrExit;
    }
    /*begin add by LiuHuafeng 200701022 */
    ret=pubf_card_chk_msr(1,g_pub_tx.crd_no);
    if (ret != 0)
    {
        sprintf(acErrMsg,"查询卡户登记簿错误!!!");
        WRITEMSG
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
        strcpy(g_pub_tx.reply,"AT06");
        goto ErrExit;
    }
    /* end by LiuHuafeng 20070122 */
    ret = Dd_mst_hst_Dec_Sel(g_pub_tx.reply,"ac_id =%ld and ac_seqn=%ld order bytx_date,tx_time desc",g_dd_mst.ac_id,g_dd_mst.ac_seqn);    
    if(ret)
    {
	vtcp_log("%s,%d,查找明细出错！",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU14");
	goto ErrExit;
    }
    i=0;
    while(1)
    {
	vtcp_log("%s,%d,开始查询帐户明细记录!",__FILE__,__LINE__);
	if( i > 9)
	{
		vtcp_log("%s,%d,已经取完10条明细",__FILE__,__LINE__);
		break;		
	}
    	ret = Dd_mst_hst_Fet_Sel(&sDd_mst_hst,g_pub_tx.reply);
    	if(ret && ret ==100)
    	{
		vtcp_log("%s,%d,查找明细出错！",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
    	}else if(ret == 100)
	{
		vtcp_log("%s,%d,找到的记录数为[%d]",__FILE__,__LINE__,i);
		break;
	}
	memset(mx_tmp,0,sizeof(mx_tmp));	
	sprintf(mx_tmp,"%08d%-6.6s%016.0lf%016.0lf",sDd_mst_hst.tx_date,sDd_mst_hst.brf,sDd_mst_hst.tx_amt*100,sDd_mst_hst.bal*100);
	memcpy(fd95+i * 44,mx_tmp,44);
	i ++;
    }
    ret = Td_mst_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_dd_mst.ac_id);  
    if(ret)
    {
	vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
	strcpy(g_pub_tx.reply,"CU14");
	goto ErrExit;
    }
    j=0;
    while(1)
    {
	if(j > 3)
	{
		vtcp_log("%s,%d,可以查询4个定期户",__FILE__,__LINE__);
		break;
	}
	vtcp_log("%s,%d,开始查询定期帐户!",__FILE__,__LINE__);
	ret = Td_mst_Fet_Sel(&sTd_mst,g_pub_tx.reply);
    	if(ret && ret ==100)
    	{
		vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
    	}else{
		vtcp_log("%s,%d,定期户数为[%d]",__FILE__,__LINE__,j);
		break;
	}
	ret = Td_parm_Sel(&sTd_parm,g_pub_tx.reply);
	if(ret)
	{
		vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU14");
		goto ErrExit;
	}
	memset(mx_tmp,0,sizeof(mx_tmp));
	sprintf(mx_tmp,"%04d%-14s%016.0lf",sTd_mst.ac_seqn,sTd_parm.title,sTd_mst.bal*100);
	memcpy(fd95+440+ j*34,mx_tmp,34);
	j++;
    }
    set_zd_data("0950",fd95);
    vtcp_log("%s,%d,余额[%.2f],可用余额[%.2f]\n",__FILE__,__LINE__,g_dd_mst.bal,ky_amt);
    

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
