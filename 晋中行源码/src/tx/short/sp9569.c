/***************************************************************
* �� �� ��:     sp9569.c
* ����������    С��ݰ���ѯ��¼��ϸ
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
#include "lv_pkgreg_c.h"
#include "lv_define.h"
#include  "lv_wbctl_c.h"
#include "lv_lbctl_c.h"

char * iNum2TxType(char *ch,char *cTmpList);
char * iNum2Stat(char ch,char *cTmpList);
char * iNum2RcpStat(char *rcpstat,char *cTmpList ,char *pkgno);

sp9569()
{


	vtcp_log("[%s][%d]���ݰ���Ϣ��ѯ��¼��ϸ��Ϣ\n",__FILE__,__LINE__);
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
	HV_FD123_AREA  fd123;
	struct lv_pkgreg_c  lv_pkgreg;
	struct hv_fzf_c hv_fzf;
	struct lv_wbctl_c lv_wbctl;
	struct lv_lbctl_c lv_lbctl;

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
	char    cStat[21];
	char 	cRcp_stat[21];
	int 	iCmtno;
	char    cPack_date[9];
	char    cPack_id  [9];
	char    cPkgno    [4];
	char    cQs_brno  [13];
	int     iPackid = 0;



	memset(&lv_wbctl,	0x00 ,	sizeof(lv_wbctl));
	memset(&lv_lbctl,	0x00 , 	sizeof(lv_lbctl));
	memset(cLw_ind,		0x00 ,	sizeof(cLw_ind));
	memset(cBr_no,		0x00 ,	sizeof(cBr_no ));
	memset(cTx_date,	0x00 ,	sizeof(cTx_date));
	memset(cZf_type,	0x00 ,	sizeof(cZf_type));
	memset(cTx_type,	0x00 ,	sizeof(cTx_type));
	memset(cTx_stat,	0x00 ,	sizeof(cTx_stat ));
	memset(cFilename,	0x00 ,	sizeof(cFilename));
	memset(&fd123,		0x00 ,	sizeof(fd123));
	memset(&lv_pkgreg,	0x00 ,	sizeof(struct lv_pkgreg_c));
	memset(&hv_fzf,		0x00 ,	sizeof(struct hv_fzf_c));
	memset(&hv_notpayin,0x00 ,	sizeof(hv_notpayin));
	memset(cWherelist,	0x00 ,	sizeof(cWherelist));
	memset(cTmplist,	0x00 ,	sizeof(cTmplist));
	memset(cPack_date,	0x00 ,	sizeof(cPack_date));
	memset(cStat,		0x00 ,	sizeof(cStat));
	memset(cRcp_stat,	0x00 ,	sizeof(cRcp_stat));
	memset(cPack_date,	0x00 ,	sizeof(cPack_date));
	memset(cPack_id,	0x00 ,	sizeof(cPack_id));
	memset(cPkgno,		0x00 ,	sizeof(cPkgno));
	memset(cQs_brno,	0x00 ,	sizeof(cQs_brno));

	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0440",cPack_id);     
	get_zd_data("0450",cPack_date);
	get_zd_data("0670",cLw_ind);
	get_zd_data("0520",cQs_brno);
	
	
	
	iPackid = atoi(cPack_id);
	sprintf(cPack_id,"%08d",iPackid);
	
	
	vtcp_log("[%s][%d]cPack_id==[%s] cPack_date==[%s] cLw_ind=[%c]\n",__FILE__,__LINE__,cPack_id,cPack_date,cLw_ind[0]);
	vtcp_log("[%s][%d]cQs_brno==[%s]\n",__FILE__,__LINE__,cQs_brno);
	
	if(cLw_ind[0]=='1')/***����****/
	{
		ret = Lv_wbctl_Sel(g_pub_tx.reply,&lv_wbctl,"packid = '%s' and pack_date = '%s'  ",cPack_id,cPack_date);
		if(ret)
		{
			vtcp_log("[%s][%d]��ѯlv_wbctl����[%s][%s]\n",cPack_id,cPack_date);
			goto ErrExit;
		}
		memcpy(cPkgno,lv_wbctl.pkgno,sizeof(lv_wbctl.pkgno)-1);
		memcpy(cBr_no,lv_wbctl.br_no,sizeof(lv_wbctl.br_no)-1);
		
	}
	else if(cLw_ind[0]=='2')/***����****/
	{
		ret = Lv_lbctl_Sel(g_pub_tx.reply,&lv_lbctl,"packid = '%s' and pack_date = '%s'  and pay_qs_no = '%s' ",cPack_id,cPack_date,cQs_brno);
		if(ret)
		{
			vtcp_log("[%s][%d]��ѯlv_wbctl����[%s][%s]\n",cPack_id,cPack_date);
			goto ErrExit;
		}
		memcpy(cPkgno,lv_lbctl.pkgno,sizeof(lv_lbctl.pkgno)-1);
		memcpy(cBr_no,lv_lbctl.br_no,sizeof(lv_lbctl.br_no)-1);
		
	}
		
	
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
	
	sprintf(cTmplist,"br_no = '%s' and ",cBr_no);
	
	/****������������Ϊ�ղ�ѯ���еļ�¼****/
	if(cLw_ind[0]=='1')
	{
		sprintf(cWherelist,"br_no  like  '%%%s%%' and pack_date='%s'  and  packid = '%s' and pkgno='%s' and lw_ind ='%c'",cBr_no,cPack_date,cPack_id,cPkgno,cLw_ind[0]);
	}
	else if(cLw_ind[0]=='2')
	{
		sprintf(cWherelist," pack_date='%%%s%%'  and  packid = '%s' and pkgno='%s' and lw_ind ='%c'",cPack_date,cPack_id,cPkgno,cLw_ind[0]);
	}
	vtcp_log("[%s][%d]cWherelist==[%s]\n",__FILE__,__LINE__,cWherelist);
	
	ret = Lv_pkgreg_Dec_Sel(g_pub_tx.reply, cWherelist);
	if(ret)
	{
		sprintf(acErrMsg,"��ѯС��֧�����׻�����Ϣ����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	ttlnum=0;
	while(1)
	{
		ret=Lv_pkgreg_Fet_Sel(&lv_pkgreg,g_pub_tx.reply);
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
			strcpy(g_pub_tx.reply,"HV01");
			goto ErrExit;
		}
		/***��lv_pkgreg �е�or_br_no�Ƿ�����һ�����ж�Ӧ�Ļ���***/
		if(!iInSameBrno(lv_pkgreg.or_br_no,cQs_brno))
		{
			break;
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
			fprintf( fp,"~PKG���|��������|������|˳���|��������|���׽��|������|������|�����  |������  |����״̬|��ִ״̬|\n" );
		}
		if(lv_pkgreg.pack_date==0)
			strcpy(cPack_date,"        ");
		else
			sprintf(cPack_date,"%08d",lv_pkgreg.pack_date);

		memset(cTmplist,0x00,sizeof(cTmplist));
		vtcp_log("[%s][%d]cTmplist===[%s]\n",__FILE__,__LINE__,cTmplist);
		vtcp_log("[%s][%d]tx_type==[%s]\n",__FILE__,__LINE__,iNum2TxType(lv_pkgreg.pkgno,cTmplist));
		vtcp_log("[%s][%d]�ڡ�%d����,��������=[%s]\n",__FILE__,__LINE__,ttlnum,iNum2TxType(lv_pkgreg.pkgno,cTmplist));
		fprintf( fp,"%s|%s|%s|%s|%ld|%.2f|%s|%s|%s|%s|%s|%s|\n",lv_pkgreg.pkgno,cTmplist,lv_pkgreg.br_no,lv_pkgreg.orderno,lv_pkgreg.wt_date,lv_pkgreg.tx_amt,lv_pkgreg.or_br_no,lv_pkgreg.ac_br_no,lv_pkgreg.packid,cPack_date,iNum2Stat(lv_pkgreg.lv_sts[0],cStat),iNum2RcpStat(lv_pkgreg.rcpstat,cRcp_stat,lv_pkgreg.pkgno));
		ttlnum++;

	}      
	ret=Lv_pkgreg_Clo_Sel();
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
	





OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"�ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
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

/**����״̬**/
char * iNum2Stat(char ch,char *cTmpList)
{
	vtcp_log("[%s][%d]ch===[%c]\n",__FILE__,__LINE__,ch);
	if(ch=='0')
	{
		strcpy(cTmpList,"��Ч״̬");
	}
	else if(ch=='1')
	{
		strcpy(cTmpList,"����¼��");
	}
	else if(ch=='3')
	{
		strcpy(cTmpList,"���˸���");
	}
	else if(ch=='4')
	{
		strcpy(cTmpList,"���˷���");
	}
	else if(ch=='5')
	{
		strcpy(cTmpList,"���˾ܾ�");
	}
	else if(ch=='6')
	{
		strcpy(cTmpList,"��������");
	}
	else if(ch=='7')
	{
		strcpy(cTmpList,"�����Ŷ�");
	}
	else if(ch=='8')
	{
		strcpy(cTmpList,"���˳����ȴ�");
	}
	else if(ch=='9')
	{
		strcpy(cTmpList,"���˳���");
	}
	else if(ch=='A')
	{
		strcpy(cTmpList,"�˻س���ֹ���ȴ�");
	}
	else if(ch=='B')
	{
		strcpy(cTmpList,"�����˻�");
	}
	else if(ch=='C')
	{
		strcpy(cTmpList,"�����ѳ���");
	}
	else if(ch=='D')
	{
		strcpy(cTmpList,"������ֹ��");
	}
	else if(ch=='G')
	{
		strcpy(cTmpList,"�����쳣����");
	}
	else if(ch=='H')
	{
		strcpy(cTmpList,"��ʱ����,������");
	}
	else if(ch=='I')
	{
		strcpy(cTmpList,"��ʽ����");
	}
	else if(ch=='J')
	{
		strcpy(cTmpList,"ȫ��Ѻ��");
	}
	else if(ch=='K')
	{
		strcpy(cTmpList,"�ط�Ѻ��");
	}
	else if(ch=='L')
	{
		strcpy(cTmpList,"ȫ��Ѻ�͵ط�Ѻ����");
	}
	else if(ch=='M')
	{
		strcpy(cTmpList,"�������Ĺ���");
	}
	else if(ch=='N')
	{
		strcpy(cTmpList,"֧�й���");
	}
	else if(ch=='O')
	{
		strcpy(cTmpList,"�����˻صȴ�");
	}
	else if(ch=='P')
	{
		strcpy(cTmpList,"�����˻�");
	}
	else if(ch=='Q')
	{
		strcpy(cTmpList,"ͬ��δ����");
	}
	else if(ch=='R')
	{
		strcpy(cTmpList,"ͬ���ѷ���");
	}
	else if(ch=='S')
	{
		strcpy(cTmpList,"��ͬ��δ����");
	}
	else if(ch=='T')
	{
		strcpy(cTmpList,"��ͬ���ѷ���");
	}
	else if(ch=='U')
	{
		strcpy(cTmpList,"�����ѳ���");
	}
	else if(ch=='V')
	{
		strcpy(cTmpList,"������ֹ��");
	}
	else if(ch=='W')
	{
		strcpy(cTmpList,"�����Ѹ���");
	}
	else if(ch=='Z')
	{
		strcpy(cTmpList,"�����쳣����");
	}
	else
		strcpy(cTmpList,"δ֪״̬");

	vtcp_log("[%s][%d]cTmpList==[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;

}

/***��ִ״̬*****/
char * iNum2RcpStat(char *rcpstat , char *cTmpList,char *pkgno)
{
	vtcp_log("liuyue[%s][%d]rcpstat==[%s]\n",__FILE__,__LINE__,rcpstat);

	if(!memcmp(pkgno,PKGNO_PTJJ,3) ||!memcmp(pkgno,PKGNO_PTJJHZ,3) ||!memcmp(pkgno,PKGNO_DQJJ,3)||!memcmp(pkgno,PKGNO_DQJJHZ,3)||!memcmp(pkgno,PKGNO_SSJJ,3) ||!memcmp(pkgno,PKGNO_SSJJHZ,3))
	{
			if(!memcmp(rcpstat,"00",2))
			{
				strcpy(cTmpList,"�ɹ�");
			}	
			else if(!memcmp(rcpstat,"01",2))
			{
				strcpy(cTmpList,"�ʺŲ���");
			}	
			else if(!memcmp(rcpstat,"02",2))
			{
				strcpy(cTmpList,"�ʺŻ�������");
			}	
			else if(!memcmp(rcpstat,"03",2))
			{
				strcpy(cTmpList,"�˻�����֧��");
			}	
			else if(!memcmp(rcpstat,"10",2))
			{
				strcpy(cTmpList,"�˻��������");
			}	
			else if(!memcmp(rcpstat,"11",2))
			{
				strcpy(cTmpList,"�ʺ�״̬����");
			}	
			else if(!memcmp(rcpstat,"20",2))
			{
				strcpy(cTmpList,"ҵ���ѳ���");
			}	
			else if(!memcmp(rcpstat,"90",2))
			{
				strcpy(cTmpList,"��������");
			}
			else 
				strcpy(cTmpList,"δ����ִ");	
	}
	else
				strcpy(cTmpList,"       ");

	vtcp_log("[%s][%d]cTmpList===[%s]\n",__FILE__,__LINE__,cTmpList);
	return cTmpList;
}


int iInSameBrno(char *or_br_no,char *qs_brno)
{
    char cOr_br_no[13];
    char cQs_brno[13];
    struct hv_uniontab_c hv_uniontab;
    int ret = 0;

    memset(cOr_br_no,0x00,sizeof(cOr_br_no));
    memset(cQs_brno,0x00,   sizeof(cQs_brno));
    memset(&hv_uniontab,0x00,   sizeof(hv_uniontab));


    memcpy(cOr_br_no,or_br_no,sizeof(cOr_br_no)-1);
    memcpy(cQs_brno, qs_brno,sizeof(cQs_brno)-1);


    ret = Hv_uniontab_Sel(g_pub_tx.reply,&hv_uniontab,"or_br_no ='%.12s' ",cOr_br_no);
    if(ret)
    {
        vtcp_log("[%s][%d]����hvuniontabl����orbrno==[%s]\n",__FILE__,__LINE__,cOr_br_no);
        strcpy(g_pub_tx.reply,"P000");
        return (-1);
    }

    vtcp_log("[%s][%d]qsbrno:table==[%s] other==[%s]\n",__FILE__,__LINE__,hv_uniontab.qs_no,cOr_br_no);

    if(memcmp(cQs_brno,hv_uniontab.qs_no,sizeof(hv_uniontab.qs_no)-1))
    {
        return 0;
    }
    return 1;

}
