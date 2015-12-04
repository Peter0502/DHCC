/*************************************************
* �� �� ��:  spJ091.c
* ���������� С����Ϣ����ʷ��ѯ������
* ��    ��:  ligl
* ������ڣ� 2006��8��29��
* �޸ļ�¼�� 
*   ��    ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_fd123.h"
#include "find_debug.h"

spJ091()
{
        int ret=0,i;
	struct lv_pkgreg_c lv_pkgreg;
	LV_FD123_AREA lv_fd123;
	char cKinbr[6];
	char cBr_no[6];/*������*/
	char cWt_date[9];/*ί������*/
	char cKinbr2[6];
	char cBr_no2[6];/*������*/
	char cWt_date2[9];/*ί������*/
	char cDate[9];
	char cPkgflag[2];/*��־*/
	char cPkgflag2[2];/*��־*/

	char cTxstat[2];/*����״̬*/
	char cPkgno[4];/*������*/
	char cPkgno1[4];
	char cPkgno12[4];

	char cPay_ac_no[33];/**/
	char cCash_ac_no[33];/**/
	long cMintxamt=0.0;/*��С���*/
	long cMaxtxamt=0.0;/*�����*/
	char cFilename[100];/*�ļ���*/
	int ttlnum=0;	    /**��ȡ��������**/
	FILE *fp;
	char wherelist[1024];  /**��ѯ����**/
	char Pkgnoname[11];
	char RcpStat[3];
	char RcpStatname[21];
	memset(RcpStatname ,0, sizeof(RcpStatname ));
	memset(RcpStat     ,0, sizeof(RcpStat ));
	memset(Pkgnoname   ,'\0', sizeof(Pkgnoname ));
	memset(cBr_no      ,'\0', sizeof(cBr_no ));
	memset(cKinbr      ,'\0', sizeof(cKinbr ));
	memset(cDate   	   ,'\0', sizeof(cDate));	
	memset(cWt_date    ,'\0', sizeof(cWt_date));	
	memset(cPkgflag    ,'\0', sizeof(cPkgflag));
	memset(cBr_no2      ,'\0', sizeof(cBr_no2 ));
	memset(cKinbr2      ,'\0', sizeof(cKinbr2 ));
	memset(cPkgno12   	   ,'\0', sizeof(cPkgno12));	
	memset(cWt_date2    ,'\0', sizeof(cWt_date2));	
	memset(cPkgflag2    ,'\0', sizeof(cPkgflag2));

	memset(cTxstat     ,'\0', sizeof(cTxstat ));
	memset(cPkgno      ,'\0', sizeof(cPkgno  ));
	memset(cPkgno1     ,'\0', sizeof(cPkgno1 ));
	memset(cFilename  ,'\0', sizeof(cFilename));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(&lv_fd123,0x00,sizeof(lv_fd123));
	memset(wherelist ,0,sizeof(wherelist));
	vtcp_log("spJ090��ʼ��[%s][%d]",__FILE__,__LINE__);
	/**��ʼ����**/
	pub_base_sysinit();
	get_zd_data("0030",cKinbr);
	get_zd_data("0440",cWt_date);
	get_zd_data("0670",cPkgflag);
	get_zd_data("0770",cBr_no);
	get_zd_data("0230",cPkgno1);
	get_zd_long("0410",&cMintxamt);
	get_zd_long("0420",&cMaxtxamt);
	get_zd_data("123A",cPay_ac_no);
	get_zd_data("123B",cCash_ac_no);
	if(cWt_date[0]=='\0') /****����Ϊ��ʱ��Ҳ���Բ���*******/
	{
	 	MEMCPY_DEBUG(cDate, "%", 1);
	}
	else
	{
	 	MEMCPY_DEBUG(cDate, cWt_date,sizeof(cDate)-1);
	}
	if(memcmp(cPkgno1,"000",3)==0)
	{
	 	MEMCPY_DEBUG(cPkgno, "%", 1);
	}
	 else
	{
	 	MEMCPY_DEBUG(cPkgno, cPkgno1,sizeof(cPkgno)-1);
	}
	/**��ɲ�ѯ����**/
	if(memcmp(cKinbr, QS_BR_NO ,5)==0)
	{
		if(cBr_no[0]=='\0')
		{
			if(memcmp(cPkgno,"007",3)==0)
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%'",cDate);
				}
				else
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%' and lv_sts='%s'",cTxstat);
				}
			}
			else
			{
				
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%'",cDate,cPkgno);
				}
				else
				{
					sprintf(wherelist,"wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%' AND lv_sts='%s'",cDate,cPkgno,cTxstat);
				}
			}		
		}
		else
		{
			if(memcmp(cPkgno,"007",3)==0)
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%'",cBr_no,cDate);
				}
				else
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%' and lv_sts='%s'",cBr_no,cDate,cTxstat);
				}
			}
			else
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%'  pkgno like '%%%%%s%%%%'",cBr_no,cDate,cPkgno);
				}
				else
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%' AND stat='%s'",cBr_no,cDate,cPkgno,cTxstat);
				}
			}	
		}
	}
	else
	{
		if(memcmp(cBr_no,cKinbr,sizeof(cBr_no))!=0)
		{
			if(cBr_no[0]=='\0')
			{
				sprintf(acErrMsg," �����в���Ϊ��[%s][%d]",__FILE__,__LINE__);
				WRITEMSG
				goto ErrExit;				
			}
			else
			{
				sprintf(acErrMsg,"[%s][%d]���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
				WRITEMSG
				goto ErrExit;
			}	
	        }
	        else
	        {
	        	if(memcmp(cPkgno,"007",3)==0)
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no = '%s' and wt_date LIKE '%%%%%s%%%%'",cBr_no,cDate);
				}
				else
				{
					sprintf(wherelist,"br_no='%s' and wt_date LIKE '%%%%%s%%%%' and lv_sts='%s'",cBr_no,cDate,cTxstat);
				}
			}
			else
			{
				if(cTxstat[0]=='A')
				{
					sprintf(wherelist,"br_no='%s' and wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%'",cBr_no,cDate,cPkgno);
				}
				else
				{
					sprintf(wherelist,"br_no ='%s' and wt_date LIKE '%%%%%s%%%%' and pkgno like '%%%%%s%%%%' AND stat='%s'",cBr_no,cDate,cPkgno,cTxstat);
				}
			}		
	        }
	        
	}
	printf("[%s][%d]��ѯ���:\n%s\n",__FILE__,__LINE__,wherelist);
	/***�����տ��ʺź͸����ʺ��������***/
	if(strlen(cPay_ac_no)!=0)
	{
		sprintf(wherelist+strlen(wherelist),"  and pay_ac_no='%s' ",cPay_ac_no);
	}
	printf("[%s][%d]��ѯ���:\n%s\n",__FILE__,__LINE__,wherelist);
	if(strlen(cCash_ac_no)!=0)
	{	
		sprintf(wherelist+strlen(wherelist),"  and cash_ac_no='%s' ",cCash_ac_no);
	}
	printf("[%s][%d]��ѯ���:\n%s\n",__FILE__,__LINE__,wherelist);
	/****************���******************/
	/*******��������Ľ���ȡֵ��Χ******************************/
	if(cMintxamt!=0||cMaxtxamt!=0)
	{	
		sprintf(wherelist+strlen(wherelist)," and tx_amt between '%.2f' and '%.2f' ",cMintxamt,cMaxtxamt);
	}
		printf("[%s][%d]��ѯ���:\n%s\n",__FILE__,__LINE__,wherelist);
	/*��ʼ��ѯ*/
        ret=Lv_pkgreg_Dec_Sel(g_pub_tx.reply, wherelist);
        ERR_DEAL
        ttlnum=0;
        while(1)
        {
        	ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
        	if( ret==100 ) 
        	{
        		break;
        	}
		ERR_DEAL
		if( !ttlnum )
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
			fprintf( fp,"~��  ��  ��|ί��  ����|����  ���|����  ���|��  ��  ��|״   ̬|����  �к�|����  �к�|¼�����Ա|���˲���Ա|\n" );
		}
	   	if(lv_pkgreg.lv_sts[0]== 'H')
	   	{
			memset(RcpStat, 0, sizeof(RcpStat));
			memset(RcpStatname,0, sizeof(RcpStatname));
			if(lv_pkgreg.resp1[3] == 'I')
			{
				MEMCPY_DEBUG(RcpStat,lv_pkgreg.rcpstat,sizeof(lv_pkgreg.rcpstat)-1);
				pub_base_strpack_all(RcpStat);
				if(strlen(RcpStat)==0) 
				{
					MEMCPY_DEBUG(RcpStat,"11",2);
				}
				if(memcmp(RcpStat, "00", 2) == 0)
				{
	                		MEMCPY_DEBUG(RcpStatname,"�ɹ�", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "01", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"�ʺŲ�����", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "02", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"�˺Ż�������", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "03", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"�˻�����֧��", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "10", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"���������", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "11", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"�˻�״̬����", sizeof(RcpStatname)-1);
				}
				else if(memcmp(RcpStat, "20", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"ҵ���ѳ���", sizeof(RcpStatname)-1);
				}else if(memcmp(RcpStat, "90", 2) == 0)
				{
					MEMCPY_DEBUG(RcpStatname,"��������", sizeof(RcpStatname)-1);
				} 		
			}else
			{
				if(lv_pkgreg.resp1[3] == '0')
				{
					MEMCPY_DEBUG(RcpStatname,"¼��ȡ��", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '1')
				{
					MEMCPY_DEBUG(RcpStatname,"����¼��", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '2')
				{
					MEMCPY_DEBUG(RcpStatname,"���˸��˹���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '3')
				{
					MEMCPY_DEBUG(RcpStatname,"���˸���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '4')
				{
					MEMCPY_DEBUG(RcpStatname,"���˷���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '5')
				{
					MEMCPY_DEBUG(RcpStatname,"���˾ܾ�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '6')
				{
					MEMCPY_DEBUG(RcpStatname,"��������", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '7')
				{
					MEMCPY_DEBUG(RcpStatname,"�����Ŷ�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '8')
				{
					MEMCPY_DEBUG(RcpStatname,"���˳����ȴ�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == '9')
				{
					MEMCPY_DEBUG(RcpStatname,"���˳���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'A')
				{
					MEMCPY_DEBUG(RcpStatname,"�˻�ȴ�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'B')
				{
					MEMCPY_DEBUG(RcpStatname,"�����˻�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'C')
				{
					MEMCPY_DEBUG(RcpStatname,"���˳���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'D')
				{
					MEMCPY_DEBUG(RcpStatname,"����ֹ��", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'G')
				{
					MEMCPY_DEBUG(RcpStatname,"�����쳣����", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'H')
				{
					MEMCPY_DEBUG(RcpStatname,"��ʱ���ղ�����", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'I')
				{
					MEMCPY_DEBUG(RcpStatname,"��ʽ����", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'J')
				{
					MEMCPY_DEBUG(RcpStatname,"ȫ��Ѻ��", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'K')
				{
					MEMCPY_DEBUG(RcpStatname,"�ط�Ѻ��", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'L')
				{
					MEMCPY_DEBUG(RcpStatname,"ȫ��Ѻ�ط�Ѻ����", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'M')
				{
					MEMCPY_DEBUG(RcpStatname,"���Ĺ���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'N')
				{
					MEMCPY_DEBUG(RcpStatname,"֧�й���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'O')
				{
					MEMCPY_DEBUG(RcpStatname,"�����˻صȴ�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'P')
				{
					MEMCPY_DEBUG(RcpStatname,"�����˻�", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'Q')
				{
					MEMCPY_DEBUG(RcpStatname,"ͬ��δ����", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'R')
				{
					MEMCPY_DEBUG(RcpStatname,"ͬ���ѷ���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'S')
				{
					MEMCPY_DEBUG(RcpStatname,"��ͬ��δ����", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'T')
				{
					MEMCPY_DEBUG(RcpStatname,"��ͬ���ѷ���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'U')
				{
					MEMCPY_DEBUG(RcpStatname,"�����ѳ���", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'V')
				{
					MEMCPY_DEBUG(RcpStatname,"������ֹ��", sizeof(RcpStatname)-1);						
				}
				else if(lv_pkgreg.resp1[3] == 'Z')
				{
					MEMCPY_DEBUG(RcpStatname,"�����쳣����", sizeof(RcpStatname)-1);						
				}
				
			}
			
		}
		else
		{
			if(lv_pkgreg.lv_sts[0]== '0')
			{
				MEMCPY_DEBUG(RcpStatname,"¼��ȡ��", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '1')
			{
				MEMCPY_DEBUG(RcpStatname,"����¼��", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '2')
			{
				MEMCPY_DEBUG(RcpStatname,"���˸��˹���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '3')
			{
				MEMCPY_DEBUG(RcpStatname,"���˸���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '4')
			{
				MEMCPY_DEBUG(RcpStatname,"���˷���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '5')
			{
				MEMCPY_DEBUG(RcpStatname,"���˾ܾ�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '6')
			{
				MEMCPY_DEBUG(RcpStatname,"��������", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '7')
			{
				MEMCPY_DEBUG(RcpStatname,"�����Ŷ�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '8')
			{
				MEMCPY_DEBUG(RcpStatname,"���˳����ȴ�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == '9')
			{
				MEMCPY_DEBUG(RcpStatname,"���˳���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'A')
			{
				MEMCPY_DEBUG(RcpStatname,"�˻�ȴ�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'B')
			{
				MEMCPY_DEBUG(RcpStatname,"�����˻�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'C')
			{
				MEMCPY_DEBUG(RcpStatname,"���˳���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'D')
			{
				MEMCPY_DEBUG(RcpStatname,"����ֹ��", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'G')
			{
				MEMCPY_DEBUG(RcpStatname,"�����쳣����", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'H')
			{
				MEMCPY_DEBUG(RcpStatname,"��ʱ���ղ�����", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'I')
			{
				MEMCPY_DEBUG(RcpStatname,"��ʽ����", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'J')
			{
				MEMCPY_DEBUG(RcpStatname,"ȫ��Ѻ��", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'K')
			{
				MEMCPY_DEBUG(RcpStatname,"�ط�Ѻ��", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'L')
			{
				MEMCPY_DEBUG(RcpStatname,"ȫ��Ѻ�ط�Ѻ����", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'M')
			{
				MEMCPY_DEBUG(RcpStatname,"���Ĺ���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'N')
			{
				MEMCPY_DEBUG(RcpStatname,"֧�й���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'O')
			{
				MEMCPY_DEBUG(RcpStatname,"�����˻صȴ�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'P')
			{
				MEMCPY_DEBUG(RcpStatname,"�����˻�", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'Q')
			{
				MEMCPY_DEBUG(RcpStatname,"ͬ��δ����", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'R')
			{
				MEMCPY_DEBUG(RcpStatname,"ͬ���ѷ���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'S')
			{
				MEMCPY_DEBUG(RcpStatname,"��ͬ��δ����", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'T')
			{
				MEMCPY_DEBUG(RcpStatname,"��ͬ���ѷ���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'U')
			{
				MEMCPY_DEBUG(RcpStatname,"�����ѳ���", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'V')
			{
				MEMCPY_DEBUG(RcpStatname,"������ֹ��", sizeof(RcpStatname)-1);						
			}
			else if(lv_pkgreg.lv_sts[0] == 'Z')
			{
				MEMCPY_DEBUG(RcpStatname,"�����쳣����", sizeof(RcpStatname)-1);						
			}
		}		
		fprintf( fp,"%s|%ld|%s|%.2f|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.br_no,lv_pkgreg.wt_date,lv_pkgreg.orderno,lv_pkgreg.tx_amt,lv_pkgreg.packid,RcpStatname,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.input_tel,lv_pkgreg.check_tel);
		ttlnum++;
        }
	ret=Lv_pkgreg_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;	
}
