/*************************************************
* 文 件 名:  spB511.c
* 功能描述： 同业拆入还款明细查询
*
* 作    者:  martin
* 完成日期： 2006年9月24日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_ibl_ret_c.h"

int spB511()
{
	char	cFileName[50];			/* 临时文件名 */
	int	ret, num=0;
	FILE 	*fp;

	struct mo_ibl_ret_c sMo_ibl_ret;
	struct mdm_ac_rel_c sMdm_ac_rel;

	char wherelist[500];
	char tmpstr[50];

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( wherelist, 0x0, sizeof(wherelist));
	memset(&sMo_ibl_ret, 0x0, sizeof(struct mo_ibl_ret_c));
	memset(&sMdm_ac_rel, 0x0, sizeof(struct mdm_ac_rel_c));
	
	/* 数据初始化 */
	pub_base_sysinit();

	/* 根据柜员号、终端号生成临时文件 */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "生成下传全路经文件名(批量)错" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "临时文件错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	get_zd_data("0730", sMo_ibl_ret.pact_no);
	get_zd_double("0390", &sMo_ibl_ret.ret_amt);
	get_zd_long("0440", &sMo_ibl_ret.tx_date);

	strcpy(wherelist, " 1=1 ");
	if (strcmp(sMo_ibl_ret.pact_no, "") != 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr," and pact_no = '%s' ", sMo_ibl_ret.pact_no);
		strcat(wherelist, tmpstr);
	}

	if (pub_base_CompDblVal(sMo_ibl_ret.ret_amt, 0.00) == 1)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and ret_amt = %lf ", sMo_ibl_ret.ret_amt);
		strcat(wherelist, tmpstr);
	}

	if (sMo_ibl_ret.tx_date > 0)
	{
		memset(tmpstr, 0x0, sizeof(tmpstr));
		sprintf(tmpstr, " and tx_date = %ld ", sMo_ibl_ret.tx_date);
		strcat(wherelist, tmpstr);
	}
  strcat(wherelist, " order by pact_no , tx_br_no ");
	sprintf(acErrMsg, "-------> wherelist = [%s]", wherelist);
	WRITEMSG

	ret = Mo_ibl_ret_Dec_Upd( g_pub_tx.reply , wherelist);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mo_ibl_ret_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	fprintf( fp, "~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", 
			"合同号", "机构编码", "归还本金", "归还利息",
			"交易日期", "主机流水号", "标志", "操作员", "复合员", 
			"授权人");
	/*
	"~%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\n", \
			"合同号", "拆借账号", "对方账号", "产品编号", "拆借类型", \
			"拆借户名", "拆借期限", "拆借金额", "拆借利率", "逾期利率", \
			"尚欠金额", "开户日期","到期日期", "交易日期", "拆借状态");
	*/
	while(1)
	{
		memset( &sMo_ibl_ret, 0x0, sizeof( struct mo_ibl_ret_c ) );
		
		ret = Mo_ibl_ret_Fet_Upd( &sMo_ibl_ret, g_pub_tx.reply );
		if ( ret == 100 && num == 0)
		{
			sprintf( acErrMsg, "同业拆入还款登记簿中无记录!" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B254" );
			goto ErrExit;
		}
		else if ( ret == 100 && num > 0 )
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "查询mo_ibl_ret错误!! [%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
/***12:55 here 检查mdm_ac_rel  
		if (sMo_ibl_ret.sts[0] != '1')
		{
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_id=%ld and \
			ac_seqn=0", sMo_ibl.ac_id);
		if( ret == 100 ) 
		{
        	sprintf(acErrMsg,"取介质与账户关系表mdm_ac_rel错!无此账户信息! \
					ac_id=[%ld]", sMo_ibl.ac_id);
       		WRITEMSG
       		strcpy( g_pub_tx.reply, "M003" );
       		goto ErrExit;
    	}
		else if ( ret )
    	{
       		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错误!ret=[%d]", ret );
       		WRITEMSG
       		goto ErrExit;
    	}
		}
***/		
			switch(sMo_ibl_ret.sts[0])
			{
				case '1':
			fprintf(fp,"%s|%s|%.2lf|%.2lf|%ld|%ld|%s|%s|%s|%s\n",
					sMo_ibl_ret.pact_no, sMo_ibl_ret.tx_br_no,
					sMo_ibl_ret.ret_amt,sMo_ibl_ret.ret_rate, sMo_ibl_ret.tx_date,
					sMo_ibl_ret.trace_no, "录入", sMo_ibl_ret.tel, 
					sMo_ibl_ret.chk,sMo_ibl_ret.auth);
					break;
				case '2':
			fprintf(fp,"%s|%s|%.2lf|%.2lf|%ld|%ld|%s|%s|%s|%s\n",
					sMo_ibl_ret.pact_no, sMo_ibl_ret.tx_br_no,
					sMo_ibl_ret.ret_amt,sMo_ibl_ret.ret_rate, sMo_ibl_ret.tx_date,
					sMo_ibl_ret.trace_no, "已记帐", sMo_ibl_ret.tel, 
					sMo_ibl_ret.chk,sMo_ibl_ret.auth);
					break;
				default:
					sprintf(acErrMsg, "同业拆入还款登记簿中状态错误!! [%s]",
						sMo_ibl_ret.sts);
					WRITEMSG
					break;
			}
		num++;	
	}

	Mo_ibl_ret_Clo_Upd( );
	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

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
