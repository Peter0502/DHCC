/***** �������շ��嵥��ѯ(sp5348)��������**********/
/*****************************************************
* �� �� ��:  sp5893.c
* ���������� ����շ��嵥��ѯ
* ��    ��:  dadary
* ������ڣ� 2006��11��2�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "hv_chargemx_c.h"
#include "lv_c554r_c.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "hv_zf_c.h"
#include "hv_zf_c.h"
#include "hv_fzf_c.h"
#include "hv_chargemx_c.h"
#include "com_branch_c.h"

#define  PRICE_303      100.00
#define  PRICE_301      100.00
#define  PRICE_PKG	550.00
#define  PRICE_121	350.00
#define  PRICE_721	100.00

/***���ڴ�С���֧�����׺ͷ�֧�����׵��շѽ��зֿ���ѯ,���ɸ�ʽ�����շѱ�֧���ĵ�****/


struct{
	char sendco[4];	/**���Ĵ���**/
	char qs_brno[12];/**�����к�**/
	char beg_date[8];/*��ʼ����**/
	char end_date[8];/*��������**/
	char txamt[15];	/*�ܽ��**/
	char num[8];	/*�ܱ���**/
	char rate[6];	/*���ۼ�ҵ�����շѱ���*/
}tos88;
char * iNum2TxType(char *ch,char *cTmpList);
int dGet_CMTNO_PRICE( char *cmtno);
int sp5893()
{
	vtcp_log("��ʼС���շ��嵥��ѯ[%s][%d]",__FILE__,__LINE__);

	struct lv_c554r_c lv_c554;
	struct hv_chargemx_c hv_charge;
	LV_FD123_AREA     fd123;
	struct hv_zf_c hv_zf;
	struct hv_fzf_c 	hv_fzf;
	struct com_branch_c	s_com_branch;
	
	char  otype[2];			/***����С��ı�ʶ***/
	
	int   QUERY_DATE;		/***��ѯ����***/
	
	
	
	int    head_flag = 0 ;/****�Ƿ�д���ļ�ͷ�ı�ʶ   1 �Ѿ�д���ļ�ͷ   0  û��д******/
	
	int    NUM_121	= 0;
	int    NUM_721	= 0;
	int    NUM_303	= 0;	/**ʱ�����303�ı���**/
	int    NUM_301  = 0;    /*ʱ�����301�ı���**/
	int    NUM_PKG  = 0;    /*С���֧������**/
	int    NUM_HV   = 0;	/*����֧��������Ŀ**/
	
	double AVERATE_PRICE = 0.0;/**�����շ�-303-301���շѳ����ܵ�֧���ı���,�ó�֧�����׵ĵ���ƽ������**/
	double totamt        = 0.0;/**ĳ������ʱ����ڵ��շ�����**/
	char  cTotamt[15];		/***ĳ�����շѵ��ܺ�***/
	
	double	TMP_PRICE	=	0.0;
	
	int  total_record = 0;     /**�ܵļ�¼����**/
	int  charge_record = 0;		/*802��¼�ĸ���**/
	
	
	char   br_no[6];	/*������*/
	char   beg_mon[7];	/*��ʼ�·�*/
	char   end_mon[7];	/*�����·�*/
	FILE 	*fp;            /*�ļ����*/
	char	cFilename[50];	/*�ļ�����**/	 	
	char 	tx_type[2];	/*��ѯ����,1-552   2-554****/
	int	iRecordNum = 0; /*��¼������*/
	char 	c552_iftype[9]; /*552���ĵ�֪ͨ����**/
	char    c552_pkgno [9]; /*552��ϸ���ı��**/
	int 	ret = 0;
	char    cWherelist[500];
	char 	cTmplist [100];
	
	
	int  	iRecordNum_zf	=	0;
	int  	iRecordNum_ot	=	0;
	int	iRecordNum_fzf	=	0;
	int	iRecordNum_121	=	0;
	int	iRecordNum_721	=	0;
	double  totamt_zf	=	0.0;
	double  totamt_ot	=	0.0;
	double  totamt_fzf	=	0.0;
	double	totamt_121	=	0.0;
	double	totamt_721	=	0.0;
	int	iBrnoNum	=	0;


	memset(&tos88,	0x00,	sizeof(tos88));
	memset(&hv_charge,0x00,sizeof(hv_charge));
	memset(&lv_c554,0x00,sizeof(lv_c554));
	memset(&hv_charge,0x00,sizeof(hv_charge));
	
	memset(&hv_zf,0x00,sizeof(hv_zf));
	memset(&hv_fzf,	0x00,	sizeof(hv_fzf));
	memset(&hv_zf,	0x00,	sizeof(hv_zf));
	
	
	memset(&fd123,	0x00,	sizeof(fd123));
	memset(br_no,	0x00,sizeof(br_no));
	memset(beg_mon,	0x00,sizeof(beg_mon));
	memset(end_mon,	0x00,sizeof(end_mon));
	memset(tx_type,	0x00,sizeof(tx_type));
	memset(c552_pkgno,0x00,sizeof(c552_pkgno));
	memset(c552_iftype,0x00,sizeof(c552_iftype));
	memset(cFilename,0x00,sizeof(cFilename));
	memset(cWherelist,0x00,sizeof(cWherelist));
	memset(cTmplist,	0x00,	sizeof(cTmplist));
	memset(cTotamt,0x00,sizeof(cTotamt));

	memset(&s_com_branch,0x00,sizeof(s_com_branch));
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0910",br_no);
	
	
	QUERY_DATE = apatoi(beg_mon,sizeof(beg_mon));
	QUERY_DATE = QUERY_DATE*100;

	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(br_no,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("[%s][%d]���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	
	/***������������Ĳ�ѯ,���Բ�ѯһ�����ܵ���Ϣ***/
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && !memcmp(br_no,"99999",5))
	{
		vtcp_log("[%s][%d]���������Ĳ�����99999�����Ų�ѯ!\n",__FILE__,__LINE__);
		set_zd_data("0130","���������Ĳ�����99999�����Ų�ѯ!");
		goto ErrExit;
	}
	
	
	/****���ȴ�hv_charge������ȡ���Ʒѿ�ʼ����ʱ��*****/
	sprintf(cWherelist,"edate>=%d and edate<=%d   and ",QUERY_DATE+1,QUERY_DATE+31);
	
	strcat (cWherelist," 1=1  ");
	
	ret=Hv_chargemx_Sel(g_pub_tx.reply,&hv_charge,cWherelist);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(ret==100&&charge_record==0)
	{
		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	
	
	/*****ͨ����hv_charge�л�ȡ��indate�� ��hv_zf�л�ȡ��¼  cmtno+tx_date,ע��  802һ�첻�ܷ�����****/
	
	sprintf(cWherelist,"wt_date=%d and  cmtno='802'  and ",hv_charge.in_date);
	
	strcat (cWherelist," 1=1  ");

		
	ret=Hv_zf_Sel(g_pub_tx.reply,&hv_zf,cWherelist);
	if(ret &&ret!=100)
	{
		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	if(ret==100&&charge_record==0)
	{
		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	totamt = hv_zf.tx_amt * 100;
	memset(&hv_zf,0x00,sizeof(hv_zf));
	
		/***��ʼ�ֶ�δ�hv_charge�� ȡ��¼***/
		/***��ȡ��303�ı���,��301�ı���***/
		NUM_303 = sql_count("hv_fzf",	" wt_date>=%d and wt_date<=%d and cmtno in ('303','301','313','721')  and lw_ind='1'  and otype='1' ",hv_charge.sdate,hv_charge.edate);
		NUM_121 = sql_count("hv_zf",	" wt_date>=%d and wt_date<=%d and cmtno in ('121')  and hv_sts  in('1','2') and lw_ind='1'   ",hv_charge.sdate,hv_charge.edate);
		
		/*********
		NUM_721 = sql_count("hv_fzf",	" wt_date>=%d and wt_date<=%d and cmtno in ('721')   and lw_ind='1'  and otype='1' ",hv_charge.sdate,hv_charge.edate);
		**************/
		NUM_HV =  sql_count("hv_zf",    " wt_date>=%d and wt_date<=%d and hv_sts not in('C','0') and lw_ind='1'  and cmtno not in ('108' ,'121')  ",hv_charge.sdate,hv_charge.edate);
		
		
		 
		vtcp_log("[%s][%d]NUM303==[%d],NUM_HV==[%d]\n",__FILE__,__LINE__,NUM_303,NUM_HV);
		
		vtcp_log("[%s][%d]�ܽ��===[%lf]\n",__FILE__,__LINE__,totamt);
		
		if( (NUM_HV)!=0)
			AVERATE_PRICE = ( totamt-NUM_303*PRICE_303-NUM_121*PRICE_121-NUM_721*PRICE_721 ) /(NUM_HV);
		else 
			AVERATE_PRICE = 0.0;
		
		
		vtcp_log("[%s][%d]ƽ���۸�===[%lf]\n",__FILE__,__LINE__,AVERATE_PRICE);
		
		
		/*************�ṩ�������ĵĻ��ܲ�ѯ*************/
		
		
		if(!memcmp(br_no,"99999",5))
		{
			vtcp_log("[%s][%d]�����������Ĵ�ӡ������Ϣ!\n",__FILE__,__LINE__);
			iBrnoNum=0	;
	
			/*ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1') order by br_no");20150525 lwb  ���ӷ���   */
			ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1','3') order by br_no");
			
			if(ret)
			ERR_DEAL
			
			while(1)
			{
				
				ret = Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
				if(ret && ret!=100)
				ERR_DEAL
				
				if(ret==100)
				{
					vtcp_log("[%s][%d]��ʼ��ӡ������!\n",__FILE__,__LINE__);
					fprintf(fp,"%d|%s|%d|%.2f|%s|",iBrnoNum+1,"��         ��",NUM_PKG+NUM_303+NUM_HV,totamt,"С��");
					break;
				}
					
				iBrnoNum++;
				vtcp_log("[%s][%d]���ǵڡ�%d�����ˣ�br_no=[%s]\n",__FILE__,__LINE__,iBrnoNum,s_com_branch.br_no);
				if(iBrnoNum==1)	/**��һ��д�ļ�**/
				{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w" );
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
					vtcp_log("[%s][%d]��ʼ��ӡ��!!!!!!!\n",__FILE__,__LINE__);
					fprintf( fp,"~���|��������|�ܱ���|�ܽ��|������|�����|�ƴ����|�ƴ���|��֧������|��֧�����|ժҪ|\n" );
					
				}
				
				iRecordNum_zf = 0;
				iRecordNum_fzf= 0;
				iRecordNum_ot = 0;
				iRecordNum_121 =0;
				
				totamt_121	= 0.0;
				totamt_ot	= 0.0;
				totamt_zf	= 0.0;
				totamt_fzf	= 0.0;
				
				iRecordNum_ot = sql_count ( "hv_zf" ,  " br_no = '%.5s' and wt_date>=%d and wt_date<=%d and hv_sts not in('0','C' ) and lw_ind='1' and cmtno not in ('108','121')   ",s_com_branch.br_no,hv_charge.sdate,hv_charge.edate);
				iRecordNum_121 = sql_count ( "hv_zf" ,  " br_no = '%.5s' and wt_date>=%d and wt_date<=%d and hv_sts  in('1','2' ) and lw_ind='1' and cmtno  in ('121')   ",s_com_branch.br_no,hv_charge.sdate,hv_charge.edate);
				iRecordNum_fzf = sql_count ( "hv_fzf" ,  "br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('301','303','313','721')   and lw_ind='1' and otype='1'  ",s_com_branch.br_no,hv_charge.sdate,hv_charge.edate);
				
				totamt_ot = iRecordNum_ot * AVERATE_PRICE;
				totamt_121= iRecordNum_121* PRICE_121;
				totamt_fzf= iRecordNum_fzf* PRICE_303;
				totamt_zf = totamt_ot + totamt_121;
				
				
				iRecordNum_zf = iRecordNum_121 + iRecordNum_ot;
				
				vtcp_log("[%s][%d]�ܽ��=[%.2f],iRecordNum_zf=[%d],iRecordNum_fzf==[%d],iRecordNum_121==[%d]\n",__FILE__,__LINE__,totamt,iRecordNum_zf,iRecordNum_fzf,iRecordNum_121);
				fprintf(fp,"%d|%s|%d|%.2f|%d|%.2f|%d|%.2f|%d|%.2f|%s|\n",iBrnoNum,s_com_branch.br_name,iRecordNum_zf+iRecordNum_fzf,totamt_zf+totamt_fzf,
							iRecordNum_ot,totamt_ot,iRecordNum_121,totamt_121,iRecordNum_fzf,totamt_fzf,"С��");
				
			
			}
		
		
		
		
		set_zd_data( DC_FILE_SND,"1");
		
		fclose(fp);
		
		vtcp_log("[%s][%d]����ȥ��!\n",__FILE__,__LINE__);
		goto OkExit;
		
		}
		
		/**1.��ʼ����pkgno����****/
		iRecordNum = 0;
		ret = Hv_zf_Dec_Sel(g_pub_tx.reply," br_no = '%s' and wt_date>=%d and wt_date<=%d  and lw_ind='1'  and ( ( cmtno not in ('108' ,'121') and hv_sts not in('C','0') )  or ( cmtno='121' and hv_sts in ('1','2')) ) order by cmtno,wt_date ",br_no,hv_charge.sdate,hv_charge.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			TMP_PRICE= 0.0;
			ret = Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )
			{	
				vtcp_log("[%s][%d]���֧����¼�Ѿ�������,��ʼ�ҷ�֧��!\n",__FILE__,__LINE__);
				totamt_zf = iRecordNum_ot * AVERATE_PRICE + iRecordNum_121 * PRICE_121;
				vtcp_log("[%s][%d]֧�����׵��ܽ��===[%.2lf]\n",__FILE__,__LINE__,totamt_zf);
				iRecordNum_fzf  = 0;
		
		
				ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('303','301','313','721')  and lw_ind='1'  and otype='1' ",br_no,hv_charge.sdate,hv_charge.edate);
				if(ret )
				{
					vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
					strcpy(g_pub_tx.reply,"D104");
					goto ErrExit;
				}
				while(1)
				{
					ret = Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
					if(ret && ret!=100)
					{
						vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"D104");
						goto ErrExit;
					}
					else if(ret==100 )
					{	
						vtcp_log("[%s][%d]�Ѿ������˷�֧��!\n",__FILE__,__LINE__);
						totamt_fzf = iRecordNum_fzf * PRICE_303;
						vtcp_log("[%s][%d]��֧�����׵��ܽ��===[%.2lf]\n",__FILE__,__LINE__,totamt_fzf);
						if(head_flag!=1)	/**��һ��д�ļ�**/
						{
							pub_base_AllDwnFilName(cFilename);
							fp = fopen(cFilename,"w" );
							if( fp==NULL )
							{
								sprintf(acErrMsg," open file [%s] error ",cFilename);
								WRITEMSG
								strcpy( g_pub_tx.reply,"S047" );
								goto ErrExit;
							}
							vtcp_log("!!!!!!!!!!!!!!![%s]\n",cFilename);
							fprintf( fp,"~���|CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע||||||\n" );
							head_flag   =1 ;
						}
						/**
						fprintf( fp,"%s|%d|%s|%.2f|%s|%d|%s|%.2f|%s|%d|%s|%.2f|\n","�ܱ���:",iRecordNum_zf+iRecordNum_fzf,"�ܽ��:",totamt_zf+totamt_fzf,
							"֧������:",iRecordNum_zf,"֧���ܽ��:",totamt_zf,"��֧������:",iRecordNum_fzf,"��֧���ܽ��:",totamt_fzf);*/
						
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","�ܱ���:",iRecordNum_zf+iRecordNum_fzf,"�ܽ��:",totamt_zf+totamt_fzf);
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","������:",iRecordNum_ot,"�����:",iRecordNum_ot * AVERATE_PRICE);
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","�ƴ����:",iRecordNum_121,"�ƴ���:",iRecordNum_121 * PRICE_121);
						fprintf(fp,"%s|%s|%d|%s|%.2f|\n","         ","��֧������:",iRecordNum_fzf,"��֧�����:",totamt_fzf);
						/***
						fprintf(fp,"%s|%d|%s|%.2f|%s|%d|%s|%.2f|%s|%d|%s|%.2f|\n","���ȱ���:",iRecordNum_ot,"���Ƚ��:",iRecordNum_ot * AVERATE_PRICE,
								"�ʽ��ƴ����:",iRecordNum_121,"�ƴ��",iRecordNum_121 * PRICE_121,"��֧������:",iRecordNum_fzf,"��֧�����:",totamt_fzf);
						****/
						break;
					}
					
					iRecordNum_fzf ++;
					vtcp_log("[%s][%d]���Ƿ�֧���ĵ�[%d]�ε�!\n",__FILE__,__LINE__,iRecordNum_fzf);
					if(head_flag!=1)	/**��һ��д�ļ�**/
					{
						pub_base_AllDwnFilName(cFilename);
						fp = fopen(cFilename,"w" );
						if( fp==NULL )
						{
							sprintf(acErrMsg," open file [%s] error ",cFilename);
							WRITEMSG
							strcpy( g_pub_tx.reply,"S047" );
							goto ErrExit;
						}
						vtcp_log("!!!!!!!!!!!!!!![%s]\n",cFilename);
						fprintf( fp,"~���|CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע||||||\n" );
						head_flag   =1 ;
					}
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
					fprintf( fp,"%d|%s|%s|%s|%d|%.2f|%.2f|%s||||||\n",iRecordNum_fzf+iRecordNum_zf,hv_fzf.cmtno,br_no,hv_fzf.orderno,hv_fzf.wt_date,PRICE_303,PRICE_303,"С��");
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
				 }
				 Hv_fzf_Clo_Sel();
				 break;  /***����С��֧����Ӧ��whileѭ��***/
			}
			
			iRecordNum_zf ++;
			vtcp_log("[%s][%d]hv_zf�ĵ�[%d]��ѭ��!\n",__FILE__,__LINE__,iRecordNum_zf);
			
			if(iRecordNum_zf==1)	/**��һ��д�ļ�**/
			{
				pub_base_AllDwnFilName(cFilename);
				fp = fopen(cFilename,"w" );
				if( fp==NULL )
				{
					sprintf(acErrMsg," open file [%s] error ",cFilename);
					WRITEMSG
					strcpy( g_pub_tx.reply,"S047" );
					goto ErrExit;
				}
				fprintf( fp,"~���|CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע||||||||\n" );
				head_flag   =1 ;
			}
			if(!memcmp(hv_zf.cmtno,"121",3))
			{
				iRecordNum_121 ++;
				TMP_PRICE = PRICE_121;
			}
			else
			{
				iRecordNum_ot ++;
				TMP_PRICE = AVERATE_PRICE;
			}	
				
			fprintf( fp,"%d|%s|%s|%s|%d|%.2f|%.2f|%s|\n",iRecordNum_zf,hv_zf.cmtno,br_no,hv_zf.orderno,hv_zf.wt_date,TMP_PRICE,TMP_PRICE,"���");
			
		}
		
		
		Hv_zf_Clo_Sel();
		
		
		set_zd_data( DC_FILE_SND,"1");
		fclose(fp);
		

OkExit:
	sprintf(cTotamt,"%15f",totamt);
	set_zd_data("0390",cTotamt);
		
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

char * iNum2TxType(char *ch,char *cTmpList)
{

	vtcp_log("[%s][%d]ch===[%s]\n",__FILE__,__LINE__,ch);
	
	char cName[20];
	memset(cName,0x00,sizeof(cName));

        if(!memcmp(ch,"001",3))
        {
                strcpy(cName,"��ͨ����");
        }
        else if(!memcmp(ch,"002",3))
        {
                strcpy(cName,"��ͨ���");
        }
        else if(!memcmp(ch,"003",3))
        {
                strcpy(cName,"ʵʱ����");
        }
        else if(!memcmp(ch,"004",3))
        {
                strcpy(cName,"ʵʱ���");

	}
        else if(!memcmp(ch,"005",3))
        {
                strcpy(cName,"���ڴ���");
        }
        else if(!memcmp(ch,"006",3))
        {
                strcpy(cName,"���ڽ��");
        }
        else if(!memcmp(ch,"007",3))
        {
                strcpy(cName,"�����˻�");
        }
        else if(!memcmp(ch,"008",3))
        {
                strcpy(cName,"��ͨ��ǻ�ִ");
        }
        else if(!memcmp(ch,"009",3))
        {
                strcpy(cName,"ʵʱ���ǻ�ִ");
        }
        else if(!memcmp(ch,"010",3))

	 {
                strcpy(cName,"ʵʱ��ǻ�ִ");
        }
        else if(!memcmp(ch,"011",3))
        {
                strcpy(cName,"���ڽ�ǻ�ִ");
        }
        else if(!memcmp(ch,"012",3))
        {
                strcpy(cName,"ͨ����Ϣ");
        }
        else if(!memcmp(ch,"013",3))
        {
                strcpy(cName,"ʵʱ��Ϣ");
        }
        else if(!memcmp(ch,"301",3))
        {
                strcpy(cName,"С���ѯ");
        }
        else if(!memcmp(ch,"302",3))
        {
                strcpy(cName,"С��鸴");

	 }
        else if(!memcmp(ch,"303",3))
        {
                strcpy(cName,"���ɸ�ʽ");
        }
        else if(!memcmp(ch,"319",3))
        {
                strcpy(cName,"�����˻�");
        }
        else if(!memcmp(ch,"320",3))
        {
                strcpy(cName,"�˻�Ӧ��");
        }
        else if(!memcmp(ch,"321",3))
        {
                strcpy(cName,"���볷��");
        }
        else if(!memcmp(ch,"322",3))
        {
                strcpy(cName,"����Ӧ��");
        }
        else if(!memcmp(ch,"323",3))
	{
	        strcpy(cName,"��������");
        }
        else if(!memcmp(ch,"324",3))
        {
                strcpy(cName,"����Ӧ��");
        }
        else if(!memcmp(ch,"325",3))
        {
                strcpy(cName,"����֪ͨ");
        }
        else if(!memcmp(ch,"327",3))
        {
                strcpy(cName,"ֹ������");
        }
        else if(!memcmp(ch,"328",3))
        {
                strcpy(cName,"ֹ��Ӧ��");
        }
        else
                strcpy(cName,"δ֪��������");
        memcpy(cTmpList,cName,strlen(cName));

	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
        return  cName;
}

int dGet_CMTNO_PRICE( char *cmtno)
{
	if(!strncmp(cmtno,"301",3))
		return PRICE_301;
	if(!strncmp(cmtno,"303",3))
		return PRICE_303;
	return 0;
}

char * cGet_LV_HV(char otype)
{
	char otype_des[5];
	memset(otype_des,0x00,sizeof(otype_des));
	
	if(otype==1)
		memcpy(otype_des,"���",4);
	if(otype==2)
		memcpy(otype_des,"С��",4);
	
	return otype_des;
		
} 
