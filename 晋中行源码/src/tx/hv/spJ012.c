/***************************************************************
* �� �� ��:     spJ012.c
* ����������    ���л�Ʊ��Ϣ��ѯ(ģ����ѯ�;�ȷ��ѯ)			
*
* ��    ��:   LiuYue
* ������ڣ�  2006-8-18	09:20 
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "find_debug.h"

/*****ǩ�����ڷ����ֶ�FD44��,ǩ���в��ô��ͣ�ֻ��Ҫ�����������PAY_OPN_BR_NO��ͬ��ֵ����,Ʊ�ݺ������FD38�ֶ���,��Ʊ״̬�����ֶ� $FD72��****/
	

spJ012()
{
	int ret = 0;
	char  filename[40];
	FILE *fp;
	HV_NOTPAY_AREA hv_notpayin;
	struct hv_poinfo_c hv_poinfo;
	HV_FD123_AREA  fd123;
	char	cWt_day[9];
	char	cPo_no[9];
	char	cPo_sts[2];
	char  cBank_type[2];
	char	cPay_br_no[13];	
	char  cWherelist[200];
	char  cTmplist[200];
	int   num  =0;
	char  cStat[20];
	char  cPoType[20];
	char  cSign_date[9];
	int   iPo_no = 0;
	int   iTotcnt=0;
	double dTotamt=0.0;

	memset(filename,    0x00,   sizeof(filename));
	memset(cWt_day,     0x00,   sizeof(cWt_day));
	memset(cPo_no,      0x00,   sizeof(cPo_no));
	memset(cPo_sts,     0x00,   sizeof(cPo_sts));
	memset(cBank_type,  0x00,   sizeof(cBank_type));
	memset(cPay_br_no,  0x00,   sizeof(cPay_br_no));
	memset(cWherelist,  0x00,   sizeof(cWherelist));
	memset(cTmplist,    0x00,   sizeof(cTmplist));
	memset(cStat,       0x00,   sizeof(cStat));
	memset(cPoType,     0x00,   sizeof(cPoType));
	memset(&fd123,      0x00,   sizeof(fd123));
	memset(cSign_date,	0x00,	sizeof(cSign_date));
	
	iTotcnt=0;
	dTotamt=0.0;
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));

    /******************************
     *PART 1   ��ȡǰ̨��������� *
     ******************************/

	/***�õ����֧��ҵ��8583����***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	/*****��ȡTAGֵ*********/
	get_zd_data("0450",cWt_day);	/**ǩ������***/
	zip_space(cWt_day);
	get_zd_data("0720",cPo_sts);	/**��Ʊ״̬***/
	get_zd_data("0710",cBank_type);	/**�б��־***/
	get_fd_data("1230",(char *)&fd123);/**��ȡ123������**/
	MEMCPY_DEBUG(cPo_no,fd123.po_no,sizeof(cPo_no)-1);

	/********�������Ȳ�����,�ȸ���ֵ����*******/
	if(cBank_type[0]=='0')/****���Ϊ��˵���Ǳ���***/
	{
		vtcp_log("++++++tx_br_no==[%s]++++++",hv_notpayin.F_tx_br_no);
		GetOr_br_noByBr_no(hv_notpayin.F_tx_br_no,cPay_br_no);
	}
	else
	{
		MEMCPY_DEBUG(cPay_br_no,hv_notpayin.F_ac_br_no,sizeof(cPay_br_no)-1);/*ǩ���к�*/
	}
	vtcp_log("++++++++++++++++cPay_br_no==[%s]+++++++++++++",cPay_br_no);
	

	/***************************
	*PART 2   �������ݿ�      *
	***************************/

	/****���where����*********/
	vtcp_log("++++++++cpo_no===[%s]\n",fd123.po_no);
	iPo_no = apatoi(cPo_no,sizeof(cPo_no)-1);
 	vtcp_log("[%s][%d]liuyue====[%d]\n",__FILE__,__LINE__,iPo_no);
	if(iPo_no!=0) /** ��ѯ������,����������Ϊ��****/
	{
		sprintf(cTmplist,"po_no = '%.8s' and   ",fd123.po_no);
		strcat(cWherelist,cTmplist);
	}
	pub_base_strpack(cPay_br_no);
	if(strlen(cPay_br_no)!=0 &&!MEMCPY_DEBUG(g_pub_tx.tx_br_no, QS_BR_NO ,5))
	{
		sprintf(cTmplist, "pay_br_no = '%s' and ",cPay_br_no);
		strcat(cWherelist,cTmplist);
	}

	pub_base_strpack(cWt_day);
	if(strlen(cWt_day)==8)
	{
		vtcp_log("%s,%d ί������===[%s]",__FILE__,__LINE__,cWt_day);
		sprintf(cTmplist, " (reg_date ='%s' or sign_date='%s' )  and ",cWt_day,cWt_day);
		strcat(cWherelist,cTmplist);
	}

	if(cPo_sts[0]!='Z')
	{	
		sprintf(cTmplist, "Po_sts='%c' and  ",cPo_sts[0]);
		strcat(cWherelist,cTmplist);
	}


	sprintf(cTmplist, "bank_type='%c' ",cBank_type[0]);
	strcat(cWherelist,cTmplist);

	sprintf(cTmplist,"order by br_no , sign_date desc ,Po_no desc ");
	strcat(cWherelist,cTmplist);
	
	ret = Hv_poinfo_Dec_Sel(g_pub_tx.reply,cWherelist);	
	if(ret)
	{
		sprintf(acErrMsg,"���һ�Ʊ��Ϣ�������룽[%s] �к�==[%s]ret=[%d]\n",hv_poinfo.po_no,hv_poinfo.pay_br_no);
		WRITEMSG
		return (-1);
	}
	num = 0;
	while(1)
	{
		vtcp_log("++++++++[%d]+++++++\n",__LINE__);
		ret = Hv_poinfo_Fet_Sel(&hv_poinfo,g_pub_tx.reply);
		if(ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"ȡ�α���� ret==[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret ==100)
		{
			if(num!=0)
			{
			fprintf(fp,"%s|%s|%s|%.2f|%s|%s|%s|%d|%s|\n", "�ܼ�","        ","���  ",dTotamt,"","","����",iTotcnt,"");
			num++;	
			}
			break;
		}
		if(!num)
		{
			pub_base_AllDwnFilName( filename);
			fp = fopen(filename,"w");
			if(fp==NULL)
			{
				sprintf(acErrMsg,"open file [%s] failed!\n",filename);
				WRITEMSG
				strcpy(g_pub_tx.reply,"S047");
				goto ErrExit;	
			}
			/****��ʾ����****/
			fprintf(fp,"~@ǩ������  |@ǩ���к�      |@��Ʊ����    |$��Ʊ���        |@��Ʊ����  |@��Ʊ״̬    |@�����ʺ�                      |@��������          |@����|\n" );
		}/*****END if(!num)****/

			/*******״̬ת��******/
			iNum2Stat(hv_poinfo.po_sts[0],cStat);
			/*******ת����Ʊ����*****/
			iNum2PoType(hv_poinfo.po_ind[0],cPoType);
			if(hv_poinfo.sign_date==0)
				MEMCPY_DEBUG(cSign_date,"        ",8);
			else 	
				sprintf(cSign_date,"%08d",hv_poinfo.sign_date);

			vtcp_log("++++++++[%d]+++++++\n",__LINE__);
			fprintf(fp,"%s|%s|%s|%.2f|%s|%s|%s|%s|%s|\n", cSign_date,hv_poinfo.pay_br_no,hv_poinfo.po_no,hv_poinfo.po_bal,cPoType,cStat,hv_poinfo.pay_ac_no,hv_poinfo.pay_name,hv_poinfo.brf);
			num++;	
			iTotcnt++;
			dTotamt+=hv_poinfo.po_bal;
		}/***end while (1)*******/
			ret = Hv_poinfo_Clo_Sel();
			if(ret)
			{
				sprintf(acErrMsg,"sql error\n");
				WRITEMSG
				goto ErrExit;	
			}
			if(!num)
			{
				strcpy(g_pub_tx.reply,"S049");
				goto ErrExit;
			}
			else
			{
				fclose(fp);
				set_zd_data(DC_FILE_SND,"1");
			}

    /**********************
     *PART 4 ��β����     *
     **********************/
	 
	/* д��ҵ����ˮ */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
		WRITEMSG
		goto ErrExit;
	}
	else 
		goto OkExit;
	/*** ������� ***/

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





int iNum2Stat(char ch,char *cTmpList)
{
		if(ch=='0')
		{
			strcpy(cTmpList,"�Ǽ�");
		}
		else if(ch=='1')
		{
			strcpy(cTmpList,"ǩ��");
		}
	 	else if(ch=='2')
		{
			strcpy(cTmpList,"�ʽ��ƴ����");
		}
 	    else if(ch=='3')
		{
			strcpy(cTmpList,"����Ҹ�");
		}
		else if(ch=='4')
		{
			strcpy(cTmpList,"��Ʊ�Ҹ�");
		}
		else if(ch=='5')
		{
			strcpy(cTmpList,"ȫ�����");
		}
		else if(ch=='6')
		{
			strcpy(cTmpList,"�����˻�");
		}
		else if(ch=='7')
		{
			strcpy(cTmpList,"δ���˻�");
		}
		else if(ch=='8')
		{
			strcpy(cTmpList,"��Ʊ�⸶");
		}
		else if(ch=='9')
		{
			strcpy(cTmpList,"��Ʊǩ��ȡ��");
		}
		else if(ch=='A')
		{
			strcpy(cTmpList,"�⸶ȡ��");
		}
		else if(ch=='B')
		{
			strcpy(cTmpList,"����");
		}
		else if(ch=='C')
		{
			strcpy(cTmpList,"����Ҹ�ȡ��");
		}
		else if(ch=='D')
		{
			strcpy(cTmpList,"��Ʊ�Ǽ�ȡ��");
		}
		else if(ch=='E')
		{
		strcpy(cTmpList,"��ʧ");
		}
		else if(ch=='F')
		{
			strcpy(cTmpList,"���ʧ");
		}
		else if(ch=='G')
		{
			strcpy(cTmpList,"����δ���˻�");
		}
		else if(ch=='H')
		{
			strcpy(cTmpList,"δ�ý⸶");
		}
		else if(ch=='I')
		{
			strcpy(cTmpList,"δ���˻س���");
		}
		else if(ch=='J')
		{
			strcpy(cTmpList,"��Ʊ����ע��");
		}
		else if(ch=='K')
		{
			strcpy(cTmpList,"�����˻�");
		}
		else
			strcpy(cTmpList,"δ֪״̬");
		return 0;
	}
int iNum2PoType(char ch,char *cTmpList)
{
		if(ch=='0')
		{	
			strcpy(cTmpList,"��ת�û�Ʊ");
		}
		else if(ch=='1')
		{	
			strcpy(cTmpList,"����ת�û�Ʊ");
		}
	    	else if(ch=='2')
		{	
			strcpy(cTmpList,"�ֽ��Ʊ");
		}
		else
			strcpy(cTmpList,"δ֪����");


		return (0);
}
	
