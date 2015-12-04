/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2004-08-27                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : hv_pub.pc                                                ***
 *** �������� : �뽻���޹صĹ�������                                     ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
 ***************************************************************************/
#include"hv_pub.h"
#include"errlog.h"
#include"stdlib.h"
#include"string.h"
#include"ejfno.h"
#include "find_debug.h"
static unsigned char base64[]={
   'A','B','C','D','E','F','G',
   'H','I','J','K','L','M','N',
   'O','P','Q','R','S','T',
   'U','V','W','X','Y','Z',
   'a','b','c','d','e','f','g',
   'h','i','j','k','l','m','n',
   'o','p','q','r','s','t',
   'u','v','w','x','y','z',
   '0','1','2','3','4','5','6','7','8','9',
   '+','/'};
/**********************************************************************
*int  findElement(int arr[],int element)                              *
*  ���ܣ��������в���Ԫ�أ�����ĵ�һ��Ԫ��ΪԪ�ظ���                 *
*        ����Ԫ�ذ���������                                           *
*  ����ֵ��                                                           *
*      Ԫ���������е�λ��,�Ҳ�������-1                                *
**********************************************************************/		
int  findElement(int arr[],int element){
	int cnt,pos1,pos2,pos;
	cnt=arr[0];
	pos1=1;
	pos2=pos1+cnt-1;
	while(pos1<=pos2){
		pos=(pos1+pos2)/2;
		if(arr[pos]==element)
			return(pos);
		else if(element>arr[pos])
			pos1++;
		else
			pos2--;
	}
	return(-1);
}

/**********************************************************************
* int  hv_pack910(THVPACKAGE *pReqPack,char *pResCode,                *
*                              char *pAdd,THVPACKAGE *pResPack);      *
* ����:��֯ͨ��Ӧ����CMT910��                                       *
* ������                                                              *
*      pReqPack:�����Ľṹ                                          *
*      pResCode:Ӧ����                                                *
*      pAdd:����                                                      *
*      pResPack:Ӧ���Ľṹ                                          *
*  ����ֵ��0/-1                                                       *
**********************************************************************/
int  hv_pack910(THVPACKAGE *pReqPack,char *pResCode,char *pAdd,THVPACKAGE *pResPack){
	char pcHDate[9];
	int iRc;
	MEMSET_DEBUG(pcHDate,'0',sizeof(pcHDate)-1);
	pcHDate[sizeof(pcHDate)-1]='\0';
	iRc=hv_get_tag(pReqPack,"30A",pcHDate);
	MEMSET_DEBUG((char *)(pResPack),'\0',1024);
	iRc=hv_set_tag(pResPack,"30A",pcHDate);
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d Generate package error!!!",__FILE__,__LINE__);
		return(iRc);
	}
	iRc=hv_set_tag(pResPack,"C42",pResCode);/*Ӧ����*/
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d Generate package error!!!",__FILE__,__LINE__);
		return(iRc);
	}
	iRc=hv_set_tag(pResPack,"72A",pAdd);/*����*/
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d Generate package error!!!",__FILE__,__LINE__);
		return(iRc);
	}
	MEMCPY_DEBUG(pResPack->CMTCode,"910",3);
	getPackageFlgNo(pResPack);/*���ı�ʶ��*/
	iRc=hv_gen_pack(pResPack);/*���ɱ���*/
	if(iRc<0){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d Generate package error!!!",__FILE__,__LINE__);
		return(iRc);
	}
	return(0);
}
/**********************************************************************
* int base64_encode(unsigned char *pSrcCode,unsigned char *pTgtCode,  *
*                        int nSrcLen)                                 *
* ����:BASE64����ı��뺯��                                           *
* ������                                                              *
*      pSrcCode:Դ����                                                *
*      pTgtCode:Ŀ�ı���                                              *
*      nSrcLen :Դ���ĳ���                                            *
*  ����ֵ��0/-1                                                       *
**********************************************************************/
int base64_encode(unsigned char *pSrcCode,unsigned char *pTgtCode,int nSrcLen){
	int nTgtLen,nTgtEqLen;
	unsigned char buf3[3];
	unsigned char buf4[4];
	int nSrcPos,nTgtPos;
	int sum,dum;
	int i,j;
	nSrcPos=nTgtPos=0;
	if(nSrcLen%3==0){
		nTgtLen=nSrcLen/3*4;
		nTgtEqLen=0;
	}else{
		nTgtLen=(nSrcLen/3+1)*4;
		nTgtEqLen=3-(nSrcLen%3);
	}
	while(nSrcPos<nSrcLen){
		if(nSrcPos+3>nSrcLen){
			/*���������ӽ�*/
			MEMCPY_DEBUG((char *)buf3,(char *)(pSrcCode+nSrcPos),nSrcLen-nSrcPos);
			MEMSET_DEBUG((char *)(buf3+nSrcLen-nSrcPos),'\0',3-nSrcLen+nSrcPos);
		}else{
			/*�������ӽ�*/
			MEMCPY_DEBUG((char *)buf3,(char *)(pSrcCode+nSrcPos),3);
		}
		sum=0;
		for(i=0;i<3;i++){
			sum*=256;
			sum+=buf3[i];
		}
		dum=64*64*64;
		for(i=0;i<4;i++){
			buf4[i]=base64[sum/dum];
			sum=sum%dum;
			dum/=64;
		}
		MEMCPY_DEBUG((char *)(pTgtCode+nTgtPos),(char *)buf4,4);
		nTgtPos+=4;
		nSrcPos+=3;
	}
	if(nTgtEqLen!=0){
		MEMSET_DEBUG((char *)(pTgtCode+nTgtLen-nTgtEqLen),'=',nTgtEqLen);
	}
	pTgtCode[nTgtLen]='\0';
	return(0);
}
/**********************************************************************
* int base64_decode(unsigned char *pSrcCode,unsigned char *pTgtCode,  *
*                                                 int *nTgtLen)       *
* ����:BASE64����Ľ��뺯��                                           *
* ������                                                              *
*      pSrcCode:Դ����                                                *
*      pTgtCode:Ŀ�ı���                                              *
*      nTgtLen :Ŀ�ı��ĳ��ȱ���ָ��                                  *
*  ����ֵ��0/-1                                                       *
**********************************************************************/
int base64_decode(unsigned char *pSrcCode,unsigned char *pTgtCode,int *nTgtLen){
	int nSrcLen;
	unsigned char buf3[3];
	unsigned char buf4[4];
	int nSrcPos,nTgtPos;
	int sum,dum;
	int i,j;
	nSrcPos=nTgtPos=0;
	nSrcLen=strlen(pSrcCode);
	if(nSrcLen%4!=0)
		return(-1);
	if(pSrcCode[nSrcLen-2]=='='){
		*nTgtLen=nSrcLen/4*3-2;
	}else if(pSrcCode[nSrcLen-1]=='='){
		*nTgtLen=nSrcLen/4*3-1;
	}else{
		*nTgtLen=nSrcLen/4*3;
	}
	
	while(nSrcPos<nSrcLen){
		MEMCPY_DEBUG((char *)buf4,(char *)(pSrcCode+nSrcPos),4);
		sum=0;
		for(i=0;i<4;i++){
			int ch;
			ch=pSrcCode[nSrcPos+i];
			if(ch>='A' && ch<='Z'){
				ch=ch-'A';
			}else if(ch >='a' && ch<='z'){
				ch=ch-'a'+26;
			}else if(ch>='0' && ch<='9'){
				ch=ch-'0'+52;
			}else if(ch=='+'){
				ch=62;
			}else if(ch=='/'){
				ch=63;
			}else if(ch=='='){
				ch=0;
			}else{
				return(-1);
			}
			sum*=64;
			sum+=ch;
		}
		dum=256*256;
		for(i=0;i<3;i++){
			buf3[i]=sum/dum;
			sum=sum%dum;
			dum/=256;
		}
		if(nTgtPos+3>*nTgtLen){
			MEMCPY_DEBUG((char *)(pTgtCode+nTgtPos),(char *)buf3,*nTgtLen-nTgtPos);
		}else{
			MEMCPY_DEBUG((char *)(pTgtCode+nTgtPos),(char *)buf3,3);
		}
		nTgtPos+=3;
		nSrcPos+=4;
	}
	return(0);
}
/**********************************************************************
* void rtrim(char *str)                                               *
* ����:ȥ���ַ����ұߵĿո�                                           *
**********************************************************************/
void rtrim(char *str){
	int len;
	len=strlen(str);
	while(len>0){
		if(str[len-1]==' '){
			str[len-1]='\0';
			len--;
		}else
			break;
	}
}
/**********************************************************************
* int hv_get_idtno()                                                  *
* ����:   �õ����ı�ʶ��                                              *
* ����ֵ�����ı�ʶ��                                                  *
**********************************************************************/
int hv_get_idtno(){
	int no;
	no=fetchEjfno(1,EJF_IDTNO);
	commitEjfno(1);
	return(no);
}
/**********************************************************************
* int hv_get_refno()                                                  *
* ����:   �õ����Ĳο���                                              *
* ����ֵ�����Ĳο���                                                  *
**********************************************************************/
int hv_get_refno(){
	int no;
	no=fetchEjfno(1,EJF_REFNO);
	commitEjfno(1);
	return(no);
}
/**********************************************************************
* int hv_get_ltxno()                                                  *
* ����:   �õ�С������                                            *
* ����ֵ��С������                                                *
**********************************************************************/
int hv_get_ltxno(){
	int no;
	no=fetchEjfno(1,EJF_LTXNO);
	commitEjfno(1);
	return(no);
}
/**********************************************************************
* int hv_get_htxno()                                                  *
* ����:   �õ��������                                            *
* ����ֵ���������                                                *
**********************************************************************/
int hv_get_htxno(){
	int no;
	no=fetchEjfno(1,EJF_HTXNO);
	commitEjfno(1);
	return(no);
}
int hv_get_htxno_eis(){
	int no;
	no=fetchEjfno(1,EJF_HTXNO);
	commitEjfno(1);
	return(no+atoi(JOIN_FORENO)*100000);
}
/**********************************************************************
* int hv_get_cancno()                                                 *
* ����:   �õ�����/�˻������                                         *
* ����ֵ������/�˻������                                             *
**********************************************************************/
int hv_get_cancno(){
	int no;
	no=fetchEjfno(1,EJF_CANCNO);
	commitEjfno(1);
	return(no);
}
/**********************************************************************
* int hv_get_cancno()                                                 *
* ����:   �õ���ѯ���                                         *
* ����ֵ������/�˻������                                             *
**********************************************************************/
int hv_get_queryno(){
	int no;
	no=fetchEjfno(2,EJF_QUERYNO);
	commitEjfno(2);
	return(no);
}
/**********************************************************************
* int hv_get_callbackno()                                                 *
* ����:   �õ��鸴��                                         *
* ����ֵ������/�˻������                                             *
**********************************************************************/
int hv_get_callbackno(){
	int no;
	no=fetchEjfno(2,EJF_CALLBACKNO);
	commitEjfno(2);
	return(no);
}

/**********************************************************************
* char* fcopy(char *pcFileName,THVPACKAGE *pHvPack,int flag)          *
* ����:   ��ָ���������ݴ�ŵ�һ���ļ��У��������ļ���                *
* ����:                                                               *
*       pcFileName-��ŷ��ص��ļ���                                   *
*       pHvPack-���ݱ���                                              *
*       flag-������־                                                 *
* ����ֵ���ļ���                                                      *
* ˵����                                                              *
*      �ļ����Ĳ��������ǣ�$(WORKDIR)/dat/����_(W/L)���ı�ʶ��.dat    *
**********************************************************************/
char* fcopy(char *pcFileName,THVPACKAGE *pHvPack,int flag){
	FILE *fp;
	char *chLW;
	if(flag==FLAG_SENDER){
		/*����*/
		chLW[0]='W';
	}else{
		/*����*/
		chLW[0]='L';
	}
	sprintf(pcFileName,"%s/dat/%.8s_%c%.8s.dat",getenv("WORKDIR"),pHvPack->workDate,chLW,pHvPack->flgNo);
	
	fp=fopen(pcFileName,"wb");
	if(fp==NULL){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,fcopy error,can not open:[%s] ",__FILE__,__LINE__,pcFileName);
		return(NULL);
	}
	fwrite(pHvPack->body,pHvPack->length,1,fp);
	fclose(fp);
	return(pcFileName);
}		
/*��������ַ����е�С��128��ת��ȫ��*/
char *DBC2SBC(char *DBCstr,int cplen)
{
	int Length,i;
	char *dp;
	char *sp;
	static   char SBCstr[128];
	dp = DBCstr;
	sp = SBCstr;
	MEMSET_DEBUG(SBCstr,'\0',sizeof(SBCstr));
	Length = 0;
	while(*dp!=0)
	{
		if(Length>=cplen)
		{
			break;
		}
		if((unsigned char)(*dp)>128)
		{
			*sp = *dp;
			*(sp+1) = *(dp+1);
			sp +=2;
			dp +=2;
			Length+=2;
		}else{
			if(*dp == ' ')
			{
				*sp = (char)(161);
				*(sp+1) = (char)(161); 
			}else{
				*sp = (char)(163);
				*(sp+1) = (char)( (unsigned char)(*dp)+128 );
			}
			sp +=2;
			dp ++;
			Length+=2;
		}
	}
	while(cplen>Length){
		*sp=(char)(161);
		*(sp+1)=(char)(161);
		sp+=2;
		Length+=2;
	}	
	sp[cplen]='\0';	
	return(SBCstr);
}
