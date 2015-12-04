#include "find_debug.h"
/***************************************************************
* 文 件 名:     pubf_base_ac.c
* 功能描述：
*              1、pub_base_CrtAcChkb生成账号校验位；
*              2、pub_base_CrtChkbAr生成账号校验位算法;
*              3、pub_base_CrtCifAc生成客户显示账号；
*              4、pub_base_CrtInAc生成内部帐账号；
*              5、pub_base_CrtAcSeqn	生成账户序号；
*
*			   6.pub_base_CurGetddFile
*							(活期)币种转换成账户序号
*			   7.pub_base_CurToSeqn
*							根据币种、显示账号取活期主文件
*			   8  pub_base_disac_file
*							根据显示账号/序号取得主文件、产品参数
*			   9  pub_base_currentdis_file
*							根据活期显示账号和账号序号取活期主文件函数
*			  10  pub_base_termidis_file
*							根据定期显示账号和账号序号取定期主文件函数
*			  12  pub_base_currentid_file
*							根据活期账号ID取活期主文件函数
*			  13  pub_base_termiid_file
*							根据定期账号ID取定期主文件函数
*			  14  pub_base_idseqn_file
*   						根据账号ID和账户序号返回账户类型，并取主文件
*			  15  pub_base_actoseqn
*							根据帐号返回帐户序号
*			  16  pub_base_acgetname
*							根据账号得到客户号和户名
*			  17  pub_base_CrtDiscntAc	生成贴现账号；
*
*			  18  pub_base_ac_prdt()	根据帐号，帐号序号取产品代码
*
*			  19  pub_base_Exist_MainCrd()  判断是否存在主卡
*
*			  20  pub_base_opn_sub_ac()	开立子帐号
*
*			  21  pub_base_ins_sub_ac_hst() 登记子帐号明细台帐
*
*			  22  pub_base_sub_ac_trance()  子交易存取款主控
*
*			  23  pub_base_getdd_mst_parm( ) 活期账户取主文件和参数

                          24  pub_base_old_acno()
* 作    者:
* 完成日期：    2003年12月16日
*
* 修改记录：
* 1. 日    期: 	2003/12/30
*    修 改 人: 	rob	/ lance
*    修改内容: 	补充 5 - 11
*
**************************************************************/
#define ERR_DEAL if(ret){\
			sprintf( acErrMsg,"SQL ERROR[%d]",ret);\
			WRITEMSG\
			goto ErrExit;\
			}

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"				/*产品帐号对照表*/
#include "ac_seqn_id_c.h"				/*账户序号ID表*/
#include "cif_basic_inf_c.h"
#include "com_item_c.h"
#include "rsp_code_c.h"
#include "in_ac_seqn_c.h"
#include "mdm_attr_c.h"
#include "mo_coll_c.h"
#include "com_branch_c.h"
#include "mo_merge_ac_c.h"
#include "sub_dd_mst_c.h"
#include "sub_dd_mst_hst_c.h"
#include "dc_acc_rel_c.h"

struct prdt_ac_id_c g_prdt_ac_id;		/*产品帐号对照表结构*/

#define     ACNO_LEN   13              /* 客户帐号长度 */
#define     INACNO_LEN 13              /* 内部帐号长度 */
#define     CARDNO_LEN 19              /* 卡帐号长度   */
#define     CIFNO_LEN  8               /* 客户号长度   */
#define     CHKBT_LEN  1               /* 校验位长度   */

static int ret=0;
/**********************************************************************
* 函数名：    pub_base_CrtAcChkb
* 函数功能：    生成账号校验位
* 作者/时间：    2003年12月16日
*
* 参数：
*     输入： acno       ：无校验的账号
*           int mode   : 帐号介质(1 客户帐号 2 内部帐号 3 卡帐号 4 客户号)
*           chkb_len   : 校验位的长度
*
*     输出： chkb_acno  : 带校验位的帐号
*
* 返 回 值: 0 成功,1 失败
*
* 修改历史：
*
********************************************************************/

int pub_base_CrtAcChkb( char *acno, char *chkb_acno, int mode, int chkb_len )
{
    int Des_Code ;      /* 返回的校验位值 */

    char fmtstr[10];    /* 校验位输出格式,前面补0 */

    char Des_Str[4];    /* 校验位 */

	char cAcno[32];
	int iLen = 0;

    /* 调用生成校验位算法生成校验位 */
    switch ( mode )
    {
        case 1 :  /* 客户帐号 */
            Des_Code = pub_base_CrtChkbAr( acno , ACNO_LEN - CHKBT_LEN );
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        case 2:  /* 内部帐号 */
            Des_Code = pub_base_CrtChkbAr( acno , INACNO_LEN - CHKBT_LEN );
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        case 3:  /* 卡帐号 */
			/*** modify by dgf ***/
			MEMSET_DEBUG( cAcno, 0x00, sizeof(cAcno) );
			strcpy( cAcno,acno );
			pub_base_strpack ( cAcno );
			iLen = strlen( cAcno );
            Des_Code = pub_base_CrtChkbAr( acno , iLen);

            /*Des_Code = pub_base_CrtChkbAr( acno , CARDNO_LEN - CHKBT_LEN );*/
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        case 4 :  /* 客户号 */
            Des_Code = pub_base_CrtChkbAr( acno , CIFNO_LEN - CHKBT_LEN );
            if (Des_Code < 0 )
            {
                return (1);
            }
            break;
        default:  /* 错误返回 */
            return (1);
            break;
    }

    /* 将校验位加到帐号后面 */
    sprintf(acErrMsg,"产生的校验码为 [%d]",Des_Code);
    WRITEMSG

    sprintf( fmtstr , "%%0%dd" , chkb_len );
    sprintf(acErrMsg,"产生的fmtstr为 [%s]",fmtstr);
    WRITEMSG

    sprintf( Des_Str , fmtstr , Des_Code );
    sprintf(acErrMsg,"产生的Des_Str为 [%s]",Des_Str);
    WRITEMSG

    strncat( acno , Des_Str , chkb_len );
    sprintf(acErrMsg,"产生的acno为 [%s]",acno);
    WRITEMSG

    sprintf(acErrMsg,"00000客户码为 [%s]",chkb_acno);
    WRITEMSG
    sprintf( chkb_acno ,"%s", acno );
    sprintf(acErrMsg,"aaaaaaaaaaa");
    WRITEMSG
    sprintf(acErrMsg,"客户码为 [%s]",chkb_acno);
    WRITEMSG
    return 0;
}

/**********************************************************************
* 函 数 名：    pub_base_CrtChkbAr
* 函数功能：    生成校验位算法(客户帐号、内部帐号、卡号校验位都用这个算法)
* 作者/时间：    2003年12月16日
*
* 参数：
*     输入：char * acno     无校验位的账号
*          int len         不带校验位的帐号长度
*
*     输出：check_num:校验位
*
* 返 回 值: >=0 成功 ，<0 失败
*
* 修改历史：
*
********************************************************************/

int pub_base_CrtChkbAr( char * acno , int len )
{
    register     i, j;
    int          sum = 0;
    /*char      slstring[len+CHKBT_LEN+1];  *len+CHKBT_LEN带校验位的帐号长度*/
    char      slstring[256];
    int          check_num;

	sprintf(acErrMsg,"acno[%s],len[%d]",acno,len);
    WRITEMSG

    MEMSET_DEBUG( slstring, 0x00, strlen(slstring) );

	sprintf(acErrMsg,"acno[%s],len[%d]",acno,len);
    WRITEMSG

    MEMCPY_DEBUG(slstring, acno, len);
    slstring[len] = '\0';

    for (i = len - 1, j = 1; i >= 0 ; i --, j ++) {
       if ((j % 2) != 0)
         sum = sum + (((slstring[i] - 48) >= 5)?
                   ((slstring[i] - 48 - 5) * 2 + 1):
                          ((slstring[i] - 48) * 2));
       else
         sum = sum + (slstring[i] - 48);
    }
      check_num = (((sum % 10) == 0)? 0: (10 - (sum % 10)));

    sprintf(acErrMsg,"校验码为 [%d]",check_num);
    WRITEMSG

   return(check_num);
}

/**********************************************************************
* 函数名：    pub_base_CrtCifAc
* 函数功能：    生成客户帐账号（带校验位）
* 作者/时间：    2003年12月16日
*
* 参数：
*     输入：cif_no    :    客户号
*
*     输出：acno            :    根据客户号生成的带校验的账号
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/

int pub_base_CrtCifAc(int cif_no, char *acno)
{
    char    s_acno[21];       /* 不带校验位的账号 */
    char    s_tmp1[65];     /* 临时变量1 */
    char    s_tmp2[65];     /* 临时变量2 */
    int    ac_seqn;     /* 客户号的最大顺序号 */
    int    ret;
	struct cif_basic_inf_c	cif_basic_inf_c;
	MEMSET_DEBUG(&cif_basic_inf_c,0x00,sizeof(struct cif_basic_inf_c));

	sprintf(acErrMsg,"客户号![%d]",cif_no);
	WRITEMSG
    /* 根据客户号取最大顺序号 */
	g_reply_int=Cif_basic_inf_Dec_Upd(g_pub_tx.reply,"cif_no=%ld",cif_no);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"客户基本信息异常![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P135");
		return 1;
	}

	g_reply_int=Cif_basic_inf_Fet_Upd(&cif_basic_inf_c,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"客户基本信息不存在![%d][%ld]", \
				g_reply_int,cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P134");
		return 1;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"客户基本信息异常![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P135");
		return 1;
	}

    /* 顺序号+1 */
	cif_basic_inf_c.cif_seqn++;

    /* 连接字符串cif_no和顺序号 */
    sprintf(s_tmp1,"%04d", cif_basic_inf_c.cif_seqn);   /* 将顺序号补足为4位 */
    sprintf(s_tmp2,"%08d", cif_no);        		/* 将客户号补足为8位 */
    strcat(s_tmp2 , s_tmp1);
    strcpy(s_acno, s_tmp2);

    /* 生成校验位 */
    ret=pub_base_CrtAcChkb(s_acno , acno , 1, CHKBT_LEN);
    if (ret < 0 ){
        return (1);
    }

    /* 调用修改客户号顺序号表 */
	g_reply_int=Cif_basic_inf_Upd_Upd(cif_basic_inf_c,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"客户基本信息异常![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P135");
		return 1;
	}
	Cif_basic_inf_Clo_Upd();
    return 0;
}

/**********************************************************************
* 函数名：    pub_base_CrtInAc
* 函数功能：    生成内部帐账号（带校验位）
* 作者/时间：    jane/2003年12月16日
*
* 参数：
*     输入：acc_hrt    :    科目控制字
*
*     输出：acno            :    根据科目控制字生成的有校验的账号
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/

int pub_base_CrtInAc( char *acc_hrt, char *acno )
{
	struct com_item_c sComItem;
	struct  in_ac_seqn_c sInAcSeqn;

    char    s_acno[21];       /* 不带校验位的账号 */
    char    s_tmp[65];     /* 临时变量，将顺序号补足为4位 */
    char    app_code[5];     /* 应用代码 */
    int     ac_seqn;     /* 顺序号 */
    int     ret;

    MEMSET_DEBUG( &sComItem, 0x00, sizeof( struct com_item_c ) );
    MEMSET_DEBUG( &sInAcSeqn, 0x00, sizeof( struct in_ac_seqn_c ) );

    /* 根据科目控制字取应用代码 */
    ret = Com_item_Sel( g_pub_tx.reply , &sComItem , "acc_hrt = '%s'" , acc_hrt );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "无此科目,acchrt=[%s]", acc_hrt );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S033" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "查找公共科目表错，acchrt=[%s],[%d]", acc_hrt, ret );
    	WRITEMSG
    	return(-1);
    }

    /* 根据应用代码取最大顺序号 */
    pub_base_strpack( sComItem.app_cod );
    ret = In_ac_seqn_Sel( g_pub_tx.reply , &sInAcSeqn , "app_cod = '%s'", \
    				sComItem.app_cod );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "无此应用代码,app_cod=[%s]", sComItem.app_cod );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S044" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "查找内部帐号序号表错，app_cod=[%s],[%d]", sComItem.app_cod, ret );
    	WRITEMSG
    	return(-1);
    }

    /* 顺序号+1 */
    sInAcSeqn.seque++;

    /* 连接字符串app_code和顺序号 */
    sprintf(s_tmp,"%07d", sInAcSeqn.seque);
    s_acno[0]='9';               /* 第一位9表示内部帐号 */
	s_acno[1]='\0';
    strcat(s_acno, sComItem.app_cod);
    strcat(s_acno, s_tmp);

    /* 生成校验位 */
    ret=pub_base_CrtAcChkb( s_acno, acno, 2, CHKBT_LEN );
    if (ret < 0 ){
        return (1);
    }

    /* 调用修改内部帐号顺序号表 */
    ret = sql_execute("update in_ac_seqn set seque=%d where app_cod='%s'",sInAcSeqn.seque,sInAcSeqn.app_cod);
    if ( ret )
    {
    	sprintf( acErrMsg, "修改	in_ac_seqn错，ret = [%d]" , ret );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "E021" );
    	return(-1);
    }

    return 0;
}

/**********************************************************************
* 函数名：    pub_base_disac_file
* 函数功能：  根据显示账号/序号返回账户类型,并取得主文件、产品参数
* 作者/时间： rob/2003/12/31
*
* 参数：
*		输入: 显示帐号			ac_no			char[24];
*			  账户序号			ac_seqn			long;
*     	输出：账户类型			ac_type			char[2];
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：rob 2003/01/14
*
********************************************************************/
int pub_base_disac_file ( ac_no, ac_seqn, ac_type )
char *ac_no;
int ac_seqn;
char *ac_type;
{
	/*根据显示账号,序号取得账户id*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(g_mdm_ac_rel) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no ='%s'", ac_no);
	if ( ret == 100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"SELECT mdm_ac_rel error,error code=[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	/*根据账户id和账户序号取得帐号类型*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id,
			   "ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W012");
		return 1;
	}
	else if( ret )
	{
	    	sprintf(acErrMsg,"查询产品帐号对照表异常[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W013");
		return 1;
	}

	/*返回帐号类型*/
    MEMSET_DEBUG(ac_type, '0', sizeof(ac_type));
	strcpy(ac_type , g_prdt_ac_id.ac_id_type);

    /***** add by rob at 05/01/14*****/
    strcpy(g_pub_tx.ac_id_type,ac_type);
    strcpy(g_pub_tx.prdt_code,g_prdt_ac_id.prdt_no);

    sprintf(acErrMsg,"g_pub_tx.prdt_code=[%s]!!",g_pub_tx.prdt_code);
    WRITEMSG

    if (pub_base_get_prdtparm(g_pub_tx.prdt_code))
    {
        strcpy(acErrMsg,"取产品参数出错!!");
        WRITEMSG
        return 1;
    }
    /***** add end *****/

	switch ( atoi(ac_type) )
	{
		case 1:									/*1-活期*/
			MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );
			MEMSET_DEBUG( &g_dd_parm, 0x00, sizeof(struct dd_parm_c) );

			/*主文件*/
			ret = Dd_mst_Sel(&g_pub_tx.reply, &g_dd_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}

			/*查询活期参数表*/
			ret = Dd_parm_Sel(&g_pub_tx.reply, &g_dd_parm,
							"prdt_no='%s'", g_dd_mst.prdt_no);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 2:									/*2-定期*/
			MEMSET_DEBUG( &g_td_mst, 0x00, sizeof(struct td_mst_c) );
			MEMSET_DEBUG( &g_td_parm, 0x00, sizeof(struct td_parm_c) );

			ret = Td_mst_Sel(&g_pub_tx.reply, &g_td_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W016");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W017");
				return 1;
			}

			/*查询定期参数表*/
			ret = Td_parm_Sel(&g_pub_tx.reply, &g_td_parm,
							"prdt_no='%s'", g_td_mst.prdt_no);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 3:									/*3-贷款*/
			MEMSET_DEBUG( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );

			ret = Ln_mst_Sel(&g_pub_tx.reply, &g_ln_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"贷款主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W018");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询贷款主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W019");
				return 1;
			}
			break;
		case 4:									/*4-透支*/
			MEMSET_DEBUG( &g_od_mst, 0x00, sizeof(struct od_mst_c) );

			ret = Od_mst_Sel(&g_pub_tx.reply, &g_od_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if( ret==100 )
			{
				sprintf(acErrMsg,"透支主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W020");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询透支主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W021");
				return 1;
			}
			break;
		case 9:									/*5-内部产品*/
			MEMSET_DEBUG( &g_in_mst, 0x00, sizeof(struct in_mst_c) );
			ret = In_mst_Sel(&g_pub_tx.reply, &g_in_mst,
							"ac_id=%ld and ac_seqn=%d",
							g_mdm_ac_rel.ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"内部产品主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W022");
				return 1;
			}
			else if(ret)
			{
	    		sprintf(acErrMsg,"查询内部主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W023");
				return 1;
			}
			break;
		default:
			sprintf( acErrMsg, "帐户类型错" );
			WRITEMSG
			return 1;
	}
	sprintf(acErrMsg,"g_mdm_ac_rel.opn_br_no=[%s],g_mdm_ac_rel.ac_id=[%ld]", g_mdm_ac_rel.opn_br_no, g_mdm_ac_rel.ac_id );
	WRITEMSG
	return 0;
}

/**********************************************************************
* 函数名：    pub_base_currentdis_file
* 函数功能：  根据活期显示账号和账号序号取活期主文件函数
* 作者/时间： rob/2003/12/31
*
* 参数：
*     输入：活期显示帐号		char(24);
*			帐号序号			long ;
*
*     输出：活期主文件结构
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_currentdis_file( ac_no,ac_seqn,f_dd_mst)
char *ac_no;
int ac_seqn;
struct dd_mst_c *f_dd_mst;
{
	/*根据显示账号,序号取得账户id*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &g_mdm_ac_rel,"ac_no='%s' ", ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"查询介质帐号表异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	/*根据活期帐户id和账户序号取活期主文件*/
	MEMSET_DEBUG(f_dd_mst, 0x00, sizeof(struct dd_mst_c) );

vtcp_log("[%ld],[%d]",g_mdm_ac_rel.ac_id,ac_seqn);
	ret = Dd_mst_Sel(g_pub_tx.reply,f_dd_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"活期账号不存在![%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}

	return 0;
}

/**********************************************************************
* 函数名：    pub_base_termidis_file();
* 函数功能：  根据定期显示账号和账号序号取定期主文件函数
* 作者/时间： rob/2003/12/31
*
* 参数：
*     输入：定期显示帐号		char(24);
*			帐号序号			int ;
*
*     输出：定期主文件结构
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_termidis_file(char ac_no[24],int ac_seqn,struct td_mst_c *f_td_mst)
{
	/*根据显示账号,序号取得账户id*/
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(&g_pub_tx.reply, &g_mdm_ac_rel,
						"ac_no='%s' ", ac_no);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"介质账户对照表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if (ret && ret!=100)
	{
 		sprintf(acErrMsg,"查询介质账户对照表异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	/*根据定期帐户id和账户序号取定期主文件*/
	MEMSET_DEBUG( f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret = Td_mst_Sel(&g_pub_tx.reply, f_td_mst,
					"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=0)
	{
 		sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}
	return 0;
}

/**********************************************************************
* 函数名：    pub_base_currentid_file
* 函数功能：  根据活期账号ID取活期主文件函数
* 作者/时间： rob/2003/12/31
*
* 参数：
*     输入：活期帐号id			ac_id		long;
*			活期帐号序号		ac_seqn		int;
*
*     输出：活期主文件结构		f_dd_mst;
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_currentid_file( long ac_id,int ac_seqn,struct dd_mst_c *f_dd_mst )
{
	/*根据活期帐户id和账户序号取活期主文件*/
	MEMSET_DEBUG(f_dd_mst, 0x00, sizeof(struct dd_mst_c) );
	sprintf( acErrMsg , "ac_id[%ld]ac_seqn[%d]",ac_id,ac_seqn);
		WRITEMSG

	TRACE
	ret = Dd_mst_Sel(g_pub_tx.reply,f_dd_mst,
					"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
	TRACE
		sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if(ret && ret!=100)
	{
	TRACE
 		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}
	TRACE
	return 0;
}

/**********************************************************************
* 函数名：    pub_base_termiid_file
* 函数功能：  根据定期账号ID取定期主文件函数?
* 作者/时间： rob/2003/12/31
*
* 参数：
*     输入：定期帐号id			ac_id			long;
*			定期帐号序号		ac_seqn     	int;
*
*     输出：定期主文件结构		f_td_mst;
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_termiid_file( ac_id,ac_seqn,f_td_mst )
	long ac_id;
	int ac_seqn;
	struct td_mst_c	*f_td_mst;
{
	/*根据定期帐户id和账户序号取定期主文件*/
	MEMSET_DEBUG( f_td_mst, 0x00, sizeof(struct td_mst_c) );

	ret=Td_mst_Sel(g_pub_tx.reply,f_td_mst, \
				   "ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}
	return 0;
}


/**********************************************************************
* 函数名：    pub_base_getparm
* 函数功能：  取产品参数函数
* 作者/时间： rob/2003/04/01
*
* 参数：
*     输入：定期帐号id			ac_id			long;
*			定期帐号序号		ac_seqn     	int;
*
*     输出：定期主文件结构		f_td_mst;
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_getparm( ac_id,ac_seqn,cif_type )
	long ac_id;
	int ac_seqn;
	char *cif_type;
{
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id, \
	   "ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"查询异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}

	switch(g_prdt_ac_id.prdt_no[0])
	{
		case '1':
				ret=Dd_parm_Sel(g_pub_tx.reply,g_dd_parm, \
	   				"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"活期参数表中不存在该记录[%d]",ret);
					WRITEMSG
					strcpy (g_pub_tx.reply,"W016");
					return 1;
				}
				else if(ret && ret!=100)
				{
 					sprintf(acErrMsg,"查询异常[%d]",ret);
 					WRITEMSG
					strcpy (g_pub_tx.reply,"W017");
					return 1;
				}
				strcpy(cif_type,g_dd_parm.cif_type);

				break;
		case '2':
				ret=Td_parm_Sel(g_pub_tx.reply,g_td_parm, \
	   				"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
				if ( ret==100 )
				{
					sprintf(acErrMsg,"定期参数表中不存在该记录[%d]",ret);
					WRITEMSG
					strcpy (g_pub_tx.reply,"W016");
					return 1;
				}
				else if(ret && ret!=100)
				{
 					sprintf(acErrMsg,"查询异常[%d]",ret);
 					WRITEMSG
					strcpy (g_pub_tx.reply,"W017");
					return 1;
				}
				strcpy(cif_type,g_dd_parm.cif_type);
				break;
		default:
				break;
	}

	return 0;
}

/**********************************************************************
* 函数名：    pub_base_idseqn_file
* 函数功能：  根据账号ID和账户序号返回账户类型，并取主文件函数
* 作者/时间： rob/2003/12/31
*
* 参数：
*     输入：帐号id			ac_id			long;
*			帐号序号		ac_seqn     	int;
*
*     输出：账户类型		ac_type			char[2];
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_idseqn_file( ac_id,ac_seqn,ac_type )
	long ac_id;
	int ac_seqn;
	char *ac_type;
{
	/*根据帐户id和账户序号取账户类型*/
	struct prdt_ac_id_c  g_prdt_ac_id;
	MEMSET_DEBUG( &g_prdt_ac_id, 0x00, sizeof(struct prdt_ac_id_c) );

	ret = Prdt_ac_id_Sel(g_pub_tx.reply, &g_prdt_ac_id,
						"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"产品账户对照表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W016");
		return 1;
	}
	else if(ret && ret!=100)
	{
 		sprintf(acErrMsg,"查询账户对照表异常!![%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W017");
		return 1;
	}

	strcpy(ac_type,g_prdt_ac_id.ac_id_type);

	switch(atoi(ac_type))
	{
		case 1:									/*1-活期*/
			MEMSET_DEBUG( &g_dd_mst, 0x00, sizeof(struct dd_mst_c) );
			ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"活期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W014");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W015");
				return 1;
			}
			break;
		case 2:									/*2-定期*/
			MEMSET_DEBUG( &g_td_mst, 0x00, sizeof(struct td_mst_c) );

			ret = Td_mst_Sel(g_pub_tx.reply, &g_td_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"定期主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W016");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询定期主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W017");
				return 1;
			}
			break;
		case 3:									/*3-贷款*/
			MEMSET_DEBUG( &g_ln_mst, 0x00, sizeof(struct ln_mst_c) );

			ret = Ln_mst_Sel(g_pub_tx.reply, &g_ln_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"贷款主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W018");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询贷款主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W019");
				return 1;
			}
			break;
		case 4:									/*4-透支*/
			MEMSET_DEBUG( &g_od_mst, 0x00, sizeof(struct od_mst_c) );

			ret = Od_mst_Sel(g_pub_tx.reply, &g_od_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"透支主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W020");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询透支主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W021");
				return 1;
			}
			break;
		case 9:									/*5-内部产品*/
			MEMSET_DEBUG( &g_in_mst, 0x00, sizeof(struct in_mst_c) );
			ret = In_mst_Sel(g_pub_tx.reply, &g_in_mst,
							"ac_id=%ld and ac_seqn=%d",ac_id,ac_seqn);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"内部产品主文件中不存在该记录[%d]",ret);
				WRITEMSG
				strcpy (g_pub_tx.reply,"W022");
				return 1;
			}
			else if(ret&&ret!=100)
			{
	    		sprintf(acErrMsg,"查询内部主文件异常[%d]",ret);
	    		WRITEMSG
				strcpy (g_pub_tx.reply,"W023");
				return 1;
			}
			break;
		default:
			sprintf( acErrMsg, "帐户类型错" );
			WRITEMSG
			return 1;
	}
	return 0;
}
/**********************************************************************
* 函 数 名：  pub_base_odid_file
* 函数功能：  根据透支账号ID和账号序号取透支主文件
* 作    者：  lance
* 完成时间：  2003年12月30日
*
* 参    数：
*     输  入：  ac_id       integer   账号ID
*               ac_seqn     smallint  账号序号
*
*     输  出:                         透支主文件结构
*                           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_odid_file(long ac_id ,int ac_seqn ,struct od_mst_c *od_mst )
{

	int ret = 0 ;

	MEMSET_DEBUG( od_mst, 0x00, sizeof( struct od_mst_c ) );

    		/* 取透支主文件信息 */
	ret = Od_mst_Sel(g_pub_tx.reply, od_mst , "ac_id=%d and ac_seqn=%d" ,
			        ac_id,ac_seqn );
	if( ret==100 )
	{
		sprintf( acErrMsg,"透支主文件表中无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"T001" );
		return 1;
	}
	else if( ret !=0 )
	{
   		sprintf( acErrMsg,"取透支主文件异常 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
   		return -1;
   	}
  	return 0 ;
}

/**********************************************************************
 * 函  数 名：  pub_base_ChkAc
 * 函数 功能:   根据显示账号/序号取得主文件;
 *		帐户有效性检查:检查帐号是否为正常状态。(凭证管理用）
 *
 * 作者/时间：  jane/2003年1月8日
 *
 * 参  数：
 *     输入：  char	*ac_no			帐号
 *	       int	ac_seqn			帐号序号
 *
 *     输出：  char	*open_br_no		开户机构号
 *	      long	*ac_id			帐号id
 *   返回值：  0: 成功   -1: 失败
 *
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_base_ChkAc( char *ac_no, int ac_seqn, char *open_br_no, long *ac_id )
{
	char	cAcType[2];			/* 账户类型 */
	char	cAcSts[2];			/* 帐户状态 */
	int	iRet;

	MEMSET_DEBUG( cAcType, 0x00, sizeof( cAcType ) );
	MEMSET_DEBUG( cAcSts, 0x00, sizeof( cAcSts ) );

	/* 根据显示账号/序号取得主文件 */
	iRet = pub_base_disac_file( ac_no, ac_seqn, cAcType );
	if( iRet )
	{
		sprintf( acErrMsg, "根据显示账号/序号取得主文件失败!" );
		WRITEMSG
		return(-1);
	}
	vtcp_log("g_pub_tx.tx_code[%s]\n",g_pub_tx.tx_code);
	switch( cAcType[0] )
	{
		case '1':
			strcpy( open_br_no, g_dd_mst.opn_br_no );
			strcpy( cAcSts, g_dd_mst.ac_sts );
			*ac_id = g_dd_mst.ac_id;
			break;
		case '9':
			if(!memcmp(g_pub_tx.tx_code,"5105",4))
			{
			vtcp_log("g_pub_tx.tx_code1[%s]\n",g_pub_tx.tx_code);
			strcpy(open_br_no,g_in_mst.opn_br_no);
			strcpy(cAcSts,g_in_mst.sts);
			*ac_id =g_in_mst.ac_id;
			break;
			}
		case '2':
		case '3':
		case '4':
		case '5':
		default:
			strcpy( g_pub_tx.reply,"M018" );
			sprintf( acErrMsg, "无此帐户类型" );
			WRITEMSG
			return(-1);
	}

	if( cAcSts[0] == '3' )
	{
		sprintf( acErrMsg, "账户为挂失结清状态，不允许做此交易!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P247" ) ;
		return(-1);
	}else if( cAcSts[0] == '4' )
	{
		sprintf( acErrMsg, "账户为开户更正状态，不允许做此交易!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P248" ) ;
		return(-1);
	}else if( cAcSts[0] == '5' )
	{
		sprintf( acErrMsg, "账户为临时销户状态，不允许做此交易!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P249" ) ;
		return(-1);
	}else if( cAcSts[0] == '*' )
	{
		sprintf( acErrMsg, "账户为销户状态，不允许做此交易!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P250" ) ;
		return(-1);
	}

	sprintf( acErrMsg, "g_dd_mst.ac_type=[%s]",g_dd_mst.ac_type );
	WRITEMSG

	/**对凭证交易会有问题**
	if( (strcmp( g_dd_mst.ac_type,"1") != 0)
		&& (strcmp( g_dd_mst.ac_type, "3" ) != 0) )
	{
		sprintf( acErrMsg, "此户非对公基本户或专用户!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "N072" );
		return(-1);
	}
	**/
	return 0;
}

/**********************************************************************
 * 函  数 名：  pub_base_ChkAcBal
 * 函数 功能:   根据账号和帐户序号取得主文件;
 *		帐户有效性检查:检查帐户余额。(银行汇票用）
 *
 * 作者/时间：  jane/2003年1月17日
 *
 * 参  数：
 *     输入：  char	*ac_no			申请人帐号
 *	       int	ac_seqn			帐号序号
 *
 *     输出：  double	*dAcBal			帐户余额
 *
 *   返回值：  0: 成功   -1: 失败
 *
 * 修改历史：  包括修改人、时间、修改内容，以时间倒序方式
 *
********************************************************************/
int pub_base_ChkAcBal( char *ac_no, int ac_seqn, double *dAcBal )
{
	char	cAcType[2];			/* 账户类型 */
	int	iRet;

	MEMSET_DEBUG( cAcType, 0x00, sizeof( cAcType ) );

	/* 根据显示账号/序号取得主文件 */
	iRet = pub_base_disac_file( ac_no, ac_seqn, cAcType );
	if( iRet )
	{
		sprintf( acErrMsg, "根据显示账号/序号取得主文件失败!" );
		WRITEMSG
		return(-1);
	}

	switch( cAcType[0] )
	{
		case '1':
			*dAcBal = g_dd_mst.bal;
			break;
		case '2':
			*dAcBal = g_td_mst.bal;
			break;
		case '3':
			*dAcBal = g_ln_mst.bal;
			break;
		case '4':
			*dAcBal= g_od_mst.bal;
			break;
		case '5':
			*dAcBal = g_in_mst.bal;
			break;
		default:
			sprintf( acErrMsg, "无此帐户类型" );
			WRITEMSG
			return(-1);
	}
	return 0;
}

/**********************************************************************
* 函 数 名：  pub_base_CrtAcSeqn
* 函数功能：  生成账户序号
* 作    者：  lance
* 完成时间：  2003年01月12日
*
* 参    数：
*     输  入：  ac_id       integer   账号ID
*
*     输  出:
*               ac_seqn     smallint  账号序号
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_CrtAcSeqn(long ac_id , int *ac_seqn )
{
	int g_reply_int =0;

	struct ac_seqn_id_c ac_seqn_id;		/* 账号序号ID表结构 */

	g_reply_int=Ac_seqn_id_Dec_Upd(g_pub_tx.reply,"ac_id=%ld", ac_id);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"账号序号ID表异常错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P140");
		goto ErrExit;
	}
	g_reply_int=Ac_seqn_id_Fet_Upd(&ac_seqn_id,g_pub_tx.reply);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"账号不存在![%d][%ld]",g_reply_int,ac_id);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P102");
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"账号序号ID表异常错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P140");
		goto ErrExit;
	}

	ac_seqn_id.ac_seqn_id++;
	g_reply_int=Ac_seqn_id_Upd_Upd(ac_seqn_id,g_pub_tx.reply);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"账号序号ID表异常错误![%d]",g_reply_int);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P140");
		goto ErrExit;
	}
	Ac_seqn_id_Clo_Upd();

	if( ac_seqn_id.ac_seqn_id>=9998 )
	{
		sprintf(acErrMsg,"该介质不已经不能再开新账户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P165");
		goto ErrExit;
	}

	*ac_seqn = ac_seqn_id.ac_seqn_id ;

	return 0;
ErrExit:
	return 1;
}


/**********************************************************************
* 函 数 名：  pub_base_CurToSeqn
* 函数功能：  (活期)币种转换成账户序号
* 作    者：  lance
* 完成时间：  2003年01月13日
*
* 参    数：
*     输  入：  cur_no      char      币种
*
*     输  出:
*               ac_seqn     smallint  账号序号
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_CurToSeqn(char *cur_no , int *ac_seqn )
{
	*ac_seqn = atoi( cur_no );	/* 账户序号 */

	return 0;
}


/**********************************************************************
* 函 数 名：  pub_base_CurGetddFile
* 函数功能：  根据币种、显示账号取活期主文件
* 作    者：  lance
* 完成时间：  2003年01月13日
*
* 参    数：
*     输  入：  cur_no      char      币种
*		ac_no	    char      显示账号
*
*     输  出:
*               活期主文件
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_CurGetddFile(char ac_no[25],char cur_no[3],struct dd_mst_c *f_dd_mst )
{
	int ac_seqn=0;
	MEMSET_DEBUG( f_dd_mst , 0x00 ,sizeof( struct dd_mst_c ));
	ret = pub_base_CurToSeqn( cur_no , &ac_seqn );
	if ( ret )
	{
		sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
		WRITEMSG
		return 1;
	}
	sprintf(acErrMsg,"根据币种生成账户序号 PASS[%d]!",ac_seqn);
	WRITEMSG


	ret = pub_base_currentdis_file( ac_no, ac_seqn, f_dd_mst);
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据显示账户、账户序号返回活期主文件错误!");
			WRITEMSG
			return 1;
		}
	sprintf(acErrMsg,"根据显示账户、账户序号返回活期主文件 PASS!");
	WRITEMSG
	vtcp_log( "xxxxxx__name[%s]",f_dd_mst->name);
	return 0;
}

/**********************************************************************
* 函 数 名：  pub_base_CheckCrdNo
* 函数功能：  检查卡号的合法性
* 作    者：  chengguohong
* 完成时间：  2003年02月18日
*
* 参    数：
*     输  入：
*	         g_pub_tx.crd_no 卡号
*
*     输  出:
*               活期主文件
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/

int pub_base_CheckCrdNo()
{
     char sCard_flag[60];
     memset(sCard_flag,0,sizeof(sCard_flag));
     get_zd_data("0830",sCard_flag);
     ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",
             g_pub_tx.crd_no);
    if ( ret == 100)
    {
        sprintf(acErrMsg,"该卡号不存在!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT20");
        return ret;
    }
    else if ( ret )
    {
        sprintf(acErrMsg,"查询介质账号对照表错误!!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT21");
        return ret;
    }

    /* 检查卡的状态 */
    /**根据行里要求挂失状态下转入转账仍然可以成功，这里加标志放行 20130811 hzh**/
    if ( g_mdm_ac_rel.note_sts[0] == '1' && memcmp(sCard_flag,"tdbzr",5) !=0 && memcmp(g_pub_tx.tx_code,"6203",4) == 0)
    {
        sprintf(acErrMsg,"该卡已经挂失!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT22");
        return 1;
    }

    if ( g_mdm_ac_rel.note_sts[0] == '2' )
    {
        sprintf(acErrMsg,"该卡已经挂失换证!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT23");
        return 1;
    }

    if ( g_mdm_ac_rel.note_sts[0] == '3' || g_mdm_ac_rel.note_sts[0] == '4')
    {
        sprintf(acErrMsg,"该卡已经正常换证!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT24");
        return 1;
    }

    if ( g_mdm_ac_rel.note_sts[0] == '*' )
    {
        sprintf(acErrMsg,"该帐号已经销户!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT25");
        return 1;
    }

    if ( g_mdm_ac_rel.coll_sts[0] == '1')
    {
        sprintf(acErrMsg,"该卡的状态为捡拾状态!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT26");
        return 1;
    }

    if ( g_mdm_ac_rel.coll_sts[0] == '2')
    {
        sprintf(acErrMsg,"该卡的状态为吞卡状态!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT27");
        return 1;
    }

    /**根据行里要求挂失状态下转入转账仍然可以成功，这里加标志放行 20130811 hzh**/
    if ((g_mdm_ac_rel.note_sts[0] == '1' && memcmp(sCard_flag,"tdbzr",5) !=0 && memcmp(g_pub_tx.tx_code,"6203",4) == 0) || (g_mdm_ac_rel.note_sts[0] != '0' && g_mdm_ac_rel.note_sts[0] != '1') || (g_mdm_ac_rel.coll_sts[0] != '0'))
    {
        sprintf(acErrMsg,"该卡的状态为非正常状态!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT01");
        return 1;
    }

    /* 检查卡的有效性 */
    if ((g_pub_tx.tx_date < g_mdm_ac_rel.beg_date)
            || (g_pub_tx.tx_date >= g_mdm_ac_rel.end_date))
    {
        sprintf(acErrMsg,"卡的有效期限已过!![%ld] [%ld] [%ld]",g_pub_tx.tx_date,g_mdm_ac_rel.beg_date,g_mdm_ac_rel.end_date);
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT28");
        return 1;
    }

    /* 检查卡的支取方式 *
    if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
    {
        sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT02");
        return 1;
    }

    * 检查支取方式的合法性 *
    if (strcmp(g_mdm_ac_rel.draw_pwd,g_pub_tx.draw_pwd) != 0)
    {
        sprintf(acErrMsg,"卡的支取密码错误!!!");
        WRITEMSG
        strcpy(g_pub_tx.reply,"AT04");
        return 1;
    }
    ***/
    return 0;
}

/**********************************************************************
* 函 数 名：  pub_base_actoseqn
* 函数功能：  根据帐号返回账户序号(若账户存在多个序号返回账户序号为0)
* 作    者：  rob
* 完成时间：  2003年02月21日
* 参    数：
*     输  入：
*				ac_no	    char      	显示账号
*     输  出:
*               ac_seqn		smallint	账户序号
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_actoseqn(char *ac_no,int *ac_seqn)
{
	MEMSET_DEBUG(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
    if ( ret == 100)
    {
        sprintf(acErrMsg,"该帐号不存在!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"M003");
        return ret;
    }
	else if(ret)
    {
        sprintf(acErrMsg,"查询介质帐号对照表异常!! [%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"W011");
        return ret;
    }

	/*清空产品帐号结构*/
	MEMSET_DEBUG( &g_prdt_ac_id, 0x0, sizeof(struct prdt_ac_id_c) );

	if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)
	{
TRACE
		ret=Prdt_ac_id_Dec_Sel(g_pub_tx.reply,"ac_id=%ld",g_mdm_ac_rel.ac_id);
		if(ret)
    	{
        	sprintf(acErrMsg,"查询产品帐号对照表异常!! [%d]",ret);
        	WRITEMSG
        	strcpy(g_pub_tx.reply,"W013");
        	return ret;
		}

		ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if(ret==100)
    	{
       		sprintf(acErrMsg,"产品帐号对照表中不存在该记录!! [%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"W012");
       		return ret;
		}
		else if(ret)
    	{
       		sprintf(acErrMsg,"FETCH产品帐号对照表异常!! [%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"W059");
       		return ret;
		}

		ret=Prdt_ac_id_Fet_Sel(&g_prdt_ac_id,g_pub_tx.reply);
		if(ret==100)
			*ac_seqn=g_prdt_ac_id.ac_seqn;
		else if(ret)
    	{
       		sprintf(acErrMsg,"FETCH产品帐号对照表异常!! [%d]",ret);
       		WRITEMSG
       		strcpy(g_pub_tx.reply,"W059");
       		return ret;
		}
		else if(ret==0)
			*ac_seqn=0;
		Prdt_ac_id_Clo_Sel();
	}
	else
    {
		*ac_seqn=g_mdm_ac_rel.ac_seqn;
    }

	return 0;
}
/**
* 根据ac_id 或 ac_seqn取得ac_no
*/
int pub_base_acid_acno( long ac_id,int ac_seqn,char *ac_no )
{
	int ret=0;
	int i=0;
	struct mdm_ac_rel_c vmdm_ac;
	struct mdm_ac_rel_c smdm_ac;

	ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_id=%ld and ac_seqn=%d order by ac_no",
		ac_id,ac_seqn );
	ERR_DEAL

	ret = Mdm_ac_rel_Fet_Sel( &vmdm_ac , g_pub_tx.reply );
	if( ret==100 )
	{
		Mdm_ac_rel_Clo_Sel();

		ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_id=%ld and ac_seqn=%d order by ac_no",
			ac_id,ALL_SEQN );
		ERR_DEAL
		ret = Mdm_ac_rel_Fet_Sel( &vmdm_ac , g_pub_tx.reply );
		if( ret==100 )
		{
			Mdm_ac_rel_Clo_Sel();

			ret = Mdm_ac_rel_Dec_Sel( g_pub_tx.reply," ac_id=%ld order by ac_no",
				ac_id );
			ERR_DEAL
			ret = Mdm_ac_rel_Fet_Sel( &vmdm_ac , g_pub_tx.reply );
			if( ret==100 )
			{
				strcpy( ac_no, "" );
				goto ErrExit;
			}
		}
	}
	if ( ret!=100 ) { ERR_DEAL }

	if( vmdm_ac.note_sts[0]=='0' || vmdm_ac.note_sts[0]=='1' )
	{
		TRACE
		pub_base_strpack( vmdm_ac.ac_no );
		strcpy( ac_no,vmdm_ac.ac_no );
		goto OkExit;
	}

	while( 1 )
	{
		ret = Mdm_ac_rel_Fet_Sel( &smdm_ac , g_pub_tx.reply );
		if( ret ) break;

		if( smdm_ac.note_sts[0]=='0' || smdm_ac.note_sts[0]=='1' )
		{
			TRACE
			pub_base_strpack( smdm_ac.ac_no );
			strcpy( ac_no,smdm_ac.ac_no );
			goto OkExit;
		}

		MEMCPY_DEBUG( &vmdm_ac,&smdm_ac,sizeof(vmdm_ac) );
	}

	Mdm_ac_rel_Clo_Sel();

		pub_base_strpack( vmdm_ac.ac_no );
		strcpy( ac_no,vmdm_ac.ac_no );

OkExit:
	return 0;
ErrExit:
	return 1;
}
/**
* 根据 ac_no 取得 ac_id 或 ac_seqn
*/
int pub_base_acno_acid( long * ac_id,char *ac_no )
{
	int ret=0;
	struct mdm_ac_rel_c vmdm_ac;

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&vmdm_ac," ac_no='%s' ",
		ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"ac_no='%s'",ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply,"L014" );
		return 1;
	}
	else if( ret )
	{
		return 1;
	}
	else
	{
		*ac_id = vmdm_ac.ac_id ;
	}

return 0;
}
/******************************************************
*
* 函数名：根据账号查找客户号、户名
*
*****************************************************/
int pub_base_acgetname_mst( long ac_id,int ac_seqn,char *name )
{
	int ret = 0;
	struct dd_mst_c dd_mst_c;
	struct td_mst_c td_mst_c;

		TRACE
	ret=Dd_mst_Sel( g_pub_tx.reply,&dd_mst_c,"ac_id=%d and ac_seqn=1",ac_id );
	if( ret )
	{
		TRACE
		ret=Td_mst_Sel( g_pub_tx.reply,&td_mst_c,"ac_id=%d",ac_id );
		if( ret )
		{
		TRACE
			strcpy( g_pub_tx.reply,"W011" );
			return ret;
		}
		else
		{
			strcpy( name,td_mst_c.name );
		}
	}
	else
	{
			strcpy( name,dd_mst_c.name );
	}

	return (0);
}

/******************************************************
*
* 函数名：根据账号查找客户号、户名
*
*****************************************************/
int pub_base_acgetname(char *ac_no, long *cif_no, char *name)
{
	int ret = 0;

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	struct cif_basic_inf_c sCif_basic_inf;

	MEMSET_DEBUG( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, "ac_no = '%s'", ac_no );
	if( ret != 0 )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return 1;
	}

	MEMSET_DEBUG( &sPrdt_ac_id, 0x00, sizeof(sPrdt_ac_id) );
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &sPrdt_ac_id, \
		"ac_id = '%ld' order by ac_id_type", sMdm_ac_rel.ac_id );
	if( ret != 0 )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return ret;
	}

	MEMSET_DEBUG( &sCif_basic_inf, 0x00, sizeof(sCif_basic_inf) );
	ret = Cif_basic_inf_Sel( g_pub_tx.reply, &sCif_basic_inf, \
		"cif_no = '%ld'", sPrdt_ac_id.cif_no );
	if( ret != 0 )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return 3;
	}

	sprintf(acErrMsg,"ac_id is [%ld];cif_no is [%ld]-------------", sMdm_ac_rel.ac_id, sPrdt_ac_id.cif_no );
	WRITEMSG

	*cif_no = sPrdt_ac_id.cif_no;
	strcpy( name, sCif_basic_inf.name );

	return (0);
}


/**********************************************************************
* 函 数 名：  pub_base_getprdtparm
* 函数功能：  取产品参数
* 作    者：  rob
* 完成时间：  2003年05月21日
*
* 参    数：
*     输  入：
*				prdt_no    	char      	产品编号
*     输  出:
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_getprdtparm(prdt_no,term,term_type,deadline,interest)
char *prdt_no;
int  *term;
char *term_type;
long *deadline;
double *interest;
{

	switch(prdt_no[0])
	{
		case '1':
				 *term=g_td_parm.term;
				 strcpy(term_type,g_td_parm.term_type);
				 *deadline=g_td_mst.mtr_date;
				 *interest=0.00;
		case '2':
				 *term=g_td_parm.term;
				 strcpy(term_type,g_td_parm.term_type);
				 *deadline=g_td_mst.mtr_date;
				 *interest=0.00;
		default :
				 return 1;
	}

	return 0;
}

/**********************************************************************
* 函 数 名：  pub_base_checkopnbr
* 函数功能：  判断办理业务的网点是否为账户的开户行
* 作    者：  rob
* 完成时间：  2003年05月21日
*
* 参    数：
*     输  入：
*				ac_no    	char      	帐号
*     输  出:
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_checkopnbr(ac_no)
char *ac_no;
{
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no );
	if( ret==100 )
	{
		strcpy( g_pub_tx.reply,"M003" );
		return 1;
	}
	else if( ret )
	{
		strcpy( g_pub_tx.reply,"W011" );
		return 1;
	}

	if(strncmp(g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no,5))
	{
		strcpy( g_pub_tx.reply,"O171" );
		return 1;
	}

	return 0;
}

/**********************************************************************
* 函数名：    pub_base_CrtDiscntAc
* 函数功能：    生成贴现账号（带校验位）
* 作者/时间：    jane/2003年6月13日
*
* 参数：
*     输入：acc_hrt    :    科目控制字
*
*     输出：acno       :    根据科目控制字生成的有校验的账号
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/

int pub_base_CrtDiscntAc( char *acc_hrt, char *acno )
{
	struct	com_item_c 		sComItem;
	struct  in_ac_seqn_c 	sInAcSeqn;

    char    s_acno[21];		/* 不带校验位的账号 */
    char    s_tmp[65];     	/* 临时变量，将顺序号补足为4位 */
    char    app_code[5];    /* 应用代码 */
    int     ac_seqn;     	/* 顺序号 */
    int     ret;

    MEMSET_DEBUG( &sComItem, 0x00, sizeof( struct com_item_c ) );
    MEMSET_DEBUG( &sInAcSeqn, 0x00, sizeof( struct in_ac_seqn_c ) );

    /* 根据科目控制字取应用代码 */
    ret = Com_item_Sel( g_pub_tx.reply, &sComItem, "acc_hrt = '%s'", acc_hrt );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "无此科目,acchrt=[%s]", acc_hrt );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S033" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "查找公共科目表错，acchrt=[%s],[%d]", acc_hrt, ret );
    	WRITEMSG
    	return(-1);
    }

    /* 根据应用代码取最大顺序号 */
    pub_base_strpack( sComItem.app_cod );
    ret = In_ac_seqn_Sel( g_pub_tx.reply , &sInAcSeqn , "app_cod = '%s'", \
    				sComItem.app_cod );
    if ( ret == 100 )
    {
    	sprintf( acErrMsg, "无此应用代码,app_cod=[%s]", sComItem.app_cod );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "S044" );
    	return(-1);
    }else if ( ret )
    {
    	sprintf( acErrMsg, "查找内部帐号序号表错，app_cod=[%s],[%d]", sComItem.app_cod, ret );
    	WRITEMSG
    	return(-1);
    }

    /* 顺序号+1 */
    sInAcSeqn.seque++;

    /* 连接字符串app_code和顺序号 */
    sprintf(s_tmp,"%04d", sInAcSeqn.seque);
    s_acno[0]='5';               /* 第一位5表示对公帐号 */
	s_acno[1]='\0';
    strcat(s_acno, sComItem.app_cod);
    strcat(s_acno, s_tmp);

    /* 生成校验位 */
    ret=pub_base_CrtAcChkb( s_acno, acno, 2, CHKBT_LEN );
    if (ret < 0 )
        return (1);

    /* 调用修改内部帐号顺序号*/
    ret = sql_execute("update in_ac_seqn set seque = %d where app_cod = '%s'", \
    			sInAcSeqn.seque, sInAcSeqn.app_cod );
    if ( ret )
    {
    	sprintf( acErrMsg, "修改in_ac_seqn错，ret = [%d]" , ret );
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "E021" );
    	return(-1);
    }

    return 0;
}

/*根据相应代码取返回信息*/
int pub_base_regetinf(char *rsp_code, char *inf)
{
	int ret = 0;

	struct rsp_code_c g_rsp_code;

	MEMSET_DEBUG( &g_rsp_code, 0x00, sizeof(struct rsp_code_c) );

	ret = Rsp_code_Sel(g_pub_tx.reply,&g_rsp_code,"code='%s'",rsp_code);
	if( ret )
	{
		strcpy( g_pub_tx.reply,"O206" );
		return 1;
	}

	strcpy(inf,g_rsp_code.memo);

	return (0);
}

int pub_base_ac_prdt(struct prdt_ac_id_c	*sPrdt_ac_id)
{
	int ret=0;

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",g_pub_tx.ac_no);
	if( ret==100 )
	{
    	sprintf( acErrMsg, "帐号不存在!ac_no=[%s]",g_pub_tx.ac_no);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}
	if( ret )
	{
    	sprintf( acErrMsg, "取介质帐号对照表错误!ac_no=[%s],ret=[%d]",g_pub_tx.ac_no,ret);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,sPrdt_ac_id,"ac_id='%ld' and ac_seqn='%d' ",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret==100 )
	{
    	sprintf( acErrMsg, "帐号不存在!ac_id=[%d],ac_seqn=[%d]",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}
	if( ret )
	{
    	sprintf( acErrMsg, "取产品帐号对照表错误!ac_id=[%d],ac_seqn=[%d],ret=[%d]",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn,ret);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "M003" );
    	return(1);
	}
	return 0;
}

int pub_base_Exist_MainCrd(long ac_id,int *flag)
{
	int	ret=0;
	struct mdm_ac_rel_c Tmp_mdm_ac_rel;
	struct mdm_attr_c 	Tmp_mdm_attr;

	MEMSET_DEBUG(&Tmp_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	ret=Mdm_ac_rel_Dec_Sel(g_pub_tx.reply," ac_id=%d and main_ind='1'",ac_id);
	if( ret )
	{
    	sprintf(acErrMsg,"取介质帐号对照表错误!ac_id=[%d],ret=[%d]",ac_id,ret);
    	WRITEMSG
    	sprintf( g_pub_tx.reply, "P147" );
    	return (-1);
	}

	*flag=1;		/** 默认主卡 **/
	while(1)
	{
		ret=Mdm_ac_rel_Fet_Sel(&Tmp_mdm_ac_rel,g_pub_tx.reply);
		if( ret==100 )	break;
		if( ret )
		{
    		sprintf(acErrMsg,"介质帐号表错误!ac_id=[%d],ret=[%d]",ac_id,ret);
    		WRITEMSG
    		sprintf( g_pub_tx.reply, "P147" );
    		return (-1);
		}

		/**
		if( !strncmp(Tmp_mdm_ac_rel.ac_no,CRD_HEAD,9) )	 ** 存在主卡 **
			*flag=0;
		**/
		ret=Mdm_attr_Sel(g_pub_tx.reply,&Tmp_mdm_attr,"mdm_code='%s'",Tmp_mdm_ac_rel.mdm_code);
		if( ret )
		{
    		sprintf(acErrMsg,"取介质属性表错误!mdm_code=[%s],ret=[%d]",Tmp_mdm_ac_rel.mdm_code,ret);
    		WRITEMSG
    		sprintf( g_pub_tx.reply, "P133" );
    		return (-1);
		}

		if( Tmp_mdm_attr.mdm_type[0]=='3' || Tmp_mdm_attr.mdm_type[0]=='4'
			|| Tmp_mdm_attr.mdm_type[0]=='5' )
			*flag=0;
	}
	Mdm_ac_rel_Clo_Sel();

	return 0;
}
/**处理新旧账号**/
int pub_base_old_acno( char * ac_no )
{
	int ret=0;
	struct mo_merge_ac_c mo_merge_ac_c;

	/**旧账号**/
	if( strlen(ac_no)==18 )
		;
	else if( !strncmp(ac_no,"7861",4) && strlen(ac_no)==20 )
		;
	else if( !strncmp(ac_no,"7861",4) && strlen(ac_no)==19 )
		;
	else
		return 0;

	ret=Mo_merge_ac_Sel( g_pub_tx.reply , &mo_merge_ac_c , "old_ac_no='%s'" , \
						 ac_no );
	if( ret==100 )
	{
		return 0;
	}
	else if( ret )
	{
		return 1;
	}
	pub_base_strpack(mo_merge_ac_c.ac_no);
	strcpy( ac_no,mo_merge_ac_c.ac_no );
        return 0;
}

int pub_base_opn_sub_ac(int sub_ac_seqn,char *intst_type,double rate)
{
	int ret=0;
	struct prdt_ac_id_c	sPrdt_ac_id;
	struct sub_dd_mst_c	sSub_dd_mst;
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));

	if( g_pub_tx.ac_seqn==0 )		/*** 需要生成分帐号 ***/
	{
		ret = pub_base_CurToSeqn( g_pub_tx.cur_no , &g_pub_tx.ac_seqn );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			    return 1;
		}
	}

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"取介质帐号对照表错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	if(memcmp(g_pub_tx.tx_br_no,g_mdm_ac_rel.opn_br_no,strlen(g_pub_tx.tx_br_no))){
		sprintf(acErrMsg,"只能在一级账户开户机构开子账户!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P427");
		return 1;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"取产品帐号对照表错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	if( sPrdt_ac_id.ac_id_type[0]!='1' )
	{
		sprintf(acErrMsg,"非活期产品帐号不能开子帐号!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P426");
		return 1;
	}
	if( strcmp(g_mdm_ac_rel.opn_br_no,g_pub_tx.tx_br_no) )
	{
		sprintf(acErrMsg,"只能在该母帐号开户机构开子帐号!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P427");
		return 1;
	}
	strcpy(g_pub_tx.prdt_code,sPrdt_ac_id.prdt_no);

	strcpy(sSub_dd_mst.br_no,g_mdm_ac_rel.opn_br_no);
	strcpy(sSub_dd_mst.ac_no,g_pub_tx.ac_no);
	sSub_dd_mst.sub_ac_seqn=sub_ac_seqn;
	sprintf(sSub_dd_mst.sub_ac_no,"%s-%d",g_pub_tx.ac_no,sub_ac_seqn);
	sSub_dd_mst.bal=0.00;
	sSub_dd_mst.ys_bal=0.00;
	sSub_dd_mst.intst_acm=0.00;
	sSub_dd_mst.opn_date=g_pub_tx.tx_date;
	sSub_dd_mst.ic_date=g_pub_tx.tx_date;
	sSub_dd_mst.lst_date=g_pub_tx.tx_date;
	sSub_dd_mst.hst_cnt=1;
	sSub_dd_mst.hst_pg=0;
	strcpy(sSub_dd_mst.name,g_pub_tx.name);
	strcpy(sSub_dd_mst.ac_sts,"1");
	strcpy(sSub_dd_mst.intst_type,intst_type);
	sSub_dd_mst.rate=rate;
	/**------------ 登记子帐号台帐 -------------**/
	ret=Sub_dd_mst_Ins(sSub_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记子帐号台帐错误!");
		WRITEMSG
		Sub_dd_mst_Debug(&sSub_dd_mst);
		    strcpy(g_pub_tx.reply,"P429");
		return 1;
	}
	/**------------- 登记子帐号明细台帐 ------------**/
	ret=pub_base_ins_sub_ac_hst(sSub_dd_mst);
	if( ret )
	{
		WRITEMSG
		   return 1;
	}
	/**------------- 登记业务流水 --------------**/
	ret=pub_ins_trace_log();
	if( ret )
	{
		WRITEMSG
		    return 1;
	}

	return 0;
}

int pub_base_ins_sub_ac_hst(struct sub_dd_mst_c	sSub_dd_mst)
{
	int ret=0;
	struct sub_dd_mst_hst_c	sSub_dd_mst_hst;
	memset(&sSub_dd_mst_hst,0x00,sizeof(struct sub_dd_mst_hst_c));

	sSub_dd_mst_hst.trace_no=g_pub_tx.trace_no;
	get_zd_long( DC_TRACE_CNT , &sSub_dd_mst_hst.trace_cnt);
	sSub_dd_mst_hst.trace_cnt++;
	strcpy(sSub_dd_mst_hst.opn_br_no,sSub_dd_mst.br_no);
	strcpy(sSub_dd_mst_hst.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(sSub_dd_mst_hst.ac_no,sSub_dd_mst.ac_no);
	strcpy(sSub_dd_mst_hst.sub_ac_no,sSub_dd_mst.sub_ac_no);
	strcpy(sSub_dd_mst_hst.add_ind,g_pub_tx.add_ind);
	strcpy(sSub_dd_mst_hst.ct_ind,g_pub_tx.ct_ind);
	sSub_dd_mst_hst.tx_amt=g_pub_tx.tx_amt1;
	sSub_dd_mst_hst.bal=sSub_dd_mst.bal;
	sSub_dd_mst_hst.intst_acm=sSub_dd_mst.intst_acm;
	sSub_dd_mst_hst.tx_date=g_pub_tx.tx_date;
	sSub_dd_mst_hst.tx_time=g_pub_tx.tx_time;
	sSub_dd_mst_hst.hst_cnt=sSub_dd_mst.hst_cnt;
	strcpy(sSub_dd_mst_hst.brf,g_pub_tx.brf);
	strcpy(sSub_dd_mst_hst.tel,g_pub_tx.tel);
	strcpy(sSub_dd_mst_hst.chk,g_pub_tx.chk);
	strcpy(sSub_dd_mst_hst.auth,g_pub_tx.auth);
	strcpy(sSub_dd_mst_hst.note_type,g_pub_tx.note_type);
	strcpy(sSub_dd_mst_hst.note_no,g_pub_tx.beg_note_no);
	get_zd_long("1261",&sSub_dd_mst_hst.tt_no);/*add by cgx 20090818 nxhhyh */
	sSub_dd_mst_hst.sub_ac_seqn=sSub_dd_mst.sub_ac_seqn;

	ret=Sub_dd_mst_hst_Ins(sSub_dd_mst_hst,g_pub_tx.reply);
	if( ret )
	{
		Sub_dd_mst_hst_Debug(&sSub_dd_mst_hst);
		sprintf(acErrMsg,"登记子帐号明细台帐错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P430");
		return 1;
	}
	return 0;
}

/* 注意的几个参数 pub_tx.ac_no   主账户
				  pub_tx.ac_no1  子帐号
                  pub_tx.add_ind 借贷标志
                  pub_tx.cd_ind  现转标志
				  pub_tx.brf     摘要
				  pub_tx.cur_no  币种
*/
int pub_base_sub_ac_trance(int sub_ac_seqn)
{
	int	ret=0;
	char sub_ac_no[26];

	char add_ind[2];
	struct sub_dd_mst_c	sSub_dd_mst;
	struct prdt_ac_id_c	sPrdt_ac_id;
	struct mdm_ac_rel_c s_mdm_ac_rel;
	struct dd_parm_c    s_dd_parm;

	memset(&sSub_dd_mst,0x00,sizeof(struct sub_dd_mst_c));
	memset(sub_ac_no,0,sizeof(sub_ac_no));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&s_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&s_dd_parm,0x00,sizeof(struct dd_parm_c));
	memset(add_ind,0,sizeof(add_ind));
	vtcp_log(" %s, %d, function[pub_base_sub_ac_trance] ac_seqn[%ld] cur_no[%s] ac_seqn[%d] ",__FILE__,__LINE__,g_pub_tx.ac_seqn,g_pub_tx.cur_no,sub_ac_seqn);
	if( g_pub_tx.ac_seqn==0 )
	{
		ret = pub_base_CurToSeqn( g_pub_tx.cur_no , &g_pub_tx.ac_seqn );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			    return 1;
		}
	}

	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&s_mdm_ac_rel,"ac_no='%s'",g_pub_tx.ac_no);
	if( ret )
	{
		sprintf(acErrMsg,"取介质帐号对照表错误![%s]",g_pub_tx.ac_no1);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	/* 这里判断一下若是开通标志没有开通直接报错! 201011114*/
	/* if(s_mdm_ac_rel.kt_flag[0]!='1') */
	if(strcmp(s_mdm_ac_rel.draw_pwd_chg,"1"))
	{
		vtcp_log("%s,%d,该账户没有开通二级账户!",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"SN09");
		return 1;
	}
	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&sPrdt_ac_id,"ac_id=%ld and ac_seqn=%d",s_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"取产品帐号对照表错误!");
		WRITEMSG
	    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}

	ret=Dd_parm_Sel(g_pub_tx.reply,&s_dd_parm,"prdt_no='%s'",sPrdt_ac_id.prdt_no);
	if( ret )
	{
		sprintf(acErrMsg,"取产品参数表错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}

	/**--------- 取母帐号主文件 -----------**
	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%d and ac_seqn=%d",g_mdm_ac_rel.ac_id,g_pub_tx.ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"取产品参数表错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
	*/
	/**--------- 取子帐号台帐 -----------**/
	ret=Sub_dd_mst_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and sub_ac_seqn=%d",g_pub_tx.ac_no,sub_ac_seqn);
	if( ret )
	{
		sprintf(acErrMsg,"取子帐号台帐错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P431");
		return 1;
	}
	ret=Sub_dd_mst_Fet_Upd(&sSub_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"取子帐号台帐错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P431");
		return 1;
	}
	memcpy(g_pub_tx.opn_br_no ,sSub_dd_mst.br_no,sizeof(g_pub_tx.opn_br_no)-1);
	if( sSub_dd_mst.ac_sts[0]!='1' )
	{
		sprintf(acErrMsg,"该子帐号已经销户!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P432");
		return 1;
	}
	vtcp_log(" %s, %d, add_ind[%s] bal[%.2lf] ",__FILE__,__LINE__,g_pub_tx.add_ind,sSub_dd_mst.bal);
	if( g_pub_tx.add_ind[0]=='1' )		/*** 存款 ***/
	{
		vtcp_log(" %s, %d, g_pub_tx.tx_amt1[%.2lf] bal[%.2lf] ",__FILE__,__LINE__,g_pub_tx.tx_amt1,sSub_dd_mst.bal);
		if( pub_base_CalAcm(sSub_dd_mst.lst_date,g_pub_tx.tx_date,
		    s_dd_parm.intst_term_type,s_dd_parm.acm_calc_type,
		    sSub_dd_mst.bal,g_pub_tx.tx_amt1,&sSub_dd_mst.intst_acm,
		    sSub_dd_mst.intst_type,s_dd_parm.intst_mon,s_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			    return 1;
		}
		sSub_dd_mst.bal+=g_pub_tx.tx_amt1;
		vtcp_log(" %s, %d, sSub_dd_mst.bal=[%f]",__FILE__,__LINE__,sSub_dd_mst.bal);
	}
	else	/* 取款,销户 */
	{
		vtcp_log(" %s, %d, g_pub_tx.tx_amt1[%.2lf] bal[%.2lf] ",__FILE__,__LINE__,g_pub_tx.tx_amt1,sSub_dd_mst.bal);
		if( pub_base_CalAcm(sSub_dd_mst.lst_date,g_pub_tx.tx_date,
		    s_dd_parm.intst_term_type,s_dd_parm.acm_calc_type,
		    sSub_dd_mst.bal,-g_pub_tx.tx_amt1,&sSub_dd_mst.intst_acm,
		    sSub_dd_mst.intst_type,s_dd_parm.intst_mon,s_dd_parm.intst_day) )
		{
			sprintf(acErrMsg,"计算利息积数错误!");
			WRITEMSG
			    return 1;
		}
		vtcp_log(" %s, %d,   sSub_dd_mst.bal=%.2f",__FILE__,__LINE__,sSub_dd_mst.bal);
		vtcp_log(" %s, %d,   g_pub_tx.tx_amt1=%.2f",__FILE__,__LINE__,g_pub_tx.tx_amt1);
		sSub_dd_mst.bal-=g_pub_tx.tx_amt1;
		vtcp_log(" %s, %d,   sSub_dd_mst.bal=%.2f",__FILE__,__LINE__,sSub_dd_mst.bal);
		if(pub_base_CompDblVal(sSub_dd_mst.bal,0.0)<0)
		{
				sprintf(acErrMsg,"子账户不能透支!,sub_dd_mst.bal=%.2f",sSub_dd_mst.bal);
				WRITEMSG
				    strcpy(g_pub_tx.reply,"T002");
				return 1;
		}
		if( g_pub_tx.add_ind[0]=='3' )	/*** 销户 ***/
		{
			if( pub_base_CompDblVal(sSub_dd_mst.bal,0.00)!=0 )
			{
				sprintf(acErrMsg,"子帐号销户后子帐号余额不为零!");
				WRITEMSG
				    strcpy(g_pub_tx.reply,"P433");
				return 1;
			}
			strcpy(sSub_dd_mst.ac_sts,"*");
		}
		strcpy(g_pub_tx.add_ind,"0");
	}
	sSub_dd_mst.hst_cnt++;
	sSub_dd_mst.lst_date=g_pub_tx.tx_date;

	ret=Sub_dd_mst_Upd_Upd(sSub_dd_mst,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"取子帐号台帐错误!");
		WRITEMSG
		    strcpy(g_pub_tx.reply,"P431");
		return 1;
	}
	Sub_dd_mst_Clo_Upd();

	/**------------- 登记子帐号明细台帐 ------------**/
	ret=pub_base_ins_sub_ac_hst(sSub_dd_mst);
	if( ret )
	{
		WRITEMSG
		return 1;
	}
	/**------------- 登记业务流水 --------------**/
	/* by Wang Yongwei 为什么需要子账户序号放到摘要里面 20101214*/
	/* strcpy(g_pub_tx.brf,g_pub_tx.ac_no1); */
	/* modify begin by wzs 20120514 reason: 子账户的序号扩成了8位,trace_log的ac_no不够用,所以子账号的流水序号放在ac_id里面,ac_no存主账号 */
	/* g_pub_tx.ac_id=s_mdm_ac_rel.ac_id;
	sprintf(sub_ac_no,"%.13s-%d",g_pub_tx.ac_no,sub_ac_seqn);
	memcpy(g_pub_tx.ac_no,sub_ac_no,sizeof(g_pub_tx.ac_no)-1);*//*登记子账号用 add by chenchao 20110110 23:48:30*/
	g_pub_tx.ac_id=sub_ac_seqn;
	/* modify end by wzs 20120514 */
	ret=pub_ins_trace_log();
	if( ret )
	{
		WRITEMSG
		    return 1;
	}
	strcpy(g_pub_tx.ac_no,sSub_dd_mst.ac_no);/* 母账号付给全局变量为了母账号记账 */

	return 0;
}
/**********************************************************************
* 函 数 名：  pub_base_checkcollyn
* 函数功能：  判断是否捡拾
* 作    者：  rob
* 完成时间：  2003年07月21日
*
* 参    数：
*     输  入：
*				ac_no    	char      	帐号
*     输  出:
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_checkcollyn( char *ac_no )
{
	struct mo_coll_c		g_mo_coll;
	struct com_branch_c		g_com_branch;

	MEMSET_DEBUG( &g_mo_coll,0x00,sizeof(struct mo_coll_c) );
	MEMSET_DEBUG( &g_com_branch,0x00,sizeof(struct com_branch_c) );

	g_reply_int = Mo_coll_Sel(g_pub_tx.reply,&g_mo_coll, \
				   			  "ac_no='%s' and sts='1'",g_mdm_ac_rel.ac_no);
   	if( g_reply_int==100 )
   	{
	   	sprintf(acErrMsg,"捡拾登记薄无该笔捡拾记录");
	   	WRITEMSG
	   	strcpy(g_pub_tx.reply,"D141");
		return 1;
   	}
   	else if( g_reply_int==0 )
   	{
	   	g_reply_int = Com_branch_Sel(g_pub_tx.reply,&g_com_branch, \
									 "br_no='%s'",g_mo_coll.pick_br_no);
		if( g_reply_int==100 )
		{
			sprintf(acErrMsg,"网点号码不存在[%s]",g_mo_coll.pick_br_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D141");
			return 1;
		}
	   	else if( g_reply_int )
		{
		  	sprintf(acErrMsg,"查询出错[%d]",g_reply_int);
			WRITEMSG
		   	strcpy(g_pub_tx.reply,"D103");
			return 1;
		}

	   	pub_base_strpack(g_com_branch.br_name);
	   	sprintf( acErrMsg,"该介质被[%s]捡拾",g_com_branch.br_name);
	  	WRITEMSG
   		strcpy(g_pub_tx.reply,"D141");
		sprintf( acErrMsg,"捡拾机构:(%s),请办理归还.", \
		g_com_branch.br_name);
		set_zd_data( DC_GET_MSG,acErrMsg );
		WRITEMSG
		return(2);
	}
	else if( g_reply_int )
	{
		sprintf(acErrMsg,"查找错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	return 0;
}
/**********************************************************************
* 函 数 名：  pub_base_getbrname
* 函数功能：  根据行号取行名
* 作    者：  rob
* 完成时间：  2003年07月28日
*
* 参    数：
*     输  入：
*				br_no    	char      	机构号
*     输  出:
*				br_name		char 		机构名称
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_getbrname( char *br_no ,char *br_name )
{
	struct com_branch_c		g_com_branch;

	MEMSET_DEBUG( &g_com_branch,0x00,sizeof(struct com_branch_c) );

	g_reply_int = Com_branch_Sel(g_pub_tx.reply,&g_com_branch, "br_no='%s'", \
								 br_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"网点号码不存在[%s]",br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D141");
		return 1;
	}
   	else if( g_reply_int )
	{
	  	sprintf(acErrMsg,"查询出错[%d]",g_reply_int);
		WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	strcpy( br_name,g_com_branch.br_name );
	return 0;
}
/**********************************************************************
* 函 数 名：  pub_base_getbrtele
* 函数功能：  根据行号取电话号码
* 作    者：  rob
* 完成时间：  2003年07月28日
* 参    数：
*     输  入：br_no    	char      	机构号
*     输  出: br_name	char 		机构名称
*     返回值： 0 成功
*            非0 失败
* 修改历史：
*
********************************************************************/
int pub_base_getbrtele( char *br_no ,char *br_tele )
{
	struct com_branch_c		g_com_branch;

	MEMSET_DEBUG( &g_com_branch,0x00,sizeof(struct com_branch_c) );

	g_reply_int = Com_branch_Sel(g_pub_tx.reply,&g_com_branch,"br_no='%s'", \
								 br_no);
	if( g_reply_int==100 )
	{
		sprintf(acErrMsg,"网点号码不存在[%s]",br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D141");
		return 1;
	}
   	else if( g_reply_int )
	{
	  	sprintf(acErrMsg,"查询出错[%d]",g_reply_int);
		WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	strcpy( br_tele,g_com_branch.br_tele );
	return 0;
}
/**取得产品的科目**/
int pub_base_prdt_accno( char *prdt_no,char *acc_no )
{
	int ret;
	struct dd_parm_c dd_parm;
	struct td_parm_c td_parm;
	struct ln_parm_c ln_parm;
	struct in_parm_c in_parm;
	struct dc_acc_rel_c dc_acc_rel;
	struct com_item_c com_item;
	char dc_code[5];

	if( prdt_no[0]=='1' )
	{
		ret=Dd_parm_Sel( g_pub_tx.reply,&dd_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,dd_parm.dc_code );
	}
	else if( prdt_no[0]=='2' )
	{
		ret=Td_parm_Sel( g_pub_tx.reply,&td_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,td_parm.dc_code );
	}
	else if( prdt_no[0]=='3' )
	{
		ret=Ln_parm_Sel( g_pub_tx.reply,&ln_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,ln_parm.dc_code );
	}
	else if( prdt_no[0]=='9' )
	{
		ret=In_parm_Sel( g_pub_tx.reply,&in_parm,"prdt_no='%s'",prdt_no );
		if( ret ) return 1;
		strcpy( dc_code,in_parm.dc_code );
	}
	else
	{
		strcpy( acc_no,"" );
		return 0;
	}
	pub_base_strpack(acc_no);
	ret=Dc_acc_rel_Sel( g_pub_tx.reply,&dc_acc_rel,"dc_code='%s' and data_code='0152'",dc_code);
	if( ret ) { strcpy( acc_no,"" ); return 0; }
	pub_base_strpack(dc_acc_rel.acc_hrt);
	ret=Com_item_Sel( g_pub_tx.reply,&com_item,"acc_hrt='%s'",dc_acc_rel.acc_hrt);
	if( ret ) { strcpy( acc_no,"" ); return 0; }
	strcpy( acc_no,com_item.acc_no );

	return 0;
}
/**********************************************************************
* 函 数 名：  pub_base_getacname
* 函数功能：  根据账户ID取户名
* 作    者：  rob
* 完成时间：  2003年07月28日
* 参    数：
*     输  入：br_no    	char      	机构号
*     输  出: br_name	char 		机构名称
*     返回值： 0 成功
*            非0 失败
* 修改历史：
*
********************************************************************/
int pub_base_getacname( long *ac_id,char *name )
{
	int ret;

	MEMSET_DEBUG( &g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c) );

	g_reply_int =Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld",ac_id);

	switch( g_prdt_ac_id.prdt_no[0] )
	{
		case '1':
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld and ac_seqn=1",ac_id);
				strcpy( name,g_dd_mst.name );
				break;
		case '2':
				ret = Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld",ac_id);
				strcpy( name,g_td_mst.name );
				break;
		case '3':
				ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst,"ac_id=%ld and ac_seqn=0",ac_id);
				strcpy( name,g_ln_mst.name );
				break;
		case '9':
				ret = In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%ld",ac_id);
				strcpy( name,g_in_mst.name );
				break;
		default:
				break;
	}

	if( ret==100 || g_reply_int==100)
	{
		sprintf(acErrMsg,"账户不存在[%ld][%d][%d]",ac_id,ret,g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		return 1;
	}
   	else if( ret || g_reply_int )
	{
	  	sprintf(acErrMsg,"查询出错[%d][%d]",ret,g_reply_int);
		WRITEMSG
	   	strcpy(g_pub_tx.reply,"D103");
		return 1;
	}

	return 0;
}
/**********************************************************************
* 函数名：    pub_base_getdd_mst_parm
* 函数功能：  根据活期账号、序号取主文件和产品参数
* 作者/时间： rob
*
* 参数：
*     输入：活期帐号			char(24);
*			帐号序号			int ;
*
*     输出：活期主文件结构
*     		活期参数结构
*
* 返 回 值: 0 成功 1 失败
*
* 修改历史：
*
********************************************************************/
int pub_base_getdd_mst_parm(char *ac_no, int ac_seqn,
							struct dd_mst_c *f_dd_mst,
							struct dd_parm_c *f_dd_parm )
{
	MEMSET_DEBUG( &g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c) );
	MEMSET_DEBUG( f_dd_mst, 0x00, sizeof(struct dd_mst_c) );
	MEMSET_DEBUG( f_dd_parm, 0x00, sizeof(struct dd_parm_c) );

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号表中不存在该记录");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W010");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"查询介质帐号表异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W011");
		return 1;
	}

	ret = Dd_mst_Sel(g_pub_tx.reply,f_dd_mst,"ac_id=%ld and ac_seqn=%d", \
					 g_mdm_ac_rel.ac_id,ac_seqn);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"活期账号不存在!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}

	ret = Dd_parm_Sel( g_pub_tx.reply,f_dd_parm,"prdt_no='%s'", \
						f_dd_mst->prdt_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"活期产品不存在!");
		WRITEMSG
		strcpy (g_pub_tx.reply,"W014");
		return 1;
	}
	else if( ret )
	{
 		sprintf(acErrMsg,"查询活期主文件异常[%d]",ret);
 		WRITEMSG
		strcpy (g_pub_tx.reply,"W015");
		return 1;
	}

	return 0;
}
