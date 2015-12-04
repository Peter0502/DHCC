/***************************************************************************/
/* �ļ�����: lv_gscomm.pc                                                  */
/* ��������: С��֧������������������                                    */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
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
���ش�����Ϣ
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
	/****NEWYX2:���󸽼��򳤶�****/
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
		/*****��������Ķ�������Ҫѹ���𣿣� xyy 2006-9-11 10:05
		zip_space(cBuff);
		memcpy(lvca.lv_fd123+sizeof(BASE_EXCHANGE_AREA),cBuff,sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));
		*********************/
		memcpy(lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA),cBuff,strlen(cBuff));
	}
	else
	{
		vtcp_log("%s,%d �õ������ļ�����",__FL__);
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
/* ��������: int iLvTisToPayin(PAY_IN_AREA *wp_payin)                      */
/* ��������: С��ϵͳ��,����ǰ̨tis����payin��ֵ                           */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/

int iLvTisToPayin(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char tmpbuf[255];
	char cTxamt[16];
	/**һ�㣬����ʹ�ý������ĵ�һ���������������ȳ�ʼ��������**/
	memset(wp_payin,'\0',sizeof(PAY_IN_AREA));
	memset(cTxamt,0,sizeof(cTxamt));
	memcpy(wp_payin->type,ATYPE_PAY_IN,sizeof(wp_payin->type));
	initLvca();

	sprintf(tmpbuf, "%04d", strlen(lvca.lv_fd123));
	memcpy(wp_payin->length, tmpbuf, sizeof(wp_payin->length));
	vtcp_log("[%s][%d]fd123=[%s](len=%d)\n",__FL__,lvca.lv_fd123, strlen(lvca.lv_fd123));
	vtcp_log("[%s][%d]size==[%d]\n",__FL__,sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));/*�̶�����**/
	memcpy(((char *)wp_payin)+sizeof(BASE_EXCHANGE_AREA),lvca.lv_fd123,
		sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA));/*�̶�����**/
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
			/**ͨ�����ͨ��**/
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
			/* ֻ��¼���ʱ������ø����� */
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
			/****д�븽��������****/
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
			/****д�븽��������****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_FSSR,sizeof(wp_payin->F_txnum))==0)
		{
			FSSR_ADD_AREA   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****д�븽��������****/
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
			/****д�븽��������****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))==0)
		{
			/****NEWYX2:ԭ��С���ô�������¼�걳����֮������ø�����,
			*****   ��һ������ϵͳû��Ҫ��ô��������Ҳ���׳�����Ϊ�ڴ˴�***/
			if(strncmp(g_pub_tx.tx_code,"5925",4)==0)/**add by NEWBP5**/
			{
					vtcp_log("[%s][%d]��������ͨ���֧Ʊ��ִҵ����Ҫ������!\n",__FILE__,__LINE__);
			}
			else/**�����ԭ�ȵĶ������ӵ�else����    ****modify by NEWBP5**/
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
			    	vtcp_log("[%s][%d] ȡ�������ļ�ʧ��\n",__FL__);
			    	sprintf(acErrMsg,"ȡ�������ļ�ʧ��!\n");
			    	return(-1);
			    }
			    fread(cLargeBuffer,sizeof(cLargeBuffer)-1,1,fp);
			    fclose(fp);
			    /****д�븽��������****/
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
			    	vtcp_log("[%s][%d]���������������\n",__FILE__,__LINE__);
			    }
			    if(!iRc){
			    	iRc=iLvSetAddit(wp_payin,"003",&wd_add3,sizeof(wd_add3));
			    }
			    vtcp_log("[%s][%d]���������������\n",__FILE__,__LINE__);
			    /****NEWYX2 END:����֧Ʊ����������****/
			}
			/**end by NEWBP5**/
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0)
		{
			char cHyflag[2]; /**��Ѻ��ʽYHBP**/
			char cPjtype[3]; /**Ʊ������YHBP**/
			char cPayno[13]; /**��������YHBP**/
			char cTxno[5];   /**YHBP**/
			char cKinbr[BRNO_LEN+1];
			char cOrbrno[13];
			/****NEWYX2:����ͨ�ý���������******/
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
			
			get_zd_data("0670",cHyflag);/***��ȡ��ѺYHBP***/
			get_zd_data("0220",cPjtype);/***�õ�Ʊ������HPBP***/
			get_zd_data("0160",cTxno);/**YHBP**/
			get_zd_data("0030",cKinbr);/**YHBP**/
			memcpy(cPayno,wp_payin->F_acbrno,sizeof(cPayno)-1);/**YHBP**/
			vtcp_log("[%s][%d]cPayno=[%s]cTxno=[%s]\n",__FILE__,__LINE__,cPayno,cTxno);
			vtcp_log("[%s][%d]��Ѻ��ʽΪ=[%s]Ʊ������=[%s]\n",__FILE__,__LINE__,cHyflag,cPjtype);
			
			pub_base_AllDwnFilName( cFilename);
			fp=fopen(cFilename,"rb");
			if(fp==NULL){
				vtcp_log("[%s][%d] ȡ�������ļ�ʧ��\n",__FL__);
				sprintf(acErrMsg,"ȡ�������ļ�ʧ��!\n");
				return(-1);
			}
			fread(cLargeBuffer,sizeof(cLargeBuffer)-1,1,fp);
			fclose(fp);
			/****д�븽��������****/
			pcAddStr=lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA);
			vtcp_log("[%s][%d]pcAddStr=[%s]\n",__FILE__,__LINE__,pcAddStr);
			memcpy(&wd_add1,pcAddStr,sizeof(wd_add1));	
			if(memcmp(cPjtype,"05",sizeof(cPjtype)-1)==0)/**��������б�ƱYHBP,**/
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
			
			/***YHBP�������Ӷ����б�Ʊ����Ѻ�Ĵ���***/
			if(memcmp(cPjtype,"05",2)==0  )/**��������б�Ʊ**/
			{
				  vtcp_log("[%s][%d]cHyflag=[%s]cPayno=[%s]\n",__FILE__,__LINE__,cHyflag,cPayno);
				  /***����������:��д��Ʊ�������ƻ��������������***/
				  g_reply_int = Hv_uniontab_Sel( g_pub_tx.reply,&hv_uniontab,"or_br_no='%s' ",cPayno);
          if (g_reply_int)
          {
              vtcp_log("[%s][%d]��ѯ�Է��������ĺ������\n",__FILE__,__LINE__);
              return(99);
          }
          memcpy(wd_add4.paybrname2,hv_uniontab.br_name_f,sizeof(hv_uniontab.br_name_f)-1);
          vtcp_log("[%s][%d]wd_add4.paybrname2==[%s]",__FILE__,__LINE__,wd_add4.paybrname2);

				  if(cHyflag[0]== '0')/**���ѡ����ǻ�����Ѻ�Ļ�**/
				  {
				     printf("[%s][%d]��Ѻ��ʽΪ[%s]\n",__FILE__,__LINE__,cHyflag);
				     /**������Ҫ���ú�Ѻ����,�õ���Ѻ**/
				     memcpy(wd_add4.mac,"12345678901234567890",sizeof(wd_add4.mac));
				  }
			}
			pad_tyjl4(&wd_add4);
			pcAddStr+=sizeof(wd_add4);
			memcpy(&wd_add5,pcAddStr,sizeof(wd_add5));	
			/**YHBP�ж�һ�£�������������б�Ʊ��Ϣ�޸ĵĻ����Ͳ���Ҫ������addid��**/
			if(memcmp(cTxno,"5321",sizeof(cTxno)-1)==0)
			{
				   GetOr_br_noByBr_no(cKinbr, cOrbrno);
				   vtcp_log("[%s][%d]������������б�Ʊ��Ϣ�޸�addid=[%s]\n",__FILE__,__LINE__,wp_payin->T_addid);
				   iRc = Lv_pkgreg_Sel(g_pub_tx.reply, &wd_lv_pkgreg, "or_br_no = '%.12s' and wt_date = %.8s and \
				                      orderno = '%.8s' and lw_ind='%c' and pkgno='%.3s' ",cOrbrno, wp_payin->F_wtday, \
				                      wp_payin->F_orderno,wp_payin->F_lw_ind[0],wp_payin->F_pkgno);
           if (iRc!=DB_OK)
           {
                vtcp_log("%s,%d,Find error,iRc=%d\n",__FILE__,__LINE__,iRc);
                sprintf(acErrMsg,"δ�ҵ�ԭҵ���¼");
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
			/****NEWYX2 END:����֧Ʊ����������****/
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_JF,sizeof(wp_payin->F_txnum))==0)
		{
			JF_ADD_AREA     wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****д�븽��������****/
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
			/****д�븽��������****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));
		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_TYXX,sizeof(wp_payin->F_txnum))==0)
		{
			vtcp_log("[%s][%d]��ʱ���ṩͨ����Ϣ�ĸ�������!\n",__FILE__,__LINE__);

		}
		else if (memcmp(wp_payin->F_txnum,TXNUM_DS,sizeof(wp_payin->F_txnum))==0)
		{
			SFYW_ADD   wd_add;
			memset(&wd_add,'\0',sizeof(wd_add));
			memcpy(&wd_add, lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)
				-sizeof(BASE_EXCHANGE_AREA), sizeof(wd_add));
			/****д�븽��������****/
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
			/****д�븽��������****/
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
			/****д�븽��������****/
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
			/****д�븽��������****/
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
			/****д�븽��������****/
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
			/****д�븽��������****/
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
			/****д�븽��������****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			/****NEWTCTD:����ͨ��ͨ�ҿͻ�����****/
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
				vtcp_log("%s,%d,�ʺŲ�ѯ��¼��ֵΪ[%s]\n",__FILE__,__LINE__,sTmp);
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
			/****д�븽��������****/
			memcpy(wp_payin->F_wtday,pcLvGetDay(),sizeof(wp_payin->F_wtday));
			vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%.8s]",__FL__,wp_payin->F_wtday);
			iRc=iLvSetAddit(wp_payin,"000",&wd_add,sizeof(wd_add));

		}
	}
	/* begin add by LiuHuafeng 2006-4-22 16:52 ���ڽ��ҵ����෽Э��� 
	if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ,sizeof(wp_payin->F_pkgno))==0)
	{
	char cDfxyh[61];
	memset(cDfxyh, 0 , sizeof(cDfxyh));
	memcpy(cDfxyh,lvca.lv_fd123+sizeof(PAY_IN_FIX_AREA)-sizeof(BASE_EXCHANGE_AREA),sizeof(cDfxyh)-1);
	vtcp_log("[%s][%d] �෽Э���==[%s]\n",__FL__,cDfxyh);
	iRc=iLvSetAddit(wp_payin,"XYH",cDfxyh,strlen(cDfxyh));
	if (iRc)
	{
	vtcp_log("[%s][%d] error here at add xyh iRc=[%d]\n",__FL__,iRc);
	sprintf(acErrMsg,"����Э��ų��� %d", iRc);
	return(-1);
	}
	}
	****************************/    
	/* end by LiuHuafeng 2006-4-22 17:04*/
	iLvPayinAutoInfo(wp_payin);/**�õ����ں���ˮ��**/
	vtcp_log("[%s][%d] actype==[%.c] \n",__FL__,wp_payin->F_actype[0]);
	get_fd_data("0030", wp_payin->A_brno);
	iLvZipAll(wp_payin);/**ѹ�����пո�***/
	vtcp_log("[%s][%d] wp_payin->F_opackid==[%s] \n",__FL__,wp_payin->F_packid);
	vtcp_log("[%s][%d]wp_payin->F_txamt=[%.15s]\n",__FL__,wp_payin->F_txamt);
	vtcp_log("[%s][%d] !F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	vtcp_log("[%s][%d] !F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	vtcp_log("[%s][%d] !F_wtday       =[%.8s] ",__FL__,wp_payin->F_wtday       );
	/**NEWTCTD9:���������ѱ�־�ͽ��***/
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
	/**NEWTCTD9:���������ѱ�־�ͽ��***/
	return(0);
}
/***************************************************************************/
/* ��������: int iLvNewRecFromPayin(PAY_IN_AREA *wp_payin)                 */
/* ��������: ����payin�������ݲ���һ���µ�֧����¼                         */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
int iLvNewRecFromPayin(PAY_IN_AREA *wp_payin)
{
	/**��Ϊ���ô˺���ʱ�����벿���Ѿ���ֵ�������¼����û������**/
	/**��Ϊ���ô˺���ʱ���Ѿ�ͨ����飬�����ֶ�����ֶ�ֵ��������**/
	/* add by LiuHuafeng 2006-9-29 23:54 ��ֹ���ǰ���пո�,�Ƚ����ת��һ�� */
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
	/* add by LiuHuafeng 2006-11-11 0:30 ����Ϣ����ת��һ�·�ֹ���ֿո�ת��Ϊ\0 */

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
	/**�������������ｫƽ̨����¼��״̬��Ϊ����״̬ 20130602 hzh**/
	if (strcmp(g_pub_tx.tx_code,"6388") == 0 || strcmp(g_pub_tx.tx_code,"6389") == 0 )
	{
	strcpy(wp_payin->T_pkgno,"001");
	strcpy(wp_payin->T_stat,"3");
	vtcp_log("%s,%d,�������������ｫƽ̨����¼��״̬10��Ϊ����״̬30\n", __FL__);
	}
	/**�������������ｫƽ̨����¼��״̬��Ϊ����״̬ 20130602 hzh**/
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

	/****Modified by SSH,2005.12.2,���ʿ��ܻ����Ų�ȡkinbr****/
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
			wp_payin->T_stat[0]=wp_payin->A_stat[0];/*����ȡ�������õ�״̬*/
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
		/**�������������ｫƽ̨����¼��״̬��Ϊ����״̬ 20130602 hzh**/
	if (strcmp(g_pub_tx.tx_code,"6388") == 0 || strcmp(g_pub_tx.tx_code,"6389") == 0 )
	{
	strcpy(wp_payin->T_stat,"3");
	vtcp_log("%s,%d,�������������ｫ״̬��Ϊ����״̬\n", __FL__);
	}
	/**�������������ｫƽ̨����¼��״̬��Ϊ����״̬ 20130602 hzh**/
	vtcp_log("%s,%d ע����������״̬T_STAT=[%c]",__FL__,wp_payin->T_stat[0]);
	vtcp_log("%s,%d ע����������״̬A_STAT=[%c]",__FL__,wp_payin->A_stat[0]);
	vtcp_log("%s,%d ע����������״̬beg_STAT=[%c]",__FL__,wp_payin->T_begstat[0]);
	memcpy(wp_payin->T_operlevel,wp_payin->F_operlevel, sizeof(wp_payin->T_operlevel));
	memcpy(wp_payin->T_feeflag,wp_payin->F_feeflag, sizeof(wp_payin->T_feeflag));
	memcpy(wp_payin->T_feeamt,wp_payin->F_feeamt, sizeof(wp_payin->T_feeamt));
	wp_payin->T_checkflag[0]='0';/**δ����**/
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
/* ��������: int iLvResetPayin(PAY_IN_AREA *wp_payin)                      */
/* ��������: ����payin����������������������                               */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
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
	memcpy(wp_payin->A_tbsdy,pcLvGetDay(),sizeof(wp_payin->A_tbsdy));/*����*/
	get_fd_data("0040", wp_payin->A_wssrno);

	memcpy(wp_payin->A_brno,wp_payin->T_brno,sizeof(wp_payin->T_brno));
	vtcp_log("%s,%d,After reset,pkgno=%.3s,crdb=%c\n",__FL__, wp_payin->T_pkgno,wp_payin->F_crdb[0]);
	return(0);		

}
/***************************************************************************/
/* ��������: int iLvPayinCheck0(PAY_IN_AREA *wp_payin)                     */
/* ��������: С��ϵͳͨ�ü�麯��                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
int iLvPayinCheck0(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char *ptr;
	/****NEWYX BEGIN:������֯������޼���sql���****/
	char sql_str[512];
	/****NEWYX END:������֯������޼���sql���****/
	/**1.�����־��pkgno��txnum��ϵ���**/
	vtcp_log("[%s][%d]===========wp_payin->F_crdb[0]=[%c]\n",__FL__,wp_payin->F_crdb[0]);
	vtcp_log("[%s][%d]===========wp_payin->F_txnum=[%.5s]\n",__FL__,wp_payin->F_txnum);
	if (wp_payin->F_crdb[0]==CRDB_DR)
	{
		/**��Ǳ�־Ϊ��**/
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJ, sizeof(wp_payin->F_pkgno))==0||	
			memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0)
		{
			/**��ͨ���,��ִ**//****ADD NEWYX BEGIN : TYJL ****/
			if (memcmp(wp_payin->F_txnum,TXNUM_GKJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_QTJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_ZJZF,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_SQZF,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_GZJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))!=0 &&    
				memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))!=0 &&
                memcmp(wp_payin->F_txnum,"30104",5)!=0&&memcmp(wp_payin->F_txnum,"30105",5)!=0)
			{
				vtcp_log("%s,%d,txnum[%.5s]���Ϸ���Χ\n",__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ���Ų��ڷ�Χ��");
				return(-1);
			}
			/****NEWYX END : TYJL ****/
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJ, sizeof(wp_payin->F_pkgno))==0||	
			memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**ʵʱ���,��ִ**/
			/****NEWYX BEGIN :ͨ�ý������ڴ����ø�����****/
			if (memcmp(wp_payin->F_txnum,TXNUM_SSKS, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TD, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_TYJL, sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_DGTD, sizeof(wp_payin->F_txnum))!=0)
			{
				vtcp_log("%s,%d,txnum[%.5s]���ںϷ���Χ��\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ���Ų��ڷ�Χ��");
				return(-1);
			}
			/****NEWYX END :ͨ�ý������ڴ����ø�����****/
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ, sizeof(wp_payin->F_pkgno))==0||	
			memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**���ڽ��,��ִ**/
			if (memcmp(wp_payin->F_txnum,TXNUM_DQJJ,sizeof(wp_payin->F_txnum))!=0 &&
				memcmp(wp_payin->F_txnum,TXNUM_PLKS,sizeof(wp_payin->F_txnum))!=0)
			{
				vtcp_log("%s,%d,txnum[%.5s]���ںϷ���Χ��\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ���Ų��ڷ�Χ��");
				return(-1);
			}
		}
		else
		{
			vtcp_log("%s,%d,pkgno[%.3s]���ںϷ���Χ��\n",
				__FL__,wp_payin->F_pkgno);
			sprintf(acErrMsg,"����Ų��ڷ�Χ��");
			return(-1);
		}
		vtcp_log("[%s][%d]wp_payin->F_txnum=[%.5s]\n",__FL__,wp_payin->F_txnum);
	}
	else if (wp_payin->F_crdb[0]==CRDB_CR)
	{
		/**����ҵ��**/
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,sizeof(wp_payin->F_pkgno))==0)
		{
			/**��ͨ����**/
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
				vtcp_log("%s,%d,txnum[%.5s]���ںϷ���Χ��\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ�����Ͳ��ڷ�Χ��");
				return(-1);
			}
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_DQDJ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**���ڴ���**/
			if (memcmp(wp_payin->F_txnum,TXNUM_DQDJ, sizeof(wp_payin->F_txnum))!=0)
			{ 
				vtcp_log("%s,%d,txnum[%.5s]���ںϷ���Χ��\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ���Ų��ڷ�Χ��");
				return(-1);
			}
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJ, sizeof(wp_payin->F_pkgno))==0 ||
			memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ, sizeof(wp_payin->F_pkgno))==0)
		{
			/**ʵʱ���ǡ���ִ**/
			if (memcmp(wp_payin->F_txnum,TXNUM_TC, sizeof(wp_payin->F_txnum))!=0)
			{ 
				vtcp_log("%s,%d,txnum[%.5s]���ںϷ���Χ��\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ���Ų��ڷ�Χ��");
				return(-1);
			}
			vtcp_log("[%s][%d]wp_payin->F_txnum=[%s]\n",__FL__,wp_payin->F_txnum);
		}
		else if (memcmp(wp_payin->F_pkgno,PKGNO_DJTH, sizeof(wp_payin->F_pkgno))==0)
		{
			/**�����˻�**/
			if (memcmp(wp_payin->F_txnum,TXNUM_DJTH, sizeof(wp_payin->F_txnum))!=0)
			{ 
				vtcp_log("%s,%d,txnum[%.5s]���ںϷ���Χ��\n",
					__FL__,wp_payin->F_txnum);
				sprintf(acErrMsg,"ҵ���Ų��ڷ�Χ��");
				return(-1);
			}
		}
		else
		{
			vtcp_log("%s,%d,pkgno[%.3s]���ںϷ���Χ��\n",
				__FL__,wp_payin->F_pkgno);
			sprintf(acErrMsg,"����Ų��ڷ�Χ��");
			return(-1);
		}
	}
	else
	{
		/**����Ľ����־**/
		vtcp_log("%s,%d,crdb[%.1s]���ںϷ���Χ��\n",
			__FL__,wp_payin->F_crdb[0]);
		sprintf(acErrMsg,"�����־���ڷ�Χ��");
		return(-1);
	}
	vtcp_log("[%s][%d]wp_payin->F_txnum=[%.5s]\n",__FL__,wp_payin->F_txnum);
	/**2.����״̬���**/
	if (memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_LR_QUERY, sizeof(wp_payin->F_opcd))==0)
	{
		/**¼�뽻�ײ����״̬**/
	}
	else if (memcmp(wp_payin->F_opcd,OPCD_CHECK, sizeof(wp_payin->F_opcd))==0)
	{
		/**���˽��ף�ԭ����״̬ӦΪ¼��״̬**/
		if (wp_payin->T_stat[0]!=STAT_WZLR)
		{	
			vtcp_log("%s,%d,ԭ����״̬����¼��״̬[%.c]\n",
				__FL__,wp_payin->T_stat[0]);
			sprintf(acErrMsg,"ԭ���ײ���¼��״̬");
			return(-1);
		}
	}
	else if (memcmp(wp_payin->F_opcd,OPCD_SEND, sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_SEND_QUERY, sizeof(wp_payin->F_opcd))==0)
	{
		/**���ͽ��ף�ԭ״̬ӦΪ��ʽ����״̬**/
		if (wp_payin->T_stat[0]!=STAT_WZCHECK2)
		{	
			vtcp_log("%s,%d,ԭ����״̬����[%.c]\n",
				__FL__,wp_payin->T_stat[0]);
			sprintf(acErrMsg,"ԭ���ײ��Ǹ���״̬");
			return(-1);
		}
	}
	else if (memcmp(wp_payin->F_opcd,OPCD_INACT, sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_INACT_QUERY, sizeof(wp_payin->F_opcd))==0 ||
		memcmp(wp_payin->F_opcd,OPCD_INACT_MODIFY, sizeof(wp_payin->F_opcd))==0)
	{
		/**���˴����ף�ԭ״̬ӦΪ����״̬**/
		if (wp_payin->T_stat[0]!=STAT_LZNMAC_ERR &&	
			wp_payin->T_stat[0]!=STAT_LZLMAC_ERR &&	
			wp_payin->T_stat[0]!=STAT_LZAMAC_ERR &&	
			wp_payin->T_stat[0]!=STAT_LZGZ_QS &&	
			wp_payin->T_stat[0]!=STAT_LZGZ_BR)
		{
			vtcp_log("%s,%d,ԭ����״̬����1[%.c]\n",
				__FL__,wp_payin->T_stat[0]);
			sprintf(acErrMsg,"ԭ���ײ��Ǵ���״̬");
			return(-1);
		}
	}
	/**�������������ｫƽ̨����¼��״̬��Ϊ����״̬ 20130602 hzh**/
		if (strcmp(g_pub_tx.tx_code,"6388") == 0 || strcmp(g_pub_tx.tx_code,"6389") == 0 )
	{
	strcpy(wp_payin->F_opcd,"30");
	vtcp_log("%s,%d,�������������ｫƽ̨����¼��״̬10��Ϊ����״̬30\n", __FL__);
	}
	/**�������������ｫƽ̨����¼��״̬��Ϊ����״̬ 20130602 hzh**/
	/***�Ǹ��˽��������Ѳ�����ת��**/
	if (memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))!=0)
	{
		if (wp_payin->F_feeflag[0]==FEEFLAG_ACT ||
			wp_payin->F_feeflag[0]==FEEFLAG_ACTJJ)
		{
			if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin->F_pkgno)))
			{
				vtcp_log("%s,%d,�˽��ײ�������ȡת��������\n", __FL__);
				sprintf(acErrMsg,"�˽��ײ�������ȡת��������");
				return(-1);
			}
		}
	}
	/****Add by xyy 2006-9-11 10:42 ֻ�д���ҵ��ż��������****/
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,3)==0 ||memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,3)==0 || \
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJ,3)==0 || memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,3)==0)
	{/****Add by SSH,2006.1.25,���ǽ�����޼��****/
		struct lv_param_c wd_lv_param;
		memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
		memcpy(wd_lv_param.par_code,"CREBD",5);
		memcpy(wd_lv_param.par_code+5,wp_payin->F_pkgno,3);
		vtcp_log("%s,%d,�����ǽ������[%s]\n",__FL__,wd_lv_param.par_code);
		iRc = Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "par_code = '%s' ", wd_lv_param.par_code);
		if (iRc!=0 && iRc!= DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"���ݿ����");
			return(-1);
		}
		if (iRc!=DB_NOTFOUND)
		{
			/****����вż��****/
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
				vtcp_log("%s,%d,���[%.15s]��������[%.15s]\n",__FL__,wp_payin->F_txamt,wd_lv_param.par_value);
				sprintf(acErrMsg,"���׽�������");
				return(-1);

			}
		}
		/**begin add by LiuHuafeng CREBOUND ��CREBD��Ҫ�ж� */
		memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
		/****Modified by SSH,NEWYX2:֧Ʊ��ִ�����CREBOUND****/
		if (memcmp(wp_payin->F_txnum,"30102",5)!=0 &&
			memcmp(wp_payin->F_txnum,"30103",5)!=0 &&
			memcmp(wp_payin->F_txnum,"30104",5)!=0 &&
			memcmp(wp_payin->F_txnum,"30105",5)!=0)
		{
			memcpy(wd_lv_param.par_code,"CREBOUND",8);
			vtcp_log("%s,%d,�����ǽ������[%s]\n",__FL__,wd_lv_param.par_code);
			iRc=Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "par_code = '%s' ", wd_lv_param.par_code);
			if (iRc!=0 && iRc!= DB_NOTFOUND)
			{
				vtcp_log("%s,%d,���ݿ����[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���ݿ����");
				return(-1);
			}
			if (iRc!=DB_NOTFOUND)
			{
				/****����вż��****/
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
					vtcp_log("%s,%d,���[%.15s]��������[%.15s]\n",__FL__,wp_payin->F_txamt,wd_lv_param.par_value);
					sprintf(acErrMsg,"���׽�������");
					return(-1);
				}
			}
		}
		/****NEWYX BEGIN:V3.0.2��С�������ޱ����ֱ���****/
		memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
		iRc=Lv_param_Sel(g_pub_tx.reply, &wd_lv_param, "substr(par_code,1,1)='C' and (substr(par_code,2,3)='%.3s' or substr(par_code,2,3)='ALL') and (substr(par_code,5,3)='%.3s' or substr(par_code,5,3)='ALL') and (substr(par_code,8,5)='%.5s' or substr(par_code,8,5)='XX%.3s' or substr(par_code,8,5)='%.3sXX')", wp_payin->F_orbrno,wp_payin->F_acbrno,wp_payin->F_txnum,wp_payin->F_pkgno,wp_payin->F_pkgno);
		if (iRc!=0 && iRc!= DB_NOTFOUND)
		{
			vtcp_log("%s,%d,���ݿ����[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"���ݿ����");
			return(-1);
		}
		if (iRc!=DB_NOTFOUND)
		{
			/****����вż��****/
			vtcp_log("[%s][%d]���￪ʼ���������!!!",__FILE__,__LINE__);
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
				vtcp_log("%s,%d,���[%.15s]��������[%.15s]\n",__FILE__,__LINE__,wp_payin->F_txamt,wd_lv_param.par_value);
				vtcp_log("%s,%d,���׽�������[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���׽�������");
				return(-1);
			}
		}
		/****NEWYX BEGIN:V3.0.2��С�������ޱ����ֱ���****/
		/**end add by LiuHuafeng CREBOUND ��CREBD��Ҫ�ж� */
	}
	/****Deleted by SSH,2006.1.25,���ڵ�ģʽӦ���������С����ȡ����****
	**************���ܿ�С������ȡ��********
	*****************************************************************/
	/*****add by LiuHuafeng 2006-3-29 23:05 */

	return(0);
}
/***************************************************************************/
/* ��������: int iLvPayinCheck1(PAY_IN_AREA *wp_payin)                     */
/* ��������: С��ϵͳ�кż�麯��                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
/****Add by SSH,2006.5.9,����ˮ�������º�ල����****/

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
		vtcp_log("[%s] [%d] ��ѯ�������кŴ�[%d]\n",
			__FL__,iRc);
		sprintf(acErrMsg,"�������кŲ�����!");
		return(-1);
	}
	vtcp_log("%s,%d,������:[%s][%s]\n",__FL__,
		wp_hvuniontab_send.or_br_no,wp_hvuniontab_send.qs_no);
	if ( wp_hvuniontab_send.br_sts[0] != '1')
	{
		vtcp_log("[%s] [%d] ������״̬����!\n",__FL__);
		sprintf(acErrMsg,"������״̬����!");
		return(-1);
	}
	memcpy(wp_hvuniontab_rec.or_br_no,wp_payin->F_acbrno, sizeof(wp_hvuniontab_rec.or_br_no)-1);
	iRc=Hv_uniontab_Sel(g_pub_tx.reply, &wp_hvuniontab_rec, "or_br_no = '%s' ", wp_hvuniontab_rec.or_br_no);
	if (iRc!=DB_OK)
	{
		vtcp_log("[%s] [%d] ��ѯ�������кŴ�rec.or_br_no=[%s]sqlcode=[%d]\n",
			__FL__,wp_hvuniontab_rec.or_br_no,iRc);
		sprintf(acErrMsg,"�������кŲ�����!");
		return(-1);
	}
	vtcp_log("%s,%d,������:[%s][%s]\n",__FL__,
		wp_hvuniontab_rec.or_br_no,wp_hvuniontab_rec.qs_no);
	if ( wp_hvuniontab_rec.br_sts[0] != '1')
	{
		vtcp_log("[%s] [%d] ������״̬����!\n",__FL__);
		sprintf(acErrMsg,"������״̬����!");
		return(-1);
	}
	/* add by LiuHuafeng 20070508 ¼����503�� */
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
			vtcp_log("[%s] [%d] ������ҵ��Ȩ�޲�����!\n",__FL__);
			sprintf(acErrMsg,"�������޴�ҵ��Ȩ��!");
			return(-1);
		}
		if(sLv_c503r.sendcnt<0)
		{
			vtcp_log("[%s] [%d] ������ҵ��Ȩ��ʵЧ!\n",__FL__);
			sprintf(acErrMsg,"�����д�ҵ��Ȩ��ʵЧ!");
			return(-1);
		}
		memcpy(cAuth,wp_payin->F_pkgno,sizeof(wp_payin->F_pkgno));
		memcpy(cAuth+sizeof(wp_payin->F_pkgno),wp_payin->F_txnum,5);
		P=strstr(sLv_c503r.recvgrant,cAuth);
		if(P)
		{
			vtcp_log("%s,%d ���������д�%sҵ��Ȩ��",__FILE__,__LINE__,cAuth);
		}
		else
		{
			vtcp_log("%s,%d ������%s��%sҵ��Ȩ��",__FILE__,__LINE__,wp_hvuniontab_rec.qs_no,cAuth);
			sprintf(acErrMsg,"�������޴�ҵ��Ȩ��!");
			return(-1);
		}
	}
	/* end by LiuHuafeng 20070508 */
	/** rem by LiuHuafeng 20070407 for NEWYX
	if (memcmp(wp_hvuniontab_send.qs_no,wp_hvuniontab_rec.qs_no, sizeof(wp_hvuniontab_send.qs_no)-1)==0)
	{
	vtcp_log("[%s][%d]���������кͽ��������в�����ͬһ����!\n",
	__FL__);
	sprintf(acErrMsg,"���������кͽ��������в�����ͬһ����!");
	return(-1);
	}
	***********************/
	memcpy(wp_payin->A_sendco,wp_hvuniontab_send.sendco, sizeof(wp_payin->A_sendco));
	memcpy(wp_payin->A_payqsactno,wp_hvuniontab_send.qs_no, sizeof(wp_payin->A_payqsactno));
	memcpy(wp_payin->A_receco,wp_hvuniontab_rec.sendco, sizeof(wp_payin->A_receco));
	memcpy(wp_payin->A_cashqsactno,wp_hvuniontab_rec.qs_no, sizeof(wp_payin->A_cashqsactno));
	return(0);
}
/****�����ʺż��************************************/
/*   ����ֵ�ĺ���                                   */
/*   #define ACT_OK  0                 �ʻ�״̬���� */
/*   #define ACT_ERR 1                   �ʺŲ����� */
/*   #define ACT_NAME_ERR 2            �ʻ��������� */
/*   #define ACT_AVBAL_LOW 3           �ʻ����� */
/*   #define ACT_PASSWD_ERR 10         �ʻ�������� */
/*   #define ACT_FZN 11                    ״̬���� */
/*   #define ACT_OTHER_ERR -1              �������� */
/****************************************************/   
/****Modified by SSH,2005.12.2,��������ʱҪ�����û�����****/

/***************************************************************************/
/* ��������: int iLvGetRecvBrno(PAY_IN_AREA *wp_payin)                     */
/* ��������: ���ݽ��ܻ�������A_brno                                        */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
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
		vtcp_log("%s,%d,δ�ҵ����ܻ������������Ľ���\n",__FL__);
		sprintf(acErrMsg,"δ�ҵ����ܻ���");
		/****���������ͳ�ʼ��Ϊ��������,���Բ���������****/
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
/* ��������: int iLvPayinCheck2_Batch(PAY_IN_AREA *wp_payin)               */
/* ��������: ���������ʵĸ����ʺż��,��Ҫ�ǲ��ܸ��Ļ�����                 */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
/*int iLvPayinCheck2_Batch(PAY_IN_AREA *wp_payin)*/

/***************************************************************************/
/* ��������: int iLvPayinCheck2(PAY_IN_AREA *wp_payin)                     */
/* ��������: �����ʺż��                                                  */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:      ȥ�� gujy 060830                                                             */
/***************************************************************************/
/*int iLvPayinCheck2(PAY_IN_AREA *wp_payin)*/

/***************************************************************************/
/* ��������: int iLvPayinCheck3_Batch(PAY_IN_AREA *wp_payin)               */
/* ��������: ���������ʵ��տ��ʺż��,��Ҫ�ǲ��ܸ��Ļ�����                 */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:       ȥ��gujy.060830                                                            */
/***************************************************************************/

/***************************************************************************/
/* ��������: int iLvPayinCheck3(PAY_IN_AREA *wp_payin)                     */
/* ��������: ����A_actype��ʾ�ͻ��ʺ�����                                  */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:     0-����      																										 */
/*           1-����,2-�Թ�,3-�ڲ���,4-��Ŀ                                 */
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
	/**********���û����ų���ֵ************/
	if (memcmp(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd))==0)
	{
		vtcp_log("%s,%d\n", __FL__);
		/***ֻ�н��ս��ײ���Ҫ���û�����***/
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
			/**�����в�����,����ȡ��������**/
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
		get_fd_data("0040", wp_payin->A_wssrno);/**��ˮ��**/
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
/* ��������: int iLvPayinCheck4(PAY_IN_AREA *wp_payin)                     */
/* ��������: Ҫ�غϷ��Լ��                                                */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:         																														 */
/***************************************************************************/
int iLvPayinCheck4(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char cDate[9];
	memset(cDate,'\0',sizeof(cDate));
	/****Add by SSH,2006.4.19,����ʺ�����û�к���****/
	if (isAnyChinese(wp_payin->F_payactno,sizeof(wp_payin->F_payactno)))
	{
		vtcp_log("%s,%d,�����ʺ��к��зǷ��ַ�\n", __FL__);		
		sprintf(acErrMsg,"�����ʺ��к��зǷ��ַ�");
		return(-1);
	}
	if (isAnyChinese(wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno)))
	{
		vtcp_log("%s,%d,�տ��ʺ��к��зǷ��ַ�\n", __FL__);		
		sprintf(acErrMsg,"�տ��ʺ��к��зǷ��ַ�");
		return(-1);
	}
	/****��齻�׽��ķ�Χ**/
	/************************/
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))!=0 &&
		memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0)
	{
		/***��Ӧ����¼�룬��дwtday****/
		memcpy(wp_payin->F_wtday,wp_payin->A_tbsdy,
			sizeof(wp_payin->F_wtday));

	}else
	{
		/***ί�����ڲ�Ӧ����С��ϵͳ����***/
		if (memcmp(wp_payin->F_wtday,wp_payin->A_tbsdy,
			sizeof(wp_payin->F_wtday))>0)
		{
			/* add by LiuHuafeng �����жϣ�
			* ����ǻ��պ��յ����������ô���л�ִ¼��ͻ�ʧ�� */
			if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno)))
			{
				vtcp_log("%s,%d,ί�����ڲ��ܴ���С��ϵͳ����!wtday=[%.8s] tbsdy=[%.8s]\n",__FL__,wp_payin->F_wtday,wp_payin->A_tbsdy);
				sprintf(acErrMsg,"ί�����ڲ��ܴ���С��ϵͳ����!");
				return(-1);
			}
		}
	}
	vtcp_log("[%s][%d]wp_payin->A_tbsdy===[%s]",__FILE__,__LINE__,wp_payin->A_tbsdy);
	memcpy(cDate,wp_payin->F_pdate,sizeof(cDate)-1);
	zip_space(cDate);
	if (memcmp(cDate,wp_payin->F_wtday,sizeof(cDate)-1)>0)
	{
		vtcp_log("%s,%d,Ʊ������[%.8s]���ܴ���ί������[%.8s]!\n",
			__FL__,cDate,wp_payin->F_wtday);
		sprintf(acErrMsg,"Ʊ�����ڲ��ܴ���ί������!");
		return(-1);
	}
	memcpy(cDate,pcLvGetDay(),sizeof(cDate)-1);
	zip_space(cDate);
	if (memcmp(cDate,wp_payin->A_tbsdy,sizeof(cDate)-1)<0 && cDate[0]!='\0')
	{
		vtcp_log("%s,%d,Ӧ�����ڲ���С�ڵ�ǰ����!cDate=[%s],wtday=[%s]\n",__FL__,cDate,wp_payin->F_wtday);
		sprintf(acErrMsg,"Ӧ�����ڲ���С�ڵ�ǰ����!");
		return(-1);
	}
	/****Add by SSH,2006.5.9,����ˮ��׷���º���Ϣ***/
	return(0);
}
/****��Ȩ��ϵ���******************************/
int Ilvpayincheck5(PAY_IN_AREA *wp_payin)
{
	/**CAUTION:***/
	/**�Ժ��ϴ��˴������Ȩ���***/
	return(0);
}    
/***************************************************************************/
/* ��������: int iLvPayinCheck6(PAY_IN_AREA *wp_payin)                     */
/* ��������: Ҫ��һ���Լ��                                                */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:         																														 */
/***************************************************************************/
int iLvPayinCheck6(PAY_IN_AREA *wp_payin)
{
	/****�Ƚ�����Ҫ�������Ҫ���Ƿ�һ��****/
	pub_base_full_anti_zero(wp_payin,sizeof(PAY_IN_AREA));
	if (memcmp(wp_payin->F_pkgno,wp_payin->T_pkgno,
		sizeof(wp_payin->F_pkgno))!=0)
	{
		vtcp_log("%s,%d,pkgno[%.3s][%.3s]��һ��",__FL__,
			wp_payin->F_pkgno,wp_payin->T_pkgno);
		sprintf(acErrMsg,"���ı�Ų�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_orderno,wp_payin->T_orderno,
		sizeof(wp_payin->F_orderno))!=0)
	{
		vtcp_log("%s,%d,orderno[%.8s][%.8s]��һ��",__FL__,
			wp_payin->F_orderno,wp_payin->T_orderno);
		sprintf(acErrMsg,"������Ų�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_txnum,wp_payin->T_txnum,
		sizeof(wp_payin->F_txnum))!=0)
	{
		vtcp_log("%s,%d,txnum[%.5s][%.5s]��һ��",__FL__,
			wp_payin->F_txnum,wp_payin->T_txnum);
		sprintf(acErrMsg,"ҵ�����ͱ�Ų�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_acbrno,wp_payin->T_acbrno,
		sizeof(wp_payin->F_acbrno))!=0)
	{
		vtcp_log("%s,%d,acbrno[%.12s][%.12s]��һ��",__FL__,
			wp_payin->F_acbrno,wp_payin->T_acbrno);
		sprintf(acErrMsg,"�����в�һ��");
		return(-1);
	}
	if (iAmtCmp(wp_payin->F_txamt,wp_payin->T_txamt,
		sizeof(wp_payin->F_txamt))!=0)
	{
		vtcp_log("%s,%d,txamt[%.15s][%.15s]��һ��",__FL__,
			wp_payin->F_txamt,wp_payin->T_txamt);
		sprintf(acErrMsg,"���׽�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_payactno,wp_payin->T_payactno,
		sizeof(wp_payin->F_payactno))!=0)
	{
		vtcp_log("%s,%d,payactno:\n[%.32s]\n[%.32s]\n��һ��",__FL__,
			wp_payin->F_payactno,wp_payin->T_payactno);
		sprintf(acErrMsg,"�����ʺŲ�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_payname,wp_payin->T_payname,
		sizeof(wp_payin->F_payname))!=0)
	{
		vtcp_log("%s,%d,F_payname==[%.60s]\nT_payname=[%.60s]\n��һ��",__FL__,
			wp_payin->F_payname,wp_payin->T_payname);
		sprintf(acErrMsg,"�����˲�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_cashno,wp_payin->T_cashno,
		sizeof(wp_payin->F_cashno))!=0)
	{
		vtcp_log("%s,%d,cashno[%.12s][%.12s]��һ��\n",__FL__,
			wp_payin->F_cashno,wp_payin->T_cashno);
		sprintf(acErrMsg,"�տ��˿����в�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_cashactno,wp_payin->T_cashactno,
		sizeof(wp_payin->F_cashactno))!=0)
	{
		vtcp_log("%s,%d,cashactno:\n[%.32s][%.32s]\n��һ��",__FL__,
			wp_payin->F_cashactno,wp_payin->T_cashactno);
		sprintf(acErrMsg,"�տ����ʺŲ�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_ywtype,wp_payin->T_ywtype,
		sizeof(wp_payin->F_ywtype))!=0)
	{
		vtcp_log("%s,%d,ywtype[%.12s][%.12s]��һ��",__FL__,
			wp_payin->F_ywtype,wp_payin->T_ywtype);
		sprintf(acErrMsg,"ҵ��������һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_wtday,wp_payin->T_wtday,
		sizeof(wp_payin->F_wtday))!=0)
	{
		vtcp_log("%s,%d,F_wtday====[%s]T_wtday===[%s]��һ��",__FL__,
			wp_payin->F_wtday,wp_payin->T_wtday);
		sprintf(acErrMsg,"ί�����ڲ�һ��");
		return(-1);
	}
	/****Modified by SSH,2006.1.10,С������ȼ�****/
	/****
	if(memcmp(wp_payin->F_operlevel,wp_payin->T_operlevel,
	sizeof(wp_payin->F_operlevel))!=0){
	vtcp_log("%s,%d,operlevel[%.1s][%.1s]��һ��",__FL__,
	wp_payin->F_operlevel,wp_payin->T_operlevel);
	sprintf(acErrMsg,"���ȼ���һ��");
	return(-1);
	}
	*****/
	/******************************************************/
	/********************�ɱ䲿��**************************/
	/******************************************************/

	/********delete by xyy 2006-9-15 13:40 ¼�벿��û�У��������еĲ�����ʱע��
	zip_space(wp_payin->F_voctype);
	if (memcmp(wp_payin->F_voctype,wp_payin->T_voctype,
	sizeof(wp_payin->F_voctype))!=0)
	{
	vtcp_log("%s,%d,voctype[%.2s][%.2s]��һ��",__FL__,
	wp_payin->F_voctype,wp_payin->T_voctype);
	sprintf(acErrMsg,"ƾ֤���಻һ��");
	return(-1);
	}
	zip_space(wp_payin->F_vocnum);
	if (memcmp(wp_payin->F_vocnum,wp_payin->T_vocnum,
	sizeof(wp_payin->F_vocnum))!=0)
	{
	vtcp_log("%s,%d,vocnum[%.12s][%.12s]��һ��",__FL__,
	wp_payin->F_vocnum,wp_payin->T_vocnum);
	sprintf(acErrMsg,"ƾ֤���벻һ��");
	return(-1);
	}
	if (memcmp(wp_payin->F_feeflag,wp_payin->T_feeflag,
	sizeof(wp_payin->F_feeflag))!=0)
	{
	vtcp_log("%s,%d,feeflag[%.1s][%.1s]��һ��",__FL__,
	wp_payin->F_feeflag,wp_payin->T_feeflag);
	sprintf(acErrMsg,"�����ѱ�־��һ��");
	return(-1);
	}
	if (iAmtCmp(wp_payin->F_feeamt,wp_payin->T_feeamt,
	sizeof(wp_payin->F_feeamt))!=0)
	{
	vtcp_log("%s,%d,feeamt[%.15s][%.15s]��һ��",__FL__,
	wp_payin->F_feeamt,wp_payin->T_feeamt);
	sprintf(acErrMsg,"�����ѽ�һ��");
	return(-1);
	}

	if (memcmp(wp_payin->F_ptype,wp_payin->T_ptype,
	sizeof(wp_payin->F_ptype))!=0)
	{
	vtcp_log("%s,%d,ptype[%.2s][%.2s]��һ��\n",__FL__,
	wp_payin->F_ptype,wp_payin->T_ptype);
	sprintf(acErrMsg,"Ʊ�����಻һ��");
	return(-1);
	}
	if (memcmp(wp_payin->F_pdate,wp_payin->T_pdate,
	sizeof(wp_payin->F_pdate))!=0)
	{
	vtcp_log("%s,%d,pdate[%.8s][%.8s]��һ��",__FL__,
	wp_payin->F_pdate,wp_payin->T_pdate);
	sprintf(acErrMsg,"Ʊ�����ڲ�һ��");
	return(-1);
	}

	if (memcmp(wp_payin->F_pnum,wp_payin->T_pnum,
	sizeof(wp_payin->F_pnum))!=0)
	{
	vtcp_log("%s,%d,pnum[%.8s][%.8s]��һ��",__FL__,
	wp_payin->F_pnum,wp_payin->T_pnum);
	sprintf(acErrMsg,"Ʊ�ݺ��벻һ��");
	return(-1);
	}

	if (iAmtCmp(wp_payin->F_ptxamt,wp_payin->T_ptxamt,
	sizeof(wp_payin->F_ptxamt))!=0)
	{
	vtcp_log("%s,%d,ptxamt[%.15s][%.15s]��һ��",__FL__,
	wp_payin->F_ptxamt,wp_payin->T_ptxamt);
	sprintf(acErrMsg,"�⳥��һ��");
	return(-1);
	}
	if (iAmtCmp(wp_payin->F_jtxamt,wp_payin->T_jtxamt,
	sizeof(wp_payin->F_jtxamt))!=0)
	{
	vtcp_log("%s,%d,ptxamt[%.15s][%.15s]��һ��",__FL__,
	wp_payin->F_ptxamt,wp_payin->T_ptxamt);
	sprintf(acErrMsg,"�ܸ���һ��");
	return(-1);
	}
	if (memcmp(wp_payin->F_actype,wp_payin->T_actype,
	sizeof(wp_payin->F_actype))!=0)
	{
	vtcp_log("%s,%d,actype��һ��[%c][%c]",__FL__,
	wp_payin->F_actype[0],wp_payin->T_actype[0]);
	sprintf(acErrMsg,"�Է��˻����಻һ��");
	return(-1);
	}
	if (memcmp(wp_payin->F_pswdtype,wp_payin->T_pswdtype,
	sizeof(wp_payin->F_pswdtype))!=0)
	{
	vtcp_log("%s,%d,pswdtype[%.1s][%.1s]��һ��",__FL__,
	wp_payin->F_pswdtype,wp_payin->T_pswdtype);
	sprintf(acErrMsg,"�Է��������಻һ��");
	return(-1);
	}
	***********************************/
	if (iDateCmp(wp_payin->F_owtday,wp_payin->T_owtday,
		sizeof(wp_payin->F_owtday))!=0)
	{
		vtcp_log("%s,%d,owtday[%.8s][%.8s]��һ��",__FL__,
			wp_payin->F_owtday,wp_payin->T_owtday);
		sprintf(acErrMsg,"ԭί�����ڲ�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_otxnum,wp_payin->T_otxnum,
		sizeof(wp_payin->F_otxnum))!=0)
	{
		vtcp_log("%s,%d,otxnum[%.5s][%.5s]��һ��",__FL__,
			wp_payin->F_otxnum,wp_payin->T_otxnum);
		sprintf(acErrMsg,"ԭҵ���Ų�һ��");
		return(-1);
	}
	if (memcmp(wp_payin->F_retcode,wp_payin->T_retcode,
		sizeof(wp_payin->F_retcode))!=0)
	{
		vtcp_log("%s,%d,retcode[%.2s][%.2s]��һ��",__FL__,
			wp_payin->F_retcode,wp_payin->T_retcode);
		sprintf(acErrMsg,"�˻�ԭ��һ��");WRITEMSG
			return(-1);
	}
	if (memcmp(wp_payin->F_rcpstat,wp_payin->T_rcpstat,
		sizeof(wp_payin->F_rcpstat))!=0)
	{
		vtcp_log("%s,%d,rcpstat[%.2s][%.2s]��һ��",__FL__,
			wp_payin->F_rcpstat,wp_payin->T_rcpstat);
		sprintf(acErrMsg,"��ִ���벻һ��");WRITEMSG
			return(-1);
	}
	if (memcmp(wp_payin->T_brno,lvca.brno,BRNO_LEN)!=0)
	{
		vtcp_log("%s,%d,Brno[%.7s][%.7s]��һ��",__FL__,
			lvca.brno,wp_payin->T_brno);
		sprintf(acErrMsg,"�Ǳ���������");WRITEMSG
			return(-1);
	}
	return(0);
}
/***************************************************************************/
/* ��������: int iLvPayinCheck7(PAY_IN_AREA *wp_payin)                     */
/* ��������: ��¼�����Լ��                                                */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:         																														 */
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
/* ��������: iLvPayinCheck8                                                */
/* ��������: ֧�����װ�״̬����Լ�����                                    */
/* ����:                                                                   */
/*            wp_payin   -֧������ͨ������ṹ��                           */
/*            wp_lv_wbctl-�������Ʊ�ṹ                                   */
/*            wp_lv_lbctl-�������Ʊ�ṹ                                   */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:     ��������Ӧ��wp_lv_wbctl��NULL,��������Ӧ��wp_lv_lbctl��NULL   */
/*           Add by SSH,2006.1.9,���ӶԻ�ִ����ƴ�������⴦��              */
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
		vtcp_log("%s,%d,��������Ϊ��\n",__FL__);
		sprintf(acErrMsg,"��������Ϊ��");WRITEMSG
			return(-1);
	}
	else if (iLvGetOrient(wp_payin)==TX_SEND && wp_lv_wbctl==NULL)
	{
		vtcp_log("%s,%d,��������Ϊ��\n",__FL__);
		sprintf(acErrMsg,"��������Ϊ��");WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]=========wp_payin->F_packid=[%s]\n",__FL__,wp_payin->F_packid);
	memcpy(cPackday,wp_payin->F_packday,sizeof(cPackday)-1);
	memcpy(cPackid,wp_payin->F_packid,sizeof(cPackid)-1);
	vtcp_log("[%s][%d]=========wp_payin->F_pkgno=[%.3s]\n",__FL__,wp_payin->F_pkgno);
	vtcp_log("[%s][%d]=========cPackid=[%s]\n",__FL__,cPackid);
	vtcp_log("[%s][%d]========cPackday=[%s]\n",__FL__,cPackday);
	dblTmp=0;
	/****NBWYX BEGIN :֧Ʊ��ִҵ��ƴ������ͨ���Ƿ�ʽ��****/
	/****NEWYX2:��ϵͳ�ж���ͨ��ǻ�ִ����Ʊ����ִ����TXNO����****/
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
		/****��ִ���������⴦��****/
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
			sprintf(acErrMsg,"�����α�������¼���� %d", iRc);
			WRITEMSG
				return(-1);
		}
		iRc = Lv_lbctl_Fet_Upd(&wd_lv_lbctl, g_pub_tx.reply);
		if (iRc !=DB_OK )
		{
			vtcp_log("%s,%d,Fet_Upd lbctl error[%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"����������¼���� %d", iRc);WRITEMSG
				return(-1);
		}
		vtcp_log("[%s],[%d],A_tbsdy============[%.8s]",__FL__,wp_payin->A_tbsdy);
		vtcp_log("[%s],[%d],wd_lv_lbctl->o_pack_date===[%d] o_packid=[%s]",__FL__,wd_lv_lbctl.o_pack_date,wd_lv_lbctl.o_packid);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		if (strlen(wd_lv_lbctl.o_packid)<sizeof(wd_lv_lbctl.o_packid)-1) 
		{
			struct lv_wbctl_c lv_wbctl;
			memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
			/****ԭ������¼��û��������ִ****/
			/**��ʼ����һ������¼**/
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
			/****content���ڽ����и�ֵ****/

			vtcp_log("[%s][%d]******************wbctl.pay_qs_no=[%s]\n", __FL__,wp_lv_wbctl->pay_qs_no);

			memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
			iRc=Lv_wbctl_Ins(lv_wbctl, g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Insert wbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"�����������Ʊ�ʧ��");WRITEMSG
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
				sprintf(acErrMsg,"�����������Ʊ�ʧ��");WRITEMSG
					return(-1);
			}
			apitoa(wp_lv_wbctl->o_pack_date,sizeof(wp_payin->F_packday),
				wp_payin->F_packday);
			memcpy(wp_payin->F_packid,wp_lv_wbctl->packid,
				sizeof(wp_payin->F_packid));
			memcpy(wp_payin->F_packday,(char*)pcLvGetDay(),8);
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("%s,%d,�����°�\n",__FL__);
			return(PCHK_NEW);
		}
		else
		{
			struct lv_wbctl_c lv_wbctl;
			memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
			/****��ִ����,�Ѿ��й�ƴ����¼****/
			wp_lv_wbctl->pack_date=wd_lv_lbctl.o_pack_date;
			memcpy(wp_lv_wbctl->packid,wd_lv_lbctl.o_packid,
				sizeof(wp_lv_wbctl->packid)-1);

			iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date=%d and packid = '%s' ",
				wp_lv_wbctl->pack_date, wp_lv_wbctl->packid);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Lock wbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���������Ʊ���δ�ҵ���Ӧ��¼");WRITEMSG
					return(-1);
			}
			iRc = Lv_wbctl_Fet_Upd(wp_lv_wbctl,g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Fet_Upd wbctl error  [%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���������Ʊ���δ�ҵ���Ӧ��¼");WRITEMSG
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
			/****content���ڽ����и�ֵ****/
			memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
			iRc=Lv_wbctl_Upd_Upd(lv_wbctl, g_pub_tx.reply);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Upd_Upd wbctl error �����������Ʊ�ʧ��[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"�����������Ʊ�ʧ��");WRITEMSG
					return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("[%s][%d]wbctl.totamt=[%f]",__FILE__,__LINE__,lv_wbctl.totamt);
			vtcp_log("[%s][%d]wbctl.dtlcnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlcnt);
			vtcp_log("[%s][%d]wbctl.dtlsuccnt=[%ld]",__FILE__,__LINE__,lv_wbctl.dtlsuccnt);
			vtcp_log("[%s][%d]wbctl.dtlsucamt=[%f]",__FILE__,__LINE__,lv_wbctl.dtlsucamt);
			vtcp_log("[%s][%d]wbctl.packid=[%s]",__FILE__,__LINE__,lv_wbctl.packid);
		}
		return(0);/****��ִƴ���������,���ؼ�������ִ����****/
	}
	vtcp_log("[%s][%d] cPackid=====[%s]\n",__FL__,cPackid);
	zip_space(cPackid);
	vtcp_log("[%s][%d] cPackid=====[%s]\n",__FL__,cPackid);
	if (strlen(cPackid)==0)
	{
		struct lv_wbctl_c lv_wbctl;
		memset(&lv_wbctl, 0 , sizeof(lv_wbctl));
		vtcp_log("[%s][%d] \n",__FL__);
		/**�����δƴ��������������һ������¼**/
		if (iLvGetOrient(wp_payin)==TX_RECEIVE)
		{
			/**��������Ҫƴ����������Ϊ������**/
			vtcp_log("%s,%d,���˼�¼δ�ҵ�����Ϣ\n",__FL__);
			sprintf(acErrMsg,"���˼�¼δ�ҵ�����Ϣ");WRITEMSG
			return(-1);
		}
		/**��ʼ����һ������¼**/
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
		/****Ӧ�����ڡ�ԭ�������ڡ�ԭ����id����ϸҵ��ɹ���������ϸҵ��ɹ����
		*****ԭ�����ͺš�����ڵ㡢���սڵ���ڽ���������*********************/
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
		/****content���ڽ����и�ֵ****/
		/****content���ڽ����и�ֵ****/
		/****HSYX BEGIN: ֧Ʊ��ִҵ��ƴ��Ҫ��****/
		/****��ִҵ�����ߵ������ֻ��֧Ʊ������ҵ��,���Բ����ж�ҵ������****/

		vtcp_log("[%s][%d]ע�Ⱑ���������Ҫ��������payin.F_pkgno=[%.3s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
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
				  vtcp_log("[%s][%d]YHBP���ԣ�����",__FILE__,__LINE__);
				  memcpy(wp_lv_wbctl->o_pkgno,"002",3);
			}
			else
			{
			    memcpy(wp_lv_wbctl->o_pkgno,"999",3);
			    wp_lv_wbctl->o_pack_date=apatoi(pcLvGetDay(),8);
			    memcpy(wp_lv_wbctl->o_packid,"99999999",8);
			}
			vtcp_log("%s,%d ע������opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
			/** ����Ӧ����һ�µİ� dongxy
			memcpy(wp_lv_wbctl->cash_qs_no,wp_payin->A_payqsactno, sizeof(wp_lv_wbctl->cash_qs_no)-1);
			memcpy(wp_lv_wbctl->pay_qs_no,wp_payin->A_cashqsactno, sizeof(wp_lv_wbctl->pay_qs_no)-1);
			*/
			memcpy(wp_lv_wbctl->cash_qs_no,wp_payin->A_cashqsactno, sizeof(wp_lv_wbctl->cash_qs_no)-1);
			memcpy(wp_lv_wbctl->pay_qs_no,wp_payin->A_payqsactno, sizeof(wp_lv_wbctl->pay_qs_no)-1);
		}
		/****HSYX END: ֧Ʊ��ִҵ��ƴ��Ҫ��****/
		vtcp_log("[%s][%d]******************wbctl.pay_qs_no=[%s]\n", __FL__,wp_lv_wbctl->pay_qs_no);
		vtcp_log("%s,%d before insert wbctl pkgno==[%s]",__FILE__,__LINE__,lv_wbctl.pkgno);
		vtcp_log("%s,%d before insert wbctl packid==[%s]",__FILE__,__LINE__,lv_wbctl.packid);
		vtcp_log("%s,%d before insert wbctl packdate==[%d]",__FILE__,__LINE__,lv_wbctl.pack_date);
		memcpy(&lv_wbctl, wp_lv_wbctl, sizeof(lv_wbctl));
		iRc=Lv_wbctl_Ins(lv_wbctl, g_pub_tx.reply);
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Insert wbctl error  [%d]\n",__FL__,iRc);
			sprintf(acErrMsg,"�����������Ʊ�ʧ��");
			return(-1);
		}
		vtcp_log("%s,%d �������� ",__FILE__,__LINE__);
		vtcp_log("%s,%d ע������opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		Lv_wbctl_Debug(&lv_wbctl);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		apitoa(wp_lv_wbctl->pack_date,sizeof(wp_payin->F_packday),wp_payin->F_packday);
		vtcp_log("%s,%d ע������opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		memcpy(wp_payin->F_packid,wp_lv_wbctl->packid,sizeof(wp_payin->F_packid));
		vtcp_log("%s,%d ע������opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		apitoa(wp_lv_wbctl->pack_date,sizeof(wp_payin->T_packday),wp_payin->T_packday);
		vtcp_log("%s,%d ע������opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		memcpy(wp_payin->T_packid,wp_lv_wbctl->packid,sizeof(wp_payin->T_packid));
		vtcp_log("%s,%d ע������opackid==[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
		vtcp_log("%s,%d,wp_payin->T_packday =[%.8s]",__FL__,wp_payin->T_packday);
		vtcp_log("%s,%d,wp_payin->T_packid =[%.8s]",__FL__,wp_payin->T_packid);
		vtcp_log("%s,%d,wp_payin->F_packday =[%.8s]",__FL__,wp_payin->F_packday);
		vtcp_log("%s,%d,wp_lv_wbctl->pkgno-------------------------- =[%s]",__FL__,wp_lv_wbctl->pkgno);
		vtcp_log("%s,%d,wp_lv_wbctl->packid-------------------------- =[%s]",__FL__,wp_lv_wbctl->packid);
		vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
		vtcp_log("%s,%d,�����°�\n",__FL__);
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
		/**����Ѿ�ƴ��,������¼�Ƿ����*/
		if (iLvGetOrient(wp_payin)==TX_RECEIVE)
		{
			vtcp_log("[%s][%d] \n",__FL__);
			/***����****/
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
				sprintf(acErrMsg,"���������Ʊ���δ�ҵ���Ӧ��¼");
				return(-1);
			}
			/***���������ҵ���¼���Ƚ�pkgno��txnum
			****���������С������������Ƿ�һ��****/
			if (memcmp(wp_payin->F_pkgno,wp_lv_lbctl->pkgno,
				sizeof(wp_payin->F_pkgno))!=0)
			{
				vtcp_log("%s,%d,pkgno���� ���а���������벻�� [%d]\n",__FL__,iRc);
				return(-1);
			}
			if (memcmp(wp_payin->A_payqsactno,wp_lv_lbctl->pay_qs_no,
				sizeof(wp_payin->A_payqsactno))!=0)
			{
				vtcp_log("%s,%d,payqsactno����[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���з����������к������벻��");
				return(-1);
			}
			if (memcmp(wp_payin->A_cashqsactno,wp_lv_lbctl->cash_qs_no,
				sizeof(wp_payin->A_cashqsactno))!=0)
			{
				vtcp_log("%s,%d,cashqsactno����[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���н����������к������벻��");
				return(-1);
			}
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("%s,%d,���Ѵ���\n",__FL__);
			return(PCHK_EXISTS);
		}
		else
		{
			struct lv_wbctl_c lv_wbctl;
			memset(&lv_wbctl,0,sizeof(lv_wbctl));
			vtcp_log("[%s][%d]wp_payin->F_pkgno=[%.3s] ",__FL__,wp_payin->F_pkgno);
			/***����***/
			wp_lv_wbctl->pack_date=apatoi(wp_payin->F_packday,8);
			memcpy(wp_lv_wbctl->packid,wp_payin->F_packid,sizeof(wp_lv_wbctl->packid)-1);
			iRc=Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date =%ld and packid = '%s' ", wp_lv_wbctl->pack_date, wp_lv_wbctl->packid);
			if (iRc!=DB_OK)
			{
				vtcp_log("%s,%d,Lock wbctl error[%d]\n",__FL__,iRc);
				sprintf(acErrMsg,"���������Ʊ���δ�ҵ���Ӧ��¼");
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
			/***���������ҵ���¼���Ƚ�pkgno��txnum
			****���������С������������Ƿ�һ��****/
			if (memcmp(wp_payin->F_pkgno,wp_lv_wbctl->pkgno,
				sizeof(wp_payin->F_pkgno))!=0)
			{
				vtcp_log("%s,%d,pkgno����[%d]\n",__FL__,iRc);
				vtcp_log("%s,%d,payin->F_pkgno=[%.3s]lv_wbctl->pkgno=[%s]",__FL__,wp_payin->F_pkgno,wp_lv_wbctl->pkgno);
				sprintf(acErrMsg,"���а���������벻��");
				return(-1);
			}
			/****HSYX BEGIN:֧Ʊ��ִҵ��ķ���ͽ����������Ƿ�������****/
			if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0)
			{
				/*** ���񲻶�dongxy
				if(memcmp(wp_payin->A_cashqsactno,wp_lv_wbctl->pay_qs_no,
					sizeof(wp_payin->A_cashqsactno))!=0)
				***/
				if (memcmp(wp_payin->A_payqsactno,wp_lv_wbctl->pay_qs_no,
					sizeof(wp_payin->A_payqsactno))!=0)
				{
					vtcp_log("%s,%d,payqsactno����\n",__FILE__,__LINE__);
					vtcp_log("%s,%d,wbctl->pay_qs_n=[%s],A_payqsactno[%s]\n",__FILE__,__LINE__,
						wp_lv_wbctl->pay_qs_no,wp_payin->A_payqsactno);
					sprintf(acErrMsg,"���з����������к������벻��");
					return(-1);
				}
				/** ���񲻶� dongxy
				if(memcmp(wp_payin->A_payqsactno,wp_lv_wbctl->cash_qs_no,
					sizeof(wp_payin->A_payqsactno))!=0)
				**/
				if (memcmp(wp_payin->A_cashqsactno,wp_lv_wbctl->cash_qs_no,
					sizeof(wp_payin->A_cashqsactno))!=0)
				{
					vtcp_log("%s,%d,cashqsactno����[%d]\n",__FILE__,__LINE__,iRc);
					vtcp_log("%s,%d,wbctl->cash_qs_n=[%s],A_cashqsactno[%s]\n",__FILE__,__LINE__,
						wp_lv_wbctl->cash_qs_no,wp_payin->A_cashqsactno);
					sprintf(acErrMsg,"���н����������к������벻��");
					return(-1);
				}
			}else
			{
				if (memcmp(wp_payin->A_payqsactno,wp_lv_wbctl->pay_qs_no,
					sizeof(wp_payin->A_payqsactno))!=0)
				{
					vtcp_log("%s,%d,payqsactno����[%d]\n",__FL__,iRc);
					sprintf(acErrMsg,"���з����������к������벻��");
					return(-1);
				}
				if (memcmp(wp_payin->A_cashqsactno,wp_lv_wbctl->cash_qs_no,
					sizeof(wp_payin->A_cashqsactno))!=0)
				{
					vtcp_log("%s,%d,A_cashqsactno=[%s]cash_qs_no=[%s]����[%d]",__FL__,wp_payin->A_cashqsactno,wp_lv_lbctl->cash_qs_no,iRc);
					sprintf(acErrMsg,"���н����������к������벻��");
					return(-1);
				}
			}
			/****HSYX END:֧Ʊ��ִҵ��ķ���ͽ����������Ƿ�������****/
			vtcp_log("%s,%d,wp_lv_wbctl->pack_date-------------------------- =[%ld]",__FL__,wp_lv_wbctl->pack_date);
			vtcp_log("%s,%d wbctl.stat====[%c]",__FL__,wp_lv_wbctl->stat[0]);
			if (wp_lv_wbctl->stat[0]==PSTAT_PACKING||
				wp_lv_wbctl->stat[0]==PSTAT_PACKED)
			{
				vtcp_log("%s,%d wbctl.stat====[%c]",__FL__,wp_lv_wbctl->stat[0]);
				/***�������ƴ��****/
				if (wp_lv_wbctl->pack_date!=apatoi(wp_payin->A_tbsdy,8))
				{
					/***����ƴ���������ڲ��ǽ��죬����***/
					vtcp_log("%s,%d,��ͼƴ���ǽ���İ�\n",__FL__);
					sprintf(acErrMsg,"����ƴ���ǽ���İ�");
					return(-1);
				}
				/***add by xyy YHBP ****/
				vtcp_log("[%s][%d]ע�������TXCODE==[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				if( !strncmp("5321",g_pub_tx.tx_code,4))
				{
					vtcp_log("[%s][%d]�Ҹ��������б�Ʊ�޸�[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				}
				else if (wp_lv_wbctl->stat[0]!=PSTAT_PACKING)/**modify by xyy YHBP**/
				{
					vtcp_log("%s,%d,�������Ҳ�����ƴ��!\n",__FL__);
					return(PCHK_EXISTS);
				}
				/****NEWYX BEGIN:����֧Ʊ��ִƴ��Ҫ��****/
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
				  vtcp_log("[%s][%d]ע�������TXCODE==[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
				  if( !strncmp("5321",g_pub_tx.tx_code,4))
				  {
				  	  vtcp_log("[%s][%d]�Ҹ��������б�Ʊ�޸�[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
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
				/****NEWYX END:����֧Ʊ��ִƴ��Ҫ��****/
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
					sprintf(acErrMsg,"�����������Ʊ�ʧ��");
					return(-1);
				}
				vtcp_log("%s,%d,���Ѹ���packid=%.8s,dtlcnt=%.0f,totamt=[%.0f]\n",
					__FL__,wp_lv_wbctl->packid,wp_lv_wbctl->dtlcnt,
					wp_lv_wbctl->totamt);
				return(PCHK_UPDATE);
			}
			else
			{
				vtcp_log("[%s][%d] \n",__FL__);
				/***�����ڶ��Ҳ�������ƴ��***/
				vtcp_log("%s,%d,���Ѵ���\n",__FL__);
				return(PCHK_EXISTS);
			}
		}	
	}
}
/***************************************************************************/
/* ��������: int iLvPayinCheck9(PAY_IN_AREA *wp_payin)                     */
/* ��������: �෽Э��ż��                                                */
/* ��������: 2005.12.2                                                     */
/* ����:     SSH                                                           */
/* ˵��:         																														 */
/***************************************************************************/
int iLvPayinCheck9(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char pcDfxyNo[61];
	double dTxamt=0;
	struct lv_newxyh_c wd_lv_newxyh;
	memset(&wd_lv_newxyh,'\0',sizeof(wd_lv_newxyh));

	/****NEWYX BEGIN:���Զ෽Э��Ų����****/
	if (getenv("FHTEST")!=NULL && memcmp((char*)getenv("FHTEST"),"LVTEST",6)==0)
	{
		return(0);
	}	
	/****NEWYX END:���Զ෽Э��Ų����****/

	if (memcmp((char*)getenv("FHTEST"),"LVTEST",6)==0)
	{
		/****���Բ����෽Э���**/
		return(0);
	}
	memset(pcDfxyNo,'\0',sizeof(pcDfxyNo));
	/* rem by LiuHuafeng 20070116
	iRc=iLvGetAddit(wp_payin,"XYH",pcDfxyNo,sizeof(pcDfxyNo)-1);
	if (iRc)
	{
	vtcp_log("%s,%d,�õ��෽Э��Ŵ���\n",__FL__);
	sprintf(acErrMsg,"�õ��෽Э��Ŵ���");
	return(-1);
	}
	******************************/
	memcpy(pcDfxyNo,wp_payin->F_dfxyno,sizeof(pcDfxyNo)-1);
	zip_space(pcDfxyNo);
	if (strlen(pcDfxyNo)==0)
	{
		vtcp_log("%s,%d,�෽Э��Ų�����\n",__FL__);
		sprintf(acErrMsg,"�෽Э��Ų�����!");
		return(-1);
	}
	/*add by chenggx 20131029 �෽Э���60��0����� beg*/
	if (memcmp(pcDfxyNo,"000000000000000000000000000000000000000000000000000000000000",60)==0)
	{
		vtcp_log("%s,%d,�෽Э���[%s],�����\n",__FL__,pcDfxyNo);
		return(0);
	}
	/*add by chenggx 20131029 �෽Э���60��0����� end*/
	memcpy(wd_lv_newxyh.xyno,pcDfxyNo,sizeof(wd_lv_newxyh.xyno)-1);
	memcpy(wd_lv_newxyh.pay_ac_no,wp_payin->F_payactno,sizeof(wd_lv_newxyh.pay_ac_no)-1);
	iRc=Lv_newxyh_Sel(g_pub_tx.reply,&wd_lv_newxyh, "xyno = '%s' and pay_ac_no = '%s'", wd_lv_newxyh.xyno, wd_lv_newxyh.pay_ac_no);
	if (iRc)
	{
		vtcp_log("%s,%d,�෽Э��Ų���ȷ\n",__FL__);
		sprintf(acErrMsg,"�෽Э��Ų���ȷ");
		return(-1);
	}
	str2dbl(wp_payin->F_txamt,sizeof(wp_payin->F_txamt),0,&dTxamt);
	vtcp_log("%s,%d,���󸶿���Ϊ:%.0f\n",__FL__,dTxamt);
	if (wd_lv_newxyh.limitamt-dTxamt<-0.01)
	{
		vtcp_log("%s,%d,�������������޶�[%.0f][%.0f]\n",__FL__,wd_lv_newxyh.limitamt,dTxamt);
		sprintf(acErrMsg,"�������������޶�");
		return(-1);
	}
	return(0);
}
/********************************************************************************/
/* ��������: int iLvCreateHz(PAY_IN_AREA *wp_payin,PAY_IN_AREA *wp_payin_hz)    */
/* ��������: ����payin���ݲ���һ���µĻ�ִ��¼                                  */
/* ��������: 2005.12.2                                                       			*/
/* ����:     SSH                                                               		*/
/* ˵��:     ֻ��������򣬱����ݿ��Ե���newrec����������       						 			*/
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
		vtcp_log("%s,%d,������[%.3s]û��ҵ���ִ\n",
			__FL__,wp_payin->F_pkgno);
		sprintf(acErrMsg,"������û��ҵ���ִ!");
		return(-1);
	}
	/*Modified by xyy 2006-9-19 11:16��ԭ��¼�Ľ�����ţ��ŵ���ִ��¼��oorderno��**/
	memcpy(wp_payin_hz->F_oorderno,wp_payin->F_orderno,
		sizeof(wp_payin_hz->F_oorderno));
	vtcp_log("%s,%d,ʹ��ԭ֧���������:[%.8s][%.8s]\n",__FL__,
		wp_payin->F_orderno,wp_payin_hz->F_oorderno);
	iOrderno=iLvGetOrderno();
	if (iOrderno<=0)
	{
		vtcp_log("%s,%d,�õ�֧��������Ŵ���!\n",__FL__);
		return(-1);
	}
	apitoa(iOrderno,sizeof(wp_payin_hz->F_orderno),wp_payin_hz->F_orderno);
	/************************************************************************/
	memcpy(wp_payin_hz->F_txnum,wp_payin->F_txnum,
		sizeof(wp_payin_hz->F_txnum));
	/**����Ӧ������һ�£���ԭ�������кŷŵ���ִ��Ľ������к��У�
	��ԭ�������кţ��ŵ���ִ���׵ķ��������к���**/	
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

	/****Modified by xyy û�������°�,Ӧ�������packid��packday�ÿգ���������Ҫ��ԭ���׵�packday
	��packday�ŵ�lv_pkgreg���е�opackid��opackday��,������ʱ�����������ֶ���****/
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
/* ��������: int iLvCreateTh(PAY_IN_AREA *wp_payin,PAY_IN_AREA *wp_payin_th)    */
/* ��������: ����payin���ݲ���һ���µ��˻��¼                                  */
/* ��������: 2005.12.2                                                       			*/
/* ����:     SSH                                                               		*/
/* ˵��:     ֻ��������򣬱����ݿ��Ե���newrec����������       						 			*/
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
		vtcp_log("%s,%d,������[%.3s]û���˻�  ֻ�д���ҵ��ſ��˻�! \n",
			__FL__,wp_payin->F_pkgno);
		sprintf(acErrMsg,"ֻ�д���ҵ��ſ��˻�!");
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
/* ��������: int iLvSetPayout1(PAY_IN_AREA  *wp_payin)                           */    
/* ��������: ������������ƴһ�������                                            */
/* ��������: 2005.12.2                                                           */
/* ����:     SSH                                                                 */
/* ˵��:                                                                         */
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
/*******���Ӹ�������**************/
/***************************************************************************/
/* ��������: iLvSetAddit                                                   */
/* ��������: ���Ӹ���������                                                */
/* ����:                                                                   */
/*            wp_payin   -֧������ͨ������ṹ��                           */
/*            name       -���������������                                 */
/*            buf        -���������������                                 */
/*            length     -����������ĳ���                                 */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     ʯ����                                                        */
/* ˵��:     �������ݵ�����Ϊ3���ֽڳ������ֻ�����ĸ, Ϊ000��ʾ�˸�����Ҫ  */
/*           ֱ��ƴ����������������,����Ϊ�����ڲ�����                     */
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
	vtcp_log("[%s][%d] ��Ҫ�������BUF==[%s]\n",__FL__,buf);
	if (strlen(cAddid)==0)
	{
		apitoa(iLvGetAddid(),sizeof(wd_lv_addit.addid)-1,wd_lv_addit.addid);
		vtcp_log("[%s][%d] wd_lv_addit.addid==[%s]\n",__FL__,wd_lv_addit.addid);
		memcpy(wd_lv_addit.tagname,name, sizeof(wd_lv_addit.tagname)-1);
		memcpy(wd_lv_addit.tagdata,buf,length);
		/****NEWYX2:���ó���****/
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
				/* ����ʯ��Ҫ�󣬶��ղ��������Ӹ����� 20060406*/
				/** NEWYX BEGIN:����BUG**/
				memcpy(wd_lv_addit.tagdata,buf,length);
				/** NEWYX END:����BUG**/
				/****NEWYX2:���ó���****/
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
			/******** ���û��ҲҪ���� dongxy 20060227 begin **********/
			if (iRc==100)
			{
				vtcp_log("[%s][%d] wd_lv_addit.addid==[%s]\n",__FL__,wd_lv_addit.addid);
				vtcp_log("[%s][%d] buf==[%s]\n",__FL__,buf);
				vtcp_log("[%s][%d] length==[%d]\n",__FL__,length);
				/* ����ʯ��Ҫ�󣬶��ղ��������Ӹ����� 20060406*/
				/** NEWYX BEGIN:����BUG**/
				memcpy(wd_lv_addit.tagdata,buf,length);
				/** NEWYX END:����BUG**/
				/****NEWYX2:���ó���****/
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
                                vtcp_log("[%s][%d] ���ر��α���˳�??\n",__FILE__,__LINE__);
				Lv_addit_Clo_Upd(); /**add by NEWBP5***/
				return 0;
			}
			else
			{
				/******** ���û��ҲҪ���� dongxy 20060227 end **********/
				vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FL__,iRc);
				Lv_addit_Clo_Upd(); /**add by NEWBP5***/
				return(-1);
			}
		}
		vtcp_log("[%s][%d] buf==[%s]\n",__FL__,wd_lv_addit.tagdata);
		memcpy(wd_lv_addit.tagdata,buf,length);
		/****NEWYX2:���ó���****/
		wd_lv_addit.taglen=length;
		iRc=Lv_addit_Upd_Upd(wd_lv_addit, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d, Insert lv_addit error=%d   ���븽�����ݱ�ʧ��\n",__FL__,iRc);
			Lv_addit_Clo_Upd(); /**add by NEWBP5**/
			return(-1);
		}
	}
	Lv_addit_Clo_Upd();/**add by NEWBP5**/
	memcpy(wp_payin->T_addid,wd_lv_addit.addid,sizeof(wp_payin->T_addid));
	vtcp_log("[%s][%d] wp_payin->T_addid==[%s]\n",__FL__,wp_payin->T_addid);
	return(0);
}
/*******1.3.7.    �õ���������**************/
/***************************************************************************/
/* ��������: iLvGetAddit                                                   */
/* ��������: �õ�����������                                                */
/* ����:                                                                   */
/*            wp_payin   -֧������ͨ������ṹ��                           */
/*            name       -���������������                                 */
/*            buf        -���������������                                 */
/*            length     -����������ĳ���                                 */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     ʯ����                                                        */
/* ˵��:     �������ݵ�����Ϊ3���ֽڳ������ֻ�����ĸ                       */
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
		vtcp_log("%s,%d,�����������ݱ�ʧ��,��Ϊ������%d\n",__FL__,iRc);
		return(0);
	}
	/****NEWYX2:�����г���ȡ,������ո�****/
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
/************************|��ʵ�ֵ��ڲ�����|***************************/
/************************|----------------|***************************/
/*********************************************************************/

/*******************************************/
/* ��������: char* pcLvGetDay()            */
/* ��������: �õ�С��ϵͳ����            	   */
/* ��������: 2005.12.2               			   */
/* ����:     SSH                       		   */
/* ˵��:          								  				 */
/* 2006.4.8�޸�,������ʱ��workdate�⣬�����ط���ȡ�ۺ�ҵ������****/
/*******************************************/
char* pcLvGetDay()
{
	static char txday[9];
	/*FD5��ŷ�������*/
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
/* ��������: char* pcLvGetNextDay()        */
/* ��������: �õ�С��ϵͳ����һ��        	   */
/* ��������: 2005.12.2               			   */
/* ����:     SSH                       		   */
/* ˵��:          								  				 */
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
		vtcp_log("%s,%d here��������=[%d]!\n",__FL__,lv_sysctl.next_date);
		sprintf(sql_txday, "%d",lv_sysctl.next_date);
	}
	return (sql_txday)	;
}
/*******************************************/
/* ��������: char* pcLvGetChgNode()        */
/* ��������: �õ�С��chgnode            	   */
/* ��������: 2005.12.2               			   */
/* ����:     SSH                       		   */
/* ˵��:          								  				 */
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
/* ��������: char* pcLvGetCurStat()        */
/* ��������: �õ�С��curstat            	   */
/* ��������: 2005.12.2               			   */
/* ����:     SSH                       		   */
/* ˵��:          								  				 */
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
/* ��������: char* pcLvGetChgTime()        */
/* ��������: �õ�С��chgtime            	   */
/* ��������: 2005.12.2               			   */
/* ����:     SSH                       		   */
/* ˵��:          								  				 */
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
/* ��������: char* pcLvGetMsg()            */
/* ��������: �õ�С��message            	   */
/* ��������: 2005.12.2               			   */
/* ����:     SSH                       		   */
/* ˵��:          								  				 */
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
/* ��������: int iUpdLvCheckstat(chat stat) */
/* ����lv_sysctl���еĶ���״̬              */
/* ��������: 2006-11-2 15:08                */
/* ����: LiuHuafeng                         */
/* ˵��:                                    */
/********************************************/
int iUpdLvCheckstat(char stat)
{
	struct lv_sysctl_c sLv_sysctl;
	memset(&sLv_sysctl, 0 , sizeof(sLv_sysctl));
	g_reply_int=Lv_sysctl_Dec_Upd(g_pub_tx.reply,"1=1");
	if (g_reply_int)
	{
		vtcp_log("%s,%d ��ȡϵͳ״̬����ִ���%d",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	g_reply_int=Lv_sysctl_Fet_Upd(&sLv_sysctl,g_pub_tx.reply);
	if (g_reply_int)
	{
		vtcp_log("%s,%d ��ȡϵͳ���α���ִ���%d",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	sLv_sysctl.checkstat[0]=stat;
	g_reply_int=Lv_sysctl_Upd_Upd(sLv_sysctl,g_pub_tx.reply);
	if (g_reply_int)
	{
		vtcp_log("%s,%d ����ϵͳ����ִ���%d",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	return 0;
}

/****�ж�һ���ڲ��ʺţ�ȡ�����ʺţ��Ƿ�����͸֧**/
/*BOOL iLvCanAccTz(PAY_IN_AREA *wp_payin){  */ /*ȥ��gujy 060829*/

/******************************************************************/
/* ��������: int iLvCountFeeamt(PAY_IN_AREA *wp_payin)            */
/* ��������: ���������ѽ��              	  												*/
/* ��������: 2005.12.2               			  												*/
/* ����:     SSH                       		  												*/
/* ˵��:          								  																*/
/******************************************************************/
int iLvCountFeeamt(PAY_IN_AREA *wp_payin)
{
	if (wp_payin->F_feeflag[0]!='\0' && wp_payin->F_feeflag[0]!=FEEFLAG_NO)
	{
		/***��ʼ����**/
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
		sprintf(acErrMsg,"ִ��Point_point_mst_Dec_Upd��[%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf(acErrMsg,"��ѯС��֧����������ű����![%d]",ret);
		WRITEMSG
			goto ErrExit;
	}
	else if (ret)
	{
		sprintf(acErrMsg,"ִ��seqctl_Fet_Upd ��[%d]",ret);
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
		sprintf(acErrMsg,"�޸Ļ����������!");
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		sprintf(acErrMsg,"ȡ��֧�����׸�������ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
		return seqctl_c.seqno;
	ErrExit:
		sprintf(acErrMsg,"ȡ��֧�����׸��������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
		return -1;
}
/******************************************************************/
/* ��������: int iLvGetAddid()    											            */
/* ��������: �õ���һ������������id,�뽻�׻����޹�    							*/
/* ��������: 2005.12.2               			  												*/
/* ����:     SSH                       		  												*/
/* ˵��:          								  																*/
/******************************************************************/
int iLvGetAddid()
{
	return iLvGetSeqno(HV_BR_NO, LV_ADDID_IND);
}
/******************************************************************/
/* ��������: int iLvGetPackid()    											            */
/* ��������: �õ���һ����id,�뽻�׻����޹�    											*/
/* ��������: 2005.12.2               			  												*/
/* ����:     SSH                       		  												*/
/* ˵��:          								  																*/
/******************************************************************/
int iLvGetPackid()
{
	return iLvGetSeqno(HV_BR_NO, LV_PACKID_IND);

}
/******************************************************************/
/* ��������: int iLvGetOrderno()                                  */
/* ��������: �õ���һ��֧���������,�뽻�׻������                */
/* ��������: 2005.12.2                                            */
/* ����:     SSH                                                  */
/* ˵��:                                                          */
/******************************************************************/
int iLvGetOrderno()
{
	char cBrno[6];

	memset(cBrno, 0x0, sizeof(cBrno));
	get_fd_data("0030", cBrno);

	return iLvGetSeqno(cBrno, LV_ACTYPE);
}
/****************************************************************************************/
/* ��������: int iLvPayinAutoInfo(PAY_IN_AREA *wp_payin)                                */
/* ��������: �õ�֧���������ݽ��������Զ���Ϣ:��ˮ�š�С�����ںͷ����к�    							*/
/* ��������: 2005.12.2               			  																							*/
/* ����:     SSH                       		  																							*/
/* ˵��:          								  																											*/
/****************************************************************************************/
int iLvPayinAutoInfo(PAY_IN_AREA *wp_payin)
{
	char cBrno[12];

	memset(cBrno, 0x0, sizeof(cBrno));
	get_fd_data("0030", cBrno);
	memcpy(wp_payin->A_tbsdy,pcLvGetDay(),sizeof(wp_payin->A_tbsdy));/*����*/
	vtcp_log("[%s][%d]wp_payin->F_wtdaty=[%s]",__FL__,wp_payin->F_wtday);
	get_fd_data("0040", wp_payin->A_wssrno);

	/****����orbrno*****/
	/**Modified by SSH,��ִ���ײ��Զ���д�����к�****/
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
/* ��������: int iLvZipAll(PAY_IN_AREA *wp_payin)        */
/* ��������: ѹ�����пո�    														*/
/* ��������: 2005.12.2               			  						*/
/* ����:     SSH                       		  						*/
/* ˵��:          								  										*/
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
/* ��������: char *pcPayinTxname(PAY_IN_AREA *wp_payin) */
/* ��������: �õ����׵�������Ϣ    												*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:          								  											*/
/********************************************************/
char *pcPayinTxname(PAY_IN_AREA *wp_payin)
{
	static char txname[21];
	memset(txname,'\0',sizeof(txname));
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"��ͨ����");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSDJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"ʵʱ����[ͨ��]");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"���ڴ���");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"ʵʱ���ǻ�ִ");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_PTJJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"��ͨ���");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSJJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"ʵʱ���");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DQJJ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"���ڽ��");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"���ڽ�ǻ�ִ");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"ʵʱ��ǻ�ִ");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"��ͨ��ǻ�ִ");
	}else if(memcmp(wp_payin->F_pkgno,PKGNO_DJTH,
	sizeof(wp_payin->F_pkgno))==0)
	{
		strcpy(txname,"�����˻�");
	}else
	{
		strcpy(txname,"δ֪ҵ��");
	}
	if (memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0)
	{
		strcat(txname,"¼��");
	}else if(memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))==0)
	{ 
		if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJ, sizeof(wp_payin->F_pkgno))==0)
		{
			strcat(txname,"����");	
		}	 
		else	
			strcat(txname,"����");
	}else if(memcmp(wp_payin->F_opcd,OPCD_SEND,sizeof(wp_payin->F_opcd))==0)
	{
		strcat(txname,"����");
	}else if(memcmp(wp_payin->F_opcd,OPCD_RECEIVE,
	sizeof(wp_payin->F_opcd))==0)
	{
		strcat(txname,"����");
	}else
	{
		strcat(txname,"δ֪����");
	}
	return(txname);
}
/********************************************************/
/* ��������: char* pcPayinRetdtl(PAY_IN_AREA *wp_payin) */
/* ��������: �õ��˻���������     												*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:          								  											*/
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
	vtcp_log("%s,%d,�˻�ԭ�����:%02d\n",__FL__,iRetno);
	switch(iRetno)
	{
	case 1:
		strcpy(pcRetdtl,"�ʺŲ�����");
		break;
	case 2:
		strcpy(pcRetdtl,"�ʺŻ�������");
		break;
	case 3:
		strcpy(pcRetdtl,"�ʻ�����֧��");
		break;
	case 10:
		strcpy(pcRetdtl,"�ʻ��������");
		break;
	case 11:
		strcpy(pcRetdtl,"�ʻ�״̬����");
		break;
	case 20:
		strcpy(pcRetdtl,"ҵ���ѳ���");
		break;
	default:
		strcpy(pcRetdtl,"��������");
	}
	return(pcRetdtl);
}
/********************************************************/
/* ��������: char *pcLvPayinTxtype(char *txnum)         */
/* ��������: �õ�ҵ�����͵�����    												*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:          								  											*/
/********************************************************/
char *pcLvPayinTxtype(char *txnum)
{
	static char pcTxtype[13];
	memset(pcTxtype,'\0',sizeof(pcTxtype));
	vtcp_log("[%s][%d] txnum==[%.5s]\n",__FL__,txnum);
	if (memcmp(txnum,TXNUM_HD,5)==0)
	{
		strcpy(pcTxtype,"С����");
	}	else if(memcmp(txnum,TXNUM_ZJTH,5)==0)
	{
		strcpy(pcTxtype,"����ֱ���˻�");
	}else if(memcmp(txnum,TXNUM_SQTH,5)==0)
	{
		strcpy(pcTxtype,"������Ȩ�˻�");
	}else if(memcmp(txnum,TXNUM_GZDJ,5)==0)
	{
		strcpy(pcTxtype,"��ծ�Ҵ�����");
	} else if (memcmp(txnum,TXNUM_TSCF,5)==0)
	{
		strcpy(pcTxtype,"С�����ճи�");
	}else if(memcmp(txnum,TXNUM_WT,5)==0)
	{
		strcpy(pcTxtype,"С��ί���տ�");
	}else if(memcmp(txnum,TXNUM_GKDJ,5)==0)
	{
		strcpy(pcTxtype,"������ǻ���");
	}else if(memcmp(txnum,TXNUM_QTDJ,5)==0)
	{
		strcpy(pcTxtype,"��������ҵ��");
	}else if(memcmp(txnum,TXNUM_JS,5)==0)
	{
		strcpy(pcTxtype,"��˰");
	}else if(memcmp(txnum,TXNUM_FSSR,5)==0)
	{
		strcpy(pcTxtype,"��˰����");
	}else if(memcmp(txnum,TXNUM_JF,5)==0)
	{
		strcpy(pcTxtype,"�ɷ�");
	}else if(memcmp(txnum,TXNUM_GKJJ,5)==0)
	{
		strcpy(pcTxtype,"�����ǻ���");
	}else if(memcmp(txnum,TXNUM_QTJJ,5)==0)
	{
		strcpy(pcTxtype,"�������ҵ��");
	}else if(memcmp(txnum,TXNUM_ZJZF,5)==0)
	{
		strcpy(pcTxtype,"����ֱ��֧��");
	}else if(memcmp(txnum,TXNUM_SQZF,5)==0)
	{
		strcpy(pcTxtype,"������Ȩ֧��");
	}else if(memcmp(txnum,TXNUM_ZPJL,5)==0)
	{
		strcpy(pcTxtype,"֧Ʊ����ҵ��");
		/****NEWYX BEGIN:YXJL****/
	}else if(memcmp(txnum,TXNUM_TYJL,5)==0)
	{
		strcpy(pcTxtype,"ͨ�ý���ҵ��");
		/****NEWYX END:YXJL****/
	}else if(memcmp(txnum,TXNUM_GZJJ,5)==0)
	{
		strcpy(pcTxtype,"��ծ��ǻ���");
	}else if(memcmp(txnum,TXNUM_DJTH,5)==0)
	{
		strcpy(pcTxtype,"�����˻�ҵ��");
	}else if(memcmp(txnum,TXNUM_DQDJ,5)==0)
	{
		strcpy(pcTxtype,"���ڴ���ҵ��");
	}else if(memcmp(txnum,TXNUM_DQJJ,5)==0)
	{
		strcpy(pcTxtype,"���ڽ��ҵ��");
	}else if(memcmp(txnum,TXNUM_PLKS,5)==0)
	{
		strcpy(pcTxtype,"������˰ҵ��");
	}	else if(memcmp(txnum,TXNUM_TC,5)==0)
	{
		strcpy(pcTxtype,"ͨ��ҵ��");
	}	else if(memcmp(txnum,TXNUM_SSKS,5)==0)
	{
		strcpy(pcTxtype,"ʵʱ��˰ҵ��");
	}else if(memcmp(txnum,TXNUM_TD,5)==0)
	{
		strcpy(pcTxtype,"����ͨ��ҵ��");
	}else if(memcmp(txnum,TXNUM_DGTD,5)==0)
	{
		strcpy(pcTxtype,"�Թ�ͨ��ҵ��");
	}else if(memcmp(txnum,TXNUM_TYXX,5)==0)
	{
		strcpy(pcTxtype,"ͨ����Ϣҵ��");
	}else if(memcmp(txnum,TXNUM_DS,5)==0)
	{
		strcpy(pcTxtype,"����ҵ��");
	}else if(memcmp(txnum,TXNUM_DF,5)==0)
	{
		strcpy(pcTxtype,"����ҵ��");
	}else if(memcmp(txnum,TXNUM_ZPQC,5)==0)
	{
		strcpy(pcTxtype,"֧ƱȦ��ҵ��");
	}else if(memcmp(txnum,TXNUM_ZHCX,5)==0)
	{
		strcpy(pcTxtype,"�ʻ���ѯҵ��");
	}else if(memcmp(txnum,TXNUM_CXYD,5)==0)
	{
		strcpy(pcTxtype,"�ʻ���ѯӦ��");
	}else if(memcmp(txnum,TXNUM_ZSCX,5)==0)
	{
		strcpy(pcTxtype,"CFCA֤���ѯ");
	}else if(memcmp(txnum,TXNUM_ZSYD,5)==0)
	{
		strcpy(pcTxtype,"CFCA��ѯӦ��");
	}else if(memcmp(txnum,TXNUM_ZPYD,5)==0)
	{
		strcpy(pcTxtype,"֧ƱȦ���Ӧ");
	}else
	{
		strcpy(pcTxtype,"δ֪ҵ��");
		vtcp_log("[%s][%d]pcTxtype=[%s]txnum=[%s]\n",__FL__,pcTxtype,txnum);
	}
	return(pcTxtype);
}
/********************************************************/
/* ��������: char *pcLvPayinYwtype(char *pywtype)       */
/* ��������: �õ�ҵ�����������    											*/
/* ��������: 2005.12.2               			  						*/
/* ����:     SSH                       		  						*/
/* ˵��:          								  										*/
/********************************************************/
char *pcLvPayinYwtype(char *pywtype)
{
	static char pcYwtype[13];
	memset(pcYwtype,'\0',sizeof(pcYwtype));
	if (memcmp(pywtype,"11",2)==0)
	{
		strcpy(pcYwtype,"��ͨ���");
	}else if(memcmp(pywtype,"10",2)==0)
	{
		strcpy(pcYwtype,"�ֽ���");
	}else if(memcmp(pywtype,"30",2)==0)
	{
		strcpy(pcYwtype,"�������");
	}else if(memcmp(pywtype,"40",2)==0)
	{
		strcpy(pcYwtype,"�ʽ����");
	}else if(memcmp(pywtype,"50",2)==0)
	{
		strcpy(pcYwtype,"������");
	}else if(memcmp(pywtype,"51",2)==0)
	{
		strcpy(pcYwtype,"����ͬ�Ǿ���");
	}else if(memcmp(pywtype,"61",2)==0)
	{
		strcpy(pcYwtype,"����Ʊ������");
	}else
	{
		memcpy(pcYwtype,pywtype,sizeof(pcYwtype)-1);
	}
	return(pcYwtype);
}
/***************************************************************************/
/* ��������: iAmtCmp                                                       */
/* ��������: �Ƚ�����������ֶ�                                            */
/* ����:                                                                   */
/*            pcNum1   -��1��������ֶ�                                    */
/*            pcNum2   -��2��������ֶ�                                    */
/*            iSize    -�Ƚϳ���                                           */
/* ����ֵ:   0:���  1:pcNum1>pcNum2 -1:pcNum1<pcNum2                      */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
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
	/***CAUTION:�ڴ˲��������뽻����Ĵ���***/

	return pcDesCode;
}
char * pcUndesPswdCode(char *code)
{
	static char pcUndesCode[9];
	memset(pcUndesCode,'\0',sizeof(pcUndesCode));
	memset(pcUndesCode,'0',sizeof(pcUndesCode)-1);
	/***CAUTION:�ڴ˲������������ĵĴ���****/

	return(pcUndesCode);
}
/********************************************************/
/* ��������: int iLvDateStat()                          */
/* ��������: �õ�С��ҵ���Ƿ��յı�־      							*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:    1-�Ѿ����� 0 -û�л��� �������ݿ����    			*/
/********************************************************/
int iLvDateStat()
{
	char cG_txday[9];
	char cL_txday[9];

	/****CAUTION:test******/
	return(0);

}
/********************************************************/
/* ��������: char* pcLvGetRecvno()                      */
/* ��������: ���ݵ�ǰ�����õ����ղ���Ա��     						*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:                                             			*/
/********************************************************/
/*??Gc*/
char* pcLvGetRecvno()
{
	return("00000");
}
/*****************���payout��********add by liuyue 20051103*******
*************************������************************************/
int print_pay_out1(PAY_OUT1_AREA *pay_out1)
{
	return(0);
}
int print_payin_ly(PAY_IN_AREA *wp_payin)
{
	vtcp_log("[%s][%d]��ʼ��ӡpayin�ṹ��!\n",__FILE__,__LINE__);
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

	vtcp_log("[%s][%d]��ʼ��ӡpayin�ṹ�� ���!\n",__FILE__,__LINE__); 
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
* �õ�ͨ��ͨ��ҵ��Ŀ����е�������                         *
* ���Ե�ʱ����ʱ����1%��ȡ��������δ�����Ҫ���������Э�� *
***********************************************************/
double dLvGetOpenFee(double txamt,char flag)
{
	/* ��ˮ�շѱ�׼ */
	return 600.0;
}
/***********************************************************
* �õ�ͨ��ͨ��ҵ��Ĵ����е�������                         *
* ���Ե�ʱ����ʱ����1%��ȡ��������δ�����Ҫ���������Э�� *
***********************************************************/
double dLvGetAgentFee(double txamt,char flag)
{
	/* ��ˮ�շѱ�׼ */
	return 600.0;
}
#if 0
/********************************************************/
/* ��������: int iLvGetPrgStat(char *prgname)           */
/* ��������: ����ʱƴ������ĵײ㺯��       						*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:                                             			*/
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
		vtcp_log("%s,%d,δ�ҵ�ָ������sqlcode=%d\n",__FL__,ret);
		return(-1);
	}	
	if (ret!=0)
	{
		vtcp_log("%s,%d,���ݿ����,sqlcode=%d\n",__FL__,ret);
		return(-1);
	}
	return(lv_prgnm.prgstat[0]);
}
/********************************************************/
/* ��������: int iLvGetPrgId(char *prgname)             */
/* ��������: �õ�pid               						*/
/* ��������: 2005.12.2            						*/
/* ����:     SSH                   						*/
/* ˵��:                                       			*/
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
		vtcp_log("%s,%d,δ�ҵ�ָ������sqlcode=%d\n",__FL__,ret);
		return(-1);
	}	
	if (ret!=0)
	{
		vtcp_log("%s,%d,���ݿ����,sqlcode=%d\n",__FL__,ret);
		return(-1);
	}
	return(lv_prgnm.prgpid);
}
/***************************************************************************/
/* ��������: iLvSetPrg                                                     */
/* ��������: ���ú�̨���������״̬                                        */
/* ����:                                                                   */
/*            pcPrgName-�������������                                     */
/*            cPrgStat-���������״̬                                      */
/*            iPid     -��������Ľ��̺�                                   */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
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
		vtcp_log("%s,%d,���ݿ����,sqlcode=%d\n",__FL__,ret);
		return(-1);
	}
	ret = Lv_prgnm_Fet_Upd(&lv_prgnm, g_pub_tx.reply);
	if (DB_OK != ret)
	{
		vtcp_log("%s,%d,���ݿ����,sqlcode=%d\n",__FL__,ret);

		return(-1);
	}
	sprintf(lv_prgnm.prgstat, "%c", cPrgStat);
	lv_prgnm.prgpid = iPid;
	ret = Lv_prgnm_Upd_Upd(lv_prgnm, g_pub_tx.reply);
	if (DB_OK != ret)
	{
		vtcp_log("%s,%d,���ݿ����,sqlcode=%d\n",__FL__,ret);

		return(-1);
	}
	return(0);
}
#endif

/********************************************************/
/* ��������: int zipn_space(char *s,int len)            */
/* ��������: ѹ��ָ�����ȵ��ַ����еĿո�     						*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:                                             			*/
/********************************************************/
int zipn_space(char *s,int len)
{
	int i=0,j=0;
	i=j=0;
	/***Modified by SSH,2006.5.9,��ֹ��������****/
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
/* ��������: char *strtokk(char *s,char *substr)        */
/* ��������: �ҵ�substr,�����Ӵ�substr�滻��'\0'        */
/*    				 ���û���ҵ�,����ԭ�ַ���          				*/
/* ��������: 2005.12.2               			  							*/
/* ����:     SSH                       		  							*/
/* ˵��:                                             			*/
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
/* ��������: int iLvCheckStat()                         */
/* ��������: ���Ӷ�С��ϵͳ״̬���ж�                   */
/* ��������: 2005.12.2                                  */
/* ����:     SSH                                        */
/* ˵��:                                                */
/********************************************************/
int iLvCheckStat()
{
	int iRc=0;
	char cPtr[2];
	memset(cPtr, 0 , sizeof(cPtr));
	iRc=iLvGetSysStat(cPtr);	
	if (iRc)
	{
		vtcp_log("%s,%d,С��ϵͳ״̬�����\n",__FL__);
		return(-1);
	}
	vtcp_log("%s,%d out from pcLvGetSysStat ",__FL__);
	vtcp_log("%s,%d out from pcLvGetSysStat ptr=[%s] ",__FL__,cPtr);
	vtcp_log("%s,%d out from pcLvGetSysStat ptr=[%c] ",__FL__,cPtr[0]);
	vtcp_log("%s,%d out from pcLvGetSysStat ptr=[%c] ",__FL__,cPtr[1]);
	if (cPtr[0]!='1')
	{
		vtcp_log("%s,%d out from pcLvGetSysStat ",__FL__);
		vtcp_log("%s,%d,С��ϵͳ״̬[%c]����[%s]\n",
			__FL__,cPtr[0],cPtr);
		return(-1);
	}
	return(0);
}
/*****************************************************************************************************/
/* ��������: int lv_orderno_add0(char *s_string, int length, char *d_string)                         */
/* ��������: ���ڰѲ���lengthλ��cmtno����Ϊlengthλ���ַ���                                         */
/* ��������: 2006-02-15                                                                              */
/* ����:     ���յ�                                                                                  */
/* ˵��:     char *s_string ----------------- source string                                          */
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
/* ��������: int logwrite(PAY_IN_AREA* p_pay_in)                                                     */
/* ��������: ��PAY_IN�ṹ������Ҫ����Ϣд��log�ļ�����    		            														 */
/* ��������: 2006-02-25             			  																													 */
/* ����:     ���յ�                    		  																													 */
/* ˵��:     ��ע���޸� FileName��·��                           																			 */
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
/* ��������: int iCheckHzdate(char *cashqsactno,int daycnt)                                          */
/* ��������: ��֤��ִ����                                  		            														 */
/* ��������: 2006-02-11             			  																													 */
/* ����:     SSH                    		  																													   */
/* ˵��:                                                        																			 */
/*****************************************************************************************************/
int iCheckHzdate(char *cashqsactno,int daycnt)
{
	struct lv_param_c wd_lv_param;
	char pcRececo[5];
	char pcSendco[5];
	int  nMaxDayCnt=0;/**����ִ����****/
	int  nMinCityCnt=0;/***ͬ����С��ִ����****/
	int  nMinGlobalCnt=0;/****�����С��ִ����****/
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
		vtcp_log("%s,%d,���ݿ����[%d]\n",__FL__,iRc);
		return(-1);
	}
	if (iRc==100)
	{
		vtcp_log("%s,%d,δ�ҵ��������ִ����,ȡ5\n",__FL__);
		nMaxDayCnt=5;
	}else
	{
		nMaxDayCnt=atoi(wd_lv_param.par_value);
	}
	if (memcmp(pcSendco,pcRececo,sizeof(pcSendco)-1)==0)
	{
		/****ͬ��ҵ��****/
		memcpy(wd_lv_param.par_code,"TDBRTDAY",8);

	}else
	{
		/****���ҵ��****/
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
		vtcp_log("%s,%d,��ؽ�ǻ�ִ���ޱ�����%d��%d֮��--%d\n",__FL__,nMinCityCnt,nMaxDayCnt,daycnt);
		sprintf(acErrMsg,"��ؽ�ǻ�ִ���ޱ�����%d��%d֮��",nMinCityCnt,nMaxDayCnt);
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
		vtcp_log("[%s][%d]�õ������ߺŴ���!",__FL__);
		sprintf(acErrMsg,"�õ������ߺŴ���!");
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
	if (cReqtype=='1') /* ���������� */
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
/***Add by SSH,2006.4.19,����ַ������Ƿ��к���****/
/****0-û��,1-��****/
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
/* ��������: int iLvGetOrient(PAY_IN_AREA *wp_payin)*/
/* ��������: �ж�һ�ʽ����Ƿ��ͽ��׻��ǽ��ս���*/
/* ��������: 2005.07.20*/
/* ����:     SSH*/
/* ˵��:
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
*��������:iLvPayin_characterCheck                               *
*��������:���֧���ཻ�׻�������ַ��������Ŀ�Ƿ��зǷ��ַ�����  *
*��������: ���룺HV_PAYIN_AREA *hv_payin ���֧���ཻ�׽ṹ��   *
*          �����char * errsmg           ������Ϣ����           *
*                                                               *
*                                                               *
*��Ƶ���:                                                      *
*��    ��:������ 2006-8-26 16:20                                *
****************************************************************/
int iLvPayin_characterCheck(PAY_IN_AREA *lv_payin,char * msg)
{
	int iRet=0;
	iRet=iCharacterType(lv_payin->F_payname,sizeof(lv_payin->F_payname),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,����ĸ���������[%.60s]�е�%dλ���ڷǷ��ַ�",__FILE__,__LINE__,lv_payin->F_payname,iRet);
		strncpy(msg,"����ĸ����������д��ڷǷ��ַ�",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_cashname,sizeof(lv_payin->F_cashname),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,������տ�������[%.60s]�е�%dλ���ڷǷ��ַ�",__FILE__,__LINE__,lv_payin->F_cashname,iRet);
		strncpy(msg,"������տ��������д��ڷǷ��ַ�",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_payaddress,sizeof(lv_payin->F_payaddress),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,����ĸ����˵�ַ[%.60s]�е�%dλ���ڷǷ��ַ�",__FILE__,__LINE__,lv_payin->F_payaddress,iRet);
		strncpy(msg,"����ĸ����˵�ַ�д��ڷǷ��ַ�",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_cashaddress,sizeof(lv_payin->F_cashaddress),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,������տ��˵�ַ[%.60s]�е�%dλ���ڷǷ��ַ�",__FILE__,__LINE__,lv_payin->F_cashaddress,iRet);
		strncpy(msg,"������տ��˵�ַ�д��ڷǷ��ַ�",50);
		return iRet;
	}
	iRet=iCharacterType(lv_payin->F_content,sizeof(lv_payin->F_content),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,����ĸ�����[%.60s]��%dλ���ڷǷ��ַ�",__FILE__,__LINE__,lv_payin->F_content,iRet);
		strncpy(msg,"����ĸ����д��ڷǷ��ַ�",50);
		return iRet;
	}
	return 0;
}
/****NEWYX BEGIN:���ո�ĺ���****/
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
	if(memcmp(ps->ptype,"05",2)!=0)/**YHBP����������б�Ʊ�Ļ����ٲ��ո�**/
	{
	    pad_field(ps->reqname,sizeof(ps->reqname));/**����������**/
	    pad_field(ps->cashbrname,sizeof(ps->cashbrname));/**�տ��˿���������**/
	    pad_field(ps->content,sizeof(ps->content));/**����**/
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
/****NEWYX END:���ո�ĺ���****/


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
/****Add by SSH,2007.5.15,����֧Ʊҵ���ĵ���ӡ������*****/
/****Add by SSH,2007.5.15,����֧Ʊҵ���ĵ���ӡ������*****/
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
	pack = &cifpack;  /***���Ľṹ****/
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
			vtcp_log("[%s][%d] ȡ������1ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������1ʧ��!\n");
			return(-1);
		}
		vtcp_log("%s,%d,Gethere,pack=[%s]\n",__FILE__,__LINE__,&wd_zp_add1);
		pad_zpjl1(&wd_zp_add1);
		iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������2ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������2ʧ��!\n");
			return(-1);
		}
		pad_zpjl2(&wd_zp_add2);
		iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_zp_add3,sizeof(wd_zp_add3));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������3ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������3ʧ��!\n");
			return(-1);
		}
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0){
		iRc=iLvGetAddit(wp_payin,"001",(char *)&wd_ty_add1,sizeof(wd_ty_add1));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������1ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������1ʧ��!\n");
			return(-1);
		}
		iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������2ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������2ʧ��!\n");
			return(-1);
		}
		pad_zpjl2(&wd_zp_add2);
		iRc=iLvGetAddit(wp_payin,"004",(char *)&wd_ty_add4,sizeof(wd_ty_add4));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������4ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������4ʧ��!\n");
			return(-1);
		}
		pad_tyjl1(&wd_ty_add1);
		pad_tyjl4(&wd_ty_add4);

		iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_ty_add3,sizeof(wd_ty_add3));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������3ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������3ʧ��!\n");
			return(-1);
		}
		/****pad_tyjl3(&wd_ty_add3);*******/
		iRc=iLvGetAddit(wp_payin,"005",(char *)&wd_ty_add5,sizeof(wd_ty_add5));
		if(iRc){
			vtcp_log("[%s][%d] ȡ������5ʧ��\n",__FL__);
			sprintf(acErrMsg,"ȡ������5ʧ��!\n");
			return(-1);
		}
	}
	/************����cif����******************/
	vInitCifPack(pack);
#define ADDCIFTAG(pack,tag,data) {	char *p; \
									p=malloc(sizeof(data)+1); \
									memcpy(p,data,sizeof(data)); \
									p[sizeof(data)]='\0'; \
									vAddCifTag(pack,tag,p); \
									free(p); \
								}
	/***********����Ҫ��**********/
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
	ADDCIFTAG(pack,"59A",wp_payin->T_cashname);    /******����������******/
	ADDCIFTAG(pack,"BS6",wp_payin->T_respday);     /***��ִ����***/
	ADDCIFTAG(pack,"TXC","0100"); 		/***����ӵĸ�����,���״���(4)*****/
	ADDCIFTAG(pack,"DA0","00");                /***��ǩ״̬****/
	if(memcmp(wp_payin->T_txnum,"30102",5)==0){
	 /*******֧Ʊ����ҵ��Ҫ��*********/
		char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
		int  nBsrCnt=0;
		int  tmpdate=0,tmpdate1=0;
		memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
		memcpy(pack->head.cifno,"100",3);
		ADDCIFTAG(pack,"D35",wd_zp_add1.cpday);
		ADDCIFTAG(pack,"D01",wd_zp_add1.cpno);
		ADDCIFTAG(pack,"D37",wd_zp_add1.use);     /******��;****/
		ADDCIFTAG(pack,"D41",wd_zp_add2.bscnt);
		nBsrCnt=apatoi(wd_zp_add2.bscnt,2);
		if(nBsrCnt>0){
			memcpy(pcBsrBuffer,(char *)wd_zp_add2.bslist,nBsrCnt*60);
			vAddCifTag(pack,"D42",pcBsrBuffer);
		}
		ADDCIFTAG(pack,"D30",wd_zp_add3.bscheckpswd); 
		/**modify wyw 20070618  YZJҪ����ʾ��������,Ҫ���ڵ���С��ϵͳϵͳ����,������ʾ��������ȡ��Ʊ���ڼ�10**/
		tmpdate=apatoi(wd_zp_add1.cpday,8);
		vtcp_log("%s,%d ��ʾ��������:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
		pub_base_deadlineD(tmpdate,9,&tmpdate1);
		vtcp_log("%s,%d ��ʾ��������:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
		memset(tmpbuf,'\0',sizeof(tmpbuf));
		apitoa(tmpdate1,8,tmpbuf);
		vtcp_log("%s,%d ��ʾ��������:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
		vAddCifTag(pack,"D36",tmpbuf);	
	 }else{
		/********ͨ�ý���ҵ��Ҫ��*********/
		char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
		int  nBsrCnt=0;
		memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
		memcpy(pack->head.cifno,"101",3);
		ADDCIFTAG(pack,"D35",wd_ty_add1.cpday);
		ADDCIFTAG(pack,"D01",wd_ty_add1.cpno);
		ADDCIFTAG(pack,"D37",wd_ty_add1.use);     /******��;****/
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
	vtcp_log("[%s][%d]������ϵ�cis����[%d]:\n",__FILE__,__LINE__,strlen((char *)pack));
	vtcp_log("%s\n",(char *)pack);
	/**********����CIF�������*******/
	/****Add by SSH,2007.5.15,ת����׼cis��ʽΪ��֮�ܸ�ʽ****/
	vCif2yzj(pack,pcYzjBuffer,wp_payin->T_inwssrno);
	nYzjLength=strlen(pcYzjBuffer);
	vtcp_log("[%s][%d]ת��֮�����֮�ܱ���[%d]:\n",__FILE__,__LINE__,nYzjLength);
	vtcp_log("%s\n",(char *)pcYzjBuffer);
	/********************************************************/	

	memset(pcFileName,'\0',sizeof(pcFileName));
	sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
	memset(cifip,'\0',sizeof(cifip));
	/*******��ȡ��ӡ��������IP�Ͷ˿ں�*******/
 
	iRc=hv_readIni(pcFileName,"CIFSERVER","Address",cifip);
	if(iRc){
		vtcp_log("[%s][%d] ��ȡ�����ļ�����\n",__FL__);
		sprintf(acErrMsg,"��ȡ�����ļ�����!\n");
		return(-1);
	}
	vtcp_log("[%s][%d]CIF IP=[%s]\n",__FILE__,__LINE__,cifip);
	memset(tmp_port,'\0',sizeof(tmp_port));
	iRc=hv_readIni(pcFileName,"CIFSERVER","Port",tmp_port);
	if(iRc){
		vtcp_log("[%s][%d] ��ȡ�����ļ�����\n",__FL__);
		sprintf(acErrMsg,"��ȡ�����ļ�����!\n");
		return(-1);
	}
	cifport=apatoi(tmp_port,strlen(tmp_port));
	vtcp_log("[%s][%d]CIF PORT=[%s]\n",__FILE__,__LINE__,tmp_port);
	/*******������ӡ������********/
	iSocketID=Cif_connect(cifip,cifport);
	if(iSocketID<=0){
		vtcp_log("[%s][%d] ������ӡ������ʧ��!\n",__FL__);
		sprintf(acErrMsg,"������ӡ������ʧ��!\n");
		return(-1);
	}
	vtcp_log("socket=%d\n", iSocketID);
	/*******����CIF���ĵ���ӡ������****/
	vtcp_log("Try to send[%d]...\n",nYzjLength);
	iRc=Cif_send(iSocketID,pcYzjBuffer,nYzjLength);
	if(iRc<0){
		vtcp_log("[%s][%d] ���͵���ӡ������ʧ��!\n",__FL__);
		sprintf(acErrMsg,"���͵���ӡ������ʧ��!\n");
		Cif_close(iSocketID);
		return(-1);
	}
	vtcp_log("Send finished,try to recv...\n");
	iRc=Cif_recv(iSocketID,pcYzjBuffer,8);
	if(iRc<0){
		vtcp_log("iRc =%d\n",iRc);
		vtcp_log("[%s][%d] ������ӡ������ʧ��!\n",__FL__);
		sprintf(acErrMsg,"������ӡ������ʧ��!\n");
		Cif_close(iSocketID);
		return(-1) ;
	}
	vtcp_log("Recv finished...\n");
	pcYzjBuffer[8]='\0';
	nYzjLength=atoi(pcYzjBuffer);
	iRc=Cif_recv(iSocketID,pcYzjBuffer+8,nYzjLength-8);
	if(iRc<0){
		vtcp_log("iRc =%d\n",iRc);
		vtcp_log("[%s][%d] ������ӡ������ʧ��!\n",__FL__);
		sprintf(acErrMsg,"������ӡ������ʧ��!\n");
		Cif_close(iSocketID);
		return(-1) ;
	}
	/*******�ر�����*******/
	Cif_close(iSocketID);
	memcpy(status,pcYzjBuffer+79,3);
	vtcp_log("[%s][%d],**************���ص�״̬stat=[%s]\n",__FILE__,__LINE__,status);
	if(memcmp(status,"000",3)==0){
		iRc = iLvSetAddit(wp_payin,"QSX","1",1);
		if(iRc){
			vtcp_log("%s,%d,CIF���ķ��ͳɹ���,���÷��ͱ�־ʧ��!",__FILE__,__LINE__);
			sprintf(acErrMsg,"������ӡ������ʧ��!\n");
			return(-1);
		}
	}else{
		iRc = iLvSetAddit(wp_payin,"QSX","0",1);
		vtcp_log("%s,%d,CIF���ķ���ʧ��,��Ҫ���·���!",__FILE__,__LINE__);
		return(-1);
	} 
	vtcp_log("[%s][%d] ������ӡӦ����Ϣ,���¼�¼********************�ɹ�[%s]\n",__FILE__,__LINE__,status); 
	/****�ɹ�����cif������ȷ������Ӧ*****/
	return(0);
}
/****modiby by wyw,2007.7.15,����֧Ʊҵ���ĵ���ӡ������*****/
int SendZpCifToYy_HsZy(PAY_IN_AREA *wp_payin) /**HSYXר��**/
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
        pack = &cifpack;  /***���Ľṹ****/
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
                        vtcp_log("[%s][%d] ȡ������1ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������1ʧ��!\n");
                        return(-1);
                }
                vtcp_log("%s,%d,Gethere,pack=[%s]\n",__FILE__,__LINE__,&wd_zp_add1);
                pad_zpjl1(&wd_zp_add1);
                iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������2ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������2ʧ��!\n");
                        return(-1);
                }
                pad_zpjl2(&wd_zp_add2);
                iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_zp_add3,sizeof(wd_zp_add3));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������3ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������3ʧ��!\n");
                        return(-1);
                }
        }else if(memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0){
                iRc=iLvGetAddit(wp_payin,"001",(char *)&wd_ty_add1,sizeof(wd_ty_add1));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������1ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������1ʧ��!\n");
                        return(-1);
                }
                iRc=iLvGetAddit(wp_payin,"002",(char *)&wd_zp_add2,sizeof(wd_zp_add2));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������2ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������2ʧ��!\n");
                        return(-1);
                }
                pad_zpjl2(&wd_zp_add2);
                iRc=iLvGetAddit(wp_payin,"004",(char *)&wd_ty_add4,sizeof(wd_ty_add4));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������4ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������4ʧ��!\n");
                        return(-1);
                }
                pad_tyjl1(&wd_ty_add1);
                pad_tyjl4(&wd_ty_add4);

                iRc=iLvGetAddit(wp_payin,"003",(char *)&wd_ty_add3,sizeof(wd_ty_add3));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������3ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������3ʧ��!\n");
                        return(-1);
                }
                /****pad_tyjl3(&wd_ty_add3);*******/
                iRc=iLvGetAddit(wp_payin,"005",(char *)&wd_ty_add5,sizeof(wd_ty_add5));
                if(iRc){
                        vtcp_log("[%s][%d] ȡ������5ʧ��\n",__FL__);
                        sprintf(acErrMsg,"ȡ������5ʧ��!\n");
                        return(-1);
                }
        }
        /************����cif����******************/
        vInitCifPack(pack);
#define ADDCIFTAG(pack,tag,data) {      char *p; \
                                                                        p=malloc(sizeof(data)+1); \
                                                                        memcpy(p,data,sizeof(data)); \
                                                                        p[sizeof(data)]='\0'; \
                                                                        vAddCifTag(pack,tag,p); \
                                                                        free(p); \
                                                                }
        /***********����Ҫ��**********/
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
        ADDCIFTAG(pack,"59A",wp_payin->T_cashname);    /******����������******/
        ADDCIFTAG(pack,"BS6",wp_payin->T_respday);     /***��ִ����***/
        ADDCIFTAG(pack,"TXC","0100");           /***����ӵĸ�����,���״���(4)*****/
        memcpy(sendtlrno,g_pub_tx.tel,sizeof(g_pub_tx.tel)-1);/**����Ա**/
        if(sendtlrno[0]==0){
            get_zd_data(DC_TEL,sendtlrno);
        }
        vAddCifTag(pack,"RES",sendtlrno);
        ADDCIFTAG(pack,"DA0","00");                /***��ǩ״̬****/
        if(memcmp(wp_payin->T_txnum,"30102",5)==0){
         /*******֧Ʊ����ҵ��Ҫ��*********/
                char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
                int  nBsrCnt=0;
                int  tmpdate=0,tmpdate1=0;
                memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
                memcpy(pack->head.cifno,"100",3);
                ADDCIFTAG(pack,"D35",wd_zp_add1.cpday);
                ADDCIFTAG(pack,"D01",wd_zp_add1.cpno);
                ADDCIFTAG(pack,"D37",wd_zp_add1.use);     /******��;****/
                ADDCIFTAG(pack,"D41",wd_zp_add2.bscnt);
                nBsrCnt=apatoi(wd_zp_add2.bscnt,2);
                if(nBsrCnt>0){
                        memcpy(pcBsrBuffer,(char *)wd_zp_add2.bslist,nBsrCnt*60);
                        vAddCifTag(pack,"D42",pcBsrBuffer);
                }
                ADDCIFTAG(pack,"D30",wd_zp_add3.bscheckpswd); 
                vAddCifTag(pack,"D32","01");
                /**modify wyw 20070618  YZJҪ����ʾ��������,Ҫ���ڵ���С��ϵͳϵͳ����,������ʾ��������ȡ��Ʊ���ڼ�10**/
                tmpdate=apatoi(wd_zp_add1.cpday,8);
                pub_base_deadlineD(tmpdate,9,&tmpdate1);
                vtcp_log("%s,%d ��ʾ��������:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
                memset(tmpbuf,'\0',sizeof(tmpbuf));
                apitoa(tmpdate1,8,tmpbuf);
                vtcp_log("%s,%d ��ʾ��������:tmpbuf[%s]\n",__FILE__,__LINE__,tmpbuf);
                vAddCifTag(pack,"D36",tmpbuf);  
         }else{
                /********ͨ�ý���ҵ��Ҫ��*********/
                char pcBsrBuffer[sizeof(wd_zp_add2.bslist)+1];
                int  nBsrCnt=0;
                memset(pcBsrBuffer,'\0',sizeof(pcBsrBuffer));
                memcpy(pack->head.cifno,"101",3);
                ADDCIFTAG(pack,"D35",wd_ty_add1.cpday);
                ADDCIFTAG(pack,"D01",wd_ty_add1.cpno);
                ADDCIFTAG(pack,"D37",wd_ty_add1.use);     /******��;****/
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
        vtcp_log("[%s][%d]������ϵ�cis����[%d]:\n",__FILE__,__LINE__,strlen((char *)pack));
        vtcp_log("%s\n",(char *)pack);
        /**********����CIF�������*******/
        /****Add by SSH,2007.5.15,ת����׼cis��ʽΪ��֮�ܸ�ʽ****/
        /********************************************************/      

        memset(pcFileName,'\0',sizeof(pcFileName));
        sprintf(pcFileName,"%s/etc/cif.ini",getenv("WORKDIR"));
        memset(cifip,'\0',sizeof(cifip));
        /*******��ȡ��ӡ��������IP�Ͷ˿ں�*******/
 
        iRc=hv_readIni(pcFileName,"CIFSERVER","Address",cifip);
        if(iRc){
                vtcp_log("[%s][%d] ��ȡ�����ļ�����\n",__FL__);
                sprintf(acErrMsg,"��ȡ�����ļ�����!\n");
                return(-1);
        }
        vtcp_log("[%s][%d]CIF IP=[%s]\n",__FILE__,__LINE__,cifip);
        memset(tmp_port,'\0',sizeof(tmp_port));
        iRc=hv_readIni(pcFileName,"CIFSERVER","Port",tmp_port);
        if(iRc){
                vtcp_log("[%s][%d] ��ȡ�����ļ�����\n",__FL__);
                sprintf(acErrMsg,"��ȡ�����ļ�����!\n");
                return(-1);
        }
        cifport=apatoi(tmp_port,strlen(tmp_port));
        vtcp_log("[%s][%d]CIF PORT=[%s]\n",__FILE__,__LINE__,tmp_port);
        /*******������ӡ������********/
        iSocketID=Cif_connect(cifip,cifport);
        if(iSocketID<=0){
                vtcp_log("[%s][%d] ������ӡ������ʧ��!\n",__FL__);
                sprintf(acErrMsg,"������ӡ������ʧ��!\n");
                return(-1);
        }
        vtcp_log("socket=%d\n", iSocketID);
        /*******����CIF���ĵ���ӡ������****/
        iPacklength=apatoi(pack->head.length,sizeof(pack->head.length)); 
        vtcp_log("%s,%d Try to send[%d]...\n",__FILE__,__LINE__,iPacklength);
        iRc=Cif_send(iSocketID,pack,iPacklength);
        if(iRc<0){
                vtcp_log("[%s][%d] ���͵���ӡ������ʧ��!\n",__FL__);
                sprintf(acErrMsg,"���͵���ӡ������ʧ��!\n");
                Cif_close(iSocketID);
                return(-1);
        }
        vtcp_log("Send finished,try to recv...\n");
        iRc=Cif_recv(iSocketID,pack,9);
        if(iRc<0){
                vtcp_log("iRc =%d\n",iRc);
                vtcp_log("[%s][%d] ������ӡ������ʧ��!\n",__FL__);
                sprintf(acErrMsg,"������ӡ������ʧ��!\n");
                Cif_close(iSocketID);
                return(-1) ;
        }
        vtcp_log("Recv finished...\n");
        /*******�ر�����*******/
        Cif_close(iSocketID);
        memcpy(status,pack->head.status,3);
        vtcp_log("[%s][%d],**************���ص�״̬stat=[%s]\n",__FILE__,__LINE__,status);
        if(memcmp(status,"000",3)==0){
                iRc = iLvSetAddit(wp_payin,"QSX","1",1);
                if(iRc){
                        vtcp_log("%s,%d,CIF���ķ��ͳɹ���,���÷��ͱ�־ʧ��!",__FILE__,__LINE__);
                        sprintf(acErrMsg,"������ӡ������ʧ��!\n");
                        return(-1);
                }
        }else{
                iRc = iLvSetAddit(wp_payin,"QSX","0",1);
                vtcp_log("%s,%d,CIF���ķ���ʧ��,��Ҫ���·���!",__FILE__,__LINE__);
                return(-1);
        } 
        vtcp_log("[%s][%d] ������ӡӦ����Ϣ,���¼�¼********************�ɹ�[%s]\n",__FILE__,__LINE__,status); 
        /****�ɹ�����cif������ȷ������Ӧ*****/
        return(0);
}
/****NEWTCTD:���Ӽӽ��ܵĺ���****/
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
		/****�����****/
		char pcKey[33];
		char pcBinKey[17];
		char pcPinBlock[8];
		char sTmp[33];
		vtcp_log("%s,%d,Gethere,��������:[%.6s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		memset(pcKey,'\0',sizeof(pcKey));
		memset(pcBinKey,'\0',sizeof(pcBinKey));
		memset(pcPinBlock,'\0',sizeof(pcPinBlock));
		memset(sTmp,'\0',sizeof(sTmp));
		/****ȡ��KEY****/
		if(getenv("TCTDKEY")!=NULL){
			strncpy(pcKey,getenv("TCTDKEY"),32);
		}else{
			memset(pcKey,'2',32);	
		}
		iAscToBcd(pcKey,pcBinKey,32);
		/****����PINBLOCK****/
		pcPinBlock[0]=0x06;
		iAscToBcd(wp_td->pswdcode,pcPinBlock+1,6);
		memset(pcPinBlock+4,0xff,4);
		/****����****/
		des2key(pcBinKey,0);
		Ddes(pcPinBlock,sTmp);
		/****��base64����,����bcd - asc****/
		memset(wp_td->pswdcode,'\0',sizeof(wp_td->pswdcode));
		iBcdToAsc(sTmp,wp_td->pswdcode,8);	
		sprintf(sTmp,"%08d",strlen(wp_td->pswdcode));
		memcpy(wp_td->pswdlength,sTmp,8);
		vtcp_log("%s,%d,Gethere,���Լ�������:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		{
			/****���ܲ���****/
			/****���ܲ���****/
			TD_ADD_AREA wd_tdtest;
			memcpy(&wd_tdtest,wp_td,sizeof(wd_tdtest));
			nBepsDescrypt(wp_payin,&wd_tdtest);
			vtcp_log("%s,%d,Gethere,���Խ���:[%s][%s]\n",__FILE__,__LINE__,wd_tdtest.pswdlength,wd_tdtest.pswdcode);
		}
	}else{
		/****TODO:�˴�Ӧ���ü��ܻ�����****/
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
		vtcp_log("%s,%d,Gethere,���Լ�������ǰ:[%s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		vtcp_log("%s,%d,ip=:[%s]\n",__FILE__,__LINE__,cHostIp);
		vtcp_log("%s,%d,port=[%d]\n",__FILE__,__LINE__,atoi(cHostPort));
		/*vtcp_log("%s,%d,inputpwd=[%s]\n",__FILE__,__LINE__,cInputPwd);*/
		vtcp_log("%s,%d,pinactno=[%s]\n",__FILE__,__LINE__,cPinActno);
		vtcp_log("%s,%d,pcKey=[%s]\n",__FILE__,__LINE__,pcKey);
		vtcp_log("%s,%d,pcBinkey[%s]\n",__FILE__,__LINE__,pcBinKey);		
		iRet=UnionEncryptPin(cHostIp,atoi(cHostPort),cInputPwd,cPinActno,pcKey,pcBinKey);
		/**add by wangyongwei 20070905**/
		if(iRet){
			sprintf(acErrMsg,"���ܴ�����鿴���ܻ����¼���");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			return(-1);
		}
		/**add by wangyongwei 20070905**/
		memcpy(wp_td->pswdcode,pcBinKey,sizeof(wp_td->pswdcode));
		vtcp_log("%s,%d,Gethere,���Լ�������:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		/* test ֱ����֤����
		{
		char cUserKey[7];
		memset(cUserKey, 0 , sizeof(cUserKey));
		vtcp_log("%s,%d,ip=:[%s]\n",__FILE__,__LINE__,cHostIp);
		vtcp_log("%s,%d,port=[%d]\n",__FILE__,__LINE__,atoi(cHostPort));
		vtcp_log("%s,%d,pcKey=[%s]\n",__FILE__,__LINE__,pcKey);
		vtcp_log("%s,%d,pinactno=[%s]\n",__FILE__,__LINE__,cPinActno);
		vtcp_log("%s,%d,����=[%s]\n",__FILE__,__LINE__,pcBinKey);
		memset(wp_td->pswdcode, 0 , sizeof(wp_td->pswdcode));

		vtcp_log("%s,%d,Gethere,���Խ�������ǰ:[%s]\n",__FILE__,__LINE__,cUserKey);
		iRet=UnionDecryptPin(cHostIp,atoi(cHostPort),pcKey,pcBinKey,cPinActno,cUserKey);
		vtcp_log("%s,%d,Gethere,���Խ�������:[%.6s] ret=%d\n",__FILE__,__LINE__,cUserKey,iRet);
		}
		******************************/
		return(0);
	}
}
int nBepsDescrypt(PAY_IN_AREA *wp_payin,TD_ADD_AREA *wp_td){
	if(getenv("FHTEST")!=NULL && strcmp(getenv("FHTEST"),"LVTEST")==0){
		/****�����****/
		char pcKey[33];
		char pcBinKey[17];
		char pcPinBlock[8];
		char sTmp[33];
		int nPswdLen=0;
		vtcp_log("%s,%d,Gethere,��������:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		memset(pcKey,'\0',sizeof(pcKey));
		memset(pcBinKey,'\0',sizeof(pcBinKey));
		memset(pcPinBlock,'\0',sizeof(pcPinBlock));
		memset(sTmp,'\0',sizeof(sTmp));
		/****ȡ��KEY****/
		if(getenv("TCTDKEY")!=NULL){
			strncpy(pcKey,getenv("TCTDKEY"),32);
		}else{
			memset(pcKey,'2',32);	
		}
		iAscToBcd(pcKey,pcBinKey,32);
		/****���ܵõ�PINBLOCK****/
		nPswdLen=apatoi(wp_td->pswdlength,8);
		wp_td->pswdcode[nPswdLen]='\0';
		iAscToBcd(wp_td->pswdcode,sTmp,nPswdLen);
		vtcp_log("%s,%d,Gethere,���Խ��볤��:[%d]\n",__FILE__,__LINE__,nPswdLen);
		des2key(pcBinKey,1);
		Ddes(sTmp,pcPinBlock);
		/****����PINBLOCK�������****/
		iBcdToAsc(pcPinBlock+1,wp_td->pswdcode,3);
		wp_td->pswdcode[6]='\0';
		memcpy(wp_td->pswdlength,"00000006",8);

		vtcp_log("%s,%d,Gethere,���Խ�������:[%.6s]\n",__FILE__,__LINE__,wp_td->pswdcode);
	}else{
		/****TODO:�˴�Ӧ���ü��ܻ�����****/
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
		/****TODO:�˴�Ӧ���ü��ܻ�����****/
		vtcp_log("%s,%d,Gethere,���Խ�������ǰ:[%.16s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		iRet=UnionDecryptPin(cHostIp,atoi(cHostPort),pcKey,wp_td->pswdcode,cPinActno,wp_td->pswdcode);
		/**add by wangyongwei 20070905**/
		if(iRet){
			vtcp_log("%s,%d,Gethere,���Խ������Ľ��ܴ���:[%d]\n",__FILE__,__LINE__,iRet);
		}
		/**add by wangyongwei 20070905**/
		vtcp_log("%s,%d,Gethere,���Խ�������:[%.6s]\n",__FILE__,__LINE__,wp_td->pswdcode);
		return(0);
	}
}
