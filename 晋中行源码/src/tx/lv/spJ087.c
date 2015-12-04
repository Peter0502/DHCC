/*************************************************
* 文 件 名:  spJ087.c
* 功能描述： 小额来帐交易查询
* 作    者:  ligl
* 完成日期： 2006年8月29日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ){\
	sprintf( acErrMsg, "sql error" ); \
	WRITEMSG \
	goto ErrExit; \
}
#define EXTERN
#include <string.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"

char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);
char * iNum2RcpStat(char *rcpstat,char *cTmpList ,char *pkgno);

spJ087()
{
	int ret=0,i;
	struct lv_pkgreg_c lv_pkgreg;
	struct hv_fzf_c hv_fzf;
	struct com_branch_c com_branch;
	/*HV_FD123_AREA fd123;*/
	char cBr_no[6];/*机构号*/
	char cTx_date[9];/*交易日期*/
	char cPrtflag[2];/*打印标志*/
	char cTxstat[2];/*交易状态*/
	char cPkgno[4];/*汇票类型*/
	char cFilename[100];/*文件名*/
	int ttlnum=0;	    /**读取的总条数**/
	FILE *fp;
	char wherelist[1024];  /**查询条件**/
	char cPrtname[7];
	char Cashqsactno[13];
	char RcpStat[3];
	char RcpStatname[21];
	char rcp_content[41];
	char Pkgnoname[11];
	char cZf_type[2];
	memset(RcpStatname ,0, sizeof(RcpStatname ));
	memset(Pkgnoname   ,0, sizeof(Pkgnoname ));
	memset(RcpStat     ,0, sizeof(RcpStat ));
	memset(cBr_no     ,0, sizeof(cBr_no ));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cPrtflag   ,0, sizeof(cPrtflag));
	memset(cTxstat    ,0, sizeof(cTxstat ));
	memset(cPkgno     ,0, sizeof(cPkgno  ));
	memset(cFilename  ,0, sizeof(cFilename));
	memset(cPrtname   ,0, sizeof(cPrtname));
	/*memset(&fd123, ' ',sizeof(fd123));*/
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(&com_branch,0x00,sizeof(struct com_branch_c));
	memset(Cashqsactno,0,sizeof(Cashqsactno));
	memset(&hv_fzf,0,sizeof(struct hv_fzf_c));
	memset(cZf_type,0,sizeof(cZf_type));
	vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0720",cZf_type);
	get_zd_data("0440",cTx_date);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0690",cTxstat);
	get_zd_data("0230",cPkgno);
	/*根据参与者行号得到清算中心行号*/
	memcpy(Cashqsactno,LV_QSBRNO,strlen(LV_QSBRNO));	
	vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,Cashqsactno);
	/**组成查询条件**/
	vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
	/***检查机构是否为清算中心****/
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",cBr_no);
		if (cZf_type[0]=='0')
		{
			if (memcmp(cPkgno,"000",3)!=0)
			{
				vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);

				if (cTxstat[0]=='a')/*全部交易状态*/
				{
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date,cPkgno);

					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date,cPkgno);

					}	
					else
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s'",cBr_no,cTx_date,cPkgno);

					}		
				}
				else
				{
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and lv_sts='%s'and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date,cPkgno,cTxstat);

					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and lv_sts='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date,cPkgno,cTxstat);

					}
					else
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and lv_sts='%s'",cBr_no,cTx_date,cPkgno,cTxstat);

					}
				}
			}
			else
			{
				vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
				if (cTxstat[0]=='a')/*全部交易状态*/
				{	
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date);

					}	
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date);
					} 
					else
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s'",cBr_no,cTx_date);

					}		
				}
				else
				{
					if (cPrtflag[0]=='0')
					{   
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and lv_sts='%s' and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date,cTxstat);

					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and lv_sts='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date,cTxstat);

					}
					else
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and and lv_sts='%s'",cBr_no,cTx_date,cTxstat);

					}
				}
			}
			/*开始查询*/
			/* ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, wherelist); */
			vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
			ERR_DEAL
				ttlnum=0;
			while (1)
			{
				memset(cPrtname   ,0, sizeof(cPrtname));
				memset(RcpStat    ,0, sizeof(RcpStat));
				ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
				if ( ret==100 ) 
				{
					break;
				}
				ERR_DEAL
				if ( !ttlnum )
				{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if ( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
							strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}        
				fprintf( fp,"~交易行|交易日期|顺序号|交易金额|开户行(付)|开户行(收)|状   态|报文编号|打印标志|包序号|发起清算行|回执状态|发起行行号|\n" );
				}
				if (lv_pkgreg.brprtflag[0]=='1' ||lv_pkgreg.qsprtflag[0]=='1')
				{
					memcpy(cPrtname,"已打印", sizeof(cPrtname)-1);
				}
				else
				{
					memcpy(cPrtname,"未打印", sizeof(cPrtname)-1);
				}
				memset(RcpStatname,0x0,sizeof(RcpStatname));
				if((lv_pkgreg.lv_sts[0]=='I') && (memcmp(lv_pkgreg.rcpstat,"00",2)!=0) && (memcmp(lv_pkgreg.pkgno,PKGNO_PTJJHZ,sizeof(lv_pkgreg.pkgno)-1)==0) && (memcmp(lv_pkgreg.txnum,"30104",5)==0 || !memcmp(lv_pkgreg.txnum,"30105",5)==0)){
					strcpy(RcpStatname,"正常接受，回执未付款");
				}else {
					iNum2Stat(lv_pkgreg.lv_sts[0],RcpStatname);
				}
				iNum2RcpStat(lv_pkgreg.rcpstat,rcp_content,lv_pkgreg.pkgno);
				/*fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,lv_pkgreg.in_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,lv_pkgreg.o_or_br_no,lv_pkgreg.ac_br_no,RcpStatname,iNum2TxType(lv_pkgreg.pkgno,Pkgnoname),cPrtname,lv_pkgreg.packid,lv_pkgreg.pay_qs_no,rcp_content);
        in_date  修改成wt_date*/
        fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,lv_pkgreg.wt_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,lv_pkgreg.o_or_br_no,lv_pkgreg.ac_br_no,RcpStatname,iNum2TxType(lv_pkgreg.pkgno,Pkgnoname),cPrtname,lv_pkgreg.packid,lv_pkgreg.pay_qs_no,rcp_content,lv_pkgreg.or_br_no);

				ttlnum++;
			}
			ret=Lv_pkgreg_Clo_Sel();
			ERR_DEAL
			if ( !ttlnum )
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data( DC_FILE_SND,"1");
			}
		}
		else
		{
			if (memcmp(cPkgno,"000",3)!=0)
			{
				vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
				if (cTxstat[0]=='a')/*全部交易状态*/
				{
					if (cPrtflag[0]=='0')
					{
						if (memcmp(cPkgno,"302",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and (hv_prt_ind='0' or hv_prt_ind is null)",cBr_no,cTx_date,LV_QSBRNO);
						}
						else if (memcmp(cPkgno,"301",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301') and (hv_prt_ind='0' or hv_prt_ind is null)",cBr_no,cTx_date,LV_QSBRNO);
						}
						else
						{   
							vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2'  and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s' and (hv_prt_ind='0' or hv_prt_ind is null)",cBr_no,cTx_date,cPkgno);				    
						}
					}
					else if (cPrtflag[0]=='1')
					{
						if (memcmp(cPkgno,"302",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and hv_prt_ind='1'",cBr_no,cTx_date,LV_QSBRNO);
						}
						else if (memcmp(cPkgno,"301",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2'  and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301') and hv_prt_ind='1'",cBr_no,cTx_date,LV_QSBRNO);
						}
						else
						{
							vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s' and hv_prt_ind='1'",cBr_no,cTx_date,cPkgno);
						}
					}
					else
					{
						if (memcmp(cPkgno,"302",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') ",cBr_no,cTx_date,LV_QSBRNO);
						}
						else if (memcmp(cPkgno,"301",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2'  and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301')",cBr_no,cTx_date,LV_QSBRNO);
						}
						else
						{
							vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s'",cBr_no,cTx_date,cPkgno);
						}
					}			
				}
				else
				{
					if (cPrtflag[0]=='0')
					{
						if (memcmp(cPkgno,"302",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and hv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (cmtno='302' or cmtno='301')",cBr_no,cTx_date,LV_QSBRNO,cTxstat);
						}
						else if (memcmp(cPkgno,"301",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and hv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (cmtno='302' or cmtno='301')",cBr_no,cTx_date,LV_QSBRNO,cTxstat);
						}
						else
						{  
							vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s' and lv_fzf_sts='%s'and (hv_prt_ind='0' or hv_prt_ind is null)",cBr_no,cTx_date,cPkgno);
						}
					}
					else if (cPrtflag[0]=='1')
					{
						if (memcmp(cPkgno,"302",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and hv_fzf_sts='%s' and hv_prt_ind='1'and (cmtno='302' or cmtno='301')",cBr_no,cTx_date,LV_QSBRNO,cTxstat);
						}
						else if (memcmp(cPkgno,"301",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and hv_fzf_sts='%s' and hv_prt_ind='1'and (cmtno='302' or cmtno='301')",cBr_no,cTx_date,LV_QSBRNO,cTxstat);
						}
						else
						{
							vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s' and lv_fzf_sts='%s'and hv_prt_ind='1'",cBr_no,cTx_date,cPkgno);
						}
					}
					else
					{
						if (memcmp(cPkgno,"302",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no='%s' and (cmtno='302' or cmtno='301') and hv_fzf_sts='%s'",cBr_no,cTx_date,LV_QSBRNO,cTxstat);
						}
						else if (memcmp(cPkgno,"301",3)==0)
						{
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and tx_date='%s' and pay_qs_no!='%s' and (cmtno='302' or cmtno='301') and hv_fzf_sts='%s'",cBr_no,cTx_date,LV_QSBRNO,cTxstat);
						}
						else
						{
							vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s' and lv_fzf_sts='%s'",cBr_no,cTx_date,cPkgno);
						}
					}
				}
			}
			else
			{
				vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
				if (cTxstat[0]=='a')/*全部交易状态*/
				{	
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and hv_prt_ind='1' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
					}
					else
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s'  and wt_date='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
					}	
				} 		
				else
				{
					if (cPrtflag[0]=='0')
					{	
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and and lv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date,cTxstat);
					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and and lv_fzf_sts='%s' and hv_prt_ind='1' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date,cTxstat);
					}
					else 
					{
						vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and and lv_fzf_sts='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date,cTxstat);
					}
				}
			}
			/*开始查询*/
			/* ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, wherelist); */
			vtcp_log("spJ087开始！[%s][%d]",__FILE__,__LINE__);
			ERR_DEAL
				ttlnum=0;
			while (1)
			{
				memset(cPrtname   ,0, sizeof(cPrtname));
				memset(RcpStat    ,0, sizeof(RcpStat));
				ret=Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
				if ( ret==100 ) 
				{
					break;
				}
				ERR_DEAL
				if ( !ttlnum )
				{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if ( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
							strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}        
					fprintf( fp,"~交易行|交易日期|顺序号|交易金额|开户行(付)|开户行(收)|状   态|报文编号|打印标志|包序号|发起清算行|\n" );
				}
				if (hv_fzf.hv_prt_ind[0]=='1')
				{
					memcpy(cPrtname,"已打印", sizeof(cPrtname)-1);
				}
				else
				{
					memcpy(cPrtname,"未打印", sizeof(cPrtname)-1);
				}
				memset(RcpStatname,0x0,sizeof(RcpStatname));
				iNum2Stat(hv_fzf.hv_fzf_sts[0],RcpStatname);
				fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|%s|%s|\n",hv_fzf.br_no,hv_fzf.wt_date,hv_fzf.orderno,hv_fzf.o_tx_amt,hv_fzf.o_or_br_no,hv_fzf.ac_br_no,RcpStatname,iNum2TxType(hv_fzf.cmtno,Pkgnoname),cPrtname,lv_pkgreg.packid,hv_fzf.pay_qs_no);
				ttlnum++;
			}
			ret=Hv_fzf_Clo_Sel();
			ERR_DEAL
			if ( !ttlnum )
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data( DC_FILE_SND,"1");
			}
		}
	GoodExit:
		strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
	ErrExit:
		sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}

/**交易类型**/
char * iNum2TxType(char *ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);

	char cName[20];
	memset(cName,0x00,sizeof(cName));

	if (!memcmp(ch,"001",3))
	{           
		strcpy(cName,"普通贷记");
	} 
	else if (!memcmp(ch,"002",3))
	{
		strcpy(cName,"普通借记");
	}
	else if (!memcmp(ch,"003",3))
	{
		strcpy(cName,"实时贷记");
	}
	else if (!memcmp(ch,"004",3))
	{
		strcpy(cName,"实时借记");
	}
	else if (!memcmp(ch,"005",3))
	{
		strcpy(cName,"定期贷记");
	}
	else if (!memcmp(ch,"006",3))
	{
		strcpy(cName,"定期借记");
	}
	else if (!memcmp(ch,"007",3))
	{
		strcpy(cName,"贷记退汇");
	}
	else if (!memcmp(ch,"008",3))
	{
		strcpy(cName,"普借回执");
	}
	else if (!memcmp(ch,"009",3))
	{
		strcpy(cName,"实时贷记回执");
	}
	else if (!memcmp(ch,"010",3))
	{
		strcpy(cName,"实时借记回执");
	}
	else if (!memcmp(ch,"011",3))
	{
		strcpy(cName,"定期借记回执");
	}
	else if (!memcmp(ch,"012",3))
	{
		strcpy(cName,"通用信息");
	}
	else if (!memcmp(ch,"013",3))
	{
		strcpy(cName,"实时信息");
	}
	else if (!memcmp(ch,"301",3))
	{
		strcpy(cName,"小额查询");
	}
	else if (!memcmp(ch,"302",3))
	{
		strcpy(cName,"小额查复");
	}
	else if (!memcmp(ch,"303",3))
	{
		strcpy(cName,"自由格式");
	}
	else if (!memcmp(ch,"319",3))
	{
		strcpy(cName,"申请退回");
	}
	else if (!memcmp(ch,"320",3))
	{
		strcpy(cName,"退回应答");
	}
	else if (!memcmp(ch,"321",3))
	{
		strcpy(cName,"申请撤销");
	}
	else if (!memcmp(ch,"322",3))
	{
		strcpy(cName,"撤销应答");
	}
	else if (!memcmp(ch,"323",3))
	{
		strcpy(cName,"冲正申请");
	}
	else if (!memcmp(ch,"324",3))
	{
		strcpy(cName,"冲正应答");
	}
	else if (!memcmp(ch,"325",3))
	{
		strcpy(cName,"冲正通知");
	}
	else if (!memcmp(ch,"327",3))
	{
		strcpy(cName,"止付申请");
	}
	else if (!memcmp(ch,"328",3))
	{
		strcpy(cName,"止付应答");
	}
	else
		strcpy(cName,"未知交易类型");
	memcpy(cTmpList,cName,strlen(cName));
	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
	return  cName;
}

/**交易状态**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if (ch=='0')
	{
		strcpy(cTmpList,"无效状态");
	}
	else if (ch=='1')
	{
		strcpy(cTmpList,"往账录入");
	}
	else if (ch=='3')
	{
		strcpy(cTmpList,"往账复核");
	}
	else if (ch=='4')
	{
		strcpy(cTmpList,"往账发送");
	}
	else if (ch=='5')
	{
		strcpy(cTmpList,"往账拒绝");
	}
	else if (ch=='6')
	{
		strcpy(cTmpList,"往账扎差");
	}
	else if (ch=='7')
	{
		strcpy(cTmpList,"往账排队");
	}
	else if (ch=='8')
	{
		strcpy(cTmpList,"往账撤销等待");
	}
	else if (ch=='9')
	{
		strcpy(cTmpList,"往账撤销");
	}
	else if (ch=='A')
	{
		strcpy(cTmpList,"退回冲正止付等待");
	}
	else if (ch=='B')
	{
		strcpy(cTmpList,"往账退回");
	}
	else if (ch=='C')
	{
		strcpy(cTmpList,"往账已冲正");
	}
	else if (ch=='D')
	{
		strcpy(cTmpList,"往账已止付");
	}
	else if (ch=='G')
	{
		strcpy(cTmpList,"往账异常挂帐");
	}
	else if (ch=='H')
	{
		strcpy(cTmpList,"临时接收,不记帐");
	}
	else if (ch=='I')
	{
		strcpy(cTmpList,"正式接收");
	}
	else if (ch=='J')
	{
		strcpy(cTmpList,"全国押错");
	}
	else if (ch=='K')
	{
		strcpy(cTmpList,"地方押错");
	}
	else if (ch=='L')
	{
		strcpy(cTmpList,"全国押和地方押都错");
	}
	else if (ch=='M')
	{
		strcpy(cTmpList,"清算中心挂帐");
	}
	else if (ch=='N')
	{
		strcpy(cTmpList,"支行挂帐");
	}
	else if (ch=='O')
	{
		strcpy(cTmpList,"来账退回等待");
	}
	else if (ch=='P')
	{
		strcpy(cTmpList,"来账退回");
	}
	else if (ch=='Q')
	{
		strcpy(cTmpList,"同意未发送");
	}
	else if (ch=='R')
	{
		strcpy(cTmpList,"同意已发送");
	}
	else if (ch=='S')
	{
		strcpy(cTmpList,"不同意未发送");
	}
	else if (ch=='T')
	{
		strcpy(cTmpList,"不同意已发送");
	}
	else if (ch=='U')
	{
		strcpy(cTmpList,"来账已冲正");
	}
	else if (ch=='V')
	{
		strcpy(cTmpList,"来账已止付");
	}
	else if (ch=='W')
	{
		strcpy(cTmpList,"来账已付款");
	}
	else if (ch=='Z')
	{
		strcpy(cTmpList,"来账异常挂帐");
	}
	else
		strcpy(cTmpList,"未知状态");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}

/***回执状态*****/
char * iNum2RcpStat(char * rcpstat , char *cTmpList,char *pkgno)
{
	vtcp_log("liuyue[%s][%d]rcpstat==[%s]\n",__FILE__,__LINE__,rcpstat);

	if (!memcmp(pkgno,PKGNO_PTJJ,3) ||!memcmp(pkgno,PKGNO_PTJJHZ,3) ||!memcmp(pkgno,PKGNO_DQJJ,3)||!memcmp(pkgno,PKGNO_DQJJHZ,3)||!memcmp(pkgno,PKGNO_SSJJ,3) ||!memcmp(pkgno,PKGNO_SSJJHZ,3))
	{
		if (!memcmp(rcpstat,"00",2))
		{
			strcpy(cTmpList,"成功");
		}	
		else if (!memcmp(rcpstat,"01",2))
		{
			strcpy(cTmpList,"帐号不符");
		}	
		else if (!memcmp(rcpstat,"02",2))
		{
			strcpy(cTmpList,"帐号户名不符");
		}	
		else if (!memcmp(rcpstat,"03",2))
		{
			strcpy(cTmpList,"账户余额不足支付");
		}	
		else if (!memcmp(rcpstat,"10",2))
		{
			strcpy(cTmpList,"账户密码错误");
		}	
		else if (!memcmp(rcpstat,"11",2))
		{
			strcpy(cTmpList,"帐号状态错误");
		}	
		else if (!memcmp(rcpstat,"20",2))
		{
			strcpy(cTmpList,"业务已撤销");
		}	
		else if (!memcmp(rcpstat,"90",2))
		{
			strcpy(cTmpList,"其他错误");
		}
		else if (!memcmp(rcpstat,"21",2)){
			strcpy(cTmpList,"大小写金额不符");
		}else if (!memcmp(rcpstat,"22",2)){
			strcpy(cTmpList,"支票记载事项不全");
		}else if (!memcmp(rcpstat,"23",2)){
			strcpy(cTmpList,"出票人签章不符");
		}else if (!memcmp(rcpstat,"24",2)){
			strcpy(cTmpList,"支付密码错误");
		}else if (!memcmp(rcpstat,"25",2)){
			strcpy(cTmpList,"持票人未做背书");
		}else if (!memcmp(rcpstat,"26",2)){
			strcpy(cTmpList,"支票影像不相符");
		}else if (!memcmp(rcpstat,"27",2)){
			strcpy(cTmpList,"出票人帐号户名不符");
		}else if (!memcmp(rcpstat,"28",2)){
			strcpy(cTmpList,"出票人余额不足");
		}else if (!memcmp(rcpstat,"29",2)){
			strcpy(cTmpList,"重复提示付款");
		}else if (!memcmp(rcpstat,"30",2)){
			strcpy(cTmpList,"非本行票据");
		}else if (!memcmp(rcpstat,"31",2)){
			strcpy(cTmpList,"出票人已销户");
		}else if (!memcmp(rcpstat,"32",2)){
			strcpy(cTmpList,"出票人帐户已冻结");
		}else if (!memcmp(rcpstat,"33",2)){
			strcpy(cTmpList,"出票人帐户已止付");
		}else if (!memcmp(rcpstat,"34",2)){
			strcpy(cTmpList,"业务已经申请止付");
		}else if (!memcmp(rcpstat,"35",2)){
			strcpy(cTmpList,"数字签名或证书错");
		}else if (!memcmp(rcpstat,"09",2)){
			strcpy(cTmpList,"对方退票");
		}else if (rcpstat[0]=='\0'){
			strcpy(cTmpList,"未被回执");	
		}else{
			strcpy(cTmpList,"错误的回执状态");
		}			 
	}
	else
		strcpy(cTmpList,"       ");

	vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;
}

