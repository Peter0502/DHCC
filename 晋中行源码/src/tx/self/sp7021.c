/*******************************************************************************
* 文 件 名:  sp7021.c
* 功能描述： 联机单笔现金记账交易(二代支付专用接口)
*            本交易用于行内帐户/科目联机单笔现金记账。
* 作  者:  chenchao
* 完成日期： 2011-7-12 13:57:59
*
* 修改记录： 
* 日    期:20110816
* 修 改 人：xueke
* 修改内容:
	pub_com_test.c()控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
*
insert into TX_DEF  values ('7021', '联机单笔现金记账', '10000000000000000000000000000100000000010000010000000000000001000100000000000000110000000000000000000000000000000000000000000100', '1', '3', '0');

insert into TX_SUB_DEF  values ('7021', '联机单笔现金记账交易', 'sp7021', '0', '0');

insert into TX_FLOW_DEF values ('7021', 0, '7021', '#=0001@1@#=0002@3@#=0003@5@#=0004@6@#$');
insert into TX_FLOW_DEF values ('7021', 1, 'D003', '#=0000@2@#$');
insert into TX_FLOW_DEF values ('7021', 2, 'M002', '#=0000@7@#$');
insert into TX_FLOW_DEF values ('7021', 3, 'D099', '#=0000@4@#$');
insert into TX_FLOW_DEF values ('7021', 4, 'M001', '#=0000@7@#$');
insert into TX_FLOW_DEF values ('7021', 5, 'A016', '#=0000@2@#$');
insert into TX_FLOW_DEF values ('7021', 6, 'A017', '#=0000@4@#$');
insert into TX_FLOW_DEF values ('7021', 7, 'F059', '#=0000@8@#$');
insert into TX_FLOW_DEF values ('7021', 8, 'F000', '#$');

insert into TX_SUB_REL  values ('7021', 'A016', '0', '0300', '1201', '账号');
insert into TX_SUB_REL  values ('7021', 'A016', '1', '01', '1204', '币种');
insert into TX_SUB_REL  values ('7021', 'A016', '1', '1', '1205', '转账');
insert into TX_SUB_REL  values ('7021', 'A016', '0', '0400', '1208', '金额');
insert into TX_SUB_REL  values ('7021', 'A016', '0', '0810', '120A', '摘要');
insert into TX_SUB_REL  values ('7021', 'A017', '0', '0300', '1211', '账号');
insert into TX_SUB_REL  values ('7021', 'A017', '1', '1', '1215', '现转标志');
insert into TX_SUB_REL  values ('7021', 'A017', '0', '0400', '1218', '金额');
insert into TX_SUB_REL  values ('7021', 'A017', '0', '0810', '121A', '摘要');
insert into TX_SUB_REL  values ('7021', 'A017', '1', '01', '1214', '币种');
insert into TX_SUB_REL  values ('7021', 'D003', '0', '0300', '1021', '账号');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '0000', '0192', '控制标志');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '1', '0191', '控制标志');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '1', '1022', '账号');
insert into TX_SUB_REL  values ('7021', 'D003', '0', '0400', '102F', '金额');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '01', '102J', '币种');
insert into TX_SUB_REL  values ('7021', 'D003', '1', '1', '102K', '现转标志');
insert into TX_SUB_REL  values ('7021', 'D003', '0', '0810', '102Y', '摘要');
insert into TX_SUB_REL  values ('7021', 'D099', '0', '0300', '1011', '账号');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '01', '101A', '币种');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '1', '101B', '现转');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '0000', '0192', '控制标志');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '1', '0191', '控制标志');
insert into TX_SUB_REL  values ('7021', 'D099', '0', '0810', '1016', '摘要');
insert into TX_SUB_REL  values ('7021', 'D099', '1', '1', '1012', '序号');
insert into TX_SUB_REL  values ('7021', 'D099', '0', '0400', '1013', '金额');
insert into TX_SUB_REL  values ('7021', 'M001', '0', '0400', '1184', '金额');
insert into TX_SUB_REL  values ('7021', 'M001', '0', '0300', '1188', '对方账号');
insert into TX_SUB_REL  values ('7021', 'M001', '1', '01', '1181', '对方账号');
insert into TX_SUB_REL  values ('7021', 'M001', '0', '0810', '1187', '摘要');
insert into TX_SUB_REL  values ('7021', 'M002', '0', '0400', '1194', '金额');
insert into TX_SUB_REL  values ('7021', 'M002', '1', '01', '1191', '币种');
insert into TX_SUB_REL  values ('7021', 'M002', '0', '0810', '1197', '摘要');
insert into TX_SUB_REL  values ('7021', 'M002', '0', '0300', '1198', '对方账号');

*******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#define EXTERN
#include "public.h"
#include "chnl_single_cnaps_c.h"
#include "dd_mst_c.h"
#include "com_item_c.h"
#include "dd_parm_c.h"
#include "in_mst_c.h"
#include "prdt_ac_id_c.h"


int sp7021()
{
  int i=0;
  int iRet = 0;
  char  cAc_name[61]; /** 户名 **/
  char  tmpstr[41];
  char  cFee_flag[2]; /*收费标志*/
  char  cCk_flag[2];  /*存款标志1-存款确认*/
  char  cPt_old_trace[13]; /*原平台流水*/
  char  cFd123[1000]; /*接收123域的收费内容*/
  long  lFee_cnt=0;     /*收费条数*/
  char  cFee_cnt[3];    /*收费条数*/
  char  cFee_acno[25];  /*费用账号或科目*/
  char  cFee_amt[13];   /*收费金额*/
  char  cAc_no[25]; /** 主账号 **/
  char  cDc_ind[2]; /** 借贷标志 1借2贷 **/
  char  cFee_dc_ind[2]; /*费用的借贷标志*/
  char  cMsr2[38];  /** 二磁道信息 **/
  char  cMsr3[105]; /** 三磁道信息 **/
  char  cPwd[17]; /** 客户密码 **/
  char  cBrf[51]; /** 记账摘要 **/
  char  cGz_flag[2];/**挂账标志**/
  double  dTx_amt = 0.00; /** 交易金额 **/
  double  dFee_amt = 0.00; /*手续费金额*/
  double  dFee_amt1=0.00;
  double  dBal=0.00;
  double  dCif_fee=0.00;  /*应收客户手续费*/
  double  dKy_amt = 0.00;/*可用金额*/
  char  cCert_no[21]; /** 证件类型(1位)+号码(19位) 证件类型：1：身份证；2：户口簿；
          3：护照；4：军人证；5：回乡证；6：士兵证；7：港澳居民来往通行证
          8：企业代码证；9：经营许可证；A: 营业执照；B: 事业法人证书；
          C：工商核准号；D：其他（对公）；E：临时身份证；F：外国人居留证；
          G：警官证；H：其他证件（对私）；F：台湾同胞来往通行证**/
  char  cCtrlcmd[11]; /**控制标志 本标志适用于新平台所有交易 73域接收 每个字节代表一种控制或检查
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
  char  cDc_entry[601]; /** (除主帐号和现金外)其它会计分录
          开户机构,帐号/科目,借贷标志，金额+回车
          开户机构=END 标识结束
          ******************/
  long  lPt_date=0;
  long  lPt_trace_no = 0;
  long	lPt_old_trace=0;  /*原平台流水 long型*/
  long	lPt_old_date=0;   /*原平台交易日期*/
  
  struct  chnl_single_cnaps_c  stChnlCnaps;
  struct  dd_mst_c    sDd_mst;
  struct  com_item_c    sCom_item;
  struct  dd_parm_c   sDd_parm;
  struct    prdt_ac_id_c    sPrdt_ac_id;
  struct    in_mst_c        sIn_mst;
  /** 初始化各变量 **/
  char	cFd125[282] = {0};/*Added by YangMeng 20140703 登记交易关联变量定义*/
  char	cHx_term[5] = {0};/*Added by YangMeng 20140703 终端号 COMM-通用 HVPS-大额 BEPS-小额*/
  
  memset(cFee_flag, 0x00 ,sizeof(cFee_flag));
  memset(cFd123,0x00,sizeof(cFd123));
  memset(cPwd, 0x00, sizeof(cPwd));
  memset(cBrf, 0x00, sizeof(cBrf));
  memset(cMsr2, 0x00, sizeof(cMsr2));
  memset(cMsr3, 0x00, sizeof(cMsr3));
  memset(tmpstr, 0x00, sizeof(tmpstr));
  memset(cAc_no, 0x00, sizeof(cAc_no));
  memset(cDc_ind, 0x00, sizeof(cDc_ind));
  memset(cFee_dc_ind,0x00,sizeof(cFee_dc_ind));
  memset(cGz_flag,0x00,sizeof(cGz_flag));
  memset(cCk_flag,0x00,sizeof(cCk_flag));
  memset(cAc_name, 0x00, sizeof(cAc_name));
  memset(cCert_no, 0x00, sizeof(cCert_no));
  memset(cCtrlcmd, 0x00, sizeof(cCtrlcmd));
  memset(cDc_entry, 0x00, sizeof(cDc_entry));
  memset(cFee_amt,0x00,sizeof(cFee_amt));
  memset(cFee_cnt,0x00,sizeof(cFee_cnt));
  memset(cFee_acno,0x00,sizeof(cFee_acno));
  memset(cPt_old_trace,0x00,sizeof(cPt_old_trace));
  memset(&sDd_mst,0x00,sizeof(sDd_mst));
  memset(&sCom_item,0x00,sizeof(sCom_item));
  memset(&sDd_parm,0x00,sizeof(sDd_parm));
  memset(&sPrdt_ac_id,0x00,sizeof(sPrdt_ac_id));
  memset(&sIn_mst,0x00,sizeof(sIn_mst));
  memset(&stChnlCnaps,0x00,sizeof(struct  chnl_single_cnaps_c));
  
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
  get_zd_data("0250", cAc_name );
  get_zd_data("0300", cAc_no   );
  get_zd_data("0620", cCert_no);
  get_zd_data("0660", cDc_ind);
  get_zd_data("0700", cCk_flag);
  get_zd_data("0710", cFee_flag);
  get_zd_data("0730", cCtrlcmd);
  get_zd_data("0750", cMsr2);
  get_zd_data("0760", cMsr3);
  /**
  get_zd_data("0790", cPwd);
  **/
  get_zd_data("0810", cBrf);
  get_zd_data("0950", cDc_entry);
  get_fd_data("1230", cFd123);
  get_zd_double("0400", &dTx_amt);
  
  vtcp_log("%s,%d,startsp7021",__FILE__,__LINE__);
  
	/**Added by YangMeng 20140703 渠道交易增加相应的业务信息(对方账号、户名等)begin*/
	memset(cFd125,0x00,sizeof(cFd125));
	memset(cHx_term,0x00,sizeof(cHx_term));
	get_zd_data("0100",cHx_term);
	vtcp_log("[%s],[%d],cHx_term=[%s]",__FILE__,__LINE__,cHx_term);
	get_fd_data("1250",cFd125);
	if(!memcmp(cHx_term,"HVPS",4) || !memcmp(cHx_term,"BEPS",4))
	{
		vtcp_log("[%s][%d],cFd125=[%s]",__FILE__,__LINE__,cFd125);
		iRet=pubf_com_checkin_txdetail(cFd125);
		if (iRet)
		{
		  sprintf(acErrMsg,"%s,%d,执行渠道交易增加相应业务信息函数失败",__FILE__,__LINE__,iRet);
		  WRITEMSG
		  goto ErrExit;	
		}
	}
	/**Added by YangMeng 20140703 渠道交易增加相应的业务信息(对方账号、户名等)end*/
	
  if( cCk_flag[0] == '1' )  /*存款确认*/
  {
     get_zd_long("0450", &lPt_old_date);
     get_zd_long("0780", &lPt_old_trace);
     memset(cPt_old_trace,0x00,sizeof(cPt_old_trace));
     sprintf(cPt_old_trace,"%ld",lPt_old_trace);	/*原平台流水由long转char*/ 
     iRet=Chnl_single_cnaps_Sel(g_pub_tx.reply,&stChnlCnaps,"pt_date=%ld and pt_trace_no='%s' and sts='1'",lPt_old_date,cPt_old_trace);
     if( iRet == 0)
     {
          /*已经记账成功*/
          vtcp_log("[%s][%d] 重复交易",__FILE__,__LINE__);
          /* mod by sunhongwei 2013-05-09 存款确认重复交易要返回银联成功
          strcpy(g_pub_tx.reply,"CU94");
          */
          strcpy(g_pub_tx.reply, "CU00");
          set_zd_long("0040",stChnlCnaps.trace_no);
          set_zd_long("0460",stChnlCnaps.tx_date);
          goto ErrExit;
     }
  }
  zip_space(cAc_no);
  /*计算手续费*/
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
             /**
             memcpy(cFee_amt,cFd123+27 + 46 * i,12);
             **/
             memcpy(cFee_acno,cFd123+3 + 48 * i,24);
             zip_space(cFee_acno);
             memcpy(cFee_amt,cFd123 + 29 + 48 * i,12);
             memcpy(cFee_dc_ind,cFd123 + 27 + 48 * i,1);
             vtcp_log("[%s][%d],cFee_amt=[%s]",__FILE__,__LINE__,cFee_amt);
             vtcp_log("[%s][%d],cFee_dc_ind =[%s]",__FILE__,__LINE__,cFee_dc_ind);
             dFee_amt1=atof(cFee_amt) / 100.00; /**modify 20110822: 金额以分为单位**/
             vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
             if(cFee_dc_ind[0] == '1')
             {
                if(strcmp(cAc_no,cFee_acno)==0)
                {
                    dCif_fee = dCif_fee + dFee_amt1;
                }
                dFee_amt=dFee_amt + dFee_amt1;
             }
        }
        vtcp_log("[%s][%d],dFee_amt=[%lf]",__FILE__,__LINE__,dFee_amt);
        stChnlCnaps.fee_amt=dCif_fee;      /*手续费金额*/             
  }
  vtcp_log("[%s][%d]应收客户手续费=[%lf]",__FILE__,__LINE__,dCif_fee);
  pub_base_old_acno(cAc_no);  /**新旧账号转换**/
  memcpy(g_pub_tx.ac_no,cAc_no,sizeof(g_pub_tx.ac_no)-1);
  zip_space(g_pub_tx.ac_no);
  if( strlen ( g_pub_tx.ac_no ) >10) /*** 此种情况下,只能是账号 ***/
  {
        iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",cAc_no);
        if (iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
            strcpy(g_pub_tx.reply,"P102");
            goto ErrExit;
        }
        iRet=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id," ac_id = %ld ",g_mdm_ac_rel.ac_id);
        if(iRet)
        {
            vtcp_log("%s,%d,ac_no[%s]查询prdt_ac_rel错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
            strcpy(g_pub_tx.reply,"K106");
            goto ErrExit;
        }
        if(sPrdt_ac_id.ac_id_type[0] == '1')
        {
            iRet = Dd_mst_Sel(g_pub_tx.reply,&sDd_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            iRet = Dd_parm_Sel(g_pub_tx.reply,&sDd_parm,"prdt_no='%s'",sDd_mst.prdt_no);
            if(iRet)
            {
                vtcp_log("%s,%d, 查找活期产品错误!",__FILE__,__LINE__);
                strcpy(g_pub_tx.reply,"W015");
                goto ErrExit;
            }
            vtcp_log("%s,%d,ac_no[%s]该账户正常!",__FILE__,__LINE__,g_pub_tx.ac_no);
            /*73域控制标志检查*/
            iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no,"","","2",cGz_flag);
            vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cGz_flag);
            if (iRet)
             {
                sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
                WRITEMSG
                goto ErrExit;	
             }
            set_zd_data("0680","1");/** 活期帐户 **/         
            /*** 开始配置记帐要素 **/
            if(cDc_ind[0] == '1')
            {
                /*账户取款时 检查账户的状态*/
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
                 /*检查可用余额*/
                if(pub_base_CompDblVal(sDd_mst.hold_amt,sDd_parm.min_bal) >= 0)
                {
                    dKy_amt = sDd_mst.bal -sDd_mst.ctl_amt -sDd_mst.hold_amt;
                }
                else
                {
                    dKy_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_parm.min_bal + sDd_mst.hold_amt;
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
                dBal=sDd_mst.bal - dTx_amt;
                dKy_amt=dBal - sDd_mst.ctl_amt - sDd_mst.hold_amt; 
                memcpy(tmpstr,"1001156C",8);
                sprintf(tmpstr+8,"%012.0f",dBal*100);
                memcpy(tmpstr+20,"1002156C",8);
                sprintf(tmpstr+28,"%012.0f",dKy_amt*100);
                vtcp_log("%s,%d,查询到的余额是：tmpstr=[%s]",__FILE__,__LINE__,tmpstr);
                set_zd_data("0820",tmpstr);
                strcpy(g_pub_tx.reply, "0001");
            }
            else if(cDc_ind[0] == '2')
            {
                /*存款后返回余额*/
                if(pub_base_CompDblVal(sDd_mst.hold_amt,sDd_parm.min_bal) >= 0)
                {
                    dKy_amt = sDd_mst.bal -sDd_mst.ctl_amt -sDd_mst.hold_amt;
                }
                else
                {
                    dKy_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_parm.min_bal + sDd_mst.hold_amt;
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
                
                strcpy(g_pub_tx.reply, "0002");
            }else
            {
              sprintf(acErrMsg,"借贷标志错误!!!");
              WRITEMSG
              strcpy(g_pub_tx.reply, "P404");
              goto ErrExit;
            }
        }
        else if( sPrdt_ac_id.ac_id_type[0] == '9')
        {
            iRet = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%ld",g_mdm_ac_rel.ac_id);
            if (iRet)
            {
                vtcp_log("%s,%d,ac_no[%s]查找错误!",__FILE__,__LINE__,g_pub_tx.ac_no);
                strcpy(g_pub_tx.reply,"P102");
                goto ErrExit;
            }
            if(sIn_mst.sts[0] != '1')
            {
                vtcp_log("%s,%d,ac_no[%s]该账户不正常!",__FILE__,__LINE__,g_pub_tx.ac_no);
                strcpy(g_pub_tx.reply,"O085");
                goto ErrExit;
            }
            set_zd_data("0680","9");/** 内部帐户 **/
            /*** 开始配置记帐要素 **/
            if(cDc_ind[0] == '1')
            {
               strcpy(g_pub_tx.reply, "0003");
            }else if(cDc_ind[0] == '2')
            {
               strcpy(g_pub_tx.reply, "0004");
            }else
            {
                sprintf(acErrMsg,"借贷标志错误!!!"); 
                WRITEMSG
                strcpy(g_pub_tx.reply, "P404");
                goto ErrExit;
            }
        } 
  }
  else
  {/**** 否则是科目  **/
      iRet = Com_item_Sel(g_pub_tx.reply,&sCom_item,"acc_no = '%s'",g_pub_tx.ac_no);
      if(iRet)
      {
        sprintf(acErrMsg,"查询科目失败!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply, "B172");
        goto ErrExit;
      }
       /*73域控制标志检查*/
      iRet=pub_com_test(cCtrlcmd,cCert_no,cAc_name,cAc_no,"","","2",cGz_flag);
      vtcp_log("%s,%d,cCtrlcmd=[%s],cCert_no=[%s],cAc_name=[%s],cAc_no=[%s],cGz_flag=[%s]",__FILE__,__LINE__,cCtrlcmd,cCert_no,cAc_name,cAc_no,cGz_flag);
      if (iRet)
       {
           sprintf(acErrMsg,"%s,%d,公共函数pub_com_test()调用失败",__FILE__,__LINE__);
           WRITEMSG
           goto ErrExit;	
       }
       /*** 开始配置记帐要素 **/
      if(cDc_ind[0] == '1')
      {
         strcpy(g_pub_tx.reply, "0003");
      }else if(cDc_ind[0] == '2')
      {
         strcpy(g_pub_tx.reply, "0004");
      }else
      {
          sprintf(acErrMsg,"借贷标志错误!!!"); 
          WRITEMSG
          strcpy(g_pub_tx.reply, "P404");
          goto ErrExit;
      }
  }
  
  
   /**　登记单笔记账登记薄　***/
   
   stChnlCnaps.chnl_type[0] = cCtrlcmd[9]; /* 20120915 zws 增加渠道标志 */
   vtcp_log("[%s][%d],渠道标识=[%s]",__FILE__,__LINE__,stChnlCnaps.chnl_type);
   
   memcpy(stChnlCnaps.bbmk,g_pub_tx.tty,sizeof(stChnlCnaps.bbmk)-1); /** 系统种类 **/
   vtcp_log("[%s][%d],系统种类=[%s]",__FILE__,__LINE__,stChnlCnaps.bbmk);
   
   stChnlCnaps.tx_date = g_pub_tx.tx_date; /** 系统日期  **/
   vtcp_log("[%s][%d],核心日期=[%d]",__FILE__,__LINE__,stChnlCnaps.tx_date);
   sprintf(stChnlCnaps.tx_time,"%6ld", g_pub_tx.tx_time);  /** 系统时间  **/
 
   stChnlCnaps.trace_no=g_pub_tx.trace_no;
   vtcp_log("[%s][%d],核心流水=[%d]",__FILE__,__LINE__,stChnlCnaps.trace_no);
   
   memcpy(stChnlCnaps.cur_no,"01",2); /**  暂时写死为01人民币 ***/
   memcpy(stChnlCnaps.ac_no, g_pub_tx.ac_no,sizeof( stChnlCnaps.ac_no)-1); /** 主账号 **/
   memcpy(stChnlCnaps.tx_code,g_pub_tx.tx_code,sizeof(stChnlCnaps.tx_code)-1); /** 交易码 **/
   memcpy(stChnlCnaps.tx_br_no,g_pub_tx.tx_br_no,sizeof(stChnlCnaps.tx_br_no)-1);  /** 交易机构 **/
   memcpy(stChnlCnaps.opn_br_no,stChnlCnaps.opn_br_no,sizeof(stChnlCnaps.opn_br_no)-1);  /** 开户机构 **/
   memcpy(stChnlCnaps.tel,g_pub_tx.tel,sizeof(stChnlCnaps.tel)-1); /** 交易柜员  **/
   memcpy(stChnlCnaps.ttyn,g_pub_tx.tty,sizeof(stChnlCnaps.ttyn)-1); /** 终端号 **/
   
   stChnlCnaps.pt_date = lPt_date; /** 平台日期  **/
   vtcp_log("[%s][%d],平台日期=[%d]",__FILE__,__LINE__,stChnlCnaps.pt_date);
   
   sprintf(stChnlCnaps.pt_trace_no ,"%ld" ,lPt_trace_no); /** 平台流水 **/
   vtcp_log("[%s][%d],平台流水=[%s]",__FILE__,__LINE__,stChnlCnaps.pt_trace_no);
  
   stChnlCnaps.tx_amt = dTx_amt ;  /** 交易金额 **/
   vtcp_log("[%s][%d],交易金额=[%.2f]",__FILE__,__LINE__,stChnlCnaps.tx_amt);
   
   stChnlCnaps.fee_amt = dFee_amt1 ;  /** 交易金额 **/
   vtcp_log("[%s][%d],手续费=[%.2f]",__FILE__,__LINE__,dFee_amt1);
    
   stChnlCnaps.ct_ind[0] = '1';  /** 现转标志  本交易只处理现金 **/
   stChnlCnaps.sts[0] = '1'; /** 记账状态初始为成功 **/
   stChnlCnaps.dz_ind[0] = '0';  /**  对账状态 **/
   
   memcpy(stChnlCnaps.brf,cBrf,sizeof(stChnlCnaps.brf)-1); /** 记账摘要 **/
   
   Chnl_single_cnaps_Debug(&stChnlCnaps);
   iRet = Chnl_single_cnaps_Ins(stChnlCnaps);
   if(iRet)
   {
     sprintf(acErrMsg,"登记单笔记账登记薄出错!!!");
     WRITEMSG
     strcpy(g_pub_tx.reply, "D107");
     goto ErrExit;
   }

GoodExit:
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


