
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
           char  filnam[10];
};
static struct sqlcxp sqlfpn =
{
    9,
    "rpt974.pc"
};


static unsigned int sqlctx = 40139;


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
   unsigned char  *sqhstv[13];
   unsigned long  sqhstl[13];
            int   sqhsts[13];
            short *sqindv[13];
            int   sqinds[13];
   unsigned long  sqharm[13];
   unsigned long  *sqharc[13];
   unsigned short  sqadto[13];
   unsigned short  sqtdso[13];
} sqlstm = {12,13};

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

 static char *sq0002 = 
"select br_no ,prdt_no ,cur_no ,sum(fct_intst) ,sum(intst_tax)  from intst_h\
st where (pt_ind='1' and intst_date=:b0) group by br_no,prdt_no,cur_no order \
by br_no,prdt_no,cur_no            ";

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4242,852,0,0,
5,0,0,1,129,0,4,75,0,0,12,0,0,1,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,
2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,
68,0,0,2,187,0,9,99,0,0,1,1,0,1,0,1,3,0,0,
87,0,0,2,0,0,13,104,0,0,5,0,0,1,0,2,97,0,0,2,97,0,0,2,97,0,0,2,4,0,0,2,4,0,0,
122,0,0,2,0,0,15,140,0,0,0,0,0,1,0,
137,0,0,3,127,0,4,267,0,0,13,1,0,1,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,
0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,1,97,0,0,
};


#line 1 "rpt974.pc"
/*************************************************************
* 文 件 名: rpt974.c
* 功能描述：网点存款结息合计的清单
*
* 作    者: mike
* 完成日期: 2003年09月15日
**************************************************************/
#define ERR_DEAL if( sqlca.sqlcode ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define ERR_DEAL1 if( ret ) {\
		sprintf( acErrMsg, "ret error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "com_sys_parm_c.h"
/* EXEC SQL INCLUDE sqlca.h;
 */ 
#line 1 "/oracle/product/9.2.0.1/precomp/public/sqlca.h"
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

#line 27 "rpt974.pc"
#include "com_branch_c.h"
#include "intst_hst_c.h"

struct com_sys_parm_c	s_com_sys_parm;

int page=0;
int line=0;
int line_file=0;
char vpfm[21];
char t_str[41];
int opncnt;
FILE    *fp; 
int ret = 0;
/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 40 "rpt974.pc"

	struct intst_hst_c	tIntst_hst;
	struct com_branch_c	tCom_branch;
/* EXEC SQL END DECLARE SECTION; */ 
#line 43 "rpt974.pc"



int get_ratio_cxxhqd(int bli,int bll,char str[100]);

rpt974()
{
/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 50 "rpt974.pc"

	long intst_date1=0;
	long intst_date2=0;
/* EXEC SQL END DECLARE SECTION; */ 
#line 53 "rpt974.pc"

	int ttlnum;
	char fname[100];
	char filename[100];

	memset( &s_com_sys_parm , 0x00 , sizeof(struct com_sys_parm_c));
	memset( &tCom_branch , 0x00 , sizeof(struct com_branch_c));
	ret=sql_begin(); /*打开事务*/
	if(ret != 0)
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}
	/**------- 初始化全局变量 --------**/
	pub_base_sysinit();
/*
	strcpy( filename , "BJwdjxqd" );	
	strcpy(vpfm, "wdjxqd" );
*/
	strcpy( filename , "RPT974");
	strcpy(vpfm, "RPT974");
	/* EXEC SQL select rowid,com_branch.* into :tCom_branch from com_branch where br_type='6'; */ 
#line 75 "rpt974.pc"

{
#line 75 "rpt974.pc"
 struct sqlexd sqlstm;
#line 75 "rpt974.pc"
 sqlstm.sqlvsn = 12;
#line 75 "rpt974.pc"
 sqlstm.arrsiz = 12;
#line 75 "rpt974.pc"
 sqlstm.sqladtp = &sqladt;
#line 75 "rpt974.pc"
 sqlstm.sqltdsp = &sqltds;
#line 75 "rpt974.pc"
 sqlstm.stmt = "select rowid ,com_branch.* into :s1 ,:s2 ,:s3 ,:s4 ,:s5 ,:s\
6 ,:s7 ,:s8 ,:s9 ,:s10 ,:s11 ,:s12   from com_branch where br_type='6'";
#line 75 "rpt974.pc"
 sqlstm.iters = (unsigned int  )1;
#line 75 "rpt974.pc"
 sqlstm.offset = (unsigned int  )5;
#line 75 "rpt974.pc"
 sqlstm.selerr = (unsigned short)1;
#line 75 "rpt974.pc"
 sqlstm.cud = sqlcud0;
#line 75 "rpt974.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 75 "rpt974.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 75 "rpt974.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[0] = (unsigned char  *)tCom_branch.rowid;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[0] = (unsigned long )20;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[0] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[0] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[0] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[1] = (unsigned char  *)tCom_branch.br_no;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[1] = (unsigned long )6;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[1] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[1] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[1] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[2] = (unsigned char  *)tCom_branch.br_name;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[2] = (unsigned long )31;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[2] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[2] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[2] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[3] = (unsigned char  *)tCom_branch.wrk_sts;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[3] = (unsigned long )11;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[3] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[3] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[3] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[3] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[3] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[3] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[4] = (unsigned char  *)tCom_branch.br_type;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[4] = (unsigned long )2;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[4] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[4] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[4] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[4] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[4] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[4] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[5] = (unsigned char  *)tCom_branch.ipaddr;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[5] = (unsigned long )16;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[5] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[5] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[5] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[5] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[5] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[5] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[6] = (unsigned char  *)tCom_branch.ind;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[6] = (unsigned long )2;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[6] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[6] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[6] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[6] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[6] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[6] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[7] = (unsigned char  *)&tCom_branch.city_no;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[7] = (unsigned long )sizeof(long);
#line 75 "rpt974.pc"
 sqlstm.sqhsts[7] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[7] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[7] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[7] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[7] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[7] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[8] = (unsigned char  *)tCom_branch.spe_ind;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[8] = (unsigned long )11;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[8] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[8] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[8] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[8] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[8] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[8] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[9] = (unsigned char  *)tCom_branch.up_br_no;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[9] = (unsigned long )6;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[9] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[9] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[9] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[9] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[9] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[9] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[10] = (unsigned char  *)&tCom_branch.tx_type;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[10] = (unsigned long )sizeof(long);
#line 75 "rpt974.pc"
 sqlstm.sqhsts[10] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[10] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[10] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[10] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[10] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[10] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqhstv[11] = (unsigned char  *)tCom_branch.br_tele;
#line 75 "rpt974.pc"
 sqlstm.sqhstl[11] = (unsigned long )21;
#line 75 "rpt974.pc"
 sqlstm.sqhsts[11] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqindv[11] = (         short *)0;
#line 75 "rpt974.pc"
 sqlstm.sqinds[11] = (         int  )0;
#line 75 "rpt974.pc"
 sqlstm.sqharm[11] = (unsigned long )0;
#line 75 "rpt974.pc"
 sqlstm.sqadto[11] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqtdso[11] = (unsigned short )0;
#line 75 "rpt974.pc"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 75 "rpt974.pc"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 75 "rpt974.pc"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 75 "rpt974.pc"
 sqlstm.sqpind = sqlstm.sqindv;
#line 75 "rpt974.pc"
 sqlstm.sqpins = sqlstm.sqinds;
#line 75 "rpt974.pc"
 sqlstm.sqparm = sqlstm.sqharm;
#line 75 "rpt974.pc"
 sqlstm.sqparc = sqlstm.sqharc;
#line 75 "rpt974.pc"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 75 "rpt974.pc"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 75 "rpt974.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 75 "rpt974.pc"
}

#line 75 "rpt974.pc"

	ERR_DEAL

	page=1;
	line_file=1;
	/* 删除原文件 */
	ret = pub_rpt_rmfile(tCom_branch.br_no , filename , 0 );
	ERR_DEAL1
	/* 打开文件 */	
	ret = pub_rpt_openfile( &fp, tCom_branch.br_no , filename );
	ERR_DEAL1
	/* 打印页首 */
	ret = print_head();
	ERR_DEAL1
	
	intst_date2=g_pub_tx.tx_date;
	/* EXEC SQL declare wd_intst_cur cursor for 
		select br_no,prdt_no,cur_no,sum(fct_intst),sum(intst_tax)
		from intst_hst
	where pt_ind='1' and intst_date=:intst_date2 
	group by br_no,prdt_no,cur_no
	order by br_no,prdt_no,cur_no; */ 
#line 96 "rpt974.pc"

	ERR_DEAL

	/* EXEC SQL open wd_intst_cur; */ 
#line 99 "rpt974.pc"

{
#line 99 "rpt974.pc"
 struct sqlexd sqlstm;
#line 99 "rpt974.pc"
 sqlstm.sqlvsn = 12;
#line 99 "rpt974.pc"
 sqlstm.arrsiz = 12;
#line 99 "rpt974.pc"
 sqlstm.sqladtp = &sqladt;
#line 99 "rpt974.pc"
 sqlstm.sqltdsp = &sqltds;
#line 99 "rpt974.pc"
 sqlstm.stmt = sq0002;
#line 99 "rpt974.pc"
 sqlstm.iters = (unsigned int  )1;
#line 99 "rpt974.pc"
 sqlstm.offset = (unsigned int  )68;
#line 99 "rpt974.pc"
 sqlstm.selerr = (unsigned short)1;
#line 99 "rpt974.pc"
 sqlstm.cud = sqlcud0;
#line 99 "rpt974.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 99 "rpt974.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 99 "rpt974.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 99 "rpt974.pc"
 sqlstm.sqcmod = (unsigned int )0;
#line 99 "rpt974.pc"
 sqlstm.sqhstv[0] = (unsigned char  *)&intst_date2;
#line 99 "rpt974.pc"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(long);
#line 99 "rpt974.pc"
 sqlstm.sqhsts[0] = (         int  )0;
#line 99 "rpt974.pc"
 sqlstm.sqindv[0] = (         short *)0;
#line 99 "rpt974.pc"
 sqlstm.sqinds[0] = (         int  )0;
#line 99 "rpt974.pc"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 99 "rpt974.pc"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 99 "rpt974.pc"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 99 "rpt974.pc"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 99 "rpt974.pc"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 99 "rpt974.pc"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 99 "rpt974.pc"
 sqlstm.sqpind = sqlstm.sqindv;
#line 99 "rpt974.pc"
 sqlstm.sqpins = sqlstm.sqinds;
#line 99 "rpt974.pc"
 sqlstm.sqparm = sqlstm.sqharm;
#line 99 "rpt974.pc"
 sqlstm.sqparc = sqlstm.sqharc;
#line 99 "rpt974.pc"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 99 "rpt974.pc"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 99 "rpt974.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 99 "rpt974.pc"
}

#line 99 "rpt974.pc"

	ttlnum=0;
	while(1)
	{
		memset(&tIntst_hst,0x00,sizeof(struct intst_hst_c));
		/* EXEC SQL fetch wd_intst_cur into :tIntst_hst.br_no,:tIntst_hst.prdt_no,:tIntst_hst.cur_no,:tIntst_hst.fct_intst,:tIntst_hst.intst_tax; */ 
#line 104 "rpt974.pc"

{
#line 104 "rpt974.pc"
  struct sqlexd sqlstm;
#line 104 "rpt974.pc"
  sqlstm.sqlvsn = 12;
#line 104 "rpt974.pc"
  sqlstm.arrsiz = 12;
#line 104 "rpt974.pc"
  sqlstm.sqladtp = &sqladt;
#line 104 "rpt974.pc"
  sqlstm.sqltdsp = &sqltds;
#line 104 "rpt974.pc"
  sqlstm.iters = (unsigned int  )1;
#line 104 "rpt974.pc"
  sqlstm.offset = (unsigned int  )87;
#line 104 "rpt974.pc"
  sqlstm.selerr = (unsigned short)1;
#line 104 "rpt974.pc"
  sqlstm.cud = sqlcud0;
#line 104 "rpt974.pc"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 104 "rpt974.pc"
  sqlstm.sqlety = (unsigned short)256;
#line 104 "rpt974.pc"
  sqlstm.occurs = (unsigned int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqfoff = (         int )0;
#line 104 "rpt974.pc"
  sqlstm.sqfmod = (unsigned int )2;
#line 104 "rpt974.pc"
  sqlstm.sqhstv[0] = (unsigned char  *)(tIntst_hst.br_no);
#line 104 "rpt974.pc"
  sqlstm.sqhstl[0] = (unsigned long )6;
#line 104 "rpt974.pc"
  sqlstm.sqhsts[0] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqindv[0] = (         short *)0;
#line 104 "rpt974.pc"
  sqlstm.sqinds[0] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 104 "rpt974.pc"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqhstv[1] = (unsigned char  *)(tIntst_hst.prdt_no);
#line 104 "rpt974.pc"
  sqlstm.sqhstl[1] = (unsigned long )4;
#line 104 "rpt974.pc"
  sqlstm.sqhsts[1] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqindv[1] = (         short *)0;
#line 104 "rpt974.pc"
  sqlstm.sqinds[1] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 104 "rpt974.pc"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqhstv[2] = (unsigned char  *)(tIntst_hst.cur_no);
#line 104 "rpt974.pc"
  sqlstm.sqhstl[2] = (unsigned long )3;
#line 104 "rpt974.pc"
  sqlstm.sqhsts[2] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqindv[2] = (         short *)0;
#line 104 "rpt974.pc"
  sqlstm.sqinds[2] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqharm[2] = (unsigned long )0;
#line 104 "rpt974.pc"
  sqlstm.sqadto[2] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqtdso[2] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqhstv[3] = (unsigned char  *)&(tIntst_hst.fct_intst);
#line 104 "rpt974.pc"
  sqlstm.sqhstl[3] = (unsigned long )sizeof(double);
#line 104 "rpt974.pc"
  sqlstm.sqhsts[3] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqindv[3] = (         short *)0;
#line 104 "rpt974.pc"
  sqlstm.sqinds[3] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqharm[3] = (unsigned long )0;
#line 104 "rpt974.pc"
  sqlstm.sqadto[3] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqtdso[3] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqhstv[4] = (unsigned char  *)&(tIntst_hst.intst_tax);
#line 104 "rpt974.pc"
  sqlstm.sqhstl[4] = (unsigned long )sizeof(double);
#line 104 "rpt974.pc"
  sqlstm.sqhsts[4] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqindv[4] = (         short *)0;
#line 104 "rpt974.pc"
  sqlstm.sqinds[4] = (         int  )0;
#line 104 "rpt974.pc"
  sqlstm.sqharm[4] = (unsigned long )0;
#line 104 "rpt974.pc"
  sqlstm.sqadto[4] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqtdso[4] = (unsigned short )0;
#line 104 "rpt974.pc"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 104 "rpt974.pc"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 104 "rpt974.pc"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 104 "rpt974.pc"
  sqlstm.sqpind = sqlstm.sqindv;
#line 104 "rpt974.pc"
  sqlstm.sqpins = sqlstm.sqinds;
#line 104 "rpt974.pc"
  sqlstm.sqparm = sqlstm.sqharm;
#line 104 "rpt974.pc"
  sqlstm.sqparc = sqlstm.sqharc;
#line 104 "rpt974.pc"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 104 "rpt974.pc"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 104 "rpt974.pc"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 104 "rpt974.pc"
}

#line 104 "rpt974.pc"

		if( sqlca.sqlcode==1403 )
		{
			if( ttlnum )
			{
				/* 打页尾 */
				ret = print_tail();
				ERR_DEAL1
				fclose(fp);
			}
			break;
		}
		else if( sqlca.sqlcode )
		{
			sprintf(acErrMsg,"读取结息表异常出错!");
			WRITEMSG			
			goto ErrExit;			
		}
		
    	/* 打印信息 */
		ret = print_body();
		ERR_DEAL1
		line_file++;
		if( line_file>50 )	/* 大于50行换页 */
		{
			line_file=0;
			page++;
			/* 打印页尾 */
			ret = print_tail();
			ERR_DEAL1
			/* 打印页首 */
			ret = print_head();
			ERR_DEAL1
		}
		ttlnum++;
	}		
	/* EXEC SQL close wd_intst_cur; */ 
#line 140 "rpt974.pc"

{
#line 140 "rpt974.pc"
 struct sqlexd sqlstm;
#line 140 "rpt974.pc"
 sqlstm.sqlvsn = 12;
#line 140 "rpt974.pc"
 sqlstm.arrsiz = 12;
#line 140 "rpt974.pc"
 sqlstm.sqladtp = &sqladt;
#line 140 "rpt974.pc"
 sqlstm.sqltdsp = &sqltds;
#line 140 "rpt974.pc"
 sqlstm.iters = (unsigned int  )1;
#line 140 "rpt974.pc"
 sqlstm.offset = (unsigned int  )122;
#line 140 "rpt974.pc"
 sqlstm.cud = sqlcud0;
#line 140 "rpt974.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 140 "rpt974.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 140 "rpt974.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 140 "rpt974.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 140 "rpt974.pc"
}

#line 140 "rpt974.pc"

	
OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成网点计息交易清单成功!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"生成网点计息清单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{	
	ret=pub_rpt_read_pfm_qd(fp,&line,vpfm,"0001",vrec,&opncnt,get_ratio_cxxhqd);
	ERR_DEAL1
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表首 */
int print_head( )
{
	char vrec[3];
	
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL1
	
	strcpy( vrec,"AA" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL1
		
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表尾 */
int print_tail( )
{
	char vrec[3];

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL1
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 打印表体 */
int print_body( )
{
	char vrec[3];

	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL1
	
GoodExit:
	return 0;
ErrExit:
	return 1;
}


/* 赋值函数 */
int get_ratio_cxxhqd( bli,bll,str )
int bli,bll;
char str[100];
{ 
/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 226 "rpt974.pc"

	struct com_branch_c	vCom_branch;
/* EXEC SQL END DECLARE SECTION; */ 
#line 228 "rpt974.pc"

	char vhm[101]; 
	char vstr[101]; 
	char l_kmm[31];
	char amt_tmp[21]; 
	char tmp_inf[21];
	char fmt[10]; 
	int i=0; 
	long tmpdate=0;
	char tmpname[51];
	char tmpac_no[51];
	
	switch( bli ) 
	{ 
		case 'A': /* 交易日期 */
			pub_base_deadlineD(g_pub_tx.tx_date,-1,&tmpdate);
			sprintf( t_str , "%4d年%2d月%2d日" , tmpdate/10000 , tmpdate%10000/100 , tmpdate%100 );
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , t_str ); 
			break;
		case 'B': /* 机构编码 */
			sprintf(fmt,"%%-%ds",bll); 
			sprintf( str , fmt , tCom_branch.br_no ); 
			break; 
		case 'C': /* 机构名称 */
			pub_base_strpack( tCom_branch.br_name ); 
			sprintf( t_str , "%s    ( %5s )" , tCom_branch.br_name , tCom_branch.br_no); 
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		case 'D': /* 页码 */
			sprintf(fmt,"%%%dld",bll); 
			sprintf(str, fmt, page); 
			break; 
		case 'E': /*机构编码*/
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , tIntst_hst.br_no); 
			break; 
		case 'M': /*机构名称*/
			/* EXEC SQL select rowid,com_branch.* into :vCom_branch from com_branch
			where br_no=:tIntst_hst.br_no; */ 
#line 268 "rpt974.pc"

{
#line 267 "rpt974.pc"
   struct sqlexd sqlstm;
#line 267 "rpt974.pc"
   sqlstm.sqlvsn = 12;
#line 267 "rpt974.pc"
   sqlstm.arrsiz = 13;
#line 267 "rpt974.pc"
   sqlstm.sqladtp = &sqladt;
#line 267 "rpt974.pc"
   sqlstm.sqltdsp = &sqltds;
#line 267 "rpt974.pc"
   sqlstm.stmt = "select rowid ,com_branch.* into :s1 ,:s2 ,:s3 ,:s4 ,:s5 ,\
:s6 ,:s7 ,:s8 ,:s9 ,:s10 ,:s11 ,:s12   from com_branch where br_no=:b1";
#line 267 "rpt974.pc"
   sqlstm.iters = (unsigned int  )1;
#line 267 "rpt974.pc"
   sqlstm.offset = (unsigned int  )137;
#line 267 "rpt974.pc"
   sqlstm.selerr = (unsigned short)1;
#line 267 "rpt974.pc"
   sqlstm.cud = sqlcud0;
#line 267 "rpt974.pc"
   sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 267 "rpt974.pc"
   sqlstm.sqlety = (unsigned short)256;
#line 267 "rpt974.pc"
   sqlstm.occurs = (unsigned int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[0] = (unsigned char  *)vCom_branch.rowid;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[0] = (unsigned long )20;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[0] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[0] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[0] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[0] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[0] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[0] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[1] = (unsigned char  *)vCom_branch.br_no;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[1] = (unsigned long )6;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[1] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[1] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[1] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[1] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[1] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[1] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[2] = (unsigned char  *)vCom_branch.br_name;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[2] = (unsigned long )31;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[2] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[2] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[2] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[2] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[2] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[2] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[3] = (unsigned char  *)vCom_branch.wrk_sts;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[3] = (unsigned long )11;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[3] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[3] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[3] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[3] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[3] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[3] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[4] = (unsigned char  *)vCom_branch.br_type;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[4] = (unsigned long )2;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[4] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[4] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[4] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[4] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[4] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[4] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[5] = (unsigned char  *)vCom_branch.ipaddr;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[5] = (unsigned long )16;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[5] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[5] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[5] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[5] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[5] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[5] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[6] = (unsigned char  *)vCom_branch.ind;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[6] = (unsigned long )2;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[6] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[6] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[6] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[6] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[6] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[6] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[7] = (unsigned char  *)&vCom_branch.city_no;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[7] = (unsigned long )sizeof(long);
#line 267 "rpt974.pc"
   sqlstm.sqhsts[7] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[7] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[7] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[7] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[7] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[7] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[8] = (unsigned char  *)vCom_branch.spe_ind;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[8] = (unsigned long )11;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[8] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[8] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[8] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[8] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[8] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[8] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[9] = (unsigned char  *)vCom_branch.up_br_no;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[9] = (unsigned long )6;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[9] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[9] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[9] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[9] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[9] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[9] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[10] = (unsigned char  *)&vCom_branch.tx_type;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[10] = (unsigned long )sizeof(long);
#line 267 "rpt974.pc"
   sqlstm.sqhsts[10] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[10] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[10] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[10] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[10] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[10] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[11] = (unsigned char  *)vCom_branch.br_tele;
#line 267 "rpt974.pc"
   sqlstm.sqhstl[11] = (unsigned long )21;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[11] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[11] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[11] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[11] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[11] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[11] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqhstv[12] = (unsigned char  *)(tIntst_hst.br_no);
#line 267 "rpt974.pc"
   sqlstm.sqhstl[12] = (unsigned long )6;
#line 267 "rpt974.pc"
   sqlstm.sqhsts[12] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqindv[12] = (         short *)0;
#line 267 "rpt974.pc"
   sqlstm.sqinds[12] = (         int  )0;
#line 267 "rpt974.pc"
   sqlstm.sqharm[12] = (unsigned long )0;
#line 267 "rpt974.pc"
   sqlstm.sqadto[12] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqtdso[12] = (unsigned short )0;
#line 267 "rpt974.pc"
   sqlstm.sqphsv = sqlstm.sqhstv;
#line 267 "rpt974.pc"
   sqlstm.sqphsl = sqlstm.sqhstl;
#line 267 "rpt974.pc"
   sqlstm.sqphss = sqlstm.sqhsts;
#line 267 "rpt974.pc"
   sqlstm.sqpind = sqlstm.sqindv;
#line 267 "rpt974.pc"
   sqlstm.sqpins = sqlstm.sqinds;
#line 267 "rpt974.pc"
   sqlstm.sqparm = sqlstm.sqharm;
#line 267 "rpt974.pc"
   sqlstm.sqparc = sqlstm.sqharc;
#line 267 "rpt974.pc"
   sqlstm.sqpadto = sqlstm.sqadto;
#line 267 "rpt974.pc"
   sqlstm.sqptdso = sqlstm.sqtdso;
#line 267 "rpt974.pc"
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 267 "rpt974.pc"
}

#line 268 "rpt974.pc"

			if( sqlca.sqlcode )
			{
				sprintf(acErrMsg, "查询公共机构编码表错误! [%d]", ret);
				WRITEMSG
				goto ErrExit;
			}
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt , vCom_branch.br_name); 
			break; 
		case 'H': /* 产品 */
			sprintf(fmt, "%%-%ds", bll);
			sprintf( str, fmt , tIntst_hst.prdt_no);
			break;
		case 'J': /* 利息 */
			sprintf( amt_tmp, "%.2lf" , tIntst_hst.fct_intst ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		case 'F': /* 利息税 */
			sprintf( amt_tmp, "%.2lf" , tIntst_hst.intst_tax ); 
			pub_rpt_flttomon( amt_tmp,amt_tmp );
							
			sprintf(fmt,"%%%ds",bll); 		
			sprintf( str, fmt, amt_tmp ); 
			break;
		default : 
			memset( str,' ',bll ); 
			break; 
	} 
	
GoodExit: 
	return 0; 
ErrExit: 
	return 1; 
}
