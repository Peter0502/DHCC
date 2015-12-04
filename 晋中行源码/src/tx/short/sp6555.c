/*************************************************
* 文 件 名: sp6555.c
* 功能描述：根据客户号返回客户风险评估等级
* 作    者: lwb
* 完成日期：2014年10月24日
*************************************************/

#include "public.h"
#include <string.h>
#include "cif_ass_reg_c.h"
#define EXTERN

int sp6555()
{
	int iRet = 0;
	long lCif_no = 0;;
	struct cif_ass_reg_c sCif_ass_reg;
	
	memset(&sCif_ass_reg,0x00,sizeof(struct cif_ass_reg_c));
	
	pub_base_sysinit();
	
	get_zd_long("0280",&lCif_no);
	
	iRet = Cif_ass_reg_Sel(g_pub_tx.reply,&sCif_ass_reg,"cif_no=%d",lCif_no);
	if(iRet == 100)
	{
		sprintf(acErrMsg,"此客户未做过风险等级评估！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C151");
		set_zd_data("0130","此客户未做过风险等级评估！");
		goto ErrExit;	
	}
	else if(iRet)
	{
		sprintf(acErrMsg,"查询客户风险等级表失败！");
		WRITEMSG
		strcpy(g_pub_tx.reply,"C150");
		set_zd_data("0130","查询客户风险等级表失败！");
		goto ErrExit;		
	}
	
	set_zd_long("0440",sCif_ass_reg.ass_date);
	set_zd_long("0450",sCif_ass_reg.end_date);
	set_zd_data("0680",sCif_ass_reg.ass_lvl);

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;

}
