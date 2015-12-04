/*************************************************
* �� �� ��:  spJ087.c
* ���������� С�����ʽ��ײ�ѯ
* ��    ��:  ligl
* ������ڣ� 2006��8��29��
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
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
	char cBr_no[6];/*������*/
	char cTx_date[9];/*��������*/
	char cPrtflag[2];/*��ӡ��־*/
	char cTxstat[2];/*����״̬*/
	char cPkgno[4];/*��Ʊ����*/
	char cFilename[100];/*�ļ���*/
	int ttlnum=0;	    /**��ȡ��������**/
	FILE *fp;
	char wherelist[1024];  /**��ѯ����**/
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
	vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0720",cZf_type);
	get_zd_data("0440",cTx_date);
	get_zd_data("0670",cPrtflag);
	get_zd_data("0690",cTxstat);
	get_zd_data("0230",cPkgno);
	/*���ݲ������кŵõ����������к�*/
	memcpy(Cashqsactno,LV_QSBRNO,strlen(LV_QSBRNO));	
	vtcp_log("[%s][%d][%s]\n",__FILE__,__LINE__,Cashqsactno);
	/**��ɲ�ѯ����**/
	vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
	/***�������Ƿ�Ϊ��������****/
	ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",cBr_no);
		if (cZf_type[0]=='0')
		{
			if (memcmp(cPkgno,"000",3)!=0)
			{
				vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);

				if (cTxstat[0]=='a')/*ȫ������״̬*/
				{
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date,cPkgno);

					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date,cPkgno);

					}	
					else
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s'",cBr_no,cTx_date,cPkgno);

					}		
				}
				else
				{
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and lv_sts='%s'and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date,cPkgno,cTxstat);

					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and lv_sts='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date,cPkgno,cTxstat);

					}
					else
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and pkgno ='%s' and lv_sts='%s'",cBr_no,cTx_date,cPkgno,cTxstat);

					}
				}
			}
			else
			{
				vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
				if (cTxstat[0]=='a')/*ȫ������״̬*/
				{	
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date);

					}	
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date);
					} 
					else
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s'",cBr_no,cTx_date);

					}		
				}
				else
				{
					if (cPrtflag[0]=='0')
					{   
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and lv_sts='%s' and (brprtflag='0' or brprtflag is null) and (qsprtflag='0' or qsprtflag is null)",cBr_no,cTx_date,cTxstat);

					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and lv_sts='%s' and (brprtflag='1' or qsprtflag='1')",cBr_no,cTx_date,cTxstat);

					}
					else
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, "lw_ind='2' and br_no='%s' and in_date='%s' and and lv_sts='%s'",cBr_no,cTx_date,cTxstat);

					}
				}
			}
			/*��ʼ��ѯ*/
			/* ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, wherelist); */
			vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
				fprintf( fp,"~������|��������|˳���|���׽��|������(��)|������(��)|״   ̬|���ı��|��ӡ��־|�����|����������|��ִ״̬|�������к�|\n" );
				}
				if (lv_pkgreg.brprtflag[0]=='1' ||lv_pkgreg.qsprtflag[0]=='1')
				{
					memcpy(cPrtname,"�Ѵ�ӡ", sizeof(cPrtname)-1);
				}
				else
				{
					memcpy(cPrtname,"δ��ӡ", sizeof(cPrtname)-1);
				}
				memset(RcpStatname,0x0,sizeof(RcpStatname));
				if((lv_pkgreg.lv_sts[0]=='I') && (memcmp(lv_pkgreg.rcpstat,"00",2)!=0) && (memcmp(lv_pkgreg.pkgno,PKGNO_PTJJHZ,sizeof(lv_pkgreg.pkgno)-1)==0) && (memcmp(lv_pkgreg.txnum,"30104",5)==0 || !memcmp(lv_pkgreg.txnum,"30105",5)==0)){
					strcpy(RcpStatname,"�������ܣ���ִδ����");
				}else {
					iNum2Stat(lv_pkgreg.lv_sts[0],RcpStatname);
				}
				iNum2RcpStat(lv_pkgreg.rcpstat,rcp_content,lv_pkgreg.pkgno);
				/*fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,lv_pkgreg.in_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,lv_pkgreg.o_or_br_no,lv_pkgreg.ac_br_no,RcpStatname,iNum2TxType(lv_pkgreg.pkgno,Pkgnoname),cPrtname,lv_pkgreg.packid,lv_pkgreg.pay_qs_no,rcp_content);
        in_date  �޸ĳ�wt_date*/
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
				vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
				if (cTxstat[0]=='a')/*ȫ������״̬*/
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
							vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
							vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
							vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
							vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
							vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
							vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
							ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and lw_ind='2' and wt_date='%s' and cmtno ='%s' and lv_fzf_sts='%s'",cBr_no,cTx_date,cPkgno);
						}
					}
				}
			}
			else
			{
				vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
				if (cTxstat[0]=='a')/*ȫ������״̬*/
				{	
					if (cPrtflag[0]=='0')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and hv_prt_ind='1' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
					}
					else
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s'  and wt_date='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date);
					}	
				} 		
				else
				{
					if (cPrtflag[0]=='0')
					{	
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and and lv_fzf_sts='%s' and (hv_prt_ind='0' or hv_prt_ind is null) and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date,cTxstat);
					}
					else if (cPrtflag[0]=='1')
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and and lv_fzf_sts='%s' and hv_prt_ind='1' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date,cTxstat);
					}
					else 
					{
						vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
						ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, "otype='2' and br_no='%s' and wt_date='%s' and and lv_fzf_sts='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') ",cBr_no,cTx_date,cTxstat);
					}
				}
			}
			/*��ʼ��ѯ*/
			/* ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, wherelist); */
			vtcp_log("spJ087��ʼ��[%s][%d]",__FILE__,__LINE__);
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
					fprintf( fp,"~������|��������|˳���|���׽��|������(��)|������(��)|״   ̬|���ı��|��ӡ��־|�����|����������|\n" );
				}
				if (hv_fzf.hv_prt_ind[0]=='1')
				{
					memcpy(cPrtname,"�Ѵ�ӡ", sizeof(cPrtname)-1);
				}
				else
				{
					memcpy(cPrtname,"δ��ӡ", sizeof(cPrtname)-1);
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

/**��������**/
char * iNum2TxType(char *ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);

	char cName[20];
	memset(cName,0x00,sizeof(cName));

	if (!memcmp(ch,"001",3))
	{           
		strcpy(cName,"��ͨ����");
	} 
	else if (!memcmp(ch,"002",3))
	{
		strcpy(cName,"��ͨ���");
	}
	else if (!memcmp(ch,"003",3))
	{
		strcpy(cName,"ʵʱ����");
	}
	else if (!memcmp(ch,"004",3))
	{
		strcpy(cName,"ʵʱ���");
	}
	else if (!memcmp(ch,"005",3))
	{
		strcpy(cName,"���ڴ���");
	}
	else if (!memcmp(ch,"006",3))
	{
		strcpy(cName,"���ڽ��");
	}
	else if (!memcmp(ch,"007",3))
	{
		strcpy(cName,"�����˻�");
	}
	else if (!memcmp(ch,"008",3))
	{
		strcpy(cName,"�ս��ִ");
	}
	else if (!memcmp(ch,"009",3))
	{
		strcpy(cName,"ʵʱ���ǻ�ִ");
	}
	else if (!memcmp(ch,"010",3))
	{
		strcpy(cName,"ʵʱ��ǻ�ִ");
	}
	else if (!memcmp(ch,"011",3))
	{
		strcpy(cName,"���ڽ�ǻ�ִ");
	}
	else if (!memcmp(ch,"012",3))
	{
		strcpy(cName,"ͨ����Ϣ");
	}
	else if (!memcmp(ch,"013",3))
	{
		strcpy(cName,"ʵʱ��Ϣ");
	}
	else if (!memcmp(ch,"301",3))
	{
		strcpy(cName,"С���ѯ");
	}
	else if (!memcmp(ch,"302",3))
	{
		strcpy(cName,"С��鸴");
	}
	else if (!memcmp(ch,"303",3))
	{
		strcpy(cName,"���ɸ�ʽ");
	}
	else if (!memcmp(ch,"319",3))
	{
		strcpy(cName,"�����˻�");
	}
	else if (!memcmp(ch,"320",3))
	{
		strcpy(cName,"�˻�Ӧ��");
	}
	else if (!memcmp(ch,"321",3))
	{
		strcpy(cName,"���볷��");
	}
	else if (!memcmp(ch,"322",3))
	{
		strcpy(cName,"����Ӧ��");
	}
	else if (!memcmp(ch,"323",3))
	{
		strcpy(cName,"��������");
	}
	else if (!memcmp(ch,"324",3))
	{
		strcpy(cName,"����Ӧ��");
	}
	else if (!memcmp(ch,"325",3))
	{
		strcpy(cName,"����֪ͨ");
	}
	else if (!memcmp(ch,"327",3))
	{
		strcpy(cName,"ֹ������");
	}
	else if (!memcmp(ch,"328",3))
	{
		strcpy(cName,"ֹ��Ӧ��");
	}
	else
		strcpy(cName,"δ֪��������");
	memcpy(cTmpList,cName,strlen(cName));
	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
	return  cName;
}

/**����״̬**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if (ch=='0')
	{
		strcpy(cTmpList,"��Ч״̬");
	}
	else if (ch=='1')
	{
		strcpy(cTmpList,"����¼��");
	}
	else if (ch=='3')
	{
		strcpy(cTmpList,"���˸���");
	}
	else if (ch=='4')
	{
		strcpy(cTmpList,"���˷���");
	}
	else if (ch=='5')
	{
		strcpy(cTmpList,"���˾ܾ�");
	}
	else if (ch=='6')
	{
		strcpy(cTmpList,"��������");
	}
	else if (ch=='7')
	{
		strcpy(cTmpList,"�����Ŷ�");
	}
	else if (ch=='8')
	{
		strcpy(cTmpList,"���˳����ȴ�");
	}
	else if (ch=='9')
	{
		strcpy(cTmpList,"���˳���");
	}
	else if (ch=='A')
	{
		strcpy(cTmpList,"�˻س���ֹ���ȴ�");
	}
	else if (ch=='B')
	{
		strcpy(cTmpList,"�����˻�");
	}
	else if (ch=='C')
	{
		strcpy(cTmpList,"�����ѳ���");
	}
	else if (ch=='D')
	{
		strcpy(cTmpList,"������ֹ��");
	}
	else if (ch=='G')
	{
		strcpy(cTmpList,"�����쳣����");
	}
	else if (ch=='H')
	{
		strcpy(cTmpList,"��ʱ����,������");
	}
	else if (ch=='I')
	{
		strcpy(cTmpList,"��ʽ����");
	}
	else if (ch=='J')
	{
		strcpy(cTmpList,"ȫ��Ѻ��");
	}
	else if (ch=='K')
	{
		strcpy(cTmpList,"�ط�Ѻ��");
	}
	else if (ch=='L')
	{
		strcpy(cTmpList,"ȫ��Ѻ�͵ط�Ѻ����");
	}
	else if (ch=='M')
	{
		strcpy(cTmpList,"�������Ĺ���");
	}
	else if (ch=='N')
	{
		strcpy(cTmpList,"֧�й���");
	}
	else if (ch=='O')
	{
		strcpy(cTmpList,"�����˻صȴ�");
	}
	else if (ch=='P')
	{
		strcpy(cTmpList,"�����˻�");
	}
	else if (ch=='Q')
	{
		strcpy(cTmpList,"ͬ��δ����");
	}
	else if (ch=='R')
	{
		strcpy(cTmpList,"ͬ���ѷ���");
	}
	else if (ch=='S')
	{
		strcpy(cTmpList,"��ͬ��δ����");
	}
	else if (ch=='T')
	{
		strcpy(cTmpList,"��ͬ���ѷ���");
	}
	else if (ch=='U')
	{
		strcpy(cTmpList,"�����ѳ���");
	}
	else if (ch=='V')
	{
		strcpy(cTmpList,"������ֹ��");
	}
	else if (ch=='W')
	{
		strcpy(cTmpList,"�����Ѹ���");
	}
	else if (ch=='Z')
	{
		strcpy(cTmpList,"�����쳣����");
	}
	else
		strcpy(cTmpList,"δ֪״̬");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}

/***��ִ״̬*****/
char * iNum2RcpStat(char * rcpstat , char *cTmpList,char *pkgno)
{
	vtcp_log("liuyue[%s][%d]rcpstat==[%s]\n",__FILE__,__LINE__,rcpstat);

	if (!memcmp(pkgno,PKGNO_PTJJ,3) ||!memcmp(pkgno,PKGNO_PTJJHZ,3) ||!memcmp(pkgno,PKGNO_DQJJ,3)||!memcmp(pkgno,PKGNO_DQJJHZ,3)||!memcmp(pkgno,PKGNO_SSJJ,3) ||!memcmp(pkgno,PKGNO_SSJJHZ,3))
	{
		if (!memcmp(rcpstat,"00",2))
		{
			strcpy(cTmpList,"�ɹ�");
		}	
		else if (!memcmp(rcpstat,"01",2))
		{
			strcpy(cTmpList,"�ʺŲ���");
		}	
		else if (!memcmp(rcpstat,"02",2))
		{
			strcpy(cTmpList,"�ʺŻ�������");
		}	
		else if (!memcmp(rcpstat,"03",2))
		{
			strcpy(cTmpList,"�˻�����֧��");
		}	
		else if (!memcmp(rcpstat,"10",2))
		{
			strcpy(cTmpList,"�˻��������");
		}	
		else if (!memcmp(rcpstat,"11",2))
		{
			strcpy(cTmpList,"�ʺ�״̬����");
		}	
		else if (!memcmp(rcpstat,"20",2))
		{
			strcpy(cTmpList,"ҵ���ѳ���");
		}	
		else if (!memcmp(rcpstat,"90",2))
		{
			strcpy(cTmpList,"��������");
		}
		else if (!memcmp(rcpstat,"21",2)){
			strcpy(cTmpList,"��Сд����");
		}else if (!memcmp(rcpstat,"22",2)){
			strcpy(cTmpList,"֧Ʊ�������ȫ");
		}else if (!memcmp(rcpstat,"23",2)){
			strcpy(cTmpList,"��Ʊ��ǩ�²���");
		}else if (!memcmp(rcpstat,"24",2)){
			strcpy(cTmpList,"֧���������");
		}else if (!memcmp(rcpstat,"25",2)){
			strcpy(cTmpList,"��Ʊ��δ������");
		}else if (!memcmp(rcpstat,"26",2)){
			strcpy(cTmpList,"֧ƱӰ�����");
		}else if (!memcmp(rcpstat,"27",2)){
			strcpy(cTmpList,"��Ʊ���ʺŻ�������");
		}else if (!memcmp(rcpstat,"28",2)){
			strcpy(cTmpList,"��Ʊ������");
		}else if (!memcmp(rcpstat,"29",2)){
			strcpy(cTmpList,"�ظ���ʾ����");
		}else if (!memcmp(rcpstat,"30",2)){
			strcpy(cTmpList,"�Ǳ���Ʊ��");
		}else if (!memcmp(rcpstat,"31",2)){
			strcpy(cTmpList,"��Ʊ��������");
		}else if (!memcmp(rcpstat,"32",2)){
			strcpy(cTmpList,"��Ʊ���ʻ��Ѷ���");
		}else if (!memcmp(rcpstat,"33",2)){
			strcpy(cTmpList,"��Ʊ���ʻ���ֹ��");
		}else if (!memcmp(rcpstat,"34",2)){
			strcpy(cTmpList,"ҵ���Ѿ�����ֹ��");
		}else if (!memcmp(rcpstat,"35",2)){
			strcpy(cTmpList,"����ǩ����֤���");
		}else if (!memcmp(rcpstat,"09",2)){
			strcpy(cTmpList,"�Է���Ʊ");
		}else if (rcpstat[0]=='\0'){
			strcpy(cTmpList,"δ����ִ");	
		}else{
			strcpy(cTmpList,"����Ļ�ִ״̬");
		}			 
	}
	else
		strcpy(cTmpList,"       ");

	vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;
}

