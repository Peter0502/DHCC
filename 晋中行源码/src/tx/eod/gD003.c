/***************************************************
* 文 件 名:  spD003.c
* 功能描述： 口头挂失自动解挂
*
* 作    者:  jack
* 完成日期： 2003年3月9日
*
* 修改记录：
* 日   期:  2003.03.14
* 修 改 人: rob
* 修改内容:
*************************************************/
#define MYSQLERR if( ret ) {\
	strcpy( g_pub_tx.reply,"AT03" );\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#define MYERR if( ret ) {\
	sprintf(acErrMsg,"SQLERR [%d]",ret);\
	WRITEMSG\
	goto ErrExit;\
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "com_parm_c.h"
#include "mo_loss_c.h"
#include "com_eod_exec_c.h"
#include "com_sys_parm_c.h"

struct com_sys_parm_c com_sys_parm;
extern struct com_eod_exec_c grz_com_eod_exec;/*全局,用于向子程序传递控制序列*/

gD003()
{
	int ret = 0;

	int dates = 0;		/*--口挂天数--*/
	int jglx = 0;
	long gsdate = 0;
	long gsdate1 = 0;

	struct mo_loss_c sMo_loss;
	struct com_parm_c sCom_parm;
	
	memset(&g_pub_tx,0x00,sizeof(g_pub_tx));

	ret=sql_begin(); /*打开事务*/
	MYSQLERR

	/** 初始化全局变量 **/
	pub_base_sysinit();

	ret=pub_base_GetSysparm(&com_sys_parm);
	MYERR

	if( grz_com_eod_exec.do_type[0]=='0' ) /*翻牌前*/
		g_pub_tx.tx_date=com_sys_parm.sys_date;
	else
		g_pub_tx.tx_date=com_sys_parm.lst_date;

	/*-------- 查询限制天数 --------*/
	ret=Com_parm_Sel(g_pub_tx.reply,&sCom_parm,\
			"parm_code='KGTS' and parm_seqn = 1");
	MYSQLERR
	dates = atoi(sCom_parm.val);

	/*--------- 计算挂失记录到期的挂失日期 ---------*/
	gsdate = pub_base_daynumSUB( g_pub_tx.tx_date, dates );
	
	sprintf( acErrMsg, "--------->处理 [%ld] 挂失的口挂", gsdate );
	WRITEMSG

	/**因为老数据原因 所以只处理当天**/
	ret=Mo_loss_Dec_Sel(g_pub_tx.reply,
		"loss_ind='1' and loss_date=%ld and unloss_ind='0' ",gsdate );
	MYSQLERR
 	
	while(1)
	{
		memset(&sMo_loss,0x00,sizeof(struct mo_loss_c));

		ret=Mo_loss_Fet_Sel( &sMo_loss, g_pub_tx.reply );
		if( ret == 100 ) break;
		MYSQLERR

		memset( g_pub_tx.ac_no, 0x0 ,sizeof(g_pub_tx.ac_no) );
		strcpy( g_pub_tx.ac_no, sMo_loss.ac_no );
		strcpy( g_pub_tx.opn_br_no, sMo_loss.opn_br_no );
		strcpy( g_pub_tx.tx_br_no, sMo_loss.opn_br_no );

		sprintf(acErrMsg,"自动解除口挂[%s][%ld]",
				sMo_loss.ac_no,sMo_loss.loss_date );
		WRITEMSG;

		/*-------------------- 开始解挂 ----------------*/
		jglx = 21;
		ret=pub_acct_loss( jglx, 1 );
		if(ret)
        {
           	sprintf(acErrMsg,"挂失解挂主控[%s]出错!", g_pub_tx.reply );
            WRITEMSG
            goto ErrExit;
        }

		pub_base_prom_inf( g_pub_tx.opn_br_no,"1",
			"%s已经超过口挂期限，被自动解除口挂",
			g_pub_tx.ac_no );
	}


OkExit:
	sql_commit();	/*--提交事务--*/
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"自动解挂成功![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 0;
ErrExit:
	sql_rollback(); /*--事务回滚--*/
        sprintf(acErrMsg,"自动解挂失败![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data("0120",g_pub_tx.reply);
        return 1;
}
