/************************************************************
** 程序功能：实现对支票的历史查询
** 程序名称：spJ413.c
** 函数原型：dh6560023_work()
** 输入参数：
** 输出参数：
** 说   明：
** 作   者：
**********************************************************/
#define ERR_DEAL if(ret){ \
           sprintf(acErrMsg,"sql error.[%d]",ret);\
           WRITEMSG \
          goto ErrExit;\
          }

#define EXTERN
#include "public.h"
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include "lv_addit_c.h"

char txnum_content[20];
char stat_content[21];
char Rcp_content[41];
char ZP_NUM[13];
char ZP_DATE[8];

spJ413()
{
     int ret=0;
     FILE *fp;
     char filename[100];
     long txdate=0;
     char brno[8];
     char acno[51];
     struct lv_pkgreg_c lv_pkgreg;
     struct lv_addit_c  lv_addit_g;
     int i=0;
     char Wlflag[2];
     char Ywtype[2];
     char Zpstat[2];
     char tempstr[512];
     char wherelist[512];
     double tmptxamt=0;
     double tmptxamt2=0;

     memset(ZP_NUM,0x00,sizeof(ZP_NUM));     
     memset(txnum_content,0x00,sizeof(txnum_content));     
     memset(Rcp_content,0x00,sizeof(Rcp_content));     
     memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
     memset(stat_content,'\0',sizeof(stat_content));
     memset(brno,'\0',sizeof(brno));     
     memset(filename,'\0',sizeof(filename));
     memset(acno,'\0',sizeof(acno));
     memset(Wlflag, '\0', sizeof(Wlflag));
     memset(Ywtype,  '\0', sizeof(Ywtype));
     memset(Zpstat,  '\0', sizeof(Zpstat));
     memset(tempstr, '\0', sizeof(tempstr));
     memset(wherelist, '\0', sizeof(wherelist));
 
     get_zd_double("0400",&tmptxamt);
     get_zd_double("0410",&tmptxamt2);
     get_zd_long("0440",&txdate);
     get_zd_data("0640",brno);
     get_zd_data("0670",Wlflag);     /**  1，往帐； 2来帐 **/
     get_zd_data("0680",Ywtype);     /** 0.全部 1.截留业务  2.回执业务 **/
     get_zd_data("0690",Zpstat);
     get_zd_data("0300",acno);

     pub_base_sysinit();

     pub_base_strpack_all(acno);
     pub_base_strpack_all(brno);
     vtcp_log("%s,%d,txdate=[%ld],brno=[%s],Wlflag=[%s],Ywtype=[%s],Zpstat=[%s],acno=[%s]",__FILE__,__LINE__,txdate,brno,Wlflag,Ywtype,Zpstat,acno);     
 sprintf(tempstr, " (txnum='30102' or txnum='30103' or txnum='30104' or txnum='30105')  and  in_date='%ld'", txdate);
 strcat(wherelist, tempstr);

      if(brno[0]!='\0')
      {
             sprintf(tempstr," and br_no ='%s'", brno); 
             strcat(wherelist, tempstr);
      }

      if(acno[0]!='\0')
      { 
          sprintf(tempstr, " and (pay_ac_no='%s' or cash_ac_no='%s')", acno,acno);
             strcat(wherelist,tempstr);
      }

      if(tmptxamt!=0)
      { 
          sprintf(tempstr, " and tx_amt >= %.2lf", tmptxamt);
             strcat(wherelist, tempstr);
      }
      if(tmptxamt2!=0)
      { 
          sprintf(tempstr, " and tx_amt <= %.2lf", tmptxamt2);
             strcat(wherelist, tempstr);
      }

     /** dongxy 判断是来帐还是往帐 **/
      if(Wlflag[0]=='1') /** 往帐 **/
      {
         sprintf(tempstr, " and lw_ind='1'");
         strcat(wherelist,tempstr);
      }else if(Wlflag[0]=='2') /** 来帐 **/
     {
         sprintf(tempstr, " and lw_ind='2'");
         strcat(wherelist,tempstr);
     }


      if(Ywtype[0]=='1')
      {
         sprintf(tempstr, " and pkgno='002'");
         strcat(wherelist,tempstr);
      }
      else if(Ywtype[0]=='2')
      { 
             sprintf(tempstr, " and pkgno='008'");
             strcat(wherelist, tempstr);
      }
/** 董晓燕增加，否则跳转有问题 **/
     else
     {
             sprintf(tempstr, " and (pkgno='008' or pkgno = '002')");
             strcat(wherelist, tempstr);
     }

      if(Zpstat[0]!='0')
      {
            sprintf(tempstr, " and lv_sts='%s' ", Zpstat ); 
            strcat(wherelist, tempstr);
      }
      sprintf(tempstr, "  order by orderno desc");
      strcat(wherelist, tempstr);
     vtcp_log("%s,%d,wherelist=[%s]",__FILE__,__LINE__,wherelist);
     i=0;

     vtcp_log("%s,%d*************",__FILE__,__LINE__);
     ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply,"%s",wherelist);
     ERR_DEAL

     vtcp_log("%s,%d",__FILE__,__LINE__);
     while(1)
     {/*1 while*/
          memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
          memset(stat_content,'\0',sizeof(stat_content));
          memset(txnum_content,0x00,sizeof(txnum_content));     
          
	  vtcp_log("%s,%d",__FILE__,__LINE__);
          ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
          if(ret == 100) break;
          else ERR_DEAL

	  vtcp_log("%s,%d",__FILE__,__LINE__);
          if(!i)
          {
               pub_base_AllDwnFilName( filename );
               fp = fopen( filename,"w" );
               if(!fp)
               {
                    sprintf(acErrMsg," open file [%s] error ",filename );
                    WRITEMSG
                    strcpy( g_pub_tx.reply,"S047" );
                    goto ErrExit;
               }
               fprintf(fp,"~交易行|票据号码|出票日期|委托日期|顺序号|交易金额|状态|发起行号|接收行号|包序号|业务类型|包编号|回执状态|\n");
          }
	  /**取支票号码**/
	  if(memcmp(lv_pkgreg.txnum,"30102",5)==0 || memcmp(lv_pkgreg.txnum,"30104",5)==0){
		memset(&lv_addit_g,0,sizeof(lv_addit_g));
		ret=Lv_addit_Sel(g_pub_tx.reply,&lv_addit_g,"tx_date=%d and tagname='001' and addid='%s'",lv_pkgreg.wt_date,lv_pkgreg.addid);
		if(ret){
			sprintf(acErrMsg,"未找到支票号码!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			/**strcpy(g_pub_tx.reply,"D014");**/
			WRITEMSG;
			/*goto ErrExit;**/
		}
		memset(ZP_NUM,0,sizeof(ZP_NUM));
		memset(ZP_DATE,0,sizeof(ZP_DATE));
		memcpy(ZP_NUM,lv_addit_g.tagdata+8,12);
		memcpy(ZP_DATE,lv_addit_g.tagdata,8);
	  }
	  vtcp_log("%s,%d",__FILE__,__LINE__);
          Rtn_PAY_Txnum_Mesg(lv_pkgreg.txnum,txnum_content);
	  Rtn_PAY_Rcpstat_Mesg(lv_pkgreg.rcpstat,Rcp_content);
     
	  vtcp_log("%s,%d",__FILE__,__LINE__);
        /***对状态进行描述赋值***/
        if(!memcmp(lv_pkgreg.lv_sts, "I", 1) && memcmp(lv_pkgreg.pkgno,PKGNO_PTJJHZ,sizeof(lv_pkgreg.pkgno)-1)==0 && memcmp(lv_pkgreg.rcpstat,"00",2)!=0) 
        {
		memcpy(stat_content,"正常接受，回执未付款",20);
           /**if(lv_pkgreg.resp1[3] == 'I'){
              if(!memcmp(lv_pkgreg.rcpstat, "00", 2))
                  memcpy(stat_content, "成功", 4);
              else if(!memcmp(lv_pkgreg.rcpstat, "01", 2))
                  memcpy(stat_content, "帐号不符", 8);
              else if(!memcmp(lv_pkgreg.rcpstat, "02", 2))
                  memcpy(stat_content, "帐号户名不符", 12);
              else if(!memcmp(lv_pkgreg.rcpstat, "03", 2))
                  memcpy(stat_content, "帐号余额不足支付", 16);
              else if(!memcmp(lv_pkgreg.rcpstat, "10", 2))
                  memcpy(stat_content, "帐号密码错误", 12);
              else if(!memcmp(lv_pkgreg.rcpstat, "11", 2))
                  memcpy(stat_content, "帐号状态错误", 12);
              else if(!memcmp(lv_pkgreg.rcpstat, "20", 2))
                  memcpy(stat_content, "业务已撤销", 10);
              else if(!memcmp(lv_pkgreg.rcpstat, "90", 2))
                  memcpy(stat_content, "其他错误", 8);
           }else{
                Rtn_PAY_Stat_Mesg(lv_pkgreg.lv_sts,stat_content);
	        }**/
     }else{
              Rtn_PAY_Stat_Mesg(lv_pkgreg.lv_sts,stat_content);
	  }
       i++;
	vtcp_log("%s,%d",__FILE__,__LINE__);
     fprintf(fp,"%s|%s|%s|%ld|%s|%.2lf|%s|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,ZP_NUM,ZP_DATE,lv_pkgreg.wt_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,stat_content,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.packid,txnum_content,lv_pkgreg.pkgno,Rcp_content);
      }
     Lv_pkgreg_Clo_Sel();
     if(!i)
     {
          strcpy( g_pub_tx.reply,"S049" );
          goto ErrExit;
     }
     else
     {
          fclose(fp);
          set_zd_data( DC_FILE_SND,"1" );
     }
vtcp_log("%s,%d,iiiiiiiiiiiiiiiiiii=[%d]",__FILE__,__LINE__,i);
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
int Rtn_PAY_Rcpstat_Mesg(char *rcpstat,char *rcp_content)
{
	vtcp_log("[%s][%d],回执状态rcpstat[%s],rcp_content[%s]\n",__FILE__,__LINE__,rcpstat,rcp_content);
	if(memcmp(rcpstat,"00",2)==0){
		strcpy(rcp_content,"成功");
	}else if(!memcmp(rcpstat,"21",2)){
		strcpy(rcp_content,"大小写金额不符");
	}else if(!memcmp(rcpstat,"22",2)){
		strcpy(rcp_content,"支票记载事项不全");	
	}else if(!memcmp(rcpstat,"23",2)){
		strcpy(rcp_content,"出票人签章不符");	
	}else if(!memcmp(rcpstat,"24",2)){
		strcpy(rcp_content,"支付密码错误");	
	}else if(!memcmp(rcpstat,"25",2)){
		strcpy(rcp_content,"持票人未做背书");	
	}else if(!memcmp(rcpstat,"26",2)){
		strcpy(rcp_content,"支票影像不相符");	
	}else if(!memcmp(rcpstat,"27",2)){
		strcpy(rcp_content,"出票人帐号户名不符");	
	}else if(!memcmp(rcpstat,"28",2)){
		strcpy(rcp_content,"出票人余额不足");	
	}else if(!memcmp(rcpstat,"29",2)){
		strcpy(rcp_content,"重复提示付款");	
	}else if(!memcmp(rcpstat,"30",2)){
		strcpy(rcp_content,"非本行票据");	
	}else if(!memcmp(rcpstat,"31",2)){
		strcpy(rcp_content,"出票人已销户");	
	}else if(!memcmp(rcpstat,"32",2)){
		strcpy(rcp_content,"出票人帐户已冻结");	
	}else if(!memcmp(rcpstat,"33",2)){
		strcpy(rcp_content,"出票人帐户已止付");	
	}else if(!memcmp(rcpstat,"34",2)){
		strcpy(rcp_content,"业务已经申请止付");	
	}else if(!memcmp(rcpstat,"35",2)){
		strcpy(rcp_content,"数字签名或证书错");	
	}else if(rcpstat[0]='\0'){
		strcpy(rcp_content," ");	
	}
	return(0);
}
