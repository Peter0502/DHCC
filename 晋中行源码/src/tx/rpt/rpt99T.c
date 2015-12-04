/*************************************************************
* 文 件 名: rpt99T.c
* 功能描述：日终特别多的传票打印
*
* 作    者: rob
* 完成日期: 2003年09月20日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "bb0000.h"

#include "rpt_note_inf_c.h"
#include "com_item_c.h"
#include "com_sys_parm_c.h"
#include "com_cur_no_code_c.h"

#define EXTERN
#include "public.h"
#define	ERR_XYM { strcpy(acErrMsg,"SQL ERROR!");strcpy(g_pub_tx.reply,"0230");WRITEMSG goto ErrExit; }
#define MYPZ  3

static	struct	rpt_note_inf_c	RPT_NOTE0;    
static	struct	com_sys_parm_c	s_com_sys_parm;    
static	char    gx_brno[10];         	 

FILE    *fp;

rpt99T()
{
	int    pzs;    /* 凭证数 */
	int    ret;
	char   flag ;

	memset(&RPT_NOTE0, 		0x00, sizeof(struct rpt_note_inf_c)); 
	memset(&s_com_sys_parm, 0x00, sizeof(struct com_sys_parm_c)); 
         
	ret=sql_begin(); /*打开事务*/
	if( ret != 0 )
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

	/* 初始化全局变量 */
	pub_base_sysinit();
    
	/* 计算翻盘前日期 */
	ret = Com_sys_parm_Sel(g_pub_tx.reply,&s_com_sys_parm,"sys_date=%ld",
						   g_pub_tx.tx_date);
	if( ret )	ERR_DEAL

	/* 读内容表 */    
	ret = Rpt_note_inf_Dec_Sel(g_pub_tx.reply," (tx_date=%d or tx_date=%d) and txcode[1]='T'  ORDER BY tx_tx_br_no,tx_date,trace_no,acc_hrt,dc_ind ", \
		s_com_sys_parm.lst_date,s_com_sys_parm.sys_date);
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR  ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	while ( 1 )
	{
		ret = Rpt_note_inf_Fet_Sel( &RPT_NOTE0 , g_pub_tx.reply) ;
		if( ret==100 )
		{
			if( flag=='1' )
			{
				fprintf( fp,"\n");
				fclose(fp);
			}
			break;
		}
		else if( ret )
		{
			sprintf(acErrMsg,"定义游标错[%d]",ret);
			strcpy (g_pub_tx.reply, "0230");
			ERR_DEAL
		}
	
		if( RPT_NOTE0.amt<0.005 && RPT_NOTE0.amt>-0.005 ) continue;
		ldchar( RPT_NOTE0.tx_tx_br_no,strlen(RPT_NOTE0.tx_tx_br_no),RPT_NOTE0.tx_tx_br_no );
		ldchar( RPT_NOTE0.name,strlen(RPT_NOTE0.name),RPT_NOTE0.name );
		ldchar( RPT_NOTE0.ac_no,strlen(RPT_NOTE0.ac_no),RPT_NOTE0.ac_no );
		ldchar( RPT_NOTE0.acc_hrt,strlen(RPT_NOTE0.acc_hrt),RPT_NOTE0.acc_hrt );
		if( strcmp(gx_brno,RPT_NOTE0.tx_tx_br_no) )
		{
			if( flag=='1' ) fclose(fp);
			flag='1';
			strcpy( gx_brno,RPT_NOTE0.tx_tx_br_no);
		    if( openfile( gx_brno ) )
			{
				printf("open file failed,return val=%d\n",ret);
				Rpt_note_inf_Clo_Sel();
				return(-1);
			}
			if( draw_head1( fp,gx_brno ) )
			{
				printf("draw_head1 failed\n");
				Rpt_note_inf_Clo_Sel();
				return(-1);
			}
			pzs=0;
		}
		if( pzs==3 )
		{
			fprintf( fp,"\n");
			pzs=0;
			if( (draw_head1(fp,gx_brno)) ) 
			{   
				printf("draw_head1 failed\n");  
				Rpt_note_inf_Clo_Sel();
				return(-1);
			}
		}
				  /*   打印凭证    */
		if( RPT_NOTE0.dc_ind[0]=='1' ) {
		    if( draw_head2( fp ) ) { printf("draw_head2 failed\n"); exit(1); }
		    if( draw_body( fp ) ) { printf("draw_body failed\n");  exit(1);  }
		    if( draw_end( fp ) ) { printf("draw_end failed\n"); exit(1); }
			pzs++;
			if( pzs!=3 )
		    fprintf(fp,"\n\n\n- - - - - - - - - - - - - - - - - -  - - -\n");
			/***不要客户回单***
			if( pzs==3 )
			{
				fprintf( fp,"@HY@\n");
				pzs=0;
				if( (draw_head1(fp,gx_brno)) ) 
				{   
					printf("draw_head1 failed\n");  
					EXEC SQL CLOSE RPT_NOTE_cur;
					EXEC SQL FREE  RPT_NOTE_cur;
					return(-1);
				}
			}
			if( draw_head3( fp ) ) { printf("draw_head2 failed\n"); exit(1); }
			if( draw_body( fp ) ) { printf("draw_body failed\n");  exit(1);  }
			if( draw_end( fp ) ) { printf("draw_end failed\n"); exit(1); }
			pzs++;
		    fprintf(fp,"\n\n\n\n");
			***/
		}
		else {
			if( draw_head2( fp ) ) { printf("draw_head2 failed\n"); exit(1); }
			if( draw_body( fp ) ) { printf("draw_body failed\n");  exit(1);  }
			if( draw_end( fp ) ) { printf("draw_end failed\n"); exit(1); }
			pzs++;
			if( pzs!=3 )
		    fprintf(fp,"\n\n- - - - - - - - - - - - - - - - - -  - - -\n");
		}
	}

	Rpt_note_inf_Clo_Sel();	
	/* 记录分组项目 */
	
	/* 根据数据来源表不同,统计数据 */
	/* 统计dc_log_bk */
	/* 统计dc_log_bk */


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"日终特别多的传票统计数据处理程序成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"日终特别多的传票统计数据处理程序失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
int   openfile( tmpjgbm )
char   tmpjgbm[10];
{
	char	fullname[100];
		memset(fullname,0x00,sizeof(fullname) );
		sprintf( fullname,"%s/report/%s/BRRPTOTSTT.txt",getenv("HOME"),tmpjgbm);
		if( (fp=fopen(fullname,"w"))==NULL )
		{
			sprintf( fullname,"%s/report/%s",getenv("HOME"),tmpjgbm);
			mkdir( fullname, S_IRWXU|S_IRWXG|S_IRWXO);
			sprintf( fullname,"%s/report/%s/BRRPTOTSTT.txt",getenv("HOME"),tmpjgbm);
			if( (fp=fopen(fullname,"w"))==NULL )
			{
				printf("open file failed,filename=%s\n",fullname);
				return(1);
			}
		}
	return(0);
}
/* ------------- draw_head1() ----------------- */
int   draw_head1(fp)
FILE    *fp;
{
	return 0;
	fprintf( fp,"                              市商业银行批量记帐凭证\n");
	/** YUDONG
	exec sql select cha_name into :tmpjgmc from COM_BRANCH where brno=:RPT_NOTE0.brno; 
	if( sqlca.sqlcode ) 
	{
		printf("select from COM_BRANCH failed,brno=%s\n",RPT_NOTE0.brno);
		return(1);
	}
	fprintf(fp,"                                %s\n\n",tmpjgmc);
	***/

  return(0);
}
/* ----------------- draw_head2(fp) ------------------- */
int  draw_head2(fp)
FILE	*fp;
{
	struct	com_cur_no_code_c  no_code;
	char   tmpvar[200];
	int		ret=0;

	memset( &no_code,0x00,sizeof(struct com_cur_no_code_c));
	fprintf(fp,"\n@BIG@@MID@");

	if( RPT_NOTE0.dc_ind[0]=='1' )
		fprintf(fp,"借方凭证");
	else 
		fprintf(fp,"贷方凭证");

	fprintf(fp,"@NOBIG@\n\n");

	ret = Com_cur_no_code_Sel(g_pub_tx.reply,&no_code,"cur_no='%s'",RPT_NOTE0.cur_no);
	if( ret )
	{
		strcpy( no_code.cur_no_name,"????" );
	}
	ldchar( no_code.cur_no_name,strlen(no_code.cur_no_name),no_code.cur_no_name );
	/*sprintf( tmpvar," 币种:%s",tmpbzmc);*/
	sprintf( tmpvar," 机构编码：%s",RPT_NOTE0.tx_tx_br_no);
	draw_string( fp, tmpvar,36,'0' );
	fprintf(fp,"%4d年%02d月%02d日                     序号:%06d\n",
		RPT_NOTE0.tx_date/10000,RPT_NOTE0.tx_date%10000/100,
		RPT_NOTE0.tx_date%100,RPT_NOTE0.trace_no );
	draw_line(fp,b1,1);
	draw_line(fp,b3,10);
	draw_line(fp,b7,1);
	draw_line(fp,b3,20);
	draw_line(fp,b7,1);
	draw_line(fp,b3,9);
	draw_line(fp,b2,1);
	fprintf(fp,"\n");
	sprintf(tmpvar,"%s    帐        号    %s           户              名           %s    金      额    %s\n",b4,b4,b4,b4 );
	fprintf( fp, tmpvar);
	draw_line(fp,b5,1);
	draw_line(fp,b3,10);
	draw_line(fp,b9,1);
	draw_line(fp,b3,20);
	draw_line(fp,b9,1);
	draw_line(fp,b3,9);
	draw_line(fp,b6,1);
	fprintf(fp,"\n");

   return(0);
}
/* ----------------- draw_head3(fp) ------------------- */
int  draw_head3(fp)
FILE	*fp;
{
	char   tmpvar[200];
	struct	com_cur_no_code_c  no_code;
	int		ret;

	fprintf(fp,"                                  客户回单\n");
	ret = Com_cur_no_code_Sel(g_pub_tx.reply,&no_code,"cur_no='%s'",RPT_NOTE0.cur_no);
	if( ret )
	{
		strcpy( no_code.cur_no_name,"????" );
	}
	ldchar( no_code.cur_no_name,strlen(no_code.cur_no_name),no_code.cur_no_name );
	/*sprintf( tmpvar," 币种:%s",tmpbzmc);*/
	sprintf( tmpvar," ");
	draw_string( fp, tmpvar,36,'0' );
	fprintf(fp,"%4d年%02d月%02d日                         流水号:%06d\n",
		RPT_NOTE0.tx_date/10000,RPT_NOTE0.tx_date%10000/100,
		RPT_NOTE0.tx_date%100,RPT_NOTE0.trace_no );
	draw_line(fp,b1,1);
	draw_line(fp,b3,10);
	draw_line(fp,b7,1);
	draw_line(fp,b3,20);
	draw_line(fp,b7,1);
	draw_line(fp,b3,9);
	draw_line(fp,b2,1);
	fprintf(fp,"\n");
	sprintf(tmpvar,"%s    帐        号    %s           户              名           %s    金      额    %s\n",b4,b4,b4,b4 );
	fprintf( fp, tmpvar);
	draw_line(fp,b5,1);
	draw_line(fp,b3,10);
	draw_line(fp,b9,1);
	draw_line(fp,b3,20);
	draw_line(fp,b9,1);
	draw_line(fp,b3,9);
	draw_line(fp,b6,1);
	fprintf(fp,"\n");

   return(0);
}
/* ---------------- draw_body(fp) --------------- */
int draw_body(fp)
FILE   *fp;
{
	struct	com_item_c		g_com_item;
	double	amt;
	char   tmpvar[200];
	int    i,  j,   pos,ret;

	memset(&g_com_item,0x00,sizeof(struct com_item_c));

/* acno,name,amt */
		draw_line(fp,b4,1);
		draw_string(fp,RPT_NOTE0.ac_no,20,'0');
		draw_line(fp,b4,1);
		draw_string(fp,RPT_NOTE0.name,40,'0');
		draw_line(fp,b4,1);

		draw_double(fp,RPT_NOTE0.amt,18,'R');

		draw_line(fp,b4,1);
		fprintf(fp,"\n");
/* 划线  */ 
	draw_line(fp,b5,1);
	draw_line(fp,b3,10);
	draw_line(fp,b8,1);
	draw_line(fp,b3,14);
	draw_line(fp,b7,1);
	draw_line(fp,b3,5);
	draw_line(fp,b9,1);
	draw_line(fp,b3,9);
	draw_line(fp,b6,1);
	fprintf(fp,"\n");
/* 摘要行 */
	fprintf( fp, "%s摘要：",b4); pos=0;
	for(i=0;i<40;i++)
	{
		if( RPT_NOTE0.summary[i]=='|' || RPT_NOTE0.summary[i]==' ') { i++; break; }
		tmpvar[pos++]=RPT_NOTE0.summary[i];
	}
	if( pos==0 ) strcpy( tmpvar,"  ");
	else   tmpvar[pos]=0x00;
	draw_string( fp,tmpvar,44,'0' );
	fprintf( fp,"│          │");
	/*根据科目号查询科目名称*/
	ret=Com_item_Sel(g_pub_tx.reply,&g_com_item,"acc_hrt='%s'",RPT_NOTE0.acc_hrt);
 	if( ret==100 )
 	{
	 	sprintf(acErrMsg,"该科目号不存在!!acc_hrt=[%s]", g_in_parm.acc_hrt);
	 	WRITEMSG
	 	strcpy(g_pub_tx.reply,"O074");
	 	goto ErrExit;
 	}
 	else if( ret )
 	{
	 	sprintf( acErrMsg, "查询公共科目表异常!!ret=[%d]", ret );
	 	WRITEMSG
	 	goto ErrExit;
 	}

	ldchar(g_com_item.acc_no,strlen(g_com_item.acc_no),g_com_item.acc_no);
	fprintf( fp,"                  │\n");
	/**
	draw_string(fp,g_com_item.acc_no,18,'0');
	**/
	draw_line(fp,b4,1);
	fprintf(fp,"                                                  │          │                  │\n");
/* 摘要后的第一行  */
	draw_line(fp,b4,1);
	memset( tmpvar,' ',20);    pos=5;
    for( j=i;j<i+40;j++ )
	{
			if( RPT_NOTE0.summary[j]=='|' ||  RPT_NOTE0.summary[j]==' ' ) {  j++;    break; }
		tmpvar[pos++]=RPT_NOTE0.summary[j];
	}
    tmpvar[pos]=0x00;
    draw_string( fp,tmpvar,50,'0' );

	fprintf( fp,"│科  目  号│%-18s│\n",g_com_item.acc_no);
/* ----------- */
	draw_line(fp,b4,1);
	memset( tmpvar,' ',20 );     pos=5;
    for(i=j;i<j+40;i++ ) 
	{
		if( RPT_NOTE0.summary[i]==' ' ) break;
		tmpvar[pos++]=RPT_NOTE0.summary[i];
	}
	tmpvar[pos]=0x00; tmpvar[49]=0x00;
	draw_string(fp,tmpvar,50,'0');
	fprintf( fp,"│          │");
	strcpy(tmpvar," ");
	draw_string(fp,tmpvar,18,'0');
	draw_line(fp,b4,1);
	fprintf(fp,"\n");

	return(0);
ErrExit:
	return 1;
}

/* ----------------- draw_end() ---------------- */
int  draw_end(fp)
FILE 	*fp;
{
  char  tmpvar[200];
	fprintf( fp, "│                                                  │          │                  │\n");
	fprintf( fp, "│                                                  │          │                  │\n");
	fprintf( fp, "└─────────────────────────┴─────┴─────────┘\n");
	fprintf( fp,"                  经办：                  复核：                   制单：\n\n");

 return(0);
}
