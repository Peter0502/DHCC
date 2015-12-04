/*************************************************
* 文 件 名:    sp9978.c
* 功能描述：   根据帐号回显户名
*
* 作    者:    mike
* 完成日期：   2003年08月14日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "ag_peritemmap_c.h"
sp9978()
{ 		
	int		ret;
	char	addr_tmp[61];
  char  cFlag[2];
  char cUnit_no[5];
  struct ag_peritemmap_c Ag_peritemmap;
	/** 数据初始化 **/
	ret = 0;
	memset(addr_tmp,0x00,sizeof(addr_tmp));
	memset(cFlag,0x00,sizeof(cFlag));
	memset(cUnit_no,0x00,sizeof(cUnit_no));
	memset(&Ag_peritemmap,0x00,sizeof(struct ag_peritemmap_c));
	get_zd_data("0310",g_pub_tx.ac_no);
	get_zd_data("0920",cUnit_no);
  get_zd_data("0200",cFlag);
  if(cFlag[0]=='1')
  {
  	if ( Pub_base_Getacname(g_pub_tx.ac_no,g_pub_tx.name,&g_pub_tx.ac_id, \
				&g_pub_tx.cif_no,&g_pub_tx.tx_amt1,g_pub_tx.reply))
				{
					sprintf(acErrMsg,"ac_no to name erro [%s] ",g_pub_tx.ac_no,g_pub_tx.cif_no);
					WRITEMSG
					goto ErrExit;
				}
				sprintf(acErrMsg,"CHK 9978 [%s] [%d] [%d]",g_pub_tx.name,g_pub_tx.ac_id,g_pub_tx.cif_no);
				WRITEMSG
				ret = Ag_peritemmap_Sel(g_pub_tx.reply,&Ag_peritemmap,"unit_no='%s' and ac_no='%s'",cUnit_no,g_pub_tx.ac_no);
				if(ret==0)
				{
					set_zd_data("0710","1");
				}
				if(ret==100)
				{
					set_zd_data("0963",g_pub_tx.name);
					set_zd_long("0290",g_pub_tx.cif_no);
					set_zd_data("0964",addr_tmp);
					goto OkExit;
				}
				else if(ret)
				{
					sprintf(acErrMsg,"select addr error cif_no=[%d] [%d]",g_pub_tx.cif_no,ret);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C067");
					goto ErrExit;
				}				
				set_zd_data("0963",g_pub_tx.name);
				set_zd_long("0290",g_pub_tx.cif_no);
				set_zd_data("0964",addr_tmp);
	}
	else
	{
				if ( Pub_base_Getacname(g_pub_tx.ac_no,g_pub_tx.name,&g_pub_tx.ac_id, \
				&g_pub_tx.cif_no,&g_pub_tx.tx_amt1,g_pub_tx.reply))
				{
					sprintf(acErrMsg,"ac_no to name erro [%s] ",g_pub_tx.ac_no,g_pub_tx.cif_no);
					WRITEMSG
					goto ErrExit;
				}

				sprintf(acErrMsg,"CHK 9978 [%s] [%d] [%d]",g_pub_tx.name,g_pub_tx.ac_id,g_pub_tx.cif_no);
				WRITEMSG

				/*ret = distinct_sel("cif_addr_inf","addr",addr_tmp,"cif_no=%d",g_pub_tx.cif_no);
				if (ret)
				{
					sprintf(acErrMsg,"select addr error cif_no=[%d] ",g_pub_tx.cif_no);
					WRITEMSG
					strcpy(g_pub_tx.reply,"C067");
					goto ErrExit;
				}*/
				set_zd_data("0963",g_pub_tx.name);
				set_zd_long("0290",g_pub_tx.cif_no);
				set_zd_data("0964",addr_tmp);
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
