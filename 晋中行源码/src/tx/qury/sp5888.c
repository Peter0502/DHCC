/***** �������շ��嵥��ѯ(sp5348)��������**********/
/*****************************************************
* �� �� ��:  sp5888.c
* ���������� �շ��嵥��ѯ
* ��    ��:  wangyh
* ������ڣ� 2006��8��30�� 
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
int sp5888()
{
	vtcp_log("��ʼС���շ��嵥��ѯ[%s][%d]",__FILE__,__LINE__);

	struct lv_c552r_c lv_c552;
	struct lv_c554r_c lv_c554;
	LV_FD123_AREA     fd123;
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
	memset(&lv_c552,0x00,sizeof(lv_c552));
	memset(&lv_c554,0x00,sizeof(lv_c554));
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

	
	
	pub_base_sysinit();

	get_zd_data("0440",beg_mon);
	get_zd_data("0450",end_mon);
	get_zd_data("0680",tx_type);
	get_zd_data("0910",br_no);


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
	zip_space(br_no);
	if(br_no[0]!=0x00)
	{
		sprintf(cTmplist," brno = '%s' and ",br_no);
		strcat(cWherelist,cTmplist);
	}
	
	strcat (cWherelist," 1=1 order by paymon ");
	
	if(tx_type[0]=='1')/**c552��¼**/
	{
		
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
			else if(ret==100 &&iRecordNum==0)
			{
				vtcp_log("[%s][%d]�Ҳ�����Ӧ�ļ�¼!\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"D014");
				goto ErrExit;
			}
			
			if(ret==100)	break;
			iRecordNum ++; /*��¼��+1**/
			
			if(iRecordNum==1) /**����ǵ�һ��д�ļ�ͷ**/
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
				fprintf( fp,"~CMT���|�������|�������|��ϸ���|��������|�Ʒ��·�|�ܼƷѽ��|�ܼƷѱ���|֪ͨ����|��ϸ���|�Ʒ�����|�Ʒѵ���|��������|��ϸ���|\n" );
				/****�ڵ�һ��ʱ���һЩ�����ֶθ��Ƶ�fd123����***/
				memcpy(tos88.sendco,lv_c552.sendco,4);
				memcpy(tos88.qs_brno,lv_c552.acbrno,12);
				sprintf(tos88.beg_date,"%8d",lv_c552.sdate);
				sprintf(tos88.end_date,"%8d",lv_c552.edate);
				memcpy(tos88.rate,lv_c552.monrate,sizeof(lv_c552.monrate)-1);
				
			}
			memset(c552_pkgno,	0x00,	sizeof(c552_pkgno));
			memset(c552_iftype,	0x00,	sizeof(c552_pkgno));
			iNum2TxType(lv_c552.pkgno,c552_pkgno);
			if(lv_c552.iftype[0]=='1')	memcpy(c552_iftype,"����֪ͨ",8);
			if(lv_c552.iftype[0]=='2')	memcpy(c552_iftype,"�տ�֪ͨ",8);
			if(lv_c552.iftype[0]=='3')	memcpy(c552_iftype,"�����ִ",8);
			if(lv_c552.iftype[0]=='4')	memcpy(c552_iftype,"�տ��ִ",8);
		   	fprintf( fp,"%s|%s|%d|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",lv_c552.cmtno,lv_c552.brno,lv_c552.dtlid,lv_c552.seqno,lv_c552.in_date,lv_c552.paymon,lv_c552.totamt,lv_c552.num,c552_iftype,c552_pkgno,lv_c552.totnum,lv_c552.price,lv_c552.brate,lv_c552.txamt);
	
		}
		Lv_c552r_Clo_Sel();
	}
	else  if(tx_type[0]=='2')/**c554��¼**/
	{
	  	ret=Lv_c554r_Dec_Sel(g_pub_tx.reply,cWherelist);
	  	if(ret)
	  	{
	  		vtcp_log("[%s][%d]���ݿ����ʧ��!\n",__FILE__,__LINE__);
	  		strcpy(g_pub_tx.reply,"D103");
	  		goto ErrExit;
	  	}
	  	while(1)
	  	{
	  		ret = Lv_c554r_Fet_Sel(&lv_c552,g_pub_tx.reply);
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
          	
	  		iRecordNum ++; /*��¼��+1**/
	  		
	  		if(iRecordNum==1) /**����ǵ�һ��д�ļ�ͷ**/
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
				memcpy(tos88.sendco,lv_c554.sendco,4);
				memcpy(tos88.qs_brno,lv_c554.acbrno,12);
				sprintf(tos88.beg_date,"%8d",lv_c554.sdate);
				sprintf(tos88.end_date,"%8d",lv_c554.edate);
				memcpy(tos88.rate,lv_c554.brate,sizeof(lv_c554.brate)-1);
	  			fprintf( fp,"~CMT���|�������|�������|��ϸ���|��������|�Ʒ��·�|�ܷ��ؽ��|�ܷ��ر���|֪ͨ����|��ϸ���|�Ʒ�����|�Ʒѵ���|��������|��ϸ���|\n" );
	  		}
	  		memset(c552_pkgno,	0x00,	sizeof(c552_pkgno));
	  		memset(c552_iftype,	0x00,	sizeof(c552_pkgno));
	  		iNum2TxType(lv_c554.pkgno,c552_pkgno);
	  		if(lv_c552.iftype[0]=='1')	memcpy(c552_iftype,"����֪ͨ",8);
	  		if(lv_c552.iftype[0]=='2')	memcpy(c552_iftype,"�տ�֪ͨ",8);
	  		if(lv_c552.iftype[0]=='3')	memcpy(c552_iftype,"�����ִ",8);
	  		if(lv_c552.iftype[0]=='4')	memcpy(c552_iftype,"�տ��ִ",8);
	  	   	fprintf( fp,"%s|%s|%d|%d|%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n",lv_c554.cmtno,lv_c554.brno,lv_c554.dtlid,lv_c554.seqno,lv_c554.in_date,lv_c554.paymon,lv_c554.rebamt,lv_c554.rebnum,c552_iftype,c552_pkgno,lv_c554.totnum,lv_c554.price,lv_c554.brate,lv_c554.txamt);
	  	
	  	}
	}
	
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
	memcpy(&fd123,&tos88,sizeof(tos88));
	set_zd_data("1230",(char *)&fd123);	
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



