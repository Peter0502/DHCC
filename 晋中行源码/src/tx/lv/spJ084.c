/***************************************************************
* 文 件 名:     spJ084.c
* 功能描述：    行名行号查询
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18	09:20 
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
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
		* 1 初始化全局变量 *
		********************/

		pub_base_sysinit();


		/*******************
		* 2.获取前台数据  *
		*******************/

		get_fd_data("0810",cArea_81);
		get_fd_data("0820",cBr_name);
		get_fd_data("0820",cBr_name_f);
		get_zd_data("0220",cBrno_type);

		/********************
		* 3.执行基本的检查 *
		********************/


		zip_space(cArea_81);
		zip_space(cBr_name_f);

		vtcp_log("[%s][%d]cArea_81===[%s],cBr_name==[%s],cBrno_type==[%s]\n",__FILE__,__LINE__,cArea_81,cBr_name,cBrno_type);

		memcpy(cOr_br_no,cArea_81,sizeof(cOr_br_no)-1);

		zip_space(cOr_br_no);

		if(strlen(cOr_br_no)==0 &&strlen(cBr_name_f)==0)
		{
				vtcp_log("[%s][%d]不能两个条件都为空!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"两个条件不能都为空!");
				strcpy(g_pub_tx.reply,"HV01");
				goto ErrExit;
		}
		if(strlen(cOr_br_no)!=12 &&cOr_br_no[0]!=0x00)
		{
				vtcp_log("[%s][%d]交易行输入错误!\n",__FILE__,__LINE__);
				set_zd_data(DC_GET_MSG,"交易行输入错误!");
				strcpy(g_pub_tx.reply,"HV01");
				goto ErrExit;
		}

		/*******************
		* 4.数据库操作    *
		*******************/

		if(cOr_br_no[0]!=0x00 &&strlen(cOr_br_no)==12)
		{
				sprintf(cTmplist,"or_br_no ='%.12s' and ",cOr_br_no);
				strcat(cWherelist,cTmplist);
		}

		/***行号类别********/	
		if(memcmp(cBrno_type,"09",sizeof(cBrno_type)-1))
		{
				sprintf(cTmplist,"br_categ = '%.2s' and ",cBrno_type);
				strcat(cWherelist,cTmplist);
		}

		/******行号********/	
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
			vtcp_log("[%s][%d]操作数据库失败!\n",__FILE__,__LINE__);
			goto ErrExit;
		}
		while(1)
		{
				ret = Hv_uniontab_Fet_Sel(&hv_uniontab,g_pub_tx.reply);
				if(ret != 0 && ret != 100)
				{
						vtcp_log("[%s][%d]取游标错误!\n",__FILE__,__LINE__);
						strcpy(g_pub_tx.reply,"D103");
						goto ErrExit;
				}
				else if(num==0&&ret==100)
				{
						vtcp_log("[%s][%d]没有找到对应的记录!\n",__FILE__,__LINE__);
						set_zd_data(DC_GET_MSG,"没有找到相关记录!");
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
				/****显示标题****/
				fprintf(fp,"~@支付机构号 |@行别名称|@清算行号  |@发报中心 |@行别状态|@截止日期|@行号类型     |@行别地址|@邮编|@电话|\n" );
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
		*PART 4 结尾工作     *
		**********************/

		/* 写入业务流水 
		ret=pub_ins_trace_log();
		if(ret)
		{
				sprintf(acErrMsg,"登记交易流水出错!");
				WRITEMSG
				goto ErrExit;
		}
		else 
				goto OkExit;

		*/

OkExit:
		strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 0;
ErrExit:
		sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
}





char * iNum2Stat(char ch)
{
		vtcp_log("行别状态====[%c]\n",ch);
		char cBank_sts[10];
		memset(cBank_sts,	0x00,	sizeof(cBank_sts));

		if(ch=='0')
		{
				strcpy(cBank_sts,"生效前");
		}
		else if(ch=='1')
		{
				strcpy(cBank_sts,"有效");
		}
		else if(ch=='2')
		{
				strcpy(cBank_sts,"注销");
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
				strcpy(cBank_type,"直接参与人行");
		}
		if(!memcmp(banktype,"02",2))
		{	
				strcpy(cBank_type,"直接参与国库");
		}
		if(!memcmp(banktype,"03",2))
		{	
				strcpy(cBank_type,"EIS转换中心");
		}
		if(!memcmp(banktype,"04",2))
		{	
				strcpy(cBank_type,"直接参与商业银行");
		}
		if(!memcmp(banktype,"05",2))
		{	
				strcpy(cBank_type,"开户特许直接参与者");
		}
		if(!memcmp(banktype,"06",2))
		{	
				strcpy(cBank_type,"开户特许间接参与者");
		}
		if(!memcmp(banktype,"07",2))
		{	
				strcpy(cBank_type,"间接参与者");
		}
		if(!memcmp(banktype,"08",2))
		{	
				strcpy(cBank_type,"未开户特许直接参与者");
		}


		return cBank_type;
}

