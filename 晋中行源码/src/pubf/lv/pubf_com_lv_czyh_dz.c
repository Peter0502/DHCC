/***************************************************************************/
/* �ļ�����: pubf_com_lv_czyh_dz.c                                         */
/* ��������: С��֧�������ļ�������                                      */
/* ��������: 2009.05.14  .                                                 */
/* ����:      CGX                                                          */
/* ˵��:    ����ֵFilenameΪ����·�����ļ�����BasenameΪ����·���ļ���     */
/***************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hv_define.h"
#include "lv_define.h"
#include "lv_tito.h"
#include "lv_pub.h"
#include "hv_orno_brno_c.h"
#include "lv_newxyh_c.h"
#include "lv_addit_c.h"
#include "lv_sysctl_c.h"
#include "seqctl_c.h"
#include "lv_fd123.h"
#include "public.h"
#include "lv_add.h"
#include "lv_wbctl_c.h"

 char *msstrtok(char *instr, char *outstr, char *delimiter);
/***********************************************************
��������׼��дС������ļ�������
************************************************************/
int pubf_com_lv_write_dz(char *Filename,char *Basename){
	
	int iRc;
	FILE *fp;
	char buf[1024];
	char r_Basename[50];
	char r_Filename[200];
	char r_Date[9];
	struct lv_wbctl_c r_Lv_wbctl;
	
	memset(buf,'\0',sizeof(buf));
	memset(r_Basename,'\0',sizeof(r_Basename));
	memset(r_Filename,'\0',sizeof(r_Filename));
	memset(r_Date,'\0',sizeof(r_Date));
	memset(&r_Lv_wbctl,'\0',sizeof(struct lv_wbctl_c));
	
	sprintf(r_Basename,"lv_dz",5);
	sprintf(r_Basename+5,"_%s_",HV_CUNZHEN_BR_NO);/**add by xyy 2010-6-20 10:48:24���ӶԶ���������е�֧��**/
	sprintf(r_Basename+12,"%08ld",g_pub_tx.tx_date);
	sprintf(r_Basename+20,".txt",4);
	sprintf(r_Filename,"%s/%s",(char*)getenv("CZYH_PATH"),r_Basename);
	memcpy(Filename,r_Filename,sizeof(r_Filename));/*���ش�·�����ļ���*/
	memcpy(Basename,r_Basename,sizeof(r_Basename));/*���ز���·�����ļ���*/
	
	vtcp_log("%s,%d  file name  %s",__FILE__,__LINE__,r_Filename);
	fp=fopen(r_Filename,"w+");
	vtcp_log("%s,%d",__FILE__,__LINE__);
	if(fp==NULL){
		vtcp_log("%s,%d  %s",__FILE__,__LINE__,r_Filename);
		vtcp_log("%s,%d,open file error",__FILE__,__LINE__);
		fclose(fp);
		return(-1);
	}
	vtcp_log("%s,%d,open file error",__FILE__,__LINE__);
	iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply,"qs_date=%ld and packstat in ('02','03','04','05')",0);
	vtcp_log("%s,%d,open file error",__FILE__,__LINE__);
	if(iRc && iRc!=100){
			vtcp_log("%s,%d, Lv_wbctl_dec_upd error=%d\n",__FILE__,__LINE__,iRc);
			Lv_wbctl_Clo_Upd();
			fclose(fp);
			return(iRc);
	}
	while(1){
		vtcp_log("%s,%d,open file ",__FILE__,__LINE__);
		
		iRc=Lv_wbctl_Fet_Upd(&r_Lv_wbctl,g_pub_tx.reply);
		vtcp_log("%s,%d",__FILE__,__LINE__);
		if(iRc==100){
			Lv_wbctl_Clo_Upd();
			break;
		}else if(iRc){
			vtcp_log("%s,%d Lv_wbctl_fet_upd ",__FILE__,__LINE__);
			Lv_wbctl_Clo_Upd();
			fclose(fp);
			return(iRc);
		}
		else{
			memset(buf,'\0',sizeof(buf));
			sprintf(buf,"%ld|%s|%s|%s|%s|%ld|%s|%ld|%.2f|%s|%ld|%s|%ld|%ld|%s|%ld|%.2f|%s|%s|%s|%s|%s|%s|%ld|%s|%s|%s|%s|%s|%s|%s\n",
				r_Lv_wbctl.pack_date,r_Lv_wbctl.packid,r_Lv_wbctl.pkgno,r_Lv_wbctl.pay_qs_no,r_Lv_wbctl.cash_qs_no,r_Lv_wbctl.in_date,r_Lv_wbctl.packmy,r_Lv_wbctl.dtlcnt,r_Lv_wbctl.totamt,r_Lv_wbctl.zcnode,r_Lv_wbctl.zc_date,r_Lv_wbctl.zcid,r_Lv_wbctl.hz_date,r_Lv_wbctl.o_pack_date,r_Lv_wbctl.o_packid,r_Lv_wbctl.dtlsuccnt,r_Lv_wbctl.dtlsucamt,r_Lv_wbctl.o_pkgno,r_Lv_wbctl.ornode,r_Lv_wbctl.acnode,r_Lv_wbctl.ffflag,r_Lv_wbctl.txnum,r_Lv_wbctl.bfflag,r_Lv_wbctl.qs_date,r_Lv_wbctl.packstat,r_Lv_wbctl.stat,r_Lv_wbctl.brprtflag,r_Lv_wbctl.qsprtflag,r_Lv_wbctl.content,HV_CUNZHEN_BR_NO,r_Lv_wbctl.filler);
		
		fwrite(buf,strlen(buf),1,fp); /**modify by xyy 2009-08-04 ���ﲻ����sizeof**/
		
		}
	}
		Lv_wbctl_Clo_Upd();
	
	fclose(fp);
	return(0);
}

/************************
���� ���������������ʱӦ��
************************/

int pubf_com_lv_read_dz(char * tx_br_no){
	
	int iRc;
	FILE *fp;
	char buf[1024];
	char *p_buf=buf;
	char tmp[100];
	char cCmd[300];
	char cBasename[100];
	char s_Filename[200];
	char cKinbr[BRNO_LEN+1];
	struct lv_wbctl_c s_Lv_wbctl;
	struct lv_wbctl_c x_Lv_wbctl;
	
	memset(buf,'\0',sizeof(buf));
	memset(tmp,'\0',sizeof(tmp));
	memset(cCmd, 0 , sizeof(cCmd));
	memset(cBasename, 0 , sizeof(cBasename));
	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(s_Filename,'\0',sizeof(s_Filename));
	memset(&s_Lv_wbctl,'\0',sizeof(struct lv_wbctl_c));
	memset(&x_Lv_wbctl,'\0',sizeof(struct lv_wbctl_c));
	
	memcpy(cKinbr,tx_br_no , BRNO_LEN);

	sprintf(cBasename,"lv_dz_%s_%08ld.txt",cKinbr,g_pub_tx.tx_date);
	sprintf(s_Filename,"%s/lv_dz_%s_%08ld.txt",(char*)getenv("YQSH_PATH"),cKinbr,g_pub_tx.tx_date);

	sprintf(cCmd,"ftprecv %s %s",(char*)getenv("YQSH_PATH"),cBasename);
	/* �´������ļ����������� */
	iRc = system(cCmd);
	if(iRc)
	{
		vtcp_log("%s,%d cmd -- %s ִ��ʧ��",__FILE__,__LINE__,cCmd);
		vtcp_log("%s,%d �Ӵ������еõ��ļ�ʧ��!!!![%d]",__FILE__,__LINE__,iRc);
		if(g_pub_tx.tx_date==20101008)
		{
			/* 20100108�����Ĺ���ʦ���������⣬�Ȳ����� */
		}
		else
		{
			strncpy(g_pub_tx.reply,"P157",4);
			return iRc;
		}
	}
	vtcp_log("[%s][%d] ׼�����ļ�=[%s]!!",__FILE__,__LINE__,s_Filename);
	
	fp=fopen(s_Filename,"r");
	if(fp==NULL){
		vtcp_log("%s,%d,open file name  %s",__FILE__,__LINE__,s_Filename);
		vtcp_log("%s,%d open file error",__FILE__,__LINE__);
		fclose(fp);
		return(-1);
	}
	while(1){
		memset(buf,'\0',sizeof(buf));
		p_buf=fgets(buf,1024,fp);
		if(strlen(buf)<5){
			break;
		}	
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.pack_date=atol(tmp);
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.packid,tmp,sizeof(s_Lv_wbctl.packid));
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.pkgno,tmp,sizeof(s_Lv_wbctl.pkgno));
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.pay_qs_no,tmp,sizeof(s_Lv_wbctl.pay_qs_no));
		
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.cash_qs_no,tmp,sizeof(s_Lv_wbctl.cash_qs_no));
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.in_date=atof(tmp);/*��������*/
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.packmy,tmp,sizeof(s_Lv_wbctl.packmy));
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.dtlcnt=atol(tmp);/*��ϸҵ���ܱ���*/
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.totamt=atof(tmp);/*��ϸҵ���ܽ��*/
						
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.zcnode,tmp,sizeof(s_Lv_wbctl.zcnode));
				
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.zc_date=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.zcid,tmp,sizeof(s_Lv_wbctl.zcid));
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.hz_date=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.o_pack_date=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.o_packid,tmp,sizeof(s_Lv_wbctl.o_packid));
										
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.dtlsuccnt=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.dtlsucamt=atof(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.o_pkgno,tmp,sizeof(s_Lv_wbctl.o_pkgno));
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.ornode,tmp,sizeof(s_Lv_wbctl.ornode));
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.acnode,tmp,sizeof(s_Lv_wbctl.acnode));
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.ffflag,tmp,sizeof(s_Lv_wbctl.ffflag));
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.txnum,tmp,sizeof(s_Lv_wbctl.txnum));
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.bfflag,tmp,sizeof(s_Lv_wbctl.bfflag));
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		s_Lv_wbctl.qs_date=atol(tmp);
								
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.packstat,tmp,sizeof(s_Lv_wbctl.packstat));
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.stat,tmp,sizeof(s_Lv_wbctl.stat));
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.brprtflag,tmp,sizeof(s_Lv_wbctl.brprtflag));
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.qsprtflag,tmp,sizeof(s_Lv_wbctl.qsprtflag));
			
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.content,tmp,sizeof(s_Lv_wbctl.content));
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.br_no,tmp,sizeof(s_Lv_wbctl.br_no));
		
		if(memcmp(s_Lv_wbctl.br_no,cKinbr,BRNO_LEN)!=0)/****add by xyy 2010-6-20 10:56:32 ���Ӷ��ļ����ݵ��ж�****/
		{
			vtcp_log("[%s][%d] Ҫ�����ļ��Ļ���[%s]���ļ��еĻ���[%s]����!!!!!",__FILE__,__LINE__,cKinbr,s_Lv_wbctl.br_no);
			fclose(fp);
			return(-1);
		}
									
		memset(tmp,'\0',sizeof(tmp));
		p_buf=msstrtok(p_buf,tmp,"|");
		memcpy(s_Lv_wbctl.filler,tmp,sizeof(s_Lv_wbctl.filler));
		vtcp_log("[%s][%d] s_Lv_wbctl.br_no =[%s] \n",__FILE__,__LINE__,s_Lv_wbctl.br_no);
		
		/*���Ȳ�ѯһ�� ��lv_wbctl���Ƿ����д�����¼ ����ѻ����Ÿ�Ϊ99999*/

		iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply,"pack_date=%ld and packid='%s' and pkgno='%s'",s_Lv_wbctl.pack_date,s_Lv_wbctl.packid,s_Lv_wbctl.pkgno);
		if(iRc){
			vtcp_log("[%s][%d] Lv_wbctl_Dec_Upd error  iRc[%ld]",__FILE__,__LINE__,iRc);
			Lv_wbctl_Clo_Upd();
    	fclose(fp);
			return(-1);
		}
		memset(&x_Lv_wbctl , 0 , sizeof(x_Lv_wbctl));
		iRc=Lv_wbctl_Fet_Upd(&x_Lv_wbctl,g_pub_tx.reply);
		if(iRc==100){/*û���ҵ���¼�������޸ģ�����ִ��while ���������¼   ����ҵ������*/
			vtcp_log("[%s][%d] Lv_wbctl_Fet_Upd  iRc[%d] �����������",__FILE__,__LINE__,iRc);
			Lv_wbctl_Clo_Upd();
		}else if(iRc){/*�α��ѯ������*/
			vtcp_log("[%s][%d] Lv_wbctl_Fet_Upd error iRc[%d] ",__FILE__,__LINE__,iRc);
			Lv_wbctl_Clo_Upd();
			fclose(fp);
			return(-1);
		}else{/*�ҵ��˼�¼ ��Ҫ��lv_wbctl�����*/	
			vtcp_log("[%s][%d]  �ҵ���¼ ��������¼���� ",__FILE__,__LINE__);
			memcpy(x_Lv_wbctl.br_no,cKinbr,sizeof(x_Lv_wbctl.br_no)-1);
			vtcp_log("[%s][%d]  lv_wbctl.br_no=[%s] ",__FILE__,__LINE__,x_Lv_wbctl.br_no);
			/*�������п���ı��� �޷���������ڵ� ���� ������ �ڴ˴����� 20131014 wudawei*/
			memcpy(x_Lv_wbctl.zcnode,s_Lv_wbctl.zcnode,sizeof(x_Lv_wbctl.zcnode)-1);
			vtcp_log("[%s][%d]  lv_wbctl.zcnode=[%s] ",__FILE__,__LINE__,x_Lv_wbctl.zcnode);
			
			x_Lv_wbctl.zc_date = s_Lv_wbctl.zc_date;
			vtcp_log("[%s][%d]  lv_wbctl.zc_date=[%ld] ",__FILE__,__LINE__,x_Lv_wbctl.zc_date);
			
			memcpy(x_Lv_wbctl.zcid,s_Lv_wbctl.zcid,sizeof(x_Lv_wbctl.zcid)-1);
			vtcp_log("[%s][%d]  lv_wbctl.zcid=[%s] ",__FILE__,__LINE__,x_Lv_wbctl.zcid);
			
			memcpy(x_Lv_wbctl.stat,s_Lv_wbctl.stat,sizeof(x_Lv_wbctl.stat)-1);
			vtcp_log("[%s][%d]  lv_wbctl.stat=[%s] ",__FILE__,__LINE__,x_Lv_wbctl.stat);
			/*add end wudawei 20131014*/
			iRc=Lv_wbctl_Upd_Upd(x_Lv_wbctl,g_pub_tx.reply);
			if(iRc){
				vtcp_log("[%s][%d] Lv_wbctl_Upd_Upd error iRc[%ld] ",__FILE__,__LINE__,iRc);
				Lv_wbctl_Clo_Upd();
				fclose(fp);
				return(-1);
			}
			vtcp_log("[%s][%d]  ������ɼ���while  ",__FILE__,__LINE__);
			Lv_wbctl_Clo_Upd();
			continue;
		}
				
		iRc=Lv_wbctl_Ins(s_Lv_wbctl,g_pub_tx.reply);
		if(iRc==-239){
			continue;
			vtcp_log("%s,%d Lv_wbctl_Ins iRc[%d]",__FILE__,__LINE__,iRc);
		}
		else if(iRc){
			vtcp_log("%s,%d Lv_wbctl_Ins error",__FILE__,__LINE__);
			fclose(fp);
			return(iRc);
		}
	}
	
	fclose(fp);
	return(0);
}

/*****
���ܣ�ȡ��instr�е�һ���ָ���delimiterǰ������
������ outstrָ��instr�е�һ��delimiter֮ǰ������
����ֵ��������һ��delimiter��λ��
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
