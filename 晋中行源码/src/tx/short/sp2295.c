/*************************************************************
* 文 件 名:  sp2295.c                                        *
* 功能描述： 输入机构代码和日期回显金额(ctl_accountreg)      *
*                                                            *
* 作    者:    Amelia                                        *
* 完成日期：   2006-10-15                                    *
*************************************************************/
#include <string.h>
#define  EXTERN
#include "public.h"
#include "ctl_accountreg_c.h"

int sp2295()
{
	struct ctl_accountreg_c sCtl_accountreg_c;
	char   flag[2];
	int    ret;

	memset(&sCtl_accountreg_c,0x0,sizeof(struct ctl_accountreg_c));

	pub_base_sysinit();/*好像没有用*/

	get_zd_data("0300",sCtl_accountreg_c.seqno);
	get_zd_data("0700",flag);
		
	sprintf(acErrMsg,"sCtl_accountreg_c.seqno=[%s]!! ",sCtl_accountreg_c.seqno);
        WRITEMSG
        
        if (flag[0] == '0')
    {
    	ret = Ctl_accountreg_Sel(g_pub_tx.reply,&sCtl_accountreg_c,"seqno='%s'", sCtl_accountreg_c.seqno);
    	 
    	if (ret != 0 && ret != 100)
    	{
        	sprintf(acErrMsg,"可能填写错误!! ret=[%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"G006");
        	goto ErrExit;
    	}
		else if (ret == 0)
		{
			sprintf(acErrMsg,"该信息已经存在!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"T048");
			goto ErrExit;
		}
        goto OkExit;  
    }
    
    ret = Ctl_accountreg_Sel(g_pub_tx.reply,&sCtl_accountreg_c,"seqno='%s'",sCtl_accountreg_c.seqno);
    if (ret != 0)
    {
        sprintf(acErrMsg,"可能填写错误!! ret=[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"G006");
        goto ErrExit;
    }
     
    set_zd_double( "0400", sCtl_accountreg_c.amt);  /*金额*/
       
/***	g_reply_int = Ctl_accountreg_Sel( g_pub_tx.reply, &ctl_accountreg, "seqno='%s'", ctl_accountreg.seqno );
	  
	 if( g_reply_int == 100 )
            {
              sprintf( acErrMsg, "不存在此信息!" );
              WRITEMSG
              goto ErrExit;
            }
         else if( g_reply_int  )
                {
                 sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", g_reply_int );
                 WRITEMSG
                 goto ErrExit;
                }
         set_zd_double( "1001", ctl_accountreg.amt );
***/

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
