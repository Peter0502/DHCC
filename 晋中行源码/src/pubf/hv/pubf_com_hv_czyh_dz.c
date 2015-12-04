#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "find_debug.h"
#include "hv_zf_c.h"
#include "hv_zfhtr_c.h"
#include "hv_chkmr_c.h"
#include "hv_chkagemr_c.h"

char *msstrtok(char *instr, char *outstr, char *delimiter);
/****************************************************
村镇银行准备写大额对账文件给商行
*****************************************************/
int pubf_com_hv_write_dz(char *Filename,char *Basename){
	
	int iRc;
	FILE *fp;
	char buf[1024];
	char r_Basename[50];
	char r_Filename[100];
	char r_Date[9];
	struct hv_zf_c r_Hv_zf;
	
	memset(buf,'\0',sizeof(buf));
	memset(r_Basename,'\0',sizeof(r_Basename));
	memset(r_Filename,'\0',sizeof(r_Filename));
	memset(r_Date,'\0',sizeof(r_Date));
	memset(&r_Hv_zf,'\0',sizeof(struct hv_zf_c));
	
	sprintf(r_Basename,"hv_dz",5);
	sprintf(r_Basename+5,"_%s_",HV_CUNZHEN_BR_NO);/**add by xyy 2010-6-20 10:48:24增加对多个村镇银行的支持**/
	sprintf(r_Basename+12,"%08ld",g_pub_tx.tx_date);
	sprintf(r_Basename+20,".txt",4);
	sprintf(r_Filename,"%s/%s",(char*)getenv("CZYH_PATH"),r_Basename);
	memcpy(Filename,r_Filename,sizeof(r_Filename)-1);/*返回带路径的文件名*/
	memcpy(Basename,r_Basename,sizeof(r_Basename)-1);/*返回不带路径的文件名*/
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	
	fp=fopen(r_Filename,"w+");
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	if(fp==NULL){
		vtcp_log("%s,%d open file error",__FILE__,__LINE__);
		fclose(fp);
		return(-1);
	}
	vtcp_log("%s,%d filename=%s",__FILE__,__LINE__,r_Filename);
	iRc=Hv_zf_Dec_Upd(g_pub_tx.reply,"tx_date=%ld and lw_ind='%s' and hv_sts in ('1','2','3','4','7') ",g_pub_tx.tx_date,"1");
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	if(iRc && iRc!=100){
			vtcp_log("%s,%d hv_zf_dec_upd error iRc=%d",__FILE__,__LINE__,iRc);
			Hv_zf_Clo_Upd();
			fclose(fp);
			return(iRc);
	}
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	while(1){
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		memset(&r_Hv_zf,'\0',sizeof(struct hv_zf_c));
		iRc=Hv_zf_Fet_Upd(&r_Hv_zf,g_pub_tx.reply);
		if(iRc == 100)
		{
			Hv_zf_Clo_Upd();
			break;
		}else if(iRc){
			vtcp_log("%s,%d hv_zf_fet_upd error iRc=%d",__FILE__,__LINE__,iRc);
			Hv_zf_Clo_Upd();
			fclose(fp);
			return(iRc);
		}
		
		else{
			/***add by xyy 2010-6-20 10:53:21 注意这里用的是HV_CUNZHEN_BR_NO,每个村镇银行要设置好自己的参数,不能重复,谢谢****/
			sprintf(buf,"%s|%ld|%ld|%ld|%s|%s|%s|%s|%f|%s|%f|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|%ld|%s|%ld|%ld|%s|%s|%s|%s|%s|%ld|%s|%ld|%s|%ld|%ld|%s|%s|%s\n",HV_CUNZHEN_BR_NO,r_Hv_zf.tx_date,r_Hv_zf.wt_date,r_Hv_zf.tx_time,r_Hv_zf.orderno,r_Hv_zf.cur_no,r_Hv_zf.note_type,r_Hv_zf.note_no,r_Hv_zf.tx_amt,r_Hv_zf.tx_chrg_ind,r_Hv_zf.chrg_amt,r_Hv_zf.pay_qs_no,r_Hv_zf.or_br_no,r_Hv_zf.pay_opn_br_no,r_Hv_zf.pay_ac_no,r_Hv_zf.pay_name,r_Hv_zf.pay_addr,r_Hv_zf.cash_qs_no,r_Hv_zf.ac_br_no,r_Hv_zf.cash_opn_br_no,r_Hv_zf.cash_ac_no,r_Hv_zf.cash_name,r_Hv_zf.cash_addr,r_Hv_zf.yw_type,r_Hv_zf.sendco,r_Hv_zf.receco,r_Hv_zf.cmtno,r_Hv_zf.beg_sts,r_Hv_zf.operlevel,r_Hv_zf.hv_sts,r_Hv_zf.tel,r_Hv_zf.trace_no,r_Hv_zf.hv_osts,r_Hv_zf.checkflag,r_Hv_zf.hv_prt_ind,r_Hv_zf.resend_ind,r_Hv_zf.resend_date,r_Hv_zf.o_orderno,r_Hv_zf.o_tx_date,r_Hv_zf.o_wt_date,r_Hv_zf.o_cmtno,r_Hv_zf.lw_ind,r_Hv_zf.addid,r_Hv_zf.refno,r_Hv_zf.auth_tel,r_Hv_zf.auth_trace_no,r_Hv_zf.chk,r_Hv_zf.chk_trace_no,r_Hv_zf.send_tel,r_Hv_zf.send_tel_trace_no,r_Hv_zf.dealerr_date,r_Hv_zf.dealerr_tel,r_Hv_zf.resp1,r_Hv_zf.hv_brf);
		
			iRc=fwrite(buf,strlen(buf),1,fp);
		}
	}
	vtcp_log("%s,%d ",__FILE__,__LINE__);
	Hv_zf_Clo_Upd();
	fclose(fp);
	return(0);
}
/**********************************************************
商行 导入村镇银行发来的大额往帐数据

modify  by xyy 2010-6-20 10:48:24增加对多个村镇银行的支持
***********************************************************/

int pubf_com_hv_read_dz(char * tx_br_no){
	
	int iRc;
	FILE *fp;
	char buf[1025];
	char *p_buf;
	char tmp[100];
	char cCmd[300];
	char cBasename[100];
	char s_Filename[200];
	char cKinbr[BRNO_LEN+1];
	struct hv_zf_c s_Hv_zf;
	
	memset(buf,'\0',sizeof(buf));
	memset(tmp,'\0',sizeof(tmp));
	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(cCmd, 0 , sizeof(cCmd));
	memset(cBasename, 0 , sizeof(cBasename));
	memset(s_Filename,'\0',sizeof(s_Filename));
	memset(&s_Hv_zf,'\0',sizeof(struct hv_zf_c));
	
	memcpy(cKinbr,tx_br_no , BRNO_LEN);
	
	sprintf(cBasename,"hv_dz_%s_%08ld.txt",cKinbr,g_pub_tx.tx_date);
	sprintf(s_Filename,"%s/hv_dz_%s_%08ld.txt",(char*)getenv("YQSH_PATH"),cKinbr,g_pub_tx.tx_date);
	
	sprintf(cCmd,"ftprecv %s %s",(char*)getenv("YQSH_PATH"),cBasename);
	/* 下传对账文件到村镇银行 */
	iRc = system(cCmd);
	if(iRc)
	{
		vtcp_log("%s,%d 从村镇银行得到文件失败!!!![%d]",__FILE__,__LINE__,iRc);
		strncpy(g_pub_tx.reply,"P157",4);
		return iRc;
	}
	/* 生产与村镇银行清算的数据 */
	vtcp_log("[%s][%d] 注意:filename ==%s",__FILE__,__LINE__,s_Filename);
	fp=fopen(s_Filename,"r");
	if(fp==NULL){
		vtcp_log("%s,%d open file error",__FILE__,__LINE__);
		fclose(fp);
		return(-1);
	}
	vtcp_log("%s,%d  filename %s",__FILE__,__LINE__,s_Filename);
	while(1){
		memset(buf,'\0',sizeof(buf));
		memset(&s_Hv_zf,'\0',sizeof(struct hv_zf_c));
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		p_buf=fgets(buf,1024,fp);
		vtcp_log("%s,%d len_buf=%ld ",__FILE__,__LINE__,strlen(buf));
		if (strlen(buf)<5){
			break;
		}
		vtcp_log("%s,%d ",__FILE__,__LINE__);
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.br_no,tmp,sizeof(s_Hv_zf.br_no)-1);
		
		if(memcmp(s_Hv_zf.br_no,cKinbr,BRNO_LEN)!=0)/****add by xyy 2010-6-20 10:56:32 增加对文件内容的判断****/
		{
			vtcp_log("[%s][%d] 要处理文件的机构[%s]和文件中的机构[%s]不符!!!!!",__FILE__,__LINE__,cKinbr,s_Hv_zf.br_no);
			fclose(fp);
			return(-1);
		}
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.tx_date=atol(tmp);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.wt_date=atol(tmp);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.tx_time=atol(tmp);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.orderno,tmp,sizeof(s_Hv_zf.orderno)-1);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cur_no,tmp,sizeof(s_Hv_zf.cur_no)-1);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.note_type,tmp,sizeof(s_Hv_zf.note_type)-1);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.note_no,tmp,sizeof(s_Hv_zf.note_no)-1);
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.tx_amt=atof(tmp);/*交易金额*/
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.tx_chrg_ind,tmp,sizeof(s_Hv_zf.tx_chrg_ind)-1);
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.chrg_amt=atof(tmp);/*手续费*/
						
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.pay_qs_no,tmp,sizeof(s_Hv_zf.pay_qs_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.or_br_no,tmp,sizeof(s_Hv_zf.or_br_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.pay_opn_br_no,tmp,sizeof(s_Hv_zf.pay_opn_br_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.pay_ac_no,tmp,sizeof(s_Hv_zf.pay_ac_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.pay_name,tmp,sizeof(s_Hv_zf.pay_name)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.pay_addr,tmp,sizeof(s_Hv_zf.pay_addr)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cash_qs_no,tmp,sizeof(s_Hv_zf.cash_qs_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.ac_br_no,tmp,sizeof(s_Hv_zf.ac_br_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cash_opn_br_no,tmp,sizeof(s_Hv_zf.cash_opn_br_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cash_ac_no,tmp,sizeof(s_Hv_zf.cash_ac_no)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cash_name,tmp,sizeof(s_Hv_zf.cash_name)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cash_addr,tmp,sizeof(s_Hv_zf.cash_addr)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.yw_type,tmp,sizeof(s_Hv_zf.yw_type)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.sendco,tmp,sizeof(s_Hv_zf.sendco)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.receco,tmp,sizeof(s_Hv_zf.receco)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.cmtno,tmp,sizeof(s_Hv_zf.cmtno)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.beg_sts,tmp,sizeof(s_Hv_zf.beg_sts)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.operlevel,tmp,sizeof(s_Hv_zf.operlevel)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.hv_sts,tmp,sizeof(s_Hv_zf.hv_sts)-1);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.tel,tmp,sizeof(s_Hv_zf.tel)-1);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.trace_no=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.hv_osts,tmp,sizeof(s_Hv_zf.hv_osts)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.checkflag,tmp,sizeof(s_Hv_zf.checkflag)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.hv_prt_ind,tmp,sizeof(s_Hv_zf.hv_prt_ind)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.resend_ind,tmp,sizeof(s_Hv_zf.resend_ind)-1);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.resend_date=atol(tmp);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.o_orderno,tmp,sizeof(s_Hv_zf.o_orderno)-1);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.o_tx_date=atol(tmp);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.o_wt_date=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.o_cmtno,tmp,sizeof(s_Hv_zf.o_cmtno)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.lw_ind,tmp,sizeof(s_Hv_zf.lw_ind)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.addid,tmp,sizeof(s_Hv_zf.addid)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.refno,tmp,sizeof(s_Hv_zf.refno)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.auth_tel,tmp,sizeof(s_Hv_zf.auth_tel)-1);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.auth_trace_no=atol(tmp);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.chk,tmp,sizeof(s_Hv_zf.chk)-1);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.chk_trace_no=atol(tmp);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.send_tel,tmp,sizeof(s_Hv_zf.send_tel)-1);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.send_tel_trace_no=atol(tmp);
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Hv_zf.dealerr_date=atol(tmp);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.dealerr_tel,tmp,sizeof(s_Hv_zf.dealerr_tel)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.resp1,tmp,sizeof(s_Hv_zf.resp1)-1);
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Hv_zf.hv_brf,tmp,sizeof(s_Hv_zf.hv_brf)-1);
				
		Hv_zf_Debug(&s_Hv_zf);
		iRc=Hv_zf_Ins(s_Hv_zf,g_pub_tx.reply);
		if(iRc==-239){
			continue;
		}
		else if(iRc){
				vtcp_log("%s,%d Hv_zf_Ins error",__FILE__,__LINE__);
				fclose(fp);
				return(iRc);
		}
	}
	
	fclose(fp);
	return(0);
}
/*************************************
村镇银行需要导入商行发来的对账文件
**************************************/
int pubf_com_hv_czyh_read_dz(){
	
	int iRc;
	FILE *fp;
	char buf[1024];
	char *p_buf=buf;
	char tmp[100];
	char s_Filename[200];
	struct hv_chkmr_c s_Hv_chkmr;
	struct hv_chkagemr_c s_Hv_chkagemr;
	
	memset(buf,'\0',sizeof(buf));
	memset(tmp,'\0',sizeof(tmp));
	memset(s_Filename,'\0',sizeof(s_Filename));
	memset(&s_Hv_chkmr,'\0',sizeof(struct hv_chkmr_c));
	memset(&s_Hv_chkagemr,'\0',sizeof(struct hv_chkagemr_c));
	
	 sprintf(s_Filename,"%s/hv_xc_dz_%s_%08ld.txt",(char*)getenv("CZYH_PATH"),HV_CUNZHEN_BR_NO,g_pub_tx.tx_date);

	fp=fopen(s_Filename,"r");
	if(fp==NULL){
		vtcp_log("%s,%d open file error filename[%s]",__FILE__,__LINE__,s_Filename);
		fclose(fp);
		return(-1);
	}
	
	p_buf=fgets(buf,1024,fp);
	if(strlen(buf)<5){
		vtcp_log("%s,%d read buf error buf[%s]",__FILE__,__LINE__,buf);
		fclose(fp);
		return(-1);	
	}

	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.tx_date=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	memcpy(s_Hv_chkmr.ccpc_no,tmp,sizeof(s_Hv_chkmr.ccpc_no));
		
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.send_cnt=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.send_amt=atof(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.send_item=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	memcpy(s_Hv_chkmr.send_dtl,tmp,sizeof(s_Hv_chkmr.send_dtl));
		
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.recevice_cnt=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.recevice_amt=atof(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.recevice_item=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	memcpy(s_Hv_chkmr.recevice_dtl,tmp,sizeof(s_Hv_chkmr.recevice_dtl));
		
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.online_cnt=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.online_amt=atof(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	s_Hv_chkmr.online_item=atol(tmp);
	
	memset(tmp,'\0',sizeof(tmp));
	p_buf=msstrtok(p_buf,tmp,"|");
	memcpy(s_Hv_chkmr.online_dtl,tmp,sizeof(s_Hv_chkmr.online_dtl));
		
	iRc=Hv_chkmr_Ins(s_Hv_chkmr,g_pub_tx.reply);
	if(iRc==-239){
		}
	else if(iRc){
			vtcp_log("%s,%d Hv_chkmr_Ins error",__FILE__,__LINE__);
			fclose(fp);
			return(iRc);
	}
	
	if(s_Hv_chkmr.send_cnt==0 && s_Hv_chkmr.recevice_cnt==0 && s_Hv_chkmr.online_cnt==0){
		fclose(fp);
		return(0);
	}else{

		while(1){
			memset(buf,'\0',sizeof(buf));
			p_buf=fgets(buf,1024,fp);
			if (strlen(buf)<5){
				break;
			}
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.orderno,tmp,sizeof(s_Hv_chkagemr.orderno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.pay_qs_no,tmp,sizeof(s_Hv_chkagemr.pay_qs_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.br_no,tmp,sizeof(s_Hv_chkagemr.br_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.tx_date=atol(tmp);
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cmtno,tmp,sizeof(s_Hv_chkagemr.cmtno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.o_cmtno,tmp,sizeof(s_Hv_chkagemr.o_cmtno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.tx_time=atol(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cur_no,tmp,sizeof(s_Hv_chkagemr.cur_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.tx_amt=atof(tmp);
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.other_amt=atof(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.jf_amt=atof(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.or_br_no,tmp,sizeof(s_Hv_chkagemr.or_br_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.pay_no,tmp,sizeof(s_Hv_chkagemr.pay_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.pay_ac_no,tmp,sizeof(s_Hv_chkagemr.pay_ac_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.payname,tmp,sizeof(s_Hv_chkagemr.payname));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.payaddress,tmp,sizeof(s_Hv_chkagemr.payaddress));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cashqsactno,tmp,sizeof(s_Hv_chkagemr.cashqsactno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.acbrno,tmp,sizeof(s_Hv_chkagemr.acbrno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cashno,tmp,sizeof(s_Hv_chkagemr.cashno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cash_ac_no,tmp,sizeof(s_Hv_chkagemr.cash_ac_no));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cash_name,tmp,sizeof(s_Hv_chkagemr.cash_name));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.cash_addr,tmp,sizeof(s_Hv_chkagemr.cash_addr));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.yw_type,tmp,sizeof(s_Hv_chkagemr.yw_type));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.sendco,tmp,sizeof(s_Hv_chkagemr.sendco));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.receco,tmp,sizeof(s_Hv_chkagemr.receco));
		

			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.operlevel,tmp,sizeof(s_Hv_chkagemr.operlevel));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.obrno,tmp,sizeof(s_Hv_chkagemr.obrno));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.voctype,tmp,sizeof(s_Hv_chkagemr.voctype));
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.vocnum,tmp,sizeof(s_Hv_chkagemr.vocnum));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.p_date=atol(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.p_num,tmp,sizeof(s_Hv_chkagemr.p_num));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.hp_type,tmp,sizeof(s_Hv_chkagemr.hp_type));
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.pass_wd,tmp,sizeof(s_Hv_chkagemr.pass_wd));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.inst_rate=atol(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.term=atol(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.o_tx_date,tmp,sizeof(s_Hv_chkagemr.o_tx_date));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.ou_tx_date,tmp,sizeof(s_Hv_chkagemr.ou_tx_date));
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.o_orderno_hv,tmp,sizeof(s_Hv_chkagemr.o_orderno_hv));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.o_tx_type,tmp,sizeof(s_Hv_chkagemr.o_tx_type));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.p_orderno_hv,tmp,sizeof(s_Hv_chkagemr.p_orderno_hv));
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.dc,tmp,sizeof(s_Hv_chkagemr.dc));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.pay_date=atol(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			s_Hv_chkagemr.intst=atof(tmp);
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.reason,tmp,sizeof(s_Hv_chkagemr.reason));
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.repson,tmp,sizeof(s_Hv_chkagemr.repson));
		
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.chk_flg,tmp,sizeof(s_Hv_chkagemr.chk_flg));
			
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.brf1,tmp,sizeof(s_Hv_chkagemr.brf1));
					
			memset(tmp,'\0',sizeof(tmp));
			p_buf=msstrtok(p_buf,tmp,"|");
			memcpy(s_Hv_chkagemr.brf2,tmp,sizeof(s_Hv_chkagemr.brf2));
		
			iRc=Hv_chkagemr_Ins(s_Hv_chkagemr,g_pub_tx.reply);
			if(iRc==-239){
				continue;
			}
			else if(iRc){
					vtcp_log("%s,%d Hv_zf_Ins error",__FILE__,__LINE__);
					fclose(fp);
					return(iRc);
			}

		}
	}
	fclose(fp);
	return(0);	
	
}


/*****
功能：取得instr中第一个分隔符delimiter前的内容
参数： outstr指向instr中第一个delimiter之前的内容
返回值：返回下一个delimiter的位置
***************/
char * strstr(char *,char *);
char *msstrtok(char *instr, char *outstr, char *delimiter)
{
  char *tmpstr;

  if(memcmp(instr, delimiter, strlen(instr))==0){
    return(NULL);
  }
  if(instr==NULL || strlen(instr)==0){return(NULL);}
  tmpstr=strstr(instr,delimiter);
  if(tmpstr!=NULL){
    memcpy(outstr, instr, strlen(instr)-strlen(tmpstr));
    return(strstr(instr,delimiter)+strlen(delimiter));
  }else{
    memcpy(outstr, instr, strlen(instr));
    return(NULL);
  }
}
