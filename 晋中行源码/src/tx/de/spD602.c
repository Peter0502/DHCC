/***************************************************************
* 文 件 名: spD602.c
* 功能描述：支取信息维护
* 作    者: mike
* 完成日期：2003年3月9日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card.h"
#include "mo_infrm_c.h"
#include "prdt_ac_id_c.h"
#include "com_parm_c.h"
#include "cif_id_code_rel_c.h"

spD602()
{   
	int ret; 
	char ac_no[20],ac_type[2],intst_type[2];
	long tmp_date1,tmp_date2;
	double tmp_amt;
	long date=0;
	char id_type[2];
	char id_no[21];
        char option[2];   /* 20110725 add by yanqq  动作标志追加 */
	char fname[100]={'0'};
	struct mo_infrm_c	g_mo_infrm;
	struct prdt_ac_id_c	g_prdt_ac_id;
	struct com_parm_c sCom_parm;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_mo_infrm,0x00,sizeof(struct mo_infrm_c));
	memset(&sCom_parm, 0x0, sizeof(struct com_parm_c));
	memset(id_type, 0x0, sizeof(id_type));
	memset(id_no, 0x0, sizeof(id_no));
	memset(option,0x0,sizeof(option));    /* 20110725 add by yanqq  动作标志追加 */

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	
	set_zd_long("0460",g_pub_tx.tx_date);/**add by spj 20140721**/
	
	get_zd_data("0300",g_pub_tx.ac_no);          	/* 账号 */
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);      	/* 密码标志 */
	get_zd_data("0680", g_pub_tx.id_type); 			/* 证件类型 */
	get_zd_data("0620", g_pub_tx.id_no);   			/* 证件号码 */
	get_zd_data("0690",option);                     /* 20110725 add by yanqq 动作标志 */

	if( g_pub_tx.ac_no[0]=='5' )
	{
		sprintf(acErrMsg,"请输入储蓄账户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O164");
		goto ErrExit;
	}

	/*根据帐户查找*/
	ret = Mdm_ac_rel_Dec_Upd(g_pub_tx.reply, "ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"游标定义错误!");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mdm_ac_rel_Fet_Upd(&g_mdm_ac_rel,g_pub_tx.reply);
	if( ret==100 )
	{
		sprintf(acErrMsg,"账号不存在!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.draw_pwd_yn[0]=='N' )
	{
		/* add by xxx at 20030726 **/
		ret = Com_parm_Sel( g_pub_tx.reply, &sCom_parm, "parm_code='ZJJY' and \
							parm_seqn=0");
		if( ret )
		{
			sprintf(acErrMsg, "查询公共参数表错误!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		date = atol(sCom_parm.val);
		sprintf(acErrMsg, "beg_date=[%ld], date=[%ld]", g_mdm_ac_rel.beg_date,
			date);
		WRITEMSG
		if (g_mdm_ac_rel.beg_date >= date)
		{
			ret = pub_cif_AcToIdNoType_jy(g_mdm_ac_rel.ac_id, 
				g_mdm_ac_rel.ac_seqn, id_type, id_no);
			if (ret != 0)
			{
				sprintf(acErrMsg, "查询客户中心中的证件类型和证件号码错误");
				WRITEMSG
				goto ErrExit;
			}

			sprintf(acErrMsg, "-------> id_type=[%s] [%s]", 
				g_pub_tx.id_type, id_type);
			WRITEMSG
			if (strncmp(id_type, g_pub_tx.id_type, 1) != 0)
			{
				sprintf(acErrMsg, "证件类型不符!! [%s]", g_pub_tx.id_type);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P444");
				goto ErrExit;
			}

			strcpy(g_mdm_ac_rel.id_type, g_pub_tx.id_type);

			sprintf(acErrMsg, "-------> id_no=[%s] [%s]", 
				g_pub_tx.id_no, id_no);
			WRITEMSG
			if (strcmp(id_no, g_pub_tx.id_no) != 0)
			{
				sprintf(acErrMsg, "证件号码不正确!! [%s]", g_pub_tx.id_no);
				WRITEMSG
				strcpy(g_pub_tx.reply, "P445");
				goto ErrExit;
			}
			strcpy(g_mdm_ac_rel.id_no, g_pub_tx.id_no);
		}

		get_zd_data("1027",g_pub_tx.draw_pwd); 			/* 支取密码 */
		pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
		strcpy(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
	}
	else
	{
		vtcp_log("[%s][%d]g_pub_tx.ac_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.ac_no);
		vtcp_log("[%s][%d]动作标志option=[%s]\n",__FILE__,__LINE__,option);
		/***add by hxc 091021 晋中卡密码维护核心不进行原密码校验***/
		/*20110725 mod by yanqq 重置时不进行原密码校验*/
		/*if (memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) != 0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) != 0) */
		if (memcmp(option,"1",1))
		{
			if(memcmp(g_pub_tx.ac_no,CARDHEADCODE,6) != 0 && memcmp(g_pub_tx.ac_no,ICCARDHEADCODE,6) != 0)
			{
				/* add by xxx at 20030726 密码校验**/
				get_zd_data("0790",g_pub_tx.draw_pwd); 			/* 原支取密码 */
				pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
				pub_base_strpack(g_mdm_ac_rel.draw_pwd);
				pub_base_strpack(g_pub_tx.draw_pwd);
				if ( strcmp(g_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) != 0)
				{
					sprintf(acErrMsg, "密码错误!!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "O123");
					goto ErrExit;
				}
			}
		}
		/***end by hxc 091021 卡密码维护不进行原密码校验***/
		
		get_zd_data("1027",g_pub_tx.draw_pwd); 			/* 新支取密码 */
		pub_base_EnDes(g_pub_tx.tx_date,"",g_pub_tx.draw_pwd);
		/*vtcp_log("[%s][%d]g_pub_tx.draw_pwd=[%s]\n",__FILE__,__LINE__,g_pub_tx.draw_pwd);*/
		strcpy(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd);
	}

	strcpy(g_mdm_ac_rel.draw_pwd_yn,g_pub_tx.draw_pwd_yn);
	ret = Mdm_ac_rel_Upd_Upd(g_mdm_ac_rel,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"修改错误!");
		WRITEMSG
		goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf(acErrMsg,"登记交易流水错误!");
	  	WRITEMSG
	    goto ErrExit;
	}
        /*mod by lwb 20140122 */	
	/*pub_mob_sendmail("柜面密码修改成功", g_pub_tx.tx_code,g_pub_tx.ac_no,g_pub_tx.add_ind,0.0, 0.0 );*/
	pub_mob_sendmail("柜面密码修改成功", g_pub_tx.tx_code,g_pub_tx.ac_no,"1",0.0, 0.0 );
	
	/****add 20120104密码修改后把错误文件删除*/
	sprintf( fname,"rm -f /backup/pass_error/%s*",g_pub_tx.ac_no);
        ret = system( fname );
        if( ret )
        {
        sprintf(acErrMsg,"rm file error [%s] [%d]",fname,ret);
        WRITEMSG
        goto ErrExit;
        }

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"支取信息维护成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"支取信息维护失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
/**********************************************************************
 * 函 数 名：pub_cif_AcToIdNoType_jy();(修改版)
 * 函数功能：根据证件类型和证件号码取得客户号及客户类型
 *			 
 * 作者/时间：rob/20051010
 * 
 * 参数：
 *     输入：	
 *		ac_id		账户id
 *		ac_seqn	    账户序号
 *     输出: 	
 *		id_type		证件类型		
 *		id_no		证件号码
 *
 *          	响应码
 *   返回值：
 *           返回0成功，
 *           返回非0失败；
 * 修改历史：
 *          
********************************************************************/
int pub_cif_AcToIdNoType_jy(long ac_id, int ac_seqn, char *id_type, char *id_no)
{
	int ret_jy = 0;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct cif_id_code_rel_c sCif_id_code_rel;

	memset(&sPrdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c));
	memset(&sCif_id_code_rel, 0x0, sizeof(struct cif_id_code_rel_c));

	sprintf(acErrMsg, " CGH -------> ac_id=[%ld], ac_seqn=[%d]", 
		ac_id, ac_seqn);
	WRITEMSG
	if( ac_seqn!=9999 )
	{
		ret_jy = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld and \
				ac_seqn=%d", ac_id, ac_seqn);
	}else{
		ret_jy = Prdt_ac_id_Sel(g_pub_tx.reply, &sPrdt_ac_id, "ac_id=%ld", ac_id);
	}	
	
	if (ret_jy != 0 && ret_jy != 100)
	{
		sprintf(acErrMsg, "查询产品账号对照表错误!! [%d]", ret_jy);
		WRITEMSG
		strcpy(g_pub_tx.reply, "W013");
		return ret_jy;
	}
	else if (ret_jy == 100)
	{
		sprintf(acErrMsg, "产品账号对照表中不存在该记录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "W012");
		return ret_jy;
	}

	ret_jy = Cif_id_code_rel_Sel(g_pub_tx.reply, &sCif_id_code_rel, 
		"cif_no=%ld", sPrdt_ac_id.cif_no);
	if (ret_jy != 0 && ret_jy != 100)
	{
		sprintf(acErrMsg, "客户证件与客户号对照表错误!! [%d]", ret_jy);
		WRITEMSG
		strcpy(g_pub_tx.reply, "C002");
		return ret_jy;
	}
	else if (ret_jy == 100)
	{
		sprintf(acErrMsg, "客户证件与客户号对照表没有此记录!!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "B115");
		return ret_jy;
	}

	pub_base_strpack(sCif_id_code_rel.id_type);
	pub_base_strpack(sCif_id_code_rel.id_no);
	strcpy(id_type, sCif_id_code_rel.id_type);
	strcpy(id_no, sCif_id_code_rel.id_no);

	return 0;
}
