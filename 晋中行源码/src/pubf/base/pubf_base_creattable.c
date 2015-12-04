#include "find_debug.h" 
/*************************************************
* �� �� ��:     pubf_base_creattable.c
* ����������    
* ��    ��:     rob
* ������ڣ�    2003��03��19��
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "doc_dic_c.h"

/**********************************************************************
 * �� �� ����   pub_base_creattable
 * �������ܣ�   �½���
 * ����/ʱ�䣺  rob/20030319
 *
 * ������
 *     ����: 
 *     ���: 
 *     ����: <>0ʧ��
             =0�ɹ�
 *
 * �޸���ʷ��
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

    sprintf(acErrMsg,"������[%]��sql= [%s] ",tabname,sql);
    WRITEMSG
    sprintf(acErrMsg,"��������sql = [%s]",sqlindex);

    ret = sql_execute(sql);
    if (ret != 0)
    {
        sprintf(acErrMsg,"ִ�н���sql����!! [%d]",ret);
        WRITEMSG
        return ret;
    }

    if (sqlindex[0] != '\0')
    {
        ret = sql_execute(sqlindex);
        if (ret != 0)
        {
            sprintf(acErrMsg,"���½�������������!![%d]",ret);
            WRITEMSG
            return ret;
        }
    }
    sprintf(acErrMsg,"�����±�[%s]�ɹ�!!",tabname);
    WRITEMSG
    return 0;
}
