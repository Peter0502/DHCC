/*******************************************************
 * 程 序 名:  sp6330()  
 * 程序功能： 卡业务对帐程序
 * 作    者： ChenMing
 * 开发日期： 20070227
 * 修 改 人：
 * 修改日期：
 *******************************************************/

#define EXTERN
#include	<stdio.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<stropts.h>
#include 	<poll.h>
#include 	<sys/ipc.h>
#include 	<sys/msg.h>
#include 	<errno.h>
#include    <stdlib.h>
#include	<memory.h>
#include	<fcntl.h>
#include	<sys/socket.h>
#include	<time.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<string.h>
#include "public.h"
#include "card_check_sheet_c.h"

#define FILE_DIR "/home/newsys/"

int sp6330()
{
	  int i=0;
	  long sys_date;
	  char cFullFileName[300];
	  char cFilename[12][100];
	  char line[500];
	  FILE *fp;
	  /* 1. 读文件，写入太原中心对帐表Card_check_sheet */	  
		vtcp_log("卡业务对帐程序开始 !!!\n\n");	
		memset(cFilename, 0, sizeof(20*100));		
		get_zd_long(DC_DATE,&sys_date);

		memcpy(cFilename[0], "/fromCup/ATMACQRTR", 100);
		memcpy(cFilename[1], "/fromCup/ATMISSRTR", 100);	
		memcpy(cFilename[2], "/fromCup/POSACQRTR", 100);
		memcpy(cFilename[3], "/fromCup/POSISSRTR", 100);	

		memcpy(cFilename[4], "/fromLocal/ATMACQRTL", 100);
		memcpy(cFilename[5], "/fromLocal/ATMISSRTL", 100);
		memcpy(cFilename[6], "/fromLocal/POSACQRTL", 100);
		memcpy(cFilename[7], "/fromLocal/POSISSRTL", 100);		
				
		memcpy(cFilename[8], "/fromTyCup/ATMACQRTT", 100);	
		memcpy(cFilename[9], "/fromTyCup/ATMISSRTT", 100);				
		memcpy(cFilename[10], "/fromTyCup/POSACQRTT", 100);	
		memcpy(cFilename[11], "/fromTyCup/POSISSRTT", 100);	
						
		for(i=0; i<12; i++)
		{			
			  memset(cFullFileName, 0, sizeof(cFullFileName));
				sprintf(cFullFileName, "%s%ld%s", FILE_DIR, sys_date, cFilename[i]);
		    vtcp_log("cFullFileName=======[%s]", cFullFileName);	 
			  
			  fp=fopen(cFullFileName,"rb");
      	if(fp==NULL)
      	{
      		 vtcp_log("无法打开文件[%s]\n",cFullFileName);
      		 goto ErrExit;
      	}	      		 
      
      	memset(line, 0, sizeof(line));
      	while(fgets(line, sizeof(line)-1, fp)!=NULL)
      	{
      		  vtcp_log("strlen(line)==========[%d]\n",strlen(line));
						if(strlen(line)==0)		break;
						
						if(vInsert_Card_Check_Sheet(line))
					  {
      		 			vtcp_log("line==========[%s]\n",line);
      		 			vtcp_log("vInsert_Card_Check_Sheet() 出错！\n");
      		 			goto ErrExit;
					  }					
				}	
				fclose(fp);
	  }
		vtcp_log("读文件，写入太原中心对帐表成功 !!");	
		
		/* 2. 与自助设备交易日记表chnl_self_trade比较，列出两者不一致的记录 */		
		
		
		
		
OkExit:
	strcpy(g_pub_tx.reply,"0000");
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


int vInsert_Card_Check_Sheet(char *cLine)
{
	  int ret=0;
	  vtcp_log("strlen(cLine)==========[%d]\n",strlen(cLine));
	  struct card_check_sheet_c sCard_check_sheet;
	  
	  memset(&sCard_check_sheet, 0, sizeof(sCard_check_sheet));
	  
	  memcpy(sCard_check_sheet.proxy_br_no 	    , cLine+0,   sizeof(sCard_check_sheet.proxy_br_no 	  ));
	  memcpy(sCard_check_sheet.sys_traceno 	    , cLine+12,  sizeof(sCard_check_sheet.sys_traceno 	  ));
	  memcpy(sCard_check_sheet.tx_time          , cLine+19,  sizeof(sCard_check_sheet.tx_time         ));	  
	  memcpy(sCard_check_sheet.ac_no 				    , cLine+30,  sizeof(sCard_check_sheet.ac_no 				  ));	  
	  memcpy(sCard_check_sheet.tx_amt 				  , cLine+50,  sizeof(sCard_check_sheet.tx_amt 				  ));	  
	  memcpy(sCard_check_sheet.pack_type 		    , cLine+63,  sizeof(sCard_check_sheet.pack_type 		  ));	  
	  memcpy(sCard_check_sheet.tx_type		 		  , cLine+68,  sizeof(sCard_check_sheet.tx_type		 		  ));	  	
	  memcpy(sCard_check_sheet.merch_type		    , cLine+75,  sizeof(sCard_check_sheet.merch_type		  ));
	  memcpy(sCard_check_sheet.term_no		      , cLine+80,  sizeof(sCard_check_sheet.term_no		      ));
	  memcpy(sCard_check_sheet.retrive_refno	  , cLine+89,  sizeof(sCard_check_sheet.retrive_refno	  ));
	  memcpy(sCard_check_sheet.serv_cond_no	    , cLine+102, sizeof(sCard_check_sheet.serv_cond_no	  ));	  
	  memcpy(sCard_check_sheet.auth_resp_no	    , cLine+105, sizeof(sCard_check_sheet.auth_resp_no	  ));	  
	  memcpy(sCard_check_sheet.recv_br_no  	    , cLine+112, sizeof(sCard_check_sheet.recv_br_no  	  ));	  
	  memcpy(sCard_check_sheet.old_sys_traceno	, cLine+125, sizeof(sCard_check_sheet.old_sys_traceno	));	  
	  memcpy(sCard_check_sheet.old_tx_time	    , cLine+132, sizeof(sCard_check_sheet.old_tx_time	    ));	  	
	  memcpy(sCard_check_sheet.tx_resp_code     , cLine+143, sizeof(sCard_check_sheet.tx_resp_code    ));
	  memcpy(sCard_check_sheet.input_mode       , cLine+146, sizeof(sCard_check_sheet.input_mode      ));
	  memcpy(sCard_check_sheet.get_fee          , cLine+150, sizeof(sCard_check_sheet.get_fee         ));
	  memcpy(sCard_check_sheet.pay_fee          , cLine+163, sizeof(sCard_check_sheet.pay_fee         ));	  
	  memcpy(sCard_check_sheet.trans_serv_fee   , cLine+176, sizeof(sCard_check_sheet.trans_serv_fee  ));	  
	  memcpy(sCard_check_sheet.cardholder_fee   , cLine+189, sizeof(sCard_check_sheet.cardholder_fee  ));	  
	  memcpy(sCard_check_sheet.err_get_fee      , cLine+202, sizeof(sCard_check_sheet.err_get_fee     ));	  
	  memcpy(sCard_check_sheet.err_pay_fee      , cLine+215, sizeof(sCard_check_sheet.err_pay_fee     ));	  	
	  memcpy(sCard_check_sheet.err_tx_flg       , cLine+228, sizeof(sCard_check_sheet.err_tx_flg      ));                                        
	  memcpy(sCard_check_sheet.transout_brno    , cLine+232, sizeof(sCard_check_sheet.transout_brno   ));
	  memcpy(sCard_check_sheet.transout_cardno  , cLine+244, sizeof(sCard_check_sheet.transout_cardno ));
	  memcpy(sCard_check_sheet.transin_brno     , cLine+264, sizeof(sCard_check_sheet.transin_brno    ));	  
	  memcpy(sCard_check_sheet.transin_cardno   , cLine+276, sizeof(sCard_check_sheet.transin_cardno  ));	  
	  memcpy(sCard_check_sheet.card_serial_no   , cLine+296, sizeof(sCard_check_sheet.card_serial_no  ));	  
	  memcpy(sCard_check_sheet.term_capbility   , cLine+300, sizeof(sCard_check_sheet.term_capbility  ));	  
	  memcpy(sCard_check_sheet.card_cond_code   , cLine+302, sizeof(sCard_check_sheet.card_cond_code  ));	  	
	  memcpy(sCard_check_sheet.place_flg        , cLine+304, sizeof(sCard_check_sheet.place_flg       ));
	  memcpy(sCard_check_sheet.tx_start_chnl    , cLine+306, sizeof(sCard_check_sheet.tx_start_chnl   ));
	  memcpy(sCard_check_sheet.reserved         , cLine+309, sizeof(sCard_check_sheet.reserved        ));
	  
	  vtcp_log("sCard_check_sheet.sys_traceno========[%s]", sCard_check_sheet.sys_traceno);	                                        
  	ret = Card_check_sheet_Ins(sCard_check_sheet, g_pub_tx.reply);
	  if ( ret )
	  {
	  	  vtcp_log("Card_check_sheet_Ins() 出错 !!");
  	      return -1;
	  }	
	  vtcp_log("Card_check_sheet_Ins() 插入一条记录 !!");	
	  
	  return 0;
}
