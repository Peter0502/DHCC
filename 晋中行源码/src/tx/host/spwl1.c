/*************************************************
* 文 件 名: sp9602.c
* 功能描述：修改操作员
*
* 作    者: rob
* 完成日期：20030309
*
* 修改记录：修改授权柜员时,向auth_tel_auth插记录 
* 日    期: 20061201
* 修 改 人: wanglei
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_tel_c.h"
#include "com_branch_c.h"
#include "auth_tx_code_c.h"
#include "cash_mst_c.h"
#include "com_cur_no_code_c.h"
#include "auth_tel_auth_c.h"

int sp9602( )
{
    int ret;
    char flag[2];
    char cbr_no[10];
    char ctel[10];
	char tx_type[25];
    struct com_tel_c com_tel_c,com_tel;
    struct com_branch_c com_branch_c;
    struct auth_tx_code_c auth_tx_code_c;
    struct cash_mst_c sCash_mst;
    struct com_cur_no_code_c com_cur_no_code_c;
    struct auth_tel_auth_c auth_tel_auth_c;

    memset(&com_tel_c,0x0,sizeof(struct com_tel_c));
    memset(&com_tel,0x0,sizeof(struct com_tel_c));
    memset(&com_branch_c,0x0,sizeof(struct com_branch_c));
    memset(&auth_tx_code_c,0x0,sizeof(struct auth_tx_code_c));
    memset(&sCash_mst,0x0,sizeof(struct cash_mst_c));
    memset(&com_cur_no_code_c,0x0,sizeof(struct com_cur_no_code_c));
    memset(&auth_tel_auth_c,0x0,sizeof(struct auth_tel_auth_c));

    if (pub_base_sysinit())
    {
        sprintf(acErrMsg,"初始化公用结构错误!!");
        WRITEMSG
        goto ErrExit;
    }
    
    /******
    get_zd_data("0920",ctel);
    get_zd_data("0910",cbr_no);
    *******/
    get_zd_data("0920",com_tel_c.tel);
    get_zd_data("0910",com_tel_c.br_no);
    get_zd_data("0250",com_tel_c.name);
    get_zd_data("0790",com_tel_c.pwd);
    get_zd_data("0860",com_tel_c.lvl);
    get_zd_data("0640",com_tel_c.beg_menu);
    get_zd_data("0300",tx_type);
	get_zd_data("0680", com_tel_c.chk_ind);
	get_zd_data("0690", com_tel_c.csts);

	strncpy(com_tel_c.tx_type, tx_type, 10);
    pub_base_strpack(com_tel_c.br_no);
	com_tel_c.tx_type[10] = 0;
    pub_base_strpack(com_tel_c.tel);

	sprintf(acErrMsg, "******** chk_ind=[%s], tx_type=[%s]", 
		com_tel_c.chk_ind, tx_type);
	WRITEMSG

    /*******
    if (strlen(ctel) != 4)
    {
        sprintf(acErrMsg,"柜员号码的长度应为4位!! [%s] ",ctel);
        WRITEMSG
        goto ErrExit;
    }

    if (strlen(cbr_no) != 5)
    {
        sprintf(acErrMsg,"机构号的长度应为5位!! [%s] ",cbr_no);
        WRITEMSG
        goto ErrExit;
    }

    strcpy(com_tel_c.tel,ctel);
    strcpy(com_tel_c.br_no,cbr_no);
    ********/

    /* 检查柜员号是否唯一 */
    ret = Com_tel_Sel(g_pub_tx.reply,&com_tel,"tel='%s'",com_tel_c.tel);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询柜员信息表错误!!");
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"柜员号不存在!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O120");
        goto ErrExit;
    }
	
    /* 检查机构号码是否存在 */
    strcpy(com_branch_c.br_no,com_tel_c.br_no);

    ret = Com_branch_Sel(g_pub_tx.reply,&com_branch_c,"br_no='%s'",
          com_branch_c.br_no);
    if (ret != 0 && ret != 100)
    {
        sprintf(acErrMsg,"查询公共机构表错误!!!!");
        WRITEMSG
        goto ErrExit;
    }
    else if (ret == 100)
    {
        sprintf(acErrMsg,"机构编码表中没有该机构号!! [%s]",com_branch_c.br_no);
        WRITEMSG
        strcpy(g_pub_tx.reply,"O116");
        goto ErrExit;
    }

	/* 如果机构变化的话需要检查钱箱和凭证 */
	if (strcmp(com_tel.br_no , com_tel_c.br_no) != 0)
	{
		ret = sql_count("note_mst","tel='%s' and sts = '0'", com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "统计柜员凭证表异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret > 0)
		{
			sprintf(acErrMsg, "该柜员还有凭证!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O258");
			goto ErrExit;
		}

		ret = sql_count("note_mst","tel='%s' and sts = '2'", com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "统计柜员凭证表异常 [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret > 0 )
		{
			sprintf(acErrMsg, "该柜员还有调出待收的凭证!! [%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O259");
			goto ErrExit;
		}

		ret = sql_count("cash_mst", "tel='%s' and sts='0' and bal != 0.00",
			com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "统计柜员钱箱异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret > 0 )
		{
			sprintf(acErrMsg, "该柜员的钱箱中还有钱不能调动!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "O260");
			goto ErrExit;
		}

		/* 如果没有钱箱则增加钱箱 */
		ret = sql_count("cash_mst", "tel='%s'", com_tel.tel);
		if ( ret < 0 )
		{
			sprintf(acErrMsg, "统计柜员钱箱异常!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		else if ( ret == 0 )
		{
			/* 增加钱箱 */ 
			/* 生成一个内部账号 * 
			ret=pub_base_CrtInAc("10101",g_pub_tx.ac_no); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"生成该柜员的现金账号错误!!"); 
				WRITEMSG 
				strcpy(g_pub_tx.reply,"O119"); 
				goto ErrExit; 
			} 
			**/
			strcpy(g_pub_tx.ac_no, "910110");
			strcat(g_pub_tx.ac_no, com_tel_c.tel);
			g_pub_tx.ac_no[10]=0;
			
			strcpy(sCash_mst.cur_no,"01"); 
			strcpy(sCash_mst.ac_no,g_pub_tx.ac_no); 
			strcpy(sCash_mst.br_no, com_tel_c.br_no);
			strcpy(sCash_mst.tel, com_tel_c.tel);
			sCash_mst.bal = 0.00;
			sCash_mst.ys_bal = 0.00;
			strcpy(sCash_mst.sts, "0");

			/* 登记现金台账 */ 
			ret = Cash_mst_Ins(sCash_mst,g_pub_tx.reply); 
			if (ret != 0) 
			{ 
				sprintf(acErrMsg,"登记现金台账错误 !!"); 
				WRITEMSG 
				goto ErrExit; 
			}
		}
	}

    /* 判断柜员级别是否正确 */
    if ( atoi(com_tel_c.lvl) != 1 && atoi(com_tel_c.lvl) != 2
      && atoi(com_tel_c.lvl) != 3 && atoi(com_tel_c.lvl) != 4)
    {
        sprintf(acErrMsg,"柜员级别错误!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"O118");
        goto ErrExit;
    }

    /* 给密码加密 */
    /*sprintf(acErrMsg,"pwd =[%s]",com_tel_c.pwd);*/
    WRITEMSG

    if (strcmp(com_tel_c.pwd,"") == 0 || com_tel_c.pwd[0] == '\0')
    {
        sprintf(acErrMsg,"没有修改密码!!");
        WRITEMSG
        strcpy(com_tel_c.pwd,com_tel.pwd);
        com_tel_c.pwd_date = com_tel.pwd_date;
    }
    else 
    {
        pub_base_strpack(com_tel_c.pwd);
        pub_base_EnDes(g_pub_tx.tx_date,"",com_tel_c.pwd);
        com_tel_c.pwd_date = g_pub_tx.tx_date;
    }

    /*sprintf(acErrMsg,"pwd =[%s]",com_tel_c.pwd);*/
    WRITEMSG

    /* 修改柜员信息表 
    strcpy(com_tel_c.csts, com_tel.csts);*/
  
    ret = Com_tel_Dec_Upd(g_pub_tx.reply,"tel='%s'",com_tel_c.tel);
    if (ret != 0)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }

    ret = Com_tel_Fet_Upd(&com_tel,g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
        WRITEMSG
        goto ErrExit;
    }
vtcp_log("11111111111111111111111");
    /* 检查是否修改了机构号 */
    if (strcmp(com_tel.br_no,com_tel_c.br_no) != 0)
    {
        /* 更新 cash_mst */
		ret = sql_execute("update cash_mst set br_no='%s' where tel='%s'",
			com_tel_c.br_no, com_tel.tel);
		if ( ret )
		{
			sprintf(acErrMsg, "修改该柜员的钱箱错误!! [%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
    }
    
vtcp_log("2222222222222222222222222");
    sprintf(acErrMsg,"bef upd !!! ");
    WRITEMSG

	strcpy(com_tel_c.rowid,com_tel.rowid);

    ret = Com_tel_Upd_Upd(com_tel_c,g_pub_tx.reply);
vtcp_log("3333333333333333333333333");
    if (ret != 0)
    {
        sprintf(acErrMsg,"修改柜员信息表错误!!!");
        WRITEMSG
        goto ErrExit;
    }
    Com_tel_Clo_Upd();


      
	/***向auth_tel_auth表中插记录  add by wanglei 20061201***/
	if(com_tel_c.tx_type[1] == '1')/***支行主办柜员***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[2] == '1')/***清算中心普通柜员***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[3] == '1')/***清算中心主办***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZJ1");
		strcpy(auth_tel_auth_c.au_xj,"ZJ1");
		strcpy(auth_tel_auth_c.au_zz,"ZJ2");
		strcpy(auth_tel_auth_c.au_tc,"ZJ3");
		strcpy(auth_tel_auth_c.au_de,"ZJ3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[4] == '1')/***管理人员***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"ZG1");
		strcpy(auth_tel_auth_c.au_xj,"ZG1");
		strcpy(auth_tel_auth_c.au_zz,"ZG2");
		strcpy(auth_tel_auth_c.au_tc,"ZG3");
		strcpy(auth_tel_auth_c.au_de,"ZG3");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[5] == '1')/***机房系统操作员***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"HZ1");
		strcpy(auth_tel_auth_c.au_xj,"HZ1");
		strcpy(auth_tel_auth_c.au_zz,"HZ1");
		strcpy(auth_tel_auth_c.au_tc,"HZ1");
		strcpy(auth_tel_auth_c.au_de,"HZ1");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	else if(com_tel_c.tx_type[6] == '1')/***机房结帐柜员***/
	{
		strcpy(auth_tel_auth_c.tel,com_tel_c.tel);
		strcpy(auth_tel_auth_c.au_al,"GZ1");
		strcpy(auth_tel_auth_c.au_xj,"GZ1");
		strcpy(auth_tel_auth_c.au_zz,"GZ1");
		strcpy(auth_tel_auth_c.au_tc,"GZ1");
		strcpy(auth_tel_auth_c.au_de,"GZ1");
		strcpy(auth_tel_auth_c.ind,"0000000000");
	}
	ret = Auth_tel_auth_Ins(auth_tel_auth_c,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"Insert Auth_tel_auth error!!!");
		WRITEMSG
		goto ErrExit;
	}

vtcp_log("4444444444444444444444444");
    strcpy(g_pub_tx.ac_no,com_tel_c.tel);
    strcpy(g_pub_tx.brf,"修改柜员");
vtcp_log("55555555555555555555555");

    if (pub_ins_trace_log())
    {
        sprintf(acErrMsg,"登记交易流水错误!!");
        WRITEMSG
        goto ErrExit;
    }

       
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"操作员修改成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"操作员修改失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
