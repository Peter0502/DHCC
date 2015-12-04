/*************************************************
* 文 件 名:  spG501.c
* 功能描述： 用来检查日终处理顺序表，用来打开关闭数据库，做各种合法性检查，然后
*		 调用各个具体的日终处理程序，并执行。 
*	     
*
* 作    者:  jane
* 完成日期： 2003年1月13日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "doc_dic_c.h"
#include "data_dic_c.h"
#include "tx_def_c.h"
#include "tx_flow_def_c.h"
#include "tx_sub_def_c.h"

struct S_fied_filter
{
	int seqn;
	char fldname[21];
	char comp[3];
	int tim;
	char data_code[5];
	int leng;
	int dec;
};
struct S_fied_qry
{
	int seqn;
	char fldname[21];
	char valname[21];
	int beg;
	int end;
};
	struct S_fied_filter vfiedfilter[128]; /**条件字段列表**/
	struct doc_dic_c vfiedfilter_attr[128];
	int vfiedfilter_i;
	struct S_fied_qry vfiedqry[128]; /**需要查询的字段列表**/
	struct doc_dic_c vfiedqry_attr[128];
	int vfiedqry_i;
	int vtim=0;
	char vtxcode[5]; /*交易代码*/
	char vmenucode[5]; /*菜单代码*/
	char vupmenu[5]; /*上级菜单*/
	char vscrcode[5]; /*屏幕代号*/
	char vtxname[21]; /*交易名称*/
	char vtabname[21]; /*表名*/
	char vBtabname[21]; /*表名：首字母大写*/
	char vtype[11]; 
	char vpoint[11]; 
	char vfied[21];
	char vdatacode[5];
	char fiedlist[1024];

	int ac_seqn_flag=0;

int spG501()
{
	int i=0;

	memset( fiedlist,0,sizeof(fiedlist) );

	get_zd_data("0580", vtxcode );
	get_zd_data("0590", vmenucode );
	get_zd_data("0600", vupmenu );
	get_zd_data("0610", vscrcode );
	get_zd_data("0620", vtxname );
	get_zd_data("0630", vtabname );
	strcpy( vBtabname,vtabname );
	vBtabname[0]+='A'-'a';

TRACE
	sub_wrt_tx_def();

TRACE
	sub_read_file();

TRACE
	sub_get_val_attr();

TRACE
 	sub_make_pgm();

	/***/
		for( i=0;i<vfiedqry_i;i++ )
		{
			vtcp_log("Q %02d-%s",vfiedqry[i].seqn,vfiedqry[i].fldname);
		}
		for( i=0;i<vfiedfilter_i;i++ )
		{
			vtcp_log("F %02d-%s,%s",vfiedfilter[i].seqn,
				vfiedfilter[i].fldname,vfiedfilter[i].comp);
		}
	/****/

TRACE
	sub_wrt_yjg();
TRACE

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	commit_work();
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
sub_read_file()
{
	int ret=0;
	char filename[100];
	char tmpstr[1024];
	char tmpfld[128];
	int i=0;
	int j=0;
	int k=0;
	int m=0;
	FILE *fp;

	Gl_file_first=0;
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"r" );
	if( fp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	while(1)
	{
		fgets( tmpstr,1024,fp );
		if( feof(fp) ) break;
vtcp_log(" tmp[%s]",tmpstr);
		m=0;
		for( i=1;i<=4;i++ )
		{
			ret=pub_base_cut_str( tmpstr,tmpfld,"|",i );
			if( i==1 ) 
			{
				tmpfld[2]='\0';
				vfiedqry[j].seqn=atoi(tmpfld);
				vfiedfilter[k].seqn=atoi(tmpfld);
			}
			else if( i==2 ) 
			{
				if( tmpfld[0]=='0' )
				{
					m=1;
				}
				else
				{
					m=2;
				}
			}
			else if( i==3 )
			{
				if( m==1 )
				{
					strcpy( vfiedfilter[k].fldname,tmpfld );
				}
				else
				{
					strcpy( vfiedqry[j].fldname,tmpfld );
				}
			}
			else if( i==4 )
			{
				if( m==1 )
				{
					strcpy( vfiedfilter[k].comp,tmpfld );
				}
			}
		}

		if( m==1 ) 
		{
			vfiedfilter[k].tim=0;
			for( i=0; i<k; i++ )
			{
				if( !strcmp(vfiedfilter[i].fldname,vfiedfilter[k].fldname) )
					vfiedfilter[k].tim++;
			}
			if( vfiedfilter[k].tim>vtim )
			{
				vtim=vfiedfilter[k].tim;
			}
			k++;
		}
		else j++;

	}
	fclose(fp);


	vfiedfilter_i=k;
	vfiedqry_i=j;

	return 0;
ErrExit:
	return 1;
}
sub_make_pgm()
{
	FILE *mfp;
	FILE *wfp;
	char mfile[100]; /**模板文件**/
	char wfile[100]; /**程序文件**/
	char tmpstr[1024];
	char tmpstra[1024];
	char titstr[1024];
	int lines=0;
	int i;
	char tb[2];

	sprintf( mfile,"%s/pfm/qry_model.c",getenv("HOME") );
	sprintf( wfile,"%s/sp%s.c",getenv("QRYSRC"),vtxcode );
vtcp_log("WFILE[%s]",wfile);

	mfp=fopen( mfile,"r" );
	if( mfp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",mfile );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	wfp=fopen( wfile,"w" );
	if( wfp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",wfile );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	while(1)
	{
		fgets( tmpstr,1024,mfp );
		if( feof(mfp) ) break;

		lines++;

		if( lines==1 ) continue; /*第一行是说明*/

		if( !strncmp(tmpstr,"[-",2) ) /**定义变量**/
		{
			for( i=0;i<vtim;i++ )
			{
				tb[0]='a'+i;
				strcpy( tmpstra,tmpstr );
				fprintf( wfp,"\tstruct %s_c f_%s%c;\n",vtabname,vtabname,tb[0] );
				/*sub_chg_str( tmpstra,i );
				fprintf( wfp,"%s",tmpstra+2 ); */
			}
			continue;
		}
		if( !strncmp(tmpstr,"[=",2) ) /**定义变量**/
		{
			for( i=0;i<vfiedqry_i;i++ )
			{
				if( strstr(vfiedqry[i].fldname,"ac_id") 
					&& !strncmp(vfiedqry_attr[i].type,"integer",7) )
				{
					fprintf( wfp,"\tchar ac_no%d[25];\n",i );
				}
				else if( strlen(vfiedqry_attr[i].dic_code) )
				{
					fprintf( wfp,"\tchar f_%s%d[41];\n",vfiedqry[i].fldname,i );
				}
			}
			for( i=0;i<vfiedfilter_i;i++ )
			{
				if( strstr(vfiedfilter[i].fldname,"ac_id") 
					&& !strncmp(vfiedfilter_attr[i].type,"integer",7) )
				{
					fprintf( wfp,"\tchar f_acno%d[25];\n",i );
				}
			}
			continue;
		}
		if( !strncmp(tmpstr,"[>",2) ) /**度取变量**/
		{
			sub_do_val( wfp,tmpstr );
			continue;
		}
		if( !strncmp(tmpstr,"[(",2) ) /**头文件**/
		{
			if( !strcmp(vtabname,"dd_mst")
				|| !strcmp(vtabname,"td_mst")
				|| !strcmp(vtabname,"ln_mst")
				|| !strcmp(vtabname,"od_mst")
				|| !strcmp(vtabname,"ln_mst")
				)
			continue;

			sub_chg_str( tmpstr,0 );
			fprintf( wfp,"%s",tmpstr+2 );
			continue;
		}
		if( !strncmp(tmpstr,"[{",2) ) /**拼接条件**/
		{
			sub_do_sub_fil( wfp );
			continue;
		}
		if( !strncmp(tmpstr,"[#",2) ) /**字段列表**/
		{
			sub_do_fied_list( wfp );
			continue;
		}
		if( !strncmp(tmpstr,"[[",2) ) /**拼接条件**/
		{
			continue;
		}
		if( !strncmp(tmpstr,"[:",2) ) /**字段列表**/
		{
			sub_do_wrt_data( wfp );
			continue;
		}
		if( !strncmp(tmpstr,"[@",2) ) /**字段列表**/
		{
			pub_pubdb_prt_title( titstr,vtabname,fiedlist );
			fprintf( wfp,"\t\t\tfprintf( fp,\"~%s\\n\" );\n",titstr );
			continue;
		}

		sub_chg_str( tmpstr,0 );

		fprintf( wfp,"%s",tmpstr );
	}

	fclose(wfp);
	fclose(mfp);

	return 0;
ErrExit:
	return 1;
}
int sub_chg_str(  char *sstr,int mode )
{
	char *b;
	char *e;
	char *q;
	char *p;
	int i=0;
	int j=0;
	char dstr[1024];
	char subno[2];

	memset(subno,0,sizeof(subno));
	if( mode ) 
		subno[0]='a'+mode-1;

	memset( dstr,0,sizeof(dstr) );
	q=sstr;

	for( i=0; ; i++ )
	{
		b=strstr( q, "<[" );
		if( b==NULL ) break;
		e=strstr( q, "]>" );
		if( e==NULL ) break;

		strncat( dstr,q,b-q );
		q=e+2;

		if( !strncmp(b,"<[TXCODE]>",e+2-b) )
			strcat( dstr,vtxcode );
		else if( !strncmp(b,"<[MENUCODE]>",e+2-b) )
			strcat( dstr,vmenucode );
		else if( !strncmp(b,"<[UPMENU]>",e+2-b) )
			strcat( dstr,vupmenu );
		else if( !strncmp(b,"<[SCRCODE]>",e+2-b) )
			strcat( dstr,vscrcode );
		else if( !strncmp(b,"<[TXNAME]>",e+2-b) )
			strcat( dstr,vtxname );
		else if( !strncmp(b,"<[TABNAME]>",e+2-b) )
		{
			strcat( dstr,vtabname );
			strcat( dstr,subno );
		}
		else if( !strncmp(b,"<[tABNAME]>",e+2-b) )
			strcat( dstr,vBtabname );
		else if( !strncmp(b,"<[TYPE]>",e+2-b) )
			strcat( dstr,vtype );
		else if( !strncmp(b,"<[&]>",e+2-b) )
			strcat( dstr,vpoint );
		else if( !strncmp(b,"<[FIELD]>",e+2-b) )
			strcat( dstr,vfied );
		else if( !strncmp(b,"<[DATACODE]>",e+2-b) )
			strcat( dstr,vdatacode );
		else j=0;

	}

	strcat( dstr,q );
	strcpy( sstr,dstr );
}
sub_get_val_attr() 
{
	int i=0;
	int ret=0;
	struct doc_dic_c vdoc_dic;
	char vt;

	/***取得字段属性***/
	for( i=0; i<vfiedfilter_i; i++ )
	{
		ret=Doc_dic_Sel( g_pub_tx.reply,&vdoc_dic,"tab='%s' and fied='%s'",
			vtabname,vfiedfilter[i].fldname );
		if( ret )
		{
vtcp_log("NO FIND[%s].[%s]",vtabname,vfiedfilter[i].fldname );
			memset( &vdoc_dic,0,sizeof(vdoc_dic) );
		}
		pub_base_strpack( vdoc_dic.dic_code );
		memcpy( &vfiedfilter_attr[i],&vdoc_dic,sizeof(vdoc_dic) );
	}
	for( i=0; i<vfiedqry_i; i++ )
	{
		ret=Doc_dic_Sel( g_pub_tx.reply,&vdoc_dic,"tab='%s' and fied='%s'",
			vtabname,vfiedqry[i].fldname );
		if( ret )
		{
			memset( &vdoc_dic,0,sizeof(vdoc_dic) );
		}
		
		memcpy( &vfiedqry_attr[i],&vdoc_dic,sizeof(vdoc_dic) );
	}
}
sub_wrt_yjg()
{
	int i=0;
	struct doc_dic_c vdoc_dic;
	struct data_dic_c vdata_dic;
	char tstr[1024];
	char k;
	int j=0;
	int ret;
	FILE *yjgfp;
	int yjgxh=5;
	int x=1;
	int y=6;
	char filename[100];
	char datatype[2];

TRACE

	pub_base_AllDwnFilName( filename );
	yjgfp=fopen(filename,"w" );
	if( yjgfp==NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	for( i=0; i<vfiedfilter_i; i++ )
	{
		memcpy( &vdoc_dic,&vfiedfilter_attr[i],sizeof(vdoc_dic) );

		if( !strncmp(vdoc_dic.type,"char",4) )
		{
			strcpy( vtype,"data" );
			strcpy( datatype,"0" );
			k='0';
		}
		else if( !strncmp(vdoc_dic.type,"smallint",8) )
		{
			strcpy( vtype,"int" );
			strcpy( datatype,"1" );
			k='1';
		}
		else if( !strncmp(vdoc_dic.type,"integer",7) )
		{
			strcpy( vtype,"long" );
			strcpy( datatype,"1" );
			k='1';
			if( strstr(vfiedfilter[i].fldname,"ac_id")  )
			{
			strcpy( datatype,"0" );
			k='0';
			}
		}
		else if( !strncmp(vdoc_dic.type,"decimal",7) )
		{
			strcpy( vtype,"double" );
			strcpy( datatype,"4" );
			k='4';
		}
		else
		{
			strcpy( vtype,"XXXX" );
		}

		fprintf(yjgfp,"%s|%d|1%c|%s||%d|%d|%d|%d|%d|%c|%c|%c|%s||||0|0|1|1|0||\n",
			vscrcode,yjgxh,datatype[0],vfiedfilter_attr[i].mo,
			vfiedfilter[i].leng,1,x,y,vfiedfilter[1].dec,
			'0','0','0',vfiedfilter[i].data_code );
		x++; if( x>22 ) y+=4;
		yjgxh+=5;
	}

	fclose( yjgfp );

	set_zd_data(DC_FILE_SND,"1" );

	return 0;
ErrExit:
	return 1;
}
sub_do_val( FILE *wfp,char *sstr )
{
	int i=0;
	struct doc_dic_c vdoc_dic;
	struct data_dic_c vdata_dic;
	char tstr[1024];
	char k;
	int j=0;
	int ret;
	int yjgxh=5;
	int x=4;
	int y=6;
	char tb[2];

	for( i=0; i<vfiedfilter_i; i++ )
	{
		strcpy( tstr,sstr );
		memcpy( &vdoc_dic,&vfiedfilter_attr[i],sizeof(vdoc_dic) );

		strcpy( vfied,vfiedfilter[i].fldname );

		if( !strncmp(vdoc_dic.type,"char",4) )
		{
			strcpy( vtype,"data" );
			k='0';
		}
		else if( !strncmp(vdoc_dic.type,"smallint",8) )
		{
			strcpy( vtype,"int" );
			k='1';
		}
		else if( !strncmp(vdoc_dic.type,"integer",7) )
		{
			strcpy( vtype,"long" );
			vdoc_dic.leng=8;
			k='1';
			if( strstr(vdoc_dic.fied,"cif_no") )
			{
				vdoc_dic.leng=9;
				k='1';
			}
			else if( strstr(vfiedfilter[i].fldname,"ac_id") )
			{
				strcpy( vtype,"data" );
				vdoc_dic.leng=19;
				sprintf( vfied,"f_acno%d",i );
				k='0';
			}
		}
		else if( !strncmp(vdoc_dic.type,"decimal",7) )
		{
			if( vdoc_dic.leng>=16 )
				vdoc_dic.leng=16;
			strcpy( vtype,"double" );
			k='4';
		}
		else
		{
			strcpy( vtype,"XXXX" );
		}

		ret=Data_dic_Dec_Sel( g_pub_tx.reply,
		"((data_code>='0210' and data_code<'0920') or data_code[1,3]='100' ) \
        and data_type=%c and data_leng>=%d and data_dec>=%d order by data_leng,data_dec",
			k,vdoc_dic.leng,vdoc_dic.dec );
		if( ret )
		{
			sprintf(acErrMsg,"sqlerror" );
			WRITEMSG
		}
		strcpy(vfiedfilter[i].data_code,"XXXX") ;
		while(1)
		{
NEXT:
			ret=Data_dic_Fet_Sel( &vdata_dic,g_pub_tx.reply );
			if( ret ) 
			{
				strcpy( vdata_dic.data_code,"XXXX" );
				break;
			}
			
			for( j=0;j<i;j++ )
			{
				if( !strncmp(vdata_dic.data_code,vfiedfilter[j].data_code,4) )
					goto NEXT;
			}
				strcpy(vfiedfilter[i].data_code,vdata_dic.data_code) ;
				vfiedfilter[i].leng=vdata_dic.data_leng;
				vfiedfilter[i].dec=vdata_dic.data_dec;
				break;
		}

		if( !strncmp(vdoc_dic.type,"char",4) )
			strcpy( vpoint,"" );
		else
			strcpy( vpoint,"&" );
		
		strcpy( vdatacode,vfiedfilter[i].data_code );
vtcp_log("[%s] [%d]",vfied,i); TRACE

		if( strstr(vfiedfilter[i].fldname,"ac_id") 
			&& !strncmp(vdoc_dic.type,"integer",7) )
		{
			if( vfiedfilter[i].tim==0 )
				strcpy( tb,"" );
			else
				tb[0]='a'+vfiedfilter[i].tim-1;

			fprintf( wfp,"\tget_zd_data(\"%s\",f_acno%d);\n",vdatacode,i );
		}
		else
		{
			sub_chg_str(  tstr,vfiedfilter[i].tim ) ;
			fprintf( wfp,"%s",tstr+2 );
		}
	}

	return 0;
ErrExit:
	return 1;
}
sub_do_sub_fil( FILE *wfp )
{
	int i;
	char tb[2];
	char valstr[128];
	char tmpstr[1024];

	memset( tb,0,2 );

	for ( i=0;i<vfiedfilter_i;i++ )
	{
			if( vfiedfilter[i].tim==0 )
				strcpy( tb,"" );
			else
				tb[0]='a'+vfiedfilter[i].tim-1;
		sprintf( valstr,"f_%s%s.%s",vtabname,tb,vfiedfilter[i].fldname );
		
		if( vfiedfilter[i].fldname,vfiedfilter[i].comp,valstr);

vtcp_log("PPPPPPP[%s][%s]",vfiedfilter_attr[i].type,vfiedfilter[i].fldname);
			
		if( !strncmp(vfiedfilter_attr[i].type,"char",4) )
		{
			fprintf( wfp,"\tpub_base_strpack( %s );\n",valstr );
			fprintf( wfp,"\tif( strlen(%s) )\n",valstr );
			fprintf( wfp,"\t{\n" );
			if( vfiedfilter[i].comp[0]=='~' )
			{
			fprintf(wfp,"\t\tsprintf( tmpstr,\" %s like '%%%%%%s%%%%' and\",%s );\n",
				vfiedfilter[i].fldname,valstr);
			}
			else
			{
			fprintf(wfp,"\t\tsprintf( tmpstr,\" %s%s'%%s' and\",%s );\n",
				vfiedfilter[i].fldname,vfiedfilter[i].comp,valstr);
			}
			fprintf( wfp,"\t\tstrcat( wherelist,tmpstr );\n" );
			fprintf( wfp,"\t}\n" );
		}
		else if( !strncmp(vfiedfilter_attr[i].type,"integer",7) 
			&& strstr(vfiedfilter[i].fldname,"ac_id") )
		{
			fprintf( wfp,"\tpub_base_strpack( f_acno%d );\n",i );
			fprintf( wfp,"\tif( strlen(f_acno%d) )\n",i );
			fprintf( wfp,"\t{\n" );
			fprintf( wfp,"\t\tif ( pub_base_acno_acid(&%s,f_acno%d) )\n"
				,valstr,i );
			fprintf( wfp,"\t\t{\n" );
			fprintf( wfp,"\t\tgoto ErrExit;\n" );
			fprintf( wfp,"\t\t}\n" );
			fprintf( wfp,"\t\tsprintf( tmpstr,\" %s%s%%ld and\",%s );\n",
				vfiedfilter[i].fldname,vfiedfilter[i].comp,valstr);
			fprintf( wfp,"\t\tstrcat( wherelist,tmpstr );\n" );
			fprintf( wfp,"\t}\n" );
		}
		else if( !strncmp(vfiedfilter_attr[i].type,"integer",7) )
		{
			fprintf( wfp,"\tif( %s )\n",valstr );
			fprintf( wfp,"\t{\n" );
			fprintf( wfp,"\t\tsprintf( tmpstr,\" %s%s%%ld and\",%s );\n",
				vfiedfilter[i].fldname,vfiedfilter[i].comp,valstr);
			fprintf( wfp,"\t\tstrcat( wherelist,tmpstr );\n" );
			fprintf( wfp,"\t}\n" );
		}
		else if( !strncmp(vfiedfilter_attr[i].type,"decimal",7) )
		{
			fprintf( wfp,"\tif( %s )\n",valstr );
			fprintf( wfp,"\t{\n" );
			fprintf( wfp,"\t\tsprintf( tmpstr,\" %s%s%%lf and\",%s );\n",
				vfiedfilter[i].fldname,vfiedfilter[i].comp,valstr);
			fprintf( wfp,"\t\tstrcat( wherelist,tmpstr );\n" );
			fprintf( wfp,"\t}\n" );
		}
		else if( !strncmp(vfiedfilter_attr[i].type,"smallint",8) )
		{
			fprintf( wfp,"\tif( %s )\n",valstr );
			fprintf( wfp,"\t{\n" );
			fprintf( wfp,"\t\tsprintf( tmpstr,\" %s%s%%d and\",%s );\n",
				vfiedfilter[i].fldname,vfiedfilter[i].comp,valstr);
			fprintf( wfp,"\t\tstrcat( wherelist,tmpstr );\n" );
			fprintf( wfp,"\t}\n" );
		}
	}
}
sub_do_fied_list( FILE *wfp )
{
	int i=0;

	fprintf( wfp,"\tstrcpy( fieldlist,\"" );

	for ( i=0;i<vfiedqry_i;i++ )
	{
		if( i ) 
		{
			fprintf( wfp,"," );
			strcat( fiedlist,"," );
		}	
		fprintf( wfp,"%s",vfiedqry[i].fldname );
		strcat( fiedlist,vfiedqry[i].fldname );
	}
	fprintf( wfp,"\");\n" );
}
sub_wrt_tx_def()
{
	struct tx_def_c vtx_def;
	struct tx_flow_def_c vtx_flow_def;
	struct tx_sub_def_c vtx_sub_def;

	memset( &vtx_def,0,sizeof(vtx_def) );
	memset( &vtx_flow_def,0,sizeof(vtx_flow_def) );
	memset( &vtx_sub_def,0,sizeof(vtx_sub_def) );

	strcpy( vtx_def.tx_code,vtxcode );
	strcpy( vtx_def.tx_name,vtxname );
	memset( vtx_def.bit_map,'0',128 );
	strcpy( vtx_def.log_ind,"0" );
	strcpy( vtx_def.tx_type,"0" );
	strcpy( vtx_def.wrk_sts,"0" );

vtcp_log( "TTT txcode[%s][%s]",vtx_def.tx_code,vtxcode );
	if( sql_execute("delete from tx_def where tx_code='%s'",vtxcode) )
	{
		return 1;
	}
	
	if( Tx_def_Ins( vtx_def,g_pub_tx.reply ) )
	{
		return 1;
	}

	strcpy( vtx_flow_def.tx_code,vtxcode );
	vtx_flow_def.flow_code=0;
	strcpy( vtx_flow_def.sub_tx_code,vtxcode );
	strcpy( vtx_flow_def.sub_tx_rslt,"#$" );

	if( sql_execute("delete from tx_flow_def where tx_code='%s'",vtxcode) )
	{
		return 1;
	}

	if( Tx_flow_def_Ins( vtx_flow_def,g_pub_tx.reply ) )
	{
		return 1;
	}

	strcpy( vtx_sub_def.sub_tx_code,vtxcode );
	strcpy( vtx_sub_def.sub_tx_name,vtxname );
	sprintf( vtx_sub_def.tx_func,"sp%s",vtxcode );
	sprintf( vtx_sub_def.acct_ind,"0" );
	sprintf( vtx_sub_def.chnl_ind,"0" );

	if( sql_execute("delete from tx_sub_def where sub_tx_code='%s'",vtxcode) )
	{
		return 1;
	}

	if( Tx_sub_def_Ins( vtx_sub_def,g_pub_tx.reply ) )
	{
		return 1;
	}
}
sub_do_wrt_data( FILE *wfp )
{
	int i=0;

	fprintf( wfp,"\n" );

	for( i=0;i<vfiedqry_i;i++ )
	{
		if( strstr(vfiedqry[i].fldname,"ac_id") 
			&& !strncmp(vfiedqry_attr[i].type,"integer",7) )
		{
			fprintf( wfp,"\t\tpub_base_acid_acno( v%s.%s,v%s.ac_seqn,ac_no%d );\n"
				,vtabname,vfiedqry[i].fldname, vtabname, i );
		}
		else if( strlen(vfiedqry_attr[i].dic_code) )
		{
			pub_base_strpack(vfiedqry_attr[i].dic_code);
			if( !strncmp(vfiedqry_attr[i].type,"integer",7) 
				|| !strncmp(vfiedqry_attr[i].type,"smallint",8) )
			{
				fprintf( wfp,"\t\tsprintf( tmpmo,\"%%d\",v%s.%s);",
					vtabname,vfiedqry[i].fldname);
			fprintf( wfp,"\t\tpub_base_dic_show_str(f_%s%d,\"%s\",tmpmo);\n",
				vfiedqry[i].fldname,i,vfiedqry_attr[i].dic_code );
			}
			else
			{
			fprintf( wfp,"\t\tpub_base_dic_show_str(f_%s%d,\"%s\",v%s.%s);\n",
				vfiedqry[i].fldname,i,vfiedqry_attr[i].dic_code,vtabname,vfiedqry[i].fldname );
			}
		}
		else if( !strncmp(vfiedqry_attr[i].type,"integer",7) )
		{
			fprintf( wfp,"\t\tif(v%s.%s<-2147000000) v%s.%s=0;\n",
				vtabname,vfiedqry[i].fldname ,vtabname,vfiedqry[i].fldname );
		}
		else if( !strncmp(vfiedqry_attr[i].type,"decimal",7) )
		{
			fprintf( wfp,"\t\tif(v%s.%s<-2147000000) v%s.%s=0.0;\n",
				vtabname,vfiedqry[i].fldname ,vtabname,vfiedqry[i].fldname );
		}
	}

	fprintf( wfp,"\n" );
	fprintf( wfp,"\t\tfprintf( fp, \"" );

	for( i=0;i<vfiedqry_i;i++ )
	{
		if( strstr(vfiedqry[i].fldname,"ac_id") 
			&& !strncmp(vfiedqry_attr[i].type,"integer",7) )
			fprintf( wfp,"%%s|" );
		else if( strlen(vfiedqry_attr[i].dic_code) )
			fprintf( wfp,"%%s|" );
		else if( !strncmp(vfiedqry_attr[i].type,"char",4) )
			fprintf( wfp,"%%s|" );
		else if( !strncmp(vfiedqry_attr[i].type,"decimal",7) )
			fprintf( wfp,"%%.%dlf|",vfiedqry_attr[i].dec );
		else if( !strncmp(vfiedqry_attr[i].type,"smallint",8) )
			fprintf( wfp,"%%d|" );
		else if( !strncmp(vfiedqry_attr[i].type,"integer",7) )
			fprintf( wfp,"%%ld|" );
	}

	fprintf( wfp,"\\n\"" );

	for( i=0;i<vfiedqry_i;i++ )
	{
		if( strstr(vfiedqry[i].fldname,"ac_id") 
			&& !strncmp(vfiedqry_attr[i].type,"integer",7) )
			fprintf( wfp,",ac_no%d",i );
		else if( strlen(vfiedqry_attr[i].dic_code) )
			fprintf( wfp,",f_%s%d",vfiedqry[i].fldname,i );
		else
			fprintf( wfp,",v%s.%s",vtabname,vfiedqry[i].fldname );
	}

	fprintf( wfp,");\n" );
}
