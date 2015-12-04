/*************************************************
* 文 件 名:  spH626.c
* 功能描述： 查询单位编号信息
*
* 作    者:  jack
* 完成日期： 2004年5月26日
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
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"

int spH626()
{
		struct ag_unitinfo_c sUnitInfo;	/*单位信息表*/
		int ret = 0;
		char cBr_no[6];
		char cAc_no[20];
  	char cUnit_no[5];
  	char cUnit_name[51];
  	char cBank_ac_no[26];
  	char cOpr_attri[2];
  	char cPay_type[3];
  	char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		FILE *fp;
		char filename[100];
		char f_opr_attri[5];
		char f_pay_type[21];
	memset( cAc_no, 0x0, sizeof(cAc_no) );
	memset( cUnit_no,0x0,sizeof(cUnit_no));
	memset( cUnit_name,0x0,sizeof(cUnit_name));
	memset( cBank_ac_no,0x0,sizeof(cBank_ac_no));
	memset( cOpr_attri,0x0,sizeof(cOpr_attri));
	memset( cPay_type,0x0,sizeof(cPay_type));
	memset( wherelist,0,sizeof(wherelist) );
	memset( cBr_no,0x0,sizeof(cBr_no));
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data( "0300", cAc_no );
	get_zd_data( "0920", cUnit_no);
	get_zd_data( "0250", cUnit_name);
	get_zd_data( "0710", cOpr_attri);
	get_zd_data( "0210", cPay_type);
	pub_base_old_acno( cAc_no );
	/*set_zd_data( "0300", cAc_no );*/
  /**组成查询条件**/
	if( strlen( cAc_no ) )
	{
		sprintf( tmpstr," bank_acct_no='%s' and ",cAc_no );
		strcat( wherelist,tmpstr );
	}
	if( strlen( cUnit_no ) )
	{
		sprintf( tmpstr," unit_no='%s' and ",cUnit_no );
		strcat( wherelist,tmpstr );
	}
	if( strlen( cUnit_name ) )
  {
  	sprintf( tmpstr," unit_name like '%%%%%s%%%%' and ",cUnit_name );
    strcat( wherelist,tmpstr );
  }
  if( strlen( cOpr_attri ) )
  {
  	sprintf( tmpstr," opr_attri='%s' and ",cOpr_attri );
  	strcat( wherelist,tmpstr );
  }
  if( strlen(cPay_type) )
  {
  	sprintf( tmpstr," pay_type='%s' and ",cPay_type );
  	strcat( wherelist,tmpstr);
  }
  	sprintf(tmpstr,"opn_br_no='%s' and ",cBr_no);
    strcat( wherelist,tmpstr);
	/**组成查询**/
	strcat(wherelist," 1=1 order by unit_no");
	vtcp_log( "WHERE [%s]",wherelist );
	ret=Ag_unitinfo_Dec_Sel(g_pub_tx.reply,wherelist);
	ERR_DEAL

  ttlnum=0;
  while(1)
	{
		ret=Ag_unitinfo_Fet_Sel(&sUnitInfo, g_pub_tx.reply );
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
			fprintf( fp,"~单位编号|单位名称|代理账号|业务性质|代理类型|建立柜员|建立机构|\n" );
		}

		pub_base_dic_show_str(f_opr_attri,"opr_attri",sUnitInfo.opr_attri);
    pub_base_dic_show_str(f_pay_type,"ag_pay_type",sUnitInfo.pay_type);
		fprintf( fp, "%s|%s|%s|%s|%s|%s|%s|\n",sUnitInfo.unit_no,sUnitInfo.unit_name,sUnitInfo.bank_acct_no,f_opr_attri,f_pay_type,sUnitInfo.opn_tel,sUnitInfo.opn_br_no);
		ttlnum++;
	}
	ret=Ag_unitinfo_Clo_Sel();
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
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
