#include "lv_pub.h"

int  lv_pack911(TLVPACKAGE *pReqPack,char *pResCode,char *pAdd,TLVPACKAGE *pResPack){
	return 0;
}

int  lv_pack920(TLVPACKAGE *pReqPack,char *pResCode,char *pAdd,TLVPACKAGE *pResPack){
	return 0;
}

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
* char* fcopy(char *pcFileName,THVPACKAGE *pLvpack,int flag)          *
* ����:   ��ָ���������ݴ�ŵ�һ���ļ��У��������ļ���                *
* ����:                                                               *
*       pcFileName-��ŷ��ص��ļ���                                   *
*       pLvpack-���ݱ���                                              *
*       flag-������־                                                 *
* ����ֵ���ļ���                                                      *
* ˵����                                                              *
*      �ļ����Ĳ��������ǣ�$(WORKDIR)/dat/����_(W/L)���ı�ʶ��.dat    *
**********************************************************************/
char* fcopy(char *pcFileName,TLVPACKAGE *pLvPack,int flag){
	FILE *fp;
	char chLW;
	char buf[1025];
	int iRc;
	if(flag==FLAG_SENDER){
		/*����*/
		chLW='W';
	}else{
		/*����*/
		chLW='L';
	}
	sprintf(pcFileName,"%s/dat/%.8s_%c%.8s.dat",getenv("WORKDIR"),pLvPack->workDate,chLW,pLvPack->flgNo);
	
	/*fp=fopen(pcFileName,"rb");
	if(fp!=NULL){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,fcopy error,file[%s] exists ",__FILE__,__LINE__,pcFileName);
		fclose(fp);
		return(NULL);
	}del by weizhao 2005-11-30 ����ļ��Ѿ����ڣ��͸��ǣ��������ﲻ�������*/
	fp=fopen(pcFileName,"wb");
	if(fp==NULL){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,fcopy error,can not open:[%s] ",__FILE__,__LINE__,pcFileName);
		return(NULL);
	}
	printf("%s,%d,In fcopy:%s\n",__FILE__,__LINE__,pcFileName);
	
	pLvPack->dm.curNum = 1;
	while(pLvPack->dm.curNum<pLvPack->dm.length){
		memset(buf,0,sizeof(buf));
	  iRc = dm_dmemcpy2(buf,&pLvPack->dm,pLvPack->dm.curNum,1024); 		
		if (iRc<0 )                                                  	
			return 0;                                              	
		else if (iRc == 0)                                           	
			break;                                                     	
		fwrite(buf,1,1024,fp);                                       	
		/*pReqPack->dm.curNum+=1024;*/	                             	
		pLvPack->dm.curNum++;
	}	                                                             	
		                                                               
	fclose(fp);
	return(pcFileName);
}	

/**********************************************************************
* char* regerr(char *pcFileName                                       *
* ����:   �ǼǴ������ı����ļ���                                    *
* ����:                                                               *
*       pcFileName-��ŷ��ص��ļ���                                   *
* ����ֵ���ļ���                                                      *
* ˵����                                                              *
*      �Ǽǵ��ļ�$WORKDIR/dat/err.dat��                               *
**********************************************************************/
int regerr(char *pcFileName){
        char cmd[256];
        sprintf(cmd,"echo %s >>%s/dat/err.dat",pcFileName,getenv("WORKDIR"));
        system(cmd);
        return(0);
}






