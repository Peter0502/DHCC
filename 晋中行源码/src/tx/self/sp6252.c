/*************************************************
* 文 件 名:  sp6238.c
* 功能描述： 联机单笔转帐记账交易
*            本交易用于行内帐户/科目联机单笔转账记账。
* 作    者:  chenchao
* 完成日期： 2011-7-12 13:57:59
*
* 修改记录： 
*    日    期:2011-8-5
*    修 改 人:gong
*    修改内容:添加手续费收费项目
*    日    期:20110816
*    修 改 人：xueke
*    修改内容:pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
* 修改记录： 
*    日    期:2012-2-25
*    修 改 人:商永超
*    修改内容:增加挂账检查
insert into tx_def  values ('6238', '联机单笔转账记账', '10000000000000000000000000000100000000010000000000000000000001000100010000000000100000000000000000000000000000000000000000000000', '1', '1', '0');
insert into tx_flow_def  values ('6238', 0, '6238', '#=0000@1@#$');
insert into tx_flow_def  values ('6238', 1, 'HPDF', '#=0000@2@#$');
insert into tx_flow_def  values ('6238', 2, 'NULL', '#=0001@3@#=0002@4@#=0003@5@#=0004@6@#$');
insert into tx_flow_def  values ('6238', 3, 'D003', '#=0000@7@#$');
insert into tx_flow_def  values ('6238', 4, 'D099', '#=0000@7@#$');
insert into tx_flow_def  values ('6238', 5, 'A016', '#=0000@7@#$');
insert into tx_flow_def  values ('6238', 6, 'A017', '#=0000@7@#$');
insert into tx_flow_def  values ('6238', 7, 'F059', '#=0000@8@#$');
insert into tx_flow_def  values ('6238', 8, 'F000', '#$');

insert into tx_sub_def  values ('6238', '联机单笔转账记账交易', 'sp6238', '0', '0');

insert into tx_sub_rel  values ('6238', 'HPDF', '0', '0660', '0720', '借贷标志');
insert into tx_sub_rel  values ('6238', 'HPDF', '1', '01', '0210', '币种');
insert into tx_sub_rel  values ('6238', 'HPDF', '0', '0300', '0300', '账号');
insert into tx_sub_rel  values ('6238', 'HPDF', '1', '2', '0670', '现转标志');
insert into tx_sub_rel  values ('6238', 'HPDF', '0', '0400', '0400', '金额');
insert into tx_sub_rel  values ('6238', 'HPDF', '0', '0810', '0880', '摘要');
insert into tx_sub_rel  values ('6238', 'HPDF', '0', '0890', '0890','凭证种类');
insert into tx_sub_rel  values ('6238', 'HPDF', '0', '0590', '0580','凭证号码');

insert into tx_sub_rel  values ('6238', 'NULL', '0', '0700', '0120','配置分支');

insert into tx_sub_rel  values ('6238', 'D003', '0', '0310', '1021', '账号');
insert into tx_sub_rel  values ('6238', 'D003', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6238', 'D003', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6238', 'D003', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6238', 'D003', '1', '1', '1022', '账户序号');
insert into tx_sub_rel  values ('6238', 'D003', '0', '0400', '102F', '金额');
insert into tx_sub_rel  values ('6238', 'D003', '1', '01', '102J', '币种');
insert into tx_sub_rel  values ('6238', 'D003', '1', '2', '102K', '现转标志');
insert into tx_sub_rel  values ('6238', 'D003', '0', '0810', '102Y', '摘要');

insert into tx_sub_rel  values ('6238', 'D099', '0', '0310', '1011', '账号');
insert into tx_sub_rel  values ('6238', 'D099', '1', '01', '101A', '币种');
insert into tx_sub_rel  values ('6238', 'D099', '1', '2', '101B', '现转');
insert into tx_sub_rel  values ('6238', 'D099', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6238', 'D099', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6238', 'D099', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6238', 'D099', '0', '0810', '1016', '摘要');
insert into tx_sub_rel  values ('6238', 'D099', '1', '1', '1012', '序号');
insert into tx_sub_rel  values ('6238', 'D099', '0', '0400', '1013', '金额');

insert into tx_sub_rel  values ('6238', 'A016', '0', '0310', '1201', '账号');
insert into tx_sub_rel  values ('6238', 'A016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('6238', 'A016', '1', '2', '1205', '转账');
insert into tx_sub_rel  values ('6238', 'A016', '0', '0400', '1208', '金额');
insert into tx_sub_rel  values ('6238', 'A016', '0', '0810', '120A', '摘要');

insert into tx_sub_rel  values ('6238', 'A017', '0', '0310', '1211', '账号');
insert into tx_sub_rel  values ('6238', 'A017', '1', '2', '1215', '现转标志');
insert into tx_sub_rel  values ('6238', 'A017', '0', '0400', '1218', '金额');
insert into tx_sub_rel  values ('6238', 'A017', '0', '0810', '121A', '摘要');
insert into tx_sub_rel  values ('6238', 'A017', '1', '01', '1214', '币种');

*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_acct_c.h"
#include "dd_mst_c.h"
#include "com_item_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "dd_parm_c.h"

int sp6238()
{
    int     iRet = 0;
    int     i=0;
    char    cFee_flag[2]; /*收费标志*/
    char    tmpstr[41];
    char    cFd123[1000]; /*接收123域的收费内容*/
    long    lFee_cnt=0;     /*收费条数*/
    char    cFee_cnt[3];    /*收费条数*/
    char    cFee_amt[13];   /*收费金额*/
    char    cFee_acno[25];  /*费用账号或科目*/
    char    cAc_name1[61];    /** 第一户名 **/
    char    cAc_name2[61];    /** 第二户名 **/
    char    cAc_no1[25];    /** 主账号 **/
    char    cAc_no2[25];    /** 第二账号 **/
    char    cDc_ind[2];    /** 借贷标志 1借2贷 **/
    char    cFee_dc_ind[2]; /*费用的借贷标志 1 借 2贷*/
    char    cMsr2[38];    /** 二磁道信息 **/
    char    cNote_type[4];    /** 凭证种类 **/
    char    cNote_no[17];    /** 凭证号码 **/
    char    cName_tmp[61];    /** 比对用NAME **/
    char    cMsr3[105];    /** 三磁道信息 **/
    char    cPwd[17];    /** 客户密码 **/
    char    cBrf[51];    /** 记账摘要 **/
    char    cGz_flag[2];    /*挂账标志*/
    double  dTx_amt = 0.00;    /** 交易金额 **/
    double  dFee_amt = 0.00; /*手续费金额*/
    double  dFee_amt1=0.00;
    double  dKy_amt=0.00;/*可用金额*/
    double  dBal=0.00;
    double  dCif_fee=0.00;  /*应收客户手续费*/
    char    cCert_no[21];    /** 证件类型(1位)+号码(19位) 证件类型：1：身份证；2：户口簿；
                    3：护照；4：军人证；5：回乡证；6：士兵证；7：港澳居民来往通行证
                    8：企业代码证；9：经营许可证；A: 营业执照；B: 事业法人证书；
                    C：工商核准号；D：其他（对公）；E：临时身份证；F：外国人居留证；
                    G：警官证；H：其他证件（对私）；F：台湾同胞来往通行证**/
    char    cCtrlcmd[11];    /**控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
                    检查标志(0不检查1检查)：
                    [0]:密码检查标志0不查1查
                    [1]:磁道信息检查标志0不查1查
                    [2]:证件检查标志0不查1查
                    [3]:户名检查标志0不查不查
                    [4]:帐号检查标志0不查不查
                    [5]:收款人挂帐标志0帐号必须相符不查户名1帐号户名必须都相符2帐号必须相符，户名不符挂帐3帐号或户名有一不符挂帐,9强制挂帐
                    [6]:通存通兑标志0允许1不允许
                    [7]:凭证检查标志0不查1查柜员凭证2查客户凭证
                    ******************/
    char    cDc_entry[601];    /** (除主帐号和现金外)其它会计分录
                    开户机构,帐号/科目,借贷标志，金额+回车
                    开户机构=END 标识结束
                    ******************/
    /**add by  shangyongchao **/
    char    cHx_term[5];/*终端号 COMM-通用 HVPS-大额 BEPS-小额*/
    char    cFd27[61];/*接收27域业务种类*/
    char    cYw_type[5];/*业务种类 1000来账 1001 往帐*/
    char    cGz_ind[2];/*挂账标志 0不挂账 1挂账*/
    char    cGz_br_no[BRNO_LEN+1];/*挂账机构*/
    char    cGz_reason[2];/*挂账原因 1：账号不存在 2：户名不符 
                            3：账户状态非法 4：介质状态非法 5：冻结标志非法
                             6：止付标志非法 7：其他原因 */
    char    cTmp_ac_no1[25];/*保存原cAc_no1*/
    char    cTmp_ac_no2[25];/*保存原cAc_no2*/
    int     iParm_seqn = 0;/*com_parm 表 参数序号*/
    /**end add          **/
    long    lPt_date=0;
    long    lPt_trace_no = 0;
    
    struct    chnl_single_acct_c    sChnl_single_acct;
    struct    dd_mst_c        sDd_mst;
    struct    com_item_c      sCom_item;
    struct    mdm_ac_rel_c    sMdm_ac_rel;
    struct    prdt_ac_id_c    sPrdt_ac_id;
    struct    in_mst_c        sIn_mst;
    struct    dd_parm_c       sDd_parm;
    /** 初始化各变量 **/
    memset(cHx_term,0x00,sizeof(cHx_term));
    memset(cFd27,0x00,sizeof(cFd27));
    memset(cYw_type,0x00,sizeof(cYw_type));
    memset(cGz_ind,0x00,sizeof(cGz_ind));
    memset(cGz_br_no,0x00,sizeof(cGz_br_no));
    memset(cGz_reason,0x00,sizeof(cGz_reason));
    memset(cTmp_ac_no1,0x00,sizeof(cTmp_ac_no1));
    memset(cTmp_ac_no2,0x00,sizeof(cTmp_ac_no2));
    
    memset(cFee_flag, 0x00 ,sizeof(cFee_flag));
    memset(cPwd, 0x00, sizeof(cPwd));
    memset(cBrf, 0x00, sizeof(cBrf));
    memset(cMsr2, 0x00, sizeof(cMsr2));
    memset(cMsr3, 0x00, sizeof(cMsr3));
    memset(cGz_flag,0x00,sizeof(cGz_flag));
    memset(cAc_no1, 0x00, sizeof(cAc_no1));
    memset(cDc_ind, 0x00, sizeof(cDc_ind));
    memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
    memset(cFee_acno,0x00,sizeof(cFee_acno));
    memset(cAc_name1, 0x00, sizeof(cAc_name1));
    memset(cCert_no, 0x00, sizeof(cCert_no));
    memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
    memset(cDc_entry, 0x00, sizeof(cDc_entry));
    memset(tmpstr, 0x00, sizeof(tmpstr));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sChnl_single_acct,0x00,sizeof(sChnl_single_acct));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sCom_item,0x00,sizeof(sCom_item));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(cFee_cnt,0x00,sizeof(cFee_cnt));
    memset(cFee_amt,0x00,sizeof(cFee_amt));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "CU06");
        goto ErrExit;
    }
    vtcp_log("[%s][%d],tx_date=[%d]",__FILE__,__LINE__,g_pub_tx.tx_date);
    set_zd_long("0460",g_pub_tx.tx_date);
    /*** 取值 **/
    
    get_zd_long("0440", &lPt_date);
    get_zd_long("0520", &lPt_trace_no);
    get_zd_data("0250", cAc_name1 );
    get_zd_data("0260", cAc_name2 );
    get_zd_data("0300", cAc_no1   );
    get_zd_data("0310", cAc_no2   );
    get_zd_data("0620", cCert_no);
    get_zd_data("0660", cDc_ind);
    get_zd_data("0710", cFee_flag);
    get_zd_data("0730", cCtrlcmd);
    get_zd_data("0750", cMsr2);
    get_zd_data("0760", cMsr3);
    /**
    get_zd_data("0790", cPwd);
    **/
    get_zd_data("0810", cBrf);
    get_zd_data("0890", cNote_type);
    get_zd_data("0590", cNote_no);
    get_zd_data("0950", cDc_entry);
    get_fd_data("1230", cFd123);
    get_zd_double("0400", &dTx_amt);
    get_zd_data("0100",cHx_term);
    get_zd_data("0270",cFd27);
    get_zd_data("0930",cGz_ind);
    get_zd_data("0910",cGz_br_no);
    /*收取手续费功能模块，add by gong 20110805*/
    if(cFee_flag[0] == '0')
    {
         sChnl_single_acct.fee_amt=0.00;
    }
    else if( cFee_flag[0] == '1' || cFee_flag[0] == '2')
    {
         memcpy(cFee_cnt,cFd123+1,2);
         vtcp_log("[%s][%d],cFee_cnt=[%s]",__FILE__,__LINE__,cFee_cnt);
         lFee_cnt=atoi(cFee_cnt);
         vtcp_log("[%s][%d],lFee_cnt=[%d]",__FILE__,__LINE__,lFee_cnt);
         i=0;
         dFee_amt1=0.00;
         for(i=0;lFee_cnt > i; i++)
         {    
              memset(cFee_amt,0x00,sizeof(cFee_amt));
              memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
              memcpy(cFee_acno,cFd123+3 + 48 * i,24);
              zip_space(cFee_acno);
              memcpy(cFee_dc_ind,cFd123+27+48*i,1);
              memcpy(cFee_amt,cFd123+29 + 48 * i,12);
              vtcp_log("[%s][%d],cFee_dc_ind =[%s]",__FILE__,__LINE__,cFee_dc_ind);
              vtcp_log("[%s][%d],cFee_amt=[%s]",__FILE__,__LINE__,cFee_amt);
              dFee_amt1=atof(cFee_amt) / 100.00;	/**modify 20110822: 金额以分为单位**/
              vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
              if(cFee_dc_ind[0] == '1')
              {     
                    if( strcmp(cFee_acno,cAc_no1) == 0 )
                    {
                        dCif_fee=dCif_fee + dFee_amt1;
                    }
                    dFee_amt=dFee_amt + dFee_amt1;
              }
    
    		 }  	
    }
    sChnl_single_acct.fee_amt=dCif_fee;      /*手续费金额*/    
    vtcp_log("[%s][%d]应收客户手续费=[%lf]",__FILE__,__LINE__,dCif_fee);
    /*******end by gong 20110805*******/
    
    pub_base_old_acno(cAc_no1);    /** 新旧账号转换 **/
    pub_base_old_acno(cAc_no2);    /** 新旧账号转换 **/
    zip_space(cAc_no1);
    zip_space(cAc_no2);
    /**add by     shangyongchao           挂账检查 2012/2/25 11:20:30                           **/
    strcpy(cTmp_ac_no1,cAc_no1);
    strcpy(cTmp_ac_no2,cAc_no2);
    memcpy(cYw_type,cFd27,sizeof(cYw_type)-1);
    /*当大小额且业务种类为1000（来账）时根据借贷标志对入账账号进行挂账检查 借：检查cAc_no2，贷：检查cAc_no1*/
     cGz_ind[0] = cCtrlcmd[5];
    vtcp_log("[%s][%d]终端号=[%s] cYw_type =[%s],cgz_ind=[%s],cgz_br_no=[%s],dTx_amt=[%lf]",\
    __FILE__,__LINE__,cHx_term,cYw_type,cGz_ind,cGz_br_no,dTx_amt);
    if( (memcmp(cHx_term,"HVPS",4) == 0  && memcmp(cYw_type,"1000",4) ==0) ||
        (memcmp(cHx_term,"BEPS",4) == 0  && memcmp(cYw_type,"1000",4) ==0) ||
	(strcmp(cHx_term, "sxps") == 0 && strcmp(cYw_type, "1000") == 0)
      )
    {
        if(memcmp(cHx_term,"HVPS",4) == 0)
        {
            iParm_seqn = 1000;
        }
        else if (strcmp(cHx_term, "sxps") == 0)
	{
            iParm_seqn = 30;
	}
	else
        {
            iParm_seqn = 2000;
        }
        if(cDc_ind[0] == '1')
        {
            iRet = pubf_com_CheckGZ(cAc_no2,cAc_name2,cGz_ind,cGz_br_no,cGz_reason,iParm_seqn);
            if (iRet)
            {
              sprintf(acErrMsg,"%s,%d,挂账检查函数失败",__FILE__,__LINE__);
              WRITEMSG
              goto ErrExit;	
            }
            vtcp_log("[%s][%d]cAc_no2=[%s],cAc_name2=[%s]",__FILE__,__LINE__,cAc_no2,cAc_name2);
            set_zd_data("0310",cAc_no2);
        }
        else if(cDc_ind[0] =='2')
        {
            iRet = pubf_com_CheckGZ(cAc_no1,cAc_name1,cGz_ind,cGz_br_no,cGz_reason,iParm_seqn);
            if (iRet)
            {
              sprintf(acErrMsg,"%s,%d,挂账检查函数失败",__FILE__,__LINE__);
              WRITEMSG
              goto ErrExit;	
            }
            vtcp_log("[%s][%d]cAc_no1=[%s],cAc_name1=[%s]",__FILE__,__LINE__,cAc_no1,cAc_name1);
            set_zd_data("0300",cAc_no1);
        }
        else
        {
            sprintf(acErrMsg,"借贷标志错误!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "P404");
            goto ErrExit;
        }
        set_zd_data("0690",cGz_reason);
	if(strlen(cGz_reason))
        {            
		set_zd_data("0930","1");        
	}        
	else        
	{            
		set_zd_data("0930","0");        
	}
        set_zd_data("0910",cGz_br_no);
    }
     /**end  add by     shangyongchao           挂账检查 2012/2/25 11:20:30                           **/
    /*73域控制标志检查*/
    iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name1,cAc_no1,cNote_type,cNote_no,"2",cGz_flag);
    vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cNote_type=[%s],cNote_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name1,cAc_no1,cNote_type,cNote_no,cGz_flag);
    if (iRet)
    {
      sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
      WRITEMSG
      goto ErrExit;	
    }
    
    
    if( strlen ( cAc_no1 ) >10) /*** 此种情况下,只能是账号 ***/
    {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no1);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no1);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        if(g_mdm_ac_rel.note_sts[0] != '0')
        {
            vtcp_log("%s,%d,ac_no[%s]介质状态不正常!",__FILE__,__LINE__,cAc_no1);
            strcpy(g_pub_tx.reply,"D140");
            goto ErrExit;
        }
        iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",g_mdm_ac_rel.ac_id);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cAc_no1);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no1);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sDd_mst.ac_sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no1);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no = '%s'",sDd_mst.prdt_no);
            if(iRet)
            {
                vtcp_log("%s,%d, 查找活期产品错误!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"W015");
                goto ErrExit;
            }
            memcpy(cName_tmp,sDd_mst.name,sizeof(cName_tmp)-1);
            set_zd_data("0680","1");/** 活期帐户 **/
            /*从活期户往外转钱时计算可用金额*/
            if(cDc_ind[0] == '1')   /*1-借*/
            {
                /*账号被全部冻结和只进不出，不允许取款*/
                 if(sDd_mst.hold_sts[0] == '1')
                 {
                    strcpy(g_pub_tx.reply,"P110");
                    sprintf(acErrMsg,"%s,%d,该账号被全部冻结",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
                }
                else if (sDd_mst.hold_sts[0] == '2')
                {
                    strcpy(g_pub_tx.reply,"P111");
                    sprintf(acErrMsg,"%s,%d,该账号处于只进不出状态",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
                }
                 /*检查可用余额 注意：冻结金额包含了 最小留存金额*/
                if(pub_base_CompDblVal(sDd_mst.hold_amt,sDd_parm.min_bal) >= 0)
                {
                    dKy_amt = sDd_mst.bal -sDd_mst.ctl_amt -sDd_mst.hold_amt;
                }
                else
                {
                    dKy_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_parm.min_bal;
                }             
                if(pub_base_CompDblVal(dKy_amt,dTx_amt + dCif_fee) < 0)
                {
                    strcpy(g_pub_tx.reply,"A030");
                    sprintf(acErrMsg,"%s,%d,可用余额不足",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
                }
                
                dBal=0.00;
                dKy_amt=0.00;
                dBal=sDd_mst.bal - dTx_amt - dCif_fee;
                dKy_amt=dBal - sDd_mst.ctl_amt - sDd_mst.hold_amt; 
                memcpy(tmpstr,"1001156C",8);
                sprintf(tmpstr+8,"%012.0f",dBal*100);
                memcpy(tmpstr+20,"1002156C",8);
                sprintf(tmpstr+28,"%012.0f",dKy_amt*100);
                vtcp_log("%s,%d,查询到的余额是：tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
                set_zd_data("0820",tmpstr);
            }
            else if( cDc_ind[0] == '2' )
            {
                if(pub_base_CompDblVal(sDd_mst.hold_amt,sDd_parm.min_bal) >= 0)
                {
                    dKy_amt = sDd_mst.bal -sDd_mst.ctl_amt -sDd_mst.hold_amt;
                }
                else
                {
                    dKy_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_parm.min_bal;
                }             
                
                dBal=0.00;
                dKy_amt=0.00;
                dBal=sDd_mst.bal + dTx_amt - dCif_fee;
                dKy_amt=dBal - sDd_mst.ctl_amt - sDd_mst.hold_amt; 
                memcpy(tmpstr,"1001156C",8);
                sprintf(tmpstr+8,"%012.0f",dBal*100);
                memcpy(tmpstr+20,"1002156C",8);
                sprintf(tmpstr+28,"%012.0f",dKy_amt*100);
                vtcp_log("%s,%d,查询到的余额是：tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
                set_zd_data("0820",tmpstr);
            }
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no1);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no1);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            memcpy(cName_tmp,sIn_mst.name,sizeof(cName_tmp)-1);
            set_zd_data("0680","9");/** 内部帐户 **/
        }
    }
    else
    {        /**** 否则是科目  **/
        iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no1);
        if(iRet)
        {
            sprintf(acErrMsg,"查询科目失败!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
    }
    if( strlen ( cAc_no2 ) >10) /*** 此种情况下,只能是账号 ***/
    {
        vtcp_log("[%s][%d] ac_no=[%s]",__FILE__,__LINE__,cAc_no2);
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",cAc_no2);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no2);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        if(sMdm_ac_rel.note_sts[0] != '0')
        {
            vtcp_log("%s,%d,ac_no[%s]介质状态不正常 note_sts=[%s]!",__FILE__,__LINE__,cAc_no2,sMdm_ac_rel.note_sts);
            strcpy(g_pub_tx.reply,"D140");
            goto ErrExit;
        }
        iRet = Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld ",sMdm_ac_rel.ac_id);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,cAc_no2);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no2);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sDd_mst.ac_sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no2);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            /**set_zd_data("0680","1"); 活期帐户 **/
            if(sDd_mst.hold_sts[0] == '1')
            {
                    strcpy(g_pub_tx.reply,"P999");
                    sprintf(acErrMsg,"%s,%d,该账号被全部冻结",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
            }
            if(cDc_ind[0] == '1')   /*1-借*/
            {
                set_zd_data("0700","2");
                vtcp_log("[%s][%d],FD70",__FILE__,__LINE__);
            }
            else if(cDc_ind[0] == '2')   /*2-贷*/
            {
                set_zd_data("0700","1");
                vtcp_log("[%s][%d],FD70",__FILE__,__LINE__);
            }
            else
            {
                sprintf(acErrMsg,"借贷标志错误!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "P404");
                goto ErrExit;
            }
        }
        else if(sPrdt_ac_id.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",sMdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,cAc_no2);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,cAc_no2);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            /**set_zd_data("0680","9"); 内部帐户 **/
            vtcp_log("[%s][%d] cDc_ind=%s \n",__FILE__,__LINE__,cDc_ind);
            if(cDc_ind[0] == '1')   /*1-借*/
            {
                set_zd_data("0700","4");
                vtcp_log("[%s][%d],FD70",__FILE__,__LINE__);
            }
            else if(cDc_ind[0] == '2')   /*2-贷*/
            {
                set_zd_data("0700","3");
                vtcp_log("[%s][%d],FD70",__FILE__,__LINE__);
            }
            else
            {
                sprintf(acErrMsg,"借贷标志错误!!!");
                WRITEMSG
                strcpy(g_pub_tx.reply, "P404");
                goto ErrExit;
            }
        }        
    }
    else
    {        /**** 否则是科目  **/
        iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",cAc_no2);
        if(iRet)
        {
            sprintf(acErrMsg,"查询科目失败!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "B172");
            goto ErrExit;
        }
        /*** 开始配置记帐要素 **/
        if(cDc_ind[0] == '1')   /*1-借*/
        {
            set_zd_data("0700","4");
            vtcp_log("[%s][%d],FD70",__FILE__,__LINE__);
        }
        else if(cDc_ind[0] == '2')   /*2-贷*/
        {
            set_zd_data("0700","3");
            vtcp_log("[%s][%d],FD70",__FILE__,__LINE__);
        }
        else
        {
            sprintf(acErrMsg,"借贷标志错误!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "P404");
            goto ErrExit;
        }
    }
    
    /**　登记单笔记账登记薄　***/
    sChnl_single_acct.tx_date = g_pub_tx.tx_date;    /** 系统日期  **/
    sprintf(sChnl_single_acct.tx_time,"%6ld", g_pub_tx.tx_time);    /** 系统时间  **/
    /*sChnl_single_acct.trace_no = g_pub_tx.ejfno;    ** 核心流水 非柜员流水号，登记的是核心交易流水号 Mod by CHENGBO 20110727**/
    sChnl_single_acct.trace_no = g_pub_tx.trace_no;    /** 核心流水  **/
    memcpy(sChnl_single_acct.cur_no,"01",2); /**  暂时写死为01人民币 ***/
    memcpy(sChnl_single_acct.ac_no, cTmp_ac_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** 主账号 **/
    if(cDc_ind[0]=='1')
    {
        memcpy(sChnl_single_acct.ac_no1, cAc_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** 付款账号 **/
        /* 登记cAc_no2时用保存的平台发送来的 账号 shangyongchao 2012/2/25 11:27:08*/
        memcpy(sChnl_single_acct.ac_no2, cTmp_ac_no2,sizeof( sChnl_single_acct.ac_no)-1);    /** 收款账号 **/
        /*如果账号发生变化 表示进行了挂账 登记挂账账号 shangyongchao 2012/2/25 11:27:08*/
        if(memcmp(cTmp_ac_no2,cAc_no2,sizeof(cTmp_ac_no2)-1) != 0)
        {
            memcpy(sChnl_single_acct.g_ac_no2,cAc_no2,sizeof(sChnl_single_acct.g_ac_no2)-1);/*挂账账号*/
        }
    }
    else if(cDc_ind[0]=='2')
    {
        memcpy(sChnl_single_acct.ac_no1, cAc_no2,sizeof( sChnl_single_acct.ac_no)-1);    /** 付款账号 **/
        /* 登记cAc_no1时用保存的平台发送来的 账号 shangyongchao 2012/2/25 11:27:08*/
        memcpy(sChnl_single_acct.ac_no2, cTmp_ac_no1,sizeof( sChnl_single_acct.ac_no)-1);    /** 收款账号 **/
        /*如果账号发生变化 表示进行了挂账 登记挂账账号 shangyongchao 2012/2/25 11:27:08*/
        if(memcmp(cTmp_ac_no1,cAc_no1,sizeof(cTmp_ac_no1)-1) != 0)
        {
            memcpy(sChnl_single_acct.g_ac_no2,cAc_no1,sizeof(sChnl_single_acct.g_ac_no2)-1);/*挂账账号*/
        }
    }
    memcpy(sChnl_single_acct.tx_code,g_pub_tx.tx_code,sizeof(sChnl_single_acct.tx_code)-1);    /** 交易码 **/
    memcpy(sChnl_single_acct.tx_br_no,g_pub_tx.tx_br_no,sizeof(sChnl_single_acct.tx_br_no)-1);    /** 交易机构 **/
    memcpy(sChnl_single_acct.opn_br_no,sChnl_single_acct.opn_br_no,sizeof(sChnl_single_acct.opn_br_no)-1);    /** 开户机构 **/
    memcpy(sChnl_single_acct.tel,g_pub_tx.tel,sizeof(sChnl_single_acct.tel)-1);    /** 交易柜员  **/
    memcpy(sChnl_single_acct.bbmk,g_pub_tx.tty,sizeof(sChnl_single_acct.bbmk)-1);    /** 系统种类 **/
    memcpy(sChnl_single_acct.ttyn,g_pub_tx.tty,sizeof(sChnl_single_acct.ttyn)-1);    /** 终端号 **/
    sChnl_single_acct.pt_date = lPt_date;    /** 平台日期  **/
    sprintf(sChnl_single_acct.pt_trace_no ,"%ld" ,lPt_trace_no); /** 平台流水 **/
    sChnl_single_acct.tx_amt = dTx_amt ;    /** 交易金额 **/
    sChnl_single_acct.ct_ind[0] = '2';    /** 现转标志  本交易只处理转账 **/
    memcpy(sChnl_single_acct.brf,cBrf,sizeof(sChnl_single_acct.brf)-1);    /** 记账摘要 **/
    sChnl_single_acct.sts[0] = '1';    /** 记账状态初始为成功 **/
    sChnl_single_acct.dz_ind[0] = '0';    /**  对账状态 **/
    sChnl_single_acct.chnl_type[0] = cCtrlcmd[9]; /* 20120915 增加渠道标志 */
    Chnl_single_acct_Debug(&sChnl_single_acct);
    iRet = Chnl_single_acct_Ins(sChnl_single_acct,g_pub_tx.reply);
    if(iRet)
    {
        sprintf(acErrMsg,"登记单笔记账登记薄出错!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D107");
        goto ErrExit;
    }
GoodExit:
    /*add by shangyongchao 2012/2/25 13:46:06 增加正确出口对reply的赋值*/
    sprintf(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
ErrExit:
    if(memcmp(g_pub_tx.reply,"0000",4) == 0)
    {
        strcpy(g_pub_tx.reply, "H034");
    }
    sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}

