/*************************************************
* 文 件 名:  sp9708.c
* 功能描述： 由卡号回显户名
*
* 作    者:  rob 
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9781()
{
	int	ret;
	char card_no[20],tmp_name[50];
	struct 	prdt_ac_id_c   g_prdt_ac_id;			/*产品账户结构*/

	/*清空*/
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
	memset(&g_td_mst,0x00,sizeof(struct td_mst_c));

	/*前台取值*/
	get_zd_data("0370",card_no);       				/*卡号*/

   	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",card_no);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"无此卡号，请检查!card_no=[%s]", card_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "读取介质账户对照表异常![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W011" );
        goto ErrExit;
    }

	/*根据账户ID找出账户类型*/
   	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
						"ac_id=%ld",g_mdm_ac_rel.ac_id);
   	if( ret==100 )
   	{
        sprintf( acErrMsg,"无此卡号，请检查!card_no=[%ld]", card_no );
        WRITEMSG
        strcpy( g_pub_tx.reply, "M003" );
        goto ErrExit;
   	}
    else if( ret )
   	{
        sprintf( acErrMsg, "读取产品账户对照表异常![%d]", ret );
        WRITEMSG
        strcpy( g_pub_tx.reply, "W013" );
        goto ErrExit;
    }

	switch(g_prdt_ac_id.ac_id_type[0])
	{
		case '1':							/*1-活期*/
   				ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
								"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该账户ID不存在!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "读取活期主文件异常![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W015" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_dd_mst.name);
				break;
		case '2':							/*2-定期*/
   				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,
									"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该账户ID不存在!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "读取定期主文件异常![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W017" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_td_mst.name);
				break;
		case '3':							/*3-贷款*/
   				ret=Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,
									"ac_id=%ld",g_prdt_ac_id.ac_id);
   				if( ret==100 )
   				{
        			sprintf( acErrMsg,"该账户ID不存在!card_no=[%ld]", 
							card_no );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "M003" );
        			goto ErrExit;
   				}
    			else if( ret )
   				{	
        			sprintf( acErrMsg, "读取贷款主文件异常![%d]", ret );
        			WRITEMSG
        			strcpy( g_pub_tx.reply, "W019" );
        			goto ErrExit;
    			}
				strcpy(tmp_name,g_ln_mst.name);
				break;
		default : 	
        		sprintf( acErrMsg, "该卡异常!![%d]", ret );
        		WRITEMSG
        		strcpy( g_pub_tx.reply, "O024" );
        		goto ErrExit;
				break;
	}

    /* 向前台屏幕赋值 */
	set_zd_data("0250",tmp_name);						/*户名*/

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

