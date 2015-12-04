
/*************************************************
* 文 件 名: sp9702.c
* 功能描述: 根据帐号和账户序号返回相关信息___久悬交易用
*
* 作    者: mike
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"
#include "mo_unchange_c.h"
#include "mo_hold_c.h"

char fd125[256];   /*取冻结的余额*/
/****Modified by SSH,2006.9.13****/
/****对于销户状态,返回错误码的同时也把帐户信息返回****/
sp9702()  
{               
        int ret=0,ac_seqn;
        char title[40],cur_no[3];
        char tmp_type[2], hold_sts;
        char ac_no[24],name[61];
        double bal, hold_amt;

        struct prdt_ac_id_c g_prdt_ac_id;
        struct mo_unchange_c g_mo_unchange;
        struct mo_hold_c g_mo_hold;

        memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
        memset(&g_mo_unchange,0x00,sizeof(struct mo_unchange_c));

        g_mo_unchange.bal=0.00;
        strcpy(g_pub_tx.reply,"0000");

        /** 取值、赋值 **/      
        get_zd_data("0310",ac_no);                                      /*帐号*/
        get_zd_data("1250",fd125);                                      /*帐号*/
        get_zd_int("0340",&ac_seqn);                            /*帐户序号*/
        pub_base_old_acno( ac_no );                 /*对旧帐号的处理*/
vtcp_log("Jarod:ac_no=[%s],ac_seqn=[%d] [%s],[%d]",ac_no,ac_seqn,__FILE__,__LINE__);    
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
        if( ret==100 )
        {
                sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"M003");
                goto ErrExit;
        }
        else if( ret )
        {
                sprintf(acErrMsg,"查找出错[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"D103");
                goto ErrExit;
        }

        if( g_mdm_ac_rel.note_sts[0]=='3' )
        {
                sprintf(acErrMsg,"该介质已被换证!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"M022");
                goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='4' )
        {
                sprintf(acErrMsg,"该介质已部提换证!");
                WRITEMSG
                strcpy(g_pub_tx.reply,"D215");
                goto ErrExit;
        }
        else if( g_mdm_ac_rel.note_sts[0]=='*' )
        {
                sprintf(acErrMsg,"该介质已做销户处理![%s]",ac_no);
                WRITEMSG
                strcpy(g_pub_tx.reply,"M012");
                /****
                goto ErrExit;
                ****/
        }

        ret = Prdt_ac_id_Sel( g_pub_tx.reply,&g_prdt_ac_id, \
                                                        "ac_id=%ld and ac_seqn=%d", \
                                                         g_mdm_ac_rel.ac_id,ac_seqn );
        if( ret==100 )
        {
                sprintf(acErrMsg,"账号不存在ac_id=%ld,ac_seqn=%d", g_mdm_ac_rel.ac_id,ac_seqn);
                WRITEMSG
                strcpy(g_pub_tx.reply,"M003");
                goto ErrExit;
        }
        else if( ret )
        {
                sprintf(acErrMsg,"查找出错ret=[%d]",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"W012");
                goto ErrExit;
        }
        
        switch( g_prdt_ac_id.ac_id_type[0] )
        {
                case '1':                                                               /*1-活期*/
                                ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
                                                                "ac_id=%ld and ac_seqn=%d", \
                                                                g_mdm_ac_rel.ac_id,ac_seqn);
                                if( ret )
                                {
                                        sprintf(acErrMsg,"该账户不存在ac_id=[%ld],ac_seqn=[%d]",
                                                        g_mdm_ac_rel.ac_id,ac_seqn);
                                        WRITEMSG
                                        strcpy(g_pub_tx.reply,"M003");
                                        goto ErrExit;
                                }

                                strcpy(g_prdt_ac_id.prdt_no,g_dd_mst.prdt_no);

                                if(g_dd_mst.ac_sts[0]=='*')     
                                {
                                        sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
                                        WRITEMSG
                                        strcpy(g_pub_tx.reply,"P245");
                                        /****
                                        goto ErrExit;
                                        ****/
                                }

                                ret = Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'", \
                                                                        g_prdt_ac_id.prdt_no);
                                if( ret )
                                {
                                        sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
                                                        g_prdt_ac_id.prdt_no);
                                        WRITEMSG
                                        strcpy(g_pub_tx.reply,"P145");
                                        goto ErrExit;
                                }

                                bal=g_dd_mst.bal;
                                hold_amt=g_dd_mst.hold_amt;
                                strcpy(name,g_dd_mst.name);
                                strcpy(tmp_type,g_dd_mst.ac_type);
                                strcpy(title,g_dd_parm.title);
                                strcpy(cur_no,g_dd_parm.cur_no);
                                hold_sts=g_dd_mst.hold_sts[0];
                                break;
                case '2':                                                               /*2-定期*/
                                ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
                                                                "ac_id=%ld and ac_seqn=%d", \
                                                                g_mdm_ac_rel.ac_id,ac_seqn);
                                if( ret )
                                {
                                        sprintf(acErrMsg,"不存在该账户!!");
                                        WRITEMSG
                                        strcpy(g_pub_tx.reply,"M003");
                                        goto ErrExit;
                                }

                                strcpy(g_prdt_ac_id.prdt_no,g_td_mst.prdt_no);

                                if(g_td_mst.ac_sts[0]=='*')     
                                {
                                        sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
                                        WRITEMSG
                                        strcpy(g_pub_tx.reply,"P245");
                                        /****
                                        goto ErrExit;
                                        ****/
                                }

                                ret = Td_parm_Sel(g_pub_tx.reply,&g_td_parm,"prdt_no='%s'", \
                                                                  g_prdt_ac_id.prdt_no);
                                if( ret )
                                {
                                        sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",
                                                        g_prdt_ac_id.prdt_no);
                                        WRITEMSG
                                        strcpy(g_pub_tx.reply,"M003");
                                        goto ErrExit;
                                }

                                bal=g_td_mst.bal;
                                hold_amt=g_td_mst.hold_amt;
                                strcpy(name,g_td_mst.name);
                                strcpy(title,g_td_parm.title);
                                strcpy(cur_no,g_td_parm.cur_no);
                                hold_sts=g_td_mst.hold_sts[0];
                                break;
                default :
                                 break;
        }
                /*处理冻结部分,gujy JinZ 20070511*/
        if(0 == memcmp(fd125, "_DJ_", 4)){
                if(4 != strlen(fd125) && 0 != memcmp(fd125+4, "0000", 4)){
                        double sum_holdamt;             
                        if(ret = Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"ac_id=%ld and ac_seqn=%ld and hold_seqn = %.04s",g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn, fd125+4))

                        {
                                sprintf(acErrMsg,"不存在该帐号冻结记录,序号%.04s!!ac_no=[%s] ret = %d",fd125+4, ac_no, ret);
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"O016");
                                goto ErrExit;
                        }
                        set_zd_double("1003",g_mo_hold.plan_hold_amt);
                        set_zd_data("0690",g_mo_hold.hold_sts);

                        if(ret = sql_sum_double("mo_hold","plan_hold_amt",&sum_holdamt, "ac_id=%ld and ac_seqn=%ld and hold_seqn < %.04s and hold_sts = '0' ",g_mdm_ac_rel.ac_id, g_mdm_ac_rel.ac_seqn, fd125+4)){
                                sprintf(acErrMsg,"SUM数据库错误,序号%.04s!!ac_no=[%s] ret = %d",fd125+4, ac_no, ret);
                                WRITEMSG
                                strcpy(g_pub_tx.reply,"O016");
                                goto ErrExit;
                        }
                        vtcp_log("sum_holdamt=[%.2lf]\n", sum_holdamt);
                        set_zd_double("1004",sum_holdamt);
                }
                
        }

        /* 输出变量 */
        set_zd_data("0200",g_prdt_ac_id.ac_id_type);
        set_zd_data("0210",cur_no);
        set_zd_data("0230",g_prdt_ac_id.prdt_no);
        vtcp_log("XXXXXXXXXXXXXXXXX[%s]",name);
        set_zd_data("0310",ac_no);
        set_zd_data("0250",name);
        set_zd_data("0260",title);
        set_zd_data("0670",tmp_type);
        set_zd_data("0680",&hold_sts);
        set_zd_double("1001",bal);
        set_zd_double("1002",hold_amt);
        /****Add by SSH,2006.9.13,对于销户的帐户,试图从久悬登记簿找余额****/  /*add by lwb 金额为0时也检查*/
        if(memcmp(g_pub_tx.reply,"P245",4)==0||
                memcmp(g_pub_tx.reply,"M012",4)==0||bal == 0.0){
                ret = Mo_unchange_Sel( g_pub_tx.reply,&g_mo_unchange, 
                                        "ac_id=%ld and ac_seqn=%d and ind='N'",g_mdm_ac_rel.ac_id,ac_seqn );
                if(ret&&bal!=0.0){
                        sprintf(acErrMsg,"不存在该账户!!");
                        WRITEMSG
                        strcpy(g_pub_tx.reply,"M003");
                        goto ErrExit;
                }
		strcpy(g_pub_tx.reply,"0000");
        }
        set_zd_double("0410",g_mo_unchange.bal);
	set_zd_data("0320",g_mo_unchange.in_acno);
        if(memcmp(g_pub_tx.reply,"0000",4)!=0){
                goto ErrExit;
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
