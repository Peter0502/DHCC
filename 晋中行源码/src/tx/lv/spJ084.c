/***************************************************************
* �� �� ��:     spJ084.c
* ����������    �����кŲ�ѯ
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
#include "hvpack.h"
#include "hv_define.h"
#define EXTERN
#include "hv_uniontab_c.h"

char * iNum2Stat(char ch);
char * iNum2BankType(char *banktype);
spJ084()
{
		int ret = 0;
		int index = 0;
		int i = 0;
		int num = 0;

		char  filename[40];
		FILE *fp;
		struct hv_uniontab_c hv_uniontab;

		char cOr_br_no[13];
		char cBr_name [61];
		char cBrno_type[3];
		char cArea_81  [61];
		char cTmplist  [100];
		char cSub_name [50];
		char cWherelist[1000];
		
		char cBr_name_f[61];



		memset(filename,	0x00,	sizeof(filename));
		memset(&hv_uniontab,0x00,	sizeof(hv_uniontab));

		memset(cOr_br_no,	0x00,	sizeof(cOr_br_no));
		memset(cBr_name,	0x00,	sizeof(cBr_name));
		memset(cBrno_type,	0x00,	sizeof(cBrno_type));
		memset(cArea_81,	0x00,	sizeof(cArea_81));
		memset(cTmplist,	0x00,	sizeof(cTmplist));
		memset(cWherelist,	0x00,	sizeof(cWherelist));
		memset(cSub_name,	0x00,	sizeof(cSub_name));
		memset(cBr_name_f,	0x00,	sizeof(cBr_name_f));





		/********************
		* 1 ��ʼ��ȫ�ֱ��� *
		********************/

		pub_base_sysinit();


		/*******************
		* 2.��ȡǰ̨����  *
		*******************/

		get_fd_data("0810",cArea_81);
		get_fd_data("0820",cBr_name);
		get_fd_data("0820",cBr_name_f);
		get_zd_data("0220",cBrno_type);

		/********************
		* 3.ִ�л����ļ�� *
		********************/


		zip_space(cArea_81);
		zip_space(cBr_name_f);

		vtcp_log("[%s][%d]cArea_81===[%s],cBr_name==[%s],cBrno_type==[%s]\n",__FILE__,__LINE__,cArea_81,cBr_name,cBrno_type);

		memcpy(cOr_br_no,cArea_81,sizeof(cOr_br_no)-1);

		zip_space(cOr_br_no);

		if(strlen(cOr_br_no)==0 &&strlen(cBr_name_f)==0)
		{
				vtcp_log("[%s][%d]��������������Ϊ��!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"�����������ܶ�Ϊ��!");
				strcpy(g_pub_tx.reply,"HV01");
				goto ErrExit;
		}
		if(strlen(cOr_br_no)!=12 &&cOr_br_no[0]!=0x00)
		{
				vtcp_log("[%s][%d]�������������!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"�������������!");
				strcpy(g_pub_tx.reply,"HV01");
				goto ErrExit;
		}

		/*******************
		* 4.���ݿ����    *
		*******************/

		if(cOr_br_no[0]!=0x00 &&strlen(cOr_br_no)==12)
		{
				sprintf(cTmplist,"or_br_no ='%.12s' and ",cOr_br_no);
				strcat(cWherelist,cTmplist);
		}

		/***�к����********/	
		if(memcmp(cBrno_type,"09",sizeof(cBrno_type)-1))
		{
				sprintf(cTmplist,"br_categ = '%.2s' and ",cBrno_type);
				strcat(cWherelist,cTmplist);
		}

		/******�к�********/	
		vtcp_log("[%s][%d]cBr_name===[%s]\n",__FILE__,__LINE__,cBr_name);
		while(cBr_name[index]!=0x00)
		{
				if(cBr_name[index]!=' ' && cBr_name[index]!='\t')
				{
						cSub_name[i] = cBr_name[index];
						index ++;
						i++;
						if(cBr_name[index]==' ' ||cBr_name[index]=='\t' ||cBr_name==0x00)
						{
								sprintf(cTmplist,"br_name_f  like '%%%%%s%%%%'   and ",cSub_name);
								i = 0;
								strcat(cWherelist,cTmplist);
								memset(cTmplist,	0x00,	sizeof(cTmplist));
								memset(cSub_name,	0x00,	sizeof(cSub_name));
								vtcp_log("[%s][%d]cWherelist=[%s]\n",__FILE__,__LINE__,cWherelist);
						}

				}
				else if(cBr_name[index]==' ' ||cBr_name[index]=='\t')
				{
						index ++;	
				}

		}
		strcat(cWherelist,"1=1 order by br_name_f,  qs_no");

		vtcp_log("[%s][%d]cWherelist==[%s]\n",__FILE__,__LINE__,cWherelist);	
		ret =  Hv_uniontab_Dec_Sel ( g_pub_tx.reply , cWherelist);
		if(ret)
		{
			vtcp_log("[%s][%d]�������ݿ�ʧ��!\n",__FILE__,__LINE__);
			goto ErrExit;
		}
		while(1)
		{
				ret = Hv_uniontab_Fet_Sel(&hv_uniontab,g_pub_tx.reply);
				if(ret != 0 && ret != 100)
				{
						vtcp_log("[%s][%d]ȡ�α����!\n",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
				}
				else if(num==0&&ret==100)
				{
						vtcp_log("[%s][%d]û���ҵ���Ӧ�ļ�¼!\n",__FILE__,__LINE__);
						set_zd_data(DC_GET_MSG,"û���ҵ���ؼ�¼!");
						strcpy(g_pub_tx.reply,"HV01");
						goto ErrExit;
						
				}
				else if(num!=0&&ret ==100)
				{
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
				fprintf(fp,"~@֧�������� |@�б�����|@�����к�  |@�������� |@�б�״̬|@��ֹ����|@�к�����     |@�б��ַ|@�ʱ�|@�绰|\n" );
				}
				memset(cTmplist,0x00,sizeof(cTmplist));
				memcpy(	cTmplist, iNum2Stat(hv_uniontab.br_sts[0]),20);
				vtcp_log("[%s][%d]cTmplist==[%s]\n",__FILE__,__LINE__,cTmplist);
				fprintf(fp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", hv_uniontab.or_br_no,hv_uniontab.br_name_f,hv_uniontab.qs_no,hv_uniontab.sendco,cTmplist,hv_uniontab.end_date,iNum2BankType(hv_uniontab.br_categ), hv_uniontab.addr,hv_uniontab.postcode,hv_uniontab.phone);		
				num++;	
		}
		ret = Hv_uniontab_Clo_Sel();
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

		/* д��ҵ����ˮ 
		ret=pub_ins_trace_log();
		if(ret)
		{
				sprintf(acErrMsg,"�Ǽǽ�����ˮ����!");
				WRITEMSG
				goto ErrExit;
		}
		else 
				goto OkExit;

		*/

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





char * iNum2Stat(char ch)
{
		vtcp_log("�б�״̬====[%c]\n",ch);
		char cBank_sts[10];
		memset(cBank_sts,	0x00,	sizeof(cBank_sts));

		if(ch=='0')
		{
				strcpy(cBank_sts,"��Чǰ");
		}
		else if(ch=='1')
		{
				strcpy(cBank_sts,"��Ч");
		}
		else if(ch=='2')
		{
				strcpy(cBank_sts,"ע��");
		}
		vtcp_log("[%s][%d]cbank_sts=[%s]\n",__FILE__,__LINE__,cBank_sts);
		return cBank_sts;
}

		char * iNum2BankType(char *banktype)
		{
		char cBank_type [30];
		memset(cBank_type,	0x00,	sizeof(cBank_type));


		if(!memcmp(banktype,"01",2))
		{	
				strcpy(cBank_type,"ֱ�Ӳ�������");
		}
		if(!memcmp(banktype,"02",2))
		{	
				strcpy(cBank_type,"ֱ�Ӳ������");
		}
		if(!memcmp(banktype,"03",2))
		{	
				strcpy(cBank_type,"EISת������");
		}
		if(!memcmp(banktype,"04",2))
		{	
				strcpy(cBank_type,"ֱ�Ӳ�����ҵ����");
		}
		if(!memcmp(banktype,"05",2))
		{	
				strcpy(cBank_type,"��������ֱ�Ӳ�����");
		}
		if(!memcmp(banktype,"06",2))
		{	
				strcpy(cBank_type,"���������Ӳ�����");
		}
		if(!memcmp(banktype,"07",2))
		{	
				strcpy(cBank_type,"��Ӳ�����");
		}
		if(!memcmp(banktype,"08",2))
		{	
				strcpy(cBank_type,"δ��������ֱ�Ӳ�����");
		}


		return cBank_type;
}

