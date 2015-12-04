/*******************************************************
**   程序功能：实现对支票业务明细的查询
**   程序名称：spJ414.c
**   函数原型：spJ414()
**   输入参数：
**   输出参数：
**   说明： 本程序实现对支票截留,通用截留以及去相应回执业务的明细查询。
**   作者：wyw
*******************************************************/
#define ERR_DEAL if(ret){ \
         sprintf(acErrMsg,"sql error.[%d]",ret);\
         WRITEMSG \
        goto ErrExit;\
        }

#define EXTERN
#include "public.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_addit_c.h"
#include "lv_fd123.h"

spJ414()
{  
    int ret=0;
    FILE *fp;
    char filename[100];
    char stat_content[21];
    char txnum_content[20];
    PAY_IN_AREA     payin;
    struct lv_pkgreg_c lv_pkgreg;
    struct lv_addit_c lv_addit;
    int wtday=0;
    char txtp[2];
    char tagdata[10];
    long Orderno=0;
    char Wtday[9];
    char Orbrno[13];
    char orderno[9];
    char Pkgno[4];
    char ZP_NUM[13];
    char ZP_DATE[9];
    LV_FD123_AREA fd123;
    memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
    memset(&lv_addit,0x00,sizeof(struct lv_addit_c));
    memset(txtp,0,sizeof(txtp));
    memset(orderno,0,sizeof(orderno));
    memset(ZP_NUM,0,sizeof(ZP_NUM));
    memset(ZP_DATE,0,sizeof(ZP_DATE));
    memset(&payin,0,sizeof(payin));
    memset(Wtday,0,sizeof(Wtday));
    memset(Orbrno,0,sizeof(Orbrno));
    memset(Pkgno,0,sizeof(Pkgno));
    memset(&fd123,' ',sizeof(fd123));

     get_zd_data("0440",Wtday);
     get_zd_data("0300",Orbrno);
     get_zd_data("0670",txtp);     /** 1发送，2接收 **/
     get_zd_data("0230",Pkgno);
     get_zd_long("0280",&Orderno);
	
     vtcp_log("begin sysinit...%s,%d  wtday[%s]",__FILE__,__LINE__,Wtday);
     pub_base_sysinit();     
	wtday=apatoi(Wtday,8);
	apitoa(Orderno,8,orderno);
     vtcp_log("%s,%d",__FILE__,__LINE__);
     vtcp_log("begin txtp=====%s...%s,%d",txtp,__FILE__,__LINE__);
     ret=Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg,"pkgno='%s' and orderno='%s' and in_date=%d and lw_ind='%s' and or_br_no='%s'",Pkgno,orderno,wtday,txtp,Orbrno);
     if(ret){
	sprintf(acErrMsg,"未找到符合条件的记录!");
	set_zd_data(DC_GET_MSG,acErrMsg);
	WRITEMSG
	strcpy(g_pub_tx.reply,"S047");
	goto ErrExit;
     }
     if(memcmp(lv_pkgreg.txnum,"30102",5)==0 || memcmp(lv_pkgreg.txnum,"30104",5)==0){
     	vtcp_log("begin 提取支票号码=====...%s,%d",__FILE__,__LINE__);
	ret=Lv_addit_Sel(g_pub_tx.reply,&lv_addit,"tx_date=%d and tagname='001' and addid='%s'",lv_pkgreg.in_date,lv_pkgreg.addid);
     	if(ret){
       		sprintf(acErrMsg,"未找到符合条件的记录!");
        	set_zd_data(DC_GET_MSG,acErrMsg);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"S047");
        	goto ErrExit;
     	}
	memset(ZP_NUM,0,12);
	memset(ZP_DATE,0,8);
	memcpy(ZP_NUM,lv_addit.tagdata+8,12);
	memcpy(ZP_DATE,lv_addit.tagdata,8);
     }
	/**用来传支票号码***/
	memcpy(fd123.note_no,ZP_NUM,sizeof(ZP_NUM)-1);
	vtcp_log("[%s][%d]fd123.note_no=[%s] ",__FL__,ZP_NUM);	
	memcpy(fd123.po_date,ZP_DATE,sizeof(ZP_DATE)-1);
	vtcp_log("[%s][%d]fd123.po_date=[%s] ",__FL__,ZP_DATE);	
	memcpy(fd123.or_br_no,lv_pkgreg.or_br_no,sizeof(lv_pkgreg.or_br_no)-1);
	memcpy(fd123.pkgno,lv_pkgreg.pkgno,sizeof(lv_pkgreg.pkgno)-1);
	vtcp_log("[%s][%d]fd123.pkgno=[%s] ",__FL__,fd123.pkgno );	
        memcpy(fd123.pay_qs_no ,lv_pkgreg.pay_qs_no ,sizeof(lv_pkgreg.pay_qs_no)-1);
        vtcp_log("[%s][%d]fd123.pay_qs_no=[%s] ",__FL__,fd123.pay_qs_no );
        memcpy(fd123.pay_ac_no ,lv_pkgreg.pay_ac_no ,sizeof(lv_pkgreg.pay_ac_no)-1);
        vtcp_log("[%s][%d]fd123.pay_ac_no=[%s] ",__FL__,fd123.pay_ac_no );
        memcpy(fd123.pay_name  ,lv_pkgreg.pay_name  ,sizeof(lv_pkgreg.pay_name ) -1);
        vtcp_log("[%s][%d]fd123.pay_name=[%s] ",__FL__,fd123.pay_name );
        memcpy(fd123.pay_addr  ,lv_pkgreg.pay_addr  ,sizeof(lv_pkgreg.pay_addr ) -1);
        vtcp_log("[%s][%d]fd123.pay_addr=[%s] ",__FL__,fd123.pay_addr );
        memcpy(fd123.ac_br_no  ,lv_pkgreg.ac_br_no  ,sizeof(lv_pkgreg.ac_br_no )
-1);
        vtcp_log("[%s][%d]fd123.ac_br_no=[%s] ",__FL__,fd123.ac_br_no );
        memcpy(fd123.cash_ac_no  ,lv_pkgreg.cash_ac_no  ,sizeof(lv_pkgreg.cash_ac_no  )-1);
        vtcp_log("[%s][%d]fd123.cash_ac_no=[%s] ",__FL__,fd123.cash_ac_no );
        memcpy(fd123.cash_qs_no  ,lv_pkgreg.cash_qs_no  ,sizeof(lv_pkgreg.cash_qs_no  )-1);
        vtcp_log("[%s][%d]fd123.cash_qs_no=[%s] ",__FL__,fd123.cash_qs_no );
        memcpy(fd123.cash_name ,lv_pkgreg.cash_name ,sizeof(lv_pkgreg.cash_name)-1);
        vtcp_log("[%s][%d]fd123.cash_name=[%s] ",__FL__,fd123.cash_name );
        memcpy(fd123.cash_addr ,lv_pkgreg.cash_addr ,sizeof(lv_pkgreg.cash_addr)-1);
        vtcp_log("[%s][%d]fd123.cash_addr=[%s] ",__FL__,fd123.cash_addr );
        memcpy(fd123.orderno   ,lv_pkgreg.orderno   ,sizeof(lv_pkgreg.orderno )- 1);
        vtcp_log("[%s][%d]fd123.orderno=[%s] ",__FL__,fd123.orderno );
        memcpy(fd123.o_orderno ,lv_pkgreg.o_orderno ,sizeof(lv_pkgreg.o_orderno)-1);
        vtcp_log("[%s][%d]fd123.o_orderno=[%s] ",__FL__,fd123.o_orderno );
        memcpy(fd123.pay_opn_br_no ,lv_pkgreg.pay_opn_br_no ,sizeof(lv_pkgreg.pay_opn_br_no)-1);
        vtcp_log("[%s][%d]fd123.pay_opn_br_no=[%s] ",__FL__,fd123.pay_opn_br_no);
        memcpy(fd123.cash_opn_br_no  ,lv_pkgreg.cash_opn_br_no ,sizeof(lv_pkgreg.cash_opn_br_no)-1);
        vtcp_log("[%s][%d]fd123.cash_opn_br_no=[%s] ",__FL__,fd123.cash_opn_br_no );
        memcpy(fd123.txnum ,lv_pkgreg.txnum ,sizeof(lv_pkgreg.txnum )-1);
        vtcp_log("[%s][%d]fd123.txnum=[%s] ",__FL__,fd123.txnum );
        memcpy(fd123.yw_type ,lv_pkgreg.lv_yw_ind ,sizeof(lv_pkgreg.lv_yw_ind )-1);
        vtcp_log("[%s][%d]fd123.yw_type=[%s] ",__FL__,fd123.yw_type );
        memcpy(fd123.brf ,lv_pkgreg.lv_brf ,sizeof(lv_pkgreg.lv_brf )-1);
        vtcp_log("[%s][%d]fd123.brf=[%s] ",__FL__,fd123.brf );
  memcpy(fd123.rcp_sts  ,lv_pkgreg.rcpstat,sizeof(fd123.rcp_sts));/**应答状态**/
  memcpy(fd123.ret_code ,lv_pkgreg.retcode,sizeof(fd123.ret_code));/**应答状态**/
  memcpy(fd123.pack_id,lv_pkgreg.packid,sizeof(fd123.pack_id));/**原包号**/
  memcpy(fd123.o_or_br_no,lv_pkgreg.o_or_br_no,sizeof(fd123.o_or_br_no));
  vtcp_log("[%s][%d]fd123.o_or_br_no=[%s] ",__FL__,fd123.o_or_br_no );
  memcpy(fd123.o_ac_br_no,lv_pkgreg.o_ac_br_no,sizeof(fd123.o_ac_br_no));
  vtcp_log("[%s][%d]fd123.o_ac_br_no=[%s] ",__FL__,fd123.o_ac_br_no );
  apitoa(lv_pkgreg.pack_date,sizeof(fd123.pack_date),fd123.pack_date);
  apitoa(lv_pkgreg.o_wt_date,sizeof(fd123.o_wt_date),fd123.o_wt_date);/**原包日�**/
  vtcp_log("[%s][%d]lv_pkgreg.o_pack_date=[%d] ",__FL__,lv_pkgreg.o_pack_date);
  vtcp_log("[%s][%d]fd123.pack_id=[%s] ",__FL__,fd123.pack_id );
  vtcp_log("[%s][%d]fd123.rcp_sts=[%s] ",__FL__,fd123.rcp_sts );
      fd123.chrg_ind[0]  = lv_pkgreg.tx_chrg_ind[0];
        fd123.lw_ind[0] = lv_pkgreg.lw_ind[0];
        fd123.lv_sts[0] = lv_pkgreg.lv_sts[0];
        vtcp_log("[%s][%d]fd123.chrg_ind=[%c] ",__FL__,fd123.chrg_ind[0] );
        vtcp_log("[%s][%d]fd123.lv_pkgreg.tx_amt*100=[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt*100);
        {
        double dTmp1=0.0;
        dTmp1=lv_pkgreg.tx_amt*100.0;
        dbl2str(&dTmp1,0,sizeof(fd123.tx_amt),0,fd123.tx_amt);
        }
        vtcp_log("[%s][%d]fd123.tx_amt=[%s]",__FILE__,__LINE__,fd123.tx_amt);
        vtcp_log("[%s][%d]lv_pkgreg.tx_amt=[%f]",__FILE__,__LINE__,lv_pkgreg.tx_amt);
        vtcp_log("[%s][%d]fd123.chrg_ind=[%c] ",__FL__,fd123.chrg_ind[0] );
        apitoa(lv_pkgreg.o_wt_date, sizeof(fd123.o_wt_date),fd123.o_wt_date);
        apitoa(lv_pkgreg.wt_date,sizeof(fd123.wt_date),fd123.wt_date);
        vtcp_log("[%s][%d]fd123.o_wt_date=[%s] ",__FL__,fd123.o_wt_date );
        vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
        pub_base_full_space((char*)&fd123,sizeof(fd123));
        vtcp_log("[%s,%d]1230=[%d]",__FILE__,__LINE__,sizeof(fd123));
        fd123.buff[sizeof(fd123.buff)-1]='\0';
        set_zd_data("1230",(char*)&fd123);
OkExit:
     strcpy(g_pub_tx.reply,"0000");
     sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 0;     
ErrExit:
     sprintf(acErrMsg,"Before ERROR return: reply [%s]",g_pub_tx.reply);
     WRITEMSG
     set_zd_data(DC_REPLY,g_pub_tx.reply);
     return 1;     
}
int Rtn_PAY_Stat_Mesg(char  *Stat,char *stat_content)
{
 char  TRAN_STAT[50][2][30]={
                                                "0","无效状态",
                                                "1","往账录入",
                                                "2","往账复核挂帐",
                                                "3","往账复核记帐",
                                                "4","往账发送",
                                                "5","往账拒绝",
                                                "6","往账轧差",
                                                "7","往账排队",
                                                "8","往账撤销等待",
                                                "9","往账撤销",
                                                "A","往账退汇等待",
                                                "B","往账退回",
                                                "C","往账冲正",
                                                "D","往账止付",
                                                "G","往账异常挂帐",
                                                "H","临时接收不记帐",
                                                "I","正式接收正常记帐",
                                                "J","全国押错",
                                                "K","地方押错",
                                                "L","全国押和地方押都错",
                                                "M","信息不符清算挂帐",
                                                "N","信息不符支行挂帐",
                                                "O","来账退回等待",
                                                "P","来账退回",
                                                "Q","已同意应答且未发送",
                                                "R","已同意应答且已发送",
                                                "S","已不同意应答未发送",
                                                "T","已不同意应答已发送",
                                                "U","来账已冲正",
                                                "V","来账已止付",
                                                "Z","来账异常挂帐",
                                                "\0"
                };
        int j=0;
        char StatMesg[21];
        memset(StatMesg,'\0',sizeof(StatMesg));
        for(j=0;j<50;j++)
        {
                if(strcmp(TRAN_STAT[j][0],Stat)==0)
                {
                        strcpy(StatMesg,TRAN_STAT[j][1]);
                        break;
                }
                if(strcmp(TRAN_STAT[j][0],"\0")==0){
                        return NULL;
               }
        }
     strcpy(stat_content,StatMesg);
        return 0;
}

int Rtn_PAY_Txnum_Mesg(char *txnum,char *txnum_content)
{
vtcp_log("%s,%d",__FILE__,__LINE__);
     char putmsg[21];
     long i=0;
     memset(putmsg,0x00,sizeof(putmsg));
     i=atoi(txnum);
vtcp_log("%s,%d,i=[%ld]",__FILE__,__LINE__,i);
      switch(i)
      {
         case 113:
                                       memcpy(putmsg, "国库资金借记划拨",16);
                                       break;
         case 119:
                                       memcpy(putmsg, "其他借记业务",12);
                                       break;
         case 20103:
                                       memcpy(putmsg, "财政直接支付",12);
                                       break;
         case 20104:
                                       memcpy(putmsg, "财政授权支付",12);
                                       break;
         case 30102:
                    memcpy(putmsg, "支票截留业务",12);
                    break;
         case 30103:
                    memcpy(putmsg, "通用截留业务",12);
                    break;
         case 30104: 
                    memcpy(putmsg, "支票回执业务",12);
                    break;
         case 30105: 
                    memcpy(putmsg, "通用回执业务",12);
                    break;  
                    
         default:   break; 
     }
     strcpy(txnum_content,putmsg); 
vtcp_log("%s,%d putmsg=[%s]",__FILE__,__LINE__,putmsg);
    return 0;
}

