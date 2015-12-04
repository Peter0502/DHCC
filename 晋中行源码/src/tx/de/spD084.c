/***************************************************************
* 文 件 名: spD084.c
* 功能描述：用于维护理财产品签约账号
            精确到账户因为冻结是账户级的
* 作    者: shangyongchao
* 完成日期：2013/5/20 15:54:58
*
* 修改记录:
* 日    期:
* 修 改 人:
* 备    注:
*******************************************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "fn_parm_c.h"
#include "fn_reg_c.h"
#include "fn_reg_hst_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "dd_parm_c.h"
#include "mdm_attr_c.h"
int spD084()
{
    int iRet = 0;
    int iCnt  = 0;/*计数用*/
    int iLen = 0;
    int iAc_seqn_new = 0;
    int iAc_seqn_old =0;
    char cAc_no_old[25];/*原账号*/
    char cAc_no_new[25];/*新账号*/
    char cPrdt_code[21];/*产品代码*/
    char cAgent_id_type[2];/*代理人证件类型*/
    char cAgent_id[21];/*代理人证件号码*/
    char cAgent_name[61];/*代理人姓名*/
    char cTx_type[2];/*0只更换传进来的产品代码对应的账号，1该账号的都更换*/
    char cAc_no_old_tmp[5];
    struct mdm_ac_rel_c sMdm_ac_rel;
    struct mdm_ac_rel_c sMdm_ac_rel_old;
    struct dd_mst_c sDd_mst;
    struct fn_reg_hst_c sFn_reg_hst;
    struct fn_reg_c sFn_reg;
    struct fn_parm_c sFn_parm;
    
    memset(cAc_no_old_tmp,0x00,sizeof(cAc_no_old_tmp));
    memset(cAc_no_old,0x00,sizeof(cAc_no_old));
    memset(cAc_no_new,0x00,sizeof(cAc_no_new));
    memset(cPrdt_code,0x00,sizeof(cPrdt_code));
    memset(cAgent_id_type,0x00,sizeof(cAgent_id_type));
    memset(cAgent_id,0x00,sizeof(cAgent_id));
    memset(cAgent_name,0x00,sizeof(cAgent_name));
    memset(cTx_type,0x00,sizeof(cTx_type));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sMdm_ac_rel_old,0x00,sizeof(sMdm_ac_rel_old));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sFn_reg,0x00,sizeof(sFn_reg));
    memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
    memset(&sFn_parm,0x00,sizeof(sFn_parm));

    pub_base_sysinit();

    get_zd_data("0300",cAc_no_old);
    pub_base_old_acno(cAc_no_old);
    get_zd_data("0310",cAc_no_new);
    pub_base_old_acno(cAc_no_new);
    get_zd_data("0630",cPrdt_code);
    get_zd_data("0660",cTx_type);
    get_zd_int("0480",&iAc_seqn_old);
    get_zd_int("0490",&iAc_seqn_new);
    get_zd_data("0690",cAgent_id_type);
    get_zd_data("0620",cAgent_id);
    get_zd_data("0250",cAgent_name);
    iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no_new);
    if( iRet==100 )
    {
        vtcp_log("该账号不存在,请检查! ");
        strcpy(g_pub_tx.reply,"C115");
        goto ErrExit;
    }
    else if( iRet )
    {
        vtcp_log(" 程序定义游标出错,请与中心机房联系 ",iRet);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    if(sMdm_ac_rel.note_sts[0] != '0')
    {
        vtcp_log("新账号的介质状态不正常! note_sts[%s] ",sMdm_ac_rel.note_sts);
        strcpy(g_pub_tx.reply,"D140");
        goto ErrExit;
    }
    iRet=Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld and ac_seqn = %ld",sMdm_ac_rel.ac_id,iAc_seqn_new);
    if( iRet==100 )
    {
        vtcp_log(" fatal error! table no record ");
        strcpy(g_pub_tx.reply,"D104");
        goto ErrExit;
    }
    else if( iRet )
    {
        vtcp_log("database error! ret[%d] ",iRet);
        strcpy(g_pub_tx.reply,"D101");
        goto ErrExit;
    }
    if(sDd_mst.ac_sts[0] != '1')
    {
        vtcp_log(" 新账号的账户状态不正常! ac_sts[%s] ",sDd_mst.ac_sts);
        strcpy(g_pub_tx.reply,"O085");
        goto ErrExit;
    }
    if(sDd_mst.hold_sts[0] == '1' )
            
    {
        vtcp_log("该账户被全部冻结了 [%s] ",sDd_mst.hold_sts);
        strcpy(g_pub_tx.reply,"KH01");
        goto ErrExit;
    }
    /*查询下旧账号*/
    iRet=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel_old,"ac_no='%s'",cAc_no_old);
    if( iRet==100 )
    {
        vtcp_log("fatal error! table no record ");
        strcpy(g_pub_tx.reply,"P088");
        goto ErrExit;
    }
    else if( iRet )
    {
        vtcp_log(" database error! ret[%d] ",iRet);
        strcpy(g_pub_tx.reply,"P084");
        goto ErrExit;
    }
    if(strcmp(sMdm_ac_rel_old.id_no,sMdm_ac_rel.id_no)) 
    {  vtcp_log(" 更换换理财账户非同一人所开账号 ");
    	 vtcp_log("old.id_no= [%s] new.id_no=[%s]  ",sMdm_ac_rel_old.id_no,sMdm_ac_rel.id_no);
        strcpy(g_pub_tx.reply,"S093");
        goto ErrExit;
    	}
    /*根据不同的操作标志 建游标*/
    if(cTx_type[0] == '0')/*只更换一个产品的收款账号*/
    {
        iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",cPrdt_code);
        if(iRet == 100)
        {
            vtcp_log("没有这个理财产品! prdt_code[%s] ",cPrdt_code);
            strcpy(g_pub_tx.reply,"D104");
            goto ErrExit;
        }
        else if(iRet)
        {
            vtcp_log("查询理财产品参数表错误! prdt_code[%s] iRet [%d] ",cPrdt_code,iRet);
            strcpy(g_pub_tx.reply,"D324");
            goto ErrExit;
        }
        /*币种不同的不能更换*/
        /*if(strcmp(sDd_mst.cur_no,sFn_parm.cur_no))
        {
            vtcp_log("不能更换为其他币种的账户parm.cur_no[%s] dd.cur_no[%s]",sFn_parm.cur_no,sDd_mst.cur_no);
            strcpy(g_pub_tx.reply,"D337");
            goto ErrExit;
        }*/
        /*只更换认购、已扣款(等着分配收益)、申购的*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"prdt_code ='%s' and ac_id=%ld and ac_seqn=%d and sts in('0','1','4')",cPrdt_code,sMdm_ac_rel_old.ac_id,iAc_seqn_old);
        if(iRet)
        {
            vtcp_log("建立更新游标错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333");
            goto ErrExit;
        }
        while(1)
        {
            memset(&sFn_reg,0x00,sizeof(sFn_reg));
            memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            if(iRet != 0 && iRet != 100)
            {
                vtcp_log(" 取更新游标错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            else if(iRet == 100)
            {
                if(iCnt  == 0)
                {
                    vtcp_log("这个账户没有认购该理财产品 iRet[%d] ",iRet);
                    strcpy(g_pub_tx.reply,"D104");
                    goto ErrExit;
                }
                break;
            }
            strcpy(sFn_reg.filler,"");
            sprintf(sFn_reg.filler,"%s|%d",sFn_reg.ac_no,sFn_reg.ac_seqn);
            strcpy(sFn_reg.ac_no,cAc_no_new);
            sFn_reg.ac_id  = sDd_mst.ac_id;
            sFn_reg.ac_seqn  = sDd_mst.ac_seqn;
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新fn_reg错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D332");
                goto ErrExit;
            }
            /*如果是认购的，必须将新账号金额控制住，同时把原账号控制金额解除 为什么不做撤销呢*/
            if(sFn_reg.sts[0] == '0')
            {
                /*更新认购账户的控制金额*/
                iRet  = upd_dd_mst(0,sMdm_ac_rel_old.ac_id,iAc_seqn_old,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("更新活期存款主文件错误 iRet[%d] ",iRet);
                    goto ErrExit;
                }
                /*更新替换账号的控制金额*/
                iRet  = upd_dd_mst(1,sFn_reg.ac_id,sFn_reg.ac_seqn,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("更新活期存款主文件错误 iRet[%d] ",iRet);
                    goto ErrExit;
                }
            }
            iCnt ++;
            /*登记明细*/
            sFn_reg_hst.tx_date = g_pub_tx.tx_date;
            sFn_reg_hst.trace_no = g_pub_tx.trace_no;
            strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
            strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
            strcpy(sFn_reg_hst.type,"7");
            sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
            strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
            strcpy(sFn_reg_hst.agent_id,cAgent_id);
            strcpy(sFn_reg_hst.agent_name,cAgent_name);
            sFn_reg_hst.buy_date=sFn_reg.tx_date;
            sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
            strcpy(sFn_reg_hst.brf,"更改签约账号");
            sprintf(sFn_reg_hst.filler,"%s|%d|%s|%d",cAc_no_old,iAc_seqn_old,cAc_no_new,iAc_seqn_new);
            iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
            if(iRet)
            {
                Fn_reg_hst_Debug(&sFn_reg_hst);
                vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }

       }
       Fn_reg_Clo_Upd();
    }
    else if(cTx_type[0] == '1')
    {
        /*这个账号 认购的 已经扣款等分配的 申购等确认的*/
        iRet = Fn_reg_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn =%d and sts in('0','1','4') order by prdt_code",sMdm_ac_rel_old.ac_id,iAc_seqn_old);
        if(iRet)
        {
            vtcp_log("建立更新游标错误! iRet[%d] ",iRet);
            strcpy(g_pub_tx.reply,"D333");
            goto ErrExit;
        }
        while(1)
        {
            memset(&sFn_reg,0x00,sizeof(sFn_reg));
            memset(&sFn_reg_hst,0x00,sizeof(sFn_reg_hst));
            memset(&sFn_parm,0x00,sizeof(sFn_parm));
            iRet = Fn_reg_Fet_Upd(&sFn_reg,g_pub_tx.reply);
            if(iRet != 0 && iRet != 100)
            {
                vtcp_log("取更新游标错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            else if(iRet == 100)
            {
                if(iCnt  == 0)
                {
                    vtcp_log("这个账户没有认购任何理财产品 iRet[%d] ",iRet);
                    strcpy(g_pub_tx.reply,"D104");
                    goto ErrExit;
                }
                break;
            }
            iRet = Fn_parm_Sel(g_pub_tx.reply,&sFn_parm,"prdt_code='%s'",sFn_reg.prdt_code);
            if(iRet)
            {
                vtcp_log("没有这个理财产品! prdt_code[%s] iRet[%d]",sFn_reg.prdt_code,iRet);
                strcpy(g_pub_tx.reply,"D326");
                goto ErrExit;
            }
            /*币种不同的不能更换*/
            /*if(strcmp(sDd_mst.cur_no,sFn_parm.cur_no))
            {                 
                vtcp_log("不能更换为其他币种的账户parm.cur_no[%s] dd.cur_no[%s]",sFn_parm.cur_no,sDd_mst.cur_no);
                strcpy(g_pub_tx.reply,"D337");
                goto ErrExit;
            }*/
            strcpy(sFn_reg.filler,"");
            sprintf(sFn_reg.filler,"%s|%d",sFn_reg.ac_no,sFn_reg.ac_seqn);
            strcpy(sFn_reg.ac_no,cAc_no_new);
            sFn_reg.ac_id  = sDd_mst.ac_id;
            sFn_reg.ac_seqn  = sDd_mst.ac_seqn;
            iRet = Fn_reg_Upd_Upd(sFn_reg,g_pub_tx.reply);
            if(iRet)
            {
                vtcp_log("更新fn_reg错误! iRet[%d] ",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
            /*如果是认购的，必须将新账号金额控制住，同时把原账号控制金额解除 为什么不做撤销呢*/
            if(sFn_reg.sts[0] == '0')
            {
                /*更新认购账户的控制金额*/
                iRet  = upd_dd_mst(0,sMdm_ac_rel_old.ac_id,iAc_seqn_old,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("更新活期存款主文件错误 iRet[%d] ",iRet);
                    goto ErrExit;
                }
                /*更新替换账号的控制金额*/
                iRet  = upd_dd_mst(1,sFn_reg.ac_id,sFn_reg.ac_seqn,sFn_reg.tx_amt);
                if(iRet)
                {
                    vtcp_log("更新活期存款主文件错误 iRet[%d] ",iRet);
                    goto ErrExit;
                }
            }
            iCnt ++;
            sFn_reg_hst.tx_date = g_pub_tx.tx_date;
            sFn_reg_hst.trace_no = g_pub_tx.trace_no;
            strcpy(sFn_reg_hst.tx_br_no,g_pub_tx.tx_br_no);
            strcpy(sFn_reg_hst.tel,g_pub_tx.tel);
            strcpy(sFn_reg_hst.type,"7");
            sFn_reg_hst.tx_amt = sFn_reg.tx_amt;
            strcpy(sFn_reg_hst.agent_id_type,cAgent_id_type);
            strcpy(sFn_reg_hst.agent_id,cAgent_id);
            strcpy(sFn_reg_hst.agent_name,cAgent_name);
            sFn_reg_hst.buy_date=sFn_reg.tx_date;
            sFn_reg_hst.buy_trace_no=sFn_reg.trace_no;
            strcpy(sFn_reg_hst.brf,"更改签约账号");
            sprintf(sFn_reg_hst.filler,"%s|%d|%s|%d",cAc_no_old,iAc_seqn_old,cAc_no_new,iAc_seqn_new);
            iRet = Fn_reg_hst_Ins(sFn_reg_hst,g_pub_tx.reply);
            if(iRet)
            {
                Fn_reg_hst_Debug(&sFn_reg_hst);
                vtcp_log("登记理财产品登记簿错误iRet[%d]",iRet);
                strcpy(g_pub_tx.reply,"D333");
                goto ErrExit;
            }
        }
        Fn_reg_Clo_Upd();
    }
    else
    {
        vtcp_log( "操作类型错误!");
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    iLen=strlen(cAc_no_old);
    cAc_no_old_tmp[0]=cAc_no_old[iLen-4];
    cAc_no_old_tmp[1]=cAc_no_old[iLen-3];
    cAc_no_old_tmp[2]=cAc_no_old[iLen-2];
    cAc_no_old_tmp[3]=cAc_no_old[iLen-1];
    cAc_no_old_tmp[4]=0x00;

		pub_mob_sendmail( cAc_no_old_tmp, "D084",cAc_no_new,"2",0, 0.00 );

    memcpy(g_pub_tx.ac_no,cAc_no_old,sizeof(g_pub_tx.ac_no)-1);
    strcpy(g_pub_tx.brf,"更改理财签约账号");
    vtcp_log(" insert trace_log record ");
    if( pub_ins_trace_log() )
    {
        vtcp_log("登记交易流水出错! ");
        strcpy(g_pub_tx.reply,"P126");
        goto ErrExit;
    }

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    vtcp_log( "Before OK return: reply is[%s]", g_pub_tx.reply);
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 0;
ErrExit:    
    vtcp_log("Before return: reply is[%s]", g_pub_tx.reply);
    if(strlen(g_pub_tx.reply) == 0 || strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"S035");
    }
    set_zd_data(DC_REPLY, g_pub_tx.reply);
    return 1;
}
/*更新活期主表的控制金额*/
int upd_dd_mst(int mode,long ac_id,int ac_seqn,double tx_amt)
{
    int iRet = 0;
    double dAvbal = 0.00;
    struct dd_mst_c sDd_mst;
    struct dd_parm_c sDd_parm;

    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    iRet = Dd_mst_Dec_Upd(g_pub_tx.reply,"ac_id = %ld and ac_seqn = %d",ac_id,ac_seqn);
    if(iRet)
    {
        vtcp_log("定义更新活期主文件游标错误");
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    iRet = Dd_mst_Fet_Upd(&sDd_mst,g_pub_tx.reply);
    if(iRet)
    {
        vtcp_log("取更新活期主文件游标错误");
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
    if(iRet)
    {
        vtcp_log("查询活期产品参数错误");
        strcpy(g_pub_tx.reply,"015");
        goto ErrExit;
    }
    /*dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.zf_bal - sDd_mst.ctl_amt - sDd_parm.min_bal;*/
    dAvbal = sDd_mst.bal -  sDd_mst.hold_amt - sDd_mst.ctl_amt - sDd_parm.min_bal;
    if(mode  == 0)
    {
        sDd_mst.ctl_amt  -= tx_amt;
        if(pub_base_CompDblVal(sDd_mst.ctl_amt,0.00) < 0)
        {
            vtcp_log("控制金额小于0了啊[%lf]",sDd_mst.ctl_amt);
            strcpy(g_pub_tx.reply,"D229");
            goto ErrExit;
        }
    }
    else if(mode == 1)
    {
        /*检查下可用余额够不够啊？*/
        if(pub_base_CompDblVal(tx_amt,dAvbal) > 0)
        {
            vtcp_log("可用金额不足了 tx_amt[%lf] dAvbal[%lf]",tx_amt,dAvbal);
            strcpy(g_pub_tx.reply,"A030");
            goto ErrExit;
        }
        sDd_mst.ctl_amt += tx_amt;
    }
    else
    {
        vtcp_log("操作标志错误[%d]",mode);
        strcpy(g_pub_tx.reply,"S035");
        goto ErrExit;
    }
    iRet = Dd_mst_Upd_Upd(sDd_mst,g_pub_tx.reply);
    if(iRet)
    {
        vtcp_log("更新活期主文件失败");
        strcpy(g_pub_tx.reply,"O012");
        goto ErrExit;
    }
    Dd_mst_Clo_Upd();
OkExit:
    vtcp_log("更新活期主文件成功");
    strcpy(g_pub_tx.reply,"0000");
    return 0;
ErrExit:
    vtcp_log("更新活期主文件失败");
    return 1;
}

