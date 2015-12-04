/*************************************************
* 文 件 名:  sp8213.c
* 功能描述： 老贷款欠息登记簿intrtmpreg查询
*
* 作    者:  Gujy JinZ
* 完成日期： 
*
* 修改记录：20070428
* 日   期:
* 修 改 人:
* 修改内容: 从intrtmpreg表中取欠息登记簿内容
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "ln_reg_c.h"
#include "ln_mst_c.h"
#include "intrtmpreg_c.h"

int sp8213()
{
	struct ln_reg_c vln_reg;
	struct ln_reg_c f_ln_reg;
	struct ln_reg_c f_ln_rega;
	struct intrtmpreg_c intrtmpreg;
	char ac_no0[25];
	char actno[25];
	char f_type4[41];
	char f_sts7[41];
	char f_acno0[25];
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
		char bbr_no[6];
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

  get_zd_data("0030",bbr_no);
	get_zd_data("0380",f_acno0);


	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		sprintf( tmpstr," actno='%s' and ", f_acno0 );
		strcat( wherelist,tmpstr );
	}


	strcpy( tablelist,"intrtmpreg" );

	/**组成查询**/
	sprintf(wherelist+strlen(wherelist)," kinbr='%s' and 1=1  ORDER BY actno,actnoseq,txdate " , bbr_no);

	vtcp_log( "[%s][%d] WHERE [%s]",__FILE__,__LINE__,wherelist );
	strcpy( fieldlist,"actno,actnoseq,txdate,atiday,adjday,rate,dinum,intr,avbal, stat");
	memset(actno, 0x0, sizeof(actno));
	ret=Intrtmpreg_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
		ttlnum=0;

	while(1)
	{
		ret=Intrtmpreg_Fet_Sel( &intrtmpreg, g_pub_tx.reply );
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
			fprintf( fp,"~账    号|户名|序 号|计息日|$利率|$积数|$欠息金额|还款状态|\n" );
		}

                /* 取贷款户名称 */
		memset(&g_ln_mst, 0x0, sizeof(g_ln_mst));
		if(0 != strcmp(actno, intrtmpreg.actno)){
                ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id in (select ac_id from mdm_ac_rel where ac_no='%s') " , intrtmpreg.actno );              
                if( ret && 100 != ret)
                {
                        sprintf(acErrMsg,"读取贷款主文件信息error[%d]",ret);
                        WRITEMSG
                        strcpy( g_pub_tx.reply,"S049" );
                        goto ErrExit;
                }               
		strcpy(actno, intrtmpreg.actno);
		}
				
		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		 /**88888**/
		memset(f_sts7, 0x0, sizeof(f_sts7));
		if('1' == intrtmpreg.stat[0]){strcpy(f_sts7, "未还");}else
		if('2' == intrtmpreg.stat[0]){strcpy(f_sts7, "部分还款");}else
		if('3' == intrtmpreg.stat[0]){strcpy(f_sts7, "全还");}else
		if('9' == intrtmpreg.stat[0]){strcpy(f_sts7, "取消");}
vtcp_log("%d@%s   ttlnum=[%d]\n", __LINE__, __FILE__, ttlnum);

			/****增加返回还款序号和还款状态 modify by wanglei 20070529****/
		fprintf( fp, "%s|%.20s|%5d|%.08s|%.4lf|%.2lf|%.2lf|%8s|\n",
				intrtmpreg.actno,g_ln_mst.name,intrtmpreg.actnoseq,
			 intrtmpreg.adjday, intrtmpreg.rate, 
			intrtmpreg.dinum,intrtmpreg.avbal,f_sts7);
			/****end 20070529****/
vtcp_log("%d@%s   ttlnum=[%d]\n", __LINE__, __FILE__, ttlnum);
		ttlnum++;
	}

	ret=Intrtmpreg_Clo_Sel( );
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
