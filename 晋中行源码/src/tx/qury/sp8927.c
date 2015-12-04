/**************************************************************
* 文 件 名: sp8927.c
* 功能描述：可用资金管理历史表查询
* 数据库表：zjgl_mst_hst_hst
*
* 作    者: jack
* 完成日期：2004年3月28日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "zjgl_mst_hst_c.h"

sp8927()
{
	int ret = 0;
	char flag[2];        
	char wherelist[128];    /***查询条件***/
  long lTx_date=0;
	char cOpn_br_no[6];   /***开户机构***/
	char cCur_no   [3];   /***币种***/
	char cAcc_no   [8];   /***科目号***/
	char tmpstr[128];
	char cBr_no[6];
	char cFlag[2];
	struct zjgl_mst_hst_c zjgl_mst_hst;  /*可用资金管理历史表*/
  int  ttlnum=0;
  FILE *fp;
  char filename[100];
  char cStat[7];
  char cStat1[7];
  char cCurName[11];
  char cIntsttype[13];
  memset(cIntsttype,0x0,sizeof(cIntsttype));
  memset(cCurName,0x0,sizeof(cCurName));
  memset(cStat,0x0,sizeof(cStat));
  memset(cStat1,0x0,sizeof(cStat1));
  memset(filename,0x0,sizeof(filename));
	memset(wherelist,0,sizeof(wherelist));
  memset(tmpstr,0x0,sizeof(tmpstr));
	memset(&zjgl_mst_hst,0x0,sizeof(zjgl_mst_hst));
  memset(cFlag,0x0,sizeof(cFlag));
	memset(cOpn_br_no,0x0,sizeof(cOpn_br_no));
	memset(cCur_no,0x0,sizeof(cCur_no));
	memset(cAcc_no,0x0,sizeof(cAcc_no));
	pub_base_sysinit();
  
	/*-------- 取输入变量  ---------*/
	memset(cOpn_br_no,'\0',sizeof(cOpn_br_no));
	get_zd_data("0910",cOpn_br_no);
	get_zd_data("0220",cCur_no);
	get_zd_data("0650",cAcc_no);
	get_zd_long("0440",&lTx_date);
	get_zd_data("0030",cBr_no);
	if(strlen(cOpn_br_no))
	{
	 	sprintf(tmpstr,"opn_br_no='%s' and ",cOpn_br_no);
	 	strcat(wherelist,tmpstr);
	}
	else{
		if(memcmp(cBr_no,QS_BR_NO,sizeof(cBr_no)-1)!=0)
	 		sprintf(tmpstr,"opn_br_no='%s' and ",cBr_no);
	 	strcat(wherelist,tmpstr);
	}
	if(strlen(cCur_no))
	{
	 sprintf(tmpstr,"cur_no='%s' and ",cCur_no);
	 strcat(wherelist,tmpstr);
	}
	if(strlen(cAcc_no))
	{
		sprintf(tmpstr,"acc_no like '%%%%%s%%%%' and ",cAcc_no);
	 	strcat(wherelist,tmpstr);
	}
	if(lTx_date)
	{
		sprintf(tmpstr,"tx_date=%ld and ",lTx_date);
	 	strcat(wherelist,tmpstr);
	}
		strcat(wherelist," 1=1 order by tx_date,opn_br_no,acc_no");
	vtcp_log( "WHERE [%s]",wherelist );
	ret=Zjgl_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		memset(&zjgl_mst_hst,0x0,sizeof(zjgl_mst_hst));
		ret=Zjgl_mst_hst_Fet_Sel( &zjgl_mst_hst, g_pub_tx.reply );
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
		 fprintf( fp,"~科目号|@每日增减额|@借方余额|@贷方余额|@交易日期|机构||@借方积数|@贷方积数|\n" );
		}
		memset(cStat,0x0,sizeof(cStat));
		if(zjgl_mst_hst.add_ind[0]=='1')
		{
			memcpy(cStat,"借",2);
		}
		else
		{
			memcpy(cStat,"贷",2);
		}
		memset(cStat1,0x0,sizeof(cStat1));
		if(zjgl_mst_hst.ct_ind[0]=='1')
		{
			memcpy(cStat1,"现金",4);
		}
		else
		{
			memcpy(cStat1,"转帐",4);
		}
		  memset(cCurName,0x0,sizeof(cCurName));
    		fprintf( fp, "%s|%.2lf|%.2lf|%.2lf|%ld|%.5s|%.2lf|%.2lf|\n",zjgl_mst_hst.acc_no,zjgl_mst_hst.tx_amt,zjgl_mst_hst.dr_bal,zjgl_mst_hst.cr_bal,zjgl_mst_hst.tx_date,zjgl_mst_hst.opn_br_no,zjgl_mst_hst.dr_intst_acm,zjgl_mst_hst.cr_intst_acm);
		ttlnum++;
	}

	ret=Zjgl_mst_hst_Clo_Sel( );
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
