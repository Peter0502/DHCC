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
  int	f_ind ;   /*=-1 ��ʾ��ֵ*/
  int   f_int ;
  double f_double;
  char 	f_char[256] ;
  char  f_foot ;   /*  new ���б�־, ���ڴ�ӡ����ͳ��:C����, S���� */
  double f_footsum ;    /* �������߻��� */
} ty_Field ;

typedef struct 
{
   int  getstru_ed ;	/* ��ȡ�ýṹ��  */
   int  FieldCount ; /*�ֶ��� */
   int  sql_type ; /*����:0����  >0��ѯ  <0���� */
   ty_Field  *Fields;
   int (*Field_dofunc)() ;
} ty_SQLCUR ;

 extern char * btrim(char *) ;   /* ɾ���ұ߿ո�  */
 extern char * ltrim(char *) ;   /* ɾ����߿ո�  */
 extern int strcent(char *, int, char *) ;

#ifndef _SQLTOOL_INCL /* allow multiple inclusions */
  #define _SQLTOOL_INCL
  ty_SQLCUR sqlcur;    /*SQL���ʹ��*/
#else
 extern int  _printf_proc ,_printf_cut, _printf_numltrim  ;  /* ������Ϣ��ʾ��, �Ƿ�ضϣ� �Ƿ�ɾ����ֵ����߿ո�(��wprt_txt�����У�  */
 extern ty_SQLCUR sqlcur;    /*SQL���ʹ��*/
 extern int sql_Gettype(int, char *) ;
  /* ����:SQL���, ��¼��������
     ��ʼSQL���, �����ֶ���, �緵��0��Ϊ��������, ���ظ���,Ϊ����(�����ΪSQLCODE)
     SQL����ֶζ���: field caption, 
        ����caption���Զ��峤��nn, ����: _nnXXXXX 
        	   ���Զ���С��λm,����: _nncmXXXXX, ����cΪ9����D��ʾ��,�ŷָ���ֵ,c����Ϊ�ո�Ͷ��ŵȲ��ɶ���Ϊ�ֶε��ַ�
        	   XXXXX��������пո�Ȳ��ɶ���Ϊ�ֶε��ַ�,Ҫ��[]������,���ǲ��ܺ��ж��� 
        	   c=C����,S���� (���б�־)
   */
 extern int sql_Prep(char *, int (*)() ) ; 
 extern int sql_Close() ;
 extern int sql_Open () ;
 extern int sql_Fetch() ;
 extern int sql_execute() ;
 extern ty_Field * sql_Field( int ) ;
 extern ty_Field * sql_FieldByName( char * ) ;
 extern int sql_FieldCount() ;
 /* �ձ��ʽ�ı���ӡ (sql, nskip, fprt, nr, n0, title, chsp, pg0, func��¼���� )  */
 extern int wprt_txt(char *, int, FILE *, int, int, char *, char , int, int (*)()  ) ;
 /* ���ֱ���ӡ  (sql, nskip, fprt, nr, title, nsp, pg0, pgEnd ,func_fd��¼����, func_newpg��ҳ����, func_btop��ͷ����, func_bbott��β����  )
    SQL���, ����(NOTOP�����ͷ, NOBOTT�����β  NOTOPBOTT����ӡ��ͷ��β) 
    (*func_fd)();               ��¼������
    (*func_btop)(fprt, pg );    ��ͷ��ӡ����(��ӡ��, ��ǰҳ��)
    (*func_newpg)( r, i ) ;  ��ҳ����(��¼��, ��ǰҳ��¼��)
    (*func_bbott)(fprt, pg, r) ;    ��β���� (��ӡ��, ��ǰҳ��, ��¼��)
   */
 extern  int wprt_hzcell(char *, int, FILE *, int, char *, int, int, int, int (*)(), int (*)(), int (*)(), int (*)()  );
 extern char * sql_insertvalues( char * ) ;  /* ����insert values�� */
 
#endif
