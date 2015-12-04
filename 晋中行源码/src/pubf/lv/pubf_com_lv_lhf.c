/***************************************************************************/
/* 文件名称: lv_gscomm.pc                                                  */
/* 功能描述: 小额支付检查类操作公共函数                                    */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cifpack.h"
#include "lv_define.h"
#include "hv_define.h"
#include "lv_tito.h"
#include "lv_pub.h"
#include "lv_add.h"
#include "hv_orno_brno_c.h"
#include "lv_newxyh_c.h"
#include "lv_addit_c.h"
#include "lv_sysctl_c.h"
#include "seqctl_c.h"
#include "lv_fd123.h"
#include "public.h"
#include "hv_uniontab_c.h"/*YHBP*/
#include "lv_pkgreg_c.h"  /*YHBP*/
#include "lv_c554r_c.h"
#include "lv_c503r_c.h"

char* pcLvGetDay();
char* pcPayinRetdtl(PAY_IN_AREA *wp_payin);
char * pcLvDesPswdCode(char *code);
char* pcLvGetRecvno();
void lv_zip_payinF_space(PAY_IN_AREA * wp_payin);
extern int iLvGetSysStat(char *);
/*
extern int UnionEncryptPin(char *cHostIp,int cHostPort,char *pcPinBuf,char *pcPanBuf,char *pcZpkBuf,char *pcPbkBuf);
extern int UnionDecryptPin(char *cHostIp,int cHostPort,char *pcZpkBuf,char *pcPbkBuf,char *pcPanBuf,char *pcPinBuf);
*/
double smnttostr(s)
	double s;
{
	char    buf[30];
	double     t=0;
	memset(buf, '\0', 30);
	s = s + 0.0001;
	sprintf(buf, "%.0f", s);
	t = atof(buf);
	return(t);
}
/*******************************
返回错误信息
err 0130*
**********************************/
void lv_setf(int iline, char *file)
{
	lvca.errline = iline;
	strcpy(lvca.errfm , file);
}

void initLvca()
{
	FILE *fpr;
	/****NEWYX2:扩大附加域长度****/
	char cBuff[8001];
	char cFilename[100];
	memset(cFilename,0,sizeof(cFilename));
	memset(cBuff, 0 , sizeof(cBuff));
	memset(&lvca, 0x0, sizeof(lvca));
	get_fd_data("0070", lvca.tlrno);
	get_fd_data("0030", lvca.brno);
	get_fd_data("0050", lvca.tbsdy);
	get_fd_data("0060", lvca.wssrno);
	get_fd_data("0390", lvca.txamt);
	get_fd_data("1230", lvca.lv_fd123);
	Gl_file_first=0;
	pub_base_AllDwnFilName( cFilename);
	vtcp_log("%s,%d filename=[%s]\n",__FL__,cFilename);
	fpr=fopen(cFilename,"rb");
	if (fpr!=NULL)
	{
		fgets(cBuff,sizeof(cBuff),fpr);
		vtcp_log("%s,%d file=[%s]\n",__FL__,cBuff);
		/*****附加域里的东西，需要压缩吗？？ xyy 2006-9-11 10:05
		zip_space(cBuff);
		memcpy(lvca.lv_fd123+sizeof(BASE_EXCHANGE_AREA),cBuff,sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));
		*********************/
		memcpy(lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA),cBuff,strlen(cBuff));
	}
	else
	{
		vtcp_log("%s,%d 得到附加文件错误",__FL__);
	}	
	fclose(fpr);
}

char *lv_date(long l_d)
{
	sprintf(lvca.lv_date, "%08ld", l_d);
	return lvca.lv_date;
}
char *lv_ldate(long *l_d, char *str_date, int len)
{
	*l_d = apatoi(str_date, len);	
	return str_date;
}

/****************************************************************************/
/***************************************************************************/
/* 函数名称: int iLvTisToPayin(PAY_IN_AREA *wp_payin)                      */
/* 功能描述: 小额系统中,根据前台tis包对payin赋值                           */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/

int iLvTisToPayin(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char tmpbuf[255];
	char cTxamt[16];
	/**一般，这是使用交换区的第一个函数，所以首先初始化交换区**/
	memset(wp_payin,'\0',sizeof(PAY_IN_AREA));
	memset(cTxamt,0,sizeof(cTxamt));
	memcpy(wp_payin->type,ATYPE_PAY_IN,sizeof(wp_payin->type));
	initLvca();

	sprintf(tmpbuf, "%04d", strlen(lvca.lv_fd123));
	memcpy(wp_payin->length, tmpbuf, sizeof(wp_payin->length));
	vtcp_log("[%s][%d]fd123=[%s](len=%d)\n",__FL__,lvca.lv_fd123, strlen(lvca.lv_fd123));
	vtcp_log("[%s][%d]size==[%d]\n",__FL__,sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));/*固定部分**/
	memcpy(((char *)wp_payin)+sizeof(BASE_EXCHANGE_AREA),lvca.lv_fd123,
		sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));/*固定部分**/
	vtcp_log("[%s][%d]payin=[%s](len=%d)\n",__FL__,((char*)wp_payin)+sizeof(BASE_EXCHANGE_AREA),sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));
	memcpy(cTxamt,wp_payin->F_txamt,sizeof(cTxamt)-1);
	zipn_space(cTxamt,15);
	del_dot(cTxamt,cTxamt,15);
	memcpy(wp_payin->F_txamt,cTxamt,sizeof(cTxamt)-1);
	vtcp_log("[%s][%d]wp_payin->F_txamt=[%s]",__FILE__,__LINE__,wp_payin->F_txamt);

	vtcp_log("[%s][%d] F_crdb        =[%c] ",__FL__,wp_payin->F_crdb[0]        );    
	vtcp_log("[%s][%d] F_crdb        =[%c] ",__FL__,wp_payin->F_crdb[0]        );    
	vtcp_log("[%s][%d] F_opcd        =[%.2s] ",__FL__,wp_payin->F_opcd        ); 
	vtcp_log("[%s][%d] F_pkgno       =[%.3s] ",__FL__,wp_payin->F_pkgno       );
	vtcp_log("[%s][%d] F_orderno     =[%.8s] ",__FL__,wp_payin->F_orderno     );
	vtcp_log("[%s][%d] F_txnum       =[%.5s] ",__FL__,wp_payin->F_txnum       );
	vtcp_log("[%s][%d] F_orbrno      =[%.12s]",__FL__,wp_payin->F_orbrno      );
	vtcp_log("[%s][%d] F_acbrno      =[%.12s]",__FL__,wp_payin->F_acbrno      );
	vtcp_log("[%s][%d] F_txamt       =[%.15s]",__FL__,wp_payin->F_txamt       );
	vtcp_log("[%s][%d] F_payno       =[%.12s]",__FL__,wp_payin->F_payno       );
	vtcp_log("[%s][%d] F_payqsno     =[%.12s]",__FL__,wp_payin->F_payqsno     );
	vtcp_log("[%s][%d] F_payactno    =[%.32s]",__FL__,wp_payin->F_payactno    );
	vtcp_log("[%s][%d] F_payname     =[%.60s]",__FL__,wp_payin->F_payname     );
	vtcp_log("[%s][%d] F_payaddress  =[%.60s]",__FL__,wp_payin->F_payaddress  );
	vtcp_log("[%s][%d] F_userpswd    =[%.8s]",__FL__,wp_payin->F_userpswd    ); 
	vtcp_log("[%s][%d] F_cashqsno    =[%.12s]",__FL__,wp_payin->F_cashqsno    );
	vtcp_log("[%s][%d] F_cashno      =[%.12s]",__FL__,wp_payin->F_cashno      );
	vtcp_log("[%s][%d] F_cashactno   =[%.32s]",__FL__,wp_payin->F_cashactno   );
	vtcp_log("[%s][%d] F_cashname    =[%.60s]",__FL__,wp_payin->F_cashname    );
	vtcp_log("[%s][%d] F_cashaddress =[%.60s]",__FL__,wp_payin->F_cashaddress );
	vtcp_log("[%s][%d] F_ywtype      =[%.12s]",__FL__,wp_payin->F_ywtype      );
	vtcp_log("[%s][%d] F_content     =[%.60s]",__FL__,wp_payin->F_content     );
	vtcp_log("[%s][%d] F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	vtcp_log("[%s][%d] F_operlevel   =[%c] ",__FL__,wp_payin->F_operlevel[0]   );
	vtcp_log("[%s][%d] F_authno      =[%.6s] ",__FL__,wp_payin->F_authno      );
	vtcp_log("[%s][%d] F_authpswd    =[%.8s] ",__FL__,wp_payin->F_authpswd    );   
	vtcp_log("[%s][%d] F_voctype     =[%.2s] ",__FL__,wp_payin->F_voctype     );   
	vtcp_log("[%s][%d] F_vocnum      =[%.12s]",__FL__,wp_payin->F_vocnum      );   
	vtcp_log("[%s][%d] F_packday     =[%.8s] ",__FL__,wp_payin->F_packday     );   
	vtcp_log("[%s][%d] F_packid      =[%.8s] ",__FL__,wp_payin->F_packid      );   
	vtcp_log("[%s][%d] F_feeflag     =[%c] ",__FL__,wp_payin->F_feeflag[0]     );   
	vtcp_log("[%s][%d] F_feeamt      =[%.15s]",__FL__,wp_payin->F_feeamt      );   
	vtcp_log("[%s][%d] F_respday     =[%.8s] ",__FL__,wp_payin->F_respday     );   
	vtcp_log("[%s][%d] F_ptype       =[%.2s] ",__FL__,wp_payin->F_ptype       );   
	vtcp_log("[%s][%d] F_pdate       =[%.8s] ",__FL__,wp_payin->F_pdate       );   
	vtcp_log("[%s][%d] F_pnum        =[%.8s] ",__FL__,wp_payin->F_pnum        );   
	vtcp_log("[%s][%d] F_ptxamt      =[%.15s]",__FL__,wp_payin->F_ptxamt      );   
	vtcp_log("[%s][%d] F_jtxamt      =[%.15s]",__FL__,wp_payin->F_jtxamt      );   
	vtcp_log("[%s][%d] F_owtday      =[%.8s] ",__FL__,wp_payin->F_owtday      );   
	vtcp_log("[%s][%d] F_otxnum      =[%.5s] ",__FL__,wp_payin->F_otxnum      );   
	vtcp_log("[%s][%d] F_oorderno    =[%.8s] ",__FL__,wp_payin->F_oorderno    );   
	vtcp_log("[%s][%d] F_retcode     =[%.2s] ",__FL__,wp_payin->F_retcode     );   
	vtcp_log("[%s][%d] F_rcpstat     =[%.2s] ",__FL__,wp_payin->F_rcpstat     );   
	vtcp_log("[%s][%d] F_resp1       =[%.8s] ",__FL__,wp_payin->F_resp1       );   
	vtcp_log("[%s][%d] F_resp2       =[%.8s] ",__FL__,wp_payin->F_resp2       );   
	vtcp_log("[%s][%d] F_packend     =[%s] ",__FL__,wp_payin->F_packend     );   
	vtcp_log("[%s][%d] F_lw_ind      =[%s] ",__FL__,wp_payin->F_lw_ind        );   
	vtcp_log("[%s][%d] F_dfxyh       =[%.60s]",__FL__,wp_payin->F_dfxyno       ); 
	vtcp_log("[%s][%d] F_o_or_br_no     =[%s] ",__FL__,wp_payin->F_o_or_br_no        );   
	vtcp_log("[%s][%d] F_o_ac_br_no     =[%.60s]",__FL__,wp_payin->F_o_ac_br_no       );        
	/** rem by LiuHuafeng 20060310 
	sizeof(PAY_IN_FIX_AREA));
	*********************************/
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno)) &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno)) &&
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		/* add by LiuHuaefeng 20070331 */
		if (memcmp(wp_payin->F_txnum,TXNUM_TC,sizeof(wp_payin->F_txnum))==0)
		{
			PAY_IN_CXTC_AREA wc_tctd;
			vtcp_log("[%s][%d]sizeof(PAY_IN_TCTD_AREA)===[%d] \n",__FL__,sizeof(PAY_IN_TCTD_AREA));
			memset(&wc_tctd,'\0',sizeof(wc_tctd));
			memcpy(&wc_tctd, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(PAY_IN_TCTD_AREA));
			memcpy(wp_payin->F_actype,wc_tctd.actype,sizeof(wp_payin->F_actype));
		}
		/* end by LiuHuafeng 20070331 */

		if (memcmp(wp_payin->F_txnum,TXNUM_TD,sizeof(wp_payin->F_txnum))==0||
			memcmp(wp_payin->F_txnum,TXNUM_DGTD,sizeof(wp_payin->F_txnum))==0)
		{
			/**通存或者通兑**/
			PAY_IN_TCTD_AREA wc_tctd;
			vtcp_log("[%s][%d]sizeof(PAY_IN_TCTD_AREA)===[%d] \n",__FL__,sizeof(PAY_IN_TCTD_AREA));
			memset(&wc_tctd,'\0',sizeof(wc_tctd));
			memcpy(&wc_tctd, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(PAY_IN_TCTD_AREA));
			memcpy(wp_payin->F_actype,wc_tctd.actype,sizeof(wp_payin->F_actype));
			memcpy(wp_payin->F_pswdtype,wc_tctd.pswdtype,
				sizeof(wp_payin->F_pswdtype));
			memcpy(wp_payin->F_pswd,wc_tctd.pswd, sizeof(wp_payin->F_pswd));
		}
		/** add by lhf 20060311 */
		if (memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))!=0)
		{
			/* 只有录入的时候才设置附加域 */
			vtcp_log("[%s][%d]wp_payin->F_txamt=[%.15s]\n",__FL__,wp_payin->F_txamt);
		}
		/**end add by lhf 20060311 */
		else if (memcmp(wp_payin->F_txnum,TXNUM_WT,sizeof(wp_payin->F_txnum))==0)
		{
			WTSK_ADD_AREA wd_add;
			memset(&wd_add, 0 , sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			vtcp_log("[%s][%d]wd_add===[%s] \n",__FL__,&wd_add);	
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_TSCF,sizeof(wp_payin->F_txnum))==0)
		{
			LVTSCF_ADD_AREA wd_add;
			memset(&wd_add, 0 , sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			vtcp_log("[%s][%d]wd_add===[%s] \n",__FL__,&wd_add);	
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_GKDJ,sizeof(wp_payin->F_txnum))==0||
			memcmp(wp_payin->F_txnum,TXNUM_GKJJ,sizeof(wp_payin->F_txnum))==0)
		{
			GKDJ_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			vtcp_log("[%s][%d]wd_add===[%s] \n",__FL__,&wd_add);	
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_SSKS,sizeof(wp_payin->F_txnum))==0 ||\
			memcmp(wp_payin->F_txnum,TXNUM_PLKS,sizeof(wp_payin->F_txnum))==0)
		{
			SSKS_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			vtcp_log("[%s][%d]wd_add===[%s] \n",__FL__,&wd_add);	
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_FSSR,sizeof(wp_payin->F_txnum))==0)
		{
			FSSR_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZJZF,sizeof(wp_payin->F_txnum))==0||
			memcmp(wp_payin->F_txnum,TXNUM_SQZF,sizeof(wp_payin->F_txnum))==0||
			memcmp(wp_payin->F_txnum,TXNUM_ZJTH,sizeof(wp_payin->F_txnum))==0||
			memcmp(wp_payin->F_txnum,TXNUM_SQTH,sizeof(wp_payin->F_txnum))==0)
		{
			ZJZF_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))==0)
		{
			/****NEWYX2:原来小董得处理是在录完背书人之后才设置附加域,
			*****   但一方面新系统没必要这么做，二来也容易出错，改为在此处***/
			if(strncmp(g_pub_tx.tx_code,"5925",4)==0)/**add by NEWBP5**/
			{
					vtcp_log("[%s][%d]这里是普通借记支票回执业务不需要往下走!\n",__FILE__,__LINE__);
			}
			else/**这里把原先的东西都加到else里了    ****modify by NEWBP5**/
			{
			    ZPJL1_ADD_AREA   wd_add1;
			    ZPJL2_ADD_AREA   wd_add2;
			    ZPJL3_ADD_AREA   wd_add3;
			    char             *pcAddStr;
			    int		 nBsrCnt=0;
			    char			 cFilename[128];
			    char			 cLargeBuffer[8192];
			    FILE			 *fp;
			    memset(&wd_add1,'\0',sizeof(wd_add1));
			    memset(&wd_add2,'\0',sizeof(wd_add2));
			    memset(&wd_add3,'\0',sizeof(wd_add3));
          
			    memset(cFilename,'\0',sizeof(cFilename));
			    memset(cLargeBuffer,'\0',sizeof(cLargeBuffer));
			    pub_base_AllDwnFilName( cFilename);
			    fp=fopen(cFilename,"rb");
			    if(fp==NULL){
			    	vtcp_log("[%s][%d] 取附加域文件失败\n",__FL__);
			    	sprintf(acErrMsg,"取附加域文件失败!\n");
			    	return(-1);
			    }
			    fread(cLargeBuffer,sizeof(cLargeBuffer)-1,1,fp);
			    fclose(fp);
			    /****写入附加数据域****/
			    pcAddStr=cLargeBuffer;
			    memcpy(&wd_add1,pcAddStr,sizeof(wd_add1));	
			    pad_zpjl1(&wd_add1);
			    
			    pcAddStr+=sizeof(wd_add1);
			    memcpy(&wd_add2,pcAddStr,2);
			    pcAddStr+=2;
			    nBsrCnt=atoi((char *)&wd_add2);
			    if(nBsrCnt!=0){
			    	memcpy((char *)&wd_add2+2,pcAddStr,nBsrCnt*60);
			    	pcAddStr+=nBsrCnt*60;
			    }
			    pad_zpjl2(&wd_add2);
			    vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			    memcpy(&wd_add3,pcAddStr,sizeof(wd_add3));
			    iRc=iLvSetAddit(wp_payin,"001",&wd_add1,sizeof(wd_add1));
			    if(!iRc){
			      iRc=iLvSetAddit(wp_payin,"002",&wd_add2,nBsrCnt*60+2);
			    	vtcp_log("[%s][%d]附加数据域处理完毕\n",__FILE__,__LINE__);
			    }
			    if(!iRc){
			    	iRc=iLvSetAddit(wp_payin,"003",&wd_add3,sizeof(wd_add3));
			    }
			    vtcp_log("[%s][%d]附加数据域处理完毕\n",__FILE__,__LINE__);
			    /****NEWYX2 END:设置支票截留附加域****/
			}
			/**end by NEWBP5**/
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0)
		{
			char cHyflag[2]; /**核押方式YHBP**/
			char cPjtype[3]; /**票据类型YHBP**/
			char cPayno[13]; /**付款行名YHBP**/
			char cTxno[5];   /**YHBP**/
			char cKinbr[BRNO_LEN+1];
			char cOrbrno[13];
			/****NEWYX2:设置通用截留附加域******/
			TYJL1_ADD_AREA   wd_add1;
			TYJL2_ADD_AREA   wd_add2;
			TYJL3_ADD_AREA   wd_add3;
			TYJL4_ADD_AREA   wd_add4;
			TYJL5_ADD_AREA   wd_add5;
			char             *pcAddStr;
			int			 nBsrCnt=0;
			char			 cFilename[128];
			char			 cLargeBuffer[8192];
			FILE			 *fp;
			
			/****add YHBP *******************/
			struct hv_uniontab_c hv_uniontab;
			struct lv_pkgreg_c wd_lv_pkgreg;
			
			memset(cHyflag, 0 , sizeof(cHyflag));
			memset(cPjtype, 0 , sizeof(cPjtype));
			memset(cPayno , 0 , sizeof(cPayno));
			memset(cTxno  , 0 , sizeof(cTxno));
			memset(cOrbrno, 0 , sizeof(cOrbrno));
			memset(cKinbr , 0 , sizeof(cKinbr));
			memset(&wd_lv_pkgreg,0,sizeof(wd_lv_pkgreg));
			memset(&hv_uniontab ,0,sizeof(hv_uniontab));
			/**end  YHBP**/
			memset(&wd_add1,'\0',sizeof(wd_add1));
			memset(&wd_add2,'\0',sizeof(wd_add2));
			memset(&wd_add3,'\0',sizeof(wd_add3));
			memset(&wd_add4,'\0',sizeof(wd_add4));
			memset(&wd_add5,'\0',sizeof(wd_add5));
			memset(cFilename,'\0',sizeof(cFilename));
			memset(cLargeBuffer,'\0',sizeof(cLargeBuffer));
			
			get_zd_data("0670",cHyflag);/***获取密押YHBP***/
			get_zd_data("0220",cPjtype);/***得到票据类型HPBP***/
			get_zd_data("0160",cTxno);/**YHBP**/
			get_zd_data("0030",cKinbr);/**YHBP**/
			memcpy(cPayno,wp_payin->F_acbrno,sizeof(cPayno)-1);/**YHBP**/
			vtcp_log("[%s][%d]cPayno=[%s]cTxno=[%s]\n",__FILE__,__LINE__,cPayno,cTxno);
			vtcp_log("[%s][%d]核押方式为=[%s]票据类型=[%s]\n",__FILE__,__LINE__,cHyflag,cPjtype);
			
			pub_base_AllDwnFilName( cFilename);
			fp=fopen(cFilename,"rb");
			if(fp==NULL){
				vtcp_log("[%s][%d] 取附加域文件失败\n",__FL__);
				sprintf(acErrMsg,"取附加域文件失败!\n");
				return(-1);
			}
			fread(cLargeBuffer,sizeof(cLargeBuffer)-1,1,fp);
			fclose(fp);
			/****写入附加数据域****/
			pcAddStr=lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA);
			vtcp_log("[%s][%d]pcAddStr=[%s]\n",__FILE__,__LINE__,pcAddStr);
			memcpy(&wd_add1,pcAddStr,sizeof(wd_add1));	
			if(memcmp(cPjtype,"05",sizeof(cPjtype)-1)==0)/**如果是银行本票YHBP,**/
			{
				 memcpy(wd_add1.payno,cPayno,sizeof(wp_payin->F_acbrno));
				 vtcp_log("[%s][%d]wd_add1.payno=[%s]\n",__FILE__,__LINE__,wd_add1.payno);
			}
			pad_tyjl1(&wd_add1);
			pcAddStr+=sizeof(wd_add1);
			memcpy(&wd_add2,pcAddStr,2);
			pcAddStr+=2;
			nBsrCnt=atoi((char *)&wd_add2);
			if(nBsrCnt!=0){
				memcpy((char *)&wd_add2+2,pcAddStr,nBsrCnt*60);
				pcAddStr+=nBsrCnt*60;
			}
			memcpy(&wd_add3,pcAddStr,sizeof(wd_add3));	
			pcAddStr+=sizeof(wd_add3);
			memcpy(&wd_add4,pcAddStr,sizeof(wd_add4));	
			
			/***YHBP这里增加对银行本票的密押的处理***/
			if(memcmp(cPjtype,"05",2)==0  )/**如果是银行本票**/
			{
				  vtcp_log("[%s][%d]cHyflag=[%s]cPayno=[%s]\n",__FILE__,__LINE__,cHyflag,cPayno);
				  /***付款行名称:填写出票银行名称或代理清算行名称***/
				  g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cPayno);
          if (g_reply_int)
          {
              vtcp_log("[%s][%d]查询对方清算中心号码错误\n",__FILE__,__LINE__);
              return(99);
          }
          memcpy(wd_add4.paybrname2,hv_uniontab.br_name_f,sizeof(hv_uniontab.br_name_f)-1);
          vtcp_log("[%s][%d]wd_add4.paybrname2==[%s]",__FILE__,__LINE__,wd_add4.paybrname2);

				  if(cHyflag[0]== '0')/**如果选择的是机器核押的话**/
				  {
				     printf("[%s][%d]核押方式为[%s]\n",__FILE__,__LINE__,cHyflag);
				     /**这里需要调用核押函数,得到密押**/
				     memcpy(wd_add4.mac,"12345678901234567890",sizeof(wd_add4.mac));
				  }
			}
			pad_tyjl4(&wd_add4);
			pcAddStr+=sizeof(wd_add4);
			memcpy(&wd_add5,pcAddStr,sizeof(wd_add5));	
			/**YHBP判断一下，这里如果是银行本票信息修改的话，就不需要新生成addid了**/
			if(memcmp(cTxno,"5321",sizeof(cTxno)-1)==0)
			{
				   GetOr_br_noByBr_no(cKinbr, cOrbrno);
				   vtcp_log("[%s][%d]这个程序是银行本票信息修改addid=[%s]\n",__FILE__,__LINE__,wp_payin->T_addid);
				   iRc = Lv_pkgreg_Sel(g_pub_tx.reply, &wd_lv_pkgreg, "or_br_no = '%.12s' and wt_date = %.8s and \
				                      orderno = '%.8s' and lw_ind='%c' and pkgno='%.3s' ",cOrbrno, wp_payin->F_wtday, \
				                      wp_payin->F_orderno,wp_payin->F_lw_ind[0],wp_payin->F_pkgno);
           if (iRc!=DB_OK)
           {
                vtcp_log("%s,%d,Find error,iRc=%d\n",__FILE__,__LINE__,iRc);
                sprintf(acErrMsg,"未找到原业务记录");
                strncpy( g_pub_tx.reply,"H031",4);
                return(iRc);
           }
           memcpy(wp_payin->F_orbrno,cOrbrno,sizeof(wp_payin->F_orbrno));
           memcpy(wp_payin->T_addid,wd_lv_pkgreg.addid,sizeof(wd_lv_pkgreg.addid)-1);
           vtcp_log("[%s][%d]wp_payin->T_addid=[%s]\n",__FILE__,__LINE__,wp_payin->T_addid);
		  }
			iRc=iLvSetAddit(wp_payin,"001",&wd_add1,sizeof(wd_add1));
			vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			if(!iRc){
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
				iRc=iLvSetAddit(wp_payin,"002",&wd_add2,nBsrCnt*60+2);
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			}
			if(!iRc){
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
				iRc=iLvSetAddit(wp_payin,"003",&wd_add3,sizeof(wd_add3));
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			}
			if(!iRc){
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
				iRc=iLvSetAddit(wp_payin,"004",&wd_add4,sizeof(wd_add4));
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			}
			if(!iRc){
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
				iRc=iLvSetAddit(wp_payin,"005",&wd_add5,sizeof(wd_add5));
				vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			}
			vtcp_log("[%s][%d]nBsrCnt=[%d]\n",__FILE__,__LINE__,nBsrCnt);
			/****NEWYX2 END:设置支票截留附加域****/
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_JF,sizeof(wp_payin->F_txnum))==0)
		{
			JF_ADD_AREA     wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_GZDJ,sizeof(wp_payin->F_txnum))==0||
			memcmp(wp_payin->F_txnum,TXNUM_GZJJ,sizeof(wp_payin->F_txnum))==0)
		{
			GZDJ_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_TYXX,sizeof(wp_payin->F_txnum))==0)
		{
			vtcp_log("[%s][%d]暂时不提供通用信息的附加域处理!\n",__FILE__,__LINE__);

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_DS,sizeof(wp_payin->F_txnum))==0)
		{
			SFYW_ADD   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_DF,sizeof(wp_payin->F_txnum))==0)
		{
			SFYW_ADD   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZPQC,sizeof(wp_payin->F_txnum))==0)
		{
			QCSQ_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZPYD,sizeof(wp_payin->F_txnum))==0)
		{
			QCYD_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZSCX,sizeof(wp_payin->F_txnum))==0)
		{
			ZSSQ_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZSYD,sizeof(wp_payin->F_txnum))==0)
		{
			ZSYD_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZHCX,sizeof(wp_payin->F_txnum))==0)
		{
			ZHCX_ADD   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			/****NEWTCTD:处理通存通兑客户密码****/
			if(memcmp(wd_add.pswdtp,"01",2)==0){
				TD_ADD_AREA wd_td;
				char sTmp[1024];
				int nLen=0;
				int nAddLen=0;
				memset(&wd_td,'\0',sizeof(wd_td));
				memset(sTmp,'\0',sizeof(sTmp));
				memcpy(wd_td.pswdmethod,wd_add.pswdtp,2);
				memcpy(wd_td.pswdlength,wd_add.pswdlen,8);
				nLen=apatoi(wd_td.pswdlength,8);
				memcpy(wd_td.pswdcode,wd_add.passwd,nLen);
				nAddLen=(char *)&wd_add.pswdlen-(char *)&wd_add.acttype;
				memcpy(sTmp,&wd_add,nAddLen);
				nBepsEncrypt(wp_payin,&wd_td);
				nLen=apatoi(wd_td.pswdlength,8);
				memcpy(sTmp+nAddLen,&wd_td.pswdlength,8+nLen);
				nAddLen+=8+nLen;
				memcpy(sTmp+nAddLen,&wd_add.qtype,sizeof(wd_add.qtype)+sizeof(wd_add.reason));
				nAddLen+=sizeof(wd_add.qtype)+sizeof(wd_add.reason);				
				vtcp_log("%s,%d,帐号查询的录入值为[%s]\n",__FILE__,__LINE__,sTmp);
				iRc=iLvSetAddit(wp_payin,"000",sTmp,nAddLen);
			}else{
				iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
			}

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_CXYD,sizeof(wp_payin->F_txnum))==0)
		{
			/*BEGIN NEWYX by LiuHuafeng 2007-4-4 4:14 
			ZHCXYD_ADD   wd_add;
			** replace by next line *****/
			NEW_ZHCXYD_ADD wd_add;
			/* END NEWYX by LiuHuafeng 2007-4-4 4:15 */
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****写入附加数据域****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));

		}
	}
	/* begin add by LiuHuafeng 2006-4-22 16:52 定期借记业务处理多方协议号 
	if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ,sizeof(wp_payin->F_pkgno))==0)
	{
	char cDfxyh[61];
	memset(cDfxyh, 0 , sizeof(cDfxyh));
	memcpy(cDfxyh,lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA),sizeof(cDfxyh)-1);
	vtcp_log("[%s][%d] 多方协议号==[%s]\n",__FL__,cDfxyh);
	iRc=iLvSetAddit(wp_payin,"XYH",cDfxyh,strlen(cDfxyh));
	if (iRc)
	{
	vtcp_log("[%s][%d] error here at add xyh iRc=[%d]\n",__FL__,iRc);
	sprintf(acErrMsg,"处理协议号出错 %d", iRc);
	return(-1);
	}
	}
	****************************/    
	/* end by LiuHuafeng 2006-4-22 17:04*/
	iLvPayinAutoInfo(wp_payin);/**得到日期和流水号**/
	vtcp_log("[%s][%d] actype==[%.c] \n",__FL__,wp_payin->F_actype[0]);
	get_fd_data("0030", wp_payin->A_brno);
	iLvZipAll(wp_payin);/**压缩所有空格***/
	vtcp_log("[%s][%d] wp_payin->F_opackid==[%s] \n",__FL__,wp_payin->F_packid);
	vtcp_log("[%s][%d]wp_payin->F_txamt=[%.15s]\n",__FL__,wp_payin->F_txamt);
	vtcp_log("[%s][%d] !F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	vtcp_log("[%s][%d] !F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	vtcp_log("[%s][%d] !F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	/**NEWTCTD9:增加手续费标志和金额***/
	{
		char cFeeFlag[2];
		double dFeeAmt;	
		char sTmp[20];
		
		memset(cFeeFlag,'\0',sizeof(cFeeFlag));
		memset(sTmp,'\0',sizeof(sTmp));
		dFeeAmt=0;
		get_zd_data("0093",cFeeFlag);
		get_zd_double("1229",&dFeeAmt);
		wp_payin->F_feeflag[0]=cFeeFlag[0];
		sprintf(sTmp,"%015.0f",dFeeAmt*100);
		memcpy(wp_payin->F_feeamt,sTmp,sizeof(wp_payin->F_feeamt));
		vtcp_log("[%s][%d] !Gethere9,F_feeamt       =[%.15s] ",__FL__,wp_payin->F_feeamt       );
	}
	/**NEWTCTD9:增加手续费标志和金额***/
	return(0);
}
/***************************************************************************/
/* 函数名称: int iLvNewRecFromPayin(PAY_IN_AREA *wp_payin)                 */
/* 功能描述: 根据payin输入内容产生一条新的支付记录                         */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvNewRecFromPayin(PAY_IN_AREA *wp_payin)
{
	/**认为调用此函数时，输入部分已经有值，但表记录区还没有内容**/
	/**认为调用此函数时，已经通过检查，所有字段域的字段值都已生成**/
	/* add by LiuHuafeng 2006-9-29 23:54 防止金额前边有空格,先将金额转换一下 */
	vtcp_log("[%s][%d]wp_payin->F_payactno=[%.32s]",__FL__,wp_payin->F_payactno);	/* Added by ChenMing  2007-1-15 10:42*/

	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	{
		double dTmpbal=0.0;
		char cTmpbal1[20];
		char cTmpbal2[20];
		memset(cTmpbal1, 0,sizeof(cTmpbal1));
		memset(cTmpbal2, 0,sizeof(cTmpbal2));
		memcpy(cTmpbal1,wp_payin->F_txamt,sizeof(wp_payin->F_txamt));
		zip_space(cTmpbal1);
		vtcp_log("[%s][%d]wp_payin->cTmpbal=[%s]",__FL__,cTmpbal1);
		lv_orderno_add0(cTmpbal1, sizeof(wp_payin->F_txamt), cTmpbal2);
		vtcp_log("[%s][%d]wp_payin->cTmpbal2=[%s]",__FL__,cTmpbal2);
		memcpy(wp_payin->F_txamt,cTmpbal2,sizeof(wp_payin->F_txamt));
		vtcp_log("[%s][%d]wp_payin->F_txamt=[%.15s]",__FL__,wp_payin->F_txamt);

		memset(cTmpbal1, 0,sizeof(cTmpbal1));
		memset(cTmpbal2, 0,sizeof(cTmpbal2));
		memcpy(cTmpbal1,wp_payin->F_ptxamt,sizeof(wp_payin->F_ptxamt));
		zip_space(cTmpbal1);
		lv_orderno_add0(cTmpbal1, sizeof(wp_payin->F_ptxamt), cTmpbal2);
		memcpy(wp_payin->F_ptxamt,cTmpbal2, sizeof(wp_payin->F_ptxamt));
		vtcp_log("[%s][%d]wp_payin->F_ptxamt=[%.15s]",__FL__,wp_payin->F_ptxamt);

		memset(cTmpbal1, 0,sizeof(cTmpbal1));
		memset(cTmpbal2, 0,sizeof(cTmpbal2));
		memcpy(cTmpbal1,wp_payin->F_jtxamt,sizeof(wp_payin->F_jtxamt));
		zip_space(cTmpbal1);
		lv_orderno_add0(cTmpbal1, sizeof(wp_payin->F_jtxamt), cTmpbal2);
		memcpy(wp_payin->F_jtxamt,cTmpbal2, sizeof(wp_payin->F_jtxamt));
		vtcp_log("[%s][%d]wp_payin->F_jtxamt=[%.15s]",__FL__,wp_payin->F_jtxamt);
	}
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname); 
	/* end by LiuHuafeng 2006-9-29 23:55 */
	/* add by LiuHuafeng 2006-11-11 0:30 将信息内容转换一下防止出现空格被转换为\0 */

	vtcp_log("[%s][%d]lw_ind=[%s]\n",__FILE__,__LINE__,wp_payin->F_lw_ind);
	if (wp_payin->F_lw_ind[0]=='2')
	{
		char cName1[130];
		memset(cName1, 0 , sizeof(cName1));
		strncpy(cName1,wp_payin->F_payname,60);
		dbc_zipspace(cName1);
		memcpy(wp_payin->F_payname,cName1,60);
		memset(cName1, 0 , sizeof(cName1));
		strncpy(cName1,wp_payin->F_cashname,60);
		dbc_zipspace(cName1);
		memcpy(wp_payin->F_cashname,cName1,60);
		memset(cName1, 0 , sizeof(cName1));
		strncpy(cName1,wp_payin->F_content,60);
		dbc_zipspace(cName1);
		memcpy(wp_payin->F_content,cName1,60);
		memset(cName1, 0 , sizeof(cName1));
		strncpy(cName1,wp_payin->F_payaddress,60);
		dbc_zipspace(cName1);
		memcpy(wp_payin->F_payaddress,cName1,60);
		memset(cName1, 0 , sizeof(cName1));
		strncpy(cName1,wp_payin->F_cashaddress,60);
		dbc_zipspace(cName1);
		memcpy(wp_payin->F_cashaddress,cName1,60);
	}
	lv_zip_payinF_space(wp_payin);/*add by LiuHuafeng 2007-1-15 16:19 */
	pub_base_full_anti_zero(wp_payin,sizeof(PAY_IN_AREA));
	memcpy(wp_payin->T_pkgno,wp_payin->F_pkgno, sizeof(wp_payin->T_pkgno));
	/**晋中网银在这里将平台传来录入状态改为复核状态 20130602 hzh**/
	if (strcmp(g_pub_tx.tx_code,"6388") == 0 || strcmp(g_pub_tx.tx_code,"6389") == 0 )
	{
	strcpy(wp_payin->T_pkgno,"001");
	strcpy(wp_payin->T_stat,"3");
	vtcp_log("%s,%d,晋中网银在这里将平台传来录入状态10改为复核状态30\n", __FL__);
	}
	/**晋中网银在这里将平台传来录入状态改为复核状态 20130602 hzh**/
	memcpy(wp_payin->T_inday,wp_payin->A_tbsdy, sizeof(wp_payin->T_inday));
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	memcpy(wp_payin->T_wtday,wp_payin->F_wtday, sizeof(wp_payin->T_wtday));
	vtcp_log("[%s][%d]wp_payin->T_wtday=[%s]",__FL__,wp_payin->T_wtday);
	memcpy(wp_payin->T_packday,wp_payin->F_packday,sizeof(wp_payin->T_packday));
	memcpy(wp_payin->T_packid,wp_payin->F_packid, sizeof(wp_payin->T_packid));
	vtcp_log("[%s][%d]wp_payin->T_packday=[%s]",__FL__,wp_payin->T_packday);
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	vtcp_log("[%s][%d]wp_payin->T_packid=[%s]",__FL__,wp_payin->T_packid);
	memcpy(wp_payin->T_orderno,wp_payin->F_orderno,sizeof(wp_payin->T_orderno));
	vtcp_log("[%s][%d]wp_payin->T_orderno=[%s]",__FL__,wp_payin->T_orderno);
	memcpy(wp_payin->T_ywdtlid,wp_payin->F_orderno,sizeof(wp_payin->T_ywdtlid));
	get_fd_data("0050", wp_payin->T_txtime);
	get_fd_data("0060", wp_payin->T_txtime+8);
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	vtcp_log("[%s][%d]wp_payin->T_txtime=[%s]",__FL__,wp_payin->T_txtime);
	memcpy(wp_payin->T_txnum,wp_payin->F_txnum,sizeof(wp_payin->T_txnum));
	vtcp_log("[%s][%d]===========wd_payin->F_txnum=[%s]\n",__FL__,wp_payin->F_txnum);
	vtcp_log("[%s][%d]===========wd_payin->T_txnum=[%s]\n",__FL__,wp_payin->T_txnum);
	memcpy(wp_payin->T_orbrno,wp_payin->F_orbrno, sizeof(wp_payin->T_orbrno));
	vtcp_log("[%s][%d]wp_payin->T_orbrno=[%s]",__FL__,wp_payin->T_orbrno);
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	memcpy(wp_payin->T_payqsactno,wp_payin->A_payqsactno,sizeof(wp_payin->T_payqsactno));
	memcpy(wp_payin->T_sendco,wp_payin->A_sendco,sizeof(wp_payin->T_sendco));
	memcpy(wp_payin->T_acbrno,wp_payin->F_acbrno,sizeof(wp_payin->T_acbrno));
	vtcp_log("[%s][%d]wp_payin->T_acbrno=[%s]",__FL__,wp_payin->T_acbrno);
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	memcpy(wp_payin->T_cashqsactno,wp_payin->A_cashqsactno,sizeof(wp_payin->T_cashqsactno));
	memcpy(wp_payin->T_receco,wp_payin->A_receco,sizeof(wp_payin->T_receco));
	memcpy(wp_payin->T_txamt,wp_payin->F_txamt, sizeof(wp_payin->T_txamt));
	vtcp_log("[%s][%d]===========wd_payin->F_txamt=[%s]\n",__FL__,wp_payin->F_txamt);
	vtcp_log("[%s][%d]===========wd_payin->T_txamt=[%s]\n",__FL__,wp_payin->T_txamt);
	memcpy(wp_payin->T_payno,wp_payin->F_payno, sizeof(wp_payin->T_payno));
	vtcp_log("[%s][%d]wp_payin->F_payno=[%.12s]",__FL__,wp_payin->F_payno);

	vtcp_log("[%s][%d]wp_payin->F_payactno=[%.32s]",__FL__,wp_payin->F_payactno);	/* Added by ChenMing  2007-1-15 10:42*/
	memcpy(wp_payin->T_payactno,wp_payin->F_payactno,sizeof(wp_payin->T_payactno));
	vtcp_log("[%s][%d]wp_payin->F_payactno=[%.32s]",__FL__,wp_payin->F_payactno);

	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	memcpy(wp_payin->T_payname,wp_payin->F_payname,sizeof(wp_payin->T_payname));
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	vtcp_log("[%s][%d]wp_payin->T_payname=[%.60s]",__FL__,wp_payin->T_payname);
	memcpy(wp_payin->T_payaddress,wp_payin->F_payaddress,sizeof(wp_payin->T_payaddress));
	memcpy(wp_payin->T_cashno,wp_payin->F_cashno,sizeof(wp_payin->T_cashno));
	memcpy(wp_payin->T_cashactno,wp_payin->F_cashactno,sizeof(wp_payin->T_cashactno));
	vtcp_log("[%s][%d]wp_payin->T_cashactno=[%.32s]",__FL__,wp_payin->T_cashactno);
	vtcp_log("[%s][%d]wp_payin->F_cashname=[%.60s]",__FL__,wp_payin->F_cashname);
	memcpy(wp_payin->T_cashname,wp_payin->F_cashname,sizeof(wp_payin->T_cashname));
	vtcp_log("[%s][%d]wp_payin->T_cashname=[%.60s]",__FL__,wp_payin->T_cashname);
	memcpy(wp_payin->T_cashaddress,wp_payin->F_cashaddress,sizeof(wp_payin->T_cashaddress));
	vtcp_log("[%s][%d]wp_payin->T_cashaddress=[%.60s]",__FL__,wp_payin->T_cashaddress);
	memcpy(wp_payin->T_ywtype,wp_payin->F_ywtype,sizeof(wp_payin->T_ywtype));
	vtcp_log("[%s][%d]wp_payin->T_ywtype=[%.12s]",__FL__,wp_payin->T_ywtype);
	memcpy(wp_payin->T_content,wp_payin->F_content,sizeof(wp_payin->T_content));
	memcpy(wp_payin->T_respday,wp_payin->F_respday,sizeof(wp_payin->T_respday));
	vtcp_log("[%s][%d]wp_payin->T_respday=[%s]",__FILE__,__LINE__,wp_payin->T_respday);
	memcpy(wp_payin->T_rcpstat,wp_payin->F_rcpstat,sizeof(wp_payin->T_rcpstat));
	memcpy(wp_payin->T_resp1,wp_payin->F_resp1, sizeof(wp_payin->T_resp1));
	memcpy(wp_payin->T_resp2,wp_payin->F_resp2, sizeof(wp_payin->T_resp2));
	memcpy(wp_payin->T_voctype,wp_payin->F_voctype,sizeof(wp_payin->T_voctype));
	vtcp_log("[%s][%d]wp_payin->F_voctype=[%s]",__FL__,wp_payin->F_voctype);
	memcpy(wp_payin->T_vocnum,wp_payin->F_vocnum, sizeof(wp_payin->T_vocnum));
	memcpy(wp_payin->T_ptype,wp_payin->F_ptype, sizeof(wp_payin->T_ptype));
	vtcp_log("[%s][%d]wp_payin->T_vocnum=[%s]",__FL__,wp_payin->T_vocnum);
	memcpy(wp_payin->T_pdate,wp_payin->F_pdate, sizeof(wp_payin->T_pdate));
	memcpy(wp_payin->T_pnum,wp_payin->F_pnum, sizeof(wp_payin->T_pnum));
	vtcp_log("[%s][%d]wp_payin->T_pnum=[%.5s]",__FL__,wp_payin->T_pnum);
	memcpy(wp_payin->T_ptxamt,wp_payin->F_ptxamt, sizeof(wp_payin->T_ptxamt));
	vtcp_log("[%s][%d]wp_payin->F_ptxamt=[%.15s]",__FL__,wp_payin->F_ptxamt);
	memcpy(wp_payin->T_jtxamt,wp_payin->F_jtxamt, sizeof(wp_payin->T_jtxamt));
	wp_payin->T_lw_ind[0]  = wp_payin->F_lw_ind[0];
	vtcp_log("[%s][%d]wp_payin->T_lw_ind=[%c]",__FL__,wp_payin->T_lw_ind[0]);
	memcpy(wp_payin->T_dfxyno,wp_payin->F_dfxyno, sizeof(wp_payin->T_dfxyno));
	vtcp_log("[%s][%d]wp_payin->T_dfxyno=[%s]",__FL__,wp_payin->T_dfxyno);
	memcpy(wp_payin->T_o_or_br_no,wp_payin->F_o_or_br_no, sizeof(wp_payin->T_o_or_br_no));
	vtcp_log("[%s][%d]wp_payin->T_o_or_br_no=[%s]",__FL__,wp_payin->T_o_or_br_no);
	memcpy(wp_payin->T_o_ac_br_no,wp_payin->F_o_ac_br_no, sizeof(wp_payin->T_o_ac_br_no));
	vtcp_log("[%s][%d]wp_payin->T_o_ac_br_no=[%s]",__FL__,wp_payin->T_o_ac_br_no);

	/****Modified by SSH,2005.12.2,来帐可能机构号不取kinbr****/
	if (wp_payin->A_brno[0]=='\0')
	{
		get_fd_data("0030", wp_payin->T_brno);
		vtcp_log("[%s][%d]wp_payin->T_brno=[%s]",__FL__,wp_payin->T_brno);
	}else
	{
		memcpy(wp_payin->T_brno,wp_payin->A_brno,sizeof(wp_payin->T_brno));
		vtcp_log("[%s][%d]wp_payin->T_brno=[%s]",__FL__,wp_payin->T_brno);
	}
	if (memcmp(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd))==0)
	{
		get_fd_data("0070", wp_payin->T_recvno);
		vtcp_log("[%s][%d] recvwssrno=[%.6s],a_wssrno=[%.6s]\n",
			__FL__,wp_payin->T_recvwssrno,wp_payin->A_wssrno);
		memcpy(wp_payin->T_recvwssrno,wp_payin->A_wssrno,sizeof(wp_payin->T_recvwssrno));
		vtcp_log("[%s][%d] recvwssrno=[%.6s],a_wssrno=[%.6s]\n",
			__FL__,wp_payin->T_recvwssrno,wp_payin->A_wssrno);
		if (wp_payin->A_stat[0]!='\0')
		{
			wp_payin->T_stat[0]=wp_payin->A_stat[0];/*接收取检查后设置的状态*/
		}
		wp_payin->T_begstat[0]=wp_payin->T_stat[0];
	}
	else
	{
		get_fd_data("0070", wp_payin->T_inputno);
		memcpy(wp_payin->T_inwssrno,wp_payin->A_wssrno,sizeof(wp_payin->T_inwssrno));
		wp_payin->T_stat[0]=STAT_WZLR;
		wp_payin->T_begstat[0]=wp_payin->T_stat[0];
	}
		/**晋中网银在这里将平台传来录入状态改为复核状态 20130602 hzh**/
	if (strcmp(g_pub_tx.tx_code,"6388") == 0 || strcmp(g_pub_tx.tx_code,"6389") == 0 )
	{
	strcpy(wp_payin->T_stat,"3");
	vtcp_log("%s,%d,晋中网银在这里将状态改为复核状态\n", __FL__);
	}
	/**晋中网银在这里将平台传来录入状态改为复核状态 20130602 hzh**/
	vtcp_log("%s,%d 注意这里设置状态T_STAT=[%c]",__FL__,wp_payin->T_stat[0]);
	vtcp_log("%s,%d 注意这里设置状态A_STAT=[%c]",__FL__,wp_payin->A_stat[0]);
	vtcp_log("%s,%d 注意这里设置状态beg_STAT=[%c]",__FL__,wp_payin->T_begstat[0]);
	memcpy(wp_payin->T_operlevel,wp_payin->F_operlevel, sizeof(wp_payin->T_operlevel));
	memcpy(wp_payin->T_feeflag,wp_payin->F_feeflag, sizeof(wp_payin->T_feeflag));
	memcpy(wp_payin->T_feeamt,wp_payin->F_feeamt, sizeof(wp_payin->T_feeamt));
	wp_payin->T_checkflag[0]='0';/**未对账**/
	memcpy(wp_payin->T_owtday,wp_payin->F_owtday, sizeof(wp_payin->T_owtday));
	memcpy(wp_payin->T_otxnum,wp_payin->F_otxnum, sizeof(wp_payin->T_otxnum));
	memcpy(wp_payin->T_oorderno,wp_payin->F_oorderno, sizeof(wp_payin->T_oorderno));
	memcpy(wp_payin->T_retcode,wp_payin->F_retcode, sizeof(wp_payin->T_retcode));
	memcpy(wp_payin->T_actype,wp_payin->F_actype, sizeof(wp_payin->T_actype));
	memcpy(wp_payin->T_pswdtype,wp_payin->F_pswdtype, sizeof(wp_payin->T_pswdtype));
	memcpy(wp_payin->T_pswdcode,pcLvDesPswdCode(wp_payin->F_pswd), sizeof(wp_payin->T_pswdcode));
	memcpy(wp_payin->T_payunit,wp_payin->F_payunit, sizeof(wp_payin->T_payunit));
	memcpy(wp_payin->T_payterm,wp_payin->F_payterm, sizeof(wp_payin->T_payterm));
	memcpy(wp_payin->T_paytype,wp_payin->F_paytype, sizeof(wp_payin->T_paytype));
	return(0);
}
/***************************************************************************/
/* 函数名称: int iLvResetPayin(PAY_IN_AREA *wp_payin)                      */
/* 功能描述: 根据payin表数据区反填输入区内容                               */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvResetPayin(PAY_IN_AREA *wp_payin)
{
	memcpy(wp_payin->F_pkgno,wp_payin->T_pkgno, sizeof(wp_payin->F_pkgno));
	memcpy(wp_payin->F_orderno,wp_payin->T_orderno, sizeof(wp_payin->F_orderno));
	memcpy(wp_payin->F_txnum,wp_payin->T_txnum, sizeof(wp_payin->F_txnum));
	memcpy(wp_payin->F_orbrno,wp_payin->T_orbrno, sizeof(wp_payin->F_orbrno));
	memcpy(wp_payin->F_acbrno,wp_payin->T_acbrno, sizeof(wp_payin->F_acbrno));
	memcpy(wp_payin->F_txamt,wp_payin->T_txamt, sizeof(wp_payin->F_txamt));
	memcpy(wp_payin->F_payno,wp_payin->T_payno, sizeof(wp_payin->F_payno));
	memcpy(wp_payin->F_payactno,wp_payin->T_payactno, sizeof(wp_payin->F_payactno));
	memcpy(wp_payin->F_payname,wp_payin->T_payname, sizeof(wp_payin->F_payname));
	memcpy(wp_payin->F_payaddress,wp_payin->T_payaddress, sizeof(wp_payin->F_payaddress));
	memcpy(wp_payin->F_cashno,wp_payin->T_cashno, sizeof(wp_payin->F_cashno));
	memcpy(wp_payin->F_cashactno,wp_payin->T_cashactno, sizeof(wp_payin->F_cashactno));
	memcpy(wp_payin->F_cashname,wp_payin->T_cashname, sizeof(wp_payin->F_cashname));
	memcpy(wp_payin->F_cashaddress,wp_payin->T_cashaddress, sizeof(wp_payin->F_cashaddress));
	memcpy(wp_payin->F_ywtype,wp_payin->T_ywtype, sizeof(wp_payin->F_ywtype));
	memcpy(wp_payin->F_content,wp_payin->T_content, sizeof(wp_payin->F_content));
	memcpy(wp_payin->F_wtday,wp_payin->T_wtday, sizeof(wp_payin->F_wtday));
	memcpy(wp_payin->F_operlevel,wp_payin->T_operlevel, sizeof(wp_payin->F_operlevel));
	memcpy(wp_payin->F_authno,wp_payin->T_authno, sizeof(wp_payin->F_authno));
	memcpy(wp_payin->F_voctype,wp_payin->T_voctype, sizeof(wp_payin->F_voctype));
	memcpy(wp_payin->F_vocnum,wp_payin->T_vocnum, sizeof(wp_payin->F_vocnum));
	memcpy(wp_payin->F_packday,wp_payin->T_packday, sizeof(wp_payin->F_packday));
	memcpy(wp_payin->F_packid,wp_payin->T_packid, sizeof(wp_payin->F_packid));
	vtcp_log("[%s][%d]wp_payin->T_packday=[%s]",__FILE__,__LINE__,wp_payin->T_packday);
	vtcp_log("[%s][%d]wp_payin->T_packid=[%s]",__FILE__,__LINE__,wp_payin->T_packid);
	memcpy(wp_payin->F_feeflag,wp_payin->T_feeflag, sizeof(wp_payin->F_feeflag));
	memcpy(wp_payin->F_feeamt,wp_payin->T_feeamt, sizeof(wp_payin->F_feeamt));
	memcpy(wp_payin->F_respday,wp_payin->T_respday, sizeof(wp_payin->F_respday));
	memcpy(wp_payin->A_oldrespday,wp_payin->T_respday, sizeof(wp_payin->A_oldrespday));
	vtcp_log("[%s][%d]wp_payin->A_oldrespday=[%s]",__FILE__,__LINE__,wp_payin->A_oldrespday);
	memcpy(wp_payin->F_ptype,wp_payin->T_ptype, sizeof(wp_payin->F_ptype));
	memcpy(wp_payin->F_pdate,wp_payin->T_pdate, sizeof(wp_payin->F_pdate));
	memcpy(wp_payin->F_pnum,wp_payin->T_pnum, sizeof(wp_payin->F_pnum));
	memcpy(wp_payin->F_ptxamt,wp_payin->T_ptxamt, sizeof(wp_payin->F_ptxamt));
	memcpy(wp_payin->F_jtxamt,wp_payin->T_jtxamt, sizeof(wp_payin->F_jtxamt));
	memcpy(wp_payin->F_owtday,wp_payin->T_owtday, sizeof(wp_payin->F_owtday));
	memcpy(wp_payin->F_otxnum,wp_payin->T_otxnum, sizeof(wp_payin->F_otxnum));
	memcpy(wp_payin->F_oorderno,wp_payin->T_oorderno, sizeof(wp_payin->F_oorderno));
	memcpy(wp_payin->F_retcode,wp_payin->T_retcode, sizeof(wp_payin->F_retcode));
	memcpy(wp_payin->F_rcpstat,wp_payin->T_rcpstat, sizeof(wp_payin->F_rcpstat));
	memcpy(wp_payin->F_resp1,wp_payin->T_resp1, sizeof(wp_payin->F_resp1));
	memcpy(wp_payin->F_resp2,wp_payin->T_resp2, sizeof(wp_payin->F_resp2));
	memcpy(wp_payin->F_actype,wp_payin->T_actype, sizeof(wp_payin->F_actype));
	memcpy(wp_payin->F_pswdtype,wp_payin->T_pswdtype, sizeof(wp_payin->F_pswdtype));
	memcpy(wp_payin->F_payunit,wp_payin->T_payunit, sizeof(wp_payin->F_payunit));
	memcpy(wp_payin->F_payterm,wp_payin->T_payterm, sizeof(wp_payin->F_payterm));
	memcpy(wp_payin->F_paytype,wp_payin->T_paytype, sizeof(wp_payin->F_paytype));
	memcpy(wp_payin->F_dfxyno,wp_payin->T_dfxyno, sizeof(wp_payin->F_dfxyno));
	memcpy(wp_payin->F_o_or_br_no,wp_payin->T_o_or_br_no, sizeof(wp_payin->F_o_or_br_no));
	memcpy(wp_payin->F_o_ac_br_no,wp_payin->T_o_ac_br_no, sizeof(wp_payin->F_o_ac_br_no));
	memcpy(wp_payin->A_stat,wp_payin->T_stat, sizeof(wp_payin->A_stat));
	memcpy(wp_payin->A_sendco,wp_payin->T_sendco, sizeof(wp_payin->A_sendco));
	memcpy(wp_payin->A_payqsactno,wp_payin->T_payqsactno, sizeof(wp_payin->A_payqsactno));
	memcpy(wp_payin->A_receco,wp_payin->T_receco, sizeof(wp_payin->A_receco));
	memcpy(wp_payin->A_cashqsactno,wp_payin->T_cashqsactno, sizeof(wp_payin->A_cashqsactno));
	vtcp_log("[%s][%d]==============wp_payin->T_pkgno=[%s]\n",__FL__,wp_payin->T_pkgno);
	if (memcmp(wp_payin->T_pkgno,PKGNO_PTDJ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_DQDJ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_SSDJ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->T_pkgno))==0)
	{ 
		wp_payin->F_crdb[0]=CRDB_CR;
	}else if(memcmp(wp_payin->T_pkgno,PKGNO_PTJJ, sizeof(wp_payin->T_pkgno))==0 ||
	memcmp(wp_payin->T_pkgno,PKGNO_SSJJ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_DQJJ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->T_pkgno))==0 ||
		memcmp(wp_payin->T_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->T_pkgno))==0)
	{
		wp_payin->F_crdb[0]=CRDB_DR;
	}
	memcpy(wp_payin->A_tbsdy,pcLvGetDay(),sizeof(wp_payin->A_tbsdy));/*日期*/
	get_fd_data("0040", wp_payin->A_wssrno);

	memcpy(wp_payin->A_brno,wp_payin->T_brno,sizeof(wp_payin->T_brno));
	vtcp_log("%s,%d,After reset,pkgno=%.3s,crdb=%c\n",__FL__, wp_payin->T_pkgno,wp_payin->F_crdb[0]);
	return(0);		

}
/***************************************************************************/
/* 函数名称: int iLvPayinCheck0(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 小额系统通用检查函数                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvPayinCheck0(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char *ptr;
	/****NEWYX BEGIN:用于组织金额上限检查的sql语句****/
	char sql_str[512];
	/****NEWYX END:用于组织金额上限检查的sql语句****/
	/**1.借贷标志、pkgno、txnum关系检查**/
	vtcp_log("[%s][%d]===========wp_payin->F_crdb[0]=[%c]\n",__FL__,wp_payin->F_crdb[0]);
	vtcp_log("[%s][%d]===========wp_payin->F_txnum=[%.5s]\n",__FL__,wp_payin->F_txnum);
	if (wp_payin->F_crdb[0]==CRDB_DR)
	{
		/**借记标志为借**/
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJ, sizeof(wp_payin->F_pkgno))==0||	
			memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0)
		{
			/**普通借记,回执**//****ADD NEWYX BEGIN : TYJL ****/
			if (memcmp(wp_payin->F_txnum,TXNUM_GKJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_QTJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_ZJZF,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_SQZF,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_GZJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))!=0 &&    
				memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))!=0 &&
                memcmp(wp_payin->F_txnum,"30104",5)!=0&&memcmp(wp_payin->F_txnum,"30105",5)!=0)
			{
				vtcp_log("%s,%d,txnum[%.5s]不合法范围\n",__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务编号不在范围内");
				return(-1);
			}
			/****NEWYX END : TYJL ****/
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJ, sizeof(wp_payin->F_pkgno))==0||	
			memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**实时借记,回执**/
			/****NEWYX BEGIN :通用截留不在此设置附加域****/
			if (memcmp(wp_payin->F_txnum,TXNUM_SSKS, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TD, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TYJL, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_DGTD, sizeof(wp_payin->F_txnum))!=0)
			{
				vtcp_log("%s,%d,txnum[%.5s]不在合法范围内\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务编号不在范围内");
				return(-1);
			}
			/****NEWYX END :通用截留不在此设置附加域****/
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ, sizeof(wp_payin->F_pkgno))==0||	
			memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**定期借记,回执**/
			if (memcmp(wp_payin->F_txnum,TXNUM_DQJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_PLKS,sizeof(wp_payin->F_txnum))!=0)
			{
				vtcp_log("%s,%d,txnum[%.5s]不在合法范围内\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务编号不在范围内");
				return(-1);
			}
		}
		else
		{
			vtcp_log("%s,%d,pkgno[%.3s]不在合法范围内\n",
				__FL__,wp_payin->F_pkgno);
			sprintf(acErrMsg,"包编号不在范围内");
			return(-1);
		}
		vtcp_log("[%s][%d]wp_payin->F_txnum=[%.5s]\n",__FL__,wp_payin->F_txnum);
	}
	else if (wp_payin->F_crdb[0]==CRDB_CR)
	{
		/**贷记业务**/
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,sizeof(wp_payin->F_pkgno))==0)
		{
			/**普通贷记**/
			if (memcmp(wp_payin->F_txnum,TXNUM_HD, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_WT, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TSCF, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_GKDJ, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_QTDJ, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_JS, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_FSSR, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_GZDJ, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_ZJTH, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_SQTH, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_JF, sizeof(wp_payin->F_txnum))!=0 )
			{
				vtcp_log("%s,%d,txnum[%.5s]不在合法范围内\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务类型不在范围内");
				return(-1);
			}
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_DQDJ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**定期贷记**/
			if (memcmp(wp_payin->F_txnum,TXNUM_DQDJ, sizeof(wp_payin->F_txnum))!=0)
			{ 
				vtcp_log("%s,%d,txnum[%.5s]不在合法范围内\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务编号不在范围内");
				return(-1);
			}
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJ, sizeof(wp_payin->F_pkgno))==0 ||
			memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**实时贷记、回执**/
			if (memcmp(wp_payin->F_txnum,TXNUM_TC, sizeof(wp_payin->F_txnum))!=0)
			{ 
				vtcp_log("%s,%d,txnum[%.5s]不在合法范围内\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务编号不在范围内");
				return(-1);
			}
			vtcp_log("[%s][%d]wp_payin->F_txnum=[%s]\n",__FL__,wp_payin->F_txnum);
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_DJTH, sizeof(wp_payin->F_pkgno))==0)
		{
			/**贷记退汇**/
			if (memcmp(wp_payin->F_txnum,TXNUM_DJTH, sizeof(wp_payin->F_txnum))!=0)
			{ 
				vtcp_log("%s,%d,txnum[%.5s]不在合法范围内\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"业务编号不在范围内");
				return(-1);
			}
		}
		else
		{
			vtcp_log("%s,%d,pkgno[%.3s]不在合法范围内\n",
				__FL__,wp_payin->F_pkgno);
			sprintf(acErrMsg,"包编号不在范围内");
			return(-1);
		}
	}
	else
	{
		/**错误的借贷标志**/
		vtcp_log("%s,%d,crdb[%.1s]不在合法范围内\n",
			__FL__,wp_payin->F_crdb[0]);
		sprintf(acErrMsg,"借贷标志不在范围内");
		return(-1);
	}
	vtcp_log("[%s][%d]wp_payin->F_txnum=[%.5s]\n",__FL__,wp_payin->F_txnum);
	/**2.交易状态检查**/
	if (memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_LR_QUERY, sizeof(wp_payin->F_opcd))==0)
	{
		/**录入交易不检查状态**/
	}
	else if (memcmp(wp_payin->F_opcd,OPCD_CHECK, sizeof(wp_payin->F_opcd))==0)
	{
		/**复核交易，原交易状态应为录入状态**/
		if (wp_payin->T_stat[0]!=STAT_WZLR)
		{	
			vtcp_log("%s,%d,原交易状态不是录入状态[%.c]\n",
				__FL__,wp_payin->T_stat[0]);
			sprintf(acErrMsg,"原交易不是录入状态");
			return(-1);
		}
	}
	else if (memcmp(wp_payin->F_opcd,OPCD_SEND, sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_SEND_QUERY, sizeof(wp_payin->F_opcd))==0)
	{
		/**发送交易，原状态应为正式复核状态**/
		if (wp_payin->T_stat[0]!=STAT_WZCHECK2)
		{	
			vtcp_log("%s,%d,原交易状态错误[%.c]\n",
				__FL__,wp_payin->T_stat[0]);
			sprintf(acErrMsg,"原交易不是复核状态");
			return(-1);
		}
	}
	else if (memcmp(wp_payin->F_opcd,OPCD_INACT, sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_INACT_QUERY, sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_INACT_MODIFY, sizeof(wp_payin->F_opcd))==0)
	{
		/**错账处理交易，原状态应为错账状态**/
		if (wp_payin->T_stat[0]!=STAT_LZNMAC_ERR &&	
			wp_payin->T_stat[0]!=STAT_LZLMAC_ERR &&	
			wp_payin->T_stat[0]!=STAT_LZAMAC_ERR &&	
			wp_payin->T_stat[0]!=STAT_LZGZ_QS &&	
			wp_payin->T_stat[0]!=STAT_LZGZ_BR)
		{
			vtcp_log("%s,%d,原交易状态错误1[%.c]\n",
				__FL__,wp_payin->T_stat[0]);
			sprintf(acErrMsg,"原交易不是错账状态");
			return(-1);
		}
	}
	/**晋中网银在这里将平台传来录入状态改为复核状态 20130602 hzh**/
		if (strcmp(g_pub_tx.tx_code,"6388") == 0 || strcmp(g_pub_tx.tx_code,"6389") == 0 )
	{
	strcpy(wp_payin->F_opcd,"30");
	vtcp_log("%s,%d,晋中网银在这里将平台传来录入状态10改为复核状态30\n", __FL__);
	}
	/**晋中网银在这里将平台传来录入状态改为复核状态 20130602 hzh**/
	/***非复核交易手续费不能收转帐**/
	if (memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))!=0)
	{
		if (wp_payin->F_feeflag[0]==FEEFLAG_ACT ||
			wp_payin->F_feeflag[0]==FEEFLAG_ACTJJ)
		{
			if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin->F_pkgno)))
			{
				vtcp_log("%s,%d,此交易不允许收取转帐手续费\n", __FL__);
				sprintf(acErrMsg,"此交易不允许收取转帐手续费");
				return(-1);
			}
		}
	}
	/****Add by xyy 2006-9-11 10:42 只有贷记业务才检查金额上限****/
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,3)==0 ||memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,3)==0 || \
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJ,3)==0 || memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,3)==0)
	{/****Add by SSH,2006.1.25,贷记金额上限检查****/
		struct lv_param_c wd_lv_param;
		memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
		memcpy(wd_lv_param.par_code,"CREBD",5);
		memcpy(wd_lv_param.par_code+5,wp_payin->F_pkgno,3);
		vtcp_log("%s,%d,检查贷记金额上限[%s]\n",__FL__,wd_lv_param.par_code);
		iRc = Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "par_code = '%s' ", wd_lv_param.par_code);
		if (iRc!=0 && iRc!= DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"数据库错误");
			return(-1);
		}
		if (iRc!=DB_NOTFOUND)
		{
			/****如果有才检查****/
			zip_space(wd_lv_param.par_value);
			if (strchr(wd_lv_param.par_value,'.')!=NULL || strlen(wd_lv_param.par_value)!=15)
			{
				double dTmp=0;
				dTmp=atof(wd_lv_param.par_value)*100;
				memset(wd_lv_param.par_value,'\0',sizeof(wd_lv_param.par_value));
				dbl2str(&dTmp,0,sizeof(wp_payin->F_txamt),0,wd_lv_param.par_value);

			}
			if (memcmp(wp_payin->F_txamt,wd_lv_param.par_value,sizeof(wp_payin->F_txamt))>0)
			{
				vtcp_log("%s,%d,金额[%.15s]超出上限[%.15s]\n",__FL__,wp_payin->F_txamt,wd_lv_param.par_value);
				sprintf(acErrMsg,"交易金额超出上限");
				return(-1);

			}
		}
		/**begin add by LiuHuafeng CREBOUND 和CREBD都要判断 */
		memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
		/****Modified by SSH,NEWYX2:支票回执不检查CREBOUND****/
		if (memcmp(wp_payin->F_txnum,"30102",5)!=0 &&
			memcmp(wp_payin->F_txnum,"30103",5)!=0 &&
			memcmp(wp_payin->F_txnum,"30104",5)!=0 &&
			memcmp(wp_payin->F_txnum,"30105",5)!=0)
		{
			memcpy(wd_lv_param.par_code,"CREBOUND",8);
			vtcp_log("%s,%d,检查贷记金额上限[%s]\n",__FL__,wd_lv_param.par_code);
			iRc=Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "par_code = '%s' ", wd_lv_param.par_code);
			if (iRc!=0 && iRc!= DB_NOTFOUND)
			{
				vtcp_log("%s,%d,数据库错误[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"数据库错误");
				return(-1);
			}
			if (iRc!=DB_NOTFOUND)
			{
				/****如果有才检查****/
				zip_space(wd_lv_param.par_value);
				if (strchr(wd_lv_param.par_value,'.')!=NULL || strlen(wd_lv_param.par_value)!=15)
				{
					double dTmp=0;
					dTmp=atof(wd_lv_param.par_value)*100;
					memset(wd_lv_param.par_value,'\0',sizeof(wd_lv_param.par_value));
					dbl2str(&dTmp,0,sizeof(wp_payin->F_txamt),0,wd_lv_param.par_value);

				}
				if (memcmp(wp_payin->F_txamt,wd_lv_param.par_value,sizeof(wp_payin->F_txamt))>0)
				{
					vtcp_log("%s,%d,金额[%.15s]超出上限[%.15s]\n",__FL__,wp_payin->F_txamt,wd_lv_param.par_value);
					sprintf(acErrMsg,"交易金额超出上限");
					return(-1);
				}
			}
		}
		/****NEWYX BEGIN:V3.0.2版小额金额上限报文又变了****/
		memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
		iRc=Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "substr(par_code,1,1)='C' and (substr(par_code,2,3)='%.3s' or substr(par_code,2,3)='ALL') and (substr(par_code,5,3)='%.3s' or substr(par_code,5,3)='ALL') and (substr(par_code,8,5)='%.5s' or substr(par_code,8,5)='XX%.3s' or substr(par_code,8,5)='%.3sXX')", wp_payin->F_orbrno,wp_payin->F_acbrno,wp_payin->F_txnum,wp_payin->F_pkgno,wp_payin->F_pkgno);
		if (iRc!=0 && iRc!= DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"数据库错误");
			return(-1);
		}
		if (iRc!=DB_NOTFOUND)
		{
			/****如果有才检查****/
			vtcp_log("[%s][%d]这里开始检查金额上限!!!",__FILE__,__LINE__);
			zip_space(wd_lv_param.par_value);
			if (strchr(wd_lv_param.par_value,'.')!=NULL || strlen(wd_lv_param.par_value)!=15)
			{
				double dTmp=0;
				dTmp=atof(wd_lv_param.par_value)*100;
				memset(wd_lv_param.par_value,'\0',sizeof(wd_lv_param.par_value));
				dbl2str(&dTmp,0,sizeof(wp_payin->F_txamt),0,wd_lv_param.par_value);
			}
			if (memcmp(wp_payin->F_txamt,wd_lv_param.par_value,sizeof(wp_payin->F_txamt))>0)
			{
				vtcp_log("%s,%d,金额[%.15s]超出上限[%.15s]\n",__FILE__,__LINE__,wp_payin->F_txamt,wd_lv_param.par_value);
				vtcp_log("%s,%d,交易金额超出上限[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"交易金额超出上限");
				return(-1);
			}
		}
		/****NEWYX BEGIN:V3.0.2版小额金额上限报文又变了****/
		/**end add by LiuHuafeng CREBOUND 和CREBD都要判断 */
	}
	/****Deleted by SSH,2006.1.25,现在的模式应该是允许跨小额日取消的****
	**************不能跨小额日期取消********
	*****************************************************************/
	/*****add by LiuHuafeng 2006-3-29 23:05 */

	return(0);
}
/***************************************************************************/
/* 函数名称: int iLvPayinCheck1(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 小额系统行号检查函数                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
/****Add by SSH,2006.5.9,在流水中增加事后监督内容****/

int iLvPayinCheck1(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	struct hv_uniontab_c   wp_hvuniontab_send;
	struct hv_uniontab_c   wp_hvuniontab_rec;
	memset(&wp_hvuniontab_send, 0, sizeof(wp_hvuniontab_send));
	memset(&wp_hvuniontab_rec,  0, sizeof(wp_hvuniontab_rec));
	memcpy(wp_hvuniontab_send.or_br_no,wp_payin->F_orbrno, sizeof(wp_hvuniontab_send.or_br_no)-1);
	vtcp_log("[%s][%d]==========wp_hvuniontab_send.or_br_no=[%s]\n",__FL__,wp_hvuniontab_send.or_br_no);
	iRc=Hv_uniontab_Sel(g_pub_tx.reply, &wp_hvuniontab_send, "or_br_no = '%s' ", wp_hvuniontab_send.or_br_no);
	if (iRc!=DB_OK)
	{
		vtcp_log("[%s] [%d] 查询发起行行号错[%d]\n",
			__FL__,iRc);
		sprintf(acErrMsg,"发起行行号不存在!");
		return(-1);
	}
	vtcp_log("%s,%d,发起行:[%s][%s]\n",__FL__,
		wp_hvuniontab_send.or_br_no,wp_hvuniontab_send.qs_no);
	if ( wp_hvuniontab_send.br_sts[0] != '1')
	{
		vtcp_log("[%s] [%d] 发起行状态错误!\n",__FL__);
		sprintf(acErrMsg,"发起行状态错误!");
		return(-1);
	}
	memcpy(wp_hvuniontab_rec.or_br_no,wp_payin->F_acbrno, sizeof(wp_hvuniontab_rec.or_br_no)-1);
	iRc=Hv_uniontab_Sel(g_pub_tx.reply, &wp_hvuniontab_rec, "or_br_no = '%s' ", wp_hvuniontab_rec.or_br_no);
	if (iRc!=DB_OK)
	{
		vtcp_log("[%s] [%d] 查询接收行行号错rec.or_br_no=[%s]sqlcode=[%d]\n",
			__FL__,wp_hvuniontab_rec.or_br_no,iRc);
		sprintf(acErrMsg,"接收行行号不存在!");
		return(-1);
	}
	vtcp_log("%s,%d,接收行:[%s][%s]\n",__FL__,
		wp_hvuniontab_rec.or_br_no,wp_hvuniontab_rec.qs_no);
	if ( wp_hvuniontab_rec.br_sts[0] != '1')
	{
		vtcp_log("[%s] [%d] 接收行状态错误!\n",__FL__);
		sprintf(acErrMsg,"接收行状态错误!");
		return(-1);
	}
	/* add by LiuHuafeng 20070508 录入检查503表 */
	if(!memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd)))
	{
		struct lv_c503r_c sLv_c503r;
		char cAuth[9];
		char *P;
		memset(cAuth, 0 , sizeof(cAuth));
		memset(&sLv_c503r, 0 , sizeof(sLv_c503r));
		iRc=Lv_c503r_Sel(g_pub_tx.reply, &sLv_c503r, "bakcode = '%s' ", wp_hvuniontab_rec.qs_no);
		if(iRc)
		{
			vtcp_log("[%s] [%d] 接收行业务权限不存在!\n",__FL__);
			sprintf(acErrMsg,"接收行无此业务权限!");
			return(-1);
		}
		if(sLv_c503r.sendcnt<0)
		{
			vtcp_log("[%s] [%d] 接收行业务权限实效!\n",__FL__);
			sprintf(acErrMsg,"接收行此业务权限实效!");
			return(-1);
		}
		memcpy(cAuth,wp_payin->F_pkgno,sizeof(wp_payin->F_pkgno));
		memcpy(cAuth+sizeof(wp_payin->F_pkgno),wp_payin->F_txnum,5);
		P=strstr(sLv_c503r.recvgrant,cAuth);
		if(P)
		{
			vtcp_log("%s,%d 该清算行有此%s业务权限",__FILE__,__LINE__,cAuth);
		}
		else
		{
			vtcp_log("%s,%d 清算行%s无%s业务权限",__FILE__,__LINE__,wp_hvuniontab_rec.qs_no,cAuth);
			sprintf(acErrMsg,"接收行无此业务权限!");
			return(-1);
		}
	}
	/* end by LiuHuafeng 20070508 */
	/** rem by LiuHuafeng 20070407 for NEWYX
	if (memcmp(wp_hvuniontab_send.qs_no,wp_hvuniontab_rec.qs_no, sizeof(wp_hvuniontab_send.qs_no)-1)==0)
	{
	vtcp_log("[%s][%d]发起清算行和接收清算行不能是同一机构!\n",
	__FL__);
	sprintf(acErrMsg,"发起清算行和接收清算行不能是同一机构!");
	return(-1);
	}
	***********************/
	memcpy(wp_payin->A_sendco,wp_hvuniontab_send.sendco, sizeof(wp_payin->A_sendco));
	memcpy(wp_payin->A_payqsactno,wp_hvuniontab_send.qs_no, sizeof(wp_payin->A_payqsactno));
	memcpy(wp_payin->A_receco,wp_hvuniontab_rec.sendco, sizeof(wp_payin->A_receco));
	memcpy(wp_payin->A_cashqsactno,wp_hvuniontab_rec.qs_no, sizeof(wp_payin->A_cashqsactno));
	return(0);
}
/****付款帐号检查************************************/
/*   返回值的含义                                   */
/*   #define ACT_OK  0                 帐户状态正常 */
/*   #define ACT_ERR 1                   帐号不存在 */
/*   #define ACT_NAME_ERR 2            帐户户名错误 */
/*   #define ACT_AVBAL_LOW 3           帐户余额不足 */
/*   #define ACT_PASSWD_ERR 10         帐户密码错误 */
/*   #define ACT_FZN 11                    状态错误 */
/*   #define ACT_OTHER_ERR -1              其他错误 */
/****************************************************/   
/****Modified by SSH,2005.12.2,考虑来帐时要先设置机构号****/

/***************************************************************************/
/* 函数名称: int iLvGetRecvBrno(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 根据接受机构设置A_brno                                        */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvGetRecvBrno(PAY_IN_AREA *wp_payin)
{
	struct hv_orno_brno_c sHv_orno_brno;
	int iRc=0;
	memset(&sHv_orno_brno,'\0',sizeof(sHv_orno_brno));
	if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))==0)
	{
		memcpy(sHv_orno_brno.or_br_no,wp_payin->F_orbrno, sizeof(sHv_orno_brno.or_br_no)-1);
		vtcp_log("[%s] [%d] or_br_no=[%s]\n",__FL__,sHv_orno_brno.or_br_no);
	}else
	{
		memcpy(sHv_orno_brno.or_br_no,wp_payin->F_acbrno,sizeof(sHv_orno_brno.or_br_no)-1);
		vtcp_log("[%s] [%d] or_br_no=[%s]\n",__FL__,sHv_orno_brno.or_br_no);
	}
	iRc=Hv_orno_brno_Sel(g_pub_tx.reply, &sHv_orno_brno, "or_br_no = '%s' ", sHv_orno_brno.or_br_no);
	if (iRc)
	{
		vtcp_log("[%s] [%d] or_br_no=[%s]\n",__FL__,sHv_orno_brno.or_br_no);
		vtcp_log("%s,%d,未找到接受机构，清算中心接受\n",__FL__);
		sprintf(acErrMsg,"未找到接受机构");
		/****本来机构就初始化为清算中心,所以不用做处理****/
	}else
	{
		zip_space(sHv_orno_brno.br_no);
		vtcp_log("[%s] [%d] or_br_no=[%s]\n",__FL__,sHv_orno_brno.or_br_no);
		memcpy(wp_payin->A_brno,sHv_orno_brno.br_no,sizeof(wp_payin->A_brno));
		set_zd_data("0030",sHv_orno_brno.br_no);
	}
	return(0);
}
/***************************************************************************/
/* 函数名称: int iLvPayinCheck2_Batch(PAY_IN_AREA *wp_payin)               */
/* 功能描述: 批量包来帐的付款帐号检查,主要是不能更改机构号                 */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
/*int iLvPayinCheck2_Batch(PAY_IN_AREA *wp_payin)*/

/***************************************************************************/
/* 函数名称: int iLvPayinCheck2(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 付款帐号检查                                                  */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:      去掉 gujy 060830                                                             */
/***************************************************************************/
/*int iLvPayinCheck2(PAY_IN_AREA *wp_payin)*/

/***************************************************************************/
/* 函数名称: int iLvPayinCheck3_Batch(PAY_IN_AREA *wp_payin)               */
/* 功能描述: 批量包来帐的收款帐号检查,主要是不能更改机构号                 */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:       去掉gujy.060830                                                            */
/***************************************************************************/

/***************************************************************************/
/* 函数名称: int iLvPayinCheck3(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 增加A_actype表示客户帐号类型                                  */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:     0-错误      																										 */
/*           1-储蓄,2-对公,3-内部帐,4-科目                                 */
/***************************************************************************/
int iLvPayinCheck3(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	int flag = 0;
	int length = 0;
	char cName[61];
	char cActno[33];
	char cLeftvalue[17];
	char cBrno[16];

	struct hv_orno_brno_c hv_orno_brno;

	memset(&hv_orno_brno,'\0',sizeof(hv_orno_brno));
	memset(cName, 0, sizeof(cName));
	memset(cActno, 0, sizeof(cActno));
	memset(cLeftvalue, 0, sizeof(cLeftvalue));
	wp_payin->A_stat[0]=STAT_LZRECV2;
	wp_payin->A_acttype[0]='4';
	memset(wp_payin->A_acno,'\0',sizeof(wp_payin->A_acno));
	/**********设置机构号初试值************/
	if (memcmp(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd))==0)
	{
		vtcp_log("%s,%d\n", __FL__);
		/***只有接收交易才需要设置机构号***/
		/*begin add by LiuHuafeng 20060302 */
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0
			||memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))==0
			||memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0)
			memcpy(hv_orno_brno.or_br_no,wp_payin->F_orbrno,
			sizeof(hv_orno_brno.or_br_no)-1);	
		else
		/* end by LiuHuafeng 20060302 */
			memcpy(hv_orno_brno.or_br_no, wp_payin->F_acbrno,
			sizeof(hv_orno_brno.or_br_no)-1);
		vtcp_log("%s,%dhv_orno_brno.or_br_no=[%s]", __FL__,hv_orno_brno.or_br_no);	
		iRc=Hv_orno_brno_Sel(g_pub_tx.reply, &hv_orno_brno, "or_br_no = '%s' ", hv_orno_brno.or_br_no);
		if (iRc!=DB_OK)
		{
			/**接收行不存在,机构取清算中心**/
			memset(cBrno, 0x0, sizeof(cBrno));
			memcpy(cBrno,HV_CZ_QSBRNO,BRNO_LEN);
			vtcp_log("%s,%dcBrno=[%s]", __FL__,cBrno);
			set_zd_data("0030", cBrno);

		}else
		{
			zip_space(hv_orno_brno.br_no);
			set_zd_data("0030", hv_orno_brno.br_no);
			vtcp_log("%s,%dhv_orno_brno.br_no=[%s]", __FL__,hv_orno_brno.br_no);

		}
		vtcp_log("%s,%dhv_orno_brno.br_no=[%s]", __FL__,hv_orno_brno.br_no);
		pcLvGetRecvno();
		vtcp_log("%s,%dhv_orno_brno.br_no=[%s]", __FL__,hv_orno_brno.br_no);
		get_fd_data("0040", wp_payin->A_wssrno);/**流水号**/
	}
	/*begin add by LiuHuafeng 20060218 */
	memcpy(cActno,wp_payin->F_cashactno,sizeof(cActno)-1);
	zip_space(cActno);
	memcpy(wp_payin->F_cashactno,cActno,sizeof(wp_payin->F_cashactno));
	vtcp_log("%s,%dhv_orno_brno.br_no=[%s]", __FL__,hv_orno_brno.br_no);
	/* end by LiuHuafeng 20060218 */
	/**************************************/
	return(0);
}
/***************************************************************************/
/* 函数名称: int iLvPayinCheck4(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 要素合法性检查                                                */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:         																														 */
/***************************************************************************/
int iLvPayinCheck4(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char cDate[9];
	memset(cDate,'\0',sizeof(cDate));
	/****Add by SSH,2006.4.19,检查帐号中有没有汉字****/
	if (isAnyChinese(wp_payin->F_payactno,sizeof(wp_payin->F_payactno)))
	{
		vtcp_log("%s,%d,付款帐号中含有非法字符\n", __FL__);		
		sprintf(acErrMsg,"付款帐号中含有非法字符");
		return(-1);
	}
	if (isAnyChinese(wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno)))
	{
		vtcp_log("%s,%d,收款帐号中含有非法字符\n", __FL__);		
		sprintf(acErrMsg,"收款帐号中含有非法字符");
		return(-1);
	}
	/****检查交易金额的范围**/
	/************************/
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0)
	{
		/***非应答交易录入，填写wtday****/
		memcpy(wp_payin->F_wtday,wp_payin->A_tbsdy,
			sizeof(wp_payin->F_wtday));

	}else
	{
		/***委托日期不应大于小额系统日期***/
		if (memcmp(wp_payin->F_wtday,wp_payin->A_tbsdy,
			sizeof(wp_payin->F_wtday))>0)
		{
			/* add by LiuHuafeng 这样判断，
			* 如果是换日后收到借记来帐那么进行回执录入就会失败 */
			if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno)))
			{
				vtcp_log("%s,%d,委托日期不能大于小额系统日期!wtday=[%.8s] tbsdy=[%.8s]\n",__FL__,wp_payin->F_wtday,wp_payin->A_tbsdy);
				sprintf(acErrMsg,"委托日期不能大于小额系统日期!");
				return(-1);
			}
		}
	}
	vtcp_log("[%s][%d]wp_payin->A_tbsdy===[%s]",__FILE__,__LINE__,wp_payin->A_tbsdy);
	memcpy(cDate,wp_payin->F_pdate,sizeof(cDate)-1);
	zip_space(cDate);
	if (memcmp(cDate,wp_payin->F_wtday,sizeof(cDate)-1)>0)
	{
		vtcp_log("%s,%d,票据日期[%.8s]不能大于委托日期[%.8s]!\n",
			__FL__,cDate,wp_payin->F_wtday);
		sprintf(acErrMsg,"票据日期不能大于委托日期!");
		return(-1);
	}
	memcpy(cDate,pcLvGetDay(),sizeof(cDate)-1);
	zip_space(cDate);
	if (memcmp(cDate,wp_payin->A_tbsdy,sizeof(cDate)-1)<0 && cDate[0]!='\0')
	{
		vtcp_log("%s,%d,应答日期不能小于当前日期!cDate=[%s],wtday=[%s]\n",__FL__,cDate,wp_payin->F_wtday);
		sprintf(acErrMsg,"应答日期不能小于当前日期!");
		return(-1);
	}
	/****Add by SSH,2006.5.9,在流水中追加事后信息***/
	return(0);
}
/****授权关系检查******************************/
int Ilvpayincheck5(PAY_IN_AREA *wp_payin)
{
	/**CAUTION:***/
	/**以后不上错账处理的授权检查***/
	return(0);
}    
/***************************************************************************/
/* 函数名称: int iLvPayinCheck6(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 要素一致性检查                                                */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:         																														 */
/***************************************************************************/
int iLvPayinCheck6(PAY_IN_AREA *wp_payin)
{
	/****比较输入要素与表中要素是否一致****/
	pub_base_full_anti_zero(wp_payin,sizeof(PAY_IN_AREA));
	if (memcmp(wp_payin->F_pkgno,wp_payin->T_pkgno,
		sizeof(wp_payin->F_pkgno))!=0)
	{
		vtcp_log("%s,%d,pkgno[%.3s][%.3s]不一致",__FL__,
			wp_payin->F_pkgno,wp_payin->T_pkgno);
		sprintf(acErrMsg,"报文编号不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_orderno,wp_payin->T_orderno,
		sizeof(wp_payin->F_orderno))!=0)
	{
		vtcp_log("%s,%d,orderno[%.8s][%.8s]不一致",__FL__,
			wp_payin->F_orderno,wp_payin->T_orderno);
		sprintf(acErrMsg,"交易序号不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_txnum,wp_payin->T_txnum,
		sizeof(wp_payin->F_txnum))!=0)
	{
		vtcp_log("%s,%d,txnum[%.5s][%.5s]不一致",__FL__,
			wp_payin->F_txnum,wp_payin->T_txnum);
		sprintf(acErrMsg,"业务类型编号不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_acbrno,wp_payin->T_acbrno,
		sizeof(wp_payin->F_acbrno))!=0)
	{
		vtcp_log("%s,%d,acbrno[%.12s][%.12s]不一致",__FL__,
			wp_payin->F_acbrno,wp_payin->T_acbrno);
		sprintf(acErrMsg,"接收行不一致");
		return(-1);
	}
	if (iAmtCmp(wp_payin->F_txamt,wp_payin->T_txamt,
		sizeof(wp_payin->F_txamt))!=0)
	{
		vtcp_log("%s,%d,txamt[%.15s][%.15s]不一致",__FL__,
			wp_payin->F_txamt,wp_payin->T_txamt);
		sprintf(acErrMsg,"交易金额不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_payactno,wp_payin->T_payactno,
		sizeof(wp_payin->F_payactno))!=0)
	{
		vtcp_log("%s,%d,payactno:\n[%.32s]\n[%.32s]\n不一致",__FL__,
			wp_payin->F_payactno,wp_payin->T_payactno);
		sprintf(acErrMsg,"付款帐号不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_payname,wp_payin->T_payname,
		sizeof(wp_payin->F_payname))!=0)
	{
		vtcp_log("%s,%d,F_payname==[%.60s]\nT_payname=[%.60s]\n不一致",__FL__,
			wp_payin->F_payname,wp_payin->T_payname);
		sprintf(acErrMsg,"付款人不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_cashno,wp_payin->T_cashno,
		sizeof(wp_payin->F_cashno))!=0)
	{
		vtcp_log("%s,%d,cashno[%.12s][%.12s]不一致\n",__FL__,
			wp_payin->F_cashno,wp_payin->T_cashno);
		sprintf(acErrMsg,"收款人开户行不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_cashactno,wp_payin->T_cashactno,
		sizeof(wp_payin->F_cashactno))!=0)
	{
		vtcp_log("%s,%d,cashactno:\n[%.32s][%.32s]\n不一致",__FL__,
			wp_payin->F_cashactno,wp_payin->T_cashactno);
		sprintf(acErrMsg,"收款人帐号不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_ywtype,wp_payin->T_ywtype,
		sizeof(wp_payin->F_ywtype))!=0)
	{
		vtcp_log("%s,%d,ywtype[%.12s][%.12s]不一致",__FL__,
			wp_payin->F_ywtype,wp_payin->T_ywtype);
		sprintf(acErrMsg,"业务描述不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_wtday,wp_payin->T_wtday,
		sizeof(wp_payin->F_wtday))!=0)
	{
		vtcp_log("%s,%d,F_wtday====[%s]T_wtday===[%s]不一致",__FL__,
			wp_payin->F_wtday,wp_payin->T_wtday);
		sprintf(acErrMsg,"委托日期不一致");
		return(-1);
	}
	/****Modified by SSH,2006.1.10,小额不记优先级****/
	/****
	if(memcmp(wp_payin->F_operlevel,wp_payin->T_operlevel,
	sizeof(wp_payin->F_operlevel))!=0){
	vtcp_log("%s,%d,operlevel[%.1s][%.1s]不一致",__FL__,
	wp_payin->F_operlevel,wp_payin->T_operlevel);
	sprintf(acErrMsg,"优先级不一致");
	return(-1);
	}
	*****/
	/******************************************************/
	/********************可变部分**************************/
	/******************************************************/

	/********delete by xyy 2006-9-15 13:40 录入部分没有，但复核有的部分暂时注掉
	zip_space(wp_payin->F_voctype);
	if (memcmp(wp_payin->F_voctype,wp_payin->T_voctype,
	sizeof(wp_payin->F_voctype))!=0)
	{
	vtcp_log("%s,%d,voctype[%.2s][%.2s]不一致",__FL__,
	wp_payin->F_voctype,wp_payin->T_voctype);
	sprintf(acErrMsg,"凭证种类不一致");
	return(-1);
	}
	zip_space(wp_payin->F_vocnum);
	if (memcmp(wp_payin->F_vocnum,wp_payin->T_vocnum,
	sizeof(wp_payin->F_vocnum))!=0)
	{
	vtcp_log("%s,%d,vocnum[%.12s][%.12s]不一致",__FL__,
	wp_payin->F_vocnum,wp_payin->T_vocnum);
	sprintf(acErrMsg,"凭证号码不一致");
	return(-1);
	}
	if (memcmp(wp_payin->F_feeflag,wp_payin->T_feeflag,
	sizeof(wp_payin->F_feeflag))!=0)
	{
	vtcp_log("%s,%d,feeflag[%.1s][%.1s]不一致",__FL__,
	wp_payin->F_feeflag,wp_payin->T_feeflag);
	sprintf(acErrMsg,"手续费标志不一致");
	return(-1);
	}
	if (iAmtCmp(wp_payin->F_feeamt,wp_payin->T_feeamt,
	sizeof(wp_payin->F_feeamt))!=0)
	{
	vtcp_log("%s,%d,feeamt[%.15s][%.15s]不一致",__FL__,
	wp_payin->F_feeamt,wp_payin->T_feeamt);
	sprintf(acErrMsg,"手续费金额不一致");
	return(-1);
	}

	if (memcmp(wp_payin->F_ptype,wp_payin->T_ptype,
	sizeof(wp_payin->F_ptype))!=0)
	{
	vtcp_log("%s,%d,ptype[%.2s][%.2s]不一致\n",__FL__,
	wp_payin->F_ptype,wp_payin->T_ptype);
	sprintf(acErrMsg,"票据种类不一致");
	return(-1);
	}
	if (memcmp(wp_payin->F_pdate,wp_payin->T_pdate,
	sizeof(wp_payin->F_pdate))!=0)
	{
	vtcp_log("%s,%d,pdate[%.8s][%.8s]不一致",__FL__,
	wp_payin->F_pdate,wp_payin->T_pdate);
	sprintf(acErrMsg,"票据日期不一致");
	return(-1);
	}

	if (memcmp(wp_payin->F_pnum,wp_payin->T_pnum,
	sizeof(wp_payin->F_pnum))!=0)
	{
	vtcp_log("%s,%d,pnum[%.8s][%.8s]不一致",__FL__,
	wp_payin->F_pnum,wp_payin->T_pnum);
	sprintf(acErrMsg,"票据号码不一致");
	return(-1);
	}

	if (iAmtCmp(wp_payin->F_ptxamt,wp_payin->T_ptxamt,
	sizeof(wp_payin->F_ptxamt))!=0)
	{
	vtcp_log("%s,%d,ptxamt[%.15s][%.15s]不一致",__FL__,
	wp_payin->F_ptxamt,wp_payin->T_ptxamt);
	sprintf(acErrMsg,"赔偿金额不一致");
	return(-1);
	}
	if (iAmtCmp(wp_payin->F_jtxamt,wp_payin->T_jtxamt,
	sizeof(wp_payin->F_jtxamt))!=0)
	{
	vtcp_log("%s,%d,ptxamt[%.15s][%.15s]不一致",__FL__,
	wp_payin->F_ptxamt,wp_payin->T_ptxamt);
	sprintf(acErrMsg,"拒付金额不一致");
	return(-1);
	}
	if (memcmp(wp_payin->F_actype,wp_payin->T_actype,
	sizeof(wp_payin->F_actype))!=0)
	{
	vtcp_log("%s,%d,actype不一致[%c][%c]",__FL__,
	wp_payin->F_actype[0],wp_payin->T_actype[0]);
	sprintf(acErrMsg,"对方账户种类不一致");
	return(-1);
	}
	if (memcmp(wp_payin->F_pswdtype,wp_payin->T_pswdtype,
	sizeof(wp_payin->F_pswdtype))!=0)
	{
	vtcp_log("%s,%d,pswdtype[%.1s][%.1s]不一致",__FL__,
	wp_payin->F_pswdtype,wp_payin->T_pswdtype);
	sprintf(acErrMsg,"对方密码种类不一致");
	return(-1);
	}
	***********************************/
	if (iDateCmp(wp_payin->F_owtday,wp_payin->T_owtday,
		sizeof(wp_payin->F_owtday))!=0)
	{
		vtcp_log("%s,%d,owtday[%.8s][%.8s]不一致",__FL__,
			wp_payin->F_owtday,wp_payin->T_owtday);
		sprintf(acErrMsg,"原委托日期不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_otxnum,wp_payin->T_otxnum,
		sizeof(wp_payin->F_otxnum))!=0)
	{
		vtcp_log("%s,%d,otxnum[%.5s][%.5s]不一致",__FL__,
			wp_payin->F_otxnum,wp_payin->T_otxnum);
		sprintf(acErrMsg,"原业务编号不一致");
		return(-1);
	}
	if (memcmp(wp_payin->F_retcode,wp_payin->T_retcode,
		sizeof(wp_payin->F_retcode))!=0)
	{
		vtcp_log("%s,%d,retcode[%.2s][%.2s]不一致",__FL__,
			wp_payin->F_retcode,wp_payin->T_retcode);
		sprintf(acErrMsg,"退汇原因不一致");WRITEMSG
			return(-1);
	}
	if (memcmp(wp_payin->F_rcpstat,wp_payin->T_rcpstat,
		sizeof(wp_payin->F_rcpstat))!=0)
	{
		vtcp_log("%s,%d,rcpstat[%.2s][%.2s]不一致",__FL__,
			wp_payin->F_rcpstat,wp_payin->T_rcpstat);
		sprintf(acErrMsg,"回执代码不一致");WRITEMSG
			return(-1);
	}
	if (memcmp(wp_payin->T_brno,lvca.brno,BRNO_LEN)!=0)
	{
		vtcp_log("%s,%d,Brno[%.7s][%.7s]不一致",__FL__,
			lvca.brno,wp_payin->T_brno);
		sprintf(acErrMsg,"非本机构交易");WRITEMSG
			return(-1);
	}
	return(0);
}
/***************************************************************************/
/* 函数名称: int iLvPayinCheck7(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 记录存在性检查                                                */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:         																														 */
/***************************************************************************/
int iLvPayinCheck7(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	/** note rem by lhf for test 2006-9-10 15:34 
	iRc=iLvFetchRecToPayin(wp_payin);
	***/
	if (iRc!=0)
		return(iRc);
	iLvResetPayin(wp_payin);	
	return(0);
}
/***************************************************************************/
/* 函数名称: iLvPayinCheck8                                                */
/* 功能描述: 支付交易包状态检查以及设置                                    */
/* 参数:                                                                   */
/*            wp_payin   -支付交易通用输入结构体                           */
/*            wp_lv_wbctl-往包控制表结构                                   */
/*            wp_lv_lbctl-来包控制表结构                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:     来包交易应把wp_lv_wbctl置NULL,往包交易应把wp_lv_lbctl置NULL   */
/*           Add by SSH,2006.1.9,增加对回执交易拼包的特殊处理              */
/***************************************************************************/
int iLvPayinCheck8(PAY_IN_AREA *wp_payin, struct lv_wbctl_c *wp_lv_wbctl, struct lv_lbctl_c *wp_lv_lbctl)
{
	char cPackday[9];
	char cPackid[9];
	double dblTmp=0;
	int iRc=0;
	memset(cPackday,'\0',sizeof(cPackday));
	memset(cPackid,'\0',sizeof(cPackid));
	vtcp_log("[%s][%d]lvca.brno=[%s]",__FILE__,__LINE__,lvca.brno);
	vtcp_log("%s,%d wp_payin->A_cashqsactno==[%s]",__FL__,wp_payin->A_cashqsactno);
	vtcp_log("%s,%d wp_payin->A_payqsactno==[%s]",__FL__,wp_payin->A_payqsactno);

	/*** rem by LiuHuafeng 20061013
	if (wp_lv_wbctl!=NULL)
	memset(wp_lv_wbctl,'\0',sizeof(*wp_lv_wbctl));
	if (wp_lv_lbctl!=NULL)
	memset(wp_lv_lbctl,'\0',sizeof(*wp_lv_lbctl));
	***/
	if (wp_lv_wbctl!=NULL)
		memset(wp_lv_wbctl,'\0',sizeof(struct lv_wbctl_c));
	if (wp_lv_lbctl!=NULL)
		memset(wp_lv_lbctl,'\0',sizeof(struct lv_lbctl_c));

	if (iLvGetOrient(wp_payin)==TX_RECEIVE && wp_lv_lbctl==NULL)
	{
		vtcp_log("%s,%d,来包不能为空\n",__FL__);
		sprintf(acErrMsg,"来包不能为空");WRITEMSG
			return(-1);
	}
	else if (iLvGetOrient(wp_payin)==TX_SEND && wp_lv_wbctl==NULL)
	{
		vtcp_log("%s,%d,往包不能为空\n",__FL__);
		sprintf(acErrMsg,"往包不能为空");WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]=========wp_payin->F_packid=[%s]\n",__FL__,wp_payin->F_packid);
	memcpy(cPackday,wp_payin->F_packday,sizeof(cPackday)-1);
	memcpy(cPackid,wp_payin->F_packid,sizeof(cPackid)-1);
	vtcp_log("[%s][%d]=========wp_payin->F_pkgno=[%.3s]\n",__FL__,wp_payin->F_pkgno);
	vtcp_log("[%s][%d]=========cPackid=[%s]\n",__FL__,cPackid);
	vtcp_log("[%s][%d]========cPackday=[%s]\n",__FL__,cPackday);
	dblTmp=0;
	/****NBWYX BEGIN :支票回执业务拼包按普通贷记方式走****/
	/****NEWYX2:新系统判断普通借记回执还是票交回执根据TXNO即可****/
	if (
		(memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))==0  ) 
		&& wp_lv_wbctl!=NULL &&  memcmp(g_pub_tx.tx_code,"5926",4)!=0)
	{
		struct lv_lbctl_c wd_lv_lbctl;
		memset(&wd_lv_lbctl, 0 , sizeof(wd_lv_lbctl));
		vtcp_log("[%s][%d] \n",__FL__);
		/****回执往包的特殊处理****/
		memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
		wd_lv_lbctl.pack_date=apatoi(wp_payin->T_o_pack_date,8);
		memcpy(wd_lv_lbctl.packid,wp_payin->T_o_packid,
			sizeof(wd_lv_lbctl.packid)-1);

		memcpy(wd_lv_lbctl.pay_qs_no,wp_payin->A_cashqsactno,
			sizeof(wd_lv_lbctl.pay_qs_no)-1);

		iRc=Lv_lbctl_Dec_Upd(g_pub_tx.reply, "pack_date =%ld and packid = '%s' and pay_qs_no = '%s' ",
			wd_lv_lbctl.pack_date, wd_lv_lbctl.packid, wd_lv_lbctl.pay_qs_no );
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Lock lbctl error[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"更新游标来包记录出错 %d", iRc);
			WRITEMSG
				return(-1);
		}
		iRc = Lv_lbctl_Fet_Upd(&wd_lv_lbctl, g_pub_tx.reply);
		if (iRc !=DB_OK )
		{
			vtcp_log("%s,%d,Fet_Upd lbctl error[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"更新来包记录出错 %d", iRc);WRITEMSG
				return(-1);
		}
		vtcp_log("[%s],[%d],A_tbsdy============[%.8s]",__FL__,wp_payin->A_tbsdy);
		vtcp_log("[%s],[%d],wd_lv_lbctl->o_pack_date===[%d] o_packid=[%s]",__FL__,wd_lv_lbctl.o_pack_date,wd_lv_lbctl.o_packid);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		if (strlen(wd_lv_lbctl.o_packid)<sizeof(wd_lv_lbctl.o_packid)-1) 
		{
			struct lv_wbctl_c lv_wbctl;
			memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
			/****原来包记录还没有做过回执****/
			/**开始生成一条包记录**/
			vtcp_log("[%s],[%d],A_tbsdy=[%.8s]",__FL__,wp_payin->A_tbsdy);
			wp_lv_wbctl->pack_date = apatoi((char*)pcLvGetDay(),8);
			sprintf(wp_lv_wbctl->packid,"%08d",iLvGetPackid());
			memcpy(wp_lv_wbctl->pkgno,wp_payin->F_pkgno,sizeof(wp_lv_wbctl->pkgno)-1);
			memcpy(wp_lv_wbctl->pay_qs_no,wp_payin->A_payqsactno,
				sizeof(wp_lv_wbctl->pay_qs_no)-1);
			memcpy(wp_lv_wbctl->cash_qs_no,wp_payin->A_cashqsactno,
				sizeof(wp_lv_wbctl->cash_qs_no)-1);
			vtcp_log("%s,%d wp_payin->A_cashqsactno==[%s]",__FL__,wp_payin->A_cashqsactno);
			vtcp_log("%s,%d wp_payin->A_payqsactno==[%s]",__FL__,wp_payin->A_payqsactno);
			wp_lv_wbctl->in_date = apatoi(wp_payin->A_tbsdy, 8);
			vtcp_log("[%s][%d]lvca.brno=[%s]",__FILE__,__LINE__,lvca.brno);
			get_fd_data("0030", lvca.brno);
			vtcp_log("[%s][%d]lvca.brno=[%s]",__FILE__,__LINE__,lvca.brno);
			memcpy(wp_lv_wbctl->br_no,lvca.brno,sizeof(wp_lv_wbctl->br_no)-1);
			wp_lv_wbctl->hz_date = apatoi(wp_payin->A_tbsdy, 8);
			wp_lv_wbctl->o_pack_date = wd_lv_lbctl.pack_date;
			memcpy(wp_lv_wbctl->o_packid,wd_lv_lbctl.packid,sizeof(wp_lv_wbctl->o_packid));
			wd_lv_lbctl.o_pack_date = wp_lv_wbctl->pack_date;
			memcpy(wd_lv_lbctl.o_packid,wp_lv_wbctl->packid,sizeof(wd_lv_lbctl.o_packid));
			memcpy(wp_lv_wbctl->o_pkgno,wd_lv_lbctl.pkgno,sizeof(wp_lv_wbctl->o_pkgno));
			memcpy(wp_lv_wbctl->ornode,wd_lv_lbctl.ornode,sizeof(wp_lv_wbctl->ornode));
			memcpy(wp_lv_wbctl->acnode,wd_lv_lbctl.acnode,sizeof(wp_lv_wbctl->acnode));
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			if (memcmp(wp_payin->F_rcpstat,"00",2)==0)
			{
				str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0,&wp_lv_wbctl->dtlsucamt);
				wp_lv_wbctl->dtlsucamt/=100.0;
				wp_lv_wbctl->dtlsuccnt=1;
				vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,wp_lv_wbctl->dtlsucamt);
			}
			wp_lv_wbctl->dtlcnt=1;
			wp_lv_wbctl->totamt=wd_lv_lbctl.totamt;

			wp_lv_wbctl->bfflag[0]='0';
			vtcp_log("[%s][%d]==============packend[0]=[%c]\n",__FL__,wp_payin->F_packend[0]);
			if (wp_payin->F_packend[0]=='1')
				wp_lv_wbctl->stat[0]=PSTAT_PACKED;
			else
				wp_lv_wbctl->stat[0]=PSTAT_PACKING;
			/****content域在交易中赋值****/

			vtcp_log("[%s][%d]******************wbctl.pay_qs_no=[%s]\n", __FL__,wp_lv_wbctl->pay_qs_no);

			memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
			iRc=Lv_wbctl_Ins(lv_wbctl, g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Insert wbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"插入往包控制表失败");WRITEMSG
					return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("[%s][%d]wbctl.totamt=[%f]",__FILE__,__LINE__,lv_wbctl.totamt);
			vtcp_log("[%s][%d]wbctl.dtlcnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlcnt);
			vtcp_log("[%s][%d]wbctl.dtlsuccnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlsuccnt);
			vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,lv_wbctl.dtlsucamt);
			vtcp_log("[%s][%d]wbctl.packid=[%s]",__FILE__,__LINE__,lv_wbctl.packid);
			iRc=Lv_lbctl_Upd_Upd(wd_lv_lbctl, g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Update Lbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"更新来包控制表失败");WRITEMSG
					return(-1);
			}
			apitoa(wp_lv_wbctl->o_pack_date,sizeof(wp_payin->F_packday),
				wp_payin->F_packday);
			memcpy(wp_payin->F_packid,wp_lv_wbctl->packid,
				sizeof(wp_payin->F_packid));
			memcpy(wp_payin->F_packday,(char*)pcLvGetDay(),8);
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("%s,%d,生成新包\n",__FL__);
			return(PCHK_NEW);
		}
		else
		{
			struct lv_wbctl_c lv_wbctl;
			memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
			/****回执交易,已经有过拼包记录****/
			wp_lv_wbctl->pack_date=wd_lv_lbctl.o_pack_date;
			memcpy(wp_lv_wbctl->packid,wd_lv_lbctl.o_packid,
				sizeof(wp_lv_wbctl->packid)-1);

			iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date=%d and packid = '%s' ",
				wp_lv_wbctl->pack_date, wp_lv_wbctl->packid);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Lock wbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"在往包控制表中未找到相应记录");WRITEMSG
					return(-1);
			}
			iRc = Lv_wbctl_Fet_Upd(wp_lv_wbctl,g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Fet_Upd wbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"在往包控制表中未找到相应记录");WRITEMSG
					return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			wp_lv_wbctl->dtlcnt++;
			if (memcmp(wp_payin->F_rcpstat,"00",2)==0)
			{
				double dAmt=0;
				str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0,&dAmt);
				dAmt=dAmt/100.0;
				wp_lv_wbctl->dtlsucamt+=dAmt;
				vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,wp_lv_wbctl->dtlsucamt);
				wp_lv_wbctl->dtlsuccnt++;
			}
			if (wp_payin->F_packend[0]=='1')
				wp_lv_wbctl->stat[0]=PSTAT_PACKED;
			else
				wp_lv_wbctl->stat[0]=PSTAT_PACKING;
			/****content域在交易中赋值****/
			memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
			iRc=Lv_wbctl_Upd_Upd(lv_wbctl, g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Upd_Upd wbctl error 更新往包控制表失败[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"更新往包控制表失败");WRITEMSG
					return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("[%s][%d]wbctl.totamt=[%f]",__FILE__,__LINE__,lv_wbctl.totamt);
			vtcp_log("[%s][%d]wbctl.dtlcnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlcnt);
			vtcp_log("[%s][%d]wbctl.dtlsuccnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlsuccnt);
			vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,lv_wbctl.dtlsucamt);
			vtcp_log("[%s][%d]wbctl.packid=[%s]",__FILE__,__LINE__,lv_wbctl.packid);
		}
		return(0);/****回执拼包处理完毕,不必继续往下执行了****/
	}
	vtcp_log("[%s][%d] cPackid=====[%s]\n",__FL__,cPackid);
	zip_space(cPackid);
	vtcp_log("[%s][%d] cPackid=====[%s]\n",__FL__,cPackid);
	if (strlen(cPackid)==0)
	{
		struct lv_wbctl_c lv_wbctl;
		memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
		vtcp_log("[%s][%d] \n",__FL__);
		/**如果还未拼包，则现在生成一个包记录**/
		if (iLvGetOrient(wp_payin)==TX_RECEIVE)
		{
			/**来包不需要拼包，所以认为出错了**/
			vtcp_log("%s,%d,来账记录未找到包信息\n",__FL__);
			sprintf(acErrMsg,"来账记录未找到包信息");WRITEMSG
			return(-1);
		}
		/**开始生成一条包记录**/
		memcpy(wp_lv_wbctl->txnum,wp_payin->F_txnum,sizeof(wp_lv_wbctl->txnum)-1);/***YHBP***/
		vtcp_log("[%s],[%d],A_tbsdy=-------------------------[%s]",__FL__,wp_payin->A_tbsdy);
		wp_lv_wbctl->pack_date=apatoi(wp_payin->A_tbsdy,8);
		vtcp_log("[%s],[%d],wp_lv_wbctl->pack_date-----------[%d]",__FL__,wp_lv_wbctl->pack_date);
		sprintf(wp_lv_wbctl->packid,"%08d",iLvGetPackid());
		memcpy(wp_lv_wbctl->pkgno,wp_payin->F_pkgno,sizeof(wp_lv_wbctl->pkgno)-1);
		vtcp_log("[%s],[%d],wp_lv_wbctl->pkgno-----------[%s]",__FL__,wp_lv_wbctl->pkgno);
		memcpy(wp_lv_wbctl->pay_qs_no,wp_payin->A_payqsactno,	sizeof(wp_lv_wbctl->pay_qs_no)-1);
		memcpy(wp_lv_wbctl->cash_qs_no,wp_payin->A_cashqsactno,	sizeof(wp_lv_wbctl->cash_qs_no)-1);
		wp_lv_wbctl->in_date=apatoi(wp_payin->A_tbsdy,sizeof(wp_payin->A_tbsdy));
		get_fd_data("0030", lvca.brno);
		vtcp_log("[%s][%d]lvca.brno=[%s]",__FILE__,__LINE__,lvca.brno);
		memcpy(wp_lv_wbctl->br_no,lvca.brno,BRNO_LEN);/***YHBP***/
		vtcp_log("[%s][%d]gu zong de ------lv_wbctl->brno=[%s]",__FILE__,__LINE__,wp_lv_wbctl->br_no);
		wp_lv_wbctl->dtlcnt=1;
		vtcp_log("[%s][%d]==============wp_payin->F_txamt=[%s]\n",__FL__,wp_payin->F_txamt);
		str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0,&wp_lv_wbctl->totamt);
		vtcp_log("[%s][%d]==============wp_lv_wbctl->totamt=[%f]\n",__FL__,wp_lv_wbctl->totamt);
		wp_lv_wbctl->totamt=wp_lv_wbctl->totamt/100.0;
		vtcp_log("[%s][%d]==============wp_lv_wbctl->totamt=[%f]\n",__FL__,wp_lv_wbctl->totamt);
		/****应答日期、原来包日期、原来包id、明细业务成功必数、明细业务成功金额
		*****原包类型号、发起节点、接收节点等在交易中设置*********************/
		wp_lv_wbctl->bfflag[0]='0';
		vtcp_log("[%s][%d]==============packend[0]=[%c]\n",__FL__,wp_payin->F_packend[0]);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		if (wp_payin->F_packend[0]=='1')
			wp_lv_wbctl->stat[0]=PSTAT_PACKED;
		else
			wp_lv_wbctl->stat[0]=PSTAT_PACKING;
		memcpy(wp_lv_wbctl->brprtflag,"000",sizeof(wp_lv_wbctl->brprtflag)-1);
		memcpy(wp_lv_wbctl->qsprtflag,"000",sizeof(wp_lv_wbctl->qsprtflag)-1);
		memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
		/****content域在交易中赋值****/
		/****content域在交易中赋值****/
		/****HSYX BEGIN: 支票回执业务拼包要素****/
		/****回执业务能走到这里的只有支票截留类业务,所以不再判断业务类型****/

		vtcp_log("[%s][%d]注意啊，这里可能要有问题了payin.F_pkgno=[%.3s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0||
			memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))==0)
		{
			vtcp_log("[%s][%d]===========1111111\n", __FL__);
			if (memcmp(wp_payin->F_rcpstat,"00",2)==0)
			{
				wp_lv_wbctl->dtlsuccnt=1;
				wp_lv_wbctl->dtlsucamt=wp_lv_wbctl->totamt;
			}else
			{
				wp_lv_wbctl->dtlsuccnt=0;
				wp_lv_wbctl->dtlsucamt=0;
			}
			if(getenv("BPTEST")!=NULL && strcmp(getenv("BPTEST"),"BP_TEST")==0)
			{
				  vtcp_log("[%s][%d]YHBP测试！！！",__FILE__,__LINE__);
				  memcpy(wp_lv_wbctl->o_pkgno,"002",3);
			}
			else
			{
			    memcpy(wp_lv_wbctl->o_pkgno,"999",3);
			    wp_lv_wbctl->o_pack_date=apatoi(pcLvGetDay(),8);
			    memcpy(wp_lv_wbctl->o_packid,"99999999",8);
			}
			vtcp_log("%s,%d 注意这里opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
			/** 现在应该是一致的吧 dongxy
			memcpy(wp_lv_wbctl->cash_qs_no,wp_payin->A_payqsactno, sizeof(wp_lv_wbctl->cash_qs_no)-1);
			memcpy(wp_lv_wbctl->pay_qs_no,wp_payin->A_cashqsactno, sizeof(wp_lv_wbctl->pay_qs_no)-1);
			*/
			memcpy(wp_lv_wbctl->cash_qs_no,wp_payin->A_cashqsactno, sizeof(wp_lv_wbctl->cash_qs_no)-1);
			memcpy(wp_lv_wbctl->pay_qs_no,wp_payin->A_payqsactno, sizeof(wp_lv_wbctl->pay_qs_no)-1);
		}
		/****HSYX END: 支票回执业务拼包要素****/
		vtcp_log("[%s][%d]******************wbctl.pay_qs_no=[%s]\n", __FL__,wp_lv_wbctl->pay_qs_no);
		vtcp_log("%s,%d before insert wbctl pkgno==[%s]",__FILE__,__LINE__,lv_wbctl.pkgno);
		vtcp_log("%s,%d before insert wbctl packid==[%s]",__FILE__,__LINE__,lv_wbctl.packid);
		vtcp_log("%s,%d before insert wbctl packdate==[%d]",__FILE__,__LINE__,lv_wbctl.pack_date);
		memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
		iRc=Lv_wbctl_Ins(lv_wbctl, g_pub_tx.reply);
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Insert wbctl error  [%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"插入往包控制表失败");
			return(-1);
		}
		vtcp_log("%s,%d 插入往包 ",__FILE__,__LINE__);
		vtcp_log("%s,%d 注意这里opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		Lv_wbctl_Debug(&lv_wbctl);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		apitoa(wp_lv_wbctl->pack_date,sizeof(wp_payin->F_packday),wp_payin->F_packday);
		vtcp_log("%s,%d 注意这里opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		memcpy(wp_payin->F_packid,wp_lv_wbctl->packid,sizeof(wp_payin->F_packid));
		vtcp_log("%s,%d 注意这里opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		apitoa(wp_lv_wbctl->pack_date,sizeof(wp_payin->T_packday),wp_payin->T_packday);
		vtcp_log("%s,%d 注意这里opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		memcpy(wp_payin->T_packid,wp_lv_wbctl->packid,sizeof(wp_payin->T_packid));
		vtcp_log("%s,%d 注意这里opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		vtcp_log("%s,%d,wp_payin->T_packday =[%.8s]",__FL__,wp_payin->T_packday);
		vtcp_log("%s,%d,wp_payin->T_packid =[%.8s]",__FL__,wp_payin->T_packid);
		vtcp_log("%s,%d,wp_payin->F_packday =[%.8s]",__FL__,wp_payin->F_packday);
		vtcp_log("%s,%d,wp_lv_wbctl->pkgno-------------------------- =[%s]",__FL__,wp_lv_wbctl->pkgno);
		vtcp_log("%s,%d,wp_lv_wbctl->packid-------------------------- =[%s]",__FL__,wp_lv_wbctl->packid);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		vtcp_log("%s,%d,生成新包\n",__FL__);
		vtcp_log("%s,%d lv_wbctl.pkgno xyy ===[%s] ",__FL__,lv_wbctl.pkgno);
		vtcp_log("%s,%d lv_wbctl.packid xyy ===[%s] ",__FL__,lv_wbctl.packid);
		vtcp_log("%s,%d lv_wbctl.packday xyy ===[%ld] ",__FL__,lv_wbctl.pack_date);
		vtcp_log("%s,%d lv_wbctl.o_packid ===[%.8s] ",__FL__,lv_wbctl.o_packid);	
		vtcp_log("%s,%d lv_wbctl.o_pack_date ===[%d] ",__FL__,lv_wbctl.o_pack_date);					
		vtcp_log("%s,%d lv_wbctl.cash_qs_no ===[%.12s] ",__FL__,lv_wbctl.cash_qs_no);							
		return(PCHK_NEW);
	}
	else
	{
		vtcp_log("[%s][%d]wp_payin->F_pkgno=[%.3s] ",__FL__,wp_payin->F_pkgno);
		/**如果已经拼包,检查包记录是否存在*/
		if (iLvGetOrient(wp_payin)==TX_RECEIVE)
		{
			vtcp_log("[%s][%d] \n",__FL__);
			/***来包****/
			wp_lv_lbctl->pack_date=apatoi(wp_payin->F_packday,8);
			memcpy(wp_lv_lbctl->packid,wp_payin->F_packid,
				sizeof(wp_lv_lbctl->packid)-1);
			memcpy(wp_lv_lbctl->pay_qs_no,wp_payin->A_payqsactno,
				sizeof(wp_lv_lbctl->pay_qs_no)-1);
			iRc=Lv_lbctl_Sel(g_pub_tx.reply, wp_lv_lbctl, "pack_date = '%ld' and packid = '%s' and pay_qs_no = '%s' ",
				wp_lv_lbctl->pack_date, wp_lv_lbctl->packid, wp_lv_lbctl->pay_qs_no);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Lock lbctl error [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"在来包控制表中未找到相应记录");
				return(-1);
			}
			/***在来包中找到记录，比较pkgno、txnum
			****发起清算行、接收清算行是否一致****/
			if (memcmp(wp_payin->F_pkgno,wp_lv_lbctl->pkgno,
				sizeof(wp_payin->F_pkgno))!=0)
			{
				vtcp_log("%s,%d,pkgno不符 包中包编号与输入不符 [%d]\n",__FL__,iRc);
				return(-1);
			}
			if (memcmp(wp_payin->A_payqsactno,wp_lv_lbctl->pay_qs_no,
				sizeof(wp_payin->A_payqsactno))!=0)
			{
				vtcp_log("%s,%d,payqsactno不符[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"包中发起清算行行号与输入不符");
				return(-1);
			}
			if (memcmp(wp_payin->A_cashqsactno,wp_lv_lbctl->cash_qs_no,
				sizeof(wp_payin->A_cashqsactno))!=0)
			{
				vtcp_log("%s,%d,cashqsactno不符[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"包中接收清算行行号与输入不符");
				return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("%s,%d,包已存在\n",__FL__);
			return(PCHK_EXISTS);
		}
		else
		{
			struct lv_wbctl_c lv_wbctl;
			memset(&lv_wbctl,0,sizeof(lv_wbctl));
			vtcp_log("[%s][%d]wp_payin->F_pkgno=[%.3s] ",__FL__,wp_payin->F_pkgno);
			/***往包***/
			wp_lv_wbctl->pack_date=apatoi(wp_payin->F_packday,8);
			memcpy(wp_lv_wbctl->packid,wp_payin->F_packid,sizeof(wp_lv_wbctl->packid)-1);
			iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date =%ld and packid = '%s' ", wp_lv_wbctl->pack_date, wp_lv_wbctl->packid);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Lock wbctl error[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"在往包控制表中未找到相应记录");
				return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			iRc = Lv_wbctl_Fet_Upd(wp_lv_wbctl,g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Fet_Upd wbctl error[%d]\n",__FL__,iRc);
				return(-1);
			}
			Lv_wbctl_Debug(wp_lv_wbctl);
			vtcp_log("[%s][%d]lv_wbctl->pkgno=[%s]",__FILE__,__LINE__,wp_lv_wbctl->pkgno);
			/***在来包中找到记录，比较pkgno、txnum
			****发起清算行、接收清算行是否一致****/
			if (memcmp(wp_payin->F_pkgno,wp_lv_wbctl->pkgno,
				sizeof(wp_payin->F_pkgno))!=0)
			{
				vtcp_log("%s,%d,pkgno不符[%d]\n",__FL__,iRc);
				vtcp_log("%s,%d,payin->F_pkgno=[%.3s]lv_wbctl->pkgno=[%s]",__FL__,wp_payin->F_pkgno,wp_lv_wbctl->pkgno);
				sprintf(acErrMsg,"包中包编号与输入不符");
				return(-1);
			}
			/****HSYX BEGIN:支票回执业务的发起和接收清算行是反过来的****/
			if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0)
			{
				/*** 好像不对dongxy
				if(memcmp(wp_payin->A_cashqsactno,wp_lv_wbctl->pay_qs_no,
					sizeof(wp_payin->A_cashqsactno))!=0)
				***/
				if (memcmp(wp_payin->A_payqsactno,wp_lv_wbctl->pay_qs_no,
					sizeof(wp_payin->A_payqsactno))!=0)
				{
					vtcp_log("%s,%d,payqsactno不符\n",__FILE__,__LINE__);
					vtcp_log("%s,%d,wbctl->pay_qs_n=[%s],A_payqsactno[%s]\n",__FILE__,__LINE__,
						wp_lv_wbctl->pay_qs_no,wp_payin->A_payqsactno);
					sprintf(acErrMsg,"包中发起清算行行号与输入不符");
					return(-1);
				}
				/** 好像不对 dongxy
				if(memcmp(wp_payin->A_payqsactno,wp_lv_wbctl->cash_qs_no,
					sizeof(wp_payin->A_payqsactno))!=0)
				**/
				if (memcmp(wp_payin->A_cashqsactno,wp_lv_wbctl->cash_qs_no,
					sizeof(wp_payin->A_cashqsactno))!=0)
				{
					vtcp_log("%s,%d,cashqsactno不符[%d]\n",__FILE__,__LINE__,iRc);
					vtcp_log("%s,%d,wbctl->cash_qs_n=[%s],A_cashqsactno[%s]\n",__FILE__,__LINE__,
						wp_lv_wbctl->cash_qs_no,wp_payin->A_cashqsactno);
					sprintf(acErrMsg,"包中接收清算行行号与输入不符");
					return(-1);
				}
			}else
			{
				if (memcmp(wp_payin->A_payqsactno,wp_lv_wbctl->pay_qs_no,
					sizeof(wp_payin->A_payqsactno))!=0)
				{
					vtcp_log("%s,%d,payqsactno不符[%d]\n",__FL__,iRc);
					sprintf(acErrMsg,"包中发起清算行行号与输入不符");
					return(-1);
				}
				if (memcmp(wp_payin->A_cashqsactno,wp_lv_wbctl->cash_qs_no,
					sizeof(wp_payin->A_cashqsactno))!=0)
				{
					vtcp_log("%s,%d,A_cashqsactno=[%s]cash_qs_no=[%s]不符[%d]",__FL__,wp_payin->A_cashqsactno,wp_lv_lbctl->cash_qs_no,iRc);
					sprintf(acErrMsg,"包中接收清算行行号与输入不符");
					return(-1);
				}
			}
			/****HSYX END:支票回执业务的发起和接收清算行是反过来的****/
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("%s,%d wbctl.stat====[%c]",__FL__,wp_lv_wbctl->stat[0]);
			if (wp_lv_wbctl->stat[0]==PSTAT_PACKING||
				wp_lv_wbctl->stat[0]==PSTAT_PACKED)
			{
				vtcp_log("%s,%d wbctl.stat====[%c]",__FL__,wp_lv_wbctl->stat[0]);
				/***如果正在拼包****/
				if (wp_lv_wbctl->pack_date!=apatoi(wp_payin->A_tbsdy,8))
				{
					/***正在拼包但包日期不是今天，错误***/
					vtcp_log("%s,%d,企图拼不是今天的包\n",__FL__);
					sprintf(acErrMsg,"不能拼不是今天的包");
					return(-1);
				}
				/***add by xyy YHBP ****/
				vtcp_log("[%s][%d]注意这里的TXCODE==[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				if( !strncmp("5321",g_pub_tx.tx_code,4))
				{
					vtcp_log("[%s][%d]兑付他行银行本票修改[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				}
				else if (wp_lv_wbctl->stat[0]!=PSTAT_PACKING)/**modify by xyy YHBP**/
				{
					vtcp_log("%s,%d,包存在且不正在拼包!\n",__FL__);
					return(PCHK_EXISTS);
				}
				/****NEWYX BEGIN:处理支票回执拼包要素****/
				if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0)
				{
					double dAmt=0;
					vtcp_log("%s,%d F_txamt==[%.16s]",__FL__,wp_payin->F_txamt);
					str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0,&dAmt);
					wp_lv_wbctl->dtlcnt++;
					vtcp_log("[%s][%d]================dblTmp=[%f]\n",__FILE__,__LINE__,dblTmp);
					/*begin NEWYX rem by LiuHuafeng 2007-4-2 16:45
					dblTmp=dblTmp/100.0;
					*********REPLACE BY NEXT LINE ****************************/
					dblTmp=dAmt/100.0;
					/* end NEWYX BY LiuHuafeng 2007-4-2 16:46 */
					wp_lv_wbctl->totamt+=dblTmp;
					vtcp_log("[%s][%d]================wp_lv_wbctl->totamt=[%f]\n",__FILE__,__LINE__,wp_lv_wbctl->totamt);
					if (memcmp(wp_payin->F_rcpstat,"00",2)==0)
					{
						wp_lv_wbctl->dtlsucamt+=dblTmp;
						wp_lv_wbctl->dtlsuccnt++;
					}
				}else
				{
					/***add by xyy YHBP ****/
				  vtcp_log("[%s][%d]注意这里的TXCODE==[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				  if( !strncmp("5321",g_pub_tx.tx_code,4))
				  {
				  	  vtcp_log("[%s][%d]兑付他行银行本票修改[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				  	  wp_lv_wbctl->dtlcnt=1;
					    str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0, &dblTmp);
					    vtcp_log("[%s][%d]================dblTmp=[%f]\n",__FILE__,__LINE__,dblTmp);
					    dblTmp=dblTmp/100.0;
					    wp_lv_wbctl->totamt=dblTmp;
				  }
				  else/**modify by YHBP**/
				  {
					    wp_lv_wbctl->dtlcnt+=1;
					    str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0, &dblTmp);
					    vtcp_log("[%s][%d]================dblTmp=[%f]\n",__FILE__,__LINE__,dblTmp);
					    dblTmp=dblTmp/100.0;
					    wp_lv_wbctl->totamt+=dblTmp;
					    vtcp_log("[%s][%d]================wp_lv_wbctl->totamt=[%f]\n",__FILE__,__LINE__,wp_lv_wbctl->totamt);
					}
				}
				/****NEWYX END:处理支票回执拼包要素****/
				vtcp_log("[%s][%d]*****************wbctl.o_pack_date=[%d],o_packid=[%s]\n",
					__FL__,wp_lv_wbctl->o_pack_date,wp_lv_wbctl->o_packid);

				if (wp_payin->F_packend[0]=='1')
					wp_lv_wbctl->stat[0]=PSTAT_PACKED;
				else
					wp_lv_wbctl->stat[0]=PSTAT_PACKING;
				vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
				vtcp_log("%s,%d lv_wbctl.o_packid ===[%.8s] ",__FL__,lv_wbctl.o_packid);	
				vtcp_log("%s,%d lv_wbctl.o_pack_date ===[%d] ",__FL__,lv_wbctl.o_pack_date);					
				vtcp_log("%s,%d lv_wbctl.cash_qs_no ===[%.12s] ",__FL__,lv_wbctl.cash_qs_no);							

				memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
				vtcp_log("%s,%d before update wbctl cnt=[%d],amt=[%f] ",__FL__,lv_wbctl.dtlcnt,lv_wbctl.totamt);							
				iRc=Lv_wbctl_Upd_Upd(lv_wbctl, g_pub_tx.reply);
				if (iRc!=DB_OK)
				{
					vtcp_log("%s,%d,Upd wbctl error[%d]\n",__FL__,iRc);
					sprintf(acErrMsg,"更新往包控制表失败");
					return(-1);
				}
				vtcp_log("%s,%d,包已更新packid=%.8s,dtlcnt=%.0f,totamt=[%.0f]\n",
					__FL__,wp_lv_wbctl->packid,wp_lv_wbctl->dtlcnt,
					wp_lv_wbctl->totamt);
				return(PCHK_UPDATE);
			}
			else
			{
				vtcp_log("[%s][%d] \n",__FL__);
				/***包存在而且不是正在拼包***/
				vtcp_log("%s,%d,包已存在\n",__FL__);
				return(PCHK_EXISTS);
			}
		}	
	}
}
/***************************************************************************/
/* 函数名称: int iLvPayinCheck9(PAY_IN_AREA *wp_payin)                     */
/* 功能描述: 多方协议号检查                                                */
/* 创建日期: 2005.12.2                                                     */
/* 作者:     SSH                                                           */
/* 说明:         																														 */
/***************************************************************************/
int iLvPayinCheck9(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char pcDfxyNo[61];
	double dTxamt=0;
	struct lv_newxyh_c wd_lv_newxyh;
	memset(&wd_lv_newxyh,'\0',sizeof(wd_lv_newxyh));

	/****NEWYX BEGIN:测试多方协议号不检查****/
	if (getenv("FHTEST")!=NULL && memcmp((char*)getenv("FHTEST"),"LVTEST",6)==0)
	{
		return(0);
	}	
	/****NEWYX END:测试多方协议号不检查****/

	if (memcmp((char*)getenv("FHTEST"),"LVTEST",6)==0)
	{
		/****测试不检查多方协议号**/
		return(0);
	}
	memset(pcDfxyNo,'\0',sizeof(pcDfxyNo));
	/* rem by LiuHuafeng 20070116
	iRc=iLvGetAddit(wp_payin,"XYH",pcDfxyNo,sizeof(pcDfxyNo)-1);
	if (iRc)
	{
	vtcp_log("%s,%d,得到多方协议号错误\n",__FL__);
	sprintf(acErrMsg,"得到多方协议号错误");
	return(-1);
	}
	******************************/
	memcpy(pcDfxyNo,wp_payin->F_dfxyno,sizeof(pcDfxyNo)-1);
	zip_space(pcDfxyNo);
	if (strlen(pcDfxyNo)==0)
	{
		vtcp_log("%s,%d,多方协议号不存在\n",__FL__);
		sprintf(acErrMsg,"多方协议号不存在!");
		return(-1);
	}
	/*add by chenggx 20131029 多方协议号60个0不检查 beg*/
	if (memcmp(pcDfxyNo,"000000000000000000000000000000000000000000000000000000000000",60)==0)
	{
		vtcp_log("%s,%d,多方协议号[%s],不检查\n",__FL__,pcDfxyNo);
		return(0);
	}
	/*add by chenggx 20131029 多方协议号60个0不检查 end*/
	memcpy(wd_lv_newxyh.xyno,pcDfxyNo,sizeof(wd_lv_newxyh.xyno)-1);
	memcpy(wd_lv_newxyh.pay_ac_no,wp_payin->F_payactno,sizeof(wd_lv_newxyh.pay_ac_no)-1);
	iRc=Lv_newxyh_Sel(g_pub_tx.reply,&wd_lv_newxyh, "xyno = '%s' and pay_ac_no = '%s'", wd_lv_newxyh.xyno, wd_lv_newxyh.pay_ac_no);
	if (iRc)
	{
		vtcp_log("%s,%d,多方协议号不正确\n",__FL__);
		sprintf(acErrMsg,"多方协议号不正确");
		return(-1);
	}
	str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0,&dTxamt);
	vtcp_log("%s,%d,请求付款金额为:%.0f\n",__FL__,dTxamt);
	if (wd_lv_newxyh.limitamt-dTxamt<-0.01)
	{
		vtcp_log("%s,%d,超出了允许付款限额[%.0f][%.0f]\n",__FL__,wd_lv_newxyh.limitamt,dTxamt);
		sprintf(acErrMsg,"超出了允许付款限额");
		return(-1);
	}
	return(0);
}
/********************************************************************************/
/* 函数名称: int iLvCreateHz(PAY_IN_AREA *wp_payin,PAY_IN_AREA *wp_payin_hz)    */
/* 功能描述: 根据payin内容产生一条新的回执记录                                  */
/* 创建日期: 2005.12.2                                                       			*/
/* 作者:     SSH                                                               		*/
/* 说明:     只填充输入域，表数据可以调用newrec函数来生成       						 			*/
/********************************************************************************/
int iLvCreateHz(PAY_IN_AREA *wp_payin,PAY_IN_AREA *wp_payin_hz)
{
	int iOrderno=0;
	vtcp_log("[%s][%d]payin->F_wtday==[%.8s]\n",__FILE__,__LINE__,wp_payin->F_wtday);
	memset(wp_payin_hz,'\0',sizeof(wp_payin_hz));						
	memcpy(wp_payin_hz->F_crdb,wp_payin->F_crdb,   
		sizeof(wp_payin_hz->F_crdb));
	memcpy(wp_payin_hz->F_opcd,OPCD_LR,
		sizeof(wp_payin_hz->F_opcd));
	vtcp_log("[%s][%d]===========wp_payin_hz->F_pkgno=[%s]\n",__FL__,wp_payin_hz->F_pkgno);
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJ,sizeof(wp_payin->F_pkgno))==0)
	{
		memcpy(wp_payin_hz->F_pkgno,PKGNO_PTJJHZ,
			sizeof(wp_payin_hz->F_pkgno));
	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJ,
		sizeof(wp_payin->F_pkgno))==0)
	{
		memcpy(wp_payin_hz->F_pkgno,PKGNO_SSJJHZ,
			sizeof(wp_payin_hz->F_pkgno));
	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ,
		sizeof(wp_payin->F_pkgno))==0)
	{
		memcpy(wp_payin_hz->F_pkgno,PKGNO_DQJJHZ,
			sizeof(wp_payin_hz->F_pkgno));
	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJ,
		sizeof(wp_payin->F_pkgno))==0)
	{
		memcpy(wp_payin_hz->F_pkgno,PKGNO_SSDJHZ,
			sizeof(wp_payin_hz->F_pkgno));
	}
	else
	{
		vtcp_log("%s,%d,本报文[%.3s]没有业务回执\n",
			__FL__,wp_payin->F_pkgno);
		sprintf(acErrMsg,"本报文没有业务回执!");
		return(-1);
	}
	/*Modified by xyy 2006-9-19 11:16把原记录的交易序号，放到回执记录的oorderno中**/
	memcpy(wp_payin_hz->F_oorderno,wp_payin->F_orderno,
		sizeof(wp_payin_hz->F_oorderno));
	vtcp_log("%s,%d,使用原支付交易序号:[%.8s][%.8s]\n",__FL__,
		wp_payin->F_orderno,wp_payin_hz->F_oorderno);
	iOrderno=iLvGetOrderno();
	if (iOrderno<=0)
	{
		vtcp_log("%s,%d,得到支付交易序号错误!\n",__FL__);
		return(-1);
	}
	apitoa(iOrderno,sizeof(wp_payin_hz->F_orderno),wp_payin_hz->F_orderno);
	/************************************************************************/
	memcpy(wp_payin_hz->F_txnum,wp_payin->F_txnum,
		sizeof(wp_payin_hz->F_txnum));
	/**这里应该重置一下，把原发起行行号放到回执表的接收行行号中，
	把原接收行行号，放到回执交易的发起行行行号中**/	
	memcpy(wp_payin_hz->F_acbrno,wp_payin->F_orbrno,
		sizeof(wp_payin_hz->F_acbrno));
	vtcp_log("[%s][%d]wp_payin_hz->F_acbrno=[%s]",__FILE__,__LINE__,wp_payin_hz->F_acbrno);	
	/***
	memcpy(wp_payin_hz->F_orbrno,wp_payin->F_acbrno,
	sizeof(wp_payin_hz->F_orbrno));liuyue 20091006
	**/
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,wp_payin_hz->F_orbrno);

	vtcp_log("[%s][%d]wp_payin_hz->F_orbrno=[%s]",__FILE__,__LINE__,wp_payin_hz->F_orbrno);	
	memcpy(wp_payin_hz->F_txamt,wp_payin->F_txamt,
		sizeof(wp_payin_hz->F_txamt));
	memcpy(wp_payin_hz->F_payno,wp_payin->F_payno,
		sizeof(wp_payin_hz->F_payno));
	memcpy(wp_payin_hz->F_payactno,wp_payin->F_payactno,
		sizeof(wp_payin_hz->F_payactno));
	memcpy(wp_payin_hz->F_payname,wp_payin->F_payname,
		sizeof(wp_payin_hz->F_payname));
	memcpy(wp_payin_hz->F_payaddress,wp_payin->F_payaddress,
		sizeof(wp_payin_hz->F_payaddress));
	memset(wp_payin_hz->F_userpswd,'\0',sizeof(wp_payin_hz->F_userpswd));
	memcpy(wp_payin_hz->F_cashno,wp_payin->F_cashno,
		sizeof(wp_payin_hz->F_cashno));
	memcpy(wp_payin_hz->F_cashactno,wp_payin->F_cashactno,
		sizeof(wp_payin_hz->F_cashactno));
	memcpy(wp_payin_hz->F_cashname,wp_payin->F_cashname,
		sizeof(wp_payin_hz->F_cashname));
	memcpy(wp_payin_hz->F_cashaddress,wp_payin->F_cashaddress,
		sizeof(wp_payin_hz->F_cashaddress));
	memcpy(wp_payin_hz->F_ywtype,wp_payin->F_ywtype,
		sizeof(wp_payin_hz->F_ywtype));
	memset(wp_payin_hz->F_content,'\0',sizeof(wp_payin_hz->F_content));

	memcpy(wp_payin_hz->F_content,wp_payin->F_content,sizeof(wp_payin_hz->F_content));
	memcpy(wp_payin_hz->F_owtday,wp_payin->F_wtday,sizeof(wp_payin_hz->F_owtday));
	vtcp_log("[%s][%d]wp_payin_hz->F_owtday==[%.12s]\n",__FILE__,__LINE__,wp_payin_hz->F_owtday);
	vtcp_log("[%s][%d]payin->F_wtday==[%.12s]\n",__FILE__,__LINE__,wp_payin->F_wtday);
	memcpy(wp_payin_hz->F_wtday,pcLvGetDay(),sizeof(wp_payin_hz->F_wtday));
	memcpy(wp_payin_hz->F_operlevel,wp_payin->F_operlevel,
		sizeof(wp_payin_hz->F_operlevel));
	memset(wp_payin_hz->F_authno,'\0',sizeof(wp_payin_hz->F_authno));
	memset(wp_payin_hz->F_authpswd,'\0',sizeof(wp_payin_hz->F_authpswd));
	memset(wp_payin_hz->F_voctype,'\0',sizeof(wp_payin_hz->F_voctype));
	memset(wp_payin_hz->F_vocnum,'\0',sizeof(wp_payin_hz->F_vocnum));

	/****Modified by xyy 没有生成新包,应该这里把packid和packday置空，但是我们要把原交易的packday
	和packday放到lv_pkgreg表中的opackid和opackday中,所以暂时放在这两个字段里****/
	/** rem by LiuHuafeng 2006-10-7 22:26
	memcpy(wp_payin_hz->F_packday,wp_payin->F_packday,sizeof(wp_payin_hz->F_packday));
	memcpy(wp_payin_hz->F_packid,wp_payin->F_packid,sizeof(wp_payin_hz->F_packid));
	**************************replace by next four line*************************************/
	memcpy(wp_payin_hz->T_o_pack_date,wp_payin->F_packday,sizeof(wp_payin_hz->T_o_pack_date));
	memcpy(wp_payin_hz->T_o_packid,wp_payin->F_packid,sizeof(wp_payin_hz->T_o_packid));
	vtcp_log("[%s][%d]wp_payin_hz->T_o_pack_date==[%s]",__FILE__,__LINE__,wp_payin_hz->T_o_pack_date);
	vtcp_log("[%s][%d]wp_payin_hz->T_o_packid==[%s]",__FILE__,__LINE__,wp_payin_hz->T_o_packid);
	/* add by LiuHuafeng 20060306 */
	if (memcmp(wp_payin_hz->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin_hz->F_pkgno))==0)
		memcpy(wp_payin_hz->F_feeflag,wp_payin->F_feeflag,sizeof(wp_payin_hz->F_feeflag));
	else
	/* end by LiuHuafeng 20060306 */
		wp_payin_hz->F_feeflag[0]=FEEFLAG_NO;
	vtcp_log("[%s][%d] hz_feeflag==[%c]\n",__FL__,wp_payin_hz->F_feeflag[0]);

	memset(wp_payin_hz->F_feeamt,'\0',sizeof(wp_payin_hz->F_feeamt));
	memcpy(wp_payin_hz->F_respday,wp_payin->A_tbsdy,
		sizeof(wp_payin_hz->F_respday));
	memcpy(wp_payin_hz->A_oldrespday,wp_payin->F_respday,
		sizeof(wp_payin_hz->A_oldrespday));
	memcpy(wp_payin_hz->F_ptype,wp_payin->F_ptype,
		sizeof(wp_payin_hz->F_ptype));
	memcpy(wp_payin_hz->F_pdate,wp_payin->F_pdate,
		sizeof(wp_payin_hz->F_pdate));
	memcpy(wp_payin_hz->F_pnum,wp_payin->F_pnum,
		sizeof(wp_payin_hz->F_pnum));
	memcpy(wp_payin_hz->F_ptxamt,wp_payin->F_ptxamt,
		sizeof(wp_payin_hz->F_ptxamt));
	memcpy(wp_payin_hz->F_jtxamt,wp_payin->F_jtxamt,
		sizeof(wp_payin_hz->F_jtxamt));
	memcpy(wp_payin_hz->T_addid,wp_payin->T_addid,
		sizeof(wp_payin_hz->T_addid));
	memcpy(wp_payin_hz->F_owtday,wp_payin->F_wtday,sizeof(wp_payin_hz->F_owtday));
	memset(wp_payin_hz->F_otxnum,'\0',sizeof(wp_payin_hz->F_otxnum));
	memset(wp_payin_hz->F_retcode,'\0',sizeof(wp_payin_hz->F_retcode));
	memset(wp_payin_hz->F_rcpstat,'\0',sizeof(wp_payin_hz->F_rcpstat));
	memset(wp_payin_hz->F_resp1,'\0',sizeof(wp_payin_hz->F_resp1));
	memset(wp_payin_hz->F_resp2,'\0',sizeof(wp_payin_hz->F_resp2));
	/*************************/
	memcpy(wp_payin_hz->A_tbsdy,wp_payin->A_tbsdy,
		sizeof(wp_payin_hz->A_tbsdy));	
	memcpy(wp_payin_hz->A_brno,wp_payin->A_brno,
		sizeof(wp_payin_hz->A_brno));	
	memcpy(wp_payin_hz->A_wssrno,wp_payin->A_wssrno,
		sizeof(wp_payin_hz->A_wssrno));	
	memcpy(wp_payin_hz->A_sendco,wp_payin->A_receco,
		sizeof(wp_payin_hz->A_sendco));	
	memcpy(wp_payin_hz->A_receco,wp_payin->A_sendco,
		sizeof(wp_payin_hz->A_receco));	
	vtcp_log("%s,%d wp_payin->A_cashqsactno==[%s]",__FL__,wp_payin->A_cashqsactno);
	vtcp_log("%s,%d wp_payin->A_payqsactno ==[%s]",__FL__,wp_payin->A_payqsactno);
	memcpy(wp_payin_hz->A_payqsactno,wp_payin->A_cashqsactno,
		sizeof(wp_payin_hz->A_payqsactno));	
	memcpy(wp_payin_hz->A_cashqsactno,wp_payin->A_payqsactno,
		sizeof(wp_payin_hz->A_cashqsactno));	
	wp_payin_hz->F_lw_ind[0]='1';
	memcpy(wp_payin_hz->F_o_or_br_no,wp_payin->F_orbrno,sizeof(wp_payin_hz->F_o_or_br_no));
	memcpy(wp_payin_hz->F_o_ac_br_no,wp_payin->F_acbrno,sizeof(wp_payin_hz->F_o_ac_br_no));
	/* begin add by LiuHuafeng 20070116 */
	memcpy(wp_payin_hz->F_dfxyno,wp_payin->F_dfxyno,sizeof(wp_payin_hz->F_dfxyno));
	memcpy(wp_payin_hz->T_dfxyno,wp_payin->T_dfxyno,sizeof(wp_payin_hz->T_dfxyno));
	vtcp_log("%s,%d wp_payin_hz->A_cashqsactno==[%s]",__FL__,wp_payin_hz->A_cashqsactno);
	vtcp_log("%s,%d wp_payin_hz->A_payqsactno ==[%s]",__FL__,wp_payin_hz->A_payqsactno);

	vtcp_log("%s,%d wp_payin_hz->f_dfxyh==[%s]",__FL__,wp_payin_hz->F_dfxyno);
	vtcp_log("%s,%d wp_payin_hz->t_dfxyh ==[%s]",__FL__,wp_payin_hz->T_dfxyno);

	vtcp_log("[%s][%d]wp_payin_hz->F_owtday==[%.12s]\n",__FILE__,__LINE__,wp_payin_hz->F_owtday);
	vtcp_log("[%s][%d]payin->F_wtday==[%.12s]\n",__FILE__,__LINE__,wp_payin->F_wtday);
	return(0);
}
/********************************************************************************/
/* 函数名称: int iLvCreateTh(PAY_IN_AREA *wp_payin,PAY_IN_AREA *wp_payin_th)    */
/* 功能描述: 根据payin内容产生一条新的退汇记录                                  */
/* 创建日期: 2005.12.2                                                       			*/
/* 作者:     SSH                                                               		*/
/* 说明:     只填充输入域，表数据可以调用newrec函数来生成       						 			*/
/********************************************************************************/
int iLvCreateTh(PAY_IN_AREA *wp_payin,PAY_IN_AREA *wp_payin_th)
{
	memset(wp_payin_th,'\0',sizeof(wp_payin_th));						
	memcpy(wp_payin_th->F_crdb,wp_payin->F_crdb,
		sizeof(wp_payin_th->F_crdb));
	memcpy(wp_payin_th->F_opcd,OPCD_LR,
		sizeof(wp_payin_th->F_opcd));
	memcpy(wp_payin_th->T_ostat,wp_payin->T_stat,
		sizeof(wp_payin_th->T_ostat));

	if (wp_payin->F_crdb[0]==CRDB_CR)
	{
		memcpy(wp_payin_th->F_pkgno,PKGNO_DJTH,
			sizeof(wp_payin_th->F_pkgno));
	}else
	{
		vtcp_log("%s,%d,本报文[%.3s]没有退汇  只有贷记业务才可退汇! \n",
			__FL__,wp_payin->F_pkgno);
		sprintf(acErrMsg,"只有贷记业务才可退汇!");
		return(-1);
	}
	memset(wp_payin_th->F_orderno,'\0',sizeof(wp_payin_th->F_orderno));
	memcpy(wp_payin_th->F_txnum,TXNUM_DJTH,
		sizeof(wp_payin_th->F_txnum));

	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,wp_payin_th->F_orbrno);
	/**	
	memcpy(wp_payin_th->F_orbrno,wp_payin->F_acbrno,
	sizeof(wp_payin_th->F_orbrno));	
	***/
	memcpy(wp_payin_th->F_acbrno,wp_payin->F_orbrno,
		sizeof(wp_payin_th->F_acbrno));	

	memcpy(wp_payin_th->F_o_ac_br_no,wp_payin->F_acbrno,
		sizeof(wp_payin_th->F_o_ac_br_no));	
	memcpy(wp_payin_th->F_o_or_br_no,wp_payin->F_orbrno,
		sizeof(wp_payin_th->F_o_or_br_no));		

	memcpy(wp_payin_th->F_txamt,wp_payin->F_txamt,
		sizeof(wp_payin_th->F_txamt));	

	memcpy(wp_payin_th->F_payno,wp_payin->F_cashno,
		sizeof(wp_payin_th->F_payno));	
	memcpy(wp_payin_th->F_cashno,wp_payin->F_payno,
		sizeof(wp_payin_th->F_cashno));		

	memcpy(wp_payin_th->F_payactno,wp_payin->F_cashactno,
		sizeof(wp_payin_th->F_payactno));	
	memcpy(wp_payin_th->F_cashactno,wp_payin->F_payactno,
		sizeof(wp_payin_th->F_cashactno));

	memcpy(wp_payin_th->F_payname,wp_payin->F_cashname,
		sizeof(wp_payin_th->F_payname));
	memcpy(wp_payin_th->F_cashname,wp_payin->F_payname,
		sizeof(wp_payin_th->F_cashname));	

	memcpy(wp_payin_th->F_payaddress,wp_payin->F_cashaddress,
		sizeof(wp_payin_th->F_payaddress));	
	memcpy(wp_payin_th->F_cashaddress,wp_payin->F_payaddress,
		sizeof(wp_payin_th->F_cashaddress));

	memset(wp_payin_th->F_userpswd,'\0',sizeof(wp_payin_th->F_userpswd));

	memcpy(wp_payin_th->F_ywtype,wp_payin->F_ywtype,
		sizeof(wp_payin_th->F_ywtype));	

	memset(wp_payin_th->F_content,'\0',sizeof(wp_payin_th->F_content));
	memcpy(wp_payin_th->F_wtday,wp_payin->A_tbsdy,
		sizeof(wp_payin_th->F_wtday));	
	wp_payin_th->F_operlevel[0]='0';
	memset(wp_payin_th->F_authno,'\0',sizeof(wp_payin_th->F_authno));
	memset(wp_payin_th->F_authpswd,'\0',sizeof(wp_payin_th->F_authpswd));
	memset(wp_payin_th->F_voctype,'\0',sizeof(wp_payin_th->F_voctype));
	memset(wp_payin_th->F_vocnum,'\0',sizeof(wp_payin_th->F_vocnum));
	memset(wp_payin_th->F_packday,'\0',sizeof(wp_payin_th->F_packday));
	memset(wp_payin_th->F_packid,'\0',sizeof(wp_payin_th->F_packid));
	memset(wp_payin_th->F_feeflag,'\0',sizeof(wp_payin_th->F_feeflag));
	memset(wp_payin_th->F_feeamt,'\0',sizeof(wp_payin_th->F_feeamt));
	memset(wp_payin_th->F_respday,'\0',sizeof(wp_payin_th->F_respday));
	memcpy(wp_payin_th->F_ptype,wp_payin->F_ptype,
		sizeof(wp_payin_th->F_ptype));	
	memcpy(wp_payin_th->F_pdate,wp_payin->F_pdate,
		sizeof(wp_payin_th->F_pdate));	
	memcpy(wp_payin_th->F_pnum,wp_payin->F_pnum,
		sizeof(wp_payin_th->F_pnum));	
	memcpy(wp_payin_th->F_ptxamt,wp_payin->F_ptxamt,
		sizeof(wp_payin_th->F_ptxamt));	
	memcpy(wp_payin_th->F_jtxamt,wp_payin->F_jtxamt,
		sizeof(wp_payin_th->F_jtxamt));	
	memcpy(wp_payin_th->F_owtday,wp_payin->F_wtday,
		sizeof(wp_payin_th->F_owtday));	
	memcpy(wp_payin_th->F_otxnum,wp_payin->F_txnum,
		sizeof(wp_payin_th->F_otxnum));	
	memcpy(wp_payin_th->F_oorderno,wp_payin->F_orderno,
		sizeof(wp_payin_th->F_oorderno));	
	memset(wp_payin_th->F_retcode,'\0',sizeof(wp_payin_th->F_retcode));
	memset(wp_payin_th->F_rcpstat,'\0',sizeof(wp_payin_th->F_rcpstat));
	memset(wp_payin_th->F_resp1,'\0',sizeof(wp_payin_th->F_resp1));
	memset(wp_payin_th->F_resp2,'\0',sizeof(wp_payin_th->F_resp2));
	/*************************/
	memcpy(wp_payin_th->A_tbsdy,wp_payin->A_tbsdy,
		sizeof(wp_payin_th->A_tbsdy));	
	memcpy(wp_payin_th->A_brno,wp_payin->A_brno,
		sizeof(wp_payin_th->A_brno));	
	memcpy(wp_payin_th->A_wssrno,wp_payin->A_wssrno,
		sizeof(wp_payin_th->A_wssrno));	
	memcpy(wp_payin_th->A_sendco,wp_payin->A_receco,
		sizeof(wp_payin_th->A_sendco));	
	memcpy(wp_payin_th->A_receco,wp_payin->A_sendco,
		sizeof(wp_payin_th->A_receco));	
	memcpy(wp_payin_th->A_payqsactno,wp_payin->A_cashqsactno,
		sizeof(wp_payin_th->A_payqsactno));	
	memcpy(wp_payin_th->A_cashqsactno,wp_payin->A_payqsactno,
		sizeof(wp_payin_th->A_cashqsactno));	
	return(0);
}
/*********************************************************************************/
/* 函数名称: int iLvSetPayout1(PAY_IN_AREA  *wp_payin)                           */    
/* 功能描述: 根据输入内容拼一般输出包                                            */
/* 创建日期: 2005.12.2                                                           */
/* 作者:     SSH                                                                 */
/* 说明:                                                                         */
/*********************************************************************************/
int iLvSetPayout1(PAY_IN_AREA *wp_payin)
{
	LV_FD123_AREA fd123;
	memset(&fd123, 0 , sizeof(fd123));
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("%s,%d 123",__FILE__,__LINE__);
	vtcp_log("[%s]",(char*)&fd123);
	vtcp_log("%s,%d 123",__FILE__,__LINE__);
	memcpy(fd123.orderno,wp_payin->T_orderno,sizeof(fd123.orderno));
	memcpy(fd123.o_orderno,wp_payin->T_oorderno,sizeof(fd123.o_orderno));
	memcpy(fd123.pay_opn_br_no,wp_payin->T_payno,sizeof(fd123.pay_opn_br_no));
	memcpy(fd123.pay_qs_no,wp_payin->T_payqsactno,sizeof(fd123.pay_qs_no));
	memcpy(fd123.cash_qs_no,wp_payin->T_cashqsactno,sizeof(fd123.cash_qs_no));
	memcpy(fd123.pack_date,wp_payin->T_packday,sizeof(fd123.pack_date));
	memcpy(fd123.pack_id,wp_payin->T_packid,sizeof(fd123.pack_id));
	memcpy(fd123.lv_sts,wp_payin->T_stat,sizeof(fd123.lv_sts));
	memcpy(fd123.resp1,wp_payin->T_resp1,sizeof(fd123.resp1));
	memcpy(fd123.resp2,wp_payin->T_resp2,sizeof(fd123.resp2));
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("%s,%d 123",__FILE__,__LINE__);
	vtcp_log("[%s]",(char*)&fd123);
	vtcp_log("%s,%d 123",__FILE__,__LINE__);

	return(0);	
}    				
/*******增加附加数据**************/
/***************************************************************************/
/* 函数名称: iLvSetAddit                                                   */
/* 功能描述: 增加附加数据域                                                */
/* 参数:                                                                   */
/*            wp_payin   -支付交易通用输入结构体                           */
/*            name       -附加数据域的名称                                 */
/*            buf        -附加数据域的内容                                 */
/*            length     -附加数据域的长度                                 */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     石升辉                                                        */
/* 说明:     附加数据的名称为3个字节长的数字或者字母, 为000表示此附加域要  */
/*           直接拼到报文中做附加域,其他为程序内部处理                     */
/***************************************************************************/
int iLvSetAddit(PAY_IN_AREA *wp_payin,char *name,char *buf,int length)
{
	int iRc=0;
	struct lv_addit_c wd_lv_addit;
	/*begin add by LiuHuafeng 20060323 */
	char cTxday[9];
	char cAddid[9];
	memset(cTxday, 0 , sizeof(cTxday));
	memcpy(cTxday,wp_payin->T_inday,sizeof(cTxday)-1);
	zip_space(cTxday);
	memset(cAddid, 0 ,sizeof(cAddid));
	memset(&wd_lv_addit,'\0',sizeof(wd_lv_addit));
	if (strlen(cTxday)==0)
		memcpy(cTxday,pcLvGetDay(),sizeof(cTxday)-1);
	wd_lv_addit.tx_date=atoi(cTxday);
	vtcp_log("[%s][%d] wd_lv_addit.tx_date==[%ld]\n",__FL__,wd_lv_addit.tx_date);
	memcpy(cAddid,wp_payin->T_addid,sizeof(cAddid)-1);
	vtcp_log("[%s][%d] cAddid==[%s]\n",__FL__,cAddid);
	zip_space(cAddid);
	vtcp_log("[%s][%d] strlen(cAddid)==[%d]\n",__FL__,strlen(cAddid));
	vtcp_log("[%s][%d] 我要看这里的BUF==[%s]\n",__FL__,buf);
	if (strlen(cAddid)==0)
	{
		apitoa(iLvGetAddid(),sizeof(wd_lv_addit.addid)-1,wd_lv_addit.addid);
		vtcp_log("[%s][%d] wd_lv_addit.addid==[%s]\n",__FL__,wd_lv_addit.addid);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		memcpy(wd_lv_addit.tagdata,buf,length);
		/****NEWYX2:设置长度****/
		wd_lv_addit.taglen=length;
		vtcp_log("[%s][%d] wd_lv_addit.tagname==[%s]\n",__FL__,wd_lv_addit.tagname);
		iRc=Lv_addit_Ins(wd_lv_addit, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FL__,iRc);

			return(-1);
		}
	}else
	{
		memcpy(wd_lv_addit.addid,wp_payin->T_addid,sizeof(wd_lv_addit.addid)-1);
		vtcp_log("[%s][%d] wd_lv_addit.addid==[%s]\n",__FL__,wd_lv_addit.addid);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		vtcp_log("[%s][%d] find addid[%s] \n",__FL__,wd_lv_addit.addid);
		iRc=Lv_addit_Dec_Upd(g_pub_tx.reply, "addid = '%s' and tagname = '%s' ", wd_lv_addit.addid, wd_lv_addit.tagname);
		if (iRc)
		{
			if (iRc==100)
			{
				vtcp_log("[%s][%d] wd_lv_addit.addid==[%s]\n",__FL__,wd_lv_addit.addid);
				/* 根据石总要求，对照不到的增加附加域 20060406*/
				/** NEWYX BEGIN:修正BUG**/
				memcpy(wd_lv_addit.tagdata,buf,length);
				/** NEWYX END:修正BUG**/
				/****NEWYX2:设置长度****/
				wd_lv_addit.taglen=length;
				iRc=Lv_addit_Ins(wd_lv_addit, g_pub_tx.reply);
				if (iRc)
				{
					vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FL__,iRc);
					return(-1);
				}
				return 0;
			}
			else
			{
				vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FL__,iRc);
				return(-1);
			}
		}
		vtcp_log("[%s][%d] buf==[%s]\n",__FL__,buf);
		vtcp_log("[%s][%d] buf==[%s]\n",__FL__,wd_lv_addit.tagdata);
		/****
		memcpy(wd_lv_addit.tagdata,buf,length);****/
		iRc = Lv_addit_Fet_Upd(&wd_lv_addit,g_pub_tx.reply);
		if (iRc)
		{
			/******** 如果没有也要插入 dongxy 20060227 begin **********/
			if (iRc==100)
			{
				vtcp_log("[%s][%d] wd_lv_addit.addid==[%s]\n",__FL__,wd_lv_addit.addid);
				vtcp_log("[%s][%d] buf==[%s]\n",__FL__,buf);
				vtcp_log("[%s][%d] length==[%d]\n",__FL__,length);
				/* 根据石总要求，对照不到的增加附加域 20060406*/
				/** NEWYX BEGIN:修正BUG**/
				memcpy(wd_lv_addit.tagdata,buf,length);
				/** NEWYX END:修正BUG**/
				/****NEWYX2:设置长度****/
				vtcp_log("[%s][%d] length==[%d]\n",__FL__,length);
				wd_lv_addit.taglen=length;
				vtcp_log("[%s][%d] length==[%d]\n",__FL__,wd_lv_addit.taglen);
				iRc=Lv_addit_Ins(wd_lv_addit, g_pub_tx.reply);
                                vtcp_log("[%s][%d] iRc=[%d]\n",__FILE__,__LINE__,iRc);
				if (iRc)
				{
					vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FL__,iRc);
					Lv_addit_Clo_Upd(); /**add by NEWBP5***/
					return(-1);
				}
                                vtcp_log("[%s][%d] 不关闭游标就退出??\n",__FILE__,__LINE__);
				Lv_addit_Clo_Upd(); /**add by NEWBP5***/
				return 0;
			}
			else
			{
				/******** 如果没有也要插入 dongxy 20060227 end **********/
				vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FL__,iRc);
				Lv_addit_Clo_Upd(); /**add by NEWBP5***/
				return(-1);
			}
		}
		vtcp_log("[%s][%d] buf==[%s]\n",__FL__,wd_lv_addit.tagdata);
		memcpy(wd_lv_addit.tagdata,buf,length);
		/****NEWYX2:设置长度****/
		wd_lv_addit.taglen=length;
		iRc=Lv_addit_Upd_Upd(wd_lv_addit, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d, Insert lv_addit error=%d   插入附加数据表失败\n",__FL__,iRc);
			Lv_addit_Clo_Upd(); /**add by NEWBP5**/
			return(-1);
		}
	}
	Lv_addit_Clo_Upd();/**add by NEWBP5**/
	memcpy(wp_payin->T_addid,wd_lv_addit.addid,sizeof(wp_payin->T_addid));
	vtcp_log("[%s][%d] wp_payin->T_addid==[%s]\n",__FL__,wp_payin->T_addid);
	return(0);
}
/*******1.3.7.    得到附加数据**************/
/***************************************************************************/
/* 函数名称: iLvGetAddit                                                   */
/* 功能描述: 得到附加数据域                                                */
/* 参数:                                                                   */
/*            wp_payin   -支付交易通用输入结构体                           */
/*            name       -附加数据域的名称                                 */
/*            buf        -附加数据域的内容                                 */
/*            length     -附加数据域的长度                                 */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     石升辉                                                        */
/* 说明:     附加数据的名称为3个字节长的数字或者字母                       */
/***************************************************************************/
int iLvGetAddit(PAY_IN_AREA *wp_payin,char *name,char *buf,int length)
{
	int iRc=0;
	int i=0,j=0;
	struct lv_addit_c wd_lv_addit;
	/*begin add by LiuHuafeng 20060324 */
	char cTxday[9];
	memset(cTxday,0 , sizeof(cTxday));
	memcpy(cTxday,wp_payin->T_inday,sizeof(cTxday)-1);
	zip_space(cTxday);
	if (strlen(cTxday)!=8)
		memcpy(cTxday,wp_payin->F_wtday,sizeof(cTxday)-1);
	/*end add by LiuHuafeng 20060324 */
	vtcp_log("[%s][%d]",__FL__);
	memset(&wd_lv_addit,'\0',sizeof(wd_lv_addit));
	/* rem by LiuHuafeng 20060324
	memcpy(wd_lv_addit.txday,wp_payin->F_wtday,sizeof(wd_lv_addit.txday)-1);
	**********replace by next line******************/
	wd_lv_addit.tx_date=apatoi(cTxday,8);
	vtcp_log("[%s][%d]wd_lv_addit.txday=[%d]\n",__FL__,wd_lv_addit.tx_date);
	memcpy(wd_lv_addit.addid,wp_payin->T_addid,sizeof(wd_lv_addit.addid)-1);
	vtcp_log("[%s][%d]wd_lv_addit.addid=[%s]\n",__FL__,wd_lv_addit.addid);
	vtcp_log("[%s][%d]payin.T_addid=[%s]\n",__FL__,wp_payin->T_addid);
	memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
	vtcp_log("[%s][%d]wd_lv_addit.tagname=[%s]\n",__FL__,wd_lv_addit.tagname);
	iRc=Lv_addit_Sel(g_pub_tx.reply, &wd_lv_addit, "tx_date = '%ld' and addid = '%s' and tagname = '%s'", wd_lv_addit.tx_date, wd_lv_addit.addid, wd_lv_addit.tagname );
	if (iRc)
	{
		vtcp_log("%s,%d,锁定附加数据表失败,认为无数据%d\n",__FL__,iRc);
		return(0);
	}
	/****NEWYX2:按表中长度取,并补充空格****/
	if (length<=0)
	{
		strcpy(buf,wd_lv_addit.tagdata);
		length=wd_lv_addit.taglen;
	}else
	{
		memcpy(buf,wd_lv_addit.tagdata,length);
	}
	for(i=0;i<length;i++){
		if(buf[i]=='\0'){
			break;
		}
	}
	for(j=i;j<length;j++){
		buf[j]=' ';
	}
	return(0);
}
/*********************************************************************/
/************************|----------------|***************************/
/************************|待实现的内部函数|***************************/
/************************|----------------|***************************/
/*********************************************************************/

/*******************************************/
/* 函数名称: char* pcLvGetDay()            */
/* 功能描述: 得到小额系统日期            	   */
/* 创建日期: 2005.12.2               			   */
/* 作者:     SSH                       		   */
/* 说明:          								  				 */
/* 2006.4.8修改,除发送时的workdate外，其他地方都取综合业务日期****/
/*******************************************/
char* pcLvGetDay()
{
	static char txday[9];
	/*FD5存放发生日期*/
	get_fd_data("0050",txday);
	txday[8]='\0';
	return txday;/* add by LiuHuafeng 20060421*/
}
char * pcLvGetDay_work()
{
	int  ret = 0;
	static	char cLvWorkday[9];
	struct  lv_sysctl_c lv_sysctl;
	memset(cLvWorkday, 0 , sizeof(cLvWorkday));
	ret = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl, "1=1");
	if (ret)
	{
		vtcp_log("%d@%s  (%s) return NULL &&", __LINE__, __FILE__, __FUNCTION__);
		return NULL;
	}else
	{
		vtcp_log("%s,%d here! txdate=[%d]\n",__FL__,lv_sysctl.tx_date);
		sprintf(cLvWorkday,"%d",lv_sysctl.tx_date);
		return cLvWorkday;
	}
}
/************/
/*******************************************/
/* 函数名称: char* pcLvGetNextDay()        */
/* 功能描述: 得到小额系统的下一天        	   */
/* 创建日期: 2005.12.2               			   */
/* 作者:     SSH                       		   */
/* 说明:          								  				 */
/*******************************************/
char* pcLvGetNextDay()
{
	int  ret = 0;
	struct  lv_sysctl_c lv_sysctl;
	static       char sql_txday[9];

	memset(&sql_txday,'\0',sizeof(sql_txday));
	ret = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl, "1=1");
	if (ret)
	{
		vtcp_log("%d@%s  (%s) return NULL &&", __LINE__, __FILE__, __FUNCTION__);
		return NULL;
	}else
	{
		vtcp_log("%s,%d here交易日期=[%d]!\n",__FL__,lv_sysctl.next_date);
		sprintf(sql_txday, "%d",lv_sysctl.next_date);
	}
	return (sql_txday)	;
}
/*******************************************/
/* 函数名称: char* pcLvGetChgNode()        */
/* 功能描述: 得到小额chgnode            	   */
/* 创建日期: 2005.12.2               			   */
/* 作者:     SSH                       		   */
/* 说明:          								  				 */
/*******************************************/ 
char* pcLvGetChgNode()
{
	int  ret = 0;
	struct  lv_sysctl_c lv_sysctl;
	static       char sql_chgnode[4];

	memset(&sql_chgnode,'\0',sizeof(sql_chgnode));
	ret = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl, "1=1");
	if (ret)
	{
		vtcp_log("%d@%s  (%s) return NULL &&", __LINE__, __FILE__, __FUNCTION__);
		return NULL;
	}else
	{
		strcpy(sql_chgnode, lv_sysctl.chgnode);
	}
	vtcp_log("%s,%d here!\n",__FL__);
	return (sql_chgnode);

}
/*******************************************/
/* 函数名称: char* pcLvGetCurStat()        */
/* 功能描述: 得到小额curstat            	   */
/* 创建日期: 2005.12.2               			   */
/* 作者:     SSH                       		   */
/* 说明:          								  				 */
/*******************************************/ 
char* pcLvGetCurStat()
{
	int  ret = 0;
	struct  lv_sysctl_c lv_sysctl;
	static  char sql_curstat[8];

	memset(sql_curstat,'\0',sizeof(sql_curstat));
	ret = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl, "1=1");
	if (ret)
	{
		vtcp_log("%d@%s  (%s) return NULL &&", __LINE__, __FILE__, __FUNCTION__);
		return NULL;
	}else
	{
		strcpy(sql_curstat, lv_sysctl.curstat);
	}
	vtcp_log("%s,%d here!\n",__FL__);
	return (sql_curstat);
}
/*******************************************/
/* 函数名称: char* pcLvGetChgTime()        */
/* 功能描述: 得到小额chgtime            	   */
/* 创建日期: 2005.12.2               			   */
/* 作者:     SSH                       		   */
/* 说明:          								  				 */
/*******************************************/
char* pcLvGetChgTime()
{
	int  ret = 0;
	struct  lv_sysctl_c lv_sysctl;
	static       char sql_chgtime[14];

	memset(&sql_chgtime,'\0',sizeof(sql_chgtime));
	ret = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl, "1=1");
	if (ret)
	{
		vtcp_log("%d@%s  (%s) return NULL &&", __LINE__, __FILE__, __FUNCTION__);
		return NULL;
	}else
	{
		strcpy(sql_chgtime, lv_sysctl.chgtime);
	}
	vtcp_log("%s,%d here!\n",__FL__);
	return (sql_chgtime);

}
/*******************************************/
/* 函数名称: char* pcLvGetMsg()            */
/* 功能描述: 得到小额message            	   */
/* 创建日期: 2005.12.2               			   */
/* 作者:     SSH                       		   */
/* 说明:          								  				 */
/*******************************************/
char* pcLvGetMsg()
{
	int  ret = 0;
	struct  lv_sysctl_c lv_sysctl;
	static       char sql_msg[60];

	memset(&sql_msg,'\0',sizeof(sql_msg));
	ret = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl, "1=1");
	if (ret)
	{
		vtcp_log("%d@%s  (%s) return NULL &&", __LINE__, __FILE__, __FUNCTION__);
		return NULL;
	}else
	{
		strcpy(sql_msg, lv_sysctl.message);
	}
	vtcp_log("%s,%d here!\n",__FL__);
	return (sql_msg);

}
/********************************************/
/* 函数名称: int iUpdLvCheckstat(chat stat) */
/* 更新lv_sysctl表中的对帐状态              */
/* 创建日期: 2006-11-2 15:08                */
/* 作者: LiuHuafeng                         */
/* 说明:                                    */
/********************************************/
int iUpdLvCheckstat(char stat)
{
	struct lv_sysctl_c sLv_sysctl;
	memset(&sLv_sysctl, 0 , sizeof(sLv_sysctl));
	g_reply_int=Lv_sysctl_Dec_Upd(g_pub_tx.reply,"1=1");
	if (g_reply_int)
	{
		vtcp_log("%s,%d 读取系统状态表出现错误%d",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	g_reply_int=Lv_sysctl_Fet_Upd(&sLv_sysctl,g_pub_tx.reply);
	if (g_reply_int)
	{
		vtcp_log("%s,%d 读取系统表游标出现错误%d",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	sLv_sysctl.checkstat[0]=stat;
	g_reply_int=Lv_sysctl_Upd_Upd(sLv_sysctl,g_pub_tx.reply);
	if (g_reply_int)
	{
		vtcp_log("%s,%d 更新系统表出现错误%d",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	return 0;
}

/****判断一个内部帐号（取付款帐号）是否允许透支**/
/*BOOL iLvCanAccTz(PAY_IN_AREA *wp_payin){  */ /*去掉gujy 060829*/

/******************************************************************/
/* 函数名称: int iLvCountFeeamt(PAY_IN_AREA *wp_payin)            */
/* 功能描述: 计算手续费金额              	  												*/
/* 创建日期: 2005.12.2               			  												*/
/* 作者:     SSH                       		  												*/
/* 说明:          								  																*/
/******************************************************************/
int iLvCountFeeamt(PAY_IN_AREA *wp_payin)
{
	if (wp_payin->F_feeflag[0]!='\0' && wp_payin->F_feeflag[0]!=FEEFLAG_NO)
	{
		/***开始计算**/
		memcpy(wp_payin->F_feeamt,"000000000000500",15);
		memcpy(wp_payin->T_feeamt,"000000000000500",15);
		return(0);
	}else
	{
		/* add by LiuHuafeng */
		memcpy(wp_payin->F_feeamt,"000000000000000",15);
		memcpy(wp_payin->T_feeamt,"000000000000000",15);
		/* end by lhf */
		return(0);
	}
}
long iLvGetSeqno(char *brno, char *ac_ind)
{
	struct seqctl_c seqctl_c;
	char cFullRefno[21];
	char cSeqno[9];
	char cKinbr[6];
	int ret=0;
	memset(cSeqno, 0 ,sizeof(cSeqno));
	memset(cKinbr, 0 ,sizeof(cKinbr));
	memset(&seqctl_c, 0 , sizeof(seqctl_c));

	memcpy(cKinbr, brno, sizeof(cKinbr)-1);
	if (memcmp(ac_ind,HV_ORDERNO_IND,2)==0 || memcmp(ac_ind,LV_ACTYPE,2)==0)
	{
		GetOr_br_noByBr_no(cKinbr,seqctl_c.br_no);
	}
	else
	{
		memcpy(seqctl_c.br_no,cKinbr,sizeof(cKinbr)-1);  
	}
	memcpy(seqctl_c.ac_ind, ac_ind, sizeof(seqctl_c.ac_ind)-1);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",
		seqctl_c.br_no,seqctl_c.ac_ind);
	if (ret)
	{
		sprintf(acErrMsg,"执行Point_point_mst_Dec_Upd错[%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf(acErrMsg,"查询小额支付附加域序号表错误![%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	else if (ret)
	{
		sprintf(acErrMsg,"执行seqctl_Fet_Upd 错[%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	vtcp_log("[%s][%d]!!!!!\n",__FILE__,__LINE__);
	sprintf(cSeqno,"%08.0f",seqctl_c.seqno);
	vtcp_log("[%s][%d]!!!!!\n",__FILE__,__LINE__);
	if (seqctl_c.seqno==99999999) seqctl_c.seqno=0;
	seqctl_c.seqno++;
	ret=Seqctl_Upd_Upd(seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf(acErrMsg,"修改积分主表错误!");
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		sprintf(acErrMsg,"取得支付交易附加域序号成功![%s]",g_pub_tx.reply);
	WRITEMSG
		return seqctl_c.seqno;
	ErrExit:
		sprintf(acErrMsg,"取得支付交易附加域序号失败![%s]",g_pub_tx.reply);
	WRITEMSG
		return -1;
}
/******************************************************************/
/* 函数名称: int iLvGetAddid()    											            */
/* 功能描述: 得到下一个附加数据域id,与交易机构无关    							*/
/* 创建日期: 2005.12.2               			  												*/
/* 作者:     SSH                       		  												*/
/* 说明:          								  																*/
/******************************************************************/
int iLvGetAddid()
{
	return iLvGetSeqno(HV_BR_NO, LV_ADDID_IND);
}
/******************************************************************/
/* 函数名称: int iLvGetPackid()    											            */
/* 功能描述: 得到下一个包id,与交易机构无关    											*/
/* 创建日期: 2005.12.2               			  												*/
/* 作者:     SSH                       		  												*/
/* 说明:          								  																*/
/******************************************************************/
int iLvGetPackid()
{
	return iLvGetSeqno(HV_BR_NO, LV_PACKID_IND);

}
/******************************************************************/
/* 函数名称: int iLvGetOrderno()                                  */
/* 功能描述: 得到下一个支付交易序号,与交易机构相关                */
/* 创建日期: 2005.12.2                                            */
/* 作者:     SSH                                                  */
/* 说明:                                                          */
/******************************************************************/
int iLvGetOrderno()
{
	char cBrno[6];

	memset(cBrno, 0x0, sizeof(cBrno));
	get_fd_data("0030", cBrno);

	return iLvGetSeqno(cBrno, LV_ACTYPE);
}
/****************************************************************************************/
/* 函数名称: int iLvPayinAutoInfo(PAY_IN_AREA *wp_payin)                                */
/* 功能描述: 得到支付交易数据交换区的自动信息:流水号、小额日期和发起行号    							*/
/* 创建日期: 2005.12.2               			  																							*/
/* 作者:     SSH                       		  																							*/
/* 说明:          								  																											*/
/****************************************************************************************/
int iLvPayinAutoInfo(PAY_IN_AREA *wp_payin)
{
	char cBrno[12];

	memset(cBrno, 0x0, sizeof(cBrno));
	get_fd_data("0030", cBrno);
	memcpy(wp_payin->A_tbsdy,pcLvGetDay(),sizeof(wp_payin->A_tbsdy));/*日期*/
	vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%s]",__FL__,wp_payin->F_wtday);
	get_fd_data("0040", wp_payin->A_wssrno);

	/****填上orbrno*****/
	/**Modified by SSH,回执交易不自动填写发起行号****/
	if (iLvGetOrient(wp_payin)==TX_SEND &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))!=0 )
	{
		vtcp_log("[%s][%d]=========wp_payin->F_crdb[0]=[%c]\n",__FL__,wp_payin->F_crdb[0]);
		vtcp_log("[%s][%d]wp_payin->F_wtdaty",__FL__,wp_payin->F_wtday);
		if (wp_payin->F_crdb[0]=='1')
		{
			GetOr_br_noByBr_no(cBrno,wp_payin->F_orbrno);
			memcpy(wp_payin->F_payno,wp_payin->F_orbrno,
				sizeof(wp_payin->F_payno));
		}
		else
		{
			GetOr_br_noByBr_no(cBrno,wp_payin->F_orbrno);
			memcpy(wp_payin->F_cashno,wp_payin->F_orbrno,
				sizeof(wp_payin->F_cashno));
		}
	}
	vtcp_log("[%s][%d]=========wp_payin->F_crdb[0]=[%c]\n",__FL__,wp_payin->F_crdb[0]);
	vtcp_log("[%s][%d]wp_payin->F_wtdaty==[%s]",__FL__,wp_payin->F_wtday);
	return(0);
}   
/******************************************************/
/* 函数名称: int iLvZipAll(PAY_IN_AREA *wp_payin)        */
/* 功能描述: 压缩所有空格    														*/
/* 创建日期: 2005.12.2               			  						*/
/* 作者:     SSH                       		  						*/
/* 说明:          								  										*/
/******************************************************/
int iLvZipAll(PAY_IN_AREA *wp_payin)
{
	zipn_space(wp_payin->F_txamt,sizeof(wp_payin->F_txamt));
	zipn_space(wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
	zipn_space(wp_payin->F_payname,sizeof(wp_payin->F_payname));
	zipn_space(wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
	zipn_space(wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
	zipn_space(wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
	zipn_space(wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress));
	zipn_space(wp_payin->F_ywtype,sizeof(wp_payin->F_ywtype));
	zipn_space(wp_payin->F_content,sizeof(wp_payin->F_content));
	zipn_space(wp_payin->F_authno,sizeof(wp_payin->F_authno));
	zipn_space(wp_payin->F_vocnum,sizeof(wp_payin->F_vocnum));
	zipn_space(wp_payin->F_feeamt,sizeof(wp_payin->F_feeamt));
	zipn_space(wp_payin->F_pnum,sizeof(wp_payin->F_pnum));
	zipn_space(wp_payin->F_ptxamt,sizeof(wp_payin->F_ptxamt));
	zipn_space(wp_payin->F_jtxamt,sizeof(wp_payin->F_jtxamt));
	zipn_space(wp_payin->T_txamt,sizeof(wp_payin->T_txamt));
	zipn_space(wp_payin->T_payactno,sizeof(wp_payin->T_payactno));
	zipn_space(wp_payin->T_payname,sizeof(wp_payin->T_payname));
	zipn_space(wp_payin->T_cashactno,sizeof(wp_payin->T_cashactno));
	zipn_space(wp_payin->T_cashname,sizeof(wp_payin->T_cashname));
	zipn_space(wp_payin->T_ywtype,sizeof(wp_payin->T_ywtype));
	zipn_space(wp_payin->T_ptxamt,sizeof(wp_payin->T_ptxamt));
	zipn_space(wp_payin->T_jtxamt,sizeof(wp_payin->T_jtxamt));
	zipn_space(wp_payin->T_brno,sizeof(wp_payin->T_brno));
	zipn_space(wp_payin->T_inputno,sizeof(wp_payin->T_inputno));
	zipn_space(wp_payin->T_checkno,sizeof(wp_payin->T_checkno));
	zipn_space(wp_payin->T_authno,sizeof(wp_payin->T_authno));
	zipn_space(wp_payin->T_sendno,sizeof(wp_payin->T_sendno));
	zipn_space(wp_payin->T_respno,sizeof(wp_payin->T_respno));
	zipn_space(wp_payin->T_recvno,sizeof(wp_payin->T_recvno));
	zipn_space(wp_payin->T_dealno,sizeof(wp_payin->T_dealno));
	zipn_space(wp_payin->T_feeamt,sizeof(wp_payin->T_feeamt));
	return(0);
}
/********************************************************/
/* 函数名称: char *pcPayinTxname(PAY_IN_AREA *wp_payin) */
/* 功能描述: 得到交易的描述信息    												*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:          								  											*/
/********************************************************/
char *pcPayinTxname(PAY_IN_AREA *wp_payin)
{
	static char txname[21];
	memset(txname,'\0',sizeof(txname));
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"普通贷记");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSDJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"实时贷记[通存]");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"定期贷记");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"实时贷记回执");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_PTJJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"普通借记");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSJJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"实时借记");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DQJJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"定期借记");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"定期借记回执");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"实时借记回执");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"普通借记回执");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DJTH,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"贷记退汇");
	}else
	{
		strcpy(txname,"未知业务");
	}
	if (memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0)
	{
		strcat(txname,"录入");
	}else if(memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))==0)
	{ 
		if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJ, sizeof(wp_payin->F_pkgno))==0)
		{
			strcat(txname,"记账");	
		}	 
		else	
			strcat(txname,"复核");
	}else if(memcmp(wp_payin->F_opcd,OPCD_SEND,sizeof(wp_payin->F_opcd))==0)
	{
		strcat(txname,"发送");
	}else if(memcmp(wp_payin->F_opcd,OPCD_RECEIVE,
	sizeof(wp_payin->F_opcd))==0)
	{
		strcat(txname,"接收");
	}else
	{
		strcat(txname,"未知操作");
	}
	return(txname);
}
/********************************************************/
/* 函数名称: char* pcPayinRetdtl(PAY_IN_AREA *wp_payin) */
/* 功能描述: 得到退汇描述代码     												*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:          								  											*/
/********************************************************/
char* pcPayinRetdtl(PAY_IN_AREA *wp_payin)
{
	static char pcRetdtl[13];
	char pcRetno[3];
	int iRetno=0;
	memset(pcRetno,'\0',sizeof(pcRetno));
	memset(pcRetdtl,'\0',sizeof(pcRetdtl));
	memcpy(pcRetno,wp_payin->F_retcode,sizeof(pcRetno)-1);
	iRetno=atoi(pcRetno);
	vtcp_log("%s,%d,退汇原因代码:%02d\n",__FL__,iRetno);
	switch(iRetno)
	{
	case 1:
		strcpy(pcRetdtl,"帐号不存在");
		break;
	case 2:
		strcpy(pcRetdtl,"帐号户名不符");
		break;
	case 3:
		strcpy(pcRetdtl,"帐户余额不足支付");
		break;
	case 10:
		strcpy(pcRetdtl,"帐户密码错误");
		break;
	case 11:
		strcpy(pcRetdtl,"帐户状态错误");
		break;
	case 20:
		strcpy(pcRetdtl,"业务已撤销");
		break;
	default:
		strcpy(pcRetdtl,"其他错误");
	}
	return(pcRetdtl);
}
/********************************************************/
/* 函数名称: char *pcLvPayinTxtype(char *txnum)         */
/* 功能描述: 得到业务类型的描述    												*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:          								  											*/
/********************************************************/
char *pcLvPayinTxtype(char *txnum)
{
	static char pcTxtype[13];
	memset(pcTxtype,'\0',sizeof(pcTxtype));
	vtcp_log("[%s][%d] txnum==[%.5s]\n",__FL__,txnum);
	if (memcmp(txnum,TXNUM_HD,5)==0)
	{
		strcpy(pcTxtype,"小额汇兑");
	}	else if(memcmp(txnum,TXNUM_ZJTH,5)==0)
	{
		strcpy(pcTxtype,"财政直接退回");
	}else if(memcmp(txnum,TXNUM_SQTH,5)==0)
	{
		strcpy(pcTxtype,"财政授权退回");
	}else if(memcmp(txnum,TXNUM_GZDJ,5)==0)
	{
		strcpy(pcTxtype,"国债兑贷划拨");
	} else if (memcmp(txnum,TXNUM_TSCF,5)==0)
	{
		strcpy(pcTxtype,"小额托收承付");
	}else if(memcmp(txnum,TXNUM_WT,5)==0)
	{
		strcpy(pcTxtype,"小额委托收款");
	}else if(memcmp(txnum,TXNUM_GKDJ,5)==0)
	{
		strcpy(pcTxtype,"国库贷记划拨");
	}else if(memcmp(txnum,TXNUM_QTDJ,5)==0)
	{
		strcpy(pcTxtype,"其他贷记业务");
	}else if(memcmp(txnum,TXNUM_JS,5)==0)
	{
		strcpy(pcTxtype,"缴税");
	}else if(memcmp(txnum,TXNUM_FSSR,5)==0)
	{
		strcpy(pcTxtype,"非税收入");
	}else if(memcmp(txnum,TXNUM_JF,5)==0)
	{
		strcpy(pcTxtype,"缴费");
	}else if(memcmp(txnum,TXNUM_GKJJ,5)==0)
	{
		strcpy(pcTxtype,"国库借记划拨");
	}else if(memcmp(txnum,TXNUM_QTJJ,5)==0)
	{
		strcpy(pcTxtype,"其他借记业务");
	}else if(memcmp(txnum,TXNUM_ZJZF,5)==0)
	{
		strcpy(pcTxtype,"财政直接支付");
	}else if(memcmp(txnum,TXNUM_SQZF,5)==0)
	{
		strcpy(pcTxtype,"财政授权支付");
	}else if(memcmp(txnum,TXNUM_ZPJL,5)==0)
	{
		strcpy(pcTxtype,"支票截留业务");
		/****NEWYX BEGIN:YXJL****/
	}else if(memcmp(txnum,TXNUM_TYJL,5)==0)
	{
		strcpy(pcTxtype,"通用截留业务");
		/****NEWYX END:YXJL****/
	}else if(memcmp(txnum,TXNUM_GZJJ,5)==0)
	{
		strcpy(pcTxtype,"国债借记划拨");
	}else if(memcmp(txnum,TXNUM_DJTH,5)==0)
	{
		strcpy(pcTxtype,"贷记退汇业务");
	}else if(memcmp(txnum,TXNUM_DQDJ,5)==0)
	{
		strcpy(pcTxtype,"定期贷记业务");
	}else if(memcmp(txnum,TXNUM_DQJJ,5)==0)
	{
		strcpy(pcTxtype,"定期借记业务");
	}else if(memcmp(txnum,TXNUM_PLKS,5)==0)
	{
		strcpy(pcTxtype,"批量扣税业务");
	}	else if(memcmp(txnum,TXNUM_TC,5)==0)
	{
		strcpy(pcTxtype,"通存业务");
	}	else if(memcmp(txnum,TXNUM_SSKS,5)==0)
	{
		strcpy(pcTxtype,"实时扣税业务");
	}else if(memcmp(txnum,TXNUM_TD,5)==0)
	{
		strcpy(pcTxtype,"储蓄通兑业务");
	}else if(memcmp(txnum,TXNUM_DGTD,5)==0)
	{
		strcpy(pcTxtype,"对公通兑业务");
	}else if(memcmp(txnum,TXNUM_TYXX,5)==0)
	{
		strcpy(pcTxtype,"通用信息业务");
	}else if(memcmp(txnum,TXNUM_DS,5)==0)
	{
		strcpy(pcTxtype,"代收业务");
	}else if(memcmp(txnum,TXNUM_DF,5)==0)
	{
		strcpy(pcTxtype,"代付业务");
	}else if(memcmp(txnum,TXNUM_ZPQC,5)==0)
	{
		strcpy(pcTxtype,"支票圈存业务");
	}else if(memcmp(txnum,TXNUM_ZHCX,5)==0)
	{
		strcpy(pcTxtype,"帐户查询业务");
	}else if(memcmp(txnum,TXNUM_CXYD,5)==0)
	{
		strcpy(pcTxtype,"帐户查询应答");
	}else if(memcmp(txnum,TXNUM_ZSCX,5)==0)
	{
		strcpy(pcTxtype,"CFCA证书查询");
	}else if(memcmp(txnum,TXNUM_ZSYD,5)==0)
	{
		strcpy(pcTxtype,"CFCA查询应答");
	}else if(memcmp(txnum,TXNUM_ZPYD,5)==0)
	{
		strcpy(pcTxtype,"支票圈存回应");
	}else
	{
		strcpy(pcTxtype,"未知业务");
		vtcp_log("[%s][%d]pcTxtype=[%s]txnum=[%s]\n",__FL__,pcTxtype,txnum);
	}
	return(pcTxtype);
}
/********************************************************/
/* 函数名称: char *pcLvPayinYwtype(char *pywtype)       */
/* 功能描述: 得到业务种类的描述    											*/
/* 创建日期: 2005.12.2               			  						*/
/* 作者:     SSH                       		  						*/
/* 说明:          								  										*/
/********************************************************/
char *pcLvPayinYwtype(char *pywtype)
{
	static char pcYwtype[13];
	memset(pcYwtype,'\0',sizeof(pcYwtype));
	if (memcmp(pywtype,"11",2)==0)
	{
		strcpy(pcYwtype,"普通汇兑");
	}else if(memcmp(pywtype,"10",2)==0)
	{
		strcpy(pcYwtype,"现金汇款");
	}else if(memcmp(pywtype,"30",2)==0)
	{
		strcpy(pcYwtype,"外汇清算");
	}else if(memcmp(pywtype,"40",2)==0)
	{
		strcpy(pcYwtype,"资金调拨");
	}else if(memcmp(pywtype,"50",2)==0)
	{
		strcpy(pcYwtype,"国库汇款");
	}else if(memcmp(pywtype,"51",2)==0)
	{
		strcpy(pcYwtype,"国库同城净额");
	}else if(memcmp(pywtype,"61",2)==0)
	{
		strcpy(pcYwtype,"人行票据扎差");
	}else
	{
		memcpy(pcYwtype,pywtype,sizeof(pcYwtype)-1);
	}
	return(pcYwtype);
}
/***************************************************************************/
/* 函数名称: iAmtCmp                                                       */
/* 功能描述: 比较两个金额型字段                                            */
/* 参数:                                                                   */
/*            pcNum1   -第1个金额型字段                                    */
/*            pcNum2   -第2个金额型字段                                    */
/*            iSize    -比较长度                                           */
/* 返回值:   0:相等  1:pcNum1>pcNum2 -1:pcNum1<pcNum2                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iAmtCmp(char *pcNum1,char *pcNum2,int iSize)
{
	int i=0;
	char c1,c2;
	if (pcNum1[0]=='\0')
		memset(pcNum1,'\0',iSize);
	if (pcNum2[0]=='\0')
		memset(pcNum2,'\0',iSize);

	for (i=0;i<iSize;i++)
	{
		c1=pcNum1[i];
		c2=pcNum2[i];
		if (c1=='\0')
			c1='0';
		if (c2=='\0')
			c2='0';
		if (c1>c2)
		{
			return(1);
		}else if(c1<c2)
		{
			return(-1);
		}
	}
	return(0);
}
char * pcLvDesPswdCode(char *code)
{
	static char pcDesCode[513];
	memset(pcDesCode,'\0',sizeof(pcDesCode));
	memset(pcDesCode,'9',sizeof(pcDesCode)-1);
	/***CAUTION:在此补上求密码交换码的代码***/

	return pcDesCode;
}
char * pcUndesPswdCode(char *code)
{
	static char pcUndesCode[9];
	memset(pcUndesCode,'\0',sizeof(pcUndesCode));
	memset(pcUndesCode,'0',sizeof(pcUndesCode)-1);
	/***CAUTION:在此补上求密码明文的代码****/

	return(pcUndesCode);
}
/********************************************************/
/* 函数名称: int iLvDateStat()                          */
/* 功能描述: 得到小额业务是否换日的标志      							*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:    1-已经换日 0 -没有换日 其他数据库错误    			*/
/********************************************************/
int iLvDateStat()
{
	char cG_txday[9];
	char cL_txday[9];

	/****CAUTION:test******/
	return(0);

}
/********************************************************/
/* 函数名称: char* pcLvGetRecvno()                      */
/* 功能描述: 根据当前机构得到接收操作员号     						*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:                                             			*/
/********************************************************/
/*??Gc*/
char* pcLvGetRecvno()
{
	return("00000");
}
/*****************输出payout包********add by liuyue 20051103*******
*************************测试用************************************/
int print_pay_out1(PAY_OUT1_AREA *pay_out1)
{
	return(0);
}
int print_payin_ly(PAY_IN_AREA *wp_payin)
{
	vtcp_log("[%s][%d]开始打印payin结构体!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]payin=[%s](len=%d)\n",__FILE__,__LINE__,((char*)wp_payin)+sizeof(BASE_EXCHANGE_AREA),sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));
	vtcp_log("[%s][%d] F_crdb        =[%c] ",__FILE__,__LINE__,wp_payin->F_crdb[0]        );    
	vtcp_log("[%s][%d] F_crdb        =[%c] ",__FILE__,__LINE__,wp_payin->F_crdb[0]        );    
	vtcp_log("[%s][%d] F_opcd        =[%.2s] ",__FILE__,__LINE__,wp_payin->F_opcd        ); 
	vtcp_log("[%s][%d] F_pkgno       =[%.3s] ",__FILE__,__LINE__,wp_payin->F_pkgno       );
	vtcp_log("[%s][%d] F_orderno     =[%.8s] ",__FILE__,__LINE__,wp_payin->F_orderno     );
	vtcp_log("[%s][%d] F_txnum       =[%.5s] ",__FILE__,__LINE__,wp_payin->F_txnum       );
	vtcp_log("[%s][%d] F_orbrno      =[%.12s]",__FILE__,__LINE__,wp_payin->F_orbrno      );
	vtcp_log("[%s][%d] F_acbrno      =[%.12s]",__FILE__,__LINE__,wp_payin->F_acbrno      );
	vtcp_log("[%s][%d] F_txamt       =[%.15s]",__FILE__,__LINE__,wp_payin->F_txamt       );
	vtcp_log("[%s][%d] F_payno       =[%.12s]",__FILE__,__LINE__,wp_payin->F_payno       );
	vtcp_log("[%s][%d] F_payqsno     =[%.12s]",__FILE__,__LINE__,wp_payin->F_payqsno     );
	vtcp_log("[%s][%d] F_payactno    =[%.32s]",__FILE__,__LINE__,wp_payin->F_payactno    );
	vtcp_log("[%s][%d] F_payname     =[%.60s]",__FILE__,__LINE__,wp_payin->F_payname     );
	vtcp_log("[%s][%d] F_payaddress  =[%.60s]",__FILE__,__LINE__,wp_payin->F_payaddress  );
	vtcp_log("[%s][%d] F_userpswd    =[%.8s]",__FILE__,__LINE__,wp_payin->F_userpswd    ); 
	vtcp_log("[%s][%d] F_cashqsno    =[%.12s]",__FILE__,__LINE__,wp_payin->F_cashqsno    );
	vtcp_log("[%s][%d] F_cashno      =[%.12s]",__FILE__,__LINE__,wp_payin->F_cashno      );
	vtcp_log("[%s][%d] F_cashactno   =[%.32s]",__FILE__,__LINE__,wp_payin->F_cashactno   );
	vtcp_log("[%s][%d] F_cashname    =[%.60s]",__FILE__,__LINE__,wp_payin->F_cashname    );
	vtcp_log("[%s][%d] F_cashaddress =[%.60s]",__FILE__,__LINE__,wp_payin->F_cashaddress );
	vtcp_log("[%s][%d] F_ywtype      =[%.12s]",__FILE__,__LINE__,wp_payin->F_ywtype      );
	vtcp_log("[%s][%d] F_content     =[%.60s]",__FILE__,__LINE__,wp_payin->F_content     );
	vtcp_log("[%s][%d] F_wtday       =[%.8s] ",__FILE__,__LINE__,wp_payin->F_wtday       );
	vtcp_log("[%s][%d] F_operlevel   =[%c] ",__FILE__,__LINE__,wp_payin->F_operlevel[0]   );
	vtcp_log("[%s][%d] F_authno      =[%.6s] ",__FILE__,__LINE__,wp_payin->F_authno      );
	vtcp_log("[%s][%d] F_authpswd    =[%.8s] ",__FILE__,__LINE__,wp_payin->F_authpswd    );   
	vtcp_log("[%s][%d] F_voctype     =[%.2s] ",__FILE__,__LINE__,wp_payin->F_voctype     );   
	vtcp_log("[%s][%d] F_vocnum      =[%.12s]",__FILE__,__LINE__,wp_payin->F_vocnum      );   
	vtcp_log("[%s][%d] F_packday     =[%.8s] ",__FILE__,__LINE__,wp_payin->F_packday     );   
	vtcp_log("[%s][%d] F_packid      =[%.8s] ",__FILE__,__LINE__,wp_payin->F_packid      );   
	vtcp_log("[%s][%d] F_feeflag     =[%c] ",__FILE__,__LINE__,wp_payin->F_feeflag[0]     );   
	vtcp_log("[%s][%d] F_feeamt      =[%.15s]",__FILE__,__LINE__,wp_payin->F_feeamt      );   
	vtcp_log("[%s][%d] F_respday     =[%.8s] ",__FILE__,__LINE__,wp_payin->F_respday     );   
	vtcp_log("[%s][%d] F_ptype       =[%.2s] ",__FILE__,__LINE__,wp_payin->F_ptype       );   
	vtcp_log("[%s][%d] F_pdate       =[%.8s] ",__FILE__,__LINE__,wp_payin->F_pdate       );   
	vtcp_log("[%s][%d] F_pnum        =[%.8s] ",__FILE__,__LINE__,wp_payin->F_pnum        );   
	vtcp_log("[%s][%d] F_ptxamt      =[%.15s]",__FILE__,__LINE__,wp_payin->F_ptxamt      );   
	vtcp_log("[%s][%d] F_jtxamt      =[%.15s]",__FILE__,__LINE__,wp_payin->F_jtxamt      );   
	vtcp_log("[%s][%d] F_owtday      =[%.8s] ",__FILE__,__LINE__,wp_payin->F_owtday      );   
	vtcp_log("[%s][%d] F_otxnum      =[%.5s] ",__FILE__,__LINE__,wp_payin->F_otxnum      );   
	vtcp_log("[%s][%d] F_oorderno    =[%.8s] ",__FILE__,__LINE__,wp_payin->F_oorderno    );   
	vtcp_log("[%s][%d] F_retcode     =[%.2s] ",__FILE__,__LINE__,wp_payin->F_retcode     );   
	vtcp_log("[%s][%d] F_rcpstat     =[%.2s] ",__FILE__,__LINE__,wp_payin->F_rcpstat     );   
	vtcp_log("[%s][%d] F_resp1       =[%.8s] ",__FILE__,__LINE__,wp_payin->F_resp1       );   
	vtcp_log("[%s][%d] F_resp2       =[%.8s] ",__FILE__,__LINE__,wp_payin->F_resp2       );   
	vtcp_log("[%s][%d] F_packend     =[%s] ",__FILE__,__LINE__,wp_payin->F_packend     );   
	vtcp_log("[%s][%d] F_lw_ind        =[%s] ",__FILE__,__LINE__,wp_payin->F_lw_ind        );   
	vtcp_log("[%s][%d] F_dfxyh       =[%.60s]",__FILE__,__LINE__,wp_payin->F_dfxyno       );  

	vtcp_log("[%s][%d]开始打印payin结构体 完毕!\n",__FILE__,__LINE__); 
	return(0);
}
int print_pay_out2(PAY_OUT2_AREA *pay_out2)
{
	return(0);
}
int print_pay_out3(PAY_OUT3_AREA *pay_out3)
{
	return(0);

}
/***********************************************************
* 得到通存通兑业务的开户行的手续费                         *
* 测试的时候暂时按照1%收取，至于如何处理需要跟相关银行协商 *
***********************************************************/
double dLvGetOpenFee(double txamt,char flag)
{
	/* 衡水收费标准 */
	return 600.0;
}
/***********************************************************
* 得到通存通兑业务的代理行的手续费                         *
* 测试的时候暂时按照1%收取，至于如何处理需要跟相关银行协商 *
***********************************************************/
double dLvGetAgentFee(double txamt,char flag)
{
	/* 衡水收费标准 */
	return 600.0;
}
#if 0
/********************************************************/
/* 函数名称: int iLvGetPrgStat(char *prgname)           */
/* 功能描述: 管理定时拼包服务的底层函数       						*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:                                             			*/
/********************************************************/
int iLvGetPrgStat(char *prgname)
{
	struct  lv_prgnm_c lv_prgnm;
	int ret = 0;

	sql_prgpid=0;
	strncpy(sql_prgname,prgname,sizeof(sql_prgname)-1);
	zip_space(sql_prgname);

	ret = Lv_prgnm_Sel(g_pub_tx.reply, &lv_prgnm, "prgname='%s'", sql_prgname);

	if (ret==1403)
	{
		vtcp_log("%s,%d,未找到指定服务sqlcode=%d\n",__FL__,ret);
		return(-1);
	}	
	if (ret!=0)
	{
		vtcp_log("%s,%d,数据库错误,sqlcode=%d\n",__FL__,ret);
		return(-1);
	}
	return(lv_prgnm.prgstat[0]);
}
/********************************************************/
/* 函数名称: int iLvGetPrgId(char *prgname)             */
/* 功能描述: 得到pid               						*/
/* 创建日期: 2005.12.2            						*/
/* 作者:     SSH                   						*/
/* 说明:                                       			*/
/********************************************************/
int iLvGetPrgId(char *prgname)
{
	struct  lv_prgnm_c lv_prgnm;
	int ret = 0;

	sql_prgpid=0;
	strncpy(sql_prgname,prgname,sizeof(sql_prgname)-1);
	zip_space(sql_prgname);

	ret = Lv_prgnm_Sel(g_pub_tx.reply, &lv_prgnm, "prgname='%s'", sql_prgname);

	if (ret==1403)
	{
		vtcp_log("%s,%d,未找到指定服务sqlcode=%d\n",__FL__,ret);
		return(-1);
	}	
	if (ret!=0)
	{
		vtcp_log("%s,%d,数据库错误,sqlcode=%d\n",__FL__,ret);
		return(-1);
	}
	return(lv_prgnm.prgpid);
}
/***************************************************************************/
/* 函数名称: iLvSetPrg                                                     */
/* 功能描述: 设置后台批量程序的状态                                        */
/* 参数:                                                                   */
/*            pcPrgName-批量程序的名称                                     */
/*            cPrgStat-批量程序的状态                                      */
/*            iPid     -批量程序的进程号                                   */
/* 返回值:   0/-1                                                          */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvSetPrg(char *pcPrgName,char cPrgStat,int iPid )
{
	char sql_prgname[33];
	char sql_prgstat[2];
	int	sql_prgpid=0;
	struct  lv_prgnm_c lv_prgnm;
	int ret = 0;

	memset(&sql_prgname,'\0',sizeof(sql_prgname));
	memset(&sql_prgstat,'\0',sizeof(sql_prgstat));
	sql_prgpid=0;
	memset(&lv_prgnm, 0x0, sizeof(lv_prgnm));

	strncpy(lv_prgnm.prgname,pcPrgName,sizeof(lv_prgnm.prgname)-1);
	ret = Lv_prgnm_Dec_Upd(g_pub_tx.reply, &lv_prgnm, "prgname = '%s'", lv_prgnm.prgname);
	if (DB_OK != ret)
	{
		vtcp_log("%s,%d,数据库错误,sqlcode=%d\n",__FL__,ret);
		return(-1);
	}
	ret = Lv_prgnm_Fet_Upd(&lv_prgnm, g_pub_tx.reply);
	if (DB_OK != ret)
	{
		vtcp_log("%s,%d,数据库错误,sqlcode=%d\n",__FL__,ret);

		return(-1);
	}
	sprintf(lv_prgnm.prgstat, "%c", cPrgStat);
	lv_prgnm.prgpid = iPid;
	ret = Lv_prgnm_Upd_Upd(lv_prgnm, g_pub_tx.reply);
	if (DB_OK != ret)
	{
		vtcp_log("%s,%d,数据库错误,sqlcode=%d\n",__FL__,ret);

		return(-1);
	}
	return(0);
}
#endif

/********************************************************/
/* 函数名称: int zipn_space(char *s,int len)            */
/* 功能描述: 压缩指定长度的字符串中的空格     						*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:                                             			*/
/********************************************************/
int zipn_space(char *s,int len)
{
	int i=0,j=0;
	i=j=0;
	/***Modified by SSH,2006.5.9,防止出现乱码****/
	while (i<len && s[i]!='\0')
	{
		if (s[i]==' ' ||s[i]==0x0a||s[i]==0x0d||s[i]=='\t')
		{
			i++;
		}else
		{
			s[j]=s[i];
			j++;
			i++;
		}
	}
	while (j<len)
	{
		s[j]='\0';
		j++;
	}
	return(0);
}
/********************************************************/
/* 函数名称: char *strtokk(char *s,char *substr)        */
/* 功能描述: 找到substr,并将子串substr替换成'\0'        */
/*    				 如果没有找到,返回原字符串          				*/
/* 创建日期: 2005.12.2               			  							*/
/* 作者:     SSH                       		  							*/
/* 说明:                                             			*/
/********************************************************/
char *strtokk(char *s,char *substr)
{
	static char *pcString;
	char *retstr;
	int i=0;
	if (s!=NULL)
		pcString=s;
	i=0;
	if (pcString[i]=='\0')
		return(pcString);
	while (pcString[i]!='\0')
	{
		if (pcString[i]!=substr[0])
			i++;
		else
			break;
	}
	if (pcString[i]=='\0')
	{
		retstr=pcString;
		pcString+=i;
	}else
	{
		retstr=pcString;
		pcString[i]='\0';
		pcString+=i+1;
	}
	return(retstr);
}
/********************************************************/
/* 函数名称: int iLvCheckStat()                         */
/* 功能描述: 增加对小额系统状态的判断                   */
/* 创建日期: 2005.12.2                                  */
/* 作者:     SSH                                        */
/* 说明:                                                */
/********************************************************/
int iLvCheckStat()
{
	int iRc=0;
	char cPtr[2];
	memset(cPtr, 0 , sizeof(cPtr));
	iRc=iLvGetSysStat(cPtr);	
	if (iRc)
	{
		vtcp_log("%s,%d,小额系统状态表错误\n",__FL__);
		return(-1);
	}
	vtcp_log("%s,%d out from pcLvGetSysStat ",__FL__);
	vtcp_log("%s,%d out from pcLvGetSysStat ptr=[%s] ",__FL__,cPtr);
	vtcp_log("%s,%d out from pcLvGetSysStat ptr=[%c] ",__FL__,cPtr[0]);
	vtcp_log("%s,%d out from pcLvGetSysStat ptr=[%c] ",__FL__,cPtr[1]);
	if (cPtr[0]!='1')
	{
		vtcp_log("%s,%d out from pcLvGetSysStat ",__FL__);
		vtcp_log("%s,%d,小额系统状态[%c]错误[%s]\n",
			__FL__,cPtr[0],cPtr);
		return(-1);
	}
	return(0);
}
/*****************************************************************************************************/
/* 函数名称: int lv_orderno_add0(char *s_string, int length, char *d_string)                         */
/* 功能描述: 用于把不足length位的cmtno补足为length位的字符串                                         */
/* 创建日期: 2006-02-15                                                                              */
/* 作者:     卿苏德                                                                                  */
/* 说明:     char *s_string ----------------- source string                                          */
/*           char *d_string ----------------- destination string                                     */
/*           int length        ----------------- d_string's length                                   */
/*****************************************************************************************************/
int lv_orderno_add0(char *s_string, int length, char *d_string)
{
	int temp_length=0;
	char cTmpSource[1024];
	memset(cTmpSource , 0 , sizeof(cTmpSource));
	if (length>sizeof(cTmpSource)) length=sizeof(cTmpSource)-1;
	memcpy(cTmpSource, s_string,length);
	zip_space(cTmpSource);
	if (strlen(cTmpSource)==0) return 0;
	if (length < 1)
	{
		fprintf(stderr, "[%s][%d]Length can't less than 1\n", __FILE__, __LINE__);
		return -1;
	}
	memset(d_string, 0, length);
	temp_length = strlen(s_string);
	if (temp_length >= length)
	{
		memcpy(d_string, s_string, length);
		return 0;
	}
	memset(d_string, '0', length - temp_length);
	/**
	strcat(d_string, s_string);
	**/
	memcpy(d_string+length - temp_length,s_string,temp_length);
	return 0;
}
/*****************************************************************************************************/
/* 函数名称: int logwrite(PAY_IN_AREA* p_pay_in)                                                     */
/* 功能描述: 将PAY_IN结构体中重要的信息写在log文件里面    		            														 */
/* 创建日期: 2006-02-25             			  																													 */
/* 作者:     卿苏德                    		  																													 */
/* 说明:     请注意修改 FileName的路径                           																			 */
/*****************************************************************************************************/
int lv_logwrite(PAY_IN_AREA* p_pay_in)
{
	vtcp_log( "\n================================================================================\n");
	vtcp_log( "F_crdb[%.1s] | ", p_pay_in->F_crdb);
	vtcp_log( "F_opcd[%.2s] | ", p_pay_in->F_opcd);
	vtcp_log( "F_pkgno[%.3s] | ", p_pay_in->F_pkgno);
	vtcp_log( "F_orderno[%.8s] | ", p_pay_in->F_orderno);
	vtcp_log( "F_txnum[%.5s] | ", p_pay_in->F_txnum);
	vtcp_log( "F_orbrno[%.12s] | ", p_pay_in->F_orbrno);
	vtcp_log( "F_acbrno[%.12s] | ", p_pay_in->F_acbrno);
	vtcp_log( "F_txamt[%.15s] | ", p_pay_in->F_txamt);	
	vtcp_log( "F_payactno[%.32s] | ", p_pay_in->F_payactno);
	vtcp_log( "F_cashactno[%.32s] | ", p_pay_in->F_cashactno);
	vtcp_log( "F_packid[%.8s] | ", p_pay_in->F_packid);
	vtcp_log( "F_feeflag[%.1s] | ", p_pay_in->F_feeflag);
	vtcp_log( "F_feeamt[%.15s] | ", p_pay_in->F_feeamt);
	vtcp_log( "F_rcpstat[%.2s] | ", p_pay_in->F_rcpstat);
	vtcp_log( "F_resp1[%.8s] | ", p_pay_in->F_resp1);
	vtcp_log( "F_resp2[%.8s] | ", p_pay_in->F_resp2);
	vtcp_log( "T_rcpstat[%.2s] | ", p_pay_in->T_rcpstat);
	vtcp_log( "T_resp1[%.8s] | ", p_pay_in->T_resp1);
	vtcp_log( "T_resp2[%.8s] | ", p_pay_in->T_resp2);
	vtcp_log( "T_stat[%.1s] | ", p_pay_in->T_stat);
	vtcp_log( "T_begstat[%.1s] | ", p_pay_in->T_begstat);
	vtcp_log( "T_ostat[%.1s] | ", p_pay_in->T_ostat);
	vtcp_log( "A_acttype[%.1s] | ", p_pay_in->A_acttype);
	vtcp_log( "A_brno[%.7s] |", p_pay_in->A_brno);
	vtcp_log( "A_wssrno[%.6s] |", p_pay_in->A_wssrno);
	vtcp_log( "A_stat[%.1s] \n", p_pay_in->A_stat);
	vtcp_log( "\n================================================================================\n");

	return 0;
}
/*****************************************************************************************************/
/* 函数名称: int iCheckHzdate(char *cashqsactno,int daycnt)                                          */
/* 功能描述: 验证回执期限                                  		            														 */
/* 创建日期: 2006-02-11             			  																													 */
/* 作者:     SSH                    		  																													   */
/* 说明:                                                        																			 */
/*****************************************************************************************************/
int iCheckHzdate(char *cashqsactno,int daycnt)
{
	struct lv_param_c wd_lv_param;
	char pcRececo[5];
	char pcSendco[5];
	int  nMaxDayCnt=0;/**最大回执天数****/
	int  nMinCityCnt=0;/***同城最小回执天数****/
	int  nMinGlobalCnt=0;/****异地最小回执天数****/
	int  iRc=0;
	nMaxDayCnt=nMinCityCnt=nMinGlobalCnt=0;
	memset(pcRececo,'\0',sizeof(pcRececo));
	memset(pcSendco,'\0',sizeof(pcSendco));
	find_sendco(cashqsactno,pcRececo,0);
	find_sendco(LV_QSBRNO,pcSendco,0);
	memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
	memcpy(wd_lv_param.par_code,"DBRTMDAY",8);

	iRc = Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "par_code = '%s' ", wd_lv_param.par_code);
	if (iRc!=DB_OK && iRc!= DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误[%d]\n",__FL__,iRc);
		return(-1);
	}
	if (iRc==100)
	{
		vtcp_log("%s,%d,未找到借记最大回执期限,取5\n",__FL__);
		nMaxDayCnt=5;
	}else
	{
		nMaxDayCnt=atoi(wd_lv_param.par_value);
	}
	if (memcmp(pcSendco,pcRececo,sizeof(pcSendco)-1)==0)
	{
		/****同城业务****/
		memcpy(wd_lv_param.par_code,"TDBRTDAY",8);

	}else
	{
		/****异地业务****/
		memcpy(wd_lv_param.par_code,"WDBRTDAY",8);
	}
	iRc = Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "par_code = '%s'", wd_lv_param.par_code);

	if (iRc)
	{
		nMinGlobalCnt=1;
	}else
	{
		nMinGlobalCnt=atoi(wd_lv_param.par_value);
	}	
	if (daycnt>nMaxDayCnt ||daycnt<nMinGlobalCnt)
	{
		vtcp_log("%s,%d,异地借记回执期限必须在%d与%d之间--%d\n",__FL__,nMinCityCnt,nMaxDayCnt,daycnt);
		sprintf(acErrMsg,"异地借记回执期限必须在%d与%d之间",nMinCityCnt,nMaxDayCnt);
		return(-1);
	}else
	{
		return(0);
	}
}
int iGetNotpaySendAmt(char * cmtno,char * wtday,char * orderno ,double *txamt)
{
	struct hv_fzf_c        sHv_fzf;
	struct lv_wbctl_c      wd_lv_wbctl;
	char cReqtype;
	char cBrno[14];
	char cWtday[9];
	memset(cWtday, 0 , sizeof(cWtday));
	memset(cBrno , 0 , sizeof(cBrno));
	memcpy(cWtday,wtday,8);
	memset(&wd_lv_wbctl,0 ,sizeof(wd_lv_wbctl));
	memset(&sHv_fzf, 0 , sizeof(sHv_fzf));
	memcpy(sHv_fzf.cmtno,   cmtno,      sizeof(sHv_fzf.cmtno)-1);
	sHv_fzf.wt_date=atoi(cWtday);
	memcpy(sHv_fzf.orderno ,orderno,    sizeof(sHv_fzf.orderno)-1);
	get_fd_data("0030", cBrno);
	if (GetOr_br_noByBr_no(cBrno, sHv_fzf.req_br_no)==NULL)
	{
		vtcp_log("[%s][%d]得到参与者号错误!",__FL__);
		sprintf(acErrMsg,"得到参与者号错误!");
		WRITEMSG
			return(-1);
	}
	if ((lvca.rtcode = Hv_fzf_Sel(g_pub_tx.reply,&sHv_fzf, 
		"cmtno = '%s' and wtday = '%s' and orderno = '%s' and reqbrno = '%s'", 
		cmtno, wtday, orderno, sHv_fzf.req_br_no)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_ctczs error [%d]\n",__FL__,lvca.rtcode);

		return(-1);
	}
	if (memcmp(cmtno,"323",3))
		cReqtype=sHv_fzf.reqtype[0];
	else
		cReqtype='1';
	if (cReqtype=='1') /* 整个包撤销 */
	{
		if ((lvca.rtcode = Lv_wbctl_Sel(g_pub_tx.reply, &wd_lv_wbctl,
			"o_pack_date = %d and opackid = '%s' ", sHv_fzf.o_pack_date, sHv_fzf.o_packid)) != DB_OK)
		{
			vtcp_log("[%s][%d]-----lv_ctczs error [%d]\n",__FL__,lvca.rtcode);

			return(-1);
		}
		*txamt=wd_lv_wbctl.totamt;
	}
	else
	{
		if ( !strcmp(sHv_fzf.o_cmtno,PKGNO_PTDJ) || !strcmp(sHv_fzf.o_cmtno,PKGNO_DQDJ)
			|| !strcmp(sHv_fzf.o_cmtno,PKGNO_DJTH) || !strcmp(sHv_fzf.o_cmtno,PKGNO_SSDJ)
			|| !strcmp(sHv_fzf.o_cmtno,PKGNO_SSJJ) || !strcmp(sHv_fzf.o_cmtno,PKGNO_PTJJ)
			|| !strcmp(sHv_fzf.o_cmtno,PKGNO_DQJJ) 
			)
		{
			struct lv_pkgreg_c lv_pkgreg;

			memset(&lv_pkgreg, 0x0, sizeof(lv_pkgreg));
			if (DB_OK != (lvca.rtcode = Lv_pkgreg_Sel(g_pub_tx.reply, &lv_pkgreg,
				"wt_date = %d and orderno = '%s' and or_br_no = '%s' ", 
				sHv_fzf.o_pack_date, sHv_fzf.o_orderno, sHv_fzf.req_br_no)))
			{
				*txamt=0;
			}else
			{
				*txamt = lv_pkgreg.tx_amt;
			}
		}
	}
	vtcp_log("[%s][%d] &&&&&&&&&&&&&&&&&&&&amt**********************====[%.0f]\n",__FL__,*txamt);
	return 0;
}
/***Add by SSH,2006.4.19,检查字符串中是否有汉字****/
/****0-没有,1-有****/
int isAnyChinese(char *s,int len)
{
	unsigned char ch;
	int i=0;
	for (i=0;i<len;i++)
	{
		ch=(unsigned char)s[i];
		if (ch>128)
		{
			return(1);
		}
	}
	return(0);
}
/*******************************************
from lv_tabcomm.c 2006-9-10 15:37
*/
/***************************************************************************/
/* 函数名称: int iLvGetOrient(PAY_IN_AREA *wp_payin)*/
/* 功能描述: 判断一笔交易是发送交易还是接收交易*/
/* 创建日期: 2005.07.20*/
/* 作者:     SSH*/
/* 说明:
int iLvGetOrient(PAY_IN_AREA *wp_payin)
{
if (memcmp(wp_payin->F_opcd,OPCD_LR_QUERY,sizeof(wp_payin->F_opcd))==0||
memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0||
memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))==0||
memcmp(wp_payin->F_opcd,OPCD_SEND_QUERY,sizeof(wp_payin->F_opcd))==0||
memcmp(wp_payin->F_opcd,OPCD_SEND,sizeof(wp_payin->F_opcd))==0||
memcmp(wp_payin->F_opcd,OPCD_RESP,sizeof(wp_payin->F_opcd))==0)
{
return(TX_SEND);
}else
{
return(TX_RECEIVE);
}
}
***************************************************************************/
static char* GETTAG(TLVPACKAGE *pack,char *name,char *value)
{
	lv_get_tag(pack,name,value); 
	return(value);
}
int iLvGETHEADTAG_DATE(TLVPACKAGE *pack,char *name,long *l_value)
{
	char cDate[9];
	memset(cDate ,0 , sizeof(cDate));
	GETHEADTAG(pack,name,cDate);
	*l_value=atoi(cDate);
	return atoi(cDate);
}
int iLv_GETTAG_DATE(TLVPACKAGE *pack,char *name,long *l_value)
{
	char cDate[9];
	memset(cDate ,0 , sizeof(cDate));
	GETTAG(pack,name,cDate);
	*l_value=atoi(cDate);
	return atoi(cDate);
}
/****************************************************************
*函数名称:iLvPayin_characterCheck                               *
*函数功能:检查支付类交易户名，地址等输入项目是否有非法字符存在  *
*参数描述: 输入：HV_PAYIN_AREA *hv_payin 大额支付类交易结构体   *
*          输出：char * errsmg           错误信息描述           *
*                                                               *
*                                                               *
*设计到表:                                                      *
*作    者:刘华锋 2006-8-26 16:20                                *
****************************************************************/
int iLvPayin_characterCheck(PAY_IN_AREA *lv_payin,char * msg)
{
	int iRet=0;
	iRet=iCharacterType(lv_payin->F_payname,sizeof(lv_payin->F_payname),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的付款人名字[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,lv_payin->F_payname,iRet);
		strncpy(msg,"输入的付款人名字中存在非法字符",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_cashname,sizeof(lv_payin->F_cashname),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的收款人名字[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,lv_payin->F_cashname,iRet);
		strncpy(msg,"输入的收款人名字中存在非法字符",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_payaddress,sizeof(lv_payin->F_payaddress),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的付款人地址[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,lv_payin->F_payaddress,iRet);
		strncpy(msg,"输入的付款人地址中存在非法字符",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_cashaddress,sizeof(lv_payin->F_cashaddress),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的收款人地址[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,lv_payin->F_cashaddress,iRet);
		strncpy(msg,"输入的收款人地址中存在非法字符",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_content,sizeof(lv_payin->F_content),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的附言中[%.60s]第%d位存在非法字符",__FILE__,__LINE__,lv_payin->F_content,iRet);
		strncpy(msg,"输入的附言中存在非法字符",50);
		return iRet;
	}
	return 0;
}
/****NEWYX BEGIN:填充空格的函数****/
int pad_field(char *pcField,int iLength)
{
	char *pcTmp;
	int  i=0;
	pcTmp=(char *)malloc(iLength+1);
	memset(pcTmp,'\0',iLength+1);
	memcpy(pcTmp,pcField,iLength);
	zip_space(pcTmp);
	strncpy(pcField,pcTmp,iLength);
	for (i=strlen(pcTmp);i<iLength;i++)
	{
		pcField[i]=' ';
	}
	free(pcTmp);
	return(0);
}
int pad_zpjl1(ZPJL1_ADD_AREA *ps)
{
	pad_field(ps->cpday,sizeof(ps->cpday));
	pad_field(ps->cpno,sizeof(ps->cpno));
	pad_field(ps->payno,sizeof(ps->payno));
	pad_field(ps->cpactno,sizeof(ps->cpactno));
	pad_field(ps->cashactno,sizeof(ps->cashactno));
	pad_field(ps->cpamt,sizeof(ps->cpamt));
	pad_field(ps->use,sizeof(ps->use));
	return(0);
}
int pad_zpjl2(ZPJL2_ADD_AREA *ps)
{
	pad_field(ps->bscnt,sizeof(ps->bscnt));
	pad_field(ps->bslist[0],sizeof(ps->bslist[0]));
	return(0);
}
int pad_tyjl1(TYJL1_ADD_AREA *ps)
{
	pad_field(ps->cpday,sizeof(ps->cpday));
	pad_field(ps->cpno,sizeof(ps->cpno));
	pad_field(ps->payno,sizeof(ps->payno));
	pad_field(ps->dlpayno,sizeof(ps->dlpayno));
	pad_field(ps->cpamt,sizeof(ps->cpamt));
	pad_field(ps->use,sizeof(ps->use));
	return(0);
}
int pad_tyjl4(TYJL4_ADD_AREA *ps)
{
	pad_field(ps->ptype,sizeof(ps->ptype));
	pad_field(ps->tsday,sizeof(ps->tsday));
	pad_field(ps->mac,sizeof(ps->mac));
	pad_field(ps->stopday,sizeof(ps->stopday));
	pad_field(ps->xyno,sizeof(ps->xyno));
	pad_field(ps->htno,sizeof(ps->htno));
	pad_field(ps->cdday,sizeof(ps->cdday));
	pad_field(ps->cdname,sizeof(ps->cdname));
	if(memcmp(ps->ptype,"05",2)!=0)/**YHBP如果不是银行本票的话，再补空格**/
	{
	    pad_field(ps->reqname,sizeof(ps->reqname));/**申请人名称**/
	    pad_field(ps->cashbrname,sizeof(ps->cashbrname));/**收款人开户行名称**/
	    pad_field(ps->content,sizeof(ps->content));/**附言**/
	}
	pad_field(ps->reqname,sizeof(ps->reqname));
	pad_field(ps->reqactno,sizeof(ps->reqactno));
	pad_field(ps->paybrname,sizeof(ps->paybrname));
	pad_field(ps->cashbrname,sizeof(ps->cashbrname));
	pad_field(ps->cpname,sizeof(ps->cpname));
	pad_field(ps->cpactno,sizeof(ps->cpactno));
	pad_field(ps->paybrname2,sizeof(ps->paybrname2));
	pad_field(ps->content,sizeof(ps->content));
	return(0);
}
int print_tyjl4(TYJL4_ADD_AREA *ps)
{
	printf("GETHERE[%.*s]\n",sizeof(ps->ptype),ps->ptype);
	printf("GETHERE[%.*s]\n",sizeof(ps->tsday),ps->tsday);
	printf("GETHERE[%.*s]\n",sizeof(ps->mac),ps->mac);
	printf("GETHERE[%.*s]\n",sizeof(ps->stopday),ps->stopday);
	printf("GETHERE[%.*s]\n",sizeof(ps->xyno),ps->xyno);
	printf("GETHERE[%.*s]\n",sizeof(ps->htno),ps->htno);
	printf("GETHERE[%.*s]\n",sizeof(ps->cdday),ps->cdday);
	printf("GETHERE[%.*s]\n",sizeof(ps->cdname),ps->cdname);
	printf("GETHERE[%.*s]\n",sizeof(ps->reqname),ps->reqname);
	printf("GETHERE[%.*s]\n",sizeof(ps->reqactno),ps->reqactno);
	printf("GETHERE[%.*s]\n",sizeof(ps->paybrname),ps->paybrname);
	printf("GETHERE[%.*s]\n",sizeof(ps->cashbrname),ps->cashbrname);
	printf("GETHERE[%.*s]\n",sizeof(ps->cpname),ps->cpname);
	printf("GETHERE[%.*s]\n",sizeof(ps->cpactno),ps->cpactno);
	printf("GETHERE[%.*s]\n",sizeof(ps->paybrname2),ps->paybrname2);
	printf("GETHERE[%.*s]\n",sizeof(ps->content),ps->content);
	return(0);
}
/****NEWYX END:填充空格的函数****/


void lv_zip_payinF_space(PAY_IN_AREA * wp_payin)
{
	char    cF_crdb       [1 +1];
	char    cF_opcd       [2 +1];
	char    cF_pkgno      [3 +1];
	char    cF_orderno    [8 +1];
	char    cF_txnum      [5 +1];
	char    cF_orbrno     [12+1];
	char    cF_acbrno     [12+1];
	char    cF_txamt      [15+1];
	char    cF_payno      [12+1];
	char    cF_payqsno    [12+1];
	char    cF_payactno   [32+1];
	char    cF_payname    [60+1];
	char    cF_payaddress [60+1];
	char    cF_userpswd   [8 +1];
	char    cF_cashqsno   [12+1];
	char    cF_cashno     [12+1];
	char    cF_cashactno  [32+1];
	char    cF_cashname   [60+1];
	char    cF_cashaddress[60+1];
	char    cF_ywtype     [12+1];
	char    cF_content    [60+1];
	char    cF_wtday      [8+1] ;
	char    cF_operlevel  [1+1] ;
	char    cF_authno     [TLRLENGTH+1];
	char    cF_authpswd   [8+1] ;
	char    cF_voctype    [2+1] ;
	char    cF_vocnum     [12+1];
	char    cF_packday    [8 +1];
	char    cF_packid     [8 +1];
	char    cF_feeflag    [1 +1];
	char    cF_feeamt     [15+1];
	char    cF_respday    [8 +1];
	char    cF_ptype      [2 +1];
	char    cF_pdate      [8 +1];
	char    cF_pnum       [8 +1];
	char    cF_ptxamt     [15+1];
	char    cF_jtxamt     [15+1];
	char    cF_owtday     [8 +1];
	char    cF_otxnum     [5 +1];
	char    cF_oorderno   [8 +1];
	char    cF_retcode    [2 +1];
	char    cF_rcpstat    [2 +1];
	char    cF_resp1      [8 +1];
	char    cF_resp2      [8 +1];
	char    cF_packend    [1 +1];
	char    cF_lvsts      [1 +1];
	char    cF_lw_ind     [1 +1];
	char    cF_dfxyno     [60+1];
	char    cF_o_or_br_no [12+1];
	char    cF_o_ac_br_no [12+1];
	char    cF_actype     [1 +1];
	char    cF_pswdtype   [1 +1];
	char    cF_pswd       [8 +1];
	char    cF_payunit    [60+1];
	char    cF_payterm    [16+1];
	char    cF_paytype    [2 +1];

	memset(cF_crdb       , 0 , sizeof(cF_crdb       ));
	memset(cF_opcd       , 0 , sizeof(cF_opcd       ));
	memset(cF_pkgno      , 0 , sizeof(cF_pkgno      ));
	memset(cF_orderno    , 0 , sizeof(cF_orderno    ));
	memset(cF_txnum      , 0 , sizeof(cF_txnum      ));
	memset(cF_orbrno     , 0 , sizeof(cF_orbrno     ));
	memset(cF_acbrno     , 0 , sizeof(cF_acbrno     ));
	memset(cF_txamt      , 0 , sizeof(cF_txamt      ));
	memset(cF_payno      , 0 , sizeof(cF_payno      ));
	memset(cF_payqsno    , 0 , sizeof(cF_payqsno    ));
	memset(cF_payactno   , 0 , sizeof(cF_payactno   ));
	memset(cF_payname    , 0 , sizeof(cF_payname    ));
	memset(cF_payaddress , 0 , sizeof(cF_payaddress ));
	memset(cF_userpswd   , 0 , sizeof(cF_userpswd   ));
	memset(cF_cashqsno   , 0 , sizeof(cF_cashqsno   ));
	memset(cF_cashno     , 0 , sizeof(cF_cashno     ));
	memset(cF_cashactno  , 0 , sizeof(cF_cashactno  ));
	memset(cF_cashname   , 0 , sizeof(cF_cashname   ));
	memset(cF_cashaddress, 0 , sizeof(cF_cashaddress));
	memset(cF_ywtype     , 0 , sizeof(cF_ywtype     ));
	memset(cF_content    , 0 , sizeof(cF_content    ));
	memset(cF_wtday      , 0 , sizeof(cF_wtday      ));
	memset(cF_operlevel  , 0 , sizeof(cF_operlevel  ));
	memset(cF_authno     , 0 , sizeof(cF_authno     ));
	memset(cF_authpswd   , 0 , sizeof(cF_authpswd   ));
	memset(cF_voctype    , 0 , sizeof(cF_voctype    ));
	memset(cF_vocnum     , 0 , sizeof(cF_vocnum     ));
	memset(cF_packday    , 0 , sizeof(cF_packday    ));
	memset(cF_packid     , 0 , sizeof(cF_packid     ));
	memset(cF_feeflag    , 0 , sizeof(cF_feeflag    ));
	memset(cF_feeamt     , 0 , sizeof(cF_feeamt     ));
	memset(cF_respday    , 0 , sizeof(cF_respday    ));
	memset(cF_ptype      , 0 , sizeof(cF_ptype      ));
	memset(cF_pdate      , 0 , sizeof(cF_pdate      ));
	memset(cF_pnum       , 0 , sizeof(cF_pnum       ));
	memset(cF_ptxamt     , 0 , sizeof(cF_ptxamt     ));
	memset(cF_jtxamt     , 0 , sizeof(cF_jtxamt     ));
	memset(cF_owtday     , 0 , sizeof(cF_owtday     ));
	memset(cF_otxnum     , 0 , sizeof(cF_otxnum     ));
	memset(cF_oorderno   , 0 , sizeof(cF_oorderno   ));
	memset(cF_retcode    , 0 , sizeof(cF_retcode    ));
	memset(cF_rcpstat    , 0 , sizeof(cF_rcpstat    ));
	memset(cF_resp1      , 0 , sizeof(cF_resp1      ));
	memset(cF_resp2      , 0 , sizeof(cF_resp2      ));
	memset(cF_packend    , 0 , sizeof(cF_packend    ));
	memset(cF_lvsts      , 0 , sizeof(cF_lvsts      ));
	memset(cF_lw_ind     , 0 , sizeof(cF_lw_ind     ));
	memset(cF_dfxyno     , 0 , sizeof(cF_dfxyno     ));
	memset(cF_o_or_br_no , 0 , sizeof(cF_o_or_br_no ));
	memset(cF_o_ac_br_no , 0 , sizeof(cF_o_ac_br_no ));
	memset(cF_actype     , 0 , sizeof(cF_actype     ));
	memset(cF_pswdtype   , 0 , sizeof(cF_pswdtype   ));
	memset(cF_pswd       , 0 , sizeof(cF_pswd       ));
	memset(cF_payunit    , 0 , sizeof(cF_payunit    ));
	memset(cF_payterm    , 0 , sizeof(cF_payterm    ));
	memset(cF_paytype    , 0 , sizeof(cF_paytype    ));

	memcpy(cF_crdb       , wp_payin->F_crdb        , sizeof(cF_crdb       )-1);
	memcpy(cF_opcd       , wp_payin->F_opcd        , sizeof(cF_opcd       )-1);
	memcpy(cF_pkgno      , wp_payin->F_pkgno       , sizeof(cF_pkgno      )-1);
	memcpy(cF_orderno    , wp_payin->F_orderno     , sizeof(cF_orderno    )-1);
	memcpy(cF_txnum      , wp_payin->F_txnum       , sizeof(cF_txnum      )-1);
	memcpy(cF_orbrno     , wp_payin->F_orbrno      , sizeof(cF_orbrno     )-1);
	memcpy(cF_acbrno     , wp_payin->F_acbrno      , sizeof(cF_acbrno     )-1);
	memcpy(cF_txamt      , wp_payin->F_txamt       , sizeof(cF_txamt      )-1);
	memcpy(cF_payno      , wp_payin->F_payno       , sizeof(cF_payno      )-1);
	memcpy(cF_payqsno    , wp_payin->F_payqsno     , sizeof(cF_payqsno    )-1);
	memcpy(cF_payactno   , wp_payin->F_payactno    , sizeof(cF_payactno   )-1);
	memcpy(cF_payname    , wp_payin->F_payname     , sizeof(cF_payname    )-1);
	memcpy(cF_payaddress , wp_payin->F_payaddress  , sizeof(cF_payaddress )-1);
	memcpy(cF_userpswd   , wp_payin->F_userpswd    , sizeof(cF_userpswd   )-1);
	memcpy(cF_cashqsno   , wp_payin->F_cashqsno    , sizeof(cF_cashqsno   )-1);
	memcpy(cF_cashno     , wp_payin->F_cashno      , sizeof(cF_cashno     )-1);
	memcpy(cF_cashactno  , wp_payin->F_cashactno   , sizeof(cF_cashactno  )-1);
	memcpy(cF_cashname   , wp_payin->F_cashname    , sizeof(cF_cashname   )-1);
	memcpy(cF_cashaddress, wp_payin->F_cashaddress , sizeof(cF_cashaddress)-1);
	memcpy(cF_ywtype     , wp_payin->F_ywtype      , sizeof(cF_ywtype     )-1);
	memcpy(cF_content    , wp_payin->F_content     , sizeof(cF_content    )-1);
	memcpy(cF_wtday      , wp_payin->F_wtday       , sizeof(cF_wtday      )-1);
	memcpy(cF_operlevel  , wp_payin->F_operlevel   , sizeof(cF_operlevel  )-1);
	memcpy(cF_authno     , wp_payin->F_authno      , sizeof(cF_authno     )-1);
	memcpy(cF_authpswd   , wp_payin->F_authpswd    , sizeof(cF_authpswd   )-1);
	memcpy(cF_voctype    , wp_payin->F_voctype     , sizeof(cF_voctype    )-1);
	memcpy(cF_vocnum     , wp_payin->F_vocnum      , sizeof(cF_vocnum     )-1);
	memcpy(cF_packday    , wp_payin->F_packday     , sizeof(cF_packday    )-1);
	memcpy(cF_packid     , wp_payin->F_packid      , sizeof(cF_packid     )-1);
	memcpy(cF_feeflag    , wp_payin->F_feeflag     , sizeof(cF_feeflag    )-1);
	memcpy(cF_feeamt     , wp_payin->F_feeamt      , sizeof(cF_feeamt     )-1);
	memcpy(cF_respday    , wp_payin->F_respday     , sizeof(cF_respday    )-1);
	memcpy(cF_ptype      , wp_payin->F_ptype       , sizeof(cF_ptype      )-1);
	memcpy(cF_pdate      , wp_payin->F_pdate       , sizeof(cF_pdate      )-1);
	memcpy(cF_pnum       , wp_payin->F_pnum        , sizeof(cF_pnum       )-1);
	memcpy(cF_ptxamt     , wp_payin->F_ptxamt      , sizeof(cF_ptxamt     )-1);
	memcpy(cF_jtxamt     , wp_payin->F_jtxamt      , sizeof(cF_jtxamt     )-1);
	memcpy(cF_owtday     , wp_payin->F_owtday      , sizeof(cF_owtday     )-1);
	memcpy(cF_otxnum     , wp_payin->F_otxnum      , sizeof(cF_otxnum     )-1);
	memcpy(cF_oorderno   , wp_payin->F_oorderno    , sizeof(cF_oorderno   )-1);
	memcpy(cF_retcode    , wp_payin->F_retcode     , sizeof(cF_retcode    )-1);
	memcpy(cF_rcpstat    , wp_payin->F_rcpstat     , sizeof(cF_rcpstat    )-1);
	memcpy(cF_resp1      , wp_payin->F_resp1       , sizeof(cF_resp1      )-1);
	memcpy(cF_resp2      , wp_payin->F_resp2       , sizeof(cF_resp2      )-1);
	memcpy(cF_packend    , wp_payin->F_packend     , sizeof(cF_packend    )-1);
	memcpy(cF_lvsts      , wp_payin->F_lvsts       , sizeof(cF_lvsts      )-1);
	memcpy(cF_lw_ind     , wp_payin->F_lw_ind      , sizeof(cF_lw_ind     )-1);
	memcpy(cF_dfxyno     , wp_payin->F_dfxyno      , sizeof(cF_dfxyno     )-1);
	memcpy(cF_o_or_br_no , wp_payin->F_o_or_br_no  , sizeof(cF_o_or_br_no )-1);
	memcpy(cF_o_ac_br_no , wp_payin->F_o_ac_br_no  , sizeof(cF_o_ac_br_no )-1);
	memcpy(cF_actype     , wp_payin->F_actype      , sizeof(cF_actype     )-1);
	memcpy(cF_pswdtype   , wp_payin->F_pswdtype    , sizeof(cF_pswdtype   )-1);
	memcpy(cF_pswd       , wp_payin->F_pswd        , sizeof(cF_pswd       )-1);
	memcpy(cF_payunit    , wp_payin->F_payunit     , sizeof(cF_payunit    )-1);
	memcpy(cF_payterm    , wp_payin->F_payterm     , sizeof(cF_payterm    )-1);
	memcpy(cF_paytype    , wp_payin->F_paytype     , sizeof(cF_paytype    )-1);

	pub_base_strpack(cF_crdb       );
	pub_base_strpack(cF_opcd       );
	pub_base_strpack(cF_pkgno      );
	pub_base_strpack(cF_orderno    );
	pub_base_strpack(cF_txnum      );
	pub_base_strpack(cF_orbrno     );
	pub_base_strpack(cF_acbrno     );
	pub_base_strpack(cF_txamt      );
	pub_base_strpack(cF_payno      );
	pub_base_strpack(cF_payqsno    );
	pub_base_strpack(cF_payactno   );
	pub_base_strpack(cF_payname    );
	pub_base_strpack(cF_payaddress );
	pub_base_strpack(cF_userpswd   );
	pub_base_strpack(cF_cashqsno   );
	pub_base_strpack(cF_cashno     );
	pub_base_strpack(cF_cashactno  );
	pub_base_strpack(cF_cashname   );
	pub_base_strpack(cF_cashaddress);
	pub_base_strpack(cF_ywtype     );
	pub_base_strpack(cF_content    );
	pub_base_strpack(cF_wtday      );
	pub_base_strpack(cF_operlevel  );
	pub_base_strpack(cF_authno     );
	pub_base_strpack(cF_authpswd   );
	pub_base_strpack(cF_voctype    );
	pub_base_strpack(cF_vocnum     );
	pub_base_strpack(cF_packday    );
	pub_base_strpack(cF_packid     );
	pub_base_strpack(cF_feeflag    );
	pub_base_strpack(cF_feeamt     );
	pub_base_strpack(cF_respday    );
	pub_base_strpack(cF_ptype      );
	pub_base_strpack(cF_pdate      );
	pub_base_strpack(cF_pnum       );
	pub_base_strpack(cF_ptxamt     );
	pub_base_strpack(cF_jtxamt     );
	pub_base_strpack(cF_owtday     );
	pub_base_strpack(cF_otxnum     );
	pub_base_strpack(cF_oorderno   );
	pub_base_strpack(cF_retcode    );
	pub_base_strpack(cF_rcpstat    );
	pub_base_strpack(cF_resp1      );
	pub_base_strpack(cF_resp2      );
	pub_base_strpack(cF_packend    );
	pub_base_strpack(cF_lvsts      );
	pub_base_strpack(cF_lw_ind     );
	pub_base_strpack(cF_dfxyno     );
	pub_base_strpack(cF_o_or_br_no );
	pub_base_strpack(cF_o_ac_br_no );
	pub_base_strpack(cF_actype     );
	pub_base_strpack(cF_pswdtype   );
	pub_base_strpack(cF_pswd       );
	pub_base_strpack(cF_payunit    );
	pub_base_strpack(cF_payterm    );
	pub_base_strpack(cF_paytype    );

	memcpy(wp_payin->F_crdb        ,cF_crdb       ,  sizeof(wp_payin->F_crdb        ));
	memcpy(wp_payin->F_opcd        ,cF_opcd       ,  sizeof(wp_payin->F_opcd        ));
	memcpy(wp_payin->F_pkgno       ,cF_pkgno      ,  sizeof(wp_payin->F_pkgno       ));
	memcpy(wp_payin->F_orderno     ,cF_orderno    ,  sizeof(wp_payin->F_orderno     ));
	memcpy(wp_payin->F_txnum       ,cF_txnum      ,  sizeof(wp_payin->F_txnum       ));
	memcpy(wp_payin->F_orbrno      ,cF_orbrno     ,  sizeof(wp_payin->F_orbrno      ));
	memcpy(wp_payin->F_acbrno      ,cF_acbrno     ,  sizeof(wp_payin->F_acbrno      ));
	memcpy(wp_payin->F_txamt       ,cF_txamt      ,  sizeof(wp_payin->F_txamt       ));
	memcpy(wp_payin->F_payno       ,cF_payno      ,  sizeof(wp_payin->F_payno       ));
	memcpy(wp_payin->F_payqsno     ,cF_payqsno    ,  sizeof(wp_payin->F_payqsno     ));
	memcpy(wp_payin->F_payactno    ,cF_payactno   ,  sizeof(wp_payin->F_payactno    ));
	memcpy(wp_payin->F_payname     ,cF_payname    ,  sizeof(wp_payin->F_payname     ));
	memcpy(wp_payin->F_payaddress  ,cF_payaddress ,  sizeof(wp_payin->F_payaddress  ));
	memcpy(wp_payin->F_userpswd    ,cF_userpswd   ,  sizeof(wp_payin->F_userpswd    ));
	memcpy(wp_payin->F_cashqsno    ,cF_cashqsno   ,  sizeof(wp_payin->F_cashqsno    ));
	memcpy(wp_payin->F_cashno      ,cF_cashno     ,  sizeof(wp_payin->F_cashno      ));
	memcpy(wp_payin->F_cashactno   ,cF_cashactno  ,  sizeof(wp_payin->F_cashactno   ));
	memcpy(wp_payin->F_cashname    ,cF_cashname   ,  sizeof(wp_payin->F_cashname    ));
	memcpy(wp_payin->F_cashaddress ,cF_cashaddress,  sizeof(wp_payin->F_cashaddress ));
	memcpy(wp_payin->F_ywtype      ,cF_ywtype     ,  sizeof(wp_payin->F_ywtype      ));
	memcpy(wp_payin->F_content     ,cF_content    ,  sizeof(wp_payin->F_content     ));
	memcpy(wp_payin->F_wtday       ,cF_wtday      ,  sizeof(wp_payin->F_wtday       ));
	memcpy(wp_payin->F_operlevel   ,cF_operlevel  ,  sizeof(wp_payin->F_operlevel   ));
	memcpy(wp_payin->F_authno      ,cF_authno     ,  sizeof(wp_payin->F_authno      ));
	memcpy(wp_payin->F_authpswd    ,cF_authpswd   ,  sizeof(wp_payin->F_authpswd    ));
	memcpy(wp_payin->F_voctype     ,cF_voctype    ,  sizeof(wp_payin->F_voctype     ));
	memcpy(wp_payin->F_vocnum      ,cF_vocnum     ,  sizeof(wp_payin->F_vocnum      ));
	memcpy(wp_payin->F_packday     ,cF_packday    ,  sizeof(wp_payin->F_packday     ));
	memcpy(wp_payin->F_packid      ,cF_packid     ,  sizeof(wp_payin->F_packid      ));
	memcpy(wp_payin->F_feeflag     ,cF_feeflag    ,  sizeof(wp_payin->F_feeflag     ));
	memcpy(wp_payin->F_feeamt      ,cF_feeamt     ,  sizeof(wp_payin->F_feeamt      ));
	memcpy(wp_payin->F_respday     ,cF_respday    ,  sizeof(wp_payin->F_respday     ));
	memcpy(wp_payin->F_ptype       ,cF_ptype      ,  sizeof(wp_payin->F_ptype       ));
	memcpy(wp_payin->F_pdate       ,cF_pdate      ,  sizeof(wp_payin->F_pdate       ));
	memcpy(wp_payin->F_pnum        ,cF_pnum       ,  sizeof(wp_payin->F_pnum        ));
	memcpy(wp_payin->F_ptxamt      ,cF_ptxamt     ,  sizeof(wp_payin->F_ptxamt      ));
	memcpy(wp_payin->F_jtxamt      ,cF_jtxamt     ,  sizeof(wp_payin->F_jtxamt      ));
	memcpy(wp_payin->F_owtday      ,cF_owtday     ,  sizeof(wp_payin->F_owtday      ));
	memcpy(wp_payin->F_otxnum      ,cF_otxnum     ,  sizeof(wp_payin->F_otxnum      ));
	memcpy(wp_payin->F_oorderno    ,cF_oorderno   ,  sizeof(wp_payin->F_oorderno    ));
	memcpy(wp_payin->F_retcode     ,cF_retcode    ,  sizeof(wp_payin->F_retcode     ));
	memcpy(wp_payin->F_rcpstat     ,cF_rcpstat    ,  sizeof(wp_payin->F_rcpstat     ));
	memcpy(wp_payin->F_resp1       ,cF_resp1      ,  sizeof(wp_payin->F_resp1       ));
	memcpy(wp_payin->F_resp2       ,cF_resp2      ,  sizeof(wp_payin->F_resp2       ));
	memcpy(wp_payin->F_packend     ,cF_packend    ,  sizeof(wp_payin->F_packend     ));
	memcpy(wp_payin->F_lvsts       ,cF_lvsts      ,  sizeof(wp_payin->F_lvsts       ));
	memcpy(wp_payin->F_lw_ind      ,cF_lw_ind     ,  sizeof(wp_payin->F_lw_ind      ));
	memcpy(wp_payin->F_dfxyno      ,cF_dfxyno     ,  sizeof(wp_payin->F_dfxyno      ));
	memcpy(wp_payin->F_o_or_br_no  ,cF_o_or_br_no ,  sizeof(wp_payin->F_o_or_br_no  ));
	memcpy(wp_payin->F_o_ac_br_no  ,cF_o_ac_br_no ,  sizeof(wp_payin->F_o_ac_br_no  ));
	memcpy(wp_payin->F_actype      ,cF_actype     ,  sizeof(wp_payin->F_actype      ));
	memcpy(wp_payin->F_pswdtype    ,cF_pswdtype   ,  sizeof(wp_payin->F_pswdtype    ));
	memcpy(wp_payin->F_pswd        ,cF_pswd       ,  sizeof(wp_payin->F_pswd        ));
	memcpy(wp_payin->F_payunit     ,cF_payunit    ,  sizeof(wp_payin->F_payunit     ));
	memcpy(wp_payin->F_payterm     ,cF_payterm    ,  sizeof(wp_payin->F_payterm     ));
	memcpy(wp_payin->F_paytype     ,cF_paytype    ,  sizeof(wp_payin->F_paytype     ));
}
/****Add by SSH,2007.5.15,发送支票业务报文到验印服务器*****/
/****Add by SSH,2007.5.15,发送支票业务报文到验印服务器*****/
int SendZpCifToYy(PAY_IN_AREA *wp_payin)
{
	ZPJL1_ADD_AREA wd_zp_add1;
	ZPJL2_ADD_AREA wd_zp_add2;
	ZPJL3_ADD_AREA wd_zp_add3;
	TYJL1_ADD_AREA wd_ty_add1;
	TYJL2_ADD_AREA wd_ty_add2;
	TYJL3_ADD_AREA wd_ty_add3;
	TYJL4_ADD_AREA wd_ty_add4;
	TYJL5_ADD_AREA wd_ty_add5;
	char pcYzjBuffer[1024*10];
	int  nYzjLength=0;
	int iRc;
	TCIFPACK *pack, cifpack;
	pack = &cifpack;  /***报文结构****/
	char pcFileName[256];
	int tmp_orderno; 
	char cifip[32];
	char tmpbuf[19];
	int  cifport;
	char tmp_port[32];
	int  iSocketID;
	char status[4];
	memset(status,0,4);
	memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
	memset(&wd_zp_add2,'\0',sizeof(wd_zp_add2));
	memset(&wd_zp_add3,'\0',sizeof(wd_zp_add3));
	memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
	memset(&wd_ty_add3,'\0',sizeof(wd_ty_add3));
	memset(&wd_ty_add4,'\0',sizeof(wd_ty_add4));
	memset(&wd_ty_add5,'\0',sizeof(wd_ty_add5));
	memset(tmpbuf,'\0',sizeof(tmpbuf));
	memset(pcYzjBuffer,'\0',sizeof(pcYzjBuffer));
	if(memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))==0){
		iRc=iLvGetAddit(wp_payin,"001",(char *)&wd_zp_add1,sizeof(wd_zp_add1));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域1失败\n",__FL__);
			sprintf(acErrMsg,"取附加域1失败!\n");
			return(-1);
		}
		vtcp_log("%s,%d,Gethere,pack=[%s]\n",__FILE__,__LINE__,&wd_zp_add1);
		pad_zpjl1(&wd_zp_add1);
		iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域2失败\n",__FL__);
			sprintf(acErrMsg,"取附加域2失败!\n");
			return(-1);
		}
		pad_zpjl2(&wd_zp_add2);
		iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_zp_add3,sizeof(wd_zp_add3));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域3失败\n",__FL__);
			sprintf(acErrMsg,"取附加域3失败!\n");
			return(-1);
		}
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0){
		iRc=iLvGetAddit(wp_payin,"001",(char *)&wd_ty_add1,sizeof(wd_ty_add1));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域1失败\n",__FL__);
			sprintf(acErrMsg,"取附加域1失败!\n");
			return(-1);
		}
		iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域2失败\n",__FL__);
			sprintf(acErrMsg,"取附加域2失败!\n");
			return(-1);
		}
		pad_zpjl2(&wd_zp_add2);
		iRc=iLvGetAddit(wp_payin,"004",(char *)&wd_ty_add4,sizeof(wd_ty_add4));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域4失败\n",__FL__);
			sprintf(acErrMsg,"取附加域4失败!\n");
			return(-1);
		}
		pad_tyjl1(&wd_ty_add1);
		pad_tyjl4(&wd_ty_add4);

		iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_ty_add3,sizeof(wd_ty_add3));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域3失败\n",__FL__);
			sprintf(acErrMsg,"取附加域3失败!\n");
			return(-1);
		}
		/****pad_tyjl3(&wd_ty_add3);*******/
		iRc=iLvGetAddit(wp_payin,"005",(char *)&wd_ty_add5,sizeof(wd_ty_add5));
		if(iRc){
			vtcp_log("[%s][%d] 取附加域5失败\n",__FL__);
			sprintf(acErrMsg,"取附加域5失败!\n");
			return(-1);
		}
	}
	/************构造cif报文******************/
	vInitCifPack(pack);
#define ADDCIFTAG(pack,tag,data) {	char *p; \
									p=malloc(sizeof(data)+1); \
									memcpy(p,data,sizeof(data)); \
									p[sizeof(data)]='\0'; \
									vAddCifTag(pack,tag,p); \
									free(p); \
								}
	/***********公共要素**********/
 	ADDCIFTAG(pack,"30A",wp_payin->T_wtday);   
	ADDCIFTAG(pack,"0BC",wp_payin->T_orderno);
	ADDCIFTAG(pack,"CC5",wp_payin->T_cashno);
	ADDCIFTAG(pack,"D21",wp_payin->T_payno);
	pad_field(wp_payin->T_payactno,sizeof(wp_payin->T_payactno));
	ADDCIFTAG(pack,"D23",wp_payin->T_payactno);   
	pad_field(wp_payin->T_payactno,sizeof(wp_payin->T_payactno));
	memset(tmpbuf,'\0',sizeof(tmpbuf));
	memcpy(tmpbuf,"RMB",3);
	memcpy(tmpbuf+3,wp_payin->T_txamt,15);
	vAddCifTag(pack,"32A",tmpbuf);

	ADDCIFTAG(pack,"D22",wp_payin->T_payname);
	pad_field(wp_payin->T_cashactno,sizeof(wp_payin->T_cashactno));
	ADDCIFTAG(pack,"59C",wp_payin->T_cashactno);
	ADDCIFTAG(pack,"59A",wp_payin->T_cashname);    /******付款人名称******/
	ADDCIFTAG(pack,"BS6",wp_payin->T_respday);     /***回执期限***/
	ADDCIFTAG(pack,"TXC","0100"); 		/***新添加的附加域,交易代码(4)*****/
	ADDCIFTAG(pack,"DA0","00");                /***核签状态****/
	if(memcmp(wp_payin->T_txnum,"30102",5)==0){
	 /*******支票截留业务要素*********/
		char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
		int  nBsrCnt=0;
		int  tmpdate=0,tmpdate1=0;
		memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
		memcpy(pack->head.cifno,"100",3);
		ADDCIFTAG(pack,"D35",wd_zp_add1.cpday);
		ADDCIFTAG(pack,"D01",wd_zp_add1.cpno);
		ADDCIFTAG(pack,"D37",wd_zp_add1.use);     /******用途****/
		ADDCIFTAG(pack,"D41",wd_zp_add2.bscnt);
		nBsrCnt=apatoi(wd_zp_add2.bscnt,2);
		if(nBsrCnt>0){
			memcpy(pcBsrBuffer,(char *)wd_zp_add2.bslist,nBsrCnt*60);
			vAddCifTag(pack,"D42",pcBsrBuffer);
		}
		ADDCIFTAG(pack,"D30",wd_zp_add3.bscheckpswd); 
		/**modify wyw 20070618  YZJ要求提示付款日期,要大于等于小额系统系统日期,现在提示付款日期取出票日期加10**/
		tmpdate=apatoi(wd_zp_add1.cpday,8);
		vtcp_log("%s,%d 提示付款日期:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
		pub_base_deadlineD(tmpdate,9,&tmpdate1);
		vtcp_log("%s,%d 提示付款日期:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
		memset(tmpbuf,'\0',sizeof(tmpbuf));
		apitoa(tmpdate1,8,tmpbuf);
		vtcp_log("%s,%d 提示付款日期:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
		vAddCifTag(pack,"D36",tmpbuf);	
	 }else{
		/********通用截留业务要素*********/
		char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
		int  nBsrCnt=0;
		memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
		memcpy(pack->head.cifno,"101",3);
		ADDCIFTAG(pack,"D35",wd_ty_add1.cpday);
		ADDCIFTAG(pack,"D01",wd_ty_add1.cpno);
		ADDCIFTAG(pack,"D37",wd_ty_add1.use);     /******用途****/
		ADDCIFTAG(pack,"D41",wd_zp_add2.bscnt);
		nBsrCnt=apatoi(wd_zp_add2.bscnt,2);
		if(nBsrCnt>0){
			memcpy(pcBsrBuffer,(char *)wd_zp_add2.bslist,nBsrCnt*60);
			vAddCifTag(pack,"D42",pcBsrBuffer);
		}
		ADDCIFTAG(pack,"D32",wd_ty_add4.ptype);
		ADDCIFTAG(pack,"D36",wd_ty_add4.tsday);
		ADDCIFTAG(pack,"D51",wd_ty_add4.mac);
		ADDCIFTAG(pack,"D61",wd_ty_add4.stopday);
		ADDCIFTAG(pack,"D62",wd_ty_add4.xyno);
		ADDCIFTAG(pack,"D63",wd_ty_add4.htno);
		ADDCIFTAG(pack,"D64",wd_ty_add4.cdday);
		ADDCIFTAG(pack,"D66",wd_ty_add4.cdname);
	}
	vtcp_log("[%s][%d]构造完毕的cis报文[%d]:\n",__FILE__,__LINE__,strlen((char *)pack));
	vtcp_log("%s\n",(char *)pack);
	/**********构造CIF报文完毕*******/
	/****Add by SSH,2007.5.15,转换标准cis格式为银之杰格式****/
	vCif2yzj(pack,pcYzjBuffer,wp_payin->T_inwssrno);
	nYzjLength=strlen(pcYzjBuffer);
	vtcp_log("[%s][%d]转换之后的银之杰报文[%d]:\n",__FILE__,__LINE__,nYzjLength);
	vtcp_log("%s\n",(char *)pcYzjBuffer);
	/********************************************************/	

	memset(pcFileName,'\0',sizeof(pcFileName));
	sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
	memset(cifip,'\0',sizeof(cifip));
	/*******读取验印服务器的IP和端口号*******/
 
	iRc=hv_readIni(pcFileName,"CIFSERVER","Address",cifip);
	if(iRc){
		vtcp_log("[%s][%d] 读取配置文件错误\n",__FL__);
		sprintf(acErrMsg,"读取配置文件错误!\n");
		return(-1);
	}
	vtcp_log("[%s][%d]CIF IP=[%s]\n",__FILE__,__LINE__,cifip);
	memset(tmp_port,'\0',sizeof(tmp_port));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Port",tmp_port);
	if(iRc){
		vtcp_log("[%s][%d] 读取配置文件错误\n",__FL__);
		sprintf(acErrMsg,"读取配置文件错误!\n");
		return(-1);
	}
	cifport=apatoi(tmp_port,strlen(tmp_port));
	vtcp_log("[%s][%d]CIF PORT=[%s]\n",__FILE__,__LINE__,tmp_port);
	/*******连接验印服务器********/
	iSocketID=Cif_connect(cifip,cifport);
	if(iSocketID<=0){
		vtcp_log("[%s][%d] 连接验印服务器失败!\n",__FL__);
		sprintf(acErrMsg,"连接验印服务器失败!\n");
		return(-1);
	}
	vtcp_log("socket=%d\n", iSocketID);
	/*******发送CIF报文到验印服务器****/
	vtcp_log("Try to send[%d]...\n",nYzjLength);
	iRc=Cif_send(iSocketID,pcYzjBuffer,nYzjLength);
	if(iRc<0){
		vtcp_log("[%s][%d] 发送到验印服务器失败!\n",__FL__);
		sprintf(acErrMsg,"发送到验印服务器失败!\n");
		Cif_close(iSocketID);
		return(-1);
	}
	vtcp_log("Send finished,try to recv...\n");
	iRc=Cif_recv(iSocketID,pcYzjBuffer,8);
	if(iRc<0){
		vtcp_log("iRc =%d\n",iRc);
		vtcp_log("[%s][%d] 接收验印服务器失败!\n",__FL__);
		sprintf(acErrMsg,"接收验印服务器失败!\n");
		Cif_close(iSocketID);
		return(-1) ;
	}
	vtcp_log("Recv finished...\n");
	pcYzjBuffer[8]='\0';
	nYzjLength=atoi(pcYzjBuffer);
	iRc=Cif_recv(iSocketID,pcYzjBuffer+8,nYzjLength-8);
	if(iRc<0){
		vtcp_log("iRc =%d\n",iRc);
		vtcp_log("[%s][%d] 接收验印服务器失败!\n",__FL__);
		sprintf(acErrMsg,"接收验印服务器失败!\n");
		Cif_close(iSocketID);
		return(-1) ;
	}
	/*******关闭连接*******/
	Cif_close(iSocketID);
	memcpy(status,pcYzjBuffer+79,3);
	vtcp_log("[%s][%d],**************返回的状态stat=[%s]\n",__FILE__,__LINE__,status);
	if(memcmp(status,"000",3)==0){
		iRc = iLvSetAddit(wp_payin,"QSX","1",1);
		if(iRc){
			vtcp_log("%s,%d,CIF报文发送成功功,设置发送标志失败!",__FILE__,__LINE__);
			sprintf(acErrMsg,"接收验印服务器失败!\n");
			return(-1);
		}
	}else{
		iRc = iLvSetAddit(wp_payin,"QSX","0",1);
		vtcp_log("%s,%d,CIF报文发送失败,需要重新发送!",__FILE__,__LINE__);
		return(-1);
	} 
	vtcp_log("[%s][%d] 接受验印应答信息,更新记录********************成功[%s]\n",__FILE__,__LINE__,status); 
	/****成功发送cif报文正确接受响应*****/
	return(0);
}
/****modiby by wyw,2007.7.15,发送支票业务报文到验印服务器*****/
int SendZpCifToYy_HsZy(PAY_IN_AREA *wp_payin) /**HSYX专用**/
{
        ZPJL1_ADD_AREA wd_zp_add1;
        ZPJL2_ADD_AREA wd_zp_add2;
        ZPJL3_ADD_AREA wd_zp_add3;
        TYJL1_ADD_AREA wd_ty_add1;
        TYJL2_ADD_AREA wd_ty_add2;
        TYJL3_ADD_AREA wd_ty_add3;
        TYJL4_ADD_AREA wd_ty_add4;
        TYJL5_ADD_AREA wd_ty_add5;
        int iRc;
        int iPacklength=0;
        TCIFPACK *pack, cifpack;
        pack = &cifpack;  /***报文结构****/
        char pcFileName[256];
        int tmp_orderno; 
        char cifip[32];
        char tmpbuf[19];
        int  cifport;
        char tmp_port[32];
        int  iSocketID;
        char status[4];
        char sendtlrno[17];
        memset(status,0,4);
        memset(sendtlrno,' ',sizeof(sendtlrno));
        sendtlrno[16]='\0';
        memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
        memset(&wd_zp_add2,'\0',sizeof(wd_zp_add2));
        memset(&wd_zp_add3,'\0',sizeof(wd_zp_add3));
        memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
        memset(&wd_ty_add3,'\0',sizeof(wd_ty_add3));
        memset(&wd_ty_add4,'\0',sizeof(wd_ty_add4));
        memset(&wd_ty_add5,'\0',sizeof(wd_ty_add5));
        memset(tmpbuf,'\0',sizeof(tmpbuf));
        if(memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))==0){
                iRc=iLvGetAddit(wp_payin,"001",(char *)&wd_zp_add1,sizeof(wd_zp_add1));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域1失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域1失败!\n");
                        return(-1);
                }
                vtcp_log("%s,%d,Gethere,pack=[%s]\n",__FILE__,__LINE__,&wd_zp_add1);
                pad_zpjl1(&wd_zp_add1);
                iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域2失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域2失败!\n");
                        return(-1);
                }
                pad_zpjl2(&wd_zp_add2);
                iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_zp_add3,sizeof(wd_zp_add3));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域3失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域3失败!\n");
                        return(-1);
                }
        }else if(memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0){
                iRc=iLvGetAddit(wp_payin,"001",(char *)&wd_ty_add1,sizeof(wd_ty_add1));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域1失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域1失败!\n");
                        return(-1);
                }
                iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域2失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域2失败!\n");
                        return(-1);
                }
                pad_zpjl2(&wd_zp_add2);
                iRc=iLvGetAddit(wp_payin,"004",(char *)&wd_ty_add4,sizeof(wd_ty_add4));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域4失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域4失败!\n");
                        return(-1);
                }
                pad_tyjl1(&wd_ty_add1);
                pad_tyjl4(&wd_ty_add4);

                iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_ty_add3,sizeof(wd_ty_add3));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域3失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域3失败!\n");
                        return(-1);
                }
                /****pad_tyjl3(&wd_ty_add3);*******/
                iRc=iLvGetAddit(wp_payin,"005",(char *)&wd_ty_add5,sizeof(wd_ty_add5));
                if(iRc){
                        vtcp_log("[%s][%d] 取附加域5失败\n",__FL__);
                        sprintf(acErrMsg,"取附加域5失败!\n");
                        return(-1);
                }
        }
        /************构造cif报文******************/
        vInitCifPack(pack);
#define ADDCIFTAG(pack,tag,data) {      char *p; \
                                                                        p=malloc(sizeof(data)+1); \
                                                                        memcpy(p,data,sizeof(data)); \
                                                                        p[sizeof(data)]='\0'; \
                                                                        vAddCifTag(pack,tag,p); \
                                                                        free(p); \
                                                                }
        /***********公共要素**********/
        ADDCIFTAG(pack,"30A",wp_payin->T_wtday);   
        ADDCIFTAG(pack,"0BC",wp_payin->T_orderno);
        ADDCIFTAG(pack,"CC5",wp_payin->T_cashno);
        ADDCIFTAG(pack,"D21",wp_payin->T_payno);
        pad_field(wp_payin->T_payactno,sizeof(wp_payin->T_payactno));
        ADDCIFTAG(pack,"D23",wp_payin->T_payactno);   
        pad_field(wp_payin->T_payactno,sizeof(wp_payin->T_payactno));
        memset(tmpbuf,'\0',sizeof(tmpbuf));
        memcpy(tmpbuf,"RMB",3);
        memcpy(tmpbuf+3,wp_payin->T_txamt,15);
        vAddCifTag(pack,"32A",tmpbuf);

        ADDCIFTAG(pack,"D22",wp_payin->T_payname);
        pad_field(wp_payin->T_cashactno,sizeof(wp_payin->T_cashactno));
        ADDCIFTAG(pack,"59C",wp_payin->T_cashactno);
        ADDCIFTAG(pack,"59A",wp_payin->T_cashname);    /******付款人名称******/
        ADDCIFTAG(pack,"BS6",wp_payin->T_respday);     /***回执期限***/
        ADDCIFTAG(pack,"TXC","0100");           /***新添加的附加域,交易代码(4)*****/
        memcpy(sendtlrno,g_pub_tx.tel,sizeof(g_pub_tx.tel)-1);/**操作员**/
        if(sendtlrno[0]==0){
            get_zd_data(DC_TEL,sendtlrno);
        }
        vAddCifTag(pack,"RES",sendtlrno);
        ADDCIFTAG(pack,"DA0","00");                /***核签状态****/
        if(memcmp(wp_payin->T_txnum,"30102",5)==0){
         /*******支票截留业务要素*********/
                char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
                int  nBsrCnt=0;
                int  tmpdate=0,tmpdate1=0;
                memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
                memcpy(pack->head.cifno,"100",3);
                ADDCIFTAG(pack,"D35",wd_zp_add1.cpday);
                ADDCIFTAG(pack,"D01",wd_zp_add1.cpno);
                ADDCIFTAG(pack,"D37",wd_zp_add1.use);     /******用途****/
                ADDCIFTAG(pack,"D41",wd_zp_add2.bscnt);
                nBsrCnt=apatoi(wd_zp_add2.bscnt,2);
                if(nBsrCnt>0){
                        memcpy(pcBsrBuffer,(char *)wd_zp_add2.bslist,nBsrCnt*60);
                        vAddCifTag(pack,"D42",pcBsrBuffer);
                }
                ADDCIFTAG(pack,"D30",wd_zp_add3.bscheckpswd); 
                vAddCifTag(pack,"D32","01");
                /**modify wyw 20070618  YZJ要求提示付款日期,要大于等于小额系统系统日期,现在提示付款日期取出票日期加10**/
                tmpdate=apatoi(wd_zp_add1.cpday,8);
                pub_base_deadlineD(tmpdate,9,&tmpdate1);
                vtcp_log("%s,%d 提示付款日期:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
                memset(tmpbuf,'\0',sizeof(tmpbuf));
                apitoa(tmpdate1,8,tmpbuf);
                vtcp_log("%s,%d 提示付款日期:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
                vAddCifTag(pack,"D36",tmpbuf);  
         }else{
                /********通用截留业务要素*********/
                char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
                int  nBsrCnt=0;
                memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
                memcpy(pack->head.cifno,"101",3);
                ADDCIFTAG(pack,"D35",wd_ty_add1.cpday);
                ADDCIFTAG(pack,"D01",wd_ty_add1.cpno);
                ADDCIFTAG(pack,"D37",wd_ty_add1.use);     /******用途****/
                ADDCIFTAG(pack,"D41",wd_zp_add2.bscnt);
                nBsrCnt=apatoi(wd_zp_add2.bscnt,2);
                if(nBsrCnt>0){
                        memcpy(pcBsrBuffer,(char *)wd_zp_add2.bslist,nBsrCnt*60);
                        vAddCifTag(pack,"D42",pcBsrBuffer);
                }
                ADDCIFTAG(pack,"D32",wd_ty_add4.ptype);
                ADDCIFTAG(pack,"D36",wd_ty_add4.tsday);
                ADDCIFTAG(pack,"D51",wd_ty_add4.mac);
                ADDCIFTAG(pack,"D61",wd_ty_add4.stopday);
                ADDCIFTAG(pack,"D62",wd_ty_add4.xyno);
                ADDCIFTAG(pack,"D63",wd_ty_add4.htno);
                ADDCIFTAG(pack,"D64",wd_ty_add4.cdday);
                ADDCIFTAG(pack,"D66",wd_ty_add4.cdname);
        }
        vtcp_log("[%s][%d]构造完毕的cis报文[%d]:\n",__FILE__,__LINE__,strlen((char *)pack));
        vtcp_log("%s\n",(char *)pack);
        /**********构造CIF报文完毕*******/
        /****Add by SSH,2007.5.15,转换标准cis格式为银之杰格式****/
        /********************************************************/      

        memset(pcFileName,'\0',sizeof(pcFileName));
        sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
        memset(cifip,'\0',sizeof(cifip));
        /*******读取验印服务器的IP和端口号*******/
 
        iRc=hv_readIni(pcFileName,"CIFSERVER","Address",cifip);
        if(iRc){
                vtcp_log("[%s][%d] 读取配置文件错误\n",__FL__);
                sprintf(acErrMsg,"读取配置文件错误!\n");
                return(-1);
        }
        vtcp_log("[%s][%d]CIF IP=[%s]\n",__FILE__,__LINE__,cifip);
        memset(tmp_port,'\0',sizeof(tmp_port));
        iRc=hv_readIni(pcFileName,"CIFSERVER","Port",tmp_port);
        if(iRc){
                vtcp_log("[%s][%d] 读取配置文件错误\n",__FL__);
                sprintf(acErrMsg,"读取配置文件错误!\n");
                return(-1);
        }
        cifport=apatoi(tmp_port,strlen(tmp_port));
        vtcp_log("[%s][%d]CIF PORT=[%s]\n",__FILE__,__LINE__,tmp_port);
        /*******连接验印服务器********/
        iSocketID=Cif_connect(cifip,cifport);
        if(iSocketID<=0){
                vtcp_log("[%s][%d] 连接验印服务器失败!\n",__FL__);
                sprintf(acErrMsg,"连接验印服务器失败!\n");
                return(-1);
        }
        vtcp_log("socket=%d\n", iSocketID);
        /*******发送CIF报文到验印服务器****/
        iPacklength=apatoi(pack->head.length,sizeof(pack->head.length)); 
        vtcp_log("%s,%d Try to send[%d]...\n",__FILE__,__LINE__,iPacklength);
        iRc=Cif_send(iSocketID,pack,iPacklength);
        if(iRc<0){
                vtcp_log("[%s][%d] 发送到验印服务器失败!\n",__FL__);
                sprintf(acErrMsg,"发送到验印服务器失败!\n");
                Cif_close(iSocketID);
                return(-1);
        }
        vtcp_log("Send finished,try to recv...\n");
        iRc=Cif_recv(iSocketID,pack,9);
        if(iRc<0){
                vtcp_log("iRc =%d\n",iRc);
                vtcp_log("[%s][%d] 接收验印服务器失败!\n",__FL__);
                sprintf(acErrMsg,"接收验印服务器失败!\n");
                Cif_close(iSocketID);
                return(-1) ;
        }
        vtcp_log("Recv finished...\n");
        /*******关闭连接*******/
        Cif_close(iSocketID);
        memcpy(status,pack->head.status,3);
        vtcp_log("[%s][%d],**************返回的状态stat=[%s]\n",__FILE__,__LINE__,status);
        if(memcmp(status,"000",3)==0){
                iRc = iLvSetAddit(wp_payin,"QSX","1",1);
                if(iRc){
                        vtcp_log("%s,%d,CIF报文发送成功功,设置发送标志失败!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"接收验印服务器失败!\n");
                        return(-1);
                }
        }else{
                iRc = iLvSetAddit(wp_payin,"QSX","0",1);
                vtcp_log("%s,%d,CIF报文发送失败,需要重新发送!",__FILE__,__LINE__);
                return(-1);
        } 
        vtcp_log("[%s][%d] 接受验印应答信息,更新记录********************成功[%s]\n",__FILE__,__LINE__,status); 
        /****成功发送cif报文正确接受响应*****/
        return(0);
}
/****NEWTCTD:增加加解密的函数****/
static void desPack(unsigned char *input,unsigned char *output,int len)
{
	int i, j;
	for (i=0, j=0; i<len; i+=2 , j++)
		output[j] = ((input[i] - 0x30) << 4) | (input[i+1] - 0x30);
}
static void desUnPack(unsigned char *input,unsigned char *output,int len)
{
	int i, j;
	for (i=0,j=0; i<len; i++, j+=2) {
		output[j] = ((input[i] & 0xf0) >> 4) + 0x30;
		output[j+1] = (input[i] & 0x0f) + 0x30;
	}
}

int nBepsEncrypt(PAY_IN_AREA *wp_payin,TD_ADD_AREA *wp_td){
	if(getenv("FHTEST")!=NULL && strcmp(getenv("FHTEST"),"LVTEST")==0){
		/****软加密****/
		char pcKey[33];
		char pcBinKey[17];
		char pcPinBlock[8];
		char sTmp[33];
		vtcp_log("%s,%d,Gethere,测试明文:[%.6s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		memset(pcKey,'\0',sizeof(pcKey));
		memset(pcBinKey,'\0',sizeof(pcBinKey));
		memset(pcPinBlock,'\0',sizeof(pcPinBlock));
		memset(sTmp,'\0',sizeof(sTmp));
		/****取得KEY****/
		if(getenv("TCTDKEY")!=NULL){
			strncpy(pcKey,getenv("TCTDKEY"),32);
		}else{
			memset(pcKey,'2',32);	
		}
		iAscToBcd(pcKey,pcBinKey,32);
		/****计算PINBLOCK****/
		pcPinBlock[0]=0x06;
		iAscToBcd(wp_td->pswdcode,pcPinBlock+1,6);
		memset(pcPinBlock+4,0xff,4);
		/****加密****/
		des2key(pcBinKey,0);
		Ddes(pcPinBlock,sTmp);
		/****非base64编码,而是bcd - asc****/
		memset(wp_td->pswdcode,'\0',sizeof(wp_td->pswdcode));
		iBcdToAsc(sTmp,wp_td->pswdcode,8);	
		sprintf(sTmp,"%08d",strlen(wp_td->pswdcode));
		memcpy(wp_td->pswdlength,sTmp,8);
		vtcp_log("%s,%d,Gethere,测试加密密文:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		{
			/****解密测试****/
			/****解密测试****/
			TD_ADD_AREA wd_tdtest;
			memcpy(&wd_tdtest,wp_td,sizeof(wd_tdtest));
			nBepsDescrypt(wp_payin,&wd_tdtest);
			vtcp_log("%s,%d,Gethere,测试解密:[%s][%s]\n",__FILE__,__LINE__,wd_tdtest.pswdlength,wd_tdtest.pswdcode);
		}
	}else{
		/****TODO:此处应调用加密机函数****/
		char pcKey[33];
		char cHostIp[16];
		char cHostPort[6];
		char cPinActno[17];
		char cInputPwd[7];
		int iRet=0;
		char sTmp[33];
		char pcBinKey[17];

		memset(pcBinKey, 0 ,sizeof(pcBinKey));
		memset(sTmp, 0 , sizeof(sTmp));
		memset(cInputPwd, 0 , sizeof(cInputPwd));
		memset(pcKey, 0 , sizeof(pcKey));
		memset(cPinActno, 0 , sizeof(cPinActno));
		memset(cHostIp, 0 , sizeof(cHostIp));
		memset(cHostPort, 0 , sizeof(cHostPort));
		memcpy(cInputPwd,wp_td->pswdcode,sizeof(cInputPwd)-1);
		memcpy(wp_td->pswdlength,"00000016",8);
		memcpy(wp_td->pswdcode,"0000000000000000",16);
		memcpy(cHostIp,(char*)getenv("HSM_HOST_IP"),sizeof(cHostIp)-1);
		if(getenv("HSM_HOST_IP")!=NULL){
			vtcp_log("%s,%d get env ip ",__FILE__,__LINE__);
			strncpy(cHostIp,(char*)getenv("HSM_HOST_IP"),sizeof(cHostIp)-1);
		}
		if(getenv("HSM_HOST_PORT")!=NULL){
			strncpy(cHostPort,getenv("HSM_HOST_PORT"),sizeof(cHostPort)-1);
		}
		strncpy(cPinActno,"0000000000000000",sizeof(cPinActno)-1);
		if(getenv("TCTDKEY")!=NULL){
			strncpy(pcKey,getenv("TCTDKEY"),32);
		}else{
			memset(pcKey,'2',32);	
		}
		vtcp_log("%s,%d,Gethere,测试加密明文前:[%s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		vtcp_log("%s,%d,ip=:[%s]\n",__FILE__,__LINE__,cHostIp);
		vtcp_log("%s,%d,port=[%d]\n",__FILE__,__LINE__,atoi(cHostPort));
		/*vtcp_log("%s,%d,inputpwd=[%s]\n",__FILE__,__LINE__,cInputPwd);*/
		vtcp_log("%s,%d,pinactno=[%s]\n",__FILE__,__LINE__,cPinActno);
		vtcp_log("%s,%d,pcKey=[%s]\n",__FILE__,__LINE__,pcKey);
		vtcp_log("%s,%d,pcBinkey[%s]\n",__FILE__,__LINE__,pcBinKey);		
		iRet=UnionEncryptPin(cHostIp,atoi(cHostPort),cInputPwd,cPinActno,pcKey,pcBinKey);
		/**add by wangyongwei 20070905**/
		if(iRet){
			sprintf(acErrMsg,"加密错误请查看加密机重新加密");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			return(-1);
		}
		/**add by wangyongwei 20070905**/
		memcpy(wp_td->pswdcode,pcBinKey,sizeof(wp_td->pswdcode));
		vtcp_log("%s,%d,Gethere,测试加密密文:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		/* test 直接验证解密
		{
		char cUserKey[7];
		memset(cUserKey, 0 , sizeof(cUserKey));
		vtcp_log("%s,%d,ip=:[%s]\n",__FILE__,__LINE__,cHostIp);
		vtcp_log("%s,%d,port=[%d]\n",__FILE__,__LINE__,atoi(cHostPort));
		vtcp_log("%s,%d,pcKey=[%s]\n",__FILE__,__LINE__,pcKey);
		vtcp_log("%s,%d,pinactno=[%s]\n",__FILE__,__LINE__,cPinActno);
		vtcp_log("%s,%d,密文=[%s]\n",__FILE__,__LINE__,pcBinKey);
		memset(wp_td->pswdcode, 0 , sizeof(wp_td->pswdcode));

		vtcp_log("%s,%d,Gethere,测试解密明文前:[%s]\n",__FILE__,__LINE__,cUserKey);
		iRet=UnionDecryptPin(cHostIp,atoi(cHostPort),pcKey,pcBinKey,cPinActno,cUserKey);
		vtcp_log("%s,%d,Gethere,测试解密明文:[%.6s] ret=%d\n",__FILE__,__LINE__,cUserKey,iRet);
		}
		******************************/
		return(0);
	}
}
int nBepsDescrypt(PAY_IN_AREA *wp_payin,TD_ADD_AREA *wp_td){
	if(getenv("FHTEST")!=NULL && strcmp(getenv("FHTEST"),"LVTEST")==0){
		/****软解密****/
		char pcKey[33];
		char pcBinKey[17];
		char pcPinBlock[8];
		char sTmp[33];
		int nPswdLen=0;
		vtcp_log("%s,%d,Gethere,测试明文:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		memset(pcKey,'\0',sizeof(pcKey));
		memset(pcBinKey,'\0',sizeof(pcBinKey));
		memset(pcPinBlock,'\0',sizeof(pcPinBlock));
		memset(sTmp,'\0',sizeof(sTmp));
		/****取得KEY****/
		if(getenv("TCTDKEY")!=NULL){
			strncpy(pcKey,getenv("TCTDKEY"),32);
		}else{
			memset(pcKey,'2',32);	
		}
		iAscToBcd(pcKey,pcBinKey,32);
		/****解密得到PINBLOCK****/
		nPswdLen=apatoi(wp_td->pswdlength,8);
		wp_td->pswdcode[nPswdLen]='\0';
		iAscToBcd(wp_td->pswdcode,sTmp,nPswdLen);
		vtcp_log("%s,%d,Gethere,测试解码长度:[%d]\n",__FILE__,__LINE__,nPswdLen);
		des2key(pcBinKey,1);
		Ddes(sTmp,pcPinBlock);
		/****根据PINBLOCK解出明文****/
		iBcdToAsc(pcPinBlock+1,wp_td->pswdcode,3);
		wp_td->pswdcode[6]='\0';
		memcpy(wp_td->pswdlength,"00000006",8);

		vtcp_log("%s,%d,Gethere,测试解密明文:[%.6s]\n",__FILE__,__LINE__,wp_td->pswdcode);
	}else{
		/****TODO:此处应调用加密机函数****/
		char cHostIp[16];
		char cHostPort[6];
		char cPinActno[17];
		char pcKey[33];
		char pcBinKey[17];
		int iRet=0;
		memset(pcBinKey, 0 , sizeof(pcBinKey));
		memset(pcKey, 0 , sizeof(pcKey));
		memset(cPinActno, 0 , sizeof(cPinActno));
		memset(cHostIp, 0 , sizeof(cHostIp));
		memset(cHostPort, 0 , sizeof(cHostPort));
		memcpy(wp_td->pswdlength,"00000006",8);
		if(getenv("HSM_HOST_IP")!=NULL){
			strncpy(cHostIp,getenv("HSM_HOST_IP"),sizeof(cHostIp)-1);
		}
		if(getenv("HSM_HOST_PORT")!=NULL){
			strncpy(cHostPort,getenv("HSM_HOST_PORT"),sizeof(cHostPort)-1);
		}
		strncpy(cPinActno,"0000000000000000",sizeof(cPinActno)-1);
		if(getenv("TCTDKEY")!=NULL){
			strncpy(pcKey,getenv("TCTDKEY"),32);
		}else{
			memset(pcKey,'2',32);	
		}
		/****TODO:此处应调用加密机函数****/
		vtcp_log("%s,%d,Gethere,测试解密明文前:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		iRet=UnionDecryptPin(cHostIp,atoi(cHostPort),pcKey,wp_td->pswdcode,cPinActno,wp_td->pswdcode);
		/**add by wangyongwei 20070905**/
		if(iRet){
			vtcp_log("%s,%d,Gethere,测试解密明文解密错误:[%d]\n",__FILE__,__LINE__,iRet);
		}
		/**add by wangyongwei 20070905**/
		vtcp_log("%s,%d,Gethere,测试解密明文:[%.6s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		return(0);
	}
}
