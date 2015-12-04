#include "card.h"
#include <fcntl.h>


/***********************************************************
* 写入制卡文件
*  参数描述:
*  input  |char * cardno    :卡号
*         |char * effectday :有效日期(年月) 4位
*         |char * servercode:服务代号  3位
*  -------|----------------------------------------------
*  output |char * filename
*  ------------------------------------------------------
*  return | 0 正常  ！=0 失败
*  作者 :LiuHuafeng 20070126
***********************************************************/
int pubf_card_write_file(const char * cardno,const char* effectday,const char * servercode,int filecode,char * filename,const char * txday)
{
	int  iFd=0,iSize=0;
	extern int errno;
	char cFilename[513];
	char cBasename[30];
	char cMktimes[3];
	char cSource[27];
	char cCvn[4];
	char cLine1[500];
	char cEffday [5];
	char cSvrcode[4];

	memset(cEffday , 0 , sizeof(cEffday ));
	memset(cSvrcode, 0 , sizeof(cSvrcode));
	memset(cLine1, 0, sizeof(cLine1));
	memset(cCvn, 0 , sizeof(cCvn));
	memset(cSource, 0 , sizeof(cSource));
	memset(cMktimes, 0 , sizeof(cMktimes));
	memset(cFilename, 0 ,sizeof(cFilename));
	memset(cBasename, 0 ,sizeof(cBasename));
	memcpy(cEffday ,effectday , sizeof(cEffday )-1);
	memcpy(cSvrcode,servercode , sizeof(cSvrcode)-1);
	memcpy(cBasename,"mk",2);
	memcpy(cBasename+2,txday,8);
	sprintf(cMktimes,"%02d",filecode);
	strncat(cBasename,cMktimes,2);
	strncat(cBasename,".txt",4);
	vtcp_log("%s,%d basename=[%s]",__FILE__,__LINE__,cBasename);
	sprintf(cFilename,"%s/cardmkfile/%s",(char *)getenv("HOME"),cBasename);
	vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
	iFd=open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
	if (iFd==-1)
	{
		vtcp_log("[%s][%d]文件描述符号==[%d] \n",__FILE__,__LINE__,iFd);
		vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
		vtcp_log("[%s][%d] 打开准备写入的文件[%s]错误 \n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	/* 
    第一字段：6位固定数字: 622465
    第二字段：81开头的10位序号加一位校验位共11位
    第三字段：从卡号第9位开始的7位
    第四字段：CBCZ四个字符
    第五字段：二磁道信息
    第六字段：三磁道信息
	*/
	{
		char cFix1    [7];
		char cSerial  [14];
		char c11add8  [9];
		char cMsr2    [38];
		char cMsr3    [108];
		memset(cFix1    , 0 , sizeof(cFix1    ));
		memset(cSerial  , 0 , sizeof(cSerial  ));
		memset(c11add8  , 0 , sizeof(c11add8  ));
		memset(cMsr2    , 0 , sizeof(cMsr2    ));
		memset(cMsr3    , 0 , sizeof(cMsr3    ));
		memcpy(cFix1    ,cardno,sizeof(cFix1)-1);
		memcpy(cSerial  ,cardno+sizeof(cFix1)-1,sizeof(cSerial)-1);
		memcpy(c11add8  ,cardno+10,8);
		vPrepareMsr(cardno,cMsr2,cMsr3);
		sprintf(cLine1,"%s %s %s CBKF %s %s",cFix1,cSerial,c11add8,cMsr2,cMsr3);
		cLine1[strlen(cLine1)]='\n';
	}
	iSize=write(iFd,cLine1,strlen(cLine1));
	if (iSize<=0)
	{
		vtcp_log("数据写入失败 errno=[%d] ,size=[%d] \n",errno,iSize);
		return -1;
	}
	if (close(iFd))
	{
		vtcp_log("[%s][%d] close id error \n",__FILE__,__LINE__);
		return -1;
	}
	strncpy(filename,cBasename,60);
	return 0;
}

int pubf_card_loop_create_cardno(const char * start_no,const char * end_no,long int txday,char * filename,char **errmsg)
{
	char cBegno[20];
	char cEndno[20];
	char cNewCardno[20];
	long int  iCardcnt=0,i;
	long int  iStartSerial=0;
	int iRet=0;
	char cTxday[9];
	int iFilecode=0;
	char cCheckflag;
	char cErrmsg[512];
	memset(cErrmsg, 0 , sizeof(cErrmsg));
	*errmsg=cErrmsg;
	memset(cTxday, 0 , sizeof(cTxday));
	memset(cBegno, 0 , sizeof(cBegno));
	memset(cEndno, 0 , sizeof(cEndno));
	sprintf(cTxday,"%08ld", txday);
	vtcp_log("%s,%d startno=[%s] endno=[%s],iCardcnt=[%ld]",__FILE__,__LINE__,start_no,end_no,iCardcnt);
	strncpy(cBegno,start_no,sizeof(cBegno)-1);
	strncpy(cEndno,end_no,sizeof(cEndno)-1);
	iCardcnt=atol(cEndno+8)-atol(cBegno+8);
	vtcp_log("%s,%d startno=[%s] endno=[%s],iCardcnt=[%ld]",__FILE__,__LINE__,cBegno,cEndno,iCardcnt);
	iStartSerial=atol(cBegno+8);
	iFilecode=sql_count("card_book","tx_date=%s",cTxday);
	vtcp_log("%s,%d iFilecode===[%d]\n",__FILE__,__LINE__,iFilecode);
	for(i=0;i<=iCardcnt;i++)
	{
		memset(cNewCardno, 0 , sizeof(cNewCardno));
		sprintf(cNewCardno,"%s%s%08ld",CARDHEADCODE,CARD_UNION_AREA,iStartSerial);
		cCheckflag=iCheckCardno(cNewCardno);
		cNewCardno[strlen(cNewCardno)]=cCheckflag;
		vtcp_log("%s,%d 产生新的卡号==[%s]\n",__FILE__,__LINE__,cNewCardno);
		iRet=pubf_card_write_file(cNewCardno,CVN_ENDDATE,CVN_SERVCODE,iFilecode,filename,cTxday);
		if(iRet)
		{
			vtcp_log("[%s][%d]产生制卡文件错误\n",__FILE__,__LINE__);
			strncpy(cErrmsg,"产生制卡文件错误",40);
			return -1;
		}
		iStartSerial++;
	}
	return 0;
}
