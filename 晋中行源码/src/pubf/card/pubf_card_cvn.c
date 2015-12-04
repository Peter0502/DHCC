#include "card.h"
#include <fcntl.h>
#include "keys_c.h"
#include "public.h"

void vPrepareMsr(const char * cardno,char * msr2,char * msr3);
char *getcvn(char *pan,const char *cvn_enddate,char * cvn_servercode);
int asc2bit(char *source,char *target,int length);
int iRevKey(char *pcKey1,char *pcKey2);

int iRevKey(char *pcKey1,char *pcKey2){
	char pcBuf1[64];
	char pcBuf2[64];
	int  id;
	memset(pcBuf1,'\0',sizeof(pcBuf1));
	memset(pcBuf2,'\0',sizeof(pcBuf2));
	asc2bit(pcKey1,pcBuf1,16);
	for(id=0;id<64;id++){
		pcBuf2[id]=pcBuf1[64-id-1];	
	}
	bit2asc(pcBuf2,pcKey2,64);
	return(0);
}

void dhc_setkey(char *ckey);
void dhc_encrypt(char *block,int ENCRYPTION_FLAG);
int asc2bit(char *source,char *target,int length)
{
	int i,j;
	for (i=0;i<length;i++)
	{
		int ch;
		ch=source[i];
		if (ch>='0' && ch<='9')
			ch=ch-'0';
		else if (ch>='A' && ch <='F')
			ch=ch-'A'+10;
		else
			ch=0;
		for (j=0;j<4;j++)
		{
			if ((ch &(0x8>>j)))
				target[i*4+j]='1';
			else
				target[i*4+j]='0';

		}
	}
	return(0);
}
int bit2asc(char *source,char *target,int length)
{
	int i,j;
	for (i=0;i<length/4;i++)
	{
		int ch;
		ch=0;
		for (j=0;j<4;j++)
		{
			ch+=(source[i*4+j]-'0')<<(3-j);
		}
		if (ch>=10)
			target[i]=(ch-10)+'A';
		else
			target[i]=ch+'0';
	}
	return(0);
}
int countcvn(char *source,char *keyA,char *keyB,char *cvn)
{
	char source_str[33];
	char source_bit[129];
	char tmpbuf[129];
	char target_str[17];
	char keyA_bit[65];
	char keyB_bit[65];
	int i,j;
	memset(source_str,'\0',sizeof(source_str));
	memset(source_bit,'\0',sizeof(source_bit));
	memset(tmpbuf,'\0',sizeof(tmpbuf));
	memset(target_str,'\0',sizeof(target_str));
	memset(keyA_bit,'\0',sizeof(keyA_bit));
	memset(keyB_bit,'\0',sizeof(keyB_bit));
	strncpy(source_str,source,sizeof(source_str)-1);
	asc2bit(source_str,source_bit,sizeof(source_str)-1);
	/*
	vtcp_log("%s,%d:\n%s\n",__FILE__,__LINE__,source_bit);
	*/
	asc2bit(keyA,keyA_bit,16);
	asc2bit(keyB,keyB_bit,16);
	dhc_setkey(keyA_bit);
	dhc_encrypt(source_bit,0);/**ʹ��KeyA��Block1���м���**/
	bit2asc(source_bit,tmpbuf,64);
	/*
	vtcp_log("%s,%d:\n%s\n",__FILE__,__LINE__,tmpbuf);
	*/
	for (i=0;i<64;i++)
	{
		if (source_bit[i]!=source_bit[i+64])
			source_bit[i+64]='1';
		else
			source_bit[i+64]='0';
	}/****��Block1�ļ��ܽ����Block2�������****/
	bit2asc(source_bit+64,tmpbuf,64);
	/*
	vtcp_log("%s,%d:\n%s\n",__FILE__,__LINE__,tmpbuf);
	*/
	dhc_encrypt(source_bit+64,0);/***ʹ��KeyA����������м���**/
	bit2asc(source_bit+64,tmpbuf,64);
	/*
	vtcp_log("%s,%d:\n%s\n",__FILE__,__LINE__,tmpbuf);
	*/
	dhc_setkey(keyB_bit);
	dhc_encrypt(source_bit+64,1);/**ʹ��KeyB�Լ��ܽ�����н���**/
	bit2asc(source_bit+64,tmpbuf,64);
	/*
	vtcp_log("%s,%d:\n%s\n",__FILE__,__LINE__,tmpbuf);
	*/
	dhc_setkey(keyA_bit);
	dhc_encrypt(source_bit+64,0);/**ʹ��KeyA�Խ��ܽ�����м���**/
	bit2asc(source_bit+64,target_str,64);
	/*
	vtcp_log("%s,%d:\n%s\n",__FILE__,__LINE__,target_str);
	*/
	memset(cvn,'\0',3);
	j=0;
	for (i=0;i<16 && j<3;i++)
	{
		if (target_str[i]>='0' && target_str[i]<='9')
			cvn[j++]=target_str[i];
	}
	for (i=0;i<16 && j<3;i++)
	{
		if (target_str[i]>='A' && target_str[i]<='F')
			cvn[j++]=target_str[i]-'A'+'0';
	}
	return(0);
}
char *getcvn(char *pan,const char *cvn_enddate,char * cvn_servercode){
	/****�����ʺŵó�CVN****/
	static char  cvn[4];
	char block[27];
	char cCvnEnddate [5];
	char cCvnServCode[4];
	char pcKeyA[17];
	char pcKeyB[17];
	char cReply[5];
	
	memset(cReply, 0 , sizeof(cReply));
	memset(cvn,'\0',sizeof(cvn));
	memset(block,'\0',sizeof(block));
	memset(pcKeyA,'\0',sizeof(pcKeyA));
	memset(pcKeyB,'\0',sizeof(pcKeyB));
	if(iGetWorkKey(KEY_TYPE_CVNA,pcKeyA,cReply)<0){
		memcpy(pcKeyA,CARD_CVN_KEYA,sizeof(pcKeyA)-1);
	}
	if(iGetWorkKey(KEY_TYPE_CVNB,pcKeyB,cReply)<0){
		memcpy(pcKeyB,CARD_CVN_KEYB,sizeof(pcKeyB)-1);
	}
	memset(cCvnEnddate , 0 , sizeof(cCvnEnddate ));
	memset(cCvnServCode, 0 , sizeof(cCvnServCode));
	memcpy(cCvnEnddate ,cvn_enddate,sizeof(cCvnEnddate )-1);
	memcpy(cCvnServCode,cvn_servercode,sizeof(cCvnServCode)-1);
	
	memcpy(block,pan,19);
	memcpy(block+19,cCvnEnddate,4);
	memcpy(block+23,cCvnServCode,3);
	countcvn(block,pcKeyA,pcKeyB,cvn);
	return(cvn);
}	

/****************************************************
*׼������2��3�ŵ�
****************************************************/
/*void 	vPrepareMsr(const char * cardno,char * msr2,char * msr3)*/
void  vPrepareMsr( char * cardno,char * msr2,char * msr3)
{
	struct str_cardmsr2_area str_msr2;
	struct str_cardmsr3_area str_msr3;
	char cCardno[CARD_NO_LEN+1];
	char cMsr2[CARD_MSR2_LEN+1];
	char cMsr3[CARD_MSR3_LEN+1];
	char cCvn[4];
	char cCustno[12];
	memset(cCustno, 0  , sizeof(cCustno));
	memset(cMsr2, 0 , sizeof(cMsr2));
	memset(cMsr3, 0 , sizeof(cMsr3));
	memset(cCvn, 0 , sizeof(cCvn));
	memset(cCardno, 0 ,sizeof(cCardno));

	memset(&str_msr2, 0 , sizeof(str_msr2));
	memset(&str_msr3, 0 , sizeof(str_msr3));
	memcpy(cCardno,cardno,sizeof(cCardno)-1);
	zip_space(cCardno);
	/*����ڶ��ŵ� */
	strncpy(str_msr2.cardno,cCardno,sizeof(str_msr2.cardno)-1);
	str_msr2.filler1[0]='=';
	memcpy(str_msr2.endday,CVN_ENDDATE,4);
	memcpy(str_msr2.servercode,CVN_SERVCODE,3);
	strncpy(cCvn,getcvn(cardno,str_msr2.endday,str_msr2.servercode),sizeof(cCvn)-1);
	sprintf(str_msr2.userdef,"1%s%s",CARD_AREA,cCvn);
	/* ��������ŵ� */
	memcpy(str_msr3.format     ,"99",sizeof(str_msr3.format     )-1);  /*[3] ;  3-1| 99 */
	memcpy(str_msr3.cardno     ,cardno,sizeof(str_msr3.cardno   )-1);  /*[18]; 21-2| */
	memcpy(str_msr3.filer1     ,"=",sizeof(str_msr3.filer1     )-1);   /*[2] ; 23-3| = */
	memcpy(str_msr3.cuntrycode ,"156",sizeof(str_msr3.cuntrycode )-1); /*[4] ; 27-4| ���Ҵ���156 */
	memcpy(str_msr3.courcode   ,"156",sizeof(str_msr3.courcode   )-1); /*[4] ; 31-5| ���Ҵ���156 */
	memcpy(str_msr3.exponent   ,"0",sizeof(str_msr3.exponent   )-1);   /*[2] ; 33-6| ����ָ�� */
	memcpy(str_msr3.cycauthamt ,"5000",sizeof(str_msr3.cycauthamt)-1); /*[5] ; 38-7| ������Ȩ��5000 */
	memcpy(str_msr3.cycavbal   ,"5000",sizeof(str_msr3.cycavbal  )-1); /*[5] ; 43-8| ���������5000 */
	memcpy(str_msr3.cycbegin   ,"2000",sizeof(str_msr3.cycbegin  )-1); /*[5] ; 48-9| ������ʾ 2000 */
	memcpy(str_msr3.cyclength  ,"01",sizeof(str_msr3.cyclength  )-1);  /*[3] ; 51-10| ���ڳ��� 01 */
	memcpy(str_msr3.pswdtime   ,"3",sizeof(str_msr3.pswdtime   )-1);   /*[2] ; 53-11| ����������� 3*/
	memcpy(str_msr3.authprivy ,"000000",6);                            /*[7];  60-12| */
	memcpy(str_msr3.changectl  ,"1",sizeof(str_msr3.changectl  )-1);   /*[2] ;   62-13| �������Ʒ� 1 */
	memcpy(str_msr3.pan_ta_sr  ,"20",sizeof(str_msr3.pan_ta_sr  )-1);  /*[3] ;   65-14| PAN��TA��SR 20 */
	memcpy(str_msr3.san1_ta_sr ,"00",sizeof(str_msr3.san1_ta_sr )-1);  /*[3] ;   68-15| SAN-1��TA��SR 00*/
	memcpy(str_msr3.san2_ta_sr ,"00",sizeof(str_msr3.san2_ta_sr )-1);  /*[3] ;   71-16| SAN-1��TA��SR 00*/
	memcpy(str_msr3.expirdate  ,CVN_ENDDATE,sizeof(str_msr3.expirdate)-1);/*[5] ;   76-17| ʵЧ����4912 */
	memcpy(str_msr3.cardseqno  ,"1",sizeof(str_msr3.cardseqno  )-1); /*[2] ;   78-18| ����� 1 */
	memcpy(str_msr3.secrecysign,"=",sizeof(str_msr3.secrecysign)-1); /*[2] ;   80-19| �����ܺ� = */
	strncpy(cCustno,cardno+8,sizeof(cCustno)-1);
	sprintf(cCustno,"%011.0f",atof(cCustno));
	memcpy(str_msr3.san1       ,cCustno,sizeof(str_msr3.san1)-1);   /*[12];   92-20| san_1 cardserno ��11λ */
	memcpy(str_msr3.filler2    ,"=",sizeof(str_msr3.filler2  )-1);   /*[2] ;   94-21|�ֶηָ���� = */
	memcpy(str_msr3.san2       ,cardno+8,9);                        /*[10];  104-22| san_2 cardserno ��9λ */
	memcpy(str_msr3.filler3    ,"=",sizeof(str_msr3.filler3  )-1);   /*[2] ;  106-23|�ֶηָ���� = */
	memcpy(str_msr3.carryflag  ,"1",sizeof(str_msr3.carryflag)-1);   /*[2] ;  108-24| ���ͱ�־ 1*/
	memcpy(str_msr3.descnt     ,"000000",sizeof(str_msr3.descnt)-1); /*[7] ;  115-25| ����У���� 000000*/
	
	sprintf(str_msr3.filler4,"1%s1",CARD_AREA);/* ���ε� */
	/*** rem by LiuHuafeng  2007-1-24 9:26
	memcpy(str_msr3.filler4    ,str_msr2.userdef,sizeof(str_msr3.filler4)-1);
	[10];125-26| �������� ������(1)+������(0355)+���ŵ����ڱ�־(1)+cvn
	*****************************************************/

	sprintf(cMsr2,"%s%s%s%s%s",str_msr2.cardno,str_msr2.filler1,str_msr2.endday,
		str_msr2.servercode,str_msr2.userdef);/*37*/
	sprintf(cMsr3,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
		str_msr3.format   ,str_msr3.cardno   ,str_msr3.filer1    ,str_msr3.cuntrycode,
		str_msr3.courcode ,str_msr3.exponent ,str_msr3.cycauthamt,str_msr3.cycavbal  ,
		str_msr3.cycbegin ,str_msr3.cyclength,str_msr3.pswdtime  ,str_msr3.authprivy ,
		str_msr3.changectl,str_msr3.pan_ta_sr,str_msr3.san1_ta_sr,str_msr3.san2_ta_sr,
		str_msr3.expirdate,str_msr3.cardseqno,str_msr3.secrecysign,str_msr3.san1      ,
		str_msr3.filler2  ,str_msr3.san2     ,str_msr3.filler3   ,str_msr3.carryflag ,
		str_msr3.descnt   ,str_msr3.filler4);
	vtcp_log("[%s][%d] msr2=[%s]\n",__FILE__,__LINE__,cMsr2);
	vtcp_log("[%s][%d] msr3=[%s]\n",__FILE__,__LINE__,cMsr3);
	memcpy(msr2,cMsr2,sizeof(cMsr2)-1);
	memcpy(msr3,cMsr3,sizeof(cMsr3)-1);
	return;
}
/***********************************************************
* д���ƿ��ļ�
*  ��������:
*  input  |char * cardno    :����
*         |char * effectday :��Ч����(����) 4λ
*         |char * servercode:�������  3λ
*  -------|----------------------------------------------
*  output |char * filename
*  ------------------------------------------------------
*  return | 0 ����  ��=0 ʧ��
***********************************************************/
/*int iInsertMakeCardFile(const char * cardno,const char* effectday,const char * servercode,int filecode,char * filename,char * txday)*/
int iInsertMakeCardFile( char * cardno,const char* effectday,const char * servercode,int filecode,char * filename,char 
* txday)
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

	sprintf(cFilename,"%s/cardmkfile/%s",(char *)getenv("Appl"),cBasename);
	vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
	iFd=open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
	if (iFd==-1)
	{
		vtcp_log("[%s][%d]�ļ���������==[%d] \n",__FILE__,__LINE__,iFd);
		vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
		vtcp_log("[%s][%d] ��׼��д����ļ�[%s]���� \n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	/*
    ��һ�ֶΣ�6λ�̶�����: 622421
    �ڶ��ֶΣ�71��ͷ��12λ��ż�һλУ��λ��13λ
    �����ֶΣ��ӿ��ŵ�11λ��ʼ��8λ
    �����ֶΣ�CBKF�ĸ��ַ�
    �����ֶΣ����ŵ���Ϣ
    �����ֶΣ����ŵ���Ϣ
	*/
	{
		char cFix1    [7];
		char cSerial  [14];
		char c11add8  [9];
		char cMsr2    [CARD_MSR2_LEN+1];
		char cMsr3    [CARD_MSR3_LEN+1];
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
		vtcp_log("����д��ʧ�� errno=[%d] ,size=[%d] \n",errno,iSize);
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


/****ȡ�ù�����Կ****/
int iGetWorkKey(int nKeyType,char *pcKeyValue,char * reply){
	char *pcProtKey[17];
	char pcBit[64];
	char sql_keyvalue[17];
	struct keys_c sKeys;
	int iRc;
	memset(&sKeys, 0 , sizeof(sKeys));
	if(nKeyType==0){
		printf("%s,%d,����ͨ���˺���ȡ�ñ�����Կ!\n",__FILE__,__LINE__);
		strcpy(acErrMsg,"����ͨ���˺���ȡ�ñ�����Կ");
		return(-1);
	}
	memset(pcProtKey,'\0',sizeof(pcProtKey));
	memset(pcBit,'\0',sizeof(pcBit));
	memset(sql_keyvalue,'\0',sizeof(sql_keyvalue));
	if(iGetProtKey(pcProtKey)<0){
		printf("%s,%d,�õ�������Կʧ��!\n",__FILE__,__LINE__);	
		return(-1);
	}
	/* rem by LiuHuafeng 2007-1-20 0:55 
	EXEC SQL SELECT keyvalue into :sql_keyvalue from keys where keytype=:nKeyType;
	if(sqlca.sqlcode){
		printf("%s,%d,ȡ�ù�����Կʧ��[%d]!\n",__FILE__,__LINE__,sqlca.sqlcode);
		strcpy(acErrMsg,"ȡ�ù�����Կʧ��!");
		return(-1);
	}
	**** replace by next LiuHuafeng 2007-1-20 0:56 */
	/* begin add by LiuHuafeng 2007-1-20 0:55 	**/
	iRc=Keys_Sel(reply,&sKeys,"keytype=%d",nKeyType);
	if(iRc!=0)
	{
		sprintf(acErrMsg,"%s,%d ��ѯ������Կʧ��,ret=%d",__FILE__,__LINE__,iRc);
		vtcp_log("%s,%d ��ѯ������Կʧ��,ret=%d",__FILE__,__LINE__,iRc);
		return(-1);
	}
	/* end by LiuHuafeng 2007-1-20 0:56 */
	asc2bit(pcProtKey,pcBit,16);
	dhc_setkey(pcBit);
	asc2bit(sql_keyvalue,pcBit,16);
	dhc_encrypt(pcBit,1);
	bit2asc(pcBit,pcKeyValue,64);
	return(0);
}
/****ȡ�ñ�����Կ,����������򴴽���****/
int iGetProtKey(char *pcProtKey,char * reply){
	int nKeyType=0;
	char pcKeyValue[17];
	int sRandId=0;/****���������****/
	int id;
	int iRc=0;
	struct keys_c sKeys;
	char cTimes[7];
	int iHour=0,iMin=0,iSec=0;
	memset(cTimes, 0 , sizeof(cTimes));
	memset(&sKeys, 0 , sizeof(sKeys));
	/****������Կ�����ظ�����,�����ȼ���Ƿ��Ѿ����ڱ�����Կ****/	
	memset(pcKeyValue,'\0',sizeof(pcKeyValue));
	/** replace by LiuHuafeng 2007-1-20 0:11 ������������޸�
	EXEC SQL SELECT keyvalue into :pcKeyValue from keys  where keytype=:nKeyType;
	if(sqlca.sqlcode!=0 && sqlca.sqlcode!=1403){
		strcpy(acErrMsg,"��ѯ������Կʧ��");
		printf("%s,%d,��ѯ������Կʧ��[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
		return(-1);
	}
	if(sqlca.sqlcode==0){
		printf("%s,%d,�Ѿ����ڱ�����Կ,������������!\n",__FILE__,__LINE__);
		iRevKey(pcKeyValue,pcProtKey);
		return(0);
	}
	************************�޸���������***************************************/
	iRc=Keys_Sel(reply,&sKeys,"keytype=%d",nKeyType);
	if(iRc!=0 && iRc!=100)
	{
		sprintf(acErrMsg,"%s,%d ��ѯ������Կʧ��,ret=%d",__FILE__,__LINE__,iRc);
		WRITEMSG
		return(-1);
	}
	if(iRc==0)
	{
		vtcp_log("%s,%d �Ѿ����ڱ�����Կ,������������!\n",__FILE__,__LINE__);
		memcpy(pcKeyValue,sKeys.keyvalue,sizeof(pcKeyValue)-1);
		iRevKey(pcKeyValue,pcProtKey);
		memcpy(reply, "0000",4);
		return 0;
	}
	/*end by LiuHuafeng �޸ĵ�������� 2007-1-20 0:40**************************/
	/****�������ɱ�����Կ****/
	pub_base_GetTime2(&iHour,&iMin,&iSec);
	sprintf(cTimes,"%02d%02d%02d",iHour,iMin,iSec);
	memset(pcKeyValue,'\0',sizeof(pcKeyValue));
	sRandId=atoi(cTimes)%31;
	srand(sRandId);
	for(id=0;id<16;id++){
		int sKeyNum=rand()%16;
		if(sKeyNum>=10){
			pcKeyValue[id]=sKeyNum-10+'A';
		}else{
			pcKeyValue[id]=sKeyNum+'0';
		}	
	}
	/**begin replace by LiuHuafeng 2007-1-20 0:40 ������������޸�
	EXEC SQL INSERT INTO KEYS(keytype,keyvalue) values(:nKeyType,:pcKeyValue);
	if(sqlca.sqlcode){
		strcpy(acErrMsg,"���ɱ�����Կʧ��");
		printf("%s,%d,���ɱ�����Կʧ��[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
		return(-1);
	}
	** �޸���������**********************************************/
	sKeys.keytype=nKeyType;
	memcpy(sKeys.keyvalue,pcKeyValue,sizeof(sKeys.keyvalue)-1);
	iRc=Keys_Ins( sKeys , reply );
	if(iRc)
	{
		sprintf(acErrMsg,"%s,%d ���ɱ�����Կʧ��",__FILE__,__LINE__);
		vtcp_log("%s,%d,���ɱ�����Կʧ��[%d]\n",__FILE__,__LINE__,iRc);
		return(-1);
	}
	/* end by LiuHuafeng 2007-1-20 0:53 ********************/
	iRevKey(pcKeyValue,pcProtKey);
	return(0);
}
/****���ù�����Կ***
int iSetWorkKey(int nKeyType,char *pcKeyValue){
	char *pcProtKey[17];* �ĳ������
	char pcProtKey[17];

	char pcBit[64];
	char sql_keyvalue[17];
	int iRc;
	if(nKeyType==0){
		printf("%s,%d,����ͨ���˺������ñ�����Կ!\n",__FILE__,__LINE__);
		strcpy(acErrMsg,"����ͨ���˺������ñ�����Կ");
		return(-1);
	}
	memset(pcProtKey,'\0',sizeof(pcProtKey));
	memset(pcBit,'\0',sizeof(pcBit));
	memset(sql_keyvalue,'\0',sizeof(sql_keyvalue));
	if(iGetProtKey(pcProtKey)<0){
		printf("%s,%d,�õ�������Կʧ��!\n",__FILE__,__LINE__);	
		return(-1);
	}
	asc2bit(pcProtKey,pcBit,16);
	dhc_setkey(pcBit);
	asc2bit(pcKeyValue,pcBit,16);
	dhc_encrypt(pcBit,0);
	bit2asc(pcBit,sql_keyvalue,64);
	
	EXEC SQL update keys set keyvalue=:sql_keyvalue where keytype=:nKeyType;
	if(sqlca.sqlcode!=0 && sqlca.sqlcode!=1403){
		printf("%s,%d,���ù�����Կʧ��[%d]!\n",__FILE__,__LINE__,sqlca.sqlcode);
		strcpy(acErrMsg,"���ù�����Կʧ��!");
		return(-1);
	}
	if(sqlca.sqlcode==1403){
		EXEC SQL insert into keys(keytype,keyvalue)values(:nKeyType,:sql_keyvalue);
		if(sqlca.sqlcode!=0){
			printf("%s,%d,���ù�����Կʧ��[%d]!\n",__FILE__,__LINE__,sqlca.sqlcode);
			strcpy(acErrMsg,"���ù�����Կʧ��!");
			return(-1);
		}
	}
	return(0);
}
******************/
