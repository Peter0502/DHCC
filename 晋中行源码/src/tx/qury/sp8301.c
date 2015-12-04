/*************************************************
* 文 件 名:  sp8301.c
* 功能描述： 客户凭证查询
*
* 作    者:  jane
* 完成日期： 2003年3月22日
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "note_cheq_mst_c.h"
#include "note_parm_c.h"

int sp8301()
{
	struct note_cheq_mst_c 	vnote_cheq_mst;
	struct note_cheq_mst_c 	f_note_cheq_mst;
	struct note_cheq_mst_c 	f_note_cheq_msta;
	struct note_parm_c 	sNoteParm;
	struct 	mdm_ac_rel_c 	sMdm_ac_rel_c;		/* 介质与账户关系表 */
	char ac_no0[25];
	char f_note_type2[41];
	char f_note_no[17];
	char f_book_sts1[41];
	char f_grp_ind7[41];
	char f_acno0[25];
	char cBookSts[2];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char    cBegHead[17], cEndHead[17], cBegHead1[17];      /* 冠子头 */
		char fmt[100];
		char cHeadLen[10];
		int ttlnum=0;			/**读取的总条数**/
		long i=0;
		long j=0;
		long lBegNoteNo, lEndNoteNo, lBegNoteNo1, lEndNoteNo1;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(&sMdm_ac_rel_c,0x00,sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
    pub_base_old_acno(f_acno0);
	get_zd_long("0510",&f_note_cheq_mst.ac_seq );
	get_zd_data("0890",f_note_cheq_mst.note_type );
	get_zd_data("0910",f_note_cheq_mst.br_no );
	get_zd_data("0590",f_note_cheq_mst.beg_note_no );
	get_zd_data("0600",f_note_cheq_mst.end_note_no );
	get_zd_long("0290",&f_note_cheq_mst.sell_date );
	get_zd_long("0440",&f_note_cheq_msta.sell_date );
	get_zd_data("0670",cBookSts );

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_note_cheq_mst.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_note_cheq_mst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_note_cheq_mst.ac_seq )
	{
		sprintf( tmpstr," ac_seq=%d and",f_note_cheq_mst.ac_seq );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_cheq_mst.note_type );
	if( strlen(f_note_cheq_mst.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_note_cheq_mst.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_note_cheq_mst.br_no );
	if( strlen(f_note_cheq_mst.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_note_cheq_mst.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_note_cheq_mst.sell_date )
	{
		sprintf( tmpstr," sell_date>=%ld and",f_note_cheq_mst.sell_date );
		strcat( wherelist,tmpstr );
	}
	if( f_note_cheq_msta.sell_date )
	{
		sprintf( tmpstr," sell_date<=%ld and",f_note_cheq_msta.sell_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"note_cheq_mst" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	
	ret=Note_cheq_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Note_cheq_mst_Fet_Sel( &vnote_cheq_mst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		/* 到种类表读取冠子头长度 */
		ret=Note_parm_Sel( g_pub_tx.reply, &sNoteParm, "note_type='%s'", vnote_cheq_mst.note_type );
		vtcp_log("note_type[%s]",vnote_cheq_mst.note_type);
		ERR_DEAL

		/* 截取冠子头 */
    	strncpy( cBegHead1, vnote_cheq_mst.beg_note_no, sNoteParm.head_len );
    	cBegHead1[sNoteParm.head_len] = '\0';

		/* 当输入凭证号码不为空时再截取冠子头 */
		if( strlen(f_note_cheq_mst.beg_note_no) )
		{
			strncpy( cBegHead,f_note_cheq_mst.beg_note_no, sNoteParm.head_len );
    		cBegHead[sNoteParm.head_len] = '\0';
    		/* 比较冠子头 */
    		if ( strcmp( cBegHead, cBegHead1 ))
        		continue;
		}
		if( strlen(f_note_cheq_mst.end_note_no) )
		{
			strncpy( cEndHead,f_note_cheq_mst.end_note_no, sNoteParm.head_len );
    			cEndHead[sNoteParm.head_len] = '\0';
    			/* 比较冠子头 */
    			if ( strcmp( cEndHead, cBegHead1 ))
        			continue;
		}

		/* 取得剩余的为号码 */
		lBegNoteNo1=atol( vnote_cheq_mst.beg_note_no + sNoteParm.head_len );
		lEndNoteNo1=atol( vnote_cheq_mst.end_note_no + sNoteParm.head_len );

		pub_base_strpack( cBookSts );
		/* 逐一显示凭证号码及其购买日期和状态 */
		for( i=lBegNoteNo1; i<=lEndNoteNo1; i++ )
		{
			j=i-lBegNoteNo1;
			if( strlen( cBookSts ) && vnote_cheq_mst.book_sts[j] != cBookSts[0] )
				continue;
			
			if( vnote_cheq_mst.book_sts[j] == '0' )
				continue;
			else if( vnote_cheq_mst.book_sts[j] == '1' )
				strcpy( f_book_sts1, "出售未使用" );
			else if( vnote_cheq_mst.book_sts[j] == '2' )
				strcpy( f_book_sts1, "挂失" );
			else if( vnote_cheq_mst.book_sts[j] == '3' )
				strcpy( f_book_sts1, "回收" );
			else if( vnote_cheq_mst.book_sts[j] == '4' )
				strcpy( f_book_sts1, "作废" );
			else
				strcpy( f_book_sts1, "使用" );

			pub_base_dic_show_str(f_note_type2,"note_type",vnote_cheq_mst.note_type);
			if(vnote_cheq_mst.sell_date<-2147000000) vnote_cheq_mst.sell_date=0;

			sprintf( fmt, "%%s%%0%dd", 16-sNoteParm.head_len );

			sprintf( f_note_no, fmt, cBegHead1, i );

			/* 判断表中的凭证号码是否包含在输入的凭证号码区间中 */
			if( strlen(f_note_cheq_mst.beg_note_no) &&
				strcmp(f_note_no, f_note_cheq_mst.beg_note_no ) <0 )
				continue;
			if( strlen(f_note_cheq_mst.end_note_no) && 
				strcmp(f_note_no, f_note_cheq_mst.end_note_no ) >0 )
				continue;

				ret=Mdm_ac_rel_Sel( g_pub_tx.reply,&sMdm_ac_rel_c,
					"ac_id=%ld and ac_seqn=%d",
					vnote_cheq_mst.ac_id, vnote_cheq_mst.ac_seq );
				if( ret == 100 )
				{
					sprintf( acErrMsg, "介质与账户关系表中不存在此帐号!" );
	        		WRITEMSG
	        		strcpy( g_pub_tx.reply, "B117" );
	        		goto ErrExit;
				}else if( ret )
				{
					sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret );
	        		WRITEMSG
	        		goto ErrExit;
				}
			
				vtcp_log("__xxxx__vnote_cheq_mst.book_sts[j]=[%s],cBookSts=[%s]",vnote_cheq_mst.book_sts[j],cBookSts);
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

			/* 显示列标题 */
			fprintf(fp,"~@账    号|@凭证种类|@凭证号码|@支票状态|购买日期|\n");
		}


			fprintf( fp, "%s|%s|%s|%s|%ld|\n",sMdm_ac_rel_c.ac_no,f_note_type2,\
				f_note_no,f_book_sts1,vnote_cheq_mst.sell_date);
			ttlnum++;
		}
	}

TRACE
	ret=Note_cheq_mst_Clo_Sel( );
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
