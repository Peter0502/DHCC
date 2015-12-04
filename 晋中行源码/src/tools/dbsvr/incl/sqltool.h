# include <stdio.h>
#define sqlfatal()  if (SQLCODE)  return -1 ; 
#define fatal(s) fatal_loc(s, __FILE__, __LINE__)
#define DESC_MAX 99
#define INTWIDTH 8
#define FLOATWIDTH 14
#define FLOATPREC 2
#define DTIMEWIDTH 25
#define INTERVALWIDTH 20

typedef struct 
{ 
  char  f_caption[31];
  char  f_name[21] ;	
  short f_displen ;
  short f_length ;
  short f_dec ;
  short f_type ;  
  int	f_ind ;   /*=-1 表示空值*/
  int   f_int ;
  double f_double;
  char 	f_char[256] ;
  char  f_foot ;   /*  new 底行标志, 用于打印汇总统计:C计数, S汇总 */
  double f_footsum ;    /* 计数或者汇总 */
} ty_Field ;

typedef struct 
{
   int  getstru_ed ;	/* 已取得结构否  */
   int  FieldCount ; /*字段数 */
   int  sql_type ; /*类型:0命令  >0查询  <0错误 */
   ty_Field  *Fields;
   int (*Field_dofunc)() ;
} ty_SQLCUR ;

 extern char * btrim(char *) ;   /* 删除右边空格  */
 extern char * ltrim(char *) ;   /* 删除左边空格  */
 extern int strcent(char *, int, char *) ;

#ifndef _SQLTOOL_INCL /* allow multiple inclusions */
  #define _SQLTOOL_INCL
  ty_SQLCUR sqlcur;    /*SQL访问光标*/
#else
 extern int  _printf_proc ,_printf_cut, _printf_numltrim  ;  /* 调试信息显示否, 是否截断， 是否删除数值型左边空格(在wprt_txt函数中）  */
 extern ty_SQLCUR sqlcur;    /*SQL访问光标*/
 extern int sql_Gettype(int, char *) ;
  /* 参数:SQL语句, 记录触发函数
     初始SQL语句, 返回字段数, 如返回0则为运行命令, 返回负数,为错误(错误号为SQLCODE)
     SQL语句字段定义: field caption, 
        其中caption可以定义长度nn, 形如: _nnXXXXX 
        	   可以定义小数位m,形如: _nncmXXXXX, 其中c为9或者D表示以,号分隔数值,c不能为空格和逗号等不可定义为字段的字符
        	   XXXXX中如果含有空格等不可定义为字段的字符,要用[]括起来,但是不能含有逗号 
        	   c=C计数,S汇总 (底行标志)
   */
 extern int sql_Prep(char *, int (*)() ) ; 
 extern int sql_Close() ;
 extern int sql_Open () ;
 extern int sql_Fetch() ;
 extern int sql_execute() ;
 extern ty_Field * sql_Field( int ) ;
 extern ty_Field * sql_FieldByName( char * ) ;
 extern int sql_FieldCount() ;
 /* 普遍格式文本打印 (sql, nskip, fprt, nr, n0, title, chsp, pg0, func记录触发 )  */
 extern int wprt_txt(char *, int, FILE *, int, int, char *, char , int, int (*)()  ) ;
 /* 汉字表格打印  (sql, nskip, fprt, nr, title, nsp, pg0, pgEnd ,func_fd记录触发, func_newpg换页触发, func_btop表头触发, func_bbott表尾触发  )
    SQL语句, 标题(NOTOP不打表头, NOBOTT不打表尾  NOTOPBOTT不打印表头表尾) 
    (*func_fd)();               记录处理函数
    (*func_btop)(fprt, pg );    表头打印函数(打印流, 当前页号)
    (*func_newpg)( r, i ) ;  换页函数(记录号, 当前页记录行)
    (*func_bbott)(fprt, pg, r) ;    表尾函数 (打印流, 当前页号, 记录号)
   */
 extern  int wprt_hzcell(char *, int, FILE *, int, char *, int, int, int, int (*)(), int (*)(), int (*)(), int (*)()  );
 extern char * sql_insertvalues( char * ) ;  /* 生成insert values串 */
 
#endif
