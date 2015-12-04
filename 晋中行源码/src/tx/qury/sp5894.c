/***** �������շ��嵥��ѯ(sp5348)��������**********/
/*****************************************************
* �� �� ��:  sp5894.c
* ���������� С���շ��嵥��ѯ
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
#include "lv_c552r_c.h"
#include "lv_c554r_c.h"
#include "lv_define.h"
#include "lv_fd123.h"
#include "lv_pkgreg_c.h"
#include "hv_zf_c.h"
#include "hv_fzf_c.h"
#include "com_branch_c.h"


/******��λ    Ԫ  *****/
#define  PRICE_303     100.00 
#define  PRICE_301     100 
#define  PRICE_PKG	   500 

/***���ڴ�С���֧�����׺ͷ�֧�����׵��շѽ��зֿ���ѯ,���ɸ�ʽ�����շѱ�֧���ĵ�****/

int dGet_CMTNO_PRICE( char *cmtno);
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
int sp5894()
{
	vtcp_log("��ʼС���շ��嵥��ѯ[%s][%d]",__FILE__,__LINE__);

	struct lv_c552r_c lv_c552;
	struct lv_c554r_c lv_c554;
	LV_FD123_AREA     fd123;
	struct lv_pkgreg_c lv_pkgreg;
	struct hv_fzf_c 	hv_fzf;
	struct hv_zf_c     hv_zf;
	
	struct com_branch_c s_com_branch;
	char  otype[2];			/***����С��ı�ʶ***/
	
	int total_num = 0;		/*�ܼƵļ�¼��,С���֧�����Ϸ�֧��***/
	
	
	
	int    head_flag = 0 ;/****�Ƿ�д���ļ�ͷ�ı�ʶ   1 �Ѿ�д���ļ�ͷ   0  û��д******/
	
	int    NUM_303	= 0;	/**ʱ�����303�ı���**/
	int    NUM_301  = 0;    /*ʱ�����301�ı���**/
	int    NUM_PKG  = 0;    /*С���֧������**/
	int    NUM_HV   = 0;	/*����֧��������Ŀ**/
	int    c552_num = 0;    /*552��¼�ĸ���****/
	int    c552_indate = 0; /*�����ܵ�552Ҫ��lv_c552��д�������¼,������ֻ��Ҫһ��,��ͨ��indate��������***/
	
	double AVERATE_PRICE = 0.0;/**�����շ�-303-301���շѳ����ܵ�֧���ı���,�ó�֧�����׵ĵ���ƽ������**/
	double totamt        = 0.0;/**ĳ������ʱ����ڵ��շ�����**/
	char  cTotamt[15];		/***ĳ�����շѵ��ܺ�***/
	
	
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

	char    old_br_no[6];	/***�ɵĻ���,�������м�¼��ʱ�� �ֿ�����***/
	
	int 	iRecordNum_zf	=	0;
	int	iRecordNum_fzf	=	0;
	double	totamt_zf	=	0.0;
	double	totamt_fzf	=	0.0;
	
	int	iBrnoNum 	=	0.0;/***֧�еĵڼ���***/
	
	


	memset(&tos88,	0x00,	sizeof(tos88));
	memset(&lv_c552,0x00,sizeof(lv_c552));
	memset(&lv_c554,0x00,sizeof(lv_c554));
	memset(&lv_pkgreg,0x00,sizeof(lv_pkgreg));
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

	memset(old_br_no,0x00,sizeof(old_br_no));
	
	
	memset(&s_com_branch,0x00,sizeof(s_com_branch));
	
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0450",end_mon);
	get_zd_data("0910",br_no);
	get_zd_data("0680",otype);
	

	vtcp_log("[%s][%d]beg_mon=[%s]\n",__FILE__,__LINE__,beg_mon);
	vtcp_log("[%s][%d]end_mon=[%s]\n",__FILE__,__LINE__,end_mon);
	vtcp_log("[%s][%d]br_no=[%s]\n",__FILE__,__LINE__,br_no);
	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(br_no,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("[%s][%d]���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	sprintf(cWherelist,"paymon>='%.6s' and paymon<='%.6s' and ",beg_mon,end_mon);
	
	
	
	strcat (cWherelist," 1=1 order by paymon ");
	
	
	/***������������Ĳ�ѯ,���Բ�ѯһ�����ܵ���Ϣ***/
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && !memcmp(br_no,"99999",5))
	{
		vtcp_log("[%s][%d]���������Ĳ�����99999�����Ų�ѯ!\n",__FILE__,__LINE__);
		set_zd_data("0130","���������Ĳ�����99999�����Ų�ѯ!");
		goto ErrExit;
	}
	
	if(!memcmp(br_no,"99999",5))
	{
		vtcp_log("[%s][%d]��������ʹ����99999�����Ų�ѯ������Ϣ",__FILE__,__LINE__);
		
		ret = Lv_c552r_Sel(g_pub_tx.reply,&lv_c552,cWherelist);
		ERR_DEAL
		
		totamt = atof(lv_c552.totamt);
		vtcp_log("[%s][%d]�ܽ��===[%.2f]\n",__FILE__,__LINE__,totamt);
		/***��ʼ�ֶ�δ�lv_c552�� ȡ��¼***/
		/***��ȡ��303�ı���,��301�ı���***/

		NUM_303 = sql_count("hv_fzf"," wt_date>=%d and wt_date<=%d and cmtno in ('303','301','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1'  and otype='2' ",lv_c552.sdate,lv_c552.edate);
		NUM_PKG = sql_count("lv_pkgreg","wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1' ",lv_c552.sdate,lv_c552.edate);
		
		if( (NUM_PKG+NUM_303)!=0) /***�ܵ�С���շѽ���ȥ ��֧����ȡ�Ľ�� /�ܵ�֧������  �õ����� ****/
		{
			AVERATE_PRICE = ( totamt - NUM_303*PRICE_303 ) / NUM_PKG;
			vtcp_log("[%.2f]= [%.2f]-[%d * %d ]/[%d] \n",AVERATE_PRICE,totamt,NUM_303,PRICE_303,NUM_PKG);
		}
		else 
			AVERATE_PRICE = 0.0;
			
	
		iBrnoNum=0	;
	
		/*ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1') order by br_no"); lwb 20150525 ������� */
		ret = Com_branch_Dec_Sel(g_pub_tx.reply," br_type in ('4','1','3') order by br_no");
		ERR_DEAL
		
		while(1)
		{
			
			ret = Com_branch_Fet_Sel(&s_com_branch,g_pub_tx.reply);
			if(ret && ret!=100)
			ERR_DEAL
			
			if(ret==100)
			{
				vtcp_log("[%s][%d]��ʼ��ӡ������!\n",__FILE__,__LINE__);
				fprintf(fp,"%d|%s|%d|%.2f|%d|%.2f|%d|%.2f|%s|",iBrnoNum+1,"��         ��",NUM_PKG+NUM_303,totamt,
						NUM_PKG,NUM_PKG*AVERATE_PRICE,NUM_303,NUM_303 * PRICE_303,"С��");
				
				break;
			}
				
			iBrnoNum++;
			
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
				fprintf( fp,"~���|��������|�ܱ���|�ܽ��|֧������|��֧�����|��֧������|��֧�����|ժҪ|\n" );
				
			}
			
			iRecordNum_zf = 0;
			iRecordNum_fzf= 0;
			totamt_zf	= 0.0;
			totamt_fzf	= 0.0;
			
			iRecordNum_zf = sql_count ( "lv_pkgreg" ,  " br_no = '%.5s' and wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1'   ",s_com_branch.br_no,lv_c552.sdate,lv_c552.edate); 
			iRecordNum_fzf = sql_count ( "hv_fzf" ,  "br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('301','303','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1' and otype='2'  ",s_com_branch.br_no,lv_c552.sdate,lv_c552.edate);
			totamt_zf = iRecordNum_zf * AVERATE_PRICE;
			totamt_fzf = iRecordNum_fzf * PRICE_303;
			
			
			vtcp_log("[%s][%d]�ܽ��=[%s],iRecordNum_zf=[%d],iRecordNum_fzf==[%d]\n",__FILE__,__LINE__,lv_c552.totamt,iRecordNum_zf,iRecordNum_fzf);
			fprintf(fp,"%d|%s|%d|%.2f|%d|%.2f|%d|%.2f|%s|\n",iBrnoNum,s_com_branch.br_name,iRecordNum_zf+iRecordNum_fzf,totamt_zf+totamt_fzf,
						iRecordNum_zf,totamt_zf,iRecordNum_fzf,totamt_fzf,"С��");
			
			
		}
		
		
		
		
		set_zd_data( DC_FILE_SND,"1");
		
		fclose(fp);
		
		vtcp_log("[%s][%d]����ȥ��!\n",__FILE__,__LINE__);
		goto OkExit;
		
		
	}
		
	ret=Lv_c552r_Dec_Sel(g_pub_tx.reply,cWherelist);
	if(ret)
	{
		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	while(1)
	{
		ret = Lv_c552r_Fet_Sel(&lv_c552,g_pub_tx.reply);
		if(ret &&ret!=100)
		{
			vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}	
		else if(ret==100 &&c552_num==0)
		{
			vtcp_log("[%s][%d]�Ҳ�����Ӧ�ļ�¼!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		
		
		if(ret==100)	break;
		
		
		if(c552_num==0)
			c552_indate = lv_c552.in_date;
		if(c552_num && (c552_indate==lv_c552.in_date)) /***�������û�б仯˵������ͬһ��552����ļ�¼***/
			continue;
		c552_indate = lv_c552.in_date;
		
		c552_num ++;
		
		
		/*str2dbl(lv_c552.totamt,sizeof(lv_c552.totamt)-1,2,&totamt);*/
		totamt = atof(lv_c552.totamt);
		vtcp_log("[%s][%d]�ܽ��===[%.2f]\n",__FILE__,__LINE__,totamt);
		/***��ʼ�ֶ�δ�lv_c552�� ȡ��¼***/
		/***��ȡ��303�ı���,��301�ı���***/

		NUM_303 = sql_count("hv_fzf"," wt_date>=%d and wt_date<=%d and cmtno in ('303','301','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1'  and otype='2' ",lv_c552.sdate,lv_c552.edate);
		NUM_PKG = sql_count("lv_pkgreg","wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1' ",lv_c552.sdate,lv_c552.edate);
		
		if( (NUM_PKG+NUM_303)!=0) /***�ܵ�С���շѽ���ȥ ��֧����ȡ�Ľ�� /�ܵ�֧������  �õ����� ****/
		{
			AVERATE_PRICE = ( totamt - NUM_303*PRICE_303 ) / NUM_PKG;
			vtcp_log("[%.2f]= [%.2f]-[%d * %d ]/[%d] \n",AVERATE_PRICE,totamt,NUM_303,PRICE_303,NUM_PKG);
		}
		else 
			AVERATE_PRICE = 0.0;
			
			
		vtcp_log("[%s][%d]�ܽ��=[%s],NUM_303=[%d],NUM_PKG==[%d]\n",__FILE__,__LINE__,lv_c552.totamt,NUM_303,NUM_PKG);
		vtcp_log("[%s][%d]֧�����׵ĵ��ʼ۸�===[%.2lf]\n",__FILE__,__LINE__,AVERATE_PRICE);	
		/**1.��ʼ����pkgno����****/
	
		ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply," br_no = '%s' and wt_date>=%d and wt_date<=%d and lv_sts not in('0','1','2','3') and lw_ind='1'  order by pkgno,wt_date,orderno ",br_no,lv_c552.sdate,lv_c552.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			ret = Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )
			{	
				vtcp_log("[%s][%d]���֧����¼�Ѿ�������,��ʼ�ҷ�֧��!\n",__FILE__,__LINE__);
				totamt_zf = iRecordNum_zf * AVERATE_PRICE;
				vtcp_log("[%s][%d]֧�����׵��ܽ��===[%.2lf]\n",__FILE__,__LINE__,totamt_zf);
				iRecordNum_fzf  = 0;
		
				ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"br_no='%s' and wt_date>=%d and wt_date<=%d and cmtno in ('301','303','319','321','323','325','327')  and hv_fzf_sts ='4' and lw_ind='1' and otype='2' order by cmtno,wt_date,orderno ",br_no,lv_c552.sdate,lv_c552.edate);
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
							fprintf( fp,"~CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע||||||\n" );
							head_flag   =1 ;
						}
						fprintf( fp,"%s|%d|%s|%.2f|%s|%d|%s|%.2f|%s|%d|%s|%.2f|\n","�ܱ���:",iRecordNum_zf+iRecordNum_fzf,"�ܽ��:",totamt_zf+totamt_fzf,
								"֧������:",iRecordNum_zf,"֧���ܽ��:",totamt_zf,"��֧������:",iRecordNum_fzf,"��֧���ܽ��:",totamt_fzf);
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
						fprintf( fp,"~CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע||||||\n" );
						head_flag   =1 ;
					}
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
					fprintf( fp,"%s|%s|%s|%d|%.2f|%.2f|%s||||||\n",hv_fzf.cmtno,br_no,hv_fzf.orderno,hv_fzf.wt_date,PRICE_303,PRICE_303,"С��");
					vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
				 }
				 Hv_fzf_Clo_Sel();
				 break;  /***����С��֧����Ӧ��whileѭ��***/
			}
					
					
			vtcp_log("[%s][%d]����֧���ĵ�[%d]�ε�!\n",__FILE__,__LINE__,iRecordNum_zf);
			iRecordNum_zf ++;
			
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
				fprintf( fp,"~CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע||||||\n" );
				head_flag   =1 ;
			}
			
			fprintf( fp,"%s|%s|%s|%d|%.2f|%.2f|%s|\n",lv_pkgreg.pkgno,br_no,lv_pkgreg.orderno,lv_pkgreg.wt_date,AVERATE_PRICE,AVERATE_PRICE,"С��");
			
		}
		Lv_pkgreg_Clo_Sel();
		
		
	}
	Lv_c552r_Clo_Sel();
	
	
	/*if( !total_num )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		set_zd_data( DC_FILE_SND,"1");
		fclose(fp);
		
	}**/
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
