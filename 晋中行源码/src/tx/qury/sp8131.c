/*************************************************
* 文 件 名:  sp8131.c
* 功能描述： 二级账户明细查询
*
* 作    者:  GUJINGYU
* 完成日期：
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
#include "sub_dd_mst_hst_c.h"
#include "com_branch_c.h"
#include "qury_reg_c.h"
#include "sub_dd_mst_c.h"
int sp8131()
{
	struct sub_dd_mst_hst_c vsub_dd_mst_hst;
	struct sub_dd_mst_hst_c f_sub_dd_mst_hst;
	struct sub_dd_mst_hst_c f_sub_dd_mst_hsta;
	struct com_branch_c f_com_branch;
	struct com_branch_c t_com_branch;
	struct qury_reg_c sQury_reg;
	struct sub_dd_mst_c s_sub_dd_mst;

	char add_ind[3];
	char ct_ind[5];
	char ac_no3[25];
	char f_note_type9[41];
	char f_acno0[25];
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char tmpmo[512];
	char fieldlist[1024];  /**要查询的字段列表**/
	char tablelist[128];   /**要查询的表名**/
	char titstr[1024];
	int ttlnum=0;			/**读取的总条数**/
	int debit_num=0;
	int loan_num=0;
	int cqy_flag=0;
	int i=0;
	int ret=0;
	double j_tx_amt=0.00;
	double d_tx_amt=0.00;
	double cqy_amt=0.00;
	FILE *fp=NULL;
	char tmp_date[9];
	char tmp_time[7];
	long currnum=0;
	char cQs[BRNO_LEN+1];
	char add_amt1[50];          /*带现转标志的金额数 贷方*/
	char add_amt2[50];          /*带现转标志的金额数 借方*/
	long tmp_ac_seqn=0;     /*保存账户判断是否更换账号*/

	memset(add_ind,0x00,sizeof(add_ind));
	memset(add_amt1,0x00,sizeof(add_amt1));
	memset(add_amt2,0x00,sizeof(add_amt2));
	memset(ct_ind,0x00,sizeof(ct_ind));
	memset(ac_no3,0x00,sizeof(ac_no3));
	memset(f_note_type9,0x00,sizeof(f_note_type9));
	memset(f_acno0,0x00,sizeof(f_acno0));
	memset(&vsub_dd_mst_hst,0x00,sizeof(vsub_dd_mst_hst));
	memset(&f_sub_dd_mst_hst,0x00,sizeof(f_sub_dd_mst_hst));
	memset(&f_sub_dd_mst_hsta,0x00,sizeof(f_sub_dd_mst_hsta));
	memset(&f_com_branch,0x00,sizeof(f_com_branch));
	memset(&t_com_branch,0x00,sizeof(t_com_branch));
	memset(&sQury_reg,0x00,sizeof(sQury_reg));
	memset(tmpstr,0x00,sizeof(tmpstr));
	memset(filename,0x00,sizeof(filename));
	memset(tmpmo,0x00,sizeof(tmpmo));
	memset(titstr,0x00,sizeof(titstr));
	memset(tmp_date,0x00,sizeof(tmp_date));
	memset(tmp_time,0x00,sizeof(tmp_time));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	pub_base_sysinit();

	get_zd_data("0380",f_acno0);
	pub_base_old_acno(f_acno0);
	get_zd_data("0700",f_sub_dd_mst_hst.add_ind );
	get_zd_data("0660",f_sub_dd_mst_hst.ct_ind );
	get_zd_long ("0780",&f_sub_dd_mst_hst.sub_ac_seqn );
	get_zd_long("0290",&f_sub_dd_mst_hst.tx_date );
	get_zd_long("0440",&f_sub_dd_mst_hsta.tx_date );

	memset(cQs, 0 , sizeof(cQs));
	iFind_qs_br_no2(g_pub_tx.tx_br_no,cQs);

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," ac_no='%s' and",f_acno0);
		strcat( wherelist,tmpstr );
	}

	/*只有总行清算中心和科技中心可以查所有存款明细 */
	if(memcmp(g_pub_tx.tx_br_no,QS_BR_NO,BRNO_LEN) != 0 && memcmp(g_pub_tx.tx_br_no,cQs,BRNO_LEN) != 0 && memcmp(g_pub_tx.tx_br_no,KJ_BR_NO,BRNO_LEN) != 0)
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," opn_br_no='%s' and",g_pub_tx.tx_br_no );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_sub_dd_mst_hst.add_ind );
	if( strlen(f_sub_dd_mst_hst.add_ind) )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," add_ind='%s' and",f_sub_dd_mst_hst.add_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst_hst.ct_ind );
	if( strlen(f_sub_dd_mst_hst.ct_ind) )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," ct_ind='%s' and",f_sub_dd_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_sub_dd_mst_hst.sub_ac_seqn)
	{
		vtcp_log("f_sub_dd_mst_hst.sub_ac_seqn >>> [%d]",f_sub_dd_mst_hst.sub_ac_seqn);
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," sub_ac_seqn=%d and",f_sub_dd_mst_hst.sub_ac_seqn);
		strcat( wherelist,tmpstr );
	}
	if( f_sub_dd_mst_hst.tx_date )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," tx_date>=%ld and",f_sub_dd_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_sub_dd_mst_hsta.tx_date )
	{
		memset(tmpstr,0x00,sizeof(tmpstr));
		sprintf( tmpstr," tx_date<=%ld and",f_sub_dd_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"sub_dd_mst_hst" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY sub_ac_seqn,tx_date,trace_no,trace_cnt") ;
	strcpy( fieldlist,"trace_no,trace_cnt,hst_cnt,opn_br_no,ac_id,ac_seqn,add_ind,ct_ind,tx_amt,bal,note_type,note_no,tx_date,tx_time,brf,tel,chk,auth");
	ret=Sub_dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;
    cqy_flag=0;
	j_tx_amt=0.00;
	debit_num=0;
	d_tx_amt=0.00;
	loan_num=0;
	while(1)
	{
		memset(&vsub_dd_mst_hst,0,sizeof(vsub_dd_mst_hst));
		ret=Sub_dd_mst_hst_Fet_Sel( &vsub_dd_mst_hst, g_pub_tx.reply );
		if( ret==100 )
			break;
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
  		fprintf( fp,"~@交易日期|@摘要|@子账号|@户名|@借方发生额|@贷方发生额|$余额|@记账柜员|@交易机构|@交易时间|\n" );
		}

		if(vsub_dd_mst_hst.trace_no<-2147000000) vsub_dd_mst_hst.trace_no=0;
		if(vsub_dd_mst_hst.trace_cnt<-2147000000) vsub_dd_mst_hst.trace_cnt=0;
		if(vsub_dd_mst_hst.tx_amt<-2147000000) vsub_dd_mst_hst.tx_amt=0.0;
		if(vsub_dd_mst_hst.bal<-2147000000) vsub_dd_mst_hst.bal=0.0;
		if(vsub_dd_mst_hst.tx_date<-2147000000) vsub_dd_mst_hst.tx_date=0;
		if(vsub_dd_mst_hst.tx_time<-2147000000) vsub_dd_mst_hst.tx_time=0;
		if(vsub_dd_mst_hst.hst_cnt<-2147000000) vsub_dd_mst_hst.hst_cnt=0;

		/*判断是否打印承前页*/
		if(tmp_ac_seqn!=vsub_dd_mst_hst.sub_ac_seqn)
		{
			/*如果不是第一次的话打印借、贷总余额*/
			if(cqy_flag!=0)
			{
				 /********
				 fprintf( fp,"借方发生额合计:");
				 fprintf( fp,"%.2lf  ",j_tx_amt);
				 fprintf( fp,"借方数目合计:%d  ",debit_num);
				 fprintf( fp,"贷方发生额合计:");
				 fprintf( fp,"%.2lf  ",d_tx_amt);
				 fprintf( fp,"贷方数目合计:%d|\n",loan_num);
				 ********/
				 fprintf( fp,"||发生额合计:|%.2lf|%.2lf|\n",j_tx_amt,d_tx_amt);
				 j_tx_amt=0.00;
				 debit_num=0;
				 d_tx_amt=0.00;
				 loan_num=0;
			}
			/*计算余额*/
			if(vsub_dd_mst_hst.add_ind[0]=='0')
			{
				 cqy_amt=vsub_dd_mst_hst.bal+vsub_dd_mst_hst.tx_amt;
			}
			else
			{
				 cqy_amt=vsub_dd_mst_hst.bal-vsub_dd_mst_hst.tx_amt;
			}
		    fprintf(fp,"|承前页|%ld|||%17.2f||||\n",vsub_dd_mst_hst.sub_ac_seqn,cqy_amt);
			tmp_ac_seqn=vsub_dd_mst_hst.sub_ac_seqn;
			cqy_flag++;
		}
		if(vsub_dd_mst_hst.add_ind[0]=='1')
		{
			strcpy(add_ind,"贷");
			d_tx_amt=d_tx_amt+vsub_dd_mst_hst.tx_amt;
			loan_num++;
		}
		else if(vsub_dd_mst_hst.add_ind[0]!='1')
		{
			strcpy(add_ind,"借");
			j_tx_amt=j_tx_amt+vsub_dd_mst_hst.tx_amt;
			debit_num++;
		}
		if(vsub_dd_mst_hst.ct_ind[0]=='1' && vsub_dd_mst_hst.add_ind[0]!='1')
		{
			sprintf(add_amt2,"%s%.2lf","现",vsub_dd_mst_hst.tx_amt);
			add_amt1[0]='\0';
		}
		if(vsub_dd_mst_hst.ct_ind[0]=='2' && vsub_dd_mst_hst.add_ind[0]!='1')
		{
			sprintf(add_amt2,"%s%.2lf","转",vsub_dd_mst_hst.tx_amt);
			add_amt1[0]='\0';
		}
		if((vsub_dd_mst_hst.ct_ind[0]!='1' && vsub_dd_mst_hst.ct_ind[0]!='2') && vsub_dd_mst_hst.add_ind[0]=='0')
		{
			sprintf(add_amt1,"%.2lf",vsub_dd_mst_hst.tx_amt);
			add_amt2[0]='\0';
		}
		if(vsub_dd_mst_hst.ct_ind[0]=='1' && vsub_dd_mst_hst.add_ind[0]=='1')
		{
			sprintf(add_amt1,"%s%.2lf","现",vsub_dd_mst_hst.tx_amt);
			add_amt2[0]='\0';
		}
		if(vsub_dd_mst_hst.ct_ind[0]=='2' && vsub_dd_mst_hst.add_ind[0]=='1')
		{
			sprintf(add_amt1,"%s%.2lf","转",vsub_dd_mst_hst.tx_amt);
			add_amt2[0]='\0';
		}
		if(vsub_dd_mst_hst.ct_ind[0]=='' && vsub_dd_mst_hst.add_ind[0]!='1')
		{
			sprintf(add_amt2,"%.2lf",vsub_dd_mst_hst.tx_amt);
			add_amt1[0]='\0';
		}
		pub_base_dic_show_str(ct_ind,"ct_ind",vsub_dd_mst_hst.ct_ind);
		sprintf(tmp_date,"%ld",vsub_dd_mst_hst.tx_date);
		sprintf(tmp_time,"%d",vsub_dd_mst_hst.tx_time);

		memset(&s_sub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
		ret=Sub_dd_mst_Sel(g_pub_tx.reply,&s_sub_dd_mst,"ac_no='%s' and sub_ac_no='%s'",vsub_dd_mst_hst.ac_no,vsub_dd_mst_hst.sub_ac_no);
		ERR_DEAL

		fprintf(fp,"%s|%s|%s|%s|%s|%s|%.2lf|%s|%s|%s|\n",tmp_date,vsub_dd_mst_hst.brf,vsub_dd_mst_hst.sub_ac_no,s_sub_dd_mst.name,add_amt2,add_amt1,vsub_dd_mst_hst.bal,vsub_dd_mst_hst.tel,vsub_dd_mst_hst.tx_br_no,tmp_time);
		ttlnum++;
	}


	ret=Sub_dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		/*set_zd_data(DC_GET_MSG,"该账号开户行不为本机构或者该户尚未有明细!");*/
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		 if(cqy_flag==0)
		 {
				 memset(&sQury_reg,0x00,sizeof(sQury_reg));
				 strcpy(sQury_reg.tx_br_no,g_pub_tx.tx_br_no);
				 sQury_reg.tx_date=g_pub_tx.tx_date;
				 sQury_reg.tx_time=g_pub_tx.tx_time;
				 strcpy(sQury_reg.tx_code,g_pub_tx.tx_code);
				 strcpy(sQury_reg.tx_tel,g_pub_tx.tel);
				 strcpy(sQury_reg.wherelist,wherelist);
				 ret = Qury_reg_Ins(sQury_reg,g_pub_tx.reply);
				 if (ret)
				 {
					sprintf( acErrMsg,"INSERT  qury_reg游标异常![ %s ]",g_pub_tx.reply);
					WRITEMSG
					strcpy(g_pub_tx.reply,"O090");
					goto ErrExit;
				 }
				 /*******
				 fprintf( fp,"借方发生额合计:|%.21f|借方数目合计:|%ld|贷方发生额合计|%.21f|贷方数目合计:|%.21f|贷方数目合计:%d|\n",j_tx_amt,debit_num,d_tx_amt,loan_num);***/
				 fprintf( fp,"||发生额合计:|%.2lf|%.2lf|\n",j_tx_amt,d_tx_amt);
		 }
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
        if(fp != NULL)
        {
           fclose(fp);
           fp = NULL;
        }
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
