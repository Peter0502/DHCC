/*************************************************
* 文 件 名:  sp8302.c
* 功能描述： 柜员凭证查询
*
* 作    者:  jane
* 完成日期： 
*
* 修改记录：
* 日   期:   2003年4月25日
* 修 改 人:  jane
* 修改内容:  此交易只允许上级机构查询下级机构，不能垮管理查询，
*	     网点间不能互查，同一网点柜员间可以互查；
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "note_mst_c.h"
#include "com_tel_c.h"
#include "com_tx_br_rel_c.h"
#include "note_mst_hst_c.h"
int sp8302()
{
	struct note_mst_c vnote_mst;
	struct note_mst_c f_note_mst;
	struct note_mst_hst_c vnote_mst_hst;
	struct com_tel_c  sComTel;		/* 柜员信息表 */
	struct com_tx_br_rel_c	sComTxBrRel;	/* 交易机构关系表 */
	char f_note_type1[41];
	char f_sts5[41];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;
	int tmpflag;
	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( &sComTel,0,sizeof( struct com_tel_c) );
	memset( &sComTxBrRel,0,sizeof( struct com_tx_br_rel_c) );
	memset( &vnote_mst_hst,0,sizeof(struct note_mst_hst_c));
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_note_mst.tel );
	get_zd_data("0890",f_note_mst.note_type );
	get_zd_data("0640",f_note_mst.br_no );
	if( !strlen(f_note_mst.br_no) )
		strcpy( f_note_mst.br_no,g_pub_tx.tx_br_no );
	get_zd_data("0590",f_note_mst.beg_note_no );
	get_zd_data("0600",f_note_mst.end_note_no );
	get_zd_data("0670",f_note_mst.sts );
	
	sprintf( acErrMsg, "beg[%s],end=[%s]", f_note_mst.beg_note_no,f_note_mst.end_note_no );
	WRITEMSG
	
	/**组成查询条件**/
	pub_base_strpack( f_note_mst.tel );
	if( strlen(f_note_mst.tel) )
	{
		/* 检查所输柜员与交易柜员之间的关系 */
		ret = pub_base_GetTelInfo( f_note_mst.tel, &sComTel );
		if ( ret )
		{
			sprintf( acErrMsg, "查找柜员信息出错,ret=[%d]", ret );
			WRITEMSG
			goto ErrExit;	
		}

                pub_base_strpack(sComTel.br_no);
                pub_base_strpack(g_pub_tx.tx_br_no);
		if ( strcmp( g_pub_tx.tx_br_no, sComTel.br_no ) != 0 )
		{
			/* 检查所输机构是否为交易机构的下级 */
			ret = pub_com_ChkBrRel( 2, sComTel.br_no, g_pub_tx.tx_br_no );
			if ( ret )
			{
				sprintf( acErrMsg, "检查机构之间的关系错" );
				WRITEMSG
				goto ErrExit;
			}	
		}
		sprintf( tmpstr," tel='%s' and",f_note_mst.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst.note_type );
	if( strlen(f_note_mst.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_note_mst.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst.sts );
	if( strlen(f_note_mst.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_note_mst.sts );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst.br_no );
	if( strlen(f_note_mst.br_no) )
	{
		/* 检查所输机构与交易机构的关系 */
		if ( strcmp( g_pub_tx.tx_br_no, f_note_mst.br_no ) != 0 )
		{
			/* 检查所输机构是否为交易机构的下级 */
			ret = pub_com_ChkBrRel( 2, f_note_mst.br_no, g_pub_tx.tx_br_no );
			if ( ret )
			{
				sprintf( acErrMsg, "检查机构之间的关系错" );
				WRITEMSG
				goto ErrExit;
			}	
		}
		sprintf( tmpstr," br_no='%s' and",f_note_mst.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst.beg_note_no );
	if( strlen(f_note_mst.beg_note_no) )
	{
		sprintf( tmpstr," end_note_no>='%s' and",f_note_mst.beg_note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_mst.end_note_no );
	
	if( strlen(f_note_mst.end_note_no) )
	{
		sprintf( tmpstr," beg_note_no<='%s' and",f_note_mst.end_note_no );
		strcat( wherelist,tmpstr );
	}
	
	/* 如果柜员号和机构号输入均为空 */
	if( !strlen(f_note_mst.tel) && !strlen(f_note_mst.br_no) )
		strcpy( f_note_mst.br_no,g_pub_tx.tx_br_no );

	if( !strlen(f_note_mst.tel) && !strlen(f_note_mst.br_no) )
	{
		strcat(wherelist,"(");
		tmpflag = 0;
		/* 查询柜员信息表 */
		ret = Com_tel_Dec_Sel( g_pub_tx.reply, "1==1" );
		while(1)
		{
			ret = Com_tel_Fet_Sel( &sComTel, g_pub_tx.reply );
			if( ret==100 ) 
			{
				if (tmpflag != 0)
				{
					strcat(wherelist,") and ");
				}
				else
				{
					goto ErrExit;
				}
				break;
				
			}
			else if( ret )
			{
				sprintf( acErrMsg, "sql error" );
				WRITEMSG
				goto ErrExit;
			}
			if( strcmp( sComTel.br_no, g_pub_tx.tx_br_no ) != 0 )
			{
				/* 检查此柜员是否为交易机构下级机构柜员 */
				ret = Com_tx_br_rel_Sel( g_pub_tx.reply, &sComTxBrRel, "rel_type='%d'\
						and br_no='%s' and up_br_no='%s'", 2, sComTel.br_no,\
						g_pub_tx.tx_br_no );
				if( ret == 100 )
					continue;
				else if( ret )
				{
					sprintf( acErrMsg, "执行Com_tx_br_rel_Sel错误!ret=[%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
			}
			
			tmpflag ++;
			
			if (tmpflag == 1)
			{
				sprintf( tmpstr," tel='%s' ",sComTel.tel );
				strcat( wherelist,tmpstr );
			}
			else
			{
				sprintf( tmpstr," or tel='%s' ",sComTel.tel );
				strcat( wherelist,tmpstr );
			}
		}
	}
	strcpy( tablelist,"note_mst" );

	/**组成查询**/
	strcat(wherelist," 1=1 order by tel,sts,note_type,beg_note_no") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"tel,note_type,br_no,beg_note_no,end_note_no,sts,cnt,ys_cnt");

	ret=Note_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Note_mst_Fet_Sel( &vnote_mst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**显示列标题**/
			fprintf( fp,"~@柜员|@凭证种类|@起始号码|@终止号码|@凭证状态|数量|@机构编码|@对方机构|对方柜员|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_dic_show_str(f_note_type1,"note_type",vnote_mst.note_type);
		pub_base_dic_show_str(f_sts5,"note_sts",vnote_mst.sts);
		if(vnote_mst.cnt<-2147000000) vnote_mst.cnt=0;
    /******add by ligl 2006-10-19 21:39*********/
    memset( &vnote_mst_hst,0,sizeof(struct note_mst_hst_c));
    /*if(vnote_mst.sts[0]=='2'||vnote_mst.sts[0]=='3')
    {
    	ret=Note_mst_hst_Des_Sel(g_pub_tx.reply,"tel='%s' and note_type='%s' and br_no='%s' and beg_note_no='%s');
    	ret=Note_mst_hst_Sel(g_pub_tx.reply,&vnote_mst_hst,"tel='%s' and note_type='%s' and br_no='%s' and beg_note_no='%s' and end_note_no='%s' and note_cnt=%ld",vnote_mst.tel,vnote_mst.note_type,vnote_mst.br_no,vnote_mst.beg_note_no,vnote_mst.end_note_no,vnote_mst.cnt);
      ERR_DEAL
    }*/
    /*************end***************/
		fprintf( fp, "%s|%s-%s|%s|%s|%s|%ld|%s|%s|%s|\n",
			vnote_mst.tel,vnote_mst.note_type,f_note_type1,
			vnote_mst.beg_note_no,vnote_mst.end_note_no,
			f_sts5,vnote_mst.cnt,
			vnote_mst.br_no,vnote_mst_hst.tw_br_no,vnote_mst_hst.tw_tel
			);
		ttlnum++;
	}

	ret=Note_mst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
