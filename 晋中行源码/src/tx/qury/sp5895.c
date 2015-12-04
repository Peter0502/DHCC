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

#define  PRICE_303      5
#define  PRICE_301      5
#define  PRICE_PKG	5

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
	
	
	char  otype[2];			/***����С��ı�ʶ***/
	
	int   QUERY_DATE;		/***��ѯ����***/
	
	
	
	int    head_flag = 0 ;/****�Ƿ�д���ļ�ͷ�ı�ʶ   1 �Ѿ�д���ļ�ͷ   0  û��д******/
	
	int    NUM_303	= 0;	/**ʱ�����303�ı���**/
	int    NUM_301  = 0;    /*ʱ�����301�ı���**/
	int    NUM_PKG  = 0;    /*С���֧������**/
	int    NUM_HV   = 0;	/*����֧��������Ŀ**/
	
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

	
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0910",br_no);
	get_zd_data("0680",otype);
	
	
	QUERY_DATE = apatoi(beg_mon,sizeof(beg_mon));
	QUERY_DATE = QUERY_DATE*100;

	
	
	if(memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(br_no,g_pub_tx.tx_br_no,5))
	{
		vtcp_log("[%s][%d]���������Ĳ��ܲ�ѯ���н���\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	sprintf(cWherelist,"sdate>=%d and edate<=%d  and ",QUERY_DATE+1,QUERY_DATE+31);
	
	
	
	strcat (cWherelist," 1=1 order by paymon ");
	

		
	ret=Hv_chargemx_Dec_Sel(g_pub_tx.reply,cWherelist);
	if(ret)
	{
		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	while(1)
	{
		ret = Hv_chargemx_Fet_Sel(&hv_charge,g_pub_tx.reply);
		if(ret &&ret!=100)
		{
			vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}	
		else if(ret==100 &&iRecordNum==0)
		{
			vtcp_log("[%s][%d]�Ҳ�����Ӧ�ļ�¼!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D014");
			goto ErrExit;
		}
		
		if(ret==100)	break;
		
		/***��ʼ�ֶ�δ�hv_charge�� ȡ��¼***/
		/***��ȡ��303�ı���,��301�ı���***/
		NUM_303 = sql_count("hv_fzf"," wt_date<=%d and wt_date>=%d and cmtno='303' and hv_sts ='1' and lw_ind='1'  and otype='1' ",hv_charge.sdate,hv_charge.edate);
		NUM_301 = 0 ; /*sql_count("hv_fzf","wt_date<=%d and wt_date>=%d and cmtno='301' and hv_sts ='1' and lw_ind='1' ",hv_charge.sdate,hv_charge.edate);*/
		NUM_PKG = 0 ;/*sql_count("hv_zf","wt_date<=%d and wt_date>=%d and lv_sts not in('0','1','2','3') and lw_ind='1' ",hv_charge.sdate,hv_charge.edate);*/
		NUM_HV =  sql_count("hv_zf",     "wt_date<=%d and wt_date>=%d and hv_sts not in('C','1') and lw_ind='1' ",hv_charge.sdate,hv_charge.edate);
		
		if( (NUM_PKG+NUM_HV)!=0)
			AVERATE_PRICE = ( apatoi(hv_charge.txamt)-NUM_303*PRICE_303-NUM_301*PRICE_301 ) /(NUM_PKG+NUM_HV);
		else 
			AVERATE_PRICE = 0.0;
			
		/**1.��ʼ����pkgno����****/
		
		ret = Hv_zf_Dec_Sel(g_pub_tx.reply," br_no = '%s' and wt_date>=%d and wt_date<=%d and hv_sts not in('C','1') and lw_ind='1'  order by cmtno",br_no,hv_charge.sdate,hv_charge.edate);
		if(ret )
		{
			vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"D104");
			goto ErrExit;
		}
		while(1)
		{
			ret = Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
			if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ��������,����ϵ���Ļ���!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret==100 )	break;
			
			iRecordNum ++;
			
			if(iRecordNum==1)	/**��һ��д�ļ�**/
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
				fprintf( fp,"~CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע|\n" );
				head_flag   =1 ;
			}
			fprintf( fp,"%s|%s|%s|%d|%d|%d|\n",hv_zf.cmtno,br_no,hv_zf.orderno,hv_zf.wt_date,AVERATE_PRICE,AVERATE_PRICE,"���");
			
		}
		Hv_zf_Clo_Sel();
		totamt = totamt +  iRecordNum *AVERATE_PRICE;
		
		/*2.**��ʼ�����֧���Ǽ�***/
		iRecordNum = 0;
		
		ret = Hv_fzf_Dec_Sel(g_pub_tx.reply,"br_no='%s' and wt_date<=%d and wt_date>=%d and cmtno in ('303')  and hv_sts ='1' and lw_ind='1' and otype='1' ",br_no,hv_charge.sdate,hv_charge.edate);
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
			else if(ret==100 )	break;
			
			iRecordNum ++;
			
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
				fprintf( fp,"~CMT���|�������|�������|��������|�Ʒѵ���|�Ʒѽ��|��ע|\n" );
				head_flag   =1 ;
			}
			fprintf( fp,"%s|%s|%s|%d|%d|%d|\n",hv_fzf.cmtno,br_no,hv_fzf.orderno,hv_fzf.wt_date,dGet_CMTNO_PRICE(hv_fzf.cmtno),dGet_CMTNO_PRICE(hv_fzf.cmtno),"���");
			
		}
		Hv_fzf_Clo_Sel();
	   	totamt = totamt +  iRecordNum *PRICE_303;
	
	}
	Lv_c552r_Clo_Sel();
	
	
	if( !iRecordNum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		set_zd_data( DC_FILE_SND,"1");
		fclose(fp);
		
	}

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