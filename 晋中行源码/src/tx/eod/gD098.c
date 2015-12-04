/*************************************************
* 文 件 名: gD008.c
* 功能描述：财税库业务日终批量代扣
*
* 作    者: jack
* 完成日期: 2003年10月16日
*
* 修改日期:               
* 修 改 人: xxxx     
* 修改内容: 
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#define  EXTERN
#define  cIdtp "1"
#include "public.h"
#include <errno.h>
#include "com_branch_c.h"
#include "rsp_code_c.h"
#include "mdm_ac_rel_c.h"
#include "dd_mst_c.h"
#include "mq_tax_head_c.h"
#include "mq_tax_payment_c.h"
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
gD098()
{
  int ret = 0;
  int ret1 = 0;
  char tmp_acct[21]; /* 临时存款帐户 */
  char cReturn[51];
  char execstr[248];
  
  int iErr_flag = 0;  /*  0－处理正确；1－处理失败  */
  
  long cif_no = 0;
  char xs_acno[20];
  char jz_acno[20];
  int  jz_ac_seq = 0;
  
  long   lSnum = 0;      /*成功笔数*/
  long   lFnum = 0;      /*失败笔数*/
  double dSamt = 0.00;   /*成功金额*/
  double dFamt = 0.00;   /*失败金额*/
  
  char filename[248];
  char filename1[248];
  char filename3[248];
  FILE *fp = NULL;
  FILE *fp1 = NULL;
  FILE *fp3 = NULL;
  
  char filename01[248];
  char filename11[248];
  char filename31[248];
  FILE *fp01 = NULL;
  FILE *fp11 = NULL;
  FILE *fp31 = NULL;
  
  int i=0;
  
  long lTrace_no = 0;
  
  char tmpstr[512];

  struct mq_tax_head_c     sMq_tax_head;
  struct mq_tax_payment_c  sMq_tax_payment;
  struct dd_mst_c          sDd_mst;
  struct mdm_ac_rel_c     sMdm_ac_rel ;
  struct com_branch_c      sCom_branch;
  struct rsp_code_c        sRsp_code;
  struct ag_unitinfo_c      sAg_unitinfo;
  struct ag_peritemmap_c sAg_peritemmap;
  struct pay_agent_c sPay_agent;

  /**------- 初始化全局变量 --------**/
  pub_base_sysinit();
  memset(&sAg_unitinfo,0x00,sizeof(sAg_unitinfo));
  memset(&sAg_peritemmap,0x00,sizeof(sAg_peritemmap));
  memset(&sPay_agent,0x00,sizeof(sPay_agent));

  /*---代发要当天，代扣要在代扣区间内---*/
  ret = Mq_tax_head_Dec_Sel_hold( g_pub_tx.reply , "sts = '0' \
    and ( entrust_date <= %ld and entrust_date+return_term >= %ld ) )", \
    g_pub_tx.tx_date, g_pub_tx.tx_date );
  if ( ret )
  {
    sprintf(acErrMsg,"执行Mq_tx_head_Dec_Sel出错[%d]", ret );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
  }

  while(1)
  {
    memset( &sMq_tax_head, 0x0, sizeof(sMq_tax_head) );
    lSnum = 0;     /*成功笔数*/
    lFnum = 0;     /*失败笔数*/
    dSamt = 0.00;  /*成功金额*/
    dFamt = 0.00;  /*失败金额*/

    ret = Mq_tax_head_Fet_Sel( &sMq_tax_head, g_pub_tx.reply );
    if ( ret == 100 )
    {
      sql_begin();
      break;
    }
    if ( ret )
    {
      sprintf(acErrMsg,"执行Mq_tax_head_Fet_Sel出错[%d]", ret );
      WRITEMSG
      goto ErrExit;
    }
    
    pub_base_strpack( sAg_unitinfo.pay_type );
  
    /*--- 一个税款包起一个事务 ---*/
    sql_begin();
    
    /* 去掉头及尾部的空格 */
    pub_base_strpack( sMq_tax_head.taxorg_code );
    pub_base_strpack( sMq_tax_head.payee_acct );
    
    /*--------------  检查单位账号状态  ---------------*/
    memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
    ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
      "ac_no = '%s'", sMq_tax_head.payee_acct  );
    if ( ret )
    {
      sprintf(acErrMsg,"查询介质账号对照表失败");
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
      sql_rollback();
      return 1;
    }    

    /* 取临时帐户 */
    strcpy(tmp_acct, sMq_tax_head.payee_acct);
    
    memset( &sDd_mst, 0x00, sizeof(sDd_mst) );
    ret = Dd_mst_Sel( g_pub_tx.reply , &sDd_mst, "ac_id = %ld \
      and ac_seqn = 1", sMdm_ac_rel.ac_id );    
    if ( ret )
    {
      sprintf( acErrMsg,"查询活期主文件失败[%d][%ld][%s]",
		ret, sMdm_ac_rel.ac_id, sAg_unitinfo.bank_acct_no );
      WRITEMSG
      set_zd_data(DC_REPLY,g_pub_tx.reply);
      sql_rollback();
      return 1;
    }
    
    if ( sMdm_ac_rel.note_sts[0] != '0' || sDd_mst.ac_sts[0] != '1' \
    || sDd_mst.hold_sts[0] == '1' )  /*销户和完全冻结*/
    {
      sprintf(acErrMsg,"账户状态不正常[%s]", sAg_unitinfo.bank_acct_no );
      WRITEMSG
      sql_rollback();
      continue;  /*跳过开始下一个单位*/
    }        
    
    /*------------ 处理结果文件 -----------*/
    memset(filename,0x0,sizeof(filename));
    sprintf(filename,"%s/report/%s/BR0%s.txt",\
	getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
    fp = fopen( filename,"w" );  /*该文件用来记录明细处理正确结果*/
    if( fp==NULL )
    {
      sprintf(acErrMsg," open file [%s] error ",filename );
      WRITEMSG
      strcpy( g_pub_tx.reply, "S047" );
      set_zd_data(DC_REPLY,g_pub_tx.reply);
         return 1;
    }
    
    fprintf( fp, "                                 [%s][%s]批量代理业务成功记录\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
    fprintf( fp, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━┯━━━━┑\n" );
        fprintf( fp, "│%20s│%20s│%16s│%20s│%20s│%8s│\n", "代理编号", "客户姓名", "开户金额", "身份证号码", "账号", "处理结果" );
        fprintf( fp, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━┿━━━━┥\n" );
          
    memset( filename1, 0x0, sizeof(filename1) );
    sprintf( filename1, "%s/report/%s/BR1%s.txt", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
    fp1 = fopen( filename1, "w" );/*该文件用来记录明细错误处理结果*/
    if( fp1==NULL )
    {
      sprintf(acErrMsg," open file [%s] error ",filename1 );
      WRITEMSG
      strcpy( g_pub_tx.reply,"S047" );
      fclose( fp );
      set_zd_data(DC_REPLY,g_pub_tx.reply);
         return 1;
    }
          
    fprintf( fp1, "                                [%s][%s]批量代理业务失败记录\n\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
    fprintf( fp1, "┍━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━┑\n" );
        fprintf( fp1, "│%20s│%20s│%16s│%20s│%50s│\n", "代理编号", "客户姓名", "金额", "身份证号码", "处理结果" );
        fprintf( fp1, "┝━━━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━┿━━━━━━━━━━┿━━━━━━━━━━━━━━━━━━━━━━━━━┥\n" );
          
          lTrace_no = 0;
          pub_base_PubQlsh_Trans( &lTrace_no );

    ret = Mq_tax_payment_Dec_Sel_hold( g_pub_tx.reply , "src = '%s' \
      and msg_id = 's' and sts = '0'", sMq_tax_head.src, sMq_tax_head.msg_id );
    if ( ret )
    {
      sprintf(acErrMsg,"执行Pay_agent_Dec_Sel出错[%d]", ret );
      WRITEMSG
      fclose( fp );
      fclose( fp1 );
      set_zd_data(DC_REPLY,g_pub_tx.reply);
         return 1;
    }    
    
    while(1)
    {      
      iErr_flag = 0;
      
      memset( &sMq_tax_payment, 0x0, sizeof(sMq_tax_payment) );
      ret = Mq_tax_payment_Fet_Sel( &sMq_tax_payment , g_pub_tx.reply );
      if ( ret == 100 )
      {
    /* 建立事务 */
        sql_begin();
        break;
      }
      if ( ret )
      {
        sprintf(acErrMsg,"执行Pay_agent_Fet_Sel出错[%d]", ret );
        WRITEMSG
        goto ErrExit;
      }

      /**----------- 每次循环都重新初始化全局变量 -------------**/
      memset( &g_pub_tx, 0x00, sizeof(g_pub_tx) );
      pub_base_sysinit();  
      
      g_pub_tx.trace_no = lTrace_no;  /*重新赋值为生成的流水号*/
      set_zd_long( "0040", lTrace_no );
          
      memset( cReturn, 0x0, sizeof(cReturn) );
      memset( xs_acno,0x0,sizeof(xs_acno) );
      
      /*---交易机构为代理单位建立机构---*/
      strcpy( g_pub_tx.tx_br_no, sAg_unitinfo.opn_br_no );
      set_zd_data( DC_TX_BR_NO,  sAg_unitinfo.opn_br_no );
      
      memset( tmpstr, 0x0, sizeof(tmpstr) );
      get_zd_data( "0030", tmpstr );
      sprintf(acErrMsg,"批量处理交易机构为[%s][%s]", g_pub_tx.tx_br_no, tmpstr );
      WRITEMSG
    
      ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
        "ac_no = '%s'", sMq_tax_payment.pay_acct );
      if ( ret)
      {
         sprintf( cReturn,  "查找账号[%s]信息错", sMq_tax_payment.pay_acct );
         sprintf( acErrMsg, "查找账号[%s]信息错", sMq_tax_payment.pay_acct );
         WRITEMSG
         iErr_flag = 1;
         goto TrLog;
      }
      else {
        memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
        ret1 = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
           "ac_id = %ld", sMdm_ac_rel.ac_id );
        if ( ret1 )
        {
          sprintf(cReturn ,"查找活期账号[%s]信息错", sMq_tax_payment.pay_acct);
           sprintf(acErrMsg,"查找活期账号[%s]信息错", sMq_tax_payment.pay_acct);
          WRITEMSG
          iErr_flag = 1;
          goto TrLog;
        }
          
        pub_base_strpack( sDd_mst.ac_sts );
        /*销户和完全冻结*/
        if ( sDd_mst.ac_sts[0] != '1' || sDd_mst.hold_sts[0] == '1' )
        {
          strcpy( cReturn, "帐户状态错" );
          sprintf(acErrMsg,"帐户状态错[%s]", sMdm_ac_rel.ac_no);
          WRITEMSG
          iErr_flag = 1;
          goto TrLog;
        }
      }
      sprintf(acErrMsg,"------开始个人活期取款------------");
      WRITEMSG
    
      strcpy( g_pub_tx.ac_no, sAg_peritemmap.ac_no );
      g_pub_tx.ac_seqn = 1;
    
      g_pub_tx.tx_amt1 = sPay_agent.tram;
    
      strcpy(g_pub_tx.add_ind,"0");  /*-- 代扣 --*/
      memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
      strcpy( g_pub_tx.sub_tx_code, "D003" );
      set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      strcpy( g_pub_tx.prt_ind, "0" );
      set_zd_data( "102O", "0" );
      strcpy( g_pub_tx.prt_ind, "0" );
      set_zd_data( "102O", "0" );
      set_zd_data( "102J", "01" );
      set_zd_data( "102K", "2" );
      set_zd_double( "102F", g_pub_tx.tx_amt1 );
      set_zd_double( "102P", 0.00 );
      set_zd_double( "102Q", 0.00 );
   
      strcpy(g_pub_tx.brf, "代扣税款");
      strcpy(g_pub_tx.ct_ind,"2");
      strcpy(g_pub_tx.ac_wrk_ind,"001001001");
      strcpy(g_pub_tx.hst_ind,"1");
      g_pub_tx.svc_ind=1001;

      sprintf(acErrMsg,"BEG D003[%s][%s][%d]",\
    g_pub_tx.ac_no,g_pub_tx.ac_wrk_ind,g_pub_tx.svc_ind);
      WRITEMSG
      
      sql_commit();
      sql_begin();

      ret = pub_acct_trance();
      if( ret != 0 )
      {
        if ( sAg_unitinfo.opr_attri[0] == '0' )
        {
          sql_rollback();
        }
        sprintf( cReturn, "%s", g_pub_tx.reply );
        sprintf(acErrMsg,"调用存取款主控失败[%s]", g_pub_tx.reply );
        WRITEMSG
        iErr_flag = 1;
        goto TrLog;
      }
      
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if ( ret )
      {
        sql_rollback();
        sprintf( cReturn, "%s", g_pub_tx.reply );
        sprintf(acErrMsg,"调用存取款主控失败[%s]", g_pub_tx.reply );
        WRITEMSG
        iErr_flag = 1;
        goto TrLog;
      }
      
TrLog:    
      pub_base_strpack( sPay_agent.name );
      if ( iErr_flag == 0)
      {
        lSnum = lSnum + 1;
        dSamt = dSamt + sPay_agent.tram;

        fprintf( fp,"│%20s│%20s│%16.2lf│%20s│%20s│%8s│\n", sPay_agent.item_no, sPay_agent.name, \
          sPay_agent.tram, sPay_agent.id_no, sPay_agent.ac_no, cReturn );
        
        sql_commit();
      }
      else
      {
        memset( &sRsp_code, 0x0, sizeof(sRsp_code) );
        ret1 = Rsp_code_Sel( g_pub_tx.reply , &sRsp_code, "code = '%s'", cReturn );
        if ( ret1 == 0 )
        {
          memset( cReturn, 0x0, sizeof(cReturn) );
          pub_base_strpack( sRsp_code.memo );
          strcpy( cReturn, sRsp_code.memo );
        }
        
        lFnum = lFnum + 1;
        dFamt = dFamt + sPay_agent.tram;
        fprintf( fp1,"│%20s│%20s│%16.2lf│%20s│%50s│\n", sPay_agent.item_no, sPay_agent.name, \
          sPay_agent.tram, sPay_agent.id_no, cReturn );
      }
    }
    Pay_agent_Clo_Sel();

    fprintf( fp, "┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━┷━━━━┙\n" );
    fprintf( fp1,"┕━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━━━┙\n" );

    fclose(fp);
    fclose(fp1);
    
    if ( lFnum > 0 )  /*有失败记录*/
    {
      memset( filename11, 0x0, sizeof(filename11) );
      sprintf( filename11, "%s/report/%s/BR1%s.name", getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
      fp11 = fopen( filename11, "w" );/*该文件用来记录明细错误处理结果*/
      if( fp11==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ", filename11 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        fclose( fp );
        set_zd_data(DC_REPLY,g_pub_tx.reply);
      }
      
      fprintf( fp11, "BR1%s.txt 代理业务失败明细清单\n", sAg_unitinfo.unit_no );
      
      fclose( fp11 );
    }
    
    if ( lSnum > 0 )  /*有成功记录*/
    {
      sprintf(acErrMsg,"----------开始代缴单位账号存款-----------");
      WRITEMSG
      
      g_pub_tx.ac_id = 0;   /*test clean */
      g_pub_tx.ac_seqn = 0;
      strcpy(g_pub_tx.beg_note_no,"");
            
      strcpy( g_pub_tx.ac_no, tmp_acct );
      g_pub_tx.tx_amt1 = dSamt;
      
      if ( sAg_unitinfo.bank_acct_no[0] == '9' )  /*-- 内部账 --*/
      {
        strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
        strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
        g_pub_tx.svc_ind=9001;
        
        strcpy(g_pub_tx.add_ind,"1");  /*-- 代缴 --*/
        memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
        strcpy( g_pub_tx.sub_tx_code, "A017" );
        set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
      
        set_zd_data( "1214", "01" );
        set_zd_data( "1215", "2" );
        set_zd_double( "1218", g_pub_tx.tx_amt1 );
          
        strcpy(g_pub_tx.brf, "代收税款");
      }
      else
      {
        strcpy( g_pub_tx.ac_id_type,"1" ); /*账户类型为活期*/
        strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
        g_pub_tx.svc_ind=1001;
              	
        strcpy(g_pub_tx.add_ind,"1");  /*-- 代缴 --*/
          
        memset( g_pub_tx.sub_tx_code, 0x0, sizeof(g_pub_tx.sub_tx_code) );
        strcpy( g_pub_tx.sub_tx_code, "D099" );
        set_zd_data( DC_SUB_TX_CODE, g_pub_tx.sub_tx_code );
          
        strcpy( g_pub_tx.prt_ind, "0" );
        set_zd_data( "1017", "0" );
            
        set_zd_data( "101A", "01" );
        set_zd_data( "101B", "2" );
        set_zd_double( "1013", g_pub_tx.tx_amt1 );    			
          
        strcpy(g_pub_tx.brf, "代收税款");
      }    
      
      strcpy(g_pub_tx.ct_ind,"2");
      strcpy(g_pub_tx.ac_wrk_ind,"001001001");
      strcpy(g_pub_tx.hst_ind,"1");
      
      ret=pub_acct_trance();
      if( ret )
      {
        sprintf( cReturn, "记代理单位账出错[%s]", g_pub_tx.reply );
        sprintf(acErrMsg,"调用存取款主控失败[%s]", g_pub_tx.reply );
        WRITEMSG
        sql_rollback();
        continue;
      }
      
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if ( ret )
      {
        sprintf( cReturn, "记代理单位账出错[%s]", g_pub_tx.reply );
        sprintf( acErrMsg, "调用存取款主控失败[%s]", g_pub_tx.reply );
        WRITEMSG
        sql_rollback();
        continue;
      }  
                  
      g_pub_tx.ac_id = 0;
      g_pub_tx.ac_seqn = 0;
      g_pub_tx.tx_amt1 = 0.00;        /*发生额*/
      
      strcpy( g_pub_tx.ac_id_type,"0" ); /*账户类型设为活期*/      
      
      memset( filename01, 0x0, sizeof(filename01) );
      sprintf( filename01, "%s/report/%s/BR0%s.name",
		getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
    
      fp01 = fopen( filename01, "w" );
      if( fp01==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ", filename01 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        fclose( fp );
        set_zd_data(DC_REPLY,g_pub_tx.reply);
      }
      else
      {
        fprintf( fp01, "BR0%s.txt 代理业务成功明细清单\n",
		  sAg_unitinfo.unit_no );
      }      
      
      fclose( fp01 );
      
      memset( filename3, 0x0, sizeof(filename3) );
      sprintf( filename3, "%s/report/%s/BRDWHZ%s.txt",
		getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
      
      fp3 = fopen( filename3, "w" ); /*------正确处理结果汇总-------*/
      if( fp1==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ",filename3 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
      }
      else
      {
        memset( &sCom_branch, 0x0, sizeof(sCom_branch) );
        ret = Com_branch_Sel( g_pub_tx.reply, &sCom_branch, "br_no = '%s'", g_pub_tx.tx_br_no );
        
        pub_base_strpack( sCom_branch.br_name );
        pub_base_strpack( sAg_unitinfo.bank_acct_no );
        fprintf( fp3, "      	[%s]批量处理单位传票	\n", sAg_unitinfo.unit_name );
        fprintf( fp3, "机构码: %s          机构名称 : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
        fprintf( fp3, "单位代码: %s         单位名称: %s  \n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name );
        fprintf( fp3, "日期 : %ld       单位账号：%s \n", g_pub_tx.tx_date, sAg_unitinfo.bank_acct_no );
        
        fprintf( fp3, "┌────┬────────────────────┬────┬────────┐\n" );
        fprintf( fp3, "│单位编号│           单   位   名   称            │  人数  │    金   额     │\n" );
        fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
        fprintf( fp3, "│%8s│%40s│%8ld│%16.2lf│\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
        fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
        fprintf( fp3, "│ 合  计 │                                        │%8ld│%16.2lf│ \n", lSnum, dSamt );
        fprintf( fp3, "└────┴────────────────────┴────┴────────┘ \n\n\n\n\n\n\n" );
        
        fprintf( fp3, "      	[%s]批量处理个人汇总传票	\n", sAg_unitinfo.unit_name );
        fprintf( fp3, "机构码: %s          机构名称 : %s\n", g_pub_tx.tx_br_no, sCom_branch.br_name );
        fprintf( fp3, "日期 : %ld       汇总科目 ： \n", g_pub_tx.tx_date );
        
        fprintf( fp3, "┌────┬────────────────────┬────┬────────┐\n" );
        fprintf( fp3, "│单位编号│           单   位   名   称            │  人数  │    金   额     │\n" );
        fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
        fprintf( fp3, "│%8s│%40s│%8ld│%16.2lf│\n", sAg_unitinfo.unit_no, sAg_unitinfo.unit_name, lSnum, dSamt );
        fprintf( fp3, "├────┼────────────────────┼────┼────────┤\n" );
        fprintf( fp3, "│ 合  计 │                                        │%8ld│%16.2lf│ \n", lSnum, dSamt );
        fprintf( fp3, "└────┴────────────────────┴────┴────────┘ \n" );
      }      
      
      fclose( fp3 );  
      
      memset( filename31, 0x0, sizeof(filename31) );
      sprintf( filename31, "%s/report/%s/BRDWHZ%s.name",
		getenv("HOME"), sAg_unitinfo.opn_br_no, sAg_unitinfo.unit_no );
      
      fp31 = fopen( filename31, "w" );
      if( fp31==NULL )
      {
        sprintf(acErrMsg," open file [%s] error ", filename31 );
        WRITEMSG
        strcpy( g_pub_tx.reply,"S047" );
        fclose( fp );
        set_zd_data(DC_REPLY,g_pub_tx.reply);
      }
      else
      {
        fprintf( fp31, "BRDWHZ%s.txt 代理业务成功汇总清单\n",
        sAg_unitinfo.unit_no );
      }
      fclose( fp31 );
    }
    sql_commit();
    
    lSnum = 0;  /*成功笔数*/
    lFnum = 0;  /*失败笔数*/
    dSamt = 0.00;  /*成功金额*/
    dFamt = 0.00;  /*失败金额*/
  }
  Ag_unitinfo_Clo_Sel();  

OkExit:
  set_zd_data( "DC_TX_BR_NO", "99999" );
  sql_commit();
  strcpy( g_pub_tx.reply, "0000" );
  sprintf( acErrMsg, "日终批量代理业务处理成功完毕！" );
  WRITEMSG
  set_zd_data( DC_REPLY, g_pub_tx.reply );
  return 0;
ErrExit:
  set_zd_data( "DC_TX_BR_NO", "99999" );
  fclose(fp);
  fclose(fp1);
  Pay_agent_Clo_Sel();
  sql_rollback();
  sprintf(acErrMsg,"日终批量代理业务处理失败！[%s]", g_pub_tx.reply );
  WRITEMSG
  set_zd_data(DC_REPLY,g_pub_tx.reply);
  return 1;
}
