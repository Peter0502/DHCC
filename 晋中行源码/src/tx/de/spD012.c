/***************************************************************
* 文 件 名:     spD012.c
* 功能描述：	利息测算(具体帐户)
*
* 作    者:     jack
* 完成日期：    2004年1月10日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD012()
{
    double dealval;
    double val;
    double factval;
    double keepval;
    int flag;
    int year,month,day;
   
	 /*清空结构*/
    if (init_struct())
    {
        strcpy(acErrMsg,"结构清空出错!");
        WRITEMSG
        goto ErrExit;
    }

    /*** 初始化全局变量 ***/
    if (pub_base_sysinit())
    {
        strcpy(acErrMsg,"初始化全局变量出错!");
        WRITEMSG
        goto ErrExit;
    }
    
    /*取值、赋值*/
    if (getpm_fun())
    {
        strcpy(acErrMsg,"取值、赋值出错!");
        WRITEMSG
        goto ErrExit;
    }
    
    sprintf(acErrMsg,"ac_no=[%s] ac_seqn =[%d],tx_date=[%ld]!! ",g_pub_tx.ac_no,g_pub_tx.ac_seqn,g_pub_tx.tx_date);
    WRITEMSG
    
    if (pub_base_disac_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,g_pub_tx.ac_type))
    {
        strcpy(acErrMsg,"取主文件和产品参数出错!!");
        WRITEMSG
        goto ErrExit;
    }

    if (g_pub_tx.ac_id_type[0] == '2')
    { /* 定期 */
        g_pub_tx.tx_amt1 = g_td_mst.bal;
		g_pub_intst.edu_ind[0] = 'Y';

		/*** 调用计算利息积数的函数 **** 定额续取 定额续存 教育储蓄*/ 
		if( g_td_parm.td_type[0] == '2' || g_td_parm.td_type[0] == '1' || 
			g_td_parm.td_type[0] == '7' ) 
		{ 
			if( pub_base_CalAcm(g_td_mst.lst_date,
				g_pub_tx.tx_date>g_td_mst.mtr_date?g_td_mst.mtr_date:g_pub_tx.tx_date, g_td_parm.intst_term_type,g_td_parm.acm_calc_mode, g_td_mst.bal,-g_pub_tx.tx_amt1,&g_td_mst.intst_acm, g_td_mst.intst_type,g_td_parm.intst_mon,g_td_parm.intst_day) ) 
			{ 
				sprintf(acErrMsg,"计算利息积数错误!"); 
				WRITEMSG 
				goto ErrExit; 
			} 
		}

        sprintf(acErrMsg,"g_td_parm.intst_term_type=[%s]!!",g_td_parm.intst_term_type);
        WRITEMSG
		dealval = 0.00;
		factval = 0.00; 
		val = 0.00;
		keepval = 0.00;

        if (pub_base_CalIntstTd(&dealval,&factval,&val,&keepval,g_pub_tx.reply))
        {
            strcpy(acErrMsg,"计算定期利息出错!!!");
            WRITEMSG
            goto ErrExit;
        }
    }
    else if (g_pub_tx.ac_id_type[0] == '1')
    { /* 活期 */
        g_pub_tx.tx_amt1 = g_dd_mst.bal;

        sprintf(acErrMsg,"活期计息!!");
        WRITEMSG

        flag = 1; /* 销户结息 */
        year = floor(g_pub_tx.tx_date / 10000);
        month = floor((g_pub_tx.tx_date - year * 10000) / 100);
        day = g_pub_tx.tx_date - year * 10000 - month * 100;

        if ((month == 6) && (day == 30))
        {
           flag = 2; /* 结息日计息 */
        }

		if( pub_base_CalAcm(g_dd_mst.lst_date,g_pub_tx.tx_date, 
				g_dd_parm.intst_term_type,g_dd_parm.acm_calc_type, 
				g_dd_mst.bal,-g_pub_tx.tx_amt1,&g_dd_mst.intst_acm, 
				g_dd_mst.intst_type,g_dd_parm.intst_mon,g_dd_parm.intst_day) ) 
		{ 
			sprintf(acErrMsg,"计算利息积数错误!"); 
			WRITEMSG 
			goto ErrExit; 
		}
         
        if (pub_base_CalIntstHQ(flag,&dealval,&factval,&val,&keepval,g_pub_tx.reply))
        {
            strcpy(acErrMsg,"计算活期利息出错!!!");
            WRITEMSG
            goto ErrExit;
        }
      
    }
    else
    {
       sprintf(acErrMsg,"该账户不能不是定期或活期帐户,不能计算");
       WRITEMSG
       strcpy(g_pub_tx.reply,"");
       goto ErrExit;    
    }

    sprintf(acErrMsg,"g_pub_tx.tx_amt1=[%lf]!!",g_pub_tx.tx_amt1);
    WRITEMSG
    
    sprintf(acErrMsg,"dealval=[%lf],factval=[%lf],val=[%lf]!!",dealval,factval,val);
    WRITEMSG
    
	/*** 输出变量 ***/
    set_zd_double("BJ00",g_pub_tx.tx_amt1);
    set_zd_double("102P",dealval);
    set_zd_double("102Q",val);
    set_zd_double("102R",keepval);
    set_zd_double("102S",g_pub_tx.tx_amt1 + factval + keepval);

	sql_rollback();
	sql_begin();
    
OkExit:
	strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg," 具体帐户计息成功!! ");
    WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    sprintf(acErrMsg,"具体帐户计息不成功!!");
    WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}

int init_struct()
{
    memset(&g_pub_tx,0x0,sizeof(struct pub_tx));
    memset(&g_td_mst,0x0,sizeof(struct td_mst_c));
    memset(&g_td_parm,0x0,sizeof(struct td_parm_c));
    memset(&g_dd_mst,0x0,sizeof(struct dd_mst_c));
    memset(&g_dd_parm,0x0,sizeof(struct dd_parm_c));

    return 0;
}
int getpm_fun()
{
    char tmp[20];
    memset(tmp,0x0,sizeof(tmp));
    get_zd_data("0310",g_pub_tx.ac_no);
    get_zd_int("0340",&g_pub_tx.ac_seqn);
    get_zd_long("0440",&g_pub_tx.tx_date);
    return 0;
}
