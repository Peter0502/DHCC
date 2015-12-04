/********************************************************************
*函数功能: 票交业务的普通借记业务回执录入                            *
*函数原型:  spJ411()                                                 *
*输入参数:                                                           *
*输出参数:                                                           *
*使用说明:                                                           *
*编 写 者: wyw 2007-03-06  10:16                                     *
*                                                                    *
*修    改:增加修改标志,如果标志为修改那么根据支付序号直接找到回执记录*
*                                                                    *
*********************************************************************/
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_fd123.h"
#include "lv_add.h"

extern char* pcLvGetDay();

static int iRc=0;
static int iOrderno=0;
static int sModifyFlag=0;  /****修改标志****/
static int a = 0;
static int e = 0 ;
static int b = 0;
static int c = 0;
static int d = 0;
static int i = 0;
char cType[2];           /** 对应前台#INTYPE **/
char cOptype[2];         /** 对应前台#OPTYPE **/
char cShresut[11];       /** 审核结果 **/
char cRcp8[9];           /** 对应前台#RCP8 **/
char pcCashno[13];
char pcAcbrno[13];
char sql_brno[8];
char sql_stat[2];
char rcpstat[3];
char TpReason[11];
char cPztype[4];
char cPnum[21];
char cReason[3];
char pcPayactno[33];
char tmpstr[61];

PAY_IN_AREA    payin,payin_hz;
LV_FD123_AREA fd123;
static  struct lv_pkgreg_c lv_pkgreg;
static  struct lv_param_c lv_param;

int spJ411()
{
     vtcp_log("[%s][%d]----------开始进入支票回执录入!!!\n",__FILE__,__LINE__);
     memset(pcPayactno,0,sizeof(pcPayactno));
     memset(cType,     0, sizeof(cType));
     memset(cOptype,   0, sizeof(cOptype));
     memset(cShresut,  0, sizeof(cShresut));
     memset(cRcp8,     0, sizeof(cRcp8));
     memset(pcCashno,  0, sizeof(pcCashno));
     memset(pcAcbrno,  0, sizeof(pcAcbrno));
     memset(sql_brno,  0, sizeof(sql_brno));  
     memset(sql_stat,  0, sizeof(sql_stat));  
     memset(&payin,    0, sizeof(payin));
     memset(&payin_hz, 0, sizeof(payin_hz));
     memset(&fd123,    0, sizeof(fd123));
     memset(&lv_pkgreg,0, sizeof(lv_pkgreg));
     memset(&lv_param ,0, sizeof(lv_param));
     memset(rcpstat,   0, sizeof(rcpstat));
     memset(TpReason,  0, sizeof(TpReason));
     memset(cPztype,   0, sizeof(cPztype));
     memset(cPnum,       0, sizeof(cPnum));
     memset(tmpstr,       0, sizeof(tmpstr));

     pub_base_sysinit();

     get_zd_data("0030",sql_brno);
     get_fd_data("0280",cRcp8);
     vtcp_log("[%s][%d]===========cRcp8=[%s]\n",__FILE__,__LINE__,cRcp8);
     if(strcmp(cRcp8,"0")==0){
          memset(cRcp8,0,sizeof(cRcp8));
     }
     get_zd_data("0230",cPztype);
     get_zd_data("0370",cPnum);
     get_fd_data("0860",cShresut);
     get_zd_data("0980",cOptype);
     get_zd_data("0990",cType);
     get_fd_data("1230",(char *)&fd123);
     sql_stat[0]=STAT_LZRECV1;
     vtcp_log("[%s][%d]===========cOptype=[%c]\n",__FILE__,__LINE__,cOptype[0]);

     if (iLvchkstat())
     {
          strncpy(g_pub_tx.reply,"P046",4);
          WRITEMSG
          goto ErrExit;
     }
     
     vtcp_log("[%s][%d],状态检查完毕!开始接收数据:=============================\n",__FL__);
     iRc = iLvTisToPayin(&payin);
     if ( iRc )
     {
          sprintf( acErrMsg,"调用函数iHvTistoNotpay出错");
          strcpy( g_pub_tx.reply, "P053" );
          WRITEMSG
          goto ErrExit; 
     }
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s][%d],lw_ind[%.1s]=============================\n",__FL__,payin.F_lw_ind);
     if (tis81_to_payin())
     {
          if(memcmp(g_pub_tx.reply,"0000",4)==0){
               strcpy(g_pub_tx.reply,"HV01");
          }
          vtcp_log("[%s][%d]tis81_to_payin错误 \n",__FILE__,__LINE__);
          WRITEMSG
          goto ErrExit;
     }
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s],[%d],F_crdb[%.1s],lw_ind[%.1s]\n",__FL__,payin.F_lw_ind,payin_hz.F_crdb[0]);
     /****基本检查****/
     if (cOptype[0]!='0')
     { 
          vtcp_log("[%s],[%d],F_crdb[%.1s],F_pkgno[%s]\n",__FL__,payin_hz.F_crdb,payin_hz.F_pkgno);
          payin_hz.F_crdb[0]=CRDB_DR;
          iRc=iLvPayinCheck0(&payin_hz);
          if (iRc)
          {
               vtcp_log("[%s][%d]iLvPayinCheck0错误 \n",__FILE__,__LINE__);
               sprintf( acErrMsg,"调用函数iLvPayinCheck0出错");
               WRITEMSG
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy( g_pub_tx.reply, "HV01" );
               }
               goto ErrExit;
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          /****要素检查****/
          vtcp_log("[%s][%d]=============================\n",__FILE__,__LINE__);
          iRc=iLvPayinCheck4(&payin_hz);
          if (iRc)
          {
               vtcp_log("[%s][%d]iLvPayinCheck4错误 \n",__FILE__,__LINE__);
               sprintf( acErrMsg,"调用函数iLvPayinCheck4出错");
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy(g_pub_tx.reply,"HV01");
               }
               WRITEMSG
               goto ErrExit;
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          /****行号检查****/
          vtcp_log("[%s][%d]=============================\n",__FILE__,__LINE__);
          iRc=iLvPayinCheck1(&payin_hz);
          if (iRc)
          {
               vtcp_log("[%s][%d]iLvPayinCheck1错误 \n",__FILE__,__LINE__);
               sprintf( acErrMsg,"调用函数iLvPayinCheck4出错");
               WRITEMSG
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy(g_pub_tx.reply,"HV01");
               }
               goto ErrExit;
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,\
		payin.T_cashaddress,payin.F_cashaddress);
          vtcp_log("[%s][%d]cShresut[%s],cRcp8[%s],F_rcpstat[%.2s]========\n",\
		__FL__,cShresut,cRcp8,payin_hz.F_rcpstat);
          vtcp_log("[%s][%d],int rcp8[%d]\n",__FL__,\
		apatoi(cRcp8,sizeof(cRcp8)-1));
          /**TODO NEWYX2:模拟测试，暂不控制验印结果,去掉cChyflag[0]!='1'条件****/
          zip_space(cShresut);
          if (((cShresut[0]!=0 && (memcmp(cShresut,"00",2)!=0)) || \
		apatoi(cRcp8,sizeof(cRcp8)-1)!=0) && memcmp(payin_hz.F_rcpstat,"00",2)==0)
          {
               vtcp_log("[%s][%d]检查验印审核错误 \n",__FILE__,__LINE__);
               sprintf( acErrMsg,"审核未通过不能成功回执");
               set_zd_data(DC_GET_MSG,acErrMsg);
               strcpy(g_pub_tx.reply,"HV01");     
               WRITEMSG
               goto ErrExit;
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,\
		payin.T_cashaddress,payin.F_cashaddress);
          if (!memcmp(payin_hz.F_rcpstat,"00",2))
          { 
               memcpy(pcCashno,payin_hz.F_cashno,sizeof(payin_hz.F_cashno));
               memcpy(pcAcbrno,payin_hz.F_acbrno,sizeof(payin_hz.F_orbrno));
               iRc=iLvChkChgbank(pcCashno,pcAcbrno);
               if (iRc)
               {
                    vtcp_log("[%s][%d]iLvChkChgbank错误 [%s][%s]\n",__FILE__,\
			__LINE__,pcCashno,pcAcbrno);
                    sprintf( acErrMsg,"交易行的票交代理关系不存在,或关系已被撤销!");
                    set_zd_data(DC_GET_MSG,acErrMsg);
                    WRITEMSG
                    strcpy(g_pub_tx.reply,"P091");
                    goto ErrExit;
               }   
          } 
          vtcp_log("[%s][%d] pkgno==[%s] opcd=[%s]\n",__FILE__,__LINE__,\
		payin_hz.F_pkgno,payin_hz.F_opcd);
          vtcp_log("[%s][%d] payactno[%s],cashname=[%s]\n",__FILE__,__LINE__,payin.F_payactno,payin.F_cashname);
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          vtcp_log("%s,%d,rcpstat=%.2s\n",__FILE__,__LINE__,payin_hz.F_rcpstat);
     }
     vtcp_log("[%s][%d]已经到这里了2222222222222222222222222222222222...----------!!!!!!",__FILE__,__LINE__);
     vtcp_log("[%s][%d]==================payin=[%s]\n",__FILE__,__LINE__,&payin);
     if (cOptype[0]=='0')
     {
          char pcTmpBuf[160];
          /****联动查询****/
          memcpy(fd123.or_br_no,payin.F_orbrno,sizeof(fd123.or_br_no));
          memcpy(fd123.wt_date,payin.F_wtday,sizeof(fd123.wt_date));
          memcpy(fd123.orderno,payin.F_orderno,sizeof(fd123.orderno));
          memcpy(fd123.txnum,payin.F_txnum,sizeof(fd123.txnum));
          memcpy(fd123.pay_ac_no,payin.F_payactno,sizeof(fd123.pay_ac_no));
          memcpy(fd123.pay_name,payin.F_payname,sizeof(fd123.pay_name));
          memcpy(fd123.cash_ac_no,payin.F_cashactno,sizeof(fd123.cash_ac_no));
          memcpy(fd123.cash_opn_br_no,payin.F_cashno,sizeof(fd123.cash_opn_br_no));
          memcpy(fd123.lv_sts,payin.T_stat,sizeof(fd123.lv_sts));
          vtcp_log("[%s][%d]sts=[%.1s]\n",__FILE__,__LINE__,fd123.lv_sts);
          memcpy(fd123.cash_name,payin.F_cashname,sizeof(fd123.cash_name));
          vtcp_log("[%s][%d]payin.F_ptype=[%s]\n",__FILE__,__LINE__,payin.F_ptype);
          vtcp_log("[%s][%d]==============fd123=[%s]\n",__FILE__,__LINE__,&fd123);
          /**提取审核结果**/
          iRc=iLvGetAddit(&payin,"SHR",TpReason,sizeof(TpReason)-1);
          if (iRc)
          {
               vtcp_log("%s,%d,提取审核结果失败!\n",__FILE__,__LINE__);
               sprintf( acErrMsg,"提取审核结果失败");
               set_zd_data(DC_GET_MSG,acErrMsg);
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy(g_pub_tx.reply,"P092");
               }
               WRITEMSG
               return(-1);
          }
          vtcp_log("%s,%d,提取的审核结果为[%s]\n",__FILE__,__LINE__,TpReason);
          set_zd_data("0860",TpReason);
          /**取得票据号码****/
          if (memcmp(payin.F_txnum,TXNUM_ZPJL,sizeof(payin.F_txnum))==0)
          {
               ZPJL1_ADD_AREA wd_zp_add1;
               memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
               iRc=iLvGetAddit(&payin,"001",&wd_zp_add1,0); 
               memcpy(fd123.po_num,wd_zp_add1.cpno,sizeof(wd_zp_add1.cpno));
               char tmpnum[21];
               memset(tmpnum,0,sizeof(tmpnum));
               memcpy(tmpnum+8,wd_zp_add1.cpno,sizeof(wd_zp_add1.cpno));
               pub_base_full_space(tmpnum,20);
               set_zd_data("0370",tmpnum);
               set_zd_data("0230","002");
          }
          else if(memcmp(payin.F_txnum,TXNUM_TYJL,sizeof(payin.F_txnum))==0)
          {
               TYJL1_ADD_AREA wd_ty_add1;
               memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
               iRc=iLvGetAddit(&payin,"001",&wd_ty_add1,0); 
               set_zd_data("0370",wd_ty_add1.cpno);
          }
          ZPJL2_ADD_AREA wd_zp_add2;
          memset(&wd_zp_add2,'\0',sizeof(wd_zp_add2));
          iRc=iLvGetAddit(&payin,"002",&wd_zp_add2,0);
          set_zd_data("0210",wd_zp_add2.bscnt); 
          memcpy(fd123.tx_amt,payin.F_txamt,sizeof(fd123.tx_amt));
          vtcp_log("[%s][%d]==========================\n",__FILE__,__LINE__);
          /**取得验印结果****/
          /*memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
          iRc=iLvGetAddit(&payin,"CHY",pcTmpBuf,1); 
          if (pcTmpBuf[0]=='\0')
          {
               pcTmpBuf[0]='0';
          } 
          set_zd_char("0680",pcTmpBuf[0]);
          vtcp_log("[%s][%d],验印标志为:[%s]\n",__FILE__,__LINE__,pcTmpBuf);**/
          /****是否已经回执**/     
          if (payin.T_stat[0]==STAT_LZRECV1)
          {
               set_zd_char("0690",'0');/**未回执**/
          }
          else
          {
               set_zd_char("0690",'1');/**已回执**/
          }
          vtcp_log("[%s][%d]=================fd123.wt_date=[%s]\n",__FILE__,__LINE__,fd123.wt_date);
          pub_base_full_space((char *)&fd123,sizeof(fd123));
          set_zd_data("1230",(char *)&fd123);
          vtcp_log("[%s][%d]=======[%s]\n",__FILE__,__LINE__,(char *)&fd123);
          goto OkExit;
     }
     /***插入新记录****/
     vtcp_log("[%s][%d]==============准备插入记录了!\n",__FILE__,__LINE__);
    /** memset(tmpstr,0,sizeof(tmpstr));
     memcmp(tmpstr,payin_hz.F_cashname,sizeof(payin_hz.F_cashname));**/
     zipn_space(payin_hz.F_cashname,sizeof(payin_hz.F_cashname));
     vtcp_log("%s,%d 正常cash_name[%s]\n",__FL__,payin_hz.F_cashname);
     memcpy(payin_hz.F_cashname,(char *)DBC2SBC(payin_hz.F_cashname,sizeof(payin_hz.F_cashname)),sizeof(payin_hz.F_cashname));
     vtcp_log("%s,%d 全角cash_name[%s]\n",__FL__,(char *)DBC2SBC(payin_hz.F_cashname,sizeof(payin_hz.F_cashname)));
     zipn_space(payin_hz.F_cashaddress,sizeof(payin_hz.F_cashaddress));
     memcpy(payin_hz.F_cashaddress,(char*)DBC2SBC(payin_hz.F_cashaddress,sizeof(payin_hz.F_cashaddress)),sizeof(payin_hz.F_cashaddress));
     if (!sModifyFlag)
     {
          iRc=iLvNewRecFromPayin(&payin_hz);
          if (iRc)
          {
               vtcp_log("[%s][%d]iLvNewRecFromPayin错误 \n",__FILE__,__LINE__);
               sprintf( acErrMsg,"iLvNewRecFromPayin建立登记记录错误!");
               WRITEMSG
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy(g_pub_tx.reply,"HV01");
               }
               goto ErrExit;
          }
          vtcp_log("[%s][%d]==============建立登记记录完成!\n",__FILE__,__LINE__);
          payin_hz.T_stat[0]=STAT_WZCHECK2;
          iRc=iLvPayinAutoInfo(&payin_hz);
          vtcp_log("[%s][%d]==============iLvPayinAutoInfo!\n",__FILE__,__LINE__);
          memcpy(pcPayactno,payin_hz.F_payactno,sizeof(payin_hz.F_payactno));
          if (memcmp(payin_hz.F_rcpstat,"00",2)==0)
          {
               /***成功回执处理****/
               memcpy(payin_hz.T_jzday, pcLvGetDay(), sizeof(payin_hz.T_jzday)); 
          }
          else
          {
     /****add by xyy 2007-4-19 3:37 退票理由检查，困的不行了，懒办法吧****/
          zip_space(cRcp8);
          vtcp_log("[%s][%d] leng[%d][%s]***\n",__FILE__,__LINE__,strlen(cRcp8),cRcp8);
           if(strlen(cRcp8)%2!=0)
           {
                vtcp_log("[%s][%d]您输入的退票理由代码长度不正确 \n",__FILE__,__LINE__);
             sprintf( acErrMsg,"您输入的退票理由代码长度不正确");
                strncpy(g_pub_tx.reply,"P046",4);
                WRITEMSG
                goto ErrExit;
           }
           for(i=0;i<strlen(cRcp8);i++,i++)
           {
          vtcp_log("[%s][%d]退票理由代码[%s]\n",__FILE__,__LINE__,cRcp8);
                memset(cReason,0,sizeof(cReason));
                vtcp_log("[%s][%d]i==[%d]\n",__FILE__,__LINE__,i);
                memcpy(cReason,cRcp8+i,sizeof(cReason)-1);
                vtcp_log("[%s][%d]cReason==[%s]\n",__FILE__,__LINE__,cReason);
                iRc = Lv_param_Sel(g_pub_tx.reply, &lv_param, " par_code like 'TPLY%%' and par_value='%s' ",cReason);
                if (iRc==100)
                {
                     vtcp_log("%s,%d,您输入的退票理由代码不正确!\n",__FILE__,__LINE__);
                     sprintf( acErrMsg,"您输入的退票理由代码不正确");
                     set_zd_data(DC_GET_MSG,acErrMsg);
                     strcpy(g_pub_tx.reply,"D104");
                     WRITEMSG
                     goto ErrExit;
                }
                else if(iRc!=0)
                {
                     vtcp_log("%s,%d,查询退票理由代码数据库异常记录!\n",__FILE__,__LINE__);
                     sprintf( acErrMsg,"查询退票理由代码数据库异常记录");
                     set_zd_data(DC_GET_MSG,acErrMsg);
                     WRITEMSG
                     strcpy(g_pub_tx.reply,"D101");
                     goto ErrExit;
                }
          vtcp_log("[%s][%d]退票理由代码[%s]\n",__FILE__,__LINE__,cRcp8);
           }
          vtcp_log("[%s][%d]退票理由代码[%s]\n",__FILE__,__LINE__,cRcp8);
	   /**不成功的回执不能包括00***/
	   if(memcmp(cRcp8,"00",2)==0 || memcmp(cRcp8+2,"00",2)==0 || memcmp(cRcp8+4,"00",2)==0 || memcmp(cRcp8+6,"00",2)==0){
		sprintf(acErrMsg,"退票理由中不能包括00!");
		set_zd_data(DC_GET_MSG,acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D106");
		goto ErrExit;
	   }
           a = apatoi(cRcp8,2);
           b = apatoi(cRcp8+2,2);
           c = apatoi(cRcp8+4,2);
           d = apatoi(cRcp8+6,2);
           
          if(strlen(cRcp8)==4)
          {
               if(a == b )
               {
                      vtcp_log("%s,%d,您输入的退票理由代码不能重复!\n",__FILE__,__LINE__);
                      sprintf( acErrMsg,"您输入的退票理由代码不能重复");
                      set_zd_data(DC_GET_MSG,acErrMsg);
                      strcpy(g_pub_tx.reply,"D104");
                      WRITEMSG
                      goto ErrExit;
                }
          }
          else if (strlen(cRcp8)==6)
          {
               if( a == b || a==c || b==c )
               {
                      vtcp_log("%s,%d,您输入的退票理由代码不能重复!\n",__FILE__,__LINE__);
                      sprintf( acErrMsg,"您输入的退票理由代码不能重复");
                      set_zd_data(DC_GET_MSG,acErrMsg);
                      strcpy(g_pub_tx.reply,"D104");
                      WRITEMSG
                      goto ErrExit;
                }
          }
          else if (strlen(cRcp8)==8)
          {
               if( a == b || a == c || a== d || b ==c || b == d || c == d )
               {
                      vtcp_log("%s,%d,您输入的退票理由代码不能重复!\n",__FILE__,__LINE__);
                      sprintf( acErrMsg,"您输入的退票理由代码不能重复");
                      set_zd_data(DC_GET_MSG,acErrMsg);
                      strcpy(g_pub_tx.reply,"D104");
                      WRITEMSG
                      goto ErrExit;
                }
            }
        }
          payin_hz.T_stat[0]=STAT_WZCHECK2;
         vtcp_log("[%s][%d]payin_hz.T_txnum=[%s]\n",__FILE__,__LINE__,payin_hz.T_txnum);
          vtcp_log("[%s][%d]payin_hz.F_txnum=[%s]\n",__FILE__,__LINE__,payin_hz.F_txnum);
          if(memcmp(payin_hz.T_txnum,"30102",5)==0)
          {
                 memcpy(payin_hz.T_txnum,"30104",5);
                 memcpy(payin_hz.F_txnum,"30104",5);
          }
          else if(memcmp(payin_hz.T_txnum,"30103",5)==0)
          {
                 memcpy(payin_hz.T_txnum,"30105",5);
                 memcpy(payin_hz.F_txnum,"30105",5);
          }
          else
          {
                 vtcp_log("[%s][%d]payin_hz.T_txnmu=[%s]\n",__FILE__,__LINE__,payin_hz.T_txnum);
                 sprintf( acErrMsg,"业务类型出错，非支票类业务不用此回执!");
                 set_zd_data(DC_GET_MSG,acErrMsg);
                 WRITEMSG
                 strcpy(g_pub_tx.reply,"D105");
                 goto ErrExit;
          }
          memcpy(payin_hz.T_checkno,payin_hz.T_inputno,sizeof(payin_hz.T_inputno));
          memcpy(payin_hz.T_checkwssrno,payin_hz.T_inwssrno,sizeof(payin_hz.T_inwssrno));
          iRc=iLvUpdRecFromPayin(&payin_hz,1);
          if(iRc){
               sprintf( acErrMsg,"更新记录失败!");
               WRITEMSG
               strcpy(g_pub_tx.reply,"D105");
               goto ErrExit;
          }
          vtcp_log("[%s][%d]==============iLvUpdRecFromPayin!\n",__FILE__,__LINE__);
     }
     else
     {
          vtcp_log("[%s],[%d],交易状态为[%s]\n",__FL__,payin_hz.T_stat);
          if (payin_hz.T_stat[0]!=STAT_WZCHECK2)
          {
               vtcp_log("[%s][%d]交易非复核挂账状态不可以修改!\n",__FL__);
               sprintf( acErrMsg,"交易非复核挂账状态不可以修改!");
               set_zd_data(DC_GET_MSG,acErrMsg);
               strcpy(g_pub_tx.reply,"P094");
               WRITEMSG
               goto ErrExit;
          }
          if (payin_hz.F_packid[0]!='\0' || payin_hz.T_packid[0]!='\0')
          {
               vtcp_log("[%s][%d]记录已经组包,不可以在进行修改!\n",__FL__);
               sprintf( acErrMsg,"记录已经组包,不可以在修改!");
               set_zd_data(DC_GET_MSG,acErrMsg);
               strcpy(g_pub_tx.reply,"P095");
               WRITEMSG
               goto ErrExit;
          }
          /***rcpstat为原回执状态; payin_hz.T_rcpstat为要更新的回执状态!**/
          if (memcmp(payin_hz.F_rcpstat,"00",2)==0 && memcmp(rcpstat,"00",2)!=0)
          {
               memcpy(payin_hz.T_jzday, pcLvGetDay(), sizeof(payin_hz.T_jzday)); 
               memcpy(pcPayactno,payin_hz.F_payactno,sizeof(payin_hz.F_payactno));
          }
          if (memcmp(rcpstat,"00",2)==0 && memcmp(payin_hz.F_rcpstat,"00",2)!=0)
          {
               vtcp_log("[%s][%d]不能由回执成功改为回执失败,已经走账!\n",__FILE__,__LINE__);
               sprintf( acErrMsg,"不能由回执成功改为回执失败!");
               set_zd_data(DC_GET_MSG,acErrMsg);
               WRITEMSG
               goto ErrExit;
          }
          vtcp_log("[%s][%d]payin_hz.T_txnum=[%.5s]\n",__FILE__,__LINE__,payin_hz.T_txnum);
          vtcp_log("[%s][%d]payin_hz.F_txnum=[%.5s]\n",__FILE__,__LINE__,payin_hz.F_txnum);
          if(memcmp(payin_hz.T_txnum,"30102",5)==0)
          {
                 memcpy(payin_hz.T_txnum,"30104",5);
                 memcpy(payin_hz.F_txnum,"30104",5);
          }
          else if(memcmp(payin_hz.T_txnum,"30103",5)==0)
          {
                 memcpy(payin_hz.T_txnum,"30105",5);
                 memcpy(payin_hz.F_txnum,"30105",5);
          }
          else
          {
                 vtcp_log("[%s][%d]payin_hz.T_txnmu=[%s]\n",__FILE__,__LINE__,payin_hz.T_txnum);
                 sprintf( acErrMsg,"业务类型出错，非支票类业务不用此回执!");
                 WRITEMSG
                 set_zd_data(DC_GET_MSG,acErrMsg);
                 strcpy(g_pub_tx.reply,"D105");
                 goto ErrExit;
          }
          iRc=iLvUpdRecFromPayin(&payin_hz,0);
     }
     if (iRc)
     {
          vtcp_log("[%s][%d]错误!\n",__FILE__,__LINE__);
          WRITEMSG
          strcpy(g_pub_tx.reply,"D105");
          goto ErrExit;
     }
     vtcp_log("[%s][%d] ying da day ==[%s] \n",__FL__,payin.F_respday);
     /* 更新原来的普通借记报文接收表的状态 */
     /****Modified by SSH,2005.12.26,应根据回执状态更新原来帐****/
     iRc=iUpdRecvTableStat(&payin_hz);
     if (iRc)
     {
          vtcp_log("[%s][%d]错误!\n",__FILE__,__LINE__);
          WRITEMSG
          strcpy(g_pub_tx.reply,"HV01");
          goto ErrExit;
     }
     vtcp_log("[%s][%d]已经到这里了2222222222222222222222222222222222...----------!!!!!!",__FILE__,__LINE__);
     g_reply_int=ZphzSetPayout(&payin_hz);
     if (g_reply_int)
     {
          sprintf(acErrMsg,"组织返回报错误!");
          strcpy(g_pub_tx.reply,"HV01");
          WRITEMSG
          goto ErrExit;
     }
     vtcp_log("[%s][%d]=======\n",__FILE__,__LINE__);
     iRc=pub_ins_trace_log();
     if (iRc)
     {
          sprintf(acErrMsg,"登记交易流水出错!");
          strncpy(g_pub_tx.reply,"S023",4);
          WRITEMSG
          goto ErrExit;
     }
     vtcp_log("[%s][%d],T_rcpstat[%.2s]\n",__FL__,payin_hz.T_rcpstat);
     if (memcmp(payin_hz.T_rcpstat,"00",2)==0)
     {
          goto OkExit1;
     }
     else
     {
          goto OkExit;
     }     
OkExit:
     strcpy(g_pub_tx.reply,"0000");
     sprintf(acErrMsg,"不回执成功时的出口![%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return(0);
OkExit1:
     strcpy(g_pub_tx.reply,"0001");
     sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;
OkExit2:
     strcpy(g_pub_tx.reply,"0002");
     sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;
ErrExit:
     sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 1;
}
/*********************内部函数************************/
int tis81_to_payin()
{
     /****首先找到来帐****/ 
     if (cType[0]=='0')
     {/**自动查找***/
          iRc=Lv_pkgreg_Sel(g_pub_tx.reply, &lv_pkgreg, "pkgno='002' and txnum in('30102','30103') and lv_sts='%s' and br_no='%s' and rownum=1",sql_stat,sql_brno);
          if (iRc==100)
          {
               vtcp_log("%s,%d,未找到来帐记录!\n",__FILE__,__LINE__);
               sprintf( acErrMsg,"未找到来帐记录");
               strcpy(g_pub_tx.reply,"D104");
               WRITEMSG
               return(-1);
          }else if(iRc!=0){
               vtcp_log("%s,%d,查询数据库异常记录!\n",__FILE__,__LINE__);
               sprintf( acErrMsg,"查询异常");
               WRITEMSG
               strcpy(g_pub_tx.reply,"D101");
               return(-1);
          }
       vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          vtcp_log("[%s][%d]=============================lv_pkgreg.lw_ind[%s]\n",__FL__,lv_pkgreg.lw_ind);
          memcpy(fd123.or_br_no,lv_pkgreg.or_br_no,sizeof(fd123.or_br_no));
          apitoa(lv_pkgreg.wt_date,sizeof(fd123.wt_date),fd123.wt_date);
          vtcp_log("[%s][%d]=================fd123.wt_date=[%s]\n",__FILE__,__LINE__,fd123.wt_date);
          memcpy(fd123.orderno,lv_pkgreg.orderno,sizeof(fd123.orderno));
          memcpy(fd123.lw_ind,lv_pkgreg.lw_ind,sizeof(fd123.lw_ind));
     }
     memcpy(payin.F_orbrno    ,fd123.or_br_no  ,sizeof(payin.F_orbrno));
     memcpy(payin.F_wtday     ,fd123.wt_date   ,sizeof(payin.F_wtday));
     memcpy(payin.F_orderno   ,fd123.orderno   ,sizeof(payin.F_orderno));
     memcpy(payin.F_pkgno   ,PKGNO_PTJJ   ,sizeof(payin.F_pkgno));
     memcpy(payin.F_opcd    ,OPCD_RECEIVE   ,sizeof(payin.F_opcd));
     memcpy(payin.F_lw_ind,"2",sizeof(fd123.lw_ind));
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s][%d]=================payin.F_orbrno=[%.12s]\n",__FILE__,__LINE__,payin.F_orbrno);
     vtcp_log("[%s][%d]=================fd123.wt_date=[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);
     vtcp_log("[%s][%d]=================fd123.orderno=[%.8s][%.1s]\n",__FILE__,__LINE__,fd123.orderno,fd123.lw_ind);
     iRc=iLvFetchRecToPayin(&payin); 
     if (iRc)
     {
          vtcp_log("%s,%d,未找到来帐记录!\n",__FILE__,__LINE__);
          sprintf( acErrMsg,"未找到来帐记录");
          WRITEMSG
          if(memcmp(g_pub_tx.reply,"0000",4)==0){
               strcpy(g_pub_tx.reply,"D014");
          }
          return(-1);
     }
     vtcp_log("[%s][%d] lv_sts[%.1s]\n",__FILE__,__LINE__,payin.F_lvsts);
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s][%d]=================fd123.wt_date=[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);
     vtcp_log("%s,%d,Gethere,T_brno=[%.4s]\n",__FILE__,__LINE__,payin.T_brno);
     if (memcmp(payin.T_brno,sql_brno,BRNO_LEN)!=0)
     {
          vtcp_log("%s,%d,不是本机构业务!\n",__FILE__,__LINE__);
          sprintf( acErrMsg,"不是本机构业务");
          WRITEMSG
          strcpy(g_pub_tx.reply,"Z10D");
          return(-1);
     }
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s][%d] lv_sts[%.1s]\n",__FILE__,__LINE__,payin.F_lvsts);
     vtcp_log("[%s][%d]=================fd123.wt_date=[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);
     iLvResetPayin(&payin);
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s][%d]=================fd123.wt_date=[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);
     iLvPayinAutoInfo(&payin);
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     vtcp_log("[%s][%d]=================fd123.wt_date=[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);

     vtcp_log("[%s][%d]=================fd123.wt_date=[%.8s]\n",__FILE__,__LINE__,fd123.wt_date);
     if (cOptype[0]=='0')
     {
          return(0);
     }
     /***试图找到回执****/
     memcpy(payin_hz.F_orbrno    ,fd123.or_br_no  ,sizeof(payin_hz.F_orbrno));
     memcpy(payin_hz.F_wtday     ,fd123.wt_date   ,sizeof(payin_hz.F_wtday));
     memcpy(payin_hz.F_orderno   ,fd123.orderno   ,sizeof(payin_hz.F_orderno));
     memcpy(payin_hz.F_txnum     ,fd123.txnum     ,sizeof(payin_hz.F_txnum));
     memcpy(payin_hz.F_pkgno,PKGNO_PTJJHZ,sizeof(payin_hz.F_pkgno));
     memcpy(payin_hz.F_opcd ,OPCD_LR,sizeof(payin_hz.F_opcd));
     memcpy(payin_hz.F_lw_ind,fd123.lw_ind,sizeof(fd123.lw_ind));
     vtcp_log("[%s][%d]=============================\n",__FILE__,__LINE__);
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     iRc=iLvFetchRecToPayin(&payin_hz); 
     if (iRc)
     {
          /****未找到回执记录,说明是新录入****/
          vtcp_log("%s,%d,未找到回执记录,说明是新录入\n",__FILE__,__LINE__);
          sModifyFlag=0;
          if (cOptype[0]!='0')
          {
               vtcp_log("[%s],[%d],F_crdb[%.1s]\n",__FL__,payin.F_crdb[0]);
               /**录入交易才需要拼回执包**/
               iLvCreateHz(&payin,&payin_hz);
               memcpy(payin_hz.F_rcpstat,fd123.rcp_sts,sizeof(fd123.rcp_sts));
               memcpy(payin_hz.F_content,fd123.brf,sizeof(fd123.brf));
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     }
     else
     {
          /****找到回执记录,说明是修改****/
          vtcp_log("%s,%d,找到回执记录,说明是修改\n",__FILE__,__LINE__);
          sModifyFlag=1;
          iLvResetPayin(&payin_hz);
          iLvPayinAutoInfo(&payin_hz);
          memcpy(rcpstat,payin_hz.T_rcpstat,2);
          memcpy(payin_hz.F_rcpstat,fd123.rcp_sts,sizeof(fd123.rcp_sts));
          memcpy(payin_hz.T_rcpstat,fd123.rcp_sts,sizeof(fd123.rcp_sts));
          memcpy(payin_hz.F_content,fd123.brf,sizeof(fd123.brf));
          memcpy(payin_hz.T_content,fd123.brf,sizeof(fd123.brf));
          /**做修改交易时,交易类型已经变成是(TXNUM_ZPJLHZ 或TXNUM_TYJLHZ),                   在check0检查交易类型的时候不让通过,因此转换过来 20070505***/
          if(memcmp(payin_hz.F_txnum,TXNUM_ZPJLHZ,sizeof(payin_hz.F_txnum))==0){
               memcpy(payin_hz.F_txnum,TXNUM_ZPJL,sizeof(payin_hz.F_txnum));
               memcpy(payin_hz.T_txnum,TXNUM_ZPJL,sizeof(payin_hz.F_txnum));
          }else if(memcmp(payin_hz.F_txnum,TXNUM_TYJLHZ,sizeof(payin_hz.F_txnum))==0){
               memcpy(payin_hz.F_txnum,TXNUM_TYJL,sizeof(payin_hz.F_txnum));
               memcpy(payin_hz.T_txnum,TXNUM_TYJL,sizeof(payin_hz.F_txnum));
          }else {
               vtcp_log("[%s][%d] 交易类型错误!txnum[%.5s]\n",payin_hz.F_txnum);
               strcpy(g_pub_tx.reply,"D107");
               return(-1);
          }
     }     
     vtcp_log("[%s][%d]============================rcpstat=[%s]=\n",__FILE__,__LINE__,payin_hz.F_rcpstat);
     vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          char pcReason[11];
     if (memcmp(payin_hz.F_rcpstat,"00",2)!=0)
     {
          memset(pcReason,'\0',sizeof(pcReason));
          vtcp_log("[%s][%d]========cShresut=[%s]\n",__FILE__,__LINE__,cShresut);
          vtcp_log("[%s][%d]========cRcp8=[%s]\n",__FILE__,__LINE__,cRcp8);
          zip_space(cShresut);
          if (cShresut[0]!=0 && (memcmp(cShresut,"00",2)!=0) && strlen(cShresut)!=0)
          {
               pub_base_strpack(cShresut);
               memcpy(pcReason,cShresut,sizeof(cShresut)-1);
          vtcp_log("[%s][%d]******^^^^^#####pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
               strcat(pcReason,cRcp8);
               pcReason[10]='\0';
          vtcp_log("[%s][%d]******^^^^^#####pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          }
          else
          {
               /**memcpy(pcReason,payin_hz.F_rcpstat,2);**/
          vtcp_log("[%s][%d]******^^^^^#####pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
               memcpy(pcReason,cRcp8,sizeof(cRcp8)-1);
          }
          vtcp_log("[%s][%d]******^^^^^#####pcReason=[%s]\n",__FILE__,__LINE__,pcReason);
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
          iRc=iLvSetAddit(&payin,"TPL",pcReason,sizeof(pcReason)-1); 
          if (iRc)
          {
               vtcp_log("%s,%d,设置退票理由[%s]附加域失败!\n",__FILE__,__LINE__,pcReason);
               sprintf( acErrMsg,"设置附加域失败");
               set_zd_data(DC_GET_MSG,acErrMsg);
               WRITEMSG
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy(g_pub_tx.reply,"D107");
               }
               return(-1);
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     }else{
          memset(pcReason,0,sizeof(pcReason));
          memcpy(pcReason,"00",2);
          iRc=iLvSetAddit(&payin,"TPL",pcReason,sizeof(pcReason)-1); 
          if (iRc){
               vtcp_log("%s,%d,设置退票理由[%s]附加域失败!\n",__FILE__,__LINE__,pcReason);
               sprintf( acErrMsg,"设置附加域失败");
               WRITEMSG
               if(memcmp(g_pub_tx.reply,"0000",4)==0){
                    strcpy(g_pub_tx.reply,"D107");
               }
               set_zd_data(DC_GET_MSG,acErrMsg);
               return(-1);
          }
          vtcp_log("[%s][%d] cashactno[%s],cashaddr=[%s]\n",__FILE__,__LINE__,payin.T_cashaddress,payin.F_cashaddress);
     } 
     return(0);
}
int ZphzSetPayout(PAY_IN_AREA *wp_payin)
{
     LV_FD123_AREA Fd123;
     memset((char*)&Fd123,0,sizeof(Fd123));
     memcpy(wp_payin->F_cashqsno,wp_payin->T_cashqsactno,12);
     vtcp_log("[%s],[%d]F_oorderno[%.8s],T_oorderno[%.8s],F_cashqsno[%.12s],T_cashqsactno[%.12s],进入 返回值!\n",__FL__,wp_payin->F_oorderno,wp_payin->T_oorderno,wp_payin->F_cashqsno,wp_payin->T_cashqsactno);
     memcpy((char*)&Fd123,((char*)wp_payin)+sizeof(BASE_EXCHANGE_AREA),sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));
     pub_base_full_space((char*)&Fd123,sizeof(Fd123));
     vtcp_log("%s,%d,进入返回值!\n",__FL__);
     set_zd_data("1230",(char*)&Fd123);
     vtcp_log("[%s][%d]=======oorderno[%.8s],cashqsno[%.12s]\n",__FL__,Fd123.o_orderno,Fd123.cash_qs_no);
     return(0);
}
