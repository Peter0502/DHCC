#include "find_debug.h" 
#include"stdlib.h"
#include"stdio.h"
#include"time.h"
#include"timeb.h"
int ldchar(char *from,int num,char *to){
	int i,j;
	for(i=0,j=0;i<num;i++){
		if(from[i]==' '){
			;
		}else{
			to[j]=from[i];
			j++;
		}
	}
	while(j<=num){
		to[j++]='\0';
	}
	return(0);
}
int rstod(char *str,double *dbval){
	*dbval=atof(str);
	return(0);
}
/****判断给定的年份是否瑞年***/
int rleapyear(int year){
	if(year%4==0 && year%100!=0){
		return(1);
	}
	if(year%400==0){
		return(1);
	}
	return(0);
}
int iGetDateTime(char *pcTimeStr){
	struct tm stTime;
	struct  timeb stTimeB;
  	MEMSET_DEBUG((char *)&stTime ,'\0',sizeof(struct tm));
	MEMSET_DEBUG((char *)&stTimeB,'\0',sizeof(struct  timeb));
	ftime(&stTimeB);
	stTimeB.time = stTimeB.time;

#ifdef AIX
	MEMCPY_DEBUG(&stTime,localtime((time_t *)&(stTimeB.time)),sizeof(struct tm));
#else
	MEMCPY_DEBUG(&stTime,localtime((long *)&(stTimeB.time)),sizeof(struct tm));
#endif
	strftime(pcTimeStr,(size_t)(14 + 1),"%Y%m%d%H%M%S",&stTime);
	/*
	printf("%s,%d,Gethere[%s]\n",__FILE__,__LINE__,pcTimeStr);
	*/
	return(0);
}
