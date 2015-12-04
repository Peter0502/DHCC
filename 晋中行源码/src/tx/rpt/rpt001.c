/*************************************************************
* 文 件 名: rpt001.c
* 功能描述：科目日结单
*
* 作    者: rob
* 完成日期: 2003年4月13日
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
#define EXTERN
#include "public.h"
#include "gl_mst_c.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "com_item_c.h"

struct gl_mst_c gl_mst_c, gl_mst_c_old;
struct com_sys_parm_c com_sys_parm_c;
struct com_branch_c com_branch_c;
struct com_item_c com_item_c;
int 	pzs=0;
	FILE 	*fp;
	int 	line;
	
int 	rpt_code;
int 	xjjbshj,xjdbshj,zzdbshj,zzjbshj,jbshj,dbshj,hzbz,spe_ind;
 	double 	xjjfsehj,xjdfsehj,zzjfsehj,zzdfsehj,jfsehj,dfsehj;
 	double 	zrjyehj,zrdyehj,jrjyehj,jrdyehj;
	int vxh=0;
 	
int Get_ratio( int bli,int bll, char str[100], int rpt_code );
rpt001()
{
	int ret = 0;
	char fullname[100];
	int flag=0;
	int opncnt;
	char vpfm[21];
	
	memset(&gl_mst_c, 0x0, sizeof(struct gl_mst_c));
	memset(&gl_mst_c_old, 0x0, sizeof(struct gl_mst_c));
	memset(&com_sys_parm_c, 0x0, sizeof(struct com_sys_parm_c));
	memset(&com_branch_c, 0x0, sizeof(struct com_branch_c));
	memset(&com_item_c, 0x0, sizeof(struct com_item_c));

    ret=sql_begin(); /*打开事务*/
    if(ret != 0)
    {
        sprintf( acErrMsg, "打开事务失败!!!" );
        WRITEMSG
        goto ErrExit;
    }

    /**------- 初始化全局变量 --------**/
    pub_base_sysinit();

    /* 取前一天日期 */
    ret = Com_sys_parm_Sel(g_pub_tx.reply,&com_sys_parm_c,"1=1");
	if (ret != 0)
	{
		sprintf(acErrMsg,"取系统参数表错误!! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

    g_pub_tx.tx_date = com_sys_parm_c.lst_date;


	/* 删除科目日结单报表 */
	ret = pub_rpt_rmfile_all("RPT001");/*对com_branch表进行操作,查询并删除日结单报表(公共机构码表)*/
	if (ret != 0)
	{
		sprintf(acErrMsg,"删除科目日结单报表错误!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"R001");
		goto ErrExit;
	}

    ret = Gl_mst_Dec_Sel(g_pub_tx.reply,"\"date\"=%ld and cur_no <> '00' \
		  order by date,br_no,cur_no,acc_hrt", g_pub_tx.tx_date);
    if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR SELECT GL_MST ERROR [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	flag=0;       
	pzs=0;
	strcpy( gl_mst_c_old.br_no,"" );
	hzbz=spe_ind=0;
			vxh=0;

	while(1)
	{
		ret = Gl_mst_Fet_Sel(&gl_mst_c, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH FOR SELECT GL_MST ERROR [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			/**********科目日结单汇总******/
			hzbz=1;
			/*
			if( pub_rpt_openfile( &fp,gl_mst_c_old.br_no,"BRkmrjd")  )
			*/
			if( pub_rpt_openfile( &fp,gl_mst_c_old.br_no,"RPT001")  )
			{
				sprintf(acErrMsg,"open file failed,return \n");
				WRITEMSG
				strcpy(g_pub_tx.reply,"R002");
				goto ErrExit;
			}
			rpt_code=0001;

			/*strcpy(vpfm,"kmrjd");*/
			strcpy(vpfm,"RPT001");

			ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,rpt_code,"00",
				  &opncnt,Get_ratio);
            if (ret != 0)
			{
				sprintf(acErrMsg,"生成日结单汇总错误!!");
				WRITEMSG
				goto ErrExit;
			}

			pzs++;

			if( pzs%3==0 )
				fprintf(fp,"@HY@");
			else
				pub_rpt_empty(opncnt,fp);

			fclose(fp);
			hzbz=0;
			break;
		}
       
		/* 查询科目表 */
		ret = Com_item_Sel(g_pub_tx.reply,&com_item_c,"acc_hrt='%s'",
			  gl_mst_c.acc_hrt);
        if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"查询科目表错误!! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"没有该科目! [%s]",gl_mst_c.acc_hrt);
			WRITEMSG
			continue;
		}

		if ( strcmp(com_item_c.acc_lvl,"1") != 0 && \
			 strcmp(com_item_c.acc_lvl,"2") != 0 && \
			 strcmp(com_item_c.acc_lvl,"3") != 0)
        {
			continue;
		}

		strcpy(gl_mst_c.br_no,pub_base_strpack(gl_mst_c.br_no));

		if (strcmp(gl_mst_c_old.br_no,gl_mst_c.br_no))
		{
			/**********科目日结单汇总******/
			spe_ind++;
			if( spe_ind!=1 ) hzbz=1;
			if( hzbz==1 ) 
			{
			    /*if( pub_rpt_openfile( &fp,gl_mst_c_old.br_no,"BRkmrjd")  )*/
			    if( pub_rpt_openfile( &fp,gl_mst_c_old.br_no,"RPT001")  )
			    {
				    sprintf(acErrMsg,"open file failed,return \n");
				    WRITEMSG
				    goto ErrExit;
			    }
			    rpt_code=0001;
			    strcpy(vpfm,"kmrjd");
			    strcpy(vpfm,"RPT001");

			    ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,rpt_code,"00",
					  &opncnt,Get_ratio);
                if (ret != 0)
				{
					sprintf(acErrMsg,"汇总科目日结单错误!!");
					WRITEMSG
				    goto ErrExit;
				}
			    pzs++;

			    if( pzs%3==0 )
				    fprintf(fp,"\f\n");
			    else
				    pub_rpt_empty(opncnt,fp);
			    fclose(fp);
			}
			hzbz=0;
			/*******************end******/
			
			ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
				  gl_mst_c.br_no);
            if (ret != 0)
			{
				sprintf(acErrMsg,"查询机构编码表错误!! [%d]",ret);
				WRITEMSG
				goto ErrExit;
			}

			pzs=0;
			xjjbshj=xjdbshj=zzdbshj=zzjbshj=jbshj=dbshj=0;
 			xjjfsehj=xjdfsehj=zzjfsehj=zzdfsehj=jfsehj=dfsehj=0.00;
 			zrjyehj=zrdyehj=jrjyehj=jrdyehj=0.00;

			strcpy( gl_mst_c_old.br_no,gl_mst_c.br_no );
			vxh=0;
		}

		if( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
			&& gl_mst_c.acc_hrt[0]<'6' )
		{
 		    zrjyehj+=gl_mst_c.ldd_bal;
 		    zrdyehj+=gl_mst_c.lcd_bal;
			jrjyehj+=gl_mst_c.dr_bal;
			jrdyehj+=gl_mst_c.cr_bal;
		}
		if( gl_mst_c.rdd_cnt==0 && gl_mst_c.rcd_cnt==0 ) continue;

		/*if( pub_rpt_openfile( &fp,gl_mst_c.br_no,"BRkmrjd")  )*/
		if( pub_rpt_openfile( &fp,gl_mst_c.br_no,"RPT001")  )
		{
			sprintf(acErrMsg,"open file failed,return \n");
			WRITEMSG
			goto ErrExit;
		}

		rpt_code=0001;

		if( gl_mst_c.acc_hrt[0]<='5' )
			/*strcpy(vpfm,"kmrjd");*/
			strcpy(vpfm,"RPT001");
		else
			/*strcpy(vpfm,"kmrjd2");*/
			strcpy(vpfm,"RPT0011");
			
		ret = pub_rpt_read_pfm_qd(fp,&line,vpfm,rpt_code,"00",
			  &opncnt,Get_ratio);
        if (ret != 0)
		{
			sprintf(acErrMsg,"生成科目日结单错误!!");
			WRITEMSG
			goto ErrExit;
		}

		pzs++;

		if( pzs%3==0 )
			fprintf(fp,"\f\n");
		else
		{
			pub_rpt_empty(opncnt,fp);
			fprintf(fp,"- - - - - - - - - - - - - - -  - - - - - - -\n");
		}
		
		fclose(fp);
	}

	Gl_mst_Clo_Sel();

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
			if( get_ratio_kmrjd(bli,bll,str) ) goto ErrExit;
			break;
		default:
			goto ErrExit;
	}
GoodExit:
	return 0;
ErrExit:
	return 1;
}

int get_ratio_kmrjd( bli,bll,str )
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
			  		gl_mst_c.acc_hrt);
        		if (ret != 0)
				{
					sprintf(acErrMsg,"查询科目表错误!! [%d][%s]",
						ret,gl_mst_c.acc_hrt);
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
			  		gl_mst_c.acc_hrt);
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
				sprintf( str,fmt,gl_mst_c.cdd_cnt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				xjjbshj=xjjbshj+gl_mst_c.cdd_cnt;
			break;
		case '8':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjjfsehj );
			else
				sprintf( str,fmt,gl_mst_c.cdd_amt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				xjjfsehj=xjjfsehj+gl_mst_c.cdd_amt;
			break;
		case '9':
			sprintf( fmt,"%%%ds",bll );
			sprintf( str,fmt,"" );
			break;
		case 'a':
			sprintf( fmt,"%%%dd",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjdbshj );
			else
				sprintf( str,fmt,gl_mst_c.ccd_cnt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				xjdbshj=xjdbshj+gl_mst_c.ccd_cnt;
			break;
		case 'b':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,xjdfsehj );
			else
				sprintf( str,fmt,gl_mst_c.ccd_amt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
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
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				zzjbshj=zzjbshj+(gl_mst_c.rdd_cnt-gl_mst_c.cdd_cnt);
			break;
		case 'e':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zzjfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rdd_amt-gl_mst_c.cdd_amt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
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
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				zzdbshj=zzdbshj+(gl_mst_c.rcd_cnt-gl_mst_c.ccd_cnt);
			break;
		case 'h':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,zzdfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rcd_amt-gl_mst_c.ccd_amt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
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
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				jbshj=jbshj+gl_mst_c.rdd_cnt;
			break;
		case 'q':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,jfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rdd_amt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
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
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				dbshj=dbshj+gl_mst_c.rcd_cnt;
			break;
		case 't':
			sprintf( fmt,"%%%d.2lf",bll );
			if ( hzbz==1 )
				sprintf( str,fmt,dfsehj );
			else
				sprintf( str,fmt,gl_mst_c.rcd_amt );
			if ( gl_mst_c.acc_hrt[3]=='0' && gl_mst_c.acc_hrt[4]=='0' \
				 && gl_mst_c.acc_hrt[0]<'6' )
				dfsehj=dfsehj+gl_mst_c.rcd_amt;
			break;
		case 'U':
			sprintf( fmt,"%%%dd",bll );
			vxh++;
			sprintf( str,fmt,vxh );
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
