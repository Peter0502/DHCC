/*************************************************
* 文 件 名:  spB701.c
* 功能描述： 证券回购信息维护
*
* 作    者:  
* 完成日期： 2003年1月29日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mo_stok_rebuy_prot_c.h"

int spB701()
{
    struct mo_stok_rebuy_prot_c sMo_stok_rebuy_prot,sMo_stok_rebuy_prot1;   
    char flag[2];
    char cashflag[2];
    int ret;
  
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
	memset(&g_td_parm,0x00,sizeof(struct td_parm_c));
	memset(&g_ln_mst,0x00,sizeof(struct ln_mst_c));
	memset(&g_ln_parm,0x00,sizeof(struct ln_parm_c));
	memset(&g_in_mst,0x00,sizeof(struct in_mst_c));
	memset(&g_in_parm,0x00,sizeof(struct in_parm_c));
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));
    memset(&sMo_stok_rebuy_prot,0x0,sizeof(struct mo_stok_rebuy_prot_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    get_zd_data("0670",flag); /* 操作标志 0 录入 1 修改 2 删除 */
    get_zd_data("0860",sMo_stok_rebuy_prot.pact_no); /* 证券回购协议编号 */
    get_zd_data("0700",sMo_stok_rebuy_prot.tcht_type); /* 证券类型 */
    get_zd_double("0390",&sMo_stok_rebuy_prot.par_value);/* 证券面值 */
    get_zd_double("0940",&sMo_stok_rebuy_prot.pledge_rate);/* 抵押率 */ 
    get_zd_data("0680",sMo_stok_rebuy_prot.buy_type); /* 借出借入标志 */
    get_zd_double("0400",&sMo_stok_rebuy_prot.bal); /* 借出借入金额 */
    get_zd_long("0440",&sMo_stok_rebuy_prot.matr_date); /* 协议到期日期 */
    get_zd_long("0480",&sMo_stok_rebuy_prot.term); /* 证券回购期限 */
    get_zd_double("0850",&sMo_stok_rebuy_prot.rate); /* 收益率 */
    get_zd_data("0690",cashflag); /* 现转标志 */
    get_zd_data("0270",sMo_stok_rebuy_prot.opt_name); /* 对方机构名 */

	strcpy(sMo_stok_rebuy_prot.opt_acno,cashflag);
    strcpy(sMo_stok_rebuy_prot.br_no,g_pub_tx.tx_br_no);
    sMo_stok_rebuy_prot.sign_date = g_pub_tx.tx_date;
    sMo_stok_rebuy_prot.reg_date = g_pub_tx.tx_date;
    sMo_stok_rebuy_prot.trace_no = g_pub_tx.trace_no;
    strcpy(sMo_stok_rebuy_prot.sts,"1");
    strcpy(sMo_stok_rebuy_prot.tel,g_pub_tx.tel);
    strcpy(sMo_stok_rebuy_prot.chk,g_pub_tx.chk);
    strcpy(sMo_stok_rebuy_prot.auth,g_pub_tx.auth);

    switch(atoi(flag))
    {
        case 0:
            sprintf(acErrMsg,"录入!!!!");
            WRITEMSG
           
            /* 检查该证券回购协议编号是否有重复 */
            ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
                  "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
            if (ret == 0)
            {
                sprintf(acErrMsg,"查询证券回购协议登记簿已存在记录 [%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"B087");
                goto ErrExit;
            }
            else if (ret != 100)
            {
                sprintf(acErrMsg,"查询证券回购协议登记簿错误!!");
                WRITEMSG
                goto ErrExit;
            }

            sprintf(acErrMsg,"AAAAAAAAAA");
            WRITEMSG
          
            ret = Mo_stok_rebuy_prot_Ins(sMo_stok_rebuy_prot,g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"向登记簿中插入记录错误!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }

            break;

        case 1 :
            /* 修改 */
            
            sprintf(acErrMsg,"修改!! ");
            WRITEMSG

            ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
                  "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"查询证券回购协议登记簿错误!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
  
            if (sMo_stok_rebuy_prot1.sts[0] != '1')
            {
                sprintf(acErrMsg,"非录入状态不允许修改!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B009");
                goto ErrExit;
            }

            ret = Mo_stok_rebuy_prot_Dec_Upd(g_pub_tx.reply,"pact_no='%s'",
                  sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Dec for update error  [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }

            ret = Mo_stok_rebuy_prot_Fet_Upd(&sMo_stok_rebuy_prot1,
                  g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Fetch error [%d]",ret);
                WRITEMSG
                Mo_stok_rebuy_prot_Clo_Upd();
                goto ErrExit;
            }

            ret = Mo_stok_rebuy_prot_Upd_Upd(sMo_stok_rebuy_prot,
                  g_pub_tx.reply);
            if (ret != 0)
            {
                sprintf(acErrMsg,"Update error [%d]",ret);
                WRITEMSG
                Mo_stok_rebuy_prot_Clo_Upd();
                goto ErrExit;
            }

            Mo_stok_rebuy_prot_Clo_Upd();
            sprintf(acErrMsg,"修改证券回购协议登记簿  pass!! ");
            WRITEMSG
            
            break;

        case 2 :
            /* 删除 */
            sprintf(acErrMsg,"删除!! ");
            WRITEMSG

            ret = Mo_stok_rebuy_prot_Sel(g_pub_tx.reply,&sMo_stok_rebuy_prot1,
                  "pact_no='%s'",sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"查询证券回购协议登记簿错误!! [%d]",ret);
                WRITEMSG
                goto ErrExit;
            }
  
            if (sMo_stok_rebuy_prot1.sts[0] != '1')
            {
                sprintf(acErrMsg,"非录入状态不允许删除!!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"B010");
                goto ErrExit;
            }

            ret = sql_execute("delete from mo_stok_rebuy_prot where \
                  pact_no='%s' and sts ='1'",sMo_stok_rebuy_prot.pact_no);
            if (ret != 0)
            {
                sprintf(acErrMsg,"删除证券回购协议登记簿出错!! [%d] ",ret);
                WRITEMSG
                goto ErrExit;
            }
            strcpy(g_pub_tx.brf,"证券回购信息删除");
            sprintf(acErrMsg,"删除证券回购协议登记簿中记录 pass!! ");
            WRITEMSG

            break;
        default:
            goto ErrExit;
    }
 
    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!!");
        WRITEMSG
        goto ErrExit;
    }

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
