/*************************************************
* �� �� ��:  spJ096.c
* ���������� ��״̬��ѯ
*
* ��    ��: liqiang 
* ������ڣ� 2006-9-4
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "lv_define.h"
#include "public.h"
#include "lv_wbctl_c.h"                                         
#include "lv_lbctl_c.h" 
#include "hv_uniontab_c.h"
int spJ096()
{
	  	char  pcDtl[17];   
	  	char name[13];  
	  	char cBr_no[6];                          	
		char cFilename[100];
		char wherelist[1024];    /**��ѯ����**/
		char tmpstr[512];
		int  i=0;
		int  ret=0;
		char stat1[2];
		long pack_date=0;
		char cPack_id[9];
		FILE *fp;
		struct lv_wbctl_c q_lv_wbctl;
		struct lv_lbctl_c q_lv_lbctl;
		memset(cBr_no,0,sizeof(cBr_no));
		memset(pcDtl,0,sizeof(pcDtl));
		memset(name,0,sizeof(name));
		memset(wherelist,0,sizeof(wherelist));
		memset(tmpstr,0,sizeof(tmpstr));
		memset(&q_lv_wbctl,0,sizeof(struct lv_wbctl_c));
		memset(&q_lv_lbctl,0,sizeof(struct lv_lbctl_c));
		memset(stat1,0,sizeof(stat1));
		memset(cFilename,0,sizeof(cFilename));
		memset(cPack_id,0x00,sizeof(cPack_id));
		pub_base_sysinit();
		get_zd_long("0460",&pack_date);	    /*ί������*/
		get_zd_data("0670",stat1);          /* ��״̬ */
		get_zd_data("0910",cBr_no);
		get_zd_data("0440",cPack_id);

		

		if(stat1[0]=='1' && memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) &&memcmp(g_pub_tx.tx_br_no,cBr_no,5))
		{
			vtcp_log("[%s][%d]���������Ĳ����������н���!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P075");
			goto ErrExit;
		}
		zip_space(cPack_id);
		vtcp_log("[%s][%d]cPack_id==[%s]\n",__FILE__,__LINE__,cPack_id);
		if(stat1[0]=='1')
		{
			if(apatoi(cPack_id,8)!=0)
			{
				if(cBr_no[0]!=0x00)
				{
					ret=Lv_wbctl_Dec_Sel( g_pub_tx.reply, "packid='%08d' and in_date='%d' and br_no='%s' ",apatoi(cPack_id,8), pack_date, cBr_no);
				}
				else
				{
					ret=Lv_wbctl_Dec_Sel( g_pub_tx.reply, "packid='%08d' and in_date='%d' ",apatoi(cPack_id,8), pack_date);
				}
			}
			else
			{
				if(cBr_no[0]!=0x00)
				{
					ret=Lv_wbctl_Dec_Sel( g_pub_tx.reply, " in_date='%d' and br_no='%s' ",pack_date, cBr_no);
				}
				else
				{
					ret=Lv_wbctl_Dec_Sel( g_pub_tx.reply,"in_date='%d'", pack_date);
				}
			}
		}
		else
		{
			if(apatoi(cPack_id,8)!=0)
			{
				if(cBr_no[0]!=0x00)
				{
					ret=Lv_lbctl_Dec_Sel( g_pub_tx.reply, "packid='%08d' and in_date='%d' and br_no='%s' ",apatoi(cPack_id,8), pack_date, cBr_no);
				}
				else
				{
					ret=Lv_lbctl_Dec_Sel( g_pub_tx.reply, "packid='%08d' and in_date='%d' ",apatoi(cPack_id,8), pack_date);
				}
			}
			else
			{
				if(cBr_no[0]!=0x00)
				{
					ret=Lv_lbctl_Dec_Sel( g_pub_tx.reply, " in_date='%d' and br_no='%s' ",pack_date, cBr_no);
				}
				else
				{
					ret=Lv_lbctl_Dec_Sel( g_pub_tx.reply,"in_date='%d'", pack_date);
				}
			}
		}

		/**��ɲ�ѯ
		strcat(wherelist,"1=1");
		vtcp_log("��ѯ���� wherelist =[%s]",wherelist);**/
		if(stat1[0]=='1')
		{
			/** ret=Lv_wbctl_Dec_Sel( g_pub_tx.reply,wherelist);**����**/
			ERR_DEAL
			i=0;
			while(1)
			{
				memset(name,'\0',sizeof(name));
        			memset(pcDtl,'\0',sizeof(pcDtl));
				ret = Lv_wbctl_Fet_Sel(&q_lv_wbctl, g_pub_tx.reply );
				vtcp_log("ret =[%d][%s][%d]+++[%s]+++++",ret,__FILE__,__LINE__,(char *)&q_lv_wbctl);	
				if( ret==100 ) 
        			{
        				break;
        			}
				ERR_DEAL
				if( !i)
				{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}          
					fprintf( fp,"~��������   | �� �� �� | �� �� �� |�Է������к�| ��   �� | ��       �� | ״      ̬ |    �����к�  |\n");
				}		
				iLvSetPackStatDtl(q_lv_wbctl.stat,q_lv_wbctl.packstat,pcDtl,q_lv_wbctl.pack_date,q_lv_wbctl.packid,q_lv_wbctl.pkgno); 
				cGetNameByPkgno(&q_lv_wbctl.pkgno,&name);
		  		fprintf(fp,"%s|%d|%s|%s|%d|%.2f|%s|%s|\n",name,q_lv_wbctl.pack_date,q_lv_wbctl.packid,q_lv_wbctl.pay_qs_no,q_lv_wbctl.dtlcnt,q_lv_wbctl.totamt,pcDtl,q_lv_wbctl.br_no);	
				i++;
			}
			ret=Lv_wbctl_Clo_Sel();
			ERR_DEAL
			if(!i)
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
			/* ret=Lv_lbctl_Dec_Sel( g_pub_tx.reply,"pack_date='%d'  ",pack_date);**����**/
			ERR_DEAL
			i=0;
			while(1)
			{
				memset(name,'\0',sizeof(name));
        			memset(pcDtl,'\0',sizeof(pcDtl));
				ret = Lv_lbctl_Fet_Sel( &q_lv_lbctl, g_pub_tx.reply );
				vtcp_log("ret =[%d][%s][%d]+++[%s]+++++",ret,__FILE__,__LINE__,(char *)&q_lv_lbctl);	
				if( ret==100 ) 
        			{
        				break;
        			}
				ERR_DEAL


				if( !i)
				{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}          
					fprintf( fp,"~��������|��  ��  ��|��  ��  ��|�Է������к�|��      ��|��       ��|״      ̬|�����к�|\n");
				}		
				iLvSetPackStatDtl(q_lv_lbctl.sts,q_lv_lbctl.packstat,pcDtl,q_lv_wbctl.pack_date,q_lv_wbctl.packid,q_lv_wbctl.pkgno);
				cGetNameByPkgno(&q_lv_lbctl.pkgno,&name);
		  		fprintf(fp,"%s|%d|%s|%s|%d|%.2f|%s|%s|\n",name,q_lv_lbctl.pack_date,q_lv_lbctl.packid,q_lv_lbctl.pay_qs_no,q_lv_lbctl.dtlcnt,q_lv_lbctl.totamt,pcDtl,q_lv_lbctl.br_no);	
				i++;
			}
			ret=Lv_lbctl_Clo_Sel();
			ERR_DEAL
			if(!i)
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
/***************************************************************************/
/* ��������: iLvSetPackStatDtl                                             */
/* ��������: ���ð�״̬������                                              */
/* ����:                                                                   */
/*            pcPstat-�Զ����״̬                                         */
/*            pcPackstat-���ж���İ�״̬                                  */
/*            pcDtl    -���ذ�״̬������                                   */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2006.01.21                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/

int iLvSetPackStatDtl(char *pcPstat,char *pcPackstat,char *pcDtl,int wt_date,char * packid,char * pkgno){
	if(pcPstat[0]==PSTAT_INVALID){
		strcpy(pcDtl,"��Ч��");
		return(0);
	}
	if(memcmp(pcPackstat,PACKSTAT_REJECT,2)==0){
		strcpy(pcDtl,"�Ѿܾ�");
	}else if(memcmp(pcPackstat,PACKSTAT_SEND,2)==0){
		if(memcmp(pkgno,"004",3)==0 || memcmp(pkgno,"003",3)==0 || memcmp(pkgno,"002",3)==0 || memcmp(pkgno,"006",3)==0 || memcmp(pkgno,"013",3)==0 )
		{
			int iCnt=0;
			iCnt=iFInd920(packid,wt_date);
			if(iCnt>0)
			{
				strcpy(pcDtl,"�������յ�920");
			}
			else
			{
				strcpy(pcDtl,"�ѷ���");
			}
		}
		else
		{
			strcpy(pcDtl,"�ѷ���");
		}
	}else if(memcmp(pcPackstat,PACKSTAT_SUCCESS,2)==0){
		strcpy(pcDtl,"������");
	}else if(memcmp(pcPackstat,PACKSTAT_INQUEUE,2)==0){
		strcpy(pcDtl,"���Ŷ�");
	}else if(memcmp(pcPackstat,PACKSTAT_QS,2)==0){
		strcpy(pcDtl,"������");
	}else if(memcmp(pcPackstat,PACKSTAT_CANCEL,2)==0){
		strcpy(pcDtl,"�ѳ���");
	}else if(memcmp(pcPackstat,PACKSTAT_RECEIPT,2)==0){
		strcpy(pcDtl,"�ѻ�ִ");
	}else if(memcmp(pcPackstat,PACKSTAT_REJECTPAY,2)==0){
		strcpy(pcDtl,"�Ѿܸ�");
	}else if(memcmp(pcPackstat,PACKSTAT_CZ,2)==0){
		strcpy(pcDtl,"�ѳ���");
	}else if(memcmp(pcPackstat,PACKSTAT_STOPPAY,2)==0){
		strcpy(pcDtl,"��ֹ��");
	}else if(memcmp(pcPackstat,PACKSTAT_TIMEOUT,2)==0){
		strcpy(pcDtl,"�ѳ�ʱ");
	}else{
		if(pcPstat[0]==PSTAT_SEND){
			strcpy(pcDtl,"�ѷ���");
		}else if(pcPstat[0]==PSTAT_RECEIVE){
			strcpy(pcDtl,"�ѽ���");
		}else if(pcPstat[0]==PSTAT_PACKED){
			strcpy(pcDtl,"�����");
		}else if(pcPstat[0]==PSTAT_INVALID){
			strcpy(pcDtl,"��Ч��");
		}else{
			strcpy(pcDtl,"δ  ֪");
		}
	}
	return(0);
}  
int cGetNameByPkgno(char *pkgno,char *pkgname)
{
	char cPkgno[4];
	char cPkgname[13];
	memset(cPkgno,'\0',sizeof(cPkgno));
	memset(cPkgname,'\0',sizeof(cPkgname));
	
	memcpy(cPkgno,pkgno,sizeof(cPkgno)-1);
	printf("[%s][%d]cPkgno==[%s]\n",__FILE__,__LINE__,cPkgno);
		
	if(!memcmp(cPkgno,PKGNO_PTDJ,3))
	{
		memcpy(cPkgname,"��ͨ����",sizeof(cPkgname));
	}
	else if(!memcmp(cPkgno,PKGNO_DQDJ,3))
	{
		memcpy(cPkgname,"���ڴ���",12);
	}
	else if(!memcmp(cPkgno,PKGNO_SSDJ,3))
	{
		memcpy(cPkgname,"ʵʱ����",12);
	}
	else if(!memcmp(cPkgno,PKGNO_PTJJ,3))
	{
		memcpy(cPkgname,"��ͨ���",12);
	}
	else if(!memcmp(cPkgno,PKGNO_DQJJ,3))
	{
		memcpy(cPkgname,"���ڽ��",12);
	}
	else if(!memcmp(cPkgno,PKGNO_SSJJ,3))
	{
		memcpy(cPkgname,"ʵʱ���",12);
	}
	else if(!memcmp(cPkgno,PKGNO_DJTH,3))
	{
		memcpy(cPkgname,"�����˻�",12);
	}
	else if(!memcmp(cPkgno,PKGNO_PTJJHZ,3))
	{
		memcpy(cPkgname,"��ͨ��ǻ�ִ",12);
	}
	else if(!memcmp(cPkgno,PKGNO_DQJJHZ,3))
	{
		memcpy(cPkgname,"���ڽ�ǻ�ִ",12);
	}
	else if(!memcmp(cPkgno,PKGNO_SSJJHZ,3))
	{
		memcpy(cPkgname,"ʵʱ��ǻ�ִ",12);
	}
	else if(!memcmp(cPkgno,PKGNO_SSDJHZ,3))
	{
		memcpy(cPkgname,"ʵʱ���ǻ�ִ",12);
	}
	else if(!memcmp(cPkgno,PKGNO_TYXX,3))
	{
		memcpy(cPkgname,"ͨ����Ϣ",12);
	}
	else if(!memcmp(cPkgno,PKGNO_SSXX,3))
	{
		memcpy(cPkgname,"ʵʱ��Ϣ",12);
	}
	printf("[%s][%d]name==[%s]\n",__FILE__,__LINE__,cPkgname);
	memcpy(pkgname,cPkgname,sizeof(cPkgname)-1);
	printf("[%s][%d]pkgname==[%s]\n",__FILE__,__LINE__,pkgname);
	return (0);
}
int iFInd920(char * packid,int wt_date)
{
	int iCnt=0;
	iCnt=sql_count("lv_pkgreg","resp1 is null or resp2 is null and pack_date=%d and packid='%s'",wt_date,packid);
	return iCnt;
}
