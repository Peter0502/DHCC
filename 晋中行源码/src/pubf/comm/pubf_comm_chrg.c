#include "find_debug.h" 
/*************************************************
* 文 件 名:    pubf_comm_GetTrad.c
* 功能描述：
*
* 作    者:    jack
* 完成日期：   2003年12月16日
* 修改记录：
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.
*************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "svrpub_c.h"
#include "public.h"
#include "com_def_chrg_c.h"
#include "mo_def_rate_c.h"
#include "com_chrg_rate_c.h"

/**费用检查主控**/
int pub_comm_chrg_chk()
{
	int ret=0;
	double	amt,chrg_amt,ttlamt;
	char vtx_code[5];
	struct	com_def_chrg_c	def_chrg;
	struct	mo_def_rate_c	mo_def_rate;
	struct	com_chrg_rate_c	com_chrg_rate;
	char filename[100];
	char tmpstr[100];
	FILE *fp;
	int need=0,needauto=0;
	char ind[2];
	char cQuickFlag[2];/* 加急标志 1-加急 其他正常*/

	MEMSET_DEBUG(vtx_code, 0 , sizeof(vtx_code));
	MEMSET_DEBUG(tmpstr, 0 , sizeof(tmpstr));
	MEMSET_DEBUG(filename, 0 , sizeof(filename));
	MEMSET_DEBUG(ind, 0 , sizeof(ind));
	MEMSET_DEBUG(cQuickFlag, 0 ,sizeof(cQuickFlag));

	get_zd_data( "0093",ind );			/*收费手段*/
vtcp_log(" GET 0093 [%s]",ind );
	get_zd_data("122F",cQuickFlag);
vtcp_log(" GET 122F [%s]",cQuickFlag );

	if( ind[0]=='1' || ind[0]=='2' || ind[0]=='3' ) 
		return 0;
	else
		;

	get_zd_data( DC_TX_CODE,vtx_code );

	ttlamt=0.00;

	vtcp_log( "TJ:tx_code='%s' order by seqn",vtx_code );

	ret=Com_def_chrg_Dec_Sel( g_pub_tx.reply,"tx_code='%s' order by seqn",
			vtx_code );
	if( ret )
	{
		goto ErrExit;
	}

	while( 1 )
	{
		ret=Com_def_chrg_Fet_Sel( &def_chrg,g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack( def_chrg.amt_no );
		pub_base_strpack( def_chrg.mo );
		pub_base_strpack( def_chrg.cond );

sprintf(acErrMsg,"收费CHRG[%s][%s]",def_chrg.cond,def_chrg.chrg_mode);
WRITEMSG
		/**条件不符合**/
		if( pub_comm_auth_condchk(def_chrg.cond) )
		{
			continue;
		}

		if( def_chrg.chrg_mode[0]=='A' || def_chrg.chrg_mode[0]=='B' )
		{
			needauto++;
			continue;
		}

		vtcp_log("[%s,%d] Enter pubf_def_chrg_proc!!!!",__FILE__,__LINE__);
		ret=pubf_def_chrg_proc( def_chrg.chrg_rate_code,
			mo_def_rate.chrg_rate_no,def_chrg.amt_no,&amt );
		if( ret ) goto ErrExit;

		vtcp_log("[%s,%d] amt===[%f]",__FILE__,__LINE__,amt);
		vtcp_log("[%s,%d] amt===[%f]",__FILE__,__LINE__,amt);
		vtcp_log("[%s,%d] amt===[%f]",__FILE__,__LINE__,amt);
		vtcp_log("[%s,%d] amt===[%f]",__FILE__,__LINE__,amt);
		vtcp_log("[%s,%d] Enter pubf_acct_chrg_proc!!!!",__FILE__,__LINE__);
		ret=pubf_acct_chrg_proc( mo_def_rate.chrg_rate_no,
			com_chrg_rate.dc_code,&chrg_amt,amt );
		if( ret ) goto ErrExit;

		if( chrg_amt>0.005 ) 
		{
			if( !need )
			{
				set_zd_data(DC_FILE_SND,"1");

				pub_base_AllDwnFilName( filename );
				fp=fopen(filename,"w");
				if (fp==NULL)
				{
					sprintf(acErrMsg,"生成文件错误![%s]",tmpfile);
					WRITEMSG
					strcpy( g_pub_tx.reply, "P157");
					goto ErrExit;
				}

				fprintf( fp,"%s|\n",def_chrg.chrg_mode );
				ind[0]=def_chrg.chrg_mode[0];
			}
			need++;

			fprintf( fp,"%s|%.2lf|\n",def_chrg.mo,chrg_amt );
			ttlamt+=chrg_amt;
		}
	}
	Com_def_chrg_Clo_Sel();

	if( need ) fclose( fp );

		set_zd_double( "1227",ttlamt );

OkExit:
	sprintf(acErrMsg,"收费检查子交易成功!%d,%d",need,needauto);
	WRITEMSG
	if( need )
	{
	strcpy( RtCode,"Z201" );
	set_zd_data(DC_REPLY,RtCode);
	}
	else if( needauto )
	{
	set_zd_data( "0093","2" );
get_zd_data("0093",RtCode);
vtcp_log("=-=-=-=-=-=[%s]",RtCode);
	strcpy( RtCode,"0000" );
	set_zd_data(DC_REPLY,RtCode);
	}
	else 
	{
	set_zd_data( "0093","0" );
	strcpy( RtCode,"0000" );
	set_zd_data(DC_REPLY,RtCode);
	}
	return need;
ErrExit:
	strcpy( RtCode,g_pub_tx.reply );
	set_zd_data(DC_REPLY,RtCode);
    return 0;
ErrExit0:
	strcpy( RtCode,g_pub_tx.reply );
	set_zd_data(DC_REPLY,RtCode);
    return 0;
}

