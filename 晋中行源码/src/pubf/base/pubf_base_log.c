/*************************************************
* 文 件 名:  pubf_base_log.c
* 功能描述：
* 作    者: jack
* 完成日期：2003/12/27
* 修改记录：  
* 1. 日   期:
*    修 改 人:
*    修改内容:
* 2. ...
*************************************************/
#include <stdio.h>
#include <varargs.h>
#include <sys/stat.h>
#include <sys/types.h>
/*#include <timeb.h> */
#define EXTERN
#include "public.h"
#include "tx_def_c.h"
#include "rsp_code_c.h"
#include "com_parm_c.h"
/*begin add by LiuHuafeng 20070201 */
#include <fcntl.h>
/* end by LiuHuafeng 20070201 */
FILE	*gl_BatchLogFile=NULL;

int iOpenflag=0;
extern char* appGetTime(char *pcInputTimeStr,int iSecNum,char *pcTimeStr);
int pub_base_wrt_log()
{
	char txcode[5],brno[6],tel[TEL_LENGTH+1],reply[5];
	/*begin add by LiuHuafeng 20070111 */
	char cTxcode[5];
	memset(txcode, 0 , sizeof(txcode));
	memset(cTxcode, 0 , sizeof(cTxcode));
	memset(brno, 0 , sizeof(brno));
	memset(tel, 0 , sizeof(tel));
	memset(reply, 0 , sizeof(reply));
	/* end by LiuHuafeng 20070111 */
	get_zd_data(DC_TX_CODE,txcode);
	get_zd_data(DC_TX_BR_NO,brno);
	get_zd_data(DC_TEL,tel);
	get_zd_data(DC_REPLY,reply);
	/* begin add by LiuHuafeng 20070111 */
	memcpy(cTxcode,txcode,sizeof(cTxcode)-1);
	pub_base_strpack(cTxcode);
	vtcp_log("[%s][%d] xyy ->tx_code=[%s] [%s]",__FILE__,__LINE__,txcode,g_pub_tx.tx_code);
	if(strlen(cTxcode)==0)
	{
		memcpy(txcode,g_pub_tx.tx_code,sizeof(txcode)-1);
	}
	/* end by LiuHuafeng 20070111 */

	pub_base_wrt_log_sub(txcode,brno,tel,reply);

	return 0;
}
/*****  add  for test begin ****/
static char g_starttime[8];
int pub_base_wrt_log_bg()
{
	int	hour,min,sec;

	pub_base_GetTime2(&hour, &min, &sec);

	sprintf(g_starttime,"%02d%02d%02d",hour,min,sec);
	vtcp_log("[%s][%d] set time [%s]",__FILE__,__LINE__,g_starttime);
	return 0;

}
/*****  add  for test end****/

pub_base_wrt_log_sub(char *txcode,char *brno,char *tel,char *reply)
{
	char	txname[30];
	char	cVBuf[80*16+1]={0};;
	char	mesg[80];
	int	i=1;
	int	j;
	long	logrq;
	static  long	oldlogrq=-1;
	int		hour,min,sec;
	int     year, mon, day;
	static FILE	*fp=NULL;
	static int    nIndex=0;
	/****Modified by SSH,20091215,只打开一次日志文件****/
	char logfile[128];
	struct  tx_def_c v_tx_def;
	struct  rsp_code_c v_rsp_code;

	memset(logfile,0x00,sizeof(logfile));

	pub_base_get_date(&year, &mon, &day);
	logrq=year*10000+mon*100+day;
	strcpy(logfile,getenv("LOGDIR"));
	logfile[strlen(logfile)]=NULL;
	strcat(logfile,"/");
	sprintf(logfile+strlen(logfile),"MLOG.%d",logrq);
	pub_base_GetTime2(&hour, &min, &sec);

	pub_base_strpack( brno );

	if ( Tx_def_Sel(g_pub_tx.reply, &v_tx_def , " tx_code='%s' " , txcode ) )
		sprintf(txname,"TXCD=%s",txcode);
	else
		strcpy( txname,v_tx_def.tx_name );
	pub_base_strpack(txname);
	
	if ( strncmp("0000",reply,4))
	{
		if(Rsp_code_Sel(g_pub_tx.reply,&v_rsp_code," code='%s' ",reply ) )
			sprintf(mesg,"响应码[%s]",reply);
		else
			strncpy(mesg,v_rsp_code.memo,sizeof(mesg)-1);
	}
	else 
	{
		memset( mesg , 0x00 , sizeof(mesg));
		strcpy(mesg,"交易成功");
	}
	i=strlen(txname);
	while(i<20){
		txname[i]=' ';
		i++;
	}
	i=strlen(mesg);
	while(i<24){
		mesg[i]=' ';
		i++;
	}
	sprintf(cVBuf+strlen(cVBuf),"[%.5s %.6s][%.6s][%02d%02d%02d][%.4s %.20s][%.24s%.4s]\n",brno,tel,g_starttime,hour,min,sec,txcode,txname,mesg,reply);
	nIndex++;

	/*if(nIndex<16){
	}else{
	*/
		fp=fopen(logfile,"a+");
		fwrite(cVBuf,strlen(cVBuf),1,fp);
		nIndex=0;
		cVBuf[0]='\0';		
		fclose(fp);
	/*}*/
	return 0;
}
/****Modified by SSH,处理批量程序不打开文件记录日志**/
static  FILE *mo_BatchLogFile=NULL;
int nOpenBatchLog(){
	char sFileName[256];
	sprintf(sFileName,"%s/log/batch%d.log",getenv("WORKDIR"),getpid());
	if(mo_BatchLogFile!=NULL){
		fclose(mo_BatchLogFile);
	}
	mo_BatchLogFile=fopen(sFileName,"wb");
	return(0);
}
int nCloseBatchLog(){
	if(mo_BatchLogFile!=NULL){
		fclose(mo_BatchLogFile);
		mo_BatchLogFile=NULL;
	}
	return(0);
}

int nOpenGlLog(char * tx_br_no,char * tel){
	char sFileName[256];
	memset(sFileName , 0 , sizeof(sFileName));
	if( (tx_br_no[0]>='0' && tx_br_no[0]<='9')&&
		(tx_br_no[1]>='0' && tx_br_no[1]<='9') &&
		(tx_br_no[2]>='0' && tx_br_no[2]<='9') &&
		(tx_br_no[3]>='0' && tx_br_no[3]<='9') &&
		(tx_br_no[4]>='0' && tx_br_no[4]<='9'))
	{
		sprintf(sFileName,"%s/log/trad_%.5s.log",getenv("WORKDIR"),tx_br_no);
	}
	else
	{
		sprintf(sFileName,"%s/log/trad.log",getenv("WORKDIR"));
	}
	if(gl_BatchLogFile!=NULL){
		fclose(gl_BatchLogFile);
	}
	iOpenflag=1;/* 使用新的方式写日志 */
	gl_BatchLogFile=fopen(sFileName,"ab");
	/**
	show_log("%s,%d openflag==%d\n",__FILE__,__LINE__,iOpenflag);
	**/
	return(0);
}
int nCloseGlLog(){
	if(gl_BatchLogFile!=NULL){
		fclose(gl_BatchLogFile);
		gl_BatchLogFile=NULL;
	}
	iOpenflag=0;/* 关闭新的写日志方式 */
	/**
	show_log("%s,%d openflag==%d\n",__FILE__,__LINE__,iOpenflag);
	**/
	return(0);
}

vtcp_log( fmtstr, va_alist )
char *fmtstr;
va_dcl
{
	va_list ap;
	int     year, mon, day, hour, min, sec;
	FILE *    fp;
	char    tmpstr[100];
	char    tmpname[100];
	char    comm_t[100];
	char	log_time[9];
	long    logsize;
	struct stat lfsts;
	char cFullTime[22];

	memset(&lfsts, 0 , sizeof(lfsts));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(tmpname,0x00,sizeof(tmpname));
	memset(log_time,0x00,sizeof(log_time));
	memset(cFullTime, 0 , sizeof(cFullTime));

	/*  struct timeb Tp; */
	if ( getenv("DEBUG")==NULL || atoi(getenv("DEBUG"))==0 )
		return ;

	va_start(ap);

	pub_base_get_date(&year, &mon, &day);
	pub_base_GetTime2(&hour, &min, &sec);
	appGetTime(NULL,0 ,cFullTime);

	sprintf(log_time,"%02d%02d%02d",day,hour,min);
	/**
	show_log("%s,%d openflag==%d\n",__FILE__,__LINE__,iOpenflag);
	**/
	/**
	printf("%s,%d openflag==%d\n",__FILE__,__LINE__,iOpenflag);
	**/
	if(iOpenflag == 0)
	{
		if ( getenv("LOGDIR") )
		{
			strcpy( tmpstr, getenv("LOGDIR") );
			if(atoi(getenv("LOGDIR_ZH"))==1 && memcmp(g_pub_tx.tx_br_no,"88",2)!=0)
			{
				if (g_pub_tx.tx_br_no[0]>='0' && g_pub_tx.tx_br_no[0]<='9')
				{
					strcat(tmpstr,"/log");
					strncpy(tmpstr+strlen(tmpstr),g_pub_tx.tx_br_no,2);
				}
			}
		} else
		{
			strcpy(tmpstr,"/tmp");
		}
  	
		strcat( tmpstr, "/" );
  	
		if ( getenv("LOGFILE") )
		{
			strcat( tmpstr, getenv("LOGFILE") );
		} else
		{
			strcat(tmpstr,"trad.log");
		}
  	
		/** add by wangjian 2009-02-24 11:23:58 **/
		if (g_pub_tx.tx_br_no[0]>='0' && g_pub_tx.tx_br_no[0]<='9')
		{
			/**
			if((g_pub_tx.tel[0]>='0' && g_pub_tx.tel[0]<='9')
			&&(g_pub_tx.tel[1]>='0' && g_pub_tx.tel[1]<='9')
			&&(g_pub_tx.tel[2]>='0' && g_pub_tx.tel[2]<='9')
			&&(g_pub_tx.tel[3]>='0' && g_pub_tx.tel[3]<='9')
			&&(g_pub_tx.tel[4]>='0' && g_pub_tx.tel[4]<='9')
			&&(g_pub_tx.tel[5]>='0' && g_pub_tx.tel[5]<='9'))
			{
				sprintf(tmpstr+strlen(tmpstr)-4,"_%.5s_%.6s.log",g_pub_tx.tx_br_no,g_pub_tx.tel);
			}
			else
			**/
				sprintf(tmpstr+strlen(tmpstr)-4,"_%.5s.log",g_pub_tx.tx_br_no);
		}
		/** end add **/
		if(mo_BatchLogFile==NULL){
			/***联机交易日志****/
			fp = fopen(tmpstr, "a");
		
			/**
			if(atoi(getenv("LOGDIR_ZH"))==1)
				;
			else
			**/
				stat(tmpstr, &lfsts);
		
			if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
			{
				logsize=500000;
			} else
			{
				logsize=atol( getenv( "LOGSIZE" ) );
			}
		}else{
			logsize=8*1024*1024;
			fp=mo_BatchLogFile;
			fseek(fp,0,SEEK_END);
			lfsts.st_size=ftell(fp);
		}
		if ( lfsts.st_size >= logsize )
		{
			fclose(fp);
			if(mo_BatchLogFile==NULL){
				strcpy(tmpname, tmpstr);
				memcpy(tmpname+strlen(tmpname)-3, log_time, 8);
				memcpy(tmpname+strlen(tmpname), ".old", 4);/*zhanghb2009-11-12*/
			}else{
				sprintf(tmpname,"%s/log/batch%d.old",getenv("WORKDIR"),getpid());
				sprintf(tmpstr,"%s/log/batch%d.log",getenv("WORKDIR"),getpid());
			}
			umask(0);
			rename(tmpstr, tmpname);
			fp = fopen(tmpstr, "a");
			if(mo_BatchLogFile!=NULL){
				/***如果是批量状态,也要更新批量日志文件指针***/
				mo_BatchLogFile=fp;
			}
		}
  	
		if (fp)
		{
			/*    ftime(&Tp);
  	      fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d:%03d ", 
  	          year, mon, day, hour, min ,sec,Tp.millitm);
			*/
			fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d %3s ", 
			    year, mon, day, hour, min ,sec,cFullTime+14);
			vfprintf(fp, fmtstr, ap);
			fprintf(fp, "\n");
			if(mo_BatchLogFile==NULL){
				fclose(fp);
			}
		}
  	
	}
	else
	{
		/* 直接写入 */
		fprintf(gl_BatchLogFile,"%04d-%02d-%02d %02d:%02d:%02d %3s ", 
		    year, mon, day, hour, min ,sec,cFullTime+14);
		vfprintf(gl_BatchLogFile, fmtstr, ap);
		fprintf(gl_BatchLogFile, "\n");
		logsize=8*1024*1024;
		fseek(gl_BatchLogFile,0,SEEK_END);
		lfsts.st_size=ftell(gl_BatchLogFile);
		if ( lfsts.st_size >= logsize )
		{
			char sFileName[100];
			memset(sFileName , 0 , sizeof(sFileName));
			fclose(gl_BatchLogFile);
			umask(0);
			if( (g_pub_tx.tx_br_no[0]>='0' && g_pub_tx.tx_br_no[0]<='9')&&
				(g_pub_tx.tx_br_no[1]>='0' && g_pub_tx.tx_br_no[1]<='9') &&
				(g_pub_tx.tx_br_no[2]>='0' && g_pub_tx.tx_br_no[2]<='9') &&
				(g_pub_tx.tx_br_no[3]>='0' && g_pub_tx.tx_br_no[3]<='9') &&
				(g_pub_tx.tx_br_no[4]>='0' && g_pub_tx.tx_br_no[4]<='9'))
			{
				sprintf(sFileName,"%s/log/trad_%.5s.log",getenv("WORKDIR"),g_pub_tx.tx_br_no);
			}
			else
			{
				sprintf(sFileName,"%s/log/trad.log",getenv("WORKDIR"));
			}
			strncpy(tmpname, sFileName,sizeof(tmpname)-1);
			memcpy(tmpname+strlen(tmpname)-3, log_time, 8);
			memcpy(tmpname+strlen(tmpname), ".old", 4);/*zhanghb2009-11-12*/
			rename(sFileName, tmpname);
			strncpy(tmpstr,sFileName,sizeof(tmpstr)-1);
			gl_BatchLogFile = fopen(tmpstr, "a");
		}
	}
	va_end(ap);
	if ( lfsts.st_size >= logsize )
	{
		/* rem by LiuHuafeng 20091214
		sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
		system(comm_t);
		****/
	}
	return 0;
}
vpgm_log( fmtstr, va_alist )
char *fmtstr;
va_dcl
{
	va_list ap;
	int     year, mon, day, hour, min, sec;
	FILE *    fp;
	char    tmpstr[100];
	char    tmpname[100];
	char    comm_t[100];
	long    logsize;
	long	trace_no;
	struct stat lfsts;
	/* struct timeb Tp; */
	memset(tmpname,0x00,sizeof(tmpname));

	if ( getenv("DEBUG")==NULL || atoi(getenv("DEBUG"))==0 )
		return ;

	va_start(ap);

	pub_base_get_date(&year, &mon, &day);
	pub_base_GetTime2(&hour, &min, &sec);

	if ( getenv("LOGDIR") )
	{
		strcpy( tmpstr, getenv("LOGDIR") );
	} else
	{
		strcpy(tmpstr,"/tmp");
	}

	strcat( tmpstr, "/" );

	strcat(tmpstr,"pgm.log");

	fp = fopen(tmpstr, "a");

	stat(tmpstr, &lfsts);

	if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
	{
		logsize=500000;
	} else
	{
		logsize=atol( getenv( "LOGSIZE" ) );
	}

	if ( lfsts.st_size >= logsize )
	{
		fclose(fp);
		strcpy(tmpname, tmpstr);
		memcpy(tmpname+strlen(tmpname)-3,"old", 3);
		rename(tmpstr, tmpname);
		fp = fopen(tmpstr, "a");
	}

	get_zd_long( DC_TRACE_NO,&trace_no );
	if (fp)
	{
		/* ftime(&Tp); 
        fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d:%03d %08d\n", 
            year, mon, day, hour, min ,sec,Tp.millitm, trace_no );
		***/
		fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d %08d\n", 
		    year, mon, day, hour, min ,sec, trace_no );
		vfprintf(fp, fmtstr, ap);
		fprintf(fp, "\n");
		fclose(fp);
	}

	va_end(ap);


	if ( lfsts.st_size >= logsize )
	{
		/* rem by LiuHuafeng  20091214
		sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
		system(comm_t);
		****/
	}

	return 0;
}
show_log( fmtstr, va_alist )
char *fmtstr;
va_dcl
{
	va_list ap;
	int     year, mon, day, hour, min, sec;
	FILE *    fp;
	char    tmpstr[100];
	char    tmpname[100];
	char    comm_t[100];
	long    logsize;
	struct stat lfsts;
	/*  struct timeb Tp; */
	if ( getenv("DEBUG")==NULL || atoi(getenv("DEBUG"))==0 )
		return ;

	va_start(ap);

	pub_base_get_date(&year, &mon, &day);
	pub_base_GetTime2(&hour, &min, &sec);

	if ( getenv("LOGDIR") )
	{
		strcpy( tmpstr, getenv("LOGDIR") );
	} else
	{
		strcpy(tmpstr,"/tmp");
	}

	strcat( tmpstr, "/" );

	strcat(tmpstr,"show.log");


	fp = fopen(tmpstr, "a");

	stat(tmpstr, &lfsts);

	if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
	{
		logsize=500000;
	} else
	{
		logsize=atol( getenv( "LOGSIZE" ) );
	}

	if ( lfsts.st_size >= logsize )
	{
		fclose(fp);
		strcpy(tmpname, tmpstr);
		memcpy(tmpname+strlen(tmpname)-3, "old", 3);
		rename(tmpstr, tmpname);
		fp = fopen(tmpstr, "a");
	}

	if (fp)
	{
		fprintf(fp,"%02d %02d:%02d:%02d ", 
		    day, hour, min ,sec);
		vfprintf(fp, fmtstr, ap);
		fprintf(fp, "\n");
		fclose(fp);
	}

	va_end(ap);


	if ( lfsts.st_size >= logsize )
	{
		/* rem by LiuHuafeng 20091214
		sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
		system(comm_t);
		*****/
	}

	return 0;
}
pub_reg_rec_file( char * mo )
{
	int     year, mon, day, hour, min, sec;
	FILE *    fp;
	char    tmpstr[100];
	char    tmpname[100];
	char    comm_t[100];
	long    logsize;
	struct stat lfsts;
	long traceno;
	char txcode[5];
	char subtxcode[5];

	pub_base_get_date(&year, &mon, &day);
	pub_base_GetTime2(&hour, &min, &sec);

	if ( getenv("LOGDIR") )
	{
		strcpy( tmpstr, getenv("LOGDIR") );
	} else
	{
		strcpy(tmpstr,"/tmp");
	}

	strcat( tmpstr, "/" );

	strcat(tmpstr,"rec.log");


	fp = fopen(tmpstr, "a");

	stat(tmpstr, &lfsts);

	if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
	{
		logsize=500000;
	} else
	{
		logsize=atol( getenv( "LOGSIZE" ) );
	}

	if ( lfsts.st_size >= logsize )
	{
		fclose(fp);
		strcpy(tmpname, tmpstr);
		memcpy(tmpname+strlen(tmpname)-3, "old", 3);
		rename(tmpstr, tmpname);
		fp = fopen(tmpstr, "a");
	}

	if (fp)
	{
		get_zd_long(DC_TRACE_NO,&traceno);
		get_zd_data(DC_TX_CODE,txcode);
		get_zd_data(DC_SUB_TX_CODE,subtxcode);

		fprintf( fp,"[%06d-%s-%s] %s\n",traceno,txcode,subtxcode,mo);

		fclose(fp);
	}
	/****Modified by SSH,2006.1.12,无此变量****/
	/****
    va_end(ap);
	****/

	if ( lfsts.st_size >= logsize )
	{
		/* rem by LiuHuafeng 20091214
		sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
		system(comm_t);
		*****/
	}

	return 0;
}
fin_report( fmtstr, va_alist )
char *fmtstr;
va_dcl
{
	va_list ap;
	int     year, mon, day, hour, min, sec;
	FILE *    fp;
	char    tmpstr[100];
	char    tmpname[100];
	char    comm_t[100];
	long    logsize;
	struct stat lfsts;

	struct com_parm_c com_parm_v;
	memset( &com_parm_v, 0x00, sizeof( struct com_parm_c ) );

	va_start(ap);

	pub_base_get_date(&year, &mon, &day);
	pub_base_GetTime2(&hour, &min, &sec);

	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &com_parm_v, "parm_code='CZJZ' and parm_seqn='4'" );
	if( g_reply_int )
	{
		strcpy(com_parm_v.val, "");
	}

	strcpy( tmpstr, getenv("HOME") );
	strcat( tmpstr, "/report/" );
	pub_base_strpack( com_parm_v.val );
	strcat( tmpstr, com_parm_v.val );
	strcat( tmpstr, "/FinAcct.txt" );

	fp = fopen(tmpstr, "a");

	stat(tmpstr, &lfsts);

	if ( getenv("LOGSIZE") == NULL || atoi(getenv("LOGSIZE")) == 0 )
	{
		logsize=500000;
	} else
	{
		logsize=atol( getenv( "LOGSIZE" ) );
	}

	if ( lfsts.st_size >= logsize )
	{
		fclose(fp);
		strcpy(tmpname, tmpstr);
		memcpy(tmpname+strlen(tmpname)-3, "old", 3);
		rename(tmpstr, tmpname);
		fp = fopen(tmpstr, "a");
	}

	if (fp)
	{
		fprintf(fp,"%04d-%02d-%02d %02d:%02d:%02d ", 
		    year, mon, day, hour, min ,sec);
		vfprintf(fp, fmtstr, ap);
		fprintf(fp, "\n");
		fclose(fp);
	}

	va_end(ap);


	if ( lfsts.st_size >= logsize )
	{
		/* rem by LiuHuafeng 20091214
		sprintf(comm_t,"chmod 666 %s >/dev/null 2>&1",tmpstr);
		system(comm_t);
		*******/
	}

	return 0;
}
int iFindCoreDumpRecord()
{
	int  iFd=0,iSize=0;
	extern int errno;
	char cFilename[513];
	char cBrno[8];
	char cTx_code[5];
	char cTel[TEL_LENGTH+1];
	char cReply[5];
	char cLine[5+1+4+1+4+1+4+1+1];

	int iYY=0,iMM=0,iDD=0;
	int iHour=0,iMin=0,iSec=0;
	memset(cLine, 0 , sizeof(cLine));
	pub_base_get_date(&iYY, &iMM, &iDD);
	pub_base_GetTime2(&iHour, &iMin, &iSec);
	memset(cFilename, 0 ,sizeof(cFilename));
	memset(cBrno, 0 , sizeof(cBrno));
	memset(cTx_code, 0 , sizeof(cTx_code));
	memset(cTel, 0 , sizeof(cTel));
	memset(cReply, 0 , sizeof(cReply));
	strncpy(cBrno, g_pub_tx.tx_br_no , sizeof(cBrno)-1);
	strncpy(cTx_code, g_pub_tx.tx_code , sizeof(cTx_code)-1);
	strncpy(cTel, g_pub_tx.tel , sizeof(cTel)-1);
	memcpy(cReply, g_pub_tx.reply , sizeof(cReply));
	sprintf(cFilename,"%s/usr/lhf/fCore%04d%02d%02d.txt",getenv("HOME"),iYY,iMM,iDD);
	memcpy(cLine,cBrno,5);
	cLine[5]='|';
	memcpy(cLine+6,cTx_code,4);
	cLine[10]='|';
	memcpy(cLine+11,cTel,4);
	cLine[15]='|';
	memcpy(cLine+16,cReply,4);
	cLine[21]='\n';

	iFd=open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
	if(iFd==-1)
	{
		printf("[%s][%d]文件描述符号==[%d] \n",__FILE__,__LINE__,iFd);
		printf("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
		printf("[%s][%d] 打开准备写入的文件[%s]错误 \n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	iSize=write(iFd,cLine,sizeof(cLine));
	if(iSize<=0)
	{
		printf("没有数据被写入 errno=[%d] ,size=[%d] \n",errno,iSize);
		return -1;
	}
	if(close(iFd))
	{
		printf("[%s][%d] close id error \n",__FILE__,__LINE__);
		return -1;
	}
	return 0;
}

