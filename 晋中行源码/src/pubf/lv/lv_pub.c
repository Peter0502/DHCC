#include "lv_pub.h"

int  lv_pack911(TLVPACKAGE *pReqPack,char *pResCode,char *pAdd,TLVPACKAGE *pResPack){
	return 0;
}

int  lv_pack920(TLVPACKAGE *pReqPack,char *pResCode,char *pAdd,TLVPACKAGE *pResPack){
	return 0;
}

/**********************************************************************
*int  findElement(int arr[],int element)                              *
*  功能：在数组中查找元素，数组的第一个元素为元素个数                 *
*        数组元素按升序排序                                           *
*  返回值：                                                           *
*      元素在数组中的位置,找不到返回-1                                *
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
* 功能:去掉字符串右边的空格                                           *
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
* 功能:   把指定报文数据存放到一个文件中，并返回文件名                *
* 参数:                                                               *
*       pcFileName-存放返回的文件名                                   *
*       pLvpack-数据报文                                              *
*       flag-来往标志                                                 *
* 返回值：文件名                                                      *
* 说明：                                                              *
*      文件名的产生规则是：$(WORKDIR)/dat/日期_(W/L)报文标识号.dat    *
**********************************************************************/
char* fcopy(char *pcFileName,TLVPACKAGE *pLvPack,int flag){
	FILE *fp;
	char chLW;
	char buf[1025];
	int iRc;
	if(flag==FLAG_SENDER){
		/*往账*/
		chLW='W';
	}else{
		/*来账*/
		chLW='L';
	}
	sprintf(pcFileName,"%s/dat/%.8s_%c%.8s.dat",getenv("WORKDIR"),pLvPack->workDate,chLW,pLvPack->flgNo);
	
	/*fp=fopen(pcFileName,"rb");
	if(fp!=NULL){
		errLog(LOG_ERR,0,RPT_TO_TTY,0,0,"%s,%d,fcopy error,file[%s] exists ",__FILE__,__LINE__,pcFileName);
		fclose(fp);
		return(NULL);
	}del by weizhao 2005-11-30 如果文件已经存在，就覆盖，所以这里不再用这段*/
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
* 功能:   登记处理出错的报文文件名                                    *
* 参数:                                                               *
*       pcFileName-存放返回的文件名                                   *
* 返回值：文件名                                                      *
* 说明：                                                              *
*      登记到文件$WORKDIR/dat/err.dat中                               *
**********************************************************************/
int regerr(char *pcFileName){
        char cmd[256];
        sprintf(cmd,"echo %s >>%s/dat/err.dat",pcFileName,getenv("WORKDIR"));
        system(cmd);
        return(0);
}






