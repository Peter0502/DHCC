/*************************************************
* 文 件 名:  sp8990.c
* 功能描述： 财政账户信息查询
*
* 作    者:  LinGuoTao
* 完成日期： 20110726
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
#include "com_branch_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"

int sp8990()
{
	vtcp_log( "8990开始=== " );
	struct dd_mst_c vdd_mst;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	char ac_no[25];
	char filename[100];
	double sumbal=0.0;
	int ttlnum=0;			/**读取的总条数**/
	int ret=0;
	FILE *fp;
	
	
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( &v_mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset( &vdd_mst , 0x00 , sizeof( struct dd_mst_c ) );
	
	pub_base_sysinit();

	
	get_zd_data("0380",ac_no);
	pub_base_strpack(ac_no);
	ret=Mdm_ac_rel_Dec_Sel( g_pub_tx.reply,
				" ac_no like '%s%%' ",ac_no );
				 
				ERR_DEAL
				ttlnum=0;
			while(1)
			{
				ret=Mdm_ac_rel_Fet_Sel( &v_mdm_ac_rel,g_pub_tx.reply );
	
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
					fprintf( fp,"~@财政帐号|@户名|@余额|\n" );
				}
				ttlnum++;
				
				ret=Dd_mst_Dec_Sel( g_pub_tx.reply," ac_id=%d ",v_mdm_ac_rel.ac_id  );
				ERR_DEAL
				while(1)
				{
					ret=Dd_mst_Fet_Sel( &vdd_mst, g_pub_tx.reply );
					if( ret==100 ) break;
					sumbal+=vdd_mst.bal;
				}
				set_zd_data("0250",vdd_mst.name);
				ret=Dd_mst_Clo_Sel( );
				ERR_DEAL
			}
			ret=Mdm_ac_rel_Clo_Sel( );
			ERR_DEAL
			vtcp_log( "kill=== [%f]",sumbal );
			if( !ttlnum )
			{
				strcpy( g_pub_tx.reply,"S049" );
				goto ErrExit;
			}
			else{
			fprintf( fp, "%s|%s|%f|\n",ac_no,vdd_mst.name,sumbal);
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
