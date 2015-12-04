/*************************************************
* 文 件 名:  sp9655.c
* 功能描述： 由卡号回显所有的帐号信息
*
* 作    者:  ZHGF
* 完成日期： 2008年2月28日
*
* 修改记录：
* 日   期 : 
* 修 改 人:
* 修改内容:
*************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"
#include "cif_addr_inf_c.h"/** Add by liuwenjie 200WT 2010-7-16 16:50:25 **/
#include "gaga_good_reg_c.h"/*Add By Yl*/
#include "impa_dep_reg_c.h"/*Add By Yl*/

sp9655()
{
  int             ret, i = 0, vcnt=0;
  char            ac_no[21], filename[300],cTx_code[10];
  FILE            *fp = NULL;
  char            flag[2];
  char Ac_Seqn[30];

  struct prdt_ac_id_c   g_prdt_ac_id; /* 产品账户结构 */
  struct mdm_ac_rel_c   g_mdm_ac_rel;
  struct gaga_good_reg_c  GagaGoodReg;  /*抵质押表*/
  struct impa_dep_reg_c   ImpaDepReg; /*质押存单表*/
  struct td_mst_c       g_td_mst;
  struct cif_addr_inf_c g_cif_addr_inf;

  memset(&g_mdm_ac_rel  , 0x00, sizeof(struct mdm_ac_rel_c));
  memset(&g_prdt_ac_id  , 0x00, sizeof(struct prdt_ac_id_c));
  memset(&g_td_mst  , 0x00, sizeof(struct td_mst_c));
  memset(&GagaGoodReg , 0x00, sizeof(struct gaga_good_reg_c));
  memset(&ImpaDepReg  , 0x00, sizeof(struct impa_dep_reg_c));
  memset(&g_cif_addr_inf  , 0x00, sizeof(struct cif_addr_inf_c));

  memset(Ac_Seqn,0x00, sizeof(Ac_Seqn));
  memset(ac_no    , 0x00, sizeof(ac_no));
  memset(filename   , 0x00, sizeof(filename));
  memset(flag   , 0x00, sizeof(flag));        /*控制返回账户类型*/
  memset(cTx_code   , 0x00, sizeof(cTx_code));

  pub_base_sysinit();

  get_zd_data("0370", ac_no); 
  get_zd_data("0720", flag);
  get_zd_data("0650",cTx_code);
  pub_base_strpack(cTx_code);

  pub_base_old_acno(ac_no); /**对旧帐号的处理**/

  ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", ac_no);
  if (ret == 100) {
    sprintf(acErrMsg, "无此卡号，请检查!ac_no=[%s]", ac_no);
    WRITEMSG
      strcpy(g_pub_tx.reply, "M003");
    goto ErrExit;
  } else if (ret) {
    sprintf(acErrMsg, "读取介质账户对照表异常![%d]", ret);
    WRITEMSG
      strcpy(g_pub_tx.reply, "W011");
    goto ErrExit;
  }
  set_zd_data("0020",g_mdm_ac_rel.opn_br_no);
  sprintf(Ac_Seqn,"%ld",g_mdm_ac_rel.ac_seqn);
  set_zd_data("0300",Ac_Seqn);

  if(strcmp(cTx_code,"2220") == 0)
  {
    set_zd_data("0700", g_mdm_ac_rel.id_type);
    set_zd_data("0620", g_mdm_ac_rel.id_no);
  }

  /* 生成下传全路经文件名(批量) */
  pub_base_AllDwnFilName(filename);

  fp = fopen(filename, "w");
  if (fp == NULL) {
    sprintf(acErrMsg, "open file error [%s]", filename);
    WRITEMSG
      strcpy(g_pub_tx.reply, "S047");
    goto ErrExit;
  }

  ret = Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id='%ld'",g_mdm_ac_rel.ac_id);
  if (ret == 100) {
    sprintf(acErrMsg, "该卡号下无其他产品，请检查!ac_no=[%s]", ac_no);
    WRITEMSG
      strcpy(g_pub_tx.reply, "M003");
    goto ErrExit;

  }

  while (1) 
  {
    ret = Prdt_ac_id_Fet_Sel(&g_prdt_ac_id, g_pub_tx.reply);
    if (ret == 100)
    {
      break;
    }
    else if(ret!=0)
    {
      goto ErrExit;
    }
    
    if(strcmp(cTx_code,"3201") == 0 || strcmp(cTx_code,"3202")==0 || strcmp(cTx_code,"3205")==0 )
    { /*贷款账户信息*/
        
         if(g_prdt_ac_id.ac_id_type[0]=='3')
        {
           /** 增加还本还息客户地址 **/
           if(!vcnt)
           {
               ret = Cif_addr_inf_Sel(g_pub_tx.reply, &g_cif_addr_inf, " cif_no=%ld ",g_prdt_ac_id.cif_no);
               if (ret && ret!=100) 
               {
                 sprintf(acErrMsg, "查询客户地址信息表异常!ret=[%d]",ret);
                 WRITEMSG
                 strcpy(g_pub_tx.reply, "C067");
                 goto ErrExit;
               }
               else
               {
                 set_zd_data("0270",g_cif_addr_inf.addr);
               }
               vcnt++;
           }
           
           ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst, "ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
           if (ret == 100)
           {
             sprintf(acErrMsg, "该账户不存在!");
             WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
             goto ErrExit;
           } else if (ret)
           {
             sprintf(acErrMsg, "读取贷款主文件异常![%d]", ret);
             WRITEMSG
               strcpy(g_pub_tx.reply, "W017");
             goto ErrExit;
           }
           set_zd_data("0330",g_ln_mst.pact_no);
             if(g_ln_mst.ac_sts[0] != '1' && g_ln_mst.ac_sts[0] != '2' && g_ln_mst.ac_sts[0] != '3' && g_ln_mst.ac_sts[0] != '4' && g_ln_mst.ac_sts[0] != '5' )
           {
               vtcp_log("[%s][%d]ac_sts=[%c]\n",__FILE__,__LINE__,g_ln_mst.ac_sts[0]);
               continue;
           }
           ret = Ln_parm_Sel(g_pub_tx.reply, &g_ln_parm, "prdt_no='%s'", \
                 g_prdt_ac_id.prdt_no);
           if (ret == 100)
           {
             sprintf(acErrMsg, "该产品不存在!prdt_no=[%s]",
               g_prdt_ac_id.prdt_no);
             WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
             goto ErrExit;
           }
           pub_base_strpack(g_ln_parm.title);
           fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_ln_mst.opn_date,g_ln_parm.title,g_ln_mst.bal,g_ln_mst.ac_sts);
           set_zd_data("0770",g_ln_mst.opn_br_no);
           
        }
      
    }else if(strcmp(cTx_code,"4462") == 0)      /**add by zyl 20111028 for 4462**/
    {
        vtcp_log("[%s][%d] aaaazyl",__FILE__,__LINE__);
        if(g_prdt_ac_id.ac_id_type[0]=='1')
        {
          /* 定期户转出交易 */
          if(strcmp(cTx_code,"2220") == 0 || strcmp(cTx_code,"2221") == 0)
          {
            continue;
          }
          /**开始查询活期产品!***/
          ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
          if (ret == 100) {
            sprintf(acErrMsg, "该账户ID不存在!ac_no=[%s]", ac_no);
            WRITEMSG
              strcpy(g_pub_tx.reply, "M003");
            goto ErrExit;
          } else if (ret) {
            sprintf(acErrMsg, "读取活期主文件异常![%d]", ret);
            WRITEMSG
              strcpy(g_pub_tx.reply, "W015");
            goto ErrExit;
          }
         
          ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", \
                g_prdt_ac_id.prdt_no);
          if (ret == 100) 
          {
            sprintf(acErrMsg, "该产品不存在!prdt_no=[%s]",
              g_prdt_ac_id.prdt_no);
            WRITEMSG
              strcpy(g_pub_tx.reply, "M003");
            goto ErrExit;
          }
            /**开始将帐号信息写入文件**/
            pub_base_strpack(g_td_parm.title);
            if(flag[0]=='1')
            {
              fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts);
            }else if(flag[0]=='2')
            {
              fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts);
            }else if(flag[0]=='3')
            {
              if(strcmp(cTx_code,"4214") == 0)
              {
                /*ADD BY CHENGBO JZHXXYD 20110916 当做新储蓄取款时查询活期信息*/
                fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts);
              }     
            }else
            {
              fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts);
            }
        }else if(g_prdt_ac_id.ac_id_type[0]=='2')
        {
           /**查询定期产品****/
           ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%d  ",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
           if (ret == 100) 
           {
             sprintf(acErrMsg, "该账户不存在定期产品!");
             WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
             goto ErrExit;
           } else if (ret)
           {
             sprintf(acErrMsg, "读取定期主文件异常![%d]", ret);
             WRITEMSG
               strcpy(g_pub_tx.reply, "W017");
             goto ErrExit;
           }
           if(g_td_mst.ac_sts[0] != '1')
           {
               continue;
           }
           /**add by zyl 20111028**/
	   if(g_td_mst.hold_sts[0] != '0')
           {
               continue;
           }
           /**end by zyl 20111028**/
           /* 黄河增加对质押存单的判断*
           *如果存单尚未出库,则不允许其解冻结***/
           if(strcmp(cTx_code,"2406")==0)
           {
             /*查询gaga_good_reg*/
             ret = Gaga_good_reg_Sel( g_pub_tx.reply, &GagaGoodReg, "bill_no='%s'", ac_no );
             if (ret == 100)
             {
               sprintf(acErrMsg, "该存单未做抵质押!");
               WRITEMSG
             } else if (ret)
             {
               sprintf(acErrMsg, "读取抵质押物主文件异常![%d]", ret);
               WRITEMSG
                 strcpy(g_pub_tx.reply, "T057");
               goto ErrExit;
             }else
             {
               if( GagaGoodReg.sts[0] != '2' )
               {
                 sprintf(acErrMsg, "该存单尚未出库,不允许解冻结![%d]", ret);
                 WRITEMSG
                   strcpy(g_pub_tx.reply, "T157");
                 goto ErrExit;
               }
             }

             /*查询impa_dep_reg*/
             ret = Impa_dep_reg_Sel( g_pub_tx.reply, &ImpaDepReg, "mort_ac_no='%s'", ac_no );
             if( ret == 0 )
             {
               sprintf(acErrMsg, "该存单尚未出库,不允许解冻结![%d]", ret);
               WRITEMSG
                 strcpy(g_pub_tx.reply, "T157");
               goto ErrExit;
             }else if(ret != 100)
             {
               sprintf(acErrMsg, "读取存单质押主文件异常![%d]", ret);
               WRITEMSG
                 strcpy(g_pub_tx.reply, "T057");
               goto ErrExit;
             }
           }

          /* 定期户转出交易 */
          if(strcmp(cTx_code,"2220") == 0 || strcmp(cTx_code,"2221") == 0)
          {
            if(!(strcmp(g_td_mst.prdt_no,"201") == 0 || strcmp(g_td_mst.prdt_no,"202") == 0 || strcmp(g_td_mst.prdt_no,"204") == 0 || strcmp(g_td_mst.prdt_no,"205") == 0 || strcmp(g_td_mst.prdt_no,"206") == 0 || strcmp(g_td_mst.prdt_no,"207") == 0 || strcmp(g_td_mst.prdt_no,"242") == 0 || strcmp(g_td_mst.prdt_no,"243") == 0))
            {
              vtcp_log("%s,%d,不是整整定期或者通知存款，不显示",__FILE__,__LINE__);
              continue;
            }
            if(g_td_mst.ac_sts[0] != '1')
            {
              if(strcmp(cTx_code, "2221") != 0 || g_td_mst.ac_sts[0] != '*') /** 2221交易显示销户的帐户 add by wangjian 20090421**/
                continue;
            }
          }

          /*end add by wangzhe*/
          
          /** add by wangjian 2221交易只显示销户的帐户 20090421 ** 没查到2221是什么交易
          if(g_td_mst.ac_sts[0] != '*')
          {
            if(strcmp(cTx_code, "2221") == 0)
              continue;
          }

          *** 生成产品列表 ***/
          ret = Td_parm_Dec_Sel(g_pub_tx.reply, "prdt_no='%s'", \
              g_prdt_ac_id.prdt_no);
          if (ret)
            goto ErrExit;
          while (1)
          {
            ret = Td_parm_Fet_Sel(&g_td_parm, g_pub_tx.reply);
            if (ret == 100)
              break;
            if (ret)
              goto ErrExit;
              if(flag[0]=='1')
              {
                 if(strcmp(g_td_parm.prdt_no,"209")==0 || strcmp(g_td_parm.prdt_no,"20A")==0 || strcmp(g_td_parm.prdt_no,"20B")==0\
                  || strcmp(g_td_parm.prdt_no,"211")==0 || strcmp(g_td_parm.prdt_no,"212")==0 ||strcmp(g_td_parm.prdt_no,"213")==0 || strcmp(g_td_parm.prdt_no,"245")==0)
                 fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts);
              }else if(flag[0]=='2')
              {
                 if(strcmp(g_td_parm.prdt_no,"221")==0 || strcmp(g_td_parm.prdt_no,"222")==0 || strcmp(g_td_parm.prdt_no,"223")==0\
                  || strcmp(g_td_parm.prdt_no,"231")==0 || strcmp(g_td_parm.prdt_no,"232")==0 ||strcmp(g_td_parm.prdt_no,"233")==0 )
                 fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts);
          
              }else if(flag[0]=='3')
              {
                 if(strcmp(g_td_parm.prdt_no,"201")==0 || strcmp(g_td_parm.prdt_no,"202")==0 || strcmp(g_td_parm.prdt_no,"204")==0\
                  || strcmp(g_td_parm.prdt_no,"205")==0 || strcmp(g_td_parm.prdt_no,"206")==0 ||strcmp(g_td_parm.prdt_no,"207")==0\
                  || strcmp(g_td_parm.prdt_no,"208")==0 || strcmp(g_td_parm.prdt_no,"209")==0|| strcmp(g_td_parm.prdt_no,"242")==0 \
                  || strcmp(g_td_parm.prdt_no,"243")==0 || strcmp(g_td_parm.prdt_no,"245")==0 ||strcmp(g_td_parm.prdt_no,"20A")==0 \
                  ||strcmp(g_td_parm.prdt_no,"20B")==0)
                 fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts);
            }else
            {
              fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts);
            }
            /*if(strcmp(cTx_code,"2220") == 0 || strcmp(cTx_code,"2221") == 0)
            {
              set_zd_data("0240",g_td_mst.prdt_no);
            }*/
            }
        }
    }else
    {
     	    if(g_prdt_ac_id.ac_id_type[0]=='1')
         {
          /* 定期户转出交易 */
          if(strcmp(cTx_code,"2220") == 0 || strcmp(cTx_code,"2221") == 0)
          {
            continue;
          }
          /**开始查询活期产品!***/
          ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
          if (ret == 100) 
          {
            sprintf(acErrMsg, "该账户ID不存在!ac_no=[%s]", ac_no);
            WRITEMSG
              strcpy(g_pub_tx.reply, "M003");
            goto ErrExit;
          } else if (ret)
          {
            sprintf(acErrMsg, "读取活期主文件异常![%d]", ret);
            WRITEMSG
              strcpy(g_pub_tx.reply, "W015");
            goto ErrExit;
          }
         
          ret = Dd_parm_Sel(g_pub_tx.reply, &g_dd_parm, "prdt_no='%s'", \
                g_prdt_ac_id.prdt_no);
          if (ret == 100) 
          {
            sprintf(acErrMsg, "该产品不存在!prdt_no=[%s]",
              g_prdt_ac_id.prdt_no);
            WRITEMSG
              strcpy(g_pub_tx.reply, "M003");
            goto ErrExit;
          }
            /**开始将帐号信息写入文件**/
            pub_base_strpack(g_td_parm.title);
            if(flag[0]=='1')
            {
              fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
            }else if(flag[0]=='2')
            {
              fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
            
              if(strcmp(cTx_code,"4214") == 0)
              {
                /*ADD BY CHENGBO JZHXXYD 20110916 当做新储蓄取款时查询活期信息*/
                fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
              }     
            }else
            {
		           fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_dd_mst.opn_date,g_dd_parm.title,g_dd_mst.bal,g_dd_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
            }
         }else if(g_prdt_ac_id.ac_id_type[0]=='2')
         {
           /**查询定期产品****/
           ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst, "ac_id=%ld and ac_seqn=%d",g_prdt_ac_id.ac_id,g_prdt_ac_id.ac_seqn);
           if (ret == 100) 
           {
             sprintf(acErrMsg, "该账户不存在定期产品!");
             WRITEMSG
               strcpy(g_pub_tx.reply, "M003");
             goto ErrExit;
           } else if (ret) 
           {
             sprintf(acErrMsg, "读取定期主文件异常![%d]", ret);
             WRITEMSG
               strcpy(g_pub_tx.reply, "W017");
             goto ErrExit;
           }
           if(g_td_mst.ac_sts[0] != '1')
           {
               continue;
           }

           /** 黄河增加对质押存单的判断*
                 *如果存单尚未出库,则不允许其解冻结*
               **/
           if(strcmp(cTx_code,"2406")==0)
           {
             /*查询gaga_good_reg*/
             ret = Gaga_good_reg_Sel( g_pub_tx.reply, &GagaGoodReg, "bill_no='%s'", ac_no );
             if (ret == 100) 
             {
               sprintf(acErrMsg, "该存单未做抵质押!");
               WRITEMSG
             } else if (ret)
             {
               sprintf(acErrMsg, "读取抵质押物主文件异常![%d]", ret);
               WRITEMSG
                 strcpy(g_pub_tx.reply, "T057");
               goto ErrExit;
             }else
             {
               if( GagaGoodReg.sts[0] != '2' )
               {
                 sprintf(acErrMsg, "该存单尚未出库,不允许解冻结![%d]", ret);
                 WRITEMSG
                   strcpy(g_pub_tx.reply, "T157");
                 goto ErrExit;
               }
             }

             /*查询impa_dep_reg*/
             ret = Impa_dep_reg_Sel( g_pub_tx.reply, &ImpaDepReg, "mort_ac_no='%s'", ac_no );
             if( ret == 0 )
             {
               sprintf(acErrMsg, "该存单尚未出库,不允许解冻结![%d]", ret);
               WRITEMSG
                 strcpy(g_pub_tx.reply, "T157");
               goto ErrExit;
             }else if(ret != 100)
             {
               sprintf(acErrMsg, "读取存单质押主文件异常![%d]", ret);
               WRITEMSG
                 strcpy(g_pub_tx.reply, "T057");
               goto ErrExit;
             }
           }

          /* 定期户转出交易 */
          if(strcmp(cTx_code,"2220") == 0 || strcmp(cTx_code,"2221") == 0)
          {
            if(!(strcmp(g_td_mst.prdt_no,"201") == 0 || strcmp(g_td_mst.prdt_no,"202") == 0 || strcmp(g_td_mst.prdt_no,"204") == 0 || strcmp(g_td_mst.prdt_no,"205") == 0 || strcmp(g_td_mst.prdt_no,"206") == 0 || strcmp(g_td_mst.prdt_no,"207") == 0 || strcmp(g_td_mst.prdt_no,"242") == 0 || strcmp(g_td_mst.prdt_no,"243") == 0))
            {
              vtcp_log("%s,%d,不是整整定期或者通知存款，不显示",__FILE__,__LINE__);
              continue;
            }
            if(g_td_mst.ac_sts[0] != '1')
            {
              if(strcmp(cTx_code, "2221") != 0 || g_td_mst.ac_sts[0] != '*') /** 2221交易显示销户的帐户 add by wangjian 20090421**/
                continue;
            }
          }

          /*end add by wangzhe*/
          
          /** add by wangjian 2221交易只显示销户的帐户 20090421 ** 没查到2221是什么交易
          if(g_td_mst.ac_sts[0] != '*')
          {
            if(strcmp(cTx_code, "2221") == 0)
              continue;
          }

          *** 生成产品列表 ***/
          ret = Td_parm_Dec_Sel(g_pub_tx.reply, "prdt_no='%s'", \
              g_prdt_ac_id.prdt_no);
          if (ret)
            goto ErrExit;
          while (1) 
          {
            ret = Td_parm_Fet_Sel(&g_td_parm, g_pub_tx.reply);
            if (ret == 100)
            {
              break;
            }if (ret)
            {
              goto ErrExit;
            }
            /*返回指定类型的产品信息*/
            if(flag[0]=='1')
            {
                if(strcmp(g_td_parm.prdt_no,"209")==0 || strcmp(g_td_parm.prdt_no,"20A")==0 || strcmp(g_td_parm.prdt_no,"20B")==0\
                  || strcmp(g_td_parm.prdt_no,"211")==0 || strcmp(g_td_parm.prdt_no,"212")==0 ||strcmp(g_td_parm.prdt_no,"213")==0 || strcmp(g_td_parm.prdt_no,"245")==0)
                fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
            }else if(flag[0]=='2')
            {
                if(strcmp(g_td_parm.prdt_no,"221")==0 || strcmp(g_td_parm.prdt_no,"222")==0 || strcmp(g_td_parm.prdt_no,"223")==0\
                  || strcmp(g_td_parm.prdt_no,"231")==0 || strcmp(g_td_parm.prdt_no,"232")==0 ||strcmp(g_td_parm.prdt_no,"233")==0 )
                fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
          
            }else if(flag[0]=='3')
            {
                if(strcmp(g_td_parm.prdt_no,"201")==0 || strcmp(g_td_parm.prdt_no,"202")==0 || strcmp(g_td_parm.prdt_no,"204")==0\
                  || strcmp(g_td_parm.prdt_no,"205")==0 || strcmp(g_td_parm.prdt_no,"206")==0 ||strcmp(g_td_parm.prdt_no,"207")==0\
                  || strcmp(g_td_parm.prdt_no,"208")==0 || strcmp(g_td_parm.prdt_no,"209")==0|| strcmp(g_td_parm.prdt_no,"242")==0 \
                  || strcmp(g_td_parm.prdt_no,"243")==0 || strcmp(g_td_parm.prdt_no,"245")==0 ||strcmp(g_td_parm.prdt_no,"20A")==0 \
                  ||strcmp(g_td_parm.prdt_no,"20B")==0)
                fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
            }else
            { /*不管产品类型都返回*/
                fprintf(fp, "%-3d|%d|%-14s|%-17.2f|%s|%c|\n", g_prdt_ac_id.ac_seqn,g_td_mst.opn_date,g_td_parm.title,g_td_mst.bal,g_td_mst.ac_sts,g_prdt_ac_id.ac_id_type[0]);
            }
         
          }
        }
       
    }
  }
  Prdt_ac_id_Clo_Sel();
  set_zd_data(DC_FILE_SND, "1");
  fclose(fp);
OkExit:
  strcpy(g_pub_tx.reply, "0000");
  sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 0;
ErrExit:
  if(fp != NULL)    /**20100903 add by zyl clear file for suini**/
  {
    fclose(fp);
    fp = NULL;
  } 
  sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
  WRITEMSG
    set_zd_data(DC_REPLY, g_pub_tx.reply);
  return 1;
}
