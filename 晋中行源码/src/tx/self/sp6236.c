/*************************************************
* 文 件 名:  sp6236.c
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
insert into tx_def  values ('6236', '联机单笔转账记账', '10000000000000000000000000000100000000010000000000000000000001000101111000000000100000000000000000000000000000000000000000000000', '1', '1', '0', '3', null);
insert into tx_flow_def  values ('6236', 0, '6236', '#=0000@1@#$');
insert into tx_flow_def  values ('6236', 1, 'HPDF', '#=0000@2@#$');
insert into tx_flow_def  values ('6236', 2, 'NULL', '#=0001@3@#=0002@4@#=0003@5@#=0004@6@#$');
insert into tx_flow_def  values ('6236', 3, 'D003', '#=0000@7@#$');
insert into tx_flow_def  values ('6236', 4, 'D099', '#=0000@7@#$');
insert into tx_flow_def  values ('6236', 5, 'A016', '#=0000@7@#$');
insert into tx_flow_def  values ('6236', 6, 'A017', '#=0000@7@#$');
insert into tx_flow_def  values ('6236', 7, 'F000', '#$');

insert into tx_sub_def  values ('6236', '联机单笔转账记账交易', 'sp6236', '0', '0');

insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0660', '0720', '借贷标志');
insert into tx_sub_rel  values ('6236', 'HPDF', '1', '01', '0210', '币种');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0300', '0300', '账号');
insert into tx_sub_rel  values ('6236', 'HPDF', '1', '2', '0670', '现转标志');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0400', '0400', '金额');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0810', '0880', '摘要');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0890', '0890','凭证种类');
insert into tx_sub_rel  values ('6236', 'HPDF', '0', '0590', '0580','凭证号码');

insert into tx_sub_rel  values ('6236', 'NULL', '0', '0700', '0120','配置分支');

insert into tx_sub_rel  values ('6236', 'D003', '0', '0310', '1021', '账号');
insert into tx_sub_rel  values ('6236', 'D003', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6236', 'D003', '1', '1', '1022', '账户序号');
insert into tx_sub_rel  values ('6236', 'D003', '0', '0400', '102F', '金额');
insert into tx_sub_rel  values ('6236', 'D003', '1', '01', '102J', '币种');
insert into tx_sub_rel  values ('6236', 'D003', '1', '2', '102K', '现转标志');
insert into tx_sub_rel  values ('6236', 'D003', '0', '0810', '102Y', '摘要');

insert into tx_sub_rel  values ('6236', 'D099', '0', '0310', '1011', '账号');
insert into tx_sub_rel  values ('6236', 'D099', '1', '01', '101A', '币种');
insert into tx_sub_rel  values ('6236', 'D099', '1', '2', '101B', '现转');
insert into tx_sub_rel  values ('6236', 'D099', '1', '0000', '0192', '控制标志');
insert into tx_sub_rel  values ('6236', 'D099', '1', '1001', '0193', '普通存取');
insert into tx_sub_rel  values ('6236', 'D099', '1', '1', '0191', '控制标志');
insert into tx_sub_rel  values ('6236', 'D099', '0', '0810', '1016', '摘要');
insert into tx_sub_rel  values ('6236', 'D099', '1', '1', '1012', '序号');
insert into tx_sub_rel  values ('6236', 'D099', '0', '0400', '1013', '金额');

insert into tx_sub_rel  values ('6236', 'A016', '0', '0310', '1201', '账号');
insert into tx_sub_rel  values ('6236', 'A016', '1', '01', '1204', '币种');
insert into tx_sub_rel  values ('6236', 'A016', '1', '2', '1205', '转账');
insert into tx_sub_rel  values ('6236', 'A016', '0', '0400', '1208', '金额');
insert into tx_sub_rel  values ('6236', 'A016', '0', '0810', '120A', '摘要');

insert into tx_sub_rel  values ('6236', 'A017', '0', '0310', '1211', '账号');
insert into tx_sub_rel  values ('6236', 'A017', '1', '2', '1215', '现转标志');
insert into tx_sub_rel  values ('6236', 'A017', '0', '0400', '1218', '金额');
insert into tx_sub_rel  values ('6236', 'A017', '0', '0810', '121A', '摘要');
insert into tx_sub_rel  values ('6236', 'A017', '1', '01', '1214', '币种');

*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_single_cnaps_c.h"
#include "dd_mst_c.h"
#include "com_item_c.h"
#include "mdm_ac_rel_c.h"
#include "prdt_ac_id_c.h"
#include "in_mst_c.h"
#include "dd_parm_c.h"
#include "mo_prdt_prot_c.h"
#include "mo_prot_ac_rel_c.h"
#include "draw_loan_limit_c.h"
#include "draw_loan_hst_c.h"

static int fd123_info_chg();

int sp6236()
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
    /**add by  shangyongchao 20121018**/
    char    cHx_term[5];/*终端号 COMM-通用 HVPS-大额 BEPS-小额*/
    char    cFd27[61];/*接收27域业务种类*/
    char    cFd28[9];/*接收代收付标示*/
    char    cYw_type[5];/*业务种类 1000来账 1001 往帐*/
    char    cGz_ind[2];/*挂账标志 cCtrlcmd[5] 改为从73域取 shangyongchao  2012/7/16 10:38:17*/
    char    cGz_br_no[BRNO_LEN+1];/*挂账机构*/
    char    cGz_reason[2];/*挂账原因 1：账号不存在 2：户名不符 
                            3：账户状态非法 4：介质状态非法 5：冻结标志非法
                             6：止付标志非法 7：其他原因 */
    char    cTmp_ac_no1[25];/*保存原cAc_no1*/
    char    cTmp_ac_no2[25];/*保存原cAc_no2*/
    int     iParm_seqn = 0;/*com_parm 表 参数序号*/
    /**end add 20121018**/
    
    long    lPt_date=0;
    long    lPt_trace_no = 0;
    
    struct    chnl_single_cnaps_c    stChnlCnaps;
    struct    dd_mst_c        sDd_mst;
    struct    com_item_c      sCom_item;
    struct    mdm_ac_rel_c    sMdm_ac_rel;
    struct    prdt_ac_id_c    sPrdt_ac_id;
    struct    in_mst_c        sIn_mst;
    struct    dd_parm_c       sDd_parm;
    struct    mo_prdt_prot_c  sMo_prdt_prot;
	struct    mo_prot_ac_rel_c    sMo_prot_ac_rel;
	struct    dd_mst_c        sDd_mst_virtual;
	
	struct draw_loan_hst_c sDraw_hst = {0};/*添加受托支付 Added by YangMeng 2014/7/10 16:40:25*/
	struct draw_loan_limit_c sDraw_limit = {0};/*添加受托支付 Added by YangMeng 2014/7/10 16:40:25*/
    /** 初始化各变量 **/
    char	cFd125[282] = {0};/*Added by YangMeng 20140703 登记交易关联变量定义*/
    char    sRz_br_no[BRNO_LEN+1];/*挂账机构*/
    
    memset(cHx_term,0x00,sizeof(cHx_term));
    memset(cFd27,0x00,sizeof(cFd27));
    memset(cFd28,0x00,sizeof(cFd28));
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
    memset(cAc_name2, 0x00, sizeof(cAc_name2));
    memset(cCert_no, 0x00, sizeof(cCert_no));
    memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
    memset(cDc_entry, 0x00, sizeof(cDc_entry));
    memset(tmpstr, 0x00, sizeof(tmpstr));
    memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
    memset(&sDd_mst,0x00,sizeof(sDd_mst));
    memset(&sCom_item,0x00,sizeof(sCom_item));
    memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
    memset(&sIn_mst,0x00,sizeof(sIn_mst));
    memset(cFee_cnt,0x00,sizeof(cFee_cnt));
    memset(cFee_amt,0x00,sizeof(cFee_amt));
    memset(&sDd_parm,0x00,sizeof(sDd_parm));
    memset(&sMo_prdt_prot,0x00,sizeof(struct mo_prdt_prot_c));
    memset(&stChnlCnaps,0x00,sizeof(struct chnl_single_cnaps_c));
	memset(&sMo_prot_ac_rel,0x00,sizeof(struct mo_prot_ac_rel_c));
	memset(&sDd_mst_virtual,0x00,sizeof(struct dd_mst_c));
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "CU06");
        goto ErrExit;
    }
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
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
    get_zd_data("0280",cFd28);
    get_zd_data("0910",cGz_br_no);
    
    /**Added by YangMeng 20140703 渠道交易增加相应的业务信息(对方账号、户名等)begin*/
    memset(cFd125,0x00,sizeof(cFd125));
    get_fd_data("1250",cFd125);
    vtcp_log("[%s][%d],代收付标示cFd28=[%s]",__FILE__,__LINE__,cFd28);
    /*if(!memcmp(cHx_term,"HVPS",4) || (!memcmp(cHx_term,"BEPS",4)) && memcmp(cFd28,"DSF",3))*/
    if(!memcmp(cFd28,"DSFO",3) || !memcmp(cFd28,"NOT",3))
    {
         vtcp_log("[%s][%d],进入此分支,不登记chnl_tx_detl表",__FILE__,__LINE__);
    }else{
			vtcp_log("[%s][%d],关联信息登记cFd125=[%s]",__FILE__,__LINE__,cFd125);
			iRet=pubf_com_checkin_txdetail(cFd125);
			if (iRet)
			{
				sprintf(acErrMsg,"%s,%d,执行渠道交易增加相应业务信息函数失败",__FILE__,__LINE__,iRet);
				/*WRITEMSG
				goto ErrExit;*/
				strcpy(g_pub_tx.reply,"0000");
			}
			strcpy(g_pub_tx.reply,"0000");
			vtcp_log("[%s][%d],渠道登记返回码，g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
    }/**Added by YangMeng 20140703 渠道交易增加相应的业务信息(对方账号、户名等)end*/
	
    /*收取手续费功能模块，add by gong 20110805*/
    if(fd123_info_chg())
    {
		goto ErrExit;	
    }
    if(cFee_flag[0] == '0')
    {
         stChnlCnaps.fee_amt=0.00;
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
              vtcp_log("[%s][%d],cFee_acno=[%s]",__FILE__,__LINE__,cFee_acno);
              memcpy(cFee_dc_ind,cFd123+3+24 + 48 * i,1);
              memcpy(cFee_amt,cFd123+3+26 + 48 * i,12);
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
         vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
         stChnlCnaps.fee_amt=dFee_amt;      /*手续费金额*/             
    }
    vtcp_log("[%s][%d]应收客户手续费=[%lf]",__FILE__,__LINE__,dCif_fee);
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
    /*******end by gong 20110805*******/
    
    pub_base_old_acno(cAc_no1);   /** 新旧账号转换 **/
    pub_base_old_acno(cAc_no2);   /** 新旧账号转换 **/
    zip_space(cAc_no1);
    zip_space(cAc_no2);
    /*新旧账号转换时，如果没有记录不需要转换，因此将返回值设为0000*/
    memcpy(g_pub_tx.reply, "0000", 4);

	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
    vtcp_log("%s,%d,cAc_no2[%s]",__FILE__,__LINE__,cAc_no2);
	
    /**add by     shangyongchao           挂账检查 2012/10/18**/
    strcpy(cTmp_ac_no1,cAc_no1);
    strcpy(cTmp_ac_no2,cAc_no2);
    memcpy(cYw_type,cFd27,sizeof(cYw_type)-1);
    /*当大小额且业务种类为1000（来账）时根据借贷标志对入账账号进行挂账检查 借：检查cAc_no2，贷：检查cAc_no1*/
    cGz_ind[0] = cCtrlcmd[5];
    vtcp_log("[%s][%d]终端号=[%s] cYw_type =[%s],cgz_ind=[%s],cgz_br_no=[%s],dTx_amt=[%lf]",\
    __FILE__,__LINE__,cHx_term,cYw_type,cGz_ind,cGz_br_no,dTx_amt);
    if( (memcmp(cHx_term,"HVPS",4) == 0  && memcmp(cYw_type,"1000",4) ==0) ||
        (memcmp(cHx_term,"BEPS",4) == 0  && memcmp(cYw_type,"1000",4) ==0)
      )
    {
        if(memcmp(cHx_term,"HVPS",4) == 0)
        {
            iParm_seqn = 6;
        }
        else
        {
            iParm_seqn = 8;
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
    		memcpy(g_pub_tx.reply, "0000", 4);
        }
        else
        {
            sprintf(acErrMsg,"借贷标志错误!!!");
            WRITEMSG
            strcpy(g_pub_tx.reply, "P404");
            goto ErrExit;
        }
        set_zd_data("0690",cGz_reason);
        set_zd_data("0770",cGz_br_no);
        vtcp_log("[%s][%d],原因cGz_reason[%s]，机构cGz_br_no[%s]",__FILE__,__LINE__,cGz_reason,cGz_br_no);
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
    /**end  add by     shangyongchao           挂账检查 2012/10/18**/
     
    /*73域控制标志检查*/
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);

	if( strlen ( cAc_no1 ) > 10 ) /** add by chenhw  20120907 借方科目不检查密码 **/
	{
		iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name1,cAc_no1,cNote_type,cNote_no,"2",cGz_flag);
	}
    
    vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cNote_type=[%s],cNote_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name1,cAc_no1,cNote_type,cNote_no,cGz_flag);
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
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
        if((!memcmp(cHx_term,"HVPS",4)  && memcmp(cYw_type,"1000",4)) ||
        (!memcmp(cHx_term,"BEPS",4)  && memcmp(cYw_type,"1000",4)))
        {
        	/*记账时修改委托支付状态 Added by YangMeng 2014/7/10 18:35:55 begin*/
			iRet = Draw_loan_hst_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and sts='1' ",g_mdm_ac_rel.ac_id);
			if(iRet)
			{
				sprintf(acErrMsg, "[%s][%d]授权支付明细表定义游标错误[%d]",__FILE__,__LINE__,iRet);
				WRITEMSG
				strcpy(g_pub_tx.reply, "D101");
				goto ErrExit;
			}
			while(1){
				memset(&sDraw_hst,0x00,sizeof(struct draw_loan_hst_c));
				iRet = Draw_loan_hst_Fet_Upd(&sDraw_hst,g_pub_tx.reply);
				if (iRet == 100){
					vtcp_log("[%s],[%d],没有受托账户退出!",__FILE__,__LINE__);
					break;
				}else if(iRet){
					sprintf(acErrMsg, "授权支付明细表取游标错误[%d]",iRet );
					WRITEMSG
					strcpy(g_pub_tx.reply, "D102");
					goto ErrExit;
				}else{
					
					iRet = sql_execute("update draw_loan_limit set sts = '*' where pact_no = '%s'",sDraw_hst.pact_no);
        			if(iRet)
        			{
        			    sprintf(acErrMsg, "修改贷款支付主表错误![%d]",iRet);
        			    WRITEMSG
        				Draw_loan_hst_Clo_Upd();
					    goto ErrExit;
					}
					
					/*memcpy(sDraw_hst.sts,"2",1);*/
					sDraw_hst.sts[0] = '2';
					vtcp_log("[%s],[%d],更新授权支付明细表sts=[%s]!",__FILE__,__LINE__,sDraw_hst.sts);
					
					iRet = Draw_loan_hst_Upd_Upd(sDraw_hst,g_pub_tx.reply);
					if (iRet){
						Draw_loan_hst_Clo_Upd();
					    sprintf(acErrMsg, "修改贷款支付明细错误![%d]",iRet);
					    WRITEMSG
					    goto ErrExit;
					}
				}
			}
			Draw_loan_hst_Clo_Upd();
		}
		/*记账时修改委托支付状态 Added by YangMeng 2014/7/10 18:35:55 end*/
		
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
            /*add fupengfei 2014/7/5 14:47:57 beg*/
			/**1.检查.是否是七天乐账户.**/
			vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,sDd_mst.ac_id);
			iRet = Mo_prdt_prot_Sel(g_pub_tx.reply,&sMo_prdt_prot," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",sDd_mst.ac_id);
			if(iRet == 100)
			{
				vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
				strcpy( g_pub_tx.reply,"0000");
				/**无七天乐签约.继续原程序.**/
			}
			else if(iRet)
			{
				sprintf(acErrMsg,"Mo_prdt_prot_Sel error iRet=[%d]",iRet);
				WRITEMSG
				strcpy( g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			else
			{
				vtcp_log("[%s][%d]主账户 zyl 20110415 [%17.2f]",__FILE__,__LINE__,sDd_mst.bal);
				vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,sDd_mst.ac_id);
				
				vtcp_log("[%s][%d] 帐号=[%s] 帐户ID=[%d] ",__FILE__,__LINE__,g_pub_tx.ac_no,g_pub_tx.ac_id);
				iRet = Mo_prot_ac_rel_Sel(g_pub_tx.reply,&sMo_prot_ac_rel," prot_type='TK7' and sts!='*' and main_ac_id = %ld ",sDd_mst.ac_id);
				if(iRet == 100)
				{
					/**七天乐签约信息存在.与虚拟账户对照关系不存在.**/
					sprintf(acErrMsg,"七天乐签约信息存在.与虚拟账户对照关系不存在.");
					WRITEMSG
					strcpy( g_pub_tx.reply,"D104");
					goto ErrExit;
				}
				else if(iRet)
				{
					sprintf(acErrMsg,"Mo_prot_ac_rel_Sel error iRet=[%d]",iRet);
					WRITEMSG
					strcpy( g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				
				/**2.七天乐账户信息..签约账户,虚拟账户信息已取得.**/
				iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst_virtual," ac_id = %ld ",sMo_prot_ac_rel.app_ac_id);
				if(iRet == 100)
				{
					sprintf(acErrMsg,"dd_mst中无虚拟账户信息.");
					WRITEMSG
					strcpy( g_pub_tx.reply,"D104");
					goto ErrExit;
				}
				else if(iRet)
				{
					sprintf(acErrMsg,"Dd_mst_Sel error iRet=[%d]",iRet);
					WRITEMSG
					strcpy( g_pub_tx.reply,"D103");
					goto ErrExit;
				}
				sDd_mst.bal=sDd_mst.bal+sDd_mst_virtual.bal;
				vtcp_log("[%s][%d]虚拟账户余额bal=[%17.2f]",__LINE__,__FILE__,sDd_mst.bal);
				vtcp_log("[%s][%d] g_dd_mst.ac_id =[%d]",__FILE__,__LINE__,g_dd_mst.ac_id);
			}
			/*add fupengfei 2014/7/5 14:47:57 end*/
            
            strcpy(cName_tmp,sDd_mst.name);
			set_zd_data("0250",sDd_mst.name);
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
                /*6236不检查余额 fupengfei 2014/7/10 15:46:26*/
                /*if(pub_base_CompDblVal(dKy_amt,dTx_amt + dCif_fee) < 0)
                {
                    strcpy(g_pub_tx.reply,"A030");
                    sprintf(acErrMsg,"%s,%d,可用余额不足",__FILE__,__LINE__);
                    WRITEMSG
                    goto ErrExit;
                }*/
                
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
				vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
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
            strcpy(cName_tmp,sIn_mst.name);
            set_zd_data("0680","9");/** 内部帐户 **/
        }
        
        memset(sRz_br_no,0x00,sizeof(sRz_br_no));/*Added by YangMeng 2014/7/9 11:23:32*/
        memcpy(sRz_br_no,g_mdm_ac_rel.opn_br_no,sizeof(sRz_br_no));
        set_zd_data("0870",g_mdm_ac_rel.opn_br_no);
        vtcp_log("[%s],[%d],入账机构，opn_br_no=[%d],sRz_br_no=[%s]!",__FILE__,__LINE__,g_mdm_ac_rel.opn_br_no,sRz_br_no);
        
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
		vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
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
		vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);
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
	vtcp_log("[%s][%d],g_pub_tx.reply[%s]",__FILE__,__LINE__,g_pub_tx.reply);   

    /**　登记单笔记账登记薄　***/
    stChnlCnaps.tx_date = g_pub_tx.tx_date;    /** 系统日期  **/
    sprintf(stChnlCnaps.tx_time,"%6ld", g_pub_tx.tx_time);    /** 系统时间  **/
    /*stChnlCnaps.trace_no = g_pub_tx.ejfno;    ** 核心流水 非柜员流水号，登记的是核心交易流水号 Mod by CHENGBO 20110727**/
    stChnlCnaps.trace_no = g_pub_tx.trace_no;    /** 核心流水  **/
    memcpy(stChnlCnaps.cur_no,"01",2); /**  暂时写死为01人民币 ***/
    memcpy(stChnlCnaps.ac_no, cAc_no1,sizeof( stChnlCnaps.ac_no)-1);    /** 主账号 **/
    if(cDc_ind[0]=='1')
    {
        memcpy(stChnlCnaps.ac_no1, cAc_no1,sizeof( stChnlCnaps.ac_no)-1);    /** 付款账号 **/
        memcpy(stChnlCnaps.ac_no2, cAc_no2,sizeof( stChnlCnaps.ac_no)-1);    /** 收款账号 **/
    }
    else if(cDc_ind[0]=='2')
    {
        memcpy(stChnlCnaps.ac_no1, cAc_no2,sizeof( stChnlCnaps.ac_no)-1);    /** 付款账号 **/
        memcpy(stChnlCnaps.ac_no2, cAc_no1,sizeof( stChnlCnaps.ac_no)-1);    /** 收款账号 **/
    }
    memcpy(stChnlCnaps.tx_code,g_pub_tx.tx_code,sizeof(stChnlCnaps.tx_code)-1);    /** 交易码 **/
    memcpy(stChnlCnaps.tx_br_no,g_pub_tx.tx_br_no,sizeof(stChnlCnaps.tx_br_no)-1);    /** 交易机构 **/
    memcpy(stChnlCnaps.opn_br_no,sRz_br_no,sizeof(stChnlCnaps.opn_br_no)-1);    /** 开户机构 **/
    memcpy(stChnlCnaps.tel,g_pub_tx.tel,sizeof(stChnlCnaps.tel)-1);    /** 交易柜员  **/
    memcpy(stChnlCnaps.bbmk,g_pub_tx.tty,sizeof(stChnlCnaps.bbmk)-1);    /** 系统种类 **/
    memcpy(stChnlCnaps.ttyn,g_pub_tx.tty,sizeof(stChnlCnaps.ttyn)-1);    /** 终端号 **/
    stChnlCnaps.pt_date = lPt_date;    /** 平台日期  **/
    sprintf(stChnlCnaps.pt_trace_no ,"%ld" ,lPt_trace_no); /** 平台流水 **/
    stChnlCnaps.tx_amt = dTx_amt ;    /** 交易金额 **/
    stChnlCnaps.ct_ind[0] = '2';    /** 现转标志  本交易只处理转账 **/
    memcpy(stChnlCnaps.brf,cBrf,sizeof(stChnlCnaps.brf)-1);    /** 记账摘要 **/
    stChnlCnaps.sts[0] = '1';    /** 记账状态初始为成功 **/
    stChnlCnaps.dz_ind[0] = '0';    /**  对账状态 **/
    stChnlCnaps.chnl_type[0] = cCtrlcmd[9]; /* 20120915 增加渠道标志 */
    iRet = Chnl_single_cnaps_Ins(stChnlCnaps,g_pub_tx.reply);
    if(iRet)
    {
    	Chnl_single_cnaps_Debug(&stChnlCnaps);
        sprintf(acErrMsg,"登记单笔记账登记薄出错!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "D107");
        goto ErrExit;
    }
    
    
    
GoodExit:
    sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
    WRITEMSG
    strcpy(g_pub_tx.reply,"0000");
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

/* 因为黄行银联手续费应付是内部账号，并且隶属于每个支行，
   平台发过来科目号不能使用，需要重新选择账号
*/
static int fd123_info_chg()
{
	char cFd123[1024];
	char cFee_flag[8];
	char cFee_cnt[8];
	char cAc_no[32];
	char cAc_no_2[32];
	char cMain_ac_no[32];
	char *str = NULL;
	int iCnt = 0;
	int i = 0;
	int j = 0;
	struct mdm_ac_rel_c stMdm_ac_rel;

	memset(cFd123,'\0',sizeof(cFd123));
	memset(cFee_flag,'\0',sizeof(cFee_flag));
	memset(cMain_ac_no,'\0',sizeof(cMain_ac_no));
	memset(&stMdm_ac_rel,'\0',sizeof(struct mdm_ac_rel_c));

	get_zd_data("0710", cFee_flag);
	get_fd_data("1230", cFd123);
	get_fd_data("0300", cMain_ac_no);

	if (cFee_flag[0] == '0')
	{
		return 0;
	}

	memcpy(cFee_cnt,cFd123+1,2);
	iCnt = atoi(cFee_cnt);

	for (i = 0; i < iCnt; i++)
	{
		memset(cAc_no, '\0', sizeof(cAc_no));
		memcpy(cAc_no, cFd123+3 + 48 * i, 24);
		str = cFd123+3 + 48 * i;
		zip_space(cAc_no);
		if (strcmp(cAc_no, "CUPSFEEIN") == 0)
		{
			int iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&stMdm_ac_rel,
				" ac_no='%s'", cMain_ac_no);
			if (iRet)
			{
				vtcp_log("%s,%d,ac_no=[%s]查找错误!",
					__FILE__,__LINE__,cMain_ac_no);
				strcpy(g_pub_tx.reply,"P102");
				return -1;
			}
			memset(cAc_no, '\0', sizeof(cAc_no));
			pub_base_GetParm_Str(stMdm_ac_rel.opn_br_no, 44,
				cAc_no);
			vtcp_log("%s,%d,found ac_no=[%s]",__FILE__,__LINE__,cAc_no);
			memset(cAc_no_2, '\0', sizeof(cAc_no_2));
			sprintf(cAc_no_2, "%-24s", cAc_no);
			for (j = 0; j < 24; j++)
			{
				str[j] = cAc_no_2[j];
			}
		}
	}
	set_zd_data("1230",cFd123);
	return 0;
}

