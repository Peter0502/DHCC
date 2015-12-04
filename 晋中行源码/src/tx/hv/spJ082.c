/***************************************************************
* 文 件 名:     spJ082.c
* 功能描述：    大额明细信息查询
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

spJ082()
{
	int ret = 0;

	FILE 	*fp;
	char 	cZf_type[2];
        char    cBuff[2001];
        char    cFilename[1000];
        char    cWherelist[200];

	HV_NOTPAY_AREA 		hv_notpayin;
        HV_FD123_AREA  		fd123;
        struct hv_zf_c  	hv_zf;
        struct hv_fzf_c 	hv_fzf;
        
	
	memset(cBuff,		0x00,	sizeof(cBuff));
	memset(cFilename,	0x00,	sizeof(cFilename));
	memset(cWherelist,	0x00,	sizeof(cWherelist));
	memset(cZf_type,	0x00,	sizeof(cZf_type));
	memset(&hv_notpayin,	0x00,	sizeof(hv_notpayin));
	memset(&fd123,		0x00,	sizeof(fd123));
	memset(&hv_zf,		0x00,	sizeof(hv_zf));
	memset(&hv_fzf,		0x00,	sizeof(hv_fzf));




        vtcp_log("[%s][%d]现在开始大额明细查询",__FILE__,__LINE__);


	/********************
	 * 1.初始化全局变量 *
	 ********************/

        pub_base_sysinit();



	 /********************
	  * 2.获取8583域内容 *
	  ********************/

        get_zd_data("0680",cZf_type);
         
        ret=iHvTistoNotpay(&hv_notpayin);
        if( ret )
        {
            WRITEMSG
            goto ErrExit;
        }


             
       /**组成查询条件**/
       if(cZf_type[0] == '0') /*支付类查询条件*/
       {
            MEMCPY_DEBUG(hv_zf.orderno,fd123.orderno,sizeof(hv_zf.orderno)-1);
            vtcp_log("[%s][%d]cZf_type=[%c]",__FILE__,__LINE__,cZf_type);
            sprintf(cWherelist,"br_no='%s' and wt_date='%s' and orderno='%s' and cmtno='%s'",cBr_no,cTx_date, hv_zf.orderno,cZftx_type);
   
       }
       else                   /* 非支付类查询条件 */
       {
           MEMCPY_DEBUG(hv_fzf.orderno,fd123.orderno,sizeof(hv_fzf.orderno)-1);
           vtcp_log("here2！[%s][%d]",__FILE__,__LINE__);
           vtcp_log("[%s][%d]cOtype[0]=[%c]",__FILE__,__LINE__,cOtype[0]);
           vtcp_log("+++++hv_fzf.orderno==[%s]!+++++",hv_fzf.orderno);  
           sprintf(cWherelist,"otype='%c' and br_no='%s' and wt_date='%s' and orderno='%s' and cmtno='%s'", cOtype[0],cBr_no,cTx_date,hv_fzf.orderno,cFzftx_type);

       }

       /***支付交易类型的转换***/
       iNum2TxType(hv_zf.cmtno,cZftx_type);

       /***非支付交易类型的转换***/
       iNum2TxType(hv_fzf.cmtno,cFzftx_type);
       
       vtcp_log("======大额明细查询开始[[%s][%d]",__FILE__,__LINE__);

       /***支付交易类的查询***/
       if(cZf_type[0] == '0')
       {
           char   tmpbuf[15] ;
           memset( tmpbuf, 0x00, sizeof(tmpbuf) );

           ret = Hv_zf_Sel(g_pub_tx.reply, &hv_zf, cWherelist);
           if ( ret==100 )
           {
               sprintf(acErrMsg,"无此大额支付交易");
               WRITEMSG
               goto    ErrExit;
           }
           else if(ret)
           {
               sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
               WRITEMSG
               goto ErrExit;

           }

           vtcp_log("=======支付类开始赋值[%s][%d]",__FILE__,__LINE__);

           /**cmtno号***/
           MEMCPY_DEBUG(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
           vtcp_log("[%s][%d] hv_zf.cmtno=[%s]",__FILE__,__LINE__,hv_zf.cmtno);
          
           /**原cmt号**/
           MEMCPY_DEBUG(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.o_cmtno));
           vtcp_log("[%s][%d] hv_zf.o_cmtno=[%s]",__FILE__,__LINE__,hv_zf.o_cmtno);

           /**发起清算行行号**/
           MEMCPY_DEBUG(fd123.pay_qs_no,hv_zf.pay_qs_no,sizeof(fd123.pay_qs_no));
           vtcp_log("[%s][%d] hv_zf.pay_qs_no=[%s]",__FILE__,__LINE__,hv_zf.pay_qs_no);


           /**发起行号 原发起行号 */
           MEMCPY_DEBUG(fd123.or_br_no,hv_zf.or_br_no,sizeof(fd123.or_br_no));
           vtcp_log("[%s][%d] hv_zf.or_br_no=[%s]",__FILE__,__LINE__,hv_zf.or_br_no);

           /**发报中心代码**/
           MEMCPY_DEBUG(fd123.sendco,hv_zf.sendco,sizeof(fd123.sendco));
           vtcp_log("[%s][%d] hv_zf.sendco=[%s]",__FILE__,__LINE__,hv_zf.sendco);

           /**收报中心代码**/
           MEMCPY_DEBUG(fd123.receco,hv_zf.receco,sizeof(fd123.receco));
           vtcp_log("[%s][%d] hv_zf.receco=[%s]",__FILE__,__LINE__,hv_zf.receco);

           /**付款人开户行**/
           MEMCPY_DEBUG(fd123.pay_opn_br_no   ,hv_zf.pay_opn_br_no   ,sizeof(fd123.pay_opn_br_no   ));
           vtcp_log("[%s][%d] hv_zf.pay_opn_br_no=[%s]",__FILE__,__LINE__,hv_zf.pay_opn_br_no);

           /**付款人帐号(原收款人帐号)**/
           MEMCPY_DEBUG(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
           vtcp_log("[%s][%d] hv_zf.pay_ac_no=[%s]",__FILE__,__LINE__,hv_zf.pay_ac_no);

           /**付款人名称(原收款人名称)**/
           MEMCPY_DEBUG(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));
           vtcp_log("[%s][%d] hv_zf.pay_name=[%s]",__FILE__,__LINE__,hv_zf.pay_name);

           /**付款人地址(原收款人地址)**/
           MEMCPY_DEBUG(fd123.pay_addr        ,hv_zf.pay_addr        ,sizeof(fd123.pay_addr        ));
           vtcp_log("[%s][%d] hv_zf.pay_addr=[%s]",__FILE__,__LINE__,hv_zf.pay_addr);

           /**接受清算行号**/
           MEMCPY_DEBUG(fd123.cash_qs_no      ,hv_zf.cash_qs_no      ,sizeof(fd123.cash_qs_no      ));
           vtcp_log("[%s][%d] hv_zf.cash_qs_no=[%s]",__FILE__,__LINE__,hv_zf.cash_qs_no);

           /**接受行号(原发起行号)**/
           MEMCPY_DEBUG(fd123.ac_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.ac_br_no        ));
           vtcp_log("[%s][%d] hv_zf.ac_br_no=[%s]",__FILE__,__LINE__,hv_zf.ac_br_no);

           /**收款人开户行号**/
           MEMCPY_DEBUG(fd123.cash_opn_br_no  ,hv_zf.cash_opn_br_no  ,sizeof(fd123.cash_opn_br_no  ));
           vtcp_log("[%s][%d] hv_zf.cash_opn_br_no=[%s]",__FILE__,__LINE__,hv_zf.cash_opn_br_no);

           /**收款人帐号,原付款人帐号**/
           MEMCPY_DEBUG(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
           vtcp_log("[%s][%d] hv_zf.cash_ac_no=[%s]",__FILE__,__LINE__,hv_zf.cash_ac_no);

           /**收款人姓名,原付款人姓名**/
           MEMCPY_DEBUG(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));
           vtcp_log("[%s][%d]hv_zf.cash_name=[%s]",__FILE__,__LINE__,hv_zf.cash_name);

           /**收款人地址**/
           MEMCPY_DEBUG(fd123.cash_addr       ,hv_zf.cash_addr       ,sizeof(fd123.cash_addr       ));
           vtcp_log("[%s][%d]hv_zf.cash_addr=[%s]",__FILE__,__LINE__,hv_zf.cash_addr);

           /**业务种类**/
           MEMCPY_DEBUG(fd123.yw_type         ,hv_zf.yw_type         ,sizeof(fd123.yw_type         ));
           vtcp_log("[%s][%d]hv_zf.yw_type=[%s]",__FILE__,__LINE__,hv_zf.yw_type);

           /*交易序号 撤销支付交易序号*/
           MEMCPY_DEBUG(fd123.orderno         ,hv_zf.orderno         ,sizeof(fd123.orderno         ));
           vtcp_log("[%s][%d]hv_zf.orderno=[%s]",__FILE__,__LINE__,hv_zf.orderno);

           /**附言**/
           MEMCPY_DEBUG(fd123.brf             ,hv_zf.hv_brf          ,sizeof(fd123.brf             ));
           vtcp_log("[%s][%d]hv_brf=[%s]",__FILE__,__LINE__,hv_zf.hv_brf);

           /**原支付交易序号**/
           MEMCPY_DEBUG(fd123.o_orderno       ,hv_zf.o_orderno       ,sizeof(fd123.o_orderno       ));
           vtcp_log("[%s][%d]hv_zf.o_orderno=[%s]",__FILE__,__LINE__,hv_zf.o_orderno);

           /**优先级**/
           fd123.operlevel[0]=hv_zf.operlevel[0];
           vtcp_log("[%s][%d]hv_zf.operlevel[0]=[%c]",__FILE__,__LINE__,hv_zf.operlevel[0]);

           /**交易状态**/
/**
           fd123.proc_sts[0]=hv_zf.hv_sts[0];
**/
           vtcp_log("[%s][%d]hv_zf.hv_sts[0]=[%c]",__FILE__,__LINE__,hv_zf.hv_sts[0]);
           get_statname(hv_zf.hv_sts[0],tmpbuf);
           vtcp_log("[%s][%d]tmp=[%s]",__FILE__,__LINE__,tmpbuf);
 
           pub_base_full_space((char*)&fd123,sizeof(fd123));
           set_zd_data("1230",(char*)&fd123);
           vtcp_log("fd123=[%s]",(char*)&fd123);
           set_zd_double("0390",hv_zf.tx_amt);         /* 交易金额 */
           vtcp_log("[%s][%d]hv_zf.tx_amt=[%f]",__FILE__,__LINE__,hv_zf.tx_amt);
              
 

           set_zd_data("0300",tmpbuf);          /* 状态中文*/
           set_zd_data("0580",hv_zf.note_no);          /* 凭证号 票据号码*/
           set_zd_data("1228",hv_zf.cur_no);           /* 货币符号 */
           set_zd_data("0890",hv_zf.note_type);        /* 凭证种类 */
           set_zd_data("1050",hv_zf.hv_brf);           /* 附言 60 */
           vtcp_log("%s,%d hv_zf.o_wt_date=[%d]",__FILE__,__LINE__,hv_zf.o_wt_date);
           set_zd_long("0450",hv_zf.o_wt_date);        /* 原委托日期 */
           if(!memcmp(hv_zf.cur_no,"RMB",3))
                       set_zd_data("0210","01");       /* 货币符号 */

           /**得到附加域并写入文件传回去**/
           if(memcmp(hv_zf.cmtno,HV_HD,sizeof(hv_zf.cmtno)-1) &&
              memcmp(hv_zf.cmtno,HV_GKZJHH,sizeof(hv_zf.cmtno)-1))
           {
                   FILE *fpw;
                   vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
                   iHvGetAddit(cTx_date,cTx_date,hv_zf.addid,"000",cBuff,sizeof(cBuff)-1);
                   vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
                   ret=pub_base_AllDwnFilName(cFilename);
                   if(ret)
                   {
                           vtcp_log("%s,%d 生成下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
                   }
                   fpw=fopen(cFilename,"w");
                   if(fpw==NULL)
                   {
                           vtcp_log("%s,%d 打开下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
                   }

                   if(!memcmp(hv_zf.cmtno,HV_CJ,sizeof(hv_zf.cmtno)-1))
                   {
                           HV_TYCJ_ADD hv_cj;
                           memset(&hv_cj, 0 , sizeof(hv_cj));
                           MEMCPY_DEBUG((char *)&hv_cj,cBuff, sizeof(hv_cj));
                           vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_cj);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_cj,sizeof(hv_cj));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                   }
                   if(!memcmp(hv_zf.cmtno,HV_WT,sizeof(hv_zf.cmtno)-1))
                   {
                           HV_WTSK_ADD hv_wt;
                           memset(&hv_wt, 0 , sizeof(hv_wt));
                           MEMCPY_DEBUG((char *)&hv_wt,cBuff, sizeof(hv_wt));
                           vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_wt);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_wt,sizeof(hv_wt));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                    }
                    if(!memcmp(hv_zf.cmtno,HV_TSCF,sizeof(hv_zf.cmtno)-1))
                    {
                           HV_TSCF_ADD hv_tscf;
                           memset(&hv_tscf, 0 , sizeof(hv_tscf));
                           MEMCPY_DEBUG((char *)&hv_tscf,cBuff, sizeof(hv_tscf));
                           vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_tscf);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_tscf,sizeof(hv_tscf));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                    } 
                    vtcp_log("cmtno=[%s][%s][%d]",hv_zf.cmtno,__FILE__,__LINE__);
                    if(!memcmp(hv_zf.cmtno,HV_HPYC,sizeof(hv_zf.cmtno)-1))
                    {
                           HV_121_ADD hv_hpyc;
                           memset(&hv_hpyc, 0 , sizeof(hv_hpyc));
                           MEMCPY_DEBUG((char *)&hv_hpyc,cBuff, sizeof(hv_hpyc));
                           vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_hpyc);
                           memset(cBuff, 0 , sizeof(cBuff));
                           MEMCPY_DEBUG(cBuff,(char *)&hv_hpyc,sizeof(hv_hpyc));
                           fprintf(fpw,"%s",cBuff);
                           set_zd_data( DC_FILE_SND,"2" );
                    }
                    fclose(fpw);
           }

       }
       else  /**非支付交易查询**/
       {
          ret =Hv_fzf_Sel(g_pub_tx.reply, &hv_fzf, cWherelist);
          if ( ret==100 )
          {
              sprintf(acErrMsg,"无此大额支付交易");
              WRITEMSG
              goto    ErrExit;
          }
          else if(ret)
          {
              sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
              WRITEMSG
              goto ErrExit;

          }

          vtcp_log("=======支付类开始赋值[%s][%d]",__FILE__,__LINE__);

          /**cmtno号码**/
          MEMCPY_DEBUG(fd123.cmtno      ,hv_fzf.cmtno           ,sizeof(fd123.cmtno));
          vtcp_log("[%s][%d]==hv_fzf.cmtno=[%s]",__FILE__,__LINE__,hv_fzf.cmtno);

          /**原o_cmtno号**/
          MEMCPY_DEBUG(fd123.o_cmtno    ,hv_fzf.o_cmtno ,sizeof(fd123.cmtno));
          vtcp_log("[%s][%d]==hv_fzf.cmtno=[%s]",__FILE__,__LINE__,hv_fzf.cmtno);

          /**发起行号 原发起行号 **/
          MEMCPY_DEBUG(fd123.or_br_no   ,hv_fzf.or_br_no,sizeof(fd123.or_br_no));
          vtcp_log("[%s][%d]==hv_fzf.or_br_no=[%s]",__FILE__,__LINE__,hv_fzf.or_br_no);

          /**付款清算行行号**/
          MEMCPY_DEBUG(fd123.pay_qs_no  ,hv_fzf.pay_qs_no,sizeof(fd123.pay_qs_no));
          vtcp_log("[%s][%d]==hv_fzf.pay_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);

          /**发报中心代码**/
          MEMCPY_DEBUG(fd123.sendco             ,hv_fzf.sendco  ,sizeof(fd123.sendco));
          vtcp_log("[%s][%d]==hv_fzf.sendco=[%s]",__FILE__,__LINE__,hv_fzf.sendco);

          /**收报中心代码**/
          MEMCPY_DEBUG(fd123.sendco,hv_fzf.receco  ,sizeof(fd123.receco));
          vtcp_log("[%s][%d]==hv_fzf.receco=[%s]",__FILE__,__LINE__,hv_fzf.receco);

          /**接受清算行号**/
          MEMCPY_DEBUG(fd123.cash_qs_no ,hv_fzf.cash_qs_no,sizeof(fd123.cash_qs_no));
          vtcp_log("[%s][%d]==hv_fzf.cash_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.cash_qs_no);

          /**接受行号(原发起行号)**/
          MEMCPY_DEBUG(fd123.ac_br_no   ,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
          vtcp_log("[%s][%d]==hv_fzf.ac_br_no=[%s]",__FILE__,__LINE__,hv_fzf.ac_br_no);

          /**交易序号 撤销支付交易序号**/
          MEMCPY_DEBUG(fd123.orderno    ,hv_fzf.orderno ,sizeof(fd123.orderno));
          vtcp_log("[%s][%d]==hv_fzf.orderno=[%s]",__FILE__,__LINE__,hv_fzf.orderno);

          /**附言**/
          MEMCPY_DEBUG(fd123.brf                ,hv_fzf.hv_brf  ,sizeof(fd123.brf));
          vtcp_log("[%s][%d]==hv_fzf.hv_brf=[%s]",__FILE__,__LINE__,hv_fzf.hv_brf);

          /**原支付交易序号**/
          MEMCPY_DEBUG(fd123.o_orderno  ,hv_fzf.o_orderno,sizeof(fd123.o_orderno));
          vtcp_log("[%s][%d]==hv_fzf.o_orderno=[%s]",__FILE__,__LINE__,hv_fzf.o_orderno);

          /**非支付交易状态**/
          fd123.proc_sts[0] = hv_fzf.hv_fzf_sts[0];
          vtcp_log("[%s][%d]==hv_fzf.hv_fzf_sts[0]=[%c]",__FILE__,__LINE__,hv_fzf.hv_fzf_sts[0]);

          /**来往表示**/
          fd123.lw_ind[0] = hv_fzf.lw_ind[0];
          vtcp_log("[%s][%d]==hv_fzf.lw_ind[0]=[%c]",__FILE__,__LINE__,hv_fzf.lw_ind[0]);

          /**应答状态**/
          fd123.resp[0]   = hv_fzf.res_sts[0];
          vtcp_log("[%s][%d]==hv_fzf.res_sts[0]=[%c]",__FILE__,__LINE__,hv_fzf.res_sts[0]);

          /**原交易金额**/
          dot_str2dbl(cTxamt ,sizeof(cTxamt),&hv_fzf.o_tx_amt);
          hv_fzf.o_tx_amt = hv_fzf.o_tx_amt/100;
          vtcp_log(" === 原交易金额 === hv_fzf.o_tx_amt=[%lf] ",hv_fzf.o_tx_amt);
          set_zd_double("0390", hv_fzf.o_tx_amt);   

          /* 原委托日期 */
          set_zd_long("0450",hv_fzf.o_wt_date);   

          /* 原交易日期 */
          set_zd_long("0470",hv_fzf.o_tx_date);
          
          /*原币种*/
          set_zd_data("1228",hv_fzf.o_cur_ind);   

          /*原交易种类*/
          set_zd_data("0710",hv_fzf.o_tx_type);

          /*原(支付交易)发起行行号*/
          set_zd_data("0310",hv_fzf.o_or_br_no);

          /*原(支付交易)接收行行号*/
          set_zd_data("0320",hv_fzf.o_ac_br_no);

          /**应答内容**/
          if(memcmp(cFzftx_type,"314",sizeof(cFzftx_type)-1)==0 || memcmp(cFzftx_type,"302",sizeof(cFzftx_type)-1)==0)
               MEMCPY_DEBUG(fd123.notpay_content,hv_fzf.res_content,sizeof(fd123.notpay_content));
          else    /**申请内容**/
               MEMCPY_DEBUG(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));


          pub_base_full_space((char*)&fd123,sizeof(fd123));
          set_zd_data("1230",(char*)&fd123);
          vtcp_log("fd123=[%s]",(char*)&fd123);

       }


OkExit:
       strcpy(g_pub_tx.reply,"0000");
       sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,g_pub_tx.reply);
       return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
       sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
       WRITEMSG
       set_zd_data(DC_REPLY,g_pub_tx.reply);
       return 1;

}

/**交易类型**/
int iNum2TxType(char *ch,char *cTmpList)
{
          if(!memcmp(ch,"100",3))
          {           
                strcpy(cTmpList,"大额汇兑");
          } 
          else if(!memcmp(ch,"101",3))
          {
               strcpy(cTmpList,"委托收款");
          }
          else if(!memcmp(ch,"105",3))
          {
               strcpy(cTmpList,"同业拆借");
          }
          else if(!memcmp(ch,"102",3))
          {
               strcpy(cTmpList,"托收承付");
          }
          else if(!memcmp(ch,"109",3))
          {
               strcpy(cTmpList,"电子联行");
          }
          else if(!memcmp(ch,"108",3))
          {
               strcpy(cTmpList,"支付退汇");
          }
          else if(!memcmp(ch,"313",3))
          {
                strcpy(cTmpList,"申请退回");
          }
          else if(!memcmp(ch,"314",3))
          {
                strcpy(cTmpList,"退回应答");
          }
          else if(!memcmp(ch,"301",3))
          {
                strcpy(cTmpList,"大额查询");
          }
          else if(!memcmp(ch,"302",3))
          {
               strcpy(cTmpList,"大额查复");
          }
          else if(!memcmp(ch,"303",3))
          {
               strcpy(cTmpList,"自由格式");
          }
          else if(!memcmp(ch,"311",3))
          {
                strcpy(cTmpList,"申请撤销");
          }
          else if(!memcmp(ch,"407",3))
          {
                strcpy(cTmpList,"质押融资");
          }
          else if(!memcmp(ch,"408",3))
          {
                strcpy(cTmpList,"质押融资扣款");
          }
          else if(!memcmp(ch,"121",3))
          {
                strcpy(cTmpList,"资金移存");
          }
          else if(!memcmp(ch,"721",3))
          {
               strcpy(cTmpList,"申请资金清算");
          }
          else if(!memcmp(ch,"122",3))
          {
               strcpy(cTmpList,"银行资金清算");
          }
          else if(!memcmp(ch,"123",3))
          {
               strcpy(cTmpList,"多余资金退回");
          }
          else if(!memcmp(ch,"725",3))
          {
                strcpy(cTmpList,"全额兑付通知");
          }
          else if(!memcmp(ch,"724",3))
          {
                strcpy(cTmpList,"汇票申请退回");
          }
          else if(!memcmp(ch,"124",3))
          {
               strcpy(cTmpList,"汇票未用退回");
          }
          else
               strcpy(cTmpList,"未知交易类型");

          return (0);
}

int get_statname( char stat, char *statname )
{
    switch(stat )
    {
         case '0':
            strcpy( statname, "往账登记记账" );
            break;
         case '1':
            strcpy( statname, "往账记账等待" );
            break;
         case '2':
            strcpy( statname, "往账记账清算" );
            break;
         case '3':
            strcpy( statname, "往账退回记账" );
            break;
         case '4':
            strcpy( statname, "往账退回等待" );
            break;
         case '5':
            strcpy( statname, "往账拒绝" );
            break;
         case '6':
            strcpy( statname, "往账撤销记账" );
            break;
         case '7':
            strcpy( statname, "往账撤销等待" );
            break;
         case '8':
            strcpy( statname, "来账拒绝" );
            break;
         case '9':
            strcpy( statname,"来账记账清算" );
            break;
         case 'A':
            strcpy( statname,"往账登记取消" );
            break;
         case 'B':
            strcpy( statname,"来账退回" );
            break;
         case 'C':
            strcpy( statname,"往帐登记" );
            break;
         case 'D':
            strcpy( statname,"往帐复核取消" );
            break;
         case 'E':
            strcpy( statname,"来账记账登记" );
            break;
         case 'F':
            strcpy( statname,"来账核地方押错" );
            break;
         case 'G':
            strcpy( statname,"来账核全国押错" );
            break;
         case 'H':
            strcpy( statname,"来账内容不符" );
            break;
         case 'I':
            strcpy( statname,"来账户名不符" );
            break;
         case 'J':
            strcpy( statname,"往账排队中" );
            break;
         case 'K':
            strcpy( statname,"核对清算记帐" );
            break;
         default:
            strcpy( statname, "无效状态" );
            break;
   }
} 
