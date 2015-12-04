/***************************************************************************
 *** �������� : Shi shenghui ʯ����                                      ***
 *** ��    �� : 2006-11-15                                               ***
 *** ����ģ�� :                                                          ***
 *** �������� : cifpack.c                                                ***
 *** �������� : CIF���ĸ�ʽ����⺯��                                    ***
 ***                                                                     ***
 *** ʹ��ע�� :                                                          ***
 ***************************************************************************/
#include"cifpack.h"
#include"stdio.h"
#include"stdlib.h"
#include"string.h"
#include"time.h"
#include"sys/timeb.h"
#include "errlog.h"

extern char pcCifGetTime(char *pcTimeStr);
static void fullspace_len(char * con,int len);
/**********************************************************************
*  void vInitCifPackage(TCIFPACK *pack);                              *
*  ���ܣ�  ��ʼ��CIF���Ľṹ                                          *
*  ������  ��                                                         *
*  ����ֵ����                                                         *
*                                                                     *
**********************************************************************/         
void vInitCifPack(TCIFPACK *pack){
	char pcTimeStr[32];
	char pcLength[7];
	memset(pcTimeStr,'\0',sizeof(pcTimeStr));
	memset(pack,'\0',sizeof(TCIFPACK));
	memcpy(pack->head.blockhead,"{1:",3);
	sprintf(pcLength,"%06d",sizeof(pack->head)+4);
	memcpy(pack->head.length,pcLength,6);	
	pack->head.use='0';
	pack->head.inout='0';
	pack->head.ywtype='2';
	memset(pack->head.status,'0',3);	
	memset(pack->head.flgno,'0',sizeof(pack->head.flgno));	
	memset(pack->head.refno,'0',sizeof(pack->head.refno));	
	pcCifGetTime(pcTimeStr);
	memcpy(pack->head.sendtm,pcTimeStr,sizeof(pack->head.sendtm));
	/****cifno��Ӧ������ֵ****/
	memcpy(pack->head.workdate,pcTimeStr,sizeof(pack->head.workdate));
	pack->head.blocktail='}';
	memcpy(pack->body.blockhead,"{3:",3);
	pack->body.buffer[0]='}';
}
/**********************************************************************
*  void vAddCifTag(TCIFPACK *pack,char *tagname,char *tagdata);       *
*  ���ܣ�  ��CIF����������һ��TAG(�������޸ĵ����)                   *
*  ������                                                             * 
*          pack		-CIF���Ľṹ��                                    *
*          tagname	-TAG������                                        *
*          tagdata	-TAG������,��\0��β                               *
*  ����ֵ����                                                         *
*                                                                     *
**********************************************************************/         
void vAddCifTag(TCIFPACK *pack,char *tagname,char *tagdata){
	int iPos=strlen(pack->body.buffer)-1;	/***���һ���ֽڴ�����***/
	char pcTagNameBlock[6];
	char pcLength[7];
	sprintf(pcTagNameBlock,":%.3s:",tagname);
	memcpy(pack->body.buffer+iPos,pcTagNameBlock,5);
	strcpy(pack->body.buffer+iPos+5,tagdata);
	strcpy(pack->body.buffer+iPos+5+strlen(tagdata),"}");		
	sprintf(pcLength,"%06d",sizeof(pack->head)+3+iPos+5+strlen(tagdata)+1);
	memcpy(pack->head.length,pcLength,6);
}
/**********************************************************************
*  void vGetCifTag(TCIFPACK *pack,char *tagname,char *tagdata);       *
*  ���ܣ�  ��CIF�����еõ�һ��TAG������                               *
*  ������                                                             * 
*          pack		-CIF���Ľṹ��                                    *
*          tagname	-TAG������                                        *
*          tagdata	-TAG������(���Զ�׷��\0)                          *
*  ����ֵ����                                                         *
*                                                                     *
**********************************************************************/         
void vGetCifTag(TCIFPACK *pack,char *tagname,char *tagdata){
	char *ptr=pack->body.buffer;
	tagdata[0]='\0';
	while(*ptr!='}' && *ptr!='\0'){
		if(*ptr==':'){
			if(*(ptr+4)!=':'){
				printf("%s,%d,������[%.5s]������ʽ����\n",__FILE__,__LINE__,ptr);
				return;
			}else if(memcmp(ptr+1,tagname,3)==0){
				/***�ҵ�TAG****/
				ptr+=5;
				int i=0;
				while(ptr[i]!=':' && ptr[i]!='}' && ptr[i]!='\0'){
					tagdata[i]=ptr[i];
					i++;
				}		
				return;
			}else{
				/***�Ǹ�TAG����������Ҫ��****/
				ptr+=5;	
				continue;
			}
		}else{
			ptr++;
		}	
	}
}
/****
int main(){
	TCIFPACK pack;
	char pcBuffer[8];
	vInitCifPack(&pack);
	memcpy(pack.head.cifno,"100",3);
	vAddCifTag(&pack,"30A","20080101");
	vAddCifTag(&pack,"0BC","90000001");
	printf("[%s]\n",&pack);
	vGetCifTag(&pack,"0BC",	pcBuffer);
	printf("[%.8s]\n",pcBuffer);
	return(0);
}
****/
/*begin add by LiuHuafeng 2007-5-16 6:42
 * �Ҳ���ո� */
static void fullspace_len(char * con,int len)
{
	char cTmpbuf[1024*10];
	int i=0,iLen=0;
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	if(len<sizeof(cTmpbuf)-1) iLen=len;
	else 
		iLen=sizeof(cTmpbuf)-1;
	memcpy(cTmpbuf,con,iLen);
	for(i=0;i<iLen;i++)
	{
		if(cTmpbuf[i]=='\0') cTmpbuf[i]=' ';
	}
	memcpy(con,cTmpbuf, len);
	return;
}
/* end by LiuHuafeng 2007-5-16 6:43 */
/*begin add by LiuHuafeng 2007-5-16 3:32 
 * ʵ��CIFϵͳ����֮��ת�� */
void vCif2yzj(TCIFPACK *pack,char * yzj_buf,char * wssrno)
{
	int i=0;
	int iLen=128;
	char cLen[9];
	char cTmpbuf[100];
	char cTmpBuff[1024];
	char pcBuffer[1024];
	char pcTimeStr[32];
	char cBuf[1024*10];
	char cTagname[4];

	memset(pcTimeStr,'\0',sizeof(pcTimeStr));

	memset(pcBuffer, 0 , sizeof(pcBuffer));
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	memset(cLen, 0 , sizeof(cLen));
	/****TODO2***/
	if(memcmp(pack->head.cifno,"100",3)==0)
	{
		iLen=iLen+684;
	}
	else
	{
		iLen=iLen+831;
	}
	/* �ܳ��� */
	sprintf(cLen,"%08d",iLen);
	strncat(pcBuffer,cLen,8);
	iLen=0;
	/*���ĺŵ�һλ */
	strncat(pcBuffer,"3",1);
	memset(cLen, 0 , sizeof(cLen));
	/* ���ĺź�4λ */
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	memcpy(cTmpbuf,pack->head.cifno,3);
	strncat(pcBuffer,cTmpbuf,3);
	/* ��� */
	strncat(pcBuffer,"1",1);
	/* ���𷽱�ʶ ����Ϊ�ո� */
	strncat(pcBuffer,"   ",3);
	/* ����IP ����Ϊ�ո� */
	strncat(pcBuffer,"                 ",15);/****TODO2****/
	/* ������ˮ�� */
	strncpy(cTmpbuf,wssrno,6);
	fullspace_len(cTmpbuf,10);
	strncat(pcBuffer,cTmpbuf,10);
	/* Ʊ�ݱ�ʶ */
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	vGetCifTag(pack,"D01",cTmpbuf);
	strncat(pcBuffer,cTmpbuf,12);
	/* ��ǩ״̬ */
	strncat(pcBuffer,"0",1);
	/* ��ǩ״̬ */
	strncat(pcBuffer,"0",1);
	/* �������� */
	pcCifGetTime(pcTimeStr);
	memset(cLen, 0 , sizeof(cLen));
	memcpy(cLen,pcTimeStr,8);
	strncat(pcBuffer,cLen,8);
	/* ����ʱ�� */
	memset(cLen, 0 , sizeof(cLen));
	memcpy(cLen,pcTimeStr+8,6);
	strncat(pcBuffer,cLen,6);
	/* ���ܷ���ˮ ����Ϊ�ո� */
	strncat(pcBuffer,"          ",10);
	/* ҵ��״̬ ֱ������000 */
	strncat(pcBuffer,"000",3);
	/* �������� */
	strncat(pcBuffer,"                                        ",40);
	/* �������ʽ */
	strncat(pcBuffer,"B",1);
	/* Ԥ���ֶ� */
	strncat(pcBuffer,"     ",5);
	memcpy(yzj_buf,pcBuffer,128);
	/** ������ͷ */
	memset(cTmpBuff, 0 , sizeof(cTmpBuff));
	memset(cLen, 0 , sizeof(cLen));
	memset(cTagname, 0 , sizeof(cTagname));
	memset(cBuf, 0 , sizeof(cBuf));
	memset(pcBuffer ,0 , sizeof(pcBuffer));
	if(memcmp(pack->head.cifno,"100",3)==0)
	{
		for(i=0; i<YZJ_100_LEN;i++)
		{
			memset(pcBuffer ,0 , sizeof(pcBuffer));
			memset(cTagname, 0 , sizeof(cTagname));
			memcpy(cTagname,str_yzj_100[i].tagname,3);
			memcpy(cLen,str_yzj_100[i].len,3);
			iLen=0;
			printf("%s,%s,%s,%s\n",str_yzj_100[i].name,str_yzj_100[i].len,str_yzj_100[i].type,str_yzj_100[i].tagname);
			if( memcmp(cTagname,"ZMC",3)==0 
				 ||memcmp(cTagname,"FMC",3)==0 
			)
			{
				iLen=atoi(cLen);
				strncat(cBuf,"00000000",iLen);
			}
			else if(memcmp(cTagname,"CSG",3)==0 )
			{
				iLen=atoi(cLen);
				strncat(cBuf,"B",iLen);
			}
			else if(memcmp(cTagname,"72E",3)==0 
				 ||memcmp(cTagname,"72F",3)==0 
			)
			{
				
			}
			else if(memcmp(cTagname,"TPD",3)==0 )
			{
				iLen=atoi(cLen);
				strncat(cBuf,"                ",iLen);
			}
			else if(memcmp(cTagname,"YWZ",3)==0)
			{
				iLen=atoi(cLen);
				strncat(cBuf,"01",iLen);
			}
			else if(memcmp(cTagname,"D42",3)==0 )
			{
				char cName60[61];
				int j=0;
				iLen=atoi(cLen);
				memset(cTmpBuff, 0 , sizeof(cTmpBuff));
				memset(cName60, 0 , sizeof(cName60));
				vGetCifTag(pack,"D41",	pcBuffer);
				j=atoi(pcBuffer);
				vGetCifTag(pack,"D42",pcBuffer);
				for(;j>0;j--)
				{
					memcpy(cName60,pcBuffer+(j-1)*60,60);
					zip_space(cName60);
					strncat(cTmpBuff,cName60,strlen(cName60));
					strncat(cTmpBuff,"|",1);
				}
				memcpy(cName60,pcBuffer,60);
				strncat(cTmpBuff,cName60,strlen(cName60));
				fullspace_len(cTmpBuff,iLen);
				strncat(cBuf,cTmpBuff,iLen);
			}
			else
			{
				iLen=atoi(cLen);
				vGetCifTag(pack,cTagname,	pcBuffer);
				fullspace_len(pcBuffer,iLen);
				strncat(cBuf,pcBuffer,iLen);
			}
		}
		printf("%s,%d pcBuffer==len=[%d]\n",__FILE__,__LINE__,strlen(cBuf));
		printf("%s,%d pcBuffer==[%s]\n",__FILE__,__LINE__,cBuf);
	}
	else
	{
		for(i=0; i<YZJ_101_LEN;i++)
		{
			memset(pcBuffer ,0 , sizeof(pcBuffer));
			memset(cTagname, 0 , sizeof(cTagname));
			memcpy(cTagname,str_yzj_101[i].tagname,3);
			memcpy(cLen,str_yzj_101[i].len,3);
			iLen=0;
			printf("%s,%s,%s,%s\n",str_yzj_101[i].name,str_yzj_101[i].len,str_yzj_101[i].type,str_yzj_101[i].tagname);
			if( memcmp(cTagname,"ZMC",3)==0 
				 ||memcmp(cTagname,"FMC",3)==0 
			)
			{
				iLen=atoi(cLen);
				strncat(cBuf,"00000000",iLen);
			}
			else if(memcmp(cTagname,"CSG",3)==0 )
			{
				iLen=atoi(cLen);
				strncat(cBuf,"B",iLen);
			}
			else if(memcmp(cTagname,"72E",3)==0 
				 ||memcmp(cTagname,"72F",3)==0 
			)
			{
				
			}
			else if(memcmp(cTagname,"TPD",3)==0 )
			{
				iLen=atoi(cLen);
				strncat(cBuf,"                ",iLen);
			}
			else if(memcmp(cTagname,"YWZ",3)==0)
			{
				iLen=atoi(cLen);
				strncat(cBuf,"01",iLen);
			}
			else if(memcmp(cTagname,"D42",3)==0 )
			{
				char cName60[61];
				int j=0;
				iLen=atoi(cLen);
				memset(cTmpBuff, 0 , sizeof(cTmpBuff));
				memset(cName60, 0 , sizeof(cName60));
				vGetCifTag(pack,"D41",	pcBuffer);
				j=atoi(pcBuffer);
				vGetCifTag(pack,"D42",pcBuffer);
				for(;j>0;j--)
				{
					memcpy(cName60,pcBuffer+(j-1)*60,60);
					zip_space(cName60);
					strncat(cTmpBuff,cName60,strlen(cName60));
					strncat(cTmpBuff,"|",1);
				}
				memcpy(cName60,pcBuffer,60);
				strncat(cTmpBuff,cName60,strlen(cName60));
				fullspace_len(cTmpBuff,iLen);
				strncat(cBuf,cTmpBuff,iLen);
			}
			else
			{
				iLen=atoi(cLen);
				vGetCifTag(pack,cTagname,	pcBuffer);
				fullspace_len(pcBuffer,iLen);
				strncat(cBuf,pcBuffer,iLen);
			}
		}
		printf("%s,%d pcBuffer==len=[%d]\n",__FILE__,__LINE__,strlen(cBuf));
		printf("%s,%d pcBuffer==[%s]\n",__FILE__,__LINE__,cBuf);
	}
	strncat(yzj_buf,cBuf,strlen(cBuf));	
	printf("%s,%d yzj_buf==len=[%d]\n",__FILE__,__LINE__,strlen(yzj_buf));
	printf("%s,%d yzj_buf==[%s]\n",__FILE__,__LINE__,yzj_buf);
}

void vYzj2cif(char * yzj_buff,TCIFPACK *pack)
{
	char cBuf[1024*10];
	int i=0,iLen=0,iTotlen=0;
	char pcBuffer[1024];
	char cTagname[4];
	char cTmpBuff[1024];
	char cLen[4];
	char cShr[3];
	memcpy(pack->head.status,yzj_buff+79,3);/****TODO2****/
	memset(cShr , 0 , sizeof(cShr));
	memset(cTmpBuff, 0 , sizeof(cTmpBuff));
	memset(cLen, 0 , sizeof(cLen));
	memset(cTagname, 0 , sizeof(cTagname));
	memset(cBuf, 0 , sizeof(cBuf));
	memcpy(cBuf,yzj_buff+128,sizeof(cBuf)-1);
	memset(pcBuffer ,0 , sizeof(pcBuffer));
	vAddCifTag(pack,"TXC","0400");/* ��֮�������ֻ����һ��ҵ�� */
	memcpy (pack->head.cifno,yzj_buff+9,3);/** add by martin **/
	errLog(LOG_ERR,0,RPT_TO_LOG,0,0,"��֮��yzj_buff+9����[%s],[%s],[%d]\n",yzj_buff+9,__FILE__,__LINE__);
	if(memcmp(yzj_buff+9,"100",3)==0)
	{
		for(i=0; i<YZJ_100_LEN;i++)
		{
			memset(pcBuffer ,0 , sizeof(pcBuffer));
			memset(cTagname, 0 , sizeof(cTagname));
			memcpy(cTagname,str_yzj_100[i].tagname,3);
			memcpy(cLen,str_yzj_100[i].len,3);
			iLen=0;
			printf("%s,%s,%s,%s\n",str_yzj_100[i].name,str_yzj_100[i].len,str_yzj_100[i].type,str_yzj_100[i].tagname);
			if( memcmp(cTagname,"ZMC",3)==0 
				 ||memcmp(cTagname,"FMC",3)==0 
			)
			{
				iLen=atoi(cLen);
				vAddCifTag(pack,cTagname,"00000000");
				iTotlen=iTotlen+iLen;
			}
			else if(memcmp(cTagname,"CSG",3)==0 )
			{
				iLen=atoi(cLen);
				vAddCifTag(pack,cTagname,"B");
			}
			else if(memcmp(cTagname,"72E",3)==0 
				 ||memcmp(cTagname,"72F",3)==0 
			)
			{
				
			}
			else if(memcmp(cTagname,"TPD",3)==0 )
			{
				/* ��Ʊ�������⴦��,���ҵ��״̬����02,ͬ��֧�� */
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				if(memcmp(cShr,"02",2)!=0)
				{
					/* ����ͬ��֧��,��ô��SHR����ΪTPD��ȥ������ֵ */
					int j=0,k=0;
					memset(cTmpBuff, 0 , sizeof(cTmpBuff));
					for(j=0,k=0;k<iLen;k++)
					{
						if(pcBuffer[k]!='|')
						{
							cTmpBuff[j]=pcBuffer[k];
							j++;
						}
					}
					vAddCifTag(pack,"SHR",cTmpBuff);
				}
				else
				{
					/* ����SHR����Ϊ00 */
					vAddCifTag(pack,"SHR","00");
				}
			}
			else if(memcmp(cTagname,"D42",3)==0 )
			{
				char cName60[61];
				int j=0;
				char *p;
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				p=strtok(pcBuffer,"|");
				strncpy(cName60,p,60);
				fullspace_len(cName60,60);
				strncat(cTmpBuff,cName60,strlen(cName60));
				do{
					p=strtok('\0',"|");
					if(p)
					{
						strncpy(cName60,p,60);
						fullspace_len(cName60,60);
						strncat(cTmpBuff,cName60,strlen(cName60));
					}
				}while(p);
				vAddCifTag(pack,cTagname,cTmpBuff);
			}
			else if(memcmp(cTagname,"YWZ",3)==0)
			{
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				fullspace_len(pcBuffer,iLen);
				memcpy(cShr,pcBuffer,iLen);
			}
			else
			{
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				fullspace_len(pcBuffer,iLen);
				vAddCifTag(pack,cTagname,pcBuffer);
			}
			iTotlen=iTotlen+iLen;
		}
	}
	else
	{
		for(i=0; i<YZJ_101_LEN;i++)
		{
			memset(pcBuffer ,0 , sizeof(pcBuffer));
			memset(cTagname, 0 , sizeof(cTagname));
			memcpy(cTagname,str_yzj_101[i].tagname,3);
			memcpy(cLen,str_yzj_101[i].len,3);
			iLen=0;
			printf("%s,%s,%s,%s\n",str_yzj_101[i].name,str_yzj_101[i].len,str_yzj_101[i].type,str_yzj_101[i].tagname);
			if( memcmp(cTagname,"ZMC",3)==0 
				 ||memcmp(cTagname,"FMC",3)==0 
			)
			{
				iLen=atoi(cLen);
				vAddCifTag(pack,cTagname,"00000000");
				iTotlen=iTotlen+iLen;
			}
			else if(memcmp(cTagname,"CSG",3)==0 )
			{
				iLen=atoi(cLen);
				vAddCifTag(pack,cTagname,"B");
			}
			else if(memcmp(cTagname,"72E",3)==0 
				 ||memcmp(cTagname,"72F",3)==0 
			)
			{
				
			}
			else if(memcmp(cTagname,"TPD",3)==0 )
			{
				/* ��Ʊ�������⴦��,���ҵ��״̬����02,ͬ��֧�� */
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				if(memcmp(cShr,"02",2)!=0)
				{
					/* ����ͬ��֧��,��ô��SHR����ΪTPD��ȥ������ֵ */
					int j=0,k=0;
					memset(cTmpBuff, 0 , sizeof(cTmpBuff));
					for(j=0,k=0;k<iLen;k++)
					{
						if(pcBuffer[k]!='|')
						{
							cTmpBuff[j]=pcBuffer[k];
							j++;
						}
					}
					vAddCifTag(pack,"SHR",cTmpBuff);
				}
				else
				{
					/* ����SHR����Ϊ00 */
					vAddCifTag(pack,"SHR","00");
				}
			}
			else if(memcmp(cTagname,"D42",3)==0 )
			{
				char cName60[61];
				int j=0;
				char *p;
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				p=strtok(pcBuffer,"|");
				strncpy(cName60,p,60);
				fullspace_len(cName60,60);
				strncat(cTmpBuff,cName60,strlen(cName60));
				do{
					p=strtok('\0',"|");
					if(p)
					{
						strncpy(cName60,p,60);
						fullspace_len(cName60,60);
						strncat(cTmpBuff,cName60,strlen(cName60));
					}
				}while(p);
				vAddCifTag(pack,cTagname,cTmpBuff);
			}
			else if(memcmp(cTagname,"YWZ",3)==0)
			{
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				fullspace_len(pcBuffer,iLen);
				memcpy(cShr,pcBuffer,iLen);
			}
			else
			{
				iLen=atoi(cLen);
				memcpy(pcBuffer,cBuf+iTotlen,iLen);
				fullspace_len(pcBuffer,iLen);
				vAddCifTag(pack,cTagname,pcBuffer);
			}
			iTotlen=iTotlen+iLen;
		}
	}
}

/* end by LiuHuafeng 2007-5-16 6:43 */
