/*************************************************************
* 文 件 名:  sp9633.c
* 功能描述： 冻结登记信息查询
*
*************************************************************/

#define EXTERN
#include "public.h"
#include "mo_hold_c.h"
#include "mdm_ac_rel_c.h"
int sp9633()
{
	struct mo_hold_c   g_mo_hold;
        struct mdm_ac_rel_c  g_mdm_ac_rel;
        int ret=0;
        long  hold_seqn;
        memset(&g_mo_hold,0x00,sizeof(struct mo_hold_c));
        memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	pub_base_sysinit();

       get_zd_long("0350",&hold_seqn);
       get_zd_data("0310",g_pub_tx.ac_no);
         vtcp_log("%s %d oooooooooooo ===[%ld] ",__FILE__,__LINE__,hold_seqn);
        ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
               if(ret==100){
                   sprintf(acErrMsg,"数据库无此记录");
                   WRITEMSG
                   goto ErrExit;
                }
                if(ret!=0){
                    sprintf(acErrMsg,"查询介质关系对照表错误");
                    WRITEMSG
                    goto ErrExit;
                }
            g_mo_hold.ac_id=g_mdm_ac_rel.ac_id;
             vtcp_log("%s %d xxxxxxxxxxxx ===[%ld] ",__FILE__,__LINE__,g_mdm_ac_rel.ac_id);
        ret=Mo_hold_Sel(g_pub_tx.reply,&g_mo_hold,"hold_seqn='%ld'  and ac_id='%ld'",hold_seqn,g_mo_hold.ac_id);
               if(ret==100){
                      sprintf(acErrMsg,"数据库无此记录");
                      WRITEMSG
                      goto ErrExit;
                      }
                if(ret!=0){
                      sprintf(acErrMsg,"查询冻结登记簿出错");
                      WRITEMSG
                      goto ErrExit;
                      }
             vtcp_log("%s %d shishislhislhishi==[%.2f] ",__FILE__,__LINE__,g_mo_hold.plan_hold_amt);
           vtcp_log("%s %d shishislhislhishi==[%.2f] ",__FILE__,__LINE__,g_mo_hold.plan_hold_amt);
            set_zd_double("0410",g_mo_hold.plan_hold_amt);
            set_zd_data("0660",g_mo_hold.hold_typ);

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
