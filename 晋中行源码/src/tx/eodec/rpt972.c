
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
* 文 件 名: rpt972.pc
* 功能描述：内部账户不动户(查找in_mst表中数据，条件lst_date < 当年(20130101)为不动户)
*
* 作    者: zgf
* 完成日期: 2013年4月16日
*
* 注：格式文件为RPT972.tbl
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define extern
#define ERR_DEAL if(ret) {\
	sprintf(acErrMsg,"数据库错误!"); \
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
static struct	acno_hst_log_c 	     	m_acno_hst_log;	/**存放某个帐号最后一个满页的记录处的那条记录**/
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
int   ptail = 0 ;    /***未满页的地方结尾打出了满页结尾***/
int	ret = 0;
int 	flag = 0;							/*** 0- 无 1-存款 2-贷款 3-内部 ***/
int 	tag = 0;
long 	line_file=0;
char 	vpfm[21];
int	l_hst_cnt = 0;	/*当前明细表有多少条待打印的记录**/
int 	line;
long	pre_ac_id = 0;
int   ftpg = 1;   /* 机构第一页 */
int   nxt  = 0;  /* 区别是否只打印内部 */

int get_ratio_lsqd(int bli,int bll,char str[100]);

rpt972()
{
	vtcp_log("[%s][%d]开始生成内部帐户不动户帐页数据!\n",__FILE__,__LINE__);
	printf("[%s][%d]开始生成内部帐户不动户帐页数据!\n",__FILE__,__LINE__);
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
	tmp_date = s_com_sys_parm.lst_date/10000*10000+101;/**获取到年初的值**/

	strcpy( filename , "RPT972");
	strcpy( filename_not_full,"RPT972_NOT_FULL");
	strcpy( vpfm, "RPT972");
	pub_rpt_rmfile( "",filename,0 );
	pub_rpt_rmfile( "",filename_not_full,0 );
printf("111111111111111111\n"); 
	ret = Mdm_ln_mst_Dec_Upd(g_pub_tx.reply,"lst_date>=%ld ",tmp_date);
	ERR_DEAL
printf("[%s][%d]日期[%s]!\n",__FILE__,__LINE__,tmp_date);
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


		lst_tx_date = pre_tx_date;		/*打印页尾巴时候，如果是某科目最后一页用这个日期**/
		pre_tx_date = s_acno_hst_log.tx_date;	/*打印页末尾时候需要用到一个日期***/

		/****如果机构号变化了需要重新开闭问文件***/
		if ( strncmp(pre_br_no,s_acno_hst_log.br_no,5) )
		{
			if ( strncmp(pre_br_no,"-----",5) )
			{
				/***这里不需要换页了
				print_HY_not_full();
				print_HY();
				*****/

				fclose(fp);
				/*fclose(fp_not_full);notfull*/

				recordnum =0; /**这个标志页设置为0  一旦换了机构就别再首先打印表尾了***/
				line = 0;
			}
			ret = pub_rpt_openfile( &fp,s_acno_hst_log.br_no,filename );
			ERR_DEAL
			/*ret = pub_rpt_openfile( &fp_not_full,s_acno_hst_log.br_no,filename_not_full );
			ERR_DEAL notfull*/
			ftpg = 1;
		}
		strcpy( pre_br_no,s_acno_hst_log.br_no );

		/***如果帐号变化了**/
		if (pre_ac_id!=s_acno_hst_log.ac_id)
		{
			l_hst_cnt =0;
			page = 0;
			/***获取当前帐号打印到了多少页了***/
			ret = sql_max_long_xier("acno_hst_log","pagecnt",&page,"ac_id=%ld and print_flag='3'",s_acno_hst_log.ac_id);
			ERR_DEAL

			vtcp_log("[%s][%d]当前帐号[%ld]已经打印的页数==[%ld]\n",__FILE__,__LINE__,s_acno_hst_log .ac_id,page);

			/* 增加了联合查询这个多余
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_id=%ld",s_acno_hst_log.ac_id);
			ERR_DEAL
			*/

			if (s_acno_hst_log.ac_type[0]=='1')/**活期**/
			{
				/****当为卡时，也不打印满页帐 20101227  ****/
				if(s_mdm_ac_rel.ac_no[0]=='1' || memcmp(s_mdm_ac_rel.ac_no,CARDHEADCODE,6) == 0 || memcmp(s_mdm_ac_rel.ac_no,ICCARDHEADCODE,6) == 0) /***活期帐户开头为1的不打非公储蓄户**/
					{
						continue;	/***单独打印储蓄户账页时注掉这句***/
					}
				ret = Dd_mst_Sel(g_pub_tx.reply,&s_dd_mst,"ac_id=%ld",s_acno_hst_log.ac_id);
				ERR_DEAL

				nxt = 1;
			}
			else if (s_acno_hst_log.ac_type[0]=='3')/**内部帐***/
			{
				ret = In_mst_Sel(g_pub_tx.reply,&s_in_mst,"ac_id=%ld",s_acno_hst_log.ac_id);
				ERR_DEAL

				if(nxt == 1)
					{
						ftpg += 1;
					}
					nxt = 1;
			}
			else if (s_acno_hst_log.ac_type[0]=='2')/**贷款***/
			{
				ret = Ln_mst_Sel(g_pub_tx.reply,&s_ln_mst,"ac_id=%ld",s_acno_hst_log.ac_id);
				ERR_DEAL

				if(nxt == 1)
					{
						ftpg += 1;
					}
			}
			/**如果不是第一次的话***/

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

			/***获取到该科目需要打印的记录条数**/

			l_hst_cnt = sql_count("acno_hst_log","print_flag='0' and br_no='%s' and ac_id='%ld'",
				s_acno_hst_log.br_no,s_acno_hst_log.ac_id);
			vtcp_log("[%s][%d]机构[%s]帐号[%ld]待打印的记录条数为[%d]\n",__FILE__,__LINE__,s_acno_hst_log.br_no,s_acno_hst_log.ac_id,
				l_hst_cnt);

			printf("[%s][%d]机构[%s]帐号[%ld]待打印的记录条数为[%d]\n",__FILE__,__LINE__,s_acno_hst_log.br_no,s_acno_hst_log.ac_id,
				l_hst_cnt);

			if (l_hst_cnt+7<PAGE_CNT)
			{
				/***避免如果只有一笔记录的情况下v_acno_hst_log 没有值***/
				memcpy(&v_acno_hst_log,&s_acno_hst_log,sizeof(v_acno_hst_log));
				/**if (g_pub_tx.tx_date%10000 ==NOT_FULL_DATE && l_hst_cnt>0) del by martin**/
				if ( l_hst_cnt>0 )
				{

					printf("[%s][%d]line_file===[%d],l_hst_cnt===[%d]\n",__FILE__,__LINE__,line_file,l_hst_cnt);
					/* 打印页首 */
					/*page = 1;*/
					page = page +1;

					ret = print_head_not_full(4);
					ERR_DEAL

						line = 0;

					ret = print_body_not_full( );
					ERR_DEAL


					recordnum_notfull++;

					/***将打印的最后一条记录的打印标志设置为3****/
					if (recordnum_notfull != l_hst_cnt)
						s_acno_hst_log.print_flag[0]='1';
					else
						s_acno_hst_log.print_flag[0]='3';
					s_acno_hst_log.pagecnt = page;
					/*****设置好了更新字段***********************/

					ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
					ERR_DEAL
						vtcp_log("[%s][%d]开始更新记录的打印标志成功!\n",__FILE__,__LINE__);

					strcpy( pre_br_no,s_acno_hst_log.br_no );
					ttlnum++;

					printf("[%s][%d]这是今年的最后一天了需要打印出未满页的记录!\n",__FILE__,__LINE__);
					printf("[%s][%d]recordnum_notfull=[%d],l_hst_cnt=[%d]!\n",__FILE__,__LINE__,recordnum_notfull,l_hst_cnt);
					/* rem by LiuYue 2007-1-30 21:49
					if(line_file == l_hst_cnt+8) 因为这里的line_file包括了标题部分*
					**************replace by next line **********************************/
					if (recordnum_notfull == l_hst_cnt) /*因为这里的line_file包括了标题部分**/
					{
						printf("[%s][%d]my xier,总记录数不足满页的情况，你硬要打印，到打印尾巴时候了!\n",__FILE__,__LINE__);
						ret = print_tail_not_full();
						ERR_DEAL
							line_file += 3; /**结转一行  结束框架占2行***/
					}
					pre_ac_id = s_acno_hst_log.ac_id;
					strcpy( vjgbm,s_acno_hst_log.br_no );
				}
				continue;
			}
			page += 1;
			pre_ac_id = s_acno_hst_log.ac_id;
			strcpy( vjgbm,s_acno_hst_log.br_no );

			/* 打印页首 */
			ret = print_head(4);

		}

		memcpy(&v_acno_hst_log,&s_acno_hst_log,sizeof(v_acno_hst_log));


		if (l_hst_cnt+7<PAGE_CNT)
		{
			/**if (g_pub_tx.tx_date%10000 ==NOT_FULL_DATE && l_hst_cnt>0)**/
				if ( l_hst_cnt>0 )
			{
				if (line_file==0 && l_hst_cnt>0)	/*由于在标志 FLAG_SEE处 没有进行表头的打印，所以放到这里来打印**/
				{
					/* 打印页首 */
					ret = print_head_not_full(4);
					ERR_DEAL
						line = 0;
				}
				ret = print_body_not_full( );
				ERR_DEAL

				recordnum_notfull ++;/* add by LiuYue 2007-1-30 21:50 */
				vtcp_log("[%s][%d]开始更新记录的打印标志!\n",__FILE__,__LINE__);

				/***将打印的最后一条记录的打印标志设置为3****/
				if (recordnum_notfull != l_hst_cnt)
					s_acno_hst_log.print_flag[0]='1';
				else
					s_acno_hst_log.print_flag[0]='3';
				s_acno_hst_log.pagecnt = page;
				/*****设置好了更新字段***********************/

				ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
				ERR_DEAL
					vtcp_log("[%s][%d]开始更新记录的打印标志成功!\n",__FILE__,__LINE__);

				pre_ac_id = s_acno_hst_log.ac_id;
				ttlnum++;

				/* rem by LiuYue 2007-1-30 21:51
				if(line_file == l_hst_cnt+8) *因为这里的line_file包括了标题部分*
				***** replace by next line  2007-1-30 21:51 ********************/
				if (recordnum_notfull == l_hst_cnt)  /*因为这里的line_file包括了标题部分**/
				{
					printf("[%s][%d]my xier,总记录数不足满页的情况，你硬要打印，到打印尾巴时候了!\n",__FILE__,__LINE__);
					ret = print_tail_not_full();
					ERR_DEAL
						line_file += 3; /**结转一行  结束框架占2行***/
				}
			}
			continue;
		}
		recordnum++; /**某个帐号的明细增加标志**/
		/* 打印分户信息 */
		/* rem by LiuYue 2007-1-30 22:38
		if(l_hst_cnt>PAGE_CNT && (recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7) > 0))*如果当前待打印的hst_cnt-当前hst_cnt<满页笔数,则不打印**
		*******************replace by next line *********************************/
		if (l_hst_cnt>=(PAGE_CNT-7) && (recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7) > 0))/*如果当前待打印的hst_cnt-当前hst_cnt<满页笔数,则不打印**/
		{
			ttlnum++;
			/****保存这最后一个满页的那一笔记录，处理当不打印未满页帐时候，最后一个满页帐的结转金额打印的不是那一笔，而是最后一笔记录，
			比如  若有132条记录，结转金额用的是第132 而不是100****/
			if ((recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7))==1)
				memcpy(&m_acno_hst_log,&s_acno_hst_log,sizeof(m_acno_hst_log));


			/*if (g_pub_tx.tx_date%10000 ==NOT_FULL_DATE && l_hst_cnt>0)del by martin*/
				if ( l_hst_cnt>0 )
			{
				if ((recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7))==1)
				{
					/*打印满页的表尾 0619*/
					if( pre_ac_id==s_acno_hst_log.ac_id )
						{
							ret = print_tail();
							ERR_DEAL
						  print_HY();
					    ERR_DEAL
							ptail=1;
						}
					printf("[%s][%d]这是今年的最后一天了需要打印出未满页的记录!\n",__FILE__,__LINE__);
					/* 打印页首 */
					line = 0;
					page = page +1;
					/*
					ret = print_head_not_full(4);
					ERR_DEAL*/
					if( page==1 )/*判断是否继续打未满页上页 070701*/
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

					vtcp_log("[%s][%d]开始更新记录的打印标志!\n",__FILE__,__LINE__);

				if (recordnum !=l_hst_cnt)
					s_acno_hst_log.print_flag[0]='1';
				else
					s_acno_hst_log.print_flag[0]='3';

				s_acno_hst_log.pagecnt = page;

				ret = Mdm_ac_rel_acno_hst_Upd_Upd(s_acno_hst_log,g_pub_tx.reply);
				ERR_DEAL

					vtcp_log("[%s][%d]开始更新记录的打印标志成功!\n",__FILE__,__LINE__);

				line_file ++;

				pre_ac_id = s_acno_hst_log.ac_id;

				ttlnum++;

				if (recordnum ==l_hst_cnt)
				{
					printf("[%s][%d]老大到了记录的最后一笔了该打印尾巴了!\n",__FILE__,__LINE__);
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

			vtcp_log("[%s][%d]开始更新记录的打印标志!\n",__FILE__,__LINE__);
		if(recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7)==0)
		{
			m_cr_sum_amt = cr_sum_amt;
			m_dr_sum_amt = dr_sum_amt;
		}
		/**if ((g_pub_tx.tx_date%10000 !=NOT_FULL_DATE && (recordnum-l_hst_cnt/(PAGE_CNT-7)*(PAGE_CNT-7)==0)) ||
			recordnum==l_hst_cnt)del by martin 如果那天不打印不满页帐，那么需要再满页处设置标志3  如果记录总数正好是满页更需要设置标志3***/
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
		vtcp_log("[%s][%d]开始更新记录的打印标志成功!\n",__FILE__,__LINE__);

	}
	vtcp_log("[%s][%d]提交数据库!\n",__FILE__,__LINE__);
	sql_commit();   /*--提交事务--*/
	vtcp_log("机构[%s]完成\n",g_brno_tmp.br_no);
	/******************程序出口************************/
	/*
	* 正常出口
	*/
	/** if (g_pub_tx.tx_date%10000 !=NOT_FULL_DATE)	如果那天不打印未满页帐，删除掉未满页文件
		pub_rpt_rmfile( "",filename_not_full,0 );del by martin **/

	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before OK return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
		vtcp_log("Before OK return: reply is [%s]\n",g_pub_tx.reply);
	return (0);
	/*
	* 错误出口
	*/
	ErrExit:
		strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"Before return: reply is [%s]",g_pub_tx.reply);
	WRITEMSG
		vtcp_log("Before bad return: reply is [%s]\n",g_pub_tx.reply);
	return (1);

}

/* 中间函数 */
int Make_yeb_sub( char vrec[3] )
{
	ret=pub_rpt_read_pfm_qd(fp,&line_file,vpfm,"0001",vrec,&opncnt,get_ratio_lsqd);
	ERR_DEAL

		GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* 打印表首 */
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
	printf("[%s][%d]打印结转标志 =[%d]\n",__FILE__,__LINE__,tag);
	ret = Make_yeb_sub("DD");
	ERR_DEAL
		tag = 0;
	line+=1;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* 打印表体 */
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

/* 打印表尾 */
int print_tail( )
{
	char vrec[3];

	tag = 8;	/**打印汇总信息**/
	ret = Make_yeb_sub("AA");
	ERR_DEAL
		line+=1;
	tag = 0;

	tag = 2;/*结转下页标志*/
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
/* 打印HY */
int print_HY( )
{
	/*
	char vrec[3];

	strcpy( vrec,"YY" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	*/
	vtcp_log("[%s][%d]+++++++++++++++++\n",__FILE__,__LINE__);
	/***fprintf(fp,"\f"); 更换到开头**/
	line=0;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}
/* 打印HY */
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

/* 打印表体 */
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

	/**处理 借贷以及余额的总计***/
	if ( s_acno_hst_log.dc_ind[0]=='1' )
		dr_sum_amt += s_acno_hst_log.tx_amt;
	else if ( s_acno_hst_log.dc_ind[0]=='2' )
		cr_sum_amt += s_acno_hst_log.tx_amt;

	/****处理 借贷 以及余额的总结**/

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* 中间函数 */
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

/* 打印表首 */
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
	/****	fprintf(fp,"\f"); 更换到开头*/
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
	printf("[%s][%d]打印结转标志 =[%d]\n",__FILE__,__LINE__,tag);
	ret = Make_yeb_sub_not_full("DD");
	ERR_DEAL
		tag = 0;
	line+=1;

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* 打印表体 */
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

/* 打印表尾 */
int print_tail_not_full( )
{
	char vrec[3];

	pre_tx_date = s_acno_hst_log.tx_date;	/**打印页末尾时候需要用到一个日期***/
	lst_tx_date = pre_tx_date;		/**打印页尾巴时候，如果是某科目最后一页用这个日期**/

	tag = 8;	/**打印汇总信息**/
	ret = Make_yeb_sub_not_full("AA");
	ERR_DEAL
		line+=1;
	tag = 0;

	printf("[%s][%d]兄弟来打印尾巴了!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]兄弟来打印尾巴了!\n",__FILE__,__LINE__);
	tag = 2;/*结转下页标志*/
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
/* 打印HY */
int print_HY_not_full( )
{
	/*
	char vrec[3];

	strcpy( vrec,"YY" );
	ret = Make_yeb_sub(vrec);
	ERR_DEAL
	*/
	vtcp_log("[%s][%d]+++++++++++++++++\n",__FILE__,__LINE__);
	/****fprintf(fp,"\f"); 更换到开头***/
	/*fprintf(fp_not_full,"\f");notfull*/
	line=0;
	printTail=1;
	GoodExit:
		return 0;
	ErrExit:
		return 1;
}
/* 打印HY */
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

/* 打印表体 */
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

	/**处理 借贷以及余额的总计***/
	if ( s_acno_hst_log.dc_ind[0]=='1' )
		dr_sum_amt += s_acno_hst_log.tx_amt;
	else if ( s_acno_hst_log.dc_ind[0]=='2' )
		cr_sum_amt += s_acno_hst_log.tx_amt;

	/****处理 借贷 以及余额的总结**/

	GoodExit:
		return 0;
	ErrExit:
		return 1;
}

/* 赋值函数 */
/* 说明：bli 代表变量字符名称 ... bll代表变量占的长度 ... str代表变量值 */
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
			strcpy( brf , "存款" ) ;
		else if ( s_acno_hst_log.ac_type[0]=='2')
			strcpy( brf , "贷款" ) ;
		else
			strcpy( brf , "内部" ) ;

		pub_base_strpack( brf );
		sprintf( t_str , "%s" , brf );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'X':	/**户名**/
		sprintf(fmt,"%%-%ds",bll);

		if ( s_acno_hst_log.ac_type[0]=='1' )
			sprintf( str , fmt , s_dd_mst.name );
		else if ( s_acno_hst_log.ac_type[0]=='3' )
			sprintf( str , fmt , s_in_mst.name );
		else if ( s_acno_hst_log.ac_type[0]=='2' )
			sprintf( str , fmt , s_ln_mst.name );
		break;
	case 'B': /**帐号**/
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , s_mdm_ac_rel.ac_no );
		break;
	case 'A': /* 出表日期 */
		sprintf( t_str , "%4d年%2d月%2d日" ,
			g_pub_tx.tx_date/10000 , g_pub_tx.tx_date%10000/100 ,
			g_pub_tx.tx_date%100 );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;
	case 'C': /* 机构名称 */
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
		/*add by martin 应银监局要求对对公户增加科目号要求*/
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
			/*add by martin 应银监局要求对对公户增加科目号要求*/
				{
					strncpy( str+i," ",1 );/*用空格间隔可以调整*/
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
	case 'D': /* 页码 */
		sprintf(fmt,"%%%dld",bll);
		sprintf(str, fmt, page);
		break;
	case 'E': /* 交易日期  */
		date = s_acno_hst_log.tx_date ;
		sprintf( t_str , "%ld" , date );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;

	case 'F': /* 摘要 */
		strcpy( brf , s_acno_hst_log.brf ) ;
		pub_base_strpack( brf );
		sprintf( t_str , "%s" , brf );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'G': /* 凭证号码 */
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
	case 'H': /* 凭证类型 */
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
	case 'I': /* 交易柜员 */
		strcpy( tel , s_acno_hst_log.tel ) ;

		pub_base_strpack( tel );
		sprintf( t_str , "%s" , tel );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'J': /* 借方发生额 */
		if ( s_acno_hst_log.dc_ind[0]=='1' )	/**1 借 */
		{
			tmp_amt = s_acno_hst_log.tx_amt ;
			sprintf( amt_tmp, "%.2lf" , s_acno_hst_log.tx_amt );
		}
		else if ( s_acno_hst_log.dc_ind[0]=='2' )	/*2 贷*/
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
	case 'K': /* 贷方发生额 */
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
	case 'L': /* 余    额 */
		bal = s_acno_hst_log.bal ;

		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'M': /* 结转日期 */
		if ( tag==1 )
			date = pre_tx_date ;
		else if ( tag==2 )
			date = lst_tx_date ;
		else if ( tag ==3 )
			date = tmp_date;
		else if ( tag ==4)
			date = s_acno_hst_log.tx_date;
		else if ( tag ==8)	/**打印汇总信息**/
			date = s_acno_hst_log.tx_date;

		sprintf( t_str , "%ld" , date );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;
	case 'N': /* 结转字样 */
		if ( tag==1 )
			strcpy(brf,"结转上页");
		else if ( tag==2 )
			strcpy(brf,"结转下页");
		else if ( tag==3)
			strcpy(brf,"首次打印");
		else if ( tag==4)
			strcpy(brf,"结转上次");
		else if ( tag==8)
			strcpy(brf,"金额汇总");

		pub_base_strpack(brf);
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, brf);
		break ;
	case 'O': /* 结转柜员 */
		strcpy( tel,"9999" );
		pub_base_strpack( tel );
		sprintf( t_str , "%s" , tel );
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, t_str);
		break;
	case 'P': /* 结转余额 */
		if ( tag==1 )
		{
			if( v_acno_hst_log.ac_type[0]=='2' )/* 贷款上页反方向*/
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
			if( s_acno_hst_log.ac_type[0]=='2' )/* 贷款上页反方向*/
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
			/*bal = v_acno_hst_log.bal ;结转下页封掉20070924*/
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
	case 'S':/*** 季度 ***/
		if( g_pub_tx.tx_date%10000/100<=3 )
			sprintf(brf,"一季度");
		else if( g_pub_tx.tx_date%10000/100<=6 )
			sprintf(brf,"二季度");
		else if( g_pub_tx.tx_date%10000/100<=9 )
			sprintf(brf,"三季度");
		else if( g_pub_tx.tx_date%10000/100<=12 )
			sprintf(brf,"四季度");

		pub_base_strpack(brf);
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, brf);
		break;
	case 'U':/**汇总栏 日期*****/
		date = lst_tx_date;

		sprintf( t_str , "%ld" , date );
		sprintf(fmt,"%%-%ds",bll);
		sprintf( str , fmt , t_str );
		break;
	case 'V':/**汇总信息 摘要***/
		strcpy(brf,"金额汇总");
		pub_base_strpack(brf);
		sprintf(fmt, "%%-%ds", bll);
		sprintf(str, fmt, brf);
		break ;
	case 'W':/**汇总信息 借合计值***/
		bal = dr_sum_amt ;
		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'Y':/**汇总信息 贷合计值***/
		bal = cr_sum_amt ;
		sprintf( amt_tmp, "%.2lf" , bal );
		pub_rpt_flttomon( amt_tmp,amt_tmp );
		sprintf(fmt,"%%%ds",bll);
		if ( strlen(amt_tmp)>bll )
			sprintf( amt_tmp, "%.2lf" , bal );
		sprintf( str, fmt, amt_tmp );
		break;
	case 'R':/**汇总信息 余额合计值***/
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
