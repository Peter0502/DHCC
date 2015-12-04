/*************************************************
* 文 件 名:  spJ402.c
* 功能描述： 未拼包交易查询 
*
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "lv_define.h"
#include "public.h"
#include "lv_wbctl_c.h"                                         
#include "lv_lbctl_c.h" 
#include "hv_uniontab_c.h"


struct {
	char wtdate[8];
	char brno[5];
}lv_unpacked;

typedef struct{
	char  brno[5];
	char  min[8];
	char  max[8];
	char  count[4];
	
}LV_TOS;
	

int spJ402()
{

	vtcp_log("[%s][%d]开始小额未拼包交易查询!\n",__FILE__,__LINE__);
	LV_TOS  lv_tos;
	
	
	char cWherelist		[1000];
	char cSelBuf		[100];/**选择的字段***/
	char cTmplist		[100];
	char cFilebuf		[100];
	
	
	int  ret = 0;
	char cFilename [40];
	FILE *fp;
	int  iRecordNum = 0;
	
	memset(cWherelist,	0x00,	sizeof(cWherelist));
	memset(cTmplist,	0x00,	sizeof(cTmplist));
	memset(cSelBuf,		0x00,	sizeof(cSelBuf));
	memset(&lv_tos,		0x00,	sizeof(lv_tos));
	memset(cFilename,	0x00,	sizeof(cFilename));
	memset(cFilebuf,	0x00,	sizeof(cFilebuf));
	
	
	get_zd_data("0440",lv_unpacked.wtdate);
	get_zd_data("0910",lv_unpacked.brno);
	
	
	
	
	vtcp_log("[%s][%d]从前台获取数据lv_unpacked==[%s]\n",__FILE__,__LINE__,(char *) &lv_unpacked);
	
	
	
	
	if( memcmp(g_pub_tx.tx_br_no,LV_CZ_QSBRNO,5) && memcmp(g_pub_tx.tx_br_no,lv_unpacked.brno,5))
	{
		vtcp_log("[%s][%d]非清算中心不允许做他行交易!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P075");
		goto ErrExit;
	}
	
	
	/***拼凑需要选取的字段****/
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	sprintf(cSelBuf," min(%s), max(%s), count(*), br_no ","orderno","orderno");
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	
	/***行号***/	
	if(lv_unpacked.brno[0]!=0x00)
	{
		sprintf( cTmplist,"br_no='%s' and ",lv_unpacked.brno);
		strcat(cWherelist,cTmplist);
	}
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	/**日期***/
	sprintf(cTmplist,"wt_date=%.8s and packid is null  and  (lv_sts='3' or (pkgno in('002','006') and lv_sts='1')) and ",lv_unpacked.wtdate);
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	strcat(cWherelist,cTmplist);
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	strcat(cWherelist,"1=1  group by br_no");
	
	
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	
	vtcp_log("[%s][%d]cWherelist===[%s]\n",__FILE__,__LINE__,cWherelist);
	vtcp_log("[%s][%d]cSelBuf======[%s]\n",__FILE__,__LINE__,cSelBuf);
	
	

	/**组成查询**/
	vtcp_log("查询条件 wherelist =[%s]",cWherelist);
	ret=Diff_items_Dec_Sel( g_pub_tx.reply,"lv_pkgreg",cSelBuf,cWherelist);
	if(ret)
	{
		vtcp_log("[%s][%d]执行函数Diff_items_Dec_Sel错误!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	iRecordNum=0;
	while(1)
	{
		vtcp_log("[%s][%d]iRecordNum=[%d]",__FILE__,__LINE__,iRecordNum);
		ret = Diff_items_Fet_Sel((char *)&lv_tos,g_pub_tx.reply);
		vtcp_log("[%s][%d]iRecordNum=[%d]",__FILE__,__LINE__,iRecordNum);
		vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
		vtcp_log("[%s][%d]ret===【%d】\n",__FILE__,__LINE__,ret);
		if(ret && ret!=100)
		{
			vtcp_log("[%s][%d]执行函数Diff_items_Fet_Sel!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P078");
			goto ErrExit;
		}
		if(ret==100 &&iRecordNum==0)
		{
			vtcp_log("[%s][%d]没有记录!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"HV01");
			set_zd_data("0130","没有未拼包交易");
			goto ErrExit;
		}
		else if(ret ==100 && iRecordNum!=0)
		{
			break;
		}
	
		vtcp_log("[%s][%d]iRecordnum====[%d]\n",__FILE__,__LINE__,iRecordNum);
		vtcp_log("[%s][%d]第【%d】笔交易lv_tos===[%s]\n",__FILE__,__LINE__,iRecordNum,(char *)&lv_tos);
		
		if(!iRecordNum)
		{
			vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
			pub_base_AllDwnFilName(cFilename);
			vtcp_log("[%s][%d]文件名:【%s】\n",__FILE__,__LINE__,cFilename);
			fp = fopen(cFilename,"w");
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",cFilename);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}          
			fprintf( fp,"~交易机构|最小序号 |最大序号 |总笔数|交易日期|\n");	
		}
		vtcp_log("[%s][%d]\n",__FILE__,__LINE__); 
		 vtcp_log("[%s][%d]开始第【%d】次写文件!\n",__FILE__,__LINE__,iRecordNum);
		 fprintf(fp,"%.5s|%.8s|%.8s|%.4s|%.8s|\n",lv_tos.brno,lv_tos.min,lv_tos.max,lv_tos.count,lv_unpacked.wtdate);
		 vtcp_log("[%s][%d]第【%d】次写文件结束!\n",__FILE__,__LINE__,iRecordNum);
		 memset(&lv_tos,0x00,sizeof(lv_tos));
		
		vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
		iRecordNum++;
	
	}
	fread(cFilebuf,40,iRecordNum,fp);
	vtcp_log("[%s][%d]cFilebuf===[%s]\n",__FILE__,__LINE__,cFilebuf);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	ret=Diff_items_Clo_Sel();
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if(ret)
	{
		vtcp_log("[%s][%d]关闭失败!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if(!iRecordNum)
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

