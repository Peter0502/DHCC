/***********************************************************************************************
*程序名称：spB383.c
*作者：lwb
*时间：20141119
*功能：实现承兑汇票到期联机处理的银行垫款
************************************************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "com_parm_c.h"
#define EXTERN

extern char *pubf_acct_acctoact(char * ,char *);


int spB383()
{
	int ret = 0;
	char cPact_no[20];
	char cAcNo[20];
	char cif_no_tmp[9];
	double basic_rate = 0.00;
	struct mo_bank_acc_po_c sMobankaccpo;
	struct com_parm_c sCom_parm;
	struct dd_mst_c sDdmst;
	char cMdm_ac_no[20];
	double tx_amt_tmp=0.00;
	
	memset(cPact_no,0x00,sizeof(cPact_no));
	memset(cAcNo,0x00,sizeof(cAcNo));
	memset(cif_no_tmp,0x00,sizeof(cif_no_tmp));
	memset(&sMobankaccpo,0x00,sizeof(struct mo_bank_acc_po_c));
	memset(&sCom_parm,0x00,sizeof(struct com_parm_c));
	memset(&sDdmst,0x00,sizeof(sDdmst));
	memset(cMdm_ac_no,0x00,sizeof(cMdm_ac_no));
	

	pub_base_sysinit();
	
	get_zd_double("1037",&g_pub_tx.tx_amt1);
	get_zd_data("1039",cPact_no);
	tx_amt_tmp=g_pub_tx.tx_amt1;
	vtcp_log("tx_amt1=%lf,cPact_no = %s",g_pub_tx.tx_amt1,cPact_no);
  vtcp_log("[%s][%d] tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
  ret = pub_base_CompDblVal( g_pub_tx.tx_amt1,0.00 );
  if( ret > 0 )
  {
	/* 修改银行汇票基本信息登记簿 */
      ret = Mo_bank_acc_po_Dec_Upd(g_pub_tx.reply,"pact_no = '%s'",cPact_no);
      if(ret)
      {
    	  sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
        WRITEMSG
        goto ErrExit;	
      }
      ret = Mo_bank_acc_po_Fet_Upd(&sMobankaccpo,g_pub_tx.reply);
      if(ret == 100 )
      {
      	sprintf( acErrMsg, "修改承兑汇票基本信息登记薄中无此协议号[%s]", cPact_no );
        WRITEMSG
        goto ErrExit;		
      }
      else if(ret)
      if(ret)
      {
    	  sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
        WRITEMSG
        goto ErrExit;	
      }      
      /* 第一部分：借：13602银行垫款 */
       ret = pub_base_GetParm_Str( "CDDK", 1, g_pub_tx.prdt_code );
            if( ret )
            {
                sprintf( acErrMsg, "取承兑垫款产品编号错!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B225" );
                goto ErrExit;
            }
						g_pub_tx.mach_date=21000101; /**到期日期**/
						strcpy(g_pub_tx.name,sMobankaccpo.payer_name); /**户名**/
            strcpy(g_pub_tx.prt_ind,"0");
						strcpy(g_pub_tx.brf,"内部帐开户");
						strcpy(g_pub_tx.ac_wrk_ind,"000");
						strcpy(g_pub_tx.add_ind,"1");
						strcpy(g_pub_tx.ct_ind,"2");
						strcpy(g_pub_tx.prt_ind,"0");
						
  					g_pub_tx.tx_amt1=0.00;
						/***开介质***/
						ret=pub_acct_opn_in_mdm();
						if( ret )
						{
							sprintf(acErrMsg,"开介质错误![%d]",ret);
							WRITEMSG
							goto ErrExit;
						}
  					
						strcpy(g_pub_tx.ac_wrk_ind,"011");
						/***开户***/
						ret=pub_acct_opn_ac();
						if( ret )
						{
							sprintf(acErrMsg,"开账号错误![%d]",ret);
							WRITEMSG
							goto ErrExit;
						}
						vtcp_log("开的账号为ac_no=【%s】，%s,%ld ",g_pub_tx.ac_no,__FILE__,__LINE__);
      g_pub_tx.ac_id=0;
      g_pub_tx.ac_seqn=0;
      /******
      g_pub_tx.tx_amt1 = sMobankaccpo.sign_amt - dTempAmt2;
      ******/
      /*g_pub_tx.tx_amt1 = sum_po_amt - dTempAmt2;*/
      /*vtcp_log("qqqqqqqqqqq[%s]",pubf_acct_acctoact("13602",g_pub_tx.tx_br_no));*/
      /*sprintf(cMdm_ac_no,"%s",pubf_acct_acctoact("13602",g_pub_tx.tx_br_no));*/
      /*cMdm_ac_no = pubf_acct_acctoact("13602",g_pub_tx.tx_br_no);*/
      vtcp_log("g_pub_tx.ac_no=%s",g_pub_tx.ac_no);
      /*strcpy( g_pub_tx.ac_no,cMdm_ac_no);lpj20151127*/

      
     
      strcpy( g_pub_tx.cur_no, "01" );
      strcpy( g_pub_tx.ct_ind, "2" );
      strcpy( g_pub_tx.brf, "借承兑垫款" );

     /* strcpy( g_pub_tx.ac_id_type,"9" ); *//*账户类型为内部*/
      strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
      /*strcpy( g_pub_tx.ac_wrk_ind,"000" ); *不核对凭证号，零金额不计流水、明细*/
      strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
      strcpy( g_pub_tx.end_note_no, g_pub_tx.beg_note_no);
      strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
      g_pub_tx.svc_ind=9001;      /*内部帐存取*/
      strcpy(g_pub_tx.prt_ind,"0");
      strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
      /** 记交易流水 **/
      strcpy( g_pub_tx.sub_tx_code, "A016" );
      strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
      strcpy(g_pub_tx.add_ind,"0");    /*借方标志*/
      g_pub_tx.tx_amt1=tx_amt_tmp;
      ret = pub_acct_trance();
      if( ret )
      {
          sprintf(acErrMsg,"调用存取款主控失败!");
          WRITEMSG
          goto ErrExit;
      }
      /* 取款方－会计记帐 */
      set_zd_data( "1204", "01" );
      set_zd_double( "1208", g_pub_tx.tx_amt1 );
      strcpy( g_pub_tx.sub_tx_code, "A016" );
        vtcp_log("[%s][%d] tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);

      /* 取承兑垫款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) */
      ret = pub_base_GetParm_Str( "CDDK", 1, g_pub_tx.prdt_code );
      if( ret )
      {
          sprintf( acErrMsg, "取承兑垫款产品编号错!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "B225" );
          goto ErrExit;
      }
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if (ret != 0)
      {
          sprintf(acErrMsg,"会计记帐不成功!!");
          WRITEMSG
          goto ErrExit;
      }
        vtcp_log("[%s][%d] tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
      
        vtcp_log("[%s][%d] tx_amt1=[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
      sMobankaccpo.advance_amt = g_pub_tx.tx_amt1;    /* 银行垫款额 */
     

      /* 第二部分：银行垫款同时开逾期贷款户 * 
       * 取承兑逾期贷款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) * 
       *ret = pub_base_GetParm_Str( "CDYQ", 1, g_pub_tx.prdt_code );
      if( ret )
      {
          sprintf( acErrMsg, "取承兑垫款转逾期贷款产品编号错!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "B265" );
          goto ErrExit;
      }
      
      ret = Ln_parm_Sel( g_pub_tx.reply , &g_ln_parm , "prdt_no='%s' " , g_pub_tx.prdt_code );
      if(ret)    
      {
          sprintf(acErrMsg,"取贷款参数信息失败!");
          WRITEMSG
          goto ErrExit;    
      }
      *20151126*
      *    为以后需要检查还款账户状态时备用
      ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
                  sMobankaccpo.payer_ac_no);
      if( ret == 100 )
      {
          sprintf( acErrMsg, "付款人账号不存在![%s]", sMobankaccpo.payer_ac_no );
          WRITEMSG
          strcpy( g_pub_tx.reply, "D164" );
          goto ErrExit;
      }else if( ret )
      {
          sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
          WRITEMSG
          goto ErrExit;
      }
      
      ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
              sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
      if( ret == 100 )
      {
          sprintf( acErrMsg, "付款人账号不是活期账号!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "B179" );
          goto ErrExit;
      }else if( ret )
      {
          sprintf( acErrMsg, "执行Dd_mst_Sel错误![%d]", ret );
          WRITEMSG
          goto ErrExit;
      }
      
      if( sDdMst.ac_sts[0] == '*' )
      {
          sprintf( acErrMsg, "此付款人账号已销户!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "B231" );
          goto ErrExit;
      }
      *

      * 截取出票人账号的前八位，客户号 *
     * sscanf(sMobankaccpo.payer_ac_no,"%8s",cif_no_tmp);
      g_pub_tx.cif_no = atoi(cif_no_tmp);

      sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
      WRITEMSG
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG
      g_ln_mst.opn_date = g_pub_tx.tx_date;        **20151126*  * 开户日期 *       
      *strcpy( g_ln_mst.pact_no, sMobankaccpo.pact_no );  **20151126*  * 合同号:承兑汇票协议编号 *   
      ******
      g_ln_mst.amt_lmt = sMobankaccpo.sign_amt - dTempAmt2;    *** 计划发放额，贷款限额:银行垫款额 *
      
      *g_ln_mst.amt_lmt = g_pub_tx.tx_amt1;
      
      g_ln_mst.rate=sMobankaccpo.over_rate;
      g_ln_mst.over_rate = sMobankaccpo.over_rate;   **20151126*     * 逾期利率:录入时输入的罚息利率 *
      *g_ln_mst.fine_rate = sMobankaccpo.over_rate; **20151126*       * 罚息利率:录入时输入的罚息利率 *
      *strcpy( g_pub_tx.cur_no, "01");              **20151126*  * 币种 *
      *strcpy( g_ln_mst.repay_type, g_ln_parm.ln_pay_type ); **20151126*   * 贷款支付方式 *    
      *g_ln_mst.mtr_date = g_pub_tx.tx_date;         **20151126*   * 到期日期:逾期当天日期 *
      *strcpy( g_pub_tx.intst_type, g_ln_parm.intst_type ); **20151126*   * 计息类型 *    
      *strcpy( g_pub_tx.name, sMobankaccpo.payer_name ); **20151126*   * 贷款户名:承兑汇票出票人户名 *        
      *strcpy( g_ln_mst.cmpd_intst_ind,g_ln_parm.cmpd_intst_ind);
      g_ln_mst.cif_no=g_pub_tx.cif_no;
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG  **20151126*          
      *ac_seqn_tmp = ;                         账号序号 *
      *strcpy( g_pub_tx.ac_no1, sMobankaccpo.payer_ac_no );     还款账号:承兑出票人账号，如果此账号有变，则在贷款还款计划中维护 *
      
      * 取还款账号ID，为以后需要检查还款账户状态时备用    
      pub_base_strpack(g_pub_tx.ac_no1);
      if( strlen(g_pub_tx.ac_no1) )
      {
          ret = pub_ln_AcCheck( g_pub_tx.ac_no1,ac_seqn_tmp,
              &cif_no_tmp,&g_ln_mst.repay_ac_id );
          if( ret )
          {
              sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
              WRITEMSG
              return 1;
          }    
      }*
      
     * ret = pub_base_getllz( g_ln_parm.rate_no, g_ln_parm.cur_no, g_pub_tx.tx_date , &basic_rate );
      if( ret )
      {
          sprintf(acErrMsg,"调用根据利率编号取得利率函数错误!");
          WRITEMSG
          return -1;    
      }
      *//*basic_rate = basic_rate/1.2;*//**20151126** 转换成月利率 *
      *sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG    **20151126*        
      * 利率浮动比例计算 */
      /*g_ln_mst.over_rate_flt = pub_base_PubDround_1( g_ln_mst.over_rate*100/basic_rate);
      g_ln_mst.fine_rate_flt = pub_base_PubDround_1( g_ln_mst.fine_rate*100/basic_rate);*/
      /*
      strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
      strcpy ( g_pub_tx.ac_id_type , "3" );   *  *20151126*   * 账号类型 3-贷款 *
      *strcpy ( g_pub_tx.add_ind , "1" );   **20151126*     * 增减1加 *    
      *strcpy ( g_pub_tx.ct_ind , "2" );    **20151126*    * 现转:1现2转 *    
      *strcpy ( g_pub_tx.prt_ind , "0" );    **20151126*    * 是否打印存折(有折无折标志)0否 *
      *strcpy ( g_pub_tx.hst_ind , "1" );     **20151126*   * 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
      *strcpy( g_pub_tx.brf,"贷款开户开介质" );        
      
      TRACE
      sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
      WRITEMSG
      g_pub_tx.ac_id =0 ;**20151126*
      ***生成账号***
      if( pub_acct_opn_cif_mdm() )
      {
          sprintf(acErrMsg,"生成账号错误!");
          WRITEMSG
          goto ErrExit;
      }
      sprintf(acErrMsg,"贷款账号[%s]",g_pub_tx.ac_no);
      WRITEMSG            
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG
      
      strcpy(cAcNo, g_pub_tx.ac_no);            * 登记承兑汇票基本信息登记簿用 *
			TRACE     
      * Pub_tx结构赋值 *
      strcpy ( g_pub_tx.ac_wrk_ind,"0110" );
      strcpy ( g_pub_tx.ac_id_type , "3" );        * 账号类型 3-贷款 *
      strcpy ( g_pub_tx.add_ind , "1" );        * 增减1加 *    
      strcpy ( g_pub_tx.ct_ind , "2" );        * 现转:1现2转 *    
      strcpy ( g_pub_tx.prt_ind , "0" );        * 是否打印存折(有折无折标志)0否 *
      strcpy ( g_pub_tx.hst_ind , "1" );        * 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
      g_pub_tx.svc_ind = 3001;
      strcpy( g_pub_tx.brf,"贷款开户" );
      TRACE
      sprintf( acErrMsg, "cif_no=[%ld]", g_pub_tx.cif_no );
      WRITEMSG
      sprintf( acErrMsg, "ac_id=[%ld]", g_pub_tx.ac_id );
      WRITEMSG
      g_pub_tx.tx_amt1 = 0.00;
      ***生成账户ID***
      if( pub_acct_opn_ac() )
      {
          sprintf(acErrMsg,"生成账号ID错误!");
          WRITEMSG
          goto ErrExit;
      }
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG
      * 第三部分：逾期贷款放款，借：13001逾期贷款 *
      sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
      WRITEMSG
      
      g_pub_tx.ac_seqn = g_ln_mst.ac_seqn;    * 贷款账户序号 *
      sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
      WRITEMSG    
      
      g_pub_tx.tx_amt1 = g_ln_mst.amt_lmt;    * 发生额 *
      strcpy( g_pub_tx.ct_ind, "2");                        * 现转标志 *
      sprintf(acErrMsg,"现转标志![%s]",g_pub_tx.ct_ind);
      WRITEMSG
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG            
      ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
          &g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); *读取贷款户信息*
      if( ret ) {return ret;}
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG            
      strcpy ( g_pub_tx.add_ind , "1" );        * 增减:0减1加2销户 *    
      
      * 是否打印存折(有折无折标志):1是0否 *
      strcpy ( g_pub_tx.prt_ind , "0" );    
      * 入明细帐标志:1日间入2日终单笔入3日终汇总入 *
      strcpy ( g_pub_tx.hst_ind , "1" );
      g_pub_tx.svc_ind = 3010 ;
      strcpy( g_pub_tx.brf,"承兑逾期贷款放款" );
      
      sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
      WRITEMSG
      sprintf( acErrMsg, "ln_bal=[%lf]", g_ln_mst.bal );
      WRITEMSG            
      ** 交易记帐处理 ***
      strcpy( g_pub_tx.sub_tx_code, "L104" );
      if( pub_acct_trance() )
      {
          sprintf(acErrMsg,"交易记帐处理错误!");
          WRITEMSG
          goto ErrExit;
      }
      sprintf(acErrMsg,"交易记帐处理 PASS!");
      WRITEMSG
      
      * 登记贷款登记薄 *
      ret = pub_ln_ln_reg( g_ln_mst,g_ln_mst.opn_date,g_ln_mst.mtr_date,"1",g_ln_mst.bal,0.00,"0",0 );    
      if( ret )
      {
          sprintf(acErrMsg,"登记贷款登记薄失败");
          WRITEMSG
          strcpy(g_pub_tx.reply,"L082");
          goto ErrExit;
      }                        
      sprintf(acErrMsg,"登记贷款登记薄 PASS!");
      WRITEMSG
      
      set_zd_data("0210",g_ln_parm.cur_no);* 对币种赋值，记会计账时在dc_entry中用到 *
      
      * 借130－会计记帐 *
      set_zd_data( "0210", "01" );    
      set_zd_double( "0430", g_pub_tx.tx_amt1 );
      strcpy( g_pub_tx.sub_tx_code, "L104" );
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if (ret != 0)
      {
          sprintf(acErrMsg,"会计记帐不成功!!");
          WRITEMSG
          goto ErrExit;
      }

      * 第四部分：贷：13602承兑垫款 *
      g_pub_tx.ac_id=0;    
      g_pub_tx.ac_seqn=0;
      *strcpy( g_pub_tx.ac_no, pubf_acct_acctoact("13602",g_pub_tx.tx_br_no) ); *   * 承兑垫款 *
      sprintf(cMdm_ac_no,"%s",pubf_acct_acctoact("13602",g_pub_tx.tx_br_no));
      *strcpy(cMdm_ac_no , pubf_acct_acctoact("13602",g_pub_tx.tx_br_no));*
      strcpy( g_pub_tx.ac_no,cMdm_ac_no);
      strcpy( g_pub_tx.cur_no, "01" );
      strcpy( g_pub_tx.ct_ind, "2" );
      strcpy( g_pub_tx.brf, "贷承兑垫款" );
  
      strcpy( g_pub_tx.ac_id_type,"9" ); *账户类型为内部*
      strcpy( g_pub_tx.ac_get_ind,"00" ); *本程序未读取分户*
      *strcpy( g_pub_tx.ac_wrk_ind,"000" ); *不核对凭证号，零金额不计流水、明细*
      strcpy(g_pub_tx.ac_wrk_ind,"000001001"); 
      strcpy( g_pub_tx.beg_note_no, cPact_no );
      strcpy( g_pub_tx.end_note_no,g_pub_tx.beg_note_no);
      strcpy(g_pub_tx.add_ind,"1");*贷方标志*
      strcpy( g_pub_tx.prt_ind,"0" );*不登折*
      g_pub_tx.svc_ind=9001;  *内部帐存取*
      strcpy(g_pub_tx.prt_ind,"0");
      strcpy(g_pub_tx.hst_ind,"1"); *日间入明细*
      ** 记交易流水 **
      strcpy( g_pub_tx.sub_tx_code, "A017" );
      ret = pub_acct_trance();
      if( ret )
      {
          sprintf(acErrMsg,"调用存取款主控失败!");
          WRITEMSG
          goto ErrExit;
      }
      * 贷方－会计记帐 *
      set_zd_data( "1214", "01" );
      set_zd_double( "1218", g_pub_tx.tx_amt1 );
      strcpy( g_pub_tx.sub_tx_code, "A017" );
  
      * 取承兑垫款产品编号:到参数表com_parm中取得(用pubf_base_GetParm.c中的函数) *
      ret = pub_base_GetParm_Str( "CDDK", 1, g_pub_tx.prdt_code );
      if( ret )
      {
          sprintf( acErrMsg, "取承兑垫款产品编号错!" );
          WRITEMSG
          strcpy( g_pub_tx.reply, "B225" );
          goto ErrExit;
      }
      ret = pubf_acct_proc( g_pub_tx.sub_tx_code );
      if (ret != 0)
      {
          sprintf(acErrMsg,"会计记帐不成功!!");
          WRITEMSG
          goto ErrExit;
      }lpj20151127*/
  		strcpy( sMobankaccpo.over_ac_no,cAcNo);
　　　ret = Mo_bank_acc_po_Upd_Upd( sMobankaccpo,g_pub_tx.reply );
　　　if ( ret )
　　　{
　　　	sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
　　　	WRITEMSG
　　　　goto ErrExit;
　　　}
	set_zd_data("0300",cAcNo);
	set_zd_double("0400",g_pub_tx.tx_amt1);
 	Mo_bank_acc_po_Clo_Upd();
  
  }
	　	
 
 
OkExit:
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	if (memcmp(g_pub_tx.reply,"0000",4)==0)
		memcpy(g_pub_tx.reply,"B380",4);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
      
}
