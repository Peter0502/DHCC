/*********************************************************************
* 版 本 号 :  V1.0
* 程序作者 :  商永超
* 日    期 :  2011-09-15
* 所属模块 :  扣划时，根据账号和序号显示详细账户信息(活期和定期)
* 程序名称 :  sp1176.c
* 程序作用 :
* 函数列表 :
* 使用注意 :
* 修改记录 :
*     修改作者 :slzh
*     修改时间 :20111101
*     修改内容 :删除dd_mst,td_mst中的name字段
*     修改作者 :slzh
*     修改时间 :20111111
*     修改内容 :增加返回 结算类型, ac_id_type , 最小余额 

交易配置：
insert into tx_def values ('1176', '账号查询', '10000000000000000001111010000001000000101110000000010000000001000011111110000000100000010011100101100100000000000000000100000100', '0', '4', '2', '3', null);
insert into tx_flow_def values ('1176', 0, '1176', '#$');
insert into tx_sub_def values ('1176', '账号查询', 'sp1176', '0', '0');
***********************************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"
int sp1176()
{
        int     ret = 0;
        int     iAc_seqn = 0;
        struct prdt_ac_id_c sPrdt_ac_id;
        struct mdm_attr_c sMdm_attr;
        struct dd_mst_c sDd_mst;
        struct td_mst_c sTd_mst;
        struct mdm_ac_rel_c sMdm_ac_rel;
        struct dd_parm_c sDd_parm;
        struct td_parm_c sTd_parm;
        memset(&sDd_mst, 0x0, sizeof(struct dd_mst_c));
        memset(&sTd_mst, 0x0, sizeof(struct td_mst_c));
        memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
        memset(&sPrdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c));
        memset(&sMdm_attr, 0x00, sizeof(struct mdm_attr_c));
        memset(&sDd_parm, 0x00, sizeof(struct dd_parm_c));
        memset(&sTd_parm, 0x00, sizeof(struct td_parm_c));
        /*初始化*/
        ret=pub_base_sysinit();
        if(ret)
        {
             sprintf(acErrMsg,"初始化公用结构错误!!");
             WRITEMSG
             goto ErrExit;
        }
        get_zd_data("1021", sMdm_ac_rel.ac_no);
        get_zd_int("1022", &iAc_seqn);
        pub_base_old_acno(sMdm_ac_rel.ac_no);
        set_zd_data("1021",sMdm_ac_rel.ac_no);
        set_zd_int("1022",iAc_seqn);
        ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel,"ac_no='%s'",sMdm_ac_rel.ac_no);
        if (ret != 0 && ret != 100)
        {
             sprintf(acErrMsg, "查询mdm_ac_rel错误!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W011");
             goto ErrExit;
        }
        else if(ret == 100)
        {
             sprintf(acErrMsg, "无此账号!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "W010");
             goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] == '*')
        {
            sprintf(acErrMsg,"该账户已经销户");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M012");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] == '2')
        {
            sprintf(acErrMsg,"该账户已挂失换证");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M010");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] =='3')
        {
            sprintf(acErrMsg,"该账户已经正常换证");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M011");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] =='5')
        {
            sprintf(acErrMsg,"该账户已经撤销");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M019");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] =='7')
        {
            sprintf(acErrMsg,"该账户未激活");
            WRITEMSG
            strcpy(g_pub_tx.reply,"M013");
            goto ErrExit;
        }
        if(strlen(g_pub_tx.ac_no)< 2 )
	{
	    memcpy(g_pub_tx.ac_no ,sMdm_ac_rel.ac_no , sizeof(g_pub_tx.ac_no)-1);
	}
#if 0
	vtcp_log("[%s][%d] chnl_type=[%s]",__FILE__,__LINE__,g_pub_tx.chnl_type);
	/*组规则表*/
	ret=pub_set_rule();
	if(ret)
	{
	    sprintf( acErrMsg,"规则结构体赋值失败！[%s]",g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply, "V020");
	    goto ErrExit;
	}
	    vtcp_log("[%s][%d] g_pub_rule.ac_no=[%s]",__FILE__,__LINE__,g_pub_rule.ac_no);
	/*规则校验*/
	ret=pub_pwd_rule();
	if(ret)
	{
	    sprintf( acErrMsg,"密码取值失败!请检查规则表！[%s]",g_pub_tx.reply);
	    WRITEMSG
	    strcpy(g_pub_tx.reply, "V021");
	    goto ErrExit;
	}
	/*vtcp_log("[%s][%d ] g_pub_tx.reply=[%s]，密码返回值[%s]",__FILE__,__LINE__,g_pub_tx.reply,g_pub_rule.pwd);*/
	/*vtcp_log("[%s][%d ] g_pub_tx.reply=[%s]，密码代码返回值[%s]",__FILE__,__LINE__,g_pub_tx.reply,g_pub_rule.pwd_code);*/
	/*end by gongll 2011/10/27 15:50:33 JZSH*/
#endif
        set_zd_data("1024",sMdm_ac_rel.note_no);        /*凭证号*/
        set_zd_data("1025",sMdm_ac_rel.draw_uncon_yn);  /*是否任意支取*/
        set_zd_data("1026",sMdm_ac_rel.draw_pwd_yn);    /*是否凭密*/
        set_zd_data("1027",sMdm_ac_rel.qry_pwd);        /*查询密码*/
        set_zd_data("1028",sMdm_ac_rel.draw_pwd);       /*支取密码*/
	set_zd_data("1029",sMdm_ac_rel.draw_id_yn);     /*是否凭证件支取*/
        set_zd_data("102A",sMdm_ac_rel.id_no);          /*证件号码*/
        set_zd_data("102B",sMdm_ac_rel.draw_seal_yn);   /*是否凭印鉴支取*/
        set_zd_data("102C",sMdm_ac_rel.pwd_mach_yn);    /*是否支付密码器*/
        set_zd_data("102G",sMdm_ac_rel.id_type);        /*证件类型*/
        set_zd_data("0710",sMdm_ac_rel.note_sts);       /*帐户状态*//****增加返回 20120522****/
        
        ret = Mdm_attr_Sel(g_pub_tx.reply, &sMdm_attr,"mdm_code='%s'",sMdm_ac_rel.mdm_code);
        if (ret)
        {
             sprintf(acErrMsg, "查询mdm_ac_rel错误!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "D103");
             goto ErrExit;
        }
        set_zd_data("1023",sMdm_attr.note_type); /*凭证种类*/
        
        ret = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
        if (ret)
        {
             sprintf(acErrMsg, "查询prdt_ac_rel错误!! [%d]", ret);
             WRITEMSG
             strcpy(g_pub_tx.reply, "D103");
             goto ErrExit;
        }
         set_zd_data("0690",sPrdt_ac_id.ac_id_type);
        if(sPrdt_ac_id.ac_id_type[0] == '1')/*活期*/
        {
            ret = Dd_mst_Sel(g_pub_tx.reply, &sDd_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
            if (ret)
            {
                sprintf(acErrMsg, "查询dd_mst错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
            }
            set_zd_double("102I",sDd_mst.bal); /*余额*/
            set_zd_data("102H",sDd_mst.name);  /*户名*/
            
            ret = Dd_parm_Sel(g_pub_tx.reply, &sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
            if (ret)
            {
                sprintf(acErrMsg, "查询dd_parm错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
            }
            set_zd_data("0700",sDd_mst.sttl_type); /*结算类型*/
            set_zd_double("102F", sDd_parm.min_bal); /*最低余额*/
            set_zd_data("102J",sDd_parm.cur_no);  /*币种*/
            set_zd_data("102V",sDd_parm.title);   /*产品名称*/
            set_zd_data("102M",sDd_parm.cif_type);/*账户类型*/
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '2')/*定期*/
        {
            ret = Td_mst_Sel(g_pub_tx.reply, &sTd_mst,"ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id,iAc_seqn);
            if (ret)
            {
                sprintf(acErrMsg, "查询td_mst错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
            }
            
            set_zd_double("102I",sTd_mst.bal);/*余额*/
            set_zd_data("102H",sTd_mst.name);/*户名*/ 

            ret = Td_parm_Sel(g_pub_tx.reply, &sTd_parm,"prdt_no='%s'",sTd_mst.prdt_no);
            if (ret)
            {
                sprintf(acErrMsg, "查询td_parm错误!! [%d]", ret);
                WRITEMSG
                strcpy(g_pub_tx.reply, "D103");
                goto ErrExit;
            }
            set_zd_double("102F", sTd_parm.min_bal); /*最低余额*/
            set_zd_data("102J",sTd_parm.cur_no);  /*币种*/
            set_zd_data("102V",sTd_parm.title);   /*产品名称*/ 
            set_zd_data("102M",sTd_parm.cif_type);/*账户类型*/
        }
        else
        {
            sprintf(acErrMsg, "账户非定期或 活期 !! [%c]",sPrdt_ac_id.ac_id_type[0] );
            WRITEMSG
            strcpy(g_pub_tx.reply, "D103");
            goto ErrExit;
        }   
        
OkExit:
        strcpy(g_pub_tx.reply, "0000");
        sprintf(acErrMsg, "success !!: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 0;
ErrExit:
        sprintf(acErrMsg, "fail !!: reply is[%s]", g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY, g_pub_tx.reply);
        return 1;
}

