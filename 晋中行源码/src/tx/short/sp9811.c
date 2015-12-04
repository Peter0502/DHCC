/*************************************************
* 文 件 名:  sp9811.c
* 功能描述： 根据操作类型和合同号查询拆借登记簿
*
* 作    者:  rob
* 完成日期： 2003年1月17日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_c.h"

int sp9811()
{
    struct mo_ibl_c sMo_ibl_c;
    struct mdm_ac_rel_c sMdm_ac_rel;
    int ret;
    char flag[2];
    char sign[2];
   
    memset(&sMo_ibl_c,0x0,sizeof(struct mo_ibl_c));

    get_zd_data("0670",flag);
    get_zd_data("0720",sign);
    get_zd_data("0730",sMo_ibl_c.pact_no);


    sprintf(acErrMsg,"pact_no=[%s] sign=[%s]!! ",sMo_ibl_c.pact_no,sign);
    WRITEMSG

    if (flag[0] == '0')
    {
    	ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c," pact_no= '%s'",
				sMo_ibl_c.pact_no,sign);
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"合同号可能填写错误!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B003");
        	goto ErrExit;
    	}
		else if (ret == 0)
		{
			sprintf(acErrMsg,"该合同号已经存在!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"B171");
			goto ErrExit;
		}
        goto OkExit;  /* 录入只做检查 */
    }

    ret = Mo_ibl_Sel(g_pub_tx.reply,&sMo_ibl_c," pact_no= '%s' and sign='%s'",
                     sMo_ibl_c.pact_no,sign);
    if (ret != 0)
    {
        sprintf(acErrMsg,"合同号可能填写错误!! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply, "B003");
        goto ErrExit;
    }
    
	/******** delete by rob 20050706
	if (sMo_ibl_c.op_ac_id != 0)
	{
    	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_id = %ld and \
           	ac_seqn= 0 ",sMo_ibl_c.op_ac_id);
    	if (ret != 0)
    	{
        	sprintf(acErrMsg,"查询对方账号错误!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply, "B004");
        	goto ErrExit;
    	}
		set_zd_data( "0300", sMdm_ac_rel.ac_no); *对方账号*
    }
	else
	{
		set_zd_data("0300",sMo_ibl_c.acc_no); * 科目号 *
	}
	*********/
	set_zd_data( "0300", sMo_ibl_c.op_ac_no); /*对方账号*/
    
	set_zd_data( "0230", sMo_ibl_c.prdt_no); /*产品编号*/
	set_zd_data( "0250", sMo_ibl_c.op_name);  /*拆借户名*/
	set_zd_double( "0400", sMo_ibl_c.amt);  /*拆借金额*/
	set_zd_double( "0840", sMo_ibl_c.rate); /*拆借利率*/
	set_zd_double( "0850", sMo_ibl_c.over_rate); /*拆借利率*/
	set_zd_long( "0440", sMo_ibl_c.matr_date);/*到期日期*/
	set_zd_long("0360", sMo_ibl_c.term); /* 拆借期限 */
	
  set_zd_data("0310",sMo_ibl_c.kh_ac_no);  /*(本行客户帐号)*/
  set_zd_data("0260", sMo_ibl_c.kh_name);  /*本行客户名*/
	set_zd_data("0520", sMo_ibl_c.op_br_no); /*外行行号*/
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
