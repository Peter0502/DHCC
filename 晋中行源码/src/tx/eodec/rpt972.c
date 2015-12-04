
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
    "rpt972.pc"
};


static unsigned int sqlctx = 40123;


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
   unsigned char  *sqhstv[1];
   unsigned long  sqhstl[1];
            int   sqhsts[1];
            short *sqindv[1];
            int   sqinds[1];
   unsigned long  sqharm[1];
   unsigned long  *sqharc[1];
   unsigned short  sqadto[1];
   unsigned short  sqtdso[1];
} sqlstm = {12,1};

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
};


#line 1 "rpt972.pc"
/*************************************************************
* �� �� ��: rpt972.pc
* �����������ڲ��˻�������(����in_mst�������ݣ�����lst_date < ����(20130101)Ϊ������)
*
* ��    ��: zgf
* �������: 2013��4��16��
*
* ע����ʽ�ļ�ΪRPT972.tbl
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define extern
#define ERR_DEAL if(ret) {\
	sprintf(acErrMsg,"���ݿ����!"); \
	WRITEMSG \
	goto ErrExit; \
	}
#include "public.h"
#include "card.h"
#include "com_sys_parm_c.h"
#include "com_item_c.h"
#include "acno_hst_log_c.h"
#include "com_branch_c.h"
#include "note_parm_c.h"
#include "dd_mst_c.h"
#include "in_mst_c.h"
#include "ln_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "acc_ac_id_c.h"

/*#define PAGE_CNT 57*/
#define PAGE_CNT 55
#define NOT_FULL_DATE 1231
int printTail=0;

static long page,total;
static struct 	com_sys_parm_c 		s_com_sys_parm;
static struct	acno_hst_log_c 	     	s_acno_hst_log;
static struct	acno_hst_log_c 	     	v_acno_hst_log;
static struct	acno_hst_log_c 	     	m_acno_hst_log;	/**���ĳ���ʺ����һ����ҳ�ļ�¼����������¼**/
static struct  	mdm_ac_rel_c	     	s_mdm_ac_rel;
static struct  	dd_mst_c	     	s_dd_mst;
static struct  	in_mst_c             	s_in_mst;
static struct  	ln_mst_c             	s_ln_mst;
static struct  	acc_ac_id_c             	s_acc_ac_id;

char 	pre_br_no[6];
char 	pre_acc_hrt[6];
char   	filename[100];
char   	filename_not_full[100];
long   	tmp_date;
long	pre_tx_date;
long	lst_tx_date;
double 	cr_sum_amt;
double 	m_cr_sum_amt;
double	dr_sum_amt;
double	m_dr_sum_amt;
double	ye_sum_amt;
int 	opncnt;
FILE 	*fp;
FILE 	*fp_not_full;
char 	vjgbm[6];
char 	t_str[41];
int   ptail = 0 ;    /***δ��ҳ�ĵط���β�������ҳ��β***/
int	ret = 0;
int 	flag = 0;							/*** 0- �� 1-��� 2-���� 3-�ڲ� ***/
int 	tag = 0;
long 	line_file=0;
char 	vpfm[21];
int	l_hst_cnt = 0;	/*��ǰ��ϸ���ж���������ӡ�ļ�¼**/
int 	line;
long	pre_ac_id = 0;
int   ftpg = 1;   /* ������һҳ */
int   nxt  = 0;  /* �����Ƿ�ֻ��ӡ�ڲ� */

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt972()
{
	vtcp_log("[%s][%d]��ʼ�����ڲ��ʻ���������ҳ����!\n",__FILE__,__LINE__);
	printf("[%s][%d]��ʼ�����ڲ��ʻ���������ҳ����!\n",__FILE__,__LINE__);
	int    	ttlnum;
	long 	  recordnum = 0;
	long  	recordnum_notfull = 0; 	/* add by LiuYue 2007-1-30 21:47 */

	memset(pre_br_no,0x00,sizeof(pre_br_no));
	memset(pre_acc_hrt,0x00,sizeof(pre_acc_hrt));

	strcpy(pre_br_no,"-----");
	pre_ac_id = 999999;

	ret = Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"1=1");
	ERR_DEAL

	g_pub_tx.tx_date = s_com_sys_parm.lst_date;
	tmp_date = s_com_sys_parm.lst_date/10000*10000+101;/**��ȡ�������ֵ**/

	strcpy( filename , "RPT972");
	strcpy( filename_not_full,"RPT972_NOT_FULL");
	strcpy( vpfm, "RPT972");
	pub_rpt_rmfile( "",filename,0 );
	pub_rpt_rmfile( "",filename_not_full,0 );
printf("111111111111111111\n"); 
	ret = Mdm_ln_mst_Dec_Upd(g_pub_tx.reply,"lst_date>=%ld ",tmp_date);
	ERR_DEAL
printf("[%s][%d]����[%s]!\n",__FILE__,__LINE__,tmp_date);
	while (1)
	{

		ret = Mdm_ac_rel_acno_hst_Fet_Upd(&s_acno_hst_log,&s_mdm_ac_rel,g_pub_tx.reply);
		if (ret==100)
			{
				if( ttlnum )
			    {
			    	fclose(fp);
			    }
				break;
			}
		else if (ret)	ERR_DEAL


		lst_tx_date = pre_tx_date;		/*��ӡҳβ��ʱ�������ĳ��Ŀ���һҳ���������**/
		pre_tx_date = s_acno_hst_log.tx_date;	/*��ӡҳĩβʱ����Ҫ�õ�һ������***/

		/****��������ű仯����Ҫ���¿������ļ�***/
		if ( strncmp(pre_br_no,s_acno_hst_log.br_no,5) )
		{
			if ( strncmp(pre_br_no,"-----",5) )
			{
				/***���ﲻ��Ҫ��ҳ��
				print_HY_not_full();
				print_HY();
				*****/

				fclose(fp);
				/*fclose(fp_not_full);notfull*/

				recordnum =0; /**�����־ҳ����Ϊ0  һ�����˻����ͱ������ȴ�ӡ��β��***/
				line = 0;
			}
			ret = pub_rpt_openfile( &fp,s_acno_hst_log.br_no,filename );
			ERR_DEAL
			/*ret = pub_rpt_openfile( &fp_not_full,s_acno_hst_log.br_no,filename_not_full );
			ERR_DEAL notfull*/
			ftpg = 1;
		}
		strcpy( pre_br_no,s_acno_hst_log.br_no );

		/***����ʺű仯��**/
		if (pre_ac_id!=s_acno_hst_log.ac_id)
		{
			l_hst_cnt =0;
			page = 0;
			/***��ȡ��ǰ�ʺŴ�ӡ���˶���ҳ��***/
			ret = sql_max_long_xier("acno_hst_log","pagecnt",&page,"ac_id=%ld and print_flag='3'",s_acno_hst_log.ac_id);
			ERR_DEAL

			vtcp_log("[%s][%d]��ǰ�ʺ�[%ld]�Ѿ���ӡ��ҳ��==[%ld]\n",__FILE__,__LINE__,s_acno_hst_log .ac_id,page);

			/* ���������ϲ�ѯ�������
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld",s_acno_hst_log.ac_id);
			ERR_DEAL
			*/

			if (s_acno_hst_log.ac_type[0]=='1')/**����**/
			{
				/****��Ϊ��ʱ��Ҳ����ӡ��ҳ�� 20101227  ****/
				if(s_mdm_ac_rel.ac_no[0]=='1' || memcmp(s_mdm_ac_rel.ac_no,CARDHEADCODE,6) == 0 || memcmp(s_mdm_ac_rel.ac_no,ICCARDHEADCODE,6) == 0) /***�����ʻ���ͷΪ1�Ĳ���ǹ����**/
					{
						continue;	/***������ӡ�����ҳʱע�����***/
					}
				ret = Dd_mst_Sel(g_pub_tx.reply,&s_dd_mst,"ac_id=%ld",s_acno_hst_log.ac_id);
				ERR_DEAL

				nxt = 1;
			}
			else if (s_acno_hst_log.ac_type[0]=='3')/**�ڲ���***/
			{
				ret = In_mst_Sel(g_pub_tx.reply,&s_in_mst,"ac_id=%ld",s_acno_hst_log.ac_id);
				ERR_DEAL

				if(nxt == 1)
					{
						ftpg += 1;
					}
					nxt = 1;
			}
			else if (s_acno_hst_log.ac_type[0]=='2')/**����***/
			{
				ret = Ln_mst_Sel(g_pub_tx.reply,&s_ln_mst,"ac_id=%ld",s_acno_hst_log.ac_id);
				ERR_DEAL

				if(nxt == 1)
					{
						ftpg += 1;
					}
			}
			/**������ǵ�һ�εĻ�***/

			if (pre_ac_id!=999999)
			{
				if (recordnum)
				{
					memcpy(&v_acno_hst_log,&m_acno_hst_log,sizeof(v_acno_hst_log));
					cr_sum_amt = m_cr_sum_amt;
					dr_sum_amt = m_dr_sum_amt;
					if( ptail==0 )
						{
						  ret = print_tail();
					    ERR_DEAL
						  print_HY();
					    ERR_DEAL
						}
					ptail = 0;
				}
			}

			recordnum =0;
			recordnum_notfull  = 0;/* add by LiuYue 2007-1-30 21:48*/
			line_file = 0;

			/***��ȡ���ÿ�Ŀ��Ҫ��ӡ�ļ�¼����**/

			l_hst_cnt = sql_count("acno_hst_log","print_flag='0' and br_no='%s' and ac_id='%ld'",
				s_acno_hst_log.br_no,s_acno_hst_log.ac_id);
			vtcp_log("[%s][%d]����[%s]�ʺ�[%ld]����ӡ�ļ�¼����Ϊ[%d]\n",__FILE__,__LINE__,s_acno_hst_log.br_no,s_acno_hst_log.ac_id,
				l_hst_cnt);

			printf("[%s][%d]����[%s]�ʺ�[%ld]����ӡ�ļ�¼����Ϊ[%d]\n",__FILE__,__LINE__,s_acno_hst_log.br_no,s_acno_hst_log.ac_id,
				l_hst_cnt);

			if (l_hst_cnt+7<PAGE_CNT)
			{
				/***�������ֻ��һ�ʼ�¼�������v_acno_hst_log û��ֵ***/
				memcpy(&v_acno_hst_log,&s_acno_hst_log,sizeof(v_acno_hst_log));
				/**if (g_pub_tx.tx_date%10000 ==NOT_FULL_DATE && l_hst_cnt>0) del by martin**/
				if ( l_hst_cnt>0 )
				{

					printf("[%s][%d]line_file===[%d],l_hst_cnt===[%d]\n",__FILE__,__LINE__,line_file,l_hst_cnt);
					/* ��ӡҳ�� */
					/*page = 1;*/
					page = page +1;

					ret = print_head_not_full(4);
					ERR_DEAL

						line = 0;

					ret = print_body_not_full( );
					ERR_DEAL


					recordnum_notfull++;

					/***����ӡ�����һ����¼�Ĵ�ӡ��־����Ϊ3****/
					if (recordnum_notfull != l_hst_cnt)
						s_acno_hst_log.print_flag[0]='1';
					else
						s_acno_hst_log.print_flag[0]='3';
					s_acno_hst_log.pagecnt = page;
					/*****���ú��˸����ֶ�***********************/

					ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
					ERR_DEAL
						vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־�ɹ�!\n",__FILE__,__LINE__);

					strcpy( pre_br_no,s_acno_hst_log.br_no );
					ttlnum++;

					printf("[%s][%d]���ǽ�������һ������Ҫ��ӡ��δ��ҳ�ļ�¼!\n",__FILE__,__LINE__);
					printf("[%s][%d]recordnum_notfull=[%d],l_hst_cnt=[%d]!\n",__FILE__,__LINE__,recordnum_notfull,l_hst_cnt);
					/* rem by LiuYue 2007-1-30 21:49
					if(line_file == l_hst_cnt+8) ��Ϊ�����line_file�����˱��ⲿ��*
					**************replace by next line **********************************/
					if (recordnum_notfull == l_hst_cnt) /*��Ϊ�����line_file�����˱��ⲿ��**/
					{
						printf("[%s][%d]my xier,�ܼ�¼��������ҳ���������ӲҪ��ӡ������ӡβ��ʱ����!\n",__FILE__,__LINE__);
						ret = print_tail_not_full();
						ERR_DEAL
							line_file += 3; /**��תһ��  �������ռ2��***/
					}
					pre_ac_id = s_acno_hst_log.ac_id;
					strcpy( vjgbm,s_acno_hst_log.br_no );
				}
				continue;
			}
			page += 1;
			pre_ac_id = s_acno_hst_log.ac_id;
			strcpy( vjgbm,s_acno_hst_log.br_no );

			/* ��ӡҳ�� */
			ret = print_head(4);

		}

		memcpy(&v_acno_hst_log,&s_acno_hst_log,sizeof(v_acno_hst_log));


		if (l_hst_cnt+7<PAGE_CNT)
		{
			/**if (g_pub_tx.tx_date%10000 ==NOT_FULL_DATE && l_hst_cnt>0)**/
				if ( l_hst_cnt>0 )
			{
				if (line_file==0 && l_hst_cnt>0)	/*�����ڱ�־ FLAG_SEE�� û�н��б�ͷ�Ĵ�ӡ�����Էŵ���������ӡ**/
				{
					/* ��ӡҳ�� */
					ret = print_head_not_full(4);
					ERR_DEAL
						line = 0;
				}
				ret = print_body_not_full( );
				ERR_DEAL

				recordnum_notfull ++;/* add by LiuYue 2007-1-30 21:50 */
				vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־!\n",__FILE__,__LINE__);

				/***����ӡ�����һ����¼�Ĵ�ӡ��־����Ϊ3****/
				if (recordnum_notfull != l_hst_cnt)
					s_acno_hst_log.print_flag[0]='1';
				else
					s_acno_hst_log.print_flag[0]='3';
				s_acno_hst_log.pagecnt = page;
				/*****���ú��˸����ֶ�***********************/

				ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
				ERR_DEAL
					vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־�ɹ�!\n",__FILE__,__LINE__);

				pre_ac_id = s_acno_hst_log.ac_id;
				ttlnum++;

				/* rem by LiuYue 2007-1-30 21:51
				if(line_file == l_hst_cnt+8) *��Ϊ�����line_file�����˱��ⲿ��*
				***** replace by next line  2007-1-30 21:51 ********************/
				if (recordnum_notfull == l_hst_cnt)  /*��Ϊ�����line_file�����˱��ⲿ��**/
				{
					printf("[%s][%d]my xier,�ܼ�¼��������ҳ���������ӲҪ��ӡ������ӡβ��ʱ����!\n",__FILE__,__LINE__);
					ret = print_tail_not_full();
					ERR_DEAL
						line_file += 3; /**��תһ��  �������ռ2��***/
				}
			}
			continue;
		}
		recordnum++; /**ĳ���ʺŵ���ϸ���ӱ�־**/
		/* ��ӡ�ֻ���Ϣ */
		/* rem by LiuYue 2007-1-30 22:38
		if(l_hst_cnt>PAGE_CNT && (recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7) > 0))*�����ǰ����ӡ��hst_cnt-��ǰhst_cnt<��ҳ����,�򲻴�ӡ**
		*******************replace by next line *********************************/
		if (l_hst_cnt>=(PAGE_CNT-7) && (recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7) > 0))/*�����ǰ����ӡ��hst_cnt-��ǰhst_cnt<��ҳ����,�򲻴�ӡ**/
		{
			ttlnum++;
			/****���������һ����ҳ����һ�ʼ�¼����������ӡδ��ҳ��ʱ�����һ����ҳ�ʵĽ�ת����ӡ�Ĳ�����һ�ʣ��������һ�ʼ�¼��
			����  ����132����¼����ת����õ��ǵ�132 ������100****/
			if ((recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7))==1)
				memcpy(&m_acno_hst_log,&s_acno_hst_log,sizeof(m_acno_hst_log));


			/*if (g_pub_tx.tx_date%10000 ==NOT_FULL_DATE && l_hst_cnt>0)del by martin*/
				if ( l_hst_cnt>0 )
			{
				if ((recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7))==1)
				{
					/*��ӡ��ҳ�ı�β 0619*/
					if( pre_ac_id==s_acno_hst_log.ac_id )
						{
							ret = print_tail();
							ERR_DEAL
						  print_HY();
					    ERR_DEAL
							ptail=1;
						}
					printf("[%s][%d]���ǽ�������һ������Ҫ��ӡ��δ��ҳ�ļ�¼!\n",__FILE__,__LINE__);
					/* ��ӡҳ�� */
					line = 0;
					page = page +1;
					/*
					ret = print_head_not_full(4);
					ERR_DEAL*/
					if( page==1 )/*�ж��Ƿ������δ��ҳ��ҳ 070701*/
					    {
					    	ret = print_head_not_full(4);
					    	ERR_DEAL
					    }else
					    {
					    	ret = print_head_not_full(1);
					    	ERR_DEAL
					    }

				}
				ret = print_body_not_full( );
				ERR_DEAL

					vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־!\n",__FILE__,__LINE__);

				if (recordnum !=l_hst_cnt)
					s_acno_hst_log.print_flag[0]='1';
				else
					s_acno_hst_log.print_flag[0]='3';

				s_acno_hst_log.pagecnt = page;

				ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
				ERR_DEAL

					vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־�ɹ�!\n",__FILE__,__LINE__);

				line_file ++;

				pre_ac_id = s_acno_hst_log.ac_id;

				ttlnum++;

				if (recordnum ==l_hst_cnt)
				{
					printf("[%s][%d]�ϴ��˼�¼�����һ���˸ô�ӡβ����!\n",__FILE__,__LINE__);
					ret = print_tail_not_full();
					ERR_DEAL
				}
			}
			continue;

		}
		pre_ac_id = s_acno_hst_log.ac_id;
		ttlnum++;

		ret = print_body( );
		ERR_DEAL

			vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־!\n",__FILE__,__LINE__);
		if(recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7)==0)
		{
			m_cr_sum_amt = cr_sum_amt;
			m_dr_sum_amt = dr_sum_amt;
		}
		/**if ((g_pub_tx.tx_date%10000 !=NOT_FULL_DATE && (recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7)==0)) ||
			recordnum==l_hst_cnt)del by martin ������첻��ӡ����ҳ�ʣ���ô��Ҫ����ҳ�����ñ�־3  �����¼������������ҳ����Ҫ���ñ�־3***/
		if ( recordnum==l_hst_cnt )
		{
			s_acno_hst_log.print_flag[0]='3';
			memcpy(&m_acno_hst_log,&s_acno_hst_log,sizeof(m_acno_hst_log));


		}
		else
			s_acno_hst_log.print_flag[0]='1';
		s_acno_hst_log.pagecnt = page;

		ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
		ERR_DEAL
		vtcp_log("[%s][%d]��ʼ���¼�¼�Ĵ�ӡ��־�ɹ�!\n",__FILE__,__LINE__);

	}
	vtcp_log("[%s][%d]�ύ���ݿ�!\n",__FILE__,__LINE__);
	sql_commit();   /*--�ύ����--*/
	vtcp_log("����[%s]���\n",g_brno_tmp.br_no);
	/******************�������************************/
	/*
	* ��������
	*/
	/** if (g_pub_tx.tx_date%10000 !=NOT_FULL_DATE)	������첻��ӡδ��ҳ�ʣ�ɾ����δ��ҳ�ļ�
		pub_rpt_rmfile( "",filename_not_full,0 );del by martin **/

	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
		vtcp_log("Before OK return: reply is [%s]\n",g_pub_tx.reply);
	return (0);
	/*
	* �������
	*/
	ErrExit:
		strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
		vtcp_log("Before bad return: reply is [%s]\n",g_pub_tx.reply);
	return (1);

}

/* �м亯�� */
int Make_yeb_sub( char vrec[3] )
{
	ret=pub_rpt_read_pfm_qd(fp,&line_file,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL

		GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ���� */
int print_head(int m_tag )
{
	if (ftpg != 1)
	{
		fprintf(fp, "\f");
	}
	char vrec[3];

	cr_sum_amt = 0.0;
	dr_sum_amt = 0.0;
	ye_sum_amt = 0.0;
/**
	if ( line+7>PAGE_CNT )
	{
		print_tail();
		print_HY();
		line=0;
	}
***/
	if (ftpg == 1)
	{
		fprintf(fp,"%c%c",0x1b,0x40);
	}
	fprintf(fp,"\n");
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

		line+=6;
		ftpg+=1;

	if (m_tag>2)
		tag = m_tag;
	else
		tag = 1;
	printf("[%s][%d]��ӡ��ת��־ =[%d]\n",__FILE__,__LINE__,tag);
	ret = Make_yeb_sub("DD");
	ERR_DEAL
		tag = 0;
	line+=1;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ���� */
int print_body_A( )
{
	char vrec[3];
/**
	if ( line+1>PAGE_CNT )
	{
		print_tail();
		print_HY();
		page++;
		print_head(1);
	}
****/
	strcpy( vrec,"DD" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		line+=1;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ��β */
int print_tail( )
{
	char vrec[3];

	tag = 8;	/**��ӡ������Ϣ**/
	ret = Make_yeb_sub("AA");
	ERR_DEAL
		line+=1;
	tag = 0;

	tag = 2;/*��ת��ҳ��־*/
	ret = Make_yeb_sub("DD");
	ERR_DEAL
		line+=1;
	tag = 0;

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		line+=2;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}
/* ��ӡHY */
int print_HY( )
{
	/*
	char vrec[3];

	strcpy( vrec,"YY" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	*/
	vtcp_log("[%s][%d]+++++++++++++++++\n",__FILE__,__LINE__);
	/***fprintf(fp,"\f"); ��������ͷ**/
	line=0;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}
/* ��ӡHY */
int print_FF( )
{
	char vrec[3];

	strcpy( vrec,"FF" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL

		line=0;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ���� */
int print_body( )
{
	char vrec[3];

	if ( line+1>PAGE_CNT )
	{
		print_tail();
		print_HY();
		page++;
		print_head(1);
	}
	strcpy( vrec,"BB" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
		line+=1;

	/**���� ����Լ������ܼ�***/
	if ( s_acno_hst_log.dc_ind[0]=='1' )
		dr_sum_amt += s_acno_hst_log.tx_amt;
	else if ( s_acno_hst_log.dc_ind[0]=='2' )
		cr_sum_amt += s_acno_hst_log.tx_amt;

	/****���� ��� �Լ������ܽ�**/

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* �м亯�� */
int Make_yeb_sub_not_full( char vrec[3] )
{
	/*ret=pub_rpt_read_pfm_qd(fp_not_full,&line_file,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);notfull*/
	ret=pub_rpt_read_pfm_qd(fp,&line_file,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL

		GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ���� */
int print_head_not_full(int m_tag )
{
	if (ftpg != 1)
	{
		fprintf(fp, "\f");
	}
	char vrec[3];

	cr_sum_amt = 0.0;
	dr_sum_amt = 0.0;
	ye_sum_amt = 0.0;
/*
	if ( line+7>PAGE_CNT )
	{
		print_tail_not_full();
		print_HY_not_full();
		line=0;
	}
*/
	if (printTail==0)
	{
	/****	fprintf(fp,"\f"); ��������ͷ*/
		/*fprintf(fp_not_full,"\f");notfull*/
	}
	else
		printTail=0;
	/*fprintf(fp_not_full,"%c%c",0x1b,0x40);
	fprintf(fp_not_full,"\n");notfull*/
	if (ftpg == 1)
	{
		fprintf(fp,"%c%c",0x1b,0x40);
	}
	fprintf(fp,"\n");
	strcpy( vrec,"HH" );
	ret = Make_yeb_sub_not_full(vrec);
	ERR_DEAL

		line+=6;
		ftpg+=1;

	if (m_tag>2)
		tag = m_tag;
	else
		tag = 1;
	printf("[%s][%d]��ӡ��ת��־ =[%d]\n",__FILE__,__LINE__,tag);
	ret = Make_yeb_sub_not_full("DD");
	ERR_DEAL
		tag = 0;
	line+=1;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ���� */
int print_body_A_not_full( )
{
	char vrec[3];
/*
	if ( line+1>PAGE_CNT )
	{
		print_tail_not_full();
		print_HY_not_full();
		page++;
		print_head_not_full(1);
	}
*/
	strcpy( vrec,"DD" );
	ret = Make_yeb_sub_not_full(vrec);
	ERR_DEAL
		line+=1;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ��β */
int print_tail_not_full( )
{
	char vrec[3];

	pre_tx_date = s_acno_hst_log.tx_date;	/**��ӡҳĩβʱ����Ҫ�õ�һ������***/
	lst_tx_date = pre_tx_date;		/**��ӡҳβ��ʱ�������ĳ��Ŀ���һҳ���������**/

	tag = 8;	/**��ӡ������Ϣ**/
	ret = Make_yeb_sub_not_full("AA");
	ERR_DEAL
		line+=1;
	tag = 0;

	printf("[%s][%d]�ֵ�����ӡβ����!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]�ֵ�����ӡβ����!\n",__FILE__,__LINE__);
	tag = 2;/*��ת��ҳ��־*/
	ret = Make_yeb_sub_not_full("DD");
	ERR_DEAL
		line+=1;
	tag = 0;

	strcpy( vrec,"CC" );
	ret = Make_yeb_sub_not_full(vrec);
	ERR_DEAL
		line+=2;

	print_HY_not_full( );

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}
/* ��ӡHY */
int print_HY_not_full( )
{
	/*
	char vrec[3];

	strcpy( vrec,"YY" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	*/
	vtcp_log("[%s][%d]+++++++++++++++++\n",__FILE__,__LINE__);
	/****fprintf(fp,"\f"); ��������ͷ***/
	/*fprintf(fp_not_full,"\f");notfull*/
	line=0;
	printTail=1;
	GoodExit:
		return 0;
	ErrExit:
		return 1;
}
/* ��ӡHY */
int print_FF_not_full( )
{
	char vrec[3];

	strcpy( vrec,"FF" );
	ret = Make_yeb_sub_not_full(vrec);
	ERR_DEAL

		line=0;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ӡ���� */
int print_body_not_full( )
{
	char vrec[3];

	if ( line+1>PAGE_CNT )
	{
		print_tail_not_full();
		print_HY_not_full();
		page++;
		print_head_not_full(1);
	}
	strcpy( vrec,"BB" );
	ret = Make_yeb_sub_not_full(vrec);
	ERR_DEAL
		line+=1;

	/**���� ����Լ������ܼ�***/
	if ( s_acno_hst_log.dc_ind[0]=='1' )
		dr_sum_amt += s_acno_hst_log.tx_amt;
	else if ( s_acno_hst_log.dc_ind[0]=='2' )
		cr_sum_amt += s_acno_hst_log.tx_amt;

	/****���� ��� �Լ������ܽ�**/

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* ��ֵ���� */
/* ˵����bli ��������ַ����� ... bll�������ռ�ĳ��� ... str�������ֵ */
int get_ratio_lsqd( bli,bll,str )
	int bli,bll;
char str[100];
{

	struct com_branch_c rggjgm;
	struct note_parm_c  s_note_parm;
	struct com_item_c  rkmb;

	char vhm[101];
	char vstr[101];
	char l_kmm[41];
	char amt_tmp[41];
	char tmp_inf[41];
	char tmp_acc_name[41];
	char fmt[20];
	char br_name[71];
	double tmp_amt;
	double bal;
	char brf[21];
	char tel[5];
	char note_no[17];
	char note_type[13];
	long date;
	int i=0;

	switch( bli )
	{
	case 'Z':
		if ( s_acno_hst_log.ac_type[0]=='1' )
			strcpy( brf , "���" ) ;
		else if ( s_acno_hst_log.ac_type[0]=='2')
			strcpy( brf , "����" ) ;
		else
			strcpy( brf , "�ڲ�" ) ;

		pub_base_strpack( brf );
		sprintf( t_str , "%s" , brf );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'X':	/**����**/
		sprintf(fmt,"%%-%ds",bll);

		if ( s_acno_hst_log.ac_type[0]=='1' )
			sprintf( str , fmt , s_dd_mst.name );
		else if ( s_acno_hst_log.ac_type[0]=='3' )
			sprintf( str , fmt , s_in_mst.name );
		else if ( s_acno_hst_log.ac_type[0]=='2' )
			sprintf( str , fmt , s_ln_mst.name );
		break;
	case 'B': /**�ʺ�**/
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , s_mdm_ac_rel.ac_no );
		break;
	case 'A': /* �������� */
		sprintf( t_str , "%4d��%2d��%2d��" ,
			g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 ,
			g_pub_tx.tx_date%100 );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;
	case 'C': /* �������� */
		ret = Com_branch_Sel(g_pub_tx.reply,&rggjgm,"br_no='%.5s'",
			s_acno_hst_log.br_no);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"SELECT COM_BRANCH ERROR !! [%d]",ret);
			WRITEMSG
				sprintf(acErrMsg,"br_no=[%s]",s_acno_hst_log.br_no);
			WRITEMSG
				goto ErrExit;
		}
		else if (ret == 100)
		{
			strcpy(rggjgm.br_name,"");
		}
		memset( str,' ',bll );
		ldchar( rggjgm.br_name,strlen(rggjgm.br_name),rggjgm.br_name );
		i=strlen(rggjgm.br_name);
		strncpy( str,rggjgm.br_name,i );
		/*add by martin Ӧ�����Ҫ��ԶԹ������ӿ�Ŀ��Ҫ��*/
			{
				ret = Acc_ac_id_Sel(g_pub_tx.reply,&s_acc_ac_id,"  data_code='0152' and ac_id='%ld'",s_acno_hst_log.ac_id);
				if (ret != 0 && ret != 100)
		      {
		      	sprintf(acErrMsg,"SELECT ACC_AC_ID ERROR !! [%d]",ret);
		      	WRITEMSG
		      		sprintf(acErrMsg,"ac_id=[%ld]",s_acno_hst_log.ac_id);
		      	WRITEMSG
		      		goto ErrExit;
		      }

			}
		if ( strncmp(vjgbm,"00000",5) )
		{
			strncpy( str+i,"(",1 );
			i++;
			strncpy( str+i,s_acno_hst_log.br_no,5 );
			i=i+5;
			/*add by martin Ӧ�����Ҫ��ԶԹ������ӿ�Ŀ��Ҫ��*/
				{
					strncpy( str+i," ",1 );/*�ÿո������Ե���*/
					i=i++;
					if(strlen(s_acc_ac_id.acc_no)>2 && strlen(s_acc_ac_id.acc_no)<6)
						{
					   strncpy( str+i,s_acc_ac_id.acc_no,strlen(s_acc_ac_id.acc_no) );
			       i=i+5;
			      }
				}
			strncpy( str+i,")",1 );
		}
		break;
	case 'D': /* ҳ�� */
		sprintf(fmt,"%%%dld",bll);
		sprintf(str, fmt, page);
		break;
	case 'E': /* ��������  */
		date = s_acno_hst_log.tx_date ;
		sprintf( t_str , "%ld" , date );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;

	case 'F': /* ժҪ */
		strcpy( brf , s_acno_hst_log.brf ) ;
		pub_base_strpack( brf );
		sprintf( t_str , "%s" , brf );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'G': /* ƾ֤���� */
		strcpy( note_no , s_acno_hst_log.note_no ) ;

		pub_base_strpack( note_no );
		if ( !strlen(note_no) )
			strcpy(note_no,"----------------" );

		if ( strlen(note_no)>8 )
			strcpy( note_no,note_no+strlen(note_no)-8 );

		sprintf( t_str , "%s" , note_no );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'H': /* ƾ֤���� */
		strcpy( note_type , s_acno_hst_log.note_type ) ;

		pub_base_strpack( note_type );
		if ( strlen(note_type) )
		{
			pub_base_dic_show_str(s_note_parm.name,"note_type",note_type);
		}
		else strcpy( s_note_parm.name,"----------------" );

		sprintf( t_str , "%s" , s_note_parm.name );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'I': /* ���׹�Ա */
		strcpy( tel , s_acno_hst_log.tel ) ;

		pub_base_strpack( tel );
		sprintf( t_str , "%s" , tel );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'J': /* �跽������ */
		if ( s_acno_hst_log.dc_ind[0]=='1' )	/**1 �� */
		{
			tmp_amt = s_acno_hst_log.tx_amt ;
			sprintf( amt_tmp, "%.2lf" , s_acno_hst_log.tx_amt );
		}
		else if ( s_acno_hst_log.dc_ind[0]=='2' )	/*2 ��*/
		{
			tmp_amt = 0.00 ;
			sprintf( amt_tmp, "%.2lf" , 0.00 );
		}
		if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , tmp_amt );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'K': /* ���������� */
		if ( s_acno_hst_log.dc_ind[0]=='2' )
		{
			tmp_amt = s_acno_hst_log.tx_amt ;
			sprintf( amt_tmp, "%.2lf" , s_acno_hst_log.tx_amt );
		}
		else if ( s_acno_hst_log.dc_ind[0]=='1' )
		{
			tmp_amt = 0.00 ;
			sprintf( amt_tmp, "%.2lf" , 0.00 );
		}
		if( !pub_base_CompDblVal(atof(amt_tmp),0.00) )
			{
				memset(str,' ',bll );
				break;
			}
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , tmp_amt );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'L': /* ��    �� */
		bal = s_acno_hst_log.bal ;

		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'M': /* ��ת���� */
		if ( tag==1 )
			date = pre_tx_date ;
		else if ( tag==2 )
			date = lst_tx_date ;
		else if ( tag ==3 )
			date = tmp_date;
		else if ( tag ==4)
			date = s_acno_hst_log.tx_date;
		else if ( tag ==8)	/**��ӡ������Ϣ**/
			date = s_acno_hst_log.tx_date;

		sprintf( t_str , "%ld" , date );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;
	case 'N': /* ��ת���� */
		if ( tag==1 )
			strcpy(brf,"��ת��ҳ");
		else if ( tag==2 )
			strcpy(brf,"��ת��ҳ");
		else if ( tag==3)
			strcpy(brf,"�״δ�ӡ");
		else if ( tag==4)
			strcpy(brf,"��ת�ϴ�");
		else if ( tag==8)
			strcpy(brf,"������");

		pub_base_strpack(brf);
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, brf);
		break ;
	case 'O': /* ��ת��Ա */
		strcpy( tel,"9999" );
		pub_base_strpack( tel );
		sprintf( t_str , "%s" , tel );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'P': /* ��ת��� */
		if ( tag==1 )
		{
			if( v_acno_hst_log.ac_type[0]=='2' )/* ������ҳ������*/
				{
					if ( v_acno_hst_log.dc_ind[0]=='2')
			   	 bal = v_acno_hst_log.bal+v_acno_hst_log.tx_amt ;
			    else if ( v_acno_hst_log.dc_ind[0]=='1')
			   	 bal = v_acno_hst_log.bal-v_acno_hst_log.tx_amt ;
				}
			else
			if(v_acno_hst_log.ac_type[0]=='3' && s_mdm_ac_rel.ac_no[1]=='1' )
				{
					if ( v_acno_hst_log.dc_ind[0]=='2')
			   	 bal = v_acno_hst_log.bal+v_acno_hst_log.tx_amt ;
			    else if ( v_acno_hst_log.dc_ind[0]=='1')
			   	 bal = v_acno_hst_log.bal-v_acno_hst_log.tx_amt ;
				}
			else
			if ( v_acno_hst_log.dc_ind[0]=='1')
			   	bal = v_acno_hst_log.bal+v_acno_hst_log.tx_amt ;
			   else if ( v_acno_hst_log.dc_ind[0]=='2')
			   	bal = v_acno_hst_log.bal-v_acno_hst_log.tx_amt ;

		}
		else if (tag==3)
			bal = 0;
		else if (tag==4)
		{
			if( s_acno_hst_log.ac_type[0]=='2' )/* ������ҳ������*/
				{
					if  ( s_acno_hst_log.dc_ind[0]=='2')
				bal = s_acno_hst_log.bal+s_acno_hst_log.tx_amt ;
			else if ( s_acno_hst_log.dc_ind[0]=='1')
				bal = s_acno_hst_log.bal-s_acno_hst_log.tx_amt ;
				}
			else
			if(s_acno_hst_log.ac_type[0]=='3' && s_mdm_ac_rel.ac_no[1]=='1' )
				{
					if  ( s_acno_hst_log.dc_ind[0]=='2')
				bal = s_acno_hst_log.bal+s_acno_hst_log.tx_amt ;
			else if ( s_acno_hst_log.dc_ind[0]=='1')
				bal = s_acno_hst_log.bal-s_acno_hst_log.tx_amt ;
				}
			else
			if ( s_acno_hst_log.dc_ind[0]=='1')
				bal = s_acno_hst_log.bal+s_acno_hst_log.tx_amt ;
			else if ( s_acno_hst_log.dc_ind[0]=='2')
				bal = s_acno_hst_log.bal-s_acno_hst_log.tx_amt ;
		}
		else if(tag==2)
		{
			/*bal = v_acno_hst_log.bal ;��ת��ҳ���20070924*/
			strcpy( brf , "------------------" ) ;

			pub_base_strpack( brf );
			sprintf( t_str , "%s" , brf );
			sprintf(fmt, "%%-%ds", bll);
			sprintf(str, fmt, t_str);
			break;
		}
		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'S':/*** ���� ***/
		if( g_pub_tx.tx_date%10000/100<=3 )
			sprintf(brf,"һ����");
		else if( g_pub_tx.tx_date%10000/100<=6 )
			sprintf(brf,"������");
		else if( g_pub_tx.tx_date%10000/100<=9 )
			sprintf(brf,"������");
		else if( g_pub_tx.tx_date%10000/100<=12 )
			sprintf(brf,"�ļ���");

		pub_base_strpack(brf);
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, brf);
		break;
	case 'U':/**������ ����*****/
		date = lst_tx_date;

		sprintf( t_str , "%ld" , date );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;
	case 'V':/**������Ϣ ժҪ***/
		strcpy(brf,"������");
		pub_base_strpack(brf);
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, brf);
		break ;
	case 'W':/**������Ϣ ��ϼ�ֵ***/
		bal = dr_sum_amt ;
		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'Y':/**������Ϣ ���ϼ�ֵ***/
		bal = cr_sum_amt ;
		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'R':/**������Ϣ ���ϼ�ֵ***/
		bal = ye_sum_amt ;
		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
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
