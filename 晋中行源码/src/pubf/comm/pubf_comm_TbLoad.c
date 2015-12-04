#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_YbLoad.c
* 功能描述：
*              tab_load    将数据文件倒入数据库中
*              tab_unload  将数据由数据库倒入文本文件
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define EXTERN
#include<public.h>
#include <varargs.h>
#include "pubf_data_code.h"

extern int NUM;

int crtdatalist(char *fileline,char *strlist)
{
 register i;
 char *p,*q,str[100];

  MEMSET_DEBUG(strlist,0,sizeof(strlist));
  strlist[0]='(';

  p=fileline;

  while(1) {

     if(*p==NULL) break;

     q=strchr(p,'|');
     if(q==NULL) break;

     *q='\0';

     if(*p!=NULL) sprintf(str,"'%s',",p);
     else sprintf(str,"%s,","NULL");

     strcat(strlist,str);

     p=++q;
  }

vtcp_log("[%s][%d] crt values is [%s]\n",__FILE__,__LINE__,strlist);

  i=strlen(strlist)-1;

  if(i) strlist[i]=')';
  else strlist[i]=0;

  return 0;
}

int tab_load(char *filename,char *tabname,char *fiedlist)
{
    FILE *fp;
    char fileline[1024],strlist[1024],*p;
    char fpath[50],tfname[60];
    int rt;

    strcpy(fpath,getenv("FILDIR"));
    sprintf(tfname,"%s/%s",fpath,filename);

    if ((fp=fopen(tfname,"r"))==NULL) 
    {
        vtcp_log("打开文件错误！");
        return 1;
    }

    while (fgets(fileline,sizeof(fileline),fp)!=NULL) 
    {
        if((p=strchr(fileline,'\n'))!=NULL) *p='\0';
        p=fileline;
        if(*p==NULL) continue;

        crtdatalist(fileline,strlist);
        if(!*strlist) continue;

        rt=insertdata(tabname,fiedlist,strlist);
        if (rt) 
        {
            vtcp_log("装入数据错误！%d",rt);
            return rt;
        }
    }  

    fclose(fp);

    return 0;
}

int tab_unload(char *filename,char *tabname,char *tbindx,char *fiedlist)
{
    FILE *fp;
    char tfname[30],dbname[20],fpath[50],commline[120],name[30],comm[100];
    char tbidx[500],fdlist[300],errlog[300];
    int repcode;

    strcpy(dbname,getenv("DBNAME"));
    strcpy(fpath,getenv("FILDIR"));
    tmpnam(tfname);
    strcat(tfname,".sql");

    set_zd_data(DC_FILE_SND,"1");

    if(tbindx==NULL)
    {
    	strcpy(tbidx,"where 1=1");
    }else
    {
    	strcpy(tbidx,tbindx);
    }

    if(fiedlist==NULL)
    {
    	strcpy(fdlist,"*");
    }else
    {
    	strcpy(fdlist,fiedlist);
    }

    fp = fopen(tfname,"w");
    if (fp==NULL)
    {
        return -1;
    }

    fprintf(fp, "UNLOAD TO \"%s/%s\" SELECT %s FROM %s %s;\n",fpath,filename,fdlist,tabname,tbidx);

    fclose(fp);

    sprintf(commline,"dbaccess %s %s 1>/dev/null 2>&1",dbname,tfname); 

    repcode=system(commline);
    if( repcode )
    {
        remove(tfname);
        vtcp_log(commline);
        sprintf(errlog, "UNLOAD TO \"%s/%s\" SELECT %s FROM %s %s;\n",fpath,filename,fdlist,tabname,tbidx);
        vtcp_log(errlog);
        return (-1);
    }
    remove(tfname);

    return 0;
}
