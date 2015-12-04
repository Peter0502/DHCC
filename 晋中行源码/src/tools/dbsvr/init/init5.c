
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,0,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[9];
};
static struct sqlcxp sqlfpn =
{
    8,
    "init5.pc"
};


static unsigned int sqlctx = 18851;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[4];
   unsigned long  sqhstl[4];
            int   sqhsts[4];
            short *sqindv[4];
            int   sqinds[4];
   unsigned long  sqharm[4];
   unsigned long  *sqharc[4];
   unsigned short  sqadto[4];
   unsigned short  sqtdso[4];
} sqlstm = {12,4};

/* SQLLIB Prototypes */
extern sqlcxt (/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlcx2t(/*_ void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * _*/);
extern sqlbuft(/*_ void **, char * _*/);
extern sqlgs2t(/*_ void **, char * _*/);
extern sqlorat(/*_ void **, unsigned int *, void * _*/);

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem(/*_ char *, int * _*/);

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4242,852,0,0,
5,0,0,1,0,0,27,29,0,0,4,4,0,1,0,1,97,0,0,1,97,0,0,1,10,0,0,1,10,0,0,
36,0,0,2,30,0,2,66,0,0,0,0,0,1,0,
51,0,0,3,0,0,29,115,0,0,0,0,0,1,0,
66,0,0,4,0,0,31,128,0,0,0,0,0,1,0,
};


#line 1 "init5.pc"
/******
  参数: 1 新数据库名
        2 起始日期
        3 终止日期
  生成节假日文件 com_parm_holiday
********/
#include <stdio.h>
#include <stdlib.h>
#include "public.h"
#include "svrpub_c.h"
/* EXEC SQL INCLUDE SQLCA;
 */ 
#line 1 "/home/oracle/product/9.2.0/precomp/public/SQLCA.H"
/*
 * $Header: sqlca.h,v 1.3 1994/12/12 19:27:27 jbasu Exp $ sqlca.h 
 */

/* Copyright (c) 1985,1986, 1998 by Oracle Corporation. */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */

#line 12 "init5.pc"

#include "com_parm_holiday_c.h"
int db_connect()
{
    /* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 16 "init5.pc"

        char    sql_username[16];
        char    sql_passwd[16];
    /* EXEC SQL END   DECLARE SECTION; */ 
#line 19 "init5.pc"


    memset(sql_passwd,   0 , sizeof(sql_passwd));
    memset(sql_username, 0 , sizeof(sql_username));

    strcpy(sql_username,getenv("DB_USER"));
    strcpy(sql_passwd,getenv("DB_PASSWD"));

    vtcp_log("%s,%d,USER[%s],PASSWD[%s]\n",__FILE__,__LINE__,sql_username,sql_passwd);

    /* EXEC SQL CONNECT :sql_username identified by :sql_passwd; */ 
#line 29 "init5.pc"

{
#line 29 "init5.pc"
    struct sqlexd sqlstm;
#line 29 "init5.pc"
    sqlstm.sqlvsn = 12;
#line 29 "init5.pc"
    sqlstm.arrsiz = 4;
#line 29 "init5.pc"
    sqlstm.sqladtp = &sqladt;
#line 29 "init5.pc"
    sqlstm.sqltdsp = &sqltds;
#line 29 "init5.pc"
    sqlstm.iters = (unsigned int  )10;
#line 29 "init5.pc"
    sqlstm.offset = (unsigned int  )5;
#line 29 "init5.pc"
    sqlstm.cud = sqlcud0;
#line 29 "init5.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 29 "init5.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 29 "init5.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 29 "init5.pc"
    sqlstm.sqhstv[0] = (unsigned char  *)sql_username;
#line 29 "init5.pc"
    sqlstm.sqhstl[0] = (unsigned long )16;
#line 29 "init5.pc"
    sqlstm.sqhsts[0] = (         int  )16;
#line 29 "init5.pc"
    sqlstm.sqindv[0] = (         short *)0;
#line 29 "init5.pc"
    sqlstm.sqinds[0] = (         int  )0;
#line 29 "init5.pc"
    sqlstm.sqharm[0] = (unsigned long )0;
#line 29 "init5.pc"
    sqlstm.sqadto[0] = (unsigned short )0;
#line 29 "init5.pc"
    sqlstm.sqtdso[0] = (unsigned short )0;
#line 29 "init5.pc"
    sqlstm.sqhstv[1] = (unsigned char  *)sql_passwd;
#line 29 "init5.pc"
    sqlstm.sqhstl[1] = (unsigned long )16;
#line 29 "init5.pc"
    sqlstm.sqhsts[1] = (         int  )16;
#line 29 "init5.pc"
    sqlstm.sqindv[1] = (         short *)0;
#line 29 "init5.pc"
    sqlstm.sqinds[1] = (         int  )0;
#line 29 "init5.pc"
    sqlstm.sqharm[1] = (unsigned long )0;
#line 29 "init5.pc"
    sqlstm.sqadto[1] = (unsigned short )0;
#line 29 "init5.pc"
    sqlstm.sqtdso[1] = (unsigned short )0;
#line 29 "init5.pc"
    sqlstm.sqphsv = sqlstm.sqhstv;
#line 29 "init5.pc"
    sqlstm.sqphsl = sqlstm.sqhstl;
#line 29 "init5.pc"
    sqlstm.sqphss = sqlstm.sqhsts;
#line 29 "init5.pc"
    sqlstm.sqpind = sqlstm.sqindv;
#line 29 "init5.pc"
    sqlstm.sqpins = sqlstm.sqinds;
#line 29 "init5.pc"
    sqlstm.sqparm = sqlstm.sqharm;
#line 29 "init5.pc"
    sqlstm.sqparc = sqlstm.sqharc;
#line 29 "init5.pc"
    sqlstm.sqpadto = sqlstm.sqadto;
#line 29 "init5.pc"
    sqlstm.sqptdso = sqlstm.sqtdso;
#line 29 "init5.pc"
    sqlstm.sqlcmax = (unsigned int )100;
#line 29 "init5.pc"
    sqlstm.sqlcmin = (unsigned int )2;
#line 29 "init5.pc"
    sqlstm.sqlcincr = (unsigned int )1;
#line 29 "init5.pc"
    sqlstm.sqlctimeout = (unsigned int )0;
#line 29 "init5.pc"
    sqlstm.sqlcnowait = (unsigned int )0;
#line 29 "init5.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 29 "init5.pc"
}

#line 29 "init5.pc"

    if(sqlca.sqlcode)
    {
        vtcp_log("%s,%d,SQLCODE=[%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
        return -1;
    }
    return (0);
}

int main(int argc, char *argv[])
{
    int sum=0;
    int num=0;
    int days=0,day1=0;
    int iRet=0;
    char pcResp[5];
    long date1=0,date2=0;
    struct com_parm_holiday_c com_parm_holiday;

    memset(pcResp,'\0',sizeof(pcResp));
    memset(&com_parm_holiday,'\0',sizeof(com_parm_holiday));
    if (argc != 3)
    {
        printf(" 参数错误!! %s + %s + date1 + date2 \n",argv[0],argv[1]);
        return 1;
    }

    date1 = atol(argv[1]);
    date2 = atol(argv[2]);
    db_connect();
    if(sqlca.sqlcode) 
    {
        printf("[%s][%d]database err [%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
        return 1;
    }
    vtcp_log("[%s][%d]date1=[%ld] date2=[%ld]\n",__FILE__,__LINE__,date1,date2);
    sum = pub_base_CalTrueDays(date1,date2);
    /* EXEC SQL DELETE FROM com_parm_holiday; */ 
#line 66 "init5.pc"

{
#line 66 "init5.pc"
    struct sqlexd sqlstm;
#line 66 "init5.pc"
    sqlstm.sqlvsn = 12;
#line 66 "init5.pc"
    sqlstm.arrsiz = 4;
#line 66 "init5.pc"
    sqlstm.sqladtp = &sqladt;
#line 66 "init5.pc"
    sqlstm.sqltdsp = &sqltds;
#line 66 "init5.pc"
    sqlstm.stmt = "delete  from com_parm_holiday ";
#line 66 "init5.pc"
    sqlstm.iters = (unsigned int  )1;
#line 66 "init5.pc"
    sqlstm.offset = (unsigned int  )36;
#line 66 "init5.pc"
    sqlstm.cud = sqlcud0;
#line 66 "init5.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 66 "init5.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 66 "init5.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 66 "init5.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 66 "init5.pc"
}

#line 66 "init5.pc"

    printf("[%s][%d]导入基础数据后处理步骤二>>>>\n",__FILE__,__LINE__);
    while(date2 >= date1)
    {
        num++;
        printf("[%s][%d]INIT2进行中,请稍候...[%2d%%]\r",__FILE__,__LINE__,num*100/sum);
        fflush(stdout);
        days = pub_base_CalTrueDays(19000101,date1);
        if (days < 0)
        {
            printf("[%s][%d]计算天数错误!! [%ld]\n",__FILE__,__LINE__,date1);
            goto ErrExit;
        }
        day1 = days%7 + 1;
        if (day1 == 6)
        {
            com_parm_holiday.beg_date = date1;
            if (pub_base_deadlineD(date1,1,&com_parm_holiday.end_date))
            {
                printf("[%s][%d]计算下一天日期错误!!\n",__FILE__,__LINE__);
                goto ErrExit;
            }
            strncpy(com_parm_holiday.date_code,"SXR",3);
            strncpy(com_parm_holiday.date_name,"双休日",6);
            com_parm_holiday.days = 2;      /**总天数**/
            com_parm_holiday.type[0] = '1'; /**0:未生效1:已生效**/
            vtcp_log("[%s][%] 准备插入数据\n",__FILE__,__LINE__);
            iRet=Com_parm_holiday_Ins(com_parm_holiday,pcResp);
            if (iRet)
            {
                printf("[%s][%d]插入数据错误[%d]\n",__FILE__,__LINE__,iRet);
                Com_parm_holiday_Debug(&com_parm_holiday);
                goto ErrExit;
            }
            if (pub_base_deadlineD(date1,2,&date1))
            {
                printf("[%s][%d]计算下两天日期错误!!\n",__FILE__,__LINE__);
                goto ErrExit;
            }
        }
        else 
        {
            if (pub_base_deadlineD(date1,1,&date1))
            {
                printf("[%s][%d]计算下一天日期错误!!\n",__FILE__,__LINE__);
                goto ErrExit;
            }
        }
    }
    /* EXEC SQL commit work; */ 
#line 115 "init5.pc"

{
#line 115 "init5.pc"
    struct sqlexd sqlstm;
#line 115 "init5.pc"
    sqlstm.sqlvsn = 12;
#line 115 "init5.pc"
    sqlstm.arrsiz = 4;
#line 115 "init5.pc"
    sqlstm.sqladtp = &sqladt;
#line 115 "init5.pc"
    sqlstm.sqltdsp = &sqltds;
#line 115 "init5.pc"
    sqlstm.iters = (unsigned int  )1;
#line 115 "init5.pc"
    sqlstm.offset = (unsigned int  )51;
#line 115 "init5.pc"
    sqlstm.cud = sqlcud0;
#line 115 "init5.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 115 "init5.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 115 "init5.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 115 "init5.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 115 "init5.pc"
}

#line 115 "init5.pc"

    if (sqlca.sqlcode) 
    { 
        printf("[%s][%d]commit db err [%d]\n",__FILE__,__LINE__,sqlca.sqlcode);
        return 1;
    }

    printf("[%s][%d]INIT2进行中,请稍候...[完成]\n",__FILE__,__LINE__);

    return 0;
ErrExit:
    printf("[%s][%d]unnormal quit from init8.ec \n",__FILE__,__LINE__);

    /* EXEC SQL ROLLBACK WORK; */ 
#line 128 "init5.pc"

{
#line 128 "init5.pc"
    struct sqlexd sqlstm;
#line 128 "init5.pc"
    sqlstm.sqlvsn = 12;
#line 128 "init5.pc"
    sqlstm.arrsiz = 4;
#line 128 "init5.pc"
    sqlstm.sqladtp = &sqladt;
#line 128 "init5.pc"
    sqlstm.sqltdsp = &sqltds;
#line 128 "init5.pc"
    sqlstm.iters = (unsigned int  )1;
#line 128 "init5.pc"
    sqlstm.offset = (unsigned int  )66;
#line 128 "init5.pc"
    sqlstm.cud = sqlcud0;
#line 128 "init5.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 128 "init5.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 128 "init5.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 128 "init5.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 128 "init5.pc"
}

#line 128 "init5.pc"

    if(sqlca.sqlcode)
    {
        printf("[%s][%d]database erro!! %d\n",__FILE__,__LINE__,sqlca.sqlcode);
        return 1;
    }

    return 1;
}
