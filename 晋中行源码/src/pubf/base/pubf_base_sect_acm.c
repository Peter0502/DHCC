#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_base_sect_acm.c
* 功能描述：
*             pubf_base_InsertSectAcm登记分段积数
* 调用函数说明：
*               
*
* 作    者:     rob
* 完成日期：    2003年12月28日
*
* 修改记录：
* 1. 日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_sect_acm_c.h"

/**********************************************************************
* 函数名：    pub_base_InsertSectAcm()
* 函数功能：  向分段积数表中插入数据
* 作者/时间： 2003年12月28日
*
* 参数：
*     输入：
*          账户ID				integer
*          账户序号				smallint
*          账户类型				char(1)｛1：存款；2：贷款本金积数； 3：贷款表内欠息积数；4：贷款表外欠息积数；5：贷款复利积数；6：透支｝
*          起息日期				integer
*          登记日期				integer
*          利息积数				decimal(19,2)
*          利率					decimal(8,6)
*     输出:
*          响应码				char(4)
* 返 回 值: 0 成功
*
* 修改历史：
*
********************************************************************/

int pub_base_InsertSectAcm(long ac_id, int ac_seqn, char ac_type,
		  long ic_date, long wrk_date, double intst_acm,
		  double rate, char *reply)
{
    struct com_sect_acm_c com_sect_acm_c;
	int ret;

	if ( pub_base_CompDblVal( intst_acm , 0.00 )==0 )
	{
		sprintf(acErrMsg,"积数为零,不处理!");
    	WRITEMSG
		return 0;
	}

    sprintf(acErrMsg,"ac_id=[%ld],ac_seqn=[%d],ac_type=[%c]",
                      ac_id,ac_seqn,ac_type);
    WRITEMSG
    
	MEMSET_DEBUG(&com_sect_acm_c, 0x0, sizeof(struct com_sect_acm_c));

	com_sect_acm_c.ac_id = ac_id;
	com_sect_acm_c.ac_seqn = ac_seqn;
	com_sect_acm_c.beg_date = ic_date;
	com_sect_acm_c.end_date = wrk_date;
	com_sect_acm_c.rate = rate;
	com_sect_acm_c.intst_acm = intst_acm;

    
	if (ac_type == '1')
	{
		com_sect_acm_c.intst_type[0] = '0';
	}
	else if (ac_type == '2')
	{
		com_sect_acm_c.intst_type[0] = '1';
	}
	else if (ac_type == '3')
	{
		com_sect_acm_c.intst_type[0] = '2';
	}
	else if (ac_type == '4')
	{
		com_sect_acm_c.intst_type[0] = '3';
	}
	else if (ac_type == '5')
	{
		com_sect_acm_c.intst_type[0] = '4';
	}
	else if (ac_type == '6')
	{
		com_sect_acm_c.intst_type[0] = '5';
	}
	else
	{
		
        sprintf(acErrMsg,"  ac_type error!! %d",ret);
        WRITEMSG
        strcpy ( g_pub_tx.reply , "W003" );
        return 1;

	}
	com_sect_acm_c.sts[0] = '0';

    ret = Com_sect_acm_Ins(com_sect_acm_c, reply);
    if (ret && ret != -239)
	{
		sprintf(acErrMsg, "INSERT com_sect_acm error %d", ret);
		WRITEMSG
		strcpy(reply, "D107");
		return 1;
	}
	else if (ret == -239)
	{
		sprintf(acErrMsg, "INSERT com_sect_acm error %d", ret);
		WRITEMSG
		strcpy(reply, "D108");
		return 1;
	}

	return 0;
}
