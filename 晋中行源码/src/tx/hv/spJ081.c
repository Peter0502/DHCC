/***************************************************************
* �� �� ��:     spJ081.c
* ����������    �����ʷ��Ϣ��ѯ
*
* �޸ļ�¼��    �������еĶ�������
* ��    ��:     20060921
* �� �� ��:     liuyue
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"


char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);

static char cCmtno[4];       /*** 20110107 ���鸴 ***/

spJ081()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
	HV_FD123_AREA  fd123;
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;

	char    cLw_ind[2];	  /*������־ 1 ��  2 ��*/
	char    cBr_no[6];        /*������*/
	char    cTx_date[9];      /*��������*/
	char    cZf_type[2];      /*֧����־ 0 ֧����  1 ��֧���� */
	char    cTx_type[4];      /*֧����������*/
	char    cTx_stat[2];       /*����״̬*/
	int     ttlnum = 0;       /*��ȡ��������*/
	FILE    *fp;
	char    cFilename[100];    /*�ļ���*/
	char    cWherelist[2000];
	char    cTmplist[200];
	int  	iTx_date = 0;
	int	iBr_no	 = 0;
	double Tamt=0.00;
	char    cStat_Desc[20];
	char 	cTx_Desc[20];

	memset(cCmtno , 0 , sizeof(cCmtno));                     /*** 20110107 ���鸴 ***/
	memset(cLw_ind,		0x00 ,	sizeof(cLw_ind));
	memset(cBr_no,		0x00 ,	sizeof(cBr_no ));
	memset(cTx_date,	0x00 ,	sizeof(cTx_date));
	memset(cZf_type,	0x00 ,	sizeof(cZf_type));
	memset(cTx_type,	0x00 ,	sizeof(cTx_type));
	memset(cTx_stat,	0x00 ,	sizeof(cTx_stat ));
	memset(cFilename,	0x00 ,	sizeof(cFilename));
	memset(&fd123,		0x00 ,	sizeof(fd123));
	memset(&hv_zf,		0x00 ,	sizeof(struct hv_zf_c));
	memset(&hv_fzf,		0x00 ,	sizeof(struct hv_fzf_c));
	memset(&hv_notpayin,  	0x00 ,	sizeof(hv_notpayin));
	memset(cWherelist,	0x00 ,	sizeof(cWherelist));
	memset(cTmplist,	0x00 ,	sizeof(cTmplist));

	memset(cStat_Desc,0x00,sizeof(cStat_Desc));
	memset(cTx_Desc,0x00,	sizeof(cTx_Desc));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0670",cLw_ind);     
	get_zd_data("0280",cBr_no);
	get_zd_data("0440",cTx_date);
	get_zd_data("0680",cZf_type);
	get_zd_data("0230",cTx_type);
	get_zd_data("0690",cTx_stat);

	vtcp_log("[%s][%d]cBr_no==[%s]\n",__FILE__,__LINE__,cBr_no);

	vtcp_log("[%s][%d]��ʼ���Ĳ�ѯ����!\n",__FILE__,__LINE__);
	/*********��ȡ������**************/
	pub_base_AllDwnFilName(cFilename);
	fp = fopen(cFilename,"rb");
	if(fp==NULL)
	{
		sprintf(acErrMsg,"���ļ�[%s]����!\n",cFilename);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
	}
	fclose(fp);


	/********�������Ŀ��Բ�ѯ���еĽ���*****/

	/**��ɲ�ѯ����**/
	if(memcmp(g_pub_tx.tx_br_no,HV_CZ_QSBRNO,5))  /****���������������**/
	{
		zip_space(cBr_no);
		iBr_no	= atoi(cBr_no);
		if(iBr_no==0)
		{
			vtcp_log("[%s][%d]����������,�����뱾������!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"����������,�����뱾������!");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;	
		}

		if(memcmp(g_pub_tx.tx_br_no,cBr_no,sizeof(cBr_no)-1))
		{
			vtcp_log("[%s][%d]���������Ĳ������ѯ����ҵ��!\n",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"���������Ĳ��ܲ�ѯ���е�ҵ��!");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
		strcat(cWherelist,cTmplist);
	}
	else
	{
		zip_space(cBr_no);
		iBr_no = atoi(cBr_no);
		if(iBr_no!=0&&strlen(cBr_no)==BRNO_LEN)
		{
			sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
			strcat(cWherelist,cTmplist);
		}
	}
	/****������������Ϊ�ղ�ѯ���еļ�¼****/

	zip_space(cTx_date);
	iTx_date = atoi(cTx_date);

	if(iTx_date!=0 )
	{
		sprintf(cTmplist," wt_date= '%s' and ",cTx_date,cTx_date);
		strcat(cWherelist,cTmplist);
	}

	if(memcmp(cTx_type,"888", sizeof(cTx_type)-1) &&cZf_type[0]=='0')/*****���ѡ��Ĳ���ȫ�� �����Ǵ���֧��***/
	{
		sprintf(cTmplist,"cmtno = '%s' and ",cTx_type);
		strcat(cWherelist,cTmplist);
	}
	if(memcmp(cTx_type,"888", sizeof(cTx_type)-1) &&cZf_type[0]=='1')/*****���ѡ��Ĳ���ȫ�� �����Ǵ��ķ�֧��***/
	{ 
		if(apatoi(cTx_type,sizeof(cTx_type)-1)%2!=0)/**����֧��������ҵ��**/
				sprintf(cTmplist,"cmtno = '%s' and ",cTx_type);
		else  if(apatoi(cTx_type,sizeof(cTx_type)-1)%2==0)/**����֧����Ӧ��ҵ��**/
				sprintf(cTmplist,"(cmtno = '%s' or (cmtno = '%d' and res_orderno is not null) )  and ",cTx_type,apatoi(cTx_type,sizeof(cTx_type)-1)-1);
	
		strcat(cWherelist,cTmplist);
	}
	if(cTx_stat[0]!='Z'&&cZf_type[0]=='1')/***��֧���� ����״̬����ѡ��ȫ��**/
	{
		sprintf(cTmplist,"hv_fzf_sts ='%c'  and  ",cTx_stat[0]);
		strcat(cWherelist,cTmplist);
	}
	else if(cTx_stat[0]!='Z'&&cZf_type[0]=='0') /**֧����*/
	{
		sprintf(cTmplist,"hv_sts ='%c' and ",cTx_stat[0]);
		strcat(cWherelist,cTmplist);
	}
	if(cZf_type[0]=='1')
	{
		sprintf(cTmplist,"otype = '%c' and ",'1');
		strcat(cWherelist,cTmplist);
	}
	/***********����Ҫ������lw_ind��Ϊ��ʶ�� ,����pay_qs_no��Ϊ��ʶ***/
	if(cZf_type[0]=='1')/*�Ƿ�֧��**/
	{
		vtcp_log("[%s][%d]lw_ind=[%c]\n",__FILE__,__LINE__,cLw_ind[0]);
		if(apatoi(cTx_type,sizeof(cTx_type)-1)==301 || apatoi(cTx_type,sizeof(cTx_type)-1)==311 || apatoi(cTx_type,sizeof(cTx_type)-1)==313) /**����֧��������ҵ��**/
		{
			if(cLw_ind[0]=='1')/*wangzhang ***/
				sprintf(cWherelist+strlen(cWherelist)," pay_qs_no='%s' and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			else if(cLw_ind[0]=='2')/*laizhang***/
				sprintf(cWherelist+strlen(cWherelist),"( pay_qs_no<>'%s' or pay_qs_no is null ) and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);

		}
		else if(apatoi(cTx_type,sizeof(cTx_type)-1)==302||apatoi(cTx_type,sizeof(cTx_type)-1)==312 || apatoi(cTx_type,sizeof(cTx_type)-1)==314)  /**���֧����Ӧ����ҵ��****/
		{
			if(cLw_ind[0]=='1')/*wangzhang ***/
				sprintf(cWherelist+strlen(cWherelist)," (  pay_qs_no <>'%s' or pay_qs_no is null ) and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			else if(cLw_ind[0]=='2')/*laizhang***/
				sprintf(cWherelist+strlen(cWherelist),"  pay_qs_no = '%s'  and 1=1 order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			
		}
		else
		{
			if(cLw_ind[0]=='1')/*wangzhang ***/
				sprintf(cWherelist+strlen(cWherelist)," pay_qs_no='%s' and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
			else if(cLw_ind[0]=='2')/*laizhang***/
				sprintf(cWherelist+strlen(cWherelist),"( pay_qs_no<>'%s' or pay_qs_no is null ) and 1=1  order by br_no,wt_date,cmtno",HV_QS_BR_NO);
		}
	}
	else  if(cZf_type[0]=='0')/*��֧��**/
	{
		sprintf(cTmplist," lw_ind ='%c' order by br_no,wt_date,cmtno ",cLw_ind[0]);
		strcat(cWherelist,cTmplist);
	}


	vtcp_log("[%s][%d]cWherelist=[%s]\n",__FILE__,__LINE__,cWherelist);

	vtcp_log("[%s][%d]֧������===[%c]\n",__FILE__,__LINE__,cZf_type[0]);
	/**��ʼ��ѯ**/
	/***֧��������Ĳ�ѯ***/
	if(cZf_type[0] == '0')
	{
		vtcp_log("[%s][%d]��ѯ֧���ཻ��!\n",__FILE__,__LINE__);
		ret = Hv_zf_Dec_Sel(g_pub_tx.reply, cWherelist);
		if(ret)
		{
			sprintf(acErrMsg,"��ѯ���֧�����׻�����Ϣ����[%d]",ret);
			WRITEMSG
			set_zd_data(DC_GET_MSG,"��ѯ���֧�����׻�����Ϣ����");
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		Tamt=0.00;
		ttlnum=0;
		while(1)
		{
			ret=Hv_zf_Fet_Sel(&hv_zf,g_pub_tx.reply);
			if( ret==100 &&ttlnum>0 )
			{
				break;
			}
			else if(ret==100 &&ttlnum==0)
			{
				vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"�Ҳ�����Ӧ�ļ�¼!\n");
				strcpy(g_pub_tx.reply,"D104");
				goto ErrExit;
			}
			else if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"���ݿ�����쳣,����Ƽ�����ϵ!\n");
				strcpy(g_pub_tx.reply,"D103");
				goto ErrExit;
			}
			if( !ttlnum )
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
				fprintf( fp,"~CMT���|��������|������|˳���|��������|���׽��|������|������|״̬|\n" );
			}
			memset(cCmtno , 0 , sizeof(cCmtno));                    /*** 20110107 ���鸴 ***/
			memcpy(cCmtno , hv_fzf.cmtno, sizeof(cCmtno)-1);        /*** 20110107 ���鸴 ***/
			memset(cTmplist,0x00,sizeof(cTmplist));
			iNum2TxType(hv_zf.cmtno,cTx_Desc);
			vtcp_log("[%s][%d]�ڡ�%d����,��������=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(hv_zf.cmtno,cTx_Desc));
			fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|\n",hv_zf.cmtno,cTx_Desc,hv_zf.br_no,hv_zf.orderno,hv_zf.wt_date,hv_zf.tx_amt,hv_zf.or_br_no,hv_zf.ac_br_no,iNum2Stat(hv_zf.hv_sts[0],cStat_Desc));
			ttlnum++;
			Tamt += hv_zf.tx_amt;
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));

		}
		fprintf( fp,"����:|%d��|����ܼ�:|%.2f|\n",ttlnum,Tamt);      
		ret=Hv_zf_Clo_Sel();
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
	}

	else if(cZf_type[0]=='1') /***��֧���ཻ�ײ�ѯ***/
	{
		ret = Hv_fzf_Dec_Sel(g_pub_tx.reply, cWherelist);
		if(ret)
		{
			sprintf(acErrMsg,"��ѯ����֧�����׻�����Ϣ����[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		ttlnum=0;
		while(1)
		{
			ret=Hv_fzf_Fet_Sel(&hv_fzf,g_pub_tx.reply);
			if( ret==100 &&ttlnum>0 )
			{
				break;
			}
			else if(ret==100 &&ttlnum==0)
			{
				vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼!\n",__FILE__,__LINE__);
				goto ErrExit;
			}
			else if(ret && ret!=100)
			{
				vtcp_log("[%s][%d]���ݿ�����쳣,����Ƽ�����ϵ!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"���ݿ�����쳣,����Ƽ�����ϵ!\n");
				goto ErrExit;
			}
			if( !ttlnum )
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
				fprintf( fp,"~CMT���|��������|������|˳���|��������|���׽��|��  ��  ��|��  ��  ��|״     ̬|\n");
			}
			memset(cCmtno , 0 , sizeof(cCmtno));        /*** 20110107 ���鸴***/
			memcpy(cCmtno , hv_fzf.cmtno, sizeof(cCmtno)-1);     /*** 20110107 ���鸴***/
vtcp_log("[%s][%d]***cCmtno==[%s]",__FILE__,__LINE__,cCmtno);
			iNum2TxType(hv_fzf.cmtno,cTx_Desc);
			vtcp_log("[%s][%d]�ڡ�%d����,��������=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(hv_fzf.cmtno,cTmplist));
			fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|\n",hv_fzf.cmtno,cTx_Desc,hv_fzf.br_no,hv_fzf.orderno,hv_fzf.wt_date,hv_fzf.o_tx_amt,hv_fzf.or_br_no,hv_fzf.ac_br_no,iNum2Stat(hv_fzf.hv_fzf_sts[0],cStat_Desc));
			ttlnum++;
			memset(cTx_Desc,0x00,sizeof(cTx_Desc));

		}
		ret=Hv_fzf_Clo_Sel();
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

	}



OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"ʧ��![%s]",g_pub_tx.reply);
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

	if(!memcmp(ch,"100",3))
	{           
		strcpy(cName,"�����");
	} 
	else if(!memcmp(ch,"101",3))
	{
		strcpy(cName,"ί���տ�");
	}
	else if(!memcmp(ch,"105",3))
	{
		strcpy(cName,"ͬҵ���");
	}
	else if(!memcmp(ch,"103",3))
	{
		strcpy(cName,"�����ʽ���ǻ���");
	}
	else if(!memcmp(ch,"102",3))
	{
		strcpy(cName,"���ճи�");
	}
	else if(!memcmp(ch,"109",3))
	{
		strcpy(cName,"��������");
	}
	else if(!memcmp(ch,"108",3))
	{
		strcpy(cName,"֧���˻�");
	}
	else if(!memcmp(ch,"313",3))
	{
		strcpy(cName,"�����˻�");
	}
	else if(!memcmp(ch,"314",3))
	{
		strcpy(cName,"�˻�Ӧ��");
	}
	else if(!memcmp(ch,"301",3))
	{
		strcpy(cName,"����ѯ");
	}
	else if(!memcmp(ch,"302",3))
	{
		strcpy(cName,"���鸴");
	}
	else if(!memcmp(ch,"303",3))
	{
		strcpy(cName,"���ɸ�ʽ");
	}
	else if(!memcmp(ch,"311",3))
	{
		strcpy(cName,"���볷��");
	}
	else if(!memcmp(ch,"407",3))
	{
		strcpy(cName,"��Ѻ����");
	}
	else if(!memcmp(ch,"408",3))
	{
		strcpy(cName,"��Ѻ���ʿۿ�");
	}
	else if(!memcmp(ch,"121",3))
	{
		strcpy(cName,"�ʽ��ƴ�");
	}
	else if(!memcmp(ch,"721",3))
	{
		strcpy(cName,"�����ʽ�����");
	}
	else if(!memcmp(ch,"122",3))
	{
		strcpy(cName,"�����ʽ�����");
	}
	else if(!memcmp(ch,"123",3))
	{
		strcpy(cName,"�����ʽ��˻�");
	}
	else if(!memcmp(ch,"725",3))
	{
		strcpy(cName,"ȫ��Ҹ�֪ͨ");
	}
	else if(!memcmp(ch,"724",3))
	{
		strcpy(cName,"��Ʊ�����˻�");
	}
	else if(!memcmp(ch,"124",3))
	{
		strcpy(cName,"��Ʊδ���˻�");
	}
	else
		strcpy(cName,"δ֪��������");



	MEMCPY_DEBUG(cTmpList,cName,strlen(cName));
	vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmpList);
	return  cName;
}

/**����״̬**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if(ch=='0')
	{
		strcpy(cTmpList,"���˸���");
	}
	else if(ch=='1')
	{
		strcpy(cTmpList,"���˷���");
	}
	else  if(ch=='2')
	{
	vtcp_log("[%s][%d]cCmtno==***===[%s]\n",__FILE__,__LINE__,cCmtno);
		if(memcmp(cCmtno,"301",3)==0)                /*** 20110107 ���鸴 ***/
		{
			strcpy(cTmpList,"��ѯ�Ѳ鸴");       /*** 20110107 ���鸴 ***/
		}
		else
		{
			strcpy(cTmpList,"���˼�������");
		}
	}
	else if(ch=='3')
	{
		strcpy(cTmpList,"�����˻ؼ���");
	}
	else if(ch=='4')
	{
		strcpy(cTmpList,"�����˻صȴ�");
	}
	else if(ch=='5')
	{
		strcpy(cTmpList,"���˾ܾ�");
	}
	else if(ch=='6')
	{
		strcpy(cTmpList,"���˳�������");
	}
	else if(ch=='7')
	{
		strcpy(cTmpList,"���˳����ȴ�");
	}
	else if(ch=='8')
	{
		strcpy(cTmpList,"���˾ܾ�");
	}
	else if(ch=='9')
	{
		if(cCmtno[0]=='1')                                 /*** 20110107 ���鸴 ***/
		{
			strcpy(cTmpList,"���˼�������");
		}
		else
		{
			if(memcmp(cCmtno,"301",3)==0)              /*** 20110107 ���鸴 ***/
			{
				strcpy(cTmpList, "��ѯ���鸴");
			}else if(memcmp(cCmtno,"313",3)==0)
			{
				 strcpy(cTmpList, "�˻ش�Ӧ��");
			}
			else
			{
				strcpy(cTmpList, "������������");
			}
		}	
	}
	else if(ch=='A')
	{
		strcpy(cTmpList,"���˵Ǽ�ȡ��");
	}
	else if(ch=='B')
	if(memcmp(cCmtno,"301",3)==0 || memcmp(cCmtno,"302",3)==0 )              /*** 20110107 ���鸴 ***/
	{
		strcpy(cTmpList, "��ѯ�Ѳ鸴");
	}
	else if(memcmp(cCmtno,"313",3)==0 || memcmp(cCmtno,"314",3)==0 )              /*** 20110107 ���鸴 ***/
	{
		strcpy(cTmpList, "�˻���Ӧ��");
	}
	else
	{
		strcpy(cTmpList,"�����˻�");
	}
	else if(ch=='C')
	{
		strcpy(cTmpList,"���ʵǼ�(������)");
	}
	else if(ch=='D')
	{
		strcpy(cTmpList,"���˸���ȡ��");
	}
	else if(ch=='E')
	{
		strcpy(cTmpList,"���˼��˵Ǽ�");
	}
	else if(ch=='F')
	{
		strcpy(cTmpList,"���˺˵ط�Ѻ��");
	}
	else if(ch=='G')
	{
		strcpy(cTmpList,"���˺�ȫ��Ѻ��");
	}
	else if(ch=='H')
	{
		strcpy(cTmpList,"�������Ĺ���");
	}
	else if(ch=='I')
	{
		strcpy(cTmpList,"֧�й���");
	}
	else if(ch=='J')
	{
		strcpy(cTmpList,"�����Ŷ�");
	}
	else if(ch=='K')
	{
		strcpy(cTmpList,"�˶��������");
	}
	else
		strcpy(cTmpList,"δ֪״̬");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}

