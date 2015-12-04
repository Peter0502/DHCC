/*********************************************************************************
***文 件 名:  sp1715.c                                                         ***
***功能描述： 卡信息查询                                                ***
***                                                                            ***
***作    者:  wangwenkui                                                      ***
***完成日期： 2010-12-14 17:23                                                 ***
***                                                                            ***
***记    录： 根据折子号码在mdm_ac_rel表中查询到客户的客户号，  ***
***           然后根据客户号到其他客户相关信息表中查询到客户相关资料信息       ***
**********************************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "cif_basic_inf_c.h"
#include "mo_loss_c.h"


char mapstat(char *stat);
int sp1715()
{
	struct mdm_ac_rel_c st_mdm_ac_rel;
	struct dd_mst_c st_dd_mst;
	struct cif_basic_inf_c st_cif_basic_inf;
	struct mo_loss_c	st_mo_loss;
	char cCheckPWD[2];
	char cStat;
	int iCif_no=0;
	int ret=0;
	
	memset( &g_pub_tx,0x00,sizeof(g_pub_tx) );
	memset(&st_dd_mst,0x00,sizeof(st_dd_mst));
	memset(&st_mdm_ac_rel,0x00,sizeof(st_mdm_ac_rel));
	memset(&st_cif_basic_inf,0x00,sizeof(st_cif_basic_inf));
	memset(&st_mo_loss, 0x00, sizeof(st_mo_loss));
	memset(cCheckPWD, 0, sizeof(cCheckPWD));	
	pub_base_sysinit();

	get_zd_data("0620",st_mdm_ac_rel.ac_no );		/*折子号码*/
	get_zd_data("0690",cCheckPWD);		/**是否查处密码，1 - 检查，0 - 不检查**/
	
	/**根据折子号找客户id**/
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&st_mdm_ac_rel,"ac_no='%s'", st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	if(cCheckPWD[0] == '1')
	{/**验证密码**/
		get_zd_data("0790",g_pub_tx.draw_pwd);
		if( pub_base_DesChk(g_pub_tx.tx_date, st_mdm_ac_rel.ac_no, g_pub_tx.draw_pwd, g_mdm_ac_rel.draw_pwd) )
      {
          sprintf(acErrMsg,"密码不符!");
          WRITEMSG
          strcpy(g_pub_tx.reply,"M005");
          return 1;
      }
    }
	st_dd_mst.ac_id=st_mdm_ac_rel.ac_id;
	set_zd_data("0620",st_mdm_ac_rel.id_no);  /**证件号码**/
	set_zd_data("0670",st_mdm_ac_rel.id_type); /**证件类型**/
	cStat = mapstat(st_mdm_ac_rel.note_sts);
	set_zd_data("0690", &cStat);		/**介质状态**/
	set_zd_long("0450",st_mdm_ac_rel.beg_date);		/**启用日期**/
	set_zd_data("0730",st_mdm_ac_rel.opn_br_no);	/**开户机构码**/
	/**根据客户id找客户号**/
	ret=Dd_mst_Sel(g_pub_tx.reply,&st_dd_mst,"ac_id='%ld'", st_dd_mst.ac_id);
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	else if(ret==0)
	{
		iCif_no=st_dd_mst.cif_no;
		vtcp_log( "[%s][%d] 客户号：[%ld]",__FILE__,__LINE__,iCif_no);
		set_zd_long("0540",iCif_no);   /**客户号**/
	}
	
	/**根据客户号到cif_basic_inf表中查询客户相关信息**/
	ret=Cif_basic_inf_Sel(g_pub_tx.reply,&st_cif_basic_inf," cif_no=%ld ",iCif_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"无匹配的纪录!! ");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O043");
		goto ErrExit;
	}
	else if(ret==0)
	{
		set_zd_data("0260",st_cif_basic_inf.name);           /**客户名**/
		set_zd_data("0340",st_cif_basic_inf.lvl);            /**信誉等级**/
	}
	ret=Mo_loss_Sel(g_pub_tx.reply,&st_mo_loss," ac_no='%s' and unloss_date != 0 ",st_mdm_ac_rel.ac_no);
	if(ret==100)
	{
		set_zd_data("0710","0");		/**挂失状态**/
	}
	else if(ret==0)
	{
		if(st_mo_loss.loss_ind[0] == '1')
			{
				set_zd_data("0710","0");		/**挂失状态**/
			}else 
			{
				set_zd_data("0710","0");		/**挂失状态**/
			}
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
char mapstat(char *stat)
{
	if(stat[0] == '*')
		{
			return '3';
		}else if(stat[0] == '0')
		{
			return '1';
		}else if(stat[0] == '1')
		{
			return '6';
		}else if(stat[0] == '2')
		{
			return '6';
		}else if(stat[0] == '3')
		{
			return '9';
		}else if(stat[0] == '4')
		{
			return '2';
		}else 
		{
			return '9';
		}
}
