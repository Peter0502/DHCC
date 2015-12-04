/*******************************************************
**   ≥Ã–Úπ¶ƒ‹£∫ µœ÷∂‘÷ß∆±“µŒÒ√˜œ∏µƒ≤È—Ø
**   ≥Ã–Ú√˚≥∆£∫spJ414.c
**   ∫Ø ˝‘≠–Õ£∫spJ414()
**    ‰»Î≤Œ ˝£∫
**    ‰≥ˆ≤Œ ˝£∫
**   Àµ√˜£∫ ±æ≥Ã–Ú µœ÷∂‘÷ß∆±Ωÿ¡Ù,Õ®”√Ωÿ¡Ù“‘º∞»•œ‡”¶ªÿ÷¥“µŒÒµƒ√˜œ∏≤È—Ø°£
**   ◊˜’ﬂ£∫wyw
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
     get_zd_data("0670",txtp);     /** 1∑¢ÀÕ£¨2Ω” ’ **/
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
	sprintf(acErrMsg,"Œ¥’“µΩ∑˚∫œÃıº˛µƒº«¬º!");
	set_zd_data(DC_GET_MSG,acErrMsg);
	WRITEMSG
	strcpy(g_pub_tx.reply,"S047");
	goto ErrExit;
     }
     if(memcmp(lv_pkgreg.txnum,"30102",5)==0 || memcmp(lv_pkgreg.txnum,"30104",5)==0){
     	vtcp_log("begin Ã·»°÷ß∆±∫≈¬Î=====...%s,%d",__FILE__,__LINE__);
	ret=Lv_addit_Sel(g_pub_tx.reply,&lv_addit,"tx_date=%d and tagname='001' and addid='%s'",lv_pkgreg.in_date,lv_pkgreg.addid);
     	if(ret){
       		sprintf(acErrMsg,"Œ¥’“µΩ∑˚∫œÃıº˛µƒº«¬º!");
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
	/**”√¿¥¥´÷ß∆±∫≈¬Î***/
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
  memcpy(fd123.rcp_sts  ,lv_pkgreg.rcpstat,sizeof(fd123.rcp_sts));/**”¶¥◊¥Ã¨**/
  memcpy(fd123.ret_code ,lv_pkgreg.retcode,sizeof(fd123.ret_code));/**”¶¥◊¥Ã¨**/
  memcpy(fd123.pack_id,lv_pkgreg.packid,sizeof(fd123.pack_id));/**‘≠∞¸∫≈**/
  memcpy(fd123.o_or_br_no,lv_pkgreg.o_or_br_no,sizeof(fd123.o_or_br_no));
  vtcp_log("[%s][%d]fd123.o_or_br_no=[%s] ",__FL__,fd123.o_or_br_no );
  memcpy(fd123.o_ac_br_no,lv_pkgreg.o_ac_br_no,sizeof(fd123.o_ac_br_no));
  vtcp_log("[%s][%d]fd123.o_ac_br_no=[%s] ",__FL__,fd123.o_ac_br_no );
  apitoa(lv_pkgreg.pack_date,sizeof(fd123.pack_date),fd123.pack_date);
  apitoa(lv_pkgreg.o_wt_date,sizeof(fd123.o_wt_date),fd123.o_wt_date);/**‘≠∞¸»’∆**/
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
                                                "0","Œﬁ–ß◊¥Ã¨",
                                                "1","Õ˘’À¬º»Î",
                                                "2","Õ˘’À∏¥∫Àπ“’ ",
                                                "3","Õ˘’À∏¥∫Àº«’ ",
                                                "4","Õ˘’À∑¢ÀÕ",
                                                "5","Õ˘’Àæ‹æ¯",
                                                "6","Õ˘’À‘˛≤Ó",
                                                "7","Õ˘’À≈≈∂”",
                                                "8","Õ˘’À≥∑œ˙µ»¥˝",
                                                "9","Õ˘’À≥∑œ˙",
                                                "A","Õ˘’ÀÕÀª„µ»¥˝",
                                                "B","Õ˘’ÀÕÀªÿ",
                                                "C","Õ˘’À≥Â’˝",
                                                "D","Õ˘’À÷π∏∂",
                                                "G","Õ˘’À“Ï≥£π“’ ",
                                                "H","¡Ÿ ±Ω” ’≤ªº«’ ",
                                                "I","’˝ ΩΩ” ’’˝≥£º«’ ",
                                                "J","»´π˙—∫¥Ì",
                                                "K","µÿ∑Ω—∫¥Ì",
                                                "L","»´π˙—∫∫Õµÿ∑Ω—∫∂º¥Ì",
                                                "M","–≈œ¢≤ª∑˚«ÂÀ„π“’ ",
                                                "N","–≈œ¢≤ª∑˚÷ß––π“’ ",
                                                "O","¿¥’ÀÕÀªÿµ»¥˝",
                                                "P","¿¥’ÀÕÀªÿ",
                                                "Q","“—Õ¨“‚”¶¥«“Œ¥∑¢ÀÕ",
                                                "R","“—Õ¨“‚”¶¥«““—∑¢ÀÕ",
                                                "S","“—≤ªÕ¨“‚”¶¥Œ¥∑¢ÀÕ",
                                                "T","“—≤ªÕ¨“‚”¶¥“—∑¢ÀÕ",
                                                "U","¿¥’À“—≥Â’˝",
                                                "V","¿¥’À“—÷π∏∂",
                                                "Z","¿¥’À“Ï≥£π“’ ",
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
                                       memcpy(putmsg, "π˙ø‚◊ ΩΩËº«ªÆ≤¶",16);
                                       break;
         case 119:
                                       memcpy(putmsg, "∆‰À˚ΩËº«“µŒÒ",12);
                                       break;
         case 20103:
                                       memcpy(putmsg, "≤∆’˛÷±Ω”÷ß∏∂",12);
                                       break;
         case 20104:
                                       memcpy(putmsg, "≤∆’˛ ⁄»®÷ß∏∂",12);
                                       break;
         case 30102:
                    memcpy(putmsg, "÷ß∆±Ωÿ¡Ù“µŒÒ",12);
                    break;
         case 30103:
                    memcpy(putmsg, "Õ®”√Ωÿ¡Ù“µŒÒ",12);
                    break;
         case 30104: 
                    memcpy(putmsg, "÷ß∆±ªÿ÷¥“µŒÒ",12);
                    break;
         case 30105: 
                    memcpy(putmsg, "Õ®”√ªÿ÷¥“µŒÒ",12);
                    break;  
                    
         default:   break; 
     }
     strcpy(txnum_content,putmsg); 
vtcp_log("%s,%d putmsg=[%s]",__FILE__,__LINE__,putmsg);
    return 0;
}

