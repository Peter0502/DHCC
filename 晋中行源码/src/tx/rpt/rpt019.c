/*************************************************************
* 文 件 名: rpt019.c
* 功能描述：科目XXXX单
*
**************************************************************/
#define MYERR if( ret ){\
	sprintf(acErrMsg,"return error[%d]",ret );\
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
#include "dc_log_bk_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "com_item_c.h"
#include "gl_mst_c.h"

struct dc_log_bk_c dc_log_c, dc_log_c_old;
struct com_sys_parm_c com_sys_parm_c;
struct com_branch_c com_branch_c;
struct com_item_c com_item_c;
struct gl_mst_c gl_mst_c;
int 	pzs=0;
	FILE 	*fp;
	int 	line;
	
int 	rpt_code;
int hzbz=0;
int 	xjjbshj,xjdbshj,zzdbshj,zzjbshj,jbshj,dbshj,hzbz,spe_ind;
 	double 	xjjfsehj,xjdfsehj,zzjfsehj,zzdfsehj,jfsehj,dfsehj;
 	double 	zrjyehj,zrdyehj,jrjyehj,jrdyehj;
	int vxh;
 	
int Get_ratio( int bli,int bll, char str[100], int rpt_code );
rpt019()
{
	int ret = 0;
	char fullname[100];
	int flag=0;
	int opncnt;
	char vpfm[21];
	
	memset(&dc_log_c, 0x0, sizeof(dc_log_c));
	memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
	memset(&dc_log_c_old, 0x0, sizeof( dc_log_c));
	memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));
	memset(&com_item_c, 0x0, sizeof(struct com_item_c));

    ret=sql_begin(); /*打开事务*/
	MYERR

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

    /* 取前一天日期 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	MYERR

    g_pub_tx.tx_date = com_sys_parm_c.lst_date;

	/* 删除科目日结单报表 */
	/*ret = pub_rpt_rmfile_all("RPT001");err*/
	ret = pub_rpt_rmfile_all("RPT019");
	if (ret != 0)
	{
		sprintf(acErrMsg,"删除科目日结单报表错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"R001");
		goto ErrExit;
	}

    ret = pub_pubdb_sumdclog_Dec();
	MYERR

	flag=0;       
	pzs=0;
	hzbz=spe_ind=0;
	vxh=0;

	while(1)
	{
		ret = pub_pubdb_sumdclog_Fet(&dc_log_c, g_pub_tx.reply);
		if( ret==1403 ) break;
		MYERR
    
vtcp_log("CCC %s,%s [%d]",dc_log_c_old.tx_tx_br_no,dc_log_c_old.acc_hrt,dc_log_c.trace_cnt );
		if ( flag==0 )
		{
			flag=1;
			memcpy( &dc_log_c_old,&dc_log_c,sizeof(dc_log_c) );
				ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
					dc_log_c.tx_tx_br_no);
				MYERR
		}

		if ( strcmp(dc_log_c.tx_tx_br_no,dc_log_c_old.tx_tx_br_no) ||
			strcmp(dc_log_c.acc_hrt,dc_log_c_old.acc_hrt) )
		{
			hzbz=0;
vtcp_log("%s,%s [%d]",dc_log_c_old.tx_tx_br_no,dc_log_c_old.acc_hrt,gl_mst_c.rdd_cnt );
			write_one();

			if ( strcmp(dc_log_c.tx_tx_br_no,dc_log_c_old.tx_tx_br_no) )
			{
				hzbz=1;
				/**********科目单汇总******/
				strcpy( gl_mst_c.br_no,dc_log_c_old.tx_tx_br_no );
				write_one();
				memset( &gl_mst_c,0,sizeof(gl_mst_c) );
					pzs=0;
					xjjbshj=xjdbshj=zzdbshj=zzjbshj=jbshj=dbshj=0;
 					xjjfsehj=xjdfsehj=zzjfsehj=zzdfsehj=jfsehj=dfsehj=0.00;
 					zrjyehj=zrdyehj=jrjyehj=jrdyehj=0.00;
				ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
					dc_log_c.tx_tx_br_no);
				MYERR
				vxh=0;
			}
			memset( &gl_mst_c,0,sizeof(gl_mst_c) );
		}

		if( dc_log_c.dc_ind[0]=='1' )
		{
			gl_mst_c.rdd_amt+=dc_log_c.amt;
			gl_mst_c.rdd_cnt+=dc_log_c.trace_cnt;
			if( dc_log_c.ct_ind[0]=='1' )
			{
				gl_mst_c.cdd_amt+=dc_log_c.amt;
				gl_mst_c.cdd_cnt+=dc_log_c.trace_cnt;
			}
		}
		if( dc_log_c.dc_ind[0]=='2' )
		{
			gl_mst_c.rcd_amt+=dc_log_c.amt;
			gl_mst_c.rcd_cnt+=dc_log_c.trace_cnt;
			if( dc_log_c.ct_ind[0]=='1' )
			{
				gl_mst_c.ccd_amt+=dc_log_c.amt;
				gl_mst_c.ccd_cnt+=dc_log_c.trace_cnt;
			}	
		}
vtcp_log("DD%s,%s [%d]",dc_log_c_old.tx_tx_br_no,dc_log_c_old.acc_hrt,gl_mst_c.rdd_cnt );

		memcpy( &dc_log_c_old,&dc_log_c,sizeof(dc_log_c) );
	}
		if ( flag )
		{
			hzbz=0;
			write_one(dc_log_c_old);
			hzbz=1;
			write_one(dc_log_c_old);
		}

	pub_pubdb_sumdclog_Clo();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"生成科目日结单成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
    if (strcmp(g_pub_tx.reply,"0000") == 0)
    {
        strcpy(g_pub_tx.reply,"G011");
    }
	sprintf(acErrMsg,"生成科目日结单失败!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

/***制表***/
int Get_ratio( bli,bll,str,rpt_code )
 int bli,bll;
 char str[100];
 int rpt_code;
{
	switch(rpt_code)
	{
		case 0001:
			if( get_ratio_kmgzd(bli,bll,str) ) goto ErrExit;
			break;
		default:
			goto ErrExit;
	}
GoodExit:
	return 0;
ErrExit:
	return 1;
}
int get_ratio_kmgzd( bli,bll,str )
 int bli,bll;
 char str[100];
{
	char vhm[101];
	char vstr[101];
	char	l_kmm[31];
	char fmt[10];
	int i=0;
	int ret = 0;

	switch( bli )
	{
		case '1':
			break;
		case '2':
			memset( str,' ',bll );
			ldchar( com_branch_c.br_name,strlen(com_branch_c.br_name),com_branch_c.br_name );
			i=strlen(com_branch_c.br_name);
			strncpy( str,com_branch_c.br_name,i );
			strncpy( str+i,"(",1 );
			i++;
			strncpy( str+i,com_branch_c.br_no,5 );
			i=i+5;
			strncpy( str+i,")",1 );
			break;
		case '3':
				sprintf( fmt,"%%-%ds",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,"表内汇总" );
			else
			{
				ret = Com_item_Sel(g_pub_tx.reply,&com_item_c,"acc_hrt='%s'",
			  		dc_log_c_old.acc_hrt);
        		if (ret != 0)
				{
					sprintf(acErrMsg,"查询科目表错误!! [%d][%s]",
						ret,dc_log_c_old.acc_hrt);
					WRITEMSG
					goto ErrExit;
				}
				strcpy(vhm,com_item_c.acc_no);
				ldchar( vhm,strlen(vhm),vhm );

				sprintf( str,fmt,vhm );
			}
			break;
		case '4':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,g_pub_tx.tx_date/10000 );
			break;
		case 'A':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,g_pub_tx.tx_date%10000/100 );
			break;
		case 'B':
			sprintf( fmt,"%%0%dd",bll );
			sprintf( str,fmt,g_pub_tx.tx_date%100 );
			break;
		case '5':
			if ( hzbz==1 )
			{
				strcpy(l_kmm, "");
			}
			else
			{
				memset(l_kmm, 0x00, sizeof(l_kmm));
				ret = Com_item_Sel(g_pub_tx.reply,&com_item_c,"acc_hrt='%s'",
			  		dc_log_c_old.acc_hrt);
        		if (ret != 0)
				{
					sprintf(acErrMsg,"查询科目表错误!! [%d]",ret);
					WRITEMSG
					goto ErrExit;
				}
			
				strcpy(l_kmm, com_item_c.acc_name);
			}
			strcpy(l_kmm,pub_base_strpack(l_kmm));
			sprintf( fmt,"%%-%ds",bll );
			sprintf( str,fmt,l_kmm );
		
			break;
		case '6':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case '7':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjjbshj );
			else
				sprintf( str,fmt,(long)gl_mst_c.cdd_cnt );

			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				xjjbshj=xjjbshj+gl_mst_c.cdd_cnt;
			break;
		case '8':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjjfsehj );
			else
				sprintf( str,fmt,gl_mst_c.cdd_amt );

			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				xjjfsehj=xjjfsehj+gl_mst_c.cdd_amt;
			break;
		case '9':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case 'U':
			sprintf( fmt,"%%%dd",bll );
			vxh++;
			sprintf( str,fmt,vxh );
			break;
		case 'a':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjdbshj );
			else
				sprintf( str,fmt,gl_mst_c.ccd_cnt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				xjdbshj=xjdbshj+gl_mst_c.ccd_cnt;
			break;
		case 'b':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjdfsehj );
			else
				sprintf( str,fmt,gl_mst_c.ccd_amt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				xjdfsehj=xjdfsehj+gl_mst_c.ccd_amt;
			break;
		case 'c':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case 'd':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zzjbshj );
			else
				sprintf( str,fmt,gl_mst_c.rdd_cnt-gl_mst_c.cdd_cnt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				zzjbshj=zzjbshj+(gl_mst_c.rdd_cnt-gl_mst_c.cdd_cnt);
			break;
		case 'e':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zzjfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rdd_amt-gl_mst_c.cdd_amt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				zzjfsehj=zzjfsehj+(gl_mst_c.rdd_amt-gl_mst_c.cdd_amt);
			break;
		case 'f':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case 'g':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zzdbshj );
			else
				sprintf( str,fmt,gl_mst_c.rcd_cnt-gl_mst_c.ccd_cnt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				zzdbshj=zzdbshj+(gl_mst_c.rcd_cnt-gl_mst_c.ccd_cnt);
			break;
		case 'h':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zzdfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rcd_amt-gl_mst_c.ccd_amt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				zzdfsehj=zzdfsehj+(gl_mst_c.rcd_amt-gl_mst_c.ccd_amt);
			break;
		case 'o':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case 'p':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,jbshj );
			else
				sprintf( str,fmt,gl_mst_c.rdd_cnt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				jbshj=jbshj+gl_mst_c.rdd_cnt;
			break;
		case 'q':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,jfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rdd_amt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				jfsehj=jfsehj+gl_mst_c.rdd_amt;
			break;
		case 'r':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case 's':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,dbshj );
			else
				sprintf( str,fmt,gl_mst_c.rcd_cnt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				dbshj=dbshj+gl_mst_c.rcd_cnt;
			break;
		case 't':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,dfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rcd_amt );
			if ( dc_log_c_old.acc_hrt[3]=='0' && dc_log_c_old.acc_hrt[4]=='0' \
				 && dc_log_c_old.acc_hrt[0]<'6' )
				dfsehj=dfsehj+gl_mst_c.rcd_amt;
			break;
		case 'u':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zrjyehj );
			else
			{
				sprintf( str,fmt,gl_mst_c.ldd_bal );
			}
			break;
		case 'v':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zrdyehj );
			else
			{
				sprintf( str,fmt,gl_mst_c.lcd_bal );
			}
			break;
		case 'w':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,jrjyehj );
			else
			{
				sprintf( str,fmt,gl_mst_c.dr_bal );
			}
			break;
		case 'x':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,jrdyehj );
			else
			{
				sprintf( str,fmt,gl_mst_c.cr_bal );
			}
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
write_one( )
{
	int ret=0;
	char vpfm[21];
	int opncnt=0;

		/* 查询科目表 */
		ret = Com_item_Sel(g_pub_tx.reply,&com_item_c,"acc_hrt='%s'",
		  dc_log_c_old.acc_hrt);
    	if ( ret != 0 && ret != 1403)
		{
			sprintf(acErrMsg,"查询科目表错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 1403)
		{
			sprintf(acErrMsg,"没有该科目! [%s]",dc_log_c_old.acc_hrt);
			WRITEMSG
			goto ErrExit;
		}

		if ( strcmp(com_item_c.acc_lvl,"1") != 0 && \
			 strcmp(com_item_c.acc_lvl,"2") != 0 && \
			 strcmp(com_item_c.acc_lvl,"3") != 0)
        {
			goto GoodExit;
		}

		if( pub_rpt_openfile( &fp,dc_log_c_old.tx_tx_br_no,"RPT019")  )
		{
			sprintf(acErrMsg,"open file failed,return \n");
			WRITEMSG
			goto ErrExit;
		}

		rpt_code=0001;

		if( hzbz==1 )
			strcpy(vpfm,"RPT019");
		else if( dc_log_c_old.acc_hrt[0]<='5' )
			strcpy(vpfm,"RPT019");
		else
			strcpy(vpfm,"RPT0192");
			
		ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,rpt_code,"00",
			  &opncnt,Get_ratio);
        if (ret != 0)
		{
			sprintf(acErrMsg,"生成科目日结单错误!!");
			WRITEMSG
			goto ErrExit;
		}

		pzs++;

		/*if( pzs%3==0 )晋中2*/
		if( pzs%2 == 0 )
		{
				fprintf(fp,"\f\n");
		}
		else
		{
			pub_rpt_empty(opncnt,fp);
			fprintf(fp,"- - - - - - - - - - - - - - -  - - - - - - -\n");
		}
		
		fclose(fp);

GoodExit:
	return 0;
ErrExit:
	return 1;
}
