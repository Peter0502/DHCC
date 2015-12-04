#include "find_debug.h" 
/*************************************************
* 文 件 名:     pubf_base_creattable.c
* 功能描述：    
* 作    者:     rob
* 完成日期：    2003年03月19日
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "doc_dic_c.h"

/**********************************************************************
 * 函 数 名：   pub_base_creattable
 * 函数功能：   新建表
 * 作者/时间：  rob/20030319
 *
 * 参数：
 *     输入: 
 *     输出: 
 *     返回: <>0失败
             =0成功
 *
 * 修改历史：
 *
 ********************************************************************/
int pub_base_creattable(char tabname[30],char *reply)
{
    int ret;
    int len;
    char tmp[100];
    char sql[1000];
    char sqlindex[100];

    struct doc_dic_c doc_dic_c;

    MEMSET_DEBUG(&doc_dic_c, 0x0, sizeof(struct doc_dic_c));
    MEMSET_DEBUG(sql,0x0,sizeof(sql));
    MEMSET_DEBUG(sqlindex, 0x0, sizeof(sqlindex));
    
    ret = Doc_dic_Dec_Sel(reply, "tab= '%s' order by seqn",tabname);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE ERROR!![%d]",ret);
        WRITEMSG
        return ret;
    }

    while(1)
    {
        ret = Doc_dic_Fet_Sel(&doc_dic_c, reply);
        if (ret != 0 && ret != 100)
        {
            sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
            WRITEMSG
            return ret;
        }
        else if (ret == 100)
        {
            len=strlen(sql);
            sql[len-1]=0;
            strcat(sql,");");
            break;

        }

        pub_base_strpack(doc_dic_c.type);
        pub_base_strpack(doc_dic_c.tab);
        pub_base_strpack(doc_dic_c.list);
        
        if (strcmp(doc_dic_c.type,"table") == 0)
        {
            sprintf(sql,"create table %s (",doc_dic_c.tab);
        }

        if (strcmp(doc_dic_c.type,"integer") == 0)
        {
            sprintf(tmp,"%s integer,",doc_dic_c.fied);
            strcat(sql,tmp);
        }
        if (strcmp(doc_dic_c.type,"smallint") == 0)
        {
            sprintf(tmp,"%s smallint,",doc_dic_c.fied);
            strcat(sql,tmp);
        }
        if (strcmp(doc_dic_c.type,"char") == 0)
        {
            sprintf(tmp,"%s char(%d),",doc_dic_c.fied,doc_dic_c.leng);
            strcat(sql,tmp);
        }
        if (strcmp(doc_dic_c.type,"decimal") == 0)
        {
            sprintf(tmp,"%s decimal(%d,%d),",doc_dic_c.fied,
                  doc_dic_c.leng,doc_dic_c.dec);
            strcat(sql,tmp);
        }
        if (strcmp(doc_dic_c.type,"Uindex") == 0)
        {
            sprintf(sqlindex,"creat unique index %s_1 on %s (%s) ",
                   doc_dic_c.tab,doc_dic_c.tab,doc_dic_c.list);
        }
        if (strcmp(doc_dic_c.type,"index") == 0)
        {
            sprintf(sqlindex,"creat index %s_1 on %s (%s) ",
                   doc_dic_c.tab,doc_dic_c.tab,doc_dic_c.list);
        }
    }
    Doc_dic_Clo_Sel();

    sprintf(acErrMsg,"建立表[%]的sql= [%s] ",tabname,sql);
    WRITEMSG
    sprintf(acErrMsg,"建立索引sql = [%s]",sqlindex);

    ret = sql_execute(sql);
    if (ret != 0)
    {
        sprintf(acErrMsg,"执行建表sql错误!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    if (sqlindex[0] != '\0')
    {
        ret = sql_execute(sqlindex);
        if (ret != 0)
        {
            sprintf(acErrMsg,"给新建表建立索引错误!![%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    sprintf(acErrMsg,"建立新表[%s]成功!!",tabname);
    WRITEMSG
    return 0;
}
