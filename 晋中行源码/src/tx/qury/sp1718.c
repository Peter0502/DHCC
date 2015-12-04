/*********************************************************************************
***文 件 名:  sp1718.c                                                         ***
***功能描述： 个人存款证明短交易                                                 ***
***                                                                            ***
***作    者:  张国锋                                                       ***
***完成日期： 2014-02-26                                                      ***
***                                                                            ***
***记    录：        ***
insert into tx_def values('1718','卡号对应证件类型查询','00000000000000000000111111111111110011111111111110000100000001100000000001000000000100000000000000000000000000000000000000000000','0','4','0');
insert into tx_flow_def values('1718',0,'1718','#$');
insert into tx_sub_def values('1718','卡号对应证件类型查询','sp1718','0','0');
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
#include "td_mst_c.h"
#include "dd_parm_c.h"
#include "td_parm_c.h"
#include "prdt_ac_id_c.h"

int sp1718()
{
	struct dd_mst_c vdd_mst;
	struct td_mst_c vtd_mst;
	struct dd_parm_c vdd_parm;
	struct td_parm_c vtd_parm;
	struct prdt_ac_id_c prdt_ac_id_c;
  struct mdm_ac_rel_c mdm_ac_rel;
	
	int ret=0;
	char ac_no1[20] = {0};
	char id_no[19] = {0};
	char t_name[41];
	char ac_sts[41];
	char parm_title[41];
	double  t_bal=0.00;
	long end_date=0;
	long opn_date=0;
	char cur_no[4]={0};
	
	memset(&mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx) );
	memset(&vdd_mst,0x00,sizeof(vdd_mst));

	pub_base_sysinit();

	get_zd_data("0300",ac_no1 );		/*折子号码*/
	get_zd_data("0630",id_no );
 
	if(strlen(ac_no1) != 0)
	{
		memset( &mdm_ac_rel , 0x00 , sizeof( struct mdm_ac_rel_c ) );
		memset(&prdt_ac_id_c,0x00,sizeof(prdt_ac_id_c));
		memset(&vdd_mst,0x00,sizeof(vdd_mst));
		memset(&vdd_parm,0x00,sizeof(vdd_parm));
		memset(&vtd_mst,0x00,sizeof(vtd_mst));
		memset(&vtd_parm,0x00,sizeof(vtd_parm));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel,"ac_no = '%s' and note_sts<>'*' order by opn_br_no,ac_id,ac_seqn", ac_no1);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		if(strcmp(mdm_ac_rel.id_no,id_no))
		{
			sprintf(acErrMsg,"该卡/账号证件类型不符!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}
		if(memcmp(mdm_ac_rel.opn_br_no, g_pub_tx.tx_br_no, 5)!=0)
		{
			sprintf(acErrMsg,"交易机构和开户行不符! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"D245");
			goto ErrExit;
		}
		ret=Prdt_ac_id_Sel(g_pub_tx.reply,&prdt_ac_id_c,"ac_id=%ld and ac_id_type in ('1','2') ",mdm_ac_rel.ac_id);
		if( ret )
		{
			sprintf(acErrMsg,"账号不存在![%ld][%d]",mdm_ac_rel.ac_id,mdm_ac_rel.ac_seqn);
			WRITEMSG
			strcpy(g_pub_tx.reply,"P102");
			goto ErrExit;
		}
		
    switch( prdt_ac_id_c.ac_id_type[0] )
	 	{
		 case '1':	/*活期*/
		 
				ret = Dd_mst_Sel( g_pub_tx.reply,&vdd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id);
				ERR_DEAL
				
				/**读取产品参数**/
	      ret = Dd_parm_Sel( g_pub_tx.reply,&vdd_parm," prdt_no='%s' ",vdd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vdd_mst.bal;
				end_date=vdd_mst.val_date;
				opn_date=vdd_mst.opn_date;
				strcpy(cur_no,vdd_parm.cur_no);
				strcpy(t_name,vdd_mst.name);
				strcpy(parm_title,vdd_parm.title);
        pub_base_dic_show_str(ac_sts,"dd_ac_sts",vdd_mst.ac_sts);
        break;
     case '2':	/*定期*/
     	  ret = Td_mst_Sel( g_pub_tx.reply,&vtd_mst," ac_id=%ld  ",mdm_ac_rel.ac_id );
				ERR_DEAL
				
				/**读取产品参数**/
	      ret = Td_parm_Sel( g_pub_tx.reply,&vtd_parm," prdt_no='%s' ",vtd_mst.prdt_no );
	      ERR_DEAL
				
				t_bal=vtd_mst.bal;
				end_date=vtd_mst.mtr_date;
				opn_date=vtd_mst.opn_date;
				strcpy(cur_no,vtd_parm.cur_no);
				strcpy(t_name,vtd_mst.name);
				strcpy(parm_title,vtd_parm.title);
    		pub_base_dic_show_str(ac_sts,"td_ac_sts",vtd_mst.ac_sts);
    		break;
     default:	
				sprintf(acErrMsg,"无此账号类型![%s]",prdt_ac_id_c.ac_id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply,"P104");
				goto ErrExit;
   }
	}
	

	vtcp_log( "[%s][%d] id_no[%s]aaaa[%s]",__FILE__,__LINE__, mdm_ac_rel.id_no,id_no);

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
