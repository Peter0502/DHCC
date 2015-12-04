/*************************************************
* 文 件 名:  sp5925.c
* 功能描述： 异地交易查询
* 作    者:  ligl
* 完成日期： 2006年10月18日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "com_item_c.h"
#include "ln_mst_c.h"

sp5925()
{
  int ret=0,ret1=0,ret2=0,i;
	char cBr_no[6];  /*机构号*/
	char cAc_no[20];
	long lTx_bdate=0;/*交易起始日期*/
	long lTx_edate=0;/*交易终止日期*/

	struct ln_mst_c f_ln_mst;
	char cFilename[100];    /*文件名*/
	int ttlnum=0;	          /**读取的总条数**/
	FILE *fp;
	char tmpstr[200];
	char wherelist[1024];  /**查询条件**/
  memset(&f_ln_mst,0x0,sizeof(struct ln_mst_c));
  memset(tmpstr,0,sizeof(tmpstr));
	memset(cBr_no     ,0, sizeof(cBr_no ));    /*机构号  */
	memset(cFilename  ,0, sizeof(cFilename));  /*文件名*/
	memset(cAc_no,0,sizeof(cAc_no));
	memset(wherelist,0x0,sizeof(wherelist));
	vtcp_log("sp5923开始！[%s][%d]",__FILE__,__LINE__);
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0300",cAc_no);
	get_zd_long("0440",&lTx_bdate);
	get_zd_long("0450",&lTx_edate);
	pub_base_old_acno(cAc_no);
	pub_base_strpack(cAc_no);
	if(strlen(cAc_no))
	{
		if ( pub_base_acno_acid(&f_ln_mst.ac_id,cAc_no) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_mst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( lTx_bdate )
	{
		sprintf( tmpstr," ic_date>=%ld and",lTx_bdate);
		strcat( wherelist,tmpstr );
	}
	if( lTx_edate )
	{
		sprintf( tmpstr," mtr_date<=%ld and",lTx_edate );
		strcat( wherelist,tmpstr );
	}
	/**组成查询条件完毕**/
	vtcp_log("sp5983开始查询记录！[%s][%d]",__FILE__,__LINE__);
	strcat(wherelist," ac_sts in ('1','2','3','4','5') and prdt_no='3Y1' and bal>0");
 /*开始查询*/
 	ret = Ln_mst_Dec_Sel( g_pub_tx.reply,wherelist );
 	ERR_DEAL
  ttlnum=0;
  while(1)
  {
 		  long days=0;
 		  double lx=0.00;
 			ret=Ln_mst_Fet_Sel(&f_ln_mst,g_pub_tx.reply);
 			if( ret==100) 
 			{
 				break;
 			}
 			ERR_DEAL
			if( !ttlnum )
			{
					pub_base_AllDwnFilName(cFilename);
					fp = fopen(cFilename,"w");
					if( fp==NULL )
					{
						sprintf(acErrMsg," open file [%s] error ",cFilename);
						WRITEMSG
						strcpy( g_pub_tx.reply,"S047" );
						goto ErrExit;
					}
					fprintf( fp,"~账号|@开户行|@户名|$余额|本金基数|开户日期|起始日期|终止日期|正常利率|利    息|计息天数|\n" );			
   		}
   		 if(f_ln_mst.ic_date<=f_ln_mst.mtr_date)
   		 {
   		 		ret=pub_base_CalDays(f_ln_mst.ic_date,f_ln_mst.mtr_date,"0",&days);
   		 		vtcp_log("aaaaaaaaaaa days=%ld\n",days);
   		 }
   		 else
   		 {
    		 		if(lTx_edate==0)
    		 		{
    		 				ret=pub_base_CalDays(f_ln_mst.ic_date,g_pub_tx.tx_date,"0",&days);
  		  		}
  		  		else
  		  		{
    		 				ret=pub_base_CalDays(f_ln_mst.ic_date,lTx_edate,"0",&days);
  		  		}
   		 }	
   		 lx=days*f_ln_mst.bal*f_ln_mst.rate/1000/30;
   		 lx = pub_base_PubDround_1(lx) ;
   		 vtcp_log("aaaaaaaaaaa lx=%lf\n",lx);
   		 if(memcmp(f_ln_mst.opn_br_no,cBr_no,5)!=0)
   		 {
   		 			continue;
   		 }
   		 ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld",f_ln_mst.ac_id);
   		 
   		 if(f_ln_mst.ic_date<=f_ln_mst.mtr_date)
   		 {
			 		fprintf( fp,"%s|%s|%s|%.2lf|%.2lf|%ld|%ld|%ld|%.2lf|%.2lf|%ld|\n",g_mdm_ac_rel.ac_no,f_ln_mst.opn_br_no,f_ln_mst.name,f_ln_mst.bal,f_ln_mst.bal*days,f_ln_mst.opn_date,f_ln_mst.ic_date,f_ln_mst.mtr_date,f_ln_mst.rate,lx,days);
			 }
			 else
			 {
			 			if(lTx_edate==0)
    		 		{
			 				fprintf( fp,"%s|%s|%s|%.2lf|%.2lf|%ld|%ld|%ld|%.2lf|%.2lf|%ld|\n",g_mdm_ac_rel.ac_no,f_ln_mst.opn_br_no,f_ln_mst.name,f_ln_mst.bal,f_ln_mst.bal*days,f_ln_mst.opn_date,f_ln_mst.ic_date,g_pub_tx.tx_date,f_ln_mst.rate,lx,days);    		 			
    		 		}
    		 		else
    		 		{
			 				fprintf( fp,"%s|%s|%s|%.2lf|%.2lf|%ld|%ld|%ld|%.2lf|%.2lf|%ld|\n",g_mdm_ac_rel.ac_no,f_ln_mst.opn_br_no,f_ln_mst.name,f_ln_mst.bal,f_ln_mst.bal*days,f_ln_mst.opn_date,f_ln_mst.ic_date,lTx_edate,f_ln_mst.rate,lx,days);    		 			    		 			
    		 		}
			 	}
			 ttlnum++;
  }
	ret=Ln_mst_Clo_Sel();
	ERR_DEAL
	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);
		set_zd_data( DC_FILE_SND,"1");
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
