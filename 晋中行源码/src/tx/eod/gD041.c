/*************************************************************
* 文 件 名: gD041.c
* 功能描述：揽存人业务量统计
*
* 作    者: jack
* 完成日期: 2003年7月19日
*
* 修改记录：
* 日    期:
* 修 改 人: 
* 修改内容: 提高效率
**************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"
#include "ass_cp_stat_c.h"
#include "tel_save_inf_c.h"
#include "mo_opn_cls_c.h"
#include "ac_bal_view_c.h"

gD041()
{
	int ret = 0;
    int flag ; /* 1 插入　2 更新 */
	char ac_type[2];
	char tel_v[5];
	char prdt_no[4];

    struct prdt_ac_id_c prdt_ac_id_c;
    struct ass_cp_stat_c ass_cp_stat_c, sAss_cp_stat;
	struct tel_save_inf_c sTel_save_inf;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mo_opn_cls_c sMo_opn_cls;
	struct ac_bal_view_c sAc_bal_view;

    memset(&prdt_ac_id_c, 0x0, sizeof(struct prdt_ac_id_c));
    memset(&ass_cp_stat_c, 0x0, sizeof(struct ass_cp_stat_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	memset(ac_type, 0x0, sizeof(ac_type));

	ret=sql_begin(); /*打开事务*/
	if(ret != 0) 
	{
		sprintf( acErrMsg, "打开事务失败!!!" );
		WRITEMSG
		goto ErrExit;
	}

    /**------- 初始化全局变量 --------**/
	pub_base_sysinit();

	/**-------- 建立临时视图 ---------**/
	ret = sql_execute2("create view ac_bal_view(gsp_no,prdt_no,ind,num,bal) \
	        as select x.tel,x1.prdt_no,'0',count(*),sum(x1.bal) \
	        from tel_save_inf x, mdm_ac_rel x0, dd_mst x1 \
	        where x.ac_no=x0.ac_no and x.ac_seqn=x0.ac_seqn and x0.ac_id=x1.ac_id and x0.ac_seqn=x1.ac_seqn \
	        group by x.tel,x1.prdt_no \
	        union select x2.tel,x4.prdt_no,'0',count(*),sum(x4.bal) \
	        from tel_save_inf x2,mdm_ac_rel x3,td_mst x4 \
	        where x2.ac_no=x3.ac_no and x2.ac_seqn=x3.ac_seqn \ 
           and x3.ac_id=x4.ac_id and x3.ac_seqn=x4.ac_seqn \
           group by x2.tel,x4.prdt_no \
   union select x5.tel,x7.prdt_no,'0',count(*),sum(x7.bal) \ 
     from tel_save_inf x5, mdm_ac_rel x6, od_mst x7 \
     where x5.ac_no=x6.ac_no and x5.ac_seqn=x7.ac_seqn \
           and x6.ac_id=x7.ac_id and x6.ac_seqn=x7.ac_seqn \
           group by x5.tel,x7.prdt_no \
   union select x8.tel,x9.prdt_no,x9.wrk_sts,count(*),sum(amt) \ 
     from tel_save_inf x8,mo_opn_cls x9 \
     where x8.ac_no=x9.ac_no and x8.ac_seqn=x9.ac_seqn \
           and wrk_sts in ('1','2') and tx_date=%d \
           group by x8.tel,x9.prdt_no,x9.wrk_sts",g_pub_tx.tx_date);
	if( ret )
	{
		sprintf(acErrMsg,"建立临时余额视图错误![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	strcpy ( tel_v , "0000" );
	strcpy ( prdt_no , "000" );

	vtcp_log("111111111111111111111");
	ret=Ac_bal_view_Dec();
	if( ret )
    {
        sprintf(acErrMsg,"声明账户余额视图游标出错![%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
	vtcp_log("22222222222222222222");
	memset( &sAc_bal_view, 0x00, sizeof( struct ac_bal_view_c ) );
	memset( &sAss_cp_stat, 0x00, sizeof( struct ass_cp_stat_c ) );
	while(1)
	{
		ret=Ac_bal_view_Fet( &sAc_bal_view );
		if (ret == 1403) break;
        else if( ret )
        {
            sprintf( acErrMsg, "取开销户登记簿游标出错![%d]", ret );
            WRITEMSG
            goto ErrExit;
        }
		sprintf(acErrMsg,"%s,%s,%s,%s",
			sAc_bal_view.gsp_no,sAc_bal_view.prdt_no,tel_v,prdt_no);
		WRITEMSG

		if ( strncmp(tel_v,sAc_bal_view.gsp_no,4)
			|| strncmp(prdt_no,sAc_bal_view.prdt_no,3) )
		{
			WRITEMSG
			if (strncmp(tel_v,"0000",4)&&strncmp(prdt_no,"000",3))
			{
				WRITEMSG
				g_reply_int = Ass_cp_stat_Ins( sAss_cp_stat, g_pub_tx.reply );
				if( g_reply_int )
				{
					sprintf(acErrMsg, "向揽存人业务量统计插入记录错误![%d]",\
					g_reply_int );
					WRITEMSG
					goto ErrExit;
				}
				memset( &sAss_cp_stat, 0x00, sizeof( struct ass_cp_stat_c ) );
			}
			strcpy ( tel_v , sAc_bal_view.gsp_no );
			strcpy ( prdt_no , sAc_bal_view.prdt_no );
			sAss_cp_stat.date = g_pub_tx.tx_date;    /*日期*/
			strcpy( sAss_cp_stat.gsp_no, sAc_bal_view.gsp_no );    /*揽存人*/
			strcpy( sAss_cp_stat.prdt_code, sAc_bal_view.prdt_no );/*产品号*/
		}
		WRITEMSG
		if ( sAc_bal_view.ind[0]=='0' )
		{
			sAss_cp_stat.ttl_amt=sAc_bal_view.bal;
			sAss_cp_stat.ttl_cnt=sAc_bal_view.num;
		}
		else if ( sAc_bal_view.ind[0]=='1' )
		{
			sAss_cp_stat.opn_ac_amt=sAc_bal_view.bal;
			sAss_cp_stat.opn_ac_cnt=sAc_bal_view.num;
		}
		else
		{
			sAss_cp_stat.cls_ac_amt=sAc_bal_view.bal;
			sAss_cp_stat.cls_ac_cnt=sAc_bal_view.num;
		}
		WRITEMSG
	}
	vtcp_log("33333333333333333333");
	Ac_bal_view_Clo();

	ret = sql_execute2( "drop view ac_bal_view" );
	if( ret )
	{
		sprintf(acErrMsg,"删除临时余额视图错误![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}


OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"揽存人业务量统计成功!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
    Trace_log_Clo_Sel();
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"揽存人业务量统计失败!!!!![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
